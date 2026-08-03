///////////////////////////////////////////////////////////////////////////////
///  This file contains all adnai-esp32 framework procedures  ///
///////////////////////////////////////////////////////////////////////////////
// CAUTION => do not modify unless you know why and how ;)

// Here are the setup parameters

// Defines whether the wifi connection uses the WiFi.scanNetworks() method (recommended)
// … or attempt to test each wifi.h line (to be used as a fallback).
#define WIFI_SCAN_METHOD 1

// Defines the server listen port
// - port 80 is the default port for unencrypted HTTP traffic but is often blocked by firewalls or ISPs.
// - port 8080 is often used as an alternative HTTP port for web servers and applications and is almost never blocked.
#define SERVER_LISTEN_PORT 8080

// This is the version of the current framework
#define IDNAI_ESP32_VERSION "1.0.1"

// This defines the hostname
#define "hostname.h"

////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdarg.h>

// Implements the echo buffering management

struct ManageEcho {
  // Variable argument à-la printf mechanism
#define arg2chars_buffer() va_list a; va_start(a, fmt); manageEcho.arg2chars_buffer_length = vsnprintf(manageEcho.arg2chars_buffer, manageEcho.arg2chars_buffer_size, fmt, a); va_end(a);
  const static unsigned int arg2chars_buffer_size = 256;
  unsigned int arg2chars_buffer_length = 0;
  char arg2chars_buffer[arg2chars_buffer_size + 1] = "";

  // Echo buffering mechanism
  const static unsigned int echo_chars_buffer_size = 4096, echo_chars_buffer_alert_size = 23;
  unsigned int echo_chars_buffer_length = 0;
  char echo_chars_buffer[echo_chars_buffer_size + echo_chars_buffer_alert_size + 2] = "";
  static constexpr char echo_chars_buffer_return[2] = "\n";
  static constexpr char echo_chars_buffer_alert[echo_chars_buffer_alert_size + 1] = "¡echo buffer overflow!";

  void echoAdd()
  {
    // Adds a \n between two echos
    if(echo_chars_buffer_length > 0) {
      memcpy(echo_chars_buffer + echo_chars_buffer_length, echo_chars_buffer_return, 2);
      echo_chars_buffer_length++;
    }
    // Buffers the current echo
    if(echo_chars_buffer_length + arg2chars_buffer_length < echo_chars_buffer_size) {
      memcpy(echo_chars_buffer + echo_chars_buffer_length, arg2chars_buffer, arg2chars_buffer_length + 1);
      echo_chars_buffer_length += arg2chars_buffer_length;
    } else {
      // Adds once the alert if the buffer overflows
      if(echo_chars_buffer_length != echo_chars_buffer_size + echo_chars_buffer_alert_size) {
	memcpy(echo_chars_buffer + echo_chars_buffer_length, echo_chars_buffer_alert, echo_chars_buffer_alert_size + 1);
        echo_chars_buffer_length = echo_chars_buffer_size + echo_chars_buffer_alert_size;
      }
    }
  }
  void echoFlush()
  {
    // Flushes the echo buffer if not emtpy
    if(echo_chars_buffer_length > 0) {
      Serial.println(echo_chars_buffer);
      echo_chars_buffer[0] = '\0', echo_chars_buffer_length = 0;
    }
  }
}
manageEcho;

void echo(const char *fmt, ...)
{
  arg2chars_buffer();
  manageEcho.echoAdd();
}
void echoFlush()
{
  manageEcho.echoFlush();
}
const char *secho(const char *fmt, ...)
{
  arg2chars_buffer();
  return manageEcho.arg2chars_buffer;
}
// Webserver answer mechanism

void answer(bool ok, const char *fmt, ...)
{
  arg2chars_buffer();
  server->sendHeader("Access-Control-Allow-Origin", "*");
  server->sendHeader("Access-Control-Allow-Methods", "GET, PUT, POST");
  server->send(ok ? 200 : 400, "application/json", manageEcho.arg2chars_buffer);
  if(VERBOSE) {
    manageEcho.echoAdd();
  }
}

