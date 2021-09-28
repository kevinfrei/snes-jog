#include <Keyboard.h>
#include <Wire.h>

/*
 * See the keybindings in the README.md file
 */

// I'd prefer a nice Makefile and a number of different .cpp files, but
// that would really make this more complicated for non-software people,
// so adapt to Arduino .ino files and just use headers, I guess...

#include "buttons.h"
#include "mytypes.h"

// if you have an LED you'd like to blink briefly when the controller is enabled
// Uncomment this line and change "13" to whatever pin number you want to use
// (13 is the pin for the LED on a Teensy 3.2)
// #define LED_PIN 13

const uint8_t I2C_ADDR = 0x52;
ControllerState_t cur_state;
uint16_t retry_counter;
uint8_t buffer[32];
uint8_t xy_cur_jog = XY_MEDIUM;
uint8_t z_cur_jog = Z_MEDIUM;
uint16_t cur_pressed = 0;
uint16_t cur_jog = 0;
State_t controller_state = ST_ControllerDisabled;
uint16_t prev_buttons = 0xFFFF;

// This needs to be below our global definitions...
#include "debugging.h"

#include "controller.h"
#include "led.h"
#include "sendkeys.h"
#include "statemachine.h"

void reportKey(uint16_t val, bool pressed) {
  if (pressed) {
    cur_pressed |= 1 << val;
  } else {
    cur_pressed &= ~(1 << val);
  }
  updateState();
}

void handleButtons() {
  // The buffer is 10 bytes
  // First check to make sure the last couple bytes are zeroes
  if (buffer[8] != 0 || buffer[9] != 0) {
    return;
  }
  // The first 4 bytes are joystick related
  uint16_t buttons = buffer[4] << 8 | buffer[5];
  // Bytes 6 and 7 are for other buttons
  if (buttons != prev_buttons) {
    for (uint8_t i = 0; i < 16; i++) {
      // XOR identifies only changes: Convenient!
      if ((buttons ^ prev_buttons) & (1 << i)) {
        bool pressed = !(buttons & (1 << i));
        dumpState();
        DBG(" {");
        reportKey(i, pressed);
        DBG("} to ");
        dumpState();
        DBG("\n");
      }
    }
    prev_buttons = buttons;
  }
}

void setup() {
  Wire.begin();
  Keyboard.begin();
  setupController();
  resetButtonStates();
  retry_counter = 0;
}

void loop() {
  uint32_t startTime = millis();
  switch (cur_state) {
    // Everything is normal: Read the device, and deal with the buttons
    case CS_OK:
      // Read 10 bytes. On the Teensy, I could read 21, but on an Elite-C, it
      // seems to fail after 10 for no reason I can ascertain...
      if (readData(10, 0) != 10) {
        cur_state = CS_RETRY;
        break;
      }
      handleButtons();
      break;
    case CS_RETRY:
    case CS_ERR:
      if (++retry_counter == 20) {
        retry_counter = 0;
        setupController();
        if (cur_state != CS_OK) {
          cur_state = CS_RETRY;
        } else {
          resetButtonStates();
        }
      }
      break;
    default:
      cur_state = CS_ERR;
  }
  uint32_t elapsed = millis() - startTime;
  // We scan at 200 hz: Seems reasonable, yes?
  // This seems to avoid the need to debounce...
  if (elapsed < 5) {
    delay(5 - elapsed);
  }
}
