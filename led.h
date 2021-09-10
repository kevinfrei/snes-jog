#pragma once

#if defined(LED_PIN)
void blinkLed() {
  pinMode(LED_PIN), OUTPUT);
  delay(1);
  digitalWrite(LED_PIN), HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
}
#else
void blinkLed() {}
#endif
