# AutWat-Sys

ESP32 automatic plant watering controller in C++ (PlatformIO): per-plant moisture profiles, unit tested control logic, Wokwi simulation setup, WiFi and BLE status with manual override.

![CI](https://github.com/Mula8641/AutWat-Sys/actions/workflows/ci.yml/badge.svg)

Demo GIF: pending (needs a Wokwi run).

## Project background and my contribution

AutWat started as a five person M.Sc. Embedded Systems Design course project at Hochschule Bremerhaven. The original team built a working Arduino Uno prototype (pump, relay, LCD, pushbutton, potentiometer standing in for the moisture probe). I wrote the control software and the Proteus simulation and its test plan. This repository is my own rebuild of that software: modular C++ on an ESP32, unit tested, with a Wokwi simulation setup, and with WiFi and BLE remote monitoring that the original project did not complete (the first attempt with an ESP8266-01 and Blynk did not connect).

## What it does

- Three plant profiles (Cactus, AloeVera, ZamioculcasZamifolia) with their own thresholds and water dose.
- Emergency watering when the soil is too dry; lockout when it is too wet (no command can override it).
- Scheduled watering every 3 days, skipped when the soil is already wet. The run time is the dose divided by the flow rate (17.54 mL/s default).
- Pump safety cutoff: an emergency longer than 60 s becomes a Fault and the pump stops until the button is pressed.
- Air humidity and temperature (DHT22), shown on the 20x4 LCD.
- Status over WiFi (HTTP JSON) and BLE; manual override from the button, HTTP and BLE.

## Evidence status

| Feature | Unit tested | Simulated | Hardware |
|---|---|---|---|
| Soil moisture input with calibration constants | [moisture_scale tests](test/test_moisture_scale/test_main.cpp) | pending | pending |
| Humidity sensor (DHT22) | [JSON](test/test_status_json/test_main.cpp) and [BLE codec](test/test_status_codec/test_main.cpp) tests | pending | pending |
| Pump output with safety cutoff | [controller tests](test/test_watering_controller/test_main.cpp) | [scripted controller run](docs/img/scripted_run.txt) (PC program, not Wokwi) | pending |
| 3 day cycle with per-plant dose | [controller](test/test_watering_controller/test_main.cpp) and [dose](test/test_dose_calculator/test_main.cpp) tests | [scripted controller run](docs/img/scripted_run.txt) | pending |
| WiFi status endpoint | [JSON tests](test/test_status_json/test_main.cpp) | pending | pending |
| BLE status and command | [codec tests](test/test_status_codec/test_main.cpp); builds in CI | pending | pending |
| Plant health and overwatering lockout | [controller tests](test/test_watering_controller/test_main.cpp) | [scripted controller run](docs/img/scripted_run.txt) | pending |
| Manual override (button, HTTP, BLE) | [command](test/test_command/test_main.cpp) and [button](test/test_button_decoder/test_main.cpp) tests | pending | pending |

Nothing has been run on real hardware. The Wokwi simulation has not been run yet; the HTTP server and BLE code build but were never exercised.

## Architecture

- `lib/`: pure C++ decision logic, no `Arduino.h`, so it runs in tests on a PC.
- `src/hal/`: one file per device (moisture, DHT22, pump, buzzer, button, LCD).
- `src/boot/`, `src/net/`: boot flow, WiFi, HTTP server, BLE.
- `src/main.cpp`: wires modules together and runs a 100 ms control tick, with no `delay()`.

## State machine

See [docs/state_machine.md](docs/state_machine.md) (Mermaid diagram and the 15 rules).

## Wiring

See [docs/wiring.md](docs/wiring.md). The build is simulated for now; the wiring image is pending.

## Run the tests

```bash
pio test -e native
```

88 test cases across 10 suites, all passing at the last run. Needs a host C++ compiler (WSL or MinGW-w64 on Windows).

## Run the simulation

1. `cp include/secrets.h.example include/secrets.h`
2. `pio run -e esp32dev`
3. Open the folder in VS Code with the Wokwi extension and start the simulation (`wokwi.toml` and `diagram.json` are included but untested).

## Remote API

Trusted local network only: there is no authentication.

```bash
curl http://<device-ip>/status
curl -X POST http://<device-ip>/water
curl -X POST http://<device-ip>/stop
```

Sample `/status` body (format produced by the tested JSON writer):

```json
{"plant":"AloeVera","moisture_pct":42,"humidity_pct":55,"temp_c":23,"health":"ok","mode":"idle","pump":false,"alarm":"none","wifi_rssi":-60,"uptime_s":123}
```

BLE: see [docs/ble_protocol.md](docs/ble_protocol.md).

## Real hardware notes

- Calibrate `kRawDry` and `kRawWet` with the real probe; measure the flow rate and set `kFlowRateMlPerSec`.
- Confirm `kRelayActiveLow` against the real relay module.
- Remove `-DSIMULATED_TIME` from `platformio.ini` so one day is 24 h instead of 10 s.
- Record results in [docs/hardware_results.md](docs/hardware_results.md).

## What changed from version 1

Single file to modules; blocking `delay()` to non blocking time; no tests to unit tests; moisture clamp fix; scheduled run skip rule from the report; pump safety cutoff; 2 day to 3 day cycle; WiFi and BLE added.

## Limitations

Simulated sensors until hardware is built, no authentication, no persistence of the plant choice, no Blynk. Source conflicts and open questions: [docs/known_issues.md](docs/known_issues.md). Environment facts: [docs/environment.md](docs/environment.md).

## License

MIT
