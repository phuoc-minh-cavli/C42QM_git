#ifndef __PM_IRQ_H__
#define __PM_IRQ_H__

/** @file pm_irq.h
 *  PMIC IRQ related type definitions.*/
/*
 *   Copyright (c) 2009-2013 Qualcomm Technologies, Inc.
 *   All Rights Reserved.
 *   Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_irq.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/26/10   wra     Changed documentation from @example to Examples so Deoxygen can parse the file
11/06/09   APU     Made the file target independant 
10/20/09   jtn     Move init function prototype to pm_device_init_i.h
06/25/09   jtn     Added "virtual" IRQ handles for SCMM charger
06/17/09   jtn     Branched for SCMM
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "com_dtypes.h"
#include "pm_err_flags.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** @addtogroup pm_irq
@{ */

/* pm_irq_trigger_type */

/**
  Type definition for the various IRQ triggers.
 */
typedef enum
{
  PM_IRQ_TRIGGER_ACTIVE_LOW,    /**< IRQ trigger active low. */
  PM_IRQ_TRIGGER_ACTIVE_HIGH,   /**< IRQ trigger active high. */
  PM_IRQ_TRIGGER_RISING_EDGE,   /**< IRQ trigger rising edge. */
  PM_IRQ_TRIGGER_FALLING_EDGE,  /**< IRQ trigger falling edge. */
  PM_IRQ_TRIGGER_DUAL_EDGE,     /**< IRQ trigger dual edge. */
  PM_IRQ_TRIGGER_INVALID
} pm_irq_trigger_type;


/* pm_irq_status_type */

/**
 Type definition for the IRQ status.
 */
typedef enum
{
  PM_IRQ_STATUS_RT,       /**< IRQ status RT. */
  PM_IRQ_STATUS_LATCHED,  /**< IRQ status latched. */
  PM_IRQ_STATUS_PENDING,  /**< IRQ status pending. */
  PM_IRQ_STATUS_ENABLE,   /**< IRQ status enable */
  PM_IRQ_STATUS_INVALID
} pm_irq_status_type;

/** @} */ /* end_addtogroup pm_irq */

#endif /* __PM_IRQ_H__ */
