#ifndef __PM_BCL_COMP_DRIVER_H__
#define __PM_BCL_COMP_DRIVER_H__

/*! \file
 *  \n
 *  \brief  pm_bcl_comp_driver.h
 *  \details  This file contains functions prototypes and variable/type/constant
 *  declarations for supporting GPIO pin services for the Qualcomm
 *  PMIC chip set.
 *  \n &copy; Copyright 2018 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/drivers/bcl/src/pm_bcl_comp_driver.h#1 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
01/25/16    rl      New File Created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "pm_target_information.h"
#include "pm_irq.h"
/*===========================================================================

                     TYPE DEFINITIONS

===========================================================================*/
typedef struct pm_bcl_comp_register_info_type
{
  pm_register_address_type    base_address;
  pm_register_address_type    status;
  pm_register_address_type    int_rt_sts;
  pm_register_address_type    int_set_type;
  pm_register_address_type    int_pol_high;
  pm_register_address_type    int_pol_low;
  pm_register_address_type    int_latched_clr;
  pm_register_address_type    int_en_set;
  pm_register_address_type    int_en_clr;
  pm_register_address_type    int_latched_sts;
  pm_register_address_type    mode_ctl;
  pm_register_address_type    mode_ctl1;
  pm_register_address_type    en_ctl;
  pm_register_address_type    vcomp_low2_thr;
  pm_register_address_type    vcomp_low1_thr;
}pm_bcl_comp_register_info_type;

typedef struct pm_bcl_comp_limits
{
  uint32 min_mv;
  uint32 max_mv;
  uint32 step_mv; 
}pm_bcl_comp_limits;

typedef struct pm_bcl_comp_data_type
{
  pm_comm_info_type               *comm_ptr;
  pm_bcl_comp_register_info_type  *bcl_comp_register;
  pm_bcl_comp_limits              *limits;
  boolean                         version_update;
}pm_bcl_comp_data_type;

/** Select voltage source.
*/
typedef enum
{
  PM_BCL_COMP_LOW1_THR=0,       /**< LOW1 Threshold. */
  PM_BCL_COMP_LOW2_THR=1,       /**< LOW2 Threshold. */
  PM_BCL_COMP_THR_INVALID
}pm_bcl_comp_thr_type;

/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/
void
pm_bcl_comp_driver_init(pm_comm_info_type *comm_ptr,
                        peripheral_info_type *peripheral_info,
                        uint8 pmic_index);


pm_bcl_comp_data_type*
pm_bcl_comp_get_data(uint8 pmic_index);

/*===========================================================================

                        API PROTOTYPE

===========================================================================*/
/**
*  Configures threshold voltage, based on threshold type provided.
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] thr_type  Threshold type to be configured.
*                        See #pm_bcl_comp_thr_type.
* @param[in] volt_mV   Voltage in mV.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*   Configure Low 1 voltage threshold on PMIC 0 at 2250 mV:
* @code
*  errFlag = pm_bcl_comp_set_thr_voltage(0,
*                                        PM_BCL_COMP_LOW1_THR,
*                                        2250); @endcode
*/
pm_err_flag_type
pm_bcl_comp_set_thr_voltage(uint8 pmic_index,
                            pm_bcl_comp_thr_type thr_type,
                            uint32 volt_mV);

/**
*  Enables/Disables interrupt for threshold level.
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] thr_type  Threshold type to be configured.
*                        See #pm_bcl_comp_thr_type.
* @param[in] enabled   Enable or disable interrupt
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*   Enable Low 1 threshold interrupt on PMIC 0:
* @code
*  errFlag = pm_bcl_comp_irq_enable(0,
*                                   PM_BCL_COMP_LOW1_THR,
*                                   TRUE); @endcode
*/
pm_err_flag_type
pm_bcl_comp_irq_enable(uint8 pmic_index,
                       pm_bcl_comp_thr_type thr_type,
                       boolean enabled);

/**
*  Clears latched interrupt for threshold level.
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] thr_type  Threshold type to be configured.
*                        See #pm_bcl_comp_thr_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*   Clear Low 1 threshold interrupt latch on PMIC 0:
* @code
*  errFlag = pm_bcl_comp_irq_clear(0,
*                                  PM_BCL_COMP_LOW1_THR); @endcode
*/
pm_err_flag_type
pm_bcl_comp_irq_clear(uint8 pmic_index,
                      pm_bcl_comp_thr_type thr_type);

/**
*  Gets enable status of BCL Comparator Peripheral.
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] *on_off  pointer to return on or off status.
*                        See #pm_on_off_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*   Check enable status of BCL Comparator Peripheral:
* @code
*  errFlag = pm_bcl_comp_enable_status(0,
*                                     &on_off ); @endcode
*/
pm_err_flag_type
pm_bcl_comp_enable_status(uint8 pmic_index,
                          pm_on_off_type *on_off);

/**
*  Enables/Disables BCL Comparator Peripheral.
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] on_off  Specify to turn on or off peripheral.
*                        See #pm_on_off_type.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*   Enable BCL Peripheral:
* @code
*  errFlag = pm_bcl_comp_enable(0,
*                               PM_ON); @endcode
*/
pm_err_flag_type
pm_bcl_comp_enable(uint8 pmic_index,
                   pm_on_off_type on_off);

/**
*  Registers ISR for threshold level.
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] thr_type  Threshold type to be configured.
*                        See #pm_bcl_comp_thr_type.
* @param[in] isr_ptr   Pointer for call-back function
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
*  <b>Example </b> \n
*   Register function: void* cb_fn(void *pArg, uint32 mask) for LOW1 Thr
* @code
*  errFlag = pm_bcl_comp_register_isr(0,
*                                     PM_BCL_COMP_LOW1_THR,
*                                     &cb_fn); @endcode
*/
pm_err_flag_type
pm_bcl_comp_register_isr(uint8 pmic_index,
                         pm_bcl_comp_thr_type thr_type,
                         void *isr_ptr);


/**
*  Enables VCOMP1 and VCOMP2
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
* @code
*  errFlag = pm_bcl_comp_enable_vcomp(0);
*/
pm_err_flag_type
pm_bcl_comp_enable_vcomp(uint8 pmic_index);

/**
*  Sets interrupt polarity for LOW1 and LOW2 Thresholds
*
* @param[in] pmic_index Each PMIC device in the systems is enumerated
*                        starting with 0.
* @param[in] thr_type  Threshold type to be configured.
*                        See #pm_bcl_comp_thr_type.
* @param[in] trigger   Polarity for triggering the IRQ
*
* @return
*  SUCCESS or Error -- See #pm_err_flag_type.
*
* @code
*  errFlag = pm_bcl_comp_register_isr(0, PM_BCL_COMP_LOW1_THR,
*                                     PM_IRQ_TRIGGER_ACTIVE_HIGH); @endcode
*/
pm_err_flag_type
pm_bcl_comp_set_irq_polarity(uint8 pmic_index,
                             pm_bcl_comp_thr_type thr_type,
                             pm_irq_trigger_type trigger);

#endif //end __PM_BCL_COMP_DRIVER_H__
