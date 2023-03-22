.section .isr_vector
.global _Reset
_Reset:
	B reset_handler
	B .
	B c_svc
	B .
	B .
	B .
	B c_irq
	B c_fiq

.text
reset_handler:
	ldr r0, =0x60000000
	mcr p15,0,r0,c12,c0,0
	ldr sp, =stackSVC

	cps #0x1b
	ldr sp, =stackUND

	cps #0x17
	ldr sp, =stackABT

	cps #0x12
	ldr sp, =stackIRQ

	cps #0x11
	ldr sp, =stackFIQ

	cps #010
	ldr sp, =stackUSR

	ldr r0, =_start_bss
	ldr r1, =_end_bss
	mov r2, #0
bss_clear_loop:
	cmp r0, r1
	strlo r2, [r0], #4
	blo bss_clear_loop

	BL main
	B .

