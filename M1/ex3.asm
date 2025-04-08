	.cdecls "msp430.h"
	.global main

	.text

saturated_sum:
	ADD 		R4,R5
	JC			carry				;if carry, jump to carry handling
	JMP 		exit				;if not, jump to exit

carry:
	MOV 		#0xFFFF,R5			
	JMP 		exit

exit:
	RET

main:
	MOV 		#8,R4
	MOV 		#8,R5
	CALL 		#saturated_sum
	MOV 		#5,R4
	MOV 		#2,R5
	CALL		saturated_sum
	JMP			$
	NOP
	