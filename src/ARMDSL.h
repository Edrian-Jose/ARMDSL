/*
  ARMDSL.h - Library for Arduino Range Measurer and Digital Spirit Level Project
  Created by Edrian Jose D. Ferrer, June 11, 2021
  Released into the public domain.
*/

#ifndef ARMDSL_h
#define ARMDSL_h

#include "Arduino.h"

class Vector
{
public:
    float x, y, z;
};

class ARMDSL
{
public:
    ARMDSL(int address = 0b1101000);
    void setupAccelerometer(int range = 0, int address = 0b1101000);
    Vector measureAccel();
    Vector measureRawAccel();
    float measureDistance(int trig, int echo);
    int readButtonState(int pin, long debounceDelay = 50);
    long watchButtonState(int pin, int expected = HIGH, long debounceDelay = 50);
    long waitButtonState(int pin,  void (*callback)(), long duration = 500, int expected = HIGH, long debounceDelay = 50);

private:
    void recordAccelRegisters(bool ado_high = false);
    void processAccelData();
    int _address;
    int16_t raw_x, raw_y, raw_z;
    float _x, _y, _z;
    int buttonState;
    int prevButtonState =LOW;
    long lastDebounceTime = 0;
};

#endif