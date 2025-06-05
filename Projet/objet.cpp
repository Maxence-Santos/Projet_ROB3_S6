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
        if(distance < 100){
            confirmation++;
        }else{
            confirmation = 0;
        }
    }
    Velocityforward(0,LEFT,RIGHT);  //stoper le robot
    delay(1000);
}

void position_zone(){
    //detecte la zone puis se cale devant
    int ref = mesure_ultrason(OBJET);
    sendVelocityCommand(-5000,BRAS);
    readMotorState(BRAS);
    Velocityforward(-6500,LEFT,RIGHT);
    int confirmation = 0;
    int distance = 0;
    while( confirmation < 1){    //leve le bras jusqu'a detectetion du haut de la zone;
        distance = mesure_ultrason(OBJET); 
        if(fabs(distance-ref) > 110){
            confirmation++;
        }else{
            confirmation = 0;
        }
    }
    Velocityforward(0,LEFT,RIGHT);
    delay(1000);
    sendVelocityCommand(5000,BRAS);
    readMotorState(BRAS);
    delay(200);
    sendVelocityCommand(0,BRAS);
    readMotorState(BRAS);
}

void depot_objet(){
    bouger_bras(-10);        //descent le bras de 1cm
    //ouverture_pince(32);    //ouvre le pince
}