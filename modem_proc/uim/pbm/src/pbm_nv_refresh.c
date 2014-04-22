/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 P B M   N V   R E F R E S H  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to handle the NV Refresh
  commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm_nv_refresh.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/16   ssr     Fix Compilation error
02/06/16   nr      ECC only support in PBM
07/21/15   nr      NV Refresh: MCFG - PBM interactions
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "uim_msg.h"
#include "uim_common_efs.h"
#include "pbm_nv_refresh.h"
#include "pbmutils.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
   Function declaration  
=========================================================================*/

boolean pbm_nv_refresh_mcfg_cb(
  const mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
);

/*===========================================================================
FUNCTION PBMI_NV_REFRESH_SLOT_ITEMS

DESCRIPTION
  Function to refresh the slot-specific items from NV/EFS at the index
  corresponding to the slot on which refresh is happening.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_nv_refresh_slot_items(
  mcfg_refresh_index_type    refresh_index)
{
  uint8                        slot_index     = 0;

  switch (refresh_index)
  {
    case MCFG_REFRESH_INDEX_0:
      slot_index   = PBM_SLOT_1_INDEX;
      break;
    case MCFG_REFRESH_INDEX_1:
      slot_index   = PBM_SLOT_2_INDEX;
      break;
    case MCFG_REFRESH_INDEX_2:
      slot_index   = PBM_SLOT_3_INDEX;
      break;
    default:
      return PBM_ERROR;
  }

  (void)pbm_init_nv_ecc_per_slot(slot_index);

  return PBM_SUCCESS;
} /* pbm_nv_refresh_slot_items */


/*===========================================================================
FUNCTION PBM_PROCESS_NV_REFRESH_REQ

DESCRIPTION
  This function processes nv refresh req - slot and session nv/efs items
  are refreshed.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_process_nv_refresh_req(
  const pbm_handle_nv_refresh_req_type *req_ptr)
{
  pbm_return_type       pbm_status    = PBM_ERROR;

  PBM_CHECK_PTR_RET(req_ptr, PBM_ERROR);

  UIM_MSG_HIGH_3("Received NV Refresh of type 0x%x - on slot: 0x%x, sub: 0x%x",
                req_ptr->type, req_ptr->slot_index, req_ptr->sub_index);

  /* Invoke NV refresh procedures here */
  if (req_ptr->type == MCFG_REFRESH_TYPE_SLOT || 
      req_ptr->type == MCFG_REFRESH_TYPE_SLOT_N_SUBS)
  {
    pbm_status = pbm_nv_refresh_slot_items(req_ptr->slot_index);
  }

  /* trigger MCFG that the procedure is done */
  (void)mcfg_refresh_done(req_ptr->reference_id);
  return PBM_SUCCESS;
}/* pbm_process_nv_refresh_req */


/*===========================================================================
FUNCTION PBM_NV_REFRESH_MCFG_CB

DESCRIPTION
  Call back registered with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_nv_refresh_mcfg_cb(
  const mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
)
{
  pbm_handle_nv_refresh_req_type  *pbm_nv_refresh_req_ptr = NULL;

  PBM_CHECK_PTR_RET(mcfg_refresh_info_ptr, FALSE);
  pbm_nv_refresh_req_ptr = (pbm_handle_nv_refresh_req_type *)
                             pbm_cmd_alloc(sizeof(pbm_handle_nv_refresh_req_type));
  if (!pbm_nv_refresh_req_ptr)
  {
    UIM_MSG_ERR_0("Unable to allocate PBM command");
    (void) rex_set_sigs(PBM_TCB, PBM_CMD_COMPLETE_SIG);
    return FALSE;
  }

  pbm_nv_refresh_req_ptr->cmd_type = PBM_MCFG_REFRESH_CB;

  /*store mcfg refresh data */
  pbm_nv_refresh_req_ptr->type         = mcfg_refresh_info_ptr->type;
  pbm_nv_refresh_req_ptr->slot_index   = mcfg_refresh_info_ptr->slot_index;
  pbm_nv_refresh_req_ptr->sub_index    = mcfg_refresh_info_ptr->sub_index;
  pbm_nv_refresh_req_ptr->reference_id = mcfg_refresh_info_ptr->reference_id;

  pbm_mmgsdi_resp_q_put(PBM_CMD_PTR(pbm_nv_refresh_req_ptr));

  UIM_MSG_HIGH_3("PBM MCFG CB: PBM_HANDLE_NV_REFRESH_REQ  type 0x%x, slot 0x%x, sub 0x%x",
                 mcfg_refresh_info_ptr->type,
                 mcfg_refresh_info_ptr->slot_index,
                 mcfg_refresh_info_ptr->sub_index);
  return TRUE;
}/* pbm_nv_refresh_mcfg_cb */


/*===========================================================================
FUNCTION PBM_NV_MCFG_REFRESH_REGISTER

DESCRIPTION
  This function registers for NV refresh notification with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_nv_mcfg_refresh_register(
  void
)
{
  mcfg_refresh_registration_s_type mcfg_reg   = {0};
  boolean                          reg_status = FALSE;

  /* MCFG Registration */
  mcfg_reg.cb        = pbm_nv_refresh_mcfg_cb;
  /* RFU - defaulting it to 0*/
  mcfg_reg.group     = 0;
  mcfg_reg.order     = MCFG_REFRESH_ORDER_50;
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.sub_mask  = MCFG_REFRESH_INDEX_MASK_ALL;

  reg_status = mcfg_refresh_register(&mcfg_reg);

  UIM_MSG_HIGH_1("PBM MCFG Registration status: 0x%x", reg_status);
} /* pbm_nv_mcfg_refresh_register */
#endif /*FEATURE_MODEM_CONFIG_REFRESH*/

