#pragma once


typedef void (*IRQHandler)(void);

int irqHighLevel_registerHandler(int irqNum, IRQHandler handler);

void irqHighLevel_enableInterrupts();
void irqHighLevel_disableInterrupts();
