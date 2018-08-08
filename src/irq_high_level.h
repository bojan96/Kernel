#pragma once


typedef void (*IRQHandler)(void);

int registerHandler(int irqNum, IRQHandler handler);

void enableInterrupts();
void disableInterrupts();
