/*
  ARMDSL.cpp - Library for Arduino Range Measurer and Digital Spirit Level Project
  Created by Edrian Jose D. Ferrer, June 11, 2021
  Released into the public domain.
*/

#include "Arduino.h"
#include "Wire.h"
#include "ARMDSL.h"
         
ARMDSL::ARMDSL(int address){
    _address = address;
}

Vector ARMDSL::measureAccel()
{
    recordAccelRegisters(false);
    return {
        x : _x,
        y : _y,
        z : _z
    };
}
Vector ARMDSL::measureRawAccel()
{
    recordAccelRegisters(false);
    return {
        x : (float) raw_x,
        y : (float) raw_y,
        z : (float) raw_z,
    };
}

void ARMDSL::setupAccelerometer(int range,int address)
{
    Wire.begin();
    Wire.beginTransmission(_address); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high)
    Wire.write(0x6B);                 //Accessing the register 6B - Power Management
    Wire.write(0b00000000);           //Setting SLEEP register to 0. 
    Wire.endTransmission();
    Wire.beginTransmission(_address); //I2C address of the MPU
    Wire.write(0x1C);                 //Accessing the register 1C - Accelerometer Configuration 
    Wire.write(range);                //Setting the accel to range input variable
    Wire.endTransmission();
}

void ARMDSL::recordAccelRegisters(bool ado_high)
{
    Wire.beginTransmission(_address);   //I2C address of the MPU
    Wire.write(0x3B);                   //Starting register for Accel Readings
    Wire.endTransmission();
    Wire.requestFrom(_address, 6);      //Request Accel Registers (3B - 40)
    while (Wire.available() < 6);

    if (ado_high)
    {
        raw_x = Wire.read() | Wire.read() << 8; //Store first two bytes into raw_x
        raw_y = Wire.read() | Wire.read() << 8; //Store middle two bytes into raw_y
        raw_z = Wire.read() | Wire.read() << 8; //Store last two bytes into raw_z
    }
    else
    {
        raw_x = Wire.read() << 8 | Wire.read(); //Store first two bytes into raw _x
        raw_y = Wire.read() << 8 | Wire.read(); //Store middle two bytes into raw_y
        raw_z = Wire.read() << 8 | Wire.read(); //Store last two bytes into raw_z
    }

    processAccelData();
}


void ARMDSL::processAccelData()
{
    _x = raw_x / 16384.0;
    _y = raw_y / 16384.0;
    _z = raw_z / 16384.0;
}

float ARMDSL::measureDistance(int trig, int echo){

  digitalWrite(trig, LOW);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return (duration * 0.034 / 2);
}


int ARMDSL::readButtonState(int pin, long debounceDelay){
    int reading = digitalRead(pin);
    if (reading!=prevButtonState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > debounceDelay){
        if (reading != buttonState) {
            buttonState = reading;
        }
    }
    prevButtonState = reading;
    return buttonState;
}



long ARMDSL::watchButtonState(int pin, int expected, long debounceDelay){
    //This is a blocking function
    while(readButtonState(pin,debounceDelay) != expected);
    long pressInTime = millis();
    while(readButtonState(pin,debounceDelay) == expected);
    long pressOutTime = millis();
    return pressOutTime - pressInTime;
}


long ARMDSL::waitButtonState(int pin,  void (*callback)(), long duration, int expected, long debounceDelay){
    //This is a blocking function
    while(readButtonState(pin,debounceDelay) != expected);
    long pressInTime = millis();
    bool callbackCalled = false;
    while(readButtonState(pin,debounceDelay) == expected){
        long lapsed = millis() - pressInTime;
        if ((lapsed >= duration) && !callbackCalled)
        {
            callback();
            callbackCalled = true;
        }
    }
    long pressOutTime = millis();
    return pressOutTime - pressInTime;
}