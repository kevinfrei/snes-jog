#include <Keyboard.h>
#include <Wire.h>

/*
 * See the keybindings in the README.md file
 */

// Also, this code is a hot mess, currently.
// I should clean it up and organize it a bit better.
// For the love of god, at least move some stuff to headers!

#if defined(LOTSA_TYPING)
#define DBG(a) Keyboard.print(a)
#define DBGN(n) TypeNumber(n)
#define DESCRIBE_NOT_TYPE 1
#else
#define DBG(a) 0
#define DBGN(a) 0
#endif

const uint8_t I2C_ADDR = 0x52;
typedef uint16_t Button_t;
enum ControllerState { CS_OK, CS_RETRY, CS_INIT, CS_ERR };
enum State_t {
  ST_ControllerDisabled,
  ST_MaybeEnabledA,
  ST_MaybeEnabledB,
  ST_MaybeEnabledC,
  ST_Enabled,
  ST_WaitForResetX,
  ST_WaitForResetY,
  ST_WaitForResetZ,
  ST_WaitForDisable,
  ST_MaybeGoHome,
  ST_WaitForMaybeHome,
  ST_WaitForReturnToZero,
  ST_WaitForSameJogUp,
  ST_WaitForUnlock,
  ST_WaitForReset,
  ST_WaitForStart,
  ST_WaitForSelect,
  ST_WaitForSSUp
};

ControllerState cur_state;
uint16_t retry_counter;

uint8_t buffer[32];

void TypeNumber(uint32_t num) {
  uint32_t val = 0;
  // Flip the number around to type it out
  uint8_t trailingZeroes = 0;
  while (num) {
    uint8_t v = num % 10;
    val = val * 10 + v;
    num = num / 10;
    if (val == 0) {
      trailingZeroes++;
    } else {
      trailingZeroes = 0;
    }
  }
  // Type the reversed number Least sig digit first...
  do {
    Keyboard.write('0' + val % 10);
    val /= 10;
  } while (val);
  while (trailingZeroes--) {
    Keyboard.write('0');
  }
}

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

// Bit numbers in the switch value
#define BTN_NONE 0
#define BTN_UP 1
#define BTN_LEFT (1 << 1)
#define BTN_DOWN (1 << 14)
#define BTN_RIGHT (1 << 15)
#define BTN_X (1 << 3)
#define BTN_A (1 << 4)
#define BTN_Y (1 << 5)
#define BTN_B (1 << 6)
#define BTN_RBUMP (1 << 9)
#define BTN_LBUMP (1 << 13)
#define BTN_START (1 << 10)
#define BTN_SELECT (1 << 12)
#define BTN_JOGS (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_A | BTN_B)
#define BTN_BBUMPS (BTN_LBUMP | BTN_RBUMP)
#define BTN_REMOVEBUMPS(val) ((val) & ~BTN_BBUMPS)
#define BTN_DPAD (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)
#if 0
const char* names[16] = {"U",
                         "L",
                         "NA-2",
                         "X",
                         "A",
                         "Y",
                         "B",
                         "NA-7",
                         "NA-8",
                         "BR",
                         "Start",
                         "NA-11",
                         "Select",
                         "BL",
                         "D",
                         "R"};
#endif

enum XY_SystemState_t { XY_SMALL = 0, XY_MEDIUM = 1, XY_LARGE = 2 };
enum Z_SystemState_t { Z_SMALL = 0, Z_MEDIUM = 1, Z_LARGE = 2 };

uint8_t XY_CurJog = XY_MEDIUM;
uint8_t Z_CurJog = Z_MEDIUM;
uint16_t curPressed = 0;
uint16_t curJog = 0;
State_t controllerState = ST_ControllerDisabled;

uint8_t isPressed(Button_t button) {
  return (curPressed & button) ? 1 : 0;
}

uint8_t isReleased(Button_t button) {
  return 1 - isPressed(button);
}

void Send(char key, const char* str) {
#if defined(DESCRIBE_NOT_TYPE)
  DBG(str);
#else
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(key);
  delay(1);
  Keyboard.releaseAll();
  delay(50);
#endif
}

void SendXYJogMultiply() {
  Send(KEY_F1, "XY Multiply");
}

void SendXYJogDivide() {
  Send(KEY_F2, "XY Divide");
}

void SendZJogMultiply() {
  Send(KEY_F5, "Z Multiply");
}

void SendZJogDivide() {
  Send(KEY_F6, "Z Divide");
}

void SendXpYp() {
  Send('u', "X+Y+ jog");
}

void SendYp() {
  Send('i', "Y+ jog");
}

void SendXmYp() {
  Send('o', "X+Y- jog");
}

void SendXp() {
  Send('j', "X+ jog");
}

