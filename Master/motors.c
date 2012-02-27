#include <p24fj256ga110.h>
#include "motors.h"

unsigned int leftMM = 0, rightMM = 0;
unsigned int leftCount = 0, rightCount = 0;

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt (void)
{
	_INT1IF = 0;
}

//Right Bottom sensor
void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt (void)
{
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
	
	//INT1 and INT2
	RPINR0bits.INT1R = 34;	//Left Encoder INT1 on RPI34
	RPINR1bits.INT2R = 33;	//Right Encoder INT2 on RPI33
	_INT1IE = 1;
	_INT2IE = 1;
	_INT1IP = 0b101;		//interrupt priority at 5
	_INT2IP = 0b101;
	
	//x2 PWM
	RPOR13bits.RP26R = 18;
	RPOR10bits.RP21R = 19;
	OC1CON1 = 0;
	OC1CON2 = 0;
	OC1CON1bits.OCTSEL = 0b111;
	OC1R = 0;			//duty cycle of 50%
	OC1RS = 50000;			//period
	OC1CON2bits.SYNCSEL = 0x1F;
	OC1CON1bits.OCM = 0b110;	//PWM, no fault
	
	OC2CON1 = 0;
	OC2CON2 = 0;
	OC2CON1bits.OCTSEL = 0b111;
	OC2R = 0;			//duty cycle of 50%
	OC2RS = 50000;			//period
	OC2CON2bits.SYNCSEL = 0x1F;
	OC2CON1bits.OCM = 0b110;	//PWM, no fault
}

//enable, disable, setSpeed, setSpeedDist, etc
void enableMotor()
{
	MOTORSTBY = 1;
	Nop();
	AIN1 = 0;
	Nop();
	AIN2 = 1;
	Nop();
	BIN1 = 1;
	Nop();
	BIN2 = 0;
	Nop();
}

void disableMotor()
{
	MOTORSTBY = 0;
}

void setSpeed(unsigned char speedL, unsigned char speedR)
{
	
}

void setSpeedDist(unsigned char speed, unsigned int dist)
{
	
}
