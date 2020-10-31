# RGBMoodLight
RGB Moodlight (optionally controllable by Smartphone)

This is an RGB moonlight which is based on my very own expectations from a moonlight.

The hardware is based on a PIC16F628 which drives three 1W LEDs of Red, Green and Blue colors.

The software has been designed such that no extra clock crystals are needed for the micro.
The software is configured for interfacing the standard UART available on board the micro.
You can also connect a HC05 or HC06 bluetooth module to the Moonlight using appropriate hardware, and then using some custom Android software it is possible to control the moonlight from a smart phone.

The software has been written in pure C language and the included MPLab project can be compiled using the XC8 compiler.
You will need a full version of XC8 to be able to compile the software successfully.

Have fun.

Cheers,
JagiChan.
