/*	.section	.startup, "ax"
	.global	_start

_start:
	ldr sp, =__stack_end__
	mov fp,sp
	bl startUpYAAB_2294
	ldr r0, =__bss_end__
	ldr r1,= __bss_start__

iniciar_bss:
	mov r2,#0
	cmp r1,r0
	beq callMain
	str r2,[r1],#2
	b iniciar_bss

ctors:
	ldr r0,=__ctors_init__
	ldr r0,[r0]
	mov lr,pc
	bx r0

callMain:
	mov	r0, #0
	mov	r1, #0
	bl	main
	b	.


	.section	.stack
	.space	1024

*/


