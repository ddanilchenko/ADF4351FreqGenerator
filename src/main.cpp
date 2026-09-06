#include <Arduino.h>


extern "C" {
#include "adf4351.h"
}


static constexpr uint32_t TARGET_FREQ =  2400000000UL;


static constexpr uint32_t INTERMEDIATE_FREQ = 28000000UL; //MHz
//static constexpr uint32_t INTERMEDIATE_FREQ = 50000000UL; //MHz
//static constexpr uint32_t INTERMEDIATE_FREQ = 144000000UL; //MHz
//static constexpr uint32_t INTERMEDIATE_FREQ = 432000000UL; //MHz


static constexpr uint32_t LO_FREQ  = TARGET_FREQ - INTERMEDIATE_FREQ;

static ADF4351_cfg vfo = {};

static constexpr uint32_t REF_FREQ = 25000000UL;     // 25 MHz

#define LED_PIN 8
int ledState = LOW;             
unsigned long previousMillis = 0;
const unsigned long interval = 1000;

#define BUTTON_PIN 9
uint8_t selectedFrequencyIndex = 0;
constexpr uint8_t maxFrequencyIndex = 5;
bool lastButtonState = HIGH;
bool buttonState = HIGH;

uint16_t lastDebounceTime = 0;
constexpr uint16_t DEBOUNCE_MS = 50;

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, ledState);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    delay(4 * 1000);

    Serial.println("ADF4351 QO-100 Upconverter");

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
    vfo.pwrlevel     = 3;

    /*
     * ESP32-C3 -> ADF4351
     *
     * Real connections:
     * GPIO5 = DATA
     * GPIO6 = CLK
     * GPIO7 = LE
     *
     * GPIO8/9/10 are dummy pins required by this library.
     */
    vfo.pins.gpio_mosi = GPIO_NUM_5;   // DATA
    vfo.pins.gpio_sclk = GPIO_NUM_6;   // CLK
    vfo.pins.gpio_le   = GPIO_NUM_7;   // LE

    //todo use GPIO_NUM_NC instead of dummy pins

    vfo.pins.gpio_ce   = GPIO_NUM_0;   // dummy

    vfo.pins.gpio_cs   = GPIO_NUM_1;   // dummy
    vfo.pins.gpio_miso = GPIO_NUM_10;  // dummy

    // LD is not connected on our board.
    // Library nevertheless requires a valid GPIO number.
    vfo.pins.gpio_ld   = GPIO_NUM_4;   // dummy input

    Serial.println("Initializing ADF4351...");

    ADF4351_initialise(&vfo);

    if (ADF4351_set_ref_freq(&vfo, REF_FREQ) != 0)
    {
        Serial.println("ERROR: invalid reference frequency");
        return;
    }

    ADF4351_enable(&vfo);

    if (ADF4351_set_freq(&vfo, LO_FREQ) != 0)
    {
        Serial.println("ERROR: cannot set 2256 MHz");
        return;
    }

    uint32_t loFreqMHz = LO_FREQ / 1000000;
    uint32_t intermediateFreqMHz = INTERMEDIATE_FREQ / 1000000;
    uint32_t targetFreqMHz = TARGET_FREQ / 1000000;

    Serial.println("ADF4351 configured");
    Serial.printf("Active frequency Index: %u", selectedFrequencyIndex); Serial.println();
    Serial.printf("IF = %lu MHz", intermediateFreqMHz);Serial.println();
    Serial.printf("LO = %lu MHz", loFreqMHz);Serial.println();

    Serial.printf("%lu MHz + %lu MHz = %Lu MHz", intermediateFreqMHz, loFreqMHz, targetFreqMHz);Serial.println();
}

void handleButton() {
  int currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long currentMillis = millis();

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = currentMillis;
    lastButtonState = currentButtonState;
  }
  if ((currentMillis - lastDebounceTime) >= DEBOUNCE_MS) {
    if (currentButtonState != buttonState) {
      buttonState = currentButtonState;
      if (buttonState == LOW) {
        ++selectedFrequencyIndex;
        if (selectedFrequencyIndex >= maxFrequencyIndex) {
          selectedFrequencyIndex = 0;
        }
        Serial.printf("Active frequency Index: %u", selectedFrequencyIndex); Serial.println();
      }
    }
  }

  /*
  if (buttonState == LOW) {
    Serial.println("Button pressed");
  }*/
}

void loop()
{
  unsigned long currentMillis = millis();

  handleButton();

  if (currentMillis - previousMillis >= interval) {
    // Save the last time you blinked the LED
    previousMillis = currentMillis;

    // Toggle the LED state
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // Apply the state to the physical pin
    digitalWrite(LED_PIN, ledState);
  }
}