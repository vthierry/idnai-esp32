// Defines general functions and variables to be used by specific mechanisms
// CAUTION => do not modify unless you know why and how ;)

// This defines the hostname

#include "hostname.h"

// String manipulation

#include "string.h"

/**
 * @function echo
 * @memberof std
 * @static
 * @description Returns a formatted message à-la printf.
 * - The formatted string is limited to the reasonable size of 64KB.
 * - The `ECHO(string, arg)` macro allows to implements such a mechanism within a function:
 *   - `string` is the name of string recipient.
 *   - `arg` is the function last argument before `...`.
 *   - For example `echo_()` is another implementation of echo():
 * ```
 * std::string echo_(String message, ...) { ECHO(result, message); return result; }
 * ```
 * @param {string} message The message, a string format à-la printf with more parameters if used.
 * @param {...anything} parameters Any message parameters.
 * @return {string} The formatted message.
 */
std::string echo(String message, ...);

#defined ECHO(string, message) va_list args; va_start(args, message); std::string string = echo(message, args); va_end(args)

// Serial interface echo method

/**
 * @function console_echo
 * @static
 * @description Prints on the serial console a formatted message à-la printf.
 * @param {bool} flush
 * - If true, sends directly to the serial console.
 * - If false, buffers the output until the next `loop()` occurence, avoiding to slow down interruption code.
 *   - It is safer than the low-level esf_printf() function since the serial output is not used during the interruption.
 * @param {string} message The message, a string format à-la printf with more parameters if needed.
 * @param {...anything} parameters Any message parameters.
 */
void console_echo(bool ok, const char *fmt = NULL, ...);

// Web service functionalities

/**
 * @function service_on
 * @static
 * @description Attachs a handler to a web-service route.
   * @param {string} route The path defining the service, e.g., "/action".
   * - It accepts GET or POST requests.
   * @param {callback} handler. A parameter less method implementing the service.
   * - It uses the `service_get(name)` method to get argument's value.
   * - The handler MUST end calling the `service_answer()` function in order to send the response message.
 * @param {string} route The web-service route.
 * @param {handler} handler A `void (* handler)(void)` handle function.
 */
void service_on(String route, void (*handler)(void));

/**
 * @function service_get
 * @static
 * @description Gets a parameter value.
 * @param {string} name The parameter name.
 * @return {string} The parameter value, or the empty string if undefined.
 */
const char *service_get(String name);
	       
/**
 * @function service_answer
 * @static
 * @description Answers to a request and send the response.
 * @param {bool} ok If true answer is ok (code 200, or 204 if no message) else error (code 400).
 * @param {string} message The message, a string format à-la printf with more parameters if needed. It is:
 * - A JSON data structure if starting with '{'.
 * - A HTML page if starting with '<'.
 * - No message if equal to "".
 * - A textual string otherwise.
 * @param {...anything} parameters Any message parameters.
 */
void service_answer(bool ok, const char *fmt, ...);

// Loop and time functions

/**
 * @function fmillis
 * @static
 * @description Returns the current time in millisecond at a microsecond precision.
 * @param {bool} [reset=false] If true, resets the time value to '0' (thus, this call returns ``0´´).
 * @return double The current time, since the processor reset.
 *  - Returns a full precision in micro-seconds for about 142 years, using the 52 bit matissa,
 *  - Avoids overflow for longer than the universes age.
 */
double fmillis(bool reset = false);

/**
 * @function getLoopDelay
 * @static
 * @description Returns an approximate value of the average `loop()` sample time.
 * - It is averaged over about 10 samples with an exponential filter.
 * - The `getLastLoopDelay()` function returns the raw last delay without filtering.
 * - The usual out of charge loop delay is:
 *   - about 3 to 4 µsec when only the _getLoopDelay_ mechanisms are active.
 *   - about 40 µsec every 10 msec for the web service handle mechanism monitoring, when no request.
 *   - about 15 µsec by char (rough estimation) for a print on the serial interface.
 *   - about 1 msec every 10 sec when in verbose mode to echo the loop-delay on the serial interface.
 * @return double The approximate sample time in milliseconds.
 */
double getLoopDelay();
double getLastLoopDelay();

// GPIO interface

/**
 * @function pinMode
 * @static
 * @description Sets the digital value input/output mode.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {IOmode} value Either [INPUT|INPUT_PULLUP|INPUT_PULLDOWN|OUTPUT|ANALOG_INPUT|ANALOG_OUTPUT|ANALOG_PWM].
 * - By default, all pins are INPUT.
 * - The [INPUT_PULLUP|INPUT_PULLDOWN] allows to use internal pull-up or pull-down resistances, simplifying the electronics.
 * @param {bool} [check=false] If true, checks that the required pinMode is compatible with the [ESP32 GPIO pin assignment](https://line.gitlabpages.inria.fr/aide-group/esp32gpiocontrol/gpio_pins_usage.html) and echoes an error on the console if not.
 */
void pinMode(unsigned int index, unsigned int mode, bool check);