////////////////////////////////////////////////////////////////////////////////

// Implements the GIO extension interface

String pinModeError = "";

// Analog interface

void pinMode(unsigned int index, unsigned int mode, bool check)
{
  if(check) {
    bool analog = (mode & 0x10) != 0, input = (mode & 0x0F) != 3, pulling = (mode & 0x0C) != 0;
    // Notice: OUTPUT: 1+2, INPUT: 1, INPUT_PULLUP: 1+4, INPUT_PULLDOWN: 1+8, ANALOG_(IN|OUT)PUT = (IN|OUT)PUT | 0x10
    String modeValueError;
    switch(mode) {
    case INPUT:
    case OUTPUT:
    case INPUT_PULLUP:
    case INPUT_PULLDOWN:
    case ANALOG_INPUT:
    case ANALOG_OUTPUT:
      modeValueError = "";
      break;
    default:
      modeValueError = "Spurious mode value";
      break;
    }
    pinModeError = "";
    if(analog) {
      if(input) {
        if(!(32 <= index && index <= 39)) {
          pinModeError = "Invalid analog input index, not in {32,39} (ADC2 is used for wifi)";
        }
      } else {
        if(!(24 <= index && index <= 26)) {
          pinModeError = "Invalid analog output index, not in {25,26}";
        }
      }
    } else {
      if(!(4 == index || (12 <= index && index <= 15) || (18 <= index && index <= 27) || (32 <= index && index <= 39))) {
        pinModeError = "Invalid digital input index, not in {4} + {12,15} + {18,27} + {32,39}, reserved for other usage";
      }
      if(34 <= index && index <= 39) {
        if(input) {
          if(pulling) {
            pinModeError = "No pulling interface for input with an index in {32,39}";
          }
        } else {
          pinModeError = "No output interface with an index in {34,39}";
        }
      }
    }
    if(modeValueError != "" || pinModeError != "") {
      pinModeError = secho("pinMode(%d, 0x%x (%s-%s%s)) error: '%s%s%s'", index, mode, analog ? "analog" : "digital", input ? "input" : "output", pulling ? "-pulling" : "", modeValueError.c_str(), modeValueError != "" && pinModeError != "" ? ". " : "", pinModeError.c_str());
    }
    if(VERBOSE && pinModeError != "") {
      echo(pinModeError.c_str());
    }
  }
  pinMode(index, mode & 0xF);
}
#include "driver/dac_oneshot.h"

int analogRawWrite(unsigned int index, unsigned int value)
{
  if(index == 25 || index == 26) {
    static bool init[2] = { false, false };
    static dac_oneshot_config_t config[2];
    static dac_oneshot_handle_t handle[2];
    unsigned int i = index - 25;
    if(!init[i]) {
      config[i].chan_id = i == 0 ? DAC_CHAN_0 : DAC_CHAN_1;
      int s = dac_oneshot_new_channel(&config[i], &handle[i]);
      if(s != ESP_OK) {
        return -2;
      }
      init[i] = true;
    }
    dac_oneshot_output_voltage(handle[i], value < 255 ? value : 255);
    return true;
  } else {
    return -1;
  }
}

////////////////////////////////////////////////////////////////////////////////

// Implements current time and loop delay.

#include <math.h>
#include <climits>
#include <map>
#include <set>
#include "time.h"

double loopDelay = 0, lastLoopDelay = 0;

