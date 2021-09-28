#pragma once

void send(char key, const char* str) {
#if defined(DESCRIBE_NOT_TYPE)
  DBG(str);
#else
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(key);
  delay(1);
  Keyboard.releaseAll();
  delay(20);
#endif
}

void sendXYJogMultiply() {
  send(KEY_F1, "XY Multiply");
}

void sendXYJogDivide() {
  send(KEY_F2, "XY Divide");
}

void sendZJogMultiply() {
  send(KEY_F5, "Z Multiply");
}

void sendZJogDivide() {
  send(KEY_F6, "Z Divide");
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

// Get ready for an XY-jog
void jogXYPrep() {
  XY_SystemState_t target =
      (XY_SystemState_t)(isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP));
  while (target != xy_cur_jog) {
    if (target < xy_cur_jog) {
      sendXYJogDivide();
      xy_cur_jog--;
    } else {
      sendXYJogMultiply();
      xy_cur_jog++;
    }
  }
}

// Get ready for a Z-jog
void jogZPrep() {
  Z_SystemState_t target =
      (Z_SystemState_t)(isPressed(BTN_RBUMP) + isPressed(BTN_LBUMP));
  while (target != z_cur_jog) {
    if (target < z_cur_jog) {
      sendZJogDivide();
      z_cur_jog--;
    } else {
      sendZJogMultiply();
      z_cur_jog++;
    }
  }
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
