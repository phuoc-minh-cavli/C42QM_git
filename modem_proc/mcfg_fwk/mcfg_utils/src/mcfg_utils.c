/*==============================================================================

                M O D E M   C O N F I G   U T I L S
 
GENERAL DESCRIPTION
  Utility library for MCFG, including config storage and authentication

Copyright (c) 2012-2013 by QUALCOMM Technologies Incorporated.
"A portion of this file was copied from zlib;
Copyright (C) 1995-2013 Jean-loup Gailly and Mark Adler
This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution."

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/mcfg_fwk.mpss/7.10.1.1/mcfg_utils/src/mcfg_utils.c#1 $ 
$DateTime: 2020/02/19 23:07:19 $ 
$Author: pwbldsvc $ 
$Change: 22644646 $ 

when     who  what, where, why
-------- ---  ----------------------------------------------------------------
05/08/12 cy   Initial creation

==============================================================================*/

#include "comdef.h"

#include "msg.h"
#include "mcfg_osal.h"

#include "mcfg_common.h"
#include "mcfg_sel.h"
#include "mcfg_uim.h"
#include "mcfg_utils.h"
#include "mcfg_proc.h"

#include "mcfg_auth_i.h"
#include "mcfg_imgsel.h"
#include "mcfg_utils_i.h"
#include "mcfg_default.h"
#include "mcfg_efs_i.h"
#include "mcfg_map_i.h"
#include "mcfg_remote_i.h"
#include "mcfg_trl.h"
#include "mcfg_refresh_i.h"
#include "mcfg_nv.h"
#include "mcfg_setting.h"

//#include "dsatapi.h"
#include "sys_m_reset.h"
#include "timer.h"

#include "DALSys.h"

#include "err.h"
#include "fs_errno.h"
#include "fs_public.h"

#include "zlib.h"

/*==============================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
  Constants and Macros
==============================================================================*/
#define DALDEVICEID_MCFG_SW           "/dev/mcfg/sw_default_config"
#define DALDEVICEID_MCFG_HW           "/dev/mcfg/hw_default_config"
#define DALDEVICEID_MCFG_PROP         "CONFIG"
#define DALDEVICEID_MCFG_PROP_SIZE    "CONFIG_SIZE"
#define MCFG_AUTO_DISCOVER_TEST_PATH    "/readonly/firmware/image/modem_pr/mcfg/configs"

/* zlib magic header */
#define MCFG_ZLIB_MAGIC_HEADER_1 0x78 /* Magic zlib buferr part 1. */
#define MCFG_ZLIB_MAGIC_HEADER_2 0x9c /* Magic zlib header part 2. */

/* mbn magic header */
#define MCFG_MBN_MAGIC_HEADER_1 0x7f /* Magic mbn buferr part 1. */
#define MCFG_MBN_MAGIC_HEADER_2 0x45 /* Magic mbn header part 2. */
#define MCFG_MBN_MAGIC_HEADER_3 0x4c /* Magic mbn header part 3. */

/*==============================================================================
  Typedefs
==============================================================================*/

/* Enum for types of config header type */
typedef enum mcfg_config_header_e{
  MCFG_CONFIG_HEADER_NONE = 0,
  MCFG_CONFIG_HEADER_COMPRESSED = 1,
  MCFG_CONFIG_HEADER_UNCOMP_MBN = 2,
  MCFG_CONFIG_HEADER_INVALID
} mcfg_config_header_e_type;

typedef struct mcfg_utils_cmd_data_s {
  mcfg_utils_cmd_e_type cmd_id;
} mcfg_utils_cmd_data_s_type;

typedef PACKED struct 
{ 
  uint32	magic1;
  uint16	mcfg_format_ver_num;
  uint16	mcfg_type;
  uint32	mcfg_num_items;
  uint16	mcfg_muxd_carrier_index_info;
  uint16	spare_crc;
}mcfg_header_type;

/*==============================================================================
  Variables
==============================================================================*/
static timer_type       mcfg_utils_reset_timer;  
static timer_group_type mcfg_utils_reset_timer_group;
#define MCFG_UTILS_RESET_TIMER_TIMEOUT (1) 

#define MCFG_UTILS_RESET_RETRY_TIMEOUT (10) 
#define MCFG_UTILS_RESET_RETRIES (12) 

static timer_group_type mcfg_utils_timer_group;
static timer_type       mcfg_utils_activate_timer;  
#define MCFG_UTILS_ACTIVATE_TIMER_TIMEOUT (1) 

#ifdef FEATURE_MCFG_SSREQ
void mcfg_utils_sys_m_ssreq_reset_cb
(
  ssreq_ind_result  cmd_result
);
#endif
static mcfg_power_down_reason_e_type curr_power_down_reason = MCFG_PWRDOWN_RES_NONE;

/*============================================================================== 
 
                  PRIVATE FUNCTION DEFINITIONS
 
==============================================================================*/
/*===========================================================================

  FUNCTION mcfg_utils_reset_enabled

  DESCRIPTION
    This function checks if the reset is enabled in NV, by default it's enabled.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_reset_enabled
(
)
{
  uint8 reset_disabled = 0;  //reset is enabled if inactive
  mcfg_nv_status_e_type status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = mcfg_nv_read(MCFG_NV_ITEM_AUTOSELECT_BY_UIM, 0, MCFG_SUB_ID_FIRST,
                        &reset_disabled, sizeof(reset_disabled));
  if (status == MCFG_NV_STATUS_OK)
  {
    MCFG_MSG_MED_1("reset_disabled NV=%x", reset_disabled);
  }

  return ((reset_disabled & (1 << 6)) == 0);
}

/*===========================================================================

  FUNCTION mcfg_utils_efs_sync

  DESCRIPTION
    This function force efs sync after MCFG task fully boots up.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_efs_sync()
{
  if (mcfg_task_bootup_completed())
  {
    MCFG_MSG_LOW("Force EFS sync");
    if (efs_sync ("/") < 0)
    {
      MCFG_MSG_ERROR_1("mcfg_utils_efs_sync returned error(%d)",
                       efs_errno);
    }
  }
}

/*===========================================================================

  FUNCTION mcfg_utils_reload_hw

  DESCRIPTION
    This function reload HW config

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_reload_hw
(
  void
)
{
  mcfg_config_id_s_type active_hw_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (mcfg_utils_get_active_config(MCFG_TYPE_HW, &active_hw_id, MCFG_SUB_ID_FIRST))
  {
    MCFG_MSG_HIGH("reload HW config");
    mcfg_utils_deactivate_config(MCFG_TYPE_HW, MCFG_SUB_ID_FIRST, TRUE);
    mcfg_utils_set_selected_config(MCFG_TYPE_HW, &active_hw_id, MCFG_SUB_ID_FIRST);

    return TRUE;
  }

  MCFG_MSG_HIGH("reload default HW config");
  return mcfg_default_config_force_load(MCFG_TYPE_HW, TRUE, MCFG_SUB_ID_FIRST);
}

/*===========================================================================

  FUNCTION mcfg_utils_reload_hw

  DESCRIPTION
    This function checks if the reset is needed for subsidized MBN

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_check_category
(
  mcfg_config_id_s_type *id,
  mcfg_category_bitmask_type bitmask
)
{
  mcfg_config_info_s_type info;
  mcfg_trl_category_s_type category;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);

  if (mcfg_utils_get_config_info(MCFG_TYPE_SW, id, &info))
  {
    if (mcfg_trl_decode_tlv(&info, MCFG_TRL_TLV_ID_CATEGORY, 
             &category, sizeof(mcfg_trl_category_s_type)))
    {
      MCFG_MSG_LOW_2("Category %x vs bitmask %x", category.category, bitmask);
      return ((category.category & bitmask) > 0);
    }
  }

  return FALSE;
}

/*===========================================================================

  FUNCTION mcfg_utils_reset_modem

  DESCRIPTION
    This function uses sys_m sys_m_request_peripheral_restart_ssreq
    for a modem initiated restart.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_reset_modem(mcfg_power_down_reason_e_type res_code)
{
#ifdef FEATURE_MCFG_SSREQ
  ssreq_status_t mcfg_ssreq_status;
  ssreq_qmi_request_res_code_type ssreq_qmi_mapped_res_code;
#endif

  mcfg_utils_efs_sync();

    //Save power down reason
    if (MCFG_NV_STATUS_OK != mcfg_nv_write(MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON,
                                           0, MCFG_SUB_ID_FIRST, &res_code, sizeof(res_code)))
    {
      MCFG_MSG_ERROR_1("Failed to write mcfg_power_down_reason, res_code was : %d ",res_code);
    }
    
#ifdef FEATURE_MCFG_SSREQ
  ssreq_qmi_mapped_res_code = mcfg_to_ssreq_qmi_map_res_code(res_code);      
  MCFG_MSG_MED_1("Reset modem through sys_m_req_pp_restart_ssreq_with_res_code 0x%x", ssreq_qmi_mapped_res_code);
#ifdef FEATURE_MCFG_SSREQ_W_REASON
  mcfg_ssreq_status = sys_m_req_pp_restart_ssreq_with_res_code(
                        &mcfg_utils_sys_m_ssreq_reset_cb, ssreq_qmi_mapped_res_code);    
#else
  mcfg_ssreq_status = sys_m_request_peripheral_restart_ssreq(&mcfg_utils_sys_m_ssreq_reset_cb);    
#endif

 if ( mcfg_ssreq_status == SSREQ_SUCCESS)
  {
    MCFG_MSG_MED("Sys M request queued for graceful modem restart, wait .. ");
	return;
  }
  else if (mcfg_ssreq_status == SSREQ_IN_PROCESS)
  {
    MCFG_MSG_MED("Sys M request already queued for graceful modem restart, wait .. ");
	return;
  }
  else
  {
    if (res_code == MCFG_PWRDOWN_RES_SEGMENT_LOADING)
    {
      MCFG_MSG_ERROR_1("SSR request failed(error 0x%x). Reset modem for segment loading through 'sys_m_initiate_shutdown()'.", mcfg_ssreq_status);
    }
    else
    {
      MCFG_MSG_ERROR_1("SSR request failed(error 0x%x). Reset modem for MBN activation through 'sys_m_initiate_shutdown()'.", mcfg_ssreq_status);
    }
  }
#else
  if (res_code == MCFG_PWRDOWN_RES_SEGMENT_LOADING)
  {
    MCFG_MSG_HIGH("SSR not enabled/supported. Reset modem for segment loading through 'sys_m_initiate_shutdown()'.");
  }
  else
  {
    MCFG_MSG_HIGH("SSR not enabled/supported. Reset modem for MBN activation through 'sys_m_initiate_shutdown()'.");
  }
#endif 
  sys_m_initiate_shutdown();

} /* mcfg_utils_reset_modem() */

