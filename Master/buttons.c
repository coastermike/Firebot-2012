#include <p24fj256ga110.h>
#include "buttons.h"

unsigned char startLastState = 1;	//1 represents off since pull up
unsigned char estopLastState = 1;

void __attribute__((__interrupt__, no_auto_psv)) _CNInterrupt(void)
{
	_CNIF = 0;
	if(EStop != estopLastState)
	{
		estopLastState = EStop;
	}
	if(Start != startLastState)
	{
		startLastState = Start;
	}			
}

void initButtons()
{
	TRISDbits.TRISD14 = 1;
	TRISDbits.TRISD15 = 1;
	
	//change notification interrupts
	_CN20IE = 1;	//enables 79, 80, 81 as change notification
	_CN21IE = 1;
	_CN20PUE = 1;	//enables the weak pull up resistor for 79, 80, 81
	_CN21PUE = 1;
	
	_CNIF = 0;				//clears interrupt flag
	_CNIP = 4;				//sets interrupt priority
	_CNIE = 1;				//enables interrupt
}
