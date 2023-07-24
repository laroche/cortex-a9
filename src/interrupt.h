#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "gic.h"

typedef void (*func_t)(void);

void interrupt_init(void);
void install_isr(unsigned int irq_num, func_t handler);
void enable_irq(unsigned int irq_num);

__attribute__ ((interrupt("IRQ"))) void IRQHandler(void);
__attribute__ ((interrupt("FIQ"))) void FIQHandler(void);

#endif