double fmillis(bool reset)
{
  /*
   *  - Value analysis
   *   - The micros() function:
   *     - returns a number between 0 to 2^32 - 1 = 4294967295,
   *     - rolls back to 0 when reaching the maximum, after 71.5 minutes
   *     (for millis(), same 2^32 - 1 overflow, after approximately 49.7 days, and millis() == micros() / 1000, exactly).
   *   - A double has a 52 bits mantissa and a 11 bit exponent.
   *     - Full precision is thus maintained for 2^52/10^6/3600/24/365.25 = 142 years.
   *     - Maximal value is 2^52x2^(2^11-1023)/10^6/3600/24/365.25 > the 14 x 10^9 years universe age.
   *  - Overflow management:
   *   - This fmillis() function must be called about every hour, otherwise overflow is missed.
   *   - Since in it is called by updateLoopDelay() the condition is fulfilled.
   */
  static double offset = 0;
  static unsigned long previous = 0;
  unsigned long now = micros();
  if(reset) {
    offset = -0.001 * now;
  } else {
    if(now + 1000000 < previous) {
      if(VERBOSE) {
        echo("{fmillis overflow offset: %.3f previous: %ld now: %ld}", offset, previous, now);
      }
      offset += 4294967.295;
    }
  }
  previous = now;
  return offset + 0.001 * now;
}
double getLoopDelay()
{
  return loopDelay;
}
double getLastLoopDelay()
{
  return lastLoopDelay;
}
void updateLoopDelay()
{
  /*
   *  - A first order average filter is used here, as computed by this Maple piexe of code
   *  ```
   # First order exponential filter
   #  rsolve({z(n) = z(n-1) + a * (x(n) - z(n-1)), z(-1) = 0}, {z(n)}) assuming 0 < a, a < 1;
   # Rewritting the formula
   #  z := n -> sum(a * (1 - a)^(n-k) * x(k), k = -infinity .. n):
   # Verifying the filter is normalized
   #  one = simplify(sum(a * (1 - a)^k, k = 0 .. infinity)) assuming 0 < a, a < 1;
   # Solving with respect to a for a window of size K to yield p in % of the filter input.
   #  a := 1 - (1 - p)^(1/K):
   #  zero = simplify(p - sum(a * (1 - a)^k, k = 0 .. K-1)) assuming 0 < K, 0 < p, p < 1;
   # Evaluating that a = 0.2, a window of size K = 10 yields p = 90% of the filter input.
   #  evalf(subs(p = 0.9, K = 10, a));
   #  ```
   */
  const static double a = 0.2;
  static double previous = 0;
  double now = fmillis();

  /* Updates the loop-delay if in verbose mode, this, only every 10 sec
   *  if(VERBOSE && millis() % 10000 == 0) {
   *  echo("{ updateLoopDelay now: %.3f average-loop-delay: %.3f last-loop-delay: %.3f }", now, loopDelay, lastLoopDelay);
   *  previous = fmillis();
   *  } else
   */
  lastLoopDelay = now - previous, previous = now, loopDelay += a * (lastLoopDelay - loopDelay);
}


////////////////////////////////////////////////////////////////////////////////

// Implements the timing mechanisms

// Event's data structure
class IntervalEvents {
  // Defines an event;
  struct Event {
    // Event handler routine
    handler handler_routine;
    // Inter event delay
    double delay = 0;
    // Number of event to be fired
    unsigned int count = 0;
    // Next firing time
    double nextTime = 0;
    // Constructs an event for a given handler routine
    Event(const handler handler_routine) : handler_routine(handler_routine) {}
    String stringify() const
    {
      return secho("{ IntervalEvent handler: 0x%lx delay: %.3f count: %d nextTime: %.3f to-be-fired: %d }", (unsigned long) handler_routine, delay, count, nextTime, nextTime <= fmillis());
    }
  };
  struct cmp {
    bool operator() (const Event *e1, const Event *e2) const {
      return e1->nextTime < e2->nextTime;
    }
  };
  // Event's buffer sorted by next firing time
  std::map < handler, Event > intervalEvents;
  std::set < Event *, cmp > nextEvents;

