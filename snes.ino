#include <Keyboard.h>
#include <Wire.h>

/*
 * See the keybindings in the README.md file
 */

// I'd prefer a nice Makefile and a number of different .cpp files, but
// that would really make this more complicated for non-software people,
// so adapt to Arduino .ino files and just use headers, I guess...

#include "mytypes.h"
#include "buttons.h"

const uint8_t I2C_ADDR = 0x52;
ControllerState cur_state;
uint16_t retry_counter;
uint8_t buffer[32];
uint8_t XY_CurJog = XY_MEDIUM;
uint8_t Z_CurJog = Z_MEDIUM;
uint16_t curPressed = 0;
uint16_t curJog = 0;
State_t controllerState = ST_ControllerDisabled;
uint16_t prevButtons = 0xFFFF;

// This needs to be below our global definitions...
#include "debugging.h"

uint8_t writeData(uint8_t* data, uint8_t size, const char* err = NULL) {
  Wire.beginTransmission(I2C_ADDR);
  uint8_t num = Wire.write(data, size);
  Wire.endTransmission();
  if (num != size && err != NULL) {
    DBG("Tried to write");
    DBGN(size);
    DBG(" but only wrote ");
    DBGN(num);
    DBG(" bytes instead: ");
    DBGN(err);
  }
  return num;
}

uint8_t readData(uint8_t count, uint8_t id) {
  writeData(&id, 1, "ID Value write");
  delayMicroseconds(200);
  Wire.requestFrom(I2C_ADDR, count);
  uint8_t numTries = 0;
  uint8_t ofs = 0;
  do {
    for (; count && Wire.available(); ofs++) {
      buffer[ofs] = Wire.read();
      count--;
    }
    if (count > 0) {
      numTries++;
      delayMicroseconds(10);
    }
  } while (count && numTries < 5);
  if (count > 0) {
    DBG("Missing remaining bytes: ");
    DBGN(count);
    DBG(": ");
    for (uint8_t i = 0; i < ofs; i++) {
      DBGN(i);
      DBG(">");
      DBGN(buffer[i]);
      DBG(" ");
    }
    return 0;
  } else {
    return ofs;
  }
}

#if defined(TEENSY_3)
void blinkLed() {
  pinMode(13, OUTPUT);
  delay(1);
  digitalWrite(13, HIGH);
  delay(100);
  digitalWrite(13, LOW);
}
#else
void blinkLed() {}
#endif

void initController() {
  cur_state = CS_ERR;
  // 0x3 is supposed to be the data format, but it seems irrelevant...
  uint8_t data[] = {0xf0, 0x55, 0xfb, 0x00, 0xfe, 0x3};
  if (writeData(data, 6, "Initializing Device") == 6) {
    cur_state = CS_INIT;
  }
}

void setupController() {
  initController();
  if (cur_state != CS_INIT) {
    return;
  }
  delay(100);
  uint8_t bytesRead = readData(6, 0xfa);
  if (bytesRead != 6) {

    cur_state = CS_ERR;
    DBG("Error in setupController");
    return;
  }
  // For now, just hard-check all 6 values
  if (buffer[0] != 1 || buffer[1] != 0 || buffer[2] != 0xa4 ||
      buffer[3] != 0x20 || buffer[4] > 1 || buffer[5] != 1) {
    cur_state = CS_ERR;
    DBG("Wrong Values in setupController:  ");
    for (int q = 0; q < 6; q++) {
      DBGN(q);
      DBG(":");
      DBGN((uint8_t)buffer[q]);
      DBG(" ");
    }

  } else {
    cur_state = CS_OK;
  }
}

uint8_t isPressed(Button_t button) {
  return (curPressed & button) ? 1 : 0;
}

uint8_t isReleased(Button_t button) {
  return 1 - isPressed(button);
}

#include "sendkeys.h"

void resetButtonStates() {
  curPressed = 0;
  controllerState = ST_ControllerDisabled;
}

