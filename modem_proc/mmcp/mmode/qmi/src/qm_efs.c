/*===========================================================================

                        QM_EFS.C

DESCRIPTION

  QMI_MMODE source file for efs module.

Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/qm_efs.c#1 $
  $Change: 46174567 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/27/13    ld     Initial version
===========================================================================*/

#include "qm_efs.h"
#include "fs_lib.h"
#include "sys_v.h"
#include "msg.h"
#include "qmi_voice_cm_if.h"
#include "qmi_voice_cm_util.h"
#include "qmi_mmode_msgr_msg.h"

#include "qmi_mmode_task_cmd.h"

#include "qmi_nas_internal.h"
//===========================================================================
// #defines, Typedefs and Data Structures
//===========================================================================

// filesize = 1 byte, valid values are 0 or 1 binary.
#define EFS_ALWAYS_RET_PLMN_FILE "/nv/item_files/modem/mmode/qmi/always_return_plmn"

#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
// filesize = 1 byte, valid values are 0 or 1 binary.
#define EFS_AUTO_SETUP_RESP "/nv/item_files/modem/mmode/qmi/setup_response"

// filesize = 1 byte, valid values are 0 or 1 binary.
#define EFS_NO_WAIT_RESP "/nv/item_files/modem/mmode/qmi/no_wait_resp"

#endif

#ifdef FEATURE_FEMTO_CSG
#if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
// filesize = 4 bytes, valid values are sys_csg_support_e_type
#define EFS_CSG_SUPPORTED_FILE "/nv/item_files/modem/nas/csg_support_configuration"
#endif
#endif

// filesize = 1 bytes, 1 = feature enabled, 0 or missing = not enabled
#define EFS_ENABLE_DRX_SCALING "/nv/item_files/modem/mmode/qmi/enable_drx_scaling_skip_meas"

#ifndef FEATURE_MMODE_QMI_LTE_ONLY
// filesize = 1 byte, valid values are 0,1,2 binary. (primary sub)
#define EFS_DATA_ROAMING_STATUS_FILE "/nv/item_files/modem/nas/data_roam_status"
#endif

#ifdef FEATURE_DUAL_SIM
// filesize = 1 byte, valid values are 0,1,2 binary. (secondary sub)
#define EFS_DATA_ROAMING_STATUS_FILE_1 "/nv/item_files/modem/nas/data_roam_status_Subscription01"
#endif

#ifdef FEATURE_TRIPLE_SIM    
// filesize = 1 byte, valid values are 0,1,2 binary. (tertiary sub)
#define EFS_DATA_ROAMING_STATUS_FILE_2 "/nv/item_files/modem/nas/data_roam_status_Subscription02"
#endif

#ifndef FEATURE_MMODE_QMI_LTE_ONLY
// filesize = 1 byte, valid values are 0 or 1 binary.
#define EFS_DISABLE_MODEM_CENTRIC_SOLUTION_FILE "/nv/item_files/modem/mmode/disable_global_mode"
#endif

#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
struct qm_efs_cache_s
{
  struct
  {
    boolean inited;
    byte    ui_tty;
  } ui_tty;
};
#endif

struct qm_efs_global_s
{
  struct always_return_plmn_s
  {
    boolean is_file_read;
    boolean flag;
  } always_return_plmn;
#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
  struct setup_autoanswer_s
  {
    boolean is_file_read;
    boolean is_auto_setup_resp_disabled; //If auto answer is disabled, wait for qmi-client to respond to setup
  } mt_setup_resp;

  struct no_wait_resp_s
  {
    boolean is_file_read;
    boolean is_enabled; //If no wait is enabled, send the resp immediately without waiting
  } no_wait_rsp;

  struct csg_supported_s
  {
    boolean                is_file_read;
    sys_csg_support_e_type is_supported;
  } csg_supported;

  struct qm_efs_cache_s cache;
#endif
};

//===========================================================================
// GLOBALS
//===========================================================================

static struct qm_efs_global_s efs_global;

//===========================================================================
// Function bodies
//===========================================================================

/*===========================================================================
  FUNCTION QM_EFS_INIT()

  DESCRIPTION
    This function initializes the efs global variables.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    Initializes all three global variables in efs_global to zero.
===========================================================================*/
void qm_efs_init( void )
{
  memset( &efs_global, 0x00, sizeof(struct qm_efs_global_s) );
#if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
  efs_global.cache.ui_tty.inited = FALSE;
  efs_global.cache.ui_tty.ui_tty = TTY_MODE_OFF_V02;
#endif
} // qm_efs_init()

