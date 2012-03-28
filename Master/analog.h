#ifndef ANALOG_H
#define ANALOG_H

#define DIST_COUNT 24

unsigned int convertIR(unsigned int raw);
void initAnalog();
void calLight(unsigned int type);
void setWhiteLight();
void setBlackLight();

#endif