#ifdef  FEATURE_MCFG_SSREQ
/*===========================================================================

  FUNCTION mcfg_utils_sys_m_ssreq_reset_cb

  DESCRIPTION
    SYS M Restart callback. 

  DEPENDENCIES
    None

  PARAMETERS
    cmd_status    [in] command status

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_sys_m_ssreq_reset_cb
(
  ssreq_ind_result  cmd_result
)
{
  static int retries = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd_result == SSREQ_REQUEST_SERVED)
  {
    MCFG_MSG_HIGH_1("SSREQ_REQUEST_SERVED retry %d time", retries);
  }
  else
  {
    MCFG_MSG_ERROR_1("SSREQ_REQUEST_NOT_SERVED retry %d time", retries);
  }
  if (retries++ < MCFG_UTILS_RESET_RETRIES)
  {
    // Try again for race condition at power up
    mcfg_utils_cmd_send(MCFG_UTILS_CMD_RESET_RETRY);
  }
  else
  {
    MCFG_MSG_ERROR_1("SSR failed after %d times retry. Reset modem for MBN activation or segment loading through 'sys_m_initiate_shutdown()'.", retries);
	sys_m_initiate_shutdown();
  }
} /* mcfg_utils_sys_m_ssreq_reset_cb() */

#endif /*FEATURE_MCFG_SSREQ*/

/*===========================================================================

  FUNCTION mcfg_utils_cmd_reset_timer_cb

  DESCRIPTION
    RESET Timer callback. 

  DEPENDENCIES
    None

  PARAMETERS
    data    [in]  timer callback data

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_utils_cmd_reset_timer_cb(timer_cb_data_type data)
{
  mcfg_utils_cmd_send(MCFG_UTILS_CMD_RESET_NOW);
} /* mcfg_utils_cmd_reset_timer_cb() */

/*===========================================================================

  FUNCTION mcfg_utils_cmd_debounce_timer_cb

  DESCRIPTION
    Debouncing Timer callback. 

  DEPENDENCIES
    None

  PARAMETERS
    data    [in]  timer callback data

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
static void mcfg_utils_cmd_debounce_timer_cb(timer_cb_data_type data)
{
  mcfg_utils_cmd_send(MCFG_UTILS_CMD_ACTIVATE_NOW);
} /* mcfg_utils_cmd_debounce_timer_cb() */

/*===========================================================================

  FUNCTION mcfg_utils_cmd_send

  DESCRIPTION
    This function sends mcfg utils commands for processing. 

  DEPENDENCIES
    None

  PARAMETERS
    utils_cmd    [in]  utils commands

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_cmd_send(mcfg_utils_cmd_e_type utils_cmd)
{
  mcfg_task_cmd_s_type *cmd;
  mcfg_utils_cmd_data_s_type *utils_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmd = mcfg_task_cmd_alloc(MCFG_TASK_CMD_UTILS, 
                            sizeof(mcfg_utils_cmd_data_s_type));
  if (cmd != NULL)
  {
    utils_cmd_data = (mcfg_utils_cmd_data_s_type *) cmd->data;
    utils_cmd_data->cmd_id = utils_cmd;
    if (!mcfg_task_cmd_send(cmd))
    {
      MCFG_MSG_ERROR("Couldn't send command to RESET!");
      mcfg_task_cmd_free(cmd);
    }
    else
    {
      MCFG_MSG_HIGH_1("Utils command sent %d", utils_cmd_data->cmd_id);
    }
  }
} /* mcfg_utils_cmd_send() */


/*==============================================================================
                    
                    PUBLIC FUNCTION DEFINITIONS
                    
==============================================================================*/

/*===========================================================================

  FUNCTION mcfg_utils_alloc_config_buffer

  DESCRIPTION
    This function is used to return a pointer to memory allocated to store 
    a new config file onto the device.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]       type of the new config to store
    config [in/out]   config structure including size to be allocated and the 
                      config buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_alloc_config_buffer
(
  mcfg_config_type_e_type type,
  mcfg_config_s_type     *config
)
{
  boolean status = FALSE;

  MCFG_CHECK_NULL_PTR_RET_FALSE(config);

  if (config->config_len <= MCFG_CONFIG_SIZE_MAX)
  {
    config->config_addr = mcfg_malloc(config->config_len);
    if (config->config_addr)
    {
      status = TRUE;
    }
  }

  return status;
} /* mcfg_utils_get_config_memory_ptr() */


/*===========================================================================

  FUNCTION mcfg_utils_free_config_buffer

  DESCRIPTION
    This function is used to free memory that was allocated to store
    a new config file.

  DEPENDENCIES
    None

  PARAMETERS
    config [in/out] config structure including size allocated and
                    the config buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_free_config_buffer
(
  mcfg_config_s_type *config
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config->config_addr);

  mcfg_free(config->config_addr);
  config->config_len = 0;

  if(config->comp_config_addr != NULL && config->comp_config_len != 0)
  {
    mcfg_free(config->comp_config_addr);
    config->comp_config_len = 0;
  }

  return TRUE;
} /* mcfg_utils_free_config_buffer() */

/*===========================================================================

  FUNCTION mcfg_utils_free_comp_config_buffer

  DESCRIPTION
    This function is used to free memory that was allocated to store
    a new config file.

  DEPENDENCIES
    None

  PARAMETERS
    config [in/out] config structure including size allocated and
                    the config buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_free_comp_config_buffer
(
  mcfg_config_s_type *config
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config->config_addr);

  if(config->comp_status == MCFG_COMP_STATUS_UNCOMP_AND_SWAPPED \
     && config->config_addr != NULL && config->config_len != 0)
  {
    mcfg_free(config->config_addr);
    config->config_len = 0;
  }
  else if(config->comp_config_addr != NULL && config->comp_config_len != 0)
  {
    mcfg_free(config->comp_config_addr);
    config->comp_config_len = 0;
  }

  config->comp_status = MCFG_COMP_STATUS_NONE;

  return TRUE;
} /* mcfg_utils_free_comp_config_buffer() */

/*===========================================================================

  FUNCTION mcfg_utils_is_config_id_unique

  DESCRIPTION
    This function returns whether the specified unique ID exists on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type    [in]  type of ID to query
    id      [in]  unique ID struct to fill with selected config

  RETURN VALUE
    TRUE if it's unique and doesn't exist, FALSE if it exists

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_is_config_id_unique
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);

  return mcfg_map_is_config_id_unique(type, id);
} /* mcfg_utils_is_config_id_unique() */

/*===========================================================================

  FUNCTION mcfg_utils_config_id_matches

  DESCRIPTION
    This function returns whether the two specified IDs match or not.

  DEPENDENCIES
    None

  PARAMETERS
    id1     [in]  config ID struct
    id2     [in]  config ID struct

  RETURN VALUE
    TRUE if two IDs match, FALSE if not

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_config_id_matches
(
  mcfg_config_id_s_type  *id1,
  mcfg_config_id_s_type  *id2
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id1);
  MCFG_CHECK_NULL_PTR_RET_FALSE(id2);

  if (id1->id_len != id2->id_len 
      || (memcmp(id1->id, id2->id, id1->id_len) != 0))
  {
    return FALSE;
  }
  return TRUE;
} /* mcfg_utils_config_id_matches() */

/*===========================================================================

  FUNCTION mcfg_utils_list_configs

  DESCRIPTION
    This function returns a list of unique ID's of the specified type that are
    available.

  DEPENDENCIES
    None

  PARAMETERS
    type          [in]  type of ID to query (max returns all types)
    max_list_size [in]  size of the id_list array (number of elements)
    id_list[]    [out] list of unique ID struct to fill with selected config

  RETURN VALUE
    number of configs

  SIDE EFFECTS
    None

===========================================================================*/
uint32 mcfg_utils_list_configs
(
  mcfg_config_type_e_type type,
  int                     max_list_size,
  mcfg_config_id_s_type  *id_list
)
{
  MCFG_CHECK_NULL_PTR_RET_ZERO(id_list);

#ifdef MCFG_DEBUG
  mcfg_map_print_map(TRUE);
#endif /* MCFG_DEBUG */

  return mcfg_map_list_configs(type, max_list_size, id_list);
} /* mcfg_utils_list_configs() */

