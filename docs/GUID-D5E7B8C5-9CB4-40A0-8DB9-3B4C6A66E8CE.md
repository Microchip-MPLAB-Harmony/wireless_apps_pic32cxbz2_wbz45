# APP_GetBleState

## C

```c
APP_BLE_LinkState_T APP_GetBleState(void);
```

## Description

Returns BLE link state

## Return values

|Return value|Description|
|:---|:---|
APP_BLE_STATE_STANDBY|Standby state. i.e. not advertising|
APP_BLE_STATE_ADVERTISING|BLE is advertising|
APP_BLE_STATE_SCANNING|BLE is scanning (only for BLE_GAP_ROLE_CENTRAL)|
APP_BLE_STATE_CONNECTED|BLE is connected|
