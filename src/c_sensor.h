#ifndef c_sensor_h
#define c_sensor_h

#include <Arduino.h>
#include <SHT2x.h>

#define REFRESH_RATE 100

struct sensorReading{
    float temp;
    float hum;
};

class c_sensor {
    private:
        SHT21 sensorObj;
    public:
        sensorReading readSensor();
};


#endif