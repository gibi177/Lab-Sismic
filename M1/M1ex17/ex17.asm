    .cdecls "msp430.h"
    .global main

    .text

main:
    mov.w       #(WDTPW|WDTHOLD), &WDTCTL

    mov         #5, R13            ; Tamanho do vetor
    mov         #vetor, R12         ; Endereço do vetor

    call        #maior16

    jmp         $                   ; Fim do programa (laço infinito)
    nop


; =====> Sub-rotina para maior valor de vetor de palavras (16 bits)
maior16:
    push        R14
    push        R15

    mov.w       @R12+, R15          ; Carrega primeiro elemento como maior até agora
    dec         R13                 ; Já processamos 1 elemento

loop1:
    tst         R13
    jz          conta_frequencia

    cmp.w       @R12,R15           ; compara maior até agora com próximo valor
    jlo         novo_maior         ; se valor atual > R15 → novo maior

    add         #2, R12             ; avança para próxima palavra
    dec         R13
    jmp         loop1

novo_maior:
    mov.w       @R12+, R15          ; atualiza maior valor e avança
    dec         R13
    jmp         loop1


; =====> Contando frequência do maior número
conta_frequencia:
    mov         #0, R14             ; contador
    mov         #5, R13            ; reseta tamanho
    mov         #vetor, R12         ; reseta ponteiro

loop2:
    tst         R13
    jz          fim

    cmp.w       @R12+, R15
    jne         proximo
    inc         R14

proximo:
    dec         R13
    jmp         loop2

fim:
    mov.w       R15, R12            ; R12 recebe maior valor
    mov.w       R14, R13            ; R13 recebe frequência

    pop         R15
    pop         R14
    ret


; =====> Vetor de palavras (word = 16 bits)
    .data
vetor: .word 1, 2, 3, 4, 5 
