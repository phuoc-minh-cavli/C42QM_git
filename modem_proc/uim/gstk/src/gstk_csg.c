/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   C S G

GENERAL DESCRIPTION
  This source file contains functions to process CSG 
  related command


FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_csg.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/18   gs      9x05 interface changes for memory reduction
09/15/16   gs      Changes in TP based on RAT and App Type
05/31/16   shr     F3 reduction: Dumping Arrays
05/25/16   shr     F3 reduction: Malloc Failure, Memcpy, Null Ptr checks
05/10/16   gm      Fixing compiler warning
04/27/16   gm      Initial Version

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Memory Header Files */
#include "memheap.h"

/* GSTK Header Files   */
#include "gstk.h"
#include "gstk_proactive_cmd.h"
#include "gstkutil.h"
#include "gstk_terminal_rsp.h"
#include "gstk_envelope_cmd.h"
#include "gstk_csg.h"

/* Output & String Header Files */
#include "err.h"
#include "string.h"
#include "uim_msg.h"

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================
FUNCTION gstk_copy_csg_info

DESCRIPTION
  This function copies CSG information

PARAMETER
  task_cmd_ptr       : Pointer to command in which CSG information to be copied
  cm_stack_info_ptr  : Pointer to CM SS stack

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
static void gstk_copy_csg_info(
  gstk_task_cmd_type*                     task_cmd_ptr,
  const cm_mm_ss_info_s_type*             cm_stack_info_ptr
)
{
  if (cm_stack_info_ptr == NULL || task_cmd_ptr == NULL)
  {
    UIM_MSG_ERR_2("Null ptr input 0x%x, 0x%x",
                  cm_stack_info_ptr, task_cmd_ptr);
    return;
  }
  switch(cm_stack_info_ptr->sys_mode)
  {
    case SYS_SYS_MODE_GSM:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.csg_id;
      UIM_MSG_HIGH_2("CSG ID: 0x%x, HNB Name len: %d",
                     cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.csg_id,
                     cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length);
      if (cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length)
        {
          return;
        }
        gstk_util_dump_byte_array("hnb_name.name",
                                  cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.name,
                                  GSTK_MIN(GSTK_HNB_NAME_MAX_LEN, (task_cmd_ptr->cmd.csg_change_ind.cmd.len + 1)));
      }
      break;
    case SYS_SYS_MODE_WCDMA:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.csg_id;
      UIM_MSG_HIGH_2("CSG ID: 0x%x, HNB Name len: %d",
                     cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.csg_id,
                     cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length);
      if (cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.length)
        {
          return;
        }
        gstk_util_dump_byte_array("hnb_name.name",
                                  cm_stack_info_ptr->rat_mode_info.wcdma_mode_info.csg_info.hnb_name.name,
                                  GSTK_MIN(GSTK_HNB_NAME_MAX_LEN, (task_cmd_ptr->cmd.csg_change_ind.cmd.len + 1)));
      }
      break;
    case SYS_SYS_MODE_LTE:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.csg_id;
      UIM_MSG_HIGH_2("CSG ID: 0x%x, HNB Name len: %d",
                     cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.csg_id,
                     cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length);
      if (cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.length)
        {
          return;
        }
        gstk_util_dump_byte_array("hnb_name.name",
                                  cm_stack_info_ptr->rat_mode_info.lte_mode_info.csg_info.hnb_name.name,
                                  GSTK_MIN(GSTK_HNB_NAME_MAX_LEN, (task_cmd_ptr->cmd.csg_change_ind.cmd.len + 1)));
      }
      break;
    case SYS_SYS_MODE_TDS:
      /* Copy CSG ID */
      task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id =
        cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.csg_id;
      UIM_MSG_HIGH_2("CSG ID: 0x%x, HNB Name len: %d",
                     cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.csg_id,
                     cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length);
      if (cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length > 0)
      {
        /* Copy Home (e)NodeB Name length */
        if(cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length <= GSTK_HNB_NAME_MAX_LEN)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len =
            cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length;
        }
        else
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.len = GSTK_HNB_NAME_MAX_LEN;
        }
        /* Copy Home (e)NodeB Name */
        if(gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.hnb_name,
                       cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.name,
                       cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       SYS_HNB_NAME_LEN) <
           cm_stack_info_ptr->rat_mode_info.gsm_mode_info.csg_info.hnb_name.length)
        {
          return;
        }
        gstk_util_dump_byte_array("hnb_name.name",
                                  cm_stack_info_ptr->rat_mode_info.tds_mode_info.csg_info.hnb_name.name,
                                  GSTK_MIN(GSTK_HNB_NAME_MAX_LEN, (task_cmd_ptr->cmd.csg_change_ind.cmd.len + 1)));
      }
      break;
    default:
      break;
  }
}
#endif  

