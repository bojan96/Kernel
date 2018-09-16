#pragma once
#include <stdint.h>
typedef struct tcb Thread;

typedef void (*ThreadProc)();
typedef enum {ThreadState_Running, ThreadState_Paused, ThreadState_Finished} ThreadState;

typedef struct lock
{
	int counter;
	int nextThread;
} Lock;

Thread* thread_createThread(ThreadProc proc);
void thread_init();
void thread_sleep(uint32_t ms);
void thread_initLock(Lock *lock);
void thread_lock(Lock *lock);
void thread_unlock(Lock *lock);
void thread_join(Thread* thread);
int thread_this();
