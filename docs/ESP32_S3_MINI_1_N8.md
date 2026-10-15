# ESP32-S3 Mini 1 N8

Build environment: `esp32_s3_mini_1_n8` (also the default in `platformio.ini`).
The target uses Arduino-ESP32 2.x through Espressif32 6.9.0, 8 MB flash,
no PSRAM, and the native USB Serial/JTAG interface.

## Connections

| Function | GPIO |
| --- | --- |
| Z STEP | 5 |
| Z DIR | 6 |
| Z enable (active low) | 21 |
| Z DIAG / sensorless home | 16 |
| TMC2209 UART TX / RX | 17 / 18 |
| AUX1 G-code RX / AUX2 G-code TX | 13 / 14 |
| USB D- / D+ | 19 / 20 |

AUX uses UART1 at 115200 baud, 8N1. Connect the host TX to AUX1,
host RX to AUX2, and ground to ground; use 3.3 V logic.
USB is a second Marlin G-code interface, as well as the flashing interface.
Normal Marlin parsing and reply routing handle both ports. Coordinate commands
from both hosts share one motion planner and one position; coordinate the hosts
so their motion sequences don't interleave unexpectedly.

UART2 configures the TMC2209 independently of both host interfaces.
The driver address is 0: leave the MS1/MS2 jumpers open. The current calculation
uses the schematic's 0.10 ohm sense resistors. The existing Z settings are
retained: 240 mA RMS, 16 microsteps, 5249.3438 steps/mm, 4 mm/s maximum,
100 mm/s^2 maximum acceleration, and 0..8 mm travel. These are mechanism-specific
values inherited from this project, not values inferred from the ESP32-S3 Mini 1 N8 controller.

Only Z has physical motor outputs. X and Y remain logical coordinates to retain
standard Marlin commands and XYZ position reporting; their step, direction, and
enable operations have no hardware effects. They have no homing endstops.
There are no extruders, heaters, fans, AS5600 reads, or I2C initialization.
The USB PD voltage-selection GPIOs retain their hardware defaults.

## Build and bring-up

1. Run `pio run -e esp32_s3_mini_1_n8`.
2. Connect a USB data cable. For download mode, hold BOOT, press and release
   RESET, then release BOOT.
3. Run `pio run -e esp32_s3_mini_1_n8 -t upload --upload-port COMx`, using the
   board's actual COM port.
4. Check `M115` and `M114` independently over USB and AUX, and check the driver
   UART with `M122` before attempting motion.
5. Verify motor current, direction, calibration, and travel against the attached
   mechanism. Existing sensorless Z homing is retained (`G28 Z`, DIAG GPIO16,
   sensitivity 8); its stall detection requires hardware tuning. For an initial
   movement test, establish a known position using standard `G92 Z...` and
   request a small Z move within the configured travel.

No G-code handler or command syntax has been changed. Hardware operation needs
validation on the actual board even after a successful compile.

Validation in the editing environment: the PlatformIO configuration was checked.
After fixing serial include order and the XYZ relative-mode array, the ESP32-S3
HAL sources compile. The full build currently stops because PlatformIO fails to
install TMCStepper, leaving `TMCStepper.h` unavailable. No complete firmware
binary or hardware validation is available.
