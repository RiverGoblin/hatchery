#include <Arduino.h>
#include <SHT2x.h>
#include <time.h>
#include <functions.h>
#include <LiquidCrystal_I2C.h>

#define SDA 22
#define SCL 21

#define BACKLIGHT 100
#define CONTRAST 255

#define READING_FREQ 100

#define HEATING_PIN 13
#define HUM_PIN 14
#define FAN_PIN 26

#define TEMP_HYST 0.5
#define HUM_HYST 3

#define ROTATION_TIME 5000
#define MOTOR_PIN 27

#define TEMP_POT 32
#define HUM_POT 33
#define TURN_POT 25

#define SET_BUTTON 2

char turns;
long turnTime;
long lastTurn = 0;
char temp;
char humidity;

long lastRead = 100;

bool mode = 0;

c_sensor sensor;
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
    pinMode(SET_BUTTON, INPUT_PULLUP);
    pinMode(HEATING_PIN, OUTPUT);
    pinMode(HUM_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(MOTOR_PIN, OUTPUT);
    Wire.begin();
    Wire.setPins(SDA, SCL);
    lcd.init();
    lcd.setBacklight(BACKLIGHT);
    lcd.setContrast(CONTRAST);
    temp = readPot('T', TEMP_POT);
    humidity = readPot('H', HUM_POT);
    turns = readPot('U', TURN_POT);
    turnTime = setTurnTime(turns);
}

void loop() {
  if (false) {
    temp = readPot('T', TEMP_POT);
    humidity = readPot('H', HUM_POT);
    turns = readPot('U', TURN_POT);
    turnTime = setTurnTime(turns);
    displayMeasurements(temp, humidity, 1);
  }else{
    if (millis() > lastRead + READING_FREQ) {
      sensorReading reading = sensor.readSensor();
      Serial.println(reading.hum);
      Serial.println(reading.temp);
      manageTemp(reading.temp, temp, TEMP_HYST);
      manageHumidity(reading.hum, humidity, HUM_HYST);
      displayMeasurements(reading.temp, reading.hum, 1);
      lastRead = millis();
      Serial.println("loop zoop");
    }
    manageTurning(turnTime);
  }
  delay(1);
}

////////////////////////////////////////////////////////////////////////FUNCTION DEFINITIONS/////////////////////////////////////////////////////////////

void manageTemp(float temp, float target, float hyst) {
  if (temp > target + hyst) {
    digitalWrite(HEATING_PIN, HIGH); //heating off
  }
  if (temp < target - hyst) {
    digitalWrite(HEATING_PIN, LOW); //heating on
  }
}

void manageHumidity(float humidity, float target, float hyst) {
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

void manageTurning(long turnTime) {
  if (millis() > turnTime + lastTurn) {
    if (millis() < turnTime + lastTurn + ROTATION_TIME)  {
      digitalWrite(MOTOR_PIN, HIGH); //turning
    }else{
      digitalWrite(MOTOR_PIN, LOW); //turning stops
      lastTurn = millis();
    }
  }
}

long setTurnTime(char turnCount) {
  return 86400000 / turnCount;
}

float readPot(char type, char pin) {
                                //POTMETER MAX IS ASSUMED TO BE 4095
  float val;
  int reading = analogRead(pin);
  switch (type) {
    case 'T':
      val = 20 + reading / 204.70;
      break;
    case 'H':
      val = reading / 40.95;
      break;
    case 'U':
      val = reading / 409.5;
      break;
    default:
      val = 0;
      break;
  }

  return val;
} 

void displayMeasurements(float temp, float hum, int turn) {
  lcd.clear();
  String output = "T: " + String(temp) + " H: " + String(hum);
  lcd.println(output);
}