/*===========================================================================
  FUNCTION QM_EFS_ALWAYS_RETURN_PLMN()

  DESCRIPTION
    Caches and verifies the boolean value contained in the EFS_ALWAYS_RET_PLMN_FILE.

  PARAMETERS
    NONE

  RETURN VALUE
    TRUE  - File was read successfully and contained a valid TRUE  value.
    FALSE - File was read successfully and contained a valid FALSE value, or
            an error encounterred, such as the file does not exist, or
            we do not have permissions to access it, or it contained an invalid
            value.

  DEPENDENCIES
    None

  SIDE EFFECTS
    If the file has not yet been cached, reads the  sets the global variables in efs_global.always_return_plmn
===========================================================================*/
boolean qm_efs_always_return_plmn( void )
{
  int status;

  if ( ! efs_global.always_return_plmn.is_file_read )
  {
    status = efs_get( EFS_ALWAYS_RET_PLMN_FILE, &efs_global.always_return_plmn.flag, (fs_size_t) sizeof(boolean) );

    QM_MSG_HIGH_3("%d = efs_get( %d (T/F), %d bytes )", status, efs_global.always_return_plmn.flag, sizeof(boolean) );

    efs_global.always_return_plmn.is_file_read = TRUE;

    if ( status == sizeof(boolean) )
    {
      if ( efs_global.always_return_plmn.flag != TRUE && efs_global.always_return_plmn.flag != FALSE )
      {
        efs_global.always_return_plmn.flag = FALSE;   // reinitialize corrupted value
      }
    }
    else
    {
      efs_global.always_return_plmn.flag = FALSE;     // Be paranoid, this is just in case get_efs() changes it.
    }
  }

  return efs_global.always_return_plmn.flag;
} //qm_efs_always_return_plmn()

