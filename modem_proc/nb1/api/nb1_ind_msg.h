/*!
  @file
  nb1_ind_msg.h

  @brief
  This file contains definitions for all the NB1 broastcast indications

*/

/*=============================================================================

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/nb1_ind_msg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/

#ifndef NB1_IND_MSG_H
#define NB1_IND_MSG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include <msgr.h>
#include <msgr_nb1.h>

/*=============================================================================

                   EXTERNAL CONSTANT/MACRO DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------*/
/*     NB1 broastcast Indication messages                                */
/*-----------------------------------------------------------------------*/

/*! @brief Broastcast Indications that the NB1 MAC module sends. Note that
there are 2 enum defines for MAC Indications. The ones defined here are
truely broastcast indications*/
enum
{
  /*! MAC sends this to RRC, MAC DL, ML1 */
  MSGR_DEFINE_UMID(NB1,MAC,IND,CONTENTION_RESULT,0x00,
                   nb1_mac_contention_result_ind_msg_s),

  /*! MAC sends this to ML1 in case MAC can't include the
  PHR in the UL TB. This could happen when there is not enough grant */
  MSGR_DEFINE_UMID(NB1,MAC,IND,SEND_PHR_FAILED,0x01,
                   nb1_mac_send_phr_failed_ind_msg_s),
#ifdef TEST_FRAMEWORK
  #error code not present
#endif
  /*! RRC sends this when Handover is successful */
  MSGR_DEFINE_UMID(NB1, RRC, IND, HANDOVER_COMPLETED, 0x08,
                   nb1_rrc_handover_completed_ind_s),

  /*! RRC sends this when Reconfig message processing is successful */
  MSGR_DEFINE_UMID(NB1, RRC, IND, CONFIG_COMPLETED, 0x09,
                   nb1_rrc_config_completed_ind_s),

};

#endif /* NB1_IND_MSG_H */

