/*******************************************************************************
   MAC Example Nobeacon Application - Coordinator

  Company:
    Microchip Technology Inc.

  File Name:
    nobeacon_ffd.c

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
    terminal program via Serial I/O (UART or USB).
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2023 Microchip Technology Inc. and its subsidiaries.
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

/* === INCLUDES ============================================================ */
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

#include "mac_api.h"
#include "ieee_mac_const.h"
#include "mac_msg_const.h"
#include "demo_app_ffd.h"
#include "pal.h"
#include "app.h"
#include "timers.h"

/* === MACROS ============================================================== */

#define CHANNEL_OFFSET                  (4U)

/** Defines Beacon Order for Nobeacon Network. */
#define DEFAULT_BO                     (15)
/** Defines Superframe Order for Nobeacon Network. */
#define DEFAULT_SO                     (15)


/** Coord Realignment is not used by default in this application. */
#define DEFAULT_COORDREALIGNMENT                (false)

/** Defines the default Coord Realign SecurityLevel */
#define DEFAULT_COORDREALIGN_SEC_LVL    (0x00)

/** Defines the default Coord Realign KeyIdMode */
#define DEFAULT_COORDREALIGN_KEYID_MODE (0x00)

/** Defines the default Coord Realign KeySource */
#define DEFAULT_COORDREALIGN_KEY_SRC    (NULL)

/** Defines the default Coord Realign KeyIndex */
#define DEFAULT_COORDREALIGN_KEY_INDEX  (0x00)

/** Defines the default Beacon Security Level */
#define DEFAULT_BEACON_SEC_LVL                  (0x05)

/** Defines the default Beacon KeyIdMode */
#define DEFAULT_BEACON_KEYID_MODE               (0x01)

/** Defines the default Beacon KeySource */
#define DEFAULT_BEACON_KEY_SRC                  (default_key_source)

/** Defines the default Beacon KeyIndex */
#define DEFAULT_BEACON_KEY_INDEX                (0x00)

#define DEBOUNCE_DELAY_MS               (200)

#define LED_BLINK_TIME_MS               (150)
/* === GLOBALS ============================================================= */
/** This array stores all device related information. */
AssociatedDevice_t device_list[MAX_NUMBER_OF_DEVICES];
/** Stores the number of associated devices. */
uint16_t no_of_assoc_devices;

/** This array stores the current beacon payload. */
uint8_t beacon_payload[] = {"Microchip beacon demo 0"};

/** This variable stores the current state of the node. */
coord_state_t coord_state = COORD_STARTING;

/** Store the current MSDU handle to be used for a data frame. */
static uint8_t curr_msdu_handle;

uint8_t current_channel;
uint8_t current_channel_page;
static uint32_t channels_supported;

#ifdef MAC_SECURITY_ZIP
/** Store the recently associated device number */
uint16_t recent_assoc_dev_no = 0xFFFF;
static bool pan_at_capacity = true;
#endif

/* The timer created for LED that blinks when it receives the data from the Co-ordinator */
static TimerHandle_t Data_Ind_LED_Timer_Handle = NULL;
/* The timer created for LED that blinks when it sends the data to the  Co-ordinator */
static TimerHandle_t Data_Conf_LED_Timer_Handle = NULL;
/*The structure variable for app_button_state*/
static app_button_state_t Button_state={UNICAST_DATA,0U};

/* This array displays the message to be displayed after the associated device is assigned a short address  */
static const char Display_Associated_Device[] = "Device %u associated\r\n";
/* This array displays the MSDU Handle message and its count*/
static const char Display_MSDU_Handle[] = "(MSDU handle: %u)";
/* This array displays the device from which it received Rx Frame*/
static const char Display_Rx_Frame_Address[] = "Rx frame Addr %x - ";
/* This array displays the result frame with handle  */
static const char Dispaly_Result_Frame[] = "\nResult frame with handle %u : ";
/* This array stores the unicast data that is sent from FFD to RFD */
static const char *payload_unicast = "-Received Unicast Data-";
/* This array stores the broadcast data that is sent from FFD to RFDs */
static const char *payload_broadcast = "-Received Broadcast Data-";
static uint8_t payload_arr[30];

