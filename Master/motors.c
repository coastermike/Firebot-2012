#include <p24fj256ga110.h>

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
	//x2 PWM
	
}

//enable, disable, setSpeed, setSpeedDist, etc
