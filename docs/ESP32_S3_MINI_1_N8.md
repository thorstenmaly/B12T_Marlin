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
| G-code RX: SDA (replaces AUX1) | 8 |
| G-code TX: SCL (replaces AUX2) | 9 |
| USB D- / D+ | 19 / 20 |

The host UART uses UART1 at 115200 baud, 8N1 on the former I2C connector J5.
Connect host TX to SDA / GPIO8 (RX, replacing AUX1), host RX to SCL / GPIO9
(TX, replacing AUX2), and ground to ground; use 3.3 V logic. The original J6
AUX pins (GPIO14 / GPIO13) are no longer used for host UART communication.
SDA and SCL are used as UART GPIOs; the Hall sensor and I2C remain unused.
USB is a second Marlin G-code interface, as well as the flashing interface.
Normal Marlin parsing and reply routing handle both ports. Coordinate commands
from both hosts share one motion planner and one position; coordinate the hosts
so their motion sequences don't interleave unexpectedly.

UART2 configures the TMC2209 independently of both host interfaces.
Its GPIO18 RX / GPIO17 TX mapping is set in early HAL initialization, before
Marlin starts the TMC UART. S3 UART2 defaults to GPIO19/20, so allowing a default
UART2 initialization first would disconnect native USB.
The driver address is 0: leave the MS1/MS2 jumpers open. The current calculation
uses the schematic's 0.10 ohm sense resistors. The existing Z settings are
configured: 240 mA RMS, 16 microsteps, 3200 steps/mm, 4 mm/s maximum,
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

TMCStepper 0.7.3 is an explicit dependency, fetched from its upstream GitHub
release archive so dependency installation doesn't require the PlatformIO
library registry.

Validation: `pio run -e esp32_s3_mini_1_n8` succeeds and generates
`.pio/build/esp32_s3_mini_1_n8/firmware.bin`. Reported usage is 22,172 bytes RAM
and 364,809 bytes flash. Firmware has not been uploaded or tested on hardware.

If USB disappears after running an older build, close serial monitors, hold
BOOT, press and release RESET, then release BOOT. Select the COM port that
appears in Windows and upload the corrected build. If Windows retains an
unresponsive port, unplug and reconnect the USB data cable while holding BOOT.

## USB startup diagnosis

The monitor explicitly sets DTR=0 and RTS=0 to avoid requesting reset/download
mode, and uses LF command termination. Restart an already open monitor after
changing these options. A COM port can also be the ROM downloader: its presence
alone does not establish that Marlin is running.

The separate `esp32_s3_mini_1_n8_usb_diagnostic` environment builds a standalone
USB heartbeat and byte-echo program. It keeps the motor enable inactive and does
not initialize the TMC UART or I2C. This diagnostic is not Marlin and cannot move
the Z axis. Build it with `pio run -e esp32_s3_mini_1_n8_usb_diagnostic` and upload
with the same environment plus `-t upload`. It should print a running message
once per second and echo input. The normal Marlin environment remains
`esp32_s3_mini_1_n8`.

Hardware investigation: the application, bootloader, and partition table were
read back and matched the local Marlin build; flash identification confirmed
8 MB quad flash. ROM communication worked, but M115 did not answer. Following a
full power cycle, Windows did not enumerate the USB device. The diagnostic
environment compiles successfully but has not yet been flashed or tested.
