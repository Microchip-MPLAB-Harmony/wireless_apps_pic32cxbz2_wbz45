/*******************************************************************************
  PHY PIC32CX Header File

  Company:
    Microchip Technology Inc.

  File Name:
    phyPic32cx.h

  Summary:
    This file provides the interface for PHY register access.

  Description:
    This file provides the interface for PHY register access.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef PHYPIC32CX_H_
#define PHYPIC32CX_H_

/******************************************************************************
                        Includes section
******************************************************************************/
#include <inttypes.h>
#include <systemenvironment/include/sysTypes.h>
#include <mac_phy/mac_hwd_phy/RF231_RF212/PHY/include/at86rf233.h>
#include <xc.h>

#ifdef _RF_AES_
#include <halRfCtrl.h>
#include <mac_phy/include/RF231_RF212/PHY/include/phyEncrypt.h>
#endif


/******************************************************************************
                        Define(s) section
******************************************************************************/

#define TRX_REQ_IDLE 2
#define TRX_REQ_RX 0
#define TRX_REQ_TX 1
#define TRX_REQ_RX_ONLY 3
#define TIME_SENSITIVE 1
#define DELAY_OK    0
#define TX_PRIORITY_ARB_SET 0x01  // For now , will change based on the Sys Req
#define RX_PRIORITY_ARB_SET 0x01  // For now , will change based on the Sys Req
#define BACKOFF_TX_FAILURE  100

//Incremental Priorities
#define RX_SYNC_TO_PACKET_SYNC_PRIORITY           0x00
#define PACKET_SYNC_TO_ADDR_MATCH_PRIORITY        0x00
#define ADDR_MATCH_TO_PACKET_RX_PRIORITY          0x03
#define PACKET_PACKET_RX_TO_TX_ACK_PRIORITY       0x03 //0x03
#define PACKET_CCA_PRIORITY                       0x00
#define PACKET_RECEPTION_OF_ACK_CCA_PRIORITY      0x03 //0x03
#define HW_RETRY_GRANT_TIMEOUT_PRIORITY           0x00
#define TX_ARET_IN_CASE_OF_GRANT_TIMEOUT_PRIORITY 0x00

//Time required - EarlyZigbee
#define RF_REQ_ON_TIME_DELTA 36 //12 + 4+20

#define TX_RETRY_COUNT   4

#define ARB_ZB_RF233_MODE     0x00
#define ARB_ZB_STATIC_MODE    0x01
#define ARB_ZB_DYNAMIC_MODE   0x02

#define phyReadFrameInline(x,y) phyReadFrame(x,y)
#define phyReadRegisterInline(x) phyReadRegister(x)
#define phyWriteRegisterInline(x,y) phyWriteRegister(x,y)

//#define x_asm_delay(X)                             do{asm("nop")}while(--X)
//#define HAL_Delay                                  x_asm_delay
#define RF_Control_Register_RF_CTRL_ll_addr    (BT_BASEADDR + 0x1000U)
#define RF_control_register_rf_ctrl_2_hl_addr  (BT_BASEADDR + 0x1002U)
#define RF_programming_register_rfprog_hl_addr (BT_BASEADDR + 0x1006U)
#define RF_ana_ip_ctrl_reg2_hl_addr               (BT_BASEADDR + 0x04baU)
#define RF_SiWData_ll_addr                        (BT_BASEADDR + 0x1028U)

