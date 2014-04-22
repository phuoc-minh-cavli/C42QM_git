/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

	MODEM_CFG processing INTERFACE for default configurations

GENERAL DESCRIPTION

  This file makes up the config processing component of the MODEM_CFG
  for default configurations

EXTERNALIZED FUNCTIONS
    mcfg_default_config_process                    
  
Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE/

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_proc/src/mcfg_default.c#1 $ $DateTime: 2020/02/19 23:07:19 $ $Author: pwbldsvc $ 
 
when       who     what, where, why
--------   ---    ----------------------------------------------------------
02/01/13   cy     Initial Creation.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "nv_items.h"

#include "mcfg_feature_config.h"
#include "mcfg_common.h"
#include "mcfg_nv.h"
#include "mcfg_proc.h"
#include "mcfg_proc_i.h"
#include "mcfg_default.h"
#include "mcfg_int.h"
#include "mcfg_osal.h"
#include "mcfg_utils.h"
#include "mcfg_trl.h"
#include "mcfg_refresh_i.h"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*===========================================================================

            GLOBAL DEFINITIONS AND DECLARATIONS

This section contains global definitions for constants, macros, types,
variables
===========================================================================*/
static boolean default_config_digest_available= FALSE;

/*===========================================================================
=============================================================================
================================= FUNCTIONS =================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION  mcfg_default_config_enabled() 

DESCRIPTION
  This function to check if force load regular configuration is necessary
  when default config is being changed.
 
DEPENDENCIES

RETURN VALUE
  TRUE - if force load required
  FALSE - if force load is not configured

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_default_config_enabled
(
   uint8  type
)
{
  uint8 enabled_mask = 0;
  mcfg_nv_status_e_type status = FALSE;

  /* Get the MCFG version from NV */
  if (!IS_VALID_CONFIG_TYPE(type))
  {
    MCFG_MSG_ERROR_1("Invalid MCFG type = 0x%x", type);
    return FALSE;
  }
  
  status = mcfg_nv_read(MCFG_NV_ITEM_DEF_CONFIG_ENABLE_I, 
                       0, 
                       MCFG_SUB_ID_FIRST,
                       &enabled_mask, 
                       sizeof(enabled_mask));
  if (MCFG_NV_STATUS_OK != status)
  {
    MCFG_MSG_LOW_1("default enabled nv for %d is not available", type);

#ifdef FEATURE_MCFG_DISABLE_DEFAULT_CONFIG_W_NV_INACTIVE
    status = FALSE;
#else
    status = TRUE;
#endif
  }
  else
  {
    status = ((enabled_mask & (1<<type)) > 0);
  }

  return status;
} /* mcfg_default_config_enabled() */

