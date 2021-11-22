#pragma once

void send(char key,
          const char*
#if defined(DESCRIBE_DONT_TYPE)
              str
#endif
) {
#if defined(DESCRIBE_DONT_TYPE)
  DBGN((uint8_t)key);
  DBG(":");
  DBG(str);
#else
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(key);
  delayMicroseconds(200);
  Keyboard.releaseAll();
  delay(10);
#endif
}

void sendXYJogMultiply() {
  send(KEY_F1, "XY Multiply");
}

void sendXYJogDivide() {
  send(KEY_F2, "XY Divide");
}

void sendXYJogReset() {
  send(KEY_PAGE_UP, "XY 10");
}

void sendZJogMultiply() {
  send(KEY_F5, "Z Multiply");
}

void sendZJogDivide() {
  send(KEY_F6, "Z Divide");
}

void sendZJogReset() {
  send(KEY_PAGE_DOWN, "Z 1");
}

void sendXpYp() {
  send('u', "X+Y+ jog");
}

void sendYp() {
  send('i', "Y+ jog");
}

void sendXmYp() {
  send('o', "X+Y- jog");
}

void sendXp() {
  send('j', "X+ jog");
}

void sendXm() {
  send('l', "X- jog");
}

void sendXpYm() {
  send('m', "X+Y- jog");
}

void sendYm() {
  send(',', "Y- jog");
}

void sendXmYm() {
  send('.', "X-Y- jog");
}

void sendZp() {
  send('h', "Z+ jog");
}

void sendZm() {
  send('y', "Z- jog");
}

void sendXZero() {
  send('p', "X Zero");
}

void sendYZero() {
  send(';', "Y Zero");
}

void sendZZero() {
  send('/', "Z Zero");
}

void sendAllZero() {
  send('k', "All Zero");
}

void sendReturnToZero() {
  send('d', "Return to Zero");
}

void sendSoftReset() {
  send('g', "Soft Reset");
}

void sendUnlock() {
  send('s', "Unlock");
}

void sendHomeMachine() {
  send('t', "Home!");
}

// These are here to make sure we're occasionally setting the jog speed, even if
// we don't think we have to. It's helpful just in case the user monkeys with
// UGS directly
bool tooLongXY() {
  uint32_t now = millis();
  bool tooLong = (now - last_jog_xy) > (JOG_TIMEOUT * 1000);
  last_jog_xy = now;
  return tooLong;
}

bool tooLongZ() {
  uint32_t now = millis();
  bool tooLong = (now - last_jog_z) > (JOG_TIMEOUT * 1000);
  last_jog_z = now;
  return tooLong;
}

// Get ready for an XY-jog
void jogXYPrep() {
  Jog_Magnitude_t target =
      (Jog_Magnitude_t)(isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP));
  if (tooLongXY() || target != xy_cur_jog) {
    sendXYJogReset();
    switch (target) {
      case JM_LARGE:
        sendXYJogMultiply();
        break;
      case JM_SMALL:
        sendXYJogDivide();
        break;
      case JM_MEDIUM:
      default:
        // Do nothing here
        break;
    }
  }
  xy_cur_jog = target;
}

// Get ready for a Z-jog
void jogZPrep() {
  Jog_Magnitude_t target =
      (Jog_Magnitude_t)(isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP));
  if (tooLongZ() || target != z_cur_jog) {
    sendZJogReset();
    switch (target) {
      case JM_LARGE:
        sendZJogMultiply();
        break;
      case JM_SMALL:
        sendZJogDivide();
        break;
      case JM_MEDIUM:
      default:
        // Do nothing heroe
        break;
    }
  }
  z_cur_jog = target;
}

void sendJog(uint16_t toJog) {
  if (toJog & (BTN_A | BTN_B)) {
    // Z Jog
    jogZPrep();
    if (toJog == BTN_A) {
      sendZm();
    } else if (toJog == BTN_B) {
      sendZp();
    } else {
      // TODO: ack
    }
  }
  if (toJog & BTN_DPAD) {
    jogXYPrep();
    switch (toJog) {
      case BTN_UP:
        sendYp();
        break;
      case BTN_DOWN:
        sendYm();
        break;
      case BTN_LEFT:
        sendXm();
        break;
      case BTN_RIGHT:
        sendXp();
        break;
      case BTN_UP | BTN_LEFT:
        sendXmYp();
        break;
      case BTN_UP | BTN_RIGHT:
        sendXpYp();
        break;
      case BTN_DOWN | BTN_LEFT:
        sendXmYm();
        break;
      case BTN_DOWN | BTN_RIGHT:
        sendXpYm();
        break;
      default:
        // TODO: ack!
        break;
    }
  }
}
