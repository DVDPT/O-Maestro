

.text
.global common_trap


common_trap:

	sub lr,lr,#4
	stmfd sp!,{r0-r12,lr}
	mov r0,sp
	mov r1,#0x1
	add r1, r1,#0x1
	ldmfd sp!, {r0-r12,pc}^
