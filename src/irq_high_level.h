


typedef void (*IRQHandler)(void);

int registerHandler(int irqNum, IRQHandler handler);
