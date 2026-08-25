# The idnai-esp32 specific development rules

BUILD_ESP32 = $(patsubst %_h.html,%.h,$(wildcard *_h.html))

BUILD += $(BUILD_ESP32)

ifneq (,which xdotools)
build_esp32: # build_esp32: Starts the idnai-esp32 IDE compilation, upload and run.
	if xdotool search --name "Arduino IDE" windowactivate key --clearmodifiers --delay 100 ctrl+shift+m ctrl+shift+m ctrl+u ; then echo "Compiling and Uploading on the Arduino IDE" ; fi
else
	echo "Runs Sketch => Upload` (`CTRL+U` as shortcut) item of the `Arduino IDE` menu."
endif

## - Uses the xdotools, if installed, to trigger the Arduino IDE command, else to be done by hand.

show_esp32: # show_esp32: Opens the idnai-esp32 web pages, setting the MAC board address if not yet done.
ifeq (,$(shell isItOn $(NAME))
	read -p "Please enter the MAC board address: " MAC ; if confirm "MAC=$$MAC OK" "Fine, please rerun" ; then isItOn $(NAME)=$$MAC > /dev/null ; fi
endif
	IP="`isItOn $(NAME)`" ;\
	$(BROWSER) http://$$IP/ ;\
	$(BROWSER) http://$$IP/gpio.html

## - Uses the isItOn utility to register the MAC address under the package name in 'node_modules/idnai-make/bin/.isItOn' file.
##   - It is assumed that one package corresponds to a given ESP32 MAC address, for the tests.
##   - Editing 'node_modules/idnai-make/bin/.isItOn' allows to adapt to another setup.
