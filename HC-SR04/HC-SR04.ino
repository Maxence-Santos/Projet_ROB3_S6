#define Broche_Echo_1 5 // Broche Echo du HC-SR04 sur D7 // 
#define Broche_Trigger_1 4 // Broche Trigger du HC-SR04 sur D8 //
#define Broche_Echo_2 8 // Broche Echo du HC-SR04 sur D7 // 
#define Broche_Trigger_2 10 // Broche Trigger du HC-SR04 sur D8 //


// Definition des variables  
int MesureMaxi = 4000; 
// Distance maxi a mesurer // 
int MesureMini = 30; 
// Distance mini a mesurer //  
long Duree; 
long Distance_1;
long Distance_2;  

void setup() 
{ 
  pinMode(Broche_Trigger_1, OUTPUT); // Broche Trigger en sortie // 
  pinMode(Broche_Echo_1, INPUT); // Broche Echo en entree // 
  pinMode(Broche_Trigger_2, OUTPUT); // Broche Trigger en sortie // 
  pinMode(Broche_Echo_2, INPUT); // Broche Echo en entree // 
  Serial.begin (115200); 
}  

void loop() 
{    
  // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
  digitalWrite(Broche_Trigger_1, LOW); 
  // On efface l'etat logique de TRIG // 
  delayMicroseconds(2);   
  digitalWrite(Broche_Trigger_1, HIGH); // On met la broche TRIG a "1" pendant 10µS //
  delayMicroseconds(10); 
  digitalWrite(Broche_Trigger_1, LOW); // On remet la broche TRIG a "0" //  

  // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
  Duree = pulseIn(Broche_Echo_1, HIGH);  

  // Calcul de la distance grace au temps mesure // 
  Distance_1 = 10*Duree*0.034/2;

  // Verification si valeur mesuree dans la plage // 
  if (Distance_1 >= MesureMaxi || Distance_1 <= MesureMini) {     
    // Si la distance est hors plage, on affiche un message d'erreur //   
    Serial.println("Distance de mesure en dehors de la plage (30 mm à 3 m)"); 
  }  

  else {      
    // Affichage dans le moniteur serie de la distance mesuree //  
    Serial.print("Distance mesuree 1 :");  
    Serial.print(Distance_1);  
    Serial.println("mm"); 
  }

  delay(1000);

  // Debut de la mesure avec un signal de 10 µS applique sur TRIG // 
  digitalWrite(Broche_Trigger_2, LOW); 
  // On efface l'etat logique de TRIG // 
  delayMicroseconds(2);   
  digitalWrite(Broche_Trigger_2, HIGH); // On met la broche TRIG a "1" pendant 10µS //
  delayMicroseconds(10); 
  digitalWrite(Broche_Trigger_2, LOW); // On remet la broche TRIG a "0" //  

  // On mesure combien de temps le niveau logique haut est actif sur ECHO // 
  Duree = pulseIn(Broche_Echo_2, HIGH);  

  // Calcul de la distance grace au temps mesure // 
  Distance_2 = 10*Duree*0.034/2;


  // Verification si valeur mesuree dans la plage // 
  if (Distance_2 >= MesureMaxi || Distance_2 <= MesureMini) {     
    // Si la distance est hors plage, on affiche un message d'erreur //   
    Serial.println("Distance de mesure en dehors de la plage (30 mm à 3 m)"); 
  }  

  else {      
    // Affichage dans le moniteur serie de la distance mesuree //  
    Serial.print("Distance mesuree 2 :");  
    Serial.print(Distance_2);  
    Serial.println("mm"); 
  }  

  delay(100); // On ajoute 0.1 seconde de delais entre chaque mesure // 
} 