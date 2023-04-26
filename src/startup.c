#include <stdio.h>
#include "cortex_config.h"
#include "interrupt.h"
#include "startup.h"
#include "pl011.h"

/* First code to execute on startup (via first entry in VectorTable).
 * CPU boots into SVC/Supervisor Mode 0x13.
 */
static void __attribute__ ((naked)) ResetHandler (void)
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

	/* setup SVC stack */
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
	/* initialize all stacks to a known value */
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
	/* initialize floating point unit (fpu) cp10 and cp11 */
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

	/* Set stack pointer for different modes.
	 * Enable interrupts. */
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
		"ldr sp, =stackUSR\n");

#if	CONFIG_INIT_ARRAY
	__asm__ __volatile__("bl __libc_init_array");
#endif

	/* In user mode call main(). */
	__asm__ __volatile__(
		/* mov r0, #0 */
		/* mov r1, #0 */
		"bl main" : : "X" (&main));

#if	CONFIG_INIT_ARRAY
	__asm__ __volatile__("bl __libc_fini_array");
#endif

	__asm__ __volatile__("b LoopHandler");
}

static void __attribute__ ((interrupt("UNDEF"),noreturn)) UndefinedHandler (void)
{
	uint32_t UndefinedExceptionAddr;

	uart_puts("Undefined instruction at address 0x");
#ifdef	__GNUC__
	/* Store instruction causing undefined exception */
	__asm__ __volatile__("sub %0, lr, #4" : "=r" (UndefinedExceptionAddr));
#else
	#error "Unsupported compiler."
#endif
	uart_print_hex(UndefinedExceptionAddr);
	uart_puts(".\n");

	LoopHandler();
}

static void __attribute__ ((interrupt("SWI"))) SVCHandler (void)
{
#if     0
	/* Record SVC number called and pointer to saved register on the stack. */
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

	uart_puts("yaay SVC ticked\n");

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

static void __attribute__ ((interrupt("ABORT"),noreturn)) PrefetchAbortHandler (void)
{
	uint32_t FaultStatus, PrefetchAbortAddr;

	arm_errata_775420();

	uart_puts("Prefetch abort with Instruction Fault Status Register 0x");
#ifdef __GNUC__
	__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 1" : "=r" (FaultStatus));
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_INST_FAULT_STATUS, FaultStatus);
#else
	volatile register uint32_t Reg __asm(XREG_CP15_INST_FAULT_STATUS);
	FaultStatus = Reg;
#endif
	uart_print_hex(FaultStatus);
	uart_puts(".\n");

	uart_puts("Prefetch abort at instruction address 0x");
#ifdef __GNUC__
	/* Store instruction causing prefetch abort */
	__asm__ __volatile__("sub %0, lr, #4" : "=r" (PrefetchAbortAddr));
#else
	#error "Unsupported compiler."
#endif
	uart_print_hex(PrefetchAbortAddr);
	uart_puts(".\n");

	LoopHandler();
}

static void __attribute__ ((interrupt("ABORT"),noreturn)) DataAbortHandler (void)
{
	uint32_t FaultStatus, DataAbortAddr;

	arm_errata_775420();

	uart_puts("Data abort with Data Fault Status Register 0x");
#ifdef __GNUC__
	__asm__ __volatile__("mrc p15, 0, %0, c5, c0, 0" : "=r" (FaultStatus));
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_DATA_FAULT_STATUS, FaultStatus);
#else
	volatile register uint32_t Reg __asm(XREG_CP15_DATA_FAULT_STATUS);
	FaultStatus = Reg;
#endif
	uart_print_hex(FaultStatus);
	uart_puts(".\n");

	uart_puts("Data abort at instruction address 0x");
#ifdef __GNUC__
	/* Store instruction causing data abort */
	__asm__ __volatile__("sub %0, lr, #8" : "=r" (DataAbortAddr));
#else
	#error "Unsupported compiler."
#endif
	uart_print_hex(DataAbortAddr);
	uart_puts(".\n");

	LoopHandler();
}

void __attribute__ ((naked,noreturn)) LoopHandler (void)
{
	__asm__ __volatile__(
		"loop:\n"
		"wfe\n"			/* XXX wfi or wfe ??? */
		"b loop\n"
	);
}

void __attribute__ ((section(".isr_vector"),naked,used)) VectorTable (void)
{
	__asm__ __volatile__("b ResetHandler" : : "X" (&ResetHandler));
	__asm__ __volatile__("b UndefinedHandler" : : "X" (&UndefinedHandler));
	__asm__ __volatile__("b SVCHandler" : : "X" (&SVCHandler));
	__asm__ __volatile__("b PrefetchAbortHandler" : : "X" (&PrefetchAbortHandler));
	__asm__ __volatile__("b DataAbortHandler" : : "X" (&DataAbortHandler));
	__asm__ __volatile__("b LoopHandler" : : "X" (&LoopHandler));
	__asm__ __volatile__("b IRQHandler" : : "X" (&IRQHandler));
	__asm__ __volatile__("b FIQHandler" : : "X" (&FIQHandler));
}