/* === PROTOTYPES ========================================================== */

/*@brief Application specific function to assign a short address */
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16,uint8_t capabilityinformation);

/*The Application specific function for denoting the button next state*/
static void button_next_state(app_button_state_t* state);

/* === IMPLEMENTATION ====================================================== */

/*The Application specific function for denoting the button next state*/
static void button_next_state(app_button_state_t* state)
{
    if((state->Sub_state) < no_of_assoc_devices)
    {
       (state->Sub_state)++;
        if(state->Sub_state == no_of_assoc_devices)
        {
            state->Next_state = BROADCAST_DATA;            
        }            
        else
        {
            state->Next_state = UNICAST_DATA;            
        }            
    }
    else 
    {
        state->Next_state = UNICAST_DATA;
        state->Sub_state = 0U;
    }
}

static void Data_Ind_LED_Timer_Callback(TimerHandle_t xTimer)
{
    RGB_LED_GREEN_Off();
    
    /* Keep compiler happy. */
     (void)xTimer;
    
}

static void Data_Conf_LED_Timer_Callback(TimerHandle_t xTimer)
{
    USER_LED_On();
    /* Keep compiler happy. */
    (void)xTimer;
}

/* * @brief Application specific function to assign a short address * */
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16,uint8_t capabilityinformation)
{
	uint16_t i;	
//    if(no_of_assoc_devices < 4U)
//    {
        for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++) /* Check if device has been associated before */
        {
            if (device_list[i].shortAddr == 0x0000U) 
            {
                /* If the short address is 0x0000, it has not been used before */
                continue;
            }
            if (device_list[i].ieeeAddr == addr64) 
            {
                /* Assign the previously assigned short address again */
                *addr16 = device_list[i].shortAddr;
                return true;
            }
        }
        for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++) 
        {
            if (device_list[i].shortAddr == 0x0000U) 
            {
                *addr16 = (i + 0x0001U);
                device_list[i].shortAddr = (i + 0x0001U);        /* get next short address */                                                                                   
                device_list[i].ieeeAddr = addr64;               /* store extended address */
                device_list[i].capability_info = capabilityinformation;
                no_of_assoc_devices++;
                SYS_CONSOLE_PRINT(Display_Associated_Device, (i + 1U));
                return true;
            }
        }
//    }
	/* If we are here, no short address could be assigned. */
	return false;
}

/* * @brief This function will initializes the MAC FTD Demo
     @param void * */ 

void MAC_FFDDemoInit(void)
{
    /* Initialize LEDs. */
	RGB_LED_RED_On(); /* indicating application is started */
	RGB_LED_BLUE_Off(); /* indicating network is started */
	RGB_LED_GREEN_Off();    /* indicating data transmission */
    USER_LED_On();
 
	SYS_CONSOLE_MESSAGE("\nMAC FFD Demo Application\r\n\n");
    print_stack_app_build_features();
    /* The timer created for LED that blinks when it receives the data from the Co-ordinator */
    Data_Ind_LED_Timer_Handle = xTimerCreate("Milli_Timer",pdMS_TO_TICKS(LED_BLINK_TIME_MS),pdFALSE, ( void * ) 0, Data_Ind_LED_Timer_Callback);
    /* The timer created for LED that blinks when it sends the data to the Co-ordinator */
    Data_Conf_LED_Timer_Handle = xTimerCreate("Milli_Timer",pdMS_TO_TICKS(LED_BLINK_TIME_MS),pdFALSE, ( void * ) 0, Data_Conf_LED_Timer_Callback);
    (void)WPAN_MLME_ResetReq(true);
}

/*
 * @brief This function will show up the application and stack build features and available in the firmware
 * @param void
 *
 */
