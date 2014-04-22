/*! \file pm_app_clk.c
 *  \n
 *  \brief Implementation file for CLOCK application level APIs. 
 *  \n  
 *  \n &copy; Copyright 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/pmic/app/clk/src/pm_app_clk.c#1 $
 
when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/23/15    kt       Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "pm_app_clk.h"
#include "pm_clk_xo.h"
#include "pm_clk_driver.h"
#include "pm_comm.h"

/*===========================================================================

                     API IMPLEMENTATION 

===========================================================================*/

pm_err_flag_type pm_app_clk_get_xo_warmup_time(uint32 *warmup_time_usec)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 num_sleep_clk_cycles = 0,xo_num_sleep_clk_cycles =0;
    pm_on_off_type on_off;

    if (warmup_time_usec == NULL)
    {
        return PM_ERR_FLAG__INVALID_POINTER;
    }

    /* Get the number of sleep (32 kHz) clock cycles to wait based on primary PMIC LN_BB_CLK timer */
    err_flag = pm_clk_xo_get_warmup_time(0, PM_CLK_BB_1, &num_sleep_clk_cycles);

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
       /* Calculate the warm-up time in micro seconds: N is the number of sleep (32 KHz) clock cycles,
          we need to add at most 7 clock cycles of synchronization time + 1 padding.
        
          warmup_time (micro secs) = ((N + 7 +1) * 1000 * 1000)/32768
       */
       
       *warmup_time_usec = ((num_sleep_clk_cycles + 7 + 1) * 1000 * 1000)/32768;
    }
    //add XO warm up delay if xo turned off completley 
    err_flag = pm_clk_sw_enable_status(0, PM_CLK_XO, &on_off);
    if ((err_flag == PM_ERR_FLAG__SUCCESS) && (PM_OFF == on_off))
    {
       err_flag = pm_clk_xo_get_warmup_time(0, PM_CLK_XO, &xo_num_sleep_clk_cycles);
       
       *warmup_time_usec =  *warmup_time_usec + ((xo_num_sleep_clk_cycles * 1000 * 1000)/32768);
    }
    
    return err_flag;
}
