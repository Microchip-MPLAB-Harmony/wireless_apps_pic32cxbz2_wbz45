# **Peripheral - FreeRTOS BLE Stack and App Initialize**

[Getting Started](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-17DABF04-E5D8-4201-A746-2FC244450A19)

[Getting Started with Peripheral Building Blocks](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-B3B46369-F5B4-401B-8405-658BE34988F4)

[**FreeRTOS and BLE Stack Setup**](#GUID-AB1A02BF-4F9B-4058-90D9-02BFB3136682) **--\>** [Adding UART **--\>**](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-E1A0E9CA-311E-4F53-9C27-160173609975) [Legacy Advertisements\(sleep\)](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-820CEA03-90AE-484F-87C9-9CF2CE652702)

## Introduction {#INTRODUCTION .section}

This document will help users create a new MCC Harmony project, configure FreeRTOS component and BLE stack component in the project, generate code using the MPLAB Code Configurator

These examples each build on top on one and other. We strongly recommend that you follow the examples in order, to learn the basics concepts before progressing to the more advanced topics.

## Recommended Reads {#RECOMMENDED-READS .section}

1.  [BLE Software Specification](https://onlinedocs.microchip.com/pr/GUID-C5EAF60E-9124-427C-A0F1-F2DBE662EA92-en-US-2/index.html?GUID-222749FE-01C5-43B6-A5C7-CD82B3FC7F5F)


## Hardware Required {#HARDWARE-REQUIRED .section}

None

## Software Setup {#SOFTWARE-SETUP .section}

-   [Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7)


## Steps to Init BLE Stack {#STEPS-TO-INIT-BLE-STACK .section}

This section explains the steps required by a user to develop this application example from scratch using MPLABx Code Configurator

**Tip:** New users of MPLAB Code Configurator are recommended to go through the [overview](https://onlinedocs.microchip.com/pr/GUID-1F7007B8-9A46-4D03-AEED-650357BA760D-en-US-6/index.html?GUID-B5D058F5-1D0B-4720-8649-ACE5C0EEE2C0).

1.  Create a new MCC Harmony Project -- [link](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-B86E8493-D00D-46EF-8624-D412342147F0) for instructions

2.  Open MPLAB Code Configurator

    ![](media/GUID-6CDA8C98-9729-4155-AAE0-328DC72575AF-low.png)

    Default MPLAB Code Configurator window

    ![](media/GUID-843B5E55-F952-404F-B2CE-BF3C910D9759-low.png)

3.  In Available component window, **expand Wireless \> BLE** and **click plus symbol** the BLE stack component on to the project graph

    ![](media/GUID-F6B2EF19-063B-4DE7-9C0F-BC2812C8A0EB-low.png)

4.  Upon BLE Stack being added to project graph, the different component dependencies will be requested to be added. User has to select "yes" to add the dependant component

5.  Activate the components, select "yes"

    ![](media/GUID-E3C9F9E5-56D0-4584-A647-2E4FBEAB4270-low.png)

6.  **Verify** Project Graph

    ![](media/GUID-39F0881D-2C4B-4B61-83F3-09217C1A4B6E-low.png)

7.  Display FreeRTOS component configuration options by Selecting the component in Project Graph. **Configure FreeRTOS** component to the following. The configuration chosen here should suit most application needs, users are recommended to follow the FreeRTOS Customization documented [here](https://www.freertos.org/a00110.html) to meet their needs Tip: Upon selecting any component the default configuration options available for user are displayed

    ![](media/GUID-3C3991D5-F4C0-4B20-891C-D2B064B8CBD4-low.png "")

    ![](media/GUID-CCBDAE5C-0D37-4B2F-8EA9-54CEF1D25CEC-low.png "")

8.  Display BLE Stack component configuration options by Selecting the component in Project Graph.

    Default Configuration: Peripheral Device functionality is enabled by default.

    ![](media/GUID-0A55D1A3-4BFA-495F-BD48-13B8CA0CC920-low.png)

9.  [Generate Code](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-9C28F407-4879-4174-9963-2CF34161398E)

10. Switch to IDE window, right click on project and open Project Properties

11. Ensure correct compiler version as suggested in[Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7) is chosen

12. Build Project, upon building project user action is required as mentioned [here](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-99583057-8B1A-42F2-84E8-CFC7717FA5D1)

13. Build Project. The project will compile successfully. ![](media/GUID-A2C78CB7-D2B1-4440-AE3B-9837780EC09D-low.png)


## Where to go from here {#WHERE-TO-GO-FROM-HERE .section}

-   [UART](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-E1A0E9CA-311E-4F53-9C27-160173609975)


**Parent topic:**[Peripheral](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-B3B46369-F5B4-401B-8405-658BE34988F4)

