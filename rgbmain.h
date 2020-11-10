/*--------------------------------------------------------------------------------------
 RGBMAIN.H - Header file for the RGBMoodlight.
 Copyright (C) 2020 Jagannatha Rao (aka JagiChan) (jagannath_raous@yahoo.com)

 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------*/

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

// PIC16F628A Configuration Bit Settings

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTRC oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RA5/MCLR pin function select (RA5/MCLR pin function is MCLR)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOD Reset disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable bit (RB4/PGM pin has digital I/O function, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection off)
#pragma config CP = ON          // Code Protection bits (Program memory code protection on)

// #pragma config statements should precede project file includes.

#include <xc.h> 				// include processor files - each processor file is guarded.
#include <stdlib.h> 			// standard library functions
#include <string.h>				// string functions
#include <stdio.h>				// standard I/O functions

#define _XTAL_FREQ (4000000UL)	//4Mhz internal clock is being used

#define TRUE    1
#define FALSE   0

#define ON      1
#define OFF     0

#define SWITCH_PRESSED 0    	//active low type
#define SWITCH_NOTPRESSED 1 	//active high
#define DEBOUNCE_VALUE 50  		//time in ms

#define LedPinRed    RA7
#define LedPinGreen  RA0
#define LedPinBlue   RA1

#define RED_BTN     RB3
#define GRN_BTN     RB4
#define BLU_BTN     RB5

//eeprom addresses to store the RGB values
#define RED_ADDR    0
#define GRN_ADDR    1
#define BLU_ADDR    2

void InitTimer1(void);

#endif	/* XC_HEADER_TEMPLATE_H */

