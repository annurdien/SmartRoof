#include <Arduino.h>

// Library untuk DHT
#include <DHT.h>

// Library untuk Limit Switch
#include <ezButton.h>

// PIN DHT Digital
#define DHT_PIN 13
#define DHT_SENSOR_TYPE DHT11

// PIN Limit Switch  Digital
#define LIMIT_SWITCH_PIN_1 15
#define LIMIT_SWITCH_PIN_2 2

// PIN LDR Analog
#define LDR_PIN 34

// PIN Driver Digital,
// digunakan untuk menentukan arah gerakan motor
#define DRIVER_PIN_1 32
#define DRIVER_PIN_2 33

// PIN Driver Analog
#define DRIVER_EN 6

// PIN Raindrop Sensor Analog
#define RAINDROP_PIN 25

// Instansiasi dht
DHT dht_sensor(DHT_PIN, DHT_SENSOR_TYPE);

// Instansiasi LimitSwitch
ezButton limSwitchOpenned(LIMIT_SWITCH_PIN_1);
ezButton limSwitchClosed(LIMIT_SWITCH_PIN_2);

// Return true when it's humid enough for rain to come
boolean isHumid()
{

  // This is the treshold for humidity.
  const int treshold = 65;

  float humid = dht_sensor.readHumidity();

  if (humid > treshold)
  {
    return true;
  }

  return false;
}

// Return true when it's wet
boolean isRain()
{
  const int treshold = 500;

  float sensorValue = analogRead(RAINDROP_PIN);

  if (sensorValue < treshold)
  {
    return true;
  }

  return false;
}

boolean isDark()
{
  const int treshold = 800;

  float sensorValue = analogRead(LDR_PIN);

  if (sensorValue < treshold)
  {
    return true;
  }

  return false;
}

boolean isRoofClosed()
{
  return limSwitchClosed.isPressed();
}

boolean isRoofOpenned()
{
  return limSwitchOpenned.isPressed();
}

void closeRoof()
{

  const int motorSpeed = 500;

  if (isRoofClosed())
  {
    return;
  }

  // Set to clockwise rotation
  digitalWrite(DRIVER_PIN_1, HIGH); // control the motor's direction in clockwise
  digitalWrite(DRIVER_PIN_2, LOW);  // control the motor's direction in clockwise

  analogWrite(DRIVER_EN, motorSpeed);

  while (!isRoofClosed())
  {
  }

  analogWrite(DRIVER_EN, 0);

  return;
}

void openRoof()
{

  const int motorSpeed = 500;

  // Set to counter clockwise rotation
  digitalWrite(DRIVER_PIN_1, LOW);  // control the motor's direction in counter clockwise
  digitalWrite(DRIVER_PIN_2, HIGH); // control the motor's direction in counter clockwise

  if (isRoofOpenned())
  {
    return;
  }

  analogWrite(DRIVER_EN, motorSpeed);

  // Ketika pintu belum tertutup tetap jalankan motor
  while (!isRoofOpenned())
  {
  }

  // Stop the motor
  analogWrite(DRIVER_EN, 0);

  return;
}

void setup()
{
  Serial.begin(115200);

  pinMode(DHT_PIN, INPUT);
  pinMode(LIMIT_SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_PIN_2, INPUT_PULLUP);
  pinMode(DRIVER_PIN_1, OUTPUT);
  pinMode(DRIVER_PIN_2, OUTPUT);
  pinMode(RAINDROP_PIN, INPUT);

  dht_sensor.begin();                   // Initialize DHT sensor
  limSwitchOpenned.setDebounceTime(50); // Set debounce time
  limSwitchClosed.setDebounceTime(50);  // Set debounce time
}

void loop()
{
  // Local Loop
  limSwitchOpenned.loop();
  limSwitchClosed.loop();

  // If it's dark, night for example and no sign of rain, close the roof
  // When it's rain and for sure it's humid, close the roof
  // Otherwise open the roof

  if ((isDark() || isRain()) || (isRain() && isHumid()))
  {
    closeRoof();
  }
  else
  {
    openRoof();
  }
}