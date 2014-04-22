/*==============================================================================

     High Speed USB Boot Dload

  GENERAL DESCRIPTION
    This file includes functions that are related to Software Download feature.

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
    
  Copyright (c) 2007-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_boot_dload.c#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  04/08/10  dk      Added hsu_al_bulk for flashless boot  
  07/23/09  jc      Added BOOT_MIHEADER_H for bootloader
  04/15/07  me      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/

#include "customer.h"

#include "hsu_boot_dload_int.h"
#include "hsu_modem_proc_services.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_common_int.h"
#include "hsu_conf_sel_stack_utils.h"

#include "hsu_hwio_dependency.h"

#include "hsu_conf_sel_types_int.h"
#include <stringl/stringl.h>
#include "hsu_phy_utils_int.h" /* For spoof_disconnect */

#include "HAL_hsusb.h"
#include "hsu_platform.h"

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
/* A global variable used in ARMPRG to hold the IMEM SW download data.
Since all accesses to the IMEM memory should be of 32bits,and our structure includes 
also smaller fields we need to copy the memory to a static 
variable and use it to read the data
*/

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_AMSS_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in AMSS, before going into 
download mode.
In targets that don't have shared segment, we save the data in IRAM.
In targets that have shared segment, we save the data in the shared segment.

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data.

SIDE EFFECTS

==============================================================================*/
static hsu_dload_info_type* hsu_boot_dload_get_amss_hsu_dload_info_ptr(void)
{
  return (hsu_dload_info_type*)HS_USB_DLOAD_INFO_IMEM_ADDR;
}
/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_TRANSITION

DESCRIPTION
  Do all the operations needed when transitioning to DLOAD/ARMPRG 
  (like writing data to the IRAM)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

==============================================================================*/
void hsu_boot_dload_transition(void)
{
  hsu_dload_info_type* hsu_dload_info_ptr = hsu_boot_dload_get_hsu_dload_info_ptr();

  /* Spoof disconnect was removed from here but added to hsu_al_dload_init_ex() as 
   * CLKRGM_USB_HS1_PBDG_CLK may be disabled when the USB cable is disconnected as
   * a transition to DLOAD mode occurs.
  */

  if (hsu_dload_info_ptr == NULL)
  {
    HSU_ERR_FATAL("hsu_dload_info_ptr is NULL", 0, 0, 0);
    return;
  }

 /* Update the product id and product id magic number according to the ARM9 static variable */
  hsu_dload_info_ptr->hsu_product_id = hsu_modem_proc_services_get_product_id();
  hsu_dload_info_ptr->pid_serial_num_cookie.magic_1 = hsu_modem_proc_services_get_magic_product_id();

  /* save the Serial Number and serial number magic number, to be used in DLOAD mode */
  if (hsu_modem_proc_services_is_serial_number_null() == FALSE)
  {
    memset(hsu_dload_info_ptr->serial_number, 0, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    (void)strlcpy(hsu_dload_info_ptr->serial_number, hsu_modem_proc_services_get_serial_number(), HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    hsu_dload_info_ptr->pid_serial_num_cookie.magic_2= hsu_modem_proc_services_get_magic_serial_number();
  }


#ifdef FEATURE_GOBI
  /* Set Vendor ID */
  hsu_dload_info_ptr->hsu_vendor_id = hsu_modem_proc_services_get_vendor_id();
#endif /* FEATURE_GOBI */

} /* hsu_boot_dload_transition */


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
void hsu_boot_dload_mark_dload_info_invalid(hsu_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We mark that the data is invalid by setting an invalid product_id */
    dload_info_ptr->hsu_product_id = HSU_PRODUCT_ID_INVALID;
  }
}

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_IS_DLOAD_INFO_PID_VALID

DESCRIPTION
Checks if the DLOAD related PID data is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the data is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean hsu_boot_dload_is_dload_info_pid_valid(const hsu_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We verify that the data is valid according to the product_id validity */
    if (dload_info_ptr->pid_serial_num_cookie.magic_1 == PID_MAGIC_NUMBER)
    {
      return TRUE;
    }
    else
    { 
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_IS_DLOAD_INFO_SERIAL_NUM_VALID

DESCRIPTION
Checks if the DLOAD related Serial Number data is valid

DEPENDENCIES
None.

RETURN VALUE
TRUE if the data is valid, FALSE if not

SIDE EFFECTS

==============================================================================*/
boolean hsu_boot_dload_is_dload_info_serial_num_valid(const hsu_dload_info_type* dload_info_ptr)
{
  if (dload_info_ptr != NULL)
  {
    /* We verify that the data is valid according to the product_id validity */
    if (dload_info_ptr->pid_serial_num_cookie.magic_2 == SERIAL_NUMBER_MAGIC_NUMBER)
    {
      return TRUE;
    }
    else
    { 
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

/*==============================================================================

FUNCTION          HSU_BOOT_DLOAD_GET_HSU_DLOAD_INFO_PTR

DESCRIPTION
Get a pointer to the structure that holds information needed for enumeration
in download mode. This information is written in AMSS, before going into 
download mode, and read in download mode, both by DLOAD and ARMPRG.
In targets that have IRAM, we save the data in IRAM while in AMSS and read it 
from there in DLOAD and ARMPRG.
In targets that don't have IRAM, we save the data in the shared segment, 
read it from the shared segment in DLOAD and save it in IMEM for the ARMPRG
usage (since shared segment is not integrated to ARMPRG).

DEPENDENCIES
None.

RETURN VALUE
Return the address of the memory to hold the SW download data.

SIDE EFFECTS

==============================================================================*/
hsu_dload_info_type* hsu_boot_dload_get_hsu_dload_info_ptr(void)
{
  return hsu_boot_dload_get_amss_hsu_dload_info_ptr();
}
