/*--------------------------------------------------------------------------------------
 RGBMAIN.C - Header file for the RGBMoodlight.
 Copyright (C) 2020 Jagannatha Rao (aka JagiChan) (jagannath_raous@yahoo.com)

 (1) Uses software PWM to generate 3 PWM with fixed frequency and variable duty cycle.
 (2) Generates Random colors or user selected color
 (3) Colors can be generated using either the buttons or by using commands on the USART. 
 (4) User selected color is stored in the EEPROM and restored during power on
 
 USART commands
 --------------
	(a) Send RRGGBB to trigger the color. The color generated is stored in the EEPROM.
	(b) Send X or x to clear the stored color and return to random color generation.

 BUTTONS
 -------
	(a) Keep Red, Green and Blue buttons pressed at power on to clear the EEPROM and user selected color.
	    The entire display will blink 5 times to indicate that the EEPROM has been cleared.
	(b) Press Red and Green buttons simultaneously when a color is being displayed to store the color in the EEPROM.
	    The entire display will blink 3 times to indicate that the selected color has been saved in the EEPROM
	(c) Press any color button during random color display to trigger user color selection.
	    The entire display will blink 2 times to indicate that the user mode has been entered.
	
 This program is free software: you can redistribute it and/or modify it under the terms
 of the version 3 GNU General Public License as published by the Free Software Foundation.
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.
 You should have received a copy of the GNU General Public License along with this program.
 If not, see <http://www.gnu.org/licenses/>.
--------------------------------------------------------------------------------------*/

#include "rgbmain.h"
#include "usart.h"
#include "eeprom.h"

