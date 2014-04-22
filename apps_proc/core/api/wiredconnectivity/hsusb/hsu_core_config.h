#ifndef _HSU_CORE_CONFIG_H_
#define _HSU_CORE_CONFIG_H_
/*=============================================================================

     High Speed USB core configuration Module
       
=============================================================================*/
/**
@file hsu_core_config.h

Core definitions used by the various HS-USB files.

  Externalized functions:
  - hsu_core_config_get_core
  - hsu_core_config_clear_core
  - hsu_core_config_get_cores_table_size
  - hsu_core_config_get_num_of_cores
  - hsu_core_config_get_hs_main_core
  - hsu_core_config_get_uicc_core
  - hsu_core_config_get_boot_core
  - hsu_core_config_get_2nd_hs_legacy_core
  - hsu_core_config_get_fs_legacy_core
  - hsu_core_config_get_stack_init_mode
  - hsu_core_config_set_hs_main_core_conf_sel_info
  - hsu_core_config_set_boot_core_conf_sel_info
  - hsu_core_config_set_2nd_hs_core_conf_sel_info
  - hsu_core_config_set_fs_legacy_core_conf_sel_info
*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: 
  $DateTime: 
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/25/10  kl  (Tech Pubs) Edited/added Doxygen comments and markup.
  07/17/08  me  Initial version

=============================================================================*/

/*=============================================================================
Copyright (c) 2008-2010 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=============================================================================*/

#include "hsu_common.h"
#include "comdef.h"

/** @ingroup hsusb_qc_api
    @{
*/
/*=============================================================================
Forward Declarations
=============================================================================*/

/*=============================================================================
Constants
=============================================================================*/

/** USB core types.
*/
typedef enum
{
  //HSU_USB2_CI_CORE,       /**< -- High-speed CI core. */
  HSU_USB3_SNPS_CORE,     /**< -- USB3.0 core.*/
  HSU_NUM_OF_CORES_TYPES, /**< -- Number of core types. */
  HSU_UKNOWN_CORE = HSU_NUM_OF_CORES_TYPES,  
}hsu_core_type_enum;

/*=============================================================================
Macros
=============================================================================*/

/*=============================================================================
Typedefs
=============================================================================*/

/** Enum for the various initialization options for the USB stack
    (currently only device and OTG modes are supported).
*/
typedef enum
{   
  HSU_CORE_CONFIG_INIT_MODE_OTG,     /**< -- Initialize as OTG device. */
  HSU_CORE_CONFIG_INIT_MODE_DEVICE,  /**<--  Initialize as device only. */
  HSU_CORE_CONFIG_INIT_MODE_HOST,    /**< -- Initialize as host only. */
  HSU_CORE_CONFIG_INIT_MODE_LAST     /**< -- Number of initialization mode
                                             types. */
} hsu_core_config_init_mode_enum;

/** USB core configuration structure.
*/
typedef struct hsu_core_config_type
{
  hsu_core_config_init_mode_enum init_mode;
    /**< Initialization mode. */
  void* hsu_conf_sel_core_info;
    /**< Pointer to the configuration selector core information structure. */
  void* hsu_os_abstrct_core_info;
    /**< Pointer to the OS abstraction core structure. */
}hsu_core_config_type;

/*=============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/

/*=============================================================================
FUNCTION    hsu_core_config_get_core
=============================================================================*/
/**
Returns the core according to the core index.

@param core_idx [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
A pointer to the core in the cores table.
*/
hsu_core_config_type* hsu_core_config_get_core(uint8 core_idx);

