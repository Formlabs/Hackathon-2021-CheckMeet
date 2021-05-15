# Quick start guide

To use this software, you need two components:
- a _device_ with an installed _firmware_, connected to your Wi-Fi network
- your computer(s) running the _service_

## Device

### Build the device

If you don't already have a physical device, you need to build it. Find instructions [here](BuildTheDevice.md)

### Install latest firmware on device

You have two options: you can either download the firmware from our [Releases](https://github.com/Formlabs/hackathon-2021-i-am-on-a-meeting-sign/releases) page, then upload it to your device, or you can compile it manually.

#### Option 1: Download firmware from Releases page

TODO

#### Option 2: Compile firmware manually

TODO

### Connect the device to your Wi-Fi network

TODO

### Find out the IP address of your device

You need to check your router's status page to find out the IP address of your device.

## Service

You have two options: you can either download the service binary from our [Releases](https://github.com/Formlabs/hackathon-2021-i-am-on-a-meeting-sign/releases) page, or you can run it from code.

### Option 1: Download service from Releases page

TODO

### Option 2: Run it from source

#### Get the source

You can either clone this repository, or grab it as a .zip file from [here](https://github.com/Formlabs/hackathon-2021-i-am-on-a-meeting-sign/archive/refs/heads/master.zip).

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
