#ifndef LEDS_H
#define LEDS_H

void initLED();

//defines LED pins as readable variables

#define LED1 PORTEbits.RE4
#define LED2 PORTEbits.RE3
#define LED3 PORTEbits.RE2
#define LED4 PORTGbits.RG13
#define LEDStatus PORTGbits.RG12

#endif
