#ifndef HC74595_H
#define HC74595_H

#include <Arduino.h>

// Shift register controller for 74HC595 using ESP32 GPIO
class HC74595 {
public:
    // Constructor with pin configuration
    HC74595(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin);

    // Initializes the GPIO pins
    void begin();

    // Sends one byte to the shift register
    void writeByte(uint8_t data, uint8_t bitOrder);

    // Clears the register (sends 0x00)
    void clear();

    // Sends multiple bytes to cascaded shift registers
    void writeBytes(const uint8_t* data, size_t length, uint8_t bitOrder);

private:
    uint8_t _dataPin;
    uint8_t _clockPin;
    uint8_t _latchPin;

    // Manually shifts out a byte, use bit order LSBFIRST or MSBFIRST
    void shiftOutByte(uint8_t data, uint8_t bitOrder);
};

#endif // HC74595_H