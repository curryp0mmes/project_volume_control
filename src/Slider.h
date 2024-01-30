#pragma once

#include <Arduino.h>

class Slider {
    public:
        Slider(int pinMotor1, int pinMotor2, int pinPotiVal, int pinPotiSwitch);
        void vibrate();
        void gotoPos(int val);
        void power(bool on = true);
        int getVal();
        int getValFast();
    private:
        const int pinMotor1;
        const int pinMotor2;
        const int pinPotiVal;
        const int pinPotiOn;
};