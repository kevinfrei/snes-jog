#if defined(NEOPIXEL_PIN)
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel* pixel = nullptr;

#endif

void displayStateSetup() {
#if defined(DIGITAL_LED_PIN)
  pinMode(DIGITAL_LED_PIN, OUTPUT);
#elif defined(NEOPIXEL_PIN)
  pixel = new Adafruit_NeoPixel(1, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
  pixel->begin();
  delay(1);
  pixel->clear();
#endif
}

uint8_t getBreatheVal(uint32_t val) {
  return max(0, min(128, abs(128 - (int32_t)(val & 0xff))));
}
// Some state variables for an RGB led

// Some state variables for a digital led

// This gets called at *most* every 1 ms
void displayState(IIC_State_t cur_state,
                  State_t controller_state,
                  uint32_t cur_time) {
  if (cur_state == IIC_OK) {
#if defined(DIGITAL_LED_PIN)
    // For "OK" state, just blink at as 1 second pulse rate
    digitalWrite(LED_PIN), ((cur_time / 500) & 1) ? HIGH : LOW);
#elif defined(NEOPIXEL_PIN)
    // For a NeoPixel, we can communicate much more than "everything is ok"
    uint8_t breath = getBreatheVal(cur_time >> 4);
    switch (controller_state) {
      case ST_ControllerDisabled:
      case ST_MaybeEnabledA:
      case ST_MaybeEnabledB:
      case ST_MaybeEnabledC:
        // Breathe red for disabled
        pixel->setPixelColor(0, pixel->Color(breath/3, 0, 0));
        break;
      case ST_Enabled:
        // Breathe green for enabled
        pixel->setPixelColor(0, pixel->Color(0, breath/3, 0));
        break;
      case ST_WaitForResetX:
      case ST_WaitForResetY:
      case ST_WaitForResetZ:
        // Breathe blue for waiting for reset
        pixel->setPixelColor(0, pixel->Color(0, 0, breath));
        break;
      case ST_WaitForMaybeHome:
      case ST_MaybeGoHome:
      case ST_WaitForDisable:
      case ST_WaitForReturnToZero:
      case ST_WaitForSameJogUp:
      case ST_WaitForUnlock:
      case ST_WaitForReset:
      case ST_WaitForStart:
      case ST_WaitForSelect:
      case ST_WaitForSSUp:
      default:
        // Breathe with no red otherwise (eventually indicate stuff?)
        pixel->setPixelColor(0,
                             pixel->Color(getBreatheVal((cur_time >> 4) + 43) / 3,
                                          breath / 3,
                                          getBreatheVal((cur_time >> 4) + 86)));
        break;
    }
    pixel->show();
#endif
  }
}
