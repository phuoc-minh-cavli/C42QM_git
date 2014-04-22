#ifndef NB1_CPHY_FTM_MSG_IDS_H
#define NB1_CPHY_FTM_MSG_IDS_H
/*!
  @file nb1_cphy_ftm_msg_ids.h

  @brief
   The FTM test Message IDs used to interact with the NB1 L1 software module by
   FTM task.
*/

/*==============================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_cphy_ftm_msg_ids.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*! @brief
  Enum for the external ftm test msg interface with L1
*/
enum
{
  /*=========================================================================*/
  /*                        Request/Confirm                                  */
  /*=========================================================================*/
  NB1_CPHY_TEST_GM_SKIP_RACH_ID = 0x0,
  NB1_CPHY_TEST_SKIP_DBCH_PRE_IDLE_ID = 0x1,
  NB1_CPHY_TEST_ML1_SKIP_RACH_ID = 0x2,
  NB1_CPHY_TEST_TX_ENABLE_ID = 0x3,
  NB1_CPHY_TEST_L1_MODE_CHANGE_ID = 0x4,
  NB1_CPHY_TEST_OVERRIDE_UL_GRANT_ID = 0x5,
  NB1_CPHY_TEST_RESET_PDSCH_STATS_ID = 0x6,
  NB1_CPHY_TEST_GET_PDSCH_STATS_ID = 0x7,
  NB1_CPHY_TEST_GET_ALL_CARR_PDSCH_STATS_ID = 0x8,
  /* Msg IDs for CPHY_TEST in PLT starts with Max of FTM CPHY_TEST ids */
  NB1_CPHY_TEST_FTM_MAX_ID = 0xF
};


#endif /* NB1_CPHY_FTM_MSG_IDS_H */