/*=============================================================================
FUNCTION    hsu_core_config_clear_core
=============================================================================*/
/**

This function clears the core according to the core index

@param core_idx [IN] -- Core index.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void hsu_core_config_clear_core(uint8 core_idx);

/*=============================================================================
FUNCTION    hsu_core_config_get_cores_table_size
=============================================================================*/
/**
Returns the number of cores in the cores table (including the NULL cores).

@dependencies
None.

@sideeffects
None.

@return
The total number of cores in the cores table.
*/
uint8 hsu_core_config_get_cores_table_size(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_num_of_cores
=============================================================================*/
/**
Returns the number of active cores in the cores table (not including the
NULL cores).

@dependencies
None.

@sideeffects
None.

@return
The number of active cores in the cores table.
*/
uint8 hsu_core_config_get_num_of_cores(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_hs_main_core
=============================================================================*/
/**
Returns the HS-USB main core address.

@dependencies
None.

@sideeffects
None.

@return
The address of the HS-USB main core.
*/
hsu_core_config_type* hsu_core_config_get_hs_main_core(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_uicc_core
=============================================================================*/
/**
Returns the UICC core address.

@dependencies
None.

@sideeffects
None.

@return
The address of the UICC core.
*/
hsu_core_config_type* hsu_core_config_get_uicc_core(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_hsic_core
=============================================================================*/
/**
Returns the HSIC core address.

@dependencies
None.

@sideeffects
None.

@return
The address of the HSIC core.
*/

hsu_core_config_type* hsu_core_config_get_hsic_core(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_boot_core
=============================================================================*/
/**
Returns the boot core address (used in DLOAD/ARMPRG/fast enumeration/dead
battery charging).

@dependencies
None.

@sideeffects
None.

@return
The address of the boot core.
*/
hsu_core_config_type* hsu_core_config_get_boot_core(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_2nd_hs_legacy_core
=============================================================================*/
/**
Returns the secondary HS-USB core address.

@dependencies
None.

@sideeffects
None.

@return
The address of the secondary HS-USB core.
*/
hsu_core_config_type* hsu_core_config_get_2nd_hs_legacy_core(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_fs_legacy_core
=============================================================================*/
/**
Returns the UICC core address.

@dependencies
None.

@sideeffects
None.

@return
The address of the UICC core.
*/
hsu_core_config_type* hsu_core_config_get_fs_legacy_core(void);

/*=============================================================================
FUNCTION    hsu_core_config_get_stack_init_mode
=============================================================================*/
/**
Returns the mode in which the stack was configured at power up (device/OTG)
for each core.

@param core_idx [IN] -- Core index.

@dependencies
Should be called after hsu_conf_sel_stack_utils_setup_stack().

@sideeffects
None.

@return
Upon error, returns the following possible error codes:
- HSU_CORE_CONFIG_INIT_MODE_OTG    -- Initialize as OTG device.
- HSU_CORE_CONFIG_INIT_MODE_DEVICE -- Initialize as device only.
- HSU_CORE_CONFIG_INIT_MODE_HOST   -- Initialize as host only.
- HSU_CORE_CONFIG_INIT_MODE_LAST   -- Number of initialization mode types.
*/
hsu_core_config_init_mode_enum hsu_core_config_get_stack_init_mode(uint8 core_idx);

/*=============================================================================
FUNCTION    hsu_core_config_set_hs_main_core_conf_sel_info
=============================================================================*/
/**
Sets the pointer to the configuration selector information for the HS-USB
main core.

@param *conf_sel_info [IN] -- Pointer to the configuration selector information
                              structure.
 
@dependencies
Should be called before hsu_conf_sel_stack_utils_setup_stack().

@sideeffects
None.

@return
None.
*/
void hsu_core_config_set_hs_main_core_conf_sel_info(void* conf_sel_info);

/*=============================================================================
FUNCTION    hsu_core_config_set_boot_core_conf_sel_info
=============================================================================*/
/**
Set the pointer to the configuration selector information for the boot core.

@param *conf_sel_info [IN] -- Pointer to the configuration selector information
                              structure.

@dependencies
Should be called before hsu_conf_sel_stack_utils_setup_stack() was called. 

@return
None.

@sideeffects
None. 
*/
void hsu_core_config_set_boot_core_conf_sel_info(void* conf_sel_info);

/*=============================================================================
FUNCTION    hsu_core_config_set_2nd_hs_core_conf_sel_info
=============================================================================*/
/**
Sets the pointer to the configuration selector information for the secondary
HS-USB core.

@param *conf_sel_info [IN] -- Pointer to the configuration selector information
                              structure.

@dependencies
Should be called before hsu_conf_sel_stack_utils_setup_stack().

@sideeffects
None.

@return
None.
*/
void hsu_core_config_set_2nd_hs_core_conf_sel_info(void* conf_sel_info);

/*=============================================================================
FUNCTION    hsu_core_config_set_fs_legacy_core_conf_sel_info
=============================================================================*/
/**
Sets the pointer to the configuration selector information for the FS-USB
legacy core.

@param *conf_sel_info [IN] -- Pointer to the configuration selector information
                              structure.

@dependencies
Should be called before hsu_conf_sel_stack_utils_setup_stack().

@sideeffects
None.

@return
None.
*/
void hsu_core_config_set_fs_legacy_core_conf_sel_info(void* conf_sel_info);

/*=============================================================================
FUNCTION    hsu_core_config_set_usb3_core_conf_sel_info
=============================================================================*/
/**
Sets the pointer to the configuration selector information for the USB3.0
core.

@param *conf_sel_info -- Pointer to the configuration selector information
structure.

@dependencies
Should be called before hsu_conf_sel_stack_utils_setup_stack().

@sideeffects
None.

@return
None.
*/
void hsu_core_config_set_usb3_core_conf_sel_info(void* conf_sel_info);

/** @} */ /* end_group hsusb_qc_api */

#endif /* _HSU_CORE_CONFIG_H_ */
