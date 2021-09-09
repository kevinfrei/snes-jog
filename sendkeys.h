#pragma once

void Send(char key, const char* str) {
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

