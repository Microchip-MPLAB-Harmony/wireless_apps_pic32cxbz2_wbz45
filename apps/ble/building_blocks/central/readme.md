# Central

BLE Central devices are devices that discover and listen to BLE devices that are advertising. These devices are capable of establishing connection to [peripherals](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-B3B46369-F5B4-401B-8405-658BE34988F4). Central devices are also capable of establishing multiple connections\(multilink\) at the same time. There are certain use cases where a connection between the devices \(Peripheral & Central\) is not essential and scanning \(Receive only, no transmit\) feature alone is sufficient. These type of devices are called observers.

**Tip:** New MPLAB Code Configurator Users are recommended to go through the [overview](https://onlinedocs.microchip.com/pr/GUID-1F7007B8-9A46-4D03-AEED-650357BA760D-en-US-6/index.html?GUID-B5D058F5-1D0B-4720-8649-ACE5C0EEE2C0).

## Prerequisites {#SDK-SETUP .section}

Install tool chain and harmony components documented in [Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7) section.

## Blocks {#BLOCKS .section}

1.  [FreeRTOS and BLE Stack Setup](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-897B75C0-387F-4AF8-B353-1F97A3B0BEC8)

    -   Create a new MCC Harmony project

    -   configure Harmony components like FreeRTOS and BLE Stack

    -   Init BLE Stack

2.  [Adding UART](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-E1A0E9CA-311E-4F53-9C27-160173609975)

    -   print Hello World

3.  [Legacy Scan](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-950762E2-DD5E-41CD-AAB4-895439CCFBC4)

    -   Receive Data Payload using BLE Legacy Scan\(no BLE connection\)

4.  [Scan Extended Advertisment](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-8C3372E5-CAB6-4923-9B59-E97022E1252D)

    -   Receive Data Payload using BLE Extended Scan\(no BLE connection\)

5.  [Connection](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-7C948236-7D8E-45CA-9511-AB244B44BE64)

    -   Configure stack for Central Functionality

    -   Connection between Peripheral and Central Device

6.  Profiles and services

    -   [Microchip propreitary - Transparent UART Central \(Legacy Scan + Connection + Data Exchange\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-7A350957-2010-468F-AF8D-0635373EF79D)

        -   Data Exchange using Microchip Transparent UART profile and service between 2 devices\(peripheral and central\) after a BLE connection

    -   [Multilink - Transparent UART Central \(Legacy Scan + Multilink Connection + Data Exchange between Multiple Links\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-741308A7-A912-4C74-BF94-D5C44132003D)

        -   Data exchange using Microchip Transparent UART profile and service between 4 devices \(1 central and 3 peripherals\) after a BLE connection.


-   **[Central - FreeRTOS BLE Stack and App Initialize](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-897B75C0-387F-4AF8-B353-1F97A3B0BEC8)**  

-   **[BLE Legacy Scan](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-950762E2-DD5E-41CD-AAB4-895439CCFBC4)**  

-   **[BLE Scanning Extended Advertisements](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-8C3372E5-CAB6-4923-9B59-E97022E1252D)**  

-   **[BLE Connection](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-7C948236-7D8E-45CA-9511-AB244B44BE64)**  

-   **[BLE Transparent UART](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-7A350957-2010-468F-AF8D-0635373EF79D)**  

-   **[BLE Multilink Transparent UART](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-741308A7-A912-4C74-BF94-D5C44132003D)**  


**Parent topic:**[Building Block Examples](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-17DABF04-E5D8-4201-A746-2FC244450A19)