  void purgeNextEvents(const handler handler_routine)
  {
    for(auto it = nextEvents.begin(); it != nextEvents.end(); it++) {
      if((*it)->handler_routine == handler_routine) {
        nextEvents.erase(it);
        return;
      }
    }
  }
public:
  // Inserts an event.
  void insert(const handler handler_routine, double delay, unsigned int count)
  {
    purgeNextEvents(handler_routine);
    auto it = intervalEvents.find(handler_routine);
    if(it == intervalEvents.end()) {
      Event event(handler_routine);
      it = intervalEvents.insert(std::pair < handler, Event > (handler_routine, event)).first;
    }
    it->second.delay = delay, it->second.count = count, it->second.nextTime = fmillis() + delay;
    nextEvents.insert(&(it->second));
  }
  // Erases an event handler.
  void erase(const handler handler_routine)
  {
    purgeNextEvents(handler_routine);
    intervalEvents.erase(handler_routine);
  }
  // Updates the event data structure, firing an event if it is time.
  void update()
  {
    // If any event …
    if(nextEvents.size() > 0) {
      // Gets the 1st incoming event.
      auto it = nextEvents.cbegin();
      // Fires if it time to do it;
      double now = fmillis();
      if((*it)->nextTime <= now) {
        (*it)->handler_routine();
        nextEvents.erase(it);
        // Updates the next event
        (*it)->nextTime = now + (*it)->delay;
        if((*it)->count != (unsigned int) -1) {
          (*it)->count--;
        }
        // Inserts the next event occurence, if any
        if((*it)->count > 0) {
          nextEvents.insert(*it);
        }
      }
    }
  }
  // Returns as a string
  String stringify() const
  {
    String s = secho("{ now: %.3f nextEvents: [\n", fmillis());
    for(auto it = nextEvents.begin(); it != nextEvents.end(); it++) {
      s += "\t" + (*it)->stringify() + "\n";
    }
    return s + "]}\n";
  }
}
intervalEvents;

void setInterval(handler handler_routine, double delay, unsigned int count)
{
  if(delay == 0 || count == 0) {
    intervalEvents.erase(handler_routine);
  } else {
    intervalEvents.insert(handler_routine, delay, count);
  }
}
void updateSetInterval()
{
  intervalEvents.update();
}

////////////////////////////////////////////////////////////////////////////////

// Implements the digital timing functions

volatile double gpio_timing_times[MAX_GPIO_INDEX] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile handler gpio_timing_callback_handler[MAX_GPIO_INDEX] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
#define GPIO_TIMING_HANDLER(index) \
        void IRAM_ATTR gpio_timing_handler_ ## index() \
        { \
          gpio_timing_times[index] = fmillis(); \
          if(gpio_timing_callback_handler[index] != NULL) { \
            gpio_timing_callback_handler[index](); \
          } \
        }
