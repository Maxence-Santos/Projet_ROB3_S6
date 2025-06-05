#ifndef RECHERCHE_BALISE_H
#define RECHERCHE_BALISE_H

#include "HCSR04.h"
#include "moteur.h"


#define Broche_Echo_1 4 // Broche Echo du HC-SR04 sur D7 // 
#define Broche_Trigger_1 5 // Broche Trigger du HC-SR04 sur D8 //
#define Broche_Echo_2 8 // Broche Echo du HC-SR04 sur D7 // 
#define Broche_Trigger_2 10 // Broche Trigger du HC-SR04 sur D8 //

#define MUR 1
#define OBJET 2

void start() ;

void avance_normal() ;

void avance_ralenti() ;

void recule_ralenti();

void balise();

int mesure_ultrason(int capteur);

int mesure_ref();

#endif
