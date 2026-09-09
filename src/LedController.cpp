#include "LedController.h"

LedController::LedController(uint8_t pin, int mode)
        : led(pin, mode) {}

void LedController::update() {
    led.loop();
}

void LedController::blinkNumberOfTimes(uint16_t onMs, uint16_t offMs, uint8_t count) {
    led.blinkNumberOfTimes(onMs, offMs, count);
}
