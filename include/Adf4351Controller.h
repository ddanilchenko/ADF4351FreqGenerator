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

        // Required by the currently used ADF4351 library even when
        // these signals are not physically connected.
        gpio_num_t dummyCs;
        gpio_num_t dummyMiso;
        gpio_num_t dummyLd;
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