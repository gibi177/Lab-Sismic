//Led verde = P4.7
//Chave S1 = P2.1

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

  //Configura P4.7 como saída (LED verde)
  P4DIR |= BIT7;                   //saida            
  P4OUT &= ~BIT7;                  // Garante LED apagado no início

  while(1) {
    if (!(P2IN & BIT1)) {          // Se botão pressionado
      debounce();
      if (!(P2IN & BIT1)) {        // Se ainda estiver pressionado
        P4OUT ^= BIT7;             // Inverte estado do LED
        while (!(P2IN & BIT1));    // Aguarda botão ser solto
        debounce();                // Debounce após soltar
      }
    }
  }
}