/*===========================================================================

FUNCTION  mcfg_default_config_force_load() 

DESCRIPTION
  This function to check if force load regular or default configuration
  is necessary when config is being changed.
 
DEPENDENCIES

RETURN VALUE
  TRUE - if force load required
  FALSE - if force load is not configured

SIDE EFFECTS
  None

===========================================================================*/
boolean mcfg_default_config_force_load
(
   uint8  type,
   boolean reset_default,
   uint8  subs_id
)
{
  uint8 force_load_mask = 0;
  uint32 nv_mcfg_version = 0; //MCFG_RESTORE_FACT_VALUE;
  uint32 nv_item;
  mcfg_nv_status_e_type nv_status;
  char nv_mcfg_digest_version[MCFG_DIGEST_SIZE];
  memset(nv_mcfg_digest_version, 0, MCFG_DIGEST_SIZE);

  /* Get the MCFG version from NV */
  if (!IS_VALID_CONFIG_TYPE(type))
  {
    MCFG_MSG_ERROR_1("Invalid MCFG type = 0x%x", type);
    return FALSE;
  }
  
  /* MCFG_NV_ITEM_CONFIG_FORCE_LOAD_I and MCFG_NV_ITEM_DEF_CONFIG_FORCE_LOAD_I
     are both bitmasks, bit 0 for HW type and bit 1 for SW type.
     When bit mask is set, MCFG_?W_VERSION_I or
     MCFG_NV_ITEM_DEF_CONFIG_?W_VERSION_I will be reset to factory default
     (0xFACD0000) respectively.
  */
  nv_item = (reset_default) ? MCFG_NV_ITEM_DEF_CONFIG_FORCE_LOAD_I
                            : MCFG_NV_ITEM_CONFIG_FORCE_LOAD_I;

  nv_status = 
    mcfg_nv_read(nv_item, 0, subs_id, &force_load_mask, sizeof(force_load_mask));
  if (nv_status == MCFG_NV_STATUS_NOT_ACTIVE)
  {
    MCFG_MSG_LOW_1("NV inactive for def config type %d, force loading", type);
  }
  else
  {
    if (MCFG_NV_STATUS_OK != nv_status)
    {
      MCFG_MSG_ERROR_2("Failed to read force_load_mask for %d status %d", 
                       type, nv_status);
      return FALSE;
    }

    if ((force_load_mask & (1<<type)) == 0)
    {
      MCFG_MSG_LOW_1("Force load is not required for def config type %d", 
                      type);
      return FALSE;
    }
  }

  if (default_config_digest_available) 
  {
     MCFG_MSG_HIGH_2("force load for digest version... reset_default is (%d)! for type (%d)", reset_default, type);
  if (reset_default)
  {
       nv_item = (type == MCFG_TYPE_SW) ? MCFG_NV_ITEM_MCFG_DEFAULT_SW_CONFIG_DIGEST_VERSION
                                        : MCFG_NV_ITEM_MCFG_DEFAULT_HW_CONFIG_DIGEST_VERSION;
     }
     else
     {
       nv_item = (type == MCFG_TYPE_SW) ? MCFG_NV_ITEM_MCFG_SW_CONFIG_DIGEST_VERSION 
                                        : MCFG_NV_ITEM_MCFG_HW_CONFIG_DIGEST_VERSION;
     }

    if (MCFG_NV_STATUS_OK != mcfg_nv_write(
        nv_item, 0, subs_id, (uint8 *)nv_mcfg_digest_version, MCFG_DIGEST_SIZE))
     {
       MCFG_MSG_ERROR_1("Force load required for digest version, but failed to reset config for %d", 
                        type);
       return FALSE;
     }   
  }

  if (reset_default)
  {
    nv_item = (type == MCFG_TYPE_SW) ? MCFG_NV_ITEM_DEF_CONFIG_SW_VERSION_I
                                     : MCFG_NV_ITEM_DEF_CONFIG_HW_VERSION_I;
  }
  else
  {
    nv_item = (type == MCFG_TYPE_SW) ? MCFG_SW_VERSION_I : MCFG_HW_VERSION_I;
  }

  if (MCFG_NV_STATUS_OK != mcfg_nv_write(
        nv_item, 0, subs_id, (uint8 *)&nv_mcfg_version, sizeof(nv_mcfg_version)))
  {
    MCFG_MSG_ERROR_1("Force load required, but failed to reset config %d", 
                     type);
    return FALSE;
  }

  MCFG_MSG_HIGH_3("Force load for default %d is successful for config type %d sub %d", 
                  reset_default, type, subs_id);
  return TRUE;
} /* mcfg_default_config_force_load() */

/*===========================================================================

FUNCTION  mcfg_default_config_update_required() 

DESCRIPTION 
  This function checks if default config needs to be updated by comparing
  the current and new config versions. 

DEPENDENCIES

RETURN VALUE
  TRUE - if default config update required
  FALSE - if default config already up-to-date  

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_default_config_update_required
(
   uint8  type, 
   uint32 def_mcfg_version,
   uint8  subs_id
)
{
  uint32    cur_mcfg_version;
  mcfg_nv_status_e_type  nv_read_status;

  /* Get the MCFG version from NV */
  if (!IS_VALID_CONFIG_TYPE(type))
  {
    MCFG_MSG_ERROR_1("Invalid MCFG type = 0x%x", type);
    return FALSE;
  }
  
  nv_read_status = mcfg_nv_read((type == MCFG_TYPE_SW)
                                  ? MCFG_NV_ITEM_DEF_CONFIG_SW_VERSION_I 
                                  : MCFG_NV_ITEM_DEF_CONFIG_HW_VERSION_I, 
                               0, 
                               subs_id,
                               (uint8 *)&cur_mcfg_version, 
                               sizeof(cur_mcfg_version));

  if (nv_read_status != MCFG_NV_STATUS_OK && 
      nv_read_status != MCFG_NV_STATUS_NOT_ACTIVE)
  {
    MCFG_MSG_ERROR_1("Failed to read cur MCFG version from NV for %d", type);
    return FALSE;
  }

  MCFG_MSG_HIGH_4("Update default config %d sub %d cur ver %lx, def %lx", 
                  type, subs_id, cur_mcfg_version, def_mcfg_version);

  return(cur_mcfg_version != def_mcfg_version);
} /* mcfg_default_config_update_required() */

