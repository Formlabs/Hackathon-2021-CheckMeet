## Setting up the development environment

The first step is to install PlatformIO by following [this article][platformio-install].

PlatformIO takes care of the following:

- install cross compiler environment for nodemcu
- install libraries CheckMeet depends on
- compile the project

The following command needs to be issued to achieve this after PlatformIO
was installed.

```
pio run
```

The bootloaders seem to communicate using 74880 baud so we use it as well.

On Mac / Big Sur I needed this:

https://www.tweaking4all.com/forum/postid/3176/

On Windows driver is needed:

https://github.com/nodemcu/nodemcu-devkit/blob/master/Drivers/CH341SER_WINDOWS.zip

Note: board is NodeMCU v0.9

[platformio-install]: https://docs.platformio.org/en/latest//core/installation.html

## Running the unit tests

For Mac (and probably Linux) run the helper script `test.sh`.

## Other software components (no need to install)

- UDP receiver: https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/udp-examples.html
- https://arduinojson.org/

## Wiring instructions

See [here](../doc/BuildTheDevice.md)
