# opentherm-gateway-simple

A simple example of using OpenTherm gateway to monitor central heating parameters. Serves a webpage with live graphs using ThingSpeak.

Forked from https://github.com/diyless/opentherm-gateway-simple and modified as follows:

* Project file structure changed from `Arduino IDE` to `PlatformIO / VSCode IDE`
* `Support` Removed support for ESP8266. Supports only ESP32.
* ~~`Functionality` Enabled over-the-air (OTA) firmware updates via `http://.../update`~~
* `Functionality` Added a boiler communication timeout check. It will email an alert and reset the ESP.
* `Interface` Neater log using human-readable values
* `Interface` Added more read-outs to the webpage and ThingSpeak
* `Interface` Dark theme
* `Code` Refactored variable names to mimic OpenTherm protocol
* `Code` Cleaned up redundant code
* `Code` Using character arrays in favor of Strings
* `Code` Removed the `disable CH` and `disable DHW` manual overrides
* `Code` Added explanatory comments
* `Fix` Report correct `Flame` status

##

![screenshot](./images/screenshot.png)

## Regarding brownouts of my microcontroller

### 01-01-2025

My Wemos D1 Mini32 board (Lolin) with ESP-WROOM-32 chipset is reporting `Brownout detector was triggered`. This is very likely the cause for the intermittent hangs I have been experiencing. Users on the
internet (https://github.com/arendst/Tasmota/discussions/19886) report on a weak
3.3V LDO regulator on board of the Wemos, triggering the voltage brownout
warning. Either replace it with one sustaining 500 mA, or try adding a 1000 uF
capacitor to the existing LDO.

Existing LDO on my Wemos D1 Mini32 board:
- RT9193-33GB (SOT23-5 package, marking DE=A1D): 3.3 V, 300 mA

Suggested pin-compatible replacement:
- RT9013-33GB (SOT23-5 package): 3.3 V, 500 mA

### 02-01-2025

Brownouts resolved by keeping the original LDO and adding a 1000 uF, 6.3V electrolytic capacitor over the 3.3V and GND pins instead.
