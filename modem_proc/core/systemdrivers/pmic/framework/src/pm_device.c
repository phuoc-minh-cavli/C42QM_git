/*! \file pm_device.c
*  
*  \brief Implementation file for PM DEVICE INIT level APIs.
*    
*  &copy; Copyright 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/
/*===================================================================
EDIT HISTORY FOR MODULE
 This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Date: 2019/03/22 $ 
$Change: 18665963 $
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/25/19   abh     Added file for Tiny Modem image in Twizy
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_pon.h"
#include "pm_err_flags.h"
#include "device_info.h"
#include "pm_comm.h"

/*===========================================================================
                       Function  Prototype
===========================================================================*/

/*===========================================================================
                   API Implementation
===========================================================================*/

/* NOTE : To enable stub code in Pre-Silicon validation, RUMI/VIRTIO
 * .cmm script should set bPMICStub = TRUE .
 */
 boolean bPMICStub = FALSE;

static boolean pm_device_setup_done = FALSE;

pm_err_flag_type
pm_device_setup( void ) 
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    /* Return from this function if the pm device init has is already done. */
    if ( pm_device_setup_done == TRUE )
        return err_flag;

    /* Initialize SPMI channel */
    err_flag |= pm_comm_channel_init_internal();
    if (err_flag != PM_ERR_FLAG__SUCCESS) 
    {
        return err_flag;
    }

    /* Detect */
    err_flag |= pm_version_detect();
    if (err_flag != PM_ERR_FLAG__SUCCESS) 
    {
        return err_flag;
    }

    pm_device_setup_done = TRUE;

    return err_flag;

}


