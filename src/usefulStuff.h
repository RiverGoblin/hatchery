#ifndef usefulStuff_h
#define usefulStuff_h

#include "Arduino.h"
#include "sensor.h"

float mapFloat(float x, float inLow, float inHigh, float outLow, float outHigh){
    return ( x - inLow) * ( outHigh - outLow ) / ( inHigh - inLow ) + outLow;
}
float lerp(int point1, int point2, float t);
void splitString(const String& input, char delimiter, String* tokens, int maxTokens);
#endif
