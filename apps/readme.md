
[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)
# Examples applications

The following applications are provided to demonstrate the typical use case of BLE, Zigbee and Multiprotocol(BLE + Zigbee) with PIC32CX_BZ2/WBZ45x devices.

## BLE

### BLE Building Blocks

* [Getting Started](ble/building_blocks/readme.md)
    * [Peripheral](ble/building_blocks/peripheral/readme.md)
        * [FreeRtos and BLE Stack Setup](ble/building_blocks/peripheral/freertos_ble_stack_init_peripheral.md)
        * [Adding UART](ble/building_blocks/chip_peripherals/uart_hello_world/readme.md)
        * [Legacy Advertisement](ble/building_blocks/peripheral/legacy_adv/readme.md)
        * [Extended Advertisement](ble/building_blocks/peripheral/ext_adv/readme.md)
        * [Connection](ble/building_blocks/peripheral/peripheral_conn/readme.md)
        * [Profiles and Services](ble/building_blocks/peripheral/profiles_services/readme.md)
        * [Transparent UART](ble/building_blocks/peripheral/profiles_services/peripheral_trp_uart/readme.md)
        * [Custom Service](ble/building_blocks/peripheral/profiles_services/custom_service/readme.md)
	* [BLE Central Code PHY transparent UART](apps/ble/building_blockscentral/profiles_services/central_trp_uart_codedPhy/readme.md)
    * [Central](ble/building_blocks/central/readme.md)
        * [FreeRtos and BLE Stack Setup](ble/building_blocks/central/freertos_ble_stack_init_central.md)
        * [Adding UART](ble/building_blocks/chip_peripherals/uart_hello_world/readme.md)
        * [Legacy Scan](ble/building_blocks/central/legacy_scan/readme.md)
        * [Scan Extended Advertisement](ble/building_blocks/central/scan_ext_adv/readme.md)
        * [Connection](ble/building_blocks/central/central_conn/readme.md)
        * [Profiles and Services](ble/building_blocks/central/profiles_services/readme.md)
        * [Transparent UART](ble/building_blocks/central/profiles_services/central_trp_uart/readme.md)
        * [Transparent UART Multilink](ble/building_blocks/central/profiles_services/multilink/readme.md)
	* [BLE Peripheral PATH Loss Advertisement](apps/ble/building_blocks/peripheral/legacy_adv_pathloss/readme.md)
	* [BLE peripheral Code PHY transparent UART](apps/ble/building_blocks/peripheral/profiles_services/peripheral_trp_uart_codedPhy/readme.md)
	* [BLE Peripheral BLE Multiple Advertising Sets](apps/ble/building_blocks/peripheral/two_set_adv/readme.md)
    * [Multirole](ble/building_blocks/multirole/multilink/readme.md)

### BLE Advanced Applications
* [BLE Sensor App](ble/advanced_applications/ble_sensor/readme.md)
* [BLE Throughput](ble/advanced_applications/ble_throughput/readme.md)
* [BLE Peripheral ANCS](apps/ble/advanced_applications/ble_ancs_app/readme.md)
* [BLE Peripheral HOGPS or HID Profile](apps/ble/advanced_applications/ble_hogps_app/readme.md)


## IEEE 802.15.4

### IEEE 802.15.4 P2P PHY Application based on Standalone PHY
* [IEEE 802.15.4 P2P PHY App](apps/ieee_802_15_4/PHY/readme.md)
* [IEEE 802.15.4 P2P PHY App](apps/ieee_802_15_4/MAC/readme.md)
