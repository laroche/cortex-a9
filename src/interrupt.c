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

void install_isr (IRQn_Type irq_num, func_t handler)
{
	if (irq_num < MAXIRQNUM) {
		isr_table[irq_num] = handler;
	} else {
#ifdef	DEBUG
		uart_puts("Too big irq number ");
		uart_print_int((unsigned long) irq_num);
		uart_puts(".\n");
#endif
	}
}

void enable_irq (IRQn_Type irq_num)
{
	GIC_EnableIRQ(irq_num);
}

#if CONFIG_ARM_NEON
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

void __attribute__ ((interrupt("IRQ"))) IRQHandler (void)
{
	unsigned int irq_num;

	save_fpu();

	/* __asm__ __volatile__("cpsid i" : : : "memory", "cc"); */

	irq_num = GIC_AcknowledgePending();
	GIC_ClearPendingIRQ(irq_num);

	if (irq_num < MAXIRQNUM && isr_table[irq_num] != NULL) {
		isr_table[irq_num]();
	} else {
		uart_puts("No handler found for irq ");
		uart_print_int((unsigned long) irq_num);
		uart_puts(".\n");
	}

	GIC_EndInterrupt(irq_num);

	restore_fpu();
}

void __attribute__ ((interrupt("FIQ"))) FIQHandler (void)
{
	save_fpu();

	uart_puts("yaay FIQ ticked\n");

	restore_fpu();
}
