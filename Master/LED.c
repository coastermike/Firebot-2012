#include <p24fj256ga110.h>
#include "LED.h"

unsigned int tempLEDCount = 0;

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
	_T1IF = 0;
/*	if(!AD1CON1bits.ASAM)
	{
		AD1CON1bits.ASAM = 1;
	}
	if(tempLEDCount < 1)
	{
		tempLEDCount++;
	}
	else
	{		
		LEDStatus = ~LEDStatus;
		tempLEDCount = 0;
	}
	*/
	LEDStatus = ~LEDStatus;
}	

void initLED()
{
	TRISEbits.TRISE4 = 0;
	TRISEbits.TRISE3 = 0;
	TRISEbits.TRISE2 = 0;
	TRISGbits.TRISG13 = 0;
	TRISGbits.TRISG12 = 0;
	
	//timer interrupt for status
	T1CONbits.TON = 1;
	T1CONbits.TCKPS = 0b11;
	T1CONbits.TCS = 0;
	T1CONbits.TGATE = 0;
	PR1 = 32500;//15625;
	_T1IE = 1;
	_T1IP = 3;
}