void print_stack_app_build_features(void)
{
#ifdef STB_ON_SAL
	SYS_CONSOLE_MESSAGE("\r\n Security Tool Box On SAL : Enabled\n");
#else
	SYS_CONSOLE_MESSAGE("\r\n Security Tool Box On SAL : Disabled");
#endif

#ifdef MAC_SECURITY_ZIP
	SYS_CONSOLE_MESSAGE("\r\n MAC Data & Security Module : Enabled\n");
#else
	SYS_CONSOLE_MESSAGE("\r\n MAC Data & Security Module : Disabled");
#endif

#ifdef HIGH_DATA_RATE_SUPPORT
	SYS_CONSOLE_MESSAGE("\r\n High Data Rate Support : Enabled");
#else
	SYS_CONSOLE_MESSAGE("\r\n High Data Rate Support : Disabled\n");
#endif
}

void Handle_DataConf(APP_Msg_T *appMsg)
{
    usr_mcps_data_cnf_t mscpDataConf;
    (void)memcpy((void *)&mscpDataConf,(const void *)appMsg->msgData,sizeof(usr_mcps_data_cnf_t));
    
	SYS_CONSOLE_PRINT(Dispaly_Result_Frame, mscpDataConf.msduHandle);

	if (mscpDataConf.status == (uint8_t)MAC_SUCCESS) 
    {

		SYS_CONSOLE_MESSAGE("Success\r\n");
        USER_LED_Off();
        if( xTimerIsTimerActive( Data_Conf_LED_Timer_Handle ) != pdFALSE )
        {
            /* xTimer is active, do something. */
           (void)xTimerStop( Data_Conf_LED_Timer_Handle, pdMS_TO_TICKS(0) );
        }
        (void)xTimerStart(Data_Conf_LED_Timer_Handle,pdMS_TO_TICKS(0));
	}
    else if (mscpDataConf.status == (uint8_t)MAC_TRANSACTION_OVERFLOW)
    {
		/* Frame could not be placed into the indirect queue. */
		SYS_CONSOLE_MESSAGE("Transaction overflow\r\n");
	} 
    else if (mscpDataConf.status == (uint8_t)MAC_TRANSACTION_EXPIRED)
    {
		/*
		 * Frame could not be delivered to the target node within
		 * the proper time.
		 */
		SYS_CONSOLE_MESSAGE("Transaction expired\r\n");
	} 
    else if (mscpDataConf.status == (uint8_t)MAC_NO_ACK)
    {
		/*
		 * Frame could not be delivered to the target node within
		 * the proper time.
		 */
		SYS_CONSOLE_MESSAGE("Frame Transmitted MAC No Ack\r\n");
	} 
    else if (mscpDataConf.status == (uint8_t)MAC_CHANNEL_ACCESS_FAILURE) 
    {
		/*
		 * Frame could not be delivered to the target node within
		 * the proper time.
		 */
		SYS_CONSOLE_MESSAGE("MAC Channel Access Failure\r\n");
	}
    else
    {
        // do nothing
    }
	/* Keep compiler happy. */
	(void)mscpDataConf.msduHandle;
#ifdef ENABLE_TSTAMP
	(void)mscpDataConf.Timestamp;
#endif  /* ENABLE_TSTAMP */
}

