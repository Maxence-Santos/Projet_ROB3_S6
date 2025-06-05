#include "moteur.h"
#include "math.h"
#include <Arduino.h>
mcp2515_can CAN(SPI_CS_PIN);  // Set CS pin

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
int printingPeriodicity = 0;

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
    sendVelocityCommand(-vel,MOTOR_ID_rigth);   //inverse la vitesse car les moteurs sont opposé
    readMotorState(LEFT);
    readMotorState(RIGHT);
}

int avance_x_mm(int distance, int sens){
  //avance le robot de distance mm si distance > 0 sinon de meme mais en reculant
  //retourne 1 lorsque le robot est arrivé à distination
  // sens 1 avance et -1 reculer
  int inc_par_mm = 400;  // ????? à verifier
  avance_x_increment(inc_par_mm * distance,sens);

  return 1;
}

int avance_x_increment(int increment, int sens){
  //avance le robot de distance mm si distance > 0 sinon de meme mais en reculant
  //retourne 1 lorsque le robot est arrivé à distination
  Serial.println("entre dans avance_x_increment");
  int vel_max = 10000;
  int vel;
  int gain_k = 30;    // à regler experimentalemnt

  int initial_increment = currentMotorPosEncoder[LEFT-1];
  Serial.println(initial_increment);
  int delta = sens*(initial_increment+increment*sens - currentMotorPosEncoder[LEFT-1]);
  while(delta > 0){
      //Serial.println(delta);
      //vel = minimum(vel_max,(gain_k*delta));
      vel = 5000;
      Velocityforward(vel*sens,LEFT,RIGHT);
      //delay(10);
      delta = sens*(initial_increment+increment*sens - currentMotorPosEncoder[LEFT-1]);
  }
  Velocityforward(0,LEFT,RIGHT);
  Serial.println(currentMotorPosEncoder[LEFT-1]);
  Serial.println("sort de avance_x_increment");
  return 1;
}

int minimum(int a, int b){
  if(a > b){
    return b;
  }
  return a;
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