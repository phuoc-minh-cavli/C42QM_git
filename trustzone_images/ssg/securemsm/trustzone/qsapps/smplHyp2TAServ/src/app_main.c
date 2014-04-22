/*
@file app_main.c
@brief App main entry point.

*/
/*===========================================================================
   Copyright (c) 2018 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

#include <stdint.h>
#include "qsee_log.h"
#include "object.h"
#include "IOpener.h"
#include "COEMHypToTAService.h"
#include "COEMHypToTAService_open.h"

uint8_t log_lvl = 0;

int32_t tz_module_open(uint32_t uid, Object cred, Object *objOut)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "smplHyp2TAServ tz_module_open() uid: %d", uid);

  switch (uid) {

     case COEMHypToTAService_UID:
        return COEMHypToTAService_open(cred, objOut);

     default:
        break;
  }

  *objOut = Object_NULL;
  return Object_ERROR_INVALID;
}

void tz_app_init(void)
{
  /*  App specific initialization code*/
  log_lvl = qsee_log_get_mask();
  qsee_log_set_mask(log_lvl | QSEE_LOG_MSG_DEBUG);
  qsee_log(QSEE_LOG_MSG_DEBUG, "smplHyp2TAServ Init ");
}

void tz_app_cmd_handler(void* cmd, uint32_t cmdlen,
                        void* rsp, uint32_t rsplen)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "smplHyp2TAServ received command.");
  return;
}

void tz_app_shutdown(void)
{
  qsee_log(QSEE_LOG_MSG_DEBUG, "smplHyp2TAServ Shutdown.");
  qsee_log_set_mask(log_lvl);
  return;
}
