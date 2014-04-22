/*!
  @file
  lte_rrc_mh_test_common.h

  @brief
  REQUIRED brief one-sentence description of this C header file.

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_test_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef LTE_RRC_MH_TEST_COMMON_H
#define LTE_RRC_MH_TEST_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*
ul_dcch_ul_info_tx:value UL-DCCH-Message ::= 
{
  message c1 : ulInformationTransfer : 
      {
        criticalExtensions c1 : ulInformationTransfer-r8 : 
            {
              informationType nas3GPP : ''H
            }
      }
}
*/
#define DECLARE_UL_DCCH_UL_INFO_TX \
static unsigned char ul_dcch_ul_info_tx[3] = {0x48, 0x00, 0x00}

/*
ul_dcch_rrc_reconfig_comp:value UL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfigurationComplete : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions rrcConnectionReconfigurationComplete-r8 : 
          {
          }
      }
}
*/
#define DECLARE_UL_DCCH_RRC_RECONFIG_COMP \
static unsigned char ul_dcch_rrc_reconfig_comp[2] = {0x10, 0x00}

/*
sec_mode_complete:value UL-DCCH-Message ::= 
{
  message c1 : securityModeComplete : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions securityModeComplete-r8 : 
          {
          }
      }
}
*/
#define DECLARE_SEC_MODE_COMPLETE \
static unsigned char sec_mode_complete[] = {0x28, 0x00}

/*
security_mode_command:value DL-DCCH-Message ::= 
{
  message c1 : securityModeCommand : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions c1 : securityModeCommand-r8 : 
            {
              securityConfiguration 
              {
                keyChangeIndicator FALSE,
                nextHopChainingCount 0
              }
            }
      }
}
*/
#define DECLARE_SEC_MODE_COMMAND \
static unsigned char sec_mode_command[] = {0x30, 0x00, 0x00}

/*
dl_dcch_connection_reconfiguration:value DL-DCCH-Message ::= 
{
  message c1 : rrcConnectionReconfiguration : 
      {
        rrc-TransactionIdentifier 0,
        criticalExtensions c1 : rrcConnectionReconfiguration-r8 : 
            {
            }
      }
}
*/
#define DECLARE_CONN_RECONFIG \
static unsigned char conn_reconfig[] = {0x20, 0x00, 0x00}

#define DECLARE_RRC_CONN_RECONFIG_8_5 \
static unsigned char rrc_conn_reconfig_8_5[] = {0x22, 0x12, 0x0A, 0x80, 0x0F, 0x80, 0x02, 0x8D, 0xD7, 0xE0, 0x02, 0xC2, 0x00, 0xF8, 0x40, 0x80, 0xAE, 0x00, 0x00, 0x01, 0x00, 0x3D, 0xB0, 0x00, 0x00, 0xC1, 0x12, 0x00, 0x00, 0x0D, 0xC1, 0x86, 0x80, 0x00, 0x0A, 0x85, 0x00, 0x75, 0x09, 0x7C, 0x80, 0x00, 0x00, 0xB3, 0x00, 0xAE, 0x00, 0x00, 0x6B, 0x02, 0x3C, 0x00, 0x00, 0x72, 0x03, 0x4C, 0x00, 0x00, 0x84, 0x81, 0x95, 0x5C, 0x00, 0x82, 0x20, 0x88, 0x00, 0x00, 0xA3, 0x26, 0x07, 0x1A, 0x02, 0x46, 0x00, 0x06, 0x05, 0x00, 0x22, 0x00, 0x0C, 0x02, 0xC8, 0xC0, 0x18, 0x05, 0x30, 0x00, 0x70, 0x0D, 0x08, 0x01, 0x00, 0x01, 0x00, 0x0E, 0x06, 0x06, 0x01, 0x00, 0x07, 0x84, 0x22, 0xC4, 0x02, 0x00, 0x08, 0x00, 0x18, 0x01, 0x80, 0x07, 0xC1, 0x50, 0x02, 0x80, 0x01, 0x40, 0x82, 0x81, 0x80, 0x03, 0xC0, 0x4B, 0xC0, 0xE0, 0x00, 0x90, 0x61, 0x40, 0xC0, 0x00, 0xD8, 0x48, 0x49, 0x80, 0xE0, 0x00, 0xE0, 0x2E, 0x60, 0xF0, 0x00, 0xE8, 0x0B, 0xDE, 0x00, 0x00, 0x30, 0x2F, 0x14, 0x00, 0x00, 0x00, 0x26, 0xC2, 0x00, 0x00, 0x40, 0x20, 0x21, 0x00, 0x00, 0x38, 0x34, 0xC5, 0x00, 0x00, 0x7C, 0x1F, 0x04, 0x3E, 0x18, 0xFC, 0x5F, 0xFF, 0xDF, 0xF1, 0x8B, 0xFE, 0xC7, 0xFB, 0x9F, 0x93, 0x5F, 0x2E, 0xFE, 0x6E, 0x7F, 0x9D, 0xFE, 0xEF, 0xFD, 0x7B, 0xE7, 0x0F, 0xD0, 0x3F, 0xA4, 0x9F, 0xC9, 0x7F, 0x83, 0x7E, 0xF6, 0xFD, 0x8F, 0xFA, 0xCB, 0xF4, 0x47, 0xE6, 0x9F, 0xC9, 0x5F, 0x52, 0xFE, 0xBA, 0xFD, 0x8C, 0xFB, 0x5B, 0xF8, 0xCF, 0xEE, 0x77, 0xDF, 0x4F, 0xC1, 0xB9, 0xF0, 0x7D, 0x40, 0xDC, 0x04, 0x00, 0x07, 0x00, 0x42, 0x60, 0x00, 0x0F, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}

#define DECLARE_SECURITY_IS_CONFIGURED \
static boolean security_is_configured


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void lte_rrc_mh_test_send_ul_dcch_msg
(
  boolean ack_needed, 
  int mu_id, 
  unsigned char *msg_ptr, 
  int length
);

#endif /* LTE_RRC_MH_TEST_COMMON_H */