//initial eeprom data
__EEPROM_DATA(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

// map of the colors which are defined as RRGGBB
const unsigned long colors[] = {0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF00FF, 0xFFFFFF, 0x9400D3};

unsigned int TMR1_Cntr;
unsigned char usartCRChar;
unsigned long userColor;
unsigned char userColorSelected;
unsigned char PWM_RedDC = 0, PWM_BlueDC = 0, PWM_GreenDC = 0;	// duty cycle for RGB pins

// The interrupt function used to generate the software PWM
void __interrupt() myISR()
{
    if (TMR1IF)
    {
        TMR1_Cntr++;

        //Red Duty Cycle Check
        if (TMR1_Cntr >= PWM_RedDC)
        {
            LedPinRed = 0; // Drive PWM Output LOW
        }

        //Green Duty Cycle Check
        if (TMR1_Cntr >= PWM_GreenDC)
        {
            LedPinGreen = 0; // Drive PWM Output LOW
        }

        //Blue Duty Cycle Check
        if (TMR1_Cntr >= PWM_BlueDC)
        {
            LedPinBlue = 0; // Drive PWM Output LOW
        }

        if (TMR1_Cntr == 100)
        {
            if (PWM_RedDC != 0)
                LedPinRed = 1; // Drive PWM Output HIGH

            if (PWM_GreenDC != 0)
                LedPinGreen = 1;

            if (PWM_BlueDC != 0)
                LedPinBlue = 1;

            TMR1_Cntr = 0; // Reset Counter
        }

        TMR1IF = 0; //clear timer 1 interrupt flag
        TMR1H = 0xFF; //reload the timer 1 for next interrupt
        TMR1L = 0x78;
    }


    //character received on USART
    if (RCIF)
    {
        if ((RCREG == '\r') || (RCREG == '\n')) //check for NL/CR and reject it and process the received data
        {
            usartCRChar = TRUE;
        }
        else
        {
            bufferWrite(RCREG);
        }
    }

}

// map of the colors used for interpolation
int map(unsigned char x, unsigned char in_min, unsigned char in_max, unsigned char out_min, unsigned char out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Confirms user operations by blinking the three LED's. This function takes number of blinks as input.
void confirmOperation(unsigned char blinks)
{
    GIE = OFF; //turn off the global interrupts

    for (unsigned char i = blinks; i > 0; i--)
    {
        CLRWDT(); //kick the dog
        //turn on all the LED's
        LedPinRed = ON;
        LedPinGreen = ON;
        LedPinBlue = ON;
        //wait for 500 ms
        __delay_ms(500);
        //turn off the LED's
        LedPinRed = OFF;
        LedPinGreen = OFF;
        LedPinBlue = OFF;
        //wait for 500 ms
        __delay_ms(500);
    }

    GIE = ON; //turn on the global interrupts
}

// Intialize the PWM to zero for soft start, reset usercolor selection.
void ledInit(void)
{
    //create a soft pwm, original duty cycle is 0Hz, range is 0~100
    PWM_RedDC = 0;
    PWM_GreenDC = 0;
    PWM_BlueDC = 0;
    TMR1_Cntr = 0;
    usartCRChar = 0;
    userColor = 0;
    userColorSelected = FALSE;
}

// Set the selected color and adapt the PWM to generate the color
void ledColorSet(unsigned long color) //set color, for example: 0xde3f47
{
    unsigned char r_val, g_val, b_val;

    r_val = (color & 0xFF0000) >> 16; //get red value
    g_val = (color & 0x00FF00) >> 8; //get green value
    b_val = (color & 0x0000FF) >> 0; //get blue value

    r_val = map(r_val, 0, 255, 0, 100); //change a num(0~255) to 0~100
    g_val = map(g_val, 0, 255, 0, 100);
    b_val = map(b_val, 0, 255, 0, 100);

    //change the duty cycle
    PWM_RedDC = r_val;
    PWM_GreenDC = g_val;
    PWM_BlueDC = b_val;

    if (userColorSelected)
    {
        EEwrite(RED_ADDR, PWM_RedDC); //save the red duty cycle
        EEwrite(GRN_ADDR, PWM_GreenDC);
        EEwrite(BLU_ADDR, PWM_BlueDC);
    }
}

// Initialize the Timer1 for software PWM generation
void InitTimer1()
{
    GIE = OFF;
    T1CON = 0x00;
    TMR1CS = 0; // Choose the local clock source (timer mode)
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    TMR1H = 0xFF; //setup the timer value for 200uS interrupt
    TMR1L = 0x38;

    TMR1ON = ON; 	//turn on Timer 1
    TMR1IF = 0; 	//clear timer 1 interrupt flag
    TMR1IE = ON; 	//enable timer 1 interrupt
    PEIE = ON; 		// Peripherals Interrupts Enable Bit
    GIE = ON; 		// Global Interrupts Enable Bit
}

void initHW(void)
{
    GIE = OFF; 		//disable global the interrupts
    CMCON = 7; 		//disable the comparators
    PCON = 0x08;
    //setup port pins
    TRISA = 0b01111100; 	//RA0,RA1,RA7 are outputs
    TRISB = 0b00111011; 	//RX is input; RB3,RB4,RB5 are inputs
    PORTA = 0x00;			//Clear Port A
    PORTB = 0x00;			//Clear Port B
    GIE = ON; 				//enable global interrupts
}

// Clear the USART receive buffer
void ResetUSARTBuffer(void)
{
    //reset the buffer and the write/read pointers
    memset(buffer.data, 0, sizeof (buffer.data)); //clear the buffer
    buffer.write_index = 0; //set head to start
    buffer.read_index = 0; //set head to start
    usartCRChar = FALSE;
}

// Process USART received data
void processUSART(void)
{
    unsigned char tempCharStorage;

    userColor = 0;

    if (usartCRChar)
    {
        CLRWDT(); //kick the dog (only 2.4 seconds available until dog barks)
        userColorSelected = TRUE;
        while (bufferRead(&tempCharStorage) == BUFFER_OK)
        {
            // if character 'X' or 'x' is sent on Serial port or detected in the color combination
			// the user created color is erased from the EEPROM and the system defaults to random color generation
			if ((tempCharStorage == 'X') || (tempCharStorage == 'x'))
            {
                userColorSelected = FALSE;	//clear the userColorSelected flag
                userColor = 0;				//reset userColor
                EEwrite(RED_ADDR, 0); 		//reset the red, green and blue duty cycle and store in EEPROM
                EEwrite(GRN_ADDR, 0);
                EEwrite(BLU_ADDR, 0);
                ResetUSARTBuffer();			//clear the USART buffer for next reception
                return; //exit this loop
            }

			// assimilate and assemble the user provided color sequence.
			// The color sequence should be provided as RRGGBB (the values are interpreted as Hex)
			// So to trigger an orange color you should send FFFF00 (which means Red = FF, Green = FF, Blue = 0)
            switch (tempCharStorage)
            {
                case '0':
                    tempCharStorage = 0;
                    break;

                case '1':
                    tempCharStorage = 1;
                    break;

                case '2':
                    tempCharStorage = 2;
                    break;

                case '3':
                    tempCharStorage = 3;
                    break;

                case '4':
                    tempCharStorage = 4;
                    break;

                case '5':
                    tempCharStorage = 5;
                    break;

                case '6':
                    tempCharStorage = 6;
                    break;

                case '7':
                    tempCharStorage = 7;
                    break;

                case '8':
                    tempCharStorage = 8;
                    break;

                case '9':
                    tempCharStorage = 9;
                    break;

                case 'A':
                case 'a':
                    tempCharStorage = 10;
                    break;

                case 'B':
                case 'b':
                    tempCharStorage = 11;
                    break;

                case 'C':
                case'c':
                    tempCharStorage = 12;
                    break;

                case 'D':
                case'd':
                    tempCharStorage = 13;
                    break;

                case 'E':
                case'e':
                    tempCharStorage = 14;
                    break;

                case 'F':
                case 'f':
                    tempCharStorage = 15;
                    break;

                default:
                    tempCharStorage = 0;
                    break;
            }
            userColor <<= 4; //shift 4 bits at a time
            userColor += tempCharStorage;
        }
        //reset usart buffer for next reading
        ResetUSARTBuffer();
        //set the user color
        ledColorSet(userColor);
    }
}

void main(void)
{
    unsigned long randcolor = 0;		//reset the randomcolor variable
    unsigned char SWDetails[10] = "";	//software details variable
    unsigned char prevSWVal = SWITCH_NOTPRESSED;	//no switches pressed

    userColorSelected = FALSE;			//user not selected a color
    userColor = 0;
    initHW(); 							//initialize the hardware
    ledInit();							//initalize the led PWM
    InitTimer1(); 						//initialize timer 1
    USARTInit(9600); 					//set 9600 baud (cannot go faster on a 4Mhz clock)

    USARTWriteConstString("# RGB LED");	//write text to USART. 
    USARTGotoNewLine();					
    USARTWriteConstString("HW Ver 1.2 & SW Ver 1.4");
    USARTGotoNewLine();
    sprintf(SWDetails, "Build %s", __DATE__);
    USARTWriteConstString(SWDetails);
    sprintf(SWDetails, " %s", __TIME__);
    USARTWriteConstString(SWDetails);
    USARTGotoNewLine();

    srand(TMR1L); 						//timer one low value will be used to seed the random number generator

    CLRWDT(); //kick the dog (only 2.4 seconds available until dog barks)

	// if all the buttons RED, GREEN and BLUE are pressed and held during power on then
    // clear eeprom of user color selection and set to random color generation
    if ((RED_BTN == SWITCH_PRESSED) && (GRN_BTN == SWITCH_PRESSED) && (BLU_BTN == SWITCH_PRESSED)) //all three buttons pressed
    {
        __delay_ms(DEBOUNCE_VALUE); 	//debounce key press
        if ((RED_BTN == SWITCH_PRESSED) && (GRN_BTN == SWITCH_PRESSED) && (BLU_BTN == SWITCH_PRESSED)) //all three buttons pressed
        {
            EEwrite(RED_ADDR, 0); //reset the red, green and blue duty cycle and write to EEPROM
            EEwrite(GRN_ADDR, 0);
            EEwrite(BLU_ADDR, 0);
            confirmOperation(5); //give 5 blinks to confirm erase of user color
        }

        for (;;); //wait for watchdog to reset
    }

    // check EEPROM if user has a color set
    PWM_RedDC = EEread(RED_ADDR);
    PWM_GreenDC = EEread(GRN_ADDR);
    PWM_BlueDC = EEread(BLU_ADDR);

	// if any one color is NOT zero in the EEPROM then user has a stored color
    if ((PWM_RedDC != 0) || (PWM_GreenDC != 0) || (PWM_BlueDC != 0))
        userColorSelected = TRUE;

    while (1)
    {
        CLRWDT(); 		//kick the dog

        processUSART(); //process any command received over serial port/BT

        //check that no buttons are pressed
        if ((RED_BTN == SWITCH_NOTPRESSED) && (GRN_BTN == SWITCH_NOTPRESSED) && (BLU_BTN == SWITCH_NOTPRESSED))
        {
            __delay_ms(DEBOUNCE_VALUE); //debounce
            if ((RED_BTN == SWITCH_NOTPRESSED) && (GRN_BTN == SWITCH_NOTPRESSED) && (BLU_BTN == SWITCH_NOTPRESSED))
            {
                prevSWVal = SWITCH_NOTPRESSED;
            }
        }
            //check if red and green button pressed simultaneously, the current color being displayed is stored in the EEPROM as user color
        else if ((RED_BTN == SWITCH_PRESSED) && (GRN_BTN == SWITCH_PRESSED))
        {
            __delay_ms(DEBOUNCE_VALUE); //debounce
            if ((RED_BTN == SWITCH_PRESSED) && (GRN_BTN == SWITCH_PRESSED))
            {
				if (userColorSelected && (prevSWVal == SWITCH_NOTPRESSED)) //user selected a color
                {
                    EEwrite(RED_ADDR, PWM_RedDC); //save the red duty cycle
                    EEwrite(GRN_ADDR, PWM_GreenDC); //save the green duty cycle
                    EEwrite(BLU_ADDR, PWM_BlueDC); //save the blue duty cycle
                    prevSWVal = SWITCH_PRESSED; //switch is pressed
                    confirmOperation(3); //give three blinks to indicate user color mode saved
                }
            }
        }

        // if any button is pressed during random color generation, then 2 blinks are given to indicate user mode entered.
        else if ((RED_BTN == SWITCH_PRESSED) || (GRN_BTN == SWITCH_PRESSED) || (BLU_BTN == SWITCH_PRESSED))
        {
            __delay_ms(DEBOUNCE_VALUE); //debounce
            if ((RED_BTN == SWITCH_PRESSED) || (GRN_BTN == SWITCH_PRESSED) || (BLU_BTN == SWITCH_PRESSED))
            {
                if ((userColorSelected == FALSE) && (prevSWVal == SWITCH_NOTPRESSED))
                {
                    userColorSelected = TRUE;
                    prevSWVal = SWITCH_PRESSED;
                    confirmOperation(2); //give two blinks to indicate user color mode selected
                }
            }
        }

        if (RED_BTN == SWITCH_PRESSED)
        {
            __delay_ms(DEBOUNCE_VALUE); //debounce
            if (RED_BTN == SWITCH_PRESSED) //check state
            {
                if (PWM_RedDC++ == 100) //if PWM reaches 100% then bring it back to zero
                    PWM_RedDC = 0;
            }
        }
        else if (GRN_BTN == SWITCH_PRESSED)
        {
            __delay_ms(DEBOUNCE_VALUE); //debounce
            if (GRN_BTN == SWITCH_PRESSED)
            {
                if (PWM_GreenDC++ == 100) //if PWM reaches 100% then bring it back to zero
                    PWM_GreenDC = 0;
            }
        }
        else if (BLU_BTN == SWITCH_PRESSED)
        {
            __delay_ms(DEBOUNCE_VALUE); //debounce
            if (BLU_BTN == SWITCH_PRESSED)
            {
                if (PWM_BlueDC++ == 100) //if PWM reaches 100% then bring it back to zero
                    PWM_BlueDC = 0;
            }
        }

        if (userColorSelected == FALSE) //show random colors
        {
            randcolor = ((unsigned long) ((rand() % 255) + 1)) << 16; // generate random red color
            randcolor += ((unsigned int) ((rand() % 255) + 1)) << 8; //generate random green color
            randcolor += ((unsigned int) ((rand() % 255) + 1)) << 0; //generate and assemble blue color
            ledColorSet(randcolor);
            __delay_ms(1000);
        }
    }
}
