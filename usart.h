/*--------------------------------------------------------------------------------------
 USART.H - Header file to support the USART functions.
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

#ifndef USART_PIC16_18_H
#define	USART_PIC16_18_H

#define _XTAL_FREQ (4000000UL)

//Constants
#define BUFFER_SIZE  20

enum BufferStatus {
    BUFFER_OK, BUFFER_EMPTY, BUFFER_FULL
};

struct Buffer {
    unsigned char data[BUFFER_SIZE];
    unsigned char write_index;
    unsigned char read_index;
};

struct Buffer buffer = {
    {0}, 0, 0};

void USARTInit(unsigned int baud_rate);
void USARTWriteChar(unsigned char ch);
void USARTWriteConstChar(const unsigned char ch);
void USARTWriteConstString(const unsigned char *str);
void USARTWriteString(unsigned char *str);
void USARTWriteConstLine(const unsigned char *str);
void USARTWriteLine(unsigned char *str);
void USARTWriteInt(signed int val, unsigned char field_length);
unsigned char USARTDataAvailable();
void USARTHandleRxInt();
char USARTReadData();
void USARTGotoNewLine();
void USARTReadBuffer(unsigned char *buff, unsigned char no_of_bytes);
void USARTFlushBuffer();
enum BufferStatus bufferWrite(unsigned char byte);
enum BufferStatus bufferRead(unsigned char *byte);

#endif	/* USART_PIC16_18_H */

