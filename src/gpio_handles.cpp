//
// GPIO command handling
//

#include "node_modules/idnai-eps32/src/esp32.hpp"

// ESP32 GPIO digital get and set value

void handle_gpio_digital_get_value(unsigned int index)
{
  pinMode(index, INPUT, true);
  unsigned int value = digitalRead(index);
  answer(true, "{\"what\": \"gpio_digital_get_value\", \"index\": %d, \"value\": %d, \"time\": %.3f, \"error\": \"%s\"}", index, value, fmillis(), pinModeError.c_str());
}
void handle_gpio_digital_get_up_value(unsigned int index)
{
  pinMode(index, INPUT_PULLDOWN, true);
  unsigned int value = digitalRead(index);
  answer(true, "{\"what\": \"gpio_digital_get_value\", \"index\": %d, \"value\": %d, \"time\": %.3f, \"error\": \"%s\"}", index, value, fmillis(), pinModeError.c_str());
}
void handle_gpio_digital_get_down_value(unsigned int index)
{
  pinMode(index, INPUT_PULLUP, true);
  unsigned int value = digitalRead(index);
  answer(true, "{\"what\": \"gpio_digital_get_value\", \"index\": %d, \"value\": %d, \"time\": %.3f, \"error\": \"%s\"}", index, value, fmillis(), pinModeError.c_str());
}
void handle_gpio_digital_set_value(unsigned int index)
{
  unsigned int value = server->arg("value").toInt();
  pinMode(index, OUTPUT, true);
  digitalWrite(index, value == 0 ? 0 : 1);
  value = digitalRead(index);
  answer(true, "{\"what\": \"gpio_digital_set_value\", \"index\": %d, \"value\": %d, \"time\": %.3f, \"error\": \"%s\"}", index, value, fmillis(), pinModeError.c_str());
}
// ESP32 GPIO digital square wave generator
void handle_gpio_square_wave(unsigned int index)
{
  double frequency = server->hasArg("frequency") ? server->arg("frequency").toDouble() : 1;
  double duration = server->hasArg("duration") ? server->arg("duration").toDouble() : 300;
  gpio_square_wave(index, frequency, duration);
  answer(true, "{\"what\": \"handle_gpio_square_wave\", \"index\": %d, \"frequency\": %f, \"duration\": %f, \"time\": %.3f, \"error\": \"%s\"}", index, frequency, duration, fmillis(), pinModeError.c_str());
}

// ESP32 GPIO analog get and set value

void handle_gpio_analog_get_value(unsigned int index)
{
  pinMode(index, ANALOG_INPUT, true);
  unsigned int resolution = server->hasArg("resolution") ? server->arg("resolution").toInt() : 10;
  unsigned int range = server->hasArg("range") ? server->arg("range").toInt() : 3;
  std::string mode = server->hasArg("mode") ? server->arg("mode").c_str() : "raw";
  bool voltage_mode = mode.substr(0, 4) == "volt";
  analogReadResolution(resolution);
  analogSetAttenuation(range == 3 ? ADC_0db : range == 2 ? ADC_2_5db : range == 1 ? ADC_6db : ADC_11db);
  if(voltage_mode) {
    double value = 0.001 * analogReadMilliVolts(index);
    answer(true, "{\"what\": \"gpio_analog_get_value\", \"index\": %d, \"range\": %d, \"value\": %.3f, \"time\": %.3f, \"error\": \"%s\"}", index, range, value, fmillis(), pinModeError.c_str());
  } else {
    unsigned int value = analogRead(index);
    answer(true, "{\"what\": \"gpio_analog_get_value\", \"index\": %d, \"range\": %d, \"value\": %d, \"time\": %.3f, \"error\": \"%s\"}", index, range, value, fmillis(), pinModeError.c_str());
  }
}
void handle_gpio_analog_set_value(unsigned int index)
{
  pinMode(index, ANALOG_OUTPUT, true);
  int value = server->arg("value").toInt();
  value = value < 0 ? 0 : 255 < value ? 255 : value;
  int ok = analogRawWrite(index, value);
  answer(true, "{\"what\": \"gpio_analog_set_value\", \"index\": %d, \"value\": %d, \"time\": %.3f, \"error\": \"%s%s%s\"}", index, value, fmillis(), ok == -2 ? "Digital analog converter initialization failed" : "", ok == -2 && pinModeError != "" ? ". " : "", pinModeError.c_str());
}

// ESP32 GPIO digital time measurement mechanism

void handle_gpio_timing_functions(unsigned int index)
{
  String action = server->arg("action"), modeLog = "";
  if(action == "start") {
    String mode = server->hasArg("mode") ? server->arg("mode") : "rising";
    modeLog = "\"mode\": \"" + mode + "\", ";
    gpio_digital_timing_start(index, mode == "rising" ? RISING : mode == "falling" ? FALLING : CHANGE);
  } else if(action == "stop") {
    gpio_digital_timing_stop(index);
  }
  answer(true, "{\"what\": \"gpio_digital_post_value\", \"index\": %d, \"action\": \"%s\", %s\"time\": %.3f, \"last-interrupt-times\": %.3f, \"error\": \"%s\"}", index, action.c_str(), modeLog.c_str(), fmillis(), gpio_digital_timing_get(index), pinModeError.c_str());
}

//
// Global service
//

void handle_gpio()
{
  if(server->hasArg("index")) {
    int index = server->arg("index").toInt();
    if(index < MAX_GPIO_INDEX) {
      if(server->hasArg("action")) {
        if(server->arg("action") == "wave") {
          handle_gpio_square_wave(index);
        } else {
          handle_gpio_timing_functions(index);
        }
      } else {
        String mode = server->hasArg("mode") ? server->arg("mode") : "digital";
        if(server->hasArg("value")) {
          if(mode == "analog") {
            handle_gpio_analog_set_value(index);
          } else {
            handle_gpio_digital_set_value(index);
          }
        } else {
          if(mode == "analog") {
            handle_gpio_analog_get_value(index);
          } else if(mode == "up") {
            handle_gpio_digital_get_up_value(index);
          } else if(mode == "down") {
            handle_gpio_digital_get_down_value(index);
          } else {
            handle_gpio_digital_get_value(index);
          }
        }
      }
    } else {
      answer(false, "{\"what\": \"gpio\", \"error\": \"spurious index\": %d}", index);
    }
  } else {
    answer(false, "{\"what\": \"gpio\", \"error\": \"undefined index\"}", "");
  }
}
