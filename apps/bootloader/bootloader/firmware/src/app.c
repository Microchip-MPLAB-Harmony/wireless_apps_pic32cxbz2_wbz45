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
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "config.h"
#include "mem_interface.h"
#include "dfu/include/progexec.h"
#include "dfu/include/uart.h"
#include "wolfcrypt/ecc.h"                // SYS function prototypes
#include "wolfcrypt/sha256.h"
#include "application.h"
#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* Authentication Method Enum Definition*/
typedef enum
{
  /* No Authentication */
  Auth_none = 0,
  /* SHA256 Authentication */
  Auth_sha256,
  /* ECDSA256 Authentication */
  Auth_ecdsa256
} AUTHEN_METHOD;

/** MetaData Header Definition **/
typedef struct __attribute__((__packed__)) __META_HEADER
{
  /*** meta data header ***/
  /* Reserved */
  uint32_t  filler1;
  uint32_t  filler2;
  /* Manufacturer Identifier */
  char      identifier[4];
  uint32_t  filler3;
  /* Sequence Number */
  uint32_t  seq_num;
  /* MetaData Revision */
  uint8_t   md_rev;
  /* Container Index */
  uint8_t   cont_idx;
  /* MetaData Authentication Method */
  uint8_t   md_auth_mthd;
  /* MetaData Authentication Key */
  uint8_t   md_auth_key;
  /* Payload Decryption Method */
  uint8_t   pl_dec_mthd;
  /* Payload Decryption Key */
  uint8_t   pl_dec_key;
  /* Payload Length */
  uint16_t  pl_len;

  /*** meta data payload = firmware image header ***/
  /* Firmware Image Revision */
  uint32_t  fw_img_rev;
  /* Firmware Image Length */
  uint32_t  fw_img_len;
  /* Firmware Image Authentication Method */
  uint8_t   fw_img_auth_mthd;
  /* Firmware Image Authentication Key */
  uint8_t   fw_img_auth_key;
  /* Firmware Image Decryption method */
  uint8_t   fw_img_dec_mthd;
  /* Firmware Image Decryption Key */
  uint8_t   fw_img_dec_key;
  /* Firmware Image Signature size */
  uint8_t   fw_img_sinature_size;
  /* Firmware Image Signature */
  uint8_t   fw_img_signature[72];

  /* Reserved */
  uint8_t   filler[326];
  /* MetaData Image Signature size */
  uint8_t   md_signature_size;
  /* MetaData Image Signature */
  uint8_t   md_signature[72];
} META_HEADER;
typedef void (*p_function)(void);


volatile uint8_t no_printout = 0;
/**
*@brief This function writes the given data to uart.
*
*@param buf  - pointer where holds the data to be written into uart
*       size - number of bytes to write.
*        
*@retval None
*/
void print_buf(uint8_t *buf, const size_t size)
{
  if( 0 == no_printout)
    SERCOM0_USART_Write(buf, size);
}

/**
*@brief This function reverses the given data.
*
*@param data  - pointer where holds the data to be reversed
*       sz - number of bytes to reverse.
*        
*@retval None
*/
void reverse_order(uint8_t *data, uint8_t sz)
{
  uint8_t i;
  uint8_t tmp;
  
  for(i = 0; i < sz/2; i++)
  {
    tmp = data[sz-1-i];
    data[sz-1-i] = data[i];
    data[i] = tmp;
  }
}

/**
*@brief This function prints the slot number.
*
*@param slot- slot number.
*        
*@retval None
*/
void print_slot(uint8_t slot)
{
  if( slot == 0 )
  {
    print_buf((uint8_t *)"Slot0 ", 6);
  }
  else if( slot == 1 )
  {
    print_buf((uint8_t *)"Slot1 ", 6);
  }
}

