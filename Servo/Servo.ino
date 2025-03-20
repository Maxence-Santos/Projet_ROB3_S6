#include <Servo.h>

Servo ServoPince;

// PIN I/O
int pinFeedback = A0;
int pinConsigne = 2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  ServoPince.attach(pinConsigne);
}

void loop() {
  Serial.println("ouverture (de 16mm)");
  ouverture_pince(32);
  int pos_feedback = read_pos();
  Serial.println(pos_feedback);
  delay(1000);
}

void ouverture_pince(int ouv_mm){
  //Ouvre la pince au mm souhaité
  //Sature la variable ouv_mm si elle est <0 ou >32
  if (ouv_mm > 32){
    ouv_mm = 32;
  }
  if (ouv_mm < 0){
    ouv_mm = 0;
  }
  int pos_deg = 180 - map(ouv_mm,0,32,0,180);
  ServoPince.write(pos_deg);
}

int read_pos(){
  //retourne la valeur en mm de l'ouverture de la pince (0 fermé, 32 ouvert)
  int pos = analogRead(pinFeedback);
  pos = map(pos,97,431,32,0);
  return pos;
}
