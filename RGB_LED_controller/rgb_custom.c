/*
 * File:   rgb_custom.c
 * Author: lucas
 *
 * Created on 28 de Maio de 2022, 16:17
 */


#include <xc.h>

#define _XTAL_FREQ 4000000 // 4 MHz

#define RED PORTBbits.RB5
#define GREEN PORTBbits.RB6
#define BLUE PORTBbits.RB7

#define INC 5

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RA5/MCLR/VPP Pin Function Select bit (RA5/MCLR/VPP pin function is MCLR)
#pragma config BOREN = OFF      // Brown-out Detect Enable bit (BOD disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection bit (Data memory code protection off)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

void verifica_ativos(unsigned short matriz[12][3], unsigned short ponteiro);

void main(void)
{
    TRISB = 0x00;
    PORTB = 0x00;
    
    unsigned short matriz_cores[12][3] = {
        {100, 0, 0},
        {0, 100, 0},
        {0, 0, 100},
        {60, 0, 70},
        {50, 30, 60},
        {0, 100, 100},
        {60, 50, 0},
        {0, 50, 80},
        {30, 30, 60},
        {80, 10, 10},
        {40, 80, 50},
        {100, 100, 0}
    };
    
    unsigned short ponteiro_cor = 0;
    unsigned short loop_cor = 0;
    unsigned short loop_atual = 0;
    
    unsigned short inicial[3] = {
        matriz_cores[0][0],
        matriz_cores[0][1],
        matriz_cores[0][2]
    };
    
    while(1)
    {
        __delay_ms(INC);
        loop_cor += INC;
        loop_atual += INC;
        
        verifica_ativos(matriz_cores, ponteiro_cor);
        
        if (loop_atual == 100)
        {
            loop_atual = 0;
            
            matriz_cores[ponteiro_cor][0] = inicial[0];
            matriz_cores[ponteiro_cor][1] = inicial[1];
            matriz_cores[ponteiro_cor][2] = inicial[2];
        }
        if (loop_cor == 500)
        {
            if (ponteiro_cor < 11)
                ponteiro_cor++;
            else
                ponteiro_cor = 0;     
            
            loop_cor = 0;
            inicial[0] = matriz_cores[ponteiro_cor][0];
            inicial[1] = matriz_cores[ponteiro_cor][1];
            inicial[2] = matriz_cores[ponteiro_cor][2];
        }   
    }
    
    return;
}

void verifica_ativos(unsigned short matriz[12][3], unsigned short ponteiro) {
    if (matriz[ponteiro][0] > 0) {
        RED = 1;
        matriz[ponteiro][0] -= INC;
    }
    else {
        RED = 0;
    }

    if (matriz[ponteiro][1] > 0) {
        GREEN = 1;
        matriz[ponteiro][1] -= INC;
    } else {
        GREEN = 0;
    }

    if (matriz[ponteiro][2] > 0) {
        BLUE = 1;
        matriz[ponteiro][2] -= INC;
    } else {
        BLUE = 0;
    }
}
