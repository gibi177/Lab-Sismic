    .cdecls "msp430.h"
    .global main
    .text

main:
    mov.w           #(WDTPW|WDTHOLD), &WDTCTL

    mov             #matriz, R11                   ; Endereço da matriz
    mov             #3, R12                        ; Número de linhas
    mov             #3, R13                        ; Número de colunas
    call            #SUM_SUB

    jmp $
    nop

SUM_SUB:
    push            R4                            ; Salva resultado da primeira soma
    push            R5                            ; Salva resultado da segunda soma
    push            R6                            ; Salva resultado da terceira soma
    push            R7                            ; Salva resultado da quarta soma
    push            R9                            ; Salva número de colunas
    push            R10                           ; Salva número de linhas

    mov             R11, R8
    mov             R13, R9
    mov             R12, R10
    clr             R4
    clr             R5
    clr             R6
    clr             R7

; Zera as somas se a matriz tiver apenas uma linha ou uma coluna
    cmp             #1, R12
    jz              SUM_SUB_1line_or_1collum
    cmp             #1, R13
    jz              SUM_SUB_1line_or_1collum

SUM_SUB_1l_1c:                         ; Posiciona o ponteiro
    rla             R13
    add             #2, R13
    add             R13, R11
    mov             R9, R13
    dec             R13
    dec             R12

SUM_SUB_1l_1c_loop:                    ; Soma exceto 1ª linha e 1ª coluna
    add             @R11, R4
    add             #2, R11
    dec             R13

    jnz             SUM_SUB_1l_1c_loop
    add             #2, R11
    mov             R9, R13
    dec             R13
    dec             R12

    jnz             SUM_SUB_1l_1c_loop
    mov             R9, R13
    mov             R10, R12
    mov             #matriz, R11

SUM_SUB_1l_uc:                         ; Posiciona o ponteiro
    rla             R13
    add             R13, R11
    mov             R9, R13
    dec             R13
    dec             R12

SUM_SUB_1l_uc_loop:                    ; Soma exceto 1ª linha e última coluna
    add             @R11, R5
    add             #2, R11
    dec             R13

    jnz             SUM_SUB_1l_uc_loop
    add             #2, R11
    mov             R9, R13
    dec             R13
    dec             R12

    jnz             SUM_SUB_1l_uc_loop
    mov             R9, R13
    mov             R10, R12
    mov             #matriz, R11
    dec             R13
    dec             R12

SUM_SUB_ul_uc:                         ; Soma exceto últimas linha e coluna
    add             @R11, R7
    add             #2, R11
    dec             R13

    jnz             SUM_SUB_ul_uc
    add             #2, R11
    mov             R9, R13
    dec             R13
    dec             R12

    jnz             SUM_SUB_ul_uc
    mov             R9, R13
    mov             R10, R12
    mov             #matriz, R11
    dec             R13
    dec             R12

SUM_SUB_ul_1c:                         ; Soma exceto última linha e 1ª coluna
    add             #2, R11
    add             @R11, R6
    dec             R13

    jnz             SUM_SUB_ul_1c
    add             #2, R11
    mov             R9, R13
    dec             R13
    dec             R12

    jnz             SUM_SUB_ul_1c

SUM_SUB_end:
    mov             R4, R11
    mov             R5, R12
    mov             R6, R13
    mov             R7, R14
    pop             R10
    pop             R9
    pop             R7
    pop             R6
    pop             R5
    pop             R4
    ret

SUM_SUB_1line_or_1collum:
    clr             R11
    clr             R12
    clr             R13
    clr             R14
    jmp             SUM_SUB_end
    nop

    .data
matriz: .word 1, 2, 3, 4,5,6,7,8,9