GPIO_TIMING_HANDLER(0)
GPIO_TIMING_HANDLER(1)
GPIO_TIMING_HANDLER(2)
GPIO_TIMING_HANDLER(3)
GPIO_TIMING_HANDLER(4)
GPIO_TIMING_HANDLER(5)
GPIO_TIMING_HANDLER(6)
GPIO_TIMING_HANDLER(7)
GPIO_TIMING_HANDLER(8)
GPIO_TIMING_HANDLER(9)
GPIO_TIMING_HANDLER(10)
GPIO_TIMING_HANDLER(11)
GPIO_TIMING_HANDLER(12)
GPIO_TIMING_HANDLER(13)
GPIO_TIMING_HANDLER(14)
GPIO_TIMING_HANDLER(15)
GPIO_TIMING_HANDLER(16)
GPIO_TIMING_HANDLER(17)
GPIO_TIMING_HANDLER(18)
GPIO_TIMING_HANDLER(19)
GPIO_TIMING_HANDLER(20)
GPIO_TIMING_HANDLER(21)
GPIO_TIMING_HANDLER(22)
GPIO_TIMING_HANDLER(23)
GPIO_TIMING_HANDLER(24)
GPIO_TIMING_HANDLER(25)
GPIO_TIMING_HANDLER(26)
GPIO_TIMING_HANDLER(27)
GPIO_TIMING_HANDLER(28)
GPIO_TIMING_HANDLER(29)
GPIO_TIMING_HANDLER(30)
GPIO_TIMING_HANDLER(31)
GPIO_TIMING_HANDLER(32)
GPIO_TIMING_HANDLER(33)
GPIO_TIMING_HANDLER(34)
GPIO_TIMING_HANDLER(35)
GPIO_TIMING_HANDLER(36)
GPIO_TIMING_HANDLER(37)
GPIO_TIMING_HANDLER(38)
GPIO_TIMING_HANDLER(39)
volatile handler gpio_timing_handlers[MAX_GPIO_INDEX] = {
  gpio_timing_handler_0,
  gpio_timing_handler_1,
  gpio_timing_handler_2,
  gpio_timing_handler_3,
  gpio_timing_handler_4,
  gpio_timing_handler_5,
  gpio_timing_handler_6,
  gpio_timing_handler_7,
  gpio_timing_handler_8,
  gpio_timing_handler_9,
  gpio_timing_handler_10,
  gpio_timing_handler_11,
  gpio_timing_handler_12,
  gpio_timing_handler_13,
  gpio_timing_handler_14,
  gpio_timing_handler_15,
  gpio_timing_handler_16,
  gpio_timing_handler_17,
  gpio_timing_handler_18,
  gpio_timing_handler_19,
  gpio_timing_handler_20,
  gpio_timing_handler_21,
  gpio_timing_handler_22,
  gpio_timing_handler_23,
  gpio_timing_handler_24,
  gpio_timing_handler_25,
  gpio_timing_handler_26,
  gpio_timing_handler_27,
  gpio_timing_handler_28,
  gpio_timing_handler_29,
  gpio_timing_handler_30,
  gpio_timing_handler_31,
  gpio_timing_handler_32,
  gpio_timing_handler_33,
  gpio_timing_handler_34,
  gpio_timing_handler_35,
  gpio_timing_handler_36,
  gpio_timing_handler_37,
  gpio_timing_handler_38,
  gpio_timing_handler_39
};
void gpio_digital_timing_start(unsigned int index, unsigned int mode, handler callback)
{
  if(index < MAX_GPIO_INDEX && (4 == index || (12 <= index && index <= 15) || (18 <= index && index <= 27) || (32 <= index && index <= 33))) {
    if(mode == RISING || mode == CHANGE || mode == FALLING) {
      gpio_timing_times[index] = 0;
      gpio_timing_callback_handler[index] = callback;
      pinMode(index, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(index), gpio_timing_handlers[index], mode);
    } else {
      pinModeError = secho("Error in gpio_digital_timing_start: Spurious mode: %d value, instead of RISING: %d, CHANGE: %d or FALLING: %d", mode, RISING, CHANGE, FALLING);
    }
  } else {
    pinModeError = secho("Error in gpio_digital_timing_start: Spurious index: %d value, only value in {4} + {12,15} + {18,27} + {32,33} are available for interrupt management", index);
  }
  if(VERBOSE && pinModeError != "") {
    echo(pinModeError.c_str());
  }
}
void gpio_digital_timing_stop(unsigned int index)
{
  if(index < MAX_GPIO_INDEX) {
    detachInterrupt(digitalPinToInterrupt(index));
  } else {
    if(VERBOSE) {
      echo("Error in gpio_digital_timing_stop: Spurious index: %d value", index);
    }
  }
}
double gpio_digital_timing_get(unsigned int index)
{
  if(index < MAX_GPIO_INDEX) {
    return gpio_timing_times[index];
  } else {
    if(VERBOSE) {
      echo("Error in gpio_digital_timing_stop: Spurious index: %d value", index);
    }
    return 0;
  }
}
//
// Software square wave generation mechanism
//

#define GPIO_SQUARE_WAVE_HANDLER(index) \
        void IRAM_ATTR gpio_square_wave_handler_ ## index() \
        { \
          static bool up_else_down = false; \
          digitalWrite(index, up_else_down = !up_else_down); \
        }
