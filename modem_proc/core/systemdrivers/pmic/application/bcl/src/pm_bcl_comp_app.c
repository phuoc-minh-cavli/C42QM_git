/*! \file pm_bcl_comp_app.c
*  \n
*  \brief BCL Comparator Application Layer.
*  \n
*  \n &copy; Copyright 2019 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/* =======================================================================
                            Edit History
This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/application/bcl/src/pm_bcl_comp_app.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/15   RL      File created
========================================================================== */
/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "pmapp_bcl.h"
#include "pm_ulog.h"
#include "pm_bcl_comp_app.h"
#include "pm_bcl_comp_driver.h"


/*===========================================================================

                     DEFINITIONS

===========================================================================*/
struct pm_bcl_comp_app_status
{
  uint32                low1_voltage_thr;
  uint32                hi1_voltage_thr;
  boolean               low1_irq_en;
  boolean               hi1_irq_en;
  pmapp_bcl_client_cb   low1_thr_cb;
  pmapp_bcl_client_cb   hi1_thr_cb;
  pm_err_flag_type      err_flag;
  boolean               is_supported;
  uint8                 pmic_chip;
}bcl_status;

extern boolean bPMICStub;

/*===========================================================================

                     FUNCTION PROTOTYPES

===========================================================================*/

static pm_err_flag_type pmapp_bcl_comp_thr_config(uint32 voltage,
                                                  pmapp_bcl_thr_type thr,
                                                  pmapp_bcl_client_cb cb_ptr,
                                                  boolean irq_en);

void* pmapp_bcl_comp_interrupt_cb(void *pArg, uint32 uIntrStatusMask);


static uint32 pmapp_bcl_get_thr(pmapp_bcl_thr_type thr_type);

/*===========================================================================

                     FUNCTION DEFINITIONS

===========================================================================*/

pm_err_flag_type
pmapp_bcl_set_thr_voltage(uint32 voltage,
                          pmapp_bcl_thr_type thr_type,
                          pmapp_bcl_client_cb cb_ptr,
                          boolean irq_en)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if(bPMICStub == TRUE)
  {
    err_flag = PM_ERR_FLAG__SUCCESS;
  }
  else if(bcl_status.is_supported==FALSE)
  {
    err_flag = PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
  }
  else
  {
    err_flag = pmapp_bcl_comp_thr_config(voltage, thr_type,
                                         cb_ptr, irq_en);
  }

  PM_LOG_MSG_INFO("Voltage=%d, ThresholdType=%d, IrqEnable=%d, CB_Addr=0x%x",
                   voltage, thr_type, irq_en, cb_ptr);

  bcl_status.err_flag = err_flag;

  PM_LOG_MSG_ERROR(err_flag, "error = %d", err_flag);

  return err_flag;
}

void pmapp_bcl_comp_init(void)
{
  bcl_status.pmic_chip = 0; // To do

  pm_bcl_comp_data_type *bcl_comp_ptr =
                                  pm_bcl_comp_get_data(bcl_status.pmic_chip);

  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

  if (bcl_comp_ptr != NULL)
  {
    err_flag |= pm_bcl_comp_irq_enable(bcl_status.pmic_chip,
                                       PM_BCL_COMP_LOW1_THR, FALSE);

    err_flag |= pm_bcl_comp_irq_enable(bcl_status.pmic_chip,
                                       PM_BCL_COMP_LOW2_THR, FALSE);

    err_flag |= pm_bcl_comp_irq_clear(bcl_status.pmic_chip,
                                      PM_BCL_COMP_LOW1_THR);

    err_flag |= pm_bcl_comp_irq_clear(bcl_status.pmic_chip,
                                      PM_BCL_COMP_LOW2_THR);

    err_flag |= pm_bcl_comp_register_isr(bcl_status.pmic_chip,
                                         PM_BCL_COMP_LOW1_THR,
                                         &pmapp_bcl_comp_interrupt_cb);

    err_flag |= pm_bcl_comp_register_isr(bcl_status.pmic_chip,
                                         PM_BCL_COMP_LOW2_THR,
                                         &pmapp_bcl_comp_interrupt_cb);

    err_flag |= pm_bcl_comp_set_irq_polarity(bcl_status.pmic_chip,
                                             PM_BCL_COMP_LOW1_THR,
                                             PM_IRQ_TRIGGER_ACTIVE_HIGH);

    err_flag |= pm_bcl_comp_set_irq_polarity(bcl_status.pmic_chip,
                                             PM_BCL_COMP_LOW2_THR,
                                             PM_IRQ_TRIGGER_ACTIVE_LOW);

    err_flag |= pm_bcl_comp_enable_vcomp(bcl_status.pmic_chip);
  }
  else
  {
    err_flag |= PM_ERR_FLAG__FEATURE_NOT_SUPPORTED;
    PM_LOG_MSG_ERROR(err_flag,"Failed to initialize BCL APP");
  }

  bcl_status.is_supported = (err_flag == PM_ERR_FLAG__SUCCESS)? TRUE : FALSE;

  return;
}

