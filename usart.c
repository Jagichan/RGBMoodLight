/*--------------------------------------------------------------------------------------
 USART.C - The file to that contains the USART implementation.
 Copyright (C) 2020 Jagannatha Rao (aka JagiChan) (jagannath_raous@yahoo.com)

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------*/

/******************************************************************************

 Serial communication library for PIC 16F/18F series MCUs.

 *******************************************************************************/
#include "rgbmain.h"		//remove this header file if you plan to use XC.h directly
#include "usart.h"

//baudrate calculation macro (done at compile time, _XTAL_FREQ is defined in header file)
#define SetBaudRate(baud_rate)   (SPBRG = (((_XTAL_FREQ/baud_rate)/16)-1))

//Initialize the hardware USART
void USARTInit(unsigned int baud_rate)
{
    SetBaudRate(baud_rate);

    //TXSTA
    CSRC = 0; //clock source don't care, async mode
    TX9 = 0; //8 bit transmission
    TXEN = 1; //Transmit enable
    SYNC = 0; //Async mode
    BRGH = 1; //High speed baud rate

    //RCSTA
    SPEN = 1; //Serial port enabled
    RX9 = 0; //8 bit mode
    SREN = 0;
    CREN = 1; //Enable receive
    ADDEN = 0; //Disable address detection

    //Receive interrupt enable
    RCIE = 1;
    RCIF = 0;
    PEIE = 1;
}

// write a constant char to serial port
void USARTWriteConstChar(const unsigned char ch)
{
    while (!PIR1bits.TXIF);

    TXREG = ch;
}

// write a constant string to serial port
void USARTWriteConstString(const unsigned char *str)
{
    while (*str != '\0')
    {
        USARTWriteConstChar(*str);
        str++;
    }
}

// write a char to serial port
void USARTWriteChar(unsigned char ch)
{
    while (!PIR1bits.TXIF);

    TXREG = ch;
}

// write a string to serial port
void USARTWriteString(unsigned char *str)
{
    while (*str != '\0')
    {
        USARTWriteChar(*str);
        str++;
    }
}

// write a constant string to a new line 
void USARTWriteConstLine(const unsigned char *str)
{
    USARTGotoNewLine();		//go to new line
	USARTWriteConstString(str);	//write the string
}

// write a string to a new line
void USARTWriteLine(unsigned char *str)
{
    USARTGotoNewLine();
	USARTWriteString(str);
}

void USARTGotoNewLine(void)
{
    USARTWriteChar('\r'); //CR
    USARTWriteChar('\n'); //LF
}

// write a character to the USART buffer. Returns BUFFER_FULL if buffer is full or BUFFER_OK if character could be written
enum BufferStatus bufferWrite (unsigned char byte)
{
    unsigned char next_index = (buffer.write_index + 1) % BUFFER_SIZE;

    if (next_index == buffer.read_index)
    {
        return BUFFER_FULL;
    }
    buffer.data[buffer.write_index] = byte;
    buffer.write_index = next_index;
    return BUFFER_OK;
}

// reads a character from the USART buffer. Returns BUFFER_EMPTY if buffer has no characters or BUFFER_OK if character could be read
enum BufferStatus bufferRead (unsigned char *byte)
{
    if (buffer.write_index == buffer.read_index)
    {
        return BUFFER_EMPTY;
    }
    *byte = buffer.data[buffer.read_index];
    if (buffer.read_index++ > BUFFER_SIZE)
        buffer.read_index = 0;

    return BUFFER_OK;
}
