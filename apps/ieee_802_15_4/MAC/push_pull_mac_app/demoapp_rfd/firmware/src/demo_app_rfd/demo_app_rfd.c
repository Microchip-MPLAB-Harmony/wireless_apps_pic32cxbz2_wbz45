// DOM-IGNORE-BEGIN
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
// DOM-IGNORE-END

/*******************************************************************************
   MAC Example No beacon Application - Device

  Company:
    Microchip Technology Inc.

  File Name:
    nobeacon_rfd.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    The basic MAC Example Nobeacon Application deploys a nonbeacon-enabled
    network.
    The coordinator starts a PAN at channel DEFAULT_CHANNEL with the PAN ID
    DEFAULT_PAN_ID.
    The device scans for this network and associates to the coordinator.
    Once the device is associated, it uses a timer that fires every 5 seconds to
    poll for pending data at the coordinator by means of transmitting a data
    request frame to the coordinator.
    On the other hand every 5 seconds the coordinator queues a dummy data frame
    for each associated device into its indirect frame queue.
    If the coordinator receives a data request frame from a particular device, it
    transmits the pending data frame to the device.
    Device after receiving the data from the Coordinator sends back the same data
    to the Coordinator itself by direct data transmission
    The results of the proper data transmission/reception are printed to a
    terminal program via Serial I/O (UART ).
 *******************************************************************************/

/* === INCLUDES ============================================================ */
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "mac_api.h"
#include "ieee_mac_const.h"
#include "mac_msg_const.h"
#include "demo_app_rfd.h"
#include "pal.h"
#include "app.h"
#include "timers.h"
#include "mac_internal.h"



/* === MACROS ============================================================== */
/** Channel Offset will give us the channel number as (CHANNEL_OFFSET + 11) */
#define CHANNEL_OFFSET                  (4U)

/** Defines the short scan duration time. */
#define SCAN_DURATION_SHORT             (5)
/** Defines the long scan duration time. */
#define SCAN_DURATION_LONG              (6)

/** This is the time period in micro seconds for polling transmissions. */
#define APP_POLL_PERIOD_MS              (5000)
#define SLEEP_DURATION_MS               (5000)

/** Define the LED on duration time. */
#define LED_ON_DURATION                 (500)

#define APP_GUARD_TIME_US               (10000)
#define MCU_WAKEUP_TIME                 (150)
#define PAYLOAD_LEN                     (104U)

#define ZERO (0)

/* === GLOBALS ============================================================= */
/* This structure stores the short and extended address of the coordinator. */
WPAN_AddrSpec_t coordAddrSpec;
/** This variable stores the current state of the node. */
DeviceState_t device_state;
/** This array stores the current msdu payload. */
static uint8_t msdu_payload[PAYLOAD_LEN];

uint8_t current_channel;
uint8_t current_channel_page;
static uint32_t channels_supported;

/** This structure stores the short address of the coordinator. */
static uint16_t coordAddr;

#ifdef MAC_SECURITY_ZIP
uint32_t __attribute__ ((persistent)) deviceShortAddress;
#endif
/* The timer created for toggling the LED stills it finds a network*/
static TimerHandle_t LED_Toggle_Timer_Handle = NULL;
static TimerHandle_t Timer_Handle = NULL;

static bool deviceCanSleep = false;
/** Store the current MSDU handle to be used for a data frame. */
static uint8_t curr_msdu_handle;
/* This array stores the payload sent  from RFD to Co-ordinator*/
static const char *payload_push_data = "Data";

/* === PROTOTYPES ========================================================== */

/**
 * @brief Callback function for the application timer
 *        initiating polling of coordinator
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void app_initiate_polling(void *parameter);

static void push_data(void);

/* === Static IMPLEMENTATION ================================================ */
/*
 * @brief Callback function for the application timer
 *        initiating polling of coordinator
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void app_initiate_polling(void *parameter)
{
	/*
	 * Initiate MLME-POLL.request service and have it placed in the MLME-SAP
	 * queue.
	 *
	 * @param CoordAddrSpec   Pointer to WPAN_AddrSpec_t structure for the
	 * coordinator.
	 *
	 * @return true - success; false - buffer not availability or queue
	 * full.
	 */
	coordAddrSpec.AddrMode = WPAN_ADDRMODE_SHORT;
	coordAddrSpec.PANId = DEFAULT_PAN_ID;
	coordAddrSpec.Addr.shortAddress = coordAddr;

	SYS_CONSOLE_MESSAGE("\nPoll coordinator\r\n");

	(void)WPAN_MLME_PollReq(&coordAddrSpec);
    
	/*Keep compiler happy*/
	parameter = parameter;
}

