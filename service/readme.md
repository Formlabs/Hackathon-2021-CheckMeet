# Service

TODO: write the section explaining that it's the script that sends the camera/mic status to the device and how to start it etc.

## Quick start guide

See Quick start guide [here](../QuickStart.md)

## For developers

### Dependencies

To install the dependencies of the project, you have two options.

#### Option 1: requirements.txt

We provide a generated `requirements.txt` that describes the necessary packages to develop and run the project.

Activate your favorite virtual python environment or call it with your system-wide python:

```
pip install -r requirements.txt
```

#### Option 2: Pipfile

##### Prerequisites

`pipenv` (https://pipenv.pypa.io/en/latest/) is a tool that can generate virtual python environments per project directory and can manage the project's dependencies.

You can install it with `pip` (https://pypi.org/project/pipenv/).

In short, for Windows:

```
pip install pipenv
```

For macOS, it's also available via `brew`:

```
brew install pipenv
```

For Linux, your package manager most probably has it, see https://pypi.org/project/pipenv/.

##### Setup environment

Once you have `pipenv` installed, in this folder, you can call

```
pipenv install
```

It will create the virtual python environment and install the dependencies for you.

You can activate the virtual python environment by `pipenv shell`.

### Launch the service

You can use the following command to launch the service in your virtual environment (replace `<DEVICE_IP>` with the IP address of your device):

```
python launch_service.py <DEVICE_IP>
```

If you're using `pipenv`, you can launch the service without explicitly activating the environment this way:

```
pipenv run python launch_service.py <DEVICE_IP>
```

### Source files

There are two main programs in this directory: `service.py` (checking the user's camera & microphone status, and sending it to the device) and the `emulator.py` (simulating a device by listening on the same port, and printing the incoming packets). Some common code among these two is stored in `common.py`.

`launch_service.py` and `launch_emulator.py` are lightweight wrappers checking if the user's using the required python version, then calling their respective implementation files.

The platform-specific parts of the code (checking whether the camera/microphone is turned on, showing notifications) are implemented in the `driver_*.py` files:
- `driver_win.py`, `driver_mac.py` and `driver_linux.py` implement the real features
- `driver_auto.py` checks the current platform, and loads one of the platform-specific drivers accordingly
- `driver_fake.py` can be used during testing when no real driver is implemented for a platform: it returns the values found in `fake.json`
