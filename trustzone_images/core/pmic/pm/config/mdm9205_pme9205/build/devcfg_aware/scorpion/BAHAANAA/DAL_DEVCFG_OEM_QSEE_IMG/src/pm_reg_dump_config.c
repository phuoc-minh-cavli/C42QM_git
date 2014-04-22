/*! \file
*  \n
*  \brief  pm_reg_dump_config.c
*  \n
*  \n This file contains pmic configuration data specific for PMIC debug APIs.
*  \n
*  \n &copy; Copyright 2014-2018 Qualcomm Technologies Incorporated, All Rights Reserved
*/
/* =======================================================================
Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.1.3/pmic/pm/config/mdm9205_pme9205/src/pm_reg_dump_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/06/17   akt     Updated for Warlock
02/02/17   akm     Moved PMIC,SPMI out to Systemdrivers
06/16/16   aab     Added support to enable desired PMIC Register dump
01/26/16   vtw     Updated for Nazgul.
========================================================================== */

/*=========================================================================
      Include Files
==========================================================================*/

#include "pm_internal.h"

/*===========================================================================
Data Declarations
===========================================================================*/

/*
 * Array PMIC peripherals supply DDR rails.
 */
const pm_reg_dump_list_type pm_reg_dump_list[] =
{
  /* SMPS */
   {0x1, 0x1400, PM_PERIPH_TYPE_SMPS},    /* S1A_CTRL */
   {0x1, 0x1600, PM_PERIPH_TYPE_SMPS},    /* S1A_FREQ */

   {0x1, 0x1700, PM_PERIPH_TYPE_SMPS},    /* S2A_CTRL */
   {0x1, 0x1900, PM_PERIPH_TYPE_SMPS},    /* S2A_FREQ */

  /* LDO */
   {0x1, 0x4000, PM_PERIPH_TYPE_SMPS},    /* LDO1A */
   {0x1, 0x4100, PM_PERIPH_TYPE_SMPS},    /* LDO2A */
   {0x1, 0x4200, PM_PERIPH_TYPE_SMPS},    /* LDO3A */
   {0x1, 0x4300, PM_PERIPH_TYPE_SMPS},    /* LDO4A */
   {0x1, 0x4400, PM_PERIPH_TYPE_SMPS},    /* LDO5A */
   {0x1, 0x4500, PM_PERIPH_TYPE_SMPS},    /* LDO6A */
   {0x1, 0x4600, PM_PERIPH_TYPE_SMPS},    /* LDO7A */
     
   /* PON: All Peripheral dump for PON */
   {0x0, 0x0800, PM_PERIPH_TYPE_FULL_PERI_REG},  /* PON */

   {0xf, 0xffff, PM_PERIPH_TYPE_INVALID}      /* End of list */

};

