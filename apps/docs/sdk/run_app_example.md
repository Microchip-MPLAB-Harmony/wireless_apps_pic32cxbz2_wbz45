# Running a Precompiled Application Example

Here is how to open, build and program an existing application example

## Prerequisites {#GUID-B8B3C1E4-3168-4861-BB06-0775AE78A159 .section}

1.  Install [MPLAB X IDE](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-0D2F114A-B713-4B27-AC5E-4593D234D9B7)

2.  Install [XC32 Compiler](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-0D2F114A-B713-4B27-AC5E-4593D234D9B7)

3.  Install [Device Family Part Pack](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-3504C96C-3B1A-40E5-AF97-A3270CA7EC77)

4.  Install [MCC Plugin](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-A55E9342-CE44-4A91-86BB-FEC6706FCD1C)

5.  Install [Harmony 3 Dependency](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-5DEB6FE0-1234-4A15-A805-E451B3E73825)


## Open, Build and Program an existing application example {#GUID-917E027F-0699-4C53-828E-A2A7E2F7C5A9 .section}

1.  Connect Curiosity Board to the PC using usb cable

2.  Open MPLAB X IDE

3.  Select **File \> Open Project**

4.  **Browse to project location** of choice \(In this example we will open "ble\_sensor\_app"\) project, select project from location **"<Harmony Content Path\>\\wireless\_apps\_pic32cxbz2\_wbz45\\apps\\ble\\advanced applications\\ble\_sensor\_app\\firmware"**

    **Attention:**

    -   If multiple projects are open within IDE, IDE may not choose to build/program the "ble\_sensor\_app" project
    -   Select the "ble\_sensor\_app" project, Right click and select the setting "Set as Main Project"
    “Information related to the workings of the application example are available in readme file available in the ble\_sensor\_app folder”

5.  Open Project Properties

    -   Select WBZ451 Curiosity Board as hardware tool for programming

    -   Ensure correct DFP v1.0.xx is selected as mentioned in the Tools and Harmony Components

6.  select XC32 compiler

7.  Select option **Build Project** in IDE to compile the application example

8.  Select option **Run Project** in IDE to program the target – the onboard debugger will program the example application


**Note:** A smartphone App might be needed to explore the full feature set of Application examples

**Parent topic:**[Getting Started with Software Development](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-2AD37FE2-1915-4E34-9A05-79E3810726D7)

