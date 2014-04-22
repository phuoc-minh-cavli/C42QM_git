#ifndef _HSU_BOOT_DLOAD_INT_H_
#define _HSU_BOOT_DLOAD_INT_H_

/*==============================================================================

     High Speed USB Boot Dload
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This file includes functions that are related to Software Download feature.

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
    Functions should only be called from the TMC's context.     
    
  Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_boot_dload_int.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  04/15/07 me  Initial version
    
==============================================================================*/
#include "hsu_conf_sel_types_int.h"
#include "hsu_boot_dload.h"
#include "comdef.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
/* Version number of the IRAM/shared segment/IMEM data */
#ifdef FEATURE_GOBI
  /* Added new variable "vendor_id" for Gobi */
  #define HSU_BOOT_DLOAD_VERSION_NUM 4
#else 
  #define HSU_BOOT_DLOAD_VERSION_NUM 3
#endif /* FEATURE_GOBI */

/* Macro DLOAD_QPST_COOKIE_SIZE is defined in the builds file*/
#ifndef DLOAD_QPST_COOKIE_SIZE
  /* Maximum size of QPST Cookie Data */
  #define DLOAD_QPST_COOKIE_SIZE    8
#endif /* DLOAD_QPST_COOKIE_SIZE */

/* Structure to define if Product ID/Serial Number is set before entering download mode. */ 
typedef struct
{
  uint32 magic_1;
  uint32 magic_2;
} pid_serial_num_cookie_type;

/* Structure to define QPST cookie to identify phone in download mode. */ 
typedef struct
{
  uint8 qpst_cookie_length;
  byte qpst_cookie_data[DLOAD_QPST_COOKIE_SIZE];
} dload_qpst_cookie_type;

/* Structure for passing information required for the SW download between AMSS, 
** DLOAD and ARMPRG  
** Use dload_info_free array within structure for all the futue additions to structure.
** This structure consumes all 168 bytes allocated for the USB IRAM parition 
*/
typedef struct
{
  uint32                         hsu_product_id;
  /* The Serial Number used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD and ARMPRG), so that the phone
  ** does not switch com ports.
  */
  char                           serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN];

  pid_serial_num_cookie_type     pid_serial_num_cookie;

#ifdef FEATURE_GOBI
  /* The vendor ID used by HS-USB before going into download mode.
  ** It is used in download mode (DLOAD (set) and ARMPRG (use)), so that the phone
  ** does not switch com ports.
  */
  uint16                         hsu_vendor_id;
#endif /* FEATURE_GOBI */
}
hsu_dload_info_type;


/* Magic numbers for notifying OEMSBL to boot if Product ID/Serial Number is set into download mode. */
#define PID_MAGIC_NUMBER           0x71432909
#define SERIAL_NUMBER_MAGIC_NUMBER 0x61945374


/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_QPST_COOKIE

DESCRIPTION
Provide the QPST Cookie Length and QPST Cookie Data to boot

DEPENDENCIES
None.

RETURN VALUE
TRUE if buffer pointer is valid, QPST cookie is returned
FALSE if buffer pointer is not valid/unallocated
FALSE if the feature FEATURE_QPST_COOKIE is not defined in builds file

SIDE EFFECTS
None
==============================================================================*/
boolean hsu_boot_dload_get_qpst_cookie (dload_qpst_cookie_type *dload_qpst_cookie);

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_MARK_DLOAD_INFO_INVALID

DESCRIPTION
Mark that the DLOAD related data is invalid

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS

==============================================================================*/
void hsu_boot_dload_mark_dload_info_invalid(hsu_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_IS_DLOAD_INFO_PID_VALID

DESCRIPTION
Checks if the DLOAD related PID is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the PID is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean hsu_boot_dload_is_dload_info_pid_valid(const hsu_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_IS_DLOAD_INFO_SERIAL_NUM_VALID

DESCRIPTION
Checks if the DLOAD related Serial Number is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the Serial Number is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean hsu_boot_dload_is_dload_info_serial_num_valid(const hsu_dload_info_type* dload_info_ptr);

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in AMSS, before going into 
download mode, and read in download mode.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data

SIDE EFFECTS

==============================================================================*/
hsu_dload_info_type* hsu_boot_dload_get_hsu_dload_info_ptr(void);


#endif /* _HSU_BOOT_DLOAD_INT_H_ */
