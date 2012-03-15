#include <stdio.h>

#define MAXTRANSMIT 40

unsigned int IR1 = 0;
unsigned int IR2 = 0;
unsigned int IR3 = 0;
unsigned int IR4 = 0;
unsigned int IR5 = 0;
unsigned int IR6 = 0;
unsigned int FireL= 0;
unsigned int FireM = 0;
unsigned int FireR = 0;
unsigned int LightF = 0;
unsigned int LightR = 0;
unsigned int Sound = 0;
unsigned int MotorL = 0;
unsigned int CountL = 0;
unsigned int MotorR = 0;
unsigned int CountR = 0;
unsigned int Mode = 0;

char IR1Str[10], IR2Str[10], IR3Str[10], IR4Str[10], IR5Str[10], IR6Str[10];
char FireLStr[10], FireMStr[10], FireRStr[10], LightFStr[10], LightRStr[10], SoundStr[10];
char MotorLStr[10], CountLStr[10], MotorRStr[10], CountRStr[10], ModeStr[30];

unsigned int receiveCount = 0;
unsigned char receiveData[MAXTRANSMIT];

void setup()
{
  background(0,0,0);
  fill(0,0,0);
  text("Firebot 2012 Debug", 55, 8, 10);
  text("Fire", 90, 30, 8);
  text("Fire", 140, 30, 8);
  text("Fire", 190, 30, 8);
  
  text("IR", 140, 50, 8);
  
  text("Motor", 40, 70, 8);
  text("IR", 90, 70, 8);
  text("Light", 140, 70, 8);
  text("IR", 190, 70, 8);
  text("Motor", 240, 70, 8);
  
  text("Count", 40, 90, 8);
  text("IR", 90, 90, 8);
  text("Light", 140, 90, 8);
  text("IR", 190, 90, 8);
  text("Count", 240, 90, 8);
  
  text("IR", 140, 110, 8);
  
  text("Sound: ", 10, 140, 8);
  text("State: ", 10, 165, 8);
  
  Serial.begin(57600);
}

void loop()
{
  if(Serial.available())
  {
    if(receiveCount<3)
    {
      if((Serial.read() == 0x40) && (receiveCount == 0))
      {
        receiveCount++;
      }
      else if((Serial.read() == 0x30) && (receiveCount == 1))
      {
        receiveCount++;
      }
      else if((Serial.read() == 0x20) && (receiveCount == 2))
      {
        receiveCount++;
      }
      else if((Serial.read() == 0x10) && (receiveCount == 3))
      {
        receiveCount++;
      }
      else
      {
        receiveCount = 0;
      }
    }
    else if(receiveCount < MAXTRANSMIT)
    {
      receiveData[receiveCount] = Serial.read();
      receiveCount++;
    }
    else
    {
      receiveCount = 0;
    }
    
    if(receiveCount == MAXTRANSMIT)
    {
      IR1 = (receiveData[4]<<8) | receiveData[5];
      IR2 = (receiveData[6]<<8) | receiveData[7];
      IR3 = (receiveData[8]<<8) | receiveData[9];
      IR4 = (receiveData[10]<<8) | receiveData[11];
      IR5 = (receiveData[12]<<8) | receiveData[13];
      IR6 = (receiveData[14]<<8) | receiveData[15];
      
      FireL = (receiveData[16]<<8) | receiveData[17];
      FireM = (receiveData[18]<<8) | receiveData[19];
      FireR = (receiveData[20]<<8) | receiveData[21];
      
      LightF = (receiveData[22]<<8) | receiveData[23];
      LightR = (receiveData[24]<<8) | receiveData[25];
      
      Sound = (receiveData[26]<<8) | receiveData[27];
      
      MotorL = (receiveData[28]<<8) | receiveData[29];
      CountL = (receiveData[30]<<8) | receiveData[31];
      MotorR = (receiveData[32]<<8) | receiveData[33];
      CountR = (receiveData[34]<<8) | receiveData[35];
      
      Mode = receiveData[36];
    }
  }
  
  stroke(0,0,0);
  rect(85, 25, 50, 8);
  stroke(255,165,0);
  sprintf(FireLStr, "%4d", FireL);
  text(FireLStr, 90, 30, 8);
  
  stroke(0,0,0);
  rect(135, 25, 50, 8);
  stroke(255,165,0);
  sprintf(FireMStr, "%4d", FireM);
  text(FireMStr, 140, 30, 8);
  
  stroke(0,0,0);
  rect(185, 25, 50, 8);
  stroke(255,165,0);
  sprintf(FireRStr, "%4d", FireR);
  text(FireRStr, 190, 30, 8);
  
  stroke(0,0,0);
  rect(135, 45, 50, 8);
  stroke(255,255,255);
  sprintf(IR1Str, "%4d", IR1);
  text(IR1Str, 140, 50, 8);
  
  
  text("Motor", 40, 70, 8);
  
  stroke(0,0,0);
  rect(85, 65, 50, 8);
  stroke(255,255,255);
  sprintf(IR6Str, "%4d", IR6);
  text(IR6Str, 90, 70, 8);
  
  
  text("Light", 140, 70, 8);
  
  stroke(0,0,0);
  rect(185, 65, 50, 8);
  stroke(255,255,255);
  sprintf(IR2Str, "%4d", IR2);
  text(IR2Str, 190, 70, 8);
  
  text("Motor", 240, 70, 8);
  
  text("Count", 40, 90, 8);
  
  stroke(0,0,0);
  rect(85, 85, 50, 8);
  stroke(255,255,255);
  sprintf(IR5Str, "%4d", IR5);
  text(IR5Str, 90, 90, 8);
  
  text("Light", 140, 90, 8);
  
  stroke(0,0,0);
  rect(185, 85, 50, 8);
  stroke(255,255,255);
  sprintf(IR3Str, "%4d", IR3);
  text(IR3Str, 190, 90, 8);
  
  text("Count", 240, 90, 8);
  
  stroke(0,0,0);
  rect(135, 105, 50, 8);
  stroke(255,255,255);
  sprintf(IR4Str, "%4d", IR4);
  text(IR4Str, 140, 110, 8);
  
  text("Sound: ", 10, 140, 8);
  text("State: ", 10, 165, 8);
  
    
}
