/*--------------------------------------------------------------------------------------
 EEPROM.C - Header file to support the USART functions.
 Copyright (C) 2020 Jagannatha Rao (aka JagiChan) (jagannath_raous@yahoo.com)

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------*/

#include <xc.h>

#define Nop()   asm("nop")

// reads a byte from the EEPROM at the given address
unsigned char EEread(unsigned char addr)
{
    EEADR = addr;
    EECON1bits.RD = 1;
    Nop(); //wait for completion of command
    Nop(); //maybe useful if the clock frequency is high
    Nop();
    Nop();
    return(EEDATA); //return the byte
}

// writes a byte to the EEPROM at the given address
void EEwrite(unsigned char addr, unsigned char data)
{
    static bit GIE_BIT_VAL;

    EEADR = addr;
    EEDATA = data;
    EECON1bits.WREN = 1;
    GIE_BIT_VAL = INTCONbits.GIE;
    INTCONbits.GIE = 0;
    EECON2 = 0x55; //critical unlock sequence
    EECON2 = 0xAA;
    EECON1bits.WR = 1; //end critical sequence
    while (EECON1bits.WR); //wait for write finish
    INTCONbits.GIE = GIE_BIT_VAL;
    EECON1bits.WREN = 0;
}
