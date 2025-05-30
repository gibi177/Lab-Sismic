#include <msp430f5529.h>

#define LED_VERMELHO BIT0   // P1.0
#define LED_VERDE    BIT7   // P4.7
#define S1           BIT1   // P2.1
#define S2           BIT1   // P1.1

volatile unsigned char contador = 0;

void debounce(void) {
    volatile unsigned int i = 5000;
    while(i--);
}

void atualizaLEDs() {
    switch(contador) {
        case 0:
            P4OUT &= ~LED_VERDE;  //Contador 0, os 2 apagados
            P1OUT &= ~LED_VERMELHO; 
            break;
        case 1:
            P4OUT |= LED_VERDE;  //Contador 1, LSB aceso
            P1OUT &= ~LED_VERMELHO; 
            break;
        case 2:
            P4OUT &= ~LED_VERDE;  //Contador 2, MSB aceso
            P1OUT |= LED_VERMELHO; 
            break;
        case 3:
            P4OUT |= LED_VERDE;  //Contador 3, os 2 acesos
            P1OUT |= LED_VERMELHO; 
            break;
    }
}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Parar watchdog

    //LEDs como saída
    P1DIR |= LED_VERMELHO;
    P1OUT &= ~LED_VERMELHO; //Começa desligado

    P4DIR |= LED_VERDE;
    P4OUT &= ~LED_VERDE;

    //Config S1 e S2 
    P2DIR &= ~S1;
    P2REN |= S1;
    P2OUT |= S1;
    P2IE  |= S1;      // Habilita interrupção local
    P2IES |= S1;      // Interrupção na borda de descida
    P2IFG &= ~S1;     // limpar flag pendente

    P1DIR &= ~S2;
    P1REN |= S2;
    P1OUT |= S2;
    P1IE  |= S2;      // Habilita interrupção local
    P1IES |= S2;      // Interrupção na borda de descida
    P1IFG &= ~S2;     // limpar flag pendente

    __enable_interrupt(); // Seta GIE = 1
    atualizaLEDs();       // Exibir estado inicial (0)

    while(1); //Loop infinito
}

//Interrupção de S1
#pragma vector = PORT2_VECTOR
__interrupt void PORT2_ISR() {
    if (P2IFG & S1) {
        debounce();
        if (!(P2IN & S1)) { //true quando press (nível lógico 0)
            contador = (contador + 1) % 4; //incremento circular
            atualizaLEDs();
        }
        P2IFG &= ~S1; //zera flag manualmente
    }
}

//Interrupção de S2
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR() {
    if (P1IFG & S2) {
        debounce();
        if (!(P1IN & S2)) { //true quando press (nível lógico 0)
            contador = (contador == 0) ? 3 : contador - 1; //decremento circular
            atualizaLEDs();
        }
        P1IFG &= ~S2; //zera flag manualmente
    }
}
