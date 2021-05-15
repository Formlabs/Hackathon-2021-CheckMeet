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

## Wiring

| Peripheral pin | NodeMCU pin | Note                       |
|----------------|-------------|----------------------------|
| LED strip GND  | G           |                            |
| LED strip 5V   | VU          |                            |
| LED strip DIN  | D2          | via a 200-500 ohm resistor |
| 7seg 5V        | VU          |                            |
| 7seg GND       | G           |                            |
| 7seg DIO       | D5          |                            |
| 7seg CLK       | D6          |                            |
| Button pin 1   | G           |                            |
| Button pin 2   | D3          | same as on board FLASH btn |

## Other software components (no need to install)

- UDP receiver: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/udp-examples.html
- https://arduinojson.org/
