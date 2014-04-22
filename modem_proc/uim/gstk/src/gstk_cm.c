/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                     G S T K   C M

GENERAL DESCRIPTION
  This source file contains functions related CM and GSTK interaction 

FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/gstk/src/gstk_cm.c#1 $ $DateTime: 2020/07/22 10:53:27 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/18   gs      9x05 interface changes for memory reduction

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

#ifdef FEATURE_CDMA
/*===========================================================================
FUNCTION gstk_copy_location_status

DESCRIPTION
  This local function converts CM service status to CM location status and copies
  it in to a variable

PARAMETER
  loc_status      : variable in which converted result to be stored
  srv_status      : CM sys mode
  hdr_personality : Type of HDR

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
static gstk_status_enum_type gstk_copy_location_status(
  gstk_location_status_enum_type*  loc_status_ptr,
  const sys_srv_status_e_type      srv_status)
{
  if (loc_status_ptr == NULL)
  {
    return GSTK_NULL_INPUT_PARAM;
  }

  switch(srv_status)
  {
    case SYS_SRV_STATUS_NO_SRV:
    case SYS_SRV_STATUS_PWR_SAVE:
      *loc_status_ptr = GSTK_NO_SERVICE;
      break;
    case SYS_SRV_STATUS_SRV:
      *loc_status_ptr = GSTK_NORMAL_SERVICE;
      break;
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
      *loc_status_ptr = GSTK_LIMITED_SERVICE;
      break;
    default:
      UIM_MSG_ERR_0("Invalid Srvc status from CM");
      *loc_status_ptr = GSTK_NO_SERVICE;
      break;
  } /* switch(ss_info_ptr->srv_status) */

  return GSTK_SUCCESS;
} /* gstk_copy_location_status */

/*===========================================================================
FUNCTION gstk_copy_advance_info_from_cm

DESCRIPTION
  Copies 1X Advance related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_advance_info_from_cm(
  gstk_cdma_loci_ext_type              * cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type* cm_stack_info_ptr
)
{
  /* check for Null ptr */
  if(cdma_ext_loc_info == NULL || cm_stack_info_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* Height = Height (2 bytes) + Vertical Uncertainty (1 byte) */
  cdma_ext_loc_info->height[0] = cm_stack_info_ptr->rat_mode_info.cdma_mode_info.vert_uncertainty;

  (void)gstk_memcpy((cdma_ext_loc_info->height + 1),
         &(cm_stack_info_ptr->rat_mode_info.cdma_mode_info.height),
         sizeof(uint16),
         sizeof(uint16),
         sizeof(uint16));

  /* Horizontal Uncertainty (1 byte) */
  cdma_ext_loc_info->horizontal_uncertainty = cm_stack_info_ptr->rat_mode_info.cdma_mode_info.horiz_uncertainty;

  return GSTK_SUCCESS;
} /* gstk_copy_advance_info_from_cm */