#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
/*===========================================================================
  FUNCTION QM_EFS_IS_AUTO_SETUP_RESPONSE_DISABLED()

  DESCRIPTION
    Qmi-voice default behaviour is to respond to setup_ind 
    for an MT call automatically(without taking respond from client).

    This function returns the status from EFS file if auto setup response is disabled.

  PARAMETERS
    None

  RETURN VALUE
    TRUE if auto setup response is disabled FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_efs_is_auto_setup_response_disabled( void )
{
  int status;

  if ( ! efs_global.mt_setup_resp.is_file_read )
  {
    status = efs_get( EFS_AUTO_SETUP_RESP, &efs_global.mt_setup_resp.is_auto_setup_resp_disabled, (fs_size_t) sizeof(boolean) );

    QM_MSG_HIGH_2("%d = efs_get(AUTO SETUP RESP DISABLED, %d (T/F) )", status, efs_global.mt_setup_resp.is_auto_setup_resp_disabled);

    efs_global.mt_setup_resp.is_file_read = TRUE;

    if ( status == sizeof(boolean) )
    {
      if ( efs_global.mt_setup_resp.is_auto_setup_resp_disabled != TRUE && efs_global.mt_setup_resp.is_auto_setup_resp_disabled != FALSE )
      {
        efs_global.mt_setup_resp.is_auto_setup_resp_disabled = FALSE;   // reinitialize corrupted value
      }
    }
    else
    {
      efs_global.mt_setup_resp.is_auto_setup_resp_disabled = FALSE;
    }
  }

  return efs_global.mt_setup_resp.is_auto_setup_resp_disabled;
} // qm_efs_is_auto_setup_response_disabled()

/*===========================================================================
  FUNCTION QM_EFS_IS_NO_WAIT_ENABLED()

  DESCRIPTION
    This function returns the status of no wait

  PARAMETERS
    None

  RETURN VALUE
    TRUE if no wait is enabled. FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_efs_is_no_wait_rsp_enabled( void )
{
  int status;

  if ( ! efs_global.no_wait_rsp.is_file_read )
  {
    status = efs_get( EFS_NO_WAIT_RESP, &efs_global.no_wait_rsp.is_enabled, (fs_size_t) sizeof(boolean) );

    QM_MSG_HIGH_2("%d = efs_get(NO WAIT RESP, %d (T/F) )", status, efs_global.no_wait_rsp.is_enabled);

    efs_global.no_wait_rsp.is_file_read = TRUE;

    if ( status == sizeof(boolean) )
    {
      if ( efs_global.no_wait_rsp.is_enabled != TRUE && efs_global.no_wait_rsp.is_enabled != FALSE )
      {
        efs_global.no_wait_rsp.is_enabled = FALSE;   // reinitialize corrupted value
      }
    }
    else
    {
      efs_global.no_wait_rsp.is_enabled = FALSE;
    }
  }

  return efs_global.no_wait_rsp.is_enabled;
} // qm_efs_is_no_wait_rsp_enabled()

/*=========================================================================== 
  FUNCTION CSG_SUPPORT_FROM_ENUM
 
  DESCRIPTION
    Static function to translate enum value to whether CSG is supported.
 
  PARAMETERS
    None
 
  RETURN VALUE
    boolean: whether CSG is supported
 
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
static boolean csg_support_from_enum( void )
{
  boolean ret_val = TRUE;

  switch ( efs_global.csg_supported.is_supported )
  {
    case SYS_CSG_SUPPORT_DISABLED:
      ret_val = FALSE;
      break;

    case SYS_CSG_SUPPORT_WCDMA:
    default:
      ret_val = TRUE;
      break;
  }

  return ret_val;
}
#endif

#ifdef FEATURE_FEMTO_CSG
/*===========================================================================
  FUNCTION QM_EFS_CSG_SUPPORTED()

  DESCRIPTION
    This function determines if CSG is supported..

  PARAMETERS
    None

  RETURN VALUE
    boolean: whether CSG is supported

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_efs_csg_supported ( void )
{
#if !(defined(FEATURE_MMODE_QMI_LTE_ONLY) || defined(FEATURE_MMODE_QMI_GPRS_LTE))
  int status;
  boolean ret_val = TRUE;

  if ( !efs_global.csg_supported.is_file_read )
  {
    status = efs_get( EFS_CSG_SUPPORTED_FILE, &efs_global.csg_supported.is_supported, (fs_size_t) sizeof(sys_csg_support_e_type) );

    ret_val = csg_support_from_enum();
    QM_MSG_HIGH_4("%d efs get %d, %d bytes, csg supported %d", status, efs_global.csg_supported.is_supported, sizeof(sys_csg_support_e_type),ret_val);

    efs_global.csg_supported.is_file_read = TRUE;

    if ( status != sizeof(sys_csg_support_e_type) )
    {
      #ifdef FEATURE_DISABLE_CSG_BYDEFAULT
      QM_MSG_HIGH("FEATURE_DISABLE_CSG_BYDEFAULT enabled");
      efs_global.csg_supported.is_supported = SYS_CSG_SUPPORT_DISABLED;
      #else
      efs_global.csg_supported.is_supported = SYS_CSG_SUPPORT_WCDMA;
      #endif
    }
  }

  return csg_support_from_enum();
#else
  return FALSE;
#endif
  
}
#endif

#ifndef FEATURE_MMODE_QMI_DISABLE_VOICE
/*===========================================================================

FUNCTION qm_efs_get_ui_tty_setting()

DESCRIPTION
  Read ui_tty_setting from EFS item

DEPENDENCIES
  None

RETURN VALUE
  boolean : 1 = Get successful
            0 = Error in Get

SIDE EFFECTS
  None.

===========================================================================*/
boolean qm_efs_get_ui_tty_setting(
uint8            *ui_tty_setting_ptr,
                /* Pointer to a ui_tty_setting data.*/
int32           ui_tty_setting_size
                /* Size of the data_ptr buffer.*/
)
{
  boolean                  ret = FALSE; /* some error */
  qmi_voice_efs_id_e_type  efs_id = QMI_VOICE_UI_TTY_SETTING_PRI_F_NAME;

  if ( efs_global.cache.ui_tty.inited )
  {
    *ui_tty_setting_ptr = efs_global.cache.ui_tty.ui_tty;
    ret                 = TRUE;
  }
  else
  {
    memset( ui_tty_setting_ptr, 0x00, ui_tty_setting_size);
    ret = qmi_voice_cm_util_efs_read_per_subs(efs_id, 
                       (uint8*)ui_tty_setting_ptr,
                       ui_tty_setting_size,
                       QMI_VOICE_CM_IF_AS_ID_PRIMARY);

    if( ret == TRUE ) /* No error in EFS read */
    {
      if ( *ui_tty_setting_ptr <= TTY_MODE_OFF_V02 )
      {
        efs_global.cache.ui_tty.ui_tty = *ui_tty_setting_ptr;
        efs_global.cache.ui_tty.inited = TRUE;
      }
      else
      {
        memset( ui_tty_setting_ptr, 0x00, ui_tty_setting_size);
        *ui_tty_setting_ptr = TTY_MODE_OFF_V02;

        ret = qmi_voice_cm_util_efs_write_per_subs( efs_id, 
                           (uint8*)ui_tty_setting_ptr,
                           ui_tty_setting_size,
                           QMI_VOICE_CM_IF_AS_ID_PRIMARY);
        if( ret == TRUE ) /* No error in EFS write */
        {
          efs_global.cache.ui_tty.ui_tty = *ui_tty_setting_ptr;
          efs_global.cache.ui_tty.inited = TRUE;
        }
        else
        {
          QM_MSG_ERROR_1("UI TTY EFS write failed %d", ret);
        }
      }
    }
    else
    {
      QM_MSG_ERROR_1("Failed to read EFS with efs_id %d", efs_id);
    }
  }

  QM_MSG_HIGH_2("UI TTY EFS get ret %d val %d subs %d", ret, efs_global.cache.ui_tty.ui_tty);

  return ret;
}