/*===========================================================================

  FUNCTION mcfg_utils_space_available

  DESCRIPTION
    This function is used to check whether there's enough space left on device
    for requested size

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
 	  mcfg_error_e

===========================================================================*/
mcfg_error_e_type mcfg_utils_space_available
(
  mcfg_config_type_e_type type,
  mcfg_config_s_type *config,
  boolean processing
)
{
  mcfg_error_e_type error = MCFG_ERR_NONE;
  uint64 max_size;
  uint64 curr_size;
  uint64 available;
  int    request_size;
  struct  fs_statvfs stat_vfs;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);

  if (processing)
  {
    if (!mcfg_efs_statvfs(&stat_vfs))
    {
      MCFG_MSG_ERROR("mcfg_efs_statvfs failed");
      return MCFG_ERR_INTERNAL;
    }

    if (stat_vfs.f_bavail * stat_vfs.f_bsize <= config->config_len)
    {
      MCFG_MSG_ERROR_2("%lu space available, not enough for mbn with size of %d", 
                       stat_vfs.f_bavail * stat_vfs.f_bsize, config->config_len);
      return MCFG_ERR_OUT_OF_SPACE;
    }

    return MCFG_ERR_NONE;
  }

  if(config->comp_status == MCFG_COMP_STATUS_COMPRESSED || \
     config->comp_status == MCFG_COMP_STATUS_UNCOMP_AND_SWAPPED)
  {
    request_size = config->comp_config_len;
  }
  else
  {
    request_size = config->config_len;
  }

  if(!mcfg_map_get_config_limits(type, &max_size, &curr_size))
  {
    MCFG_MSG_ERROR("space_available: Failed to get config limit");
    return MCFG_ERR_OUT_OF_SPACE;
  }

  if (max_size < curr_size)
  {
    return MCFG_ERR_OUT_OF_SPACE;
  }

  MCFG_MSG_LOW_2("space_available max_size %d, curr_size %d",
                   max_size, curr_size);

  available = max_size - curr_size;

  MCFG_MSG_LOW_2("space_available: space %d left for requested size %d", 
                   available, request_size);
  if(available < request_size)
  {
    MCFG_MSG_ERROR_2("space_available: no enough space %llu for requested size %d", 
                     available, request_size);
    error = MCFG_ERR_OUT_OF_SPACE;
  }

  return error;
} /* mcfg_utils_space_available() */

/*===========================================================================

  FUNCTION mcfg_utils_print_config

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_print_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id
)
{
  mcfg_config_info_s_type info;
  char name[MCFG_TRL_CARRIER_NAME_MAX_LEN+1];

  MCFG_CHECK_NULL_PTR_RET(id);

  if(mcfg_utils_get_config_info(type, id, &info))
  {
    if (mcfg_trl_decode_tlv(
          &info, MCFG_TRL_TLV_ID_CARRIER_NAME, 
          name, MCFG_TRL_CARRIER_NAME_MAX_LEN+1))
    {
      MCFG_MSG_SPRINTF_1("Config: %s", name);
    }
  }
}

/*===========================================================================

  FUNCTION mcfg_utils_add_config

  DESCRIPTION
    This function is used to store a new config file on the device.  

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer
    storage [in] storage mode

  RETURN VALUE
 	  mcfg_error_e

  SIDE EFFECTS
    Config file is stored into device for efs mode, or authenticated for remote
    mode

===========================================================================*/
mcfg_error_e_type mcfg_utils_add_config
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_s_type      *config,
  mcfg_storage_mode_e_type storage 
)
{
  mcfg_error_e_type error = MCFG_ERR_NONE;
  uint64 max_size;
  uint64 curr_size;
  mcfg_config_info_s_type info;
  mcfg_trl_config_version_s_type version;

  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(config, MCFG_ERR_NULL_POINTER);

  if (!mcfg_utils_is_config_id_unique(type, id))
  {
    MCFG_MSG_ERROR("mcfg_efs_add_config:ID is not unique");
    return MCFG_ERR_DUPLICATE_ID;
  }

  if(!mcfg_map_get_config_limits(type, &max_size, &curr_size))
  {
    MCFG_MSG_ERROR("mcfg_efs_add_config: Failed to get config limit");
    return MCFG_ERR_OUT_OF_SPACE;
  }

  //authentication is done in the sub routine
  if (storage == MCFG_STORAGE_LOCAL)
  {
    if(!mcfg_utils_check_and_update_config(config, MCFG_CONFIG_SIZE_MAX))
    {
      MCFG_MSG_ERROR_1("mcfg_utils_add_config: failed check_and_update_config, len %d", MCFG_CONFIG_SIZE_MAX);
      error = MCFG_ERR_INTERNAL;
      return error;
    }
    error = mcfg_efs_add_config(type, id, config);
  }
#ifdef FEATURE_MCFG_REMOTE_STORAGE
  else if (storage == MCFG_STORAGE_REMOTE)
  {
      error = mcfg_remote_add_config(type, id, config);
  }
#endif
  else
  {
    MCFG_MSG_ERROR_1("mcfg_utils_add_config storage mode %d not supported", 
                      storage);
    error = MCFG_ERR_NOT_SUPPORTED;
  }
    
  if (type == MCFG_TYPE_SW && error == MCFG_ERR_NONE)
  {
    mcfg_sel_sw_config_store_updated(MCFG_NUM_OF_UIM_SLOT);
  }

  return error;
} /* mcfg_utils_add_config() */


/*===========================================================================

  FUNCTION mcfg_utils_delete_config_ext

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device with an option to force delete config even if it's active

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config
    storage_mask  [in]  storage bitmask
    force_delete  [in]  force delete flag regardless of active/selected state

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_delete_config_ext
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  uint8                   storage_mask,
  boolean                 force_delete
)
{
  mcfg_config_id_s_type active_id;
  mcfg_sub_id_type_e_type sub;
  mcfg_error_e_type error = MCFG_ERR_INTERNAL;
  mcfg_map_record_s_type record;

  MCFG_ASSERT_COND_RET_ERROR(type < MCFG_NUM_OF_TYPES, MCFG_ERR_INVALID_PARAMETER);
  MCFG_CHECK_NULL_PTR_RET_ERROR(id, MCFG_ERR_NULL_POINTER);

  MCFG_MSG_MED_3("delete_config %02x%02x%02x", 
                 id->id[0], id->id[1], id->id[2]);
  mcfg_utils_print_config(type, id);

  /* Active config has to be deactivated before deleting */
  for (sub = MCFG_SUB_ID_FIRST; sub < MCFG_NUM_OF_SUB_ID; sub++)
  {
    if (mcfg_utils_get_active_config(type, &active_id, sub) || 
        mcfg_utils_get_selected_config(type, &active_id, sub))
    {
      if (mcfg_utils_config_id_matches(id, &active_id))
      {
        if (!force_delete)
        {
        MCFG_MSG_ERROR_1("Deactivate current config first before deleting for"\
                       " subscription %d", sub);
        return MCFG_ERR_CURRENT_ACTIVE;  
      }
        mcfg_utils_deactivate_config(type, sub, FALSE);
      }
    }
  }

  if (type == MCFG_TYPE_SW)
  {
    mcfg_sel_sw_config_store_updated(MCFG_NUM_OF_UIM_SLOT);
  }

  do
  {
    if (!mcfg_map_init_record_by_id(&record, id))
    {
      break;
    }

    if (!mcfg_map_read_record(type, &record))
    {
      break;
    }

    if (storage_mask & (1 << record.storage))
    {
       if (!mcfg_map_delete_record(type, &record))
       {
         break;
       }

       if(!mcfg_efs_delete_config(type, id, record.storage))
       {
         break;
       }
    }

    error = MCFG_ERR_NONE;
  }while (0); 

  return error;
} /* mcfg_utils_delete_config_ext */

/*===========================================================================

  FUNCTION mcfg_utils_delete_config

  DESCRIPTION
    This function deletes the config file with the specified unique ID from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete
    id        [in]  unique ID struct to fill with selected config
    storage_mask  [in]  storage bitmask

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_delete_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  uint8                   storage_mask
)
{
  return mcfg_utils_delete_config_ext(type, id, storage_mask, FALSE);
} /* mcfg_utils_delete_config */

/*===========================================================================

  FUNCTION mcfg_utils_delete_all

  DESCRIPTION
    This function deletes the config file with the specified type from the
    device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of the config to delete

  RETURN VALUE
    mcfg_error_e_type

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_delete_all
(
  mcfg_config_type_e_type type,
  uint8                   storage_mask
)
{
  mcfg_config_id_s_type *id_list;
  uint32 num_of_id;
  uint32 i;
  mcfg_error_e_type status = MCFG_ERR_INTERNAL;
  mcfg_error_e_type error = MCFG_ERR_NONE;
  boolean                 force_delete;

  MCFG_ASSERT_COND_RET_ERROR(type < MCFG_NUM_OF_TYPES, MCFG_ERR_INVALID_PARAMETER);
  id_list = mcfg_malloc(sizeof(mcfg_config_id_s_type) * MCFG_CONFIG_LIST_SIZE_MAX);
  if (id_list == NULL)
  {
      MCFG_MSG_ERROR("Couldn't allocate memory for config list!");
      error = MCFG_ERR_NULL_POINTER;
  }
  else
  {  
    if (mcfg_uim_nv_autoselect_enabled(MCFG_NUM_OF_UIM_SLOT) == FALSE)
    {
      force_delete = FALSE;
    }
    else
    {
      force_delete = ((storage_mask & MCFG_STORAGE_MASK_LOCAL) == 0 && (type == MCFG_TYPE_SW));
    }

      num_of_id = mcfg_utils_list_configs(type, MCFG_CONFIG_LIST_SIZE_MAX, id_list);
      MCFG_MSG_HIGH_2("Delete all %lu configs - type %d", num_of_id, type);
      for (i = 0; i < num_of_id; i++)
      {
        status =  mcfg_utils_delete_config_ext(type, &id_list[i], storage_mask, 
                force_delete);
        if(MCFG_ERR_NONE != status && MCFG_ERR_CURRENT_ACTIVE != status)
        {
          error = MCFG_ERR_INTERNAL;
        }
      }
      mcfg_free(id_list);
  }

  return error;
} /* mcfg_utils_delete_all */


