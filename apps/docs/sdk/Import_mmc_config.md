# Import existing App Example Configuration

Users who want to implement a similar application based on the existing application examples provided by Microchip can choose to import the configuration so that they srart from a known good configuration.

1.  Create a [new MPLAB MCC Harmony Project](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-F8FE2886-8A2C-4FC0-9956-C094CE44D162)

2.  Select the MPLAB Harmony option

    ![](media/GUID-302698EE-9869-4D77-94BF-362E0B834FD2-low.png "")

3.  Select Finish

    ![](media/GUID-168ACC11-EB74-4C5E-9C40-6CDB562AA361-low.png "")

4.  Select Harmony Framework

    ![](media/GUID-91CFB816-AE2F-414F-82F2-2AD2E68FBC36-low.png "")

5.  Default project graph will be loaded. Close the MPLAB Code Configurator by clicking on the MCC option in IDE

    ![](media/GUID-531449D6-5DF7-4CA6-9863-1E2FDB0072DE-low.png "")

    ![](media/GUID-851225A7-8360-4631-9CFA-320C34E65277-low.png "")

6.  User should now overwrite the .mc3 file created for the new MPLAB MCC Harmony Project \(in step 1\) with the .mc3 file of the application example of their choice provided by Microchip. For example we will now overwrite the .mc3 file \(created with the new MPLAB MCC Harmony Project\)with .mc3 file created for [legacy advertisement application](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-820CEA03-90AE-484F-87C9-9CF2CE652702)

    ![](media/GUID-489E1B74-9A91-4BAD-BD0E-9E0C7AD715B3-low.png "")

7.  Reopen MCC to see if project graph has imported the components and the respective configuration for these components as expected

    ![](media/GUID-851225A7-8360-4631-9CFA-320C34E65277-low.png "")

    ![](media/GUID-6B7A6340-AABD-4A24-B539-7C84AAA41ABF-low.png "")
