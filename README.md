# A7V
Firmware for the STM32L052 MCU controlling my A7V tank bots.

There is a separate project for the remote control and the tank.
In each project, the main behavior is defined in Core/*

## Configuration & programming
Programming and config is achieved using a 6-pin-NL Tag-Connect cable with & ST-Link.

A devices address stored in EEPROM pairs the remote and the tank. This can be written to directly using the ST-Link at address 0x08080000.

Note that the MCU must hold an enable line high to maintain 3V3. This means the power button must be held during programming.

## Dependancies
The projects are dependant on the STM HAL layers. These have been left embedded in the projects.
They also use my STM32X HAL layer, which is added as a submodule.

## Environment
These projects are buit in the STMCube IDE v1.5.0.