/*===========================================================================

FUNCTION  mcfg_default_config_digest_update_required() 

DESCRIPTION 
  This function checks if default config needs to be updated by comparing
  the current and new config digest versions. 

DEPENDENCIES

RETURN VALUE
  TRUE - if default config digest update required
  FALSE - if default config digest already up-to-date  

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_default_config_digest_update_required
(
   uint8  type, 
   mcfg_trl_mcfg_digest_s_type default_mbn_digest,
   uint8  subs_id
)
{
  char curr_default_mbn_digest[MCFG_DIGEST_SIZE];
  memset((void *)curr_default_mbn_digest, 0, MCFG_DIGEST_SIZE);
  mcfg_nv_status_e_type  nv_read_status;

  /* Get the MCFG version from NV */
  if (!IS_VALID_CONFIG_TYPE(type))
  {
    MCFG_MSG_ERROR_1("Invalid MCFG type = 0x%x", type);
    return FALSE;
  }
  
  nv_read_status = mcfg_nv_read((type == MCFG_TYPE_SW)
                                  ? MCFG_NV_ITEM_MCFG_DEFAULT_SW_CONFIG_DIGEST_VERSION 
                                  : MCFG_NV_ITEM_MCFG_DEFAULT_HW_CONFIG_DIGEST_VERSION, 
                               0, 
                               subs_id, 
                               (uint8 *)curr_default_mbn_digest, MCFG_DIGEST_SIZE);

  if (nv_read_status != MCFG_NV_STATUS_OK && 
      nv_read_status != MCFG_NV_STATUS_NOT_ACTIVE)
  {
    MCFG_MSG_ERROR_1("Failed to read cur MCFG digest version from NV for %d", type);
    return FALSE;
  }

  MCFG_MSG_HIGH_1("Update default config digest for type %d ", type);

  return(memcmp(curr_default_mbn_digest, default_mbn_digest.mcfg_digest, MCFG_DIGEST_SIZE) != 0);
} /* mcfg_default_config_digest_update_required() */

/*===========================================================================

FUNCTION  mcfg_default_config_update_version() 

DESCRIPTION
  This function updates the current default config version into NV
  for specified config type

DEPENDENCIES

RETURN VALUE
  TRUE - if NV update is required
  FALSE - if NV already up-to-date  

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_default_config_update_version
(
   uint8  type, 
   uint32 cur_mcfg_version,
   uint8  subs_id
)
{
  /* Get the MCFG version from NV */
  if (!IS_VALID_CONFIG_TYPE(type))
  {
    MCFG_MSG_ERROR_1("Invalid MCFG type = 0x%x", type);
    return FALSE;
  }
  
  if (MCFG_NV_STATUS_OK != mcfg_nv_write((type == MCFG_TYPE_SW)
                          ? MCFG_NV_ITEM_DEF_CONFIG_SW_VERSION_I 
                          : MCFG_NV_ITEM_DEF_CONFIG_HW_VERSION_I, 
                        0, 
                        subs_id,
                        (uint8 *)&cur_mcfg_version, 
                        sizeof(cur_mcfg_version)))
  {
    MCFG_MSG_ERROR_3("Failed to write cur default MCFG version %lx from NV for %d subi %d", 
                     cur_mcfg_version, type, subs_id);
    return FALSE;
  }

  return TRUE;
} /* mcfg_default_config_update_version() */

