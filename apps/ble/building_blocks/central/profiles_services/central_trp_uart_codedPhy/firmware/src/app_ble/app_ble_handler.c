/*******************************************************************************
* Copyright (C) 2022 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

/*******************************************************************************
  Application BLE Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_ble_handler.c

  Summary:
    This file contains the Application BLE functions for this project.

  Description:
    This file contains the Application BLE functions for this project.
 *******************************************************************************/


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <string.h>
#include <stdint.h>
#include "osal/osal_freertos_extend.h"
#include "app_ble_handler.h"
#include "peripheral/sercom/usart/plib_sercom0_usart.h"
#include "system/console/sys_console.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************
extern uint16_t conn_hdl;


// *****************************************************************************
// *****************************************************************************
// Section: Functions
// *****************************************************************************
// *****************************************************************************

#define APP_BLE_CODED_PHY_ENABLE
//#define APP_BLE_CODED_S8_ENABLE
#ifndef APP_BLE_CODED_S8_ENABLE
  #define APP_BLE_CODED_S2_ENABLE
#endif

static  int8_t phyOptions = BLE_GAP_PHY_PREF_NO;

void SYS_CONSOLE_PRINT_PHY(uint8_t phy) {
    switch (phy) {
        case BLE_GAP_PHY_TYPE_LE_1M:
            SYS_CONSOLE_PRINT("LE_1M \r\n");
            break;
        case BLE_GAP_PHY_TYPE_LE_2M:
            SYS_CONSOLE_PRINT("LE_2M \r\n");
            break;
        case BLE_GAP_PHY_TYPE_LE_CODED:
            SYS_CONSOLE_PRINT("LE_CODED");
            if(phyOptions == BLE_GAP_PHY_PREF_S2)
                SYS_CONSOLE_PRINT(" |S2\r\n");
            else if(phyOptions == BLE_GAP_PHY_PREF_S8)
                SYS_CONSOLE_PRINT(" |S8\r\n");
            else
                SYS_CONSOLE_PRINT("\r\n");
            break;
        default:
            SYS_CONSOLE_PRINT("UNKOWN PHY \r\n");
            break;
    }
};

void SYS_CONSOLE_PRINT_extEVENT(uint8_t extEvent) {
    if (extEvent & BLE_GAP_EXT_ADV_RPT_TYPE_CONNECTABLE)
        SYS_CONSOLE_PRINT("| CONNECTABLE");
    if (extEvent & BLE_GAP_EXT_ADV_RPT_TYPE_SCANNABLE)
        SYS_CONSOLE_PRINT("| SCANNABLE");
    if (extEvent & BLE_GAP_EXT_ADV_RPT_TYPE_DIRECTED)
        SYS_CONSOLE_PRINT("| DIRECTED");
    if (extEvent & BLE_GAP_EXT_ADV_RPT_TYPE_SCAN_RSP)
        SYS_CONSOLE_PRINT("| SCAN_RSP");
    if (extEvent & BLE_GAP_EXT_ADV_RPT_TYPE_LEGACY)
        SYS_CONSOLE_PRINT("| LEGACY");

    SYS_CONSOLE_PRINT("\r\n");
};

