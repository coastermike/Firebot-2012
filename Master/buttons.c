#include <p24fj256ga110.h>
#include "buttons.h"
#include "motors.h"
#include "valve.h"

extern unsigned int stateOfMarvin, roomCount;
unsigned char startLastState = 0;
unsigned char estopLastState = 0;

unsigned char sw1Temp = 0, sw2Temp = 0;

void __attribute__((__interrupt__, no_auto_psv)) _CNInterrupt(void)
{
	_CNIF = 0;
	if(Start && (sw1Temp == 0))
	{
		sw1Temp = 1;
	}
	else if(!Start && (sw1Temp == 1))
	{
		sw1Temp = 2;
	}
	if(EStop && (sw2Temp == 0))
	{
		sw2Temp = 1;
	}
	else if(!EStop && (sw2Temp == 1))
	{
		sw2Temp = 2;
	}			
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void)
{
	if((sw1Temp == 2) && !Start)
	{
		sw1Temp++;
	}
	else if((sw1Temp == 3) && !Start)
	{
		sw1Temp = 0;
		if((stateOfMarvin == 0) || (stateOfMarvin == 1))
		{
			stateOfMarvin = 2;
			PR1 = 10000;
		}
	}
	if((sw2Temp == 2) && !EStop)
	{
		sw2Temp++;
	}
	else if((sw2Temp == 3) && !EStop)
	{
		sw2Temp = 0;
		if(stateOfMarvin == 0)	//maybe change to during state=1 and estop press for 2 buttons activation
		{
			activateValve(100);
			disableMotor();
		}
		else if(stateOfMarvin != 0)
		{
			disableValve();
			disableMotor();
			setSpeed(0,0);
			stateOfMarvin = 0;
			roomCount = 0;
			PR1 = 32500;//15625;
		}
	}
	_T3IF = 0;
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
	
	T3CONbits.TCKPS = 0b10;
	T3CONbits.TCS = 0;
	T3CONbits.TGATE = 0;
	PR3 = 2500;
	T3CONbits.TON = 1;
	_T3IF = 0;
	_T3IP = 3;
	_T3IE = 1;
	
	_CNIF = 0;				//clears interrupt flag
	_CNIP = 4;				//sets interrupt priority
	_CNIE = 1;				//enables interrupt
}