/******************************************************************************
                        Prototypes section
******************************************************************************/
INLINE void z_reg_write32(uint32_t addr, uint32_t data);
INLINE void z_reg_write16(uint32_t addr, uint16_t data);
INLINE void z_reg_write8(uint32_t addr, uint8_t data);
INLINE void z_reg_write32_slice_RF(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value);
INLINE void z_reg_write32_slice(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value);
INLINE uint32_t z_readReg32_slice(uint32_t addr, uint32_t msb, uint32_t lsb);
INLINE void z_reg_write16_slice(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value);
INLINE uint32_t z_readReg16_slice(uint32_t addr, uint32_t msb, uint32_t lsb);
INLINE void z_reg_write8_slice(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value);
INLINE void Word_Reg_Byte_Write_Z(uint32_t regAddr, uint32_t offset/*0,1,2,3*/, uint8_t data);
INLINE uint32_t z_readReg8_slice(uint32_t addr, uint32_t msb, uint32_t lsb);
INLINE uint32_t z_readReg8_slice(uint32_t addr, uint32_t msb, uint32_t lsb);
INLINE uint32_t z_readReg32(uint32_t addr);
INLINE uint16_t z_readReg16(uint32_t addr);
INLINE uint8_t z_readReg8(uint32_t addr);
INLINE void z_do_bitwr8(uint32_t address, uint8_t mask, uint8_t val);
INLINE uint16_t z_do_bitrd16(uint32_t address, uint16_t mask);
INLINE uint32_t z_do_bitrd32(uint32_t address, uint32_t mask);
INLINE uint8_t z_do_bitrd8(uint32_t address, uint8_t mask);
INLINE void z_do_bitwr32(uint32_t address, uint32_t mask, uint32_t val);
INLINE uint32_t z_readReg8_slice(uint32_t addr, uint32_t msb, uint32_t lsb);
INLINE uint32_t z_readReg8_slice(uint32_t addr, uint32_t msb, uint32_t lsb);


/******************************************************************************
  System Reset Functionality
******************************************************************************/
INLINE void SystemReset(void)
{
#ifdef __FPGA__
  NVIC_SystemReset();
#else
  uint32_t temp;
  CFG_REGS->CFG_SYSKEY = 0x00000000; // Write junk to lock it if it is already unlocked
  CFG_REGS->CFG_SYSKEY = 0xAA996655U;
  CFG_REGS->CFG_SYSKEY = 0x556699AA;
  
  // Trigger Software Reset
  RCON_REGS->RCON_RSWRST = 0x1;

  // The read will trigger the reset to occur
  temp = RCON_REGS->RCON_RSWRST;
  (void)temp;
  // 4 NOP's to insure no other instructions get executed
  __NOP();
  __NOP();
  __NOP();
  __NOP();
#endif
}

/******************************************************************************
  Writes AT86RF2XX register.
  parameters: addr - register address
              value - data to be written.
******************************************************************************/
INLINE void phyWriteRegister(uint8_t addr, uint8_t value)
{
  *((volatile uint8_t *) (ZB_BASEADDR + addr)) = value;
}

/******************************************************************************
  Reads AT86RF2XX register.
  parameters: addr - register address.
  returnes:   value which was read.
******************************************************************************/
INLINE uint8_t phyReadRegister(uint8_t addr)
{
  return  (uint8_t) *((volatile uint8_t *) (ZB_BASEADDR + addr));
}

/******************************************************************************
  Reads data from AT86RF23X buffer.
  Parameters: buf - pointer to the data buffer.
              length - number of bytes to be read out.
******************************************************************************/
INLINE void phyWriteFrame(uint8_t* buf, uint8_t length)
{
  static uint8_t mod_size;
  static uint8_t total_len;
  static uint32_t write_data;
  static uint32_t address;
  static uint8_t k;

  // mod_size are the number of bytes initialized to 0 after frame data
  mod_size = (uint8_t)(length % 4U);

  // total_length is in multiple of 4
  if (mod_size !=0U)
  {
    total_len  = (uint8_t)(length + 4U - mod_size);
  }
  else
  {
    total_len  = length;
  }
  address = ZB_TX_FRAME_BUFFER_ADDR;
  // Upload frame to TX BUFFER
  for (k = 0 ; k < total_len; k+=4U, address+=4U)
  {
    if (k != (length - mod_size))
    {
      write_data = *(buf + k) | (((uint32_t)*(buf + k + 1)) << 8) | (((uint32_t)*(buf + k + 2)) << 16) | (((uint32_t)*(buf + k + 3)) << 24) ; 
    }
    else
    {
      write_data = 0;

      if(mod_size > 0U)
      {
        write_data  = *(buf + k);
      }
      if(mod_size > 1U)
      {
        write_data = write_data | (((uint32_t)*(buf + k + 1)) << 8);
      }
      if (mod_size > 2U)
      {
        write_data = write_data | (((uint32_t)*(buf + k + 2)) << 16) ; 
      }
    }
    z_reg_write32(address, write_data);
  }
}

