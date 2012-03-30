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

extern unsigned int leftCount, rightCount;
extern unsigned int lightCalMidF, lightCalMidR;
extern unsigned int LightF, LightR, Sound;
extern unsigned int FireL, FireM, FireR;
extern unsigned int IR1;

unsigned int stateOfMarvin = 0, roomCount = 0, lightCount = 0;

int main(void)
{
	INTCON1bits.NSTDIS = 1;
	
	initAnalog();
	initLED();
	initUart();
	initButtons();
	initI2C();
	initValve();
	initMotors();

//	test state enable	
//	stateOfMarvin = 250;
	while(1)
	{
		AD1CON1bits.ASAM = 1;
		while(AD1CON1bits.ASAM && _AD1IF);
		
		//waiting for start button
		if(stateOfMarvin == 0 || stateOfMarvin == 1)
		{
			//look for sound start for 3seconds
		}
		//after start button, read white light to Cal
		else if(stateOfMarvin == 2)
		{
			setWhiteLight();
			resetCount();
			stateOfMarvin = 3;
			stateOfMarvin = 3;
		}
		//travel 20mm to get off white circle then stop and read black to Cal
		else if(stateOfMarvin == 3)
		{
			if(leftCount < 250 && rightCount < 250) //20mm?
			{
				setSpeed(NORMSPEED, NORMSPEED);
			}
			else
			{
				setSpeed(0,0);
				setBlackLight();
				stateOfMarvin = 4;
			}		
		}
		//travel right wall until white line
		else if(stateOfMarvin == 4)
		{
			if(LightF > lightCalMidF)
			{
				lightCount++;
			}
			else
			{
				lightCount = 0;
			}
			if(lightCount > 10)
			{
				stateOfMarvin = 5;
				lightCount = 0;
			}
			followRightWall(NORMSPEED);
		}
		//drive in until rear light sensor
		else if(stateOfMarvin == 5)
		{
			if(LightR > lightCalMidR)
			{
				lightCount++;
			}
			else
			{
				lightCount = 0;
			}
			if(lightCount > 10)
			{
				lightCount = 0;
				roomCount++;
				//1, 3, 5 fire, 2, 4, 6(to room4) next room
				if((roomCount == 1) || (roomCount == 3) || (roomCount == 5))
				{
					stateOfMarvin = 42;
				}
				else if((roomCount == 2) || (roomCount == 4))
				{
					stateOfMarvin = 4;
				}		
			}
			setSpeed(NORMSPEED, NORMSPEED);		
		}
		//fire search - move in room, spin, if flame goto it, if not goto leave
		else if(stateOfMarvin == 42)
		{
			setSpeed(0,0);
			resetCount();
			while(leftCount < 125)	//10cm?
			{
				setSpeed(NORMSPEED, NORMSPEED);
			}
			setSpeed(0,0);
			resetCount();
			setSpeed(60, -60);
			while(leftCount < 140)
			{
				AD1CON1bits.ASAM = 1;
				while(AD1CON1bits.ASAM && _AD1IF);
				if(FireL < 400 || FireR < 400 || FireM < 200)
				{
					stateOfMarvin = 200;
					setSpeed(0,0);
					break;
				}
			}
			if(stateOfMarvin == 42)	
			{
				setSpeed(0,0);
				resetCount();
				//turn to face exitting wall
				setSpeed(100, -100);
				while(leftCount < 200);
				setSpeed(0,0);
				stateOfMarvin = 43;
			}	
		}
		//driving forward to get ready to follow right wall out of room
		else if(stateOfMarvin == 43)
		{
			if(IR1 > 17)
			{
				setSpeed(NORMSPEED-100, NORMSPEED-100);
			}
			else
			{
				setSpeed(0,0);
				//if in rooms 1 or 2
				if(roomCount < 5)
				{
					stateOfMarvin = 4;
				}
				//if in room 3
				else
				{
					stateOfMarvin = 50;
				}
			}
		}
		//getting to room 4 coming from room 3
		else if(stateOfMarvin == 50)
		{
			setSpeed(0,0);
		}	
		//going to flame
		else if(stateOfMarvin == 200)
		{
			//use intensity?
			//if mid is >value then too far away, while using left right to center
		}
		//test state
		else if(stateOfMarvin == 250)
		{
			followRightWall(NORMSPEED);
		}		
	}
}
