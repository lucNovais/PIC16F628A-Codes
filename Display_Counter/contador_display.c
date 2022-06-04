/*
 * File:   Contador_Display.c
 * Author: Marcelo
 *
 * Created on 22 de Março de 2021, 19:32
 */

// PIC16F628A Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "display7s.h"
#define _XTAL_FREQ 4000000 // 4MHz

void main(void) {
    unsigned char tmp;
    unsigned char msb;
    unsigned short int count = 0;
    TRISB = 0x00; // PORTB as output

    while (1) {
        if (tmp < 0x0A) {
            while (count < 60) {
                PORTB = display7s(tmp & 0x0F) | 0x10;
                // Delay: solucao ruim para esse tipo de problema
                // Proposta: usar um timer para ajustar o display
                __delay_ms(7);
                PORTB = display7s(msb);
                //PORTB = display7s((tmp & 0xF0) >> 4);
                __delay_ms(8);
                count++;
            }
            count = 0;
            tmp++;
        } else {
            if (msb < 0x05)
                msb++;
            else
                msb = 0;
            tmp = 0;
        }
    }
}