/*===========================================================================
FUNCTION gstk_packer_csg_cell_tlv

DESCRIPTION

  This function populates the passed in csg cell tlv pointer

PARAMETERS
  data                  : [Input] Indicates the battery status
  csg_cell_tlv          : [Input/Output] Pointers to which the battery status needs to be populated

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_csg_cell_tlv(
  sys_detailed_plmn_list_s_type     csg_info,
  gstk_csg_info_tlv_type        *csg_cell_tlv
)
{
#ifdef FEATURE_FEMTO_CSG
  uint8 i, j;
  uint8 length                                = 0;
  uint32 temp_csg_id                          = 0;
  gstk_available_csg_list_type *temp_csg_info = NULL;

  UIM_MSG_HIGH_0("** Packing CSG cell information");

  if(csg_cell_tlv == NULL)
  {
    UIM_MSG_ERR_0("csg_cell_tlv is NULL");
    return GSTK_BAD_PARAM;
  }

  csg_cell_tlv->csg_info = NULL;

  UIM_MSG_HIGH_1("no of available network 0x%x",csg_info.length);

  if(csg_info.length == 0)
  {
    csg_cell_tlv->csg_info = (uint8*)GSTK_CALLOC(UIM_MAX_TR_BYTES * sizeof(byte));
    if( csg_cell_tlv->csg_info == NULL)
    {
      return GSTK_MEMORY_ERROR;
    }

    csg_cell_tlv->csg_info[length++] = 0x80;
    csg_cell_tlv->csg_info[length++] = 0x03;

    length = length + gstk_byte_offset_memcpy(csg_cell_tlv->csg_info,
                                            gstk_curr_inst_ptr->csg_info.plmn_info,
                                            length,
                                            3,
                                            UIM_MAX_TR_BYTES,
                                            3);

    csg_cell_tlv->csg_info[length++] = 0x81;
    csg_cell_tlv->csg_info[length++] = gstk_curr_inst_ptr->csg_info.len + GSTK_CSG_ID_LEN;
    temp_csg_id = gstk_curr_inst_ptr->csg_info.csg_id;
    temp_csg_id = temp_csg_id << 5;
    temp_csg_id = temp_csg_id | 0x1F;
    csg_cell_tlv->csg_info[length + 3] = temp_csg_id & 0xFF;
    csg_cell_tlv->csg_info[length + 2] = (temp_csg_id >> 8) & 0xFF;
    csg_cell_tlv->csg_info[length + 1] = (temp_csg_id >> 16) & 0xFF;
    csg_cell_tlv->csg_info[length]     = (temp_csg_id >> 24) & 0xFF;
    length = length + GSTK_CSG_ID_LEN;

    length = length + gstk_byte_offset_memcpy(csg_cell_tlv->csg_info,
                                            gstk_curr_inst_ptr->csg_info.hnb_name,
                                            length,
                                            gstk_curr_inst_ptr->csg_info.len,
                                            UIM_MAX_TR_BYTES,
                                            GSTK_HNB_NAME_MAX_LEN);

    csg_cell_tlv->tag            = (int)GSTK_CSG_ID_LIST_TAG;
    csg_cell_tlv->length         =  length;
    return GSTK_SUCCESS;
  }

  temp_csg_info = GSTK_CALLOC(sizeof(gstk_available_csg_list_type));
  if(temp_csg_info == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }

  gstk_util_extract_plmn_csg_info(temp_csg_info, &csg_info);

  csg_cell_tlv->csg_info = (uint8*)GSTK_CALLOC(UIM_MAX_TR_BYTES * sizeof(byte));
  if(csg_cell_tlv->csg_info == NULL)
  {
    return GSTK_MEMORY_ERROR;
  }

  for(i = 0; i < temp_csg_info->no_of_plmn_entry; i++)
  {
    csg_cell_tlv->csg_info[length++] = 0x80;
    csg_cell_tlv->csg_info[length++] = 0x03;
    gstk_memcpy(&csg_cell_tlv->csg_info[length],
          temp_csg_info->available_plmn_info[i].plmn,
          3,
          3,
          3);
    length = length + 3;
    for(j = 0; j < temp_csg_info->available_plmn_info[i].no_of_csg_entry; j++)
    {
      csg_cell_tlv->csg_info[length++] = 0x81;
      csg_cell_tlv->csg_info[length++] = temp_csg_info->available_plmn_info[i].csg_info[j].len + GSTK_CSG_ID_LEN;
      temp_csg_id = temp_csg_info->available_plmn_info[i].csg_info[j].csg_id;
      temp_csg_id = temp_csg_id << 5;
      temp_csg_id = temp_csg_id | 0x1F;
      csg_cell_tlv->csg_info[length + 3] = temp_csg_id & 0xFF;
      csg_cell_tlv->csg_info[length + 2] = (temp_csg_id >> 8) & 0xFF;
      csg_cell_tlv->csg_info[length + 1] = (temp_csg_id >> 16) & 0xFF;
      csg_cell_tlv->csg_info[length]     = (temp_csg_id >> 24) & 0xFF;

      length = length + GSTK_CSG_ID_LEN;
      length = length + gstk_byte_offset_memcpy(csg_cell_tlv->csg_info,
               temp_csg_info->available_plmn_info[i].csg_info[j].hnb_name,
                                                length,
               temp_csg_info->available_plmn_info[i].csg_info[j].len,
                                                UIM_MAX_TR_BYTES,
                                                GSTK_HNB_NAME_MAX_LEN);
    }
  }

  csg_cell_tlv->tag            = (int)GSTK_CSG_ID_LIST_TAG;
  csg_cell_tlv->length         =  length;
  gstk_free(temp_csg_info);
  return GSTK_SUCCESS;
#else
  (void) csg_info;
  (void) csg_cell_tlv;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
}

/*===========================================================================
FUNCTION gstk_packer_csg_id_tlv

DESCRIPTION
  This function populates CSG ID information in to required structure

PARAMETERS
  csg_id      [INPUT] Data pointer from which information to be copied
  csg_id_tlv  [INPUT/OUTPUT] Data pointer to which information to be copied

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_csg_id_tlv(
  const uint32*            csg_id,
  gstk_csg_id_tlv_type*    csg_id_tlv
)
{
#ifdef FEATURE_FEMTO_CSG
  uint32 temp_csg_id = 0;

  temp_csg_id = *csg_id;

  UIM_MSG_HIGH_0("** Packing CSG ID");

  csg_id_tlv->tag = GSTK_CSG_ID_TAG;
  csg_id_tlv->csg_id_len = GSTK_CSG_ID_LEN;
  /*CSG ID can be of max lenght 27 bits and as per TS 23.003 setting unused bit to 1*/
  temp_csg_id = temp_csg_id << 5;
  temp_csg_id = temp_csg_id | 0x1F;
  csg_id_tlv->csg_id[3] = temp_csg_id & 0xFF;
  csg_id_tlv->csg_id[2] = (temp_csg_id >> 8) & 0xFF;
  csg_id_tlv->csg_id[1] = (temp_csg_id >> 16) & 0xFF;
  csg_id_tlv->csg_id[0] = (temp_csg_id >> 24) & 0xFF;
  return GSTK_SUCCESS;