/**
*@brief This function validates the signature.
*
*@param image_start_address  - start address of the image
*       memory_holder  - MetaData Header information
*        
*@retval UNAUTH_FW_SIZE or 0 - if authentication failed, total image size is returned if authentication success
*/
uint32_t check_signature_ram(uint32_t image_start_address, uint8_t *memory_holder)
{
  int           ret;
  
  uint8_t       public_key[] =   {0xc2,0x81,0x8f,0xbb,0x28,0x61,0x47,0x8b,0xa2,0x53,0x37,0x79,0xd4,0x63,0x18,0x7c,0x8b,0x41,0x59,0xa9,0x5f,0x0b,0x6b,0x94,0x4e,0xb9,0x57,0xa1,0x03,0xfe,0x20,0xbf,0x2b,0xb8,0x14,0x2a,0x64,0xb5,0xae,0x4a,0x83,0x80,0xdd,0xe6,0xee,0x29,0x89,0xdd,0xa0,0x9a,0xc7,0xda,0x82,0xeb,0x56,0x62,0x90,0x5d,0x66,0xc5,0xbc,0x30,0x3c,0x84};
  int verifyStatus = 0;
  uint8_t *hash_value;
  wc_Sha256 *sha256;
  META_HEADER *firmware_header = (META_HEADER *)image_start_address;
  META_HEADER *firmware_header_ram;
  ecc_key *userB;
  CRYPT_RNG_CTX     *rng;
  uint8_t *input;
  uint32_t count;
  uint8_t slot =2 ;
  uint8_t *ptr;
  const uint8_t expected_header[28] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                               0x00,0x00,0x00,0x00,0x02,0x01,0x00,0x00,
                               0x00,0x00,0x55,0x00};
                               
  if( image_start_address == SLOT0_HEADER )
  {
    slot = 0;
  }
  else if( image_start_address == SLOT1_HEADER )
  {
    slot = 1;
  }
  /* If the sequence number is invalid, return failure*/
  if( (firmware_header->seq_num == 0) || (firmware_header->seq_num == 0xFFFFFFFF) )
  {
    return 0;
  }
  
  memcpy((void *)memory_holder, (void *)image_start_address, METADATA_HEADER_SIZE);
  firmware_header_ram = (META_HEADER *)memory_holder;
  userB = (ecc_key *)&memory_holder[METADATA_HEADER_SIZE];
  rng = (CRYPT_RNG_CTX *)&memory_holder[METADATA_HEADER_SIZE + sizeof(ecc_key)];
  sha256 = (wc_Sha256 *)&memory_holder[METADATA_HEADER_SIZE + sizeof(ecc_key) + sizeof(CRYPT_RNG_CTX)];
  hash_value = (uint8_t *)&memory_holder[METADATA_HEADER_SIZE + sizeof(ecc_key) + sizeof(CRYPT_RNG_CTX) + sizeof(wc_Sha256)];

  memset(firmware_header_ram->identifier, 0, 4);
  firmware_header_ram->seq_num = 0;
  firmware_header_ram->md_auth_mthd = 0;
  ptr = (uint8_t *)firmware_header_ram;

  /* Compared the firmware header with the expected header */
  if( memcmp(ptr, expected_header, 28) != 0 )
  {
    return 0;
  }

  /* CRYPTO Initialization */
  CRYPT_WCCB_Initialize();
  
  reverse_order(public_key, 32);
  reverse_order(&public_key[32], 32);
  /* check meta-data header */
  if( firmware_header->md_auth_mthd > Auth_none )
  {
    wc_InitSha256(sha256);
    memcpy((void *)firmware_header_ram, firmware_header, 512);
    firmware_header_ram->md_signature_size = 0;
    memset(firmware_header_ram->md_signature, 0, 72);
    
    ptr = (uint8_t *)firmware_header_ram;

    wc_Sha256Update(sha256, (const byte *)&ptr[20], (512-20));
    wc_Sha256Final(sha256, hash_value);
    wc_Sha256Free(sha256);
    if( firmware_header->md_auth_mthd == Auth_sha256 )
    {
      print_slot(slot);
      if( 0 != memcmp(hash_value, firmware_header->md_signature, WC_SHA256_DIGEST_SIZE) )
      {
        print_buf((uint8_t *)"Mheader sha256 Failed!\r\n", 24);
        return UNAUTH_FW_SIZE;
      }
      else
      {
        print_buf((uint8_t *)"Mheader sha256 Verified!\r\n", 26);
      }
    }
    if( firmware_header->md_auth_mthd == Auth_ecdsa256 )
    {
      /* Perform ECC HASH Verification */
      wc_ecc_init_ex(userB, 0, 0);
      wc_ecc_make_key_ex((struct RNG *)rng, 32, userB, ECC_SECP256R1);

      memcpy(userB->pubkey.x[0].dp, public_key, 32);
      userB->pubkey.x[0].used = 8;
      userB->pubkey.x[0].sign = FP_ZPOS;
      memcpy(userB->pubkey.y[0].dp, &public_key[32], 32);
      userB->pubkey.y[0].used = 8;
      userB->pubkey.y[0].sign = FP_ZPOS;
      userB->pubkey.z[0].used = 1;
      userB->pubkey.z[0].dp[0] = 1;
      ret = wc_ecc_verify_hash(firmware_header->md_signature, firmware_header->md_signature_size, hash_value, WC_SHA256_DIGEST_SIZE, &verifyStatus, userB);
      print_slot(slot);
      /* Print Verified if ECC verification is success, print failed otherwise */
      if( ret == 0 )
      {
        print_buf((uint8_t *)"Metaheader ECDSA Verified!\r\n", 28);
      }    
      else
      {
        print_buf((uint8_t *)"Metaheader ECDSA Failed!\r\n", 26);
        return UNAUTH_FW_SIZE;
      }
    }
  }
  
  /* check firmware signature */
  if( firmware_header->fw_img_auth_mthd != Auth_none )
  {
    /* Wolfcrypt initialization for Sha256 */
    wc_InitSha256(sha256);
    input = (uint8_t *)(image_start_address + 0x200);
    wc_Sha256Update(sha256, input, firmware_header->fw_img_len);
    wc_Sha256Final(sha256, hash_value);
    if( firmware_header->fw_img_auth_mthd == Auth_sha256 )
    {
      print_slot(slot);
      /* Compare the Hash value with the Firmware image signature in firmware header */
      if( 0 != memcmp(hash_value, firmware_header->fw_img_signature, WC_SHA256_DIGEST_SIZE) )
      {
        print_buf((uint8_t *)"FW sha256 Failed!\r\n", 19);
        return UNAUTH_FW_SIZE;
      }
      else
      {
        print_buf((uint8_t *)"FW sha256 Verified!\r\n", 21);
      }
    }
    if( firmware_header->fw_img_auth_mthd == Auth_ecdsa256 )
    {
      /* ECC HASH verification */
      wc_ecc_init_ex(userB, 0, 0);
      wc_ecc_make_key_ex((struct RNG *)rng, 32, userB, ECC_SECP256R1);

      memcpy(userB->pubkey.x[0].dp, public_key, 32);
      userB->pubkey.x[0].used = 8;
      userB->pubkey.x[0].sign = FP_ZPOS;
      memcpy(userB->pubkey.y[0].dp, &public_key[32], 32);
      userB->pubkey.y[0].used = 8;
      userB->pubkey.y[0].sign = FP_ZPOS;
      userB->pubkey.z[0].used = 1;
      userB->pubkey.z[0].dp[0] = 1;

      ret = wc_ecc_verify_hash(firmware_header->fw_img_signature, firmware_header->fw_img_sinature_size, hash_value, WC_SHA256_DIGEST_SIZE, &verifyStatus, userB);
      print_slot(slot);
      if( ret == 0 )
      {
        print_buf((uint8_t *)"FW ECDSA Verified!\r\n", 20);
      }    
      else
      {
        print_buf((uint8_t *)"FW ECDSA Failed!\r\n", 18);
        return UNAUTH_FW_SIZE;
      }
    }
  }
  // delay to finish the uart output
  for(count = 0; count < 0x5ffff; count++);
  // return total image size = fw size + header size
  return (firmware_header->fw_img_len+METADATA_HEADER_SIZE);
}

