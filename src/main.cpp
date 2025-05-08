#include <Arduino.h>
#include <SHT2x.h>
#include <time.h>
#include <functions.h>
#include <LiquidCrystal_I2C.h>

#define SDA 22
#define SCL 21

#define BACKLIGHT 100
#define CONTRAST 255

#define HEATING_PIN 13
#define HUM_PIN 14
#define FAN_PIN 26
#define SET_BUTTON 17

#define READING_FREQ 100

#define TEMP_HYST 0.5
#define HUM_HYST 3
#define TEMP 32
#define HUM 80

long lastRead = 0;

c_sensor sensor;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
    pinMode(SET_BUTTON, INPUT_PULLUP);
    pinMode(HEATING_PIN, OUTPUT);
    pinMode(HUM_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    Wire.begin();
    Wire.setPins(SDA, SCL);
    lcd.init();
    lcd.setBacklight(BACKLIGHT);
    lcd.setContrast(CONTRAST);
}

void loop() {
  if (millis() > lastRead + READING_FREQ) {
    sensorReading reading = sensor.readSensor();
    Serial.println(reading.hum);
    Serial.println(reading.temp);
    manageTemp(reading.hum, TEMP, TEMP_HYST);
    manageHUM(reading.hum, HUM, HUM_HYST);
    displayParams(reading.temp, reading.hum, 1);
    lastRead = millis();
    Serial.println("loop zoop");
  }
}

////////////////////////////////////////////////////////////////////////FUNCTION DEFINITIONS/////////////////////////////////////////////////////////////

void manageTemp(float temperature, float target, float hyst) {
  if (temperature > target + hyst) {
    digitalWrite(HEATING_PIN, HIGH); //heating off
  }
  if (temperature < target - hyst) {
    digitalWrite(HEATING_PIN, LOW); //heating on
  }
}

void manageHUM(float humidity, float target, float hyst) {
  if (humidity < target - hyst) {
    digitalWrite(HUM_PIN, HIGH); //humidifier on
  }
  if (humidity > target + hyst) {
    digitalWrite(FAN_PIN, HIGH); //fan on
  }
  if (humidity > target - hyst && humidity < target + hyst) {
    digitalWrite(FAN_PIN, LOW); //fan off
    digitalWrite(HUM_PIN, LOW); //humidifier off
  }
}

void displayParams(float temp, float hum, int turn) {
  lcd.clear();
  String output = "T: " + String(temp) + " H: " + String(hum);
  lcd.println(output);
}