/*===========================================================================
FUNCTION gstk_copy_1x_info_from_cm

DESCRIPTION
  Copies 1x related system information from the cm ss info ptr to
  the gstk task cmd ptr.

RETURN VALUE
  SUCCESS
  FAIL

DEPENDENCIES
  None
===========================================================================*/
gstk_status_enum_type gstk_copy_1x_info_from_cm(
  gstk_cdma_loci_ext_type               *cdma_ext_loc_info,
  const cm_mm_msim_ss_stack_info_s_type *cm_stack_info_ptr
)
{
  /* check for Null ptr */
  if(cdma_ext_loc_info == NULL || cm_stack_info_ptr == NULL)
  {
    return GSTK_BAD_PARAM;
  }

  /* SID info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.sid_info.sid,
         sizeof(cdma_ext_loc_info->cdma_loc_info.sid_info.sid),
         &(cm_stack_info_ptr->sys_id.id.is95.sid),
         GSTK_MAX_SID_LEN);
  cdma_ext_loc_info->cdma_loc_info.sid_info.id_len = GSTK_MAX_SID_LEN;

  /* NID info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.nid_info.nid,
         sizeof(cdma_ext_loc_info->cdma_loc_info.nid_info.nid),
         &(cm_stack_info_ptr->sys_id.id.is95.nid),
         GSTK_MAX_NID_LEN);
  cdma_ext_loc_info->cdma_loc_info.nid_info.id_len = GSTK_MAX_NID_LEN;

  /* MCC info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.mcc,
         sizeof(cdma_ext_loc_info->cdma_loc_info.mcc),
         &(cm_stack_info_ptr->sys_id.id.is95.mcc),
         GSTK_MCC_LEN);

  /* IMSI_11_12 */
  cdma_ext_loc_info->cdma_loc_info.imsi_11_12  = cm_stack_info_ptr->sys_id.id.is95.imsi_11_12;

  /* BASE_ID info */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.base_id_info.base_id,
         sizeof(cdma_ext_loc_info->cdma_loc_info.base_id_info.base_id),
         &(cm_stack_info_ptr->mode_info.cdma_info.base_id),
         GSTK_MAX_BASE_ID_LEN);
  cdma_ext_loc_info->cdma_loc_info.base_id_info.id_len = GSTK_MAX_BASE_ID_LEN;

  /* BASE Latitude */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.base_lat,
         sizeof(cdma_ext_loc_info->cdma_loc_info.base_lat),
         &(cm_stack_info_ptr->mode_info.cdma_info.base_lat),
         GSTK_BASE_LAT_LEN);

  /* BASE Longitude */
  (void)memscpy(cdma_ext_loc_info->cdma_loc_info.base_long,
         sizeof(cdma_ext_loc_info->cdma_loc_info.base_long),
         &(cm_stack_info_ptr->mode_info.cdma_info.base_long),
         GSTK_BASE_LONG_LEN);

  return gstk_copy_advance_info_from_cm(cdma_ext_loc_info, cm_stack_info_ptr);
} /* gstk_copy_1x_info_from_cm */

/*=============================================================================
FUNCTION: gstk_cm_cmd_cb

DESCRIPTION:
  Command callback from CM to report the status of queuing a request to CM

PARAMETERS:
  *data_block_ptr : [Input]  estk data block pointer
  call_cmd        : [Input]  cm command type
  call_cmd_err    : [Input]  cm command error type
  call_cmd_err_cause : [Input]  cm command error cause
  alpha              : [Input]  cm modified alpha

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

=============================================================================*/
void gstk_cm_cmd_cb (
  void                   *data_block_ptr,
  cm_call_cmd_e_type     call_cmd,
  cm_call_cmd_err_e_type        call_cmd_err,
  cm_call_cmd_err_cause_e_type  call_cmd_err_cause,
  cm_alpha_s_type               alpha
)
{
  gstk_priv_data_type *gstk_inst_ptr = NULL;
  uint32              i              = 0;
  gstk_task_cmd_type  *task_cmd_ptr  = NULL;

  UIM_MSG_HIGH_2("gstk_cm_cmd_cb(): 0x%x, 0x%x", call_cmd, call_cmd_err);

  gstk_inst_ptr = (gstk_priv_data_type *)data_block_ptr;
  for (i = 0; i < gstk_shared_data.gstk_num_of_instances ; ++ i)
  {
    if (gstk_inst_ptr == gstk_instances_ptr[i])
      break;
  }
  if (i == gstk_shared_data.gstk_num_of_instances)
  {
    UIM_MSG_ERR_0("invalid gstk instance ptr!");
    return;
  }

  if (call_cmd == CM_CALL_CMD_ORIG
      &&
      call_cmd_err != CM_CALL_CMD_ERR_NOERR)
  {
      task_cmd_ptr = gstk_task_get_cmd_buf();
      if (task_cmd_ptr != NULL)
      {
        task_cmd_ptr->cmd.general_cmd.message_header.command_group = GSTK_SETUP_OTASP_CALL_CMD;
        task_cmd_ptr->cmd.general_cmd.message_header.command_id = GSTK_OTASP_CALL_CMD_ERROR;
        task_cmd_ptr->cmd.terminal_profile_cmd.message_header.user_data = call_cmd_err;
        gstk_task_put_cmd_buf(task_cmd_ptr);
      }
  }
} /*gstk_cm_cmd_cb*/
#endif /* FEATURE_CDMA */

