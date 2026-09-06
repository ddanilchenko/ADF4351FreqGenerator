#include "FrequencySelector.h"

FrequencySelector::FrequencySelector(const uint32_t* _frequencies,
                      uint8_t _frequencyCount,
                      const char* _preferencesNamespace,
                      const char* _preferencesKey)
        : frequencies(_frequencies),
          frequencyCount(_frequencyCount),
          preferencesNamespace(_preferencesNamespace),
          preferencesKey(_preferencesKey) {}

void FrequencySelector::init() {
    preferences.begin(preferencesNamespace, false);

    selectedIndex = preferences.getUChar(preferencesKey, 0);

    if (selectedIndex >= frequencyCount) {
        selectedIndex = 0;
        save();
    }
}

uint32_t FrequencySelector::currentFrequency() const {
    return frequencies[selectedIndex];
}

uint8_t FrequencySelector::currentIndex() const {
    return selectedIndex;
}

uint8_t FrequencySelector::count() const {
    return frequencyCount;
}

uint32_t FrequencySelector::next() {
    selectedIndex = (selectedIndex + 1) % frequencyCount;
    save();
    return currentFrequency();
}

void FrequencySelector::select(uint8_t index) {
    if (index >= frequencyCount) {
        return;
    }

    selectedIndex = index;
    save();
}

void FrequencySelector::save() {
    preferences.putUChar(preferencesKey, selectedIndex);
}
