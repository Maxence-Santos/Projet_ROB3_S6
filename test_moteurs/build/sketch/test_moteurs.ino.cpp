#include <Arduino.h>
#line 1 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
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

#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN);  // Set CS pin
#define MAX_DATA_SIZE 8

#if !defined(FALSE)
#define FALSE 0
#define TRUE 1
#endif
#include <SPI.h>
#include <math.h>

#define MY_PI 3.14159265359


#define MY_PI 3.14159265359

#define PERIOD_IN_MICROS 10000 // 10 ms


#define MOTOR_MAX_VEL_CMD 300000 
#define MOTOR_MAX_VOLTAGE_CMD 200 
#define MOTOR_MAX_POS_DEG 120.0
#define MOTOR_MIN_POS_DEG -120.0

// Movement types
#define STILL 1
#define SQUARE 2
#define TRIANGLE 3
/******************  GLOBAL VARIABLES *********************/
// Global motor state variables
double currentMotorPosDeg;
double currentMotorVelDegPerSec;
double previousMotorPosDeg;
int currentMotorPosEncoder;
int offsetMotorPosEnconder;
int currentNumOfMotorRevol;

// PIN ASSIGNMENT
uint8_t analogPinP0=A0; 
uint8_t analogPinP1=A2; 
int currentP0Rawvalue;
int currentP1Rawvalue;

// General purpose global variables
int counterForPrinting;
int printingPeriodicity;

unsigned long current_time=0;
unsigned long old_time=0;
unsigned long initial_time=0;

// MOVEMENT GENERATOR
int typeOfMovement = TRIANGLE;
double time_at_period_start_in_s=0.0;
 int signal;





  unsigned long int elapsedTimeFromePerioStartInMicros;
  double elapsedTime_from_period_start_in_s;
  long int signalPeriodInMicros = 20000000; // 20 seconds

#line 77 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void Consigne(int Mvt_type, int MOTOR_ID);
#line 118 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void motorON(int MOTOR_ID);
#line 134 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void motorOFF(int MOTOR_ID);
#line 153 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void sendVelocityCommand(long int vel, int MOTOR_ID);
#line 176 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void readMotorState(int MOTOR_ID);
#line 235 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void setup();
#line 314 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void loop();
#line 77 "/home/matteo/arduino/Projet_ROB3_S6/test_moteurs/test_moteurs.ino"
void Consigne(int Mvt_type, int MOTOR_ID){
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

  motorVelocityCommandInDegPerSec = gainPotValToVel*(signal - currentMotorPosDeg); // HERE YOU SHOULD COMPUTE THE CONTROLLER

  sendVelocityCommand((long int)(motorVelocityCommandInDegPerSec), MOTOR_ID);
  readMotorState(MOTOR_ID);




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
    currentMotorPosEncoder = (int)data6_7;
  }

  // Conversion of the velocity and writing in the global cariable
  currentMotorVelDegPerSec = ((double)(currentMotorVelRaw)); 

  // Conversion of the position (with motor revolution counting) and wirting in the global variable
  currentMotorPosEncoder -= offsetMotorPosEnconder;
  currentMotorPosDeg = ((double)(currentNumOfMotorRevol)*360.0) + (((double)currentMotorPosEncoder) * 180.0 / 32768.0);  // On convertit en degré

//  Serial.print("Dans readMotorState, ENC=");
//  Serial.print(currentMotorPosEncoder);
//  Serial.print("Dans readMotorState, REV=");
//  Serial.println(currentNumOfMotorRevol);

  if ((currentMotorPosDeg - previousMotorPosDeg) < -20.0) {
    currentNumOfMotorRevol++;
    currentMotorPosDeg = ((double)(currentNumOfMotorRevol)) * 360.0 + ((double)currentMotorPosEncoder) * 180.0 / 32768.0;
  }
  if ((currentMotorPosDeg - previousMotorPosDeg) > 20.0) {
    currentNumOfMotorRevol--;
    currentMotorPosDeg = ((double)(currentNumOfMotorRevol)) * 360.0 + ((double)currentMotorPosEncoder) * 180.0 / 32768.0;
  }

  previousMotorPosDeg = currentMotorPosDeg; // writing in the global variable for next call
}










