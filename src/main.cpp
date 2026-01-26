#include <Arduino.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
 //Initialisation bouton
  #define bp0 0
  #define bp1 2
  #define bp2 12
  #define pot 33
  #define sens 26
int etat = 0;
float tens;
float duty;
  //Initialisation PWM
const int pwmPin = 27; // broche utilisée
const int pwmChannel = 0; // Canal LEDC 
const int pwmFreq = 25000; // 25 kHz 
const int pwmResolution = 11; // 11 bits → valeurs de 0 à 2047
void setup() {
  // Initialise la liaison avec le terminal
  Serial.begin(115200);

  // Initialise l'écran LCD
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  //lcd.printf("Trieur de balles");

  //verification terminal
  Serial.printf("verification");

 

  // Initialisation du port série pour debug
  Serial.begin(115200);
  delay(1000);

  // Configuration de la broche bouton en entrée avec résistance interne pull-up
  pinMode(bp2, INPUT_PULLUP);

  Serial.println();


    // Configuration du PWM 
    ledcSetup(pwmChannel, pwmFreq, pwmResolution); 
    
    // Attache la broche au canal PWM 
    ledcAttachPin(pwmPin, pwmChannel); 
    
    // Duty cycle = 0,25 → 25 % 
    

}

void loop() {
  /*if (digitalRead(bp2) == 0) {
    lcd.setRGB(255, 0, 0);
    lcd.setCursor(0, 1);
    lcd.printf("etat = 1");
    Serial.printf("etat = 1\n");
  } else {
    lcd.setRGB(0, 0, 255);
    lcd.setCursor(0, 1);
    lcd.printf("etat = 0");
    Serial.printf("etat = 0\n");
  }*/
  
  switch(etat) {
    case 0:
      lcd.clear();
      duty = 0;
      lcd.printf("stop");
      if((digitalRead(bp0)) == 0 || (digitalRead(bp1)) == 0) { etat = 1; }
      break;
    case 1:
      lcd.clear();
      digitalWrite(sens, 1);
      duty = analogRead(pot) * 0.99 / 4096 * ((1 << pwmResolution) - 1); 
      lcd.printf("horaire");
      if(digitalRead(bp2) == 0) { etat = 0; }
      if(digitalRead(bp1) == 0) { etat = 2; }
      break;
    case 2:
      lcd.clear();
      digitalWrite(sens, 0);
      lcd.printf("anti-horaire");
      if(digitalRead(bp1) != 0) { etat = 1; }
      if(digitalRead(bp2) == 0) { etat = 0; }
      break;
  }
  ledcWrite(pwmChannel, duty);
  
  tens = analogRead(pot) * 3.3 / 4096;
  lcd.setCursor(10, 1);
  lcd.printf("%.1f V", tens);
  Serial.printf("%.1f V", tens);
  lcd.setCursor(0, 0);
 
  delay(200); // Anti-rebond simple
}