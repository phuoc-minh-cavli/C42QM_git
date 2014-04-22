#ifndef __PM_PBS_CLIENT_H__
#define __PM_PBS_CLIENT_H__

/** @file pm_pbs_client.h
 *  \n
 *  \brief This header file contains API and type definitions for PBS Client driver.
 *  \n
 *  \n &copy; Copyright 2013-2015 QUALCOMM Technologies
 *     Incorporated, All Rights Reserved
*/
/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/systemdrivers/pmic/pm_pbs_client.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/16/13   kt      Created.
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_irq.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/

/** @addtogroup pm_pbs_client
@{ */

/** 
  PBS client types.
 */
typedef enum 
{
  PM_PBS_CLIENT_0  = 0,   /**< PBS client 0. */
  PM_PBS_CLIENT_1  = 1,   /**< PBS client 1. */
  PM_PBS_CLIENT_2  = 2,   /**< PBS client 2. */
  PM_PBS_CLIENT_3  = 3,   /**< PBS client 3. */
  PM_PBS_CLIENT_4  = 4,   /**< PBS client 4. */
  PM_PBS_CLIENT_5  = 5,   /**< PBS client 5. */
  PM_PBS_CLIENT_6  = 6,   /**< PBS client 6. */
  PM_PBS_CLIENT_7  = 7,   /**< PBS client 7. */
  PM_PBS_CLIENT_8  = 8,   /**< PBS client 8. */
  PM_PBS_CLIENT_9  = 9,   /**< PBS client 9. */
  PM_PBS_CLIENT_10 = 10,  /**< PBS client 10. */
  PM_PBS_CLIENT_11 = 11,  /**< PBS client 11. */
  PM_PBS_CLIENT_12 = 12,  /**< PBS client 12. */
  PM_PBS_CLIENT_13 = 13,  /**< PBS client 13. */
  PM_PBS_CLIENT_14 = 14,  /**< PBS client 14. */
  PM_PBS_CLIENT_15 = 15,  /**< PBS client 15. */
  PM_PBS_CLIENT_INVALID
}pm_pbs_client_type;


/**
  @enum pm_pbs_client_trigger
  @brief sw/hw triggers in each PBS client
 */
typedef enum
{
  PM_PBS_CLIENT_HW_TRIGGER_0 = 0,
  PM_PBS_CLIENT_HW_TRIGGER_1 = 1,
  PM_PBS_CLIENT_HW_TRIGGER_2 = 2,
  PM_PBS_CLIENT_HW_TRIGGER_3 = 3,
  PM_PBS_CLIENT_HW_TRIGGER_4 = 4,
  PM_PBS_CLIENT_HW_TRIGGER_5 = 5,
  PM_PBS_CLIENT_HW_TRIGGER_6 = 6,
  PM_PBS_CLIENT_SW_TRIGGER,
  PM_PBS_CLIENT_TRIGGER_INVALID
}pm_pbs_client_trigger;

/** 
  Types of IRQ bit fields for a PBS client. 
 */
typedef enum 
{
  PM_PBS_CLIENT_IRQ_SEQ_ERROR = 0,  /**< IRQ sequence error. */
  PM_PBS_CLIENT_IRQ_SEQ_ENDED = 1,  /**< IRQ sequence ended. */
  PM_PBS_CLIENT_IRQ_INVALID
}pm_pbs_client_irq_type;

/** 
  Type definition for the PBS triggers.
 */
typedef enum 
{
  PM_PBS_CLIENT_TRIGGER_RISING_EDGE,   /**< Trigger on rising edge. */
  PM_PBS_CLIENT_TRIGGER_FALLING_EDGE,  /**< Trigger on falling edge. */
  PM_PBS_CLIENT_TRIGGER_DUAL_EDGE,     /**< Trigger on dual edge. */
  PM_PBS_CLIENT_TRIGGER_EDGE_INVALID
} pm_pbs_client_trigger_type;

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/* pm_pbs_client_enable */

/**
 *  Enables/disables all triggers corresponding to the specified PBS client.
 * 
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[in] enable TRUE -- Enable PBS client triggers. \n
 *                   FALSE -- Disable PBS client triggers.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_enable(uint8 pmic_chip, pm_pbs_client_type client, boolean enable);

/** pm_pbs_client_enable_status */

