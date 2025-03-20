#include <Servo.h>

Servo ServoPince;

int pinFeedback = A0;
int pinConsigne = 2;
int pos_ferme = 0;
int pos_pince = 0;
int pos_pince_deg = 0;
int pos_mm = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ServoPince.attach(pinConsigne);
  pos_ferme = analogRead(pinFeedback);
  pos_ferme = map(pos_ferme,97,431,0,180);
  Serial.println(pos_ferme);
}

void loop() {
  Serial.println(180);

  pos_mm = 180 - map(0,0,32,0,180);
  ServoPince.write(pos_mm);
  //pos_pince = analogRead(pinFeedback);
  //pos_pince_deg = map(pos_pince,97,431,0,180);
  delay(1000);

  pos_mm = 180 - map(32,0,32,0,180);
  ServoPince.write(pos_mm);
  delay(1000);
}
