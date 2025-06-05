#include "recherche_balise.h"
#include "HCSR04.h"
#include "moteur.h"
#include "math.h"
#include <Arduino.h>
#include "Calibration.h"
#include "bouton_poussoir.h"

// Definition des variables  
int MesureMaxi = 4000; 
// Distance maxi a mesurer // 
int MesureMini = 30;

// Distance mini a mesurer //  
long Duree; 
long Distance_1;

void start() {
  // put your setup code here, to run once:
  int i;
  char serialReceivedChar;
  int nothingReceived;

  // Initialization of the serial link
  Serial.begin(115200);

  // Initialization of the analog input pins
  pinMode(Broche_Trigger_1, OUTPUT); // Broche Trigger en sortie // 
  pinMode(Broche_Echo_1, INPUT); // Broche Echo en entree //
  pinMode(Broche_Trigger_2, OUTPUT); // Broche Trigger en sortie // 
  pinMode(Broche_Echo_2, INPUT); // Broche Echo en entree //
  
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
  
  
  motorOFF(1);
  delay(500);
  readMotorState(1);

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
  /*while (nothingReceived==TRUE){
    serialReceivedChar = Serial.read();
    if(serialReceivedChar == 'S') {
      nothingReceived = FALSE;
    }
  }*/

  //motorON(1);
  for (int i = 1;i<= 3; i++){
  motorON(i);
  readMotorState(i);
  delay(500);
  sendVelocityCommand((long int)(0) , (int)i);
  delay(500);
  readMotorState(i);
  
  offsetMotorPosEnconder[i-1] = currentMotorPosEncoder[i-1];
  }

  Serial.println("End of Initialization routine.");

  printingPeriodicity = 10;
}

void avance_normal() {
  // put your main code here, to run repeatedly:
  unsigned int sleep_time;

  for (int i = 2;i<= 3; i++){
    readMotorState(i);
  }
  delay(500);
  sendVelocityCommand(10000,3);
  sendVelocityCommand(-10000,2);
  delay(500);
  for (int i = 2;i<= 3; i++){
    readMotorState(i);  
    offsetMotorPosEnconder[i-1] = currentMotorPosEncoder[i-1];
  }

  sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
  if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
    delayMicroseconds(sleep_time);
  }
}

void avance_ralenti() {
  // put your main code here, to run repeatedly:
  unsigned int sleep_time;

 for (int i = 2;i<= 3; i++){
    readMotorState(i);
  }
  delay(500);
  sendVelocityCommand(2000,3);
  sendVelocityCommand(-2000,2);
  delay(500);
  for (int i = 2;i<= 3; i++){
    readMotorState(i);  
    offsetMotorPosEnconder[i-1] = currentMotorPosEncoder[i-1];
  }

  sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
  if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
    delayMicroseconds(sleep_time);
  }
}

void recule_ralenti() {
  // put your main code here, to run repeatedly:
  unsigned int sleep_time;

 for (int i = 2;i<= 3; i++){
    readMotorState(i);
  }
  delay(500);
  sendVelocityCommand(-2000,3);
  sendVelocityCommand(2000,2);
  delay(500);
  for (int i = 2;i<= 3; i++){
    readMotorState(i);  
    offsetMotorPosEnconder[i-1] = currentMotorPosEncoder[i-1];
  }

  sleep_time = PERIOD_IN_MICROS-((micros()-current_time));
  if ( (sleep_time >0) && (sleep_time < PERIOD_IN_MICROS) ) {
    delayMicroseconds(sleep_time);
  }
}

int mesure_ultrason(int capteur) { //Il faut peut-être inverser 1 et 2 pour Trigger et Echo
  int trigg;
  int echo;
  if (capteur == MUR) {
    trigg = Broche_Trigger_1;
    echo = Broche_Echo_1;
  }

  else {
    trigg = Broche_Trigger_2;
    echo = Broche_Echo_2;
  }
  
  // Definition des variables  
  int MesureMaxi = 4000; 
  // Distance maxi a mesurer // 
  int MesureMini = 30; 
  // Distance mini a mesurer //  
  long Duree; 
  long Distance;
  // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
  digitalWrite(trigg, LOW); 
  // On efface l'etat logique de TRIG // 
  delayMicroseconds(2);   
  digitalWrite(trigg, HIGH); // On met la broche TRIG a "1" pendant 10µS //
  delayMicroseconds(10); 
  digitalWrite(trigg, LOW); // On remet la broche TRIG a "0" //  
  // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
  Duree = pulseIn(echo, HIGH);  
  // Calcul de la distance grace au temps mesure // 
  Distance = 10*Duree*0.034/2;
  // Verification si valeur mesuree dans la plage // 
  // if (Distance >= MesureMaxi || Distance <= MesureMini) {     
  //   Serial.println("Distance de mesure en dehors de la plage (30 mm à 3 m)"); 
  // }  
  // else {      
  //   // Affichage dans le moniteur serie de la distance mesuree //  
  //   Serial.print("Distance mesuree :");  
  //   Serial.print(Distance);  
  //   Serial.println("mm"); 
  // }
  Serial.print("Distance mesuree :");  
  Serial.print(Distance);  
  Serial.println("mm"); 
  return Distance;
}