/**
*@brief This function reads the firmware header and checks the signature.
*
*@param image_start_address  - start address of the image
*        
*@retval UNAUTH_FW_SIZE - if authentication failed, total image size is returned if authentication success
*/
uint32_t check_signature(uint32_t image_start_address)
{
  uint8_t ram_holder[3244];
  
  /* Check the signature and return the desired value */
  return check_signature_ram(image_start_address, ram_holder);
}


/**
*@brief This function authenticates the firmware image.
*
*@param slot  - slot number
*       ram_ptr - slot header information
*        
*@retval returns true if success, false if failed
*/
__attribute__((keep, externally_visible, address(0x5000))) bool authenticate_fw_image(uint8_t slot, uint8_t *ram_ptr)
{
  uint32_t ret=0;
  uint32_t pmd3_reg = CFG_REGS->CFG_PMD3;
  
  CFG_REGS->CFG_PMD3 = (pmd3_reg & (~(CFG_PMD3_PUKCCMD_Msk | CFG_PMD3_ICMMD_Msk)));
  /* Validate the signature */
  if( slot == 0 )
  {
    ret = check_signature_ram(SLOT0_HEADER, ram_ptr);
  }
  else if( slot == 1 )
  {
    ret = check_signature_ram(SLOT1_HEADER, ram_ptr);
  }
  CFG_REGS->CFG_PMD3 = pmd3_reg;
  /* return false if failure in authentication/validation */
  if( (ret == 0) || (ret == UNAUTH_FW_SIZE ))
  {
    return false;
  }
  /* return true if the validation is successful */
  return true;
}

