/*******************************************************************************
* Copyright (C) 2024 Microchip Technology Inc. and its subsidiaries.
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

otError APP_ThreadInit(otStateChangedCallback stateChangedCallback) 
{
    otError error = OT_ERROR_NONE;
    
    if(instance != NULL)
    {
        if(error == OT_ERROR_NONE)
        {
            otLinkModeConfig mode;
            memset(&mode, 0, sizeof(mode));
            mode.mRxOnWhenIdle       = true;
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
    
    return error;
}

otError APP_ThreadSetNwParameters(APP_ProvNwData_T *provNwData)
{
    otError error = OT_ERROR_NONE;  
        otOperationalDataset aDataset;
        memset(&aDataset, 0, sizeof(otOperationalDataset));

        aDataset.mChannel                      = provNwData->channel;
        aDataset.mComponents.mIsChannelPresent = true;

        aDataset.mChannelMask                      = (otChannelMask)provNwData->channelMask;
        aDataset.mComponents.mIsChannelMaskPresent = true;

        aDataset.mPanId                      = (otPanId)provNwData->panId;
        aDataset.mComponents.mIsPanIdPresent = true;

        memcpy(aDataset.mExtendedPanId.m8, provNwData->extPanId, sizeof(aDataset.mExtendedPanId));
        aDataset.mComponents.mIsExtendedPanIdPresent = true;

        memcpy(aDataset.mNetworkKey.m8,provNwData->networkKey,sizeof(aDataset.mNetworkKey));
        aDataset.mComponents.mIsNetworkKeyPresent = true;

        size_t length = strlen(provNwData->networkName);
        memset(aDataset.mNetworkName.m8,0,OT_NETWORK_NAME_MAX_SIZE);
        if(length <= OT_NETWORK_NAME_MAX_SIZE)
        {
           memcpy(aDataset.mNetworkName.m8, provNwData->networkName, length);
        }
        else
        {
           memcpy(aDataset.mNetworkName.m8, provNwData->networkName, OT_NETWORK_NAME_MAX_SIZE); 
        }
        aDataset.mComponents.mIsNetworkNamePresent = true;

        memcpy(aDataset.mMeshLocalPrefix.m8,provNwData->meshLocalPrefix,sizeof(aDataset.mMeshLocalPrefix));
        aDataset.mComponents.mIsMeshLocalPrefixPresent = true;

        error = otDatasetSetActive(instance, &aDataset);
    return error; 
}

otError APP_ThreadNwStart(void)
{   
    otError error = OT_ERROR_NONE;
    error = otIp6SetEnabled(instance, true);

    if(error == OT_ERROR_NONE)
    {
        error = otThreadSetEnabled(instance, true);
        if(error == OT_ERROR_NONE)
        {
            /* Add debug print if required.*/ 
        }
        else
        {
            /* Add debug print if required.*/ 
        }
    }
    
    return error;
}

otInstance* APP_ThreadGetInstance(void)
{
    return instance; 
}

void APP_ThreadFactoryReset(void)
{
    otInstanceFactoryReset(instance);
}




/* *****************************************************************************
 End of File
 */
