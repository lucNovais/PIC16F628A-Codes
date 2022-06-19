/*
 * File:   Serial_Tank_16F877A.c
 * Author: Marcelo
 *
 * Created on 15 de Dezembro de 2021, 10:47
 * 
 * Link do Exemplo
 * https://github.com/lcgamboa/picsimlab_examples/tree/0964969d9aa60b44a67159b5f7d968d5b7450e50/docs/board_PICGenios/PIC18F4550/srtank_test/src/srtank_test.X
 * 
 * Para iniciar, abrir o Serial Remote Tank e Conectar usando a porta virtual complementar
 * Ex.: Se PICSimLab conectado em COM8, Conectar Serial Remore Tank na COM9
 * 
 */

/*
 Trabalho Prático - CEA580
 */

// CONFIG
#pragma config FOSC = XT        // Oscillator Selection bits (XT oscillator)
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
#define _XTAL_FREQ 4000000 // 4MHz

#include "lcd.h"
#include "serial.h"
#include "itoa.h"
//#include "teclado.h"

//entradas digitais
#define _VENT 0x00
#define _VSAI 0x01
#define _AQUE 0x02
#define _RESF 0x03
#define _AGIT 0x04
//saídas digitais
#define _BALT 0x10
#define _BBAI 0x11
#define _TMIN 0x12
#define _TMAX 0x13
//entradas analógicas
#define _ATMI 0x20
#define _ATMA 0x21
//saídas analógicas
#define _VOLU 0x30
#define _TEMP 0x31

#define _LIGA 0x01
#define _DESLIGA 0x00

#define _DEFAULT_TEMP 0
#define _DEFAULT_VOLMH 1
#define _DEFAULT_VOLML 2
#define _DEFAULT_AGIT 3

#define RB0 PORTBbits.RB0
#define RB1 PORTBbits.RB1
#define RB2 PORTBbits.RB2

#define _TEMP_MAX 0x3C
#define _TEMP_MIN 0x05
//#define _VOLM_MAX 0x


unsigned int valori;
unsigned char valorh, valorl;
unsigned char valor;
char buff[6];
unsigned int capacidade = 0x0BB8;
int setEnable = 0;
unsigned int setVolm;
unsigned int setTemp;
unsigned int setAgit;

unsigned int get_volume();
unsigned int get_temperatura();
void telaUm();
void telaUmTexto();
void EtapaCinco();
void automatico();
void manual();
void criasetpoint(int opcao);
void configura();

unsigned char enableRB0;
unsigned char enableRB1;
unsigned char enableRB2;
unsigned char enableRB3;
unsigned char mostrandoTexto;

unsigned char ret;

void __interrupt() RB0_RET() {
    if (INTF && enableRB0)
        ret = 1;
    INTF = 0;
}

void main(void) {
    TRISCbits.TRISC7 = 1; //RX
    TRISCbits.TRISC6 = 0; //TX
    serial_init();

    enableRB0 = 0;
    ret = 0;
    mostrandoTexto = 0;

    INTEDG = 1;
    GIE = 1;

    serial_tx(_VSAI);
    serial_tx(_LIGA);

    eeprom_write(_DEFAULT_TEMP, 0x1E);
    eeprom_write(_DEFAULT_VOLMH, 0x05);
    eeprom_write(_DEFAULT_VOLML, 0xDC);
    eeprom_write(_DEFAULT_AGIT, 0x01);


    // LCD
    TRISD = 0x00; //Utiliza todos os bits do port D para o LCD
    TRISEbits.TRISE0 = 0; //bit setado como saída RE1/~WR/AN6 que vai no RS
    TRISEbits.TRISE1 = 0; //bit setado como saida RE0/~RD/AN5 que vai no E
    lcd_init();

    // Alarms
    serial_tx(_ATMA); //Seta a temperatura máxima (alarme)
    serial_tx((_TEMP_MAX & 0xFF00) >> 8);
    serial_tx(_TEMP_MAX & 0x00FF);

    serial_tx(_ATMI); //Seta a temperatura mínima(alarme)
    serial_tx((_TEMP_MIN & 0xFF00) >> 8);
    serial_tx(_TEMP_MIN & 0x00FF);

    telaUm();
}

void telaUmTexto() {
    lcd_cmd(L_CLR); //Limpa
    lcd_cmd(L_L1); //Vai pra linha 1
    lcd_str("Niv "); //Escreve na linha 1
    valori = get_volume();
    valori /= (capacidade / 100);
    itoa(valori, buff);
    lcd_str(buff);
    lcd_str("%");

    valori = get_temperatura();
    itoa(valori, buff);
    lcd_str(" Temp ");
    lcd_str(buff);
    lcd_str("C");

    lcd_cmd(L_L2);
    lcd_str("Pres R0,R1 ou R2");
}

