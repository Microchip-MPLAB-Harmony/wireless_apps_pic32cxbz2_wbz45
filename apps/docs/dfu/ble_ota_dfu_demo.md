# BLE OTA DFU Demo

The below BLE OTA demo steps are common for any application implementing OTA functionality.

BLE sensor application is the sample project which implements OTA functioanlity as explained in<br /> the above steps. The current image \(ble\_sensor.X.production.signed.unified\_gpiobased.hex<br /> or ble\_sensor.X.production.signed.unified\_timerbased.hex\) with OTA capability and new<br /> upgrdable .bin \(ble\_sensor\_1.0.0.1\_enc\_ota.bin\) is also available in<br /> \\ble\\advanced\_applications\\ble\_sensor\\precompiled\_hex for your reference.

-   **Hardware Required:**


|Tool|Qty|
|<br /> WBZ451 Curiosity Board|1|
|Micro USB cable|1|
|Android/iOS Mobile|1|

-   **Smartphone App:** Microchip Bluetooth Data \(MBD\) iOS/Android app available in stores


-   **Demo steps:**

    1.  To perform the OTAU, the upgradable firmware bin file created in "[Upgradable image .bin file generation](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-6816EA2C-EDCC-48EC-9D99-BF8EAF56C5A1)" to be stored into MBD app. Send the upgrdable bin file to your mail. Follow the below screenshots to store the image received on mail into MBD app.

        -   **iOS Mobile:**
        ![](media/GUID-FC5154AA-EDA4-49BC-8CAE-0D7FCBE700CD-low.jpg)

        ![](media/GUID-0FCDE405-857D-4E07-854D-B5F1263FE1FF-low.jpg)

        ![](media/GUID-2FC7794B-C1EC-46D9-BCCD-EBF729ECF1F7-low.jpg)

        ![](media/GUID-9B87E2A3-2423-4223-9347-F01F428A9E3A-low.jpg)

        -   **Android Mobile:**

            ![](media/GUID-472BC8F0-D4BB-4B5E-93AA-FB67C1EAE6BE-low.jpg)

        ![](media/GUID-19DF5F03-6FCC-4C3A-A90E-65EE94B6BD4B-low.jpg)



        ![](media/GUID-67360FEE-06B0-4B25-8658-4A0406C05F04-low.jpg)

    2.  Turn on mobile Bluetooth and select **“OTA DFU"** tab of Microchip Bluetooth Data App on the mobile device.

        ![](media/GUID-B60BAA4E-62D1-4C60-8DCE-A5E19DF995A9-low.jpg)

    3.  Power On the OTA capable target device \( PIC32CXBZ2/WBZ451 \) which was programmed in "[Programming the Application using MPLABX IDE](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-436FCDB1-1B96-4B4C-802C-0C61967DA657)". Device will be advertising. Click on "SCAN" button in mobile app and the device be visible in scanned list.

        ![](media/GUID-295AF63F-EB80-4945-8CEC-266471708733-low.jpg)

    4.  Click on intended device from the scanned list. Mobile app will now get connected with OTA Target device. "Connected" message will appear for a short while.

        ![](media/GUID-AB0A1925-9937-4F59-8720-62812984E45A-low.jpg)

    5.  Click the ‘Select Image’ option to choose the available firmware file.

        ![](media/GUID-62D64D73-0E12-4FB0-8DCA-778CB117E24C-low.jpg)

    6.  The OTAU firmware image file copied in the step 1 will be visible on screen. Click on that image.

        ![](media/GUID-819F82DC-2E63-411A-99DD-65DA02E73384-low.jpg)

    7.  After confirming the FW version press OK button.

        ![](media/GUID-4B8CA7C7-7FCA-429E-A62E-E08F788EFF08-low.jpg)

    8.  Firmware update will be initiated and the progress will be shown in mobile app. The process would take few seconds.

        ![](media/GUID-8592457D-579A-4963-9599-EE060416DB49-low.jpg)

    9.  Once the complete image is transferred, "OTA Update successfully" status will be shown in mobile app. Click on "OK" and then device will perform authentication of the image. If validation is successful, then system reboot automatically.

        ![](media/GUID-ABA6E054-C7D8-43F2-97FA-BA778F6B3942-low.jpg)

        ![](media/GUID-B378692B-4EE5-4DCD-AB09-3AEB2F01F506-low.jpg)

    10. On bootup, device does the authentication of new image in Slot1 and then copies the new image to Slot0. Now the new image will be running from Slot0.
    11. To verify whether the running image on the target is newly upgraded image, follow steps 2 to 7 and check the current version.

**Parent topic:**[Device Firmware Upgrade Over BLE](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-908446A4-F490-4063-9096-66C4831F9BE4)

