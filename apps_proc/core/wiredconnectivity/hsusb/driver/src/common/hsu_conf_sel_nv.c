/*==============================================================================

High Speed USB Configuration Selector NV

GENERAL DESCRIPTION
This file includes function that allow reading/writing item from the NV 
memory, to be used by the High Speed USB component.

INITALIZATION AND SEQUENCING REQUIREMENTS
Functions should only be called from the TMC's context. 

Copyright (c) 2006, 2009, 2010, 2013 by QUALCOMM Technologies Incorporated.  

All Rights Reserved.

==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_conf_sel_nv.c#1 $
$DateTime: 2020/01/30 22:49:35 $

when      who        what, where, why
--------  --------   -----------------------------------------------------------
11/19/06  ke         Initial version
04/22/07  me         Adding functions to read the ESN and IMEI
12/03/13  kameya     Clean unused NV items. New architecture to update NV items.

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_conf_sel_nv.h"
//#include "diagnv.h"
//#include "task.h"
#include "hsu_common_int.h"
#include "err.h"
#include "hsu_conf_sel_types_int.h"
#include "hsu_config_selector.h"
#include <string.h>
#include <stringl/stringl.h>

#ifdef FEATURE_UIM_RUIM
#include "nvruimi.h"
#endif

#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
#include "hsu_al_task_int.h"
#include "hsu_os_abstract_sync.h"
#endif // FEATURE_GOBI && FEATURE_USB_UBIST 

#include "hsu_phy_utils_int.h"
#include "fs_public.h"

#ifdef HSU_CFG_VREG_USB_VOLT_LVL_BOOST
#include <DDIChipInfo.h>
#include "DALDeviceId.h"
#endif // HSU_CFG_VREG_USB_VOLT_LVL_BOOST

#include <string.h>
#include <stdio.h>
#include "hsu_platform.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_core_config_int.h"


/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================

CONSTANTS AND MACROS

==============================================================================*/

#define HSU_EFS_ITEMS_CONF_FILE_PERM      0777
#define EFS_ERR                           -1
#define HSU_CONF_FILE_SIZE                1024

// Prefix to where all configuration files reside
#define HSU_EFS_ITEMS_CONF_PATH_0         "/nv"
#define HSU_EFS_ITEMS_CONF_PATH_1         "/nv/item_files"
#define HSU_EFS_ITEMS_CONF_PATH           "/nv/item_files/conf"

/* Bypass uplink DBM HW accelerated path */

// HSUSB configuration file for EFS items
#define HSU_EFS_ITEMS_CONF_FILE           "/nv/item_files/conf/hsusb.conf"
#define HSU_EFS_ITEMS_CONF_FILE_NAME      "/nv/item_files/conf/hsusb"
#define HSU_EFS_ITEMS_CONF_FILE_EXT       "conf"

// Uninitialized value
#define HSU_EFS_VALUE_UNINIT              0xFFFFFFFFU


/*==============================================================================

TYPEDEFS

==============================================================================*/

// This type will hold the ESN/IMEI we read in init. We need this in order to be 
// able to get the ESN/IMEI from context other than TMC or DIAG.
typedef struct
{
  char esn_imei_buf[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE];
  boolean is_esn;
  boolean is_initialized;
} hsu_conf_sel_nv_esn_imei_type;


/*==============================================================================

VARIABLES

==============================================================================*/

static hsu_conf_sel_nv_esn_imei_type hsu_conf_sel_nv_esn_imei = {{0},FALSE,FALSE};

// Array holds all info about all EFS items.
static hsu_efs_item_info_type hsu_efs_items_info[HSU_EFS_NO_OF_EFS_ITEMS] = 
{
#ifndef USB_THREADX
  {
    HSU_EFS_HSUSB_ENABLED,
    "/nv/item_files/hsusb/hsusb_enabled",
    FALSE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },
#endif  
  {
    HSU_EFS_USB_CORE_SELECT,
    "/nv/item_files/hsusb/usb_core_select",
#ifdef FEATURE_HS_USB_CI_USB2_CORE_DEFAULT
    USB2_CI_CORE,
#else
    USB3_SNPS_CORE,
#endif
    HSU_EFS_VALUE_UNINIT,
    sizeof(hsu_usb_core_type)
  },

  {
    HSU_EFS_HIDE_FTL,
    "/nv/item_files/hsusb/hide_ftl",
    FALSE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_REMOTE_WAKEUP_DELAY,
    "/nv/item_files/hsusb/remote_wakeup_delay",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },

  {
    HSU_EFS_DISABLE_LPM,
    "/nv/item_files/hsusb/disable_lpm",
    FALSE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_RECONNECT_ON_RESET_DURING_SUSPEND,
    "/nv/item_files/hsusb/reconnect_on_reset_during_suspend",
    FALSE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

#ifdef FEATURE_HS_USB_MS_FD
  {
    HSU_EFS_NO_OF_SDCC_LUNS,
    "/nv/item_files/hsusb/no_of_sdcc_luns",
    1,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },
#endif // FEATURE_HS_USB_MS_FD

  {
    HSU_EFS_DISABLE_SLEEP_VOTING,
    "/nv/item_files/hsusb/disable_sleep_voting",
    TRUE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_PHY_CONFIG_PARAM,
    "/nv/item_files/hsusb/phy_config",
    0xFFFFFFFF,
    HSU_EFS_VALUE_UNINIT,
    sizeof(hsu_phy_utils_config_par_type)
  },

  {
    HSU_EFS_DEVICE_COMPOSITION,
    "/nv/item_files/hsusb/device_composition",
    HSU_EFS_VALUE_UNINIT,
    HSU_EFS_VALUE_UNINIT,
    sizeof(hsu_composition_enum)
  },

  {
     HSU_EFS_LDO_POWER_COLLAPSE,
    "/nv/item_files/hsusb/ldo_power_collapse",
    TRUE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_GPIO_REMOTE_WAKEUP_TIMEOUT,
    "/nv/item_files/hsusb/gpio_remote_wakeup_timeout",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },

  {
    HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING,
    "/nv/item_files/hsusb/aggressive_ep0_error_capturing",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_DISABLE_A2_PC_FOR_SUSPEND,
    "/nv/item_files/hsusb/disable_a2_pc_for_suspend",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_A2_PC_DEBOUNCE_TIME_FOR_SUSPEND,
    "/nv/item_files/hsusb/a2_pc_debounce_time_for_suspend",
    0xFFFFFFFF,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },

  {
    HSU_EFS_AHB2AHB_BYPASS,
    "/nv/item_files/hsusb/ahb2ahb_bypass",
    FALSE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },

  {
    HSU_EFS_IGNORE_DTR_DROP,
    "/nv/item_files/hsusb/ignore_dtr_drop",
    FALSE,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint8)
  },
#ifdef FEATURE_HS_USB_MBIM
  {
    HSU_EFS_MBIM_WMTU,
    "/nv/item_files/hsusb/mbim_wmtu",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint16)
  },