/******************************************************************************
  Reads data from AT86RF23X buffer.
  Parameters: buf - pointer to the data buffer.
              length - number of bytes to be read out.
******************************************************************************/
INLINE void phyReadFrame(uint8_t* buf, uint8_t length)
{
  uint32_t total_len, mod_size, read_data, address , k , mask;

#if 0
  uint32_t reg_phr,psdu_len,read_data_hdr;
  // Read PHR value
  read_data_hdr  = x_readReg32(ZB_RX_FRAME_BUFFER_ADDR);
  reg_phr        = read_data_hdr & 0x000000FF;

  // RX buffer contains PHR + PSDU data
  psdu_len       = (reg_phr & 0x7F) + 1; //to account for PHR
#endif
  
  // mod_size are the number of bytes that must be read to make total read bytes in multiple of 4
  mod_size = (uint8_t)(length % 4U);

  // Set mask for the last read based on mod_size (mask uninitialized rx buffer locations with 0)
  if (mod_size == 0U)
  {
      mask = 0xFFFFFFFFU;
  }
  else if (mod_size == 1U)
  {
      mask = 0x000000FF;
  }
  else if (mod_size == 2U)
  {
      mask = 0x0000FFFF;
  }
  else 
  {
      mask = 0x00FFFFFF;
  }
  
  (void)mask;
  if (mod_size !=0U)
  {
    total_len = (uint8_t)(length + 4U) - mod_size; 
  }
  else  
  {
    total_len   = length;
  }
  address = ZB_RX_FRAME_BUFFER_ADDR;
  // Read RX BUFFER for PHR + PSDU data (Last rx buffer read is done with mask)
  for (k = 0; k < (total_len); k+=4U, address+=4U) 
  {
    if (k != (length - mod_size))
    {
      read_data = z_readReg32(address);

      *(buf+k)  = (uint8_t)(read_data & 0x000000FFU);
      *(buf+k+1) = (uint8_t)((read_data & 0x0000FF00U)>>8);
      *(buf+k+2) = (uint8_t)((read_data & 0x00FF0000U)>>16);
      *(buf+k+3) = (uint8_t)((read_data & 0xFF000000U)>>24);

	}
    else
    {
      read_data = z_readReg32(address);
	  if(mod_size > 0U)
      {
  	    *(buf+k)  = (uint8_t)(read_data & 0x000000FFU);
      }
	  if(mod_size > 1U)
      {
        *(buf+k+1) = (uint8_t)((read_data & 0x0000FF00U)>>8);
      }
  	  if(mod_size > 2U)
      {
	  	*(buf+k+2) = (uint8_t)((read_data & 0x00FF0000U)>>16);
      }
    }
  }

  // Append ED, LQI and RX_STATUS at the end of RX_BUFFER data
  //*(buf+length)     = (read_data_hdr & 0x0000FF00)>>8;
  //*(buf+length +1)    = (read_data_hdr & 0x00FF0000)>>16;
  //*(buf+length +2)    = (read_data_hdr & 0xFF000000)>>24;
  
}