// This is a hard-coded state machine documented in the readme
// It would probably be easier for folks to edit a declarative file
// Maybe, someday, but for now this works
void UpdateState() {
  switch (controllerState) {
    case ST_ControllerDisabled:
      if (curPressed == BTN_LBUMP) {
        controllerState = ST_MaybeEnabledA;
      }
      break;
    case ST_MaybeEnabledA:
      if (curPressed == (BTN_LBUMP | BTN_START)) {
        controllerState = ST_MaybeEnabledB;
      } else if (curPressed != BTN_LBUMP) {
        controllerState = ST_ControllerDisabled;
      }
      break;
    case ST_MaybeEnabledB:
      if (curPressed == BTN_LBUMP) {
        controllerState = ST_MaybeEnabledC;
      } else if (curPressed != (BTN_LBUMP | BTN_START)) {
        controllerState = ST_ControllerDisabled;
      }
      break;
    case ST_MaybeEnabledC:
      if (curPressed == BTN_NONE) {
        controllerState = ST_Enabled;
      } else if (curPressed != BTN_LBUMP) {
        controllerState = ST_ControllerDisabled;
      }
      break;
    case ST_Enabled:
      switch (curPressed) {
        case BTN_BBUMPS | BTN_X:
          controllerState = ST_WaitForResetX;
          break;
        case BTN_BBUMPS | BTN_Y:
          controllerState = ST_WaitForResetY;
          break;
        case BTN_LBUMP | BTN_START:
          controllerState = ST_WaitForDisable;
          break;
        case BTN_BBUMPS | BTN_START:
          controllerState = ST_WaitForResetZ;
          break;
        case BTN_BBUMPS | BTN_SELECT:
          controllerState = ST_WaitForMaybeHome;
          break;
        case BTN_LBUMP | BTN_SELECT:
          controllerState = ST_WaitForReturnToZero;
          break;
        case BTN_SELECT:
          controllerState = ST_WaitForStart;
          break;
        case BTN_START:
          controllerState = ST_WaitForSelect;
          break;
        case BTN_RBUMP | BTN_START:
          controllerState = ST_WaitForUnlock;
          break;
        case BTN_RBUMP | BTN_SELECT:
          controllerState = ST_WaitForReset;
          break;
        default: {
          uint16_t noBumps = BTN_REMOVEBUMPS(curPressed);
          if (BTN_JOGS & noBumps) {
            curJog = noBumps;
            controllerState = ST_WaitForSameJogUp;
          }
          break;
        }
      }
      break;
    case ST_WaitForResetX:
      if (curPressed != (BTN_BBUMPS | BTN_X)) {
        if (curPressed == BTN_BBUMPS) {
          SendXZero();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForResetY:
      if (curPressed != (BTN_BBUMPS | BTN_Y)) {
        if (curPressed == BTN_BBUMPS) {
          SendYZero();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForResetZ:
      if (curPressed != (BTN_BBUMPS | BTN_START)) {
        if (curPressed == BTN_BBUMPS) {
          // Start was released
          SendZZero();
        }
        if (curPressed == (BTN_BBUMPS | BTN_START | BTN_SELECT)) {
          controllerState = ST_MaybeGoHome;
        } else {
          controllerState = ST_Enabled;
        }
      }
      break;
    case ST_WaitForDisable:
      if (curPressed != (BTN_LBUMP | BTN_START)) {
        if (curPressed == BTN_LBUMP) {
          controllerState = ST_ControllerDisabled;
        } else {
          controllerState = ST_Enabled;
        }
      }
      break;
    case ST_MaybeGoHome:
      if (curPressed != (BTN_BBUMPS | BTN_START | BTN_SELECT)) {
        if (curPressed == (BTN_BBUMPS | BTN_START) ||
            curPressed == (BTN_BBUMPS | BTN_SELECT) ||
            curPressed == BTN_BBUMPS) {
          SendHomeMachine();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForMaybeHome:
      if (curPressed == (BTN_BBUMPS | BTN_SELECT | BTN_START)) {
        controllerState = ST_MaybeGoHome;
      } else if (curPressed != (BTN_BBUMPS | BTN_SELECT)) {
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForReturnToZero:
      if (curPressed != (BTN_LBUMP | BTN_SELECT)) {
        if (curPressed == BTN_LBUMP) {
          SendReturnToZero();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForUnlock:
      if (curPressed != (BTN_RBUMP | BTN_START)) {
        if (curPressed == BTN_RBUMP) {
          SendUnlock();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForReset:
      if (curPressed != (BTN_RBUMP | BTN_SELECT)) {
        if (curPressed == BTN_RBUMP) {
          SendSoftReset();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForStart:
      if (curPressed != BTN_SELECT) {
        if (curPressed == (BTN_START | BTN_SELECT)) {
          controllerState = ST_WaitForSSUp;
        } else {
          controllerState = ST_Enabled;
        }
      }
      break;
    case ST_WaitForSelect:
      if (curPressed != BTN_START) {
        if (curPressed == (BTN_START | BTN_SELECT)) {
          controllerState = ST_WaitForSSUp;
        } else {
          controllerState = ST_Enabled;
        }
      }
      break;
    case ST_WaitForSSUp:
      if (curPressed != (BTN_START | BTN_SELECT)) {
        if (curPressed == BTN_START || curPressed == BTN_SELECT) {
          SendAllZero();
        }
        controllerState = ST_Enabled;
      }
      break;
    case ST_WaitForSameJogUp: {
      uint16_t noBumps = BTN_REMOVEBUMPS(curPressed);
      if (noBumps != curJog) {
        // This identifies only changed bits (xor)
        // And then masks out any that are pressed,
        // resulting in only the buttons that have been released
        SendJog((noBumps ^ curJog) & ~curPressed);
      }
      controllerState = ST_Enabled;
      break;
    }
    default:
      controllerState = ST_ControllerDisabled;
      break;
  }
}

 void ReportKey(uint16_t val, bool pressed) {
  if (pressed) {
    curPressed |= 1 << val;
  } else {
    curPressed &= ~(1 << val);
  }
  UpdateState();
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
  if (buttons != prevButtons) {
    for (uint8_t i = 0; i < 16; i++) {
      // XOR identifies only changes: Convenient!
      if ((buttons ^ prevButtons) & (1 << i)) {
        bool pressed = !(buttons & (1 << i));
        DumpState();
        DBG(" {");
        ReportKey(i, pressed);
        DBG("} to ");
        DumpState();
        DBG("\n");
      }
    }
    prevButtons = buttons;
  }
}

void setup() {
  Wire.begin();
  Keyboard.begin();
  // Blink the LED, if one exists
  blinkLed();
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
  if (elapsed < 5) {
    delay(5 - elapsed);
  }
}
