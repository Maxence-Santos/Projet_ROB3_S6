#ifndef MOTEUR_H
#define MOTEUR_H

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
extern mcp2515_can CAN;  // Set CS pin
#define MAX_DATA_SIZE 8

#if !defined(FALSE)
#define FALSE 0
#define TRUE 1
#endif
#include <SPI.h>
#include <math.h>

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

// Motor ID
#define BRAS 1
#define LEFT 2
#define RIGHT 3

// Vitesse
#define VEL_NORMAL 500
#define VEL_LENT 100

/******************  GLOBAL VARIABLES *********************/
//=================  Declaration  =========================
//___________Global motor state variables__________________
extern double currentMotorPosDeg[3];
extern double currentMotorVelDegPerSec[3];
extern double previousMotorPosDeg[3];
extern int currentMotorPosEncoder[3];
extern int offsetMotorPosEnconder[3];
extern int currentNumOfMotorRevol[3];

// PIN ASSIGNMENT
extern uint8_t analogPinP0; 
extern uint8_t analogPinP1; 
extern int currentP0Rawvalue;
extern int currentP1Rawvalue;

// General purpose global variables
extern int counterForPrinting;
extern int printingPeriodicity;

extern unsigned long current_time;
extern unsigned long old_time;
extern unsigned long initial_time;

// MOVEMENT GENERATOR
extern int typeOfMovement;
extern double time_at_period_start_in_s;
extern int signal;

extern unsigned long int elapsedTimeFromePerioStartInMicros;
extern double elapsedTime_from_period_start_in_s;
extern long int signalPeriodInMicros; // 20 seconds

void motorON(int MOTOR_ID);

void motorOFF(int MOTOR_ID);

void sendVelocityCommand(long int vel, int MOTOR_ID);

void Velocityforward(long int vel, int MOTOR_ID_left, int MOTOR_ID_rigth);

int avance_x_mm(int distance,int sens);

int avance_x_increment(int increment,int sens);

int minimum(int a, int b);

void readMotorState(int MOTOR_ID);

#endif
