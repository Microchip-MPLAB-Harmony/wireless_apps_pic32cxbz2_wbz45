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

This repository contains the MPLAB® Harmony 3 Wireless BLE solutions and example applications. Quickly incorporate connectivity to your designs with replaceme ICs, modules, software and development kits that make 
connecting effortless for your customers. Our comprehensive replaceme portfolio has the technology to meet your range, data rate, interoperability, frequency and topology needs. Refer to the following links for 
release notes, training materials, and interface reference information.

- [Release Notes](./release_notes.md)
- [MPLAB® Harmony License](mplab_harmony_license.md)


# Contents Summary

| Folder     | Description                                               |
| ---        | ---                                                       |
| apps       | [Example applications for PIC32CXB2/WBZ45x devices](./apps/readme.md)         |
| docs       | [Wireless Application Example help documentation]((https://onlinedocs.microchip.com/oxy/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C)  

# Examples applications

The following applications are provided to demonstrate the typical use case of BLE, Zigbee and Multiprotocol(BLE + Zigbee) with PIC32CX_BZ2/WBZ45x devices.

## BLE

### BLE Building Blocks
* [Getting Started](ble/building_blocks/readme.md)
    * [Peripheral](ble/building_blocks/peripheral/readme.md)
		* [BLE Deep Sleep Adv](ble/building_blocks/peripheral/deep_sleep_adv/readme.md)
        * [Extended Advertisement](ble/building_blocks/peripheral/ext_adv/readme.md)		
        * [BLE Legacy Advertisement](ble/building_blocks/peripheral/legacy_adv/readme.md)
		* [BLE Legacy Advertisement PATH Loss ](ble/building_blocks/peripheral/legacy_adv_pathloss/readme.md)
        * [Connection](ble/building_blocks/peripheral/peripheral_conn/readme.md)
		* [BLE Peripheral BLE Multiple Advertising Sets](ble/building_blocks/peripheral/two_set_adv/readme.md)
        * [Custom Service](ble/building_blocks/peripheral/profiles_services/custom_service/readme.md)
        * [Profiles and Services](ble/building_blocks/peripheral/profiles_services/readme.md)		
        * [Transparent UART](ble/building_blocks/peripheral/profiles_services/peripheral_trp_uart/readme.md)
		* [peripheral_trp_uart_codedPhy](ble/building_blocks/peripheral/profiles_services/peripheral_trp_uart/readme.md)		
	* [BLE Central Code PHY transparent UART](apps/ble/building_blockscentral/profiles_services/central_trp_uart_codedPhy/readme.md)
    * [Central](ble/building_blocks/central/readme.md)
        * [Connection](ble/building_blocks/central/central_conn/readme.md)
        * [Legacy Scan](ble/building_blocks/central/legacy_scan/readme.md)
        * [Profiles and Services](ble/building_blocks/central/profiles_services/readme.md)
        * [Transparent UART](ble/building_blocks/central/profiles_services/central_trp_uart/readme.md)
		* [BLE Central Code PHY transparent UART](apps/ble/building_blockscentral/profiles_services/central_trp_uart_codedPhy/readme.md)
        * [Transparent UART Multilink](ble/building_blocks/central/profiles_services/multilink/readme.md)
        * [Scan Extended Advertisement](ble/building_blocks/central/scan_ext_adv/readme.md)
	* [UART](ble/building_blocks/chip_peripherals/uart_hello_world/readme.md)
    * [Multirole](ble/building_blocks/multirole/multilink/readme.md)

### BLE Advanced Applications
* [BLE Advanced Applications](apps/ble/advanced_applications/readme.md)
* [BLE Peripheral ANCS](apps/ble/advanced_applications/ble_ancs_app/readme.md)
* [BLE Peripheral HOGPS or HID Profile](apps/ble/advanced_applications/ble_hogps_app/readme.md)
* [BLE Sensor App](ble/advanced_applications/ble_sensor/readme.md)
* [BLE Throughput](ble/advanced_applications/ble_throughput/readme.md)
* [BLE ble_anpc_app](ble/advanced_applications/ble_anpc_app/readme.md)
* [BLE ble_anps_app](ble/advanced_applications/ble_anps_app/readme.md)
* [BLE ble_pxpm_app](ble/advanced_applications/ble_pxpm_app/readme.md)
* [BLE ble_pxpr_app](ble/advanced_applications/ble_pxpr_app/readme.md)

## ZigBee Applications
* [zigbee](apps/zigbee/readme.md)
	* [ZIGBEE combinedInterface](apps/zigbee/combinedInterface/readme.md)
	* [ZIGBEE custom_occupancy_light](apps/zigbee/custom_occupancy_light/readme.md)	
	* [ZIGBEE ext_light](apps/zigbee/ext_light/readme.md)
	* [ZIGBEE multisensor](apps/zigbee/multisensor/readme.md)
	* [ZIGBEE multisensor_deepSleep](apps/zigbee/multisensor_deepSleep/readme.md)
	* [ZIGBEE custom_cluster_server](apps/zigbee/custom_cluster_server/readme.md)
	* [ZIGBEE custome_cluster_client](apps/zigbee/custome_cluster_client/readme.md)

## Multiprotocol Applications
* [multiprotocol](apps/multiprotocol/readme.md)
	* [MULTIPROTOCOL ble_zigbee_basic](apps/multiprotocol/ble_zigbee_basic/readme.md)
	* [MULTIPROTOCOL ble_zigbee_light_prov](apps/multiprotocol/ble_zigbee_light_prov/readme.md)
	* [MULTIPROTOCOL ble_zigbee_light_prov_wbz450](apps/multiprotocol/ble_zigbee_light_prov_wbz450/readme.md)	
	* [MULTIPROTOCOL ble_zigbee_multisensor](apps/multiprotocol/ble_zigbee_multisensor/readme.md)
	* [MULTIPROTOCOL ble_mac_prov_ffd](apps/multiprotocol/ble_mac_prov_ffd/readme.md)
	* [MULTIPROTOCOL ble_mac_prov_rfd](apps/multiprotocol/ble_mac_prov_rfd/readme.md)
	* [MULTIPROTOCOL ble_thread_prov_ftd_light](apps/multiprotocol/ble_thread_prov_ftd_light/readme.md)
    * [MULTIPROTOCOL ble_thread_prov_mtd_light](apps/multiprotocol/ble_thread_prov_mtd_light/readme.md)
	* [MULTIPROTOCOL ble_thread_prov_sed_sensor](apps/multiprotocol/ble_thread_prov_sed_sensor/readme.md)

## ieee_802_15_4 Applications
* [ieee_802_15_4](apps/ieee_802_15_4/readme.md)
	* [IEEE_802_15_4 demoapp_ffd](apps/ieee_802_15_4/MHC/push_pull_mac_app/demoapp_ffd/readme.md)
	* [IEEE_802_15_4 demoapp_rfd](apps/ieee_802_15_4/MHC/push_pull_mac_app/demoapp_rfd/readme.md)
	* [IEEE_802_15_4 p2p_phy](apps/ieee_802_15_4/PHY/p2p_phy/readme.md)

## Thread

### Thread Building Blocks
* [Building Blocks](apps/thread//building_blocks/readme.md)
    * [cli](apps/thread//building_blocks/cli/readme.md)
		* [Thread cli_ftd](thread/building_blocks/cli/cli_ftd/readme.md)
		* [Thread cli_mtd](thread/building_blocks/cli/cli_mtd/readme.md)		
    * [demo_app_udp](apps/thread//building_blocks/demo_app_udp/readme.md)
		* [Thread demo_app_udp_ftd](thread/building_blocks/demo_app_udp/demo_app_udp_ftd/readme.md)
		* [Thread demo_app_udp_mtd](thread/building_blocks/demo_app_udp/demo_app_udp_mtd/readme.md)
	* [rcp_uart](apps/thread//building_blocks/rcp_uart/readme.md)

### Thread Advanced Applications
* [Advanced Applications](apps/thread/advanced_applications/readme.md)
	* [CoAP](apps/thread//building_blocks/cli/readme.md)	
		* [Thread CoAP coap_ftd_client](thread/advanced_applications/CoAP/coap_ftd_client/readme.md)
		* [Thread CoAP coap_ftd_server](thread/advanced_applications/CoAP/coap_ftd_server/readme.md)
		* [Thread CoAP coap_sed_client](thread/advanced_applications/CoAP/coap_sed_client/readme.md)
		* [Thread CoAP coap_sed_server](thread/advanced_applications/CoAP/coap_sed_server/readme.md)		
	
**Parent topic:**[BLE Applications](https://onlinedocs.microchip.com/oxy/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C)

