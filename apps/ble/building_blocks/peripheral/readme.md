# Peripheral

BLE Peripheral devices send advertisement packets and accept connection requests from another [central device](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-E8C0DDA5-3AD0-42A4-B4EF-BCB02811BF8C). Connected BLE devices, enable bi-directional data transfer using the profiles and services. There are certain use cases where a connection between the devices \(Peripheral & Central\) is not essential and advertisement feature \(transmit only, no receive\) is sufficient

![](media/GUID-F3B97ABD-EDE5-4542-A771-093CBCB734C7-low.png)

## SDK Setup {#SDK-SETUP .section}

-   [Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7)


**Note:** SDK Setup is mandatory step before users start developing app examples based of Building Blocks

**Tip:** New users of MPLAB Code Configurator are recommended to go through the [overview](https://onlinedocs.microchip.com/pr/GUID-1F7007B8-9A46-4D03-AEED-650357BA760D-en-US-6/index.html?GUID-B5D058F5-1D0B-4720-8649-ACE5C0EEE2C0).

## Blocks {#BLOCKS .section}

-   [Create FreeRTOS project and configure BLE stack using MPLAB X Harmony Code Configurator 3](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-AB1A02BF-4F9B-4058-90D9-02BFB3136682)

    -   Create a new MCC Harmony project

    -   configure components like FreeRTOS and BLE Stack

    -   Init BLE Stack

-   [Adding UART](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-E1A0E9CA-311E-4F53-9C27-160173609975)

    -   print Hello World

-   [Legacy Advertisments](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-820CEA03-90AE-484F-87C9-9CF2CE652702)

    -   Transmit Data Payload using BLE Legacy Adv\(no BLE connection\)

    -   System enters "Sleep" mode in between ADV intervals

-   [Extended Advertisment](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-9A3C4C97-E09F-4FF9-878C-3873892445F4)

    -   Transmit Data Payload using BLE Extended Adv\(no BLE connection\)

-   [Connection](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-F9A0C390-C124-49A7-9F22-157D20BFBE5D)

    -   Configure stack for Peripheral Functionality

    -   Connection between Peripheral and Central Device

-   [Deep Sleep Advertisements](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-3D0C03E0-8EFC-455C-970B-17EBE5CB8F78)

    -   Transmit Data Payload using BLE Legact Adv

    -   System enters "Deep Sleep" mode in between ADV interval

    -   If a central device initiates connection, system enters "Sleep" mode and maintains connection

-   Profiles and services

    -   [Transparent UART Peripheral \(Legacy Adv + Connection + Data Exchange\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-07EC83B7-CC28-4C55-8634-4B7F56A9DB36)

        -   Data Exchange between 2 devices\(peripheral and central\) after a BLE connection using Microchip Transparent UART profile and service

    -   [Custom Service Peripheral \(Legacy Adv + Connection + Data Exchange\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-0F3193BF-C61D-4DDB-9E98-7F0A444269A8)

        -   Data Exchange between 2 devices\(peripheral and central\) after a BLE connection using Custom Service


-   **[Peripheral - FreeRTOS BLE Stack and App Initialize](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-AB1A02BF-4F9B-4058-90D9-02BFB3136682)**  

-   **[BLE Sleep Mode Legacy Advertisements](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-820CEA03-90AE-484F-87C9-9CF2CE652702)**  

-   **[BLE Extended Advertisements](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-9A3C4C97-E09F-4FF9-878C-3873892445F4)**  

-   **[BLE Deep Sleep Advertising](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-3D0C03E0-8EFC-455C-970B-17EBE5CB8F78)**  

-   **[BLE Connection](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-F9A0C390-C124-49A7-9F22-157D20BFBE5D)**  

-   **[BLE Transparent UART](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-07EC83B7-CC28-4C55-8634-4B7F56A9DB36)**  

-   **[BLE Custom service](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-0F3193BF-C61D-4DDB-9E98-7F0A444269A8)**  


**Parent topic:**[Building Block Examples](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-17DABF04-E5D8-4201-A746-2FC244450A19)

