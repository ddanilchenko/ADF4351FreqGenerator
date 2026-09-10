#include <Arduino.h>

#include "ButtonController.h"
#include "Adf4351Controller.h"
#include "FrequencySelector.h"
#include "LedController.h"

static constexpr uint32_t TARGET_FREQ =  2400000000UL;

static constexpr uint32_t INTERMEDIATE_FREQ[] = {
  28000000UL, //28 MHz
  50000000UL, //50 MHz
 144000000UL, //144 MHz
 432000000UL //432 MHz
};

constexpr uint8_t FREQUENCY_COUNT =
    sizeof(INTERMEDIATE_FREQ) / sizeof(INTERMEDIATE_FREQ[0]);

static constexpr uint32_t REF_FREQ = 25000000UL;     // 25 MHz

#define LED_PIN 8
LedController led(LED_PIN);   

constexpr uint8_t BUTTON_PIN  = 9;

constexpr uint16_t DEBOUNCE_MS = 20;
ButtonController button(BUTTON_PIN, DEBOUNCE_MS, LOW);

Preferences preferences;
constexpr char PREFERENCES_NAMESPACE[] = "settings";
constexpr char FREQUENCY_INDEX_KEY[] = "frqNtx";

FrequencySelector frequencySelector(
    INTERMEDIATE_FREQ,
    FREQUENCY_COUNT,
    PREFERENCES_NAMESPACE,
    FREQUENCY_INDEX_KEY
);

Adf4351Controller adf({
    .data = GPIO_NUM_5,
    .clock = GPIO_NUM_6,
    .le = GPIO_NUM_7,
    .ce = GPIO_NUM_10,

    // Not physically connected; required by the ADF4351 library.
    .cs = GPIO_NUM_1,
    .miso = GPIO_NUM_4,
    .ld = GPIO_NUM_0
});
void onPressedCallback();


uint32_t calculateLoFrequency(uint32_t intermediateFrequency) {
    return TARGET_FREQ - intermediateFrequency;
}

void printStatus(uint32_t intermediateFrequency, uint32_t loFrequency) {
    Serial.println("ADF4351 configured");
    Serial.printf("Active frequency index: %u\n",
                  frequencySelector.currentIndex());

    Serial.printf("IF = %lu MHz\n",
                  static_cast<unsigned long>(intermediateFrequency / 1000000UL));

    Serial.printf("LO = %lu MHz\n",
                  static_cast<unsigned long>(loFrequency / 1000000UL));

    Serial.printf("%lu MHz + %lu MHz = %lu MHz\n",
                  static_cast<unsigned long>(intermediateFrequency / 1000000UL),
                  static_cast<unsigned long>(loFrequency / 1000000UL),
                  static_cast<unsigned long>(TARGET_FREQ / 1000000UL));

    led.blinkNumberOfTimes(150, 150, frequencySelector.currentIndex() + 1);
}

bool applyCurrentFrequency() {
    const uint32_t intermediateFrequency =
        frequencySelector.currentFrequency();

    const uint32_t loFrequency =
        calculateLoFrequency(intermediateFrequency);

    if (!adf.setFrequency(loFrequency)) {
        return false;
    }

    printStatus(intermediateFrequency, loFrequency);
    return true;
}

void setup()
{
    Serial.begin(115200);
    
    button.init();
    button.onPressed(&onPressedCallback);

    frequencySelector.init();

    delay(4 * 1000);

    Serial.println("ADF4351 QO-100 Upconverter");

    adf.init();

    if (!adf.setReferenceFrequency(REF_FREQ)) {
        return;
    }

    adf.enable();
    applyCurrentFrequency();
}

void onPressedCallback() {
  frequencySelector.next();
  applyCurrentFrequency();

}

void loop()
{
  led.update();
  button.update();
}