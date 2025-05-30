#include <msp430f5529.h>

#define MOTOR BIT0     
#define S1 BIT1        
#define S2 BIT1        

#define PWM_PERIOD 20000 // Período de 20ms
#define MIN_PULSE 500    // 0.5ms (0°)
#define MAX_PULSE 2500   // 2.5ms (180°)
#define SEQUENCE_TIMEOUT 300 // 300 decrementos de 10ms = 3000ms

// Estados da sequência
typedef enum {
    ESPERA,
    S1_PRESSIONADO,
    S1_SOLTO,
    S2_PRESSIONADO,
    S2_SOLTO,
    S1_FINAL_PRESSIONADO,
    SEQUENCIA_COMPLETA
} EstadoSequencia;

// Variáveis globais
volatile unsigned int pulso_atual = MIN_PULSE;
volatile EstadoSequencia estado = ESPERA;
volatile unsigned int temporizador = 0;

void configurarPWM() {
    P2DIR |= MOTOR;
    P2SEL |= MOTOR;
    TA1CCR0 = PWM_PERIOD - 1;
    TA1CCTL1 = OUTMOD_7;
    TA1CCR1 = MIN_PULSE;
    TA1CTL = TASSEL_2 + MC_1;
}

void configurarBotoes() {
    P2DIR &= ~S1;
    P2REN |= S1;
    P2OUT |= S1;
    P2IES |= S1;
    P2IE |= S1;
    P2IFG &= ~S1;
    
    P1DIR &= ~S2;
    P1REN |= S2;
    P1OUT |= S2;
    P1IES |= S2;
    P1IE |= S2;
    P1IFG &= ~S2;
}

void atualizarPWM(unsigned int pulso) {
    TA1CCR1 = pulso;
}

void debounce() {
    __delay_cycles(50000); //50ms
}

#pragma vector=PORT2_VECTOR
__interrupt void InterrupcaoS1() {
    if (P2IFG & S1) {
        debounce();
        
        if (P2IES & S1) { // Borda de descida (pressionado)
            if (!(P2IN & S1)) {
                switch(estado) {
                    case ESPERA:
                        estado = S1_PRESSIONADO;
                        temporizador = SEQUENCE_TIMEOUT;
                        break;
                    case S2_SOLTO:
                        estado = S1_FINAL_PRESSIONADO;
                        break;
                    default:
                        estado = ESPERA;
                        temporizador = 0;
                }
            }
        } else { // Borda de subida (soltar)
            if ((P2IN & S1)) {
                switch(estado) {
                    case S1_PRESSIONADO:
                        estado = S1_SOLTO;
                        break;
                    case S1_FINAL_PRESSIONADO:
                        estado = SEQUENCIA_COMPLETA;
                        break;
                    default:
                        estado = ESPERA;
                        temporizador = 0;
                }
            }
        }
        P2IES ^= S1;
        P2IFG &= ~S1;
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void InterrupcaoS2() {
    if (P1IFG & S2) {
        debounce();
        
        if (P1IES & S2) { // Pressionado
            if (!(P1IN & S2)) {
                if (estado == S1_SOLTO) {
                    estado = S2_PRESSIONADO;
                } else {
                    estado = ESPERA;
                    temporizador = 0; 
                }
            }
        } else { // Solto
            if ((P1IN & S2)) {
                if (estado == S2_PRESSIONADO) {
                    estado = S2_SOLTO;
                } else {
                    estado = ESPERA;
                    temporizador = 0; 
                }
            }
        }
        P1IES ^= S2;
        P1IFG &= ~S2;
    }
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;
    configurarPWM();
    configurarBotoes();
    __enable_interrupt();
    atualizarPWM(MIN_PULSE);

    while(1) {
        if (estado == SEQUENCIA_COMPLETA) {
            // Move o servo para posição oposta
            pulso_atual = (pulso_atual == MIN_PULSE) ? MAX_PULSE : MIN_PULSE;
            atualizarPWM(pulso_atual);
            estado = ESPERA;
            temporizador = 0;
            __delay_cycles(1000000); // Espera movimento completar
        }
        
        // Verifica timeout com reset
        if (estado != ESPERA && estado != SEQUENCIA_COMPLETA) {
            if (temporizador > 0) {
                temporizador--;
            } else {
                // Reset da sequência
                estado = ESPERA;
                temporizador = 0;
            }
        }
        
        __delay_cycles(10000); // Base de tempo de 10ms
    }
}