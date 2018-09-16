#include "thread.h"
#include "irq_high_level.h"
#include "allocator.h"
#include "timer.h"
#include "assert.h"
#include "terminal.h"
#include <stdint.h>
#include <stddef.h>

#define THREAD_MAX 32

// Min value is 19 HZ, Max 1000 HZ
#define SCHEDULER_FREQUENCY 20

// Also defined in boot.asm
#define CS_REG 0x08

#ifdef DEBUG_THREADS
#define DEBUG_WRITE(x, ...) terminal_writeFormat(x, ##__VA_ARGS__)
#else
#define DEBUG_WRITE(x, ...)
#endif


typedef struct __attribute__((packed))
{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
} Regs;


typedef struct tcb
{
	int id;
	Regs registers;
	void *stack;
	ThreadState state;
	Thread* next;
	Thread* prev;
} Thread;

typedef struct __attribute__((packed))
{
	uint32_t eip;
	// cs is 16 bits wide, push on stack is extended to 32 bits because of address size attribute
	uint32_t cs;
	uint32_t eflags;
	uint32_t cleanupFunc;
} StackStart;

static Thread Threads[THREAD_MAX];
static int ThreadCount = 0;
static Thread* volatile CurrentThread;
static Thread* MainThread;
static volatile int DisableScheduler = 1;


static volatile uint32_t MsPassed = 0;
static uint32_t TimeQuantum = 1000 / SCHEDULER_FREQUENCY; // In miliseconds

// Defined in irq.asm
extern Regs* get_context();

static void thread_enableScheduler()
{
	DisableScheduler = 0;
}

static void thread_disableScheduler()
{
	DisableScheduler = 1;
}

static void thread_scheduler();

void thread_init()
{
	ThreadCount = 1;
	Thread thisThread;
	// thisThread.registers will be set on context switch
	thisThread.id = 0; // Main thread has id = 0
	thisThread.prev = thisThread.next = 0;
	thisThread.state = ThreadState_Running;
	thisThread.nextSchedule = 0;
	thisThread.stack = 0;
	Threads[0] = thisThread;
	
	MainThread = CurrentThread = &Threads[0];
	
	for(int i = 1; i < THREAD_MAX; ++i)
	{
		Threads[i].id = -1; // Negative index means this slot is free
		Threads[i].state = ThreadState_Finished;
	}
	
	// Setup thread scheduler
	int retval = irqHighLevel_registerHandler(0, &thread_scheduler);
	assert(retval);
	retval = timer_init(SCHEDULER_FREQUENCY);
	assert(retval);
	irqHighLevel_enableInterrupts();
	thread_enableScheduler();
	
	DEBUG_WRITE("Threads initialized\n");
	
}

// This is called when thread transitions to finished state
static void thread_endThread()
{
	thread_disableScheduler();
	
	CurrentThread->state = ThreadState_Finished;
	--ThreadCount;
	assert(CurrentThread->id != 0);
	allocator_freeBlocks(CurrentThread->stack, 1);
	
	DEBUG_WRITE("Thread(id = %d) finished\n", CurrentThread->id);
	thread_enableScheduler();
	
	// Busy wait for next scheduler cycle
	while(1);
}

static uint32_t thread_getFlags()
{
	volatile uint32_t eflags;
	__asm__ volatile
		(
		"pushfd\n\t"
		"pop %0\n\t":"=g"(eflags)::
		);
	
	return eflags;
}


