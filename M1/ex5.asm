sum:
    ADD 	R4,R5
    JZ		zero			;if zero, jump to zero 
    JN		negative		;if negative, jump to negative
	JMP		positive		;unconditional jump to positive

positive:
    INC		R5				;Increments R5
    JMP   	exit 	

negative:
    DEC   	R5				;Decrements R5
    JMP   	exit 	

zero:
	NOP						;doesnt do anything

exit:
	RET						;returns to main

main:
    MOV		#2, R4
    MOV		#3, R5			;R4=2, R5=3
	CALL 	sum
