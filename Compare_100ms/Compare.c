/*
 * File:   Compare.c
 * Author: Marcelo
 *
 * Created on 22 de Março de 2021, 11:12
 */

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 20000000 // 4MHz
#define LED1 PORTCbits.RC1

char flag_aux = 0;
char select = 0; // select = 0 - Option A; select = 1 - Option B

void __interrupt() CCP_ISR() {

    // Option A: Freq_Compare = (FOSC/4)/PREESCALER/CCPR1. Compare Time = 1/Freq_Compare
    // Option B: Freq_Compare = (FOSC/4)/PREESCALER/0xFFFF. Compare Time = 1/Freq_Compare
    if (CCP2IF) {
        flag_aux = 1;
        CCP2IF = 0;
    }
}

void main() {

    char count = 0;
    // TRISBbits.TRISB1 = 0; // RB1 as output
    TRISCbits.TRISC1 = 0; // RC2 as output

    if (select == 0) {
        // Option A
        //Compare mode, trigger special event (CCPxIF bit is set, CCPx pin is unaffected);
        // CCP1 resets TMR1;
        CCP2CON = 0x0B; // 0000 1011
        // Trigger special event (CCPIF bit is set) - For Option A only
        CCPR2H = 0xF4; // counter value assigned to CCPR1 Register
        CCPR2L = 0x24; // Since CCPR1 = 0x007D = 125
    } else if (select == 1) {
        // Option B
        // Compare mode, set output on match (CCPxIF bit is set)
        // CCP1 will not reset TMR1;
        CCP1CON = 0x08;
    }

    TMR1 = 0;
    T1CON = 0x30; //Prescaler 1:8

    GIE = 1; // Enable The Global Interrupt
    PEIE = 1;
    CCP2IF = 0;
    CCP2IE = 1;
    T1CONbits.TMR1ON = 1;   // Start Timer 1

    while (1) {
        if (flag_aux) {
            flag_aux = 0;

            if (select == 0) {
                // For Option A
                LED1 = ~LED1; //Toggle RB1 (LED)
            } // else if (select == 1) {
                // For Option B
                
//                if (count > 0) {
//                    OUT_RC2 = 0;
//                    count = 0;
//                } else {
//                    count++;
//                }
            //}
        }
    }

}

