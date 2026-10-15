// Standalone diagnostic environment only. The Marlin build compiles no code here.
#ifdef ESP32_S3_USB_DIAGNOSTIC
#include <Arduino.h>

void setup() {
  // Keep the PD Stepper's active-low driver enable inactive. No STEP pulses.
  digitalWrite(21, HIGH);
  pinMode(21, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  static uint32_t last_report = 0;
  if (millis() - last_report >= 1000) {
    last_report = millis();
    Serial.println("ESP32-S3 USB diagnostic: running; motor disabled");
  }
  while (Serial.available() > 0)
    Serial.write(Serial.read());
  delay(1);
}
#endif
