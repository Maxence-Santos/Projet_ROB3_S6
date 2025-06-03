#ifndef RECHERCHE_BALISE_H
#define RECHERCHE_BALISE_H

#include "HCSR04.h"
#include "moteur.h"


#define Broche_Echo_1 5 // Broche Echo du HC-SR04 sur D7 // 
#define Broche_Trigger_1 4 // Broche Trigger du HC-SR04 sur D8 //

void start() ;

void avance_normal() ;

void avance_ralenti() ;

void balise() ;

void stocker_pos();

int mesure();

int mesure_ref();

#endif