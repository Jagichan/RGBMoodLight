# RGBMoodLight
RGB Moodlight (optionally controllable by Smartphone)

This is an RGB moodlight which is based on my very own expectations from a moodlight.

The hardware is based on a PIC16F628 which drives three 1W LEDs of Red, Green and Blue colors.

The software has been designed such that no extra clock crystals are needed for the micro.
The software is configured for interfacing the standard UART available on board the micro. The UART needs to be connected to an appropriate level shifter like MAX232 or FTDI chip to enable it to communicate with a PC. You can also connect a HC05 or HC06 bluetooth module to the Moodlight using appropriate hardware, and then using some custom Android software it is possible to control the moonlight from a smart phone.

The software has been written in pure C language and the included MPLab project can be compiled using the XC8 compiler. You will need a full version of XC8 to be able to compile the software successfully.
Adapt the compiler header files if you have a different build environment other than MPLABX and XC8. 

Have fun.

Cheers,
JagiChan.
