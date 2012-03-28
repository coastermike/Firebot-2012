#include <p24fj256ga110.h>
#include "motors.h"
#include "analog.h"

#define COUNT_PER_MM 1.25

extern unsigned int IR1, IR2, IR3, IR4, IR5, IR6;

unsigned int leftMM = 0, rightMM = 0;	//distance in mm
unsigned int leftCount = 0, rightCount = 0;	//raw count from hall effects
unsigned int tempFollow = 0;
//
//1.25counts/mm, 4counts=5mm
//
//left wheel
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt (void)
{
	leftCount++;
	_INT1IF = 0;
}

//Right wheel
void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt (void)
{
	rightCount++;
	_INT2IF = 0;
}

void initMotors()
{
	TRISEbits.TRISE7 = 0;
	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	TRISCbits.TRISC3 = 0;
	TRISCbits.TRISC4 = 0;
	TRISGbits.TRISG6 = 0;
	TRISGbits.TRISG7 = 0;
	
	TRISEbits.TRISE8 = 1;
	TRISEbits.TRISE9 = 1;
	_CN67PUE = 1;	//left pull up for hall effect
	_CN66PUE = 1;	//right pull up for hall effect
	
	//INT1 and INT2
	RPINR0bits.INT1R = 34;	//Left Encoder INT1 on RPI34
	RPINR1bits.INT2R = 33;	//Right Encoder INT2 on RPI33
	_INT1EP = 0;
	_INT2EP = 0;
	_INT1IE = 1;
	_INT2IE = 1;
	_INT1IP = 5;		//interrupt priority at 5
	_INT2IP = 5;
	
	//x2 PWM
	RPOR13bits.RP26R = 18;
	RPOR10bits.RP21R = 19;
	OC1CON1 = 0;
	OC1CON2 = 0;
	OC1CON1bits.OCTSEL = 0b111;
	OC1R = 0;			//duty cycle of 0%
	OC1RS = 51000;			//period
	OC1CON2bits.SYNCSEL = 0x1F;
	OC1CON1bits.OCM = 0b110;	//PWM, no fault
	
	OC2CON1 = 0;
	OC2CON2 = 0;
	OC2CON1bits.OCTSEL = 0b111;
	OC2R = 0;			//duty cycle of 0%
	OC2RS = 51000;			//period
	OC2CON2bits.SYNCSEL = 0x1F;
	OC2CON1bits.OCM = 0b110;	//PWM, no fault
}

//enable, disable, setSpeed, setSpeedDist, etc
void enableMotor()
{
	MOTORSTBY = 1;
}

void disableMotor()
{
	MOTORSTBY = 0;
}

void setSpeed(int speedL, int speedR)
{
	if(speedL != 0 && speedR != 0)
	{
		enableMotor();
	}
	else
	{
		disableMotor();
	}		
	if(speedL == 0)
	{
		AIN1 = 0;
		AIN2 = 0;
		OC1R = 0;
	}
	else if(speedL > 0)
	{
		AIN1 = 0;
		AIN2 = 1;
		OC1R = speedL*200;
	}
	else if(speedL < 0)
	{
		AIN1 = 1;
		AIN2 = 0;
		OC1R = -speedL*200;
	}
	
	if(speedR == 0)
	{
		BIN1 = 0;
		BIN2 = 0;
		OC2R = 0;
	}
	else if(speedR > 0)
	{
		BIN1 = 1;
		Nop();
		BIN2 = 0;
		OC2R = speedR*200;
	}
	else if(speedR < 0)
	{
		BIN1 = 0;
		Nop();
		BIN2 = 1;
		OC2R = -speedR*200;
	}
}

void followRightWall(unsigned int speed)
{
	enableMotor();
	if(!tempFollow)
	{
		if(IR1 < 17)
		{
			setSpeed(-100, 100);
			tempFollow = 1;
		}
		else if(IR2 > 15)
		{
			setSpeed(speed, speed-150);
		}
		else if(IR2 < 5)
		{
			setSpeed(speed-150, speed);
		}	
		else if((int)(IR3-IR2) > 0)
		{
			setSpeed(speed-50, speed);
		}
		else if((int)(IR2-IR3) > 0)
		{
			setSpeed(speed, speed-50);
		}
		else
		{
			setSpeed(speed, speed);
		}
	}
	else if(tempFollow)
	{
		if((IR2 - IR3) > 1)
		{
			setSpeed(-100, 100);
		}	
		else if((IR3 - IR2) < 2)
		{
			tempFollow = 0;
		}	
	}		
}
	
void resetCount()
{
	setSpeed(0,0);
	leftCount = 0;
	rightCount = 0;
}
	
void setSpeedDist(int speed, unsigned int dist)
{
	
}
