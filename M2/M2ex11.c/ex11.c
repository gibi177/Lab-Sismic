#include <msp430f5529.h>

#define MOTOR BIT0     // P2.0 -> TA1.1
#define S1 BIT1        // P2.1
#define S2 BIT1        // P1.1

#define STEP 100       // 0.1ms = 100 ciclos (com SMCLK = 1 MHz)
#define MIN_PULSE 500  // 0.5 ms
#define MAX_PULSE 2500 // 2.5 ms

void debounce(void) {
    volatile unsigned int i = 5000;
    while(i--);
}

void main(void){
    WDTCTL = WDTPW | WDTHOLD;   

    // Usar pino P2.0 para o PWM (Canal 1 do Timer A1)
    P2DIR |= MOTOR;             // P2.0 como saída
    P2SEL |= MOTOR;             // Selecionar a função alternativa do pino (TA1.1)

    // Configurar pinos das chaves
    P2DIR &= ~S1;
    P2REN |= S1;
    P2OUT |= S1;

    P1DIR &= ~S2;
    P1REN |= S2;
    P1OUT |= S2;

    // Configurar Timer_A1
    TA1CTL = TASSEL__SMCLK | MC__UP | TACLR;  // SMCLK (1 MHz), modo UP
    TA1CCR0 = 20000 - 1;       // Período de 20ms (50Hz)
    TA1CCTL1 = OUTMOD_7;       // Reset/Set para PWM
    TA1CCR1 = 1500;            // Pulso inicial de 1.5 ms (posição central)

    unsigned int pulse_width = 1500;  // Armazena a largura do pulso atual

    // Loop infinito
    while (1) {
        if (!(P2IN & S1)) { // S1 pressionado (aumentar ângulo)
            debounce();
            if (!(P2IN & S1)) {
                if (pulse_width < MAX_PULSE)
                    pulse_width += STEP;
                TA1CCR1 = pulse_width;
                while (!(P2IN & S1)); // Aguarda soltar
                debounce();
            }
        }

        if (!(P1IN & S2)) { // S2 pressionado (diminuir ângulo)
            debounce();
            if (!(P1IN & S2)) {
                if (pulse_width > MIN_PULSE)
                    pulse_width -= STEP;
                TA1CCR1 = pulse_width;
                while (!(P1IN & S2)); // Aguarda soltar
                debounce();
            }
        }
    }
}
