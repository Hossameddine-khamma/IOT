#include <Arduino.h>

const int inputLaser = 33;

void setup()
{
  // put your setup code here, to run once:
  pinMode(inputLaser, OUTPUT);
  digitalWrite(inputLaser, LOW);
}

void loop()
{
  // put your main code here, to run repeatedly:

  // 1
  digitalWrite(inputLaser, HIGH);
  delay(4.7);
  digitalWrite(inputLaser, LOW);
  delay(18.7);

  // 1
  digitalWrite(inputLaser, HIGH);
  delay(4.7);
  digitalWrite(inputLaser, LOW);
  delay(18.7);

  // 0
  delay(18.7);

  // 0
  delay(18.7);

  // 1
  digitalWrite(inputLaser, HIGH);
  delay(4.7);
  digitalWrite(inputLaser, LOW);
  delay(18.7);

  delay(18.7);
}