#else
  (void) csg_id;
  (void) csg_id_tlv;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
}

/*===========================================================================
FUNCTION gstk_packer_csg_cell_selection_status_tlv

DESCRIPTION
  This function populates CSG cell selection information in to required
  structure

PARAMETERS
  csg_cell_selection_status      [INPUT] Data pointer from which information
                                 to be copied
  csg_cell_selection_status_tlv  [INPUT/OUTPUT] Data pointer to which information
                                 to be copied

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_csg_cell_selection_status_tlv(
  const gstk_cell_selection_status_type*      csg_cell_selection_status,
  gstk_csg_cell_selection_status_tlv_type*    csg_cell_selection_status_tlv
)
{
#ifdef FEATURE_FEMTO_CSG
  UIM_MSG_HIGH_0("** Packing CSG Cell Selection Status");

  csg_cell_selection_status_tlv->tag = GSTK_CSG_CELL_SLECTION_STATUS_TAG;
    csg_cell_selection_status_tlv->selection_status_len =
      GSTK_CSG_CELL_SELECTION_STATUS_LEN;
  csg_cell_selection_status_tlv->selection_status[0] =
    csg_cell_selection_status->general_info.is_camped ? 0x01 : 0x00;
  /* As per spec. CSG cell selection status may include additional info and since
  CM/NAS don't have support to provide selection mode for CSG, Additional information
  will be pass as "00" till support for CSG seleciton mode is added in CM/NAS */
  csg_cell_selection_status_tlv->selection_status[1] = 0x00;
  return GSTK_SUCCESS;
#else
  (void) csg_cell_selection_status;
  (void) csg_cell_selection_status_tlv;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
}

/*===========================================================================
FUNCTION gstk_packer_hnb_name_tlv

DESCRIPTION
  This function populates HNB Name into required structure

PARAMETERS
  hnb_name      [INPUT] Data pointer from which information to be copied
  hnb_name_len  [INPUT] Length of input HNB name
  hnb_name_tlv  [INPUT/OUTPUT] Data pointer to which information to be copied

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_packer_hnb_name_tlv(
  const uint8*               hnb_name,
  uint8                      hnb_name_len,
  gstk_hnb_name_tlv_type*    hnb_name_tlv
)
{
#ifdef FEATURE_FEMTO_CSG
  UIM_MSG_HIGH_0("** Packing HNB Name");

  hnb_name_tlv->tag = GSTK_HNB_NAME_TAG;
  hnb_name_tlv->hnb_name_len = hnb_name_len;
  if(gstk_memcpy(hnb_name_tlv->hnb_name,
                 hnb_name,
                 hnb_name_len,
                 GSTK_HNB_NAME_LEN,
                 hnb_name_len) <
     hnb_name_len)
  {
    UIM_MSG_ERR_0("gstk_packer_hnb_name_tlv(): gstk_memcpy failed");
    return GSTK_MEMORY_ERROR;
  }

  return GSTK_SUCCESS;
#else
  (void) hnb_name;
  (void) hnb_name_len;
  (void) hnb_name_tlv;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
}

/*===========================================================================
FUNCTION gstk_csg_process_cm_ph_event

DESCRIPTION
  This function process the cm_ph event

PARAMETERS
  local_info_ptr      [INPUT] Data pointer which has CSG information

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_csg_process_cm_ph_event(gstk_local_info_rsp_type *local_info_ptr)
{
#ifdef FEATURE_FEMTO_CSG
  gstk_additional_info_type       additional_info   = {0};
  
  local_info_ptr->info_type = GSTK_CSG_CELL_INFO;
  (void)gstk_memcpy(&local_info_ptr->info_data.csg_info,
                &gstk_curr_inst_ptr->available_csg_list,
                sizeof(sys_detailed_plmn_list_s_type),
                sizeof(sys_detailed_plmn_list_s_type),
                sizeof(sys_detailed_plmn_list_s_type));
  gstk_send_provide_local_info_terminal_rsp(
   command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].unique_ref_id,
   command_details_ref_table_ptr[gstk_curr_inst_ptr->in_prog_cmd_ref_id].command_details.command_number,
   GSTK_COMMAND_PERFORMED_SUCCESSFULLY, /* general_result */
   &additional_info, /* additional result */
  local_info_ptr ); /* local info union pointer */
