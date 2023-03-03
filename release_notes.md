---
title: Release notes
nav_order: 99
---

![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLABÂ® Harmony 3 Release Notes

## Harmony 3 Wireless IEEE 802.15.4 P2P Phy Application example for PIC32CXBZ family  **v1.2.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| WBZ451 Curiosity Board |       1            |

### New Features
- WBZ451 IEEE 802.15.4 Standalone PHY MCC compoenent based P2P Phy application.

### Bug fixes and Improvements
- New applications release.


### Known Issues
- 

### Limitations
- In IEEE 802.15.4 Standalone PHY based P2P Phy application, while using Data Mode, max data bytes/payload limit is 232 bytes (due to SYS CONSOLE Component limitation).

### Development Tools

- [MPLABÂ® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide)
- MPLABÂ® X IDE plug-ins:
  - MPLABÂ® Code Configurator (MCC) `v5.2.2`
- [MPLABÂ® XC32 C/C++ Compiler v4.20](https://www.microchip.com/mplab/compilers)
- **DFP 1.1.218 or above version**


--------------------------------------------------------------------------------------------------------------------
# Microchip MPLABÂ® Harmony 3 Release Notes

## Harmony 3 Wireless BLE application examples for PIC32CXBZ family  **v1.1.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| WBZ451 Curiosity Board |       6            |

### New Features
- WBZ451 ZigBee applications.
- WBZ451 BLE and ZigBee Combo applications.
- WBZ451 IEEE 802.15.4 Standalone PHY MCC compoenent based P2P Phy application.

### Bug fixes and Improvements
- New applications release.


### Known Issues
- In the Zigbee Multisensor application, System sleep Implementation source(device_sleep.c) and header(device_sleep.h) files are may not be included sometimes when regenerating the project through MCC. Work Around is remove and add the Zigbee multisensor component again in the MCC configuration.
- In IEEE 802.15.4 Standalone PHY based P2P Phy application, while using Data Mode, max data bytes/payload limit is 232 bytes (due to SYS CONSOLE Component limitation).

### Development Tools

- [MPLABÂ® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide)
- MPLABÂ® X IDE plug-ins:
  - MPLABÂ® Code Configurator (MCC) `v5.1.9`
- [MPLABÂ® XC32 C/C++ Compiler v4.10](https://www.microchip.com/mplab/compilers)
- **DFP 1.0.107 or above version**


--------------------------------------------------------------------------------------------------------------------
# Microchip MPLABÂ® Harmony 3 Release Notes

## Harmony 3 Wireless BLE application examples for PIC32CXBZ family  **v1.0.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| WBZ451 Curiosity Board |       16            |

### New Features
- WBZ451 BLE advanced applications.
- WBZ451 BLE building block applications.

### Bug fixes and Improvements
- New applications release.


### Known Issues
- In the DeepSleep application, System sleep Implementation source(device_sleep.c) and header(device_sleep.h) files are may not be included sometimes when regenerating the project through MCC. Work Around is provided in the DeepSleep application document.

### Development Tools

- [MPLABÂ® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide)
- MPLABÂ® X IDE plug-ins:
  - MPLABÂ® Code Configurator (MCC) `v5.1.9`
- [MPLABÂ® XC32 C/C++ Compiler v4.10](https://www.microchip.com/mplab/compilers)
- **DFP 1.0.107 or above version**
