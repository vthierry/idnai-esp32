// Defines general functions and variables to be used by specific mechanisms
// CAUTION => do not modify unless you know why and how ;)

#include "hostname.h"

// String manipulation

#include "string.h"
const char *secho(const char *fmt, ...);

// Serial interface echo method

void console_echo(bool ok, const char *fmt = NULL, ...);

// Web service functionalities

void service_on(String route, void (*handler)(void));

const char *service_get(String name);
	       
void service_answer(bool ok, const char *fmt, ...);

// Useful macros

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// Digital timing definitions

double fmillis(bool reset = false);

double getLoopDelay();

double getLastLoopDelay();

void setInterval(handler call, unsigned int index = 0, double delay = 0, unsigned int count = -1);

#define time_measurement_start(index, mode) double interruptTime_ ## index = -1; void IRAM_ATTR time_measurement_handle_ ## index() { if(interruptTime_ ## index == -1) interruptTime_ ## index = fmillis(); } pinMode(index, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(index), time_measurement_handle_ ## index, mode)
#define time_measurement_stop(index) detachInterrupt(digitalPinToInterrupt(index))
#define time_measurement_reset(index) interruptTime_ ## index = -1; 
#define time_measurement_get(index) interruptTime_ ## index

#define square_wave_start(timerIndex, outputPinIndex, frequency, duration) void IRAM_ATTR square_wave_handler_ ## outputPinIndex() { static bool up_else_down = false; digitalWrite(outputPinIndex, up_else_down = !up_else_down); } pinMode(outputPinIndex, OUTPUT); digitalWrite(outputPinIndex, 0); setInterval(timerIndex, square_wave_handler_ ## outputPinIndex, (int) rint(0.5 / frequency), duration == 0 ? -1 : 2 * (int) rint(duration * frequency)

// Analog interface

#define ANALOG_OUTPUT OUTPUT | 0x10
#define ANALOG_INPUT INPUT | 0x10

void pinMode(unsigned int index, unsigned int mode, bool check);

int analogRawWrite(unsigned int index, unsigned int value);