/**
 * @function digitalRead
 * @static
 * @description Inputs a digital value.
 * - The `pinMode(index, INPUT)` must be called before.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @return {bool} Either 1 (HIGH) or 0 (LOW), i.e. for a voltage threshold of about:
 * - 1.5V (5V boards)
 * - 1.0V (3.3V boards)
 */
/**
 * @function digitalWrite
 * @static
 * @description Outputs a digital value
 * - The `pinMode(index, OUTPUT)` must be called before.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {bool} value Either 1 (HIGH) or 0 (LOW), i.e. for a voltage of about:
 * - 5V if HIGH (5V boards)
 * - 3.3V if HIGH (3.3V boards)
 * - 0V if LOW.
 */
/**
 * @function analogRead
 * @static
 * @description Inputs an analog value
 * - The `pinMode(index, INPUT)` must be called before.
 * - Using the alternative mode `analogReadMilliVolts(index)` returns the result calibrated in milli-volts.
 * - Using `analogReadResolution(resolution)` allows to adjust resolution, from 9 to 12 bits, 10 bits corresponding to the best real precision.
 * - Using `analogSetAttenuation(range)` allows to adjust attenuation using values [ADC_0db|ADC_2_5db|ADC_6db|ADC_11db]:
 *   - 11dB attenuation gives full-scale voltage 3.3V (for a 3.3V chip), linear between 100 and 2450mV, default.
 *   - 6dB attenuation gives full-scale voltage 2.2V (for a 3.3V chip), linear between 100 and 1750mV.
 *   - 2.5dB attenuation gives full-scale voltage 1.5V (for a 3.3V chip), linear between 100 and 1250mV.
 *   - 0dB attenuation gives full-scale voltage 1.1V (for a 3.3V chip), linear between 100 and 950mV.
 * @param {uint} index The given pin index: 32 to 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @return {unit} The raw value, between 0 and 4095, depending on the configuration.
 */

// Analog interface

#define ANALOG_PWM OUTPUT | 0x20
#define ANALOG_OUTPUT OUTPUT | 0x10
#define ANALOG_INPUT INPUT | 0x10

/**
 * @function analogPWMWrite
 * @static
 * @description Outputs an analog value.
 * - The `pinMode(index, OUTPUT)` must be called before.
 * - It uses the LEDC firmware, which supports 16 PWM channels.
 * - Typical resolution-frequency trade-off:
 * | Resolution (bits) | Frequency (Hertz) | Typical use |
 * ----------------------------------------------
 * 8 (0-255)           | ~150 000         |   Fast switching |
 * 10 (9–1023)         | ~40 000          | Smoother dimming |
 * 13 (0–8191)         |  ~5 000          | Standard default |
 * 15 (0-32767)        |  ~1 000          | High precision, low speed |
 * - Typical usage:
 * ```
 * // First call to set channel, resolution, and frequency.
 * analogPWMWrite(myIndex, 0, [,someResolution [, someFrequency]]);
 * for(unsigned t = 0; t < 1000; t++) {
 *  // Next calls reusing the registered values.
 *  analogWrite(myIndex, t);
 *  ../.. // Other operations.
 * }
 * ```
 * @param {uint} index The given pin index, between 0 and 39.
 * - Up to 16 indexes can be used, since they are 16 PWM channels, automatically allocated, for the whole program duration.
 *   - If required more than 16 PWM channels, a message is issued in the console.
 * @param {unit} [value=0] The value depending on the resolution.
 * @param {uint} [resolution=13] The PWM resolution, 1 to 16 bits.
 * @param {uint} [frequency=5000] The PWM frequency in Hertz, from 1000 to 20000, depending on the resolution
 * - The frequency is automatically bounded so that `2^resolution x frequency < 40MHz`, a message is issued in the console.
 * @throws A `illegal-argument` exception if a channel is reused for a second pin index, without releasing it.
 */
void analogPWMWrite(unsigned int index, unsigned int value = 0, unsigned int resolution = 13, unsigned int frequency = 5000);

#define RAW_DAC 1

#if RAW_DAC

/**
 * @function analogRawWrite
 * @static
 * @description Outputs an analog value
 * - The `pinMode(index, OUTPUT)` must be called before.
 * - It uses the Digital to Analog on-board Converters (DAC), not available on all ESP32 chips:
 *   - If unavailable, an error is issued in the console at the first call.
 * @param {uint} index The given pin index: 25 or 26.
 * @param {unit} A 0 to 255 value, for a 0V to 3.3V or 5V voltage, the value is modulo 256.
 */
void analogRawWrite(unsigned int index, unsigned int value);

#endif

/**
 * @function attachInterrupt
 * @static
 * @description Attaches an interruption to a handler.
 * - It must be called using a construct of the form `attachInterrupt(digitalPinToInterrupt(index), handle);`.
 * - Timing measure example:
 * ```
 * double interruptTime = -1;
 * void IRAM_ATTR timing_handle()
 * {
 *   if(interruptTime == -1)
 *     interruptTime = fmillis();
 * }
 * pinMode(index, INPUT_PULLUP);
 * attachInterrupt(digitalPinToInterrupt(index), timing_handle, mode);
 * ```
 * - If the `handle()` is too long then a fatal error `Guru Meditation Error: … Exception was unhandled.` occurs and it reboots.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {handler} handler A `void (* handler)(void)` handle function.
 * @param {Mode} mode The detection mode:
 * - RISING when the input signal changes from low to high.
 * - FALLING when the input signal changes from hight to low.
 * - CHANGE when the input signal changes from low to high or from hight to low.
 */

