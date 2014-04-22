/*===========================================================================

           HAL_S Y S _ M . C

DESCRIPTION

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor HAL file
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x45/apps/HAL_sys_m.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/15   rks     created

===========================================================================*/

#include "HALsysmHWIO.h"
#include "msmhwio.h"
#include "msmhwiobase.h"

/*
 * @brief HAL_sys_m_set_gpio
 *
 * HAL System monitor function to raise MDM2AP_ERRFATAL in fusion configs
 * 
*/
void HAL_sys_m_set_gpio(uint32 gpio_num)
{
    HWIO_TLMM_GPIO_IN_OUTn_OUTI(gpio_num,HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK);
}

