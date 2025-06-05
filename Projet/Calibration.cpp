// les valeurs sont à tester et changer

#include "Calibration.h"
#include "HCSR04.h"
#include "moteur.h"
#include "recherche_balise.h"
#include <Arduino.h>

//se calibre droit et retourne la position du robot par rapport au mur
double calibration() {
    int pos_mur;//position du robot par rapport au mur
    int pos_mur_res; //position du robot par rapport au mur une fois calibré
    int delta; // variation de l'ecart du robot par rapport au mur
    double previous_pos_mur;
    double delta_mur;   //delta entre l'ancienne et l'actuel ecart au mur
    float temperature = 22.0;
    float gain_correcteur = 500;
    
    int motor_vel = 5000;  //vitesse des moteurs pour la calibration

    int cpt_cycle = 0;  //compteur de cycle réussit
    int cpt_cycle_tot = 0;

    int corrige_vel[3] = {0,0,0};
    //======== INITIALISATION ========
    int initial_pos_left = currentMotorPosEncoder[LEFT-1];
    int initial_pos_right = currentMotorPosEncoder[RIGHT-1];
    previous_pos_mur = mesure_ultrason(MUR);

    //========= ALLER (alignement) ===========
    while(cpt_cycle < 5){
        sendVelocityCommand(motor_vel+corrige_vel[LEFT-1],LEFT);
        sendVelocityCommand(-(motor_vel+corrige_vel[RIGHT-1]),RIGHT);
        readMotorState(LEFT);
        readMotorState(RIGHT);
        delay(100);
        //faire une pose pour laisser avancer
        pos_mur = mesure_ultrason(MUR);
        delta = pos_mur - previous_pos_mur;
        corrige_vel[LEFT-1] = 0;
        corrige_vel[RIGHT-1] = 0;

        if( delta > 0){
            if(delta > 7){   
                cpt_cycle = 0;
                corrige_vel[RIGHT-1] = delta * gain_correcteur;
                previous_pos_mur = pos_mur;
            }else{ // sinon : on fait rien, ecart trop faible (on garde l'ancienne valeur pour etre sur)
                cpt_cycle++;
            }
        }else{
            if(delta < -7){   
                cpt_cycle = 0;
                corrige_vel[LEFT-1] = -delta * gain_correcteur;
                previous_pos_mur = pos_mur;
            }else{// sinon : on fait rien, ecart trop faible (on garde l'ancienne valeur pour etre sur)
                cpt_cycle++;
            }
        }
    }
    //========= RETOUR (mesure mur) ==========
    while(cpt_cycle_tot != 0){
        int tot_cycle = cpt_cycle_tot;
        sendVelocityCommand(-motor_vel,LEFT);
        sendVelocityCommand(motor_vel,RIGHT);
        delay(500);
        pos_mur = mesure_ultrason(MUR);
        pos_mur_res += pos_mur/tot_cycle;   //moyenne des écarts
        cpt_cycle_tot-=1;
    }

    return pos_mur_res;
}

int calibre_distance_ref(){
    int nb_mesure = 100;
    int nb_tot = nb_mesure;
    long unsigned int tot = 0;
    int mesure = 0;
    //======= aller ==============
    Velocityforward(5000,LEFT,RIGHT);
    for(int i=0; i < nb_mesure/2; i++){
        mesure = mesure_ultrason(MUR);
        if(mesure > 700 || mesure < 200){
            nb_tot = nb_tot-1;
        }else{
            tot = tot + mesure;
        }
        delay(100);
    }
    //======= retour =============
    Velocityforward(-5000,LEFT,RIGHT);
    for(int i=0; i < nb_mesure/2; i++){
        mesure = mesure_ultrason(MUR);
        if(mesure > 700 || mesure < 200){
            nb_tot = nb_tot-1;
        }else{
            tot = tot + mesure;
        }
        delay(100);
    }
    return tot/nb_tot;
}