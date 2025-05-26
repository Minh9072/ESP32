#ifndef L298MotorDriver_h
#define L298MotorDriver_h

#include <Arduino.h>

class L298MotorDriver {
public:
    L298MotorDriver(int in1Pin, int in2Pin, int enPin, bool pwm = true);

    void begin();
    void forward(int speed = 255);
    void backward(int speed = 255);
    void stop();

private:
    int _in1Pin;
    int _in2Pin;
    int _enPin;
    bool _usePWM;
    int _clampSpeed(int speed);
};

#endif