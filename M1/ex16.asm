;Escreva a sub-rotina menor que tem como entradas:
;• R12 → Endereço de início de um vetor de bytes sem sinal
;• R13 → Tamanho do vetor
;e retorna:
;• R12 → Menor elemento do vetor e
;• R13 → Qual sua frequência (quantas vezes apareceu)
;Teste o programa com um vetor de 10 bytes usando valores distribuídos entre 0 e 255

    .cdecls "msp430.h"
    .global main

    .text

main:
    mov.w       #(WDTPW|WDTHOLD), &WDTCTL

    mov         #10,R13             ;tamanho do vetor 
    mov         #vetor, R12         ;endereço do vetor

    call        #menor

    jmp         $
    nop


; =====> Verifica menor elemento
menor:
    push        R14
    push        R15

    mov.b       @R12+,R15           ;menor numero, carregado como primeiro elemento do vetor
    dec         R13                 ;Decrementa tamanho (ja processamos um numero)

loop1:
    tst         R13                 ;testa se R13 e zero (0 AND 0 = 0)
    jz          freq_menor_num      ;fim da primeira varredura

    cmp.b       R15,@R12            ;compara elemento do vetor com o comparador (menor numero ate entao)
    jlo         novo_menor          ;jump se o dado do vetor for menor (novo menor numero)

    add         #1,R12              ;incrementa endereço do vetor
    dec         R13                 ;decrementa tamanho do vetor (um elemento ja foi)
    jmp         loop1               ;se nao for menor, jump incondicional para o loop novamente

novo_menor:
    mov.b       @R12+,R15           ;atualiza menor numero e incrementa endereço de memoria do vetor
    dec         R13                 ;decrementa tamanho do vetor (um elemento ja foi)
    jmp         loop1               ;senao, volta ao loop


; =====> Contando a frequencia do menor numero
freq_menor_num:                     
    mov         #0,R14              ;contador de frequencia(temporario)
    mov         #10,R13             ;tamanho do vetor 
    mov         #vetor, R12         ;endereço do vetor

loop2:
    tst         R13                 ;testa se R13 e zero (0 AND 0 = 0)
    jz          exit                         

    cmp.b       @R12+,R15           ;compara elemento do vetor com menor numero e incrementa endereço do vetor
    jne         nao_igual           ;jump if not equal
    inc         R14                 ;se for igual, incrementa contador de frequencia

nao_igual:
    dec         R13                 ;decrementa tamanho do vetor (um elemento ja foi)
    jmp         loop2               ;volta ao loop2


exit:
    mov         R15, R12            ;move menor numero para R12 conforme pedido
    mov         R14,R13             ;move frequencia do menor numero para R13 conforme pedido

    pop         R15
    pop         R14

    ret     
             

; =====> Dados
    .data ; Início da RAM
vetor: .byte 10,25,50,75,100,125,150,10,200,255 ; Vetor de bytes