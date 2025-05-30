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
double currentMotorPosDeg[3] = {0,0,0};
double currentMotorVelDegPerSec[3] = {0,0,0};
double previousMotorPosDeg[3]= {0,0,0};
int currentMotorPosEncoder[3] = {0,0,0};
int offsetMotorPosEnconder[3] = {0 , 0 , 0};
int currentNumOfMotorRevol[3] = {0,0,0};

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




  // Send motot off then motor on commande to reset
  
  //motorOFF(1);
  
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
  for (int i = 3;i<= 3; i++){
  motorON(i);
  readMotorState(i);
  delay(500);
  sendVelocityCommand((long int)(0) , (int)i);
  delay(500);
  readMotorState(i);
  
  offsetMotorPosEnconder[i-1] = currentMotorPosEncoder[i-1];
  }


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

/*
    int info_Mot[3] = {0,0,0};
   
    info_Mot[2] = Consigne( 2 , 2);
    //info_Mot[3] = Consigne(2 , 3);

    

    if (counterForPrinting >= printingPeriodicity) {  // Reset the counter and print
      counterForPrinting = 0;
        for(int i = 2;i<=2;i++){
      Serial.print("t:");
      Serial.print(elapsed_time_in_s);
      Serial.print(",Motor_" );
      Serial.print(i);
            Serial.print("_Pos:");
      Serial.print(currentMotorPosDeg[i-1]);
      Serial.print(",CONSIGNE_MOT");
           Serial.print(i);
            Serial.print(":");
      Serial.println(info_Mot[i-1]);
      }
    }*/

    sendVelocityCommand(-5000,3);
    sendVelocityCommand(5000,2);

    delay(2000);

    sendVelocityCommand(0,3);
    sendVelocityCommand(0,2);

    delay(1000);

  sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
  if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
    delayMicroseconds(sleep_time);
  }


}
