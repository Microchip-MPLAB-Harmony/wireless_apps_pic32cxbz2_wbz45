/*
 *  Copyright (c) 2023, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) [2023], Microchip Technology Inc., and its subsidiaries. All rights reserved.
  
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
// DOM-IGNORE-END

#ifndef _STACKCONFIG_H    /* Guard against multiple inclusion */
#define _STACKCONFIG_H


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


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */


/* ************************************************************************** */
/** Descriptive Constant Name

  @Summary
    Brief one-line summary of the constant.

  @Description
    Full description, explaining the purpose and usage of the constant.
    <p>
    Additional description in consecutive paragraphs separated by HTML 
    paragraph breaks, as necessary.
    <p>
    Type "JavaDoc" in the "How Do I?" IDE toolbar for more information on tags.

  @Remarks
    Any additional remarks
 */
 
//#define OT_PLATFORM                                                  (external)
#define PACKAGE_NAME                                                 "OPENTHREAD"
#define PACKAGE_VERSION                                              "mchp_harmony_wireless_thread_v1.0.0"

#define OPENTHREAD_CONFIG_FILE                                       "openthread-core-pic32cx-config.h"
#define OPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE                   "openthread-core-pic32cx-config-check.h"
#define OPENTHREAD_PROJECT_CORE_CONFIG_FILE                          "openthread-core-pic32cx-config.h"
#define MBEDTLS_CONFIG_FILE                                          "openthread-mbedtls-config.h"
#define MBEDTLS_USER_CONFIG_FILE                                     "pic32cx-mbedtls-config.h"

#define NDEBUG
#define OPENTHREAD_CONFIG_ASSERT_ENABLE                              (1)
#define OPEN_THREAD_UART_ENABLE										 (1)
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS                     (1)
#define OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT          (1)
#define OPENTHREAD_CONFIG_NCP_HDLC_ENABLE                            (1)
#define OPENTHREAD_CONFIG_THREAD_VERSION                             (OT_THREAD_VERSION_1_3)
#define OPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE           (0)
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT           (0)
#define OPENTHREAD_ENABLE_NCP_VENDOR_HOOK                            (0)
#define OPENTHREAD_RADIO                                             (1) 
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_SECURITY_ENABLE            (1) 
#define OPENTHREAD_CONFIG_LINK_RAW_ENABLE							 (1)
#define OPENTHREAD_CONFIG_MAC_SOFTWARE_TX_TIMING_ENABLE              (1)

/* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _STACKCONFIG_H */

/* *****************************************************************************
 End of File
 */
