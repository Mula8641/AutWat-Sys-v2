# Environment and verified facts

Only things actually run are listed as verified. Everything else says pending.

## Toolchain used

| Item | Version |
|---|---|
| PlatformIO Core | 6.2.0 |
| espressif32 platform | 7.1.3 (Arduino core 4.20017, xtensa gcc 8.4.0) |
| Host g++ for native tests | 13.3.0 (WSL Ubuntu 24.04) |
| DHT sensor library | 1.4.7 (+ Adafruit Unified Sensor 1.1.15) |
| LiquidCrystal | 1.0.7 |
| NimBLE-Arduino | 2.5.1 (code uses the 2.x callback signatures) |

## The four unverified facts from the plan

1. **Wokwi part names and LCD wiring: pending.** `diagram.json` uses `board-esp32-devkit-c-v4`, `wokwi-lcd2004`, `wokwi-potentiometer`, `wokwi-dht22`, `wokwi-relay-module`, `wokwi-buzzer`, `wokwi-pushbutton`, `wokwi-led`, `wokwi-resistor`. It has not been opened in Wokwi yet.
2. **LiquidCrystal compiles for esp32dev: verified.** Builds with 4 bit pin constructor.
3. **DHT22 + NimBLE + WiFi build together and fit: verified.** `pio run -e esp32dev` succeeds, no warnings in `src/` or `lib/`. RAM 16.9 % (55252 B), flash 80.9 % (1060925 of 1310720 B) with the default partition table, so `huge_app.csv` is not needed.
4. **Wokwi reaching the web server / simulating BLE: pending.** Not tested.

## Deviations and workarounds

- `include/config.h` is named `include/autwat_config.h`. The ESP32 build resolved `config.h` to another library's header first (first build failed with `kLongPressMs` not declared). `-Iinclude` is set in both environments so `lib/` can include it.
- Windows `python -m platformio` (Python 3.14) failed to install the espressif32 platform (`HTTPClientError`, while curl reached the registry). Native tests and the firmware build were run from WSL Ubuntu 24.04 (PlatformIO in a venv). CI uses Python 3.12.
- Building straight from `/mnt/d` took about 10 minutes; building from a copy on the WSL filesystem took 24 s. Use the WSL filesystem.

- **BLE in Wokwi: not simulated.** With the real BLE stack the ESP32 reset in Wokwi right after boot (task watchdog, `btController` starving IDLE0). `esp32dev` (used by Wokwi) now builds with `-DAUTWAT_NO_BLE` (stub, logs "[ble] disabled"). `esp32dev_ble` builds the real NimBLE code and is compiled in CI. BLE is therefore "builds in CI, hardware pending". Flash: 60.0 % without BLE, 80.9 % with BLE.

## Test status

- `pio test -e native`: 88 test cases, 88 passed (10 suites).
- `pio run -e esp32dev`: success.

## Simulation checklist (task 3.3)

Not run: Wokwi has not been started. All ten items are pending.
