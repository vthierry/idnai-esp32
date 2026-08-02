#!/bin/bash

echo "Runs a few http tests on a running ESP32 with the gpio framework loaded."

IP="`cat IP.txt`"

hostname="`basname pwd`"

read -p "Is the EPS32 connected with IP='$IP', with the gpio framework loaded? (y/N): " -n 1 -e cont ; if [ "$cont" \!= "y" ] ; then exit ; fi

echo "Restarts '$hostname'" ; curl -m 10 -X POST http://$IP/restart ; echo ; sleep 15

echo "Tests if '$hostname' is awake and what is the loop speed" ; $BROWSER http://$IP/loop

echo "Opens the '$hostname' webpages" ; $BROWSER http://$IP/index.html ; $BROWSER http://$IP/gpio.html

echo "Tests the LED 13 blink" ; for n in {0..7} ; do v="$(($n % 2))" ; curl -m 10 -X POST -d "index=13&value=$v" http://$IP/gpio ; echo ; sleep 1 ; done

echo "Tests the square wave on LED 13" ; curl -m 10 -X POST -d "index=13&action=wave&frequency=0.0005&duration=10000" http://$IP/gpio ; echo ; sleep 10 ; curl -m 10 -X POST -d "index=13&action=wave&frequency=0&duration=0" http://$IP/gpio ; echo

echo "Tests the software timing" ; curl -m 10 -X POST -d "index=13&action=start&mode=rising" http://$IP/gpio ; echo ; sleep 1 ; curl -m 10 -X POST -d "index=13&action=stop" http://$IP/gpio ; echo
