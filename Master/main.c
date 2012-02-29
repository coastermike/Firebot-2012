#include <p24fj256ga110.h>
#include "uart.h"
#include "LED.h"
#include "buttons.h"
#include "i2c.h"
#include "analog.h"
#include "valve.h"
#include "motors.h"

//Configs
//Reference p24FJ256GA110.h
_CONFIG1(FWDTEN_OFF & ICS_PGx2 & COE_OFF  & BKBUG_OFF & GWRP_OFF & GCP_OFF & JTAGEN_OFF)
_CONFIG2(POSCMOD_HS & IOL1WAY_OFF & FNOSC_PRI)

unsigned int stateOfMarvin = 0;

int main(void)
{
	initAnalog();
	initLED();
	initUart();
	initButtons();
	initI2C();
	initValve();
	initMotors();
	
	AD1CON1bits.ASAM = 1;
	while(1)
	{
		//waiting for start button
		if(stateOfMarvin == 0 || stateOfMarvin == 1)
		{
			//look for sound start for 3seconds
		}
		//after start button
		else if(stateOfMarvin == 2)
		{
			enableMotor();
			setSpeed(255, 255);
		}		
	}
}