/*===========================================================================
FUNCTION  mcfg_default_config_update_digest_version() 

DESCRIPTION
  This function updates the current default config digest version into NV
  for specified config type

DEPENDENCIES

RETURN VALUE
  TRUE - if NV update is required
  FALSE - if NV already up-to-date  

SIDE EFFECTS
  None

===========================================================================*/
static boolean mcfg_default_config_update_digest_version
(
   uint8  type, 
   mcfg_trl_mcfg_digest_s_type default_mbn_digest,
   uint8  subs_id
)
{
  /* Get the MCFG version from NV */
  if (!IS_VALID_CONFIG_TYPE(type))
  {
    MCFG_MSG_ERROR_1("Invalid MCFG type = 0x%x", type);
    return FALSE;
  }
  
  if (MCFG_NV_STATUS_OK != mcfg_nv_write((type == MCFG_TYPE_SW)
                          ? MCFG_NV_ITEM_MCFG_DEFAULT_SW_CONFIG_DIGEST_VERSION 
                          : MCFG_NV_ITEM_MCFG_DEFAULT_HW_CONFIG_DIGEST_VERSION, 
                        0, 
                        subs_id,
                        (uint8 *)default_mbn_digest.mcfg_digest, 
                        MCFG_DIGEST_SIZE))
  {
    MCFG_MSG_SPRINTF_2("Failed to write cur MCFG digest %s from NV for %d", 
                     default_mbn_digest.mcfg_digest, type);
    return FALSE;
  }

  return TRUE;
} /* mcfg_default_config_update_digest_version() */

