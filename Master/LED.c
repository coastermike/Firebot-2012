#include <p24fj256ga110.h>

void initLED()
{
	TRISEbits.TRISE4 = 0;
	TRISEbits.TRISE3 = 0;
	TRISEbits.TRISE2 = 0;
	TRISGbits.TRISG13 = 0;
	TRISGbits.TRISG12 = 0;
	
	//timer interrupt for status
}
