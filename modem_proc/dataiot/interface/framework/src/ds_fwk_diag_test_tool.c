/*==========================================================*/
/*!
  @file 
  ds_fwk_diag_test_tool.c

  @brief
  This file provides functions that are used to process diag commands for testing
  different test modules.

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*================= =========================================*/
/*============================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: 
  $DateTime: 

  when           who      what, where, why
  ---------- --- ----------------------------------------------------------------
  2019-04-22    sraghuve  Created module.

============================================================*/
#include "ds_headers.h"
#include "ds_fwk_diag_test_tool.h"
#include "ds_appsrv_diag.h"


/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.
===========================================================================*/
static const diagpkt_user_table_entry_type ds_fwk_diag_cmd_cfg[] =
{
  {
    DS_FWK_DIAG_APPSRV_SUBSYS,
    DS_FWK_DIAG_APPSRV_SUBSYS,
#if defined FEATURE_APPSRV_TEST_TOOL && ! defined TEST_FRAMEWORK
    ds_appsrv_diag_cmd_hdlr  /* Callback Function for APPSRV Diag Commands */
#else
    ds_fwk_diag_cmd_hdlr
#endif
  }
};

/*===========================================================================
FUNCTION      DS_FWK_DIAG_INIT

DESCRIPTION   This function initializes the ds3gdiagmgr module and registers
              ds3g diag cmds with the diag framework.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_fwk_diag_init()
{
  /* register DIAG command handler functions */
  DIAGPKT_DISPATCH_TABLE_REGISTER((diagpkt_subsys_id_type)DIAG_SUBSYS_DS_DS3G,
                                   ds_fwk_diag_cmd_cfg);
}

/*===========================================================================
FUNCTION      DS_FWK_DIAG_CMD_HDLR

DESCRIPTION   This dummy function passed as DIAG framework expects a non 
              NULL function

DEPENDENCIES  None

RETURN VALUE  A void* response pointer back to diag

SIDE EFFECTS  None
===========================================================================*/
void* ds_fwk_diag_cmd_hdlr
(
  void*    diag_msg_ptr,
  uint16   diag_msg_len
)
{
  return NULL;
}