#endif // FEATURE_HS_USB_MBIM

  {
    HSU_EFS_PHY_PERF_SETTINGS,
    "/nv/item_files/hsusb/performance_settings",
    HSU_CLKMGR_SPEED_MID,
    HSU_EFS_VALUE_UNINIT,
    sizeof(hsu_clkmgr_speed_level_type)
  },

  {
    HSU_EFS_UL_BAM_BYPASS,
    "/nv/item_files/hsusb/ul_bam_bypass",
    1,
    HSU_EFS_VALUE_UNINIT,
    sizeof(boolean)
  },

  // Please make sure same is updated in hsu_common.h
  {
    HSU_EFS_ECM_RX_DSM_ITEM_SIZE,
    "/nv/item_files/hsusb/ecm_rx_dsm_item_size",
    (16*1024),
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },

  // Please make sure same is updated in hsu_common.h
  {
    HSU_EFS_ECM_RX_DSM_ITEM_CNT,
    "/nv/item_files/hsusb/ecm_rx_dsm_item_cnt",
    25,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },

  {
    HSU_EFS_ECM_RX_TIMEOUT_MS,
    "/nv/item_files/hsusb/ecm_rx_timeout_ms",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },
  
  {
    HSU_EFS_DUN_LOG_ENABLED,
    "/nv/item_files/hsusb/dun_log_enabled",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(uint32)
  },
    
  {
    HSU_EFS_AUDIO_SELF_LOOPBACK_ENABLED,
    "/nv/item_files/hsusb/audio_self_loopback_enabled",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(boolean)
  },

  {
    HSU_EFS_COMPLIANCE_TEST_MODE_ENABLED,
    "/nv/item_files/hsusb/compliance_test_mode_enabled",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(boolean)
  },
  
  {
    HSU_EFS_SS_PHY_DISABLED,
    "/nv/item_files/hsusb/ss_phy_disabled",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(boolean)
  },

  /*=======================================
   Add new EFS item info here
  {
    HSU_EFS_NEW_ITEM,
    "/nv/item_files/hsusb/new_item",
    0,
    HSU_EFS_VALUE_UNINIT,
    sizeof(new_item)
  }
  =======================================*/

};


/*==============================================================================

FUNCTION DECLARATIONS AND DEFINITIONS FOR MODULE

==============================================================================*/

/*==============================================================================

NV APIs

==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_READ

DESCRIPTION
Read an item from the NV memory.

DEPENDENCIES
Context must be of TMC or DIAG, otherwise the function ASSERTs.

RETURN VALUE
nv_stat_enum_type (see nv.h).

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
nv_stat_enum_type hsu_conf_sel_nv_read
(
  nv_items_enum_type  nv_item,
  nv_item_type *      data_ptr  
)
{
  return NV_NOTACTIVE_S;
} // hsu_conf_sel_nv_read


/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_IMEI_READ

DESCRIPTION
Read the IMEI from NV and convert it into ASCII.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_imei_read
(
  char *return_buff_ptr,                   // Pointer to return buffer
  uint32 return_buff_size                  // The size of the giver buffer
) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;
  uint8 imei_bcd_len = 0;
  uint8 n = 0;
  uint8 digit;

  // We need an extra digit for the id digit so we keep a size of 
  // (NV_UE_IMEI_SIZE-1)*2+1
  char imei_ascii[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE+1];

#if !defined (FEATURE_GOBI) || !defined (FEATURE_USB_UBIST)
  return_buff_ptr[0] = '\0';
#endif // !FEATURE_GOBI || !FEATURE_USB_UBIST

  nv_operation_status = hsu_conf_sel_nv_read(NV_UE_IMEI_I, &nv_item_data);

  // Note that because IMEI is a special read only variable, it may return 
  // an all 0s value to indicate that it has never been written rather than 
  // the result code that indicates the item is not active.
  if ((nv_operation_status != NV_DONE_S) || (nv_item_data.ue_imei.ue_imei[0] == 0))
  {
    /*    HSU_MSG_HIGH_1(
    "*_nv_imei_read(): Couldn't read NV_UE_IMEI_I from NV (status = %d)",
    nv_operation_status);
    ERR(
    "*_nv_imei_read(): Couldn't read NV_UE_IMEI_I from NV (status = %d)",
    nv_operation_status, 0, 0);*/
    return FALSE;
  }