GPIO_SQUARE_WAVE_HANDLER(0)
GPIO_SQUARE_WAVE_HANDLER(1)
GPIO_SQUARE_WAVE_HANDLER(2)
GPIO_SQUARE_WAVE_HANDLER(3)
GPIO_SQUARE_WAVE_HANDLER(4)
GPIO_SQUARE_WAVE_HANDLER(5)
GPIO_SQUARE_WAVE_HANDLER(6)
GPIO_SQUARE_WAVE_HANDLER(7)
GPIO_SQUARE_WAVE_HANDLER(8)
GPIO_SQUARE_WAVE_HANDLER(9)
GPIO_SQUARE_WAVE_HANDLER(10)
GPIO_SQUARE_WAVE_HANDLER(11)
GPIO_SQUARE_WAVE_HANDLER(12)
GPIO_SQUARE_WAVE_HANDLER(13)
GPIO_SQUARE_WAVE_HANDLER(14)
GPIO_SQUARE_WAVE_HANDLER(15)
GPIO_SQUARE_WAVE_HANDLER(16)
GPIO_SQUARE_WAVE_HANDLER(17)
GPIO_SQUARE_WAVE_HANDLER(18)
GPIO_SQUARE_WAVE_HANDLER(19)
GPIO_SQUARE_WAVE_HANDLER(20)
GPIO_SQUARE_WAVE_HANDLER(21)
GPIO_SQUARE_WAVE_HANDLER(22)
GPIO_SQUARE_WAVE_HANDLER(23)
GPIO_SQUARE_WAVE_HANDLER(24)
GPIO_SQUARE_WAVE_HANDLER(25)
GPIO_SQUARE_WAVE_HANDLER(26)
GPIO_SQUARE_WAVE_HANDLER(27)
GPIO_SQUARE_WAVE_HANDLER(28)
GPIO_SQUARE_WAVE_HANDLER(29)
GPIO_SQUARE_WAVE_HANDLER(30)
GPIO_SQUARE_WAVE_HANDLER(31)
GPIO_SQUARE_WAVE_HANDLER(32)
GPIO_SQUARE_WAVE_HANDLER(33)
GPIO_SQUARE_WAVE_HANDLER(34)
GPIO_SQUARE_WAVE_HANDLER(35)
GPIO_SQUARE_WAVE_HANDLER(36)
GPIO_SQUARE_WAVE_HANDLER(37)
GPIO_SQUARE_WAVE_HANDLER(38)
GPIO_SQUARE_WAVE_HANDLER(39)
volatile handler gpio_square_wave_handlers[MAX_GPIO_INDEX] = {
  gpio_square_wave_handler_0,
  gpio_square_wave_handler_1,
  gpio_square_wave_handler_2,
  gpio_square_wave_handler_3,
  gpio_square_wave_handler_4,
  gpio_square_wave_handler_5,
  gpio_square_wave_handler_6,
  gpio_square_wave_handler_7,
  gpio_square_wave_handler_8,
  gpio_square_wave_handler_9,
  gpio_square_wave_handler_10,
  gpio_square_wave_handler_11,
  gpio_square_wave_handler_12,
  gpio_square_wave_handler_13,
  gpio_square_wave_handler_14,
  gpio_square_wave_handler_15,
  gpio_square_wave_handler_16,
  gpio_square_wave_handler_17,
  gpio_square_wave_handler_18,
  gpio_square_wave_handler_19,
  gpio_square_wave_handler_20,
  gpio_square_wave_handler_21,
  gpio_square_wave_handler_22,
  gpio_square_wave_handler_23,
  gpio_square_wave_handler_24,
  gpio_square_wave_handler_25,
  gpio_square_wave_handler_26,
  gpio_square_wave_handler_27,
  gpio_square_wave_handler_28,
  gpio_square_wave_handler_29,
  gpio_square_wave_handler_30,
  gpio_square_wave_handler_31,
  gpio_square_wave_handler_32,
  gpio_square_wave_handler_33,
  gpio_square_wave_handler_34,
  gpio_square_wave_handler_35,
  gpio_square_wave_handler_36,
  gpio_square_wave_handler_37,
  gpio_square_wave_handler_38,
  gpio_square_wave_handler_39
};

