#ifndef STEPPER_ULN2003_H
#define STEPPER_ULN2003_H

#include <Arduino.h>

class StepperULN2003 {
public:
    enum Direction {
        FORWARD,
        BACKWARD
    };

    StepperULN2003(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);

    void begin();
    void setStepDelay(unsigned int delayMs);

    void stepForward(unsigned int steps);     // blocking
    void stepBackward(unsigned int steps);    // blocking

    void setRPM(float rpm, unsigned int stepsPerRevolution = 2048);

    void stepAsync(unsigned int steps, Direction dir);  // non-blocking
    void update();    // call frequently in loop()
    bool isBusy();    // check if motor is still running
    void stop();

private:
    uint8_t _pins[4];
    unsigned int _stepDelay;
    int _currentStep;

    unsigned int _targetSteps;
    unsigned int _stepsTaken;
    Direction _direction;
    unsigned long _lastStepTime;
    bool _isRunning;

    void stepMotor(int stepIndex);
};

#endif // STEPPER_ULN2003_H