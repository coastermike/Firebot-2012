#ifndef MOTORS_H
#define MOTORS_H

void initMotors();
void enableMotor();
void disableMotor();
void setSpeed(int speedL, int speedR);
void setSpeedDist(int speed, unsigned int dist);
void followRightWall(unsigned int speed);
void followLeftWall(unsigned int speed);
void resetCount();

#define NORMSPEED 255

#define AIN1 PORTCbits.RC1
#define AIN2 PORTEbits.RE7
#define BIN1 PORTCbits.RC3
#define BIN2 PORTCbits.RC4
#define MOTORSTBY PORTCbits.RC2

#endif