/*
 * @brief Callback function indicating network search
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void LED_Toggle_Timer_Callback(TimerHandle_t xTimer)
{
	RGB_LED_BLUE_Toggle();

	/* Re-start led timer again. */
	(void)xTimerStart(LED_Toggle_Timer_Handle,pdMS_TO_TICKS(0));
}
static void Timer_Callback(TimerHandle_t xTimer)
{
    /* goes to sleep */  
        deviceCanSleep = true;
        #ifdef ENABLE_DEVICE_DEEP_SLEEP    
            APP_Msg_T sleepReq;
            sleepReq.msgId = APP_MSG_SLEEP_REQ;      
    
            (void)OSAL_QUEUE_Send(&appData.appQueue, &sleepReq, 0);
        #endif	
}


static void push_data(void)
{
    
    SYS_CONSOLE_MESSAGE("Data for FFD\n");    
    uint8_t src_addr_mode;
    WPAN_AddrSpec_t dst_addr;    
    uint8_t payload_len = (uint8_t)strlen(payload_push_data);
    uint8_t payload_arr[15];
    
    (void)memcpy((void *)&payload_arr,(const void *)payload_push_data,payload_len);
    
    src_addr_mode = WPAN_ADDRMODE_SHORT;
    dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
    dst_addr.PANId = DEFAULT_PAN_ID;
    ADDR_COPY_DST_SRC_16(dst_addr.Addr.shortAddress, coordAddr);
    curr_msdu_handle++; /* Increment handle */
    if (!WPAN_MCPS_Datareq(src_addr_mode,&dst_addr,
        payload_len,  /* One octet */
        payload_arr,
        curr_msdu_handle,
        WPAN_TXOPT_OFF
        #if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
          , ZIP_SEC_MIN, NULL, ZIP_KEY_ID_MODE,
            (uint8_t)deviceShortAddress
        #endif
            )
            ) 
    {
        /* goes to sleep */  
        deviceCanSleep = true;
        #ifdef ENABLE_DEVICE_DEEP_SLEEP    
            APP_Msg_T sleepReq;
            sleepReq.msgId = APP_MSG_SLEEP_REQ;      

            (void)OSAL_QUEUE_Send(&appData.appQueue, &sleepReq, 0);
        #endif   
    }
    else
    {
        USER_LED_Off();
    }
}

/***********************IMPLEMENTATION*****************************************/

void MAC_RFDDemoInit(void)
{ 
    /* The timer created for toggling the LED stills it finds a network*/
    LED_Toggle_Timer_Handle = xTimerCreate("Milli_Timer",pdMS_TO_TICKS(LED_ON_DURATION),pdFALSE, ( void * ) 0, LED_Toggle_Timer_Callback);
    Timer_Handle = xTimerCreate("Milli_Timer",pdMS_TO_TICKS(5000),pdFALSE, ( void * ) 0, Timer_Callback);
#ifdef ENABLE_DEVICE_DEEP_SLEEP  
    DEVICE_DeepSleepWakeSrc_T wakeupSrc;
    DEVICE_GetDeepSleepWakeUpSrc(&wakeupSrc);
   
    if(wakeupSrc == DEVICE_DEEP_SLEEP_WAKE_NONE  ||  wakeupSrc == DEVICE_DEEP_SLEEP_WAKE_MCLR)
#endif
    {
        /* Initialize LEDs. */
        RGB_LED_RED_On(); /* indicating application is started */
        RGB_LED_BLUE_Off(); /* indicating network is started */
        RGB_LED_GREEN_Off();    /* indicating data transmission */
        USER_LED_On();

        SYS_CONSOLE_PRINT("\nMAC RFD Demo Application\r\n\n");

        print_stack_app_build_features();

        (void)WPAN_MLME_ResetReq(true);
    }
#ifdef ENABLE_DEVICE_DEEP_SLEEP 
    else
    {
        USER_LED_On();
        MAC_Wakeup();
        app_initiate_polling(NULL);
    }
#endif
}

