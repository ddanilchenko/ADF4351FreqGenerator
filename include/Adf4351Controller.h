#pragma once

#include <Arduino.h>

extern "C" {
#include "adf4351.h"
}

class Adf4351Controller {
public:
    struct Pins {
        gpio_num_t data;
        gpio_num_t clock;
        gpio_num_t le;
        gpio_num_t ce;
        gpio_num_t cs;
        gpio_num_t miso;
        gpio_num_t ld;
    };

    explicit Adf4351Controller(const Pins& _pins);

    void init();

    bool setReferenceFrequency(uint32_t frequencyHz);

    bool setFrequency(uint32_t frequencyHz);

    void enable();

private:
    void configureDefaults();
    void configurePins();

    Pins pins;
    ADF4351_cfg vfo{};
};