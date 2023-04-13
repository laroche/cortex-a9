#include <stdio.h>
#include "cortex_config.h"
#include "interrupt.h"
#include "pl050.h"

#define MAXIRQNUM 50U

static func_t isr_table[MAXIRQNUM];

void install_isr (IRQn_Type irq_num, func_t handler)
{
	isr_table[irq_num] = handler;
}

#if CONFIG_ISR_ASM
void SWInterrupt (void)
#else
void __attribute__ ((interrupt("SWI"))) c_svc (void)
#endif
{
	printf("yaay SVC ticked\n");
}

void __attribute__ ((interrupt("FIQ"))) c_fiq (void)
{
	printf("yaay FIQ ticked\n");
}

void __attribute__ ((interrupt("IRQ"))) c_irq (void)
{
	int irq_num;

	/* __asm__ __volatile__("cpsid i" : : : "memory", "cc"); */

	irq_num = GIC_AcknowledgePending();
	GIC_ClearPendingIRQ(irq_num);

	if (isr_table[irq_num] != NULL) {
		isr_table[irq_num]();
	} else {
		printf("no handler found for %d\n", irq_num);
	}

	GIC_EndInterrupt(irq_num);
}

#if CONFIG_ISR_ASM
uint32_t UndefinedExceptionAddr;
uint32_t PrefetchAbortAddr;
uint32_t DataAbortAddr;

void __attribute__ ((noreturn)) UndefinedException (void)
{
#ifdef DEBUG
	printf("Undefined instruction at address %lx\n", UndefinedExceptionAddr);
#endif
	while(1) {
	}
}

void __attribute__ ((noreturn)) PrefetchAbortInterrupt (void)
{
#ifdef DEBUG
	uint32_t FaultStatus;
#ifdef __GNUC__
	uint32_t rval = 0U;

	__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 1\n": "=r" (rval));
	FaultStatus = rval;
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_INST_FAULT_STATUS,FaultStatus);
#else
	volatile register uint32_t Reg __asm(XREG_CP15_INST_FAULT_STATUS);
	FaultStatus = Reg;
#endif
	printf("Prefetch abort with Instruction Fault Status Register %lx\n", FaultStatus);
	printf("Prefetch abort at instruction address %lx\n", PrefetchAbortAddr);
#endif
	while (1) {
	}
}

void __attribute__ ((noreturn)) DataAbortInterrupt (void)
{
#ifdef DEBUG
	uint32_t FaultStatus;
#ifdef __GNUC__
	uint32_t rval = 0U;

	__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 0\n": "=r" (rval));
	FaultStatus = rval;
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_DATA_FAULT_STATUS, FaultStatus);
#else
	volatile register uint32_t Reg __asm(XREG_CP15_DATA_FAULT_STATUS);
	FaultStatus = Reg;
#endif
	printf("Data abort with Data Fault Status Register %lx\n", FaultStatus);
	printf("Data abort at instruction address %lx\n", DataAbortAddr);
#endif
	while (1) {
	}
}
#endif

void enable_irq (IRQn_Type irq_num)
{
	GIC_EnableIRQ(irq_num);
}

void interrupt_init (void)
{
	GIC_Enable();
	/* __asm__ __volatile__("cpsie i" : : : "memory", "cc"); */
}
