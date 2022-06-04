/*
 * File:   PWM.c
 * Author: Marcelo
 *
 * Created on 22 de Março de 2021, 11:12
 */

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 20000000 // 4MHz
#define incremento 62

void main() {
    unsigned short dutyCycle = 0;
    unsigned short reverseTrigger = 0;
    
    TRISCbits.TRISC2 = 0; // Configure PORTC as output(RC2-PWM1, RC1-PWM2)

    CCP1CON = 0x0F; // Select the PWM mode.
    PR2 = 0x9B; // PR2 = 249
    CCPR1L = 0; // By default set the dutyCycle to 0
    T2CON = 0x03; // prescaler 1:4

    // PWM frequency = (FOSC/4)/PRESCALER/(PR2+1).
    // PWM period = 1/PWM frequency

    TMR2ON = 1; //Start the Timer for PWM generation

    while (1) {
        __delay_ms(500);
        
        if ((dutyCycle < 624 - incremento) && reverseTrigger == 0) {
            dutyCycle += incremento;
        }
        else if (dutyCycle > incremento){            
            dutyCycle -= incremento;
            reverseTrigger = 1;
        }
        else {
            reverseTrigger = 0;
            dutyCycle = 0;
        }
            
        CCPR1L = (dutyCycle & 0x3FC) >> 2; // Mask1 - CCPR1L: First 8 MSBs
        CCP1X = ((dutyCycle & 0x002) >> 1); // Mask2 - CCP1CON<5>
        CCP1Y = (dutyCycle & 0x001); // Mask3 - CCP1CON<4>
    }
}