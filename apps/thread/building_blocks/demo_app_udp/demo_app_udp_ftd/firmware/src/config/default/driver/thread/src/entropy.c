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

/**
 * @file
 *   This file implements an entropy source based on TRNG.
 *
 */


//DOM-IGNORE-BEGIN
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
//DOM-IGNORE-END

#include <string.h>
#include <openthread/platform/entropy.h>

#include "platform-pic32cx.h"
#include <openthread/platform/radio.h>

#include "configuration.h"

#include "definitions.h"

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength)
{
    uint32_t random;
    uint8_t *end = aOutput;
    end += aOutputLength;
    for (uint16_t i = 0; i < (aOutputLength / sizeof(uint32_t)); i++)
    {
        random = TRNG_ReadData();
        *((uint32_t *)aOutput) = random;
        aOutput += sizeof(uint32_t);
    }
    
    random = TRNG_ReadData();
    memcpy((uint8_t *)aOutput, (uint8_t *)random, (end - aOutput));

    return OT_ERROR_NONE;
}