/*===========================================================================

  FUNCTION mcfg_utils_get_config

  DESCRIPTION
    This function is used to load a new config file on the device into
    the config buffer in memory.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the new config
    id     [in]  ID of the new config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    Config file is loaded into memory

===========================================================================*/
boolean mcfg_utils_get_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_config_s_type     *config
)
{
  boolean status = FALSE;
  mcfg_map_record_s_type record;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);

  do
  {
    if (!mcfg_map_init_record_by_id(&record, id))
    {
      MCFG_MSG_ERROR("utils_get_config: init_record_by_id failed");
      break;
    }

    if (!mcfg_map_read_record(type, &record))
    {
      MCFG_MSG_ERROR("utils_get_config: map_read_record failed");
      break;
    }

    if(!mcfg_efs_get_config(type, id, config))
    {
      MCFG_MSG_ERROR("utils_get_config: efs_get_config failed");
      break;
    }

    if(MCFG_ERR_NONE != mcfg_auth_check_config(config))
    {
      MCFG_MSG_ERROR("utils_get_config: authentication failed");
      break;
    }
    
    status = TRUE;

  }while (0); 


  return status;
} /* mcfg_utils_get_config() */


/*===========================================================================

  FUNCTION mcfg_utils_get_config_info

  DESCRIPTION
    This function is used to read extended information for a given config file
    on the device.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  ID of the config to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_id_s_type   *id,
  mcfg_config_info_s_type *info
)
{
  mcfg_map_record_s_type record;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(info);

  if (mcfg_map_init_record_by_id(&record, id))
  {
    if(mcfg_map_read_record(type, &record))
    {
      return mcfg_efs_get_saved_config_info(type, id, info);
    }
  }

  return FALSE;
} /* mcfg_utils_get_config_info() */


/*===========================================================================

  FUNCTION mcfg_utils_get_config_limits

  DESCRIPTION
    This function is used to read information about memory limits for a given
    type of config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of config memory to query
    max_size  [out] maximum size of memory allowed for this type of config
    curr_size [out] current size of memory of all configs of this type

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_config_limits
(
  mcfg_config_type_e_type type,
  uint64                 *max_size,
  uint64                 *curr_size
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(max_size);
  MCFG_CHECK_NULL_PTR_RET_FALSE(curr_size);

  return mcfg_map_get_config_limits(type, max_size, curr_size);
} /* mcfg_utils_get_config_limits() */

/*===========================================================================

  FUNCTION mcfg_utils_get_config_size

  DESCRIPTION
    This function is used to read config size for a given
    type and ID of config file.

  DEPENDENCIES
    None

  PARAMETERS
    type [in]  type of config memory to query
    id   [in]  ID of the config to query
    size [out] size of config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_config_size
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  int                    *size
)
{
  mcfg_map_record_s_type record;
  mcfg_config_info_s_type info;

  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_CHECK_NULL_PTR_RET_FALSE(size);

  if (mcfg_map_init_record_by_id(&record, id))
  {
    if(mcfg_map_read_record(type, &record))
    {
      return mcfg_efs_get_config_size(type, id, size);
    }
  }

  return FALSE;
} /* mcfg_utils_get_config_size() */

/*===========================================================================

  FUNCTION mcfg_utils_get_active_config

  DESCRIPTION
    This function returns unique ID of the currently activated config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [out] unique ID struct to fill with active config

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_active_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  return mcfg_map_get_active_config(type, id, sub);
} /* mcfg_utils_get_active_config() */

/*===========================================================================

  FUNCTION mcfg_utils_set_active_config

  DESCRIPTION
    This function sets the currently active config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct to fill with active config

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_set_active_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  boolean status = FALSE;
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  MCFG_MSG_MED_4("set_active_config %02x%02x%02x", 
                 sub, id->id[0], id->id[1], id->id[2]);
  mcfg_utils_print_config(type, id);

  status =  mcfg_map_set_active_config(type, id, sub);
  
  return status;
} /* mcfg_utils_set_active_config() */


/*===========================================================================

  FUNCTION mcfg_utils_get_selected_config

  DESCRIPTION
    This function returns unique ID of the currently selected config file.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [out] unique ID struct to fill with selected config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_selected_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(id);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  return mcfg_map_get_selected_config(type, id, sub);
} /* mcfg_utils_get_selected_config() */


/*===========================================================================

  FUNCTION mcfg_utils_set_selected_config

  DESCRIPTION
    This function sets the currently selected config file to the specified
    unique ID.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    id        [in]  unique ID struct to fill with selected config;
                    or null to deactivate pending config

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_set_selected_config
(
  mcfg_config_type_e_type type,
  mcfg_config_id_s_type  *id,
  mcfg_sub_id_type_e_type sub
)
{
  boolean status = FALSE;
  mcfg_config_id_s_type  existing_id;

  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  //If there's different pending config not activated, force cleanup
  if(!mcfg_utils_get_active_config(type, &existing_id, sub))
  {
    if(mcfg_utils_get_selected_config(type, &existing_id, sub))
    {
      if (!id || !mcfg_utils_config_id_matches(&existing_id, id))
      {
        MCFG_MSG_MED_1("there's different pending config not activated, force cleanup subi %d", sub);
        mcfg_utils_print_config(type, &existing_id);
        modem_cfg_cleanup(type, sub, MCFG_REFRESH_TYPE_SLOT_N_SUBS);
      }
    }
  }

  status = mcfg_map_set_selected_config(type, id, sub);

  if (status && id)
  {
    MCFG_MSG_MED_4("set_selected_config sub%d %02x%02x%02x", 
                   sub, id->id[0], id->id[1], id->id[2]);
    mcfg_utils_print_config(type, id);

    if (type == MCFG_TYPE_SW)
    {
      mcfg_imgsel_switch_images_by_config(id);
    }

    mcfg_default_config_force_load(type, TRUE, sub);
	
    if (!mcfg_uim_nv_autoselect_enabled(MCFG_NUM_OF_UIM_SLOT))
    {
      //Only trigger EFS sync for APPS centric MBN selection
      mcfg_utils_efs_sync();
    }
  }

  return status;
} /* mcfg_utils_set_selected_config() */


/*===========================================================================

  FUNCTION mcfg_utils_activate_config

  DESCRIPTION
    This function performs the necessary action for the currently selected
    config to become the active config.

  DEPENDENCIES
    None

  PARAMETERS
    type   [in] type of the config to activate

  RETURN VALUE
    MCFG_ERR_NONE if success, error code otherwise

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_activate_config
(
  mcfg_config_type_e_type        type,
  mcfg_activation_source_e_type  source
)
{
  mcfg_config_id_s_type pending_id;
  mcfg_config_id_s_type active_id;
  mcfg_sub_id_type_e_type sub;
  boolean found = FALSE;
  mcfg_error_e_type mcfg_error = MCFG_ERR_NONE;
  boolean update_only = FALSE;
#ifdef FEATURE_MCFG_REFRESH
  mcfg_refresh_index_type slot;
  boolean refresh_enabled = TRUE;
#endif
  static boolean inserted = FALSE;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_MSG_HIGH_2("activate_config type %d source %d", type, source);

  for (sub = MCFG_SUB_ID_FIRST; sub < MCFG_NUM_OF_SUB_ID; sub++)
  {
    if(!mcfg_utils_get_selected_config(type, &pending_id, sub))
    {
      MCFG_MSG_MED_2("No selected config for type %d sub %d, skip", type, sub);
      continue;
    }

    if(!mcfg_utils_get_active_config(type, &active_id, sub))
    {
      if (type == MCFG_TYPE_SW && sub == MCFG_SUB_ID_FIRST && 
          mcfg_utils_check_category(&pending_id, MCFG_CATEGORY_BITMASK_SUBSI))
      {
        mcfg_utils_reload_hw();
#ifdef FEATURE_MCFG_REFRESH
        refresh_enabled = FALSE;
        found = TRUE;
        continue;
#endif
      }
#ifndef FEATURE_MCFG_REFRESH
      found = TRUE;
      continue;
#endif
    }
    else
    {
      if (mcfg_utils_config_id_matches(&pending_id, &active_id))
      {
        MCFG_MSG_HIGH_1("Pending config is already activated for sub %d", sub);
        mcfg_error = MCFG_ERR_NO_EFFECT;
        continue;
      }
    }

    if(type == MCFG_TYPE_SW && sub == MCFG_SUB_ID_FIRST && 
       (mcfg_utils_check_category(&active_id, MCFG_CATEGORY_BITMASK_SUBSI)
       || mcfg_utils_check_category(&pending_id, MCFG_CATEGORY_BITMASK_SUBSI)))
    {
      mcfg_utils_reload_hw();
#ifdef FEATURE_MCFG_REFRESH
        refresh_enabled = FALSE;
#endif
    }
    else
    {
#ifdef FEATURE_MCFG_REFRESH
#ifdef FEATURE_MCFG_PARALLEL_REFRESH
      slot = mcfg_refresh_sub_to_slot_index((mcfg_refresh_index_type)sub);
      if (mcfg_refresh_is_ongoing_by_type((1<<slot), MCFG_REFRESH_TYPE_SUBS))
#else
      if (mcfg_refresh_is_ongoing_by_type(MCFG_REFRESH_INDEX_MASK_ALL, MCFG_REFRESH_TYPE_SUBS))
#endif
      {
#ifdef FEATURE_MCFG_PARALLEL_REFRESH
        MCFG_MSG_MED_2("Refresh is ongoing for slot %d, sub %d, set ACTIVATE timer", slot, sub);
#endif
        mcfg_utils_cmd_send(MCFG_UTILS_CMD_ACTIVATE_DEBOUNCE);
        return MCFG_ERR_NONE;
      }

      if (type == MCFG_TYPE_HW)
      {
        MCFG_MSG_MED("refresh only applies for sw type");
      }
      else
      {
#ifndef FEATURE_MCFG_PARALLEL_REFRESH
        slot = mcfg_refresh_sub_to_slot_index((mcfg_refresh_index_type)sub);
#endif
        if(mcfg_uim_nv_updateonly_enabled((mcfg_slot_index_type_e_type)slot))
        {
          MCFG_MSG_MED_2("PDC activation update only slot %d sub %d", slot, sub);
          //mcfg_refresh_set_orig(slot, MCFG_REFRESH_ORIG_PDC_UPDATE_ONLY);
          mcfg_utils_deactivate_config(type, sub, TRUE);
          modem_cfg_process_single(type, MCFG_REFRESH_TYPE_SLOT_N_SUBS, (mcfg_refresh_index_type)sub);
          found = TRUE;
          update_only = TRUE;
          continue;
        }
        else if(mcfg_refresh_enabled(slot))
        {
          MCFG_MSG_MED_2("PDC activation refreshing slot %d sub %d", slot, sub);
          if (source == MCFG_ACTIVATION_CROSSMAPPING)
          {
            mcfg_refresh_set_orig(slot, MCFG_REFRESH_ORIG_FLEX);
          }
          else
          {
            mcfg_refresh_set_orig(slot, MCFG_REFRESH_ORIG_PDC);
          }
          mcfg_refresh_tear_down((mcfg_refresh_index_type)sub);
          found = TRUE;
          continue;
        }
        else
        {
          found = TRUE;
          refresh_enabled = FALSE;
        }
      }
#endif
    }

    if (type == MCFG_TYPE_HW)
    {
      MCFG_MSG_MED_1(" No need to deactive for HW type, sub%d", sub);
    }
    else
    {
      MCFG_MSG_HIGH_2("Active config found type%d sub%d deactivating", type, sub);
      if (!mcfg_utils_deactivate_config(type, sub, TRUE))
      {
        MCFG_MSG_HIGH_1("Failed to deactivate existing config, sub%d", sub);
        continue;
      }
      MCFG_MSG_MED_2("Deactivate done for type%d sub%d", type, sub);
    }
    found = TRUE;
  }

  if (found && !update_only)
  {
#if defined(FEATURE_MCFG_REFRESH)
    if (refresh_enabled && type == MCFG_TYPE_SW)
    {
      MCFG_MSG_MED("Skip for PDC activation");
    }
    else
#endif
    {
#if defined(FEATURE_MCFG_ACTIVATE_W_NO_RESET)
      MCFG_MSG_MED("Calling modem_cfg_process during runtime .....");
      modem_cfg_process();
#else 
      /* Power cycle the device to load/activate the selected config */
      if (mcfg_utils_reset_enabled())
      {
        if(source == MCFG_ACTIVATION_ICCID_SELECT)
        {
          if (inserted == FALSE)
          {
            inserted = TRUE;
            MCFG_MSG_MED("First card insertion, debouncing reset");
            mcfg_utils_cmd_send(MCFG_UTILS_CMD_RESET_DEBOUNCE);
          }
          else
          {
            MCFG_MSG_MED("2nd card insertion, wait for reset to happen");
          }
        }
        else if(source == MCFG_ACTIVATION_CROSSMAPPING)
        {
            MCFG_MSG_MED("crossmapping triggered activation, debouncing reset");
            mcfg_utils_cmd_send(MCFG_UTILS_CMD_RESET_DEBOUNCE);
        }
        else
        {
          MCFG_MSG_MED("trigger modem restart for activation");
          mcfg_utils_reset_modem(MCFG_PWRDOWN_RES_MODEM_CONF_CHANGE);
        }
      }
