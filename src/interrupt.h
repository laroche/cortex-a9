#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "gic.h"

typedef void (*func_t)(void);

void interrupt_init(void);
void install_isr(IRQn_Type irq_num, func_t handler);
void enable_irq(IRQn_Type irq_num);

int main (void);
void __attribute__ ((section(".isr_vector"),naked,used)) _Reset (void);

#endif
