#pragma once

#include <Arduino.h>
#include "PID.h"

class Slider {
    public:
        Slider(int pinMotor1, int pinMotor2, int pinPotiVal, int pinPotiSwitch);
        void vibrate();
        void gotoPos(int val);
        void power(bool on = true);
        int getVal();
        int getValFast();
        void update();
        int* calibrate();
        float lastPowerVal = 0;
        int upper_bound = 4096;
        int lower_bound = 0;
    private:
        boolean reversed = false;
        const int pinMotor1;
        const int pinMotor2;
        const int pinPotiVal;
        const int pinPotiOn;
        bool atTarget = true;
        PID pidController = PID(0.1, 0.002, 0.001, 0.01, 0, 255); //0.05 0.002 0.001
        void setMotor(float val);
};