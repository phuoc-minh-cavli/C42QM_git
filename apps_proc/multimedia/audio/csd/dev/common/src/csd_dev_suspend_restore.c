/*==============================================================================
  @file csd_dev_suspend_restore.c

  The objective of this source file is to implement device suspend/restore
  functionality for CSD resource manager in order to power down/power up the
  devices when the conditions is met.

  Copyright (c) 2008-13 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/src/csd_dev_suspend_restore.c#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  02/24/13   suns    Dev module unification
  06/06/11   sgk     Dev Handle Release Fix
==============================================================================*/

#include "csd_dev_main.h"
#include "csd_dev_manager.h"
#include "csd_debug_msg.h"

extern struct csd_dev_record* csd_dev_get_active_record( const uint32_t dev_id );
extern int32_t csd_dev_appply_dev_config( csd_dev_record_t* a_dev );
extern void csd_dev_remove_active_record( csd_dev_record_t* dev );
extern int32_t csd_dev_cmd_enable_anc_control( uint32_t cmd_code,
                                               void *cmd_buf,
                                               uint32_t cmd_buf_len );

int32_t csd_dev_restore( uint32_t dev_id )
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* a_dev = NULL;

  CSD_MSG(CSD_PRIO_HIGH, "Restoring device [%d]", dev_id);
  /* Get the Device Record from the Active List */
  /*Q::1 How to Handle Device Enable in Suspend State? */
  a_dev = csd_dev_get_active_record(dev_id);
  /* Check if the Device Record is Valid? */
  if (a_dev == NULL)
  {
    CSD_MSG(CSD_PRIO_ERROR,"dev_id(%d), Not present in the Active List!!",dev_id);
    rc = CSD_EOK;
  }
  /* Check if the Device State is Suspended or not */
  else if ( 0 == (CSD_DEV_STATUS_SUSPEND_MASK & a_dev->mask) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"dev_id(%d), is Not in suspend state Mask=[0x%x]!!",dev_id,a_dev->mask);
    rc = CSD_EOK;
  }
  /* Check if the Device is Virtual Device? No Need to do Any Setup *//*Q::2 How abt ANC on Virtual Device? */
  else if ( (a_dev->dev_info.is_virtual) )
  {
    a_dev->mask ^= CSD_DEV_STATUS_SUSPEND_MASK;
    CSD_MSG( CSD_PRIO_MEDIUM,"Virtual Device (%d) No Need to Restore!", dev_id);
    rc = CSD_EOK;
  }
  else
  {
    CSD_MSG(CSD_PRIO_MEDIUM,
        "Start to Apply cached pre-device cfg for device [%d]", a_dev->dev_id);
    rc = csd_dev_apply_pre_dev_config(a_dev);
    /* Now Setup the Corresponding Device */
    CSD_MSG(CSD_PRIO_MEDIUM,"Start to Restore device [%d]", a_dev->dev_id);
    /* bring up the device */
    rc = csd_dev_manager_up(a_dev, CSD_DEV_MANAGER_SETUP);
    if (CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_ERROR, "Failed to Restore device(%d))", a_dev->dev_id);
      /* roll back the sm */
      csd_dev_manager_down(a_dev, CSD_DEV_MANAGER_TEARDOWN);
      csd_dev_remove_dev_info(a_dev->dev_id);
    }
    else
    {
      a_dev->mask ^= CSD_DEV_STATUS_SUSPEND_MASK;
      CSD_MSG(CSD_PRIO_HIGH,
          "Restore device[%d] completed successfully!!", a_dev->dev_id);

      /* Handle AFE Loopback, Companding, DTMC, ANC settings here */
      if (CSD_EOK == rc)
      {
        CSD_MSG(CSD_PRIO_MEDIUM,
            "Start to Apply cached post-device cfg for device [%d]", a_dev->dev_id);
        rc = csd_dev_apply_post_dev_config(a_dev);
      }
#if 0
      /* If ANC is Set: Then Enable ANC for the Rx Device.
       * This will implicitly enable ANC on corresponding Tx Device */
      if( (a_dev->mask & CSD_DEV_STATUS_ANC_MASK) &&
          (CSD_DEV_RX_DEVICE == a_dev->dev_info.ulDirectionMask) )
      {
        struct csd_cmd_dev_anc anc_ctl;
        anc_ctl.enable = CSD_DEV_CODEC_ANC_ENABLE;
        anc_ctl.rx_dev_id = dev_id;
        rc = csd_dev_cmd_enable_anc_control(CSD_DEV_CMD_ANC_CONTROL, &anc_ctl, sizeof(anc_ctl));

        CSD_MSG(CSD_PRIO_HIGH,
            "Setup ANC on RX device (%d),  Completed with rc=[0x%x]!", dev_id,rc);
      }
  #endif
    }
  }
  return rc;
}

