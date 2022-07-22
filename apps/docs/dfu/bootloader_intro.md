# Introduction

Microchip Utility Tool works with Microchip PIC32CXBZ2 /PIC32CX1012BZ20548 device family. This Tool Works on windows platform and provides the following functionality. This Tool communicates with the device over UART/PKOB4.

**Download** - [Microchip Utility tool](https://ww1.microchip.com/downloads/aemDocuments/documents/WSG/ProductDocuments/SoftwareTools/MicrochipUtilityTool_v1_1.zip)

## **Serial Bootloader** {#GUID-E0CA4125-690F-46D8-9B95-C2E5AF2E9D91 .example}

Bootloader enabled device is connected to the tool through Serial Port. Device is put in DFU mode and Binary Image\(\*.bin\) image is sent through Serial Port frame by frame \(payload size = 4096 bytes\). Details of this functionality is discussed in [Serial Bootloader Section](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-E24160FD-C371-4A9C-99F3-CB7C84DFD5C6).

## **Programming** \*.**hex file using MPLAB IPE** {#GUID-EF5F7D2D-A0F7-4027-B027-1E6E4BFB03F5 .example}

Image\(\*.hex\) is available in the local drive can be programmed using MPLAB IPE. Details are provided in in [Programming using MPLAB IPE section](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-931E0477-A4B1-4887-BC2C-370DC398B29C).

This Tools has three Tabs \(Zigbee OTAU, Serial Bootloader and Demo Store\). There is a common section, related to UART functionality which is visible in all three tabs of the tool. Details of each tab of the tool is described below.

**Parent topic:**[Microchip Utility Tool](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-A3F96AEA-7517-4859-9799-149B152B5BC4)

