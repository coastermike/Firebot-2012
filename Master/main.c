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
extern unsigned int IR1, IR2, IR3, IR4, IR5, IR6;

unsigned int stateOfMarvin = 0, roomCount = 0, lightCount = 0;	
unsigned int room3Pos = 0, fireTemp = 0, fireCount = 0, fireSpin = 0, fireVal = 45;
unsigned int soundTemp = 0;

int main(void)
{
	unsigned int i = 0;
	INTCON1bits.NSTDIS = 1;
	
	initAnalog();
	initLED();
	initUart();
	initButtons();
	initI2C();
	initValve();
	initMotors();
	
	T5CONbits.TCKPS = 0b11;
	T5CONbits.TCS = 0;
	T5CONbits.TGATE = 0;
	PR5 = 625;			//10ms
	T5CONbits.TON = 0;
	_T5IF = 0;
	
	AD1CON1bits.ASAM = 1;
	while(AD1CON1bits.ASAM && _AD1IF);
	
	resetCount();
					
//	test state enable	
//	stateOfMarvin = 203;
	while(1)
	{
		AD1CON1bits.ASAM = 1;
		while(AD1CON1bits.ASAM && _AD1IF);
		
		//waiting for start button
		if(stateOfMarvin == 0)
		{
			if(Sound > 400)
			{
				stateOfMarvin = 1;
			}	
		}
		//look for sound start for 1second
		else if (stateOfMarvin == 1)
		{
			if((Sound > 400) && (soundTemp < 10000))
			{
				soundTemp++;
			}
			else if(Sound > 400 && (soundTemp >= 10000))
			{
				stateOfMarvin = 2;
				soundTemp = 0;
			}
			else
			{
				soundTemp = 0;
				stateOfMarvin = 0;
			}		
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
			if(leftCount < 350 || rightCount < 350) //20mm?
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
			if(lightCount > 300)
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
			if(lightCount > 300)
			{
				lightCount = 0;
				roomCount++;
				//1, 3, 5 fire, 2, 4, 6(to room4) next room
				if((roomCount == 1) || (roomCount == 3) || (roomCount == 5) || (roomCount == 7))
				{
					if(roomCount != 5)
					{
						stateOfMarvin = 42;
					}
					else if(roomCount == 5)
					{
						if(IR2<20 || IR3<20)
						{
							stateOfMarvin = 44;
						}
						else
						{
							stateOfMarvin = 42;
						}		
					}
							
				}
				else if((roomCount == 2) || (roomCount == 4))
				{
					stateOfMarvin = 4;
				}
				else if (roomCount == 6)
				{
					setSpeed(0,0);
					resetCount();
					//modified to 100 from 50
					while(leftCount < 100)
					{
						setSpeed(-100, -100);
					}
					setSpeed(0,0);	
					stateOfMarvin = 55;
					resetCount();
				}
				else if ((roomCount == 6) && (room3Pos == 1))
				{
					setSpeed(0,0);
					stateOfMarvin = 55;
					resetCount();
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
			
			T5CONbits.TON = 1;
			for(i=NORMSPEED; i>10; i=i-10)
			{
				setSpeed(i, i);
				while(!_T5IF);
				_T5IF = 0;
			}	
			T5CONbits.TON = 0;
			
			setSpeed(0,0);
			resetCount();
			if(roomCount == 7)
			{
				setSpeed(-100, 100);
				while(leftCount < 30);
				setSpeed(0,0);
				resetCount();
			}	
					
			setSpeed(60, -60);
			while(leftCount < 140)
			{
				AD1CON1bits.ASAM = 1;
				while(AD1CON1bits.ASAM && _AD1IF);
				if((FireL < 200) || (FireR < 200) || (FireM < 200))
				{
					stateOfMarvin = 200;
					setSpeed(0,0);
					resetCount();
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
			if((roomCount == 1) || (roomCount == 3) || (roomCount == 5))
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
					else
					{
						stateOfMarvin = 4;
						room3Pos = 1;
						setSpeed(0,0);
						resetCount();
					}	
				}
			}				
		}
		//room 3 door along wall search for fire
		else if(stateOfMarvin == 44)
		{
			setSpeed(0,0);
			
			resetCount();
			while(leftCount < 125)	//10cm?
			{
				setSpeed(NORMSPEED, NORMSPEED);
			}
			
			T5CONbits.TON = 1;
			for(i=NORMSPEED; i>10; i=i-10)
			{
				setSpeed(i, i);
				while(!_T5IF);
				_T5IF = 0;
			}	
			T5CONbits.TON = 0;
			
			setSpeed(0,0);
			resetCount();
			setSpeed(-60, 60);
			while(leftCount < 100)
			{
				AD1CON1bits.ASAM = 1;
				while(AD1CON1bits.ASAM && _AD1IF);
				if((FireL < 200) || (FireR < 200) || (FireM < 200))
				{
					stateOfMarvin = 200;
					setSpeed(0,0);
					resetCount();
					break;
				}
			}
			if(stateOfMarvin == 44)	
			{
				setSpeed(0,0);
				resetCount();
				//turn to face exitting wall
//				setSpeed(-100, 100);
//				while(leftCount < 100);
//				setSpeed(0,0);
				stateOfMarvin = 50;
			}
		}	
		//getting to room 4 coming from room 3 with room 3 door in pos 1 - door next to wall
		else if(stateOfMarvin == 50)
		{
			if(IR1 > 17)
			{
				setSpeed(NORMSPEED-100, NORMSPEED-100);
			}
			else
			{
				setSpeed(0,0);
				stateOfMarvin = 4;
			}	
			
		}
		//go for a bit follow right wall to avoid problems with front sensor
		else if(stateOfMarvin == 55)
		{
			if(leftCount < 1300)
			{
				followRightWall(NORMSPEED);
			}
			else
			{	
				stateOfMarvin = 56;
				resetCount();
			}	
		}	
		//follow right wall til front sensor senses
		else if(stateOfMarvin == 56)
		{
			if(IR1 < 20)
			{
				setSpeed(0,0);
				stateOfMarvin = 57;
				resetCount();
			}
			else
			{
				followRightWall(NORMSPEED);
			}
		}
		//go a bit to get to a point where left wall can take over
		else if(stateOfMarvin == 57)
		{
			if(leftCount < 700)
			{
				followRightWall(NORMSPEED);
			}
			else
			{
				setSpeed(0,0);
				resetCount();
				stateOfMarvin = 58;
			}	
		}	
		//switch to left wall follow//starting to follow leftWall for room4 locating whiteline
		else if(stateOfMarvin == 58)
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
			followLeftWall(NORMSPEED);
		}
	
		//going to flame
		else if(stateOfMarvin == 200)
		{
			//use intensity?
			//if mid is >value then too far away, while using left right to center
			
			if(((FireM < 25) && (FireM > 0)) && ((FireL < 60) && (FireR < 60)) && ((FireR < 60) && (FireR < 60)))
			{
				fireCount++;
			}
			else if(IR6 < 8)
			{
				setSpeed(NORMSPEED, NORMSPEED-290);
			}
			else if(IR2 < 8)
			{
				setSpeed(NORMSPEED-290, NORMSPEED);
			}		
			else if ((FireL < 65) && (FireR < 65))
			{
				setSpeed(NORMSPEED, NORMSPEED);
			}	
			else if(FireL < 65)
			{
				setSpeed(NORMSPEED, NORMSPEED-150);
			}	
			else if(FireR < 65)
			{
				setSpeed(NORMSPEED-150, NORMSPEED);
			}
			else if(((FireL<250) && (FireL>0)) || ((FireR<250) && (FireR>0)))
			{
				setSpeed(NORMSPEED-150, NORMSPEED-150);
			}
			//modified by adding
			else if(FireM < fireVal)
			{
				setSpeed(NORMSPEED-200, NORMSPEED-200);
				if(fireCount == 3)
				{
					fireVal = fireVal+50;
					fireSpin = 0;
				}	
			}		
			else
			{
				setSpeed(NORMSPEED-150, -NORMSPEED+150);
				if(leftCount > 470)
				{
					fireSpin++;
					resetCount();
				}	
			}
			if(fireCount > 5)
			{
				stateOfMarvin = 201;
				fireCount = 0;
			}	
		}
		
		else if(stateOfMarvin == 201)
		{
			T5CONbits.TON = 1;
			for(i=NORMSPEED-100; i>10; i=i-10)
			{
				setSpeed(i, i);
				while(!_T5IF);
				_T5IF = 0;
			}	
			T5CONbits.TON = 0;
			setSpeed(0,0);
			stateOfMarvin = 202;
		}
		//put out fire sequence
		else if(stateOfMarvin == 202)
		{
			//fire rotate and spray
			if(fireTemp==0)
			{
				resetCount();
				activateValve(300);
				setSpeed(-NORMSPEED, NORMSPEED);
				while(leftCount < 20);
				setSpeed(0,0);
				fireTemp = 1;
			}
			if((FireL < 50) || (FireM < 50) || (FireR < 50))
			{
				resetCount();
				activateValve(500);
				setSpeed(NORMSPEED-100, -NORMSPEED+100);
				while(leftCount < 70);
				setSpeed(0,0);
				resetCount();
				activateValve(500);
				setSpeed(-NORMSPEED+100, NORMSPEED-100);
				while(leftCount < 70);
				setSpeed(0,0);
			}
			//modified by adding to double check flame
			else
			{
				stateOfMarvin = 203;
				setSpeed(0,0);
				resetCount();
			}	
		}
		//spin to double check no flame
		else if(stateOfMarvin == 203)
		{
			//full CCW 450degrees while scanning fire
			setSpeed(-NORMSPEED+100, NORMSPEED-100);
			if(leftCount < 570)
			{
				if(((FireL<150) && (FireL>0)) || ((FireR<150) && (FireR>0)) || ((FireM<150) && (FireM>0)))
				{
					setSpeed(0,0);
					stateOfMarvin = 200;
					resetCount();
				}
			}
			else
			{
				setSpeed(0,0);
				if((roomCount == 1) || (roomCount == 3) || (roomCount == 5))
				{
					stateOfMarvin = 204;
					roomCount += 10;
				}
				else
				{
					stateOfMarvin = 210;
				}
				resetCount();
			}		
		}
		//return home, rooms 1,2,3 continue states until roomCount = 6 then followrightwall till white circle
		//room 4, leave room then followright wall?
		
		//follow right wall until roomCount = 15(to avoid issue with room3) then go til circle
		else if(stateOfMarvin == 204)
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
				stateOfMarvin = 205;
				lightCount = 0;
			}
			followRightWall(NORMSPEED);
		}
		else if(stateOfMarvin == 205)
		{
			if(LightR > lightCalMidR)
			{
				lightCount++;
			}
			else
			{
				lightCount = 0;
			}
			if(lightCount > 30)
			{
				lightCount = 0;
				roomCount++;
				if(roomCount != 16)
				{
					stateOfMarvin = 204;
				}
				else if(roomCount == 16)
				{
					setSpeed(0,0);
					resetCount();
					//modified to 100 from 50
					setSpeed(-100, -100);
					while(leftCount < 60);
					setSpeed(5,5);	
					stateOfMarvin = 206;
					resetCount();
				}	
			}
			setSpeed(NORMSPEED, NORMSPEED);
		}
		//goto home circle from room 3
		else if(stateOfMarvin == 206)
		{
			if(LightF > lightCalMidF)
			{
				lightCount++;
			}
			else
			{
				lightCount = 0;
			}
			if(lightCount > 30)
			{
				stateOfMarvin = 225;
				lightCount = 0;
			}
			followRightWall(NORMSPEED);
		}
		//if in forth room to get back to home
		else if(stateOfMarvin == 210)
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
				setSpeed(0,0);
				resetCount();
				setSpeed(-100, 100);
				while(leftCount < 50);
				setSpeed(0,0);
				stateOfMarvin = 211;
				lightCount = 0;
			}
			followRightWall(NORMSPEED);
		}
		//after white front drive straight til front IR
		else if(stateOfMarvin == 211)
		{
			if(IR1 > 17)
			{
				setSpeed(NORMSPEED-100, NORMSPEED-100);
			}
			else
			{
				setSpeed(0,0);
				stateOfMarvin = 206; //follow right til circle
			}	
		}	
		//shut down
		else if(stateOfMarvin == 225)
		{
			setSpeed(0,0);
			resetCount();
			setSpeed(0,0);
			stateOfMarvin = 0;
			roomCount = 0;
		}	
		//test state
		else if(stateOfMarvin == 250)
		{
			resetCount();
			while(leftCount < 50)
			{
				setSpeed(NORMSPEED, NORMSPEED);
			}
			setSpeed(0,0);
			stateOfMarvin = 0;
		}
//	Sound = roomCount;		
	}
}
