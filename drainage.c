#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD (I2C) address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pins
const int trigPin = 18;
const int echoPin = 19;
const int relayPin = 23;  // LED pin

// Measurement variables
unsigned long duration_us = 0;
float distanceCm = -1.0;
const float blinkThresholdCm = 200.0; // LED blinks below this
bool ledState = false;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);

  digitalWrite(trigPin, LOW);
  digitalWrite(relayPin, LOW);

  Serial.begin(115200);
  delay(10);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smart Drainage");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1200);
  lcd.clear();
}

float measureDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration_us = pulseIn(echoPin, HIGH, 30000);
  if (duration_us == 0) {
    return -1.0; // No echo
  }
  return (duration_us / 2.0) / 29.1;
}

void updateLCD(float d, bool motorOn) {
  lcd.setCursor(0, 0);
  if (d > 0) {
    char line1[17];
    snprintf(line1, sizeof(line1), "Dist: %5.1f cm", d);
    lcd.print(line1);
  } else {
    lcd.print("Dist:   No Echo ");
  }

  lcd.setCursor(0, 1);
  if (motorOn) {
    lcd.print("Motor: ON         ");
  } else {
    lcd.print("Motor: OFF        ");
  }
}

void loop() {
  distanceCm = measureDistanceCM();

  if (distanceCm > 0) {
    Serial.print("Distance: ");
    Serial.print(distanceCm, 1);
    Serial.println(" cm");
  } else {
    Serial.println("No echo detected");
  }

  // Blink LED if below threshold
  if (distanceCm > 0 && distanceCm <= blinkThresholdCm) {
    ledState = !ledState;
    digitalWrite(relayPin, ledState ? HIGH : LOW);
    updateLCD(distanceCm, true);  // Show Motor: ON while blinking
    delay(300); // Blink speed
  } else {
    digitalWrite(relayPin, LOW);
    updateLCD(distanceCm, false); // Show Motor: OFF
    delay(500); // Normal update speed
  }
}
