#include <Arduino.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
float tens;
void setup() {
  // Initialise la liaison avec le terminal
  Serial.begin(115200);

  // Initialise l'écran LCD
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  lcd.printf("Trieur de balles");

  //verification terminal
  Serial.printf("verification");

  //Initialisation bouton
  #define bp2 12
  #define pot 33

  // Initialisation du port série pour debug
  Serial.begin(115200);
  delay(1000);

  // Configuration de la broche bouton en entrée avec résistance interne pull-up
  pinMode(bp2, INPUT_PULLUP);

  Serial.println();
  
}

void loop() {
  if (digitalRead(bp2) == 0) {
    lcd.setRGB(255, 0, 0);
    lcd.setCursor(0, 1);
    lcd.printf("etat = 1");
    Serial.printf("etat = 1\n");
  } else {
    lcd.setRGB(0, 0, 255);
    lcd.setCursor(0, 1);
    lcd.printf("etat = 0");
    Serial.printf("etat = 0\n");
  }
  tens = analogRead(pot) * 3.3 / 4096;
  lcd.setCursor(10, 1);
  lcd.printf("%.1f V", tens);
  Serial.printf("%.1f V", tens);

  delay(200); // Anti-rebond simple
}
