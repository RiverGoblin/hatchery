#ifndef functions_h
#define functions_h

#include <Arduino.h>
#include <c_sensor.h>
#include <time.h>

void manageTemp(float temp, float target, float hyst);

void manageHumidity(float humidity, float target, float hyst);

void manageTurning(long turnTime);

long setTurnTime(char turnCount);

float readPot(char type, char pin);

void displayParams(float temp, float hum, int turn);
void displayMeasurements(float temp, float hum, int turn); 

#endif