/**
*@brief This function populates the DEVICE_CONTEXT structure with the valid topologies for the 
* device and the count of valid topologies.
*
*@param ctx  - device context
*        
*@retval None
*/
void IMG_MEM_FindValidTopologies(DEVICE_CONTEXT * ctx)
{
    uint32_t i = 0;
    uint32_t j = 0;
    IMG_MEM_TOPOLOGY * imgMems = (IMG_MEM_TOPOLOGY *)GetTopologies();

    ctx->topologyCount = 0x00;
    /* Iterate and count the valid number of topologies */
    for (i = 0; i < IMG_MEM_TOPOLOGY_COUNT; i++)
    {
      if (j == MAX_MEM_TOPOLOGIES)
      {
          critical_error(BOOT_ERROR_CRITICAL_FAILURE);
          critical_error(BOOT_ERROR_CRITICAL_FAILURE);
      }
      ctx->validTops[j++] = &imgMems[i];
      ctx->topologyCount++;
    }
}

/**
*@brief This function starts the device firmware upgrade.
*
*@param None
*        
*@retval None
*/
void start_dfu(void)
{
  DEVICE_CONTEXT ctx;

  /*Find the valid topologies and number of valid topologies */
  IMG_MEM_FindValidTopologies(&ctx);
  /* If valid topologies available, proceed with DFU sequence*/
  if (ctx.validTops[0] != NULL)
  {
    dfu(ctx.validTops, ctx.topologyCount);
  }
}