Thread* thread_createThread(ThreadProc proc)
{
	thread_disableScheduler();
	
	if(proc == 0)
		return 0;
		
	int freeThread = 1;
		
	while(freeThread < THREAD_MAX && Threads[freeThread].id > 0)
		++freeThread;
	
	if(freeThread == THREAD_MAX)
	{
		thread_enableScheduler();
		return 0;
	}
	
	DEBUG_WRITE("Free thread: %d\n", freeThread);
	
	// Size of stack is 4K
	uint8_t* stack = (uint8_t*)allocator_allocateBlocks(1);
	if(!stack)
	{
		thread_enableScheduler();
		return 0;
	}
		
	Threads[freeThread].id = freeThread;
	Threads[freeThread].next = MainThread->next;
	Threads[freeThread].prev = MainThread;
	Threads[freeThread].stack = stack;
	Threads[freeThread].state = ThreadState_Running;
	Threads[freeThread].nextSchedule = 0;
	MainThread->next = &Threads[freeThread];
	
	uint8_t* stackEnd = stack + BLOCK_SIZE;
	
	/*
	 * Stack contents after thread creation
	 * - thread_endThread (4 bytes)
	 * - EFLAGS (4 bytes)
	 * - CS   (4 bytes)
	 * - EIP (4 bytes)
	 * 
	 * */
	
	// Setup the stack
	// Make sure flags has interrupt enable set
	uint32_t eflags = thread_getFlags();
	StackStart stackStart;
	stackStart.cleanupFunc = (uint32_t)thread_endThread;
	stackStart.eip = (uint32_t)proc;
	stackStart.cs = CS_REG;
	stackStart.eflags = eflags;
	*(StackStart *)(stackEnd - sizeof(StackStart)) = stackStart;

	
	Regs regs;
	regs.esp = (uint32_t)(stackEnd - sizeof(StackStart));
	regs.eax = regs.ecx = regs.edx = regs.ebx = regs.ebp = regs.edi = regs.esi = 0;
	Threads[freeThread].registers = regs;
	
	++ThreadCount;
	thread_enableScheduler();
	
	return &Threads[freeThread];
}

static void thread_haltCPU()
{
	__asm__ volatile
		("hlt\t\n":::);
}

void thread_sleep(uint32_t ms)
{
	if(ms < TimeQuantum / 2)
		return;
	thread_disableScheduler();
	CurrentThread->state = ThreadState_Paused;
	CurrentThread->nextSchedule = MsPassed + ms;
	uint32_t toSleep = MsPassed + ms;
	thread_enableScheduler();
	
	while(MsPassed < toSleep)
		thread_haltCPU();
		
	thread_disableScheduler();
	CurrentThread->state = ThreadState_Running;
	thread_enableScheduler();
}


static void thread_timer()
{
	MsPassed += TimeQuantum;
}

void thread_initLock(Lock *lock)
{
	lock->counter = lock->nextThread = 0;
}


void thread_lock(Lock *lock)
{
	int num = __sync_fetch_and_add(&lock->counter, 1);
	
	while(num != lock->nextThread)
	{
		thread_sleep(TimeQuantum);
	}
	
}

void thread_unlock(Lock *lock)
{
	__sync_fetch_and_add(&lock->nextThread, 1);
}

void thread_join(Thread* thread)
{
	while(thread->state != ThreadState_Finished)
	{
		thread_sleep(TimeQuantum);
	}
}

int thread_this()
{
	return CurrentThread->id;
}

static void thread_scheduler()
{
	
	thread_timer();
	
	if(DisableScheduler)
		return;
		
	Regs* currContext =	get_context();
	assert(*(uint32_t *)((uint8_t *)currContext + sizeof(Regs) + sizeof(uint32_t)) == CS_REG);
	
	CurrentThread->registers = *currContext;
	
	if(CurrentThread->state == ThreadState_Finished)
	{
		// Mark it as available
		CurrentThread->id = -1;
		// MainThread is first elem in list
		assert(CurrentThread->prev != 0);
		CurrentThread->prev->next = CurrentThread->next;
		if(CurrentThread->next)
		{
			CurrentThread->next->prev = CurrentThread->prev;
		}
	}
		
	CurrentThread = (CurrentThread->next != 0) ? CurrentThread->next : MainThread;
	*currContext = CurrentThread->registers;
	
}

