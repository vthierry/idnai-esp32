# The idnai-esp32 specific development rules

BUILD_ESP32 = build_esp32

%.hpp: %.html
	@(echo -n "static char $*[] = \"" ; tr "\n" " " < $^ | sed 's/"/\\"/g' | sed 's/  */ /g' ; echo "\";") > $@

ifneq (,which xdotools)
build_esp32: # build_esp32: Starts the idnai-esp32 IDE compilation, upload and run.
	if xdotool search --name "Arduino IDE" windowactivate key --clearmodifiers --delay 100 ctrl+shift+m ctrl+shift+m ctrl+u ; then echo "Compiling and Uploading on the Arduino IDE" ; fi
else
	echo "Runs Sketch => Upload` (`CTRL+U` as shortcut) item of the `Arduino IDE` menu."
endif

show_esp32: # show_esp32: Opens the idnai-esp32 web pages, after setting the MAC or IP board address.
ifneq (,which arp-scan)
ifeq (,ls MAC.txt)
	read -p "Please enter the MAC board address: " MAC ; echo "$$MAC" > MAC.txt
endif
	sudo arp-scan -lqx | grep "`cat MAC.txt`" | awk '{print $$1}' > IP.txt
endif
ifneq (,ls IP.txt)
	read -p "The IP board address is "`cat IP.txt`", ok ? (y/n): " ok ; if [ "ok" \!= "y" ] ; /bin/rm -f IP.txt ; fi
endif
ifeq (,ls IP.txt)
	read -p "Please enter the IP board address: " IP ; echo "$$IP" > IP.txt
endif
	IP="`cat IP.txt`" ;\
	$(BROWSER) http://$(IP)/index.html ;\
	$(BROWSER) http://$(IP)/gpio.html