#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
  return_buff_ptr[0] = '\0';
#endif // FEATURE_GOBI && FEATURE_USB_UBIST

  // Convert it to ASCII
  // The first byte is the imei_bcd_len
  imei_bcd_len = nv_item_data.ue_imei.ue_imei[0];

  // Each byte contains two digits.
  // The first byte contains an ID (that we should ignore later on 
  // when we copy the imei_ascii) and then the first actual digit.
  if( imei_bcd_len <= (NV_UE_IMEI_SIZE-1) )
  {
    // This is a valid IMEI
    memset(imei_ascii, 0, HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE+1);

    for( n = 1; n <= imei_bcd_len; n++ )
    {
      digit = nv_item_data.ue_imei.ue_imei[n] & 0x0F;
      if( ( digit <= 9 ) || ( n <= 1 ) )
      {
        imei_ascii[ (n - 1) * 2 ] = digit + '0';
      }
      else
      {
        imei_ascii[ (n - 1) * 2 ] = '\0';
        break;
      }

      digit = nv_item_data.ue_imei.ue_imei[n] >> 4;
      if( ( digit <= 9 ) || ( n <= 1 ) )
      {
        imei_ascii[ ((n - 1) * 2) + 1 ] = digit + '0';
      }
      else
      {
        imei_ascii[ ((n - 1) * 2) + 1 ] = '\0';
        break;
      }
    }
    imei_ascii[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE] = '\0';

    // Lose the first byte because it is just the ID.
    strlcpy(return_buff_ptr, imei_ascii + 1, (int)return_buff_size);
    hsu_conf_sel_nv_esn_imei.is_esn = FALSE;
    strlcpy(hsu_conf_sel_nv_esn_imei.esn_imei_buf,imei_ascii + 1, sizeof(hsu_conf_sel_nv_esn_imei.esn_imei_buf));
    hsu_conf_sel_nv_esn_imei.is_initialized = TRUE;
  }

  return TRUE;
} // hsu_conf_sel_nv_imei_read


/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_ESN_READ

DESCRIPTION
Read the ESN from NV.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_esn_read
(
  char * return_buff_ptr ,                 // Pointer to return buffer
  uint32 return_buff_size                  // The size of the giver buffer
) 
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

#ifdef FEATURE_UIM_RUIM
  nvruim_esn_usage_type esn_usage_type;
#endif

#ifdef FEATURE_UIM_RUIM
  // RUIM, make sure we can read the ESN from the NV.
  esn_usage_type = nvruim_get_esn_usage();
  nvruim_set_esn_usage(NV_RUIM_USE_ESN);
#endif

  nv_operation_status = hsu_conf_sel_nv_read(NV_ESN_I, &nv_item_data);

#ifdef FEATURE_UIM_RUIM
  // Restore the previous value of nvruim_esn_usage_type.
  nvruim_set_esn_usage(esn_usage_type);
#endif

  // since the ESN is read only field it may return 0 to indicate that it is 
  // not active the result code that indicates the item is not active
  if ((nv_operation_status != NV_DONE_S) || (nv_item_data.esn.esn == 0))
  {
    /*    HSU_MSG_HIGH_1(
    "*_nv_esn_read(): Couldn't read NV_ESN_I from NV (status = %d)",
    nv_operation_status);
    ERR(
    "*_nv_esn_read(): Couldn't read NV_ESN_I from NV (status = %d)",
    nv_operation_status, 0, 0);*/
    return FALSE;
  }

  // Convert the 32 bit unsigned number to a string.
  (void)snprintf(return_buff_ptr, (int)return_buff_size, "%lx", nv_item_data.esn.esn);

  hsu_conf_sel_nv_esn_imei.is_esn = TRUE;
  (void)snprintf(
      hsu_conf_sel_nv_esn_imei.esn_imei_buf, 
      sizeof(hsu_conf_sel_nv_esn_imei.esn_imei_buf), 
      "%lx", 
      nv_item_data.esn.esn);
  hsu_conf_sel_nv_esn_imei.is_initialized = TRUE;
  return TRUE;
} // hsu_conf_sel_nv_esn_read


#if !defined (T_UNDP)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_ESN_IMEI_READ

DESCRIPTION
Read the ESN or IMEI from NV. The function tries to read the ESN first. 
If it fails it tries to read the IMEI. 

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_esn_imei_read
(
  char * return_buff_ptr,                  // Pointer to return buffer
  uint32 return_buff_size                  // The size of the giver buffer
) 
{
  boolean ret_val;

  ret_val = hsu_conf_sel_nv_esn_read(return_buff_ptr, return_buff_size);
  if (ret_val == FALSE)
  {
    // If we didn't manage to read the ESN - try to read the IMEI.
    ret_val = hsu_conf_sel_nv_imei_read(return_buff_ptr, return_buff_size);
    if (ret_val == FALSE)
    {
      // We didn't manage to read either the ESN or the IMEI.
      HSU_MSG_ERROR(
        "*_esn_imei_read(): Couldn't read NV_ESN_I and NV_UE_IMEI_I from NV");
    }
  }

  return ret_val;
} // hsu_conf_sel_nv_esn_imei_read
#endif // !T_UNDP


