#include "HCSR04.h"
#include <Servo.h>
#include "Calibration.h"
#include "bouton_poussoir.h"

Servo ServoPince;

//_____________PIN I/O pince___________
int pinFeedback_pince = A0;
int pinConsigne_pince = 3;
//____________________________________


//____________PIN BOUTON______________
const int buttonPin = 2;  
const int ledPin = 13;    

int buttonPushCounter = 0;  
int buttonState = 0;        
int lastButtonState = 0;  
//____________________________________



void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  start();
  //pince
  ServoPince.attach(pinConsigne_pince);
}

void loop() {
  // put your main code here, to run repeatedly:
  //bouton_poussoir();
  //calibration();      //Manu
  //int mes_prec = mesure_ref();
  //balise(mes_prec);
  // objet()            //Manu et Mathis(lever_pince())
  // recherch_balise() //Maxence
  // tourne_90        /Mathis
  // recherche_balise() //Maxence
  // rouler(x cm)     // Maxence
  // depot_objet      // Manu
}
