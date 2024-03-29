/*******************************************************************************
  Wear Leveling PDS memory identificators Header File

  File Name:
   pds_config.h

  Summary:
    This file contains PDS File and Item configuration details.

  Description:
    This file contains PDS File and Item configuration details.
 *******************************************************************************/


/*******************************************************************************
* Copyright (C) [2024], Microchip Technology Inc., and its subsidiaries. All rights reserved.
  
* The software and documentation is provided by Microchip and its contributors 
* "as is" and any express, implied or statutory warranties, including, but not 
* limited to, the implied warranties of merchantability, fitness for a particular 
* purpose and non-infringement of third party intellectual property rights are 
* disclaimed to the fullest extent permitted by law. In no event shall Microchip 
* or its contributors be liable for any direct, indirect, incidental, special,
* exemplary, or consequential damages (including, but not limited to, procurement 
* of substitute goods or services; loss of use, data, or profits; or business 
* interruption) however caused and on any theory of liability, whether in contract, 
* strict liability, or tort (including negligence or otherwise) arising in any way 
* out of the use of the software and documentation, even if advised of the 
* possibility of such damage.
* 
* Except as expressly permitted hereunder and subject to the applicable license terms 
* for any third-party software incorporated in the software and any applicable open 
* source software license terms, no license or other rights, whether express or 
* implied, are granted under any patent or other intellectual property rights of 
* Microchip or any third party.
 *******************************************************************************/


#ifndef _PDS_CONFIG_H
#define _PDS_CONFIG_H

/******************************************************************************
                              Includes section
******************************************************************************/


/******************************************************************************
                              Defines section
******************************************************************************/
/* Following are the hard-coded values of different settings key sizes
 * Note: These values may vary in future. Need to adjust them accordingly
 * TODO: Other than following key sizes, remaining key size are not defined for now.
 */
/* Child Info Table entry size */
#define CHILD_TABLE_ENTRY_SIZE                          17 /*sizeof (Settings::ChildInfo)*/

/* Parent Info Size */
#define PARENT_INFO_SIZE                                10 //sizeof (Settings::ParentInfo)

/* Network Info Size */
#define NWK_INFO_SIZE                                   38 //sizeof (Settings::NetworkInfo)

/* SLAAC Iid Size */
#define SLAAC_IID_SIZE                                  34 //sizeof (Settings::SlaacIidSecretKey)

/* DAD Info size */
#define DAD_INFO_SIZE                                   2 //sizeof (Settings::DadInfo) 

/* On-link Prefix size */
#define BR_ON_LINK_PREFIXES_SIZE                       12 //sizeof (Settings::BrOnLinkPrefix)

/* OMR prefix size */
#define BR_ULA_PREFIX_SIZE                              8 //sizeof (Settings::BrUlaPrefix)

/* SRP Client ECDSA Key size */
#define SRP_CLIENT_ECDSA_KEY_SIZE                       126 //sizeof (Settings::SrpEcdsaKey) 

/* SRP Client Size */
#define SRP_CLIENT_INFO_SIZE                             18 //sizeof (Settings::SrpClientInfo) 

/* SRP Server size */
#define SRP_SERVER_INFO_SIZE                             2 //sizeof (Settings::SrpServerInfo)



/******************************************************************************************************************
 *                       Flash Item to PDS Item Mapping                                                            *
 ******************************************************************************************************************/

typedef enum _pdsItemID {
    PDS_ITEM_ACTIVE_DATASET = PDS_MODULE_OT_OFFSET, ///< Active Operational Dataset.
    PDS_ITEM_PENDING_DATASET,                       ///< Pending Operational Dataset.
    PDS_ITEM_NETWORK_INFO,                          ///< Thread network information.
    PDS_ITEM_PARENT_INFO,                           ///< Parent information.
    PDS_ITEM_CHILD_INFO,                            ///< Child information.
    PDS_ITEM_SLAAC_IID_SECRET_KEY,                  ///< SLAAC key to generate semantically opaque IID.
    PDS_ITEM_DAD_INFO,                              ///< Duplicate Address Detection (DAD) information.
    PDS_ITEM_SRP_ECDSA_KEY,                         ///< Off-mesh routable (OMR) prefix.
    PDS_ITEM_SRP_CLIENT_INFO,                       ///< On-link prefix for infrastructure link.
    PDS_ITEM_SRP_SERVER_INFO,                       ///< SRP client ECDSA public/private key pair.
    PDS_ITEM_BR_ULA_PREFIX,                         ///< The SRP client info (selected SRP server address).
    PDS_ITEM_BR_ON_LINK_PREFIXES,                   ///< The SRP server info (UDP port).
    PDS_ITEM_MAX,                                   /// < Add any new item before max enum value.   
}pdsItemID_t;

/******************************************************************************************************************/
/**
 * @def PDS_ITEM_ACTIVE_DATASET_SZ
 *
 * Length bytes of the key length stored as part of File Item.
 *
 */
#define FILE_ITEM_LEN_SZ                 2

