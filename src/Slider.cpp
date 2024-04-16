#include "Slider.h"
#include "Logger.h"


Slider::Slider(int pinMotor1, int pinMotor2, int pinPotiVal, int pinPotiSwitch) 
       :pinMotor1(pinMotor1), 
        pinMotor2(pinMotor2), 
        pinPotiVal(pinPotiVal), 
        pinPotiOn(pinPotiSwitch) {
    pinMode(pinPotiOn, OUTPUT);
    digitalWrite(pinPotiOn, HIGH);
    analogWrite(pinMotor1, 0);
    analogWrite(pinMotor2, 0);
    pidController.setActivityTimeout(0.2);
    pidController.setMaxOutput(255);

}

void Slider::vibrate() {

    for(int i = 0; i < 3; i++) {
      analogWrite(pinMotor1, 255);
      delay(4);
      analogWrite(pinMotor1, 0);
      analogWrite(pinMotor2, 255);
      delay(4);
      analogWrite(pinMotor2, 0);
    }

}

void Slider::power(bool on) {
    digitalWrite(pinPotiOn, on);
    setMotor(0);

    if(on) {

    }
}

void Slider::gotoPos(int pos) {
    if(pos > upper_bound) {
        gotoPos(upper_bound);
        return;
    }
    if(pos < lower_bound) {
        gotoPos(lower_bound);
        return;
    }


    pidController.setSetpoint(pos);
    atTarget = false;
}

int Slider::getVal() {
    int sum = 0;

    for(int i = 0; i < 500; i++) {
        sum += analogRead(pinPotiVal);
    }

    return sum/500;
}

int Slider::getValFast() {
    return analogRead(pinPotiVal);
}

void Slider::update() {
    if(!atTarget) {
        lastPowerVal = pidController.update(getVal()) * (reversed ? -1.f : 1.f);

        setMotor(lastPowerVal);

        if(lastPowerVal == 0) atTarget = true;
    }
}

int* Slider::calibrate() {
    setMotor(-255);
    delay(300);
    setMotor(0);
    delay(100);
    int val1 = getVal();
    lower_bound = val1;
    upper_bound = val1;

    setMotor(255);
    delay(300);
    setMotor(0);
    delay(100);
    int val2 = getVal();
    if(val2 < lower_bound) {
        lower_bound = val2;
        reversed = true;
    }
    if(val2 > upper_bound) {
        upper_bound = val2;
        reversed = false;
    }

    int* out = (int*) (malloc(sizeof(int) * 2));
    out[0] = lower_bound;
    out[1] = upper_bound;
    return out;
}

void Slider::setMotor(float val) {
    if(val == 0) {
        analogWrite(pinMotor1, 0);
        analogWrite(pinMotor2, 0);
    }
    else if(val < 0) {
        int mapped = map((-1) * (int)val, 0, 255, 110, 255);
        analogWrite(pinMotor1, 0);
        analogWrite(pinMotor2, mapped);
    }
    else {
        int mapped = map((int)val, 0, 255, 110, 255);
        analogWrite(pinMotor2, 0);
        analogWrite(pinMotor1, mapped);
    }
}
