#include "HCSR04.h"
#include "Servo.h"
#include "Calibration.h"
#include "bouton_poussoir.h"
#include "recherche_balise.h"
#include "tourner.h"
#include "pince.h"
#include "objet.h"
#include "bouton_poussoir.h"


Servo ServoPince;

//_____________PIN I/O pince___________
int pinFeedback_pince = A0;
int pinConsigne_pince = 2;
//____________________________________


//____________PIN BOUTON______________
const int buttonPin = 3;  
const int ledPin = 13;    

//____________________________________



void setup() {
  // initialiser_bouton();
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
   do {
    bouton();
  }while(buttonPushCounter%2 == 0);

  start();  
  Serial.write("S");
  //=========  pince  ===========
  ServoPince.attach(pinConsigne_pince);
  ouverture_pince(32);    //ouvre la pince

  Velocityforward(0,LEFT,RIGHT);

  //==== bras ==========
  sendVelocityCommand(0,BRAS);
  delay(700);
  sendVelocityCommand(0,BRAS);
}

void loop() {
  
  //============ BOUTON ============================
  
  
  //================================================

  //============ code test rotation ================
  // delay(5000);
  // tourner_experimentalement(7500*2,-1);    //7500 = 90°
  //================================================

  //=========== code test avancer/reculer ==========
  // delay(5000);
  // avance_x_increment(20000,1);    // 10000 = 2.5cm
  // delay(5000);
  // avance_x_increment(20000,-1);    // 10000 = 2.5cm
  //================================================

  //=========== Recherche de balise ================
  // Serial.println("boucle");
  // delay(4000);
  // Serial.println("je cherche la balise");
  // balise();
  // delay(2000);
  // tourner_experimentalement(7500,-1);
  // Serial.println("fin");
  //================================================

  //=========== distance reference =================
  // Serial.println("boucle");
  // delay(4000);
  // int dist = calibre_distance_ref();
  // Serial.print("dist = ");
  // Serial.println(dist);
  //===============================================


  //__________1ere Ligne___________________________
  delay(4000);
  Serial.println("je cherche la balise");
  balise();
  delay(2000);
  avance_x_mm(60,-1);
  delay(2000);
  tourner_experimentalement(7430,-1);
  Serial.println("fin de la 1ere etape");
  //__________2eme Ligne___________________________
  Serial.println("je cherche l'objet");
  delay(2000);
  objet();    //cherche l'objet puis se stop
  delay(1000);
  position_zone();    //se cale devant la zone de prise
  //avance experimentalement========
  Velocityforward(5000,LEFT,RIGHT);
  delay(2500);
  Velocityforward(0,LEFT,RIGHT);
  // //================================  delay(1000);
  ouverture_pince(9);     //prend l'objet
  delay(2000);
  sendVelocityCommand(-50000,BRAS);  //leve le bras
  readMotorState(BRAS);
  delay(2000);
  // sendVelocityCommand(0,BRAS);
  Serial.println("objet attrappé et lever");
  delay(5000);
  tourner_experimentalement(7430,-1);
  Serial.println("fin de la 2eme etape");
  //__________3eme Ligne___________________________
  delay(2000);
  Serial.println("je cherche la balise");
  balise();
  delay(2000);
  // avance_x_mm(50,1);
  delay(2000);
  tourner_experimentalement(7430,-1);
  Serial.println("fin de la 3eme etape");
  //__________4eme Ligne___________________________
  Serial.println("Je cherche la zone de depot");
  balise();
  Serial.println("zone de depot trouvé");
  delay(1000);
  //recule experimentalement========
  Velocityforward(-5000,LEFT,RIGHT);
  delay(10000);
  Velocityforward(0,LEFT,RIGHT);
  //================================
  delay(1000);
  sendVelocityCommand(5000,BRAS);  //baisse le bras
  delay(2000);
  sendVelocityCommand(0,BRAS);
  delay(500);
  ouverture_pince(32);     //lache l'objet
  Serial.println("fin de la 4eme etape");
  delay(2000);


  // _______________fin___________________________
  //recule + figure ========
  Velocityforward(-5000,LEFT,RIGHT);
  delay(10000);
  // sendVelocityCommand(30000,RIGHT);
  // sendVelocityCommand(30000,LEFT);
  // delay(5000);
  Velocityforward(0,LEFT,RIGHT);
  //==========
  delay(5000);
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
  int pos = analogRead(pinFeedback_pince);
  pos = map(pos,97,431,32,0);
  return pos;
}