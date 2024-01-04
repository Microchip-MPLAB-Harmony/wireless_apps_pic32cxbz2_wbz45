[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)
# FTD and MTD CLI Application

[Getting Started with Thread](https://onlinedocs.microchip.com/oxy/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C)

## Introduction
CLI applications are responsible for managing the parsing services of the command line interface for different device roles, MTD and FTD, on Thread.
The CLI utilizes the Thread USART service to parse incoming CLI commands, output responses, and manage Thread log output. 

## Programming the precompiled hex file or Application Example

**Programming the hex file using MPLABX IPE**

1.  Precompiled Hex file is located in "<Harmony Content Path\>/wireless\_apps\_pic32cxbz2\_wbz45\\apps\\thread\\building\_blocks\\cli\\cli\_ftd\\hex" and "<Harmony Content Path\>/wireless\_apps\_pic32cxbz2\_wbz45\\apps\\thread\\building\_blocks\\cli\\cli\_mtd\\hex" for FTD and MTD respectively.

2.  Follow the steps mentioned [here](https://microchipdeveloper.com/ipe:programming-device)


**Caution:** Users should choose the correct Device and Tool information

**Programming the Application using MPLABX IDE**

1.  Follow steps mentioned in of [Running a Precompiled Example](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-EA74172C-595E-4A34-B359-D42EE443F0EC) document

2.  Open and program the Application Example "cli\_ftd.x" located in "<Harmony Content Path\>/wireless\_apps\_pic32cxbz2\_wbz45\\apps\\thread\\building\_blocks\\cli\\cli\_ftd\\firmware" and  "cli\_mtd.x" located in "<Harmony Content Path\>/wireless\_apps\_pic32cxbz2\_wbz45\\apps\\thread\\building\_blocks\\cli\\cli\_mtd\\firmware" using MPLABX IDE for FTD and MTD respectively.

## Developing this Application from scratch using MPLAB Code Configurator

Create a new MCC Harmony Project --[link](https://onlinedocs.microchip.com/oxy/GUID-2DB248AF-C243-496D-9819-969E43CA63BC) follow the steps in Topic "6.3 MCC Component Configuration" to create new CLI project using MCC.

## Building and Running the project

Building and running a project -- [link](https://onlinedocs.microchip.com/oxy/GUID-2DB248AF-C243-496D-9819-969E43CA63BC) follow the steps in Topic "6.5 Building and running a project" to build and run the project.

**Parent topic:**[MICROCHIP THREAD SDK](https://onlinedocs.microchip.com/oxy/GUID-2DB248AF-C243-496D-9819-969E43CA63BC)