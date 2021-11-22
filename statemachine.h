#pragma once

// This is the state machine that implements the SVG you may have seen in the
// ./img foldero

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
        blinkLed();
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