void balise() {
  int dist_ref = calibre_distance_ref();
  Velocityforward(5000,LEFT,RIGHT);
  delay(1000);
  
  int delta = fabs(dist_ref - mesure_ultrason(MUR));
  int taille_balise = 60;
  int nb_confirmation = 5;

  while( delta < taille_balise){
    int val = mesure_ultrason(MUR);
    Serial.print("distance mur = ");
    Serial.print(val);
    Serial.print(" ref = ");
    Serial.println(dist_ref);
    delta = fabs(dist_ref - val);
    for(int i=0; i<nb_confirmation; i++){
      val = mesure_ultrason(MUR);
      delta = fabs(dist_ref - val);
      Serial.print("confirmation : distance mur = ");
      Serial.print(val);
      Serial.print(" ref = ");
      Serial.println(dist_ref);
      if(delta < taille_balise){
        break;
      }
    }
  }
  Velocityforward(0,LEFT,RIGHT);
}

// void objet() { //Il faut peut-être inverser 1 et 2 pour Trigger et Echo
//   // Definition des variables  
//   int MesureMaxi = 4000; 
//   // Distance maxi a mesurer // 
//   int MesureMini = 30; 
//   // Distance mini a mesurer //  
//   long Duree; 
//   long Distance;
//   long dist_obj_min = 200; //A CHANGER EN FONCTION

//   // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
//   digitalWrite(Broche_Trigger_2, LOW); 
//   // On efface l'etat logique de TRIG // 
//   delayMicroseconds(2);   
//   digitalWrite(Broche_Trigger_2, HIGH); // On met la broche TRIG a "1" pendant 10µS //
//   delayMicroseconds(10); 
//   digitalWrite(Broche_Trigger_2, LOW); // On remet la broche TRIG a "0" //  
//   // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
//   Duree = pulseIn(Broche_Echo_2, HIGH);  
//   // Calcul de la distance grace au temps mesure // 
//   Distance = 10*Duree*0.034/2;
//   // Verification si valeur mesuree dans la plage // 
//   if (Distance > dist_obj_min) {     
//     avance_ralenti();
//   }  
//   else {      
//     sendVelocityCommand(0,3);
//     sendVelocityCommand(0,2);
//   }
// }

int mesure_ref() {
  // Definition des variables  
  int MesureMaxi = 4000; 
  // Distance maxi a mesurer // 
  int MesureMini = 30; 
  // Distance mini a mesurer //  
  long Duree; 
  long Distance;
  int mes_prec;
  // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
  digitalWrite(Broche_Trigger_1, LOW); 
  // On efface l'etat logique de TRIG // 
  delayMicroseconds(2);   
  digitalWrite(Broche_Trigger_1, HIGH); // On met la broche TRIG a "1" pendant 10µS //
  delayMicroseconds(10); 
  digitalWrite(Broche_Trigger_1, LOW); // On remet la broche TRIG a "0" //  
  // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
  Duree = pulseIn(Broche_Echo_1, HIGH);  
  // Calcul de la distance grace au temps mesure // 
  Distance = 10*Duree*0.034/2;
  // Verification si valeur mesuree dans la plage // 
  if (Distance >= MesureMaxi || Distance <= MesureMini) {     
    Serial.println("Distance de mesure en dehors de la plage (30 mm à 3 m)"); 
  }  
  else {      
    // Affichage dans le moniteur serie de la distance mesuree //  
    Serial.print("Distance mesuree :");  
    Serial.print(Distance);  
    Serial.println("mm"); 
  }
  return Distance;
}
