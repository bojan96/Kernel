


typedef void (*IRQHandler)(void);

int RegisterHandler(int irqNum, IRQHandler handler);