void SendXm() {
  Send('l', "X- jog");
}

void SendXpYm() {
  Send('m', "X+Y- jog");
}

void SendYm() {
  Send(',', "Y- jog");
}

void SendXmYm() {
  Send('.', "X-Y- jog");
}

void SendZp() {
  Send('h', "Z+ jog");
}

void SendZm() {
  Send('y', "Z- jog");
}

void SendXZero() {
  Send('p', "X Zero");
}

void SendYZero() {
  Send(';', "Y Zero");
}

void SendZZero() {
  Send('/', "Z Zero");
}

void SendAllZero() {
  Send('k', "All Zero");
}

void SendReturnToZero() {
  Send('d', "Return to Zero");
}

void SendSoftReset() {
  Send('g', "Soft Reset");
}

void SendUnlock() {
  Send('s', "Unlock");
}

void SendHomeMachine() {
  Send('t', "Home!");
}

// Get ready for an XY-jog
void JogXYPrep() {
  XY_SystemState_t target = isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP);
  while (target != XY_CurJog) {
    if (target < XY_CurJog) {
      SendXYJogDivide();
      XY_CurJog--;
    } else {
      SendXYJogMultiply();
      XY_CurJog++;
    }
  }
}

// Get ready for a Z-jog
void JogZPrep() {
  Z_SystemState_t target = isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP);
  while (target != Z_CurJog) {
    if (target < Z_CurJog) {
      SendZJogDivide();
      Z_CurJog--;
    } else {
      SendZJogMultiply();
      Z_CurJog++;
    }
  }
}

void SendJog(uint16_t toJog) {
  if (toJog & (BTN_A | BTN_B)) {
    // Z Jog
    JogZPrep();
    if (toJog == BTN_A) {
      SendZm();
    } else if (toJog == BTN_B) {
      SendZp();
    } else {
      // TODO: ack
    }
  }
  if (toJog & BTN_DPAD) {
    JogXYPrep();
    switch (toJog) {
      case BTN_UP:
        SendYp();
        break;
      case BTN_DOWN:
        SendYm();
        break;
      case BTN_LEFT:
        SendXm();
        break;
      case BTN_RIGHT:
        SendXp();
        break;
      case BTN_UP | BTN_LEFT:
        SendXmYp();
        break;
      case BTN_UP | BTN_RIGHT:
        SendXpYp();
        break;
      case BTN_DOWN | BTN_LEFT:
        SendXmYm();
        break;
      case BTN_DOWN | BTN_RIGHT:
        SendXpYm();
        break;
      default:
        // TODO: ack!
        break;
    }
  }
}

void resetButtonStates() {
  curPressed = 0;
  controllerState = ST_ControllerDisabled;
}

void DumpState() {
  switch (controllerState) {
    case ST_ControllerDisabled:
      DBG("ST_ControllerDisabled");
      break;
    case ST_MaybeEnabledA:
      DBG("ST_MaybeEnabledA");
      break;
    case ST_MaybeEnabledB:
      DBG("ST_MaybeEnabledB");
      break;
    case ST_MaybeEnabledC:
      DBG("ST_MaybeEnabledC");
      break;
    case ST_Enabled:
      DBG("ST_Enabled");
      break;
    case ST_WaitForResetX:
      DBG("ST_WaitForResetX");
      break;
    case ST_WaitForResetY:
      DBG("ST_WaitForResetY");
      break;
    case ST_WaitForResetZ:
      DBG("ST_WaitForResetZ");
      break;
    case ST_WaitForDisable:
      DBG("ST_WaitForDisable");
      break;
    case ST_MaybeGoHome:
      DBG("ST_MaybeGoHome");
      break;
    case ST_WaitForMaybeHome:
      DBG("ST_WaitForMaybeHome");
      break;
    case ST_WaitForReturnToZero:
      DBG("ST_WaitForReturnToZero");
      break;
    case ST_WaitForUnlock:
      DBG("ST_WaitForUnlock");
      break;
    case ST_WaitForReset:
      DBG("ST_WaitForReset");
      break;
    case ST_WaitForStart:
      DBG("ST_WaitForStart");
      break;
    case ST_WaitForSelect:
      DBG("ST_WaitForSelect");
      break;
    case ST_WaitForSSUp:
      DBG("ST_WaitForSSUp");
      break;
    case ST_WaitForSameJogUp:
      DBG("ST_WaitForSameJogUp");
      break;
    default:
      DBG("ST_unknown");
      break;
  }
}

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

static void ReportKey(uint16_t val, bool pressed) {
  if (pressed) {
    curPressed |= 1 << val;
  } else {
    curPressed &= ~(1 << val);
  }
  UpdateState();
}

static uint16_t prevButtons = 0xFFFF;
static void handleButtons() {
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
