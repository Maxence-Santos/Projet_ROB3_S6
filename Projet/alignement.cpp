#include "alignement.h"
#include "odometrie.h"
#include "tourner.h"
#include "math.h"
#include <Arduino.h>

// void alignement() {
//   const double tolerence=1;  // tolerance d'alignement en mm
//   const double distance_avance=30;    // distance en mm parcouru avant de prendre la deuxieme mesure 
//   const double rotation_rad = 0.05;   // angle pour corriger l'orientation

//   float d1=10; // premiere mesure ( il faut mettre la fonction pour mesure la distance entre le capteur et le mur )
//   // il faut mettre la fonction avancer et lui dire d'avance de distance_avance
//   delay(200);
//   updateOdometry();
//   float d2=2; // deuxieme mesure ( il faut mettre la fonction pour mesure la distance entre le capteur et le mur )
//   float diff=d2-d1;
//   if(fabs(diff)<tolerence){
//     return;
//   }
//   if(diff>0){
//     tourner_angle(rotation_rad);
//   } 
//   else{
//     tourner_angle(rotation_rad);
//   }
//   alignement(); 
// }