static bool APP_ReadyToSleep(uint32_t *sleepDuration)
{
    bool sleep = false;
    
    if(deviceCanSleep)
    {
        if((bool)(MAC_ReadyToSleep()))
        {           
            *sleepDuration = SLEEP_DURATION_MS;
            sleep = true;                         
        }
#ifdef ENABLE_DEVICE_DEEP_SLEEP
        else
        {
            APP_Msg_T sleepReq;
            sleepReq.msgId = APP_MSG_SLEEP_REQ;      
    
            (void)OSAL_QUEUE_Send(&appData.appQueue, &sleepReq, 0); 
        }
#endif

    }    
    return sleep;
}

/*
 * @brief This function will show up the application
 *        and stack build features and available in the firmware
 * @param void
 *
 */
void print_stack_app_build_features(void)
{
#ifdef STB_ON_SAL
	SYS_CONSOLE_MESSAGE("\r\n Security Tool Box On SAL : Enabled");
#else
	SYS_CONSOLE_MESSAGE("\r\n Security Tool Box On SAL : Disabled");
#endif

#ifdef MAC_SECURITY_ZIP
	SYS_CONSOLE_MESSAGE("\r\n MAC Data & Security Module : Enabled");
#else
	SYS_CONSOLE_MESSAGE("\r\n MAC Data & Security Module : Disabled");
#endif

#ifdef HIGH_DATA_RATE_SUPPORT
	SYS_CONSOLE_MESSAGE("\r\n High Data Rate Support : Enabled");
#else
	SYS_CONSOLE_MESSAGE("\r\n High Data Rate Support : Disabled");
#endif
    
}

/* === Handle MAC Confirmation/Indication Messages ===================================== */

