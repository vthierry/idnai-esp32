# Installing the ESP32 IDE environment

The [Arduino IDE](https://www.arduino.cc/en/main/software) is used for ESP32 development using this GPIO firmware.

- Download and install the [Arduino IDE](https://www.arduino.cc/en/main/software) either consider this [link](https://www.arduino.cc/en/main/software) or use your package installer (e.g.,  _sudo apt install arduino_).

- Installing the ESP32 board support and additional library
  - Start the Arduino IDE
  - Open File => Preferences => Settings
    - Enter "https://dl.espressif.com/dl/package_esp32_index.json" into Additional Board Manager URLs field. 
    - You can add multiple URLs, separating them with commas.
  - Open `Tools => Boards => Board Manager` window
    - Search `ESP32` in the upper bar
    - Install `ESP32 by Espressif Systems`
  - In Tools => Boards 
    - Select `ESP32 Dev Module`
    - Install
  - In `Sketch => Include Library => Manage Libraries`
    - Select any library you need, e.g.: `analogWrite`,  `Adafruit PWM servo driver library` and click on Install
 - Plug a ESP32 card and select `Tools => Port => /dev/ttyUSB0`
	 - If it is a personal computer `sudo chmod a+rw /dev/ttyUSB0` with a card connected
	 - Else adds to your login account the _dialout_ and _lock_ groups in order to access `/dev/ttyUSB0` devices, this in `/etc/group`.
