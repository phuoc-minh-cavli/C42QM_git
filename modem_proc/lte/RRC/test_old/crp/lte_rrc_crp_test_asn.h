/*!
  @file lte_rrc_crp_test_asn.h

  @brief
  Utilities for CRP unit tests.

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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/crp/lte_rrc_crp_test_asn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/2008 sureshs Removed conn rel msg free function
09/15/2008 sureshs Added function to construct message with mobility info
09/15/2008 sureshs Initial version
===========================================================================*/

#ifndef LTE_RRC_CRP_TEST_ASN_H
#define LTE_RRC_CRP_TEST_ASN_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
extern "C"
{
#include <lte_rrc_int_msg.h>
#include <lte_rrc_osys_asn1.h>
#include <lte_rrc_osys_asn1util.h>
}

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/* Constructs a Connection Release message */
extern void lte_rrc_crp_test_construct_conn_rel_msg
(
  lte_rrc_mh_dlm_s *msg_ptr, /*!< Pointer to constructed message */
  boolean tau_is_reqd, /*!< Is load balancing TAU required */
  boolean redirect_info_is_present, /*!< Is redirect info present */
  boolean mobility_info_is_present /*!< Is mobility info present */
);

#if 0
extern void lte_rrc_crp_test_construct_conn_rel_msg_with_g_freqPriorityList
(
  lte_rrc_mh_dlm_s *msg_ptr, /*!< Pointer to constructed message */
  uint8 list_type
);
#endif
#endif /* LTE_RRC_CRP_TEST_ASN_H */

