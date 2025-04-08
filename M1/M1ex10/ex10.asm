    .cdecls "msp430.h"
    .global main

    .text

reduceSum8:
    push        R4
    add.b       @R12+,R4
    dec         R13
    jnz         reduceSum8
    jz          exit

exit:
    mov         R4,R12
    pop         R4
    ret

main:
    mov.w       #(WDTPW|WDTHOLD), &WDTCTL
    mov         #0,R4

    mov         #vetor, R12
    mov         #10, R13
    call        #reduceSum8
    jmp         $
    nop

    .data ; Início da RAM
vetor: .byte 1,2,3,4,5,6,7,8,9,10 ; Vetor de bytes