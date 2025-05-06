#include "c_sensor.h"

c_sensor::c_sensor() {
    sensorObj.begin();
}

sensorReading c_sensor::readSensor() {
    sensorReading output;
    
    output.hum = sensorObj.getHumidity();
    output.temp = sensorObj.getTemperature();
    return output;
}