#pragma once

#include <Arduino.h>
#include <Preferences.h>

class FrequencySelector {
public:
    FrequencySelector(const uint32_t* _frequencies,
                      uint8_t _frequencyCount,
                      const char* _preferencesNamespace,
                      const char* _preferencesKey);

    void init();

    uint32_t currentFrequency() const;
    uint8_t currentIndex() const;
    uint8_t count() const;
    uint32_t next();

    void select(uint8_t index);

private:
    void save();

    const uint32_t* frequencies;
    uint8_t frequencyCount;
    const char* preferencesNamespace;
    const char* preferencesKey;

    Preferences preferences;
    uint8_t selectedIndex = 0;
};