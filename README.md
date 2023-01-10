# opentherm-gateway-simple

A simple example of using OpenTherm gateway to monitor and/or override heating status commands.
For details see https://diyless.com/blog/opentherm-gateway-sample

Forked from https://github.com/diyless/opentherm-gateway-simple and modified as follows:

* Project file structure changed from `Arduino IDE` to `PlatformIO / VSCode IDE`
* Enabled over-the-air (OTA) firmware updates via `http://.../update`
* Refactored code: Variable names now mimic OpenTherm protocol
* Refactored code: Cleaned up redundant code
* Neater log using human-readable values
* Added `Tr` and `TrSet` to the webpage and ThingSpeak
* Fixed reporting incorrect `Flame status`
* Added reporting boiler slave status bits
* Commented out `disable CH` and `disable DHW` manual overrides
