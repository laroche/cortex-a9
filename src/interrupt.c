#include <stdio.h>
#include "cortex_config.h"
#include "interrupt.h"
#include "pl050.h"
#include "pl011.h"

void interrupt_init (void)
{
	GIC_Enable();
	/* __asm__ __volatile__("cpsie i" : : : "memory", "cc"); */
}

#define MAXIRQNUM 50U

static func_t isr_table[MAXIRQNUM];

void install_isr (unsigned int irq_num, func_t handler)
{
	if (irq_num < MAXIRQNUM) {
		isr_table[irq_num] = handler;
	} else {
#ifdef	DEBUG
		uart_printf("Too big irq number %u.\n", irq_num);
#endif
	}
}

void enable_irq (unsigned int irq_num)
{
	GIC_EnableIRQ(irq_num);
}

#if CONFIG_ARM_FP
#define save_fpu() \
  __asm__ __volatile__( \
	"vpush {d0-d15}\n" \
	"vpush {d16-d31}\n" \
	"vmrs r1, fpscr\n" \
	"push {r1}\n" \
	"vmrs r1, fpexc\n" \
	"push {r1}\n" \
  )
#define restore_fpu() \
  __asm__ __volatile__( \
	"pop {r1}\n" \
	"vmsr fpexc, r1\n" \
	"pop {r1}\n" \
	"vmsr fpscr, r1\n" \
	"vpop {d16-d31}\n" \
	"vpop {d0-d15}\n" \
  )
#else
#define save_fpu() do {} while (0)
#define restore_fpu() do {} while (0)
#endif

__attribute__ ((interrupt("IRQ"))) void IRQHandler (void)
{
	unsigned int irq_num;

	save_fpu();

	/* __asm__ __volatile__("cpsid i" : : : "memory", "cc"); */

	irq_num = GIC_AcknowledgePending();
	GIC_ClearPendingIRQ(irq_num);

	if (irq_num < MAXIRQNUM && isr_table[irq_num] != NULL) {
		isr_table[irq_num]();
	} else {
		uart_printf("No handler found for irq %u.\n", irq_num);
	}

	GIC_EndInterrupt(irq_num);

	restore_fpu();
}

__attribute__ ((interrupt("FIQ"))) void FIQHandler (void)
{
	save_fpu();

	uart_puts("yaay FIQ ticked\n");

	restore_fpu();
}
