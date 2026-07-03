# Project Context

Technical reference for the esp8266projects repository. Captures architecture decisions, hardware details, and non-obvious facts that aren't obvious from reading the code.

---

## Repository Overview

Personal smart home IoT collection by Michael Morscher. Started 2018, actively developed through 2023. Projects share a common backend stack and follow the same connectivity pattern (Wi-Fi → MQTT → InfluxDB → Grafana), but each sketch is self-contained.

---

## Hardware Platforms

| Platform | Projects |
|---|---|
| NodeMCU v3 (ESP8266) | weatherstation, garden, ambientsensorstation |
| Arduino Nano | sortinghat |

The brewing station project (ESP32 / ESP32-C6, all four generations) was extracted into its own repository at `D:\Projects\brewingstation` on 2026-07-03 and no longer lives here. See that repo's own CLAUDE.md/context for its hardware and firmware details.

All projects and examples in this repo were migrated from classic Arduino IDE + a shared `libraries/` submodule folder to individual PlatformIO projects (own `platformio.ini`, `lib_deps` fetched at build time). The `libraries/` folder and `.gitmodules` have been removed — nothing in this repo depends on them anymore. Open `esp8266projects.code-workspace` in VS Code to get every project as a separate PlatformIO root.

---

## Backend Stack

All ESP8266/ESP32 projects expect:
- **Raspberry Pi** at a fixed LAN IP (192.168.0.51 in most projects, 192.168.0.100 in newer config example)
- **Mosquitto** MQTT broker on port 1883
- **Syslog** listener on UDP port 514
- **InfluxDB** + **Telegraf** + **Grafana** for storage and visualization

No cloud services. Everything is local LAN. VPN access was planned but not implemented.

---

## MQTT Topic Structure

Root namespace: `cave/`

```
cave/
  garden/
    sprinkler         ← relay on/off (send seconds for timed, 0 to stop)
    tropfschlauch     ← drip hose relay
    lichterkette      ← fairy lights relay
    flow              ← water flow in l/s (published by device)
    dust25            ← (planned) PM2.5
    dust10            ← (planned) PM10
    debug             ← 0/1 toggles debug log level
  livingroom/
    temperature       ← °C from BME280
    humidity          ← % from BME280
    pressure          ← hPa from BME280
    brightness        ← lux from TSL2561
    co2               ← ppm from CCS811 (disabled)
    tvoc              ← ppb from CCS811 (disabled)
```

The `brewery/` topic namespace (induction cooker, PID, brew timer) belongs to the brewing station project, now in its own repository.

---

## Garden Relay Logic

The Sainsmart relay module used is **active-LOW**: `HIGH` = relay OFF, `LOW` = relay ON. This is explicitly noted in the code comments and is easy to get wrong.

Timer operation: MQTT payload is number of seconds to keep the relay on. The device counts down and publishes the remaining seconds back to the same topic. Sending 0 cancels immediately. Sending a larger value than current timer extends it; smaller values are ignored.

---

## Ambient Sensor Station Notes

- CCS811 (CO2/TVOC) is wired and code is written for it, but it is **commented out** in both setup and loop. The sensor was problematic.
- Deep sleep is implemented but **commented out**. Enabling it requires a physical jumper from D0 to RST on the NodeMCU.
- Uses the `256dpi/arduino-mqtt` library (not PubSubClient).

---

## Sorting Hat Notes

- Not an ESP8266 project — Arduino Nano only.
- DFPlayer Mini has known compatibility issues with clone modules. The README specifically notes it uses the "fake with red LED" version. MP3 file loading order on the SD card matters (not alphabetical, physical copy order).
- MP3s must be in a folder named `MP3` (uppercase) on a Fat32 SD card.

---

## Configuration Pattern

Sensitive values (SSID, passwords, IPs) are stored in `config.h` which is gitignored. Each project with this pattern includes a `config_example.h` with placeholder values. Copy and rename to configure.

Projects without a separate config file have credentials hardcoded in the `.ino` — these are the older projects (garden, ambientsensorstation, weatherstation).

---

## Known TODOs in Codebase

- CCS811 gas sensor disabled in ambientsensorstation
- Deep sleep disabled in ambientsensorstation
- Dust sensor topics defined in garden but never published

---

## Library Notes

MQTT library used across projects:
- `256dpi/arduino-mqtt` (`<MQTT.h>`) — garden, ambientsensorstation, examples/mqtt

Two different OLED libraries:
- `squix78/esp8266-oled-ssd1306` (`SSD1306Wire.h` / legacy `SSD1306.h`+`SSD1306Spi.h`) — weatherstation, dustsensor/flowsensor/relay/ssd1306 examples
- `Adafruit_SSD1306` — not currently used by any project in this repo

Each project/example now declares its own dependencies in its `platformio.ini` `lib_deps`, pinned to the commit the old vendored `libraries/` submodule was at when the migration happened (where one existed). `LiquidCrystal_I2C` (used by the two dated `inductionids2` variants) is the one exception — it was never vendored here, so it's unpinned (latest master).