void Handle_ResetConf(APP_Msg_T *appMsg)
{
    usr_mlme_reset_conf_t resetConf;
    (void)memcpy((void *)&resetConf,(const void *)appMsg->msgData,sizeof(usr_mlme_reset_conf_t));
        
	if (resetConf.status == (uint8_t)MAC_SUCCESS) {
#ifdef MAC_SECURITY_ZIP
		/*	WPAN_MLME_GetReq(phyCurrentPage,NO_PIB_INDEX);*/
		(void)WPAN_MLME_SetReq(macDefaultKeySource,
				NO_PIB_INDEX,
				&default_key_source);
#else
		WPAN_MLME_GetReq(phyCurrentPage);
#endif
	} else {
		/* Set proper state of application. */
		device_state = DEVICE_IDLE;

		/* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
	}
}

void Handle_GetConf(APP_Msg_T *appMsg)
{
    usr_mlme_get_conf_t getConf;
    (void)memcpy((void *)&getConf,(const void *)appMsg->msgData,sizeof(usr_mlme_get_conf_t));
        
#ifdef MAC_SECURITY_ZIP
	MAC_KeyTable_t *keyTable = (MAC_KeyTable_t *)getConf.PIBAttributeValue;
#endif
    
    if(getConf.status == (uint8_t)MAC_SUCCESS)
    {
        switch(getConf.PIBAttribute)
        {
            case phyCurrentPage:
            {
                
#ifdef HIGH_DATA_RATE_SUPPORT
                current_channel_page = 0;
#else
                current_channel_page = *(uint8_t *)getConf.PIBAttributeValue;
#endif
                
#ifdef MAC_SECURITY_ZIP
                (void)WPAN_MLME_GetReq(phyChannelsSupported, NO_PIB_INDEX);
#else
                (void)WPAN_MLME_GetReq(phyChannelsSupported);
#endif
            }
            break;
            
            case phyChannelsSupported:
            {
                uint32_t index;

                channels_supported = convert_byte_array_to_32_bit(
                                        getConf.PIBAttributeValue);

                for (index = 0; index < 32U; index++) {
                    if ( (bool)(channels_supported & (uint32_t)(1UL << index)) ) {
                    current_channel = (uint8_t)(index + CHANNEL_OFFSET);
                    break;
                    }
                }

                SYS_CONSOLE_PRINT("\r\nSearching network in Channel - %d\r\n",
				current_channel);


                /*
                 * Initiate an active scan over all channels to determine
                 * which channel is used by the coordinator.
                 * Use: bool wpan_mlme_scan_req(uint8_t ScanType,
                 *                              uint32_t ScanChannels,
                 *                              uint8_t ScanDuration,
                 *                              uint8_t ChannelPage);
                 *
                 * This request leads to a scan confirm message ->
                 * usr_mlme_scan_conf
                 * Scan for about 50 ms on each channel -> ScanDuration = 1
                 * Scan for about 1/2 second on each channel -> ScanDuration = 5
                 * Scan for about 1 second on each channel -> ScanDuration = 6
                 */
                (void)WPAN_MLME_ScanReq(MLME_SCAN_TYPE_ACTIVE,
                        SCAN_CHANNEL(current_channel),
                        SCAN_DURATION_LONG,
                        current_channel_page);

                /* Indicate network scanning by a LED flashing. */
                (void)xTimerStart(LED_Toggle_Timer_Handle,pdMS_TO_TICKS(0));
            }
            break;
#ifdef MAC_SECURITY_ZIP            
            case macKeyTable:
            {
                for (uint8_t i = 0; i < keyTable->KeyDeviceListEntries; i++) 
                {
                    if (EMPTY_DEV_HANDLE ==
                            (keyTable->KeyDeviceList[i].
                            DeviceDescriptorHandle)) 
                    {
                        keyTable->KeyDeviceList[i].
                        DeviceDescriptorHandle = 0x00;
                        keyTable->KeyDeviceList[i].UniqueDevice = true;
                        break;
                    }
                }
                
                (void)WPAN_MLME_SetReq(macKeyTable,
                                ((uint8_t)deviceShortAddress - 1U),
                                (uint8_t *)getConf.PIBAttributeValue);
            }
            break;
#endif
            default:
            {
                /* Do nothing */
                break;
            }            
        }
    }
    else
    {
        /* Set proper state of application. */
		device_state = DEVICE_IDLE;

		/* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
    }
}

#ifndef MAC_SECURITY_ZIP
void Handle_SetConf(APP_Msg_T *appMsg)
{
    usr_mlme_set_conf_t setConf;
    (void)memcpy((void *)&setConf,(const void *)appMsg->msgData,sizeof(usr_mlme_set_conf_t));
    
    if (setConf.status == MAC_SUCCESS) 
    {
        switch (setConf.PIBAttribute)
        {
            case macPANId:
            {
                /*
                * Set the Coordinator Short Address of the scanned network.
                * This is required in order to perform a proper sync
                * before assocation.
                */
                (void)WPAN_MLME_SetReq(macCoordShortAddress, &coordAddrSpec.Addr);
            }
            break;
            
            case macCoordShortAddress:
            {
                
            }
            break;
            
            default:
            {
                 /* Do nothing */
                 break;
            }
           
        }
        
	} 
    else 
    {
		/* Set proper state of application. */
		device_state = DEVICE_IDLE;

		/* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
	}
}

#endif

void Handle_ScanConf(APP_Msg_T *appMsg)
{
    usr_mlme_scan_conf_t scanConf;
    (void)memcpy((void *)&scanConf,(const void *)appMsg->msgData,sizeof(usr_mlme_scan_conf_t));
   
    if (scanConf.status == (uint8_t)MAC_SUCCESS) 
    {
		WPAN_Pandescriptor_t *coordinator;
		uint8_t i;

		/*
		 * Analyze the ResultList.
		 * Assume that the first entry of the result list is our
		 * coodinator.
		 */
		coordinator = (WPAN_Pandescriptor_t *)scanConf.ResultList;

		for (i = 0; i < scanConf.ResultListSize; i++) {
			/*
			 * Check if the PAN descriptor belongs to our
			 * coordinator.
			 * Check if coordinator allows association.
			 */
			if ((coordinator->LogicalChannel == current_channel) &&
					(coordinator->ChannelPage ==
					current_channel_page) &&
					(coordinator->CoordAddrSpec.PANId ==
					DEFAULT_PAN_ID) &&
					((coordinator->SuperframeSpec &
					((uint16_t)1 <<
					ASSOC_PERMIT_BIT_POS)) ==
					((uint16_t)1 << ASSOC_PERMIT_BIT_POS))
					) {
				/* Store the coordinator's address information.
				**/
				coordAddrSpec.AddrMode = WPAN_ADDRMODE_SHORT;
				coordAddrSpec.PANId = DEFAULT_PAN_ID;
				ADDR_COPY_DST_SRC_16(
						coordAddrSpec.Addr.shortAddress,
						coordinator->CoordAddrSpec.Addr.shortAddress);

				SYS_CONSOLE_PRINT("Found network\r\n");

				/*
				 * Set the PAN-Id of the scanned network.
				 * This is required in order to perform a proper
				 * sync
				 * before assocation.
				 * Use: bool wpan_mlme_set_req(uint8_t
				 * PIBAttribute,
				 *                             void
				 ******PIBAttributeValue);
				 *
				 * This request leads to a set confirm message
				 *-> usr_mlme_set_conf
				 */
				uint16_t pan_id;
				pan_id = DEFAULT_PAN_ID;
				(void)WPAN_MLME_SetReq(macPANId,
#ifdef MAC_SECURITY_ZIP
						NO_PIB_INDEX,
#endif
						&pan_id);

				/* Set proper state of application. */
				device_state = DEVICE_ASSOC_IN_PROGRESS;

				/*
				 * Associate to our coordinator.
				 * Use: bool wpan_mlme_associate_req(uint8_t
				 * LogicalChannel,
				 *                                   uint8_t
				 * ChannelPage,
				 *
				 *
				 *
				 *
				 *
				 *                              WPAN_AddrSpec_t
				 *****CoordAddrSpec,
				 *                                   uint8_t
				 * CapabilityInformation);
				 * This request will cause a mlme associate
				 * confirm message ->
				 * usr_mlme_associate_conf.
				 */
				(void)WPAN_MLME_AssociateReq(
						coordinator->LogicalChannel,
						coordinator->ChannelPage,
						&(coordinator->CoordAddrSpec),
						WPAN_CAP_ALLOCADDRESS);
				return;
			}

			/* Get the next PAN descriptor. */
			coordinator++;
		}

		/*
		 * If here, the result list does not contain our expected
		 * coordinator.
		 * Let's scan again.
		 */
		(void)WPAN_MLME_ScanReq(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL(current_channel),
				SCAN_DURATION_SHORT,
				current_channel_page);
	} 
    else if (scanConf.status == (uint8_t)MAC_NO_BEACON) 
    {
		/*
		 * No beacon is received; no coordiantor is located.
		 * Scan again, but used longer scan duration.
		 */
		(void)WPAN_MLME_ScanReq(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL(current_channel),
				SCAN_DURATION_LONG,
				current_channel_page);
	} 
    else 
    {
		/* Set proper state of application. */
		device_state = DEVICE_IDLE;

		/* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
	}

	/* Keep compiler happy. */
	scanConf.ScanType = scanConf.ScanType;
	scanConf.ChannelPage = scanConf.ChannelPage;
	scanConf.UnscannedChannels = scanConf.UnscannedChannels;
 
}

