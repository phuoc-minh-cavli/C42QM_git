/**
  @file policyman_msg.h

  @brief  
*/
/*
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

*/

#ifndef _POLICYMAN_MSG_H_
#define _POLICYMAN_MSG_H_

#include "msgr.h"


#define MSGR_MODULE_CFG       0x01
#define MSGR_POLICYMAN_CFG    MSGR_TECH_MODULE(MSGR_TECH_POLICYMAN, MSGR_MODULE_CFG)

enum
{
  MSGR_DEFINE_UMID(POLICYMAN, CFG, IND, UPDATE, 0x01, policyman_cfg_update_ind),
  MSGR_DEFINE_UMID(POLICYMAN, CFG, IND, UPDATE_MSIM, 0x02, policyman_cfg_update_msim_ind)
};


#endif

