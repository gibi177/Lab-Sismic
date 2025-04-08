    .cdecls "msp430.h"
    .global main

    .text

main:
    mov.w       #(WDTPW|WDTHOLD), &WDTCTL  ; Desabilita o watchdog timer
    
    mov     #0, R4              ; Primeiro elemento, 0
    mov     #1, R5              ; Segundo elemento, 1
    mov     #0x2400, R6         ; Endereço base para armazenar os valores
    mov     #18, R7             ; Contador

    mov.w   R4, 0(R6)           ; 0 no dado de 0x2400
    add     #2, R6              ; Incrementa para a próxima posição de memória
    mov.w   R5, 0(R6)           ; 1 no dado de 0x2402 (posicao da memoria incrementada)

fibonacci_loop:
    add     -2(R6), R5          ; Próximo numero da sequencia
    add     #2, R6              ; Incrementa para a próxima posição de memória
    mov     R5, 0(R6)           ; Armazena o número calculado na memória
    dec     R7                  
    jnz     fibonacci_loop      

exit:
    ret
    jmp     $                   
    nop