# Microchip defined BLE OTA DFU Profile

The BLE OTA DFU profile is a GATT based profile. It is designed to perform device firmware update over the air. In general mobile will perform the OTAU client role and the BLE device will be in server role. OTAU service is a Microchip’s propriety service with a 16-byte service UUID. There shall be only one instance of the OTAS in a device. The OTAS shall be instantiated as a «Primary Service». The service Universally Unique Identifier \(UUID\) value shall be set to «4D434850-253D-46B3-9923-E61B8E8215D7»

There are 3 characteristics defined under the service:

|Characteristic Name|Universally Unique Identifier \(UUID\)|Properties|Permissions|
|OTA Feature|4D434850-22E4-4246-AF03-0C4A2F906358|Read|Encryption required|
|OTA Data|4D434850-34D9-40A6-BA7E-56F57C8CD478|WriteWithoutResponse, Notify|Encryption required|
|OTA Control Point|4D434850-9327-45DE-8882-C97F39028A76|Write|Encryption required|

Further details on OTAU service and profile can be found in stack documents

-   [BLE OTA Profile](https://microchip01.sdlproducts.com/ISHCS/DraftSpace/Publication/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C/1/en-US/?navigationId=GUID-DF914131-7CEA-4CF1-8F13-C5872AAB7DC6)

-   [BLE OTA Service](https://onlinedocs.microchip.com/pr/GUID-C5EAF60E-9124-427C-A0F1-F2DBE662EA92-en-US-1/index.html?GUID-ADB0AB34-DEE4-4107-8618-C3FBA1CDDDA1)


**Parent topic:**[Device Firmware Upgrade Over BLE](https://onlinedocs.microchip.com/pr/GUID-A5330D3A-9F51-4A26-B71D-8503A493DF9C-en-US-1/index.html?GUID-908446A4-F490-4063-9096-66C4831F9BE4)