/*===========================================================================
FUNCTION   gstk_cm_ss_event_cb

DESCRIPTION
  Serving system (SS) event callback function, called when serving system
  information is changed. This function copies the required information
  and puts the command in the GSTK command queue.

PARAMETERS
  ss_event: [Input] CM serving system event
  ss_info_ptr: [Input] Pointer to the ss info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_ss_event_cb (
  cm_ss_event_e_type                ss_event,
  const cm_mm_ss_info_s_type       *ss_info_ptr
)
{
  int                     i                    = 0;
  gstk_task_cmd_type*     task_cmd_ptr         = NULL;
  gstk_slot_id_enum_type  slot                 = GSTK_SLOT_1;
#ifdef FEATURE_CDMA  
  uint64                  changed_fields_mask  = 0;
  uint64                  changed_fields_mask2 = 0;
#endif  
  gstk_status_enum_type   gstk_status          = GSTK_SUCCESS;
  gstk_nv_items_data_type nv_data;
  sys_modem_as_id_e_type  sub_id               = SYS_MODEM_AS_ID_1;

  if (ss_info_ptr == NULL || ss_event != CM_SS_EVENT_SRV_CHANGED)
  {
    UIM_MSG_ERR_2("gstk_cm_ss_event_cb(): 0x%x 0x%d ", ss_info_ptr, ss_event);
    return;
  }

  if (gstk_io_ctrl (GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
                    &sub_id,
                    &slot) != GSTK_SUCCESS)
  {
    return;
  }

  if(!GSTK_IS_VALID_SLOT_ID(slot))
  {
    return;
  }

  /* Read the CAT version */
  (void)gstk_nv_access_read(GSTK_NV_SLOT_CAT_VERSION,
                            slot,
                            &nv_data);

#ifdef FEATURE_CDMA
  GSTK_SET_CM_SS_MODE_CHANGE_MASK(changed_fields_mask, nv_data.cat_version);
  if(gstk_nv_get_feature_status(GSTK_CFG_FEATURE_SLOT_EHRPD_LOC_INFO, slot) == TRUE)
  {
    GSTK_SET_CM_SS_MODE_CHANGE_MASK2(changed_fields_mask2, nv_data.cat_version);
  }


  if (ss_info_ptr->changed_fields & changed_fields_mask)
  { 
    task_cmd_ptr = gstk_task_get_cmd_buf();
    
    if (NULL == task_cmd_ptr)
    {
      break;
    }
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_group =
      GSTK_MODE_CHANGE_IND;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.command_id =
      GSTK_MODE_CHANGE_FROM_CM;
    task_cmd_ptr->cmd.mode_change_ind.msg_hdr.sim_slot_id = slot;
  }


  for (i = 0; NULL != task_cmd_ptr && i < GSTK_CM_NUM_STACKS; ++i)
  {
    if (GSTK_SUCCESS != (gstk_status = gstk_copy_access_tech(
                           &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].access_tech,
                           ss_info_ptr)))
    {
      break;
    }
    if (GSTK_SUCCESS != (gstk_status = gstk_copy_location_status(
                           &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].loc_status,
                           ss_info_ptr->stack_info[i].srv_status)))
    {
      break;
    }

    if (SYS_SYS_MODE_CDMA == ss_info_ptr->stack_info[i].sys_mode)
    {
      if(GSTK_SUCCESS != (gstk_status = gstk_copy_1x_info_from_cm(
                          &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].loc_info.cdma_ext,
                          &ss_info_ptr->stack_info[i])))
      {
        UIM_MSG_ERR_0("Copy 1x info from CM is failed!");
        break;
      }      
    }
    if (SYS_SYS_MODE_HDR == ss_info_ptr->sys_mode)
    {
      if(GSTK_SUCCESS != (gstk_status = gstk_copy_hdr_info_from_cm(
                          &task_cmd_ptr->cmd.mode_change_ind.cm_stack[i].loc_info.cdma_ext,
                          ss_info_ptr)))
      {
        UIM_MSG_ERR_0("Copy HDR info from CM is failed!");
        break;
      }
    }
  } /* for (i = 0 ... */
  if (NULL != task_cmd_ptr)
  {
    if (GSTK_SUCCESS == gstk_status)
    {
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      task_cmd_ptr = NULL;
    }
  }
