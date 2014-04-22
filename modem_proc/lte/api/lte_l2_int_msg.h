/*!
  @file
  lte_l2_int_msg.h

  @brief
  This file contains all internal to LTE messages data structure and IDs 
  interfaces exported by LTE L2 layer.


*/

/*==============================================================================

  Copyright (c) 2015 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_l2_int_msg.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
09/08/15   ar      initial version
==============================================================================*/

#ifndef LTE_L2_INT_MSG_H
#define LTE_L2_INT_MSG_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include <lte_l2_comdef.h>
#include <cfcm.h>
#ifdef FEATURE_LTE_UDC_ENABLED
#include <udcif.h>
#endif /*FEATURE_LTE_UDC_ENABLED*/

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief All LTE subsystem messages to\from L2 start from the UMID base 
  value of 32. UMID values 0-31 are reserved for external subsystems.
*/
#define LTE_L2_MSG_UMID_ENUM_BASE        32

/*----------------------------------------------------------------------------*/
/*     LTE L2 message definitions                                             */
/*----------------------------------------------------------------------------*/

/*! @brief cause code enums for LTE_L2_RLF_IND to RRC
*/
typedef enum
{
  LTE_L2_RLF_IND_CAUSE_INVALID,
#ifdef FEATURE_LTE_UDC_ENABLED
  LTE_L2_RLF_IND_CAUSE_MANY_UDC_RESETS,/*!< UDC getting more than 5 RESET 
                                            requests from eNB within 60 secs. Treated as RLF with ELS/UDC disabled */
#endif /* FEATURE_LTE_UDC_ENABLED */
  LTE_L2_RLF_IND_CAUSE_MAX
} lte_l2_rlf_ind_cause_e;

/*! @brief indication triggered by LTE L2 to request RRC to trigger RLF
*/
typedef struct
{
  msgr_hdr_struct_type                hdr;    /*!< message header */
  lte_l2_rlf_ind_cause_e              cause;  /*!< RLF cause */
} lte_l2_rlf_ind_msg_s;

/*----------------------------------------------------------------------------*/
/*     LTE L2 External Indication messages                                    */
/*----------------------------------------------------------------------------*/

/*! @brief external Indications that the LTE L2 module sends */
enum
{
  MSGR_DEFINE_UMID(LTE,L2,IND,RLF,LTE_L2_MSG_UMID_ENUM_BASE, 
                   lte_l2_rlf_ind_msg_s)
};

#endif /* LTE_L2_INT_MSG_H */
