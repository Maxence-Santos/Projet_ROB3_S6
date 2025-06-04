#include "compenser.h"
#include "math.h"

const double longueur_bras= 10; // en mm 
double angle_bras_rad = 0.0; // angle  actuelle du bras

void compenser_bras(double nouvel_angle_bras_rad) {
  double deltaX=longueur_bras * (cos(angle_bras_rad)-cos(nouvel_angle_bras_rad));  // calcul de l’ecart horizontal cause par le mouvement du bras
  if (fabs(deltaX) > 1.0) { // eviter trop petit mouvement
     // fonction avancer . Ici on avance de -deltaX
  }
  angle_bras_rad = nouvel_angle_bras_rad;
}