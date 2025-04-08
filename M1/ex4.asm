mult:
    PUSH    R4               ;salva R4 na pilha
    MOV     #0, R4           ;R4=0

mult_loop:
    ADD     R12,R4
    DEC     R13              ;decrementa R12 ate ser 0
    JNZ     mult_loop        ;se R12 != 0, repete a subrotina mult

mult_exit:
    MOV     R4,R12           ;move R4 p/ R12, que salvara o resultado
    POP     R4               ;retorna R4 para o valor inicial salvo na pilha
    RET                      ;return, subrotina acaba

main:                        ;(5 x 6) + 8
    MOV     #8, R4 
    MOV     #5, R12
    MOV     #6, R13          ;R12=5, R13=6
    CALL    mult
    ADD     R4,R12
    