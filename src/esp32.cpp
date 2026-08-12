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

// Implements the variable argument à-la printf mechanism

const static unsigned int arg2chars_buffer_size = 1024;
unsigned int arg2chars_buffer_length = 0;
char arg2chars_buffer[arg2chars_buffer_size + 1] = "";
#define arg2chars() { va_list a; va_start(a, fmt); arg2chars_buffer_length = vsnprintf(arg2chars_buffer, arg2chars_buffer_size, fmt, a); va_end(a); }

const char *secho(const char *fmt, ...)
{
  arg2chars();
  return arg2chars_buffer;
}

// Implements the console echo with buffering management

void console_echo(bool flush, const char *fmt = NULL, ...)
{
  static String buffer = "";
  if (fmt != NULL) {
    arg2chars();
    buffer += arg2chars_buffer + '\n';
  }
  if (flush && buffer.length > 0) {
    Serial.write(buffer), Serial.flush(), buffer = "";
  }
}

// Webserver mechanims

void service_on(String route, void (*handler)()) {
  if(server != NULL) {
    server->on(route, HTTP_GET, handler);
    server->on(route, HTTP_POST, handler);
  }
}

String service_get(String name) {
  return server != NULL && server->hasArg(name) ? server->arg(name) : "";
}
	    
void service_answer(bool ok, const char *fmt = NULL, ...)
{
  if(server != NULL) {
    server->sendHeader("Access-Control-Allow-Origin", "*");
    server->sendHeader("Access-Control-Allow-Methods", "GET, POST");
    if (fmt != NULL) {
      arg2chars();
      String s(arg2chars_buffer).trim();
      server->send(ok ? 200 : 400, s[0] == '<' ? "text/html" : s[0] == '{' ? "application/json": "text/plain", arg2chars_buffer);
    } else {
      server->send(204, "", NULL);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

// Implements the GIO extension interface

void pinMode(unsigned int index, unsigned int mode, bool check)
{
  if(check) {
    bool analog = (mode & 0x10) != 0, input = (mode & 0x0F) != 3, pulling = (mode & 0x0C) != 0;
    // Notice: OUTPUT: 1+2, INPUT: 1, INPUT_PULLUP: 1+4, INPUT_PULLDOWN: 1+8, ANALOG_(IN|OUT)PUT = (IN|OUT)PUT | 0x10
    switch(mode) {
    case INPUT:
    case OUTPUT:
    case INPUT_PULLUP:
    case INPUT_PULLDOWN:
    case ANALOG_INPUT:
    case ANALOG_OUTPUT:
      break;
    default:
      console_echo(true, "Spurious mode value: %d", mode);
      break;
    }
    if(analog) {
      if(input) {
        if(!(32 <= index && index <= 39)) {
          console_echo(true, "Invalid analog input index, not in {32,39} (ADC2 is used for wifi)");
        }
      } else {
        if(!(24 <= index && index <= 26)) {
          console_echo(true, "Invalid analog output index, not in {25,26}");
        }
      }
    } else {
      if(!(4 == index || (12 <= index && index <= 15) || (18 <= index && index <= 27) || (32 <= index && index <= 39))) {
        console_echo(true, "Invalid digital input index, not in {4} + {12,15} + {18,27} + {32,39}, reserved for other usage");
      }
      if(34 <= index && index <= 39) {
        if(input) {
          if(pulling) {
            console_echo(true, "No pulling interface for input with an index in {32,39}");
          }
        } else {
          console_echo(true, "No output interface with an index in {34,39}");
        }
      }
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
  lastLoopDelay = now - previous, previous = now, loopDelay += a * (lastLoopDelay - loopDelay);
}

////////////////////////////////////////////////////////////////////////////////

// Implements the timing mechanisms
// Refs :
//  - https://docs.espressif.com/projects/esp-idf/en/v3.3.5/api-reference/peripherals/timer.html
//  - https://esp32.com/viewtopic.php?t=23372

static (void (*)(void)) setIntervalHandler[4];
static unsigned int setIntervalCounts[4];

static void IRAM_ATTR setIntervalTimerHandler(unsigned int* index) {
  if (setIntervalCounts[index] != 0) {
    (setIntervalHandler[*index])();
    if (setIntervalCounts[index] != -1)
      setIntervalCounts[index]--;
  }
}

void setInterval(unsigned int index, double delay, unsigned int count, void (handler *)(void), )
{
  if (index < 4) {
    unsigned int timer_gpx = 0x1 & (index >> 1), timer_idx = 0x1 & index;
    if (delay > 0) {
      timer_config_t config = {
	.alarm_en = true,
	.counter_en = false,
	.intr_type = TIMER_INTR_LEVEL,
	.counter_dir = TIMER_COUNT_UP,
	.auto_reload = true,
	.divider = 80
      };
      setIntervalHandler[index] = handler;
      setIntervalCounts[index] = count;
      timer_init(timer_gpx, timer_idx, &config);
      timer_set_counter_value(timer_gpx, timer_idx, 0);
      timer_set_alarm_value(timer_gpx, timer_idx, (int) rint(1000 * delay));
      timer_enable_intr(timer_gpx, timer_idx);
      timer_isr_register(timer_gpx, timer_idx, setIntervalTimerHandler, (void *) index, ESP_INTR_FLAG_IRAM, NULL);
      timer_start(timer_gpx, timer_idx);
    } else {
      timer_stop(timer_gpx, timer_idx);
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
  static double when = 0;
  double now = fmillis();
  // Handles queries, if the server is defined, every 10 msec
  if(server != NULL && now >= when) {
    when = now + 10;
    server->handleClient();
  }
}

////////////////////////////////////////////////////////////////////////////////

// Implements standard web queries

void handle_alive()
{
  answer(true, "{ \"hostname\": \"%s\", \"query\": \"hi\", \"now\": %.3f, \"average-loop-delay\": %.3f, \"last-loop-delay\": %.3f }", HOSTNAME, fmillis(), getLoopDelay(), getLastLoopDelay());
}
void handle_restart()
{
  answer(true, "{ \"hostname\": \"%s\", \"query\": \"restart\" }", HOSTNAME);
  Serial.println("Now restarting via http://…/restart query");
  delay(500);
  ESP.restart();
}
void handle_sleep()
{
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
    console_echo(true, "+ Starting '%s' connected at 115200 baud with esp32gpiocontol firmware version '%s'", HOSTNAME, GPIO_VERSION);
  }

  // Connects to the Wifi network
  {
    console_echo(true, "+ Connecting to wifi …");
    WiFi.disconnect(true);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(HOSTNAME);
    WiFi.mode(WIFI_STA);
#if WIFI_SCAN_METHOD
    // Network scanning method
    unsigned int N = WiFi.scanNetworks();
    if(N > 0) {
      console_echo(true, " … found %d networks:", N);
      for(unsigned int n = 0; n < N; n++) {
        char t = WiFi.encryptionType(n);
        console_echo(true, "\t{n: %d SSID: %s RRSI: %d channel: %d encryption: %s}", n, WiFi.SSID(n).c_str(), WiFi.RSSI(n), WiFi.channel(n), t == WIFI_AUTH_OPEN ? "OPEN" : t == WIFI_AUTH_WEP ? "WEP" : t == WIFI_AUTH_WPA_PSK ? "WPA" : t == WIFI_AUTH_WPA2_PSK ? "WPA2" : t == WIFI_AUTH_WPA_WPA2_PSK ? "WPA+WPA2" : t == WIFI_AUTH_WPA2_ENTERPRISE ? "WPA2-EAP" : t == WIFI_AUTH_WPA3_PSK ? "WPA3" : t == WIFI_AUTH_WPA2_WPA3_PSK ? "WPA2+WPA3" : t == WIFI_AUTH_WAPI_PSK ? "WAPI" : "unknown");
      }
    } else {
      console_echo(true, " …  no network found !");
    }
    for(auto it = wifis.begin(); it != wifis.end(); it++) {
      bool found = false;
      for(unsigned int n = 0; n < N && !(found = it->first == WiFi.SSID(n)); n++) {}
      if(found) {
        console_echo(true, " … attempting to connect to '%s' …", it->first.c_str());
        WiFi.begin(it->first.c_str(), it->second.c_str());
        for(unsigned int limit = millis() + 5000; WiFi.status() != WL_CONNECTED && millis() < limit; delay(500)) {}
        if(WiFi.status() == WL_CONNECTED) {
          console_echo(true, " … connected to '%s' SSID", it->first.c_str());
          break;
        }
      }
    }
    WiFi.scanDelete();
#else
    // Testing and error fallback method
    for(auto it = wifis.begin(); it != wifis.end(); it++) {
      console_echo(true, " … attempting to connect to '%s' …", it->first.c_str());
      WiFi.begin(it->first.c_str(), it->second.c_str());
      for(unsigned int limit = millis() + 5000; WiFi.status() != WL_CONNECTED && millis() < limit; delay(500)) {}
      if(WiFi.status() == WL_CONNECTED) {
        console_echo(true, " … connected to '%s' SSID", it->first.c_str());
        break;
      }
    }
#endif
    if(WiFi.status() != WL_CONNECTED) {
      server = NULL;
      console_echo(true, " … sorry! All connection's attempts failed: no Wifi, no WebService ;(");
    } else {
      // Retrieves the MAC address
      {
        byte mac[6];
        WiFi.macAddress(mac);
        console_echo(true, "+ WiFi connected from MAC '%02x:%02x:%02x:%02x:%02x:%02x' with hostname '%s' at IP '%s'", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], WiFi.getHostname(), WiFi.localIP().toString().c_str());
      }
      // Initializes the server and configure its URLs routing
      {
        server = new WebServer(SERVER_LISTEN_PORT);
        // This avoids a 1 msec delay when handling client request
        server->enableDelay(false);

	// Implements the web server handles
	setup_handles();
	
        server->begin();
        console_echo(true, "+ HTTP server started, listening at: http://%s:%d", WiFi.localIP().toString().c_str(), SERVER_LISTEN_PORT);
      }
    }
  }
  console_echo(true, "+ Setup done after %.0f milli-seconds", fmillis()), echoFlush();

  // Turns onboard LED steady on, after blinking, to indicate setup success
  {
    // Bliking period in msec and count, duration = period * count.
    static const unsigned int period = 500, count = 2;
    pinMode(13, OUTPUT);
    for(unsigned int n = 0; n < 2 * count; n++) {
      digitalWrite(13, n % 2);
      delay(period / 2);
    }
    console_echo(true, "+ The %d LED blinkings during %d sec on port 13 is done, LED is on.", count, (period * count) / 1000), echoFlush();
  }
}

////////////////////////////////////////////////////////////////////////////////

void loop()
{
  // Allows the fmillis() and getLoopDelay() functions to be effective.
  updateLoopDelay();
  // Allows the web server client request handling to be effective.
  updateWebServer();
  // Allows the echo messages to be output.
  console_echo(true);
}

////////////////////////////////////////////////////////////////////////////////