/**
 * @function detachInterrupt
 * @static
 * @description Detaches an interruption from its handler.
 * - It must be called using a construct of the form `detachInterrupt(digitalPinToInterrupt(index));`.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 */

/**
 * @function time_measurement_start
 * @static
 * @description Starts time measurement on a GPIO input pin.
 *  - It measures the first occuring event and maintain the value until reset.
 *    - The time is given in milli-seconds since the processor start.
 *  - It is used with the complementary functions:
 *    - `time_measurement_get(index)` returns the detected time.
 *    - `time_measurement_reset(index)` resets the detected time to measure a new value.
 *    - `time_measurement_stop(index)` stops the time measurement.
 *  - It is implemented as C macro and the code corresponds to the example given in the `attachInterrupt()` description.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {Mode} mode The detection mode:
 * - RISING when the input signal changes from low to high.
 * - FALLING when the input signal changes from hight to low.
 * - CHANGE when the input signal changes from low to high or from hight to low.
 */
#define time_measurement_start(index, mode) double interruptTime_ ## index = -1; void IRAM_ATTR time_measurement_handle_ ## index() { if(interruptTime_ ## index == -1) interruptTime_ ## index = fmillis(); } pinMode(index, INPUT_PULLUP); attachInterrupt(digitalPinToInterrupt(index), time_measurement_handle_ ## index, mode)
#define time_measurement_stop(index) detachInterrupt(digitalPinToInterrupt(index))
#define time_measurement_reset(index) interruptTime_ ## index = -1; 
#define time_measurement_get(index) interruptTime_ ## index

/**
 * @function runHandler
 * @static
 * @description Calls a handler at specified intervals (in milliseconds, with sub-millisecond precision).
 * - Functionalities:
 *   - _Periodic sampling_: The `runHandler(handler, delay)` generates endless periodic sampling.
 *     - This corresponds to the `setInterval(handler, delay)` JavaScript mechanism.
 *   - _Sampling stopping_: The `runHandler(handler)` stops the sampling.
 *     - This is equivalent to `clearInterval(handler)` JavaScript mechanism.
 *   - _Timeout handler call_: The `runHandler(handler, delay, 1)` simply delay the handler call.
 *     - This is equivalent to `setTimeout(handler)` JavaScript mechanism.
 * - A second call to `runHandler` with the same index erases the previous handler instance.
 * - When run:
 *   - The `getRunHandlerDelay(index)` function returns an approximate value of the average handler running time, averaged over about 10 samples with an exponential filter.
 *   - The `getRunHandlerLastDelay(index)` function returns the raw last handler running time.
 * - A square-wave generator example:
 * ```
 * void IRAM_ATTR square_wave_handler()
 * {
 *   static bool up_else_down = false;
 *   digitalWrite(outputPinIndex, up_else_down = !up_else_down);
 * }
 * pinMode(outputPinIndex, OUTPUT);
 * digitalWrite(outputPinIndex, 0);
 * runHandler(timerIndex, square_wave_handler, period, count);
 * ```
 * @param {uint} [index = 0] One of the four available timer index.
 * - If index if out of range and a message is issued in the console.
 * @param {callback} handler The `void handler()` function to be called.
 * @param {double} [delay = -1] The interval between two calls, in millisecond (with sub-millisecond precision):
 * - With a delay lower than handler runtime the process is simply run as fast as possible.
 * - With a negative delay, e.g., `-1`, the previously running handler, if any, is stopped.
 * @param {uint} [count = -1] The maximal number of calls, or `-1` to never stop.
 */
void runHandler(handler call, unsigned int index = 0, double delay = -1, unsigned int count = -1);

/**
 * @function square_wave_start
 * @static
 * @description Outputs a square wave on a digital output.
 * @param {uint} timerIndex The given timer index, between 0 and 3.
 * @param {uint} pinIndex The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {double} frequency The wave frequency in KHz.
 * @param {double} duration The duration in msec, or `0` to never stop.
 * @param {bool} [sync = false] If true, this function returns only at the end of the square wave generation.
 */
#define square_wave_start(timerIndex, outputPinIndex, frequency, duration) void IRAM_ATTR square_wave_handler_ ## outputPinIndex() { static bool up_else_down = false; digitalWrite(outputPinIndex, up_else_down = !up_else_down); } pinMode(outputPinIndex, OUTPUT); digitalWrite(outputPinIndex, 0); runHandler(timerIndex, square_wave_handler_ ## outputPinIndex, (int) rint(0.5 / frequency), duration == 0 ? -1 : 2 * (int) rint(duration * frequency)

