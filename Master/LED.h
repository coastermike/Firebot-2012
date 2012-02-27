#ifndef LEDS_H
#define LEDS_H

void initLED();

//defines LED pins as readable variables

#define LEDStatus PORTEbits.RE4
#define LED2 PORTEbits.RE3
#define LED3 PORTEbits.RE2
#define LED4 PORTGbits.RG13
#define LED1 PORTGbits.RG12

#endif
