/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      Session Manager NV Items Module

GENERAL DESCRIPTION
  This module contains the SM interface routines for the Non-Volatile Memory
  module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2006-2010, 2014-2017 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //components/rel/core.tx/6.0/debugtools/err/src/err_nv_efs.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
11/24/10   rh      Added NV item for sending NO_FIX as Intermediate Report
06/10/09   gk      Added Get UTC offset from NV
01/15/09    atien  XTRA-T support
01/25/09   ns      Added support for QWiP NV item
08/22/08   jlp     Added support of emergency call simulation for simulating
                   E911 A-GPS
09/14/07    gk     NMEA CONFIG set parameters support
08/01/07    rw     Add support for XTRA feature
07/31/07    ank    Added Dynamic QoS and Override logic.
05/01/07    lt     Restore the default for cgps_mo_method back to MO_CP.
04/17/07    lt     Changed default for cgps_mo_method to MO_CP_LOC_EST.
04/12/07    ah     Added default read values for all SM-related NV items.
12/22/06    ank    Added default values for various NV items.
11/14/06    ah     Initial Release.

=============================================================================

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include "comdef.h"
#include "msg.h"
#include "err_nv_efs.h"


/* ensure the order of the items in this table is same as dnt_nv_efs_reg_item_type */
static const dnt_nv_efs_reg_item_struct err_nv_reg_items[] =
{
  /* data location, item file name, size */

  /* DNT_NV_EFS_ERR_FATAL_ACTION */
  {"err_fatal_action", sizeof(uint8), 0, DNT_NV_EFS_REG_TYPE_INTEGRAL},
  /* DNT_NV_EFS_ERR_MPROC_TIMEOUT_VALUE */
  {"err_fatal_mproc_timeout_value", sizeof(uint8), 0, DNT_NV_EFS_REG_TYPE_INTEGRAL},  
  /* DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE */
  {"err_hw_reset_detect_enable", sizeof(uint8), 0, DNT_NV_EFS_REG_TYPE_INTEGRAL}

  /* add entries below this line and make sure you update the CGPS_NV_EFS_SM_NUM_ITEMS*/
};




/*==============================================================================
  FUNCTION
    err_nv_efs_reg_get_default

  DESCRIPTION
    Retrieve the specified item and store it in 'dest'. Note that the size of
    'dest' is implicit via the item type. NB: This function may block if the
    data source resides in another task. Returns True if successful, False on
    error. This function should be expanded to include all default values.
 
  RETURN VALUE
    TRUE if no ERROR
    FALSE if error

  
==============================================================================*/
boolean err_nv_efs_reg_get_default(dnt_nv_efs_reg_item_type const item, uint8 data_size,
                                  void *data_ptr) 
{
  boolean b_ret = TRUE;

  MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS_REG: Registry Get Default"); 

  switch (item)
  {
    case DNT_NV_EFS_ERR_FATAL_ACTION: 
      if (data_size == sizeof(uint8))
      {
        *(uint8 *)data_ptr = DNT_NV_EFS_ERR_FATAL_ACTION_DEFAULT;
      }
      break;

    case DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE: 
      if (data_size == sizeof(uint8))
      {
        *(uint8 *)data_ptr = DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE_DEFAULT; 
      }
      break;

    default:
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR,"ERR_NV_EFS_REG: Invalid item id: %d", item);
      b_ret = FALSE;
  }
  return b_ret;
}

/*===========================================================================

FUNCTION err_nv_efs_init

DESCRIPTION
  This function is used to initialize internal ERR-NV data efs structure.
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
void err_nv_efs_init (void)
{
  boolean ret_val = TRUE;
  dnt_nv_efs_reg_item_type table_start = DNT_NV_EFS_ERR_MIN_ITEM;
  dnt_nv_efs_reg_item_type table_end = DNT_NV_EFS_ERR_MAX_ITEM;
  dnt_nv_efs_reg_item_type table_entry = table_start;
  uint8 item_data = 0;

  ret_val =  dnt_nv_efs_conf_init (DNT_NV_EFS_ERR, DNT_NV_EFS_ERR_MIN_ITEM,
                               DNT_NV_EFS_ERR_MAX_ITEM,
                               err_nv_reg_items, "err");
  if (FALSE == ret_val)
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS init failed");
    return ;
  }
  MSG(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS init successful");
  
  while(table_entry <= table_end)
  {
    if( table_entry == DNT_NV_EFS_ERR_FATAL_ACTION)
      item_data = DNT_NV_EFS_ERR_FATAL_ACTION_DEFAULT ;
    else if( table_entry == DNT_NV_EFS_ERR_MPROC_TIMEOUT_VALUE)
      item_data = DNT_NV_EFS_ERR_MPROC_TIMEOUT_VALUE_DEFAULT ;
    else if( table_entry == DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE)
      item_data = DNT_NV_EFS_ERR_HW_RESET_DETECT_ENABLE_DEFAULT;

    item_data= item_data + '0'; //converting item_data to its equivalent char's ASCII
    ret_val =  dnt_nv_efs_item_init (table_entry, err_nv_reg_items, "err", item_data);
    if (FALSE == ret_val)
          MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS item init failed for %d",table_entry); //will not return and give chance to next files

    ++table_entry;
  }
  return ;
}


/*===========================================================================

FUNCTION err_nv_efs_reg_item_read

DESCRIPTION
  This function is used to read NV item. If the read fails, it sets it to
  default value .
  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean err_nv_efs_reg_item_read(dnt_nv_efs_reg_item_type const item, 
                                 uint32 data_size,
                                 void *data_ptr 
                                )
{
  dnt_nv_efs_reg_rw_stat_enum_type status = DNT_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = TRUE;

  status =  dnt_nv_efs_reg_item_read(item, data_size, data_ptr);

  if (status == DNT_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS file read succeeded for item = %d", item);
  }
  else
  {
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS file read failed for item = %d, setting defaults", item);
    return_value = err_nv_efs_reg_get_default(item, data_size, data_ptr);
  }
  return return_value;
}

#if 0 //not yet needed or supported
/*===========================================================================

FUNCTION err_nv_efs_reg_item_write

DESCRIPTION
  This function is used to write an NV item. 

  
RETURN VALUE
  None
  
DEPENDENCIES
 
===========================================================================*/
boolean err_nv_efs_reg_item_write(dnt_nv_efs_reg_item_type const item, 
                                  uint8 data_size,
                                  void *data_ptr
                                 )
{

  dnt_nv_efs_reg_rw_stat_enum_type status = DNT_NV_EFS_REG_RW_STAT_INVALID;    
  boolean return_value = TRUE;

  status =  dnt_nv_efs_reg_item_write(item, data_size, data_ptr, 
                                       DNT_NV_EFS_ERR_MIN_ITEM,
                                       DNT_NV_EFS_ERR_MAX_ITEM, err_nv_reg_items);

  if (status == DNT_NV_EFS_REG_RW_STAT_OK )
  {
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS file write succeeded for item = %d", item);
  }
  else
  {
    MSG_1(MSG_SSID_TMS, MSG_LEGACY_MED,"ERR_NV_EFS: NV EFS file write failed for item = %d", item);
  }
  return return_value;
}
#endif 
