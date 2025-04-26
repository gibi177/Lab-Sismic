    .cdecls "msp430.h"
    .global main

    .text



main:
  	mov.w #(WDTPW|WDTHOLD), &WDTCTL
	mov #matriz, R11 ; Ponteiro para a matriz de entrada
	mov #1, R12 ; Número de linhas da matriz
	mov #1, R13 ; Número de colunas da matriz
	mov	#0,r14	;endereço da matriz transposta
	call #MAT_TRANSP ; Chamar sub-rotina
	jmp $ ; Loop infinito
	nop

multiply:
	push	r4
	

	clr     r4          ; R6 = acumulador
    

multiply_loop:
	add		r7,r4
	dec		r6
	jnz		multiply_loop
	mov		r4,R7
	
	pop		r4
	ret

multiply2:
	push	r4
	

	clr     r4          ; R6 = acumulador
    

multiply2_loop:
	add		r8,r4
	dec		r6
	jnz		multiply2_loop
	mov		r4,r8
	
	pop		r4
	ret

endereco_transposta:
    push    r4
    push    r5
    push    r6

    clr     r6          ; R6 = acumulador
    mov     r12, r5     ; R5 = número de linhas
    mov     r13, r4     ; R4 = número de colunas (contador do loop)

endereco_transposta_loop:
    add     r5, r6      ; R6 += linhas
    dec     r4
    jnz     endereco_transposta_loop

    rla     r6          ; R6 = R6 * 2 (cada elemento ocupa 2 bytes)
    add     r11, r6     ; endereço da transposta = endereço da matriz + deslocamento
    mov     r6, r14     ; guarda resultado em R14

    pop     r6
    pop     r5
    pop     r4
    ret

MAT_TRANSP:
    push r4                 ; coluna atual
    push r5                 ; linha atual
    push r6                 ; endereço temporário
    push r7                 ; offset original
    push r8                 ; offset transposta
    push r9                 ; valor temporário

    ; Calcula endereço da matriz transposta
    call #endereco_transposta

    clr r4                  ; r4 = coluna (j)
loop_col:
    cmp r13, r4
    jge fim_transposicao

    clr r5                  ; r5 = linha (i)
loop_lin:
    cmp r12, r5
    jge prox_coluna

    ; Calcula offset original: (i * colunas + j) * 2
    mov r5, r7              ; R7 = i
    mov r13, r6             ; R6 = colunas
    call #multiply          ; R7 = i * colunas
    add r4, r7              ; R7 += j
    rla r7                  ; R7 *= 2 (bytes)

    ; Carrega valor da matriz original
    mov r11, r6
    add r7, r6              ; R6 = endereço de matriz[i][j]
    mov @r6, r9             ; R9 = valor

    ; Calcula offset transposta: (j * linhas + i) * 2
    mov r4, r8              ; R8 = j
    mov r12, r6             ; R6 = linhas
    call #multiply2          ; R8 = j * linhas
    add r5, r8              ; R8 += i
    rla r8                  ; R8 *= 2 (bytes)

    ; Armazena valor na transposta
    mov r14, r6
    add r8, r6              ; R6 = endereço de transposta[j][i]
    mov r9, 0(r6)           ; transposta[j][i] = valor

    inc r5
    jmp loop_lin

prox_coluna:
    inc r4
    jmp loop_col

fim_transposicao:
    pop r9
    pop r8
    pop r7
    pop r6
    pop r5
    pop r4
    ret

    .data
matriz: .word 1