#else
  (void) local_info_ptr;
#endif  
}

/*===========================================================================
FUNCTION gstk_csg_process_cm_csg_change_ind

DESCRIPTION
  This function process the CSG change indication from CM

PARAMETERS
  cmd_ptr:      [INPUT] Data pointer which csg cell information

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_csg_process_cm_csg_change_ind(gstk_cmd_type *cmd_ptr)
{
#ifdef FEATURE_FEMTO_CSG
  gstk_status_enum_type                   gstk_status       = GSTK_SUCCESS;

  /* Check the cached SETUP EVENT LIST to see whether card needs
     CSG Cell Selection changes to be notified */
  if(gstk_curr_inst_ptr->gstk_evt_list[GSTK_CSG_CELL_SELECTION] & 0x01)
  {
    gstk_csg_change_ind_type        csg_evt;
  
    memset(&csg_evt, 0x00, sizeof(gstk_csg_change_ind_type));
  
    UIM_MSG_HIGH_2("CSG: Access Tech 0x%x, CSG_ID: 0x%x",
                   cmd_ptr->csg_change_ind.cmd.access_tech,
                   cmd_ptr->csg_change_ind.cmd.csg_id);
    /* Copy CSG ID */
    csg_evt.cmd.csg_id = cmd_ptr->csg_change_ind.cmd.csg_id;
    /* Copy Home (e)NodeB Name length */
    csg_evt.cmd.len = cmd_ptr->csg_change_ind.cmd.len;
    /* Copy Home (e)NodeB Name */
    if(csg_evt.cmd.len > 0)
    {
      if(gstk_memcpy(csg_evt.cmd.hnb_name,
                     cmd_ptr->csg_change_ind.cmd.hnb_name,
                     csg_evt.cmd.len,
                     GSTK_HNB_NAME_MAX_LEN,
                     GSTK_HNB_NAME_MAX_LEN) <
         csg_evt.cmd.len)
      {
        return GSTK_MEMORY_ERROR;
      }
    }
    /* Copy access tech */
    csg_evt.cmd.access_tech =
      cmd_ptr->csg_change_ind.cmd.access_tech;
  
    /* Copy PLMN Information */
    (void)gstk_memcpy(csg_evt.cmd.plmn_info,
                           cmd_ptr->csg_change_ind.cmd.plmn_info,
                           3,
                           3,
                           3);
  
    /* Calculate CSG cell selection status */
    if(!cmd_ptr->csg_change_ind.cmd.selection_status.general_info.is_camped)
    {
      csg_evt.cmd.selection_status.general_info.is_camped = FALSE;
      csg_evt.cmd.selection_status.addi_info.is_addi_info_present = FALSE;
      if(!gstk_curr_inst_ptr->csg_info.selection_status.general_info.is_camped)
      {
        return gstk_status;
      }
    }
    else
    {
      csg_evt.cmd.selection_status.general_info.is_camped = TRUE;
      csg_evt.cmd.selection_status.addi_info.is_addi_info_present = TRUE;
    }
    if(memcmp(&gstk_curr_inst_ptr->csg_info, &csg_evt.cmd, sizeof(gstk_csg_info_type)) == 0 &&
        gstk_curr_inst_ptr->is_csg_info_sent == TRUE)
    {
      /** same information is already sent **/
      return gstk_status;
    }
    /** Cache the CSG information for future reference **/
    (void)gstk_memcpy(&gstk_curr_inst_ptr->csg_info,
                   &csg_evt.cmd,
                   sizeof(gstk_curr_inst_ptr->csg_info),
                   sizeof(gstk_csg_info_type),
                   sizeof(csg_evt.cmd));

    gstk_curr_inst_ptr->is_csg_info_sent = FALSE;

    /* Build an CSG cell selection envelope to card */
    gstk_status = gstk_build_csg_cell_selection_envelope(&csg_evt);
  
    if (gstk_status != GSTK_SUCCESS)
    {
      UIM_MSG_ERR_0("Problem sending CSG cell selection envelope");
    }
  }
  return gstk_status;
#else
  (void) cmd_ptr;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
}

