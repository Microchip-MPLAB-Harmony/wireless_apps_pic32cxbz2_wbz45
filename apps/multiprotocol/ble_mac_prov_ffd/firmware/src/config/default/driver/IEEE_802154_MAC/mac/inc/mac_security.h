/*******************************************************************************
    IEEE MAC Security 

  File Name:
    mac_security.h
 Summary:
    MAC security definitions and macros
  Description:
    This header holds MAC security related definitions and macros.

 *******************************************************************************/

//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#ifndef MAC_SECURITY_H
#define MAC_SECURITY_H

#if ((defined MAC_SECURITY_ZIP)  || (defined MAC_SECURITY_2006))

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include "mac_api.h"

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

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB macKeyTableEntries
   Description:
	None
   Remarks:
    None 
 */
        
#define macKeyTableEntriesDef              (0)

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB macDeviceTableEntries
   Description:
	None
   Remarks:
    None 
 */

#define macDeviceTableDef                  (0)

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB macSecurityLevelTableEntries
   Description:
	None
   Remarks:
    None 
 */

#define macSecurityLevelTableDef           (0)

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB macFrameCounter
   Description:
	None
   Remarks:
    None 
 */        

#define macFrameCounterDef                 (0x00000000)

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB macDefaultKeySource
   Description:
	None
   Remarks:
    None 
 */  
        
#define macDefaultKeySourceDef             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, \
					     0xFF, 0xFF}

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB KeyIdLookupListEntries
   Description:
	None
   Remarks:
    None 
 */  

#define KeyIdLookupListEntriesDef          (0)

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB KeyDeviceListEntries
   Description:
	None
   Remarks:
    None 
 */  

#define KeyDeviceListEntriesDef            (0)

// *****************************************************************************
/* 

   Summary:
    This macro holds Default value for PIB KeyUsageListEntries
   Description:
	None
   Remarks:
    None 
 */

#define KeyUsageListEntriesDef             (0)

// *****************************************************************************
// *****************************************************************************
// Section: Externals
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* externals
   Summary:
    Holds the values of all security related PIB attributes
   Description:
    None
   Remarks:
    None 
 */
        
extern MAC_SecPib_t macSecPib;

// *****************************************************************************
// *****************************************************************************
// Section: Prototypes
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*
  Function:
    uint8_t GetKeyIdFieldLen(uint8_t keyIdMode);

  Summary:
    Gets the length of the Key Identifier field

  Description:
    This function returns the length of the Key Identifier field
    within the Auxiliary Security Header of a secured frame based
    on the Key Identifier Mode.
 
  Precondition:
    None

  Parameters:
    keyIdMode    -   Key Identifier mode used to identify the key to be used                  

  Returns:
    Returns Length of Key Identifier field in octets

  Remarks:
    None 
*/

uint8_t GetKeyIdFieldLen(uint8_t keyIdMode);

// *****************************************************************************
/*
  Function:
    bool BuildSecMcpsDataFrame(MCPS_DataReq_t *mpdr, MAC_FrameInfo_t *mframe);

  Summary:
   Build the Security MCPS Data Request frame from the mpdu data

  Description:
    This function will extract the MAC Frame into individual mac parameters.
 
  Precondition:
    None

  Parameters:
    mpdr    -   mac payload pointer used to identify the payload location
    mframe  -   phy frame info used to get the mac frame info                 

  Returns:
    Return true if build the Security MCPS Data Request frame from the mpdu data
    otherwise false

  Remarks:
    None 
*/
bool BuildSecMcpsDataFrame(MCPS_DataReq_t *mpdr, MAC_FrameInfo_t *mframe);

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif  /* (MAC_SECURITY_ZIP || MAC_SECURITY_2006) */

#endif /* MAC_SECURITY_H */
/* EOF */