#endif /* FEATURE_CDMA */

  /* Time zone */
  for(i = 0, task_cmd_ptr = NULL; i < GSTK_CM_NUM_STACKS; ++i)
  {
    if (!(ss_info_ptr->changed_fields & CM_SS_MOBILITY_MGMT_MASK))
    {
      /* Just continue as neither of the interested fields are set */
      continue;
    }
    task_cmd_ptr = gstk_task_get_cmd_buf();

    if (NULL == task_cmd_ptr)
    {
      break;
    }
    task_cmd_ptr->cmd.time_zone_ind.msg_hdr.command_group =
      GSTK_TIME_ZONE_CHANGE_IND;
    task_cmd_ptr->cmd.time_zone_ind.msg_hdr.command_id = 0; /* not used */
    task_cmd_ptr->cmd.time_zone_ind.msg_hdr.sim_slot_id = slot;
    break;
  } /* for(i = 0 ... */

  for (i = 0; NULL != task_cmd_ptr && i < GSTK_CM_NUM_STACKS; ++i)
  {
    gstk_access_technology_type access_tech = GSTK_ACCESS_NONE;

    if (GSTK_SUCCESS != (gstk_status = gstk_copy_access_tech(
                           &access_tech,
                           ss_info_ptr)))
    {
      break;
    }
    task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].access_tech = access_tech;
    switch(access_tech)
    {
      case GSTK_ACCESS_TECH_GSM:
      case GSTK_ACCESS_TECH_UTRAN:
      case GSTK_ACCESS_TECH_LTE:
        if (ss_info_ptr->changed_fields & CM_SS_MOBILITY_MGMT_MASK)
        {
          UIM_MSG_HIGH_4("Access Tech: 0x%x, time_zone_avail: 0x%x, tz: %d, univ_tz_avail: 0x%x",
                         access_tech,
                         ss_info_ptr->mm_information.time_zone_avail,
                         ss_info_ptr->mm_information.time_zone,
                         ss_info_ptr->mm_information.univ_time_and_time_zone_avail);

          if (ss_info_ptr->mm_information.univ_time_and_time_zone_avail)
          {
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].time_data_avail = TRUE;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].year =
              ss_info_ptr->mm_information.univ_time_and_time_zone.year;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].month =
              ss_info_ptr->mm_information.univ_time_and_time_zone.month;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].day =
              ss_info_ptr->mm_information.univ_time_and_time_zone.day;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].hour =
              ss_info_ptr->mm_information.univ_time_and_time_zone.hour;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].minute =
              ss_info_ptr->mm_information.univ_time_and_time_zone.minute;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].second =
              ss_info_ptr->mm_information.univ_time_and_time_zone.second;
          }

          if (ss_info_ptr->mm_information.time_zone_avail)
          {
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_AVAILABLE;
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz =
              ss_info_ptr->mm_information.time_zone;
          }
          else
          {
            task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_NOT_AVAILABLE;
          }
        }
        break;
      case GSTK_ACCESS_TECH_CDMA:
      case GSTK_ACCESS_TECH_HRPD:
      case GSTK_ACCESS_TECH_EHRPD:
#ifdef FEATURE_CDMA
        if (ss_info_ptr->changed_fields & CM_SS_CDMA_TIME_CHGD_MASK)
        {
          UIM_MSG_HIGH_1("Access Tech: 0x%x - Marking TZ as available",
                         access_tech);
          task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_AVAILABLE;
          /* Assign tz to 0xFF, as GSTK doesnt use it in 3GPP2 */
          task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz = 0xFF;
        }
