//#include "HCSR04.h"
#include <Ultrasonic.h>

#define TRIG_PIN_D 10
#define ECHO_PIN_D 8
#define TRIG_PIN_C 4
#define ECHO_PIN_C 5


Ultrasonic ultra(TRIG_PIN_D,ECHO_PIN_D);
Ultrasonic ultra_2(TRIG_PIN_C,ECHO_PIN_C);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
 // HCSR04.begin(TRIG_PIN_D,ECHO_PIN_D);
  //HCSR04.begin(TRIG_PIN_C,ECHO_PIN_C);
}

void loop() {
  // put your main code here, to run repeatedly:
  //c =measureDistanceMm();
  Serial.print("C : ");
  Serial.print(ultra.read());
  Serial.println("mm");
  delay(1000);
  //d=measureDistanceMm();
  Serial.print("D : ");
  Serial.print(ultra_2.read());
  Serial.println("mm");
  delay(1000);
}
