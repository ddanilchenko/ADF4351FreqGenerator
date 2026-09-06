#include <Arduino.h>
#include <Bounce2.h>
#include <Preferences.h>
#include <ezLED.h>

extern "C" {
#include "adf4351.h"
}


static constexpr uint32_t TARGET_FREQ =  2400000000UL;

static constexpr uint32_t INTERMEDIATE_FREQ[] = {
  28000000UL, //28 MHz
  50000000UL, //50 MHz
 144000000UL, //144 MHz
 432000000UL //432 MHz
};


static ADF4351_cfg vfo = {};

static constexpr uint32_t REF_FREQ = 25000000UL;     // 25 MHz

#define LED_PIN 8
ezLED led(LED_PIN, CTRL_ANODE);        

constexpr uint8_t BUTTON_PIN  = 9;

uint8_t selectedFrequencyIndex = 0;
constexpr uint8_t maxFrequencyIndex = sizeof(INTERMEDIATE_FREQ) / sizeof(INTERMEDIATE_FREQ[0]);

Bounce2::Button button;
constexpr uint16_t DEBOUNCE_MS = 20;

Preferences preferences;
constexpr char FREQUENCY_INDEX_KEY[] = "frqNtx";

void initADF4351() {
    /*
     * PLL configuration
     */
    vfo.RD2refdouble = 0;     // reference doubler OFF
    vfo.RCounter     = 1;     // no reference division
    vfo.RD1Rdiv2     = 0;     // divide-by-2 OFF

    vfo.BandSelClock = 200;
    vfo.ClkDiv       = 150;

    // 1 MHz tuning step is sufficient for fixed 2256 MHz LO
    vfo.ChanStep     = 1000000UL;

    // Maximum ADF4351 output level: +5 dBm
    vfo.pwrlevel     = 3; //3 - MAX OUTPUT Level

    /*
     * ESP32-C3 -> ADF4351
     *
     * Real connections:
     * GPIO5 = DATA
     * GPIO6 = CLK
     * GPIO7 = LE
     * GPIO10 = CE
     *
     * GPIO1/4/0 are dummy pins required by this library.
     */
    vfo.pins.gpio_mosi = GPIO_NUM_5;   // DATA
    vfo.pins.gpio_sclk = GPIO_NUM_6;   // CLK
    vfo.pins.gpio_le   = GPIO_NUM_7;   // LE
    vfo.pins.gpio_ce   = GPIO_NUM_10;   // CE

    // CS/MISO/LD are not connected on our board.
    // Library nevertheless requires a valid GPIO number.
    vfo.pins.gpio_cs   = GPIO_NUM_1;   // dummy
    vfo.pins.gpio_miso = GPIO_NUM_4;  // dummy
    vfo.pins.gpio_ld   = GPIO_NUM_0;   // dummy input

    Serial.println("Initializing ADF4351...");
    ADF4351_initialise(&vfo);
}

bool setRefFreqADF4351(const uint32_t refFreq) {
    if (ADF4351_set_ref_freq(&vfo, refFreq) != 0)
    {
        Serial.printf("ERROR: Set reference frequency to %lu MHz", refFreq);Serial.println();
        return false;
    }
    return true;
}

bool setFreqADF4351(const uint32_t freq) {
    if (ADF4351_set_freq(&vfo, freq) != 0)
    {
        Serial.printf("ERROR: Set frequency to %lu MHz", freq);Serial.println();
        return false;
    }

  return true;
}

void printStatus(const uint32_t freq) {
    uint32_t loFreqMHz = freq / 1000000;
    uint32_t intermediateFreqMHz = INTERMEDIATE_FREQ[selectedFrequencyIndex] / 1000000;
    uint32_t targetFreqMHz = TARGET_FREQ / 1000000;

    Serial.println("ADF4351 configured");
    Serial.printf("Active frequency Index: %u", selectedFrequencyIndex); Serial.println();
    Serial.printf("IF = %lu MHz", intermediateFreqMHz);Serial.println();
    Serial.printf("LO = %lu MHz", loFreqMHz);Serial.println();

    Serial.printf("%lu MHz + %lu MHz = %Lu MHz", intermediateFreqMHz, loFreqMHz, targetFreqMHz);Serial.println();

    led.blinkNumberOfTimes(150, 150, selectedFrequencyIndex + 1);
}

void updateLOFreqADF4351() {
    uint32_t loFreq = TARGET_FREQ - INTERMEDIATE_FREQ[selectedFrequencyIndex];
    if (!setFreqADF4351(loFreq)) {
      return;
    }
    printStatus(loFreq);
}

void setup()
{
    Serial.begin(115200);
    preferences.begin("settings", false);

    selectedFrequencyIndex = preferences.getInt(FREQUENCY_INDEX_KEY);
    if (selectedFrequencyIndex >= maxFrequencyIndex) {
      selectedFrequencyIndex = 0;
    }
    
    button.attach(BUTTON_PIN, INPUT_PULLUP);
    button.interval(DEBOUNCE_MS);
    button.setPressedState(LOW);

    delay(2 * 1000);

    Serial.println("ADF4351 QO-100 Upconverter");

    initADF4351();
    if (!setRefFreqADF4351(REF_FREQ)) {
      return;
    }
   
    ADF4351_enable(&vfo);
    
    updateLOFreqADF4351();
}

void handleButton() {
  button.update();
  if (button.pressed()) {
    ++selectedFrequencyIndex;
    if (selectedFrequencyIndex >= maxFrequencyIndex) {
      selectedFrequencyIndex = 0;
    }
    preferences.putInt(FREQUENCY_INDEX_KEY, selectedFrequencyIndex);
    updateLOFreqADF4351();
  }
}

void loop()
{
  led.loop();
  handleButton();
}