#include "cortex_config.h"

.section .isr_vector, "x"
.global _Reset
_Reset:
	b reset_handler
	b loop
	b c_svc
	b loop
	b loop
	b loop
	b c_irq
	b c_fiq

.text

reset_handler:

#if	CONFIG_SMP
	/* only cpu0 continues, all others loop */
	mrc p15,#0,r1,c0,c0,#5
	and r1, r1, #3
	cmp r1, #0
	bne loop
#endif

	/* set vector address */
	ldr r0, =0x60000000
	mcr p15,0,r0,c12,c0,0

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

	//cps #0x1b
	//ldr sp, =stackUND

	//cps #0x17
	//ldr sp, =stackABT

	cps #0x12
	ldr sp, =stackIRQ

	cps #0x11
	ldr sp, =stackFIQ

	cpsie i

	cps #0x10
	ldr sp, =stackUSR

	bl main
loop:
	wfi
	b loop

