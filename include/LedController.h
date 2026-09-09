#pragma once

#include <Arduino.h>
#include <ezLED.h>

class LedController {
public:
    explicit LedController(uint8_t pin, int mode = CTRL_ANODE);
    void update();
    void blinkNumberOfTimes(uint16_t onMs, uint16_t offMs, uint8_t count);

private:
    ezLED led;
};