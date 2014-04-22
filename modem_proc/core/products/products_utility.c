 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    PRODUCTS UTILITY

GENERAL DESCRIPTION

  It has all utility functions related to products 

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
05/06/18   ahemanth     ADDED tiny_modem_flow function and ENUM 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "products_utility.h"
#include "com_dtypes.h"

/*===========================================================================

FUNCTION get_modem_boot_mode

DESCRIPTION
	This function can be used to check the flow and different

DEPENDENCIES
  static map for shared IMEM region

RETURN VALUE
  ENUM of Type FLowtype

===========================================================================*/

flowType get_modem_boot_mode(void)
{
  uint32 *ocimem_address = (uint32 *)SHARED_IMEM_PMIC_BASE;
  unsigned int bitcheck ;
  bitcheck = *ocimem_address >> MODEM_BOOT_TYPE_BIT;
  if ((bitcheck & 1) == 1)
  {
	bitcheck = *ocimem_address >> MODEM_BOOT_APPS_LOAD_CHECK_BIT;
	if ((bitcheck & 1) == 1)
	{
		return MODEM_BOOT_TINY_WITH_APSS;
	}
	else 
	{
		return MODEM_BOOT_TINY_NO_APSS;
	}			
  }
  else
  {
	  return MODEM_BOOT_FULL;
  } 
}