/*===========================================================================

FUNCTION qm_efs_put_ui_tty_setting()

DESCRIPTION
  Write ui_tty_setting to EFS item

DEPENDENCIES
  None

RETURN VALUE
  boolean : 1 = Put successful
            0 = Error in Put


SIDE EFFECTS
  None.

===========================================================================*/
boolean qm_efs_put_ui_tty_setting(
uint8           ui_tty_setting
                /* UI TTY Setting to write */
)
{
  boolean                                      ret = FALSE; /* some error */
  mmode_qmi_voice_tty_mode_info_cmd_msg_type   msg_ui_tty_pkt;
  errno_enum_type                              msgr_error;
  boolean                                      write_to_efs = TRUE;
  uint8                                        temp_ui_tty_buf;
  qmi_voice_efs_id_e_type                      efs_id = QMI_VOICE_UI_TTY_SETTING_PRI_F_NAME;

  if ( ui_tty_setting <= TTY_MODE_OFF_V02 )
  {
    if(efs_global.cache.ui_tty.inited)
    {
      if(ui_tty_setting == efs_global.cache.ui_tty.ui_tty)
      {
        /* nothing to do as the values are identical */
        write_to_efs = FALSE;
        ret = TRUE;
      }
    }
    else
    {
      /* UI TTY not initialized yet. Read it and check if write is needed */
      temp_ui_tty_buf = 0;
      ret = qmi_voice_cm_util_efs_read_per_subs(efs_id, 
                         (uint8*)&temp_ui_tty_buf,
                         sizeof(temp_ui_tty_buf),
                         QMI_VOICE_CM_IF_AS_ID_PRIMARY);

      if( ret == TRUE ) /* No error in EFS read */
      {
        efs_global.cache.ui_tty.inited = TRUE;
        efs_global.cache.ui_tty.ui_tty = temp_ui_tty_buf;
        if(temp_ui_tty_buf == ui_tty_setting)
        {
          /* nothing to do as the values are identical */
          write_to_efs = FALSE;
          ret = TRUE;
        }
      }
    }

    if(write_to_efs)
    {
      ret = qmi_voice_cm_util_efs_write_per_subs( efs_id,
                                         (uint8*)&ui_tty_setting,
                                         sizeof(uint8),
                                         QMI_VOICE_CM_IF_AS_ID_PRIMARY); /* ui_tty_setting is uint8 per IDL */

      if ( ret == TRUE ) /* No error in efs_write() */
      {
        efs_global.cache.ui_tty.ui_tty    = ui_tty_setting;
        efs_global.cache.ui_tty.inited    = TRUE;

        /* send msgr after this EFS is updated */
        memset( &msg_ui_tty_pkt, 0x00, sizeof(msg_ui_tty_pkt) );

        msgr_init_hdr( (msgr_hdr_struct_type *)&msg_ui_tty_pkt, MSGR_QMI_VOICE, QMI_VOICE_UI_TTY_MODE_SETTING_CMD );

        msg_ui_tty_pkt.tty_mode = (mmode_qmi_voice_tty_mode_e_type)ui_tty_setting;

        msgr_error = msgr_send((msgr_hdr_struct_type*)&msg_ui_tty_pkt, sizeof(mmode_qmi_voice_tty_mode_info_cmd_msg_type));
      
        QM_MSG_HIGH_1("UI TTY Update to IMS/CM: %d", (uint16)msgr_error);
      }
      else
      {
        QM_MSG_ERROR_1("UI TTY EFS write failed %d", ret);
      }
    }
    else
    {
      QM_MSG_HIGH_1("UI TTY EFS write not required for %d", ui_tty_setting);
    }
  }
  else
  {
    QM_MSG_ERROR_1("Invalid ui_tty_setting %d", ui_tty_setting);
  }

  QM_MSG_HIGH_2("UI TTY EFS put ret %d val %d", ret, efs_global.cache.ui_tty.ui_tty);

  return ret;
}
#endif
#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
  FUNCTION QM_EFS_REFRESH_MCFG()

  DESCRIPTION:
  This function updates QMI nas EFS/NV settings after a sim swap

  INPUT/OUTPUT PARAMETERS:
  **TODO subs id** - Subscription ID specific to some multi-subs NV items

  RETURN VALUE:
  None

  SIDE EFFECTS:
  None
