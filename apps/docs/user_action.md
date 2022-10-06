# **User Action**

This document explains to users the code edits that are needed to be made after a user does code generation using MPLAB Code Configurator.

Developers are required to call API - app\_idle\_task\(\) in vApplicationIdleHook\(\). This is required to enable periodic Persistent Data Storage, RF calibration. Persistant Data Storage provides means for storing data in non-volatile memory\(Flash\). User application and stack parameters like peer connection etc are saved. IDLE task is set to execute at the lowest priority in FreeRtos.

1.  Users will be prompted to call app\_idle\_task\(\) and app\_idle\_updateRtcCnt\(RTC\_Timer32CounterGet\(\)\) when they compile/build the project with a compile error.

    ![](media/GUID-A00F32C4-A6D4-429B-9685-B802325269D3-low.png "")

    action required to be taken

    ![](media/GUID-9B3D56EA-44CB-4F4A-A945-AD8B0C4F02CF-low.png)

2.  users are required to open freertos\_hooks.c file and add **\#include "definitions.h"**

    ![](media/GUID-08BBBE45-CCE5-46FE-9F03-64F4685CC001-low.png)

3.  users are required to open freertos\_hooks.c file and add API **app\_idle\_task\(\)**

    ![](media/GUID-EEC9F284-2194-48B4-8377-454481687C6A-low.png)

4.  users are required to comment out **\#error** in app\_user\_edits.c file

    ![](media/GUID-3BAE401A-06BE-458F-8BB4-1092BA1036BF-low.png)

5.  Initiate build and user will be able to build successfully

    ![](media/GUID-C5D994AE-26D0-4D52-9564-AEABEA101BD9-low.png)


**Note:** User action is only required for the first time code gets generated.

## Additional User Action when Low Power is enabled {#ADDITIONAL-USER-ACTION-WHEN-LOW-POWER-IS-ENABLED .section}

Developers are required to call app\_idle\_updateRtcCnt\(RTC\_Timer32CounterGet\(\)\) in vApplicationTickHook\(\). This API call is used to compensate for FreeRTOSTimer when in sleep mode.

![](media/GUID-0434E193-539B-4EDE-90D1-AC14F58AD571-low.png "")

1.  users are required to open freertos\_hooks.c file and add API **app\_idle\_updateRtcCnt\(RTC\_Timer32CounterGet\(\)\)**

    ![](media/GUID-7964FE1B-D00D-4FF8-B63A-C6E4F18C5A05-low.png)


**Parent topic:**[Support Documentation](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-2/index.html?GUID-5FC6D5A2-B8C3-4AD8-AC31-0AABE237A2F7)

