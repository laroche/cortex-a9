#include "cortex_config.h"

#if	CONFIG_ISR_ASM

.section .isr_vector, "x"

.global _Reset
_Reset:
	b ResetHandler
	b UndefinedHandler	/* undefined instruction */
	b SVCHandler
	b PrefetchAbortHandler
	b DataAbortHandler
	b LoopHandler		/* address exception */
	b IRQHandler
	b FIQHandler

#endif

.text

.global ResetHandler
ResetHandler:

#if	CONFIG_SMP
	/* only cpu0 continues, all others loop */
	mrc p15, #0, r1, c0, c0, #5
	and r1, r1, #3
	cmp r1, #0
	bne LoopHandler
#endif

	/* set VBAR to vector address table */
	ldr r0, =0x60000000
	mcr p15, 0, r0, c12, c0, 0

	/* CPU boots into SVC/Supervisor Mode 0x13: */
	ldr sp, =stackSVC

	ldr r0, =__bss_start
	ldr r1, =end
	mov r2, #0
bss_clear_loop:
	cmp r0, r1
	strlo r2, [r0], #4
	blo bss_clear_loop

#if	CONFIG_STACK_INIT
	ldr r0, =__stack_start
	ldr r1, =__stack_end
	movw r2, #0xbeef
	movt r2, #0xdead
stack_init_loop:
	cmp r0, r1
	strlo r2, [r0], #4
	blo stack_init_loop
#endif

#if	CONFIG_ARM_NEON
	mrc p15, 0, r0, c1, c1, 2	/* set NSACR bits 11:10 for access to CP10 and CP11 */
	orr r0, r0, #0xc00
	mcr p15, 0, r0, c1, c1, 2
	mrc p15, 0, r0, c1, c0, 2	/* set CPACR for access to CP10 and CP11 */
	orr r0, r0, #0xf00000
	mcr p15, 0, r0, c1, c0, 2
#if	0
	fmrx r0, fpexc
	orr r0, r0, #0x40000000
	fmxr fpexc, r0
#else
	mov r0, #0x40000000		/* set FPEXC EN bit to enable the FPU */
	vmsr fpexc, r0
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

	cps #0x1b
	ldr sp, =stackUND

	cps #0x17
	ldr sp, =stackABT

	cps #0x12
	ldr sp, =stackIRQ

	cps #0x11
	ldr sp, =stackFIQ

	/* cps #0x1f */
	/* ldr sp, =stackSYS */

	cpsie if

	cps #0x10
	ldr sp, =stackUSR

	/* bl __libc_init_array */

	/* mov r0, #0 */
	/* mov r1, #0 */
	bl main

	/* bl __libc_fini_array */

.global LoopHandler
LoopHandler:
	wfe				/* XXX wfi or wfe ??? */
	b LoopHandler


#if	CONFIG_ISR_ASM

.global UndefinedException
.global SWInterrupt
.global PrefetchAbortInterrupt
.global DataAbortInterrupt
.global IRQInterrupt
.global FIQInterrupt

UndefinedHandler:
	stmdb sp!, {r0-r3,r12,lr}

	ldr r0, =UndefinedExceptionAddr	/* Store instruction causing undefined exception */
	sub r1, lr, #4
	str r1, [r0]

	bl UndefinedException

	ldmia sp!, {r0-r3,r12,lr}
	movs pc, lr


SVCHandler:				/* SWI handler */
	stmdb sp!, {r0-r3,r12,lr}

#if	0
	mov r1, sp
	mrs r0, spsr
	push {r0,r3}
	tst r0, #0x20			/* check T bit for ARM or Thumb mode */
	ldrneh r0, [lr,#-2]		/* Thumb mode */
	bicne r0, r0, #0xff00		/* Thumb mode */
	ldreq r0, [lr,#-4]		/* ARM mode */
	biceq r0, r0, #0xff000000	/* ARM mode */
	/* r0 now contains SVC number */
	/* r1 now contains pointer to stacked registers */
#endif

	bl SWInterrupt

#if	0
	pop {r0,r3}
	msr spsr_cf, r0
#endif

	ldmia sp!, {r0-r3,r12,lr}
	movs pc, lr


#if CONFIG_ARM_ERRATA_775420
#define arm_errata_775420	 dsb
#else
#define arm_errata_775420
#endif

PrefetchAbortHandler:
	arm_errata_775420
	stmdb sp!, {r0-r3,r12,lr}

	ldr r0, =PrefetchAbortAddr	/* Store instruction causing prefetch abort */
	sub r1, lr, #4
	str r1, [r0]

	bl PrefetchAbortInterrupt

	ldmia sp!, {r0-r3,r12,lr}
	subs pc, lr, #4


DataAbortHandler:
	arm_errata_775420
	stmdb sp!, {r0-r3,r12,lr}

	ldr r0, =DataAbortAddr		/* Store instruction causing data abort */
	sub r1, lr, #8
	str r1, [r0]

	bl DataAbortInterrupt

	ldmia sp!, {r0-r3,r12,lr}
	subs pc, lr, #8


IRQHandler:
	stmdb sp!, {r0-r3,r12,lr}

#if	CONFIG_ARM_NEON
	vpush {d0-d15}
	vpush {d16-d31}
	vmrs r1, fpscr
	push {r1}
	vmrs r1, fpexc
	push {r1}
#endif

#if	CONFIG_PROFILING
	ldr r2, =prof_pc
	subs r3, lr, #0
	str r3, [r2]
#endif

	bl IRQInterrupt

#if	CONFIG_ARM_NEON
	pop {r1}
	vmsr fpexc, r1
	pop {r1}
	vmsr fpscr, r1
	vpop {d16-d31}
	vpop {d0-d15}
#endif

	ldmia sp!, {r0-r3,r12,lr}
	subs pc, lr, #4


FIQHandler:
	stmdb sp!, {r0-r3,r12,lr}

#if	CONFIG_ARM_NEON
	vpush {d0-d15}
	vpush {d16-d31}
	vmrs r1, fpscr
	push {r1}
	vmrs r1, fpexc
	push {r1}
#endif

	bl FIQInterrupt

#if	CONFIG_ARM_NEON
	pop {r1}
	vmsr fpexc, r1
	pop {r1}
	vmsr fpscr, r1
	vpop {d16-d31}
	vpop {d0-d15}
#endif

	ldmia sp!, {r0-r3,r12,lr}
	subs pc, lr, #4


#endif
