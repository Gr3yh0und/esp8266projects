# ESP8266/ESP32 Smart Home & IoT Projects

A collection of embedded IoT projects for smart home automation, home brewing, garden control, and more. Built around NodeMCU (ESP8266) and ESP32 hardware with a Raspberry Pi backend.

## Architecture

```
Sensor → Device → Wi-Fi → MQTT (Mosquitto) → Telegraf → InfluxDB → Grafana
                                                                  ↓
                                                         Control via MQTT
```

**Backend:** Raspberry Pi running Mosquitto, Telegraf, InfluxDB, Grafana  
**Devices:** NodeMCU v3 (ESP8266), DOIT ESP32 DEVKIT V1, ESP32-C6-WROOM-1U  
**Protocol:** MQTT over Wi-Fi, Syslog for logging, OTA for firmware updates  
**MQTT root topic:** `cave/`

---

## Projects

### Brewing Station — *moved to its own repository*
The brewing station project (all four generations: original, PCB variant, current ESP32-C6 flagship, and the archived v1.0) has been extracted into its own repository at `D:\Projects\brewingstation` (local only, not yet pushed). It no longer lives in this repo.

---

### [Garden Control](garden) — *Active, v1.5, April 2019*
Controls garden irrigation and lighting via MQTT-triggered timed relays.

**Board:** NodeMCU ESP8266 v3  
**Actuators:** 3x relays — sprinkler, drip hose (*Tropfschlauch*), fairy lights (*Lichterkette*)  
**Sensor:** FS300A water flow meter (pulse counter interrupt)  
**Features:** Timer-based relay control (send seconds via MQTT, countdown published back), remote debug level toggle  
**MQTT topics:** `cave/garden/sprinkler`, `cave/garden/tropfschlauch`, `cave/garden/lichterkette`, `cave/garden/flow`  
**Includes:** Fritzing diagram

---

### [Ambient Sensor Station](ambientsensorstation) — *Active, v1.1, April 2020*
Indoor environmental monitoring station.

**Board:** NodeMCU ESP8266 v3  
**Sensors:** BME280 (temperature, humidity, pressure), TSL2561 (light/lux), CCS811 (CO2/TVOC — wired but disabled in code)  
**Features:** Deep sleep support (pin D0→RST), OTA updates  
**MQTT topics:** `cave/livingroom/temperature`, `cave/livingroom/humidity`, `cave/livingroom/pressure`, `cave/livingroom/brightness`  
**Includes:** Fritzing diagram, PCB layout, gerber files

---

### [Weather Station](weatherstation) — *Standalone, April 2018*
Oldest project. Local-only weather display — no Wi-Fi or MQTT.

**Board:** NodeMCU ESP8266 v3  
**Sensors:** BMP280 (pressure + temperature), SHT21/SI7021 (temperature + humidity), GL55 photoresistor (light)  
**Display:** SSD1306 128x64 OLED (SPI)  
**Output:** Local display only, serial debug  

---

### [Sorting Hat](sortinghat) — *Fun project*
Harry Potter sorting hat prop that randomly assigns a Hogwarts house and plays house-specific audio with LED ring animations.

**Board:** Arduino Nano (not ESP8266)  
**Components:** DFPlayerMini MP3 player, loudspeaker, 2x WS2811B NeoPixel LED rings (12 LEDs each)  
**Audio:** MP3s on microSD (Fat32, files in `/MP3` folder, named `0001-house-gryffindor`, etc.)  
**Includes:** Fritzing diagram, schematics

---

## Building

Every project and example is a self-contained PlatformIO project (its own `platformio.ini`, dependencies fetched via `lib_deps` rather than vendored in the repo). Open `esp8266projects.code-workspace` in VS Code to get all of them as separate PlatformIO projects in one window, switchable from the PlatformIO status-bar project picker, without needing to open each folder individually.

---

## Example Sketches ([examples/](examples/))

Reference implementations for individual components, each its own PlatformIO project:

`bme280`, `ccs811`, `ds18b20`, `dustsensor`, `flowsensor`, `inductionids2` (3 variants), `ledring`, `mqtt`, `ota`, `relay`, `soundlevel`, `ssd1306`, `syslog`, `temperature`, `timer`, `tsl2561`, `capacitivebutton`, `capacitivesensor`, `dfplayer`

---

## Hardware

- NodeMCU v3 (ESP8266) — garden, ambient sensor, weather station
- Arduino Nano — sorting hat, capacitivesensor, ledring, dfplayer examples
- Arduino Uno — soundlevel example
- Raspberry Pi (backend): Mosquitto, Telegraf, InfluxDB, Grafana, Raspbian

Brewing station hardware (ESP32 / ESP32-C6) now lives in the separate brewing station repository.

## Common Features (ESP8266/ESP32 projects)

- Wi-Fi station mode with auto-reconnect and reboot on failure
- OTA (Over-The-Air) firmware updates via ArduinoOTA
- UDP Syslog logging to Raspberry Pi
- MQTT for sensor data publishing and actuator control
- mDNS hostname registration (newer projects)

## License

GNU GPL v3

Pull requests and feedback welcome — open an issue or submit a PR.
