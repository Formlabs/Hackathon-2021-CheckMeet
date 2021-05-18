# Quick start guide

To use this software, you need two components:
- a _device_ with an installed _firmware_, connected to your Wi-Fi network
- your computer(s) running the _service_

## Device

### Build the device

If you don't already have a physical device, you need to build it. Find instructions [here](BuildTheDevice.md)

### Install latest firmware on device

You have two options: you can either download the firmware from our [Releases] page, then upload it to your device, or you can compile it manually.

Independently of which approach you pick, you need to connect your device through a microUSB cable.
Make sure the cable is not a charge only one and it has data wires!
When the device is connected to the computer, a new serial device will come up in the operating system, something like

- `/dev/cu.usbserial-143330` for MacOS,
- `COM4` for Windows (you need to install [this driver](https://github.com/nodemcu/nodemcu-devkit/blob/master/Drivers/CH341SER_WINDOWS.zip) first, then check for a new "Port" device in Device Manager), or
- `TODO` for Linux.

This will be referred to `<PORT>` below.

#### Option 1: Download firmware from Releases page

1. Download `firmware-vXXX.ino.nodemcu.bin` from the [Releases] page.
2. Install esptool by running `pip3 install esptool`
3. Run the following command (replacing `<PORT>` and `<FIRMWARE_FILE_NAME>` with the appropriate values):

```
esptool.py --chip esp8266 --port <PORT> --baud 115200 --before default_reset --after hard_reset write_flash 0x0 <FIRMWARE_FILE_NAME>
```

#### Option 2: Compile firmware manually

1. Set up the development environment as described in [this document](../firmware/README.md).
2. Open `firmware.ino` in the Arduino IDE.
3. `Menu` / `Sketch` / `Upload`

### Connect the device to your Wi-Fi network

An unconfigured device always boot up acting as a Wi-Fi access point with a name like `ESP_abcdef`.

1. Connect to the device via Wi-Fi, a cellphone can be used for this.
2. The device will present a captive portal where the Wi-Fi can be configured.
   If it doesn't pop up for some reason, navigate to http://192.168.4.1/.
3. Click `Configure WiFi`
4. Pick the Wi-Fi access point the device needs to connect to and enter the password.

### Find out the IP address of your device

You need to check your router's status page to find out the IP address of your device.

## Service

You have two options: you can either download the service binary from our [Releases] page, or you can run it from code.

### Option 1: Download service from Releases page

TODO

### Option 2: Run it from source

#### Get the source

You can either clone this repository, or grab it as a .zip file from [here](https://github.com/Formlabs/Hackathon-2021-CheckMeet/archive/refs/heads/master.zip).

#### Install pipenv

The service uses `pipenv` to manage dependencies. You can install it using `pip`:

```
pip install pipenv
```

For macOS, it's also available via `brew`:

```
brew install pipenv
```

For Linux, your package manager most probably has it, see https://pypi.org/project/pipenv/.

#### Launch the service

Open a command line in the `service` directory, then use the following command to launch the service (replace `<DEVICE_IP>` with the IP address of your device):

```
pipenv run python launch_service.py <DEVICE_IP>
```

Congratulations, your status is now displayed on the device!

[Releases]: https://github.com/Formlabs/Hackathon-2021-CheckMeet/releases
