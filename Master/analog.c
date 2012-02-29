#include <p24fj256ga110.h>

#define CH_COUNT 12
unsigned int analogValues[CH_COUNT];
unsigned int count = 0;

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt (void)
{
	AD1CON1bits.ASAM = 0;
	analogValues[0]=ADC1BUF0;
	analogValues[1]=ADC1BUF1;
	analogValues[2]=ADC1BUF2;
	analogValues[3]=ADC1BUF3;
	analogValues[4]=ADC1BUF4;
	analogValues[5]=ADC1BUF5;
	analogValues[6]=ADC1BUF6;
	analogValues[7]=ADC1BUF7;
	analogValues[8]=ADC1BUF8;
	analogValues[9]=ADC1BUF9;
	analogValues[10]=ADC1BUFA;
	analogValues[11]=ADC1BUFB;
	_AD1IF = 0;
	AD1CON1bits.ASAM = 1;
}

void initAnalog()
{
	unsigned int i = 0;
	AD1PCFGL = 0x00C3;
	AD1PCFGHbits.PCFG17 = 1;
	AD1PCFGHbits.PCFG16 = 1;
	/*
	0,2-Light Rear
	1,3-IR Right Rear
	2,4-IR Left Rear
	3,5-IR Rear
	4,8-Sound
	5,9-IR Right Front
	6,10-IR Front
	7,11-Fire Right
	8,12-Fire Middle
	9,13-Fire Left
	10,14-Light Front
	11,15-IR Left Front
	*/
	
	AD1CON2bits.CSCNA = 1;	//input scan
	AD1CSSL = 0xFF3C;
	AD1CON2bits.VCFG = 0; //AVdd and AVss
	AD1CON3bits.ADCS = 0b00000001; //2*TCY
	AD1CON1bits.SSRC = 0b111;	//auto-convert
	AD1CON3bits.SAMC = 20;//auto sample time Tad
	AD1CON1bits.FORM = 0; //unsigned int
	AD1CON2bits.SMPI = 11;	//interrupt on 12th
	AD1CON2bits.BUFM = 0;	//buffer uses single word
	
	AD1CON1bits.ADON = 1;
	_AD1IF = 0;
	_AD1IP = 4;
	_AD1IE = 1;
	
	for(i=0; i<CH_COUNT; i++)
	{
		analogValues[i] = 0;
	}	
}