void telaUm() {
    TRISB = 0x07;
    __delay_ms(100);
    enableRB0 = 1;
    enableRB1 = 1;
    enableRB2 = 1;
    telaUmTexto();
    while (1) {
        if (!mostrandoTexto) {
            mostrandoTexto = 1;
            telaUmTexto();
        }
        if (!RB0 && enableRB0) {
            enableRB0 = 0;
            enableRB1 = 0;
            enableRB2 = 0;
            automatico();
            // manual();
        } else if (RB0) {
            enableRB0 = 1;
            enableRB1 = 1;
            enableRB2 = 1;
        }
        if (!RB1 && enableRB1) {
            enableRB0 = 0;
            enableRB1 = 0;
            enableRB2 = 0;
            manual();
        } else if (RB1) {
            enableRB0 = 1;
            enableRB1 = 1;
            enableRB2 = 1;
        }
        if (!RB2 && enableRB2) {
            enableRB0 = 0;
            enableRB1 = 0;
            enableRB2 = 0;
            configura();
            mostrandoTexto = 0;
        } else if (RB2)
            enableRB1 = 1;
        enableRB2 = 1;
        enableRB3 = 1;
    }
}

void automatico() {
    enableRB0 = 1;
    mostrandoTexto = 0;

    if (setEnable == 0) {
        setVolm = (unsigned int) (eeprom_read(_DEFAULT_VOLMH) << 8 | eeprom_read(_DEFAULT_VOLML));
        setTemp = (unsigned int) eeprom_read(_DEFAULT_TEMP);
        setAgit = (unsigned int) eeprom_read(_DEFAULT_AGIT);
    }

    EtapaCinco();

    INTE = 0;
    INTF = 0;

    serial_tx(_AGIT);
    serial_tx(_DESLIGA);
    serial_tx(_AQUE);
    serial_tx(_DESLIGA);
    serial_tx(_RESF);
    serial_tx(_DESLIGA);
    serial_tx(_VENT);
    serial_tx(_DESLIGA);

    return;
}

void criasetpoint(int opcao) {

    unsigned int auxVolm = 0x00;
    unsigned int auxTemp = 0x05;
    unsigned int auxAgit = 0x00;

    int attTexto = 1;
    int progressao = 1;
    int wait = 1;

    while (wait) {
        if (attTexto == 1) {
            lcd_cmd(L_CLR); //Limpa
            lcd_cmd(L_L1); //Vai pra linha 1
            lcd_str("Niv "); //Escreve na linha 1
            itoa(auxVolm, buff);
            lcd_str(buff);
            lcd_str("%");
            lcd_cmd(L_L2);
            lcd_str("R0+,R1-,R2>,R3x");
            attTexto = 0;
        } else if (attTexto == 2) {
            lcd_cmd(L_CLR); //Limpa
            lcd_cmd(L_L1); //Vai pra linha 1
            lcd_str("Temp ");
            itoa(auxTemp, buff);
            lcd_str(buff);
            lcd_str("C");
            lcd_cmd(L_L2);
            lcd_str("R0+,R1-,R2>,R3x");
            attTexto = 0;
        } else if (attTexto == 3) {
            lcd_cmd(L_CLR); //Limpa
            lcd_cmd(L_L1); //Vai pra linha 1
            lcd_str("Agitador: ");
            auxAgit == 1 ? lcd_str("ON ") : lcd_str("OFF");
            lcd_cmd(L_L2);
            lcd_str("R0+,R1-,R2>,R3x");
            attTexto = 0;
        }
        if (progressao == 1) {
            if (!RB0) {
                __delay_ms(15);
                attTexto = 1;
                if (auxVolm < 0x5F)
                    auxVolm++;
            } else if (!RB1) {
                __delay_ms(15);
                attTexto = 1;
                if (auxVolm == 0)
                    auxVolm = 0;
                else auxVolm--;
            } else if (!RB2) {
                __delay_ms(200);
                progressao++;
                attTexto = 2;
            } else if (!RB3)
                return;
        } else if (progressao == 2) {
            if (!RB0) {
                __delay_ms(15);
                attTexto = 2;
                if (auxTemp < _TEMP_MAX) {
                    auxTemp++;
                }
            } else if (!RB1) {
                __delay_ms(15);
                attTexto = 2;
                if (auxTemp > _TEMP_MIN) {
                    auxTemp--;
                }
            } else if (!RB2) {
                __delay_ms(200);
                attTexto = 3;
                progressao++;
            } else if (!RB3)
                return;
        } else if (progressao == 3) {
            if (!RB0) {
                __delay_ms(15);
                attTexto = 3;
                auxAgit = 1;
            } else if (!RB1) {
                __delay_ms(15);
                attTexto = 3;
                auxAgit = 0;
            } else if (!RB2) {
                __delay_ms(200);
                progressao++;
            } else if (!RB3)
                return;
        } else if (progressao == 4) {
            wait = 0;
            attTexto = 5;
            if (opcao == 0) { //Caso seja a opcao manual
                setVolm = (unsigned int) (((float) auxVolm / (float) 100) * capacidade);
                setTemp = auxTemp;
                setAgit = auxAgit;
                setEnable = 1;
            } else if (opcao == 1) { //caso seja a opcao configure
                eeprom_write(_DEFAULT_TEMP, auxTemp);
                unsigned int auxVolm2;
                auxVolm2 = (unsigned int) (((float) auxVolm / (float) 100) * capacidade);
                auxVolm2 &= 0xFF00;
                auxVolm2 = auxVolm2 >> 8;
                eeprom_write(_DEFAULT_VOLMH, auxVolm2);
                auxVolm2 = (unsigned int) (((float) auxVolm / (float) 100) * capacidade);
                auxVolm2 &= 0x00FF;
                eeprom_write(_DEFAULT_VOLML, auxVolm2);
                eeprom_write(_DEFAULT_AGIT, auxAgit);
                setEnable = 0;
            }
            return;
        }
    }

}