void gpio_square_wave(unsigned int index, double frequency, double duration, bool sync)
{
  if(index < MAX_GPIO_INDEX && (4 == index || (12 <= index && index <= 15) || (18 <= index && index <= 27) || (32 <= index && index <= 33))) {
    pinMode(index, OUTPUT);
    digitalWrite(index, 0);
    static const double period = (int) rint(0.5 / frequency); // … because a period is made of 2 steps
    static const unsigned int count = 2 * (int) rint(duration * frequency);
    setInterval(gpio_square_wave_handlers[index], period, count);
    if(sync) {
      delay(ceil(duration));
    }
  } else {
    pinModeError = secho("Error in gpio_square_wave: Spurious index: %d value, only value in {4} + {12,15} + {18,27} + {32,33} are available for digital output", index);
    if(VERBOSE) {
      echo(pinModeError.c_str());
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

// Implements the web service bases

#include <WiFi.h>
#include "wifi.h"
#include <WebServer.h>
WebServer *server = NULL;

void updateWebServer()
{
  // Handles queries, if the server is defined, every 10 msec
  if(server != NULL && micros() % 10000 == 0) {
    server->handleClient();
  }
}

////////////////////////////////////////////////////////////////////////////////

// Implements standard web queries

void handle_alive()
{
  answer(true, "{ \"hostname\": \"%s\", \"query\": \"hi\", \"now\": %.3f, \"answer\": \"indeed :)\" }", HOSTNAME, fmillis());
}
void handle_loop()
{
  answer(true, "{ \"hostname\": \"%s\", \"query\": \"loop\", \"now\": %.3f, \"average-loop-delay\": %.3f, \"last-loop-delay\": %.3f }", HOSTNAME, fmillis(), getLoopDelay(), getLastLoopDelay());
}
void handle_restart()
{
  VERBOSE = true;
  answer(true, "{ \"hostname\": \"%s\", \"query\": \"restart\" }", HOSTNAME);
  Serial.println("Now restarting via http://…/restart query");
  delay(500);
  ESP.restart();
}
void handle_sleep()
{
  VERBOSE = true;
  answer(true, "{ \"hostname\": \"%s\", \"query\": \"sleep\" }", HOSTNAME);
  Serial.println("Now halting via http://…/sleep query (entering deep sleeping mode)");
  delay(500);
  esp_deep_sleep_start();
}
void handle_web_page()
{
#include "web_page.hpp"
  server->send(200, "text/html", web_page);
}
void handle_gpio_web_page()
{
#include "node_modules/idnai-esp32/src/gpio_web_page.hpp"
  server->send(200, "text/html", gpio_web_page);
}
void handle_gpio();

////////////////////////////////////////////////////////////////////////////////

// Setups the ESP32 service, this routine is run once by the ESP32 system

void setup_handles();

void setup()
{
  // Starts the serial line dump
  {
    delay(5000);
    Serial.begin(115200);
    Serial.println(""), Serial.flush(); // Prevents from loosing characters at start
    Serial.println("================================================================================");
    echo("+ Starting '%s' connected at 115200 baud with esp32gpiocontol firmware version '%s'", HOSTNAME, GPIO_VERSION), echoFlush();
  }

  // Connects to the Wifi network
  {
    echo("+ Connecting to wifi …"), echoFlush();
    WiFi.disconnect(true);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(HOSTNAME);
    WiFi.mode(WIFI_STA);
#if WIFI_SCAN_METHOD
    // Network scanning method
    unsigned int N = WiFi.scanNetworks();
    if(N > 0) {
      echo(" … found %d networks:", N), echoFlush();
      for(unsigned int n = 0; n < N; n++) {
        char t = WiFi.encryptionType(n);
        echo("\t{n: %d SSID: %s RRSI: %d channel: %d encryption: %s}", n, WiFi.SSID(n).c_str(), WiFi.RSSI(n), WiFi.channel(n), t == WIFI_AUTH_OPEN ? "OPEN" : t == WIFI_AUTH_WEP ? "WEP" : t == WIFI_AUTH_WPA_PSK ? "WPA" : t == WIFI_AUTH_WPA2_PSK ? "WPA2" : t == WIFI_AUTH_WPA_WPA2_PSK ? "WPA+WPA2" : t == WIFI_AUTH_WPA2_ENTERPRISE ? "WPA2-EAP" : t == WIFI_AUTH_WPA3_PSK ? "WPA3" : t == WIFI_AUTH_WPA2_WPA3_PSK ? "WPA2+WPA3" : t == WIFI_AUTH_WAPI_PSK ? "WAPI" : "unknown"), echoFlush();
      }
    } else {
      echo(" …  no network found !"), echoFlush();
    }
    for(auto it = wifis.begin(); it != wifis.end(); it++) {
      bool found = false;
      for(unsigned int n = 0; n < N && !(found = it->first == WiFi.SSID(n)); n++) {}
      if(found) {
        echo(" … attempting to connect to '%s' …", it->first.c_str()), echoFlush();
        WiFi.begin(it->first.c_str(), it->second.c_str());
        for(unsigned int limit = millis() + 5000; WiFi.status() != WL_CONNECTED && millis() < limit; delay(500)) {}
        if(WiFi.status() == WL_CONNECTED) {
          echo(" … connected to '%s' SSID", it->first.c_str()), echoFlush();
          break;
        }
      }
    }
    WiFi.scanDelete();
#else
    // Testing and error fallback method
    for(auto it = wifis.begin(); it != wifis.end(); it++) {
      echo(" … attempting to connect to '%s' …", it->first.c_str()), echoFlush();
      WiFi.begin(it->first.c_str(), it->second.c_str());
      for(unsigned int limit = millis() + 5000; WiFi.status() != WL_CONNECTED && millis() < limit; delay(500)) {}
      if(WiFi.status() == WL_CONNECTED) {
        echo(" … connected to '%s' SSID", it->first.c_str()), echoFlush();
        break;
      }
    }
#endif
    if(WiFi.status() != WL_CONNECTED) {
      server = NULL;
      echo(" … sorry! All connection's attempts failed: no Wifi, no WebService ;("), echoFlush();
    } else {
      // Retrieves the MAC address
      {
        byte mac[6];
        WiFi.macAddress(mac);
        echo("+ WiFi connected from MAC '%02x:%02x:%02x:%02x:%02x:%02x' with hostname '%s' at IP '%s'", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], WiFi.getHostname(), WiFi.localIP().toString().c_str()), echoFlush();
      }
      // Initializes the server and configure its URLs routing
      {
        server = new WebServer(SERVER_LISTEN_PORT);
        // This avoids a 1 msec delay when handling client request
        server->enableDelay(false);

	// Implements the web server handles
	setup_handles();
	
        server->begin();
        echo("+ HTTP server started, listening at: http://%s:%d", WiFi.localIP().toString().c_str(), SERVER_LISTEN_PORT), echoFlush();
      }
    }
  }
  echo("+ Setup done after %.0f milli-seconds", fmillis()), echoFlush();

  // Turns onboard LED steady on, after blinking, to indicate setup success
  {
    // Bliking period in msec and count, duration = period * count.
    static const unsigned int period = 500, count = 2;
    pinMode(13, OUTPUT);
    for(unsigned int n = 0; n < 2 * count; n++) {
      digitalWrite(13, n % 2);
      delay(period / 2);
    }
    echo("+ The %d LED blinkings during %d sec on port 13 is done, LED is on.", count, (period * count) / 1000), echoFlush();
  }
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  // Allows the fmillis() and getLoopDelay() functions to be effective.
  updateLoopDelay();
  // Allows the setInterval() mechanism to be effective.
  updateSetInterval();
  // Allows the web server client request handling to be effective.
  updateWebServer();
  // Allows the echo messages to be output.
  echoFlush();
}

////////////////////////////////////////////////////////////////////////////////
