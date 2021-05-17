## Setting up arduino for development

https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/

Libraries to install:
- [WiFiManager by tzapu,tablatronix](https://github.com/tzapu/WiFiManager)
- [FastLED by Daniel Garcia](http://fastled.io)
- [TM1637 by Avishay Orpaz](https://github.com/avishorp/TM1637)

The bootloaders seem to communicate using 74880 baud so we use it as well.

On Mac / Big Sur I needed this:

https://www.tweaking4all.com/forum/postid/3176/

On Windows driver is needed:

https://github.com/nodemcu/nodemcu-devkit/blob/master/Drivers/CH341SER_WINDOWS.zip

Note: board is NodeMCU v0.9

## Running the unit tests

For Mac (and probably Linux) run the helper script `test.sh`.

## Other software components (no need to install)

- UDP receiver: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/udp-examples.html
- https://arduinojson.org/

## Wiring instructions

See [here](../doc/BuildTheDevice.md)
