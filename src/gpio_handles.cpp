//
// GPIO command handling
//

#include "esp32.hpp"

// ESP32 GPIO digital get and set value

void handle_digital_get_value(unsigned int index)
{
  pinMode(index, INPUT, true);
  unsigned int value = digitalRead(index);
  server_answer(true, "{\"what\": \"digital_get_value\", \"index\": %d, \"value\": %d, \"time\": %.3f}", index, value, fmillis());
}
void handle_digital_get_up_value(unsigned int index)
{
  pinMode(index, INPUT_PULLDOWN, true);
  unsigned int value = digitalRead(index);
  server_answer(true, "{\"what\": \"digital_get_up_value\", \"index\": %d, \"value\": %d, \"time\": %.3f}", index, value, fmillis());
}
void handle_digital_get_down_value(unsigned int index)
{
  pinMode(index, INPUT_PULLUP, true);
  unsigned int value = digitalRead(index);
  server_answer(true, "{\"what\": \"digital_get_down_value\", \"index\": %d, \"value\": %d, \"time\": %.3f}", index, value, fmillis());
}
void handle_digital_set_value(unsigned int index)
{
  unsigned int value = server_get("value").toInt();
  pinMode(index, OUTPUT, true);
  digitalWrite(index, value == 0 ? 0 : 1);
  value = digitalRead(index);
  server_answer(true, "{\"what\": \"digital_set_value\", \"index\": %d, \"value\": %d, \"time\": %.3f}", index, value, fmillis());
}

// ESP32 GPIO analog get and set value

void handle_analog_get_value(unsigned int index)
{
  pinMode(index, ANALOG_INPUT, true);
  unsigned int resolution = "" != server_get("resolution") ? server_get("resolution").toInt() : 10;
  unsigned int range = "" != server_get("range") ? server_get("range").toInt() : 3;
  std::string mode = "" != server_get("mode") ? server_get("mode").c_str() : "raw";
  bool voltage_mode = mode.substr(0, 4) == "volt";
  analogReadResolution(resolution);
  analogSetAttenuation(range == 3 ? ADC_0db : range == 2 ? ADC_2_5db : range == 1 ? ADC_6db : ADC_11db);
  if(voltage_mode) {
    double value = 0.001 * analogReadMilliVolts(index);
    server_answer(true, "{\"what\": \"analog_get_value\", \"index\": %d, \"range\": %d, \"value\": %.3f, \"time\": %.3f}", index, range, value, fmillis());
  } else {
    unsigned int value = analogRead(index);
    server_answer(true, "{\"what\": \"analog_get_value\", \"index\": %d, \"range\": %d, \"value\": %d, \"time\": %.3f}", index, range, value, fmillis());
  }
}
void handle_analog_set_value(unsigned int index)
{
  pinMode(index, ANALOG_OUTPUT, true);
  int value = server_get("value").toInt();
  value = value < 0 ? 0 : 255 < value ? 255 : value;
  int ok = analogRawWrite(index, value);
  server_answer(true, "{\"what\": \"analog_set_value\", \"index\": %d, \"value\": %d, \"ok\": %d, \"time\": %.3f}", index, value, ok, fmillis());
}

// ESP32 GPIO digital square wave generator

void handle_square_wave(unsigned int index)
{
  double frequency = "" != server_get("frequency") ? server_get("frequency").toDouble() : 1;
  double duration = "" != server_get("duration") ? server_get("duration").toDouble() : 1000;
  square_wave_start(0, index, frequency, duration);
  server_answer(true, "{\"what\": \"square_wave\", \"index\": %d, \"frequency\": %f, \"duration\": %f, \"time\": %.3f}", index, frequency, duration, fmillis());
}

// ESP32 GPIO digital time measurement mechanism

void handle_time_measurement(unsigned int index)
{
  String action = server_get("action"), modeLog = "";
  if(action == "start") {
    String mode = "" != server_get("mode") ? server_get("mode") : "rising";
    modeLog = "\"mode\": \"" + mode + "\", ";
    time_measurement_start(index, mode == "rising" ? RISING : mode == "falling" ? FALLING : CHANGE);
  } else if(action == "reset") {
    time_measurement_reset(index);
  } else if(action == "stop") {
    time_measurement_stop(index);
  }
  server_answer(true, "{\"what\": \"time_measurement\", \"index\": %d, \"action\": \"%s\", %s\"value\": %.3f,\"time\": %.3f}", index, action.c_str(), modeLog.c_str(), time_measurement_get(index), fmillis());;
}

//
// GPIO service
//

#include "node_modules/idnai-esp32/src/gpio_web_page.hpp"

void handle_gpio()
{
  if("" != server_get("index")) {
    int index = server_get("index").toInt();
    if(index < 40) {
      if("" != server_get("action")) {
        if(server_get("action") == "wave") {
          handle_square_wave(index);
        } else {
          handle_time_measurement(index);
        }
      } else {
        String mode = "" != server_get("mode") ? server_get("mode") : "digital";
        if("" != server_get("value")) {
          if(mode == "analog") {
            handle_analog_set_value(index);
          } else {
            handle_digital_set_value(index);
          }
        } else {
          if(mode == "analog") {
            handle_analog_get_value(index);
          } else if(mode == "up") {
            handle_digital_get_up_value(index);
          } else if(mode == "down") {
            handle_digital_get_down_value(index);
          } else {
            handle_digital_get_value(index);
          }
        }
      }
    } else {
      server_answer(false, "{\"what\": \"gpio\", \"error\": \"spurious index\": %d}", index);
    }
  } else {
    // Renders the GPIO web page
    server_answer(true, gpio_web_page);
  }
}