void APP_BleGapEvtHandler(BLE_GAP_Event_T *p_event) {
    switch (p_event->eventId) {
        case BLE_GAP_EVT_CONNECTED:
        {
            /* TODO: implement your application code.*/
            uint8_t tx_phy, rx_phy;

            SYS_CONSOLE_PRINT("\r\nConnected:\r\n");
            conn_hdl = p_event->eventField.evtConnect.connHandle;

            BLE_GAP_ReadPhy(conn_hdl, &tx_phy, &rx_phy);
            SYS_CONSOLE_PRINT("tx_phy: ");
            SYS_CONSOLE_PRINT_PHY(tx_phy);

            SYS_CONSOLE_PRINT("rx_phy: ");
            SYS_CONSOLE_PRINT_PHY(rx_phy);
            
#ifdef APP_BLE_CODED_PHY_ENABLE
            //if Le1M_CODED, set s8 or s2
            if (tx_phy == BLE_GAP_PHY_TYPE_LE_CODED) {
#ifdef APP_BLE_CODED_S2_ENABLE
                phyOptions = BLE_GAP_PHY_PREF_S2;
#else
                phyOptions = BLE_GAP_PHY_PREF_S8;
#endif
                BLE_GAP_SetPhy(conn_hdl, BLE_GAP_PHY_OPTION_CODED, BLE_GAP_PHY_OPTION_CODED, phyOptions);
            }
#endif
            
        }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("\r\n\r\nDisconnected!\r\n");
        }
        break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ENCRYPT_STATUS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("EVT_ADV:%d", p_event->eventField.evtAdvReport.eventType);

            // Filter Devices based of Address, for this example address checking only 2 bytes
            if ((p_event->eventField.evtAdvReport.addr.addr[0] == 0xA1 && p_event->eventField.evtAdvReport.addr.addr[1] == 0xA2) ||
                    (p_event->eventField.evtAdvReport.addr.addr[0] == 0xB1 && p_event->eventField.evtAdvReport.addr.addr[1] == 0xB2) ||
                    (p_event->eventField.evtAdvReport.addr.addr[0] == 0xC1 && p_event->eventField.evtAdvReport.addr.addr[1] == 0xC2)) {
                SYS_CONSOLE_PRINT("Found Peer Node\r\n");

                // found so disable legacy scanning
                BLE_GAP_SetScanningEnable(false, BLE_GAP_SCAN_FD_ENABLE, BLE_GAP_SCAN_MODE_OBSERVER, 1000);

                BLE_GAP_CreateConnParams_T createConnParam_t;
                createConnParam_t.scanInterval = 0x3C; // 37.5 ms 
                createConnParam_t.scanWindow = 0x1E; // 18.75 ms
                createConnParam_t.filterPolicy = BLE_GAP_SCAN_FP_ACCEPT_ALL;
                createConnParam_t.peerAddr.addrType = p_event->eventField.evtAdvReport.addr.addrType;
                memcpy(createConnParam_t.peerAddr.addr, p_event->eventField.evtAdvReport.addr.addr, GAP_MAX_BD_ADDRESS_LEN);
                createConnParam_t.connParams.intervalMin = 0x10; 
                createConnParam_t.connParams.intervalMax = 0x10; 
                createConnParam_t.connParams.latency = 0;
                createConnParam_t.connParams.supervisionTimeout = 0x48;
                SYS_CONSOLE_PRINT("Initiating Connection\r\n");
                BLE_GAP_CreateConnection(&createConnParam_t);
            }
        }
        break;

        case BLE_GAP_EVT_ENC_INFO_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_REMOTE_CONN_PARAM_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_EXT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("*");
            if ((p_event->eventField.evtExtAdvReport.addr.addr[0] == 0xA1 && p_event->eventField.evtExtAdvReport.addr.addr[1] == 0xA2) ||
                    (p_event->eventField.evtExtAdvReport.addr.addr[0] == 0xB1 && p_event->eventField.evtExtAdvReport.addr.addr[1] == 0xB2) ||
                    (p_event->eventField.evtExtAdvReport.addr.addr[0] == 0xC1 && p_event->eventField.evtExtAdvReport.addr.addr[1] == 0xC2)) {
                SYS_CONSOLE_PRINT("\r\next Found Peer Node\r\n");

                SYS_CONSOLE_PRINT("\r\nEXT_ADV primPhy:");
                SYS_CONSOLE_PRINT_PHY(p_event->eventField.evtExtAdvReport.priPhy);
                SYS_CONSOLE_PRINT("EXT_ADV secPhy: ");
                SYS_CONSOLE_PRINT_PHY(p_event->eventField.evtExtAdvReport.secPhy);

                //SYS_CONSOLE_PRINT_extEVENT(p_event->eventField.evtExtAdvReport.eventType);

                // found so disable ext scanning
                BLE_GAP_ExtScanningEnable_T extScan;
                extScan.duration = 0x2710; //100 Seconds scan
                extScan.enable = true; 
                extScan.filterDuplicates = BLE_GAP_SCAN_FD_DISABLE;
                extScan.period = 0x0000;
                BLE_GAP_SetExtScanningEnable(BLE_GAP_SCAN_MODE_OBSERVER, &extScan);

                SYS_CONSOLE_PRINT("\r\next Initiating Connection...\r\n");

                BLE_GAP_ExtCreateConnPhy_T extCreateConnParam;
                extCreateConnParam.le1mPhy.enable = true;
                extCreateConnParam.le1mPhy.scanInterval = 0x3C;
                extCreateConnParam.le1mPhy.scanWindow = 0x1E;
                extCreateConnParam.le1mPhy.connParams.intervalMin = 0x10;
                extCreateConnParam.le1mPhy.connParams.intervalMax = 0x10;
                extCreateConnParam.le1mPhy.connParams.latency = 0;
                extCreateConnParam.le1mPhy.connParams.supervisionTimeout = 0x48;

                extCreateConnParam.le2mPhy.enable = true;
                extCreateConnParam.le2mPhy.scanInterval = 0x3C;
                extCreateConnParam.le2mPhy.scanWindow = 0x1E;
                extCreateConnParam.le2mPhy.connParams.intervalMin = 0x10;
                extCreateConnParam.le2mPhy.connParams.intervalMax = 0x10;
                extCreateConnParam.le2mPhy.connParams.latency = 0;
                extCreateConnParam.le2mPhy.connParams.supervisionTimeout = 0x48;

                extCreateConnParam.leCodedPhy.enable = true;
                extCreateConnParam.leCodedPhy.scanInterval = 0x3C;
                extCreateConnParam.leCodedPhy.scanWindow = 0x1E;
                extCreateConnParam.leCodedPhy.connParams.intervalMin = 0x10;
                extCreateConnParam.leCodedPhy.connParams.intervalMax = 0x10;
                extCreateConnParam.leCodedPhy.connParams.latency = 0;
                extCreateConnParam.leCodedPhy.connParams.supervisionTimeout = 0x48;

                BLE_GAP_ExtCreateConnection(BLE_GAP_INIT_FP_FAL_NOT_USED,
                        &(p_event->eventField.evtExtAdvReport.addr),
                        &extCreateConnParam);
            }
        }
        break;

        case BLE_GAP_EVT_ADV_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_TX_BUF_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DEVICE_NAME_CHANGED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_AUTH_PAYLOAD_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PHY_UPDATE:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("\r\nBLE_GAP_EVT_PHY_UPDATE:\r\n");
            
            SYS_CONSOLE_PRINT("tx_phy: ");
            SYS_CONSOLE_PRINT_PHY(p_event->eventField.evtPhyUpdate.txPhy);

            SYS_CONSOLE_PRINT("rx_phy: ");
            SYS_CONSOLE_PRINT_PHY(p_event->eventField.evtPhyUpdate.rxPhy);
        }
        break;

        case BLE_GAP_EVT_SCAN_REQ_RECEIVED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_DIRECT_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_SYNC_EST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_REPORT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_PERI_ADV_SYNC_LOST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_SET_TERMINATED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_SCAN_TIMEOUT:
        {
            /* TODO: implement your application code.*/
            SYS_CONSOLE_PRINT("Scan Completed \r\n");
        }
        break;

        case BLE_GAP_EVT_TRANSMIT_POWER_REPORTING:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_GAP_EVT_ADV_COMPL:
        {
            /* TODO: implement your application code.*/

        }
        break;

        case BLE_GAP_EVT_PATH_LOSS_THRESHOLD:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}