/*===========================================================================

FUNCTION  mcfg_default_config_process() 

DESCRIPTION
  This function processes default configuration for specified config type

DEPENDENCIES
  Requires core service tasks like NV and EFS be available 

RETURN VALUE
  uint32 - Initialization success returns 0, othewise returns the error code.

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcfg_default_config_process
( 
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type subs_id
)
{
  mcfg_config_s_type  config;
  mcfg_config_info_s_type info;
  uint8* def_mcfg_data_ptr;
  uint32 def_mcfg_version;
  uint32 num_mcfg_items;
  uint32 status = MODEM_CFG_PROC_SUCCESS;
  mcfg_trl_mcfg_digest_s_type curr_default_mbn_digest;

/* -------------------------------------------------------------------- */
  MCFG_MSG_LOW_1("==== mcfg_default_config_process for type %d ==== ", type);
  MCFG_ASSERT_COND_RET_ERROR (subs_id < MCFG_NUM_OF_SUB_ID, MODEM_CFG_PROC_LOAD_FAIL);
  memset(&config, 0, sizeof(config));

  do
  {
    if(!mcfg_default_config_enabled(type))
    {
      MCFG_MSG_HIGH_1("Default config update is disabled for type(%d)!", type);
      status = MODEM_CFG_UPDATE_NOT_REQUIRED;
      break;
    }

    if (!mcfg_utils_get_default_config(type, &config))
    {
      MCFG_MSG_ERROR_1("Fail to get def cfg address for type(%d)!", type);
      status = MODEM_CFG_PROC_LOAD_FAIL;
      break;
    }

    if (!config.config_addr)
    {
      MCFG_MSG_ERROR_1("Def cfg address for type(%d) is invalid!", type);
      status = MODEM_CFG_PROC_LOAD_FAIL;
      break;
    }
    default_config_digest_available = mcfg_util_digest_version_compatibility(config.config_addr);
    MCFG_MSG_HIGH_1("default_config_digest_available  = %d", default_config_digest_available);

    if (!mcfg_utils_get_default_config_info(type, &info))
    {
      MCFG_MSG_HIGH_1("Fail to get info for def config type (%d)!", type);
      status = MODEM_CFG_UPDATE_NOT_REQUIRED;
      break;
    }

    if (default_config_digest_available) 
    {
       if (!mcfg_trl_decode_tlv(&info, MCFG_TRL_TLV_ID_MCFG_DIGEST_VERSION, 
             &curr_default_mbn_digest, sizeof(mcfg_trl_mcfg_digest_s_type)))
       {
         MCFG_MSG_ERROR_1("Fail to decode mcfg digest version for (%d)!", type);
         status = MODEM_CFG_PROC_LOAD_FAIL;
         break;
       }
       if (!mcfg_default_config_digest_update_required(type, curr_default_mbn_digest, subs_id))
       {
          MCFG_MSG_HIGH_1("Default config digest update not required for type(%d)!", type);
          status = MODEM_CFG_UPDATE_NOT_REQUIRED;
          break;
       }
    }
    else
    {
    if (!mcfg_trl_decode_tlv(
          &info, MCFG_TRL_TLV_ID_CONFIG_VERSION, 
          &def_mcfg_version, sizeof(def_mcfg_version)))
    {
      MCFG_MSG_ERROR_1("Fail to decode def config version for (%d)!", type);
      status = MODEM_CFG_PROC_LOAD_FAIL;
      break;
    }

       if (!mcfg_default_config_update_required(type, def_mcfg_version, subs_id))
    {
      MCFG_MSG_HIGH_1("Default config update not required for type(%d)!", type);
      status = MODEM_CFG_UPDATE_NOT_REQUIRED;
      break;
    }
    }

    /* Check if there's enough space to process MBN */
    if (mcfg_utils_space_available(type, &config, TRUE) != MCFG_ERR_NONE)
    {
      MCFG_MSG_ERROR_2("No enough EFS space for default cfg_type %d, subs_id %d",type, subs_id);
      status = MODEM_CFG_PROC_LOAD_FAIL;
      break;
    }

    num_mcfg_items = validate_mcfg_header(type, config.config_addr, 
                                          subs_id, 
                                          MCFG_REFRESH_TYPE_SLOT_N_SUBS);
    if (!num_mcfg_items)
    {
      MCFG_MSG_ERROR_1("Num items for def config type(%d) is invalid!", type);
      status = MODEM_CFG_HEADER_INVALID;
      break;
    }

    def_mcfg_data_ptr = (uint8 *)config.config_addr + MCFG_NV_DATA_OFFSET;


    /* Update NVEFS to account for multiSIM items in default configs,  */
    if (!mcfg_update_nvefs(type, num_mcfg_items, def_mcfg_data_ptr, subs_id,
                          MCFG_REFRESH_TYPE_SLOT_N_SUBS ) )
    {
        MCFG_MSG_ERROR_1("Update NVEFS Fail for def cfg type(%d)!", type);
        status = MODEM_CFG_UPDATE_NVEFS_FAIL;
        break;
    }

    if (default_config_digest_available)
    {
       /* Update the correct MCFG digest Version to NV to wrap up */
      if (!mcfg_default_config_update_digest_version(type, curr_default_mbn_digest, subs_id))
      {
        MCFG_MSG_ERROR_1("Update digest version fail for def cfg type(%d)!", type);
        status = MODEM_CFG_VERSION_UPDATE_FAIL;
        break;
      }
    }
    else
    {
    /* Update the correct MCFG Version to NV to wrap up */
       if (!mcfg_default_config_update_version(type, def_mcfg_version, subs_id))
    {
      MCFG_MSG_ERROR_1("Update version fail for def cfg type(%d)!", type);
      status = MODEM_CFG_VERSION_UPDATE_FAIL;
      break;
    }
    }

    /* Update the correct MCFG Version to NV to wrap up */
    if (!mcfg_default_config_force_load(type, FALSE, subs_id))
    {
      MCFG_MSG_HIGH_1("Force load not required for def type(%d)!", type);
    }

    #ifdef FEATURE_MODEM_CFG_HISTORY_LOGGING
    /* For debug purposes, update the details to the MCFG history log */
    if(status == MODEM_CFG_PROC_SUCCESS)
    {
      if(!mcfg_update_history_log(type, def_mcfg_version))
      {
        MCFG_MSG_ERROR( "MCFG default history log unsuccessful");
      }
    }
    #endif /* FEATURE_MODEM_CFG_HISTORY_LOGGING */
  }while(0);

  return status;
} /* mcfg_default_config_process() */

