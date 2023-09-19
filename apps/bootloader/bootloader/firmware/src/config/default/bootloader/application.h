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

// DOM-IGNORE-BEGIN
#ifndef __APPLICATION_H
#define __APPLICATION_H
//DOM-IGNORE-END

/** Image Memory Topology Count - Number of Image memory slots */
#define IMG_MEM_TOPOLOGY_COUNT              1 //5

/** Firmware Image Block Size */
#define FW_IMAGE_BLOCK_SIZE                 4096

/** Firmware Image Expected Payload Length */
#define FW_IMAGE_EXPECTED_PL_LEN            0xE0

/** Firmware Image Expected Continuous Index */
#define FW_IMAGE_EXPECTED_CONT_IDX          1

/** Maximum Authentication Key Length */
#define MAX_AUTH_KEY_LEN                    48

/** Critical Error while loop */
#define critical_error(x)	while(1)

/** Size of the MetaData header */
#define METADATA_HEADER_SIZE                0x200

/** Base Address of Slot 0 */
#define SLOT0_BASE_ADDR                     0x01000000

/** Base Address of Slot 1 */
#define SLOT1_BASE_ADDR                     0x01080000

/** Slot 0 Header Start Address */
#define SLOT0_HEADER                        SLOT0_BASE_ADDR

/** Slot 0 Firmware Start Address */
#define SLOT0_FIRMWARE                      (SLOT0_BASE_ADDR + METADATA_HEADER_SIZE)

/** Slot 1 Header Start Address */
#define SLOT1_HEADER                        SLOT1_BASE_ADDR

/** Slot 1 Firmware Start Address */
#define SLOT1_FIRMWARE                      (SLOT1_BASE_ADDR + METADATA_HEADER_SIZE)

/** Size of the UnAuthenticated Firmware Size - For Error purpose */
#define UNAUTH_FW_SIZE                      0xFFFFFFFF


extern void* OSAL_Malloc(size_t size);
extern void OSAL_Free(void* pData);


const void * GetTopologies(void);
const void * GetKeysSupported(void);

#endif


