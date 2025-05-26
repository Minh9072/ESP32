#include "L298MotorDriver.h"
static const int pwmChannel = 0;

L298MotorDriver::L298MotorDriver(int in1Pin, int in2Pin, int enPin, bool pwm)
    : _in1Pin(in1Pin), _in2Pin(in2Pin), _enPin(enPin), _usePWM(pwm) {}

void L298MotorDriver::begin() {
    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);
    pinMode(_enPin, OUTPUT);
    
    if (_usePWM) {
        ledcSetup(pwmChannel, 1000, 8); // 1 kHz, 8-bit
        ledcAttachPin(_enPin, pwmChannel);
    }

    stop();
}

void L298MotorDriver::forward(int speed) {
    speed = _clampSpeed(speed);
    digitalWrite(_in1Pin, HIGH);
    digitalWrite(_in2Pin, LOW);
    if (_usePWM) {
        ledcWrite(pwmChannel, speed);
    } else {
        digitalWrite(_enPin, HIGH);
    }
}

void L298MotorDriver::backward(int speed) {
    speed = _clampSpeed(speed);
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, HIGH);
    if (_usePWM) {
        ledcWrite(pwmChannel, speed);
    } else {
        digitalWrite(_enPin, HIGH);
    }
}

void L298MotorDriver::stop() {
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, LOW);
    if (_usePWM) {
        ledcWrite(pwmChannel, 0);
    } else {
        digitalWrite(_enPin, LOW);
    }
}

int L298MotorDriver::_clampSpeed(int speed) {
    if (speed < 0) return 0;
    if (speed > 255) return 255;
    return speed;
}