static pm_err_flag_type
pmapp_bcl_comp_thr_config(uint32 voltage,
                          pmapp_bcl_thr_type thr,
                          pmapp_bcl_client_cb cb_ptr,
                          boolean irq_en)
{
  pm_err_flag_type err_flag=PM_ERR_FLAG__SUCCESS;
  pm_on_off_type on_off=PM_OFF;
  pm_bcl_comp_data_type *bcl_comp_ptr =
                                  pm_bcl_comp_get_data(bcl_status.pmic_chip);

  pm_bcl_comp_thr_type thr_type=(pm_bcl_comp_thr_type)pmapp_bcl_get_thr(thr);

  if((bcl_comp_ptr == NULL) ||
     (thr_type>=PM_BCL_COMP_THR_INVALID) ||
     (cb_ptr == NULL) ||
     (voltage < bcl_comp_ptr->limits->min_mv) ||
     (voltage > bcl_comp_ptr->limits->max_mv))
  {
      err_flag |= PM_ERR_FLAG__INVALID_PARAMETER;
      PM_LOG_MSG_ERROR(err_flag, "error = %d", err_flag);
      return err_flag;
  }

  //Disable IRQ in-case its enabled to avoid triggering after
  //IRQ clear
  err_flag |= pm_bcl_comp_irq_enable(bcl_status.pmic_chip, thr_type, FALSE);

  err_flag |= pm_bcl_comp_enable_status(bcl_status.pmic_chip, &on_off);

  if(on_off==PM_OFF)
  {
      err_flag |= pm_bcl_comp_enable(bcl_status.pmic_chip, PM_ON);
  }

  err_flag |= pm_bcl_comp_set_thr_voltage(bcl_status.pmic_chip,
                                          thr_type, voltage);

  err_flag |= pm_bcl_comp_irq_clear(bcl_status.pmic_chip, thr_type);

  switch(thr_type)
  {
    case PM_BCL_COMP_LOW1_THR:
      bcl_status.low1_voltage_thr = voltage;
      bcl_status.low1_thr_cb = cb_ptr;
      bcl_status.low1_irq_en = irq_en;
      break;
    case PM_BCL_COMP_LOW2_THR:
      bcl_status.hi1_voltage_thr = voltage;
      bcl_status.hi1_thr_cb = cb_ptr;
      bcl_status.hi1_irq_en = irq_en;
      break;
    default:
      break;
  }

  err_flag |= pm_bcl_comp_irq_enable(bcl_status.pmic_chip, thr_type, irq_en);

  PM_LOG_MSG_ERROR(err_flag, "error = %d", err_flag);

  return err_flag;
}

void* pmapp_bcl_comp_interrupt_cb(void *pArg, uint32 uIntrStatusMask)
{
  pm_bcl_comp_data_type *bcl_comp_ptr =
                                pm_bcl_comp_get_data(bcl_status.pmic_chip);

  if(bcl_comp_ptr == NULL)
  {
    PM_LOG_MSG_ERR_FATAL("Unable to get bcl driver data");
  }

  PM_LOG_MSG_INFO("int received: mask = %d, hi_irq_en = %d, low_irq_en = %d",
              uIntrStatusMask, bcl_status.hi1_irq_en, bcl_status.low1_irq_en);

  if(bcl_comp_ptr->version_update)
  {
    switch(uIntrStatusMask)
    {
      case 0x02:
        pm_bcl_comp_irq_enable(bcl_status.pmic_chip,
                              PM_BCL_COMP_LOW1_THR,
                              FALSE);
        if(bcl_status.low1_thr_cb != NULL)
        {
          bcl_status.low1_thr_cb(PMAPP_BCL_LOW1_THR, NULL, NULL, NULL);
        }
        break;

      case 0x04:
        //On BCL_COMP peripheral, since HI threshold is not
        //available, low2 is used to simulate hi threshold
        //by changing interrupt polarity
        pm_bcl_comp_irq_enable(bcl_status.pmic_chip,
                              PM_BCL_COMP_LOW2_THR,
                              FALSE);
        if(bcl_status.hi1_thr_cb != NULL)
        {
          bcl_status.hi1_thr_cb(PMAPP_BCL_HI1_THR, NULL, NULL, NULL);
        }
        break;

        default:
        PM_LOG_MSG_ERR_FATAL("Unknown Irq mask received %x", uIntrStatusMask);
        break;
    }
  }
  else
  {
    switch(uIntrStatusMask)
    {
      case 0x01:
        pm_bcl_comp_irq_enable(bcl_status.pmic_chip,
                              PM_BCL_COMP_LOW1_THR,
                              FALSE);
        if(bcl_status.low1_thr_cb != NULL)
        {
          bcl_status.low1_thr_cb(PMAPP_BCL_LOW1_THR, NULL, NULL, NULL);
        }
        break;

      case 0x02:
        //On BCL_COMP peripheral, since HI threshold is not
        //available, low2 is used to simulate hi threshold
        //by changing interrupt polarity
        pm_bcl_comp_irq_enable(bcl_status.pmic_chip,
                              PM_BCL_COMP_LOW2_THR,
                              FALSE);

        if(bcl_status.hi1_thr_cb != NULL)
        {
          bcl_status.hi1_thr_cb(PMAPP_BCL_HI1_THR, NULL, NULL, NULL);
        }
        break;

      default:
        PM_LOG_MSG_ERR_FATAL("Unknown Irq mask received %x", uIntrStatusMask);
        break;
    }
  }

  return NULL;
}

static uint32 pmapp_bcl_get_thr(pmapp_bcl_thr_type thr_type)
{
  uint8 periph_thr=0;
  switch(thr_type)
  {
    case PMAPP_BCL_LOW1_THR:
      periph_thr = PM_BCL_COMP_LOW1_THR;
      break;

    case PMAPP_BCL_HI1_THR:
      //On BCL_COMP peripheral, since HI threshold is not
      //available, low2 is used to simulate hi threshold
      //by changing interrupt polarity
      periph_thr = PM_BCL_COMP_LOW2_THR;
      break;

    default:
      periph_thr = PM_BCL_COMP_THR_INVALID;
      break;
  }
  return periph_thr;
}
