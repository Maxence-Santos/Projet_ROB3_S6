// #include "recherche_balise.h"
// #include "HCSR04.h"
// #include <can-serial.h>
// #include <mcp2515_can.h>
// #include <mcp2515_can_dfs.h>
// #include <mcp_can.h>
// #if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// const int SPI_CS_PIN = BCM8;
// const int CAN_INT_PIN = BCM25;
// #else
// const int SPI_CS_PIN = 10;
// const int CAN_INT_PIN = 2;
// #endif

// #include "mcp2515_can.h"
// mcp2515_can CAN(SPI_CS_PIN);  // Set CS pin
// #define MAX_DATA_SIZE 8

// #if !defined(FALSE)
// #define FALSE 0
// #define TRUE 1
// #endif
// #include <SPI.h>
// #include <math.h>

// #define MY_PI 3.14159265359

// #define PERIOD_IN_MICROS 10000 // 10 ms

// #define MOTOR_MAX_VEL_CMD 300000 
// #define MOTOR_MAX_VOLTAGE_CMD 200 
// #define MOTOR_MAX_POS_DEG 120.0
// #define MOTOR_MIN_POS_DEG -120.0

// #define Broche_Echo_1 5 // Broche Echo du HC-SR04 sur D7 // 
// #define Broche_Trigger_1 4 // Broche Trigger du HC-SR04 sur D8 //
 
//  // Definition des variables  
// int MesureMaxi = 4000; 
// // Distance maxi a mesurer // 
// int MesureMini = 30; 
// // Distance mini a mesurer //  
// long Duree; 
// long Distance;
// /******************  GLOBAL VARIABLES *********************/
// // Global motor state variables
// double currentMotorPosDeg[3] = {0,0,0};
// double currentMotorVelDegPerSec[3] = {0,0,0};
// double previousMotorPosDeg[3]= {0,0,0};
// int currentMotorPosEncoder[3] = {0,0,0};
// int offsetMotorPosEnconder[3] = {0 , 0 , 0};
// int currentNumOfMotorRevol[3] = {0,0,0};

// // PIN ASSIGNMENT
// uint8_t analogPinP0=A0; 
// uint8_t analogPinP1=A2; 

// // General purpose global variables
// int printingPeriodicity;

// unsigned long current_time=0;

// // MOVEMENT GENERATOR

// unsigned long int elapsedTimeFromePerioStartInMicros;
// double elapsedTime_from_period_start_in_s;
// long int signalPeriodInMicros = 20000000; // 20 seconds

// void motorON(int MOTOR_ID){
//   unsigned char msg[MAX_DATA_SIZE] = {
//     0x88,
//     0x00,
//     0x00,
//     0x00,
//     0x00,
//     0x00,
//     0x00,
//     0x00
//   };

//   CAN.sendMsgBuf(0x140+MOTOR_ID, 0, 8, msg); 

// }

// void motorOFF(int MOTOR_ID){

//   unsigned char msg[MAX_DATA_SIZE] = {
//     0x80,
//     0x00,
//     0x00,
//     0x00,
//     0x00,
//     0x00,
//     0x00,
//     0x00
//   };

//   CAN.sendMsgBuf(0x140+MOTOR_ID, 0, 8, msg); 

// }

// void sendVelocityCommand(long int vel, int MOTOR_ID) {

//   long int local_velocity;

//   local_velocity = vel;

//   unsigned char *adresse_low = (unsigned char *)(&local_velocity);

//   unsigned char msg[MAX_DATA_SIZE] = {
//     0xA2,
//     0x00,
//     0x00,
//     0x00,
//     *(adresse_low),
//     *(adresse_low + 1),
//     *(adresse_low + 2),
//     *(adresse_low + 3)

//   };

//   CAN.sendMsgBuf(0x140+MOTOR_ID, 0, 8, msg); 
// }

// void readMotorState(int MOTOR_ID)  {
//   uint32_t id;
//   uint8_t type;
//   uint8_t len;
//   byte cdata[MAX_DATA_SIZE] = { 0 };
//   int data2_3, data4_5, data6_7;
//   int currentMotorVelRaw;

//   // wait for data
//   while (CAN_MSGAVAIL != CAN.checkReceive());

