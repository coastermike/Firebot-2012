#include <p24fj256ga110.h>
#include "buttons.h"
#include "motors.h"
#include "valve.h"

extern unsigned int stateOfMarvin;
unsigned char startLastState = 0;
unsigned char estopLastState = 0;

void __attribute__((__interrupt__, no_auto_psv)) _CNInterrupt(void)
{
	_CNIF = 0;
	if(EStop != estopLastState)
	{
		estopLastState = EStop;
		if(EStop && (stateOfMarvin == 0))	//maybe change to during state=1 and estop press for 2 buttons activation
		{
			activateValve(1000);
			disableMotor();
		}
		else if(EStop && (stateOfMarvin != 0))
		{
			disableValve();
			disableMotor();
			stateOfMarvin = 0;
			PR1 = 31250;
		}	
	}
	if(Start != startLastState)
	{
		startLastState = Start;
		if(Start && (stateOfMarvin == 0))
		{
			stateOfMarvin = 1;
		}
		if(!Start && (stateOfMarvin == 1))
		{
			stateOfMarvin = 2;
			PR1 = 10000;
		}		
	}			
}

void initButtons()
{
	TRISDbits.TRISD14 = 1;
	TRISDbits.TRISD15 = 1;
	
	//change notification interrupts
	_CN20IE = 1;	//enables 20, 21 as change notification
	_CN21IE = 1;
	_CN20PDE = 1;	//enables the weak pull up resistor for 20, 21
	_CN21PDE = 1;
	
	_CNIF = 0;				//clears interrupt flag
	_CNIP = 4;				//sets interrupt priority
	_CNIE = 1;				//enables interrupt
}
