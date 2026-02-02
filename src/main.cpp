#include <Arduino.h>
#include "rgb_lcd.h"
#include <ESP32Servo.h>
#include <ESP32Encoder.h>

void vTaskPeriodic(void *pvParameters);

rgb_lcd lcd;
// Initialisation bouton
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
int *pvParameters;
int consigne;
int k, kp;
int pos1, pos2;
int prop;
// Initialisation PWM
const int pwmPin = 27;        // broche utilisée
const int pwmChannel = 0;     // Canal LEDC
const int pwmFreq = 25000;    // 25 kHz
const int pwmResolution = 11; // 11 bits → valeurs de 0 à 2047

// Initialisation servo
Servo myServo;
int potServo;

void setup()
{
  // Initialise la liaison avec le terminal
  Serial.begin(115200);

  // Initialise l'écran LCD
  Wire1.setPins(15, 5);
  lcd.begin(16, 2, LCD_5x8DOTS, Wire1);
  // lcd.printf("Trieur de balles");

  // verification terminal
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

  // Servo
  /*pinMode(13, OUTPUT);
  myServo.attach(13);*/

  encoder.attachFullQuad(encoderA, encoderB);
  encoder.setCount(0);

  // Asservissement
  //  Création de la tâche périodique
  xTaskCreate(vTaskPeriodic, "vTaskPeriodic", 10000, NULL, 2, NULL);
}

void loop()
{
  /*if (!myServo.attached()) {
    myServo.setPeriodHertz(50); // standard 50 hz servo
    myServo.attach(13, 1000, 2000); // Attach the servo after it has been detatched
  }*/
 
  switch (etat)
  {
  case 0:
    consigne = 0;
    encoder.setCount(0);
    k = analogRead(pot) * 400 / 4096 + 100;
    if (digitalRead(bp0) == 0)
    {
      etat = 1;
    }
    break;
  case 1:
    consigne = 0;
    kp = analogRead(pot) * 100 / 4096 + 40;
    if (digitalRead(bp1) == 0)
    {
      etat = 2;
    }
    if (digitalRead(bp2) == 0)
    {
      etat = 4;
    }
    break;
  case 2:
    consigne = 7;
    if (newPosition > 750)
    {
      etat = 3;
    }
    break;
  case 3:
    consigne = 4;
    if (newPosition < 810)
    {
      etat = 0;
      consigne = 0;
      prop = 0;
      delay(500);
    }
    break;
  case 4:
    consigne = -7;
    if (newPosition > -750)
    {
      etat = 5;
    }
    break;
  case 5:
    consigne = -4;
    if (newPosition > -810)
    {
      etat = 0;
      consigne = 0;
      prop = 0;
      delay(500);
    }
    break;
  }

  
  pos1 = oldPosition - newPosition;
  pos2 = oldPosition - newPosition;
  
  tens = analogRead(pot) * 3.3 / 4095;
  /*lcd.setCursor(10, 1);
  lcd.printf("%.1f V", tens);*/
  lcd.setCursor(0, 0);
  lcd.printf("%d", etat);
  newPosition = encoder.getCount();
  //Serial.printf("%d\n", newPosition);
  //lcd.setCursor(0, 1);
  // Serial.printf("%d\n", analogRead(36));

  // Servo
  // potServo = analogRead(pot);

}

void vTaskPeriodic(void *pvParameters)
{
  int codeur;
  int codeur_Memo;
  int vitesse;
  int erreur;
  int commande;
  TickType_t xLastWakeTime;
  // Lecture du nombre de ticks quand la tâche commence
  xLastWakeTime = xTaskGetTickCount();
  while (1)
  {

    // Endort la tâche pendant le temps restant par rapport au réveil,
    // ici 100ms, donc la tâche s'effectue ici toutes les 100ms.
    // xLastWakeTime sera mis à jour avec le nombre de ticks au prochain
    // réveil de la tâche.
    codeur = encoder.getCount();
    vitesse = codeur - codeur_Memo;
    codeur_Memo = codeur;

    erreur = consigne - vitesse;
    prop += erreur;
    commande = k * erreur + kp * prop;

    if(commande > 2047) {
      commande = 2047;
    } else if(commande < -2047) {
      commande = -2047;
    }

    if(commande > 0) {
      digitalWrite(sens, 1);
      ledcWrite(pwmChannel, commande);
    } else {
      digitalWrite(sens, 0);
      ledcWrite(pwmChannel, -commande);
    }
    lcd.setCursor(0, 1);
    lcd.printf("vit : %2d", vitesse);
    Serial.printf("vit : %2d\n", vitesse);

    lcd.setCursor(9, 1);
    lcd.printf("k : %3d", k);

    lcd.setCursor(6, 0);
    lcd.printf("kp : %3d", kp);

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
  }
}