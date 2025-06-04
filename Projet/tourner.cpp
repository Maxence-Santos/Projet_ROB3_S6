#include "tourner.h"
#include "odometrie.h"

const double ROTATION_SPEED=10.0; 
const double MY_PI=3.141592653589793;
const int moteur_gauche_ID=3;
const int moteur_droit_ID=2;

void tourner_angle(double angle_rad){
  double orientation_initiale=orientation_rad;
  if(angle_rad>0){
    setMotorSpeed(moteur_gauche_ID, -ROTATION_SPEED); // tourner a gauche
    setMotorSpeed(moteur_droit_ID, ROTATION_SPEED);
  } 
  else{
    setMotorSpeed(moteur_gauche_ID, ROTATION_SPEED);  // tourner a droite
    setMotorSpeed(moteur_droit_ID, -ROTATION_SPEED);
  }

  while(true){
    updateOdometry();
    double deltaTheta=orientation_rad - orientation_initiale;
    if(fabs(deltaTheta)>=fabs(angle_rad)){
      break;
    }
    delay(10); 
  }

  setMotorSpeed(moteur_gauche_ID, 0);
  setMotorSpeed(moteur_droit_ID, 0);
}