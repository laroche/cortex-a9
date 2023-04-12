#include "cortex_config.h"

.section .isr_vector, "x"
.global _Reset
_Reset:
	b reset_handler
#if	CONFIG_ISR_ASM
	b UndefinedHandler
	b SVCHandler
#else
	b loop			/* undefined instruction */
	b c_svc			/* swi, svc */
#endif
#if	CONFIG_ISR_ASM
	b PrefetchAbortHandler
	b DataAbortHandler
#else
	b loop
	b loop
#endif
	b loop			/* address exception */
	b c_irq
	b c_fiq

.text

reset_handler:

#if	CONFIG_SMP
	/* only cpu0 continues, all others loop */
	mrc p15, #0, r1, c0, c0, #5
	and r1, r1, #3
	cmp r1, #0
	bne loop
#endif

	/* set VBAR to vector address table */
	ldr r0, =0x60000000
	mcr p15, 0, r0, c12, c0, 0

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

	//cps #0x1f
	//ldr sp, =stackSYS

	cpsie i

	cps #0x10
	ldr sp, =stackUSR

	bl main
loop:
	wfe				/* wfi or wfe ??? */
	b loop


#if	CONFIG_ISR_ASM

.global UndefinedException
.global SWInterrupt
.global PrefetchAbortInterrupt
.global DataAbortInterrupt

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


PrefetchAbortHandler:
#if	CONFIG_ARM_ERRATA_775420
	dsb
#endif
	stmdb sp!, {r0-r3,r12,lr}

	ldr r0, =PrefetchAbortAddr	/* Store instruction causing prefetch abort */
	sub r1, lr, #4
	str r1, [r0]

	bl PrefetchAbortInterrupt

	ldmia sp!, {r0-r3,r12,lr}
	subs pc, lr, #4


DataAbortHandler:
#if	CONFIG_ARM_ERRATA_775420
	dsb
#endif
	stmdb sp!, {r0-r3,r12,lr}

	ldr r0, =DataAbortAddr		/* Store instruction causing data abort */
	sub r1, lr, #8
	str r1, [r0]

	bl DataAbortInterrupt

	ldmia sp!,{r0-r3,r12,lr}
	subs pc, lr, #8

#endif