#endif
    }
    mcfg_error = MCFG_ERR_NONE;
  }
  else
  {
    if (mcfg_error != MCFG_ERR_NO_EFFECT)
    {
      mcfg_error = MCFG_ERR_NOT_FOUND;
    }
  }
    
  return mcfg_error;
} /* mcfg_utils_activate_config() */

/*===========================================================================

  FUNCTION mcfg_utils_activate_refreshonly

  DESCRIPTION
    This function performs refresh only activation.

  DEPENDENCIES
    None

  PARAMETERS
    slot   [in] slot to refresh
    sub    [in] sub to refresh

  RETURN VALUE
    mcfg_error_enum

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_activate_refreshonly
(
  uint8                   slot,
  mcfg_sub_id_type_e_type sub
)
{
#ifdef FEATURE_MCFG_REFRESH
  if(mcfg_refresh_enabled(slot))
  {
    MCFG_MSG_MED_2("APP centric refreshing slot %d sub %d", slot, sub);

    /* Chek if refresh is ongoing */
    if(mcfg_refresh_is_ongoing(MCFG_REFRESH_INDEX_MASK_ALL))
    {
      return MCFG_ERR_DEVICE_IN_USE;
    }

    mcfg_refresh_set_orig(slot, MCFG_REFRESH_ORIG_APP);

    return mcfg_refresh_tear_down((mcfg_refresh_index_type)sub);
  }
#endif

  MCFG_MSG_ERROR_1("refresh disabled for sloti %d", slot);
  
  return MCFG_ERR_NOT_SUPPORTED;
} /* mcfg_utils_activate_refreshonly() */

/*===========================================================================

  FUNCTION mcfg_utils_deactivate_config_by_type

  DESCRIPTION
    This function deactivates the currently active config 

  DEPENDENCIES
    None

  PARAMETERS
    type   [in] type of the config to deactivate

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_deactivate_config_by_type
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub,
  boolean                 activating,
  mcfg_refresh_type_e_type refresh_type
)
{
  mcfg_config_id_s_type id;
  mcfg_config_id_s_type pending_id;
  boolean active_exist = TRUE;

  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  active_exist = mcfg_utils_get_active_config(type, &id, sub);

  // Do not remove pending config during config switching
  if(!activating && mcfg_utils_get_selected_config(type, &pending_id, sub))
  {
    if (active_exist == FALSE || !mcfg_utils_config_id_matches(&id, &pending_id))
    {
      MCFG_MSG_LOW_2("Deactivate pending config for type %d sub%d first", type, sub);
      return mcfg_utils_set_selected_config(type, NULL, sub);
    }
  }

  if(!active_exist)
  {
    MCFG_MSG_MED_2("No selected config for type %d or error reading config; "
                     "can't deactivate sub%d", type, sub);
    return FALSE;
  }

  /* Power cycle the device to load/activate the selected config */
  MCFG_MSG_MED_3("deactivate_config %02x%02x%02x", 
                 id.id[0], id.id[1], id.id[2]);
  mcfg_utils_print_config(type, &id);
  modem_cfg_cleanup(type, sub, refresh_type);

  return mcfg_map_rmv_active_config(type, sub);
} /* mcfg_utils_deactivate_config_by_type() */

/*===========================================================================

  FUNCTION mcfg_utils_deactivate_config

  DESCRIPTION
    This function deactivates the currently active config 

  DEPENDENCIES
    None

  PARAMETERS
    type   [in] type of the config to deactivate

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_deactivate_config
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub,
  boolean                 activating
)
{
  return mcfg_utils_deactivate_config_by_type(
    type, sub, activating, MCFG_REFRESH_TYPE_SLOT_N_SUBS);
} /* mcfg_utils_deactivate_config() */


/*===========================================================================

  FUNCTION mcfg_utils_parse_config_info

  DESCRIPTION
    This function is used to read extended information for config 

  DEPENDENCIES
    None

  PARAMETERS
    config    [in]  config pointer
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_parse_config_info
(
  mcfg_config_s_type      *config,
  mcfg_config_info_s_type *info
)
{
  boolean status = FALSE;
  uint32  trailer_offset;
  uint8   prefix_type;
  uint16  trailer_type, trailer_len;
  uint8  *p_trailer_offset = NULL;
  uint32 *p_end = NULL;

  MCFG_CHECK_NULL_PTR_RET_FALSE(info);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);
  MCFG_CHECK_NULL_PTR_RET_FALSE(config->config_addr);
  MCFG_ASSERT_COND_RET_FALSE(config->config_len > 0);

  do 
  {
    p_end = (uint32 *)((uint8 *)config->config_addr + config->config_len - sizeof(trailer_offset));
    trailer_offset = *p_end;
    if (trailer_offset >= config->config_len)
    {
      MCFG_MSG_ERROR_1("parse_config_info: invalid trl offset"
                       " (0x%lx)", trailer_offset);
      break;
    }

    p_trailer_offset = 
      (uint8 *)config->config_addr + config->config_len - trailer_offset;

    prefix_type = *(p_trailer_offset + MCFG_GCI_PREFIX_TYPE_OFFSET);
    if (prefix_type != MCFG_GCI_MAGIC_PREFIX_TYPE)
    {
      MCFG_MSG_ERROR_1("parse_config_info: invalid trailer prefix type"
                       " (0x%x)", prefix_type);
      break;
    }

    trailer_type = *(p_trailer_offset + MCFG_GCI_TRAILER_TYPE_OFFSET);
    if (trailer_type != MCFG_GCI_MAGIC_TRAILER_TYPE)
    {
      MCFG_MSG_ERROR_1("parse_config_info: invalid trailer type"
                       " (0x%x)", trailer_type);
      break;
    }

    trailer_len = *(p_trailer_offset + MCFG_GCI_TRAILER_LENGTH_OFFSET);
    trailer_len += ((*(p_trailer_offset + MCFG_GCI_TRAILER_LENGTH_OFFSET + 1))<<8);
    if (trailer_len > MCFG_CONFIG_DESCRIPTION_SIZE_MAX)
    {
      MCFG_MSG_ERROR_1("parse_config_info: trailer too long"
                       " (%d)", trailer_len);
      break;
    }

    memscpy(info->description, MCFG_CONFIG_DESCRIPTION_SIZE_MAX,
            p_trailer_offset + MCFG_GCI_TRAILER_DESC_OFFSET, trailer_len);

    if (strncmp((char *)(info->description), "MCFG_TRL", 8))
    {
      MCFG_MSG_ERROR("parse_config_info: invalid MCFG trailer magic");
      break;
    }

    info->description_len = (uint32) trailer_len;
    info->config_len = config->config_len;
    status = TRUE;
  }while (0);

  //MCFG_MSG_LOW_1("parse_config_info status(%d)", status);

  return status;
} /* mcfg_utils_parse_config_info() */


