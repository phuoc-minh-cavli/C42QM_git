#ifndef DS_FWK_DIAG_TEST_TOOL_H
#define DS_FWK_DIAG_TEST_TOOL_H


#include "diagpkt.h"
#include "diagcmd.h"
#include "diagdiag.h"


/* Diag subsystem req codes currently supported */

typedef enum
{
  DS_FWK_DIAG_APPSRV_SUBSYS = 0,
  DS_FWK_DIAG_SUBSYS_MAX  = 0xFF
}ds_fwk_diag_subsys_type_e;

/*===========================================================================
FUNCTION      DS_FWK_DIAG_INIT

DESCRIPTION   This function initializes the ds3gdiagmgr module and registers
              ds3g diag cmds with the diag framework.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_fwk_diag_init();

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
);



#endif