/*==============================================================================
FUNCTION          HSU_CONF_SEL_NV_UIM_FIRST_INST_CLASS_READ

DESCRIPTION
Read the setting of whether UICC support should be turned on.

DEPENDENCIES
None.

RETURN VALUE
FALSE for failure.
TRUE for success.

SIDE EFFECTS
The calling function is blocked until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_uim_first_inst_class_read
(
  nv_uim_first_inst_class_type *nv_uim_first_inst_class_value
)
{
  nv_item_type      nv_item_data;
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_UIM_FIRST_INST_CLASS_I, 
    &nv_item_data);
  if(nv_operation_status != NV_DONE_S)
  {
    HSU_MSG_ERROR_1(
      "*_uim_first_inst_class_read):"
      " Couldn't read NV_UIM_FIRST_INST_CLASS_I from NV (status = %d)",
      nv_operation_status);
    return FALSE;
  }
  *nv_uim_first_inst_class_value = nv_item_data.uim_first_inst_class;
  return TRUE;
}// hsu_conf_sel_nv_uim_first_inst_class_read


/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_GET_ESN

DESCRIPTION
If this target is working with ESN then return the ESN.

DEPENDENCIES
hsu_conf_sel_nv_esn_imei_read() should be called prior to calling this function.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None

==============================================================================*/
boolean hsu_conf_sel_nv_get_esn
(
  char * return_buff_ptr ,                 // Pointer to return buffer
  uint32 return_buff_size                  // The size of the giver buffer
)
{
  if ((hsu_conf_sel_nv_esn_imei.is_initialized) 
        && (hsu_conf_sel_nv_esn_imei.is_esn))
  {
    // Convert the 32 bit unsigned number to a string.
    strlcpy(
      return_buff_ptr, 
      hsu_conf_sel_nv_esn_imei.esn_imei_buf, 
      (int)return_buff_size);
    return TRUE;
  }
  
  HSU_MSG_HIGH("hsu_conf_sel_nv_get_esn: Failed to read ESN");
  return FALSE;
}// hsu_conf_sel_nv_get_esn


/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_GET_IMEI

DESCRIPTION
If this target is working with IMEI then return the IMEI.

DEPENDENCIES
hsu_conf_sel_nv_esn_imei_read() should be called prior to calling this function!

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None

==============================================================================*/
boolean hsu_conf_sel_nv_get_imei
(
  char * return_buff_ptr ,                 // Pointer to return buffer
  uint16 return_buff_size                  // The size of the giver buffer
)
{
  if ((hsu_conf_sel_nv_esn_imei.is_initialized) 
        && (!hsu_conf_sel_nv_esn_imei.is_esn))
  {
    // Convert the 32 bit unsigned number to a string.
    strlcpy(
      return_buff_ptr, 
      hsu_conf_sel_nv_esn_imei.esn_imei_buf, 
      (int)return_buff_size);
    return TRUE;
  }

  HSU_MSG_HIGH("hsu_conf_sel_nv_get_imei: Failed to read IMEI");
  return FALSE;
} // hsu_conf_sel_nv_get_imei


#if defined (FEATURE_GOBI) && defined (FEATURE_USB_UBIST)
/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_MEID_READ

DESCRIPTION
Read the MEID from NV.  The MEID is converted to hex string 
   
DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
The calling function is stopped until NV read operation is finished.

==============================================================================*/
boolean hsu_conf_sel_nv_meid_read
(
  char * return_buff_ptr,                  // Pointer to return buffer
  uint32 return_buff_size                  // The size of the giver buffer
) 
{
  int    meid_size = 15; // 14 digits + eos and do not include checksum
  int    index;
  uint64 meid;
  char   meid_ascii[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE];
  
  nv_stat_enum_type nv_operation_status;

  nv_operation_status = hsu_conf_sel_nv_read(NV_MEID_I, 
                                             (nv_item_type *)&meid);
  if (nv_operation_status != NV_DONE_S)
  {
    return FALSE;
  }
  return_buff_ptr[0] = '\0';

  // Converting 4 bit decimal string to HEX string.
  for (index = 0; index < meid_size-1; index++) 
  {
     char asc;
     asc = (meid >> ((meid_size-2 - index)*4)) & 0xf;
     meid_ascii[index] = (asc <= 9) ? asc+'0' : asc-10+'A';
  }
  meid_ascii[index] = 0;

  strlcpy(return_buff_ptr, meid_ascii, return_buff_size);

  return TRUE;
} // hsu_conf_sel_nv_meid_read
#endif // FEATURE_GOBI && FEATURE_USB_UBIST


/*==============================================================================
APIS TO RETURN LOCAL CACHED VALUE OF EFS ITEMS.
PLEASE MAINTAIN API ORDER AS PER HSU_EFS_ITEM_TYPE. THANKS!
==============================================================================*/