/*===========================================================================
FUNCTION gstk_csg_cm_ss_info_change

DESCRIPTION
  This function process the CSG change indication from CM

PARAMETERS
  ss_info_ptr:      [INPUT] Data pointer which csg which has CSG cell information

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_csg_cm_ss_info_change(const cm_mm_ss_info_s_type*  ss_info_ptr)
{
#ifdef FEATURE_FEMTO_CSG
   gstk_access_technology_type access_tech = GSTK_ACCESS_NONE;
   gstk_task_cmd_type*     task_cmd_ptr         = NULL;
  gstk_slot_id_enum_type  slot                 = GSTK_SLOT_1;
   sys_modem_as_id_e_type  as_id           = SYS_MODEM_AS_ID_1;

  if (ss_info_ptr == NULL || 
      gstk_io_ctrl (GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                    &as_id,
                    &slot) != GSTK_SUCCESS)
  {
    return;
  }

  UIM_MSG_HIGH_1("CM CSG INFO CHANGE: sys_mode = 0x%x",
                ss_info_ptr->sys_mode);

  /* Queue the changed CSG information for further processing */
  task_cmd_ptr = gstk_task_get_cmd_buf();
  if (task_cmd_ptr != NULL)
  {
    task_cmd_ptr->cmd.csg_change_ind.message_header.command_group =
      GSTK_CM_CSG_CHANGE_IND;
    task_cmd_ptr->cmd.csg_change_ind.message_header.sim_slot_id = slot;
    /* At the moment CSG is maintained only on main stack. It's
       currently not allowed on any of the hybrid stacks */
    if(ss_info_ptr->srv_status == SYS_SRV_STATUS_SRV)
    {
      task_cmd_ptr->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped = TRUE;
      if (GSTK_SUCCESS == gstk_copy_access_tech(
                                           &access_tech,
                                           ss_info_ptr))
      {
        task_cmd_ptr->cmd.csg_change_ind.cmd.access_tech = access_tech;
        gstk_copy_csg_info(task_cmd_ptr, ss_info_ptr);
        if(task_cmd_ptr->cmd.csg_change_ind.cmd.csg_id == SYS_CSG_ID_INVALID)
        {
          task_cmd_ptr->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped = FALSE;
        }
        (void)gstk_memcpy(task_cmd_ptr->cmd.csg_change_ind.cmd.plmn_info,
                              ss_info_ptr->stack_info[0].sys_id.id.plmn.identity,
                              3,
                              3,
                              3);
        gstk_task_put_cmd_buf(task_cmd_ptr);
      }
      else
      {
       gstk_task_free_cmd_buf(task_cmd_ptr);
       task_cmd_ptr = NULL;
      }
    }
    else
    {
       task_cmd_ptr->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped = FALSE;
       gstk_task_put_cmd_buf(task_cmd_ptr);
    }
  }
#else
  (void) ss_info_ptr;  
#endif
}

/*===========================================================================
FUNCTION   gstk_build_csg_cell_selection_envelope

DESCRIPTION
  This function queues CSG cell selection envelope command to GSTK task queue

PARAMETERS


DEPENDENCIES
 None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_build_csg_cell_selection_envelope(
  const gstk_csg_change_ind_type* csg_ind
)
{
#ifdef FEATURE_FEMTO_CSG
  gstk_task_cmd_type* task_cmd = NULL;

  UIM_MSG_HIGH_0("CSG cell selection event download");

  GSTK_RETURN_ERROR_IF_NULL_PTR(gstk_curr_inst_ptr, GSTK_ERROR);
  GSTK_RETURN_ERROR_IF_NULL_PTR(csg_ind, GSTK_ERROR);

  task_cmd = gstk_task_get_cmd_buf();
  if (task_cmd == NULL)
  {
    return GSTK_ERROR;
  }

  task_cmd->cmd.csg_change_ind.message_header.client_id =
    gstk_curr_inst_ptr->client_id;
  task_cmd->cmd.csg_change_ind.message_header.sim_slot_id =
    gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id);
  task_cmd->cmd.csg_change_ind.message_header.command_group = GSTK_ENVELOPE_CMD;
  task_cmd->cmd.csg_change_ind.message_header.command_id =
    (int)GSTK_CSG_CELL_SELECTION_EVT_IND;
  task_cmd->cmd.csg_change_ind.message_header.user_data = 0x00;

  /* Copy CSG related data */
  task_cmd->cmd.csg_change_ind.cmd.csg_id = csg_ind->cmd.csg_id;
  task_cmd->cmd.csg_change_ind.cmd.len = csg_ind->cmd.len;
  if (task_cmd->cmd.csg_change_ind.cmd.len > 0)
  {
    if(gstk_memcpy(task_cmd->cmd.csg_change_ind.cmd.hnb_name,
                   csg_ind->cmd.hnb_name,
                   csg_ind->cmd.len,
                   GSTK_HNB_NAME_MAX_LEN,
                   GSTK_HNB_NAME_MAX_LEN) <
       csg_ind->cmd.len)
    {
      gstk_task_free_cmd_buf(task_cmd);
      return GSTK_MEMORY_ERROR;
    }
  }
  task_cmd->cmd.csg_change_ind.cmd.access_tech =
    csg_ind->cmd.access_tech;
  task_cmd->cmd.csg_change_ind.cmd.selection_status.general_info.is_camped =
    csg_ind->cmd.selection_status.general_info.is_camped;
  task_cmd->cmd.csg_change_ind.cmd.selection_status.addi_info.is_addi_info_present =
    csg_ind->cmd.selection_status.addi_info.is_addi_info_present;

  gstk_task_put_cmd_buf(task_cmd);
  return GSTK_SUCCESS;
