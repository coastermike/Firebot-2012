#include <p24fj256ga110.h>
#include "analog.h"

unsigned int count = 0;

unsigned int IR1raw = 0, IR2raw = 0, IR3raw = 0, IR4raw = 0, IR5raw = 0, IR6raw = 0;
unsigned int IR1 = 0, IR2 = 0, IR3 = 0, IR4 = 0, IR5 = 0, IR6 = 0;
unsigned int FireL = 0, FireM = 0, FireR = 0;
unsigned int LightF = 0, LightR = 0, Sound = 0;

unsigned int adc_table[DIST_COUNT][2] = 
	{
		{610, 3},
		{560, 4},
		{491, 5},
		{420, 6},
		{363, 7},
		{330, 8},
		{290, 9},
		{263, 10},
		{238, 11},
		{215, 12},
		{202, 13},
		{188, 14},
		{176, 15},
		{164, 16},
		{158, 17},
		{150, 18},
		{142, 19},
		{132, 20},
		{123, 21},
		{116, 22},
		{108, 23},
		{100, 24},
		{95, 25},
		{0, 26}
	};

void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt (void)
{
	AD1CON1bits.ASAM = 0;
	LightR = ADC1BUF0;
	IR3raw = ADC1BUF1;
	IR5raw = ADC1BUF2;
	IR4raw = ADC1BUF3;
	Sound = ADC1BUF4;
	IR2raw = ADC1BUF5;
	IR1raw = ADC1BUF6;
	FireR = ADC1BUF7;
	FireM = ADC1BUF8;
	FireL = ADC1BUF9;
	LightF = ADC1BUFA;
	IR6raw = ADC1BUFB;
	
	IR1 = convertIR(IR1raw);
	IR2 = convertIR(IR2raw);
	IR3 = convertIR(IR3raw);
	IR4 = convertIR(IR4raw);
	IR5 = convertIR(IR5raw);
	IR6 = convertIR(IR6raw);

	_AD1IF = 0;
}

unsigned int convertIR(unsigned int raw)
{
	unsigned int k=0;
	unsigned int value = 0;
	
	for(k=1; k < DIST_COUNT; k++)
	{
		if(adc_table[k][0] < raw)	
		{
			value = adc_table[k-1][1] + ((adc_table[k-1][0] - raw) / (adc_table[k-1][0] - adc_table[k][0]));
			break;
		}
	}	
//	if (k == DIST_COUNT)
//	{
//		value = adc_table[k-1][1];
//	}
	if (value < 4)
	{
		value = 3;
	}
	else if(value > 25)
	{
		value = 25;
	}			
	return value;
}

void initAnalog()
{
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
}
