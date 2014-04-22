
/*!
  @file lte_rrc_test_embms.h

  @brief
  Header defs for handover integration tests.

  
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

$Header: //source/qcom/qct/modem/lte/RRC/main/latest/test/integration/embms/
lte_rrc_test_embms.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
                   Initial Revision

===========================================================================*/

#ifndef LTE_RRC_TEST_EMBMS_H
#define LTE_RRC_TEST_EMBMS_H
#ifdef FEATURE_LTE_EMBMS

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <lte.h>
#include "lte_rrc_test_integration.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void LteRrceMBMSEnable__Test
(
  void
);

void LteRrcSendeMBMSDeactivateReqForService0
(
  void
);


void LteRrcTestMTCHRelease
(
  lte_rb_id_t mrb_id,
  byte service_id
);

void LteRrcTestSibCollectionAfterHandoverEmbmsMultiBand__Test(lte_rrc_test_cell_e cell_index,
                                                                                lte_rrc_test_sib_update_e group_index);

#endif
#endif /* LTE_RRC_TEST_EMBMS_H */