void APP_BleL2capEvtHandler(BLE_L2CAP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_L2CAP_EVT_CONN_PARA_UPD_REQ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CONN_PARA_UPD_RSP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_CONN_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_CONN_FAIL_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_SDU_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_ADD_CREDITS_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_L2CAP_EVT_CB_DISC_IND:
        {
            /* TODO: implement your application code.*/
        }
        break;        

        default:
        break;
    }
}

void APP_GattEvtHandler(GATT_Event_T *p_event)
{   
    switch(p_event->eventId)
    {
        case GATTC_EVT_ERROR_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_PRIM_SERV_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_PRIM_SERV_BY_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_CHAR_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_DESC_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_READ_USING_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_READ_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_WRITE_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_HV_NOTIFY:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_HV_INDICATE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_READ:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_WRITE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_HV_CONFIRM:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case ATT_EVT_TIMEOUT:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case ATT_EVT_UPDATE_MTU:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTC_EVT_DISC_CHAR_BY_UUID_RESP:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_SERVICE_CHANGE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_CLIENT_FEATURE_CHANGE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_CLIENT_CCCDLIST_CHANGE:
        {
            /* TODO: implement your application code.*/
            OSAL_Free(p_event->eventField.onClientCccdListChange.p_cccdList);
        }
        break;

        case GATTC_EVT_PROTOCOL_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case GATTS_EVT_PROTOCOL_AVAILABLE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;        
    }
}

void APP_BleSmpEvtHandler(BLE_SMP_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_SMP_EVT_PAIRING_COMPLETE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_SECURITY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_NUMERIC_COMPARISON_CONFIRM_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_PASSKEY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_DISPLAY_PASSKEY_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_NOTIFY_KEYS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_PAIRING_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_OOB_DATA_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_INPUT_SC_OOB_DATA_REQUEST:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_KEYPRESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_SMP_EVT_GEN_SC_OOB_DATA_DONE:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;        
    }
}

void APP_DmEvtHandler(BLE_DM_Event_T *p_event)
{
    switch(p_event->eventId)
    {
        case BLE_DM_EVT_DISCONNECTED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_CONNECTED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_START:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_SUCCESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_SECURITY_FAIL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_PAIRED_DEVICE_FULL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_PAIRED_DEVICE_UPDATED:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_CONN_UPDATE_SUCCESS:
        {
            /* TODO: implement your application code.*/
        }
        break;

        case BLE_DM_EVT_CONN_UPDATE_FAIL:
        {
            /* TODO: implement your application code.*/
        }
        break;

        default:
        break;
    }
}
