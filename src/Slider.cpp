#include "Slider.h"


Slider::Slider(int pinMotor1, int pinMotor2, int pinPotiVal, int pinPotiSwitch) 
       :pinMotor1(pinMotor1), 
        pinMotor2(pinMotor2), 
        pinPotiVal(pinPotiVal), 
        pinPotiOn(pinPotiSwitch) {
    pinMode(pinPotiOn, OUTPUT);
    digitalWrite(pinPotiOn, HIGH);
    analogWrite(pinMotor1, 0);
    analogWrite(pinMotor2, 0);
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
    analogWrite(pinMotor1, 0);
    analogWrite(pinMotor2, 0);

    if(on) {

    }
}

void Slider::gotoPos(int pos) {
    if(pos > 4096) pos = 4096;
    if(pos < 100) pos = 100;
    
    if(this->getVal() < pos) {
        
        analogWrite(pinMotor1, 255);
        while(this->getVal() < pos);
        analogWrite(pinMotor1, 0);
    }
    else {
      analogWrite(pinMotor2, 255);
      while(this->getVal() > pos);
      analogWrite(pinMotor2, 0);
    }
}

int Slider::getVal() {
    return analogRead(pinPotiVal);
}