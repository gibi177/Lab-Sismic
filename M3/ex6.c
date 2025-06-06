#include <msp430.h>
#include <stdint.h>
 
#define LCD_ADDR  0x27  // Altere para 0x3F se necessário
 
#define I2C_DELAY     __delay_cycles(100)
#define CMD_DELAY     __delay_cycles(40000)
#define LONG_DELAY    __delay_cycles(160000)
 
// Bits de controle
#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RW        0x02
#define LCD_RS        0x01
 
// ----------- I2C + LCD CORE -----------
void i2c_init() {
    UCB0CTL1 |= UCSWRST;                         // Reset da interface
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;        // Mestre, I2C síncrono
    UCB0CTL1 = UCSSEL_2 | UCSWRST;               // SMCLK, ainda em reset
    UCB0BRW = 10;                                // Aproximadamente 100kHz
    P3SEL |= BIT0 | BIT1;                        // Função alternativa (SDA/SCL)
    P3REN |= BIT0 | BIT1;
    P3OUT |= BIT0 | BIT1;
    UCB0CTL1 &= ~UCSWRST;                        // Libera a interface
}
 
uint8_t i2cSend(uint8_t addr, uint8_t data) {
    UCB0I2CSA = addr;
    UCB0CTL1 |= UCTR | UCTXSTT;
    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = data;
    while (UCB0CTL1 & UCTXSTT);
    if (UCB0IFG & UCNACKIFG) {
        UCB0CTL1 |= UCTXSTP;
        while (UCB0CTL1 & UCTXSTP);
        return 1;
    }
    while (!(UCB0IFG & UCTXIFG));
    UCB0CTL1 |= UCTXSTP;
    while (UCB0CTL1 & UCTXSTP);
    return 0;
}
 
// ----------- LCD LOW LEVEL -----------
 
void lcdPulseEnable(uint8_t data) {
    i2cSend(LCD_ADDR, data | LCD_ENABLE | LCD_BACKLIGHT);
    I2C_DELAY;
    i2cSend(LCD_ADDR, (data & ~LCD_ENABLE) | LCD_BACKLIGHT);
    I2C_DELAY;
}
 
void lcdSendNibble(uint8_t nibble, uint8_t control) {
    uint8_t data = (nibble << 4) | control | LCD_BACKLIGHT;
    i2cSend(LCD_ADDR, data);
    lcdPulseEnable(data);
}
 
void lcdSendByte(uint8_t byte, uint8_t control) {
    lcdSendNibble(byte >> 4, control); // MSNibble
    lcdSendNibble(byte & 0x0F, control); // LSNibble
}
 
// ----------- LCD HIGH LEVEL -----------
 
void lcdCommand(uint8_t cmd) {
    lcdSendByte(cmd, 0x00); // RS = 0
    if (cmd < 4) LONG_DELAY;  // Clear / Home
    else CMD_DELAY;
}
 
void lcdData(uint8_t data) {
    lcdSendByte(data, LCD_RS); // RS = 1
    CMD_DELAY;
}
 
void lcdInit() {
    __delay_cycles(50000); // Espera inicial > 40ms
 
    // Força modo 8 bits com 3 nibbles 0x3
    lcdSendNibble(0x03, 0x00); __delay_cycles(5000);
    lcdSendNibble(0x03, 0x00); __delay_cycles(5000);
    lcdSendNibble(0x03, 0x00); __delay_cycles(5000);
 
    // Modo 4 bits
    lcdSendNibble(0x02, 0x00); __delay_cycles(5000);
 
    // Inicialização
    lcdCommand(0x28); // 4 bits, 2 linhas, fonte 5x8
    lcdCommand(0x08); // Display off
    lcdCommand(0x01); // Clear display
    LONG_DELAY;
    lcdCommand(0x0C); // Display on, cursor off
    lcdCommand(0x06); // Incremento automático
}
 
// ----------- Função Principal -----------
 
void lcdWrite(char *str) {
    uint8_t count = 0;
    lcdCommand(0x80); // Início da linha 1
 
    while (*str) {
        if (*str == '\n') {
            lcdCommand(0x80 + 0x40); // Vai para a linha 2
            count = 16; // já na segunda linha
            str++;
            continue;
        }
 
        if (count == 16) {
            lcdCommand(0x80 + 0x40); // Vai para linha 2 se não foi por \n
        } else if (count == 32) {
            break; // LCD cheio
        }
 
        lcdData(*str++);
        count++;
    }
}
 
void main() {
    WDTCTL = WDTPW | WDTHOLD;
 
    i2c_init();
    lcdInit();
 
    lcdWrite("Yo: gurt\nts so kevin");
    while (1);
}