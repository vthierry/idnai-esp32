# Using the GPIO web interface

By default this web interface is available for all applications using this framework.

## General purpose functions

- `curl -X GET  http://$IP/hi`: to verify that the web service is started.
- `curl -X GET  http://$IP/loop`: to return the loop delay.
  
- `curl -X GET http://$IP` or `curl -X GET http://$IP/index.html`: to display the `web_page.html`, i.e. the dedicated application web page.
- `curl -X GET http://$IP/gpio.html`: to display the `gpio_web_page.html`, i.e. the GPIO input/output user interface.
  
- `curl -X POST http://$IP/restart`: to restart the web service.
- `curl -X POST http://$IP/sleep`: to halt the web service (processor in deep sleeping mode).

## The GPIO web service

- `curl -X POST -d "index=$index" http://$IP/gpio`: to get a digital input value.
- `curl -X POST -d "index=$index&mode=up" http://$IP/gpio`: to get a 1 digital input value, using a pulldown resistance.
- `curl -X POST -d "index=$index&mode=down" http://$IP/gpio`: to get a 0 digital input value, using a pullup resistance.
- `curl -X POST -d "index=$index&value=(0|1)" http://$IP/gpio`: to set a digital output value.
- `curl -X POST -d "index=$index&mode=analog&resolution=[9-12]&mode=(raw|volt)&range=(0|1|2|3)" http://$IP/gpio`: to get an analog input value.
- `curl -X POST -d "index=$index&mode=analog&value=$value" http://$IP/gpio`: to set an analog output value.
- `curl -X POST -d "index=$index&action=(start|stop|get)&mode=(rising|change|falling)" http://$IP/gpio`: to manage timing control.
- `curl -X POST -d "index=$index&action=wave&frequency=$frequency&duration=$duration" http://$IP/gpio`: to output a square wave.

### Digital input or output arguments

- The index is the GPIO index pin number between 0 and 31.
- The value is the 0 (LOW) or 1 (HIGH) Boolean value.

## Analog input or output arguments

- The index is the GPIO index pin number
  - On input: 4, 12, 13, 14, 15, 24, 25, 26, 32, 34, 36, 39.
  - On output: 25 or 26.
- The value:
  - On input 12bits analog value between 0 and 4095.
  - On output 8 bits analog value between 0 and 255.
- The mode, either the raw value or the value converted in volt.
- The resolution, from 9 to 12 bits, 10 bits corresponds to best real precision.
- The input range:
  - 0: 11dB attenuation gives full-scale voltage 3.3V (for a 3.3V chip), linear between 100 and 2450mV, default.
  - 1: 6dB attenuation gives full-scale voltage 2.2V (for a 3.3V chip), linear between 100 and 1750mV.
  - 2: 2.5dB attenuation gives full-scale voltage 1.5V (for a 3.3V chip), linear between 100 and 1250mV.
  - 3: 0dB attenuation gives full-scale voltage 1.1V (for a 3.3V chip), linear between 100 and 950mV.

### Timing control arguments

- The index is the GPIO index pin number between 0 and 31.
- The post action:
  - `start`: Starts the time occurence measure.
  - `stop`: Stops the time occurence measure, and reset the occurrence time.
  - `get`: Gets the last time occurrence of the signal (default).
- The post action mode:
  - `change`: to trigger the interrupt whenever the pin changes value.
  - `rising`: to trigger when the pin goes from low to high (default).
  - `falling`: to trigger when the pin goes from high to low.
- The last occurrence time:
  - The number of milliseconds passed since the program started, where the last occurence of the input signal change has been register (only one occurrence is stored).

### Square wave generator

- The index is the GPIO index pin number between 0 and 31.
- The frequency in KHz, default is 1 KHz.
- The duration in msec, default is 300 msec.

