/*===========================================================================

                    BOOT EXTERN DEBUG DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external ddr drivers used for Debug/SDI
  image functionality

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2015 - 2016 by Qualcomm Technologies Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
    
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/08/16   kvs     add el check back to ddr self refresh apis
07/15/16   yps     Remove Sec lib
03/16/16   kpa     Updates to jump back to Qsee from EL1 to EL3 via smc call
02/12/15   kpa     Initial Creation.
=============================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "boot_extern_debug_interface.h"
#include "ddr_sdi_wrapper.h"
#include "boot_fastcall_tz.h"
#include "boot_sbl_shared.h"

/* =============================================================================
**  Function : boot_ddr_exit_self_refresh
** =============================================================================
*/
/**
*   @brief
*   Enter DDR Self Refresh.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void boot_sdi_ddr_exit_sr_handler(void)
{
 ddr_sdi_init();
			
 //boot_sdi_fastcall_tz_no_rsp(TZ_DEBUG_IMG_EXIT_CMD,
//						 TZ_DEBUG_IMG_EXIT_PARAM_ID,
						 //0,0,0,0);
  

  /* If in EL3 return normally */
}



/* =============================================================================
**  Function : boot_ddr_enter_self_refresh
** =============================================================================
*/
/**
*   @brief
*   Enter DDR Self Refresh.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void boot_sdi_ddr_enter_self_refresh(void)
{
  ddr_sdi_exit(); 

     //boot_sdi_fastcall_tz_no_rsp(TZ_DEBUG_IMG_EXIT_CMD,
//                             TZ_DEBUG_IMG_EXIT_PARAM_ID,
  //                           0,0,0,0);
     /* SMC call hands control back to QSEE. Will not return here */


  /* If in EL3 return normally */
}

