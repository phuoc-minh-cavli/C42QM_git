/*!
  @file lte_rrc_sec_test_ota.h

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/sec/lte_rrc_sec_test_ota_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/11   kp      Added changes for W2L PSHO feature.
===========================================================================*/

#ifndef LTE_RRC_SEC_TEST_OTA_MSG_H
#define LTE_RRC_SEC_TEST_OTA_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/






/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External type 
*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*
 smc1_mar:value DL-DCCH-Message ::= 
{
  message c1 : securityModeCommand : 
      {
        rrc-TransactionIdentifier 2,
        criticalExtensions c1 : securityModeCommand-r8 : 
            {
              securityConfigSMC 
              {
                securityAlgorithmConfig 
                {
                  cipheringAlgorithm eea0,
                  integrityProtAlgorithm spare1
                }
              }
            }
      }
}
*/
//static byte sec_smc_1[] = {0x32, 0x06, 0x02, 0x00}; 
//static byte sec_smc_1[3] = {0x68, 0x0c, 0x11}; 
//static byte sec_smc_1[] = {0x34, 0x06, 0x22, 0x00}; 
//static byte sec_smc_1[] = {0x34, 0x06, 0xE0, 0x00}; 

#define DECLARE_LTE_RRC_SEC_SMC_EEA0_EIA0 \
static uint8 sec_smc_1[] = \
{ \
  0x34, 0x00, 0x70, \
} 

#define DECLARE_LTE_RRC_SEC_SMC_EEA1_EIA1 \
static uint8 sec_smc1_eea1_eia1[] = \
{ \
  0x34, 0x01, 0x10, \
} 

#define DECLARE_LTE_RRC_SEC_SMC_EEA2_EIA2 \
static uint8 sec_smc1_eea2_eia2[] = \
{\
  0x34, 0x02, 0x20, \
}

#define DECLARE_LTE_RRC_SEC_SMC_EEA0_EIA1 \
static uint8 sec_smc1_eea0_eia1[] = \
{\
  0x34, 0x00, 0x10, \
}
#define DECLARE_LTE_RRC_SEC_SMC_EEA0_EIA0_920 \
static uint8 sec_smc1_eea0_eia0_920[] = \
{\
  0x30, 0x00, 0x00, \
}
#define DECLARE_LTE_RRC_SEC_SMC_EEA0_EIA2 \
static uint8 sec_smc1_eea0_eia2[] = \
{\
  0x34, 0x00, 0x20, \
}

#define DECLARE_LTE_RRC_SEC_SMC_EEA2_EIA1 \
static uint8 sec_smc1_eea2_eia1[] = \
{\
  0x34, 0x02, 0x10, \
}
#define DECLARE_LTE_RRC_SEC_RECFG_NCC0_KCI0 \
static uint8 sec_recfg_ncc0_kci0[] = \
{\
  0x26, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
}
#define DECLARE_LTE_RRC_SEC_RECFG_NCC2_KCI0 \
static uint8 sec_recfg_ncc2_kci0[] = \
{\
  0x26, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, \
}
#define DECLARE_LTE_RRC_SEC_RECFG_NCC5_KCI0 \
static uint8 sec_recfg_ncc5_kci0[] = \
{\
  0x26, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, \
}
#define DECLARE_LTE_RRC_SEC_RECFG_NCC4_KCI0 \
static uint8 sec_recfg_ncc4_kci0[] = \
{\
  0x26, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, \
}
#define DECLARE_LTE_RRC_SEC_RECFG_NCC0_KCI1 \
static uint8 sec_recfg_ncc0_kci1[] = \
{\
  0x26, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, \
}
#define DECLARE_LTE_RRC_SEC_RECFG_NCC2_KCI1_EEA2_EIA2 \
static uint8 sec_recfg_ncc2_kci1_eea2_eia2[] = \
{\
  0x24, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0xa0, \
}

#define DECLARE_LTE_RRC_SEC_W2L_PSHO_RECFG_EEA1_EIA1 \
static uint8 sec_recfg_w2l_PSHO[] = \
{\
  0x26, 0x09, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x02, 0x22, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00, \
}


#endif  /*LTE_RRC_SEC_TEST_OTA_MSG_H*/

