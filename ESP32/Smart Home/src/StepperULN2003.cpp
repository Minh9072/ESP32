#include "StepperULN2003.h"

// Step sequence for 8-step half-stepping control of a 4-phase stepper motor
static const uint8_t STEP_SEQUENCE[8][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

// Constructor: initializes motor pins and default values
StepperULN2003::StepperULN2003(uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4)
    : _stepDelay(2), _currentStep(0), _targetSteps(0), _stepsTaken(0),
      _direction(FORWARD), _lastStepTime(0), _isRunning(false)
{
    _pins[0] = in1;
    _pins[1] = in2;
    _pins[2] = in3;
    _pins[3] = in4;
}

// Sets up the motor control pins as OUTPUT and turns them off initially
void StepperULN2003::begin() {
    for (int i = 0; i < 4; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW);
    }
}

// Sets the delay time (in milliseconds) between steps
void StepperULN2003::setStepDelay(unsigned int delayMs) {
    _stepDelay = delayMs;
}

// Steps the motor forward by a specific number of steps (blocking)
void StepperULN2003::stepForward(unsigned int steps) {
    for (unsigned int i = 0; i < steps; i++) {
        _currentStep = (_currentStep + 1) % 8;  // Move to next step in sequence
        stepMotor(_currentStep);               // Energize coils for this step
        delay(_stepDelay);                     // Wait before next step
    }
}

// Steps the motor backward by a specific number of steps (blocking)
void StepperULN2003::stepBackward(unsigned int steps) {
    for (unsigned int i = 0; i < steps; i++) {
        _currentStep = (_currentStep + 7) % 8;  // Move to previous step (mod 8)
        stepMotor(_currentStep);
        delay(_stepDelay);
    }
}

// Starts an asynchronous step sequence (non-blocking)
void StepperULN2003::stepAsync(unsigned int steps, Direction dir) {
    _targetSteps = steps;
    _stepsTaken = 0;
    _direction = dir;
    _lastStepTime = millis();  // Save the current time
    _isRunning = true;         // Set running flag
}

// Sets motor speed based on RPM and steps per revolution
void StepperULN2003::setRPM(float rpm, unsigned int stepsPerRevolution) {
    if (rpm <= 0 || stepsPerRevolution == 0) return;
    _stepDelay = (60 * 1000UL) / (stepsPerRevolution * rpm);  // Calculate delay
}

// Updates motor state for asynchronous stepping (should be called frequently)
void StepperULN2003::update() {
    if (!_isRunning) return;

    unsigned long now = millis();
    if (now - _lastStepTime >= _stepDelay) {
        // Advance step based on direction
        if (_direction == FORWARD) {
            _currentStep = (_currentStep + 1) % 8;
        } else {
            _currentStep = (_currentStep + 7) % 8;
        }

        stepMotor(_currentStep);     // Energize coils for current step
        _lastStepTime = now;         // Update last step time
        _stepsTaken++;               // Increment step counter

        if (_stepsTaken >= _targetSteps) {
            _isRunning = false;      // Stop running when target reached
            stop();                  // Turn off motor
        }
    }
}

// Returns true if the motor is currently moving (asynchronous)
bool StepperULN2003::isBusy() {
    return _isRunning;
}

// Stops the motor by turning off all control pins
void StepperULN2003::stop() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(_pins[i], LOW);
    }
}

// Activates the motor coils according to the step sequence
void StepperULN2003::stepMotor(int stepIndex) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(_pins[i], STEP_SEQUENCE[stepIndex][i]);
    }
}