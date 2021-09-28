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
#define NEOPIXEL_PIN 11

const uint8_t I2C_ADDR = 0x52;
IIC_State_t cur_state;
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
#include "display.h"
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
  displayStateSetup();
  retry_counter = 0;
}

void loop() {
  static uint32_t lastScanTime = 0;
  static uint32_t lastDisplayTime = 0;
  uint32_t startTime = millis();
  if (startTime - lastScanTime > 5) {
    lastScanTime = startTime;
    switch (cur_state) {
      // Everything is normal: Read the device, and deal with the buttons
      case IIC_OK:
        // Read 10 bytes. On the Teensy, I could read 21, but on an Elite-C, it
        // seems to fail after 10 for no reason I can ascertain...
        if (readData(10, 0) != 10) {
          cur_state = IIC_RETRY;
          break;
        }
        handleButtons();
        break;
      case IIC_RETRY:
      case IIC_ERR:
        if (++retry_counter == 20) {
          retry_counter = 0;
          setupController();
          if (cur_state != IIC_OK) {
            cur_state = IIC_RETRY;
          } else {
            resetButtonStates();
          }
        }
        break;
      default:
        cur_state = IIC_ERR;
    }
  }
  if (startTime - lastDisplayTime > 0) {
    displayState(cur_state, controller_state, startTime);
    lastDisplayTime = startTime;
  }
  // I should probably put some sort of yield thing in here, right?
  delay(0);
}