#endif /* FEATURE_CDMA */       
        break;
      default:
        UIM_MSG_HIGH_0("TZ info not available");
        task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz_avail = GSTK_TZ_NOT_AVAILABLE;
        task_cmd_ptr->cmd.time_zone_ind.cm_stack[i].tz = 0xFF;
        break;
    }
  }
  if (NULL != task_cmd_ptr)
  {
    if (GSTK_SUCCESS == gstk_status)
    {
      gstk_task_put_cmd_buf(task_cmd_ptr);
    }
    else
    {
      gstk_task_free_cmd_buf(task_cmd_ptr);
      task_cmd_ptr = NULL;
    }
  }

#ifdef FEATURE_FEMTO_CSG
  /* Check for CSG information change. CSG cell can be camped only on main stack */
  if (ss_info_ptr->changed_fields & CM_SS_CSG_INFO_CHANGED_MASK)
  {
    if (nv_data.cat_version >= GSTK_CFG_CAT_VER9)
    {
      gstk_csg_cm_ss_info_change(ss_info_ptr);
    }
  }
#endif /* FEATURE_FEMTO_CSG */
} /* gstk_cm_ss_event_cb */

/*===========================================================================
FUNCTION gstk_cm_init

DESCRIPTION
  Register GSTK as client of Call Manager.

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  void

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_init( void )
{
  cm_client_status_e_type cm_init_return_status = CM_CLIENT_OK;
  cm_client_status_e_type cm_reg_return_status  = CM_CLIENT_OK;
  bit_mask_256_t          bmask;

  memset(&bmask, 0x00, sizeof(bmask));

  do
  {
    /* client init with CM */
    cm_init_return_status = cm_client_init(CM_CLIENT_TYPE_GSTK,
                                           &gstk_shared_data.gstk_client_id_from_cm);
  
    if (cm_init_return_status != CM_CLIENT_OK)
    {
       break;
    }
  
    (void)cm_client_act(gstk_shared_data.gstk_client_id_from_cm);

    bit_mask_256_clear_all(&bmask);
    (void)bit_mask_256_set_bit(&bmask, CM_SS_EVENT_SRV_CHANGED);

    /* register for CM serving system events */
    cm_reg_return_status = cm_mm_client_ss_reg(
        gstk_shared_data.gstk_client_id_from_cm, /* Client id from CM */
        gstk_cm_ss_event_cb, /* CB function */
        (cm_client_event_reg_e_type)CM_CLIENT_EVENT_REG, /* Register */
        bmask,
        NULL);
  
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      break;
    }

#if defined(FEATURE_UIM_SUPPORT_DUAL_SLOTS) || defined(FEATURE_UIM_SUPPORT_TRIPLE_SLOTS)
    cm_reg_return_status = cm_client_subs_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_subs_evt_cb);
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      UIM_MSG_ERR_0("Failed to register subs evt from CM!");
    }
#endif

    bit_mask_256_clear_all(&bmask);
    (void)bit_mask_256_set_bit(&bmask, CM_PH_EVENT_DUAL_STANDBY_PREF);
    (void)bit_mask_256_set_bit(&bmask, CM_PH_EVENT_SYS_SEL_PREF);
    (void)bit_mask_256_set_bit(&bmask, CM_PH_EVENT_INFO);
    (void)bit_mask_256_set_bit(&bmask, CM_PH_EVENT_AVAILABLE_NETWORKS_CONF);

    cm_reg_return_status = cm_client_ph_reg(
        gstk_shared_data.gstk_client_id_from_cm,
        gstk_cm_ph_event_cb,
        CM_CLIENT_EVENT_REG,
        bmask,
        NULL );
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
      break;
    }

    bit_mask_256_clear_all(&bmask);
    (void)bit_mask_256_set_bit(&bmask, CM_CALL_EVENT_CONNECT);
    (void)bit_mask_256_set_bit(&bmask, CM_CALL_EVENT_END);
#ifdef FEATURE_CDMA
    (void)bit_mask_256_set_bit(&bmask, CM_CALL_EVENT_OTASP_STATUS);
#endif


    /* register for call connected and call end event */
    cm_reg_return_status = cm_mm_client_call_reg(
                          gstk_shared_data.gstk_client_id_from_cm,     /* pointer to the client struct */
                          gstk_cm_call_event_cb,      /* pointer to a cb function for call events */
                          CM_CLIENT_EVENT_REG,        /* event registration */
                          bmask,
                          NULL );                     /* pointer to a cb function for errors */
    if (cm_reg_return_status != CM_CLIENT_OK)
    {
       break;
    }

