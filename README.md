# ADF4351 Frequency Generator

A compact ESP32-C3 based controller for an ADF4351 frequency synthesizer.

The project was originally created as a local oscillator controller for a
2.4 GHz QO-100 upconverter. It allows switching between several predefined
intermediate frequencies while automatically calculating and programming
the corresponding ADF4351 LO frequency.

The firmware is built with PlatformIO and the Arduino framework and is
structured using a small object-oriented architecture to keep hardware
control and application logic separated.

## Features

- ESP32-C3 based controller
- ADF4351 frequency synthesizer control
- 2.4 GHz target output frequency
- Multiple predefined IF presets
- Automatic LO frequency calculation
- Push-button frequency selection
- Non-blocking button debouncing
- LED status indication
- Selected frequency stored in ESP32 NVS
- USB CDC serial console
- External ADF4351 reference frequency support
- Object-oriented firmware architecture
- Prepared for future OLED/I2C support

## Frequency Plan

The synthesizer is used as the LO of an upconverter:

    RF = IF + LO

For the current QO-100 configuration:

    RF = 2400 MHz

The firmware calculates:

    LO = 2400 MHz - IF

Current IF presets:

| IF | LO |
|---:|---:|
| 28 MHz | 2372 MHz |
| 50 MHz | 2350 MHz |
| 144 MHz | 2256 MHz |
| 432 MHz | 1968 MHz |

The active preset is stored in ESP32 non-volatile storage and restored
after power-up.

## Hardware

### Controller

- ESP32-C3 Super Mini
- ADF4351 synthesizer module
- Push button
- Status LED
- 5 V power supply

The ESP32-C3 is powered from the same regulated +5 V rail used by the
controller board.

### ESP32-C3 pin assignment

| Function | GPIO |
|---|---:|
| ADF4351 DATA | GPIO5 |
| ADF4351 CLK | GPIO6 |
| ADF4351 LE | GPIO7 |
| Status LED | GPIO8 |
| Button | GPIO9 |
| ADF4351 CE | GPIO10 |

Some additional GPIOs are currently reserved/free and can be used for
future extensions such as an OLED display over I2C.

### ADF4351 signals

The firmware controls the essential ADF4351 signals:

- DATA
- CLK
- LE
- CE

The current ADF4351 library also expects CS, MISO and LD pin definitions.
These are provided by the wrapper even though they are not required by the
current hardware connection.

## Reference Clock

The current configuration uses:

    REF = 25 MHz

The ADF4351 controller supports setting the reference frequency in
software, making it possible to use a more stable external reference such
as an OCXO or GPSDO.

A stable reference is particularly useful for narrow-band digital modes
and QO-100 operation.

## Controls

### Frequency button

Pressing the button selects the next configured IF frequency.

The sequence wraps around after the last preset.

Example:

    28 MHz
      ↓
    50 MHz
      ↓
    144 MHz
      ↓
    432 MHz
      ↓
    28 MHz

Button debouncing is handled by `ButtonController` using Bounce2.

### LED

The LED provides visual feedback about the selected preset.

After changing frequency, the LED blinks according to the selected
frequency index.

The LED handling is non-blocking and encapsulated in `LedController`.

## Software Architecture

The firmware intentionally separates hardware-specific code from
application logic.

    main.cpp
        │
        ▼
    Application
        │
        ├── ButtonController
        ├── LedController
        ├── FrequencySelector
        └── Adf4351Controller

### Application

`Application` is the main orchestration layer.

It is responsible for:

- initializing the system
- reacting to button events
- selecting frequencies
- calculating the LO frequency
- programming the ADF4351
- updating status indication

The goal is to keep Arduino's `setup()` and `loop()` as small as possible.

### ButtonController

Encapsulates:

- GPIO configuration
- Bounce2 debouncing
- button state handling

The controller is intended to expose button events through callbacks so
application logic does not need to poll button state directly.

### LedController

Encapsulates LED operation and status indication.

LED timing is handled without blocking the main application loop.

### FrequencySelector

Manages the available IF presets.

Responsibilities include:

- current frequency selection
- cycling through presets
- storing the selected index
- restoring the previous selection after restart

ESP32 `Preferences` is used for persistent storage.

### Adf4351Controller

A C++ wrapper around the ADF4351 library.

It hides low-level synthesizer configuration from the rest of the
application and provides a simple interface for:

- initialization
- reference frequency configuration
- output frequency configuration
- synthesizer enable control

## Project Structure

    ADF4351FreqGenerator/
    ├── include/
    │   ├── config.h
    │   ├── Application.h
    │   ├── Adf4351Controller.h
    │   ├── ButtonController.h
    │   ├── FrequencySelector.h
    │   └── LedController.h
    │
    ├── src/
    │   ├── main.cpp
    │   ├── Application.cpp
    │   ├── Adf4351Controller.cpp
    │   ├── ButtonController.cpp
    │   ├── FrequencySelector.cpp
    │   └── LedController.cpp
    │
    ├── test/
    └── platformio.ini

## Building

The project uses PlatformIO.

Clone the repository:

    git clone https://github.com/ddanilchenko/ADF4351FreqGenerator.git

Open the directory in VS Code with the PlatformIO extension installed and
build the project.

From the command line:

    pio run

Upload:

    pio run --target upload

Open the serial monitor:

    pio device monitor

Serial speed:

    115200 baud

## Dependencies

Dependencies are managed automatically by PlatformIO:

- [ADF4351-ESP32](https://github.com/YutingXu/ADF4351-ESP32.git)
- [Bounce2](https://github.com/thomasfredericks/Bounce2)
- [ArduinoGetStarted LED](https://github.com/ArduinoGetStarted/led.git)
- ESP32 Preferences
  
See `platformio.ini` for the exact configuration.

## Serial Output

The serial console reports the currently selected frequency configuration.

Example:

    ADF4351 QO-100 Upconverter
    ADF4351 configured
    Active frequency index: 0
    IF = 28 MHz
    LO = 2372 MHz
    28 MHz + 2372 MHz = 2400 MHz

## Planned Improvements

The architecture is intentionally designed to allow additional features
without putting more logic into `main.cpp`.

Possible extensions include:

- OLED status display
- I2C peripheral support
- display of IF / LO / RF frequencies
- ADF4351 lock status indication
- configurable frequency presets
- serial command interface
- external reference configuration
- additional operating profiles
- improved error/status reporting

An OLED display can be implemented as a separate `DisplayController`,
while `Application` remains responsible for coordinating the components.

## Purpose

This project is primarily intended as the controller for a compact
QO-100 2.4 GHz upconverter, but the ADF4351 abstraction makes it suitable
for other frequency synthesizer and local oscillator applications as well.

## License
The project is licensed under the MIT License.
See the [LICENSE](LICENSE) file for details.