/**
*@brief This function copies the image from slot 2 to slot 1.
*
*@param img_size  - size of the image to copy
*        
*@retval None
*/
void copy_img(uint32_t img_size)
{
  DEVICE_CONTEXT ctx;
  uint32_t dest_addr = SLOT0_BASE_ADDR;
  uint32_t src_addr =  SLOT1_BASE_ADDR;
  uint16_t pageCount;
  uint8_t *src_data = (uint8_t *)src_addr;
  uint8_t hash_value1[WC_SHA256_DIGEST_SIZE], hash_value2[WC_SHA256_DIGEST_SIZE];
  wc_Sha256 sha256;

  /*Find the valid topologies and number of valid topologies */
  IMG_MEM_FindValidTopologies(&ctx);

  /* If valid topologies available, proceed with copying image */
  if( ctx.validTops[0] != NULL )
  {
    src_addr = ((src_addr | ctx.validTops[0]->u32AddrPosMask) & ctx.validTops[0]->u32AddrNegMask);
    if( img_size > ctx.validTops[0]->u32TotSize )
    {
      // erase slot 1
      if( 0 != ctx.validTops[0]->ifFlash->fErase(src_addr, 1) )
      {
        while(1);
      }
      print_buf((uint8_t *)"Slot1 Erased\r\n", 14); 
      return;
    }
    
    // erase slot 0
    dest_addr = ((dest_addr | ctx.validTops[0]->u32AddrPosMask) & ctx.validTops[0]->u32AddrNegMask);
    pageCount = img_size/ctx.validTops[0]->u16ErasePageSz;
    if( (img_size % ctx.validTops[0]->u16ErasePageSz) > 0 )
    {
      pageCount++;
    }
    if (ctx.validTops[0]->ifFlash && ctx.validTops[0]->ifFlash->fErase)
    {
        if( 0 != ctx.validTops[0]->ifFlash->fErase(dest_addr, pageCount) )
        {
          while(1);
        }
    }
    print_buf((uint8_t *)"Slot0 Erased\r\n", 14);

    /* copy bank 2 to bank 1 */
    if (ctx.validTops[0]->ifFlash && ctx.validTops[0]->ifFlash->fWrite)
    {
        if (0 != ctx.validTops[0]->ifFlash->fWrite(src_data, dest_addr, img_size) )
        {
          while(1);
        }
    }
    print_buf((uint8_t *)"Slot1 Copied to Slot0\r\n", 23);

    // verify the slot0 is equal to slot1
    wc_InitSha256(&sha256);
    wc_Sha256Update(&sha256, (uint8_t *)SLOT0_BASE_ADDR, img_size);
    wc_Sha256Final(&sha256, hash_value1);
    
    wc_InitSha256(&sha256);
    wc_Sha256Update(&sha256, (uint8_t *)SLOT1_BASE_ADDR, img_size);
    wc_Sha256Final(&sha256, hash_value2);
    
    if( memcmp(hash_value1, hash_value2, WC_SHA256_DIGEST_SIZE) == 0 )
    {
      print_buf((uint8_t *)"Copy Verified\r\n", 15);
      if( 0 != ctx.validTops[0]->ifFlash->fErase(src_addr, pageCount) )
      {
        while(1);
      }
      print_buf((uint8_t *)"Slot1 Erased\r\n", 14); 
    }
    else
    {
      print_buf((uint8_t *)"Copy Differ\r\n", 13); 
    }
  }
}