#else
  (void) csg_ind;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
} /* gstk_build_csg_cell_selection_envelope */

/*===========================================================================
FUNCTION gstk_csg_evt_ind

DESCRIPTION

  This function builds the CSG Cell Selection event download envelope command
  and sends it to UIM

PARAMETERS
  csg_evt_cmd: [Input] Pointer to the envelope command to be processed

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_csg_evt_ind(
  const gstk_csg_change_ind_type* csg_evt_cmd
)
{
#ifdef FEATURE_FEMTO_CSG
  gstk_status_enum_type                   gstk_status       = GSTK_SUCCESS;
  uim_cmd_type*                           uim_cmd_ptr       = NULL;
  int                                     offset            = 0;
  gstk_envelope_csg_evt_dl_command_type   STK_envelope_cmd;


  if (csg_evt_cmd == NULL)
  {
    UIM_MSG_ERR_0("csg_evt_cmd is NULL");
    return GSTK_BAD_PARAM;
  }

  /* initialize STK_envelope_cmd */
  memset(&STK_envelope_cmd, 0x00, sizeof STK_envelope_cmd);

  /* Pack various TLVs */
  STK_envelope_cmd.event_download_hdr.tag = GSTK_EVENT_DOWNLOAD_TAG;

  /* Pack event list */
  gstk_status = gstk_packer_evt_list_tlv(
    GSTK_CSG_CELL_SELECTION,
    &STK_envelope_cmd.event_download_hdr.evt_list);    
  if(gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* Pack device ID */
  gstk_status = gstk_packer_dev_id_tlv(
     GSTK_ME_DEVICE,
     GSTK_UICC_SIM_DEVICE,
     &STK_envelope_cmd.event_download_hdr.device_id);
  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  if((csg_evt_cmd->cmd.selection_status.general_info.is_camped) &&
     (csg_evt_cmd->cmd.csg_id != SYS_CSG_ID_INVALID))
  {
    /* Pack access tech */
    gstk_status = gstk_packer_access_technology_tlv(
        (uint8*)&csg_evt_cmd->cmd.access_tech,
      &STK_envelope_cmd.access_tech);
    if (gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }
    /* Pack CSG ID */
    gstk_status = gstk_packer_csg_id_tlv(
      &csg_evt_cmd->cmd.csg_id,
      &STK_envelope_cmd.csg_id);
    if (gstk_status != GSTK_SUCCESS)
    {
      return gstk_status;
    }
    if(csg_evt_cmd->cmd.len > 0)
    {
      /* Pack HNB name */
      gstk_status = gstk_packer_hnb_name_tlv(
      csg_evt_cmd->cmd.hnb_name,
      csg_evt_cmd->cmd.len,
      &STK_envelope_cmd.hnb_name);
      if (gstk_status != GSTK_SUCCESS)
      {
         return gstk_status;
      }
    }    
  }
  
  /* Pack CSG Cell Selection status */
  gstk_status = gstk_packer_csg_cell_selection_status_tlv(
    &csg_evt_cmd->cmd.selection_status,
    &STK_envelope_cmd.selection_status);
  if (gstk_status != GSTK_SUCCESS)
  {
    return gstk_status;
  }

  /* total data length:  evt list data + 2 (2 is for the tag and length field)
                       + device id data + 2
                       + access tech data + 2 (optional)
                       + CSG cell selection status data + 2
                       + CSG ID data + 2 (optional)
                       + HNB name data + 2 (optional)
  */

  STK_envelope_cmd.event_download_hdr.length = 
    STK_envelope_cmd.event_download_hdr.evt_list.length + 2
    + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length + 2
    + STK_envelope_cmd.selection_status.selection_status_len + 2;

  /* add lengths of optional fields */
  if((csg_evt_cmd->cmd.selection_status.general_info.is_camped) &&
     (csg_evt_cmd->cmd.csg_id != SYS_CSG_ID_INVALID))
  {
    STK_envelope_cmd.event_download_hdr.length += 
      STK_envelope_cmd.access_tech.length + 2
      + STK_envelope_cmd.csg_id.csg_id_len + 2;
    if(STK_envelope_cmd.hnb_name.hnb_name_len > 0)
    {
      STK_envelope_cmd.event_download_hdr.length += STK_envelope_cmd.hnb_name.hnb_name_len + 2;
    }      
  }

  /* Send cmd to STK application on card */
  uim_cmd_ptr = gstk_get_uim_buf();
  if(uim_cmd_ptr != NULL )
  {
    (void)gstk_util_populate_uim_buf_hdr_info(UIM_ENVELOPE_F, &uim_cmd_ptr);
    /* set to 0 so that uim will get the data from index 0 */
    uim_cmd_ptr->envelope.offset = 0x00;
    /* set user_data */
    uim_cmd_ptr->hdr.user_data = csg_evt_cmd->message_header.user_data;
    /* populate the envelope command info */
    uim_cmd_ptr->envelope.num_bytes = STK_envelope_cmd.event_download_hdr.length +
                                      GSTK_TAG_LENGTH_LEN;
    uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.tag;
    uim_cmd_ptr->envelope.data[offset++] = STK_envelope_cmd.event_download_hdr.length;
    /* copy evt list */
    if(memscpy(&(uim_cmd_ptr->envelope.data[offset]),
               sizeof(uim_cmd_ptr->envelope.data)-offset,
               &STK_envelope_cmd.event_download_hdr.evt_list,
               (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                        GSTK_TAG_LENGTH_LEN)) <
       (size_t)(STK_envelope_cmd.event_download_hdr.evt_list.length))
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }
    /* copy device */
    else if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                    &(STK_envelope_cmd.event_download_hdr.device_id),
                                    offset +=
                                    (int)(STK_envelope_cmd.event_download_hdr.evt_list.length +
                                          GSTK_TAG_LENGTH_LEN),
                                    (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                                             GSTK_TAG_LENGTH_LEN),
                                    sizeof(uim_cmd_ptr->envelope.data),
                                    sizeof(STK_envelope_cmd.event_download_hdr.device_id)) <
            (size_t)(STK_envelope_cmd.event_download_hdr.device_id.device_tag_length +
                     GSTK_TAG_LENGTH_LEN))
    {
      gstk_status = GSTK_MEMORY_ERROR;
    }

    offset = (int)(offset + STK_envelope_cmd.event_download_hdr.device_id.device_tag_length
             + GSTK_TAG_LENGTH_LEN);

    if((gstk_status == GSTK_SUCCESS) &&
       (STK_envelope_cmd.access_tech.length > 0))
    {
      /* copy access tech */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.access_tech,
                                            offset,
                                            (size_t)STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN,
                                            sizeof(uim_cmd_ptr->envelope.data),
                                            sizeof(STK_envelope_cmd.access_tech)) <
         (size_t)(STK_envelope_cmd.access_tech.length + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + STK_envelope_cmd.access_tech.length +
                     GSTK_TAG_LENGTH_LEN);
    }

    /* Copy CSG cell selection status */
    if(gstk_status == GSTK_SUCCESS &&
       STK_envelope_cmd.selection_status.selection_status_len > 0)
    {
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.selection_status,
                                 offset,
                                 (size_t)STK_envelope_cmd.selection_status.selection_status_len + GSTK_TAG_LENGTH_LEN,
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.selection_status)) <
         (size_t)(STK_envelope_cmd.selection_status.selection_status_len + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + STK_envelope_cmd.selection_status.selection_status_len
               + GSTK_TAG_LENGTH_LEN);
    }

    if((gstk_status == GSTK_SUCCESS) &&
       (STK_envelope_cmd.csg_id.csg_id_len > 0))
    {
      /* Copy CSG ID */
      if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.csg_id,
                                 offset,
                                 (size_t)(STK_envelope_cmd.csg_id.csg_id_len + GSTK_TAG_LENGTH_LEN),
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.csg_id)) <
         (size_t)(STK_envelope_cmd.csg_id.csg_id_len + GSTK_TAG_LENGTH_LEN))
      {
        gstk_status = GSTK_MEMORY_ERROR;
      }
      offset = (int)(offset + STK_envelope_cmd.csg_id.csg_id_len
               + GSTK_TAG_LENGTH_LEN);
      /* Copy HNB Name */
      if ((gstk_status == GSTK_SUCCESS) &&
          (STK_envelope_cmd.hnb_name.hnb_name_len > 0))
      {
        if(gstk_byte_offset_memcpy(uim_cmd_ptr->envelope.data,
                                 &STK_envelope_cmd.hnb_name,
                                 offset,
                                 (size_t)(STK_envelope_cmd.hnb_name.hnb_name_len + GSTK_TAG_LENGTH_LEN),
                                 sizeof(uim_cmd_ptr->envelope.data),
                                 sizeof(STK_envelope_cmd.hnb_name)) <
            (size_t)(STK_envelope_cmd.hnb_name.hnb_name_len + GSTK_TAG_LENGTH_LEN))
        {
          gstk_status = GSTK_MEMORY_ERROR;
        }
        offset = (int)(offset + STK_envelope_cmd.hnb_name.hnb_name_len
                       + GSTK_TAG_LENGTH_LEN);
      }
    }

    if (gstk_status == GSTK_SUCCESS)
    {
      gstk_util_dump_byte_array("CSG Cell Selection status evt",
                                &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                                uim_cmd_ptr->envelope.num_bytes);
      if(GSTK_CSG_CELL_SELECTION_EVT_IND_RSP == gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.cmd_type)
      {
        gstk_util_clear_retry_envelope_data(gstk_util_get_sim_slot_id(gstk_curr_inst_ptr->client_id)); 
      } 
 
      if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE)
      {
        gstk_status = gstk_process_and_store_env_cmd_for_retry(
                       &uim_cmd_ptr->envelope.data[uim_cmd_ptr->envelope.offset],
                       (uint32)uim_cmd_ptr->envelope.num_bytes,
                       uim_cmd_ptr->hdr.user_data);
        if( gstk_status == GSTK_SUCCESS)
        {
          gstk_curr_inst_ptr->gstk_curr_envelope_info.gstk_env_state = GSTK_SUB_ST_PENDING_ENV_RSP_FROM_CARD;
        }
        else
        {
          UIM_MSG_HIGH_0("CSG selection env - no retry will be attempted ");
          gstk_status = GSTK_SUCCESS;
        }
      }/* if(gstk_curr_inst_ptr->gstk_curr_envelope_info.retry_info.retry_flag == TRUE) */
      else
      {
        UIM_MSG_HIGH_0("Envelope retry not attempted to the card as flag is set to FALSE");
      }                                
      gstk_send_cmd_to_uim_server(uim_cmd_ptr);
    }
    else
    {
      gstk_send_cmd_to_uim_free_q(uim_cmd_ptr);
      uim_cmd_ptr = NULL;
    }
  }
  else
  {
    gstk_status = GSTK_MEMORY_ERROR;
  }
  return gstk_status;
