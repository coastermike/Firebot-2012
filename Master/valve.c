#include <p24fj256ga110.h>
#include "valve.h"

unsigned int valveMS = 0;

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt (void)
{
	_T2IF = 0;
	if(valveMS>0)
	{
		valveMS--;
	}
	else
	{
		disableValve();
	}
}

void initValve()
{
	TRISGbits.TRISG15 = 0;
	
	//timer for ms counter
	T2CONbits.T32 = 0;
	T2CONbits.TCKPS = 0b10;	//1:64 prescalar
	T2CONbits.TCS = 0;
	T2CONbits.TGATE = 0;
	PR2 = 250;				//1ms timing
	T2CONbits.TON = 1;
}

void activateValve(unsigned int ms)
{
	valve = 1;
	valveMS = ms;
	_T2IE = 1;
}

void disableValve()
{
	
	_T2IE=0;
	valve = 0;
	valveMS = 0;
}	
