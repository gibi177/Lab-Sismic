#include <msp430f5529.h>

// Definições
#define MOTOR BIT0 // P2.0 -> TA1.1
#define S1 BIT1    // P2.1
#define S2 BIT1    // P1.1

#define PWM_PERIOD 20000 // período de 20ms, para SMCLK de 1MHz
#define MIN_PULSE 500    // 0.5 ms (posição 0°)
#define MAX_PULSE 2500   // 2.5 ms (posição 180°)

// Variáveis globais
volatile unsigned int current_pulse = MIN_PULSE; // Começa em 0°
volatile int direction = 1;                      // 1 para positivo, -1 para negativo
volatile unsigned int S2_pressed = 0;            // flag para botão S2 pressionado

// Constantes de tempo para diferentes velocidades
#define POSITIVE_SPEED_DELAY 100000 // 100ms para 10°/s (10 passos por segundo)
#define NEGATIVE_SPEED_DELAY 33333  // ~33.3ms para 30°/s (30 passos por segundo)

// Debounce
void debounce(void) {
    volatile unsigned int i = 5000;
    while (i--);
}

// Configura PWM no pino P2.0 usando timer A1.1
void configPWM() {
    P2DIR |= MOTOR; // P2.0 como saída 
    P2SEL |= MOTOR; // P2.0 função alternativa (TA1.1)

    TA1CTL = TASSEL__SMCLK | MC__UP | TACLR; // Timer A1 SMCLK, modo up, zera contador
    TA1CCR0 = PWM_PERIOD - 1;   // Período de 20ms
    TA1CCTL1 = OUTMOD_7;        // Modo Reset/Set
    TA1CCR1 = MIN_PULSE;        // Largura inicial do pulso (posição 0°)
}

// Configura os botões S1 (P2.1) e S2 (P1.1 com interrupção)
void configBtns() {
    // Configura S1 (P2.1)
    P2DIR &= ~S1;       
    P2REN |= S1;        
    P2OUT |= S1;        

    // Configura S2 (P1.1) com interrupção
    P1DIR &= ~S2;       
    P1REN |= S2;        
    P1OUT |= S2;        
    P1IES |= S2;        // Interrupção na borda de descida
    P1IE  |= S2;        // Habilita interrupção
    P1IFG &= ~S2;       // Limpa flag de interrupção
}

// Atualiza o pulso do PWM com novo valor (posição do servo)
void updatePWM(unsigned int pulse) {
    TA1CCR1 = pulse;
}

// Interrupção do botão S2 (P1.1)
#pragma vector = PORT1_VECTOR
__interrupt void S2_ISR() {
    if (P1IFG & S2) {   // Verifica se foi o S2 que gerou a interrupção
        debounce();       
        if (!(P1IN & S2)) { // Verifica se o botão ainda está pressionado
            direction *= -1; // Inverte a direção
        }
        P1IFG &= ~S2; // Limpa a flag de interrupção
    }
}

// Função principal
void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Desativa o watchdog timer
    
    configPWM();           // Configura PWM
    configBtns();          // Configura botões
    updatePWM(MIN_PULSE);  // Posição inicial do servo: 0°

    __enable_interrupt();  // Habilita interrupções globais

    // Calcula o incremento/decremento de pulso por grau
    unsigned int pulse_per_degree = (MAX_PULSE - MIN_PULSE) / 180;
    
    while (1) {
        if (!(P2IN & S1)) { // S1 pressionado
            // Atualiza a posição conforme a direção
            if (direction == 1) {
                if (current_pulse < MAX_PULSE) {
                    current_pulse += pulse_per_degree;
                }
            } else {
                if (current_pulse > MIN_PULSE) {
                    current_pulse -= pulse_per_degree;
                }
            }
            
            // Verifica limites
            if (current_pulse > MAX_PULSE) {
                current_pulse = MAX_PULSE;
            } else if (current_pulse < MIN_PULSE) {
                current_pulse = MIN_PULSE;
            }
            
            updatePWM(current_pulse); // Atualiza posição do servo
            
            // Delay conforme a direção
            if (direction == 1) {
                __delay_cycles(POSITIVE_SPEED_DELAY); // 10°/s
            } else {
                __delay_cycles(NEGATIVE_SPEED_DELAY); // 30°/s
            }
        }
    }
}