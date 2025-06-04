#include "tourner.h"
#include "odometrie.h"
#include "math.h"
#include "moteur.h"

const double ROTATION_SPEED=10.0;

void tourner_angle(double angle_rad){
  double orientation_initiale=orientation_rad;
  if(angle_rad>0){
    sendVelocityCommand(-ROTATION_SPEED,LEFT); // tourner a gauche
    sendVelocityCommand(ROTATION_SPEED,RIGHT);
  } 
  else{
    sendVelocityCommand(ROTATION_SPEED,LEFT); // tourner a gauche
    sendVelocityCommand(-ROTATION_SPEED,RIGHT);
  }

  while(true){
    updateOdometry();
    double deltaTheta=orientation_rad - orientation_initiale;
    if(fabs(deltaTheta)>=fabs(angle_rad)){
      break;
    }
    delay(10); 
  }

  sendVelocityCommand(0,LEFT); // tourner a gauche
  sendVelocityCommand(0,RIGHT);
}