/*==============================================================================

FUNCTION  HSU_CONF_SEL_IS_USB_CORE_SNPS

==============================================================================*/
boolean hsu_conf_sel_is_usb_core_snps(void)
{
#if 0
#ifdef FEATURE_HS_USB_SNPS_CORE
  if(USB3_SNPS_CORE == hsu_efs_items_info[HSU_EFS_USB_CORE_SELECT].current_value)
  {
     return TRUE;
  }
  else
#endif
#endif
  // Always SNPS core for this target
  return TRUE;
} // hsu_conf_sel_is_usb_core_snps


/*==============================================================================

FUNCTION  HSU_CONF_SEL_HIDE_FTL

==============================================================================*/
boolean hsu_conf_sel_hide_ftl(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_HIDE_FTL].current_value;
} // hsu_conf_sel_hide_ftl


/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_RM_WAKEUP_DELAY

==============================================================================*/
uint32 hsu_conf_sel_get_rm_wakeup_delay(void)
{
  return hsu_efs_items_info[HSU_EFS_REMOTE_WAKEUP_DELAY].current_value;
} // hsu_conf_sel_get_rm_wakeup_delay


/*==============================================================================

FUNCTION  HSU_CONF_SEL_LPM_DISABLED

==============================================================================*/
boolean hsu_conf_sel_lpm_disabled(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_DISABLE_LPM].current_value;
} // hsu_conf_sel_lpm_disabled

/*==============================================================================

FUNCTION  HSU_CONF_SEL_RECONNECT_ON_RESET_DURING_SUSPEND

==============================================================================*/
boolean hsu_conf_sel_reconnect_on_reset_during_suspend(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_RECONNECT_ON_RESET_DURING_SUSPEND].current_value;
} // hsu_conf_sel_reconnect_on_reset_during_suspend

#ifdef FEATURE_HS_USB_MS_FD
/*==============================================================================

FUNCTION  HSU_CONF_SEL_NUMBER_OF_SDCC_LUNS

==============================================================================*/
uint8 hsu_conf_sel_number_of_sdcc_luns(void)
{
  return (uint8)hsu_efs_items_info[HSU_EFS_NO_OF_SDCC_LUNS].current_value;
} // hsu_conf_sel_number_of_sdcc_luns
#endif // FEATURE_HS_USB_MS_FD


/*==============================================================================

FUNCTION  HSU_CONF_SEL_DISABLE_SLEEP_VOTING

==============================================================================*/
boolean hsu_conf_sel_disable_sleep_voting(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_DISABLE_SLEEP_VOTING].current_value;
} // hsu_conf_sel_disable_sleep_voting

/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_PHY_CONFIG_PARAM

==============================================================================*/
hsu_phy_utils_config_par_type hsu_conf_sel_get_phy_config_param(void)
{
  // hack up the code to return 0 - reading a bad EFS value can be dangerous
  hsu_phy_utils_config_par_type config_par;
  config_par.value = hsu_efs_items_info[HSU_EFS_PHY_CONFIG_PARAM].current_value;
  return config_par;
} // hsu_conf_sel_get_phy_config_param

/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_EFS_PERF_SETTINGS

==============================================================================*/
hsu_clkmgr_speed_level_type hsu_conf_sel_get_efs_perf_settings(void)
{
 return (hsu_clkmgr_speed_level_type)hsu_efs_items_info[HSU_EFS_PHY_PERF_SETTINGS].current_value;
} // hsu_conf_sel_get_efs_perf_settings


/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_DEVICE_COMPOSITION

==============================================================================*/
hsu_composition_enum hsu_conf_sel_get_device_composition(void) 
{
  return (hsu_composition_enum)hsu_efs_items_info[HSU_EFS_DEVICE_COMPOSITION].current_value;
} // hsu_conf_sel_get_device_composition


/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_LDO_POWER_COLLAPSE

==============================================================================*/
boolean hsu_conf_sel_get_ldo_power_collapse(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_LDO_POWER_COLLAPSE].current_value;
} // hsu_conf_sel_get_ldo_power_collapse


/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_REMOTE_WAKEUP_TIMEOUT

==============================================================================*/
uint32 hsu_conf_sel_get_remote_wakeup_timeout(void)
{
  return hsu_efs_items_info[HSU_EFS_GPIO_REMOTE_WAKEUP_TIMEOUT].current_value;
} // hsu_conf_sel_get_remote_wakeup_timeout_val


/*==============================================================================

FUNCTION  HSU_CONF_SEL_AGGRESSIVE_EP0_ERROR_CAPTURING_ENABLED

==============================================================================*/
uint8 hsu_conf_sel_aggressive_ep0_error_capturing_enabled(void)
{
  return (uint8)hsu_efs_items_info[HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING].current_value;
} // hsu_conf_sel_aggressive_ep0_error_capturing_enabled


/*==============================================================================

FUNCTION  HSU_CONF_SEL_DISABLE_A2_PC_FOR_SUSPEND

==============================================================================*/
boolean hsu_conf_sel_disable_a2_pc_for_suspend(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_DISABLE_A2_PC_FOR_SUSPEND].current_value;
} // hsu_conf_sel_disable_a2_pc_for_suspend


