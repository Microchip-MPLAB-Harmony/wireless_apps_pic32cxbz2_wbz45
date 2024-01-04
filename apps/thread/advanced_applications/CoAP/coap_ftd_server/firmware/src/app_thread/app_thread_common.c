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

/*******************************************************************************
  Application Thread Common Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_thread_common.c

  Summary:
    This file contains the Application Thread Common Source code for this project.

  Description:
    This file contains the Application Thread Common Source code for this project.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <string.h>
#include <stdio.h>
#include "definitions.h"

#include "configuration.h"
#include "openthread/link.h"
#include "openthread/thread.h"
#include "openthread/dataset_ftd.h"
#include "app_thread_common.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

/**Pointer to the Thread instance. */
extern otInstance *instance;

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// *****************************************************************************
// Section: Interface Functions
// *****************************************************************************
// *****************************************************************************

void APP_ThreadInit(otStateChangedCallback stateChangedCallback) 
{
    otError error = OT_ERROR_NONE;
    
    if(instance != NULL)
    {
        if(error == OT_ERROR_NONE)
        {
            otLinkModeConfig mode;
            memset(&mode, 0, sizeof(mode));
            mode.mRxOnWhenIdle       = true;
            mode.mDeviceType         = true;
            mode.mNetworkData        = true;
            if(error == OT_ERROR_NONE)
            {
                error = otThreadSetLinkMode(instance, mode);
            }
            else
            {
                /* Add debug print if required.*/ 
            }        
        }
        else
        {
            /* Add debug print if required.*/ 
        }
    }
    else
    {
        /* Add debug print if required.*/ 
    }
    
    otSetStateChangedCallback(instance, stateChangedCallback, NULL);
}

void APP_ThreadSetNwParameters(void)
{    
        otOperationalDataset aDataset;
        memset(&aDataset, 0, sizeof(otOperationalDataset));
#if DEVICE_AS_LEADER  
        otError err = otDatasetCreateNewNetwork(instance, &aDataset);
        if (err != OT_ERROR_NONE) 
        {
            /* Add debug print if required.*/
        }
#endif

        aDataset.mChannel                      = CHANNEL;
        aDataset.mComponents.mIsChannelPresent = true;

        aDataset.mChannelMask                      = (otChannelMask)CHANNEL_MASK;
        aDataset.mComponents.mIsChannelMaskPresent = true;

        aDataset.mPanId                      = (otPanId)PANID;
        aDataset.mComponents.mIsPanIdPresent = true;

        uint8_t ex_panid[OT_EXT_PAN_ID_SIZE] = EXD_PANID;
        memcpy(aDataset.mExtendedPanId.m8, ex_panid, sizeof(aDataset.mExtendedPanId));
        aDataset.mComponents.mIsExtendedPanIdPresent = true;

        uint8_t nw_key[OT_NETWORK_KEY_SIZE] = NW_KEY;
        memcpy(aDataset.mNetworkKey.m8,nw_key,sizeof(aDataset.mNetworkKey));
        aDataset.mComponents.mIsNetworkKeyPresent = true;

        char nwk_name[] = NWK_NAME;
        size_t length = strlen(nwk_name);
        memset(aDataset.mNetworkName.m8,0,OT_NETWORK_NAME_MAX_SIZE);
        if(length <= OT_NETWORK_NAME_MAX_SIZE)
        {
           memcpy(aDataset.mNetworkName.m8, nwk_name, length);
        }
        else
        {
           memcpy(aDataset.mNetworkName.m8, nwk_name, OT_NETWORK_NAME_MAX_SIZE); 
        }
        aDataset.mComponents.mIsNetworkNamePresent = true;

        uint8_t ml_prefix[OT_MESH_LOCAL_PREFIX_SIZE] = ML_PREFIX;
        memcpy(aDataset.mMeshLocalPrefix.m8,ml_prefix,sizeof(aDataset.mMeshLocalPrefix));
        aDataset.mComponents.mIsMeshLocalPrefixPresent = true;

        otDatasetSetActive(instance, &aDataset);
}

void APP_ThreadNwStart(void)
{   
    otError error = OT_ERROR_NONE;
    error = otIp6SetEnabled(instance, true);

    if(error == OT_ERROR_NONE)
    {
        error = otThreadSetEnabled(instance, true);
        if(error == OT_ERROR_NONE)
        {
            SYS_CONSOLE_MESSAGE("AppLog: Thread interface has been enabled\r\n");
            SYS_CONSOLE_PRINT("AppLog: Radio mode       : %s\r\n", otThreadGetLinkMode(instance).mRxOnWhenIdle ?
                                            "rx-on-when-idle" : "rx-off-when-idle"); 
        }
        else
        {
            /* Add debug print if required.*/ 
        }
    }
}

otInstance* APP_ThreadGetInstance(void)
{
    return instance; 
}




/* *****************************************************************************
 End of File
 */