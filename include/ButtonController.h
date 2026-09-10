#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <functional>

class ButtonController {
public:
    using Callback = std::function<void()>;
    ButtonController(uint8_t pin, uint16_t debounceMs = 20, uint8_t pressedState = LOW);

    void init(); 
    void update();

    void onPressed(Callback callback);
    void onReleased(Callback callback);

private:
    uint8_t pin;
    uint16_t debounceMs;
    uint8_t pressedState;
    Bounce2::Button button;

    Callback onPressedCallback;
    Callback onReleasedCallback;
};