/**
 * @def PDS_ITEM_ACTIVE_DATASET_SZ
 *
 * The maximum size of Active Dataset Item size.
 *
 */
#define PDS_ITEM_ACTIVE_DATASET_SZ         (OT_OPERATIONAL_DATASET_MAX_LENGTH + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_PENDING_DATASET_SZ
 *
 * The maximum size of Pending Dataset Item size.
 *
 */
#define PDS_ITEM_PENDING_DATASET_SZ        (OT_OPERATIONAL_DATASET_MAX_LENGTH + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_NETWORK_INFO_SZ
 *
 * The maximum size of Network info Item size.
 *
 */
#define PDS_ITEM_NETWORK_INFO_SZ           (NWK_INFO_SIZE + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_PARENT_INFO_SZ
 *
 * The maximum size of parent info Item size.
 *
 */
#define PDS_ITEM_PARENT_INFO_SZ            (PARENT_INFO_SIZE + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_CHILD_INFO_SZ
 *
 * The maximum size of Child info Item size.
 * 
 * Warning: There's a Thread API to set/change Max Allowed Children. 
 * If the max value is increased more than OPENTHREAD_CONFIG_MLE_MAX_CHILDREN using the API in application,
 * then PDS cannot accommodate the amount.
 * So, it's possible only to reduce the MAX Children and not to increase.
 * 
 */
#define PDS_ITEM_CHILD_INFO_SZ             (CHILD_TABLE_ENTRY_SIZE + FILE_ITEM_LEN_SZ) 

/**
 * @def PDS_ITEM_SLAAC_IID_SECRET_KEY_SZ
 *
 * The maximum size of SLAAC IID Key Item size.
 *
 */
#define PDS_ITEM_SLAAC_IID_SECRET_KEY_SZ   (SLAAC_IID_SIZE + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_DAD_INFO_SZ
 *
 * The maximum size of DAD info Item size.
 *
 */
#define PDS_ITEM_DAD_INFO_SZ               (DAD_INFO_SIZE + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_BR_ULA_PREFIX_SZ
 *
 * The maximum size of BR ULA prefix Item size.
 *
 */
#define PDS_ITEM_BR_ULA_PREFIX_SZ             (BR_ULA_PREFIX_SIZE + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_BR_ON_LINK_PREFIXES_SZ
 *
 * The maximum size of BR on-link prefix for the adjacent infrastructure link Item size.
 *
 */
#define PDS_ITEM_BR_ON_LINK_PREFIXES_SZ         (BR_ON_LINK_PREFIXES_SIZE + FILE_ITEM_LEN_SZ)


/**
 * @def PDS_ITEM_SRP_ECDSA_KEY_SZ
 *
 * The maximum size of SRP ECDSA KEY Item size.
 *
 */
#define PDS_ITEM_SRP_ECDSA_KEY_SZ          (SRP_CLIENT_ECDSA_KEY_SIZE + FILE_ITEM_LEN_SZ)


/**
 * @def PDS_ITEM_SRP_CLIENT_INFO_SZ
 *
 * The maximum size of SRP Client info Item size.
 *
 */
#define PDS_ITEM_SRP_CLIENT_INFO_SZ        (SRP_CLIENT_INFO_SIZE + FILE_ITEM_LEN_SZ)

/**
 * @def PDS_ITEM_SRP_SERVER_INFO_SZ
 *
 * The maximum size of SRP Server info Item size.
 *
 */
#define PDS_ITEM_SRP_SERVER_INFO_SZ        (SRP_SERVER_INFO_SIZE + FILE_ITEM_LEN_SZ)




const uint16_t item_size [] = 
{
    PDS_ITEM_ACTIVE_DATASET_SZ,
    PDS_ITEM_PENDING_DATASET_SZ,
    PDS_ITEM_NETWORK_INFO_SZ,
    PDS_ITEM_PARENT_INFO_SZ,
    PDS_ITEM_CHILD_INFO_SZ,
    PDS_ITEM_SLAAC_IID_SECRET_KEY_SZ,
    PDS_ITEM_DAD_INFO_SZ,
    PDS_ITEM_SRP_ECDSA_KEY_SZ,
    PDS_ITEM_SRP_CLIENT_INFO_SZ,
    PDS_ITEM_SRP_SERVER_INFO_SZ,
    PDS_ITEM_BR_ULA_PREFIX_SZ,
    PDS_ITEM_BR_ON_LINK_PREFIXES_SZ
};
/******************************************************************************************************************/


// *****************************************************************************
/**
*@brief This routine Restores all Items from Persistent Data Server
*
*@param None
*
*@retval None
*/
void flashRestoreItemsAll(void);

// *****************************************************************************
/**
*@brief This routine Stores the item with offset
*
*@param item - PDS_Operation_Offset_t type, sends the Item with offset details
*
*@retval bool
*/
extern bool PDS_storeOffset(PDS_Operation_Offset_t *item);


uint16_t getOffsetfromItemid (pdsItemID_t itemId);

pdsItemID_t getPdsItemidfromKeyid (uint16_t key);

#endif