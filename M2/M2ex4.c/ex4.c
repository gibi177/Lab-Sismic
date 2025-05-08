//Led vermelho = P1.0
//Chave S1 = P2.1
//Chave S2 = P1.1

#include <msp430.h>

#define DELAY 5000

void debounce(void){
  volatile unsigned int cnt = DELAY;
  while(cnt--);  // espera debounce 
}

int main(void) {
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  //Configura P2.1 (S1) como entrada com pull-up
  P2DIR &= ~BIT1; //entrada               
  P2REN |= BIT1;  //ativa resistor      
  P2OUT |= BIT1;  //de pullup 

  //Configura P1.1 (S2) como entrada com pull-up
  P1DIR &= ~BIT1; //entrada               
  P1REN |= BIT1;  //ativa resistor      
  P1OUT |= BIT1;  //de pullup                

  //Configura P1.0 como saída (LED vermelho)
  P1DIR |= BIT0;                   //saida   
  P1OUT &= ~BIT0;                  // Garante LED apagado no início      

  unsigned char s1_anterior = 1;
  unsigned char s2_anterior = 1;   

  while(1) {
    unsigned char s1_atual = (P2IN & BIT1) ? 1 : 0;
    unsigned char s2_atual = (P1IN & BIT1) ? 1 : 0;

    if ((s1_anterior == 1 && s1_atual == 0) || (s2_anterior == 1 && s2_atual == 0)) {
        debounce();  // Aguarda estabilizar
        // Releitura após debounce
        s1_atual = (P2IN & BIT1) ? 1 : 0;
        s2_atual = (P1IN & BIT1) ? 1 : 0;

        if ((s1_anterior == 1 && s1_atual == 0) || (s2_anterior == 1 && s2_atual == 0)) {
            P1OUT ^= BIT0;  // Inverte LED
        }
    }

    // Atualiza estados anteriores
    s1_anterior = s1_atual;
    s2_anterior = s2_atual;
  }
}
