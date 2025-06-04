#include "tourner.h"

// ========== PARAMÈTRES PHYSIQUES ==========
const double diametre_roue=50.0; // en mm
const double entraxe=215.0; // en mm
const double MY_PI=3.141592653589793;

const int moteur_gauche_ID=3;
const int moteur_droit_ID=2;

// ========== SENS DES ROTATIONS ==========
const int DIR_LEFT=+1;
const int DIR_RIGHT=-1;

// ========== ÉTAT DU ROBOT ==========
double posX=0.0; // en mm
double posY=0.0; // en mm
double orientation_rad=0.0;

// ========== MÉMOIRES INTERNES ==========
double last_left_deg=0.0;
double last_right_deg=0.0;

double currentMotorPosDeg[3]={0.0, 0.0, 0.0}; 

void updateOdometry() {
  double current_left_deg=currentMotorPosDeg[moteur_gauche_ID - 1];
  double current_right_deg=currentMotorPosDeg[moteur_droit_ID - 1];

  double delta_left_deg=current_left_deg - last_left_deg;
  double delta_right_deg=current_right_deg - last_right_deg;

  last_left_deg=current_left_deg;
  last_right_deg=current_right_deg;

  double mmPerDeg=(diametre_roue * MY_PI)/360.0;
  double deltaLeftMM=DIR_LEFT * delta_left_deg * mmPerDeg;
  double deltaRightMM=DIR_RIGHT * delta_right_deg * mmPerDeg;

  double dS=-(deltaLeftMM+deltaRightMM)/2.0;
  double dTheta=(deltaRightMM-deltaLeftMM)/entraxe;

  double thetaMid=orientation_rad+dTheta * 0.5;
  posX+=dS * cos(thetaMid);
  posY+=dS * sin(thetaMid);
  orientation_rad+=dTheta;
}