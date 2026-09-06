#include "ButtonController.h"

ButtonController::ButtonController(uint8_t pin, uint16_t debounceMs, uint8_t pressedState)
    : pin(pin), debounceMs(debounceMs), pressedState(pressedState) {}


void ButtonController::init() {
    button.attach(pin, INPUT_PULLUP);
    button.interval(debounceMs);
    button.setPressedState(pressedState);
}

void ButtonController::update() {
    button.update();
}

bool ButtonController::isPressed() const {
    return button.pressed();
}

bool ButtonController::isReleased() const {
    return button.released();
}