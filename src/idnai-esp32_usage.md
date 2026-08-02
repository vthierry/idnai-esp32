# Using the Arduino IDE to develop with the idnai-esp32 framework

The gpio framework is a minimal set of source files to ease the development of ESP32 GPIO connected objects.

## Starting

- Connect the ESP32 card via the USB cable.
- Start the `Arduino IDE`.
  - In `Tools => Port` select /dev/ttyUSB0 at `115200 baud`.
     - In `Tools => Serial Monitor` you can visualize the server dump output.
  - In `File => Open`, browse and select the `*.ino` files of the desired package.
- Run `make where_esp32` with the circuit off and then on.
  - … allowing to find out which IP appears, thus corresponding to the circuit.
  - The IP is also given on the serial line after upload.
  - Saves the IP in the `IP.txt` file, only the IP, no more texte.
  
## Development loop

- Edit the `setup_handles.cpp` and optional additional files.
- Run `make build`.
  - This allows in the `Arduino IDE` to [re]compile the package firmware and load it onto the connected ESP32.
  - This corresponds to the `Sketch => Upload` (`CTRL+U` as shortcut) item of the `Arduino IDE` menu.
  - The red LED of the EPS32 is turn on, after a few blinks, if the setup is correct.
- Run `make show_esp32` to open the index.html et gpio.html web interface on the current `$BROWSER`.
	
## More on make rules

- The `web_page.html` page is unglyfied in a `web_page.hpp` to be used directly on the ESP32.
  - The page is rendered when connected to the ESP32 web server as `http:/$IP:$port/index.html`.
