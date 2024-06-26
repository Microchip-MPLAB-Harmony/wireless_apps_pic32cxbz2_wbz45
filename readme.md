![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)
# MPLAB® Harmony  3 Wireless BLE, Zigbee, IEEE 802.15.4 Standalone Phy  and Thread Application Examples

MPLAB® Harmony  is an extension of the MPLAB® ecosystem for creating
embedded firmware solutions for Microchip 32-bit SAM and PIC® microcontroller
and microprocessor devices.  Refer to the following links for more information.

- [Microchip 32-bit MCUs](https://www.microchip.com/design-centers/32-bit)
- [Microchip 32-bit MPUs](https://www.microchip.com/design-centers/32-bit-mpus)
- [Microchip MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide)
- [Microchip MPLAB® Harmony](https://www.microchip.com/mplab/mplab-harmony)
- [Microchip MPLAB® Harmony Pages](https://microchip-mplab-harmony.github.io/)

This repository contains the MPLAB® Harmony 3 Wireless BLE solutions and example applications. Quickly incorporate connectivity to your designs with replaceme ICs, modules, software and development kits that make connecting effortless for your customers. Our comprehensive replaceme portfolio has the technology to meet your range, data rate, interoperability, frequency and topology needs. Refer to the following links for release notes, training materials, and interface reference information.

- [Release Notes](./release_notes.md)
- [MPLAB® Harmony License](mplab_harmony_license.md)


# Contents Summary

| Folder     | Description                                               |
| ---        | ---                                                       |
| apps       | [Example applications for PIC32CXB2/WBZ45x devices](./apps/readme.md)         |
| docs       | [Wireless Application Example help documentation](https://onlinedocs.microchip.com/Oxy/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-3/index.html?GUID-7663617B-0DD1-45FA-86B5-EB0778A5A424)                       |


## Code Examples

The following applications are provided to demonstrate the typical use case of one or more Peripheral libraries.

| BLE Examples | Families |
| --- | :---: |
| [BLE Sensor App](apps/ble/advanced_applications/ble_sensor/readme.md) | PIC32CX-BZ |
| [BLE Throughput](apps/ble/advanced_applications/ble_throughput/readme.md) | PIC32CX-BZ |
| [BLE Peripheral ANCS](apps/ble/advanced_applications/ble_ancs_app/readme.md) | PIC32CX-BZ |
| [BLE Peripheral HOGPS or HID Profile](apps/ble/advanced_applications/ble_hogps_app/readme.md) | PIC32CX-BZ |
| [BLE Central Connection](apps/ble/building_blocks/central/central_conn/readme.md) | PIC32CX-BZ |
| [BLE Central Legacy Scan](apps/ble/building_blocks/central/legacy_scan/readme.md) | PIC32CX-BZ |
| [BLE Central Scan Ext Adv](apps/ble/building_blocks/central/scan_ext_adv/readme.md) | PIC32CX-BZ |
| [BLE Central transparent UART](apps/ble/building_blocks/central/profiles_services/central_trp_uart/readme.md) | PIC32CX-BZ |
| [BLE Central Multilink](apps/ble/building_blocks/central/profiles_services/multilink/readme.md) | PIC32CX-BZ |
| [BLE Central Code PHY transparent UART](apps/ble/building_blockscentral/profiles_services/central_trp_uart_codedPhy/readme.md) | PIC32CX-BZ |
| [BLE Peripheral Connection](apps/ble/building_blocks/peripheral/peripheral_conn/readme.md) | PIC32CX-BZ |
| [BLE peripheral Legacy Scan](apps/ble/building_blocks/peripheral/legacy_adv/readme.md) | PIC32CX-BZ |
| [BLE peripheral Ext Adv](apps/ble/building_blocks/peripheral/ext_adv/readme.md) | PIC32CX-BZ |
| [BLE peripheral transparent UART](apps/ble/building_blocks/peripheral/profiles_services/custom_service/readme.md) | PIC32CX-BZ |
| [BLE Custom Service](apps/ble/building_blocks/peripheral/profiles_services/peripheral_trp_uart/readme.md) | PIC32CX-BZ |
| [BLE Deep Sleep Adv](apps/ble/building_blocks/peripheral/deep_sleep_adv/readme.md) | PIC32CX-BZ |
| [BLE Peripheral PATH Loss Advertisement](apps/ble/building_blocks/peripheral/legacy_adv_pathloss/readme.md) | PIC32CX-BZ |
| [BLE peripheral Code PHY transparent UART](apps/ble/building_blocks/peripheral/profiles_services/peripheral_trp_uart_codedPhy/readme.md) | PIC32CX-BZ |
| [BLE Peripheral BLE Multiple Advertising Sets](apps/ble/building_blocks/peripheral/two_set_adv/readme.md) | PIC32CX-BZ |
| [UART](apps/ble/building_blocks/chip_peripherals/uart_hello_world/readme.md) | PIC32CX-BZ |
| [BLE Multirole](apps/ble/building_blocks/multirole/multilink/readme.md) | PIC32CX-BZ |
| [Bootloader](apps/bootloader/bootloader) | PIC32CX-BZ |





| IEEE 802.15.4 Standalone P2P PHY Application Examples | Families |
| --- | :---: |
| [IEEE 802.15.4 P2P PHY](apps/ieee_802_15_4/PHY/readme.md) | PIC32CX-BZ |
| [IEEE 802 15.4 MAC Push Pull](apps/ieee_802_15_4/MAC/readme.md) | PIC32CX-BZ |

| Zigbee Examples | Families |
| --- | :---: |
| [Zigbee Combined Interface (Coordinator)](apps/zigbee/combinedInterface/readme.md) | PIC32CX-BZ |
| [Zigbee Extended Lights (Router)](apps/zigbee/ext_light/readme.md) | PIC32CX-BZ |
| [Zigbee End Device (End Device)](apps/zigbee/multisensor/readme.md) | PIC32CX-BZ |
| [Zigbee Custom Cluster Server](apps/zigbee/custom_cluster_server/readme.md) | PIC32CX-BZ |
| [Zigbee Custom Occupancy Light](apps/zigbee/custom_occupancy_light/readme.md) | PIC32CX-BZ |
| [Zigbee Custom Cluster Client](apps/zigbee/custome_cluster_client/readme.md) | PIC32CX-BZ |
| [Zigbee Multisensor DeepSleep](apps/zigbee/multisensor_deepSleep/readme.md) | PIC32CX-BZ |

| Multiprotocol(BLE+Zigbee) Examples | Families |
| --- | :---: |
| [ble_zigbee_basic - Simple steps to bring the multiprotocol base project from H3 project generation](apps/multiprotocol/ble_zigbee_basic/readme.md) | PIC32CX-BZ |
| [ble_zigbee_light_prov - Zigbee light control and Zigbee commissioning over BLE](apps/multiprotocol/ble_zigbee_light_prov/readme.md) | PIC32CX-BZ |
| [ble_zigbee_multisensor - Zigbee Multisensor device and zigbee commissioning over BLE](apps/multiprotocol/ble_zigbee_multisensor/readme.md) | PIC32CX-BZ |
| [ble_zigbee_light_prov_wbz450 - Zigbee light control and Zigbee commissioning over BLE](apps/multiprotocol/ble_zigbee_light_prov_wbz450/readme.md) | PIC32CX-BZ |

| Thread Application Examples | Families |
| --- | :---: |
| [cli_ftd - CLI Application for Full Thread Device](apps/thread/building_blocks/cli/readme.md) | PIC32CX-BZ |
| [cli_mtd - CLI Application for Minimal Thread Device](apps/thread/building_blocks/cli/readme.md) | PIC32CX-BZ |
| [rcp_uart - Radio Co-Processor (RCP) Thread device(UART to communicate with the host)](apps/thread/building_blocks/rcp_uart/readme.md) | PIC32CX-BZ |
| [demo_app_udp_ftd - Thread UDP demo application for Full Thread Device](apps/thread/building_blocks/demo_app_udp/readme.md) | PIC32CX-BZ |
| [demo_app_udp_mtd - Thread UDP demo application for Minimal Thread Device](apps/thread/building_blocks/demo_app_udp/readme.md) | PIC32CX-BZ |
| [coap_ftd_client - Thread CoAP FTD client demo application](apps/thread/advanced_applications/CoAP/coap_ftd_client/readme.md) | PIC32CX-BZ |
| [coap_ftd_server - Thread CoAP FTD server demo application](apps/thread/advanced_applications/CoAP/coap_ftd_server/readme.md) | PIC32CX-BZ |
| [coap_sed_client - Thread CoAP MTD/SED client demo application](apps/thread/advanced_applications/CoAP/coap_sed_client/readme.md) | PIC32CX-BZ |
| [coap_sed_server - Thread CoAP MTD/SED server demo application](apps/thread/advanced_applications/CoAP/coap_sed_server/readme.md) | PIC32CX-BZ |

| Multiprotocol(BLE+Thread) Examples | Families |
| --- | :---: |
| [ble_thread_prov_ftd_light - FTD Light control and Thread commissioning over BLE](apps/multiprotocol/ble_thread_prov_ftd_light/readme.md) | PIC32CX-BZ |
| [ble_thread_prov_mtd_light - MTD Light control and Thread commissioning over BLE](apps/multiprotocol/ble_thread_prov_mtd_light/readme.md) | PIC32CX-BZ |
| [ble_thread_prov_sed_sensor - SED Sensor device and Thread commissioning over BLE](apps/multiprotocol/ble_thread_prov_sed_sensor/readme.md) | PIC32CX-BZ |

| Multiprotocol(BLE+MAC) Examples | Families |
| --- | :---: |
| [ble_mac_prov_ffd](apps/multiprotocol/ble_mac_prov_ffd/readme.md) | PIC32CX-BZ |
| [ble_mac_prov_rfd](apps/multiprotocol/ble_mac_prov_rfd/readme.md) | PIC32CX-BZ |




____

[![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45/blob/master/mplab_harmony_license.md)
[![Latest release](https://img.shields.io/github/release/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45/releases/latest)
[![Latest release date](https://img.shields.io/github/release-date/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45/releases/latest)
[![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45/graphs/commit-activity)
[![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45.svg)]()

____

[![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology)
[![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology)
[![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/)
[![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech)

[![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45.svg?style=social)]()
[![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/wireless_apps_pic32cxbz2_wbz45.svg?style=social)]()
