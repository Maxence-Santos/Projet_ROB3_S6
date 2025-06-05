#include "bouton_poussoir.h"
#include <Arduino.h>
const int buttonPin = 3;  
const int ledPin = 13;    

int buttonPushCounter = 0;  
int buttonState = 0;        
int lastButtonState = 0;    

void bouton() {
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } 
    else {
      Serial.println("off");
    }
    delay(50);
    }
lastButtonState = buttonState;
}