#else
  (void) csg_evt_cmd;
  return GSTK_UNSUPPORTED_COMMAND;
#endif
} /* gstk_csg_evt_ind */

/*===========================================================================
FUNCTION gstk_util_extract_plmn_csg_info

DESCRIPTION
  This function sends out a DIAG event w.r.t current BIP Status

PARAMETERS
  dest_plmn_csg_info [output]: PLMN and CSG list
  srs_plmn_csg_info   [input]: available network list

DEPENDENCIES
  None

RETURN VALUE
  None

===========================================================================*/
void gstk_util_extract_plmn_csg_info(
  gstk_available_csg_list_type *dest_plmn_csg_info,
  sys_detailed_plmn_list_s_type *srs_plmn_csg_info)
{
#ifdef FEATURE_FEMTO_CSG
  uint8 i, j, k;

  if(dest_plmn_csg_info == NULL || srs_plmn_csg_info == NULL)
    return;

  for(i=0; i < srs_plmn_csg_info->length; i++)
  {
    if(srs_plmn_csg_info->info[i].csg_info.csg_id != SYS_CSG_ID_INVALID)
    {
      for(j=0; j < dest_plmn_csg_info->no_of_plmn_entry; j++)
      {
        if(memcmp(&srs_plmn_csg_info->info[i].plmn.identity,
                  &dest_plmn_csg_info->available_plmn_info[j],
                  3) == 0)
        {
          break;
        }
      }
      if(j == dest_plmn_csg_info->no_of_plmn_entry)
      {
        dest_plmn_csg_info->no_of_plmn_entry++;
        (void)gstk_memcpy(dest_plmn_csg_info->available_plmn_info[j].plmn,
                  srs_plmn_csg_info->info[i].plmn.identity,
                  3,
                  3,
                  3);
      }
      k = dest_plmn_csg_info->available_plmn_info[j].no_of_csg_entry++;
      /* Copy CSG ID */
      dest_plmn_csg_info->available_plmn_info[j].csg_info[k].csg_id =
        srs_plmn_csg_info->info[i].csg_info.csg_id;

       /* Copy Home (e)NodeB Name */
      dest_plmn_csg_info->available_plmn_info[j].csg_info[k].len =
        gstk_memcpy(dest_plmn_csg_info->available_plmn_info[j].csg_info[k].hnb_name,
                       srs_plmn_csg_info->info[i].csg_info.hnb_name.name,
                       srs_plmn_csg_info->info[i].csg_info.hnb_name.length,
                       GSTK_HNB_NAME_MAX_LEN,
                       GSTK_HNB_NAME_MAX_LEN);

        /* Copy access tech */
       dest_plmn_csg_info->available_plmn_info[j].csg_info[k].access_tech =
            srs_plmn_csg_info->info[i].rat;

       /* Calculate CSG cell selection status */
       if(dest_plmn_csg_info->available_plmn_info[j].csg_info[k].access_tech == GSTK_ACCESS_NONE ||
         (int)(dest_plmn_csg_info->available_plmn_info[j].csg_info[k].access_tech) == SYS_CSG_ID_INVALID)
       {
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.general_info.is_camped = FALSE;
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.addi_info.is_addi_info_present = FALSE;
       }
       else
       {
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.general_info.is_camped = TRUE;
         dest_plmn_csg_info->available_plmn_info[j].csg_info[k].selection_status.addi_info.is_addi_info_present = TRUE;
       }
    }
  }
#else
  (void) dest_plmn_csg_info;
  (void) srs_plmn_csg_info; 
#endif
}/* gstk_util_extract_plmn_csg_info */

/*===========================================================================
FUNCTION gstk_check_csg_capability

DESCRIPTION
  This function is used to check whether CSG enabled or disabled

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  boolean

===========================================================================*/
boolean gstk_check_csg_capability(void)
{
#ifdef FEATURE_FEMTO_CSG
  return TRUE;
#else
  return FALSE;
#endif  
} /* gstk_check_csg_capability */

