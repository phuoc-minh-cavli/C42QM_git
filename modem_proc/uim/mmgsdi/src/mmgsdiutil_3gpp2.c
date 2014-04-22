/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   3 G P P 2   U T I L I T Y   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the utility functions needed in the MMGSDI module

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdiutil_3gpp2.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/16   sn     Fix compiler errors
05/10/16   vdc    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_icc.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ============================================================================
FUNCTION MMGSDI_UTIL_GET_MEID_AVAILABILITY

DESCRIPTION
  This function will check the MEID availability of given session.

DEPENDENCIES
  None

LIMITATIONS
  Supports only Provisioning sessions

RETURN VALUE
  boolean:  TRUE : When APP and ME both have MEID capabilty
            FALSE: When either ME or APP does not have MEID support

SIDE EFFECTS
  None
============================================================================*/
boolean mmgsdi_util_get_meid_availability(
  mmgsdi_session_id_type       session_id
)
{
  mmgsdi_meid_data_type    meid_nv_data;
  boolean                  card_meid_avail   = FALSE;

  memset((byte*)(&meid_nv_data), 0x00, sizeof(mmgsdi_meid_data_type));
  meid_nv_data = mmgsdi_get_meid();

  /* check if service MEID is supported and activated */
  if( MMGSDI_SUCCESS !=  mmgsdi_icc_chk_srv_available(session_id,
                                                      MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                      MMGSDI_CDMA_SRV_MEID,
                                                      &card_meid_avail))
  {
    card_meid_avail = FALSE;
  }
  if( meid_nv_data.meid_available && card_meid_avail)
  {
    return TRUE;
  }
  return FALSE;
} /* mmgsdi_util_get_meid_availability */


/*===========================================================================
FUNCTION MMGSDI_UTIL_SEND_RTRE_COMMAND_TO_NV

DESCRIPTION
  Sends RTRE configuration to NV, bypassing CM.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void mmgsdi_util_send_rtre_command_to_nv(
  const nv_item_type     *data_ptr
)
{
  nv_cmd_type             *mmgsdi_nv_cmd_ptr  = NULL;
  mmgsdi_return_enum_type  mmgsdi_status      = MMGSDI_ERROR;
  nv_item_type            *cmd_data_ptr       = NULL;
  int32                    mmgsdi_nv_cmd_size = 0;

  if(data_ptr == NULL)
  {
    return;
  }

  /* Allocate memory for the NV command */
  mmgsdi_util_cal_align_size(sizeof(nv_cmd_type), &mmgsdi_nv_cmd_size);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(mmgsdi_nv_cmd_ptr,
                                     (mmgsdi_nv_cmd_size + sizeof(nv_item_type)));

  if(mmgsdi_nv_cmd_ptr == NULL)
  {
    UIM_MSG_ERR_0("Failed to allocate memory");
    return;
  }

  /* Copy data to write */
  cmd_data_ptr = (nv_item_type*)((uint8*)mmgsdi_nv_cmd_ptr + mmgsdi_nv_cmd_size);
  mmgsdi_memscpy(cmd_data_ptr,
                 sizeof(nv_item_type),
                 data_ptr,
                 sizeof(nv_item_type));

  /* Notify main MMGSDI task when done */
  mmgsdi_nv_cmd_ptr->tcb_ptr    = UIM_GSDI_TCB;
  mmgsdi_nv_cmd_ptr->sigs       = MMGSDI_NV_WRITE_SIG;
  mmgsdi_nv_cmd_ptr->done_q_ptr = &mmgsdi_nv_q;

  /* Item to write */
  mmgsdi_nv_cmd_ptr->item     = NV_RTRE_CONFIG_I;
  mmgsdi_nv_cmd_ptr->cmd      = NV_RTRE_OP_CONFIG_F;
  mmgsdi_nv_cmd_ptr->data_ptr = cmd_data_ptr;

  /* Send Command to NV */
  nv_cmd(mmgsdi_nv_cmd_ptr);

  mmgsdi_status = mmgsdi_nv_put_nv_item_in_cache(
                    NV_RTRE_CONFIG_I,
                    data_ptr,
                    NV_DONE_S,
                    MMGSDI_NV_CONTEXT_PRIMARY);

  UIM_MSG_HIGH_2("RTRE CONFIG NV Status = 0x%x, Cache write status: 0x%x",
                 mmgsdi_nv_cmd_ptr->status, mmgsdi_status);
} /* mmgsdi_util_send_rtre_command_to_nv */


/*===========================================================================
FUNCTION   MMGSDI_UTIL_CONVERT_NV_TO_CM_RTRE

DESCRIPTION
  Convert the RTRE define used by NV to the one used by CM

DEPENDENCIES
  None

RETURNS
  CM RTRE Config

SIDE EFFECTS
  None
===========================================================================*/
cm_rtre_config_e_type mmgsdi_util_convert_nv_to_cm_rtre(
  uint32 nv_rtre
)
{
  cm_rtre_config_e_type cm_rtre;

  switch (nv_rtre)
  {
    case NV_RTRE_CONFIG_SIM_ACCESS:
      cm_rtre = CM_RTRE_CONFIG_SIM_ACCESS;
      break;

    case NV_RTRE_CONFIG_RUIM_ONLY:
      cm_rtre = CM_RTRE_CONFIG_RUIM_ONLY;
      break;

    case NV_RTRE_CONFIG_NV_ONLY:
      cm_rtre = CM_RTRE_CONFIG_NV_ONLY;
      break;

    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
    default:
      cm_rtre = CM_RTRE_CONFIG_RUIM_OR_DROP_BACK;
      break;
  }
  return cm_rtre;
}/* mmgsdi_util_convert_nv_to_cm_rtre */

