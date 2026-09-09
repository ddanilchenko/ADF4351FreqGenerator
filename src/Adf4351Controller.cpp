#include "Adf4351Controller.h"

Adf4351Controller::Adf4351Controller(const Pins& _pins)
    : pins(_pins) {}

void Adf4351Controller::init() {
    configureDefaults();
    configurePins();

    Serial.println("Initializing ADF4351...");
    ADF4351_initialise(&vfo);
}

bool Adf4351Controller::setReferenceFrequency(uint32_t frequencyHz) {
    if (ADF4351_set_ref_freq(&vfo, frequencyHz) != 0) {
        Serial.printf("ERROR: Set reference frequency to %lu Hz\n",
                        static_cast<unsigned long>(frequencyHz));
        return false;
    }

    return true;
}

bool Adf4351Controller::setFrequency(uint32_t frequencyHz) {
    if (ADF4351_set_freq(&vfo, frequencyHz) != 0) {
        Serial.printf("ERROR: Set frequency to %lu Hz\n",
                        static_cast<unsigned long>(frequencyHz));
        return false;
    }

    return true;
}

void Adf4351Controller::enable() {
    ADF4351_enable(&vfo);
}


void Adf4351Controller::configureDefaults() {
    vfo = {};

    vfo.RD2refdouble = 0;       // reference doubler OFF
    vfo.RCounter = 1;           // no reference division
    vfo.RD1Rdiv2 = 0;           // divide-by-2 OFF
    vfo.BandSelClock = 200;
    vfo.ClkDiv = 150;
    vfo.ChanStep = 1000000UL;   // 1 MHz tuning step is sufficient for fixed 2256 MHz LO
    vfo.pwrlevel = 3;           // MAX OUTPUT Level
}

void Adf4351Controller::configurePins() {
    vfo.pins.gpio_mosi = pins.data;
    vfo.pins.gpio_sclk = pins.clock;
    vfo.pins.gpio_le   = pins.le;
    vfo.pins.gpio_ce   = pins.ce;

    vfo.pins.gpio_cs   = pins.cs;
    vfo.pins.gpio_miso = pins.miso;
    vfo.pins.gpio_ld   = pins.ld;
}