/* \brief This function handles the regular device tear down.  */
int32_t csd_dev_suspend( uint32_t dev_id )
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* a_dev = NULL;

  CSD_MSG(CSD_PRIO_MEDIUM,"start: Suspend device [%d]!", dev_id);

  a_dev = csd_dev_get_active_record(dev_id);
  /* Check if the Device Record is Valid? */
  if (a_dev == NULL)
  {
    CSD_MSG(CSD_PRIO_ERROR,"dev_id(%d), Not present in the Active List!!",dev_id);
    rc = CSD_EOK;
  }
  /* Check if the Device State is Suspended or not */
  else if ( CSD_DEV_STATUS_SUSPEND_MASK == (CSD_DEV_STATUS_SUSPEND_MASK & a_dev->mask) )
  {
    CSD_MSG(CSD_PRIO_ERROR,
        "dev_id(%d), is already in suspend state Mask=[0x%x]!!",dev_id,a_dev->mask);
    rc = CSD_EOK;
  }
  /*Check if the Device is Virtual Device? No Need to do Any Setup */
  else if ( (a_dev->dev_info.is_virtual) )
  {
    a_dev->mask |= CSD_DEV_STATUS_SUSPEND_MASK;
    CSD_MSG( CSD_PRIO_MEDIUM,
        "Virtual Device (%d) No Need to Suspend!", dev_id);
    rc = CSD_EOK;
  }
  else
  {
#if 0
    /* If ANC is Set: Then Disable ANC for the Rx Device */
    if ( (a_dev->mask & CSD_DEV_STATUS_ANC_MASK) &&
              (CSD_DEV_RX_DEVICE == a_dev->dev_info.is_virtual) )
    {
      struct csd_cmd_dev_anc anc_ctl;
      anc_ctl.enable = CSD_DEV_CODEC_ANC_DISABLE;
      anc_ctl.rx_dev_id = dev_id;
      rc = csd_dev_cmd_enable_anc_control(CSD_DEV_CMD_ANC_CONTROL, &anc_ctl,
          sizeof(anc_ctl));
      CSD_MSG(CSD_PRIO_HIGH,
          "Setup ANC on RX device (%d),  Completed with rc=[0x%x]!", dev_id,rc);
      /* Now Restore the ANC mask:: This is needed to setup ANC again on Restore */
      a_dev->mask |= CSD_DEV_STATUS_ANC_MASK;
    }
#endif
    /* Tear Down the Device Now */
    rc = csd_dev_manager_down(a_dev, CSD_DEV_MANAGER_TEARDOWN);
    if (CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_ERROR, "Failed to Suspend device[%d]", a_dev->dev_id);
    }
    else
    {
      a_dev->mask ^= CSD_DEV_STATUS_SUSPEND_MASK;
      CSD_MSG(CSD_PRIO_HIGH,
          "Suspend device[%d] completed successfully!!", a_dev->dev_id);
    }
  }

  return rc;
}

