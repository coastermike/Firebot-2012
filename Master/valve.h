#ifndef VALVE_H
#define VALVE_H

void initValve();
void activateValve(unsigned int ms);
void disableValve();

#define valve PORTGbits.RG15

#endif
