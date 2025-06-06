#include <msp430.h>
#include <stdint.h>
 
#define LED_RED    BIT0   // P1.0 (LED vermelho)
 
void i2c_init() {
    // 1. Habilita o reset da interface USCI_B0
    UCB0CTL1 |= UCSWRST;
 
    // 2. Configura como mestre I²C, modo síncrono (UCMODE=3)
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
    UCB0CTL1 = UCSSEL_2;  // Clock SMCLK (1MHz)
 
    // 3. Configura os pinos P3.0 (SDA) e P3.1 (SCL)
    P3SEL |= BIT0 | BIT1;  // Habilita função dedicada (I²C)
    P3REN |= BIT0 | BIT1;  // Habilita resistores de pull-up
    P3OUT |= BIT0 | BIT1;  // SDA e SCL em HIGH
 
    // 4. Configura baud rate para ~100kHz (SMCLK / 10)
    UCB0BRW = 10;
 
    // 5. Sai do reset
    UCB0CTL1 &= ~UCSWRST;
}
 
uint8_t i2cSend(uint8_t addr, uint8_t data) {
    // 1. Define o endereço do escravo
    UCB0I2CSA = addr;
 
    // 2. Configura como transmissor e envia START
    UCB0CTL1 |= UCTR | UCTXSTT;
 
    // 3. Aguarda TXBUF estar vazio (pronto para enviar)
    while (!(UCB0IFG & UCTXIFG));
 
    // 4. Envia o byte de dados
    UCB0TXBUF = data;
 
    // 5. Aguarda START ser concluído
    while (UCB0CTL1 & UCTXSTT);
 
    // 6. Verifica se houve NACK
    if (UCB0IFG & UCNACKIFG) {
        UCB0CTL1 |= UCTXSTP;  // Envia STOP em caso de NACK
        while (UCB0CTL1 & UCTXSTP);
        return 1;  // Retorna 1 (NACK)
    }
 
    // 7. Aguarda o dado ser transmitido
    while (!(UCB0IFG & UCTXIFG));
 
    // 8. Envia STOP
    UCB0CTL1 |= UCTXSTP;
    while (UCB0CTL1 & UCTXSTP);
 
    return 0;  // Retorna 0 (ACK)
}
 
void main() {
    WDTCTL = WDTPW | WDTHOLD;  // Desliga o Watchdog Timer
    P1DIR |= LED_RED;          // Configura P1.0 como saída (LED vermelho)
    P1OUT &= ~LED_RED;         // Inicia com LED apagado
 
    i2c_init();
    uint8_t lcd_addr = 0x27;   // Tenta primeiro o endereço 0x27 (PCF8574T)
 
    while (1) {
        i2cSend(lcd_addr, 0x08);  // Backlight ON
        __delay_cycles(500000);    // 0.5s (SMCLK @1MHz)
        i2cSend(lcd_addr, 0x00);  // Backlight OFF
        __delay_cycles(500000);    // 0.5s (total = 1Hz)
        P1OUT ^= LED_RED;         // Alterna o LED a cada ciclo
    }
}