INLINE void  phyReadSramInline(uint8_t* buf, uint8_t addressOffset, uint8_t length)
{
  uint32_t total_len, mod_size, read_data, address , k , mask;

#if 0
   uint32_t reg_phr,psdu_len,read_data_hdr;
  // Read PHR value
  read_data_hdr  = x_readReg32(ZB_RX_FRAME_BUFFER_ADDR);
  reg_phr        = read_data_hdr & 0x000000FF;

  // RX buffer contains PHR + PSDU data
  psdu_len       = (reg_phr & 0x7F) + 1; //to account for PHR
#endif
  
  // mod_size are the number of bytes that must be read to make total read bytes in multiple of 4
  mod_size = (uint8_t)(length % 4U);

  // Set mask for the last read based on mod_size (mask uninitialized rx buffer locations with 0)
  if (mod_size == 0U)
  {
      mask = 0xFFFFFFFFU;
  }
  else if (mod_size == 1U)
  {
      mask = 0x000000FF;
  }
  else if (mod_size == 2U)
  {
      mask = 0x0000FFFF;
  }
  else 
  {
      mask = 0x00FFFFFF;
  }
  
  (void)mask;
  if (mod_size !=0U)
  {
    total_len = (uint8_t)(length + 4U) - mod_size;
  }
  else
  {
    total_len   = length;
  }

  address = (ZB_RX_FRAME_BUFFER_ADDR+ addressOffset);
  // Read RX BUFFER for PHR + PSDU data (Last rx buffer read is done with mask)
  for (k = 0; k < (total_len); k+=4U, address+=4U) 
  {
    if (k != (length - mod_size))
    {
      read_data    = z_readReg32(address);

      *(buf+k)  = (uint8_t)(read_data & 0x000000FFU);
      *(buf+k+1) = (uint8_t)((read_data & 0x0000FF00U)>>8);
      *(buf+k+2) = (uint8_t)((read_data & 0x00FF0000U)>>16);
      *(buf+k+3) = (uint8_t)((read_data & 0xFF000000U)>>24);
    }
    else 
    {
      read_data = z_readReg32(address);
	  if(mod_size > 0U)
      {
	  	*(buf+k)  = (uint8_t)(read_data & 0x000000FFU);
      }
	  if(mod_size > 1U)
      {
          *(buf+k+1) = (uint8_t)((read_data & 0x0000FF00U)>>8);
      }
	  if(mod_size > 2U)
      {
	    *(buf+k+2) = (uint8_t)((read_data & 0x00FF0000U)>>16);
      }

    }
  }
}

INLINE void phyStartReadFrameInline(void) {

  //volatile uint32_t read_data_hdr = x_readReg32(ZB_RX_FRAME_BUFFER);
  // Not  needed actually
}

INLINE void phyStopReadFrameInline(void) {
// Not  needed actually
}

INLINE void phyStartReadSramInline (void) {
// Not  needed actually
}

INLINE void phyStopReadSramInline (void) {
// Not  needed actually
} 

INLINE void phyStartReadFrame(void) {
// Not  needed actually
}

INLINE void phyStopReadFrame(void) {
// Not  needed actually
}

INLINE void zigbeeArbReq(bool tx, uint8_t priority, bool time_sensitive)
{
  if( tx )
  {
    z_reg_write8_slice(ZB_RADIO_CTRL_8_REG_ADDR, 7, 4, priority);
  }
  else
  {
    z_reg_write8_slice(ZB_RADIO_CTRL_6_REG_ADDR, 3, 0, priority);
  }
  
  if( time_sensitive )
  {
    z_reg_write8_slice(ZB_RADIO_CTRL_REG_ADDR, 2, 2, 1);
  }
  else
  {
    z_reg_write8_slice(ZB_RADIO_CTRL_REG_ADDR, 2, 2, 0);
  }
}

INLINE bool zigbeeRadioOwnership(void)
{
  bool zigbeeOwner = false;
  uint32_t arbCoreStatus  = z_readReg32(Radio_Arbiter_Core_Status_Addr);
 // printf("Arb_Core_Status %lu \n\r",arbCoreStatus);
  if((arbCoreStatus & ARBITER_ZB_GRANT_STATE) != 0U)// &&(arbCoreStatus & RADIO_OWNER_ZB_LINK) ) 
  {
      zigbeeOwner = true;
  }
  return zigbeeOwner;
}


/******************************************************************************
  Does encryption over SPI.
  Parameters:
    request - pointer to request structure.
  Returns: status of operation. PHY_BUSY_RX_REQ_STATUS and
    PHY_SUCCESS_REQ_STATUS are all possible.
******************************************************************************/
#ifdef _RF_AES_
void phyEncryptOverSpi(PHY_EncryptReq_t *req);
#endif


