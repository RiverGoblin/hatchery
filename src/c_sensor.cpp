#include "c_sensor.h"

sensorReading c_sensor::readSensor() {
    sensorReading output;
    output.hum = sensorObj.getHumidity();
    output.temp = sensorObj.getTemperature();
    return output;
}