===========================================================================*/
void qmi_efs_refresh_mcfg
(
  uint16 subs_index
)
{
  //NVs

  qm_nv_mcfg_refresh_tty(subs_index);

  qmi_voice_cm_if_mcfg_refresh_otasp();

  //EFS
  //csg_supported
  efs_global.csg_supported.is_file_read = FALSE;
  //qm_efs_csg_supported();

}/*qmi_efs_refresh_mcfg*/
#endif // FEATURE_MODEM_CONFIG_REFRESH

/*===========================================================================
  FUNCTION QM_EFS_DRX_SCALING_ENABLED()

  DESCRIPTION:
  This function checks the status of EFS item 
  /nv/item_files/modem/mmode/qmi/enable_drx_scaling_skip_meas

  RETURN VALUE:
  true if file is 1, false if not present or 0

  SIDE EFFECTS:
  None
===========================================================================*/
boolean qmi_efs_drx_scaling_enabled ( void )
{
  int status;
  char enabled;
  
  status = efs_get( EFS_ENABLE_DRX_SCALING, &enabled, (fs_size_t) sizeof(enabled) );
  
  return status == sizeof(enabled) && enabled == 1;
}/*qmi_efs_drx_scaling_enabled*/

#ifndef FEATURE_MMODE_QMI_LTE_ONLY
#if !defined(REMOVE_QMI_NAS_GET_DATA_ROAMING_V01 ) || !defined(REMOVE_QMI_NAS_SET_DATA_ROAMING_V01)
/*===========================================================================
    FUNCTION QM_EFS_READ_DATA_ROAMING_STATUS()
  
    DESCRIPTION:
    This function reads EFS settings related to the data roaming status which will be read by 3GPP NAS during powerup.
  
    INPUT/OUTPUT PARAMETERS:
    **TODO subs id** - Subscription ID specific to some multi-subs NV items
  
    RETURN VALUE:
    None
  
    SIDE EFFECTS:
    None
  ===========================================================================*/

  boolean qm_efs_read_data_roaming_status( int32 as_id, sys_data_roaming_e_type *data_roam_status )
  {
    int status=0;

    switch(as_id)
    {
      case 0:
        status = efs_get( EFS_DATA_ROAMING_STATUS_FILE, (void*)data_roam_status, (fs_size_t) sizeof(sys_data_roaming_e_type) );
        break;
      #ifdef FEATURE_DUAL_SIM
      case 1:
        status = efs_get( EFS_DATA_ROAMING_STATUS_FILE_1, (void*)data_roam_status, (fs_size_t) sizeof(sys_data_roaming_e_type) );
        break;
      #endif
      #ifdef FEATURE_TRIPLE_SIM    
      case 2:
        status = efs_get( EFS_DATA_ROAMING_STATUS_FILE_2, (void*)data_roam_status, (fs_size_t) sizeof(sys_data_roaming_e_type) ); 
        break;
      #endif
      default:
        QM_MSG_HIGH_1("invalid subscription: %d", as_id);
        return FALSE;
    }
    QM_MSG_HIGH_4("status=%d,data roam status =%d,read=%d bytes, sub %d", status, (uint32)data_roam_status, sizeof(sys_data_roaming_e_type), as_id);

    if ( status != sizeof(sys_data_roaming_e_type) )
    {
       return FALSE;
    }
    else
    {
      return TRUE;
    }
  }
#endif

/*===========================================================================
  FUNCTION QM_EFS_MODEM_CENTRIC_SOLUTION_DISABLED()

  DESCRIPTION:
  This function checks the status of EFS item 
  /nv/item_files/modem/mmode/disable_global_mode

  RETURN VALUE:
  true if file is 1, false if not present or 0

  SIDE EFFECTS:
  None
===========================================================================*/
boolean qm_efs_modem_centric_solution_disabled ( void )
{
  int status = 0;
  byte enabled = 0;
  
  status = efs_get( EFS_DISABLE_MODEM_CENTRIC_SOLUTION_FILE, (void*)&enabled, (fs_size_t) sizeof(boolean) );
  if ( status != sizeof(boolean) )
  {
    QM_MSG_ERROR_1("qm_efs_modem_centric_solution_disabled() failed: %d, returning FALSE", status);
    return FALSE;
  }
  return enabled == 1;
}
#endif
