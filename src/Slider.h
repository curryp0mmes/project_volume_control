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
    private:
        const int pinMotor1;
        const int pinMotor2;
        const int pinPotiVal;
        const int pinPotiOn;
        bool atTarget = true;
        PID pidController = PID(0.1, 0, 0.06, 0.05, 0, 255);
};