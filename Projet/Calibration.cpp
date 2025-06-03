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
    float gain_correcteur = 0.1;
    
    int motor_vel = 100;  //vitesse des moteurs pour la calibration
    int corrige_vel[3] = {0,0,0}; //vitesse ajouter a motor_vel pour corriger l'ecart

    int cpt_cycle = 0;  //compteur de cycle réussit
    int cpt_cycle_tot = 0;


    //======== INITIALISATION ========
    int initial_pos_left = currentMotorPosEncoder[LEFT];
    int initial_pos_right = currentMotorPosEncoder[RIGHT];
    previous_pos_mur = mesure();

    //========= ALLER (alignement) ===========
    while(cpt_cycle < 5){
        sendVelocityCommand(motor_vel+corrige_vel[LEFT],LEFT);
        sendVelocityCommand(motor_vel+corrige_vel[RIGHT],RIGHT);
        //faire une pose pour laisser avancer
        pos_mur = mesure();
        delta = pos_mur - previous_pos_mur;
        corrige_vel[LEFT] = 0;
        corrige_vel[RIGHT] = 0;

        if( delta > 0){
            if(delta > 0.01){   
                cpt_cycle = 0;
                corrige_vel[RIGHT] = delta * gain_correcteur;
                previous_pos_mur = pos_mur;
            }else{ // sinon : on fait rien, ecart trop faible (on garde l'ancienne valeur pour etre sur)
                cpt_cycle++;
            }
        }else{
            if(delta < -0.01){   
                cpt_cycle = 0;
                corrige_vel[LEFT] = delta * gain_correcteur;
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
        sendVelocityCommand(-motor_vel,RIGHT);
        //faire une pose pour laisser avancer
        pos_mur = mesure();
        pos_mur_res += pos_mur/tot_cycle;   //moyenne des écarts
        cpt_cycle_tot-=1;
    }

    return pos_mur_res;
}