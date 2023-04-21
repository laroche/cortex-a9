#include <stdio.h>
#include "cortex_config.h"
#include "interrupt.h"
#include "pl050.h"

#define MAXIRQNUM 50U

static func_t isr_table[MAXIRQNUM];

void install_isr (IRQn_Type irq_num, func_t handler)
{
	if (irq_num < MAXIRQNUM) {
		isr_table[irq_num] = handler;
	} else {
#ifdef	DEBUG
		printf("Too big irq number %d.\n", irq_num);
#endif
	}
}

void enable_irq (IRQn_Type irq_num)
{
	GIC_EnableIRQ(irq_num);
}

void interrupt_init (void)
{
	GIC_Enable();
	/* __asm__ __volatile__("cpsie i" : : : "memory", "cc"); */
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
	int irq_num;

	save_fpu();

	/* __asm__ __volatile__("cpsid i" : : : "memory", "cc"); */

	irq_num = GIC_AcknowledgePending();
	GIC_ClearPendingIRQ(irq_num);

	if (isr_table[irq_num] != NULL) {
		isr_table[irq_num]();
	} else {
		printf("no handler found for %d\n", irq_num);
	}

	GIC_EndInterrupt(irq_num);

	restore_fpu();
}

void __attribute__ ((interrupt("FIQ"))) FIQHandler (void)
{
	save_fpu();

	printf("yaay FIQ ticked\n");

	restore_fpu();
}
