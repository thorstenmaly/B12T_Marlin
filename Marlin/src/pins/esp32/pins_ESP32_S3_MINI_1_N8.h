#pragma once

#include "env_validate.h"

#if NOT_TARGET(CONFIG_IDF_TARGET_ESP32S3)
  #error "ESP32-S3 Mini 1 N8 controller requires the esp32_s3_mini_1_n8 build environment."
#endif

#define BOARD_INFO_NAME "ESP32-S3 Mini 1 N8"
#define USES_USBCON

#if SERIAL_PORT != 1 || SERIAL_PORT_2 != -1
  #error "ESP32-S3 Mini 1 N8 controller uses SERIAL_PORT 1 (AUX) and SERIAL_PORT_2 -1 (USB)."
#endif
#if !ARDUINO_USB_MODE || !ARDUINO_USB_CDC_ON_BOOT
  #error "ESP32-S3 Mini 1 N8 controller requires native USB Serial/JTAG CDC."
#endif
#if EXTRUDERS != 0 || TEMP_SENSOR_0 != 0 || TEMP_SENSOR_BED != 0
  #error "ESP32-S3 Mini 1 N8 controller has no extruder or heater outputs."
#endif
#if ANY(I2S_STEPPER_STREAM, WIFISUPPORT, ESP3D_WIFISUPPORT)
  #error "ESP32-S3 Mini 1 N8 controller uses direct step outputs and reserves its second host port for USB."
#endif

// Only Z has physical motor outputs. Keep Marlin's XYZ command coordinates.
#define X_STEP_PIN -1
#define X_DIR_PIN -1
#define X_ENABLE_PIN -1
#define Y_STEP_PIN -1
#define Y_DIR_PIN -1
#define Y_ENABLE_PIN -1
#define X_STEP_INIT() NOOP
#define X_STEP_WRITE(V) NOOP
#define X_STEP_READ() false
#define X_DIR_INIT() NOOP
#define X_DIR_WRITE(V) NOOP
#define X_DIR_READ() false
#define X_ENABLE_INIT() NOOP
#define X_ENABLE_WRITE(V) NOOP
#define X_ENABLE_READ() true
#define Y_STEP_INIT() NOOP
#define Y_STEP_WRITE(V) NOOP
#define Y_STEP_READ() false
#define Y_DIR_INIT() NOOP
#define Y_DIR_WRITE(V) NOOP
#define Y_DIR_READ() false
#define Y_ENABLE_INIT() NOOP
#define Y_ENABLE_WRITE(V) NOOP
#define Y_ENABLE_READ() true

#define Z_STEP_PIN 5
#define Z_DIR_PIN 6
#define Z_ENABLE_PIN 21
#define Z_DIAG_PIN 16
#define Z_STOP_PIN Z_DIAG_PIN

// Separate hardware UART for the TMC2209. TX has the schematic's 1K resistor.
#define Z_HARDWARE_SERIAL Serial2
#define HARDWARE_SERIAL2_TX 17
#define HARDWARE_SERIAL2_RX 18
#define Z_SLAVE_ADDRESS 0 // MS1 / MS2 jumpers open (internal pulldowns).

// Host UART1 on J5: SDA replaces AUX1 (RX), SCL replaces AUX2 (TX).
// These pins are used as UART GPIOs; the Hall sensor / I2C bus is not initialized.
// USB uses GPIO19 / GPIO20.
#define AUX1 8
#define AUX2 9
#define ESP32_HOST_RX_PIN AUX1
#define ESP32_HOST_TX_PIN AUX2

// No AS5600, I2C bus, heaters, fans, or extruder outputs are configured.
