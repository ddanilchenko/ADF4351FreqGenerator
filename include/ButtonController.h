#pragma once

#include <Arduino.h>
#include <Bounce2.h>

class ButtonController {
public:
    ButtonController(uint8_t pin, uint16_t debounceMs = 20, uint8_t pressedState = LOW);

    void init(); 
    void update();

    bool isPressed() const;
    bool isReleased() const;

private:
    uint8_t pin;
    uint16_t debounceMs;
    uint8_t pressedState;
    Bounce2::Button button;
};