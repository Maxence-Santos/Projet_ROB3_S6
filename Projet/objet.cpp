#include "objet.h"
#include "Servo.h"
#include "moteur.h"
#include "recherche_balise.h"


void bouger_bras(int valeur){
    // valeur < 0 : baisse le bras de valeur mm, valeur > 0 : monte le bras de valeur mm
    // lever et descendre le bras en reculant ou avancant les roues pour rester droit

}

void objet(){
    int distance = mesure();    //distance en mm par rapport à l'objet
    while( distance > 50 ){     //avancé tant qu'on est à plus de 50mm
        Velocityforward(VEL_NORMAL,LEFT,RIGHT);
    }
    
    Velocityforward(0,LEFT,RIGHT);  //stoper le robot

    while( mesure() > distance+5){  //lever le bras tant que le capteur n'est pas detecter la zone de prise
        bouger_bras(5);
    }

    bouger_bras(15);    //leve le bras pour aligner la pince avec la zone de prise
    avance_x_mm(distance);  //avance le robot sur la zone
    ouverture_pince(0);     //prend l'objet
    bouger_bras(10);        //souleve l'objet de 10mm
}


void depot_objet(){
    bouger_bras(-10);        //descent le bras de 1cm
    ouverture_pince(32);    //ouvre le pince
}