void manual() {
    TRISB = 0x0F;
    criasetpoint(0);
    automatico();
    return;
}

void configura() {
    TRISB = 0x0F;
    criasetpoint(1);
    return;
}

void EtapaCinco() {
    unsigned char waiting = 1;

    lcd_cmd(L_CLR);
    lcd_cmd(L_L1);
    lcd_str("Iniciar?");
    lcd_cmd(L_L2);
    lcd_str("(R0)SIM, (R1)NAO");
    while (waiting) {
        if (!RB0) {
            waiting = 0;
            enableRB0 = 0;

            INTE = 1;
            INTF = 0;

            while (1) {
                lcd_cmd(L_CLR); //Limpa
                lcd_cmd(L_L1); //Vai pra linha 1
                lcd_str("Niv "); //Escreve na linha 1
                valori = get_volume();
                valori /= (capacidade / 100);
                itoa(valori, buff);
                lcd_str(buff);
                lcd_str("%");

                valori = get_temperatura();
                itoa(valori, buff);
                lcd_str(" Temp ");
                lcd_str(buff);
                lcd_str("C");
                lcd_cmd(L_L2);
                lcd_str("Pres R0 p. sair");

                if (ret) {
                    enableRB0 = 0;
                    ret = 0;
                    return;
                }
                if (RB0) {
                    enableRB0 = 1;
                }

                if ((((float) get_volume()) / ((float) capacidade)) > 0.2) {
                    if (setAgit == 0) {
                        serial_tx(_AGIT);
                        serial_tx(_DESLIGA);
                    } else if (setAgit == 1) {
                        serial_tx(_AGIT);
                        serial_tx(_LIGA);
                    }

                    if (get_temperatura() > setTemp) {
                        serial_tx(_AQUE);
                        serial_tx(_DESLIGA);
                        serial_tx(_RESF);
                        serial_tx(_LIGA);
                    } else if (get_temperatura() < setTemp) {
                        serial_tx(_AQUE);
                        serial_tx(_LIGA);
                        serial_tx(_RESF);
                        serial_tx(_DESLIGA);
                    } else if (get_temperatura() == setTemp) {
                        serial_tx(_AQUE);
                        serial_tx(_DESLIGA);
                        serial_tx(_RESF);
                        serial_tx(_DESLIGA);
                    }
                } else {
                    if (setAgit == 1) {
                        serial_tx(_AGIT);
                        serial_tx(_DESLIGA);
                    }
                    serial_tx(_AQUE);
                    serial_tx(_DESLIGA);
                    serial_tx(_RESF);
                    serial_tx(_DESLIGA);
                }

                if (get_volume() != setVolm) {
                    if (get_volume() > setVolm) {
                        serial_tx(_VENT);
                        serial_tx(_DESLIGA);
                    } else if (get_volume() < setVolm) {
                        serial_tx(_VENT);
                        serial_tx(_LIGA);
                    }
                }
            }
        }

        if (!RB1) {
            waiting = 0;
            return;
        }
    }
}

unsigned int get_volume() {
    unsigned int auxh;
    unsigned int auxl;
    unsigned int aux;
    serial_tx(_VOLU);
    auxh = serial_rx(0);
    auxl = serial_rx(0);
    aux = (unsigned int) (auxh << 8 | auxl);
    return aux;
}

unsigned int get_temperatura() {
    unsigned int auxh;
    unsigned int auxl;
    unsigned int aux;
    serial_tx(_TEMP);
    auxh = serial_rx(0);
    auxl = serial_rx(0);
    aux = (unsigned int) (auxh << 8 | auxl);
    return aux;
}