#ifdef FEATURE_GSM
    cm_reg_return_status = cm_client_stats_reg(
                             gstk_shared_data.gstk_client_id_from_cm,
                             gstk_cm_stats_event_cb,
                             CM_CLIENT_EVENT_REG,
                             CM_STATS_EVENT_MODEM_INFO,
                             CM_STATS_EVENT_MODEM_INFO,
                             NULL);
    if (cm_reg_return_status == CM_CLIENT_OK)
    {
      if (!cm_stats_set_modem_info_mask(NULL,
                                        NULL,
                                        gstk_shared_data.gstk_client_id_from_cm,
                                        SYS_MODEM_MODULE_GSM,
                                        SYS_GERAN_ENG_MODE_SCELL_INFO))
      {
        break;
      }
    }
    else
    {
      break;
    }
#endif /* FEATURE_GSM */
  }while(0);

  if((cm_reg_return_status != CM_CLIENT_OK) ||
     (cm_init_return_status != CM_CLIENT_OK))
  {
    UIM_MSG_ERR_2("Reg Status 0x%x, Init Status 0x%x",
		cm_reg_return_status, cm_reg_return_status);
    return;
  }
/* Call ESTK CM initialization function to allow ESTK to resgister with CM */
#ifdef FEATURE_ESTK
  estk_cm_init();
#endif /* FEATURE_ESTK */
} /* gstk_cm_init */

/*===========================================================================
FUNCTION   gstk_cm_ph_error_event_cb

DESCRIPTION
  Callback function called as immediate response when GSTK sends a request
  for Network Search Mode to CM as part of the Proactive Command Provide Local
  Info.

PARAMETERS
  void*                 data_block_ptr,
  cm_ss_cmd_e_type      cmd,
  cm_ss_cmd_err_e_type  cmd_err

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_ph_error_event_cb (
  void*                 data_block_ptr,
  cm_ph_cmd_e_type      cmd,
  cm_ph_cmd_err_e_type  cmd_err
)
{
  gstk_task_cmd_type * task_cmd = NULL;

  /* Data block pointer should contain the Provide 
     Local Info information */
  if (data_block_ptr == NULL || cmd != CM_PH_CMD_INFO_GET ||
      cmd_err == CM_PH_CMD_ERR_NOERR || 
      (gstk_util_compare_ref_id(*((uint32*)data_block_ptr)) != GSTK_SUCCESS))
  {
    UIM_MSG_ERR_0("Invalid data from CM");
    return;
  }

  /* get the command from buffer */
  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    return;
  }

  /* Build the gstk_cmd */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_group = GSTK_TERMINAL_RSP;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.command_id    =
    int32touint32((int32)GSTK_PROVIDE_LOCAL_INFO_CNF);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.user_data     = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.message_header.sim_slot_id   =
    gstk_curr_inst_ptr->slot_id;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.cmd_details_ref_id           =
    *((uint32*)data_block_ptr);
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_number               =
    command_details_ref_table_ptr[GSTK_CMD_REF_TABLE_INDEX(*((uint32*)data_block_ptr))].command_details.command_number;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.command_result               =
    GSTK_ME_CURRENTLY_UNABLE_TO_PROCESS_COMMAND;
  /* initialize additional info length to zero */
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.length = 0;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.result_additional_info.additional_info_ptr = NULL;
  task_cmd->cmd.provide_local_info_term_rsp_cnf.local_info_rsp.info_type = GSTK_INVALID_LOC_INFO;

  /* put command back to command queue */
  gstk_task_put_cmd_buf(task_cmd);

}/*lint !e818 "data_block_ptr not const . This lint error is suppressed as
cm_ph_cmd_get_ph_info which gstk calls with this callback does not support a const cb type ." */
/* gstk_cm_ph_error_event_cb */

