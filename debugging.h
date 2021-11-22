#pragma once

#if defined(DIAGNOSTICS)
#define DBG(a) Keyboard.print(a)
#define DBGN(n) TypeNumber(n)
#define DESCRIBE_NOT_TYPE 1

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

#else
#define DBG(a)
#define DBGN(a)
#define DumpState()
#endif
