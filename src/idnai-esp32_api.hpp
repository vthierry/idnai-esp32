/**
 * @function secho
 * @static
 * @description Returns a string formatted message à-la printf.
 * @param {string} message The message, a string format à-la printf with more parameters if needed.
 * @param {...anything} parameters Any message parameters.
 * @return {string} The formatted string, as a `const char *`.
 */

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
/**
 * @function service_get
 * @static
 * @description Gets a parameter value.
 * @param {string} name The parameter name.
 * @return {string} The parameter value, or the empty string if undefined.
 */
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

/**
 * @function pinMode
 * @static
 * @description Sets the digital value input/output mode.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {IOmode} value Either [INPUT|INPUT_PULLUP|INPUT_PULLDOWN|OUTPUT]
 * - By default, all pins are INPUT.
 * - The [INPUT_PULLUP|INPUT_PULLDOWN] allows to use internal pull-up or pull-down resistances, simplifying the electronics.
 * @param {bool} [check=false] If true, checks that the required pinMode is compatible with the [ESP32 GPIO pin assignment](https://line.gitlabpages.inria.fr/aide-group/esp32gpiocontrol/gpio_pins_usage.html) and echoes an error on the console if not.
 */

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

/**
 * @function analogRawWrite
 * @static
 * @description Outputs an analog value
 * - The `pinMode(index, OUTPUT)` must be called before.
 * - It uses the Digital to Analog on-board Converters (DAC).
 * - This a low-level interface using `dac_oneshot_output_voltage()`.
 * - An alternative is to use a PWM mechanism on a digital output to obtain an average analog value, several libraries are available.
 * @param {uint} index The given pin index: 25 or 26.
 * @param {unit} A 0 to 255 value, for a 0V to 3.3V or 5V voltage, the value is modulo 256.
 * @return {int} The status value:
 * - 0: no error.
 * - -1: bad channel index.
 * - -2: unable to initialize using `dac_oneshot_new_channel()`.
 */

/**
 * @function fmillis
 * @static
 * @description Returns the current time in millisecond at a microsecond precision.
 * - Uses the `micros()` system function and manage overflow.
 * - Using double number precision:
 *   - allows to return a full precision in micro-seconds for about 142 years, using the 52 bit matissa,
 *   - avoids overflow for longer than the universes age.
 * @param {bool} [reset=false] If true, resets the time value to '0' (thus, this call returns ``0´´).
 * @return double The current time, since the processor reset.
 */

/**
 * @function getLoopDelay
 * @static
 * @description Returns an approximate value of the average `loop()` sample time.
 * - It is averaged over about 10 samples with an exponential filter.
 * - The `getLastLoopDelay()` routine returns the raw last delay without filtering.
 * - The usual out of charge loop delay is:
 *   - about 3 to 4 µsec when only the _getLoopDelay_ mechanisms are active.
 *   - about 40 µsec every 10 msec for the web service handle mechanism monitoring, when no request.
 *   - about 15 µsec by char (rough estimation) for a print on the serial interface.
 *   - about 1 msec every 10 sec when in verbose mode to echo the loop-delay on the serial interface.
 * @return double The approximate sample time in milliseconds.
 */

/**
 * @function attachInterrupt
 * @static
 * @description Attaches an interruption to a handler.
 * - It must be called using a construct of the form `attachInterrupt(digitalPinToInterrupt(index), handle);`.
 * - The `pinMode(index, INPUT)` must be called before.
 * - Timing measure example:
 * ```
 * double interruptTime = -1;
 * void IRAM_ATTR timing_handle()
 * {
 *   if(interruptTime == -1)
 *     interruptTime = fmillis();
 * }
 * const static unsigned int interruptIndex =  ?? // in {0, 39};
 * pinMode(index, INPUT_PULLUP);
 * attachInterrupt(digitalPinToInterrupt(interruptIndex), timing_handle, mode);
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

/**
 * @function setInterval
 * @static
 * @description Calls a handler at specified intervals (in milliseconds, with sub-millisecond precision).
 * - Functionalities:
 *   - _Periodic sampling_: This corresponds to the `setInterval(handler, delay)` JavaScript mechanism.
 *   - _Endless sampling_: The `setInterval(handler, delay, -1)` generates endless stopping.
 *   - _Sampling stopping_: The `setInterval(handler, 0)` is equivalent to `clearInterval(handler)` JavaScript mechanism.
 *   - _Timeout handler call_: The `setInterval(handler, delay, 1)` is equivalent to `setTimeout(handler)` JavaScript mechanism.
 * - When calling `setInterval(index)`, the handler is stopped.
 * - A second call to `setInterval` erases the previous handler instance.
 * - The square-wave generator example:
 * ```
 * static const unsigned int outputPinIndex = ?? // in {0, 39};
 * static const double frequency = 0.5; // Kilo-Hertz
 * static const double duration =  1000; // milli-seconds.
 * void IRAM_ATTR square_wave_handler()
 * {
 *   static bool up_else_down = false;
 *   digitalWrite(outputPinIndex, up_else_down = !up_else_down);
 * }
 * static const double period = (int) rint(0.5 / frequency); // … because a period is made of 2 steps
 * static const unsigned int count = 2 * (int) rint(duration * frequency);
 * pinMode(outputPinIndex, OUTPUT);
 * digitalWrite(outputPinIndex, 0);
 * static const unsigned int timerIndex = ?? // in {0, 4{;
 * setInterval(timerIndex, square_wave_handler, period, count);
 * ```
 * @param {uint} [index = 0] One of the four available timer index. If `index >= 4` nothing happens.
 * @param {callback} handler The `void handler()` routine to be called.
 * @param {double} [delay = 0] The intervals, in millisecond, on how often to execute the code, or `0` to stop the sampling.
 * @param {uint} [count = -1] The maximal number of calls, or `-1` to never stop.
 */

/**
 * @function square_wave_start
 * @static
 * @description Outputs a square wave on a digital output.
 * @param {uint} timerIndex The given timer index, between 0 and 3.
 * @param {uint} pinIndex The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {double} frequency The wave frequency in KHz.
 * @param {double} duration The duration in msec, or `0` to never stop.
 * @param {bool} [sync = false] If true, this routine returns only at the end of the square wave generation.
 */