/*==============================================================================

FUNCTION  HSU_CONF_SEL_GET_A2_PC_DEBOUNCE_TIME_FOR_SUSPEND

==============================================================================*/
hsu_phy_utils_config_pll hsu_conf_sel_get_a2_pc_debounce_time_for_suspend(void)
{
  hsu_phy_utils_config_pll config_par;
  config_par.value = hsu_efs_items_info[HSU_EFS_A2_PC_DEBOUNCE_TIME_FOR_SUSPEND].current_value;
  return config_par;
} // hsu_conf_sel_get_a2_pc_debounce_time_for_suspend


/*==============================================================================

FUNCTION  HSU_CONF_SEL_IS_AHB2AHB_BYPASS_ENABLED

==============================================================================*/
boolean hsu_conf_sel_is_ahb2ahb_bypass_enabled(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_AHB2AHB_BYPASS].current_value;
} // hsu_conf_sel_is_ahb2ahb_bypass_enabled


/*==============================================================================

FUNCTION  HSU_CONF_SEL_IGNORE_DTR_DROP

==============================================================================*/
boolean hsu_conf_sel_ignore_dtr_drop(void)
{
  return (boolean)hsu_efs_items_info[HSU_EFS_IGNORE_DTR_DROP].current_value;
} // hsu_conf_sel_ignore_dtr_drop

#ifdef FEATURE_HS_USB_MBIM
/*==============================================================================

FUNCTION  HSU_CONF_SEL_MBIM_WMTU

==============================================================================*/
uint16 hsu_conf_sel_mbim_wMTU(void)
{
  return (uint16)hsu_efs_items_info[HSU_EFS_MBIM_WMTU].current_value;
} // hsu_conf_sel_mbim_wMTU
#endif // FEATURE_HS_USB_MBIM

/*==============================================================================

FUNCTION  HSU_CONF_SEL_UL_BAM_BYPASSED

==============================================================================*/
boolean hsu_conf_sel_ul_bam_bypassed(void)
{
  // Never bypass DBM for chipidea core
  return FALSE;
}

/*==============================================================================

FUNCTION  HSU_GET_ECM_RX_DSM_ITEM_SIZE

==============================================================================*/
uint32 hsu_conf_sel_get_ecm_rx_dsm_item_size(void)
{
  return hsu_conf_sel_nv_efs_get(HSU_EFS_ECM_RX_DSM_ITEM_SIZE);
}


/*==============================================================================

FUNCTION  HSU_GET_ECM_RX_DSM_ITEM_CNT

==============================================================================*/
uint32 hsu_conf_sel_get_ecm_rx_dsm_item_cnt(void)
{
  return hsu_conf_sel_nv_efs_get(HSU_EFS_ECM_RX_DSM_ITEM_CNT);
}


/*==============================================================================

FUNCTION  HSU_GET_ECM_RX_TIMEOUT

==============================================================================*/
uint32 hsu_conf_sel_get_ecm_rx_timeout(void)
{
  return hsu_conf_sel_nv_efs_get(HSU_EFS_ECM_RX_TIMEOUT_MS);
}

/*==============================================================================
BASIC APIS
==============================================================================*/

/*==============================================================================

FUNCTION    HSU_CONF_SEL_NV_EFS_GET

DESCRIPTION
Generalized API to retrieve local cache value of an EFS item.

DEPENDENCIES
None

RETURN VALUE
Local cache value of an EFS item.

SIDE EFFECTS
None

==============================================================================*/
uint32 hsu_conf_sel_nv_efs_get
(
  hsu_efs_item_type type
)
{
  return hsu_efs_items_info[type].current_value;
} // hsu_conf_sel_nv_efs_get


/*==============================================================================
  
FUNCTION      HSU_CONF_SEL_NV_EFS_NV_WRITE

DESCRIPTION
Write an item to the EFS.
  
DEPENDENCIES
None
  
RETURN VALUE
TRUE for success, FALSE if writing failed.
  
SIDE EFFECTS
None
  
==============================================================================*/
boolean hsu_conf_sel_nv_efs_write
(
 hsu_efs_item_type type,
 uint32 value
)
{
  if (efs_put(hsu_efs_items_info[type].filename,
              &value,
              hsu_efs_items_info[type].size,
              O_CREAT|O_AUTODIR, 
              HSU_EFS_ITEMS_CONF_FILE_PERM) != EFS_ERR)
  {
    HSU_MSG_HIGH("hsu_conf_sel_nv_efs_write succeeded");
    return TRUE;
  }

  HSU_MSG_HIGH_1("hsu_conf_sel_nv_efs_write failed",type);
  return FALSE;
} // hsu_conf_sel_nv_efs_write


/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_EFS_READ

DESCRIPTION
Reads value of EFS item from EFS.

DEPENDENCIES
None.

RETURN VALUE
TRUE for success, FALSE for failure.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_nv_efs_read
(
  hsu_efs_item_type type,
  uint32* value
) 
{
  hsu_platform_info_type const * platform_info = hsu_platform_get_info();
  if (FALSE == platform_info->is_flashless_boot)
  {
    // Zero out memory before reading from EFS
    *value = 0x0;

    // Read the NV item
    if (efs_get(hsu_efs_items_info[type].filename,
        value, hsu_efs_items_info[type].size) != EFS_ERR)
    {
      HSU_MSG_HIGH_1("hsu_conf_sel_nv_efs_read succeeded: %d", type);
      return TRUE;
    }

    HSU_MSG_HIGH_2("hsu_conf_sel_nv_efs_read failed", type, *value);    
  }
  // else return FALSE and skip all efs reads for flashless boot
  return FALSE;
} // hsu_conf_sel_nv_efs_read