void setup() {
  // put your setup code here, to run once:
  int i;
  char serialReceivedChar;
  int nothingReceived;

  // Initialization of the serial link
  Serial.begin(115200);

  // Initialization of the analog input pins
  pinMode(analogPinP0, INPUT);
  pinMode(analogPinP1, INPUT);
  currentP0Rawvalue = analogRead(analogPinP0);
  currentP1Rawvalue = analogRead(analogPinP1);

  // Initialization of the CAN communication. THis will wait until the motor is powered on
  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN init fail, retry ...");
    delay(500);
  }
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("");
  Serial.println("***********************************************************************");
  Serial.println("***********************************************************************");
  Serial.println("***********************************************************************");
  Serial.println(" Serial link and CAN initialization went ok! Power ON the motor");
  Serial.println("***********************************************************************");
  Serial.println("***********************************************************************");
  Serial.println("***********************************************************************");
  Serial.println("***********************************************************************");
  Serial.println("");
  previousMotorPosDeg = 0.0;
  currentNumOfMotorRevol = 0;
  offsetMotorPosEnconder = 0;

  // Send motot off then motor on commande to reset
  
  //motorOFF(1);
  motorOFF(2);
  delay(500);
  readMotorState(2);
  motorOFF(3);
  delay(500);
  readMotorState(3);
  Serial.println("");
  Serial.println("***********************************************************************");
  Serial.println(" Turn the rotor in its ZERO position they type 'S'");
  Serial.println("***********************************************************************");
    nothingReceived = TRUE;
  while (nothingReceived==TRUE){
    serialReceivedChar = Serial.read();
    if(serialReceivedChar == 'S') {
      nothingReceived = FALSE;
    }
  }

  //motorON(1);
  for (int i = 2;i<= 3; i++){
  motorON(i);
  readMotorState(i);
  delay(500);
  sendVelocityCommand((long int)(0) , (int)i);
  delay(500);
  readMotorState(i);
  }
  offsetMotorPosEnconder = currentMotorPosEncoder;
  currentNumOfMotorRevol = 0;
  previousMotorPosDeg = 0.0;

  Serial.println("End of Initialization routine.");

  counterForPrinting = 0;
  printingPeriodicity = 10;

}

void loop() {
  // put your main code here, to run repeatedly:
  int i;
  unsigned int sleep_time;
  unsigned long int elapsedTimeInMicros;
  double elapsed_time_in_s;

  double gainPotValToVel;
 

  int signalMin=450;
  int signalMax=550;

  old_time=current_time;
  current_time=micros();
  elapsedTimeInMicros = current_time-initial_time;
  elapsed_time_in_s = 0.000001*((double)(elapsedTimeInMicros));
  elapsedTimeFromePerioStartInMicros = elapsedTimeInMicros%signalPeriodInMicros;

  // STEP 2 : Reading the potentiometers

  currentP1Rawvalue = analogRead(analogPinP1);
  currentP0Rawvalue = analogRead(analogPinP0);

        counterForPrinting++;

  for (int i = 2; i<=2; i++){
    if (i == 3){
    Consigne( 3 , i);}
    if (i == 2){
    Consigne( 3 , i);}
    

    if (counterForPrinting >= printingPeriodicity) {  // Reset the counter and print
      counterForPrinting = 0;
      Serial.print("t:");
      Serial.print(elapsed_time_in_s);
      Serial.print(",Motor_" );
      Serial.print(i);
            Serial.print("_Pos:");
      Serial.print(currentMotorPosDeg);
      Serial.print(",CONSIGNE_MOT");
           Serial.print(i);
            Serial.print(":");
      Serial.println(signal);
    }
  }




  sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
  if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
    delayMicroseconds(sleep_time);
  }


}

