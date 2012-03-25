#include <p24FJ256GA110.h>
#include "uart.h"
#include "LED.h"

#define MAXTRANSMIT 40

extern unsigned int IR1, IR2, IR3, IR4, IR5, IR6;
extern unsigned int FireL, FireM, FireR;
extern unsigned int LightF, LightR, Sound;
extern unsigned int speedL, speedR;
extern unsigned int leftCount, rightCount;
extern unsigned int stateOfMarvin;

unsigned char transmit[MAXTRANSMIT];
unsigned char transmitCount = 0;

void __attribute__((interrupt, no_auto_psv)) _T4Interrupt (void)
{
	writeUart();
	LED4 = ~LED4;
	_T4IF = 0;
}

//U1TX interrupt
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt (void)
{
	if(transmitCount < MAXTRANSMIT)
	{
		U1TXREG = transmit[transmitCount];
		transmitCount++;
	}	
	_U1TXIF = 0;
}
	
void initUart()
{
	unsigned char i = 0;
	
	RPINR18bits.U1RXR = 25;		//U1RX on RP25
	RPOR10bits.RP20R = 3;		//U1TX on RP20
	
	for(i=0; i<MAXTRANSMIT; i++)
	{
		transmit[i] = 0;
	}
	
	transmit[0] = 0x40;	//first 4 are a header that should indicate the start of transmission
	transmit[1] = 0x30;
	transmit[2] = 0x20;
	transmit[3] = 0x10;
		
	U1MODEbits.BRGH = 1;
	U1BRG = 68;					//baud rate generator value for 57600
	U1MODEbits.PDSEL = 0b00;	//8-bit data, no parity
	U1MODEbits.STSEL = 0;		//1 stop bits
	U1STAbits.UTXISEL1 = 0;	//interrupt ever transfer, at least one in buffer.
	U1STAbits.UTXISEL0 = 0;
	_U1TXIF = 0;
	_U1TXIE = 1;
	_U1TXIP = 5;
	U1MODEbits.UARTEN = 1;		//enable uart
	U1STAbits.UTXEN = 1;		//enable uart
	
	T4CONbits.T32 = 0;
	T4CONbits.TCKPS = 0b10;	//1:64 prescalar
	T4CONbits.TCS = 0;
	T4CONbits.TGATE = 0;
	PR4 = 50000;				//200ms timing
	T4CONbits.TON = 1;
	_T4IE = 1;
	_T4IP = 4;
}

//store variables into a matrix of 28 variables. Then start transmit of 28 bytes.
void writeUart()
{
	transmit[4] = (char)(IR1>>8);//takeoff;
	transmit[5] = (char)IR1;//takeoff;
	transmit[6] = (char)(IR2>>8);//landing;
	transmit[7] = (char)IR2;//landing;
	transmit[8] = (char)(IR3>>8);
	transmit[9] = (char)(IR3);
	transmit[10] = (char)(IR4>>8);
	transmit[11] = (char)(IR4);
	transmit[12] = (char)(IR5>>8);
	transmit[13] = (char)(IR5);
	transmit[14] = (char)(IR6>>8);
	transmit[15] = (char)(IR6);
	transmit[16] = (char)(FireL>>8);
	transmit[17] = (char)FireL;
	transmit[18] = (char)(FireM>>8);
	transmit[19] = (char)FireM;
	transmit[20] = (char)(FireR>>8);
	transmit[21] = (char)FireR;
	transmit[22] = (char)(LightF>>8);
	transmit[23] = (char)LightF;
	transmit[24] = (char)(LightR>>8);
	transmit[25] = (char)LightR;
	transmit[26] = (char)(Sound>>8);
	transmit[27] = (char)Sound;
	transmit[28] = (char)((OC1R/200)>>8);
	transmit[29] = (char)(OC1R/200);
	transmit[30] = (char)(leftCount>>8);
	transmit[31] = (char)leftCount;
	transmit[32] = (char)((OC2R/200)>>8);
	transmit[33] = (char)(OC2R/200);
	transmit[34] = (char)(rightCount>>8);
	transmit[35] = (char)rightCount;
	transmit[36] = (char)stateOfMarvin;
	
	while(!U1STAbits.TRMT);
	U1TXREG = transmit[0];
	U1TXREG = transmit[1];
	
	transmitCount = 2;
}
