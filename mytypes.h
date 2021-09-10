#pragma once

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

enum XY_SystemState_t { XY_SMALL = 0, XY_MEDIUM = 1, XY_LARGE = 2 };
enum Z_SystemState_t { Z_SMALL = 0, Z_MEDIUM = 1, Z_LARGE = 2 };