//   // read data, len: data length, buf: data buf
//   CAN.readMsgBuf(&len, cdata);

//   id = CAN.getCanId();
//   type = (CAN.isExtendedFrame() << 0) | (CAN.isRemoteRequest() << 1);

//   // Check if the received ID matches the motor ID
//   if ((id - 0x140) == MOTOR_ID) { 
//     data4_5 = cdata[4] + pow(2, 8) * cdata[5];
//     currentMotorVelRaw = (int)data4_5;
//     data6_7 = cdata[6] + pow(2, 8) * cdata[7];
//     currentMotorPosEncoder[MOTOR_ID-1] = (int)data6_7;
//   }

//   // Conversion of the velocity and writing in the global cariable
//   currentMotorVelDegPerSec[MOTOR_ID-1] = ((double)(currentMotorVelRaw)); 

//   // Conversion of the position (with motor revolution counting) and wirting in the global variable
//   currentMotorPosEncoder[MOTOR_ID-1] -= offsetMotorPosEnconder[MOTOR_ID-1];
//   currentMotorPosDeg[MOTOR_ID-1] = ((double)(currentNumOfMotorRevol[MOTOR_ID-1])*360.0) + (((double)currentMotorPosEncoder[MOTOR_ID-1]) * 180.0 / 32768.0);  // On convertit en degré

// //  Serial.print("Dans readMotorState, ENC=");
// //  Serial.print(currentMotorPosEncoder);
// //  Serial.print("Dans readMotorState, REV=");
// //  Serial.println(currentNumOfMotorRevol);

//   if ((currentMotorPosDeg[MOTOR_ID-1] - previousMotorPosDeg[MOTOR_ID-1]) < -20.0) {
//     currentNumOfMotorRevol[MOTOR_ID-1]++;
//     currentMotorPosDeg[MOTOR_ID-1] = ((double)(currentNumOfMotorRevol[MOTOR_ID-1])) * 360.0 + ((double)currentMotorPosEncoder[MOTOR_ID-1]) * 180.0 / 32768.0;
//   }
//   if ((currentMotorPosDeg[MOTOR_ID-1] - previousMotorPosDeg[MOTOR_ID-1]) > 20.0) {
//     currentNumOfMotorRevol[MOTOR_ID-1]--;
//     currentMotorPosDeg[MOTOR_ID-1] = ((double)(currentNumOfMotorRevol[MOTOR_ID-1])) * 360.0 + ((double)currentMotorPosEncoder[MOTOR_ID-1]) * 180.0 / 32768.0;
//   }

//   previousMotorPosDeg[MOTOR_ID-1] = currentMotorPosDeg[MOTOR_ID-1]; // writing in the global variable for next call
// }

// void start() {
//   // put your setup code here, to run once:
//   int i;
//   char serialReceivedChar;
//   int nothingReceived;

//   // Initialization of the serial link
//   Serial.begin(115200);

//   // Initialization of the analog input pins
//   pinMode(analogPinP0, INPUT);
//   pinMode(analogPinP1, INPUT);
//   pinMode(Broche_Trigger_1, OUTPUT); // Broche Trigger en sortie // 
//   pinMode(Broche_Echo_1, INPUT); // Broche Echo en entree // 
//   currentP0Rawvalue = analogRead(analogPinP0);
//   currentP1Rawvalue = analogRead(analogPinP1);

//   // Initialization of the CAN communication. THis will wait until the motor is powered on
//   while (CAN_OK != CAN.begin(CAN_500KBPS)) {
//     Serial.println("CAN init fail, retry ...");
//     delay(500);
//   }
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   Serial.println("");
//   Serial.println("***********************************************************************");
//   Serial.println("***********************************************************************");
//   Serial.println("***********************************************************************");
//   Serial.println(" Serial link and CAN initialization went ok! Power ON the motor");
//   Serial.println("***********************************************************************");
//   Serial.println("***********************************************************************");
//   Serial.println("***********************************************************************");
//   Serial.println("***********************************************************************");
//   Serial.println("");

//   // Send motot off then motor on commande to reset
  
