#include <p24fj256ga110.h>

void initAnalog()
{
	AD1PCFGL = 0x00C3;
	/*2-Light Rear
	3-IR Right Rear
	4-IR Left Rear
	5-IR Rear
	8-Sound
	9-IR Right Front
	10-IR Front
	11-Fire Right
	12-Fire Middle
	13-Fire Left
	14-Light Front
	15-IR Left Front
	*/
}
