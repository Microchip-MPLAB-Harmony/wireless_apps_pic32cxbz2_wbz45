/**
 * @file app_p2p_phy_config.h
 *
 * @brief File contains P2P PHY App configuration parameters.
 *
 * Copyright (c) 2023 Microchip Technology Inc. and its subsidiaries.
 *
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */


/* Prevent double inclusion */
#ifndef APP_P2P_PHY_CONFIG_H
#define APP_P2P_PHY_CONFIG_H

/* === INCLUDES ============================================================ */

/* === EXTERNALS =========================================================== */

/* === MACROS ============================================================== */

/*
 * TAL PIB default values
 */
#define CHANNEL_TRANSMIT_RECEIVE            11U
#define SRC_ADDR                            0x1U
#define IEEE_ADDR                           0x1U
#define SRC_PAN_ID                          0xcafeU
#define MAXBE                               5U
#define MINBE                               3U
#define CSMA_BACKOFF                        4U
#define FRAME_RETRY                         3U
#define CCA_MODE                            1U
#define TRANSMIT_POWER                      4
#define APP_PAYLOAD_BUFFER_SIZE             500U
#define DST_ADDR                            0x2U
#define DST_IEEE_ADDR                       0x2U
#define NUM_OF_DEVICES                      64U
#define ED_SCAN_DURATION                    8U
/* === TYPES =============================================================== */


/* === PROTOTYPES ========================================================== */

#endif /* APP_P2P_PHY_CONFIG_H */
