# LoRaNodeGarage

[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![pre-commit.ci status](https://results.pre-commit.ci/badge/github/ovenystas/LoRaNodeGarage/master.svg)](https://results.pre-commit.ci/latest/github/ovenystas/LoRaNodeGarage/master)
[![code style: prettier](https://img.shields.io/badge/code_style-prettier-ff69b4.svg?style=flat-square)](https://github.com/prettier/prettier)

A LoRa wireless node for a car garage with multiple sensors and actuators.

Can sense presence of a car by measuring distance from inner garage roof to
object that can be a car roof or garage floor.

Also senses temperature and humidity.

Can also act as a cover device to sense state of garage port and open or close
the port.

Has support for Home Assistant discovery to send information on available
sensors to Home Assistant. This needs a LoRa to MQTT gateway. See
<https://github.com/ovenystas/lora2mqtt> for that.

## Build project

Use PlatformIO to build the project.

```bash
pio run build
```

## Upload to target

To upload to Arduino board:

```bash
pio run upload
```

## Unit Test

Unit tests can be compiled and run with the command:

```bash
pio test
```

The tests uses googletest and googlemocks are there to mock Arduino core and
addon libraries.

## Static Code Analysis

PlatformIO uses cppcheck for static code analysis. Run it with:

```bash
pio check
```
