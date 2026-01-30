#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Servo.h>
#include <ESP32Encoder.h>

rgb_lcd lcd;
 //Initialisation bouton
  #define bp0 0
  #define bp1 2
  #define bp2 12
  #define pot 33
  #define sens 26
  #define encoderA 23
  #define encoderB 19
  #define IR0 36
  ESP32Encoder encoder;
int etat = 0;
float tens;
float duty;
long newPosition;
long oldPosition;
int inc = 0;
  //Initialisation PWM
const int pwmPin = 27; // broche utilisée
const int pwmChannel = 0; // Canal LEDC 
const int pwmFreq = 25000; // 25 kHz 
const int pwmResolution = 11; // 11 bits → valeurs de 0 à 2047

//Initialisation servo
Servo myServo;
int potServo;

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
  delay(1000);

  // Configuration de la broche bouton en entrée avec résistance interne pull-up
  pinMode(bp0, INPUT_PULLUP);
  pinMode(bp1, INPUT_PULLUP);
  pinMode(bp2, INPUT_PULLUP);

  pinMode(sens, OUTPUT);

  Serial.println();


    // Configuration du PWM 
    ledcSetup(pwmChannel, pwmFreq, pwmResolution); 
    
    // Attache la broche au canal PWM 
    ledcAttachPin(pwmPin, pwmChannel); 
    
    // Duty cycle = 0,25 → 25 % 
    
  pinMode(13, OUTPUT);
  myServo.attach(13);

  encoder.attachFullQuad ( encoderA, encoderB );
  encoder.setCount ( 0 );
}

void loop() {
  if (!myServo.attached()) {
		myServo.setPeriodHertz(50); // standard 50 hz servo
		myServo.attach(13, 1000, 2000); // Attach the servo after it has been detatched
	}
  switch(etat) {
    case 0:
      duty = 0;
      if(digitalRead(bp0) == 0) { etat = 1; }
      if(digitalRead(bp1) == 0) { etat = 2; }
      break;
    case 1:
    myServo.write(-70);
      digitalWrite(sens, 1);
      duty = analogRead(pot) * 0.99 / 4096 * ((1 << pwmResolution) - 1); 
      if(digitalRead(bp2) == 0) { etat = 0; }
      if(newPosition > 180) { etat = 3; }
      break;
    case 2:
      digitalWrite(sens, 1);
      duty = analogRead(pot) * 0.99 / 4096 * ((1 << pwmResolution) - 1); 
      if(digitalRead(bp2) == 0) { etat = 0; }
      if(newPosition > 280) { etat = 3; }
      break;
    case 3:
      digitalWrite(sens, 1);
      duty = analogRead(pot) * 0.99 / 4096 * ((1 << pwmResolution) - 1);
      if(digitalRead(bp2) == 0) { etat = 0; }
      if(newPosition > oldPosition) { etat = 4; }
      if(newPosition > 860) { etat = 4; }
      break;
    case 4:
      duty = 0;
      ledcWrite(pwmChannel, duty);
      delay(1000);
      oldPosition += 100;
      if(newPosition > 850) { etat = 7, oldPosition = 830; }
      else { etat = 3; }

      break;
    case 5:
      digitalWrite(sens, 0);
      duty = analogRead(pot) * 0.99 / 4096 * ((1 << pwmResolution) - 1);
      if(newPosition < oldPosition) { etat = 6; }
      if(newPosition < 120) { etat = 6; }
      break;
    case 6:
      duty = 0;
      ledcWrite(pwmChannel, duty);
      if(newPosition < 120) { etat = 0; }
      delay(1000);
      oldPosition -= 100;
      etat = 5;
      break;
    case 7:
      if(digitalRead(bp0) == 0) { myServo.write(72); }
      if(digitalRead(bp1) == 0) { analogWrite(13, -50); }
      if(digitalRead(bp2) == 0) { analogWrite(13, 0); }
      break;

  }
  ledcWrite(pwmChannel, duty);
  
  tens = analogRead(pot) * 3.3 / 4095;
  lcd.setCursor(10, 1);
  lcd.printf("%.1f V", tens);
  lcd.setCursor(0, 0);
  lcd.printf("%d", etat);
  newPosition = encoder.getCount();
  Serial.printf("%d\n", newPosition);
  lcd.setCursor(0, 1);
  //Serial.printf("%d\n", analogRead(36));
 
  
  potServo = analogRead(pot);
  
  
  

}