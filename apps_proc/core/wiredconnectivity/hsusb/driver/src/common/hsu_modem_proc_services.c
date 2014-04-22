
/*==============================================================================

High Speed USB Module APIs on Modem processor 
H E A D E R   F I L E

GENERAL DESCRIPTION
This module defines High Speed USB APIs that are implemented on the modem 
processor and used by other components on the Modem processor. 
This file compiles ONLY for the main ARM9 image.

EXTERNALIZED FUNCTIONS
hsu_modem_proc_services_set_product_id
hsu_modem_proc_services_get_product_id
hsu_modem_proc_services_set_serial_number
hsu_modem_proc_services_get_serial_number

INITALIZATION AND SEQUENCING REQUIREMENTS
TODO.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_modem_proc_services.c#1 $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  05/02/07  me  Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_modem_proc_services.h"
#include "comdef.h"
#include "hsu_conf_sel_types_int.h"
#include <stringl/stringl.h>
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_boot_dload_int.h"
#ifdef FEATURE_EXPORT_HSU_PROC_COMM_APIS
  #include "hsu_mdm_apis_pcil.h"
#endif /* FEATURE_EXPORT_HSU_PROC_COMM_APIS */

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef FEATURE_GOBI
/* Taken from usbcdc.h lines 152-153 */
#define HSU_QC_VENDOR_ID_CODE              0x05C6
#endif /* FEATURE_GOBI */
/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  Variables
==============================================================================*/
static uint16 hsu_product_id = HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM;
static char hsu_serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN] = HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER;
static uint32 pid_magic_1;
static uint32 serial_num_magic_2;
static boolean hsu_is_serial_number_null = TRUE;
#ifdef FEATURE_GOBI
static uint16 hsu_vendor_id = HSU_QC_VENDOR_ID_CODE;
#endif /* FEATURE_GOBI */

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_PRODUCT_ID

DESCRIPTION
This function updates the product id in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_product_id(unsigned short product_id)
{

    #ifdef FEATURE_HS_USB_DLOAD_IRAM_DIRECT_UPDATE
    hsu_dload_info_type* hsu_dload_info_ptr = hsu_boot_dload_get_hsu_dload_info_ptr();

    hsu_product_id = product_id;

    if (hsu_dload_info_ptr)
    {
        hsu_dload_info_ptr->pid_serial_num_cookie.magic_1 = PID_MAGIC_NUMBER;
        hsu_dload_info_ptr->hsu_product_id = hsu_product_id;
    }
    #else /* !FEATURE_HS_USB_DLOAD_IMEM */
    hsu_product_id = product_id;
    pid_magic_1 = PID_MAGIC_NUMBER;
    #endif /* FEATURE_HS_USB_DLOAD_IRAM_DIRECT_UPDATE  */    
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_PRODUCT_ID

DESCRIPTION
This function returns the product id stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
The product_id (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
unsigned short hsu_modem_proc_services_get_product_id(void)
{
  return hsu_product_id;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_MAGIC_PRODUCT_ID

DESCRIPTION
This function returns the product id magic cookie stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
The magic cookie for product_id

SIDE EFFECTS
None.
===========================================================================*/
uint32 hsu_modem_proc_services_get_magic_product_id(void)
{
  return pid_magic_1;
}


/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_SERIAL_NUMBER

DESCRIPTION
This function updates the serial number in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_serial_number(const char* serial_number)
{
    if (serial_number != NULL)
    {
       #ifdef FEATURE_HS_USB_DLOAD_IRAM_DIRECT_UPDATE
       hsu_dload_info_type* hsu_dload_info_ptr = hsu_boot_dload_get_hsu_dload_info_ptr();

       (void)strlcpy(hsu_serial_number, serial_number, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);

       if (hsu_dload_info_ptr)
       {
          hsu_dload_info_ptr->pid_serial_num_cookie.magic_2 = SERIAL_NUMBER_MAGIC_NUMBER;
          memset(hsu_dload_info_ptr->serial_number, 0, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
          (void)strlcpy(hsu_dload_info_ptr->serial_number, hsu_serial_number, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
       }
       #else /* !FEATURE_HS_USB_DLOAD_IMEM */
       (void)strlcpy(hsu_serial_number, serial_number, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
       serial_num_magic_2 = SERIAL_NUMBER_MAGIC_NUMBER;
       /* Set the hsu_is_serial_number_null to False since we have valid serial string passed from APPS */
       hsu_is_serial_number_null = FALSE;
       #endif /* FEATURE_HS_USB_DLOAD_IRAM_DIRECT_UPDATE */
    }

}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_SERIAL_NUMBER

DESCRIPTION
This function returns the serial number stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
The serial number (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
char* hsu_modem_proc_services_get_serial_number(void)
{
  return hsu_serial_number;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_MAGIC_SERIAL_NUMBER

DESCRIPTION
This function returns the magic cookie for serial number stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
The magic cookie for serial number (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/

uint32 hsu_modem_proc_services_get_magic_serial_number(void)
{
  return serial_num_magic_2;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_IS_SERIAL_NUMBER_NULL

DESCRIPTION
This function updates the is serial number null boolean stored in the ARM9 static variable

DEPENDENCIES
None.

RETURN VALUE
None

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_is_serial_number_null(boolean is_serial_number_null)
{
  hsu_is_serial_number_null = is_serial_number_null;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_IS_SERIAL_NUMBER_NULL

DESCRIPTION
This function returns whether the serial number stored in the ARM9 static variable is NULL

DEPENDENCIES
None.

RETURN VALUE
TRUE if serial number should be set to NULL (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_modem_proc_services_is_serial_number_null(void)
{
  return hsu_is_serial_number_null;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_INIT

DESCRIPTION
This function performs HSUSB initializations needed on ARM9 processor

DEPENDENCIES
This function should be called from tmc_init for Linux targets.

RETURN VALUE
TRUE for success.

SIDE EFFECTS
None.
===========================================================================*/
boolean hsu_modem_proc_services_init(void)
{
#ifdef FEATURE_EXPORT_HSU_PROC_COMM_APIS
  return hsu_register_pcil_cmds();
#else
  return TRUE;
#endif /* FEATURE_EXPORT_HSU_PROC_COMM_APIS */
}
#ifdef FEATURE_GOBI
/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_SET_VENDOR_ID

DESCRIPTION
This function updates the vendor ID stored in static variable

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_modem_proc_services_set_vendor_id(unsigned short vendor_id)
{
  hsu_vendor_id = vendor_id;
}

/*===========================================================================

FUNCTION      HSU_MODEM_PROC_SERVICES_GET_VENDOR_ID

DESCRIPTION
This function returns the vendor ID stored in static variable

DEPENDENCIES
None.

RETURN VALUE
The vrendor_id (used for the USB enumeration in DLOAD/ARMPROG).

SIDE EFFECTS
None.
===========================================================================*/
unsigned short hsu_modem_proc_services_get_vendor_id(void)
{
  return hsu_vendor_id;
}
#endif /* FEATURE_GOBI */


