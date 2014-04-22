
/*!
  @file
  intf_nbfw_app_msg.h

  @brief
  This file contains NB APP command id's and parameters structures

  @detail
  description of functions in this file

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //source/qcom/qct/modem/fw/components/fw_nb.mpss/rel/15.2/api/intf_nbfw_app_msg.h#2 $
$DateTime: 2023/04/27 07:48:28 $
$Author: aakaniti $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#ifndef INTF_NBFW_APP_MSG_H
#define INTF_NBFW_APP_MSG_H

#include "msgr.h"
#include "intf_nbfw_app.h"

/*===========================================================================

      Constants

===========================================================================*/


/*===========================================================================

      Macros

===========================================================================*/

/*===========================================================================

      Typedefs

===========================================================================*/

typedef enum {
  MSGR_DEFINE_UMID(NB, FW_APP, REQ, ENABLE,      0x0, nbfw_app_enable_req_msg_s),
  MSGR_DEFINE_UMID(NB, FW_APP, REQ, DISABLE,     0x1, nbfw_app_disable_req_msg_s),
  NB_FW_APP_REQ_COUNT=0x2,

  MSGR_DEFINE_UMID(NB, FW_APP, CNF, ENABLE,      0x0, nbfw_app_enable_cnf_msg_s),
  MSGR_DEFINE_UMID(NB, FW_APP, CNF, DISABLE,     0x1, nbfw_app_disable_cnf_msg_s),
  NB_FW_APP_CNF_COUNT=0x2,

  MSGR_DEFINE_UMID(NB, FW_APP, CMDI, TERMINATE,  0x0, nbfw_terminate_cmdi_msg_s),
  NB_FW_APP_CMDI_COUNT=0x1,
} nbfw_app_msg_e;

#endif /* INTF_NBFW_APP_MSG_H */

