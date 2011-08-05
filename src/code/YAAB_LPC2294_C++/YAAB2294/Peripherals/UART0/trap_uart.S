.text
.global trap_uart


trap_uart:

	sub lr,lr,#4
	stmfd sp!,{r0-r12,lr}
	mov r0,sp
	bl doSomething
	//bl endProcessTreatmentInterrupt
	ldmfd sp!, {r0-r12,pc}

	.end
