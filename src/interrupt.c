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

#if CONFIG_ISR_ASM
void SWInterrupt (void)
#else
void __attribute__ ((interrupt("SWI"),used)) SVCHandler (void)
#endif
{
	printf("yaay SVC ticked\n");
}

#if ! CONFIG_ISR_ASM && CONFIG_ARM_ERRATA_775420
#define dsb() __asm__ __volatile__("dsb" : : : "memory")
#define arm_errata_775420() dsb()
#else
#define arm_errata_775420() do {} while (0)
#endif

#if CONFIG_ISR_ASM || ! CONFIG_ARM_NEON
#define save_fpu() do {} while (0)
#define restore_fpu() do {} while (0)
#else
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
#endif

#if CONFIG_ISR_ASM
void FIQInterrupt (void)
#else
void __attribute__ ((interrupt("FIQ"),used)) FIQHandler (void)
#endif
{
	save_fpu();

	printf("yaay FIQ ticked\n");

	restore_fpu();
}

#if CONFIG_ISR_ASM
void IRQInterrupt (void)
#else
void __attribute__ ((interrupt("IRQ"),used)) IRQHandler (void)
#endif
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

uint32_t UndefinedExceptionAddr;
uint32_t PrefetchAbortAddr;
uint32_t DataAbortAddr;

#if CONFIG_ISR_ASM
void __attribute__ ((noreturn)) UndefinedException (void)
#else
void __attribute__ ((interrupt("UNDEF"),noreturn,used)) UndefinedHandler (void)
#endif
{
#ifdef DEBUG
	printf("Undefined instruction at address %lx\n", UndefinedExceptionAddr);
#endif
	while(1) {
	}
}

#if CONFIG_ISR_ASM
void __attribute__ ((noreturn)) PrefetchAbortInterrupt (void)
#else
void __attribute__ ((interrupt("ABORT"),noreturn,used)) PrefetchAbortHandler (void)
#endif
{
	arm_errata_775420();

#ifdef DEBUG
	{
		uint32_t FaultStatus;
#ifdef __GNUC__
		__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 1" : "=r" (FaultStatus));
#elif defined (__ICCARM__)
		mfcp(XREG_CP15_INST_FAULT_STATUS, FaultStatus);
#else
		volatile register uint32_t Reg __asm(XREG_CP15_INST_FAULT_STATUS);
		FaultStatus = Reg;
#endif
		printf("Prefetch abort with Instruction Fault Status Register %lx\n", FaultStatus);
		printf("Prefetch abort at instruction address %lx\n", PrefetchAbortAddr);
	}
#endif

	while (1) {
	}
}

#if CONFIG_ISR_ASM
void __attribute__ ((noreturn)) DataAbortInterrupt (void)
#else
void __attribute__ ((interrupt("ABORT"),noreturn,used)) DataAbortHandler (void)
#endif
{
	arm_errata_775420();

#ifdef DEBUG
	{
		uint32_t FaultStatus;
#ifdef __GNUC__
		__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 0" : "=r" (FaultStatus));
#elif defined (__ICCARM__)
		mfcp(XREG_CP15_DATA_FAULT_STATUS, FaultStatus);
#else
		volatile register uint32_t Reg __asm(XREG_CP15_DATA_FAULT_STATUS);
		FaultStatus = Reg;
#endif
		printf("Data abort with Data Fault Status Register %lx\n", FaultStatus);
		printf("Data abort at instruction address %lx\n", DataAbortAddr);
	}
#endif

	while (1) {
	}
}

#if ! CONFIG_ISR_ASM
void __attribute__ ((section(".isr_vector"),naked,used)) _Reset (void)
{
	__asm__ __volatile__(
		"b ResetHandler\n"
		"b UndefinedHandler\n"
		"b SVCHandler\n"
		"b PrefetchAbortHandler\n"
		"b DataAbortHandler\n"
		"b LoopHandler\n"
		"b IRQHandler\n"
		"b FIQHandler\n"
	);
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
