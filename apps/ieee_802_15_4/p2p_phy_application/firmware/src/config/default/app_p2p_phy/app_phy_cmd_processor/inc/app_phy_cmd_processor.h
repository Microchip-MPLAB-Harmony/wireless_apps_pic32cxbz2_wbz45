/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Microchip Technology pvt Ltd

  @File Name
    app_phy_cmd_processor.h

  @Summary
    Command Interface header file for PHY & App layer functions

  @Description
    Command Interface header file for PHY & App layer functions
 */
/* ************************************************************************** */
/* ***************************************************************************** */

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
// *****************************************************************************
// *****************************************************************************

#ifndef APP_PHY_CMD_PROCESSOR_H    /* Guard against multiple inclusion */
#define APP_PHY_CMD_PROCESSOR_H
#include "definitions.h"
/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif



#define AOK                               (0U)
#define ERR_UNKNOWN_CMD                   (1U)
#define ERR_INVALID_PARAM                 (2U)
#define ERR_MISSING_PARAM                 (3U)
#define ERR_INVALID_PARAM_TYPE            (4U)
#define ERR_PARAM_OUT_OF_RANGE            (5U)
#define ERR_INVALID_FORMAT                (6U)
#define ERR_SCRIPT_PARSING                (7U)
#define ERR_HEAP_FULL                     (8U)
#define ERR_INVALID_STATE                 (9U)
#define ERR_NO_RESOURCES                  (10U)
#define ERR_BLE_ERR                       (11U)
#define ERR_HAL_ERR                       (12U)
#define ERR_UNKNOWN_ERR                   (13U)

#define PENDING                           (0xFCU)
#define SCRIPT_DBG_BREAK                  (0xFDU)
#define NO_PROMPT                         (0xFEU)
#define SEND_PROMPT                       (0xFFU)


typedef struct
{
    uint64_t nPacketsSent;
    uint64_t nBytesSent;
    uint64_t nPacketsRcvdAck; 
    bool isModeActive;
}APP_MODE_PARAMS;

typedef struct
{
  APP_MODE_PARAMS devPerformanceParam;  
}APPL_DATA_MODE;



typedef struct
{
  APP_MODE_PARAMS devPerformanceParam;  
}APPL_PERIOD_TEST_MODE;


typedef struct
{
  APP_MODE_PARAMS devPerformanceParam;  
}APPL_CNT_TEST_MODE;


typedef struct
{
  APP_MODE_PARAMS devPerformanceParam;  
}APPL_PER_TEST_MODE;


typedef struct
{
  APP_MODE_PARAMS devPerformanceParam;  
}APPL_THROUGHPUT_TEST_MODE;


void PhyCmdTable_Init(void);  
bool appPhyCmdProcessor_StrToBool(const char *str, bool *res);
bool appPhyCmdProcessor_StrToUint8(const char *str, uint8_t *res);
bool appPhyCmdProcessor_StrToInt8(const char *str, int8_t *res);
bool appPhyCmdProcessor_StrToUint8HexIp(const char *str, uint8_t *res);
bool appPhyCmdProcessor_StrToUint16(const char *str, uint16_t *res);
bool appPhyCmdProcessor_StrToUint16HexIp(const char *str, uint16_t *res);
bool appPhyCmdProcessor_StrToUint32(const char *str, uint32_t *res);
bool appPhyCmdProcessor_StrToUint64(const char *str, uint64_t *res);
bool appPhyCmdProcessor_StrToUint64DecIp(const char *str, uint64_t *res);
void appPhyCmdProcessor_PhyStatusPrint(PHY_Retval_t status);
void appPhyCmdProcessor_PhyTrxStatusPrint(PHY_TrxStatus_t status);
void appPhyCmdProcessor_CmdDoneOk(void);
void appPhyCmdProcessor_CmdDoneFail(uint8_t err_code);
void appPhyCmdProcessor_PrintReturnCode(uint8_t status);
void appPhyCmdProcessor_calculateDevicePerfParams(void);
void appPhyCmdProcessor_SetModeFlags(bool enterDM, bool enterPTM, bool enterCTM, bool enterThruputTM, bool enterPerTM, bool thruputCb, bool perTM);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif 

/* *****************************************************************************
 End of File
 */
