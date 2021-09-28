#pragma once

// This is the state machine that implements the SVG you may have seen in the
// ./img foldero

void resetButtonStates() {
  cur_pressed = 0;
  controller_state = ST_ControllerDisabled;
}

// This is a hard-coded state machine documented in the readme
// It would probably be easier for folks to edit a declarative file
// Maybe, someday, but for now this works
void updateState() {
  switch (controller_state) {
    case ST_ControllerDisabled:
      if (cur_pressed == BTN_LBUMP) {
        controller_state = ST_MaybeEnabledA;
      }
      break;
    case ST_MaybeEnabledA:
      if (cur_pressed == (BTN_LBUMP | BTN_START)) {
        controller_state = ST_MaybeEnabledB;
      } else if (cur_pressed != BTN_LBUMP) {
        controller_state = ST_ControllerDisabled;
      }
      break;
    case ST_MaybeEnabledB:
      if (cur_pressed == BTN_LBUMP) {
        controller_state = ST_MaybeEnabledC;
      } else if (cur_pressed != (BTN_LBUMP | BTN_START)) {
        controller_state = ST_ControllerDisabled;
      }
      break;
    case ST_MaybeEnabledC:
      if (cur_pressed == BTN_NONE) {
        controller_state = ST_Enabled;
        blinkLed();
      } else if (cur_pressed != BTN_LBUMP) {
        controller_state = ST_ControllerDisabled;
      }
      break;
    case ST_Enabled:
      switch (cur_pressed) {
        case BTN_BBUMPS | BTN_X:
          controller_state = ST_WaitForResetX;
          break;
        case BTN_BBUMPS | BTN_Y:
          controller_state = ST_WaitForResetY;
          break;
        case BTN_LBUMP | BTN_START:
          controller_state = ST_WaitForDisable;
          break;
        case BTN_BBUMPS | BTN_START:
          controller_state = ST_WaitForResetZ;
          break;
        case BTN_BBUMPS | BTN_SELECT:
          controller_state = ST_WaitForMaybeHome;
          break;
        case BTN_LBUMP | BTN_SELECT:
          controller_state = ST_WaitForReturnToZero;
          break;
        case BTN_SELECT:
          controller_state = ST_WaitForStart;
          break;
        case BTN_START:
          controller_state = ST_WaitForSelect;
          break;
        case BTN_RBUMP | BTN_START:
          controller_state = ST_WaitForUnlock;
          break;
        case BTN_RBUMP | BTN_SELECT:
          controller_state = ST_WaitForReset;
          break;
        default: {
          uint16_t no_bumps = BTN_REMOVEBUMPS(cur_pressed);
          if (BTN_JOGS & no_bumps) {
            cur_jog = no_bumps;
            controller_state = ST_WaitForSameJogUp;
          }
          break;
        }
      }
      break;
    case ST_WaitForResetX:
      if (cur_pressed != (BTN_BBUMPS | BTN_X)) {
        if (cur_pressed == BTN_BBUMPS) {
          sendXZero();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForResetY:
      if (cur_pressed != (BTN_BBUMPS | BTN_Y)) {
        if (cur_pressed == BTN_BBUMPS) {
          sendYZero();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForResetZ:
      if (cur_pressed != (BTN_BBUMPS | BTN_START)) {
        if (cur_pressed == BTN_BBUMPS) {
          // Start was released
          sendZZero();
        }
        if (cur_pressed == (BTN_BBUMPS | BTN_START | BTN_SELECT)) {
          controller_state = ST_MaybeGoHome;
        } else {
          controller_state = ST_Enabled;
        }
      }
      break;
    case ST_WaitForDisable:
      if (cur_pressed != (BTN_LBUMP | BTN_START)) {
        if (cur_pressed == BTN_LBUMP) {
          controller_state = ST_ControllerDisabled;
        } else {
          controller_state = ST_Enabled;
        }
      }
      break;
    case ST_MaybeGoHome:
      if (cur_pressed != (BTN_BBUMPS | BTN_START | BTN_SELECT)) {
        if (cur_pressed == (BTN_BBUMPS | BTN_START) ||
            cur_pressed == (BTN_BBUMPS | BTN_SELECT) ||
            cur_pressed == BTN_BBUMPS) {
          sendHomeMachine();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForMaybeHome:
      if (cur_pressed == (BTN_BBUMPS | BTN_SELECT | BTN_START)) {
        controller_state = ST_MaybeGoHome;
      } else if (cur_pressed != (BTN_BBUMPS | BTN_SELECT)) {
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForReturnToZero:
      if (cur_pressed != (BTN_LBUMP | BTN_SELECT)) {
        if (cur_pressed == BTN_LBUMP) {
          sendReturnToZero();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForUnlock:
      if (cur_pressed != (BTN_RBUMP | BTN_START)) {
        if (cur_pressed == BTN_RBUMP) {
          sendUnlock();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForReset:
      if (cur_pressed != (BTN_RBUMP | BTN_SELECT)) {
        if (cur_pressed == BTN_RBUMP) {
          sendSoftReset();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForStart:
      if (cur_pressed != BTN_SELECT) {
        if (cur_pressed == (BTN_START | BTN_SELECT)) {
          controller_state = ST_WaitForSSUp;
        } else {
          controller_state = ST_Enabled;
        }
      }
      break;
    case ST_WaitForSelect:
      if (cur_pressed != BTN_START) {
        if (cur_pressed == (BTN_START | BTN_SELECT)) {
          controller_state = ST_WaitForSSUp;
        } else {
          controller_state = ST_Enabled;
        }
      }
      break;
    case ST_WaitForSSUp:
      if (cur_pressed != (BTN_START | BTN_SELECT)) {
        if (cur_pressed == BTN_START || cur_pressed == BTN_SELECT) {
          sendAllZero();
        }
        controller_state = ST_Enabled;
      }
      break;
    case ST_WaitForSameJogUp: {
      uint16_t no_bumps = BTN_REMOVEBUMPS(cur_pressed);
      if (no_bumps != cur_jog) {
        // This identifies only changed bits (xor)
        // And then masks out any that are pressed,
        // resulting in only the buttons that have been released
        sendJog((no_bumps ^ cur_jog) & ~cur_pressed);
      }
      controller_state = ST_Enabled;
      break;
    }
    default:
      controller_state = ST_ControllerDisabled;
      break;
  }
}
