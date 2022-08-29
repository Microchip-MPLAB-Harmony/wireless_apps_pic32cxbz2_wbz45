# APP_GetConnInfoByConnHandle

## C

```c
APP_BLE_ConnList_T *APP_GetConnInfoByConnHandle(uint16_t connHandle);
```

## Description

Returns the connection information based on connection handle.

## Parameters

|Parameter|Description|
|:---|:---|
|\[in\] connHandle|Handle assocaited with the Connection|

## Return values

|Return value|Description|
|:---|:---|
Info|Ble link related Info **APP_BLE_ConnList_T** Structure|
