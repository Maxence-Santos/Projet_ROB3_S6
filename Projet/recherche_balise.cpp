#include "recherche_balise.h"
#include "HCSR04.h"
#include "moteur.h"

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
  pinMode(analogPinP0, INPUT);
  pinMode(analogPinP1, INPUT);
  pinMode(Broche_Trigger_1, OUTPUT); // Broche Trigger en sortie // 
  pinMode(Broche_Echo_1, INPUT); // Broche Echo en entree // 
  
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

    sendVelocityCommand(-150000,1);
    delay(1000);
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

int mesure_balise() { //Il faut peut-être inverser 1 et 2 pour Trigger et Echo
  // Definition des variables  
  int MesureMaxi = 4000; 
  // Distance maxi a mesurer // 
  int MesureMini = 30; 
  // Distance mini a mesurer //  
  long Duree; 
  long Distance;
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

void balise(int mes_prec) {
  float pos_md;
  float pos_mg;
  float nbre_tours;
  int mes = mesure_balise();
  bool flag = 0;
  avance_normal();

  if ((mes_prec - mes >= 15) || (mes_prec - mes <= 20)) {
    avance_ralenti();
    if ((mes_prec - mes >= 29) || (mes_prec - mes <= 31)) {
      if (!flag) { // Stockage des positions
        pos_md = currentMotorPosDeg[3]; // ou 2
        pos_mg = currentMotorPosDeg[2]; // ou 3
        nbre_tours = currentNumOfMotorRevol[3];
        flag = 1;
      }
    }
    if ((mes_prec - mes >= 25) || (mes_prec - mes <=29)) {
      nbre_tours = currentNumOfMotorRevol[3] - nbre_tours;
      while (currentMotorPosDeg[3] != pos_md && currentMotorPosDeg[2] != pos_mg && nbre_tours/2 != 0) { //Il faut peut-être inverser 2 et 3
        recule_ralenti();
        if (currentMotorPosDeg[3] == 2*MY_PI) { //peut-être -2*pi
          nbre_tours --;
        }
      }
      long current_pos = currentMotorPosDeg[3];
      while (current_pos != -currentMotorPosDeg[3] + MY_PI/2) {
        sendVelocityCommand(-500,3);
        sendVelocityCommand(-500,3);
      }
    }
  }
  mes_prec = mes;
}

void objet() { //Il faut peut-être inverser 1 et 2 pour Trigger et Echo
  // Definition des variables  
  int MesureMaxi = 4000; 
  // Distance maxi a mesurer // 
  int MesureMini = 30; 
  // Distance mini a mesurer //  
  long Duree; 
  long Distance;
  long dist_obj_min = 200; //A CHANGER EN FONCTION

  // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
  digitalWrite(Broche_Trigger_2, LOW); 
  // On efface l'etat logique de TRIG // 
  delayMicroseconds(2);   
  digitalWrite(Broche_Trigger_2, HIGH); // On met la broche TRIG a "1" pendant 10µS //
  delayMicroseconds(10); 
  digitalWrite(Broche_Trigger_2, LOW); // On remet la broche TRIG a "0" //  
  // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
  Duree = pulseIn(Broche_Echo_2, HIGH);  
  // Calcul de la distance grace au temps mesure // 
  Distance = 10*Duree*0.034/2;
  // Verification si valeur mesuree dans la plage // 
  if (Distance > dist_obj_min) {     
    avance_ralenti();
  }  
  else {      
    sendVelocityCommand(0,3);
    sendVelocityCommand(0,2);
  }
}

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