void Handle_DataInd(APP_Msg_T *appMsg)
{
    usr_mcps_data_ind_t mscpDataInd;
    (void)memcpy((void *)&mscpDataInd,(const void *)appMsg->msgData,sizeof(usr_mcps_data_ind_t));   

	SYS_CONSOLE_PRINT(Display_Rx_Frame_Address,(mscpDataInd.SrcAddrSpec->Addr.shortAddress));
    for (uint8_t i = 0; i < mscpDataInd.msduLength; i++)
    {
        SYS_CONSOLE_PRINT("%c", mscpDataInd.msdu[i]);
    }   
    SYS_CONSOLE_MESSAGE("\n");
    
    RGB_LED_GREEN_On();
    if( xTimerIsTimerActive( Data_Ind_LED_Timer_Handle ) != pdFALSE )
    {
        /* xTimer is active, do something. */
        (void)xTimerStop( Data_Ind_LED_Timer_Handle, pdMS_TO_TICKS(0) );
    }
    (void)xTimerStart(Data_Ind_LED_Timer_Handle,pdMS_TO_TICKS(0));
    
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

void Handle_AssociateInd(APP_Msg_T *appMsg)
{
    usr_mlme_associate_ind_t mlmeAssocInd;
    (void)memcpy((void *)&mlmeAssocInd,(const void *)appMsg->msgData,sizeof(usr_mlme_associate_ind_t));
    usr_mlme_associate_ind_t assocInd;
    
    assocInd.CapabilityInformation = mlmeAssocInd.CapabilityInformation;
    assocInd.DeviceAddress = mlmeAssocInd.DeviceAddress;
    
    /*
	 * Any device is allowed to join the network.
	 * Use: bool WPAN_MLME_AssociateResp(uint64_t DeviceAddress,uint16_t AssocShortAddress,uint8_t status);
	 *
	 * This response leads to comm status indication ->
	 * usr_mlme_comm_status_ind
	 * Get the next available short address for this device.
	 */
	uint16_t associate_short_addr = macShortAddress_def;

	if (assign_new_short_addr(assocInd.DeviceAddress,&associate_short_addr,assocInd.CapabilityInformation) == true)
    {
        RGB_LED_BLUE_Off();
        pan_at_capacity = false;
        (void)WPAN_MLME_AssociateResp(assocInd.DeviceAddress,associate_short_addr,ASSOCIATION_SUCCESSFUL);
	} 
    else 
    {
        pan_at_capacity = true;
		(void)WPAN_MLME_AssociateResp(assocInd.DeviceAddress, associate_short_addr,PAN_AT_CAPACITY);
	}

	/* Keep compiler happy. */
	(void)mlmeAssocInd.CapabilityInformation;
}

void Handle_CommStatusInd(APP_Msg_T *appMsg)
{
    usr_mlme_comm_status_ind_t mlmeCommStatusInd;
    (void)memcpy((void *)&mlmeCommStatusInd,(const void *)appMsg->msgData,sizeof(usr_mlme_comm_status_ind_t));
    if (mlmeCommStatusInd.status == (uint8_t)MAC_SUCCESS)
    {
        if(!pan_at_capacity)
        {
            #if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
                recent_assoc_dev_no++;
                (void)WPAN_MLME_SetReq(macDeviceTableEntries,NO_PIB_INDEX,&no_of_assoc_devices);
                (void)WPAN_MLME_GetReq(macKeyTable, (uint8_t)recent_assoc_dev_no);
            #endif
        }
    }
    
	/* Keep compiler happy. */
	(void)mlmeCommStatusInd.SrcAddrSpec;
	(void)mlmeCommStatusInd.DstAddrSpec;    
}
void Handle_ButtonInterrupt(APP_Msg_T *appMsg)
{
	uint8_t src_addr_mode;
	WPAN_AddrSpec_t dst_addr;
    
    SYS_CONSOLE_MESSAGE("\n---Button Pressed---");

    if(no_of_assoc_devices > 0U)
    {       
        switch(Button_state.Next_state)
        {
            case UNICAST_DATA:
            {
                const char unicast_disp[] = "\nUnicast data :Queue data for device %u ";
                SYS_CONSOLE_PRINT(unicast_disp,(((uint8_t)Button_state.Sub_state)+1U));
                
                uint8_t unicast_len = (uint8_t)strlen(payload_unicast);             
                (void)memcpy((void *)&payload_arr,(const void *)payload_unicast,unicast_len);
                
                src_addr_mode = WPAN_ADDRMODE_SHORT;
                dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
                dst_addr.PANId = DEFAULT_PAN_ID;
                dst_addr.Addr.shortAddress = device_list[Button_state.Sub_state].shortAddr;
                                           
                if((bool)((device_list[Button_state.Sub_state].capability_info) & WPAN_CAP_ALLOCADDRESS))
                {
                    curr_msdu_handle++; /* Increment handle */
                    SYS_CONSOLE_PRINT(Display_MSDU_Handle, curr_msdu_handle);
                    if (!WPAN_MCPS_Datareq(src_addr_mode,&dst_addr,
                         unicast_len,  /* One octet */
                         payload_arr,
                         curr_msdu_handle,
                         WPAN_TXOPT_INDIRECT_ACK
                     #if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
                         , ZIP_SEC_MIN, NULL, ZIP_KEY_ID_MODE,
                         (uint8_t)device_list[Button_state.Sub_state].shortAddr
                     #endif
                         )
                         ) 
                    {
                        SYS_CONSOLE_MESSAGE("\nBuffer full");
                    }
                }
            button_next_state(&Button_state);
            break;              
        } 
        case BROADCAST_DATA:
        {
            const char broadcast_disp[] = "\nBroadcast data : Queue data for all devices ";
            SYS_CONSOLE_PRINT(broadcast_disp);
            
            uint8_t broadcast_len = (uint8_t)strlen(payload_broadcast);                   
            (void)memcpy((void *)&payload_arr,(const void *)payload_broadcast,broadcast_len);
            
            for(uint8_t i=0;i<no_of_assoc_devices;i++)
            {
                src_addr_mode = WPAN_ADDRMODE_SHORT;
                dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
                dst_addr.PANId = DEFAULT_PAN_ID;                             
                
                if((bool)(device_list[i].capability_info  & WPAN_CAP_ALLOCADDRESS)) //indirect queue -rfd device
                {
                    dst_addr.Addr.shortAddress = device_list[i].shortAddr;
                    curr_msdu_handle++; /* Increment handle */
                    SYS_CONSOLE_PRINT(Display_MSDU_Handle, curr_msdu_handle);                    
                    if (!WPAN_MCPS_Datareq(src_addr_mode,
                    &dst_addr,
                    broadcast_len,  /* One octet */
                    payload_arr,
                    curr_msdu_handle,
                    WPAN_TXOPT_INDIRECT_ACK
                    #if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
                    , ZIP_SEC_MIN, NULL, ZIP_KEY_ID_MODE,
                    (uint8_t)device_list[i].shortAddr
                    #endif
                    )
                    ) 
                    {
                        SYS_CONSOLE_MESSAGE("\nBuffer full");
                    }
                }
            } 
            button_next_state(&Button_state);
            break; 
        } 
        default:
        {
            button_next_state(&Button_state);
            break;
        }
        }     
    }
}

void Handle_GetConf(APP_Msg_T *appMsg)
{
    usr_mlme_get_conf_t getConf;
    (void)memcpy((void *)&getConf,(const void *)appMsg->msgData,sizeof(usr_mlme_get_conf_t));

#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
	MAC_KeyTable_t *key_table = (MAC_KeyTable_t *)getConf.PIBAttributeValue;
#endif
    
    if (getConf.status == (uint8_t)MAC_SUCCESS)
    {
        switch(getConf.PIBAttribute)
        {
            case phyCurrentPage:
            {
#ifdef HIGH_DATA_RATE_SUPPORT
                    current_channel_page = 0; /* To do */
#else
                    current_channel_page = *(uint8_t *)getConf.PIBAttributeValue;
#endif
                    (void)WPAN_MLME_GetReq(phyChannelsSupported
#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
                            , 0
#endif
                            );
            }
            break;
            
            case phyChannelsSupported:
            {
                uint32_t index;

                channels_supported = convert_byte_array_to_32_bit(
                        getConf.PIBAttributeValue);

                for (index = 0; index < 32U; index++) {
                    if ((bool)(channels_supported & (uint32_t)(1UL << index))) {
                        current_channel = (uint8_t)(index + CHANNEL_OFFSET);
                        break;
                    }
                }           
                /*
                 * Set the short address of this node.
                 * Use: bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
                 *                             void *PIBAttributeValue);
                 *
                 * This request leads to a set confirm message ->
                 * usr_mlme_set_conf
                 */

                /* MAC Short Address will be set after setting the Security
                 * PIB's */
#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
                /* Set security PIB attributes now. */
                (void)WPAN_MLME_SetReq(macDefaultKeySource,
                        NO_PIB_INDEX,
                        &default_key_source);
#else
                uint8_t short_addr[2];

                short_addr[0] = (uint8_t)COORD_SHORT_ADDR;  /* low byte */
                short_addr[1] = (uint8_t)(COORD_SHORT_ADDR >> 8); /* high byte
                                                                  **/

                (void)WPAN_MLME_SetReq(macShortAddress, short_addr);
#endif /* (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006) */
            }
            break;
#ifdef MAC_SECURITY_ZIP
            case macKeyTable:
            {
                for (uint8_t j = 0; j < key_table->KeyDeviceListEntries; j++) 
                {
                    if (EMPTY_DEV_HANDLE ==
                            (key_table->KeyDeviceList[j].
                            DeviceDescriptorHandle)) {
                        key_table->KeyDeviceList[j].
                        DeviceDescriptorHandle
                            = (uint8_t)recent_assoc_dev_no;
                        key_table->KeyDeviceList[j].UniqueDevice = true;
                        break;
                    }
                }
                (void)WPAN_MLME_SetReq(macKeyTable, getConf.PIBAttributeIndex,
                        (uint8_t *)getConf.PIBAttributeValue);
            }
            break;
            default:
            {
                break;
            }       
#endif                       
        }
    }
    else
    {
        /* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
    } 
}

void Handle_ResetConf(APP_Msg_T *appMsg)
{
    usr_mlme_reset_conf_t resetConf;
    (void)memcpy((void *)&resetConf,(const void *)appMsg->msgData,sizeof(usr_mlme_reset_conf_t));
    
	if (resetConf.status == (uint8_t)MAC_SUCCESS) {
		(void)WPAN_MLME_GetReq(phyCurrentPage
#if (defined MAC_SECURITY_ZIP) || (defined MAC_SECURITY_2006)
				, 0
#endif
				);
	} else {
		/* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
	}
}

void Handle_ScanConf(APP_Msg_T *appMsg)
{
    usr_mlme_scan_conf_t scanConf;
    (void)memcpy((void *)&scanConf,(const void *)appMsg->msgData,sizeof(usr_mlme_scan_conf_t));
  
    /*
	 * We are not interested in the actual scan result,
	 * because we start our network on the pre-defined channel anyway.
	 * Start a beacon-enabled network
	 * Use: bool WPAN_MLME_StartReq(uint16_t PANId,
	 *                               uint8_t LogicalChannel,
	 *                               uint8_t ChannelPage,
	 *                               uint8_t BeaconOrder,
	 *                               uint8_t SuperframeOrder,
	 *                               bool PANCoordinator,
	 *                               bool BatteryLifeExtension,
	 *                               bool CoordRealignment)
	 *
	 * This request leads to a start confirm message -> usr_mlme_start_conf
	 */
	(void)WPAN_MLME_StartReq(DEFAULT_PAN_ID,
			current_channel,
			current_channel_page,
			DEFAULT_BO,
			DEFAULT_SO,
			true, false,
			DEFAULT_COORDREALIGNMENT
			);

	/* Keep compiler happy. */
	(void)scanConf.status ;
	(void)scanConf.ScanType ;
	(void)scanConf.ChannelPage;
	(void)scanConf.UnscannedChannels ;
	(void)scanConf.ResultListSize ;
	(void)scanConf.ResultList ;
}

void Handle_StartConf(APP_Msg_T *appMsg)
{
    usr_mlme_start_conf_t startConf;
    uint16_t mac_transaction_persistence_time = 700U;
    (void)memcpy((void *)&startConf,(const void *)appMsg->msgData,sizeof(usr_mlme_start_conf_t));
        
    if (startConf.status == (uint8_t)MAC_SUCCESS) 
    {
		coord_state = COORD_RUNNING;

		SYS_CONSOLE_PRINT("\r\nStarted No beacon network in Channel - %d\r\n",current_channel);

		/*
		 * Network is established.
		 * Waiting for association indication from a device.
		 * -> usr_mlme_associate_ind
		 */
        RGB_LED_RED_Off();
		RGB_LED_BLUE_On();
    } 
    else 
    {
		/* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
	}    
    #if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))
   (void)WPAN_MLME_SetReq(macTransactionPersistenceTime,NO_PIB_INDEX,&mac_transaction_persistence_time);
	#else
    (void)WPAN_MLME_SetReq(macTransactionPersistenceTime,&mac_transaction_persistence_time);
    #endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */    
}

#ifndef MAC_SECURITY_ZIP
void Handle_SetConf(APP_Msg_T *appMsg)
{
    usr_mlme_set_conf_t setConf;
    (void)memcpy(&setConf,appMsg->msgData,sizeof(usr_mlme_set_conf_t));

        
    if(setConf.status == MAC_SUCCESS)
    {
        bool flag = false;
        switch(setConf.PIBAttribute)
        {
            case macShortAddress:
            {
                /*
                 * Allow other devices to associate to this coordinator.
                 * Use: bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
                 *                             void *PIBAttributeValue);
                 *
                 * This request leads to a set confirm message ->
                 * usr_mlme_set_conf
                 */
                uint8_t association_permit = true;

                (void)WPAN_MLME_SetReq(macAssociationPermit,
                        &association_permit);
            }
            break;
                
            case macAssociationPermit:
            {
                /*
                 * Set RX on when idle to enable the receiver as default.
                 * Use: bool WPAN_MLME_SetReq(uint8_t PIBAttribute,
                 *                             void *PIBAttributeValue);
                 *
                 * This request leads to a set confirm message ->
                 * usr_mlme_set_conf
                 */

                bool rx_on_when_idle = true;

                (void)WPAN_MLME_SetReq(macRxOnWhenIdle,&rx_on_when_idle);
                
            }
            break;
                
            case macRxOnWhenIdle:
            {
                /* Set the beacon payload length. */
                uint8_t beacon_payload_len = BEACON_PAYLOAD_LEN;
                (void)WPAN_MLME_SetReq(macBeaconPayloadLength,
                                &beacon_payload_len);
            }
            break;
                
            case macBeaconPayloadLength:
            {
                /*
                 * Once the length of the beacon payload has been defined,
                 * set the actual beacon payload.
		         */
                (void)WPAN_MLME_SetReq(macBeaconPayload,
                                &beacon_payload);
            }
            break;
                
            case macBeaconPayload:
            {
                if (COORD_STARTING == coord_state) 
                {
                    /*
                     * Initiate an active scan over all channels to
                     * determine
                     * which channel to use.
                     * Use: bool wpan_mlme_scan_req(uint8_t ScanType,
                     *                              uint32_t ScanChannels,
                     *                              uint8_t ScanDuration,
                     *                              uint8_t ChannelPage);
                     *
                     * This request leads to a scan confirm message ->
                     * usr_mlme_scan_conf
                     * Scan for about 50 ms on each channel -> ScanDuration
                     *= 1
                     * Scan for about 1/2 second on each channel ->
                     * ScanDuration = 5
                     * Scan for about 1 second on each channel ->
                     * ScanDuration = 6
                     */
                    WPAN_MLME_ScanReq(MLME_SCAN_TYPE_ACTIVE,
                            SCAN_CHANNEL(current_channel),
                            SCAN_DURATION_COORDINATOR,
                            current_channel_page);
                } else {
                    /* Do nothing once the node is properly running. */
                }
            }
            break;
                
            default:
            {
                flag = true;
                break;
            }              
        }
    }
    else
    {
        /* Something went wrong; restart. */
		(void)WPAN_MLME_ResetReq(true);
    }
}
#endif

void Handle_BeaconNotifyInd(APP_Msg_T *appMsg)
{
   //Nothing to do
   (void)appMsg;    
}

void Handle_Default(void)
{
    SYS_CONSOLE_MESSAGE("\n  Default has been called  ");
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

        case APP_MSG_MAC_START_CONF:
        {
            Handle_StartConf(appMsg);
        }
        break;

        case APP_MSG_MAC_COMM_STATUS_IND:
        {
            Handle_CommStatusInd(appMsg);
        }
        break;

        case APP_MSG_MAC_ASSOCIATE_IND:
        {
            Handle_AssociateInd(appMsg);
        }
        break;
        case APP_MSG_BTN_INT_CB:
        {
            Handle_ButtonInterrupt(appMsg);
        }
        break;

        default:
        {
            Handle_Default();
            break;
        }
     }
}

