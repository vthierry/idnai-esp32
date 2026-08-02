@aideAPI

## A tiny GPIO framework for ESP32 based connected objects

This software package provides a minimal set of functionalities to develop a connected object using a [ESP32](https://en.wikipedia.org/wiki/ESP32) micro-controller.

@frame tutorial.pdf

- The ESP32 has a [GPIO](https://en.wikipedia.org/wiki/General-purpose_input/output) interface with digital and analog inputs and outputs and the capability to measure times, or connect to additional control board to drive, e.g., servomotors.
- It is connected to the user via a WiFi interface, using a web page (for human) or a web service (for machine).
- It is programmed via an USB seriel-line interface connected on a computer with an `Arduino IDE` development tool.

## Installing and configuring Arduino IDE on your desktop computer

@frame ide_install.html

Reference:

- [ESP32 tutorial](https://lastminuteengineers.com/esp32-arduino-ide-tutorial)

## Hardware developing

### GPIO pins common usage

@frame gpio_pins_usage.html

Reference:

- [ESP32 hardware pinouts documentation](https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts) 

## Software developing

@frame idnai-esp32_usage.html

References: 

- [ESP32 basic routines (similar to arduino)](https://www.arduino.cc/reference) 
- [ESP32/Arduino language reference](https://www.arduino.cc/reference/en)
- Using harware timers:
  - [ESP32 latest documentation] (https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/api/timer.html)
  - [ESP32 timer usage tutorial](https://www.upesy.fr/blogs/tutorials/timer-esp32-with-arduino-code?srsltid=AfmBOopcHI7Qo72jnn1crr_6x6vzCH_oqCy2w_cyFulsgYpUu-ptYVx0&shpxid=1250039f-aaa0-46ef-b3a2-7fefbc062a85)

## Using the GPIO web-service

### Command line usage:

@frame gpio_usage.html

### The web interface:

![HTML web interface](web_page.png "GPIO idnai-esp32 web interface")


