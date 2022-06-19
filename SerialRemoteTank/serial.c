/* ########################################################################
   PICsim - PIC simulator http://sourceforge.net/projects/picsim/
   ########################################################################
   Copyright (c) : 2015  Luis Claudio Gambôa Lopes
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include <xc.h>
#include"serial.h"
#define _XTAL_FREQ 4000000 // 4MHz

void serial_init(void) {

    // BRGH = 1 (High Speed)
    // Baud Rate = FOSC/(16 (SPBRG + 1))
    // SPBRG: Baud Rate Generator Register
    SPBRG = 12; // Baud Rate = 19200, Error (%) = 0.16
    
    TXSTAbits.TX9 = 0; // 0 = Selects 8-bit transmission
    TXSTAbits.TXEN = 1; // 1 = Transmit enabled
    TXSTAbits.SYNC = 0; // 0 = Asynchronous mode
    TXSTAbits.BRGH = 1; // 1 = High speed

    RCSTAbits.SPEN = 1; // 1 = Serial port enabled (configures RC7/RX/DT and RC6/TX/CK pins as serial port pins)
    RCSTAbits.RX9 = 0; // 0 = Selects 8-bit reception
    RCSTAbits.CREN = 1; // 1 = Enables continuous receive

    

}

void serial_tx(unsigned char val) {
    TXREG = val;
    while (!TXSTAbits.TRMT);
}

void serial_tx_str(const char* val) {
    unsigned char i = 0;

    while (val[i]) {
        serial_tx(val[i]);
        i++;
    }
}

unsigned char serial_rx(unsigned int timeout) {
    unsigned int to = 0;

    if (RCSTAbits.FERR || RCSTAbits.OERR)   // FERR: Framing Error bit, OERR: Overrun Error bit
    {
        RCSTAbits.CREN = 0;     // 0 = Disables continuous receive
        RCSTAbits.CREN = 1;     // 1 = Enables continuous receive
    }

    while (((to < timeout) || (!timeout))&&(!PIR1bits.RCIF)) {
        __delay_ms(20);
        to += 20;
    }
    if (PIR1bits.RCIF)
        return RCREG;
    else
        return 0xA5;
}