//   //motorOFF(1);
//   motorOFF(3);
//   delay(500);
//   readMotorState(3);
//   Serial.println("");
//   Serial.println("***********************************************************************");
//   Serial.println(" Turn the rotor in its ZERO position they type 'S'");
//   Serial.println("***********************************************************************");
//   nothingReceived = TRUE;
//   while (nothingReceived==TRUE){
//     serialReceivedChar = Serial.read();
//     if(serialReceivedChar == 'S') {
//       nothingReceived = FALSE;
//     }

//     sendVelocityCommand(-150000,1)
//     delay(1000);
//   }

//   //motorON(1);
//   for (int i = 3;i<= 3; i++){
//   motorON(i);
//   readMotorState(i);
//   delay(500);
//   sendVelocityCommand((long int)(0) , (int)i);
//   delay(500);
//   readMotorState(i);
  
//   offsetMotorPosEnconder[i-1] = currentMotorPosEncoder[i-1];
//   }

//   Serial.println("End of Initialization routine.");

//   printingPeriodicity = 10;
// }

// void avance_normal() {
//   // put your main code here, to run repeatedly:
//   unsigned int sleep_time;
 
//   sendVelocityCommand(10000,3);
//   sendVelocityCommand(-10000,2);

//   sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
//   if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
//     delayMicroseconds(sleep_time);
//   }
// }

// void avance_ralenti() {
//   // put your main code here, to run repeatedly:
//   unsigned int sleep_time;
 
//   sendVelocityCommand(2000,3);
//   sendVelocityCommand(-2000,2);

//   sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
//   if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
//     delayMicroseconds(sleep_time);
//   }
// }

// void balise() {
//   int mes_prec = mesure_ref();
//   int mes = mesure();
//   avance_normal();

//   if ((mes_prec - mes >= 1.5) || (mes_prec - mes <= 2)) {
//     avance_ralenti();
//   }

//   if ((mes_prec - mes >= 2.9) || (mes_prec - mes <= 3.1)) {
//     stocker_pos();
//   }

//   mes_prec = mes;
// }

// void stocker_pos() {

// }

// int mesure() {
//   // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
//   digitalWrite(Broche_Trigger_1, LOW); 
//   // On efface l'etat logique de TRIG // 
//   delayMicroseconds(2);   
//   digitalWrite(Broche_Trigger_1, HIGH); // On met la broche TRIG a "1" pendant 10µS //
//   delayMicroseconds(10); 
//   digitalWrite(Broche_Trigger_1, LOW); // On remet la broche TRIG a "0" //  
//   // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
//   Duree = pulseIn(Broche_Echo_1, HIGH);  
//   // Calcul de la distance grace au temps mesure // 
//   Distance_1 = 10*Duree*0.034/2;
//   // Verification si valeur mesuree dans la plage // 
//   if (Distance_1 >= MesureMaxi || Distance_1 <= MesureMini) {     
//     Serial.println("Distance de mesure en dehors de la plage (30 mm à 3 m)"); 
//   }  
//   else {      
//     // Affichage dans le moniteur serie de la distance mesuree //  
//     Serial.print("Distance mesuree 1 :");  
//     Serial.print(Distance_1);  
//     Serial.println("mm"); 
//   }
//   return Distance_1;
// }

// int mesure_ref() {
//   int mes_prec;
//   // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
//   digitalWrite(Broche_Trigger_1, LOW); 
//   // On efface l'etat logique de TRIG // 
//   delayMicroseconds(2);   
//   digitalWrite(Broche_Trigger_1, HIGH); // On met la broche TRIG a "1" pendant 10µS //
//   delayMicroseconds(10); 
//   digitalWrite(Broche_Trigger_1, LOW); // On remet la broche TRIG a "0" //  
//   // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
//   Duree = pulseIn(Broche_Echo_1, HIGH);  
//   // Calcul de la distance grace au temps mesure // 
//   Distance_1 = 10*Duree*0.034/2;
//   // Verification si valeur mesuree dans la plage // 
//   if (Distance_1 >= MesureMaxi || Distance_1 <= MesureMini) {     
//     Serial.println("Distance de mesure en dehors de la plage (30 mm à 3 m)"); 
//   }  
//   else {      
//     // Affichage dans le moniteur serie de la distance mesuree //  
//     Serial.print("Distance mesuree 1 :");  
//     Serial.print(Distance_1);  
//     Serial.println("mm"); 
//   }
//   return Distance_1;
// }