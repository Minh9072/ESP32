#include "HC74595.h"

// Constructor stores the pin numbers
HC74595::HC74595(uint8_t dataPin, uint8_t clockPin, uint8_t latchPin)
    : _dataPin(dataPin), _clockPin(clockPin), _latchPin(latchPin) {}

// Sets the pin modes and initializes them to LOW
void HC74595::begin() {
    pinMode(_dataPin, OUTPUT);
    pinMode(_clockPin, OUTPUT);
    pinMode(_latchPin, OUTPUT);

    digitalWrite(_dataPin, LOW);
    digitalWrite(_clockPin, LOW);
    digitalWrite(_latchPin, LOW);
}

// Sends a byte to the shift register and latches it
void HC74595::writeByte(uint8_t data, uint8_t bitOrder) {
    digitalWrite(_latchPin, LOW);
    shiftOutByte(data, bitOrder);
    digitalWrite(_latchPin, HIGH);
}

// Sends 0x00 to clear the outputs
void HC74595::clear() {
    writeByte(0x00, 1);
}

// Sends multiple bytes to chained shift registers
void HC74595::writeBytes(const uint8_t* data, size_t length, uint8_t bitOrder) {
    digitalWrite(_latchPin, LOW);
    for (size_t i = 0; i < length; ++i) {
        shiftOutByte(data[i], bitOrder);
    }
    digitalWrite(_latchPin, HIGH);
}

// Shifts out a byte bit by bit, use bit order LSBFIRST or MSBFIRST
void HC74595::shiftOutByte(uint8_t val, uint8_t bitOrder) {
    uint8_t i;

    for(i = 0; i < 8; i++) {
        if(bitOrder == LSBFIRST)
            digitalWrite(_dataPin, !!(val & (1 << i)));
        else
            digitalWrite(_dataPin, !!(val & (1 << (7 - i))));

        digitalWrite(_clockPin, HIGH);
        digitalWrite(_clockPin, LOW);
    }
}