/**
 *  Returns the PBS client enable/disable status.
 * 
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[out] status TRUE -- PBS client triggers are enabled. \n
 *                    FALSE -- PBS client triggers are disabled.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_enable_status(uint8 pmic_chip, pm_pbs_client_type client, boolean *status);

/* pm_pbs_client_trigger_cfg */

/**
 *  Configures the PBS trigger type for the specified PBS client.
 *  
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[in] trigger PBS trigger type. See #pm_pbs_client_trigger_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_trigger_cfg(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_trigger_type trigger);

/* pm_pbs_client_irq_enable */

/***  
 *  Enables/disables the PBS client IRQ.
 *  
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[in] irq PBS client IRQ type. See #pm_pbs_client_irq_type.
 * @param[in] enable TRUE -- Enable the corresponding PBS client interrupt. \n
 *                   FALSE -- Disable the corresponding PBS client interrupt.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_irq_enable(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq, boolean enable);

/* pm_pbs_client_irq_clear */

/**
 *  Clears the PBS client IRQ.
 *  
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[in] irq PBS client IRQ type. See #pm_pbs_client_irq_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_irq_clear(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq);

/* pm_pbs_client_irq_set_trigger */

/**
 *  Configures the PBS client IRQ trigger type.
 * 
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[in] irq PBS client IRQ type. See #pm_pbs_client_irq_type.
 * @param[in] trigger IRQ trigger type. See #pm_irq_trigger_type.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_irq_set_trigger(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq, pm_irq_trigger_type trigger);

/* pm_pbs_client_irq_status */

/**
 *  Returns the PBS client IRQ status.
 * 
 * @param[in] pmic_chip Primary PMIC -- 0; Secondary PMIC -- 1. 
 * @param[in] client PBS client type. See #pm_pbs_client_type.
 * @param[in] irq PBS client IRQ type. See #pm_pbs_client_irq_type.
 * @param[in] type Type of IRQ status to read. See #pm_irq_status_type.
 * @param[out] status IRQ status.
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_irq_status(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_irq_type irq, pm_irq_status_type type, boolean *status);

/**
 * Triggers the supplied PBS sequence
 *        by writing to the TRIG_CTL register
 * 
 * @param[in] pmic_chip. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] trigger: PBS Client hardware trigger number or 
 *                     software trigger. Refer enum
 *                     pm_pbs_client_trigger type above.
 * @param[in] trigger_argument: Argument to be passed to PBS 
 *                             trigger sequence. Enter NULL, if
 *                             no argument
 * @param[out] trigger_return_value: value returned by PBS 
 *                                  trigger sequence. Enter
 *                                  NULL, if no return value is
 *                                  expected
 *
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_sw_trigger(uint8 pmic_chip, pm_pbs_client_type client, pm_pbs_client_trigger trigger, 
                         uint8 *trigger_argument, uint8 *trigger_return_value);
                         
/**
 * @name pm_pbs_client_hw_trigger_disable
 *  
 * @brief Force triggers the corresponding PBS client software 
 *        or hardware trigger sequence by writing to the
 *        TRIG_CTL register.
 * 
 * @param[in] pmic_chip. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] client: PBS Client type. Refer enum 
 *                    pm_pbs_client_type above.
 * @param[in] hw_trigger: PBS Client hardware trigger number or 
 *                     software trigger. Refer enum
 *                     pm_pbs_client_trigger type above.
 * @param[in] trigger_edge: PBS Client trigger edge type .
 *                     Refer enum
 *                     pm_pbs_client_trigger_edge_type type
 *                     above.
 * @param[in] enable: TRUE to enable and FALSE to disable
 * @return 
 *  SUCCESS or Error -- See #pm_err_flag_type.
 */
pm_err_flag_type 
pm_pbs_client_hw_trigger_enable(uint8 pmic_chip, pm_pbs_client_type pbs_client_index, pm_pbs_client_trigger hw_trigger, pm_pbs_client_trigger_type trigger_edge, boolean enable);
                         

/** @} */ /* end_addtogroup pm_pbs_client */

#endif /* __PM_PBS_CLIENT_H__ */
