//Led verde = P4.7
//Chave S1 = P2.1

#include <msp430.h>

int main(void) {
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT

  //Configura P2.1 (S1) como entrada
  P2DIR &= ~BIT1;                 // P2.1 settado em 0 (entrada)
  P2REN |= BIT1;                  // Habilita resistor interno
  P2OUT |= BIT1;                  // Resistor de pull-up (pino fica em '1' quando botão solto)

  //Configura P4.7 como saida
  P4DIR |= BIT7;                  //P4.7 settado em 1 (saida)

  while(1) {
    if ((P2IN & BIT1) == 0)       //Se btn press -> P2.1 = 0
      P4OUT = BIT7;               //acende led
    else 
      P4OUT &= ~BIT7;             //se n, apaga o led
  }
}
