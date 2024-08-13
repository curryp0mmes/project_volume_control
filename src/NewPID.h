//
// Created by Simon on 13.08.2024.
//

#ifndef PROJECT_VOLUME_CONTROL_NEWPID_H
#define PROJECT_VOLUME_CONTROL_NEWPID_H

class PID {
public:
    PID();
    PID(float P, float I, float D):kp(P), ki(I), kd(D) {};

    int update(int input, unsigned long timestamp) { //timestamp is in millis since microcontroller boot

        float currentError = setpoint - input;
        float deltaTime =  (float)(timestamp - lastTime) / 1000.f; // in seconds

        //PROPORTIONAL
        float proportional = kp * currentError;

        //INTEGRAL
        float maxIntegral = maxOutput / ki;  // Adjust this based on system needs
        integral += currentError * deltaTime;
        if (integral > maxIntegral) {
            integral = maxIntegral;
        } else if (integral < -maxIntegral) {
            integral = -maxIntegral;
        }

        //DERIVATIVE
        float derivative = 0;
        if (deltaTime > 0.0f) { //prevent division by zero when dt too small
            derivative = (currentError - lastError) / deltaTime;
        }
        float filteredDerivative = (1 - alpha) * lastDerivative + alpha * derivative;
        lastDerivative = filteredDerivative;

        int outputSignal = (int) (proportional + ki * integral + kd * filteredDerivative);

        //Save Values for next iteration
        lastError = currentError;
        lastTime = timestamp;

        if (outputSignal > maxOutput)
            outputSignal = maxOutput;
        else if (outputSignal < -maxOutput)
            outputSignal = -maxOutput;

        return outputSignal;
    }

    void setSetpoint(int newSetpoint) {
        if (this->setpoint != newSetpoint) {
            integral = 0;  // Reset integral to prevent windup
        }
        this->setpoint = newSetpoint;
    }

    void setKp(float newP) { this->kp = newP; } ///< Proportional gain
    void setKi(float newI) { this->ki = newI; } ///< Integral gain
    void setKd(float newD) { this->kd = newD; } ///< Derivative gain

private:
    float kp = 1;
    float ki = 1;
    float kd = 1;
    int setpoint = 0;
    unsigned long lastTime = 0;
    float lastError = 0;
    float lastDerivative = 0;
    float integral = 0;
    float alpha = .8f; // Low pass for the derivative

    int maxOutput = 255;

};






#endif //PROJECT_VOLUME_CONTROL_NEWPID_H
