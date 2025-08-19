#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C* lcd = nullptr;
byte foundAddr = 0;

// Pins
const int ledPin = 7;
const int trigPin = 8;
const int echoPin = 9;

int distance = 0;

// Scan I2C
byte scanI2C() {
  byte addrFound = 0;
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      addrFound = addr;
      break;
    }
  }
  return addrFound;
}

// Measure distance
int measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  foundAddr = scanI2C();
  lcd = new LiquidCrystal_I2C(foundAddr, 16, 2);
  lcd->init();
  lcd->begin(16, 2);
  lcd->backlight();

  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("Smart Drainage");
  lcd->setCursor(0, 1);
  lcd->print("Initializing...");
  delay(2000);
}

void loop() {
  distance = measureDistance();

  lcd->clear();
  lcd->setCursor(0, 0);
  lcd->print("Dist: ");
  lcd->print(distance);
  lcd->print(" cm");

  bool motorOn = (distance < 200 && distance > 0);

  if (motorOn) {
    digitalWrite(ledPin, HIGH);
    lcd->setCursor(0, 1);
    lcd->print("Motor: ON ");
  } else {
    digitalWrite(ledPin, LOW);
    lcd->setCursor(0, 1);
    lcd->print("Motor: OFF");
  }

  
 // Send ONE clean JSON line every loop:
Serial.print("{\"distance\":");
Serial.print(distance);
Serial.print(",\"motor\":\"");
Serial.print((distance > 0 && distance < 200) ? "ON" : "OFF");
Serial.println("\"}");

  

  delay(500);
}
