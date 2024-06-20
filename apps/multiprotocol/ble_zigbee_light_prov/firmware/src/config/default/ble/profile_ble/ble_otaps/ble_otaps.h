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
  BLE OTA Profile Server Header File

  Company:
    Microchip Technology Inc.

  File Name:
    ble_otaps.h

  Summary:
    This file contains the BLE OTA Profile Server functions for application user.

  Description:
    This file contains the BLE OTA Profile Server functions for application user.
 *******************************************************************************/


/**
 * @addtogroup BLE_OTAPS
 * @{
 * @brief Header file for the BLE OTA Profile Server library.
 * @note Definitions and prototypes for the BLE OTA profile server stack layer application programming interface.
 */
#ifndef BLE_OTAPS_H
#define BLE_OTAPS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "stack_mgr.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Macros
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_OTAPS_DEFINES Defines
 * @{ */

/**@defgroup BLE_OTAPS_IMG_TYPE_DEF OTAPS image type
 * @brief The definition of image type.
 * @{ */
#define BLE_OTAPS_IMG_TYPE_FW                    (1U << 0U)                              /**< Firmaware image type. */
/** @} */

/**@defgroup BLE_OTAPS_FW_FEATURE_DEF OTAPS firmware extended feature
 * @brief The definition of firmware extended feature.
 * @{ */
#define BLE_OTAPS_FW_FEATURE_MASK1               (1U << 0U)                              /**< Supported feature mask 1. */
/** @} */


/**@defgroup BLE_OTAPS_FEATURE_DEF OTAPS supported feature
 * @brief The definition of supported features.
 * @{ */
#define BLE_OTAPS_FEATURE_SUPP_IMG_TYPE          (BLE_OTAPS_IMG_TYPE_FW)                 /**< The supported image type of BLE OTA profile. */
#define BLE_OTAPS_FEATURE_FW_EXT_FEATURE         (BLE_OTAPS_FW_FEATURE_MASK1)            /**< The supported firmware extended feature of BLE OTA profile. */
/** @} */


/**@defgroup BLE_OTAPS_IMG_FILE_TYPE_DEF OTAPS image file type
 * @brief The definition of image file types.
 * @{ */
#define BLE_OTAPS_IMG_FILE_TYPE_INT              (0x01U)                                 /**< The image is for internal flash. */
#define BLE_OTAPS_IMG_FILE_TYPE_APP              (0x02U)                                 /**< Pass the image to application. */
#define BLE_OTAPS_IMG_FILE_TYPE_EXT              (0x03U)                                 /**< The image is for external flash. */
#define BLE_OTAPS_IMG_FILE_TYPE_UNKNOWN          (0xFFU)                                 /**< The image file type is unknown. */
/** @} */


/**@} */ //BLE_OTAPS_DEFINES


/**@addtogroup BLE_OTAPS_ENUMS Enumerations
 * @{ */

/**@brief Enumeration type of BLE OTA profile callback events. */

typedef enum BLE_OTAPS_EventId_T
{
    BLE_OTAPS_EVT_UPDATE_REQ=0x0U,                          /**< Request to start update procedure. See @ref BLE_OTAPS_EvtUpdateReq_T for event details. */
    BLE_OTAPS_EVT_START_IND,                                /**< Indication of starting one firmware image update. See @ref BLE_OTAPS_EvtStartInd_T for event details. */
    BLE_OTAPS_EVT_UPDATING_IND,                             /**< Indication of fragmented firmware image update. See @ref BLE_OTAPS_EvtUpdatingInd_T for event details. */
    BLE_OTAPS_EVT_UPDATING_REQ,                             /**< Request to update fragmented firmware image. Application must call @ref BLE_OTAPS_UpdatingResponse to update procedure. See @ref BLE_OTAPS_EvtUpdatingInd_T for event details. */
    BLE_OTAPS_EVT_COMPLETE_IND,                             /**< Indication of firmware update completed. See @ref BLE_OTAPS_EvtCompleteInd_T for event details. */
    BLE_OTAPS_EVT_RESET_IND,                                /**< Indication of device reset request received. OTA client role may ask OTA Server device to rest after firmware update procedure is completed. */
    BLE_OTAPS_EVT_ERR_UNSPECIFIED_IND                       /**< Profile internal unspecified error occurs. */
}BLE_OTAPS_EventId_T;

/**@} */ //BLE_OTAPS_ENUMS

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_OTAPS_STRUCTS Structures
 * @{ */

/**@brief Data structure for @ref BLE_OTAPS_EVT_UPDATE_REQ event. */
typedef struct BLE_OTAPS_EvtUpdateReq_T
{
    uint16_t 	connHandle;                                 /**< The connection handle to request firmware update. */
    uint32_t 	fwImageSize;                                /**< The size of new firmware image. */
    uint32_t 	fwImageId;                                  /**< The identity of new firmware image. */
    uint32_t 	fwImageVer;                                 /**< The version of new firmware image. */
    uint16_t 	fwImageChksum;                              /**< The checksum of new firmware image. */
    uint16_t 	fwImageCrc16;                               /**< The CRC-16 value of new firmware image. */
    uint8_t  	fwImageFileType;                            /**< The file type of new firmware image. See @ref BLE_OTAPS_IMG_FILE_TYPE. */
} BLE_OTAPS_EvtUpdateReq_T;


/**@brief Data structure for @ref BLE_OTAPS_EVT_START_IND event. */
typedef struct BLE_OTAPS_EvtStartInd_T
{
    uint8_t  	imageType;                                  /**< The type of image to update, see @ref BLE_OTAPS_IMG_TYPE_DEF. */
} BLE_OTAPS_EvtStartInd_T;


/**@brief Data structure for @ref BLE_OTAPS_EVT_UPDATING_IND event. */
typedef struct BLE_OTAPS_EvtUpdatingInd_T
{
    uint16_t  	length;                                     /**< The length of image fragment be updated. */
    uint8_t   	*p_fragment;                                /**< The image fragment need to be updated. If p_fragment is not NULL, Application must update fragment by ifself. */
} BLE_OTAPS_EvtUpdatingInd_T;


/**@brief Data structure for @ref BLE_OTAPS_EVT_COMPLETE_IND event. */
typedef struct BLE_OTAPS_EvtCompleteInd_T
{
    bool  		errStatus;                                  /**< The error status of the update procedure. If errStatus is false, Application must call @ref BLE_OTAPS_CompleteResponse to respond peer device the result. */
} BLE_OTAPS_EvtCompleteInd_T;


/**@brief Union of BLE OTA profile callback event data types. */
typedef union
{
    BLE_OTAPS_EvtUpdateReq_T            evtUpdateReq;       /**< Handle @ref BLE_OTAPS_EVT_UPDATE_REQ. */
    BLE_OTAPS_EvtStartInd_T             evtStartInd;        /**< Handle @ref BLE_OTAPS_EVT_START_IND. */    
    BLE_OTAPS_EvtUpdatingInd_T          evtUpdatingInd;     /**< Handle @ref BLE_OTAPS_EVT_UPDATING_IND. */        
    BLE_OTAPS_EvtCompleteInd_T          evtCompleteInd;     /**< Handle @ref BLE_OTAPS_EVT_COMPLETE_IND. */        
} BLE_OTAPS_EventField_T;


/**@brief BLE OTA profile callback event.*/
typedef struct  BLE_OTAPS_Event_T
{
    BLE_OTAPS_EventId_T                 eventId;            /**< Event ID. */
    BLE_OTAPS_EventField_T              eventField;         /**< Event field. */
} BLE_OTAPS_Event_T;


/**@brief The structure contains current device information. \n
* It should be used to response the event @ref BLE_OTAPS_EVT_UPDATE_REQ. */
typedef struct BLE_OTAPS_DevInfo_T
{
    uint32_t  fwImageVer;                                    /**< The version of current firmware image. */
} BLE_OTAPS_DevInfo_T;

/**@brief BLE OTA profile callback type. This callback function sends BLE OTA profile events to the application.*/
typedef void(*BLE_OTAPS_EventCb_T)(BLE_OTAPS_Event_T *p_event);

/**@} */ //BLE_OTAPS_STRUCTS

// *****************************************************************************
// *****************************************************************************
// Section: Function Prototypes
// *****************************************************************************
// *****************************************************************************
/**@addtogroup BLE_OTAPS_FUNS Functions
 * @{ */

/**
 *@brief Initialize BLE OTA profile and service.
 *
 * @retval MBA_RES_SUCCESS          Success to initialize BLE OTA profile and service. 
 *
 */
uint16_t BLE_OTAPS_Init(void);

/**
 *@brief Register BLE OTA profile callback.\n
 *       Application must call this API before starting BLE OTA procedure. 
 *
 *
 *@param[in] bleOtapsRoutine        Client callback function.
 *
 */
void BLE_OTAPS_EventRegister(BLE_OTAPS_EventCb_T bleOtapsRoutine);

/**
 *@brief Stop BLE OTA procedure.
 *       Application could call this API to stop ongoing BLE OTA DFU procedure.
 *
 *
 * @retval MBA_RES_SUCCESS          Successfully stop BLE OTA procedure.
 */
uint16_t BLE_OTAPS_UpdateStop(void);

/**
 *@brief Set IV value and encryption key of AES-CBC for OTA DFU new image decryption.\n
 *       Application must call this API before starting BLE OTA procedure if image is encrpyted. 
 *
 *@param[in] p_iv                   Pointer to the 16 bytes IV value.
 *@param[in] p_key                  Pointer to the 16 bytes encryption key.
 *
 * @retval MBA_RES_SUCCESS          Successfully set IV value and Key.
 * @retval MBA_RES_BAD_STATE        Failure because the BLE OTA is in progress.
 */
uint16_t BLE_OTAPS_SetEncrytionInfo(uint8_t * p_iv, uint8_t * p_key);

/**
 *@brief Respond peer device after receiving @ref BLE_OTAPS_EVT_UPDATE_REQ event. Application must determinate if allow to start BLE OTA update procedure.
 *
 *@param[in] connHandle             Handle of the connection to send response. 
 *@param[in] isAllow                Set true if application allows device firmware update procedure, otherwise set false.
 *@param[in] p_devInfo              Pointer to the current device information. See @ref BLE_OTAPS_DevInfo_T in detail. The information would be sent to peer device if isAllow is set to true. It would be ignored if isAllow is set to false.
 *
 *@retval MBA_RES_SUCCESS           Successfully send response.
 *@retval MBA_RES_OOM               Memory not available to send response.
 *@retval MBA_RES_INVALID_PARA      Invalid parameters. Connection handle is not valid
 *@retval MBA_RES_BAD_STATE         Application should not call this API in current OTA progress.
 *
 */
uint16_t BLE_OTAPS_UpdateResponse(uint16_t connHandle, bool isAllow, BLE_OTAPS_DevInfo_T * p_devInfo);

/**
 *@brief Respond peer device after receiving the @ref BLE_OTAPS_EVT_UPDATING_REQ event.
 *       Application must update fragmented firmware image by ifself.
 *
 *@param[in] success                Set true if update is successful, otherwise set false.
 *
 *@retval MBA_RES_SUCCESS           Successfully send response.
 *@retval MBA_RES_OOM               Memory not available to send response.
 *@retval MBA_RES_BAD_STATE         Application should not call this API in current progress.
 *
 */
uint16_t BLE_OTAPS_UpdatingResponse(bool success);


/**
 *@brief Respond peer device after receiving the @ref BLE_OTAPS_EVT_COMPLETE_IND event if errStatus is false.
 *       Application must check if the device firmware update is successful or fail.
 *
 *@param[in] success                Set true if device firmware update is successful, otherwise set false.
 *
 *@retval MBA_RES_SUCCESS           Successfully send response.
 *@retval MBA_RES_OOM               Memory not available to send response.
 *@retval MBA_RES_BAD_STATE         Application should not call this API in current progress.
 *
 */
uint16_t BLE_OTAPS_CompleteResponse(bool success);


/**@brief Handle BLE_Stack events.
 *       This API should be called in the application while caching BLE_Stack events
 *
 * @param[in] p_stackEvent          Pointer to BLE_Stack events buffer.
 *
*/
void BLE_OTAPS_BleEventHandler(STACK_Event_T *p_stackEvent);


/**@} */ //BLE_OTAPS_FUNS


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif

/**
  @}
*/


