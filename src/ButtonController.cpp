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
    if (button.pressed() && onPressedCallback) {
        onPressedCallback();
    }

    if (button.released() && onReleasedCallback) {
        onReleasedCallback();
    }
}

void ButtonController::onPressed(ButtonController::Callback callback) {
    onPressedCallback = callback;
}

void ButtonController::onReleased(ButtonController::Callback callback){
    onReleasedCallback = callback;
}