/******************************************************************************
  Utility Functions of Pic32cx
  Aceess Radio , Access registers within BB , SOC for bit , byte , word
******************************************************************************/
INLINE uint16_t aclb_spi_read (char  addr) 
{
  uint16_t int_buf;
  z_reg_write8(RF_control_register_rf_ctrl_2_hl_addr, (uint8_t)addr);    // addr 8 bit
  z_do_bitwr8(RF_programming_register_rfprog_hl_addr, 0x04, 0x04);// read aclb via SPI

  int_buf = z_do_bitrd8(RF_programming_register_rfprog_hl_addr, 0x04);
  while (int_buf != 0x00U)
    {
      int_buf = z_do_bitrd8(RF_programming_register_rfprog_hl_addr, 0x04);
    }

  int_buf = z_readReg16(RF_SiWData_ll_addr); //rdata 16 bit
  return int_buf;
}

INLINE void aclb_spi_write (uint16_t addr, uint16_t wdata) 
{
  uint16_t int_buf;
  z_reg_write16(RF_Control_Register_RF_CTRL_ll_addr, wdata);     // wdata 16 bit
  z_reg_write8(RF_control_register_rf_ctrl_2_hl_addr, (uint8_t)addr);    // addr 8 bit
  z_do_bitwr8(RF_programming_register_rfprog_hl_addr, 0x10, 0x10);// write aclb via SPI
  
  int_buf = z_do_bitrd8(RF_programming_register_rfprog_hl_addr, 0x10);
  while (int_buf != 0x00U)
    {
      int_buf = z_do_bitrd8(RF_programming_register_rfprog_hl_addr, 0x10);
    }
}


INLINE void z_reg_write32(uint32_t addr, uint32_t data)
{
  *((volatile uint32_t*)addr) = data;
}

// Write 16bit data at addr 
INLINE void z_reg_write16(uint32_t addr, uint16_t data)
{
  *((volatile uint16_t*)addr) = data;
}

// Write 8bit data at addr 
INLINE void z_reg_write8(uint32_t addr, uint8_t data)
{
  *((volatile uint8_t*)addr) = data;
}

INLINE void z_reg_write32_slice(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value)
{
  uint32_t mask, write_data, read_data, i;

  mask = 0xFFFFFFFFU;
  for (i=lsb; i<=msb; i++){
        mask = mask & (uint8_t)(~(0x1U << i));
  }

  read_data  = z_readReg32(addr);
  write_data = (read_data & mask) | ((value << lsb) & ~mask);
  z_reg_write32 (addr, write_data);
}

INLINE void z_reg_write32_slice_RF(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value)
{
  uint32_t mask, write_data, read_data, i;
  addr =  addr + SUBSYS_BASE_ADDR;
  mask = 0xFFFFFFFFU;
  for (i=lsb; i<=msb; i++){
        mask = mask & (uint8_t)(~(0x1U << i));
  }

  read_data  = z_readReg32(addr);
  write_data = (read_data & mask) | ((value << lsb) & ~mask);
  z_reg_write32 (addr, write_data);
}

INLINE uint32_t z_readReg32_slice(uint32_t addr, uint32_t msb, uint32_t lsb)
{
  uint32_t mask, read_data, i;

  mask = 0x0;
  for (i=lsb; i<=msb; i++){
        mask = mask | (uint8_t)(0x1U << i);
  }

  read_data  = z_readReg32(addr);
  read_data  = ((read_data & mask)>>lsb);
  return read_data;
}

INLINE void z_reg_write16_slice(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value)
{
  uint32_t mask, write_data, read_data, i;

  mask = 0xFFFF;
  for (i=lsb; i<=msb; i++){
        mask = mask & (uint8_t)(~(0x1U << i));
  }

  read_data  = z_readReg16(addr);
  write_data = (read_data & mask) | ((value << lsb) & ~mask);
  z_reg_write16 (addr, (uint16_t)write_data);
}

INLINE uint32_t z_readReg16_slice(uint32_t addr, uint32_t msb, uint32_t lsb)
{
  uint32_t mask, read_data, i;

  mask = 0x0;
  for (i=lsb; i<=msb; i++){
        mask = mask | (uint8_t)(0x1U << i);
  }

  read_data  = z_readReg16(addr);
  read_data  = ((read_data & mask)>>lsb);
  return read_data;
}

