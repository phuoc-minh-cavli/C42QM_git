/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/uimdiag/src/uimdiag_refresh_only.c#1 $$ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
07/25/18   bcho  Initial Revision


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "diagcmd.h"
#include "uimdiag.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_mmgsdi_util.h"
#include "uim_msg.h"
#include "uimdiag_util.h"
#include "uim_p.h"
#include "modem_mem.h"

/*=============================================================================

                       GLOBAL DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   UIMDIAG_SUBSYS_DELAYED_TBL

   DESCRITPION:
     Table contains uimdiag subsystem command ids and corresponding
     call back functions. This table is used during uimdiag initialization
     for registering

   ------------------------------------------------------------------------- */
static const diagpkt_user_table_entry_type uimdiag_subsys_delayed_tbl[] =
{
   {(word)UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD ,
   (word)UIMDIAG_MMGSDI_CHECK_DIAG_SUPPORT_CMD,
   uimdiag_check_diag_support_cmd},
    {(word)UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
   (word)UIMDIAG_MMGSDI_NAA_REFRESH_CMD,
   uimdiag_mmgsdi_naa_refresh_cmd}
};

uimdiag_req_table_elem_type *uimdiag_req_table_ptr = NULL;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION UIMDIAG_REQ_TABLE_INIT

DESCRIPTION
    Initializes the req_table to NULL.

ARGUMENTS
    None

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/
static void uimdiag_req_table_init(void)
{
  /* Initialize table. Set it to NULL */
  uint32 max_client_req_tbl_size          = UIMDIAG_MAX_CLIENT_REQ_TBL_SIZE;

  UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
    uimdiag_req_table_ptr,
    (sizeof(uimdiag_req_table_elem_type) * max_client_req_tbl_size));
}/* uimdiag_req_table_init */


/*===========================================================================

FUNCTION UIMDIAG_INIT

DESCRIPTION
  The uimdiag init function initializes sub system commands to DIAG framework.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void uimdiag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY(
    DIAG_SUBSYS_CMD_VER_2_F,
    (uint8)DIAG_SUBSYS_UIM,
    uimdiag_subsys_delayed_tbl);
    uimdiag_req_table_init();
}/* uimdiag_init */

