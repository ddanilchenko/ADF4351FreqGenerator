#include "Application.h"

constexpr uint32_t Application::INTERMEDIATE_FREQ[];
constexpr char Application::PREFERENCES_NAMESPACE[];
constexpr char Application::FREQUENCY_INDEX_KEY[];

Application::Application()
    : button(
          BUTTON_PIN,
          DEBOUNCE_MS,
          LOW
      ),
      led(LED_PIN),
      frequencySelector(
          INTERMEDIATE_FREQ,
          FREQUENCY_COUNT,
          PREFERENCES_NAMESPACE,
          FREQUENCY_INDEX_KEY
      ),
      adf({
          .data = GPIO_NUM_5,
          .clock = GPIO_NUM_6,
          .le = GPIO_NUM_7,
          .ce = GPIO_NUM_10,

          .cs = GPIO_NUM_1,
          .miso = GPIO_NUM_4,
          .ld = GPIO_NUM_0
      }) {
}

void Application::setup() {
    Serial.begin(115200);

    button.init();

    button.onPressed([this]() {
        onFrequencyButtonPressed();
    });

    frequencySelector.init();

    delay(4000);

    Serial.println("ADF4351 QO-100 Upconverter");

    adf.init();

    if (!adf.setReferenceFrequency(REF_FREQ)) {
        return;
    }

    adf.enable();

    applyCurrentFrequency();
}

void Application::loop() {
    led.update();
    button.update();
}

void Application::onFrequencyButtonPressed() {
    frequencySelector.next();
    applyCurrentFrequency();
}

bool Application::applyCurrentFrequency() {
    const uint32_t intermediateFrequency =
        frequencySelector.currentFrequency();

    const uint32_t loFrequency =
        calculateLoFrequency(intermediateFrequency);

    if (!adf.setFrequency(loFrequency)) {
        return false;
    }

    printStatus(
        intermediateFrequency,
        loFrequency
    );

    led.blinkNumberOfTimes(
        150,
        150,
        frequencySelector.currentIndex() + 1
    );

    return true;
}

uint32_t Application::calculateLoFrequency(
    uint32_t intermediateFrequency
) const {
    return TARGET_FREQ - intermediateFrequency;
}

void Application::printStatus(
    uint32_t intermediateFrequency,
    uint32_t loFrequency
) {
    Serial.println("ADF4351 configured");

    Serial.printf(
        "Active frequency index: %u\n",
        frequencySelector.currentIndex()
    );

    Serial.printf(
        "IF = %lu MHz\n",
        static_cast<unsigned long>(
            intermediateFrequency / 1000000UL
        )
    );

    Serial.printf(
        "LO = %lu MHz\n",
        static_cast<unsigned long>(
            loFrequency / 1000000UL
        )
    );

    Serial.printf(
        "%lu MHz + %lu MHz = %lu MHz\n",
        static_cast<unsigned long>(
            intermediateFrequency / 1000000UL
        ),
        static_cast<unsigned long>(
            loFrequency / 1000000UL
        ),
        static_cast<unsigned long>(
            TARGET_FREQ / 1000000UL
        )
    );
}
