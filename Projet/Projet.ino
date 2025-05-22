#include "HCSR04.h"
#include "Servo.h"
#include "Calibration.h"
#include "bouton_poussoir.h"

const int buttonPin = 2;  
const int ledPin = 13;    

int buttonPushCounter = 0;  
int buttonState = 0;        
int lastButtonState = 0;  

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  bouton_poussoir();
  start();
  calibration();
  recherche_balise();
}