/**
*@brief This function deinitializes the SERCOM UART.
*
*@param None
*        
*@retval None
*/
void SERCOM0_USART_deInit(void)
{
  SERCOM0_REGS->USART_INT.SERCOM_CTRLA = 0;
  SERCOM0_REGS->USART_INT.SERCOM_BAUD = 0;
  SERCOM0_REGS->USART_INT.SERCOM_CTRLB = 0;
  
  /* Wait for sync */
  while((SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
  {
      /* Do nothing */
  }
  
  SERCOM0_REGS->USART_INT.SERCOM_INTENCLR = 0xBF;
  SERCOM0_REGS->USART_INT.SERCOM_INTFLAG = 0;  
}

/**
*@brief This function deinitializes the NVIC.
*
*@param None
*        
*@retval None
*/
void NVIC_deInit(void)
{
  NVIC_DisableIRQ(FLASH_CONTROL_IRQn);
  NVIC_DisableIRQ(SERCOM0_IRQn);
  NVIC_DisableIRQ(TC3_IRQn);
}

/**
*@brief This function clears the Watchdog counter so that watchdog reset won't occur.
*
*@param None
*        
*@retval None
*/
static void clear_watchdog_counter(void )
{
  /* Writing specific value to only upper 16 bits of WDTCON register clears WDT counter
     Only write to the upper 16 bits of the register when clearing.
     WDTCLRKEY = 0x5743 */
  volatile uint16_t * wdtclrkey;
  wdtclrkey = ( (volatile uint16_t *)&WDT_REGS->WDT_WDTCON ) + 1;
  *wdtclrkey = 0x5743;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
}

/**
*@brief This function checks the image in slot2 if available copies it
        if image is not available, then it checks image in slot 1.
*
*@param None
*        
*@retval None
*/
void check_for_image(void)
{
    uint32_t count;
    uint32_t img_size;
    uint8_t i;
    bool isWDTEnabled = ((CFG_REGS->CFG_CFGCON2 & CFG_CFGCON2_WDTEN_Msk) == CFG_CFGCON2_WDTEN_Msk ? true : false);

    /* Check Image in Slot 2*/
    img_size = check_signature(SLOT1_HEADER);
    if( img_size > 0 )
    {
      if( (img_size == UNAUTH_FW_SIZE ))
      {
        print_buf((uint8_t *)"Invalid Slot1 Image\r\n", 21);
      }
    /* If valid image size, then copy the image to slot 1*/
      copy_img(img_size);
    }
    /* Check Image in Slot 1*/
    img_size = check_signature(SLOT0_HEADER);
    if( (0 == img_size) || (UNAUTH_FW_SIZE == img_size))
    {
      print_buf((uint8_t *)"Invalid Slot0 Image\r\n", 21);
      for(count = 0; count < 0x5ffff; count++);
      while(1)
      {
        if (isWDTEnabled)
        {
          /*Clear watchdog counter so that watchdog reset won't get triggered */           
          clear_watchdog_counter();
        }
      }
    }
    for(count = 0; count < 0x5ffff; count++);
    /* deinit all peripheral settings before jumping to user application */
    SERCOM0_USART_deInit();
    NVIC_deInit();
    for(count = 0; count < 0x5ffff; count++);
    /* stop all interrupts */
    __disable_irq();

    /* Disable IRQs */
    for(i = 0;i < 8;i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
    }
    /* Clear pending IRQs */
    for(i = 0;i < 8;i++)
    {
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* enable all interrupts */
    __enable_irq();

    __set_CONTROL(0);   

    /* re-init stack pointer (first entry of the vector table) */
    __set_MSP(*(__IO uint32_t*)SLOT0_FIRMWARE);

    no_printout = 1;
    uint32_t jump_address = *(__IO uint32_t*)(SLOT0_FIRMWARE + 4);
    /* Assign Slot1 firmware address to function pointer */
    p_function p_jump_function = (p_function)jump_address;

    /* Jump to the application */
    p_jump_function();
}


/**
*@brief This function runs the bootloader functionality
*
*@param None
*        
*@retval None
*/
void bootloader_functions(void)
{
  uint32_t count;
  uint32_t portRef;
  uint32_t pval;
  bool isWDTEnabled = ((CFG_REGS->CFG_CFGCON2 & CFG_CFGCON2_WDTEN_Msk) == CFG_CFGCON2_WDTEN_Msk ? true : false);
  /* Set to Digital mode */
  ((gpio_registers_t*)GPIO_PORT_B)->GPIO_ANSELCLR = (1 << 4);

  /* Enable SERCOM0 UART fast mode */
  CFG_REGS->CFG_CFGCON1SET |= CFG_CFGCON1_SCOM0_HSEN_Msk;

  /* Read the GPIO Port for Button Press Identification */
  portRef = ((gpio_registers_t*)GPIO_PORT_B)->GPIO_PORT;
  pval = portRef & (1 << 4);
  /* If button is not pressed, check for the image and jump to application 
     If pressed, jump to DFU Mode */
  if( pval )
  {
     check_for_image();
  }
  else
  {
    /* Enters DFU Mode */
    print_buf((uint8_t *)"DFU Now!\r\n", 10);
    for(count = 0; count < 0x2ffff; count++);
    /* Initialize the SERCOM UART for read notification, thershold setting */
    UART_Init();
    /* Runs the Device Firmware Upgrade Mode */
    while(1)
    {
       start_dfu();
       if (isWDTEnabled)
       {
         /*Clear watchdog counter so that watchdog reset won't get triggered */           
         clear_watchdog_counter();
       }
    }
  }
  while(1);
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized)
            {
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            /* Runs the bootloader functionality */
            bootloader_functions();
            break;
        }

        /* The default state should never be executed. */
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
