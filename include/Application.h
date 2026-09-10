#pragma once
#include <Arduino.h>

#include "config.h"

#include "Adf4351Controller.h"
#include "ButtonController.h"
#include "FrequencySelector.h"
#include "LedController.h"

class Application {
public:
    Application();

    void setup();
    void loop();

private:
    static constexpr uint32_t TARGET_FREQ = 2400000000UL;
    static constexpr uint32_t REF_FREQ = 25000000UL; // 25 MHz

    static constexpr uint8_t LED_PIN = 8;
    static constexpr uint8_t BUTTON_PIN = 9;
    static constexpr uint16_t DEBOUNCE_MS = 20;

    static constexpr uint32_t INTERMEDIATE_FREQ[] = {
        28000000UL,             //28 MHz
        50000000UL,             //50 MHz
        144000000UL,            //144 MHz
        432000000UL             //432 MHz
    };

    static constexpr uint8_t FREQUENCY_COUNT =
        sizeof(INTERMEDIATE_FREQ) / sizeof(INTERMEDIATE_FREQ[0]);

    static constexpr char PREFERENCES_NAMESPACE[] = "settings";
    static constexpr char FREQUENCY_INDEX_KEY[] = "frqNtx";

    ButtonController button;
    LedController led;
    FrequencySelector frequencySelector;
    Adf4351Controller adf;

    void onFrequencyButtonPressed();

    bool applyCurrentFrequency();

    uint32_t calculateLoFrequency(
        uint32_t intermediateFrequency
    ) const;

    void printStatus(
        uint32_t intermediateFrequency,
        uint32_t loFrequency
    );

};