/*===========================================================================

  FUNCTION mcfg_utils_get_default_config

  DESCRIPTION
    This function is used to get the size and address of default config
    ata segment from DAL device config. 

  DEPENDENCIES
    None

  PARAMETERS
    type   [in]  type of the default config
    config [in/out] config structure including config size and buffer pointer

  RETURN VALUE
    TRUE if success, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_default_config
(
  mcfg_config_type_e_type type,
  mcfg_config_s_type     *config
)
{
  boolean status = FALSE;
  DALSYSPropertyVar property_var;

  MCFG_CHECK_NULL_PTR_RET_FALSE(config);

  // Declare a handler   
  DALSYS_PROPERTY_HANDLE_DECLARE(mcfg_property);

  if (mcfg_default_config_enabled(type))
  {
    do
    {
      if (!(type == MCFG_TYPE_SW && DALSYS_GetDALPropertyHandleStr(
            DALDEVICEID_MCFG_SW, mcfg_property) == DAL_SUCCESS) &&
          !(type == MCFG_TYPE_HW && DALSYS_GetDALPropertyHandleStr(
            DALDEVICEID_MCFG_HW, mcfg_property) == DAL_SUCCESS))
      {
        // Oh I could not find the Device .... 
        MCFG_MSG_HIGH("mcfg_utils_get_default_config: device not found");
        break;
      }

      if (DALSYS_GetPropertyValue(mcfg_property, DALDEVICEID_MCFG_PROP_SIZE, 0, 
          &property_var) != DAL_SUCCESS)
      {
        //Oh .. Could not find the size Property
        MCFG_MSG_ERROR("mcfg_utils_get_default_config: size prop not found");
        break;
      }

      if (!property_var.Val.pStruct)
      {
        //Oh .. Invalid size Property
        MCFG_MSG_ERROR("mcfg_utils_get_default_config: size prop invalid");
        break;
      }

      config->config_len = *((uint32 *)property_var.Val.pStruct);
      if (config->config_len > MCFG_CONFIG_SIZE_MAX)
      {
        //Oh .. Invalid size Property
        MCFG_MSG_ERROR_1("mcfg_utils_get_default_config: size prop invalid %d",
                     config->config_len);
        break;
      }

      if (DALSYS_GetPropertyValue(mcfg_property, DALDEVICEID_MCFG_PROP, 0, 
          &property_var) != DAL_SUCCESS)
      {
        //Oh .. Could not find the config Property
        MCFG_MSG_ERROR("mcfg_utils_get_default_config: config prop not found");
        break;
      }

      if (!property_var.Val.pStruct)
      {
        //Oh .. Invalid Property
        MCFG_MSG_ERROR("mcfg_utils_get_default_config: config prop invalid");
        break;
      }
      config->config_addr = (void *)property_var.Val.pStruct;
      status = TRUE; 

      MCFG_MSG_LOW_2("mcfg_utils_get_default_config: type %d size %d", 
                      type, config->config_len);
    }while(0);
  }

  return status;
} /* mcfg_utils_get_default_config() */

/*===========================================================================

  FUNCTION mcfg_utils_get_default_config_info

  DESCRIPTION
    This function is used to read extended information for default config
    from DAL device config.

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    info      [out] info for the specified config

  RETURN VALUE
    TRUE if it exists, FALSE if it doesn't

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_default_config_info
(
  mcfg_config_type_e_type  type,
  mcfg_config_info_s_type *info
)
{
  boolean status = FALSE;
  mcfg_config_s_type config;

  MCFG_CHECK_NULL_PTR_RET_FALSE(info);

  memset(&config, 0, sizeof(config));

  do 
  {
    if (!mcfg_utils_get_default_config(type, &config))
    {
      MCFG_MSG_HIGH_1("get_default_config_info: get config failed"
                       " (%d)", type);
      break;
    }

    if (!mcfg_utils_parse_config_info(&config, info))
    {
      MCFG_MSG_ERROR_1("get_default_config_info: parse info failed"
                       " (%d)", type);
      break;
    }

    status = TRUE;
  }while (0);

  MCFG_MSG_LOW_2("get_default_config_info type(%d) status(%d)", type, status);

  return status;
} /* mcfg_utils_get_default_config_info() */

/*===========================================================================

  FUNCTION mcfg_utils_selected_config_available

  DESCRIPTION
    This function returns whether selected/pending config available for activate

  DEPENDENCIES
    None

  PARAMETERS
    type      [in]  type of ID to query
    sub       [in]  subscription ID

  RETURN VALUE
    TRUE if selected/pending config is available for activate, FALSE if not

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_error_e_type mcfg_utils_selected_config_available
(
  mcfg_config_type_e_type type,
  mcfg_sub_id_type_e_type sub
)
{
  mcfg_config_id_s_type pending_id;
  mcfg_config_id_s_type active_id;
  boolean found = FALSE;
  mcfg_error_e_type mcfg_error = MCFG_ERR_NONE;
  boolean update_only = FALSE;

  MCFG_MSG_LOW_2("check pending_config_available type %d sub %d", type, sub);
  MCFG_ASSERT_COND_RET_FALSE(type < MCFG_NUM_OF_TYPES);
  MCFG_ASSERT_COND_RET_FALSE(sub < MCFG_NUM_OF_SUB_ID);

  if(!mcfg_utils_get_selected_config(type, &pending_id, sub))
  {
    MCFG_MSG_LOW_2("No selected config for type %d sub %d", type, sub);
    return FALSE;
  }

  if(!mcfg_utils_get_active_config(type, &active_id, sub))
  {
    MCFG_MSG_LOW_2("No active config for type %d sub %d", type, sub);

    return TRUE;
  }

  if (mcfg_utils_config_id_matches(&pending_id, &active_id))
  {
    MCFG_MSG_LOW_1("Pending config is already activated for sub %d", sub);
    return FALSE;
  }

  MCFG_MSG_LOW_1("Pending config is available for sub %d", sub);
  return TRUE;
} /* mcfg_utils_selected_config_available() */

/*===========================================================================

  FUNCTION mcfg_utils_cmd_proc

  DESCRIPTION
    Processes a mcfg_utils command in the context of the MCFG task.

  DEPENDENCIES
    None

  PARAMETERS
    cmd      [in]  command struct

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_cmd_proc
(
  mcfg_task_cmd_s_type *cmd
)
{
  mcfg_utils_cmd_data_s_type *utils_cmd_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (cmd == NULL || cmd->data == NULL)
  {
    MCFG_MSG_ERROR("Unexpected NULL pointer");
  }
  else
  {
    utils_cmd_data = (mcfg_utils_cmd_data_s_type *) cmd->data;
    switch (utils_cmd_data->cmd_id)
    {
      case MCFG_UTILS_CMD_RESET_DEBOUNCE:
        timer_set(&mcfg_utils_reset_timer, MCFG_UTILS_RESET_TIMER_TIMEOUT,
                  0, T_SEC);
        break;

      case MCFG_UTILS_CMD_RESET_NOW:
        mcfg_utils_reset_modem(MCFG_PWRDOWN_RES_MODEM_CONF_CHANGE);
        break;

      case MCFG_UTILS_CMD_RESET_RETRY:
        timer_set(&mcfg_utils_reset_timer, MCFG_UTILS_RESET_RETRY_TIMEOUT,
                  0, T_SEC);
        break;

      case MCFG_UTILS_CMD_CLEAR_RESET_TIMER:
        timer_clr(&mcfg_utils_reset_timer,T_SEC);
        break;

      case MCFG_UTILS_CMD_ACTIVATE_DEBOUNCE:
        timer_set(&mcfg_utils_activate_timer, MCFG_UTILS_ACTIVATE_TIMER_TIMEOUT,
                  0, T_SEC);
        break;

      case MCFG_UTILS_CMD_ACTIVATE_NOW:
        mcfg_utils_activate_config(MCFG_TYPE_SW, MCFG_ACTIVATION_DEBOUNCE);
        break;

      case MCFG_SEGLOAD_RESET:
        mcfg_utils_reset_modem(MCFG_PWRDOWN_RES_SEGMENT_LOADING);
        break;

      default:
        MCFG_MSG_ERROR_1("Unexpected UTILS cmd_id %d", utils_cmd_data->cmd_id);
    }
  }
} /* mcfg_utils_cmd_proc() */

/*===========================================================================

  FUNCTION mcfg_utils_init

  DESCRIPTION
    This function initializes the utils library

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
void mcfg_utils_init
(
  void
)
{
  mcfg_auth_init();
  mcfg_map_init();

  /* define timer for reset command forwarding */
  timer_def(&mcfg_utils_reset_timer,
            &mcfg_utils_reset_timer_group,
            NULL,
            0,
            mcfg_utils_cmd_reset_timer_cb,
            0);

  timer_def(&mcfg_utils_activate_timer,
            &mcfg_utils_timer_group,
            NULL,
            0,
            mcfg_utils_cmd_debounce_timer_cb,
            0);

  return;
} /* mcfg_utils_init() */

/*===========================================================================

  FUNCTION mcfg_utils_power_down_reason

  DESCRIPTION
    This function save the power down reason.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/

void mcfg_utils_saved_power_down_reason
(
  void
)
{
   mcfg_nv_status_e_type       delete_status;
   mcfg_power_down_reason_e_type res_code;

   if (MCFG_NV_STATUS_OK == mcfg_nv_read(MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON,
                                          0, MCFG_SUB_ID_FIRST, &res_code, sizeof(res_code)))
  {
    curr_power_down_reason=res_code;
    MCFG_MSG_HIGH_1("mcfg_power_down_reason ...0x%x", curr_power_down_reason);  
  }
  else
  {
     MCFG_MSG_HIGH_1("Default power down reason...Unsucessful to read the nv...mcfg_power_down_reason : 0x%x",curr_power_down_reason);
     curr_power_down_reason=MCFG_PWRDOWN_RES_NONE;
  }

  //Delete the NV from EFS

  delete_status = mcfg_nv_delete(MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON, 0, 0);                           
  if (delete_status != MCFG_NV_STATUS_OK)                                                              
  {                                                                                                    
     MCFG_MSG_ERROR_1("Couldn't delete MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON: status %d", delete_status);
  }
  else
  {
     MCFG_MSG_ERROR_1("deleted MCFG_NV_ITEM_MCFG_POWER_DOWN_REASON: status %d", delete_status);
  }

 
} /* mcfg_utils_saved_power_down_reason */


