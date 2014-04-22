#ifndef PUTILITY_H
#define PUTILITY_H

 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    PRODUCTS UTILITY

GENERAL DESCRIPTION

  It has all utility function related to products 

FUNCTIONS
	get_modem_boot_mode



Copyright (c) 2002 - 2015,2019 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

when       who     		what, where, why
--------   ---     		----------------------------------------------------------
05/06/18   ahemanth     ADDED modem_flow_check function and ENUM 
===========================================================================*/



#ifndef SHARED_IMEM_PMIC_BASE
#define SHARED_IMEM_PMIC_BASE 0x08628FFC;
#endif

typedef enum flow {
	MODEM_BOOT_FULL = 0,
	MODEM_BOOT_TINY_WITH_APSS,
	MODEM_BOOT_TINY_NO_APSS,
}flowType;

typedef enum bitCheck {
	MODEM_BOOT_TYPE_BIT = 0,
	MODEM_BOOT_APPS_LOAD_CHECK_BIT = 4,
}bitCheckType;


/** =====================================================================
 * Function:
 *     get_modem_boot_mode
 * 
 * Description:
 *     This function can be used to check the flow and different  
 * 
 * Parameters:
 *     none
 * 
 * Returns:
 *     Enum type flowType
 * =====================================================================  */

flowType get_modem_boot_mode (void);

#endif