/*==============================================================================

FUNCTION          HSU_CONF_SEL_NV_EFS_UPDATE

DESCRIPTION
Reads value of a EFS item from EFS and updates it to local cache.
If EFS read fails updates local cache with default value.

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_nv_efs_update
(
  hsu_efs_item_type type
) 
{
  if(!hsu_conf_sel_nv_efs_read(type, &hsu_efs_items_info[type].current_value))
  {
    // If NV item is not read from EFS, use default value.
    hsu_efs_items_info[type].current_value = 
      hsu_efs_items_info[type].default_value;
    HSU_MSG_ERROR_2("Using default NV value for item %d, default_value = %d", type, 
      hsu_efs_items_info[type].current_value);
  }
} // hsu_conf_sel_nv_efs_update


/*==============================================================================

FUNCTION      HSU_CONF_SEL_NV_EFS_UPDATE_ALL_ITEMS

DESCRIPTION
Updates value of all EFS items and copies them to a local cache. Currently, 
reading from NV is done only once at boot up. Changing EFS item dynamically
is not supported.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None

==============================================================================*/
void hsu_conf_sel_nv_efs_update_all_items
(
  void
)
{
  int i;
  hsu_platform_info_type const* platform_info = hsu_platform_get_info();

  // Perform sanity check on hsu_efs_items_info array before updating EFS items.
  for(i=0; i<HSU_EFS_NO_OF_EFS_ITEMS; i++) 
  {
    // Check to avoid mis-matched array index and hsu_efs_item_type
    if(i != hsu_efs_items_info[i].type) 
    {
      HSU_ERR_FATAL("hsu_conf_sel_nv_efs_update_all_items(): "
        "Order mis-match between hsu_efs_items_info data and hsu_efs_item_type."
        ,0,0,0);
    }

    // Check if size of any EFS item is greater than sizeof(uint32)
    if(hsu_efs_items_info[i].size > sizeof(uint32)) 
    {
      HSU_ERR_FATAL("hsu_conf_sel_nv_efs_update_all_items(): "
        "Size of EFS item greater than 4 bytes i.e. sizeof(uint32)."
        ,0,0,0);
    }
  }
  

  // Process each EFS item separately
  for(i=0; i<HSU_EFS_NO_OF_EFS_ITEMS; i++) 
  {
    switch(i) 
    {
      case HSU_EFS_HIDE_FTL:
      case HSU_EFS_REMOTE_WAKEUP_DELAY:
      case HSU_EFS_RECONNECT_ON_RESET_DURING_SUSPEND:
#ifdef FEATURE_HS_USB_MS_FD
      case HSU_EFS_NO_OF_SDCC_LUNS:
#endif // FEATURE_HS_USB_MS_FD
      case HSU_EFS_DISABLE_SLEEP_VOTING:
      case HSU_EFS_PHY_CONFIG_PARAM:
      case HSU_EFS_DEVICE_COMPOSITION:        
      case HSU_EFS_LDO_POWER_COLLAPSE:
      case HSU_EFS_GPIO_REMOTE_WAKEUP_TIMEOUT:
      case HSU_EFS_AGGRESSIVE_EP0_ERROR_CAPTURING:
      case HSU_EFS_DISABLE_A2_PC_FOR_SUSPEND:
      case HSU_EFS_A2_PC_DEBOUNCE_TIME_FOR_SUSPEND:
      case HSU_EFS_AHB2AHB_BYPASS:
      case HSU_EFS_UL_BAM_BYPASS:
      case HSU_EFS_ECM_RX_DSM_ITEM_SIZE:
      case HSU_EFS_ECM_RX_DSM_ITEM_CNT:
      case HSU_EFS_ECM_RX_TIMEOUT_MS:
      case HSU_EFS_DUN_LOG_ENABLED:
      case HSU_EFS_AUDIO_SELF_LOOPBACK_ENABLED:
      case HSU_EFS_COMPLIANCE_TEST_MODE_ENABLED:
      case HSU_EFS_PHY_PERF_SETTINGS:
      {
        hsu_conf_sel_nv_efs_update(hsu_efs_items_info[i].type);
      }
      break;

      case HSU_EFS_USB_CORE_SELECT:
      {
        // Ignore any user inserted value for 9x45
        hsu_efs_items_info[i].current_value = hsu_efs_items_info[i].default_value;
      }
      break;

      case HSU_EFS_DISABLE_LPM:
      {
        hsu_conf_sel_nv_efs_update(hsu_efs_items_info[i].type);
        hsu_os_bus_set_lpm_feature_is_active(
          !hsu_efs_items_info[i].current_value);
      }
      break;

      case HSU_EFS_IGNORE_DTR_DROP:
      {
        hsu_conf_sel_nv_efs_update(hsu_efs_items_info[i].type);
        hsu_efs_items_info[i].current_value = 
          hsu_efs_items_info[i].current_value && 
          (HSU_CORE_IDX_PRIMARY == HSU_CORE_CONFIG_USB_IDX);
      }
      break;

#ifdef FEATURE_HS_USB_MBIM
      case HSU_EFS_MBIM_WMTU:
      {
        hsu_conf_sel_nv_efs_update(hsu_efs_items_info[i].type);
        if(!((hsu_efs_items_info[i].current_value >= USB_MIN_MTU_SIZE) 
          && (hsu_efs_items_info[i].current_value <= USB_MAX_SEGMENT_SIZE)))
        {
          hsu_efs_items_info[i].current_value = USB_DEFAULT_MTU_SIZE;
        }
      }
      break;
#endif // FEATURE_HS_USB_MBIM
      
      case HSU_EFS_SS_PHY_DISABLED:
      {
        if (platform_info->is_ss_phy_disabled)
        {
          hsu_efs_items_info[i].default_value = TRUE;
        }
        hsu_conf_sel_nv_efs_update(hsu_efs_items_info[i].type);
      }
      break;
      
      default:
        HSU_ERR_FATAL("Memory corruption while processing EFS items",0,0,0);
    } // switch(i)

  } // for(i=0;i<HSU_EFS_NO_OF_EFS_ITEMS;i++)
  
} // hsu_conf_sel_nv_efs_update_all_items


