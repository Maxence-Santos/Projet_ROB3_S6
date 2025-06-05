#include "bouton_poussoir.h"
#include <Arduino.h>

const int buttonPin = 1;
bool running = false;
int last_button_state = LOW;
unsigned long last_debounce_time = 0;
const unsigned long debounce_delay = 50;

void initialiser_bouton() {
  pinMode(buttonPin, INPUT);
}

void attendre_depart() {
  Serial.println("Appuyez sur le bouton pour démarrer...");

  while (!running) {
    gerer_bouton(); // On surveille les appuis pendant l'attente
  }

  Serial.println("S");
}

void gerer_bouton() {
  int reading = digitalRead(buttonPin);

  if (reading != last_button_state) {
    last_debounce_time = millis();
  }

  if ((millis() - last_debounce_time) > debounce_delay) {
    if (reading == HIGH && last_button_state == LOW) {
      running = true;
    }
  }

  last_button_state = reading;
}

bool est_en_marche() {
  return running;
}
