#include "objet.h"
#include <Servo.h>
#include "moteur.h"
#include "recherche_balise.h"
#include "pince.h"


void bouger_bras(int valeur){
    // valeur < 0 : baisse le bras de valeur mm, valeur > 0 : monte le bras de valeur mm
    // lever et descendre le bras en reculant ou avancant les roues pour rester droit

}

void objet(){
    //cherche l'objet puis s'arrete à 50mm
    int distance = mesure_ultrason(OBJET);    //distance en mm par rapport à l'objet
    Velocityforward(5000,LEFT,RIGHT);
    int confirmation = 0;
    while( confirmation < 10){     //avancé tant qu'on est à plus de 50mm
        distance = mesure_ultrason(OBJET); 
        if(distance < 60){
            confirmation++;
        }else{
            confirmation = 0;
        }
    }
    
    Velocityforward(0,LEFT,RIGHT);  //stoper le robot
    delay(1000);
}

void position_zone(){

}

void depot_objet(){
    bouger_bras(-10);        //descent le bras de 1cm
    //ouverture_pince(32);    //ouvre le pince
}