#pragma once

void Send(char key,
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

void SendXYJogMultiply() {
  Send(KEY_F1, "XY Multiply");
}

void SendXYJogDivide() {
  Send(KEY_F2, "XY Divide");
}

void SendXYJogReset() {
  Send(KEY_PAGE_UP, "XY 10");
}

void SendZJogMultiply() {
  Send(KEY_F5, "Z Multiply");
}

void SendZJogDivide() {
  Send(KEY_F6, "Z Divide");
}

void SendZJogReset() {
  Send(KEY_PAGE_DOWN, "Z 1");
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

// These are here to make sure we're occasionally setting the jog speed, even if
// we don't think we have to. It's helpful just in case the user monkeys with
// UGS directly
bool tooLongXY() {
  uint32_t now = millis();
  bool tooLong = (now - lastJogXY) > (JOG_TIMEOUT * 1000);
  lastJogXY = now;
  return tooLong;
}

bool tooLongZ() {
  uint32_t now = millis();
  bool tooLong = (now - lastJogZ) > (JOG_TIMEOUT * 1000);
  lastJogZ = now;
  return tooLong;
}

// Get ready for an XY-jog
void JogXYPrep() {
  Jog_Magnitude_t target =
      (Jog_Magnitude_t)(isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP));
  if (tooLongXY() || target != XY_CurJog) {
    SendXYJogReset();
    switch (target) {
      case JM_LARGE:
        SendXYJogMultiply();
        break;
      case JM_SMALL:
        SendXYJogDivide();
        break;
      case JM_MEDIUM:
      default:
        // Do nothing here
        break;
    }
  }
  XY_CurJog = target;
}

// Get ready for a Z-jog
void JogZPrep() {
  Jog_Magnitude_t target =
      (Jog_Magnitude_t)(isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP));
  if (tooLongZ() || target != Z_CurJog) {
    SendZJogReset();
    switch (target) {
      case JM_LARGE:
        SendZJogMultiply();
        break;
      case JM_SMALL:
        SendZJogDivide();
        break;
      case JM_MEDIUM:
      default:
        // Do nothing heroe
        break;
    }
  }
  Z_CurJog = target;
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
