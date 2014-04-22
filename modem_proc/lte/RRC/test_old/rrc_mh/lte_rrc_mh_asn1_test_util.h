/*!
  @file lte_rrc_mh_asn1_test_util.h

  @brief
  File containing ASN1 utility functions that cannot be a part of the .cpp test
  file.
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

$Header: //components/rel/lte.mpss/4.7.2/RRC/test_old/rrc_mh/lte_rrc_mh_asn1_test_util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/08   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_MH_ASN1_TEST_UTIL_H
#define LTE_RRC_MH_ASN1_TEST_UTIL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/




/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/




/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

extern void *lte_rrc_mh_test_get_CDMA2000_CSFBParametersRequest
(
 void
);

extern void *lte_rrc_mh_test_get_ul_msg_enc_fail
(
 void
);

extern long lte_rrc_mh_test_get_CDMA2000_CSFBParametersResponse
(
  unsigned char **encoded_msg
);

#endif /* LTE_RRC_ASN1_TEST_UTIL_H */
