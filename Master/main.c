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
unsigned int room3Pos = 0, fireTemp = 0, fireCount = 0, fireSpin = 0, fireVal = 60;
unsigned int soundTemp = 0;

unsigned int lowestTemp = 60;
unsigned int fireState = 0;

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
//	stateOfMarvin = 6;
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
			if(leftCount < 450 || rightCount < 450) //40mm?
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
				if(roomCount == 1)
				{
					stateOfMarvin = 42;
				}
				else if(roomCount == 3)
				{
					while((IR6-IR5)>2)
					{
						AD1CON1bits.ASAM = 1;
						while(AD1CON1bits.ASAM && _AD1IF);
						setSpeed(-100,100);
					}	
					stateOfMarvin = 42;
				}
				else if((roomCount == 5) || (roomCount == 7))
				{
					stateOfMarvin = 42;
				}
				//getting out of room 1 to goto room 2 -robogames
				else if(roomCount == 2)
				{
					stateOfMarvin = 8;
					setSpeed(0,0);
				}	
				else if((roomCount == 4) || (roomCount == 6) || (roomCount == 8))
				{
					stateOfMarvin = 6;
				}
/*				else if (roomCount == 6)
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
				}*/	
			}
			setSpeed(NORMSPEED, NORMSPEED);		
		}
		//followleft until front white sensor
		else if(stateOfMarvin == 6)
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
			followLeftWall(NORMSPEED);
		}	
		//get out of room 1
		else if(stateOfMarvin == 8)
		{
			if(IR1>16)
			{
				setSpeed(NORMSPEED-100, NORMSPEED-100);
			}
			else
			{
				stateOfMarvin = 6;
			}
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
/*			if(roomCount == 1)
			{
				setSpeed(-100, 100);
				while(leftCount < 30);
				setSpeed(0,0);
				resetCount();
			}*/	
					
			setSpeed(60, -60);
			while(leftCount < 140)
			{
				AD1CON1bits.ASAM = 1;
				while(AD1CON1bits.ASAM && _AD1IF);
				if((FireL < 200) || (FireR < 200) || (FireM < 200))
				{
					stateOfMarvin = 198;
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
				if(roomCount == 1)
				{
					setSpeed(100, -100);
					while(leftCount < 200);
					setSpeed(0,0);
					stateOfMarvin = 43;
				}
				else
				{
					setSpeed(0,0);
					resetCount();
					setSpeed(-100,100);
					while(leftCount<35);
					setSpeed(0,0);
					stateOfMarvin = 50;
				}		
			}	
		}
		//driving forward to get ready to follow right wall out of room
		else if(stateOfMarvin == 43)
		{
			if(roomCount == 1)
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
		}
		//search for fire following during followleft wall
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
					stateOfMarvin = 198;
					setSpeed(0,0);
					resetCount();
					break;
				}
			}
			if(stateOfMarvin == 44)	
			{
				setSpeed(0,0);
				resetCount();
				stateOfMarvin = 50;
			}
		}	
		//leaving room 2,3
		else if(stateOfMarvin == 50)
		{	
			if(IR1 > 15)
			{
				if(IR2 < 11)
				{
					setSpeed(50, 150);
				}
				else
				{	
					setSpeed(NORMSPEED-100, NORMSPEED-100);
				}	
			}
			else
			{
				setSpeed(0,0);
				stateOfMarvin = 6;
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
		//spin til fireM is less than 60
		else if(stateOfMarvin == 198)
		{
			resetCount();
			fireSpin = 0;
			fireVal = 60;
			setSpeed(NORMSPEED-100, -NORMSPEED+100);
			while(FireM > fireVal)
			{
				AD1CON1bits.ASAM = 1;
				while(AD1CON1bits.ASAM && _AD1IF);
				if(leftCount > 470)
				{
					fireSpin++;
					resetCount();
				}
				if(fireSpin == 2)
				{
					fireVal += 10;
					fireSpin = 0;
				}	
			}	
			setSpeed(0,0);
			stateOfMarvin = 199;
		}	
		//search for lowest value on middle fire during rotation
		else if(stateOfMarvin == 199)
		{
			unsigned int lowestEscape = 0;
			lowestTemp = 60;
			while(lowestEscape == 0)
			{
				resetCount();
				setSpeed(NORMSPEED-200, -NORMSPEED+200);
				while(leftCount<1);
				setSpeed(0,0);
				AD1CON1bits.ASAM = 1;
				while(AD1CON1bits.ASAM && _AD1IF);
				if(FireM > lowestTemp)
				{
					lowestEscape = 1;
				}
				else
				{
					lowestTemp = FireM;
				}		
			}
			setSpeed(0,0);
			stateOfMarvin = 200;
		}	
		//going to flame
		else if(stateOfMarvin == 200)
		{
			//use intensity?
			//if mid is >value then too far away, while using left right to center
			
			//if(((FireM < 25) && (FireM > 0)) && ((FireL < 60) && (FireL > 0)) && ((FireR < 60) && (FireR > 0)))
			//{
			//	fireCount++;
			//}
			
			if(LightF > lightCalMidF)
			{
				fireCount++;
				fireState = 1;
			}	
			else if(IR6 < 10)
			{
				setSpeed(NORMSPEED, NORMSPEED-290);
				fireState = 2;
			}
			else if(IR5 < 11)
			{
				setSpeed(100, 80);
				fireState = 3;
			}	
			else if(IR2 < 10)
			{
				setSpeed(NORMSPEED-290, NORMSPEED);
				fireState = 4;
			}
			else if(IR3 < 11)
			{
				setSpeed(80,100);
				fireState = 5;
			}	
			else if(IR1 < 15)
			{
				setSpeed(NORMSPEED-100, NORMSPEED-270);
				fireState = 6;
			}
			else if(FireM > (lowestTemp+2))
			{
				stateOfMarvin = 198;
				//setSpeed(NORMSPEED-100, -NORMSPEED+100);
				//while(FireM > (lowestTemp + 2))
				//{
				//	AD1CON1bits.ASAM = 1;
				//	while(AD1CON1bits.ASAM && _AD1IF);
				//}
				setSpeed(0,0);
				fireState = 7;
			}
			else if((FireM < 40) && (FireM > 0))
			{
				if(((FireL < 150) && (FireL > 0)) && ((FireR < 150) && (FireR > 0)))
				{
					setSpeed(NORMSPEED, NORMSPEED);
				}
				else if((FireL < 150) && (FireL > 0))
				{
					setSpeed(NORMSPEED-90, NORMSPEED-210);
				}
				else if((FireR < 150) && (FireR > 0))
				{
					setSpeed(NORMSPEED-210, NORMSPEED-90);
				}
				else
				{
					setSpeed(50, 50);
				}
				fireState = 8;
			}	
			else
			{
				setSpeed(NORMSPEED-150, NORMSPEED-150);
				fireState = 9;
			}
			
			if(!(FireM > lowestTemp))
			{
				lowestTemp = FireM;
			}	
//			else if ((FireL < 65) && (FireR < 65))
//			{
//				setSpeed(NORMSPEED, NORMSPEED);
//				fireState = 4;
//			}	
//			else if((FireL < 65) && (FireL>0))
//			{
//				setSpeed(NORMSPEED, NORMSPEED-150);
//				fireState = 5;
//			}	
//			else if((FireR < 65) && (FireL>0))
//			{
//				setSpeed(NORMSPEED-150, NORMSPEED);
//				fireState = 6;
//			}
//			else if(((FireL<250) && (FireL>0)) || ((FireR<250) && (FireR>0)))
//			{
//				if(((FireL<250) && (FireL>0)) && ((FireR<250) && (FireR>0)))
//				{
//					setSpeed(NORMSPEED-150, NORMSPEED-150);
//				}
//				else if((FireL<250) && (FireL>0))
//				{
//					setSpeed(NORMSPEED-150, -NORMSPEED+150);
//				}
//				else if((FireR<250) && (FireR>0))
//				{
//					setSpeed(-NORMSPEED+150, NORMSPEED-150);
//				}
//				fireState = 7;
//			}
//			else if((FireM < 45) && (FireM > 0))
//			{
//				fireState = 20;
//				while((FireL > 760) || (FireR > 760))
//				{
//					setSpeed(NORMSPEED-150, -NORMSPEED+150);
//					AD1CON1bits.ASAM = 1;
//					while(AD1CON1bits.ASAM && _AD1IF);
//				}
//			}	
//			else if((FireL > FireR) && ((FireL/FireR) > 1))
//			{
//				setSpeed(-NORMSPEED+200, NORMSPEED-200);
//				fireState = 8;
//			}
//			else if((FireL < FireR) && ((FireR/FireL) > 1))
//			{
//				setSpeed(NORMSPEED-200, -NORMSPEED+200);
//				fireState = 9;
//			}
//			else if(((FireL - FireR) > 150) && ((FireL - FireR) < 300))
//			{
//				setSpeed(-NORMSPEED+200, NORMSPEED-200);
//				fireState = 8;
//			}
//			else if(((FireR - FireL) > 150) && ((FireR - FireL) < 300))
//			{
//				setSpeed(NORMSPEED-200, -NORMSPEED+200);
//				fireState = 9;
//			}		
//			//modified by adding
//			else if((FireM < fireVal) && (FireM > 0))
//			{	
//				setSpeed(NORMSPEED-200, NORMSPEED-200);
//				fireState = 10;
//				if(fireSpin == 1)
//				{
//					fireVal = fireVal+2;
//					fireSpin = 0;
//					fireState = 11;
//				}
//				
//			}
//			else
//			{
//				setSpeed(NORMSPEED-150, -NORMSPEED+150);
//				if(leftCount > 470)
//				{
//					fireSpin++;
//					resetCount();
//				}
//				fireState = 12;
//			}
			if(fireCount > 10)
			{
				stateOfMarvin = 201;
				fireCount = 0;
			}
//			if(fireSpin == 1)
//			{
//				fireVal = fireVal+3;
//				fireSpin = 0;
//				fireState = 11;
//			}	
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
				setSpeed(0,0);
				resetCount();
				setSpeed(-90, -90);
				while(leftCount < 100)
				{
					if(IR4 < 20)
					{
						setSpeed(0,0);
						break;
					}
					AD1CON1bits.ASAM = 1;
					while(AD1CON1bits.ASAM && _AD1IF);
				}		
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
				if(roomCount == 1)
				{
					setSpeed(0,0);
					resetCount();
					setSpeed(NORMSPEED-100, NORMSPEED-100);
					while((IR1 > 15) && (IR2 > 8))
					{
						AD1CON1bits.ASAM = 1;
						while(AD1CON1bits.ASAM && _AD1IF);
					}	
					setSpeed(0,0);
					stateOfMarvin = 210;
				}
				else if((roomCount == 3) || (roomCount == 5) || (roomCount == 7))
				{
					setSpeed(0,0);
					resetCount();
					setSpeed(NORMSPEED-100, NORMSPEED-100);
					while((IR1 > 15) && (IR2 > 8))
					{
						AD1CON1bits.ASAM = 1;
						while(AD1CON1bits.ASAM && _AD1IF);
					}	
					setSpeed(0,0);
					stateOfMarvin = 204;
					roomCount += 10;
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
				roomCount--;
				if(roomCount != 12)
				{
					stateOfMarvin = 204;
				}
				else if(roomCount == 12)
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
				while(leftCount < 20);
				setSpeed(0,0);
				stateOfMarvin = 211;
				lightCount = 0;
			}
			followRightWall(NORMSPEED);
		}
		//after white front drive straight til front IR
		else if(stateOfMarvin == 211)
		{
			if(IR1 > 15)
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
			stateOfMarvin = 226;
			roomCount = 0;
		}
		//stop moving state
		else if(stateOfMarvin == 226)
		{
			setSpeed(0,0);
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