/*===========================================================================

  FUNCTION mcfg_to_ssreq_qmi_map_res_code

  DESCRIPTION
    This function is to map mcfg power down reason with ssreq_qmi_request_res_code_type power down reason.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
ssreq_qmi_request_res_code_type mcfg_to_ssreq_qmi_map_res_code
(
   mcfg_power_down_reason_e_type res_code
)
{
   if (res_code == MCFG_PWRDOWN_RES_MODEM_CONF_CHANGE) 
   {
      return SSREQ_QMI_RES_MODEM_CONF_CHANGE;
   }
   else if (res_code == MCFG_PWRDOWN_RES_SEGMENT_LOADING)
   {
      return SSREQ_QMI_RES_SEGMENT_LOADING;
   }
   else
   {
      return SSREQ_QMI_RES_NONE;
   }

} /* mcfg_to_ssreq_qmi_map_res_code */

/*===========================================================================

  FUNCTION mcfg_pwr_down_reason

  DESCRIPTION
    This function is to get power down reason.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None

===========================================================================*/
mcfg_power_down_reason_e_type mcfg_pwr_down_reason(void)
{
   return curr_power_down_reason;
} /* mcfg_pwr_down_reason */

/*===========================================================================
  FUNCTION mcfg_util_digest_version_compatibility

  DESCRIPTION
    This function is to check if the config is digest compatible or not based on format version.

  DEPENDENCIES
    None

  PARAMETERS
    None

  RETURN VALUE
   TRUE - if config is digest compatible
   FALSE - if config is not digest compatible  

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_util_digest_version_compatibility(uint8* mcfg_hdr_ptr)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(mcfg_hdr_ptr);
  mcfg_header_type  *mcfg_hdr;
  mcfg_hdr = (mcfg_header_type *) mcfg_hdr_ptr;
  boolean digest_available = FALSE;
  
  MCFG_MSG_HIGH_1("mcfg_format_ver_num is : 0x%x ", mcfg_hdr->mcfg_format_ver_num);
  if(mcfg_hdr->mcfg_format_ver_num < 0x04)
  {
    MCFG_MSG_HIGH("====  isMbnWithDigest returned false === ");
  }else{
    MCFG_MSG_HIGH("====  isMbnWithDigest returned true ==== ");
    digest_available = TRUE;
  }
  return digest_available;
}/*mcfg_util_digest_version_compatibility()*/

/*===========================================================================
  FUNCTION mcfg_utils_compress_mem_alloc

  DESCRIPTION
    This function modifies the interface between zlib and modem mem heap

  DEPENDENCIES
    None

  PARAMETERS
    handle: Handle to identify multiple calls. In this case, dummy
    
    items:  Number of items which needs to be allocated

    size:  Size of each item in bytes

  RETURN VALUE
    Returns the address of memory allocated

  SIDE EFFECTS
    None
===========================================================================*/
static void* mcfg_utils_compress_mem_alloc(void* handle, unsigned items, unsigned size)
{
  uint32 total_size = items * size;
  handle = NULL;  
  return mcfg_malloc(total_size);

} /* mcfg_utils_compress_mem_alloc() */

/*===========================================================================
  FUNCTION mcfg_utils_compress_mem_free

  DESCRIPTION
    This function modifies the interface between zlib and modem mem heap

  DEPENDENCIES
    None

  PARAMETERS
    handle: Handle to identify multiple calls. In this case, dummy
    
    addr:  Memory addr to be freed

  RETURN VALUE
    Frees the address of memory allocated

  SIDE EFFECTS
    None
===========================================================================*/
static void mcfg_utils_compress_mem_free(void* handle, void* addr)
{
  handle = NULL;
  mcfg_free(addr);
  
} /* mcfg_utils_compress_mem_free() */

/*===========================================================================

  FUNCTION mcfg_utils_get_phrase

  DESCRIPTION
    Extract a phrase from the line buffer and its offset, and update offset
    for next phrase

  DEPENDENCIES
    None

  PARAMETERS
    line    [in]
    offset  [in/out]
    maxsize [in]
    buf     [out]

  RETURN VALUE
    TRUE if successful, FALSE otherwise

  SIDE EFFECTS
    None

===========================================================================*/
boolean mcfg_utils_get_phrase
(
  char   *line,
  uint32 *offset,
  uint32  maxsize,
  char   *buf
)
{
  boolean status = FALSE;
  uint32 len;
  char *result = NULL;
  char *str = NULL;
  char *saveptr1;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(line);
  MCFG_CHECK_NULL_PTR_RET_FALSE(offset);
  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);

  str = line + *offset;

  result = strtok_r( str, ",", &saveptr1);
  if (result != NULL)
  {
    len = strlen(result);

    if (len < maxsize)
    {
      memset(buf, 0, maxsize);
      memscpy(buf, maxsize, str, len);
      *offset += len + 1;
      status = TRUE;
    }
  }

  return status;
} /* mcfg_utils_get_phrase() */

/*===========================================================================
  FUNCTION mcfg_utils_compress_wrapper

  DESCRIPTION
  This function is a wrapper to compress. It passes modem_mem_alloc/modem_mem_free,
  to deflate API to use them for alloc/free functionalities. Compresses the source buffer into 
  the destination buffer. The level parameter has the same meaning as in deflateInit.

  PARAMETERS
  dest:
  Memory pointing to compressed memory

  destLen:
  destLen is the total size of the destination buffer, which must be at least 0.1%
  larger than sourceLen plus 12 bytes.Upon exit, destLen is the actual size of 
  the compressed buffer.

  source:
  Memory pointing to uncompressed memory
  
  sourceLen:
  Byte length of the source buffer

  RETURN VALUE
  Z_OK if success
  Z_MEM_ERROR if there was not enough memory
  Z_BUF_ERROR if there was not enough room in the output buffer
  Z_STREAM_ERROR if the level parameter is invalid.

    SIDE EFFECTS
    None
===========================================================================*/
static int mcfg_utils_compress_wrapper(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
  z_stream stream;
  int err;
  stream.next_in = (Bytef*)source;
  stream.avail_in = (uInt)sourceLen;

  stream.next_out = dest;
  stream.avail_out = (uInt)*destLen;
  if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)mcfg_utils_compress_mem_alloc;
  stream.zfree = (free_func)mcfg_utils_compress_mem_free;
  stream.opaque = (voidpf)0;

  err = deflateInit(&stream, Z_DEFAULT_COMPRESSION); // header 78, 9C

  if (err != Z_OK)
  {
    MCFG_MSG_ERROR_2("compress_wrapper(): deflateInit failed. sourceLen %lu, err %d", sourceLen, err);
    return err;
  }

  err = deflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) 
  {
    MCFG_MSG_ERROR_2("compress_wrapper(): deflate failed. sourceLen %lu, err %d", sourceLen, err);
    deflateEnd(&stream);
    return err == Z_OK ? Z_BUF_ERROR : err;
  }
    
  *destLen = stream.total_out;

  err = deflateEnd(&stream);
  return err;
    
}/* mcfg_utils_compress_wrapper() */

/*===========================================================================
  FUNCTION mcfg_utils_compress

  DESCRIPTION
    Compress memory buffer

  DEPENDENCIES
    None

  PARAMETERS
    [out] compressed_data_ptr
      Pointer to pointer to compressed data buffer. Caller must free this 
      buffer after use.
     
    [in, out] compressed_buffer_size
      Pointer to expected size of compressed data. Function will allocate this much
      memory which user must free after being done with compressed data.
    
    [in] uncompressed_data_ptr
      Pointer to uncompressed data, for example RFNV item to be written to EFS.
    
    [in] uncompressed_size
      Size of compressed data buffer

  RETURN VALUE
    Z_MEM_ERROR if there was not enough memory
    Z_BUF_ERROR if there was not enough room in the output buffer
    Z_STREAM_ERROR if the level parameter is invalid.
    Z_ERRNO is other error.
    See F3 logs for more detailed error message.
  
  SIDE EFFECTS
    None
===========================================================================*/
static boolean mcfg_utils_compress(void **compressed_data_ptr, uint32 *compressed_buffer_size, void *uncompressed_data_ptr, uint32 uncompressed_buffer_size)
{
  int8 compress_status = Z_BUF_ERROR;

  if ( (NULL == uncompressed_data_ptr) || (NULL == compressed_buffer_size) || (NULL == compressed_data_ptr) )
  {
     MCFG_MSG_ERROR("utils_compress(): Invalid input, one of the argumets is a NULL pointer! ");
     return FALSE ;
  }
  
  *compressed_buffer_size = compressBound(uncompressed_buffer_size);
  *compressed_data_ptr = mcfg_malloc(*compressed_buffer_size);
  
  if (*compressed_data_ptr == NULL)
  {
    MCFG_MSG_ERROR_1("utils_compress(): Failed to allocate memory for %lu bytes.",*compressed_buffer_size);
    return FALSE ;
  }
  compress_status = mcfg_utils_compress_wrapper((uint8 *)(*compressed_data_ptr), compressed_buffer_size, (const uint8 *)uncompressed_data_ptr, uncompressed_buffer_size);

  if (compress_status != Z_OK)
  {
    MCFG_MSG_ERROR_1("utils_compress(): Compression of data failed. (%d)",  compress_status);
    mcfg_free(*compressed_data_ptr);
    *compressed_data_ptr = NULL;
    return FALSE;
  }

  return TRUE;
}/* mcfg_utils_compress() */