INLINE void z_reg_write8_slice(uint32_t addr, uint32_t msb, uint32_t lsb, uint32_t value)
{
  uint32_t mask, write_data, read_data, i;

  mask = 0xFF;
  for (i=lsb; i<=msb; i++){
        mask = mask & (uint8_t)(~(0x1U << i));
  }

  read_data  = z_readReg8(addr);
  write_data = (read_data & mask) | ((value << lsb) & ~mask);
  z_reg_write8 (addr, (uint8_t)write_data);
}

static inline uint32_t z_readReg8_slice(uint32_t addr, uint32_t msb, uint32_t lsb)
{
  uint32_t mask, read_data, i;

  mask = 0x0;
  for (i=lsb; i<=msb; i++){
        mask = mask | (uint8_t)(0x1U << i);
  }

  read_data  = z_readReg8(addr);
  read_data  = ((read_data & mask)>>lsb);
  return read_data;
}


// ------------------------------------------
// Register read functions
// ------------------------------------------

// read 32bit data at addr 
INLINE uint32_t z_readReg32(uint32_t addr)
{
  return( *((volatile uint32_t *)addr));
}

// read 16bit data at addr 
INLINE uint16_t z_readReg16(uint32_t addr)
{
  return( *((volatile uint16_t *)addr));
}

// read 8bit data at addr 
INLINE uint8_t z_readReg8(uint32_t addr)
{
  uint8_t value;
  value = *((volatile uint8_t *)addr);
  //return( *((volatile uint8_t *)addr));
  return value;
}


// ----------------------------------------------------
// Bit Read FUNCTION
// ----------------------------------------------------
INLINE uint8_t z_do_bitrd8(uint32_t address, uint8_t mask)
{
  volatile uint8_t reg;
  reg = z_readReg8(address);
  reg = reg & mask;
  return reg;
}

INLINE uint16_t z_do_bitrd16(uint32_t address, uint16_t mask)
{
  volatile uint16_t reg;
  reg = z_readReg16(address);
  reg = reg & mask;
  return reg;
}

INLINE uint32_t z_do_bitrd32(uint32_t address, uint32_t mask)
{
  volatile uint32_t reg;
  reg = z_readReg32(address);
  reg = reg & mask;
  return reg;
}

// ----------------------------------------------------
// Bit Write FUNCTION
// ----------------------------------------------------
INLINE void z_do_bitwr8(uint32_t address, uint8_t mask, uint8_t val)
{
  volatile uint8_t reg;
  reg = z_readReg8(address);
  reg = (reg & ~mask) | (val & mask);
  z_reg_write8(address, reg);
}


INLINE void z_do_bitwr16(uint32_t address, uint16_t mask, uint16_t val)
{
  volatile uint16_t reg;
  reg = z_readReg16(address);
  reg = (reg & ~mask) | (val & mask);
  z_reg_write16(address, reg);
}

INLINE void z_do_bitwr32(uint32_t address, uint32_t mask, uint32_t val)
{
  volatile uint32_t reg;
  reg = z_readReg32(address);
  reg = (reg & ~mask) | (val & mask);
  z_reg_write32(address, reg);
}

static inline void spi_write_slice(uint16_t addr, uint16_t msb, uint16_t lsb, uint16_t wdata)
{
  uint16_t mask, write_data, read_data, i;
  mask = 0xFFFF;
  for (i=lsb; i<=msb; i++){
        mask =(uint16_t)(mask & (uint8_t)(~(0x1U << i)));
  }
  read_data = aclb_spi_read((char)addr);
  write_data = (read_data & mask) | ((wdata << lsb) & ~mask);
  aclb_spi_write(addr, write_data);
}

static inline uint16_t spi_read_slice (char addr, uint16_t msb, uint16_t lsb)
{
  uint16_t mask, read_data, i;
  mask = 0x0;
  for (i=lsb; i<=msb; i++){
        mask = (uint16_t)(mask | (uint8_t)(0x1U << i));
  }
  read_data  = aclb_spi_read(addr);
  read_data  = ((read_data & mask)>>lsb);
  return read_data;
}

INLINE void x_asm_delay(uint8_t loop)
{

  for (;loop>0U; loop--){ NOP;};
}
#endif /* PHYPIC32CX_H */

//eof phyRfSpiProto.h
