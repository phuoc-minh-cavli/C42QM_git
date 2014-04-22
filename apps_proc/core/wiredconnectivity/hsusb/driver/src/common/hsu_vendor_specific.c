/*==============================================================================

  High Speed USB Vendor Specific command handler
  
  GENERAL DESCRIPTION
  This module defines functions that handle Vendor Specific commands received 
  from the Host.
      
  EXTERNALIZED FUNCTIONS
  hsu_vendor_cmd_in_cb

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_vendor_specific.c#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  05/26/08  tl      Initial version
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_common_int.h"
#include "hsu_vendor_specific.h"
#include "jtypes_int.h"
#include "jerrno.h"
#include "jtypes_int.h"

#ifdef FEATURE_HS_USB_OS_DETECTION
#include "hsu_os_detection.h"
#endif /* FEATURE_HS_USB_OS_DETECTION */
/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
  Typedefs
==============================================================================*/
/*==============================================================================
  Variables
==============================================================================*/

/* This array should hold unique and positive values! 0 is illegal.*/
static juint8_t hsu_vendor_specific_str_idx_arr[HSU_STR_IDX_LAST] = 
{
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_PRODUCT */
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_MANUFACTURER */
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_SERIAL_NUMBER (not set)*/
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_CONFIGURATION (not set) */
};

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/
/*==============================================================================

FUNCTION  hsu_vendor_cmd_in_cb        

DESCRIPTION
This function is supplied as a callback for a Vendor Specific (IN) command
that is received from the host.
    
DEPENDENCIES
  None.

RETURN VALUE
0 for success, or an error code

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_vendor_cmd_in_cb(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size)
{
  jresult_t rc = HSU_JSUCCESS;
  HSU_USE_PARAM(wIndex);
  HSU_USE_PARAM(resp_buf);
  HSU_USE_PARAM(resp_buf_size);
  HSU_USE_PARAM(act_resp_buf_size);

  switch (bRequest)
  {

    case HSU_VENDOR_SPECIFIC_ENABLE_BYTE_PAD:
      /* The current OS is windows */
#ifdef FEATURE_HS_USB_OS_DETECTION
      hsu_os_detection_set_supported_drv(PROPRIETARY_DRV_ONLY);
#endif /* FEATURE_HS_USB_OS_DETECTION */
      break;
    default:
      HSU_MSG_ERROR_1("hsu_vendor_cmd_in_cb(): Unhandled Vendor command (%d)", bRequest);
      rc = JENOTSUP;
  }
  return rc;
}

/*==============================================================================

FUNCTION     hsu_vendor_specific_get_str_idx     

DESCRIPTION
This function returns the index that was reserved fo the given string (from 
device/configuration string descriptors)

DEPENDENCIES
None.

RETURN VALUE
Index of the given string or HSU_STR_IDX_NOT_SET

SIDE EFFECTS
  None.

==============================================================================*/
juint8_t hsu_vendor_specific_get_str_idx(hsu_str_idx_desc_type str_desc)
{
  if (str_desc >= HSU_STR_IDX_LAST  )
  {
    HSU_MSG_ERROR_1("*_get_str_idx(): Invalid str_desc = %d",str_desc);
    return HSU_STR_IDX_NOT_SET;
  }
  return hsu_vendor_specific_str_idx_arr[str_desc];
}

/*==============================================================================

FUNCTION     hsu_vendor_specific_is_idx_occupied     

DESCRIPTION
This function checks if the given index is allready occupied by some other string

DEPENDENCIES
None.

RETURN VALUE
TRUE if the given index is already occupied

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_vendor_specific_is_idx_occupied(juint8_t idx_to_check)
{
  uint8 i;
  for(i = 0; i < HSU_STR_IDX_LAST; i++)
  {
    if (hsu_vendor_specific_str_idx_arr[i] == idx_to_check)
    {
      return TRUE;
    }
  }
  return FALSE;
}

