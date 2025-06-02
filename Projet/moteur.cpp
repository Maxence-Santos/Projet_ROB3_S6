#include "moteur.h"

#include <can-serial.h>
#include <mcp2515_can.h>
#include <mcp2515_can_dfs.h>
#include <mcp_can.h>
#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
const int SPI_CS_PIN = BCM8;
const int CAN_INT_PIN = BCM25;
#else
const int SPI_CS_PIN = 10;
const int CAN_INT_PIN = 2;
#endif


/******************  GLOBAL VARIABLES *********************/
//================== initialisation ====================

// Global motor state variables
double currentMotorPosDeg[3] = {0,0,0};
double currentMotorVelDegPerSec[3] = {0,0,0};
double previousMotorPosDeg[3]= {0,0,0};
int currentMotorPosEncoder[3] = {0,0,0};
int offsetMotorPosEnconder[3] = {0,0,0};
int currentNumOfMotorRevol[3] = {0,0,0};

// PIN ASSIGNMENT
uint8_t analogPinP0=A0; 
uint8_t analogPinP1=A2; 

unsigned long current_time=0;
unsigned long old_time=0;
unsigned long initial_time=0;

// MOVEMENT GENERATOR
int typeOfMovement = TRIANGLE;
double time_at_period_start_in_s=0.0;

long int signalPeriodInMicros = 20000000; // 20 seconds


int Consigne(int Mvt_type, int MOTOR_ID){
  double gainPotValToVel;
  int signalMin=450;
  int signalMax=550;

  double motorVelocityCommandInDegPerSec;



  switch(Mvt_type){
  case STILL:
    signal = signalMin;
    break;
  case SQUARE:
    if (2*elapsedTimeFromePerioStartInMicros < signalPeriodInMicros )
      signal = signalMin;
    else
      signal = signalMax;
    break;
  case TRIANGLE:
    if (2*elapsedTimeFromePerioStartInMicros < signalPeriodInMicros )
      signal = signalMin+(2*((signalMax-signalMin)*elapsedTimeFromePerioStartInMicros)/(signalPeriodInMicros));
    else
      signal = signalMax-(2*((signalMax-signalMin)*(elapsedTimeFromePerioStartInMicros-(signalPeriodInMicros/2)))/(signalPeriodInMicros));
    break;
  default:
    break;
  }
  gainPotValToVel = 30.0; // HERE YOU SHOULD SET THE PROPORTIONNAL GAIN
  
  motorVelocityCommandInDegPerSec = gainPotValToVel*(signal - currentMotorPosDeg[MOTOR_ID-1]); // HERE YOU SHOULD COMPUTE THE CONTROLLER

  sendVelocityCommand((long int)(motorVelocityCommandInDegPerSec), MOTOR_ID);
  readMotorState(MOTOR_ID);

  return signal;


}


void motorON(int MOTOR_ID){
  unsigned char msg[MAX_DATA_SIZE] = {
    0x88,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  };

  CAN.sendMsgBuf(0x140+MOTOR_ID, 0, 8, msg); 

}

void motorOFF(int MOTOR_ID){

  unsigned char msg[MAX_DATA_SIZE] = {
    0x80,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
  };

  CAN.sendMsgBuf(0x140+MOTOR_ID, 0, 8, msg); 
}



void sendVelocityCommand(long int vel, int MOTOR_ID) {

  long int local_velocity;

  local_velocity = vel;

  unsigned char *adresse_low = (unsigned char *)(&local_velocity);

  unsigned char msg[MAX_DATA_SIZE] = {
    0xA2,
    0x00,
    0x00,
    0x00,
    *(adresse_low),
    *(adresse_low + 1),
    *(adresse_low + 2),
    *(adresse_low + 3)
  };
  CAN.sendMsgBuf(0x140+MOTOR_ID, 0, 8, msg); 
}

void Velocityforward(long int vel, int MOTOR_ID_left, int MOTOR_ID_rigth){
    sendVelocityCommand(vel,MOTOR_ID_left);
    sendVelocityCommand(vel,MOTOR_ID_rigth);
}


void readMotorState(int MOTOR_ID)  {
  uint32_t id;
  uint8_t type;
  uint8_t len;
  byte cdata[MAX_DATA_SIZE] = { 0 };
  int data2_3, data4_5, data6_7;
  int currentMotorVelRaw;

  // wait for data
  while (CAN_MSGAVAIL != CAN.checkReceive())
    ;

  // read data, len: data length, buf: data buf
  CAN.readMsgBuf(&len, cdata);

  id = CAN.getCanId();
  type = (CAN.isExtendedFrame() << 0) | (CAN.isRemoteRequest() << 1);

  // Check if the received ID matches the motor ID
  if ((id - 0x140) == MOTOR_ID) { 
    data4_5 = cdata[4] + pow(2, 8) * cdata[5];
    currentMotorVelRaw = (int)data4_5;
    data6_7 = cdata[6] + pow(2, 8) * cdata[7];
    currentMotorPosEncoder[MOTOR_ID-1] = (int)data6_7;
  }

  // Conversion of the velocity and writing in the global cariable
  currentMotorVelDegPerSec[MOTOR_ID-1] = ((double)(currentMotorVelRaw)); 

  // Conversion of the position (with motor revolution counting) and wirting in the global variable
  currentMotorPosEncoder[MOTOR_ID-1] -= offsetMotorPosEnconder[MOTOR_ID-1];
  currentMotorPosDeg[MOTOR_ID-1] = ((double)(currentNumOfMotorRevol[MOTOR_ID-1])*360.0) + (((double)currentMotorPosEncoder[MOTOR_ID-1]) * 180.0 / 32768.0);  // On convertit en degré

//  Serial.print("Dans readMotorState, ENC=");
//  Serial.print(currentMotorPosEncoder);
//  Serial.print("Dans readMotorState, REV=");
//  Serial.println(currentNumOfMotorRevol);

  if ((currentMotorPosDeg[MOTOR_ID-1] - previousMotorPosDeg[MOTOR_ID-1]) < -20.0) {
    currentNumOfMotorRevol[MOTOR_ID-1]++;
    currentMotorPosDeg[MOTOR_ID-1] = ((double)(currentNumOfMotorRevol[MOTOR_ID-1])) * 360.0 + ((double)currentMotorPosEncoder[MOTOR_ID-1]) * 180.0 / 32768.0;
  }
  if ((currentMotorPosDeg[MOTOR_ID-1] - previousMotorPosDeg[MOTOR_ID-1]) > 20.0) {
    currentNumOfMotorRevol[MOTOR_ID-1]--;
    currentMotorPosDeg[MOTOR_ID-1] = ((double)(currentNumOfMotorRevol[MOTOR_ID-1])) * 360.0 + ((double)currentMotorPosEncoder[MOTOR_ID-1]) * 180.0 / 32768.0;
  }

  previousMotorPosDeg[MOTOR_ID-1] = currentMotorPosDeg[MOTOR_ID-1]; // writing in the global variable for next call
}