void Handle_AssociateConf(APP_Msg_T *appMsg)
{
    usr_mlme_associate_conf_t associateConf;
    (void)memcpy((void *)&associateConf,(const void *)appMsg->msgData,sizeof(usr_mlme_associate_conf_t));
    
    if (associateConf.status == (uint8_t)MAC_SUCCESS)
    {

		SYS_CONSOLE_MESSAGE("Connected to no beacon network\r\n");

		/* Set proper state of application. */
		device_state = DEVICE_RUNNING;

		/* Stop timer used for search indication */
		(void)xTimerStop(LED_Toggle_Timer_Handle,pdMS_TO_TICKS(0));

        RGB_LED_RED_Off();
		RGB_LED_BLUE_On();
        
        (void)xTimerStart(Timer_Handle,pdMS_TO_TICKS(0));
        
        #ifdef MAC_SECURITY_ZIP
            uint16_t mac_dev_table_entries = 3;

            (void)WPAN_MLME_SetReq(macDeviceTableEntries,
                    NO_PIB_INDEX,
                    &mac_dev_table_entries);
        #endif
        SYS_CONSOLE_PRINT("Address Assigned - 0x%4x\r\n", associateConf.AssocShortAddress );
	} 
    else 
    {
		RGB_LED_BLUE_Off();
        SYS_CONSOLE_MESSAGE("\nAssociation unsuccessful\n");

//		/* Set proper state of application. */
//		device_state = DEVICE_IDLE;

//		/* Something went wrong; restart. */
//		(void)WPAN_MLME_ResetReq(true);
	}
    
	/* Keep compiler happy. */
	associateConf.AssocShortAddress = associateConf.AssocShortAddress;
#ifdef MAC_SECURITY_ZIP
	deviceShortAddress = (uint32_t)associateConf.AssocShortAddress;
#endif       
}

void Handle_PollConf(APP_Msg_T *appMsg)
{
    usr_mlme_poll_conf_t pollConf;
    (void)memcpy((void *)&pollConf,(const void *)appMsg->msgData,sizeof(usr_mlme_poll_conf_t));
  
    if(pollConf.status != (uint8_t)MAC_SUCCESS)
    {
        if(pollConf.status == (uint8_t)MAC_NO_DATA)
        {
            push_data();
        }
        else
        {
            deviceCanSleep = true;
        #ifdef ENABLE_DEVICE_DEEP_SLEEP    
            APP_Msg_T sleepReq;
            sleepReq.msgId = APP_MSG_SLEEP_REQ;      

            (void)OSAL_QUEUE_Send(&appData.appQueue, &sleepReq, 0);
        #endif        
        }
    }
    pollConf.status = pollConf.status;    /* Keep compiler happy. */   
}

void Handle_BeaconNotifyInd(APP_Msg_T *appMsg)
{
    usr_mlme_beacon_notify_ind_t beaconNotifyInd;
    (void)memcpy((void *)&beaconNotifyInd,(const void *)appMsg->msgData, sizeof(usr_mlme_beacon_notify_ind_t));
    
    if (DEVICE_RUNNING == device_state) {

		/* This is the standard portion once the node is associated
		 * with the application.
		 */

		/*
		 * Extract the beacon payload from our coordinator and feed it
		 * back
		 * to the coordinator via a data frame.
		 */

		/* Use: bool WPAN_MCPS_Datareq(uint8_t SrcAddrMode,
		 *                              WPAN_AddrSpec_t *DstAddrSpec,
		 *                              uint8_t msduLength,
		 *                              uint8_t *msdu,
		 *                              uint8_t msduHandle,
		 *                              uint8_t TxOptions);
		 *
		 * This request will cause a mcps data confirm message ->
		 * usr_mcps_data_conf
		 */
		if (beaconNotifyInd.sduLength > PAYLOAD_LEN) {
			beaconNotifyInd.sduLength = PAYLOAD_LEN;
		}

		/* Copy payload from beacon frame. */
		(void)memcpy(&msdu_payload, beaconNotifyInd.sdu, beaconNotifyInd.sduLength);

		uint8_t src_addr_mode = WPAN_ADDRMODE_SHORT;
		static uint8_t msdu_handle = 0;

		msdu_handle++;      /* Increment handle */

#ifdef MAC_SECURITY_ZIP
		(void)WPAN_MCPS_Datareq(src_addr_mode,
				&coordAddrSpec,
				beaconNotifyInd.sduLength,  /* msduLength */
				&msdu_payload[0],
				msdu_handle,
				WPAN_TXOPT_ACK,
				ZIP_SEC_MIN,     /* SecurityLevel */
				NULL,
				ZIP_KEY_ID_MODE, /* KeyIdMode */
				(uint8_t)deviceShortAddress);  /* KeyIndex */
#else
		WPAN_MCPS_Datareq(src_addr_mode,
				&coordAddrSpec,
				beaconNotifyInd.sduLength,
				&msdu_payload[0],
				msdu_handle,
				WPAN_TXOPT_ACK);
#endif
		{
			static uint32_t rx_cnt;
			const char Rx_Beacon_Payload[]
				= "Rx beacon payload (%lu): ";

			/* Print received payload. */
			rx_cnt++;
			SYS_CONSOLE_PRINT(Rx_Beacon_Payload, rx_cnt);

			/* Set last element to 0. */
			if (beaconNotifyInd.sduLength == PAYLOAD_LEN) {
				msdu_payload[PAYLOAD_LEN - 1U] = (uint8_t)'\0';
			} else {
				msdu_payload[beaconNotifyInd.sduLength] = (uint8_t)'\0';
			}

			for (uint8_t i = 0; i < beaconNotifyInd.sduLength; i++) {
				SYS_CONSOLE_PRINT("%c", msdu_payload[i]);
			}
			SYS_CONSOLE_MESSAGE("\n");
		}
	}
}

void Handle_DataInd(APP_Msg_T *appMsg)
{
    usr_mcps_data_ind_t mscpDataInd;
    (void)memcpy((void *)&mscpDataInd,(const void *)appMsg->msgData,sizeof(usr_mcps_data_ind_t));

	mscpDataInd.msdu[mscpDataInd.msduLength]=(uint8_t)'\0';   
    
    SYS_CONSOLE_PRINT((const char *)mscpDataInd.msdu);	
    SYS_CONSOLE_MESSAGE("\n\n");
        
    RGB_LED_GREEN_On();
    
    push_data();  
    
    /* Keep compiler happy. */
	(void)mscpDataInd.SrcAddrSpec;
	(void)mscpDataInd.DstAddrSpec ;
	(void)mscpDataInd.msduLength ;
	(void)mscpDataInd.msdu;
	(void)mscpDataInd.mpduLinkQuality ;
	(void)mscpDataInd.DSN ;
#ifdef ENABLE_TSTAMP
	(void)mscpDataInd.Timestamp ;
#endif  /* ENABLE_TSTAMP */     
}

#ifdef ENABLE_DEVICE_DEEP_SLEEP
void Handle_DeviceSleep(APP_Msg_T *appMsg)
{
    uint32_t sleepDuration;
    if (APP_ReadyToSleep(&sleepDuration))
    {
        /* Enter system sleep mode */
        DEVICE_EnterDeepSleep(true, sleepDuration);
    }
    
    (void)appMsg;
}
#endif

void Handle_DataConf(APP_Msg_T *appMsg)
{
    deviceCanSleep = true;
    #ifdef ENABLE_DEVICE_DEEP_SLEEP    
        APP_Msg_T sleepReq;
        sleepReq.msgId = APP_MSG_SLEEP_REQ;      

        (void)OSAL_QUEUE_Send(&appData.appQueue, &sleepReq, ZERO);
    #endif
}

void Handle_MACMessages(APP_Msg_T *appMsg)
{
    switch(appMsg->msgId)
    {
        case APP_MSG_MAC_RESET_CONF:
        {
            Handle_ResetConf(appMsg);
        }
        break;

        case APP_MSG_MAC_GET_CONF:
        {
            Handle_GetConf(appMsg);
        }
        break;

        case APP_MSG_MAC_SET_CONF:
        {
            Handle_SetConf(appMsg);
        }
        break;

        case APP_MSG_MAC_SCAN_CONF:
        {
            Handle_ScanConf(appMsg);
        }
        break;

        case APP_MSG_MAC_ASSOCIATE_CONF:
        {
            Handle_AssociateConf(appMsg);
        }
        break;

        case APP_MSG_MAC_POLL_CONF:
        {
            Handle_PollConf(appMsg);
        }
        break;

        case APP_MSG_MAC_BEACON_NOTIFY_IND:
        {
            Handle_BeaconNotifyInd(appMsg);
        }
        break;

        case APP_MSG_MAC_DATA_IND:
        {
           Handle_DataInd(appMsg); 
        }
        break;
        case APP_MSG_MAC_DATA_CONF:
        {
           Handle_DataConf(appMsg);
        }
        break;
#ifdef ENABLE_DEVICE_DEEP_SLEEP       
        case APP_MSG_SLEEP_REQ:
        {
            Handle_DeviceSleep(appMsg);
        }
        break;
#endif

        default:
        {
            /* Do nothing */
            break;
        }

    }
}



