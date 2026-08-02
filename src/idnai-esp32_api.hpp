/**
 * @function secho
 * @static
 * @description Returns a string formatted message à-la printf.
 * @param {string} message The message, a string format à-la printf with more parameters if used.
 * @param {...anything} parameters Any message parameters.
 * @return {string} The formatted string, as a `const char *`.
 */

/**
 * @function echo
 * @static
 * @description Prepares to print on the console a formatted message à-la printf.
 * - The serial output buffer is not send to the serial interface directy, but in the `loop()`, avoiding to slow down interruption code.
 * @param {string} message The message, a string format à-la printf with more parameters if used.
 * @param {...anything} parameters Any message parameters.
 */
/**
 * @function echoFlush
 * @static
 * @description Flushes the pending echo messages.
 * - The serial output buffer is not flushed by default, to avoid slowing down the code, thus output may be unsynchronized, while `Serial.flush();` waits for the transmission of outgoing serial data to complete.
 * - This routines is called in the `loop()` outside interruption code, to regularly flush the echo output..
 */
/**
 * @function answer
 * @static
 * @description Answers to http request by a formatted json message à-la printf.
 * - In verbose mode the message is also output on the console.
 * @param {bool} ok If true answer ok (code 200) else error (code 400).
 * @param {string} message The message, a json string format à-la printf with more parameters if used.
 * @param {...anything} parameters Any message parameters.
 */

/**
 * @constant VERBOSE
 * @type {bool}
 * @static
 * @default
 * @description Defines whether code is run in verbose mode.
 * - The tag is a boolean defined in the `verbose.h` local file, to set to true or false.
 * - The construct `if (VERBOSE) then { echo(fmt, …) }` allows to output a message with parameters, only in verbose mode.
 * - In verbose mode, several messages are output on the console, thus slowing down the code.
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
 * @param {bool} [check=false] If true, checks that the required pinMode is compatible with the [ESP32 GPIO pin assignment](https://line.gitlabpages.inria.fr/aide-group/esp32gpiocontrol/gpio_pins_usage.html).
 * - If an assignment error occurs, the global variable `String pinModeError` is set to the corresponding error message.
 *   - Therefore, `pinModeError != ""` equals `true` when an error occurs.
 *   - It is echoed on the serial interface in verbose mode.
 *   - It is included in webservice answers when using the gpiocontrol web interface.
 *   - The `pinModeError` variable is available until the next `pinMode(index, mode, true)` call.
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
 * @function attachInterrupt
 * @static
 * @description Attaches an interruption to a handler.
 * - It must be called using a construct of the form `attachInterrupt(digitalPinToInterrupt(index), handle);`.
 * - The `pinMode(index, INPUT)` must be called before.
 * - The handler is defined by a construct of the form:
 * ```
 * void IRAM_ATTR handle()
 * {
 *   … // handle's code
 * }
 * ```
 * - If the `handle()` is too long then a fatal error `Guru Meditation Error: … Exception was unhandled.` occurs and it reboots.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {handler} handle A `void (*handler)(void)` handle function.
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
 * @function fmillis
 * @static
 * @description Returns the current time in millisecond at a microsecond precisioin.
 * - Uses the `micros()` system function and manage overflow.
 * - Using double number precision:
 *   - allows to return a full precision in micro-seconds for about 142 years, using the 52 bit matissa,
 *   - avoids overflow for longer than the universes age.
 * @param {bool} [reset=false] If true, resets the time value to '0' (thus, this call returns ``0´´).
 * @return double The current time.
 */

/**
 * @function getLoopDelay
 * @static
 * @description Returns an approximate value of the average `loop()` sample time.
 * - It is averaged over about 10 samples with an exponential filter.
 * - The `getLastLoopDelay()` routine returns the raw last delay without filtering.
 * - The usual out of charge loop delay is:
 *   - about 3 to 4 µsec when only the _getLoopDelay_ and _setInterval_ mechanisms are active.
 *   - about 40 µsec every 10 msec for the webserver client handle mechanism monitoring, when no request.
 *   - about 15 µsec by char (rough estimation) for a print on the serial interface.
 *   - about 1 msec every 10 sec when in verbose mode to echo the loop-delay on the serial interface.
 * @return double The approximate sample time in milliseconds.
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
 * @param {callback} handler The `void handler()` routine to be called.
 * - Only one instance of a given handler is taken into account.
 *  - A second call to `setInterval` erases the previous handler instance.
 * @param {double} [delay = 0] The intervals, in millisecond, on how often to execute the code, or `0` to stop the sampling.
 * - This is performs in "best effort" mode, i.e., as soon as possible.
 * - The 1st call is performed after the given delay.
 * - When calling `setInterval(handler)`, the handler is stopped.
 * @param {uint} [count = -1] The maximal number of calls.
 */

/**
 * @function gpio_digital_timing_start
 * @static
 * @description Starts the digital timing observation.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {uint} [mode=RISING] The transition `CHANGE|FALLING|RISING` mode.
 * @param {handler} [callback = NULL] The `void handle()` routine, if any, to be called at event occurence.
 */

/**
 * @function gpio_digital_timing_stop
 * @static
 * @description Stops the digital timing observation.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 */

/**
 * @function gpio_digital_timing_get
 * @static
 * @description Gets the last digital timing observation.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @return {double} The last observation time in millisconds from the chip boot.
 */

/**
 * @function gpio_square_wave
 * @static
 * @description Outputs a square wave on a digital output.
 * - Each digital output can have its own square wave generated.
 * - The signal starts with a 1 and ends with a 0.
 * @param {uint} index The given pin index, between 0 and 39.
 * - Not all indexes are usable, see the [GPIO pins usage](gpio_pins_usage.html).
 * @param {double} frequency The wave frequency in KHz.
 * @param {double} duration The duration in msec.
 * @param {bool} [sync = false] If true, this routine returns only at the end of the square wave generation.
 */
