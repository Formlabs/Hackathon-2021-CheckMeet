# Service

TODO: write the section explaining that it's the script that sends the camera/mic status to the device and how to start it etc.

## Setup

To install the dependencies of the project, you have two options.

### requirements.txt

We provide a generated `requirements.txt` that describes the necessary packages to develop and run the project.

Activate your favorite virtual python environment or call it with your system-wide python:

```
pip install -r requirements.txt
```

### Pipfile

#### Prerequisites

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

#### Install

Once you have `pipenv` installed, in this folder, you can call

```
pipenv install
```

It will create the virtual python environment and install the dependencies for you.

You can activate the virtual python environment by `pipenv shell`.

## Run

TODO

```
pipenv run python launch_service.py
```