/*==============================================================================

FUNCTION      HSU_CONF_SEL_NV_EFS_INIT_CONFIG_FILE

DESCRIPTION
In case HSUSB EFS items configuration file doesn't exist, create one and fill 
it with the paths to all HSUSB EFS items.  QXDM will automatically redirect 
read/write to the file system to the processor that this code is running on.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None

==============================================================================*/
void hsu_conf_sel_nv_efs_init_config_file
(
  void
)
{
  static char hsu_nv_buf[HSU_CONF_FILE_SIZE];
  static char hsu_nv_conf_file_name[HSU_CONF_FILE_SIZE];
  char * seek_ptr;
  fs_size_t file_size, new_file_size;
  int i, file_num=0;
  int res;

  hsu_platform_info_type const * platform_info = hsu_platform_get_info();
  if (FALSE == platform_info->is_flashless_boot)
  {
    seek_ptr = hsu_nv_buf;
    file_size = 0;
    new_file_size = 0;

    for(i = 0; i<HSU_EFS_NO_OF_EFS_ITEMS; i++)
    {
      // Generate content of conf file
      new_file_size += strlen(hsu_efs_items_info[i].filename) + 1;

      // When file size is greater than HSU_CONF_FILE_SIZE, split the content 
      // into multiple files.
      if((new_file_size > HSU_CONF_FILE_SIZE))
      {

        // generate a new file name
        res = snprintf(hsu_nv_conf_file_name,
            sizeof(hsu_nv_conf_file_name),
            "%s%d.%s",
            HSU_EFS_ITEMS_CONF_FILE_NAME,
            file_num,
            HSU_EFS_ITEMS_CONF_FILE_EXT);

        if(res < 0 || res >= sizeof(hsu_nv_conf_file_name))
        {
          HSU_ERR_FATAL("new files name size error or overflow: res %d, siz %d",
              res, sizeof(hsu_nv_conf_file_name), 0);
        }

        HSU_ASSERT(file_size <= HSU_CONF_FILE_SIZE);
        if(efs_get(hsu_nv_conf_file_name, hsu_nv_buf, file_size) != file_size)
        {

          if(efs_put(hsu_nv_conf_file_name,
                hsu_nv_buf,
                file_size,
                O_AUTODIR|O_CREAT|O_RDWR,
                HSU_EFS_ITEMS_CONF_FILE_PERM) < 0)
          {
            HSU_ERR_FATAL("Error in writing conf file", 0, 0, 0);
          }
        }

        // Initialize for next file.
        new_file_size = strlen(hsu_efs_items_info[i].filename) + 1;;
        seek_ptr = hsu_nv_buf;
        file_num++;
      }

      // Generate the content and update file size.
      file_size = new_file_size;

      res = snprintf(seek_ptr, sizeof(hsu_nv_buf), "%s\n", hsu_efs_items_info[i].filename);

      if (res < 0 || res >= sizeof(hsu_nv_buf))
      {
        HSU_ERR_FATAL("new files name size error or overflow: res %d, siz %d",
            res, sizeof(hsu_nv_buf), 0);
      }

      seek_ptr += strlen(hsu_efs_items_info[i].filename) + 1;
    }

    // Generate the last config file
    res = snprintf(hsu_nv_conf_file_name,
        sizeof(hsu_nv_conf_file_name),
        "%s%d.%s",
        HSU_EFS_ITEMS_CONF_FILE_NAME,
        file_num,
        HSU_EFS_ITEMS_CONF_FILE_EXT);

    if (res < 0 || res >= sizeof(hsu_nv_conf_file_name))
    {
      HSU_ERR_FATAL("new files name size error or overflow: res %d, siz %d",
          res, sizeof(hsu_nv_buf), 0);
    }

    HSU_ASSERT(file_size <= HSU_CONF_FILE_SIZE);
    if (efs_get(hsu_nv_conf_file_name, hsu_nv_buf, file_size) != file_size)
    {
      if (efs_put(hsu_nv_conf_file_name,
            hsu_nv_buf,
            file_size,
            O_AUTODIR|O_CREAT|O_RDWR,
            HSU_EFS_ITEMS_CONF_FILE_PERM) < 0)
      {
        HSU_ERR_FATAL("Error in writing conf file", 0, 0, 0);
      }
    } 
  }
} // hsu_conf_sel_nv_efs_init_config_file

