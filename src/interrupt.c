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

void __attribute__ ((interrupt("SWI"),used)) SVCHandler (void)
{
#if     0
	mov r1, sp
	mrs r0, spsr
	push {r0,r3}
	tst r0, #0x20                   /* check T bit for ARM or Thumb mode */
	ldrneh r0, [lr,#-2]             /* Thumb mode */
	bicne r0, r0, #0xff00           /* Thumb mode */
	ldreq r0, [lr,#-4]              /* ARM mode */
	biceq r0, r0, #0xff000000       /* ARM mode */
	/* r0 now contains SVC number */
	/* r1 now contains pointer to stacked registers */
#endif

	printf("yaay SVC ticked\n");

#if     0
	pop {r0,r3}
	msr spsr_cf, r0
#endif
}

#if CONFIG_ARM_ERRATA_775420
#define dsb() __asm__ __volatile__("dsb" : : : "memory")
#define arm_errata_775420() dsb()
#else
#define arm_errata_775420() do {} while (0)
#endif

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

void __attribute__ ((interrupt("FIQ"),used)) FIQHandler (void)
{
	save_fpu();

	printf("yaay FIQ ticked\n");

	restore_fpu();
}

void __attribute__ ((interrupt("IRQ"),used)) IRQHandler (void)
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

void __attribute__ ((interrupt("UNDEF"),noreturn,used)) UndefinedHandler (void)
{
#if	0
	ldr r0, =UndefinedExceptionAddr /* Store instruction causing undefined exception */
	sub r1, lr, #4
	str r1, [r0]
#endif

#ifdef DEBUG
	printf("Undefined instruction at address %lx\n", UndefinedExceptionAddr);
#endif
	while(1) {
	}
}

void __attribute__ ((interrupt("ABORT"),noreturn,used)) PrefetchAbortHandler (void)
{
	arm_errata_775420();

#if	0
        ldr r0, =PrefetchAbortAddr      /* Store instruction causing prefetch abort */
        sub r1, lr, #4
        str r1, [r0]
#endif

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

void __attribute__ ((interrupt("ABORT"),noreturn,used)) DataAbortHandler (void)
{
	arm_errata_775420();

#if	0
	ldr r0, =DataAbortAddr          /* Store instruction causing data abort */
	sub r1, lr, #8
	str r1, [r0]
#endif

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

void __attribute__ ((naked,used)) ResetHandler (void)
{
#if	CONFIG_SMP
	/* only cpu0 continues, all others loop */
	__asm__ __volatile__(
		"mrc p15, #0, r1, c0, c0, #5\n"
		"and r1, r1, #3\n"
		"cmp r1, #0\n"
		"bne LoopHandler\n");
#endif

	/* set VBAR to vector address table */
	__asm__ __volatile__(
		"ldr r0, =0x60000000\n"
		"mcr p15, 0, r0, c12, c0, 0\n");

	/* CPU boots into SVC/Supervisor Mode 0x13: */
	__asm__ __volatile__("ldr sp, =stackSVC");

	/* Clear BSS */
	__asm__ __volatile__(
		"ldr r0, =__bss_start\n"
		"ldr r1, =end\n"
		"mov r2, #0\n"
	"bss_clear_loop:\n"
		"cmp r0, r1\n"
		"strlo r2, [r0], #4\n"
		"blo bss_clear_loop\n");

#if	CONFIG_STACK_INIT
	__asm__ __volatile__(
		"ldr r0, =__stack_start\n"
		"ldr r1, =__stack_end\n"
		"movw r2, #0xbeef\n"
		"movt r2, #0xdead\n"
	"stack_init_loop:\n"
		"cmp r0, r1\n"
		"strlo r2, [r0], #4\n"
		"blo stack_init_loop\n");
#endif

#if	CONFIG_ARM_NEON
	__asm__ __volatile__(
		"mrc p15, 0, r0, c1, c1, 2\n"	/* set NSACR bits 11:10 for access to CP10 and CP11 */
		"orr r0, r0, #0xc00\n"
		"mcr p15, 0, r0, c1, c1, 2\n"
		"mrc p15, 0, r0, c1, c0, 2\n"	/* set CPACR for access to CP10 and CP11 */
		"orr r0, r0, #0xf00000\n"
		"mcr p15, 0, r0, c1, c0, 2\n"
#if	0
		fmrx r0, fpexc
		orr r0, r0, #0x40000000
		fmxr fpexc, r0
#else
		"mov r0, #0x40000000\n"		/* set FPEXC EN bit to enable the FPU */
		"vmsr fpexc, r0\n");
#endif
#endif

#if	0
		mrc p15, 0, r0, c1, c0, 0	/* flow prediction enable */
		orr r0, r0, #(0x01 << 11)
		mcr p15, 0, r0, c1, c0, 0
#endif

#if	0
		mrs r0, cpsr			/* enable asynchronous abort exception */
		bic r0, r0, #0x100
		msr cpsr_xsf, r0
#endif

	__asm__ __volatile__(
		"cps #0x1b\n"
		"ldr sp, =stackUND\n"

		"cps #0x17\n"
		"ldr sp, =stackABT\n"

		"cps #0x12\n"
		"ldr sp, =stackIRQ\n"

		"cps #0x11\n"
		"ldr sp, =stackFIQ\n"

		/* cps #0x1f */
		/* ldr sp, =stackSYS */

		"cpsie if\n"

		"cps #0x10\n"
		"ldr sp, =stackUSR\n"

		/* bl __libc_init_array */

		/* mov r0, #0 */
		/* mov r1, #0 */
		"bl main\n"

		/* bl __libc_fini_array */

		"b LoopHandler\n");
}

void __attribute__ ((naked,used)) LoopHandler (void)
{
	__asm__ __volatile__(
		"loop:\n"
		"wfe\n"			/* XXX wfi or wfe ??? */
		"b loop\n"
	);
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
