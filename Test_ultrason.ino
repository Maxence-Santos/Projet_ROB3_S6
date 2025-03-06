#include "HCSR04.h"
#define TRIG_PIN 13
#define ECHO_PIN 8
double* a;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HCSR04.begin(TRIG_PIN,ECHO_PIN);
}

void loop() {
  // put your main code here, to run repeatedly:
  a=HCSR04.measureDistanceMm();
  Serial.print(a[0]);
  Serial.println("mm");
}
