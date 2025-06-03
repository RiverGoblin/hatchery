#include <Arduino.h>
#include <SHT2x.h>
#include <time.h>
//#include <functions.h>
#include <LiquidCrystal_I2C.h>
#include "usefulStuff.h"
#include "SPIFFS.h"

#define SDA 22
#define SCL 21

#define BACKLIGHT 100
#define CONTRAST 255

#define HEATING_PIN 27
#define HUM_PIN 26
#define FAN_PIN 14
#define SET_BUTTON 4
#define EGG_ROTATOR 17

#define POT_1 25
#define POT_2 33

#define READING_FREQ 100

#define TEMP_HYST 0.2
#define HUM_HYST 4

long lastRead = 0;
float humidity = 0;
float temperature = 0;
float targetTemp = 0;
float targetHum = 0;

typedef struct {
  float temperature;
  float humidity;
  long time;
} measurement;

//int recordIndex = 0;

//measurement recordBuffer[30000/READING_FREQ];
measurement averageBuffer[5000/READING_FREQ] = {0};

SHT2x sensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);


////////////////////////////////////////////////////////////////////////FUNCTION DEFINITIONS/////////////////////////////////////////////////////////////



void manageTemp(float temperature, float target, float hyst) {
  if (temperature > target + hyst) {
    digitalWrite(HEATING_PIN, HIGH); //heating off
  }else if (temperature < target - hyst) {
    digitalWrite(HEATING_PIN, LOW); //heating on
  }
}

void manageHumidity(float humidity, float target, float hyst) {
  /* if (humidity < target - hyst) {
    digitalWrite(HUM_PIN, HIGH); //humidifier on
  }
  if (humidity > target + hyst) {
    digitalWrite(FAN_PIN, HIGH); //fan on
  }
  if (humidity > target - hyst && humidity < target + hyst) {
    digitalWrite(FAN_PIN, LOW); //fan off
    digitalWrite(HUM_PIN, LOW); //humidifier off
  } */
  if (humidity < target - hyst) {
      digitalWrite(HUM_PIN, HIGH);   // humidifier on
      digitalWrite(FAN_PIN, LOW);    // fan off
  } else if (humidity > target + hyst) {
      digitalWrite(FAN_PIN, HIGH);   // fan on
      digitalWrite(HUM_PIN, LOW);    // humidifier off
  } else {
      digitalWrite(FAN_PIN, LOW);    // fan off
      digitalWrite(HUM_PIN, LOW);    // humidifier off
  }
}

void readPots(float *target1, float *target2) {
  *target1 = 73 - mapFloat(analogRead(POT_1), 0, 4095, 34, 39);
  *target2 = 110 - mapFloat(analogRead(POT_2), 0, 4095, 20, 90);
}

void displayParams(measurement average, float targetTemp, float targetHum) {
  lcd.clear();
  String output1 = " H: " + String(average.humidity) + "T: " + String(average.temperature);
  lcd.println(output1);
  lcd.setCursor(0, 1);
  String output2 = " H: " + String(targetHum) + "T: " + String(targetTemp);
  lcd.println(output2);
}

/* void writeRecord(measurement input[]) {
  File recordFile = SPIFFS.open("records.csv", FILE_APPEND);
  
  for (int i = 0; i < 30000/READING_FREQ; i++) {
    String datapointString = String(input[i].time) + ";" + input[i].temperature + ";" + input[i].humidity;
    recordFile.println(datapointString);
  } 
  recordFile.close();
} */

int getLastIndex(measurement input[]) {
  int output = 0;
  while (input[output].time != 0) {
    output++;
  }
  return output;
}

void addToAverage(float temperature, float humidity, measurement average[]) {
  int lastIndex = getLastIndex(average);
  for (int i = 0; i <= lastIndex; i++) {
    average[i] = average[i];
  }
  average[lastIndex + 1].temperature = temperature;
  average[lastIndex + 1].humidity = humidity;
  average[lastIndex + 1].time = millis();
}

measurement getAverage(measurement input[]) {
  measurement averageMeasurement;
  int lastIndex = getLastIndex(input);
  for (int i = 0; i <= lastIndex; i++) {
    averageMeasurement.humidity += input[i].humidity;
    averageMeasurement.temperature += input[i].temperature;
  }
  averageMeasurement.humidity /= lastIndex + 1;
  averageMeasurement.temperature /= lastIndex + 1;

  return averageMeasurement;
}

///////////////////////////////////////////////////////////////////SETUP & LOOP///////////////////////////////////////////////6

void setup() {
    pinMode(SET_BUTTON, INPUT_PULLUP);
    pinMode(HEATING_PIN, OUTPUT);
    pinMode(HUM_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
    pinMode(EGG_ROTATOR, OUTPUT);

    Serial.begin(115200);
    SPIFFS.begin();
    Wire.setPins(SDA, SCL);
    Wire.begin();
    sensor.begin();
    lcd.setBacklight(BACKLIGHT);
    lcd.setContrast(CONTRAST);
    lcd.init();
    digitalWrite(EGG_ROTATOR, HIGH);
    //egrotátor (technolódzsia)
}

void loop() {
  if (millis() > lastRead + READING_FREQ) {
    sensor.read();
    humidity = sensor.getHumidity();
    temperature = sensor.getTemperature();
    readPots(&targetTemp, &targetHum);
    manageTemp(temperature, targetTemp, TEMP_HYST);
    manageHumidity(humidity, targetHum, HUM_HYST);
    addToAverage(temperature, humidity, averageBuffer);
    displayParams(getAverage(averageBuffer), targetTemp, targetHum);

    //////////////////////////////////////////////////////////////EZT TÖRÖLD KI////////////////////////////////////
    /* recordBuffer[recordIndex].humidity = humidity;
    recordBuffer[recordIndex].temperature = temperature;
    recordBuffer[recordIndex].time = millis();
    if (recordIndex >= 300) {
      writeRecord(recordBuffer);
      recordIndex = 0;
    }else{
      recordIndex++;
    } */
    ////////////////////////////////////////////////////////////EZT TÖRÖLD KI//////////////////////////////////////

    lastRead = millis();
  }
  Serial.println("loop zoop");
  Serial.println(humidity);
  Serial.println(temperature);
}