/*===========================================================================
FUNCTION   gstk_cm_ph_event_cb

DESCRIPTION
  Phone (ph) event callback function, called when an ph event is happening.
  This function populates the preferred selection types and
  puts the command in the GSTK command queue.

PARAMETERS
  ph_event: [Input] CM phone event
  ph_info_ptr: [Input] Pointer to the ph info struct

DEPENDENCIES
  gstk command buffer is available

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
void gstk_cm_ph_event_cb (
  cm_ph_event_e_type          ph_event,
  const cm_ph_info_s_type    *ph_info_ptr
)
{
  gstk_task_cmd_type*    task_cmd  = NULL;
  gstk_slot_id_enum_type slot      = GSTK_SLOT_1;
  boolean                queue_cmd = TRUE;
#ifdef FEATURE_FEMTO_CSG                
  uint8                  i         = 0;
#endif
  sys_modem_as_id_e_type sub_id    = SYS_MODEM_AS_ID_1;

  if (ph_info_ptr == NULL)
  {
    return;
  }

  if(CM_PH_EVENT_DUAL_STANDBY_PREF != ph_event)
  {
    if (gstk_io_ctrl(
          GSTK_IO_MODEM_AS_ID_TO_SLOT_ID,
          &sub_id,
          &slot) != GSTK_SUCCESS)
    {
      return;
    }
  }

  UIM_MSG_HIGH_2("gstk_cm_ph_event_cb: event 0x%x nw search mode 0x%x",
                   ph_event, ph_info_ptr->network_sel_mode_pref);

  task_cmd = gstk_task_get_cmd_buf();
  if(task_cmd == NULL)
  {
    return;
  }
  task_cmd->cmd.cm_ph_ind.message_header.command_group =
    GSTK_CM_PH_IND;
  task_cmd->cmd.cm_ph_ind.message_header.command_id = 0; /* doesn't matter */
  task_cmd->cmd.cm_ph_ind.message_header.user_data = 0;  /* doesn't matter */
  task_cmd->cmd.cm_ph_ind.message_header.sim_slot_id = slot;
  task_cmd->cmd.cm_ph_ind.active_data_subs = SYS_MODEM_AS_ID_1; /* Single subscription */

  switch (ph_event)
  {
    case CM_PH_EVENT_SYS_SEL_PREF:
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_SYS_SEL_PREF;
      switch(ph_info_ptr->network_sel_mode_pref)
      {
        case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_AUTOMATIC;
          break;

        case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_MANUAL;
          break;

        default:
          queue_cmd = FALSE;
          break;
      }
      break;

    case CM_PH_EVENT_INFO:
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_INFO;
      switch(ph_info_ptr->network_sel_mode_pref)
      {
        case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_AUTOMATIC;
          break;

        case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          task_cmd->cmd.cm_ph_ind.nw_search_mode = GSTK_NW_SEARCH_MODE_MANUAL;
          break;

        default:
          queue_cmd = FALSE;
          break;
      }
      (void)gstk_memcpy(&task_cmd->cmd.cm_ph_ind.available_networks,
                        &ph_info_ptr->available_networks,
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type));
      break;
    case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
#ifdef FEATURE_FEMTO_CSG              
      task_cmd->cmd.cm_ph_ind.cm_ph_event = GSTK_CM_PH_EVENT_AVAILABLE_NETWORKS_CONF;
      (void)gstk_memcpy(&task_cmd->cmd.cm_ph_ind.available_networks,
                        &ph_info_ptr->available_networks,
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type),
                        sizeof(sys_detailed_plmn_list_s_type));
      
      for(i = 0; i < ph_info_ptr->available_networks.length ; i++)
      {
        UIM_MSG_HIGH_2("CSG ID: 0x%x, HNB Name len: 0x%x",
                      ph_info_ptr->available_networks.info[i].csg_info.csg_id,
                      ph_info_ptr->available_networks.info[i].csg_info.hnb_name.length);
      }
#endif      
      break;

    default:
      queue_cmd = FALSE;
  }
  if(queue_cmd)
  {
    gstk_task_put_cmd_buf(task_cmd);
  }
  else
  {
    gstk_free(task_cmd);
  }
} /* gstk_cm_ph_event_cb */

