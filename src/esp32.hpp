// Defines general functions and variables to be used by specific mechanisms
// CAUTION => do not modify unless you know why and how ;)

#include "hostname.h"

extern bool VERBOSE;

#include "string.h"

// Serial interface echo method

void echo(const char *fmt, ...);
void echoFlush();
const char *secho(const char *fmt, ...);

// Web server functionalities

extern WebServer *server;

void answer(bool ok, const char *fmt, ...);

// Useful macros

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// GPIO maximal index value < MAX_GPIO_INDEX, this does NOT mean that the pin is usable, see ./gpio_pins_usage.html

#define MAX_GPIO_INDEX 40

// Digital timing definitions

double fmillis(bool reset = false);

double getLoopDelay();

double getLastLoopDelay();

typedef void (*handler)(void);

void setInterval(handler call, double delay = 0, unsigned int count = -1);

void gpio_digital_timing_start(unsigned int index, unsigned int mode = RISING, handler handle_run = NULL);

void gpio_digital_timing_stop(unsigned int index);

double gpio_digital_timing_get(unsigned int index);

void gpio_square_wave(unsigned int index, double frequency = 1, double duration = 300, bool sync = false);

// Analog interface

#define ANALOG_OUTPUT OUTPUT | 0x10
#define ANALOG_INPUT INPUT | 0x10

extern String pinModeError;

void pinMode(unsigned int index, unsigned int mode, bool check);

int analogRawWrite(unsigned int index, unsigned int value);