/*===========================================================================
  FUNCTION mcfg_utils_uncompress_wrapper

  DESCRIPTION
  Decompresses the source buffer into the destination buffer.

  PARAMETERS
  dest:
  The actual size of the compressed buffer

  destLen:
    The total size of the destination buffer, which must be large enough to hold the
    entire uncompressed data. (The size of the uncompressed data must have
    been saved previously by the compressor )

  source:
  The byte length of the source buffer
  
  sourceLen:
  Byte length of the source buffer

  RETURN VALUE
  Z_OK if success
  Z_MEM_ERROR if there was not enough memory
  Z_BUF_ERROR if there was not enough room in the output buffer
  Z_STREAM_ERROR if the input data was corrupted.

    SIDE EFFECTS
    None
===========================================================================*/
static int mcfg_utils_uncompress_wrapper (Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen)
{
  z_stream stream;
  int err;

  stream.next_in = (Bytef*)source;
  stream.avail_in = (uInt)sourceLen;

  stream.next_out = dest;
  stream.avail_out = (uInt)*destLen;
  if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

  stream.zalloc = (alloc_func)mcfg_utils_compress_mem_alloc;
  stream.zfree = (free_func)mcfg_utils_compress_mem_free;

  err = inflateInit(&stream);  // for ZLIB header 78, 9C

  if (err != Z_OK) return err;

  if(sourceLen > 3)
  {
    MCFG_MSG_LOW_2("uncompress_wrapper(): first 2 bytes of source (compressed), 0x%x, 0x%x ", source[0], source[1]);
  }

  err = inflate(&stream, Z_FINISH);
  if (err != Z_STREAM_END) 
  {
      inflateEnd(&stream);
      if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
        return Z_DATA_ERROR;
     return err;
  }
  *destLen = stream.total_out;
  err = inflateEnd(&stream);

  if(*destLen > 3)
  {
    MCFG_MSG_LOW_2("uncompress_wrapper(): first 2 bytes of dest (uncompresses), 0x%x, 0x%x ", dest[0], dest[1]);
  }

  return err;
}/* mcfg_utils_uncompress_wrapper() */


/*===========================================================================
  FUNCTION mcfg_utils_check_config_header

  DESCRIPTION
    This function checks if config data in compress file or uncompressed MBN.

  DEPENDENCIES
    None

  PARAMETERS
    config    [in]  data buffer
 
  RETURN VALUE
    mcfg_config_header_e_type

  SIDE EFFECTS
    None

===========================================================================*/
static mcfg_config_header_e_type mcfg_utils_check_config_header(const void *config)
{
  MCFG_CHECK_NULL_PTR_RET_FALSE(config);

  Bytef* buf = (Bytef*)config;

  MCFG_MSG_LOW_3("mcfg_utils_check_config_header(): first 3 bytes of config, 0x%x, 0x%x, 0x%x", buf[0], buf[1], buf[2]);

  if(buf[0] == MCFG_ZLIB_MAGIC_HEADER_1 && buf[1] == MCFG_ZLIB_MAGIC_HEADER_2 )
  {
    return MCFG_CONFIG_HEADER_COMPRESSED;
  }
  else if(buf[0] == MCFG_MBN_MAGIC_HEADER_1 && buf[1] == MCFG_MBN_MAGIC_HEADER_2 && buf[2] == MCFG_MBN_MAGIC_HEADER_3)
  {
    return MCFG_CONFIG_HEADER_UNCOMP_MBN;
  }
  else
  {
    return MCFG_CONFIG_HEADER_INVALID;
  }
} /* mcfg_utils_check_config_header() */

/*===========================================================================
  FUNCTION mcfg_utils_check_and_update_config

  DESCRIPTION
    Decompress or Compress a memory buffer, and swap the buffer as needed.

  DEPENDENCIES
    None

  PARAMETERS
    [in, out] config ptr
    Pointer to  config data buffer. Caller must free this buffer after use.
     
    [in] uncompressed_buffer_size
    Pointer to expected size of uncompressed data. Function will allocate this much
    memory which user must free after being done with uncompressed data.

  RETURN VALUE
    TRUE if data was successfully uncompressed.
    FALSE if case of error like invalid inputs, uncompress or  memory allocation failures.
    See F3 logs for more detailed error message.

  SIDE EFFECTS
    None
===========================================================================*/
boolean
mcfg_utils_check_and_update_config(mcfg_config_s_type *config, uint32 uncompressed_buffer_size)
{
  /*uncompress status*/
  int8  uncompress_status = Z_ERRNO;  
  uint8 *uncompressed_data_ptr = NULL;

  /*compression status*/
  boolean compress_status = FALSE;
  mcfg_config_header_e_type config_header = MCFG_CONFIG_HEADER_NONE;

  if((NULL == config->config_addr) || (config->config_len < 3) || (uncompressed_buffer_size == 0))
  {
    MCFG_MSG_ERROR("check_and_update_config: Invalid input, one of the argumets is a NULL pointer! ");
    return FALSE;
  }
  
  if(config->comp_status != MCFG_COMP_STATUS_NONE)
  {
    MCFG_MSG_HIGH_1("check_and_update_config: compression already done, comp_status %d", config->comp_status);
    return TRUE;
  }

  config_header = mcfg_utils_check_config_header(config->config_addr);

  if(config_header == MCFG_CONFIG_HEADER_COMPRESSED)
  {
    MCFG_MSG_HIGH("check_and_update_config: received config is a zlib compressed file");
    uncompressed_data_ptr = mcfg_malloc(uncompressed_buffer_size);
    if (uncompressed_data_ptr == NULL)
    {
      MCFG_MSG_ERROR_1("check_and_update_config: Failed to allocate memory for %lu bytes.", uncompressed_buffer_size );
      return FALSE;
    }

    uncompress_status = mcfg_utils_uncompress_wrapper( (uint8 *)uncompressed_data_ptr, &uncompressed_buffer_size, (const uint8 *)config->config_addr,  config->config_len);
    if (uncompress_status != Z_OK)
    {
      MCFG_MSG_ERROR_1("check_and_update_config: Decompression of data failed, status %d", uncompress_status);
      mcfg_free(uncompressed_data_ptr);
      uncompressed_data_ptr = NULL;
      return FALSE;
    }

    /* check if provide config is a valid mbn file*/
    if(mcfg_utils_check_config_header(uncompressed_data_ptr) != MCFG_CONFIG_HEADER_UNCOMP_MBN)
    {
      MCFG_MSG_ERROR("check_and_update_config: Invalid MBN header");
      mcfg_free(uncompressed_data_ptr);
      uncompressed_data_ptr = NULL;
      return FALSE;
    }

    /* swap config addr and len */
    config->comp_config_len  = config->config_len;
    config->comp_config_addr = config->config_addr;

    config->config_len  = uncompressed_buffer_size;
    config->config_addr = (void *)uncompressed_data_ptr;
    config->comp_status = MCFG_COMP_STATUS_UNCOMP_AND_SWAPPED;

    MCFG_MSG_MED_2("check_and_update_config: comp_config_len %d, comp_status %d",\
                             config->comp_config_len, config->comp_status);
  }
  else if(config_header == MCFG_CONFIG_HEADER_UNCOMP_MBN)
  {
    MCFG_MSG_HIGH("check_and_update_config: received config is un-compressed file");

    /* compress the buf if not compresses and store */  
     compress_status = mcfg_utils_compress((void **)&config->comp_config_addr, \
                                           (uint32 *)&config->comp_config_len, \
                                            config->config_addr, (uint32)config->config_len);
    if(!compress_status)
    {
      MCFG_MSG_ERROR_1("check_and_update_config: compression failed , config_len = %d", config->config_len);
      return FALSE;
    }
    else
    {
      config->comp_status = MCFG_COMP_STATUS_COMPRESSED;
      MCFG_MSG_MED_2("check_and_update_config: comp_config_len %d, comp_status %d",\
                               config->comp_config_len, config->comp_status);
    }
  }
  else
  {
    MCFG_MSG_ERROR_1("check_and_update_config: Invalid config_header %d", config_header);
    return FALSE;
  }

  return TRUE;
} /* mcfg_utils_check_and_update_config */

static boolean mcfg_utils_is_nextline
(
  char  c
)
{
  return    c == ' '
         || c == '\t'
         || c == '\n'
         || c == '\r'
         || c == '\0'
         ;
}

/*===========================================================================
  FUNCTION mcfg_utils_read_one_line

  DESCRIPTION

  DEPENDENCIES
    None

  PARAMETERS
 
  RETURN VALUE

  SIDE EFFECTS
    None

===========================================================================*/

boolean mcfg_utils_read_one_line
(
  char   *buf,
  uint16  buf_size,
  uint16 *buf_offset,
  char   *line,
  uint16  line_max_size
)
{
  boolean status = FALSE;
  uint16  offset_s =0, offset_e =0, size = 0, i = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MCFG_CHECK_NULL_PTR_RET_FALSE(buf);
  MCFG_CHECK_NULL_PTR_RET_FALSE(line);
  MCFG_ASSERT_COND_RET_FALSE(*buf_offset < buf_size);

  MCFG_MSG_LOW_2("read_one_line bufsize %d offset %d", buf_size, *buf_offset);

  size = ((*buf_offset+line_max_size) < buf_size) ? (*buf_offset+line_max_size) : buf_size;
  offset_s = *buf_offset;
  offset_e = *buf_offset;

  for (i=*buf_offset; i<size; i++)
  {
    if (mcfg_utils_is_nextline(buf[i]))
    {
      if (i == offset_s)
      {
        MCFG_MSG_LOW_1("read_one_line skip \\r & \\n %d", i);
        offset_s++;
        offset_e++;
      }
      else
      {
        offset_e = i;
        status = TRUE;
        MCFG_MSG_LOW_2("read_one_line complete at offset %d ~ %d", offset_s, offset_e);
        break;
      }
    }
  }

  if (status == TRUE)
  {
    memscpy(line, line_max_size, buf+offset_s, offset_e - offset_s);
    line[offset_e - offset_s] = '\0';

    *buf_offset = offset_e;
    MCFG_MSG_SPRINTF_1("read_one_line %s", line);
  }

  return status;
} /* mcfg_utils_read_one_line() */

