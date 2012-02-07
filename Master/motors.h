#ifndef MOTORS_H
#define MOTORS_H

void initMotors();

#define AIN1 PORTCbits.RC1
#define AIN2 PORTEbits.RE7
#define BIN1 PORTCbits.RC3
#define BIN2 PORTCbits.RC4
#define MOTORSTBY PORTCbits.RC2

#endif
