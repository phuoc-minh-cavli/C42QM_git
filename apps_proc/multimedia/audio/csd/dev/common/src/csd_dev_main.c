/*==============================================================================
 @file csd_dev_main.c

 The objective of this source file is to dispatch all external CSD_DEV_CMD_IOCTLs
 to the lower layer (csd_dev_tgt.c) within device driver.

 Copyright (c) 2008-13, 2016 QUALCOMM Technologies Incorporated.
 All rights reserved.
 Qualcomm Confidential and Proprietary
 ==============================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

   $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/src/csd_dev_main.c#4 $
   $DateTime: 2019/03/14 20:28:08 $
   $Author: pwbldsvc $
   $Change: 18597212 $
   $Revision: #4 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  03/01/17   vk      Fix multiple MO call issue
  11/15/16   rk      Abstracting OSAL layer
  08/27/15   jk      Added support for 32khz sample rate
  09/01/14   rp      enhanced the csd init debug buffer
  02/12/14   jk      Added support for CSD_DEV_CMD_RESTART   
  10/07/13   vk      added support for AVSync feature
  07/17/13   unni    Advesrial device test cases
  02/20/13   suns    Dev modules unification
  06/06/11   sgk     Dev Suspend/Restore Updates
  12/06/11   jk     added support for connect device command
  05/06/10   suns   Initial Creation
============================================================================*/
#include "csd_dev_main.h"
#include "csd_dev_common.h"
#include "csd_dev_manager.h"
#include "csd_debug_msg.h"
#include "csd_intf.h"
#include "csd_handle.h"
#include "csd_dev_acdb.h"
#include "csd.h"
#include "csd_debug_info.h"
#include "csd_dev_port_manager.h"
#include "csd_dev_speaker_protection.h"
#include "csd_os_dependencies.h"
#include "csd_dev_codec.h"
#include "csd_dev_device_info.h"
#include "wcd_intf.h"

#define AVSYNC_HW_DELAY_DEFAULT 0

csd_dev_cb_t g_dev_cb;
struct csd_dev_data dev_data;
static struct csd_vtable csd_dev_vtbl;
static uint32_t g_dev_handle = 0;

/*
 * For Caching Device_Configurations that can be applied before Device enable
 * Limitation: Same Configurations can be supported for Only one Device
 */
static csd_dev_cache_config_t g_dev_cache_config;
/********************************************************************
 * Internal helper functions
  ********************************************************************/
static int32_t csd_dev_validate_sr(struct csd_dev_record* dev, uint32_t sr)
{
  int32_t rc = CSD_EOK;

  switch(sr)
  {
    case CSD_DEV_SR_8000:
    {
      if( 0 == (dev->dev_info.sample_rate_mask & CSD_DEV_SR_MASK_8000))
      {
        rc = CSD_EBADPARAM;
      }
      break;
    }
  case CSD_DEV_SR_16000:
    {
      if( 0 == (dev->dev_info.sample_rate_mask & CSD_DEV_SR_MASK_16000))
      {
        rc = CSD_EBADPARAM;
      }
      break;
    }
  case CSD_DEV_SR_32000:
    {
      if( 0 == (dev->dev_info.sample_rate_mask & CSD_DEV_SR_MASK_32000))
      {
        rc = CSD_EBADPARAM;
      }
      break;
    }
  case CSD_DEV_SR_48000:
    {
      if( 0 == (dev->dev_info.sample_rate_mask & CSD_DEV_SR_MASK_48000))
      {
        rc = CSD_EBADPARAM;
      }
      break;
    }
  case CSD_DEV_SR_UNKNOWN:    
  default:
    {
      rc = CSD_EUNSUPPORTED;
      break;
    }
  }

  if(CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_validate_sr: Mismatch between supported "
            "sample rate mask[%x] and input sample rate[%d]",
            dev->dev_info.sample_rate_mask, sr);
  }

  return rc;
}

static int32_t csd_dev_validate_bps(struct csd_dev_record* dev, uint32_t bps)
{
  int32_t rc = CSD_EOK;
  
  switch(bps)
  {
    case CSD_DEV_BPS_16:
      {
        if( 0 == (dev->dev_info.bits_per_sample_mask & CSD_DEV_BPS_MASK_16))
        {
          rc = CSD_EBADPARAM;
        }
        break;
      }
    case CSD_DEV_BPS_24:
    {
      if( 0 == (dev->dev_info.bits_per_sample_mask & CSD_DEV_BPS_MASK_24))
      {
        rc = CSD_EBADPARAM;
      }
      break;
    }
/* CSD API doesn't support 32 bit width on device configuration. If needed, this will be
 * expanded.
    case CSD_DEV_BPS_32:
    {
      if( 0 == (dev->dev_info.bits_per_sample_mask & CSD_DEV_BPS_MASK_32))
      {
        rc = CSD_EBADPARAM;
      }
      break;
    }
*/
    default:
    {
      rc = CSD_EUNSUPPORTED;
      break;
    }
  }

  if(CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_validate_bps: "
            "Mismatch between supported bits per sample mask[%x]"
            " and input bits per sample[%x]",
            dev->dev_info.bits_per_sample_mask, bps);
  }


  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_remove_active_record

 DESCRIPTION   Helper function to iterate through the list and remove the refer-
               enced device

 DEPENDENCIES  Expects the caller to have lock around the data

 PARAMETERS    @param dev : reference to a device record

 RETURN VALUE  void

 SIDE EFFECTS  none

 =============================================================================*/
static void csd_dev_remove_active_record( csd_dev_record_t* dev)
{
  csd_dev_record_t* a_dev = NULL;
  csd_dev_record_t* a_prev_dev = NULL;

  if(NULL == dev)
  {
    /* NO-OP */
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_remove_active_record: "
            "Tried removing Null record");
    return;
  }

  a_dev = dev_data.active_devs_head;
  while(a_dev) 
  {
    if( a_dev->dev_id == dev->dev_id)
    {
      /* Match Found */
      if( a_prev_dev == NULL) 
      {
        /* Deleting first entry */
        dev_data.active_devs_head = a_dev->next;
        csd_free(a_dev);
      }
      else 
      {
        a_prev_dev->next = a_dev->next;
        csd_free(a_dev);
      }
      /* Remove Completed */
      break;
    }
    a_prev_dev = a_dev;
    a_dev = a_dev->next;
  }
}
/*=============================================================================

 FUNCTION      csd_dev_remove_dev_info_no_lock

 DESCRIPTION   Helper function to remove record device info for a device id.

 DEPENDENCIES  Expects the caller to have lock around the data
               *Compliemented by csd_dev_get_dev_info_no_lock

 PARAMETERS    @param dev_id : device id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_remove_dev_info_no_lock(const uint32_t dev_id)
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t *dev = NULL;
  bool_t remove_device = TRUE;

  dev = csd_dev_get_active_record(dev_id);
  if(NULL == dev)
  {
    CSD_MSG(CSD_PRIO_HIGH, "csd_dev_remove_dev_info_no_lock: "
            "Unable to fetch record for dev[%d]", dev_id);
    return CSD_ENOTFOUND;
  }

  if(ACDB_DEVICE_TYPE_SLIMBUS_PARAM == dev->dev_info.interface_type)
  {
    /* check for dynamic port implementation */
    bool_t freed = FALSE;
    int32_t pm_rc = CSD_EOK;

    pm_rc = csd_dev_pm_free_sb_port(dev->dev_info.afe_port_id,&freed);
    if( CSD_ENOTFOUND == pm_rc)
    {
      /* Not Dynamic */
//      CSD_MSG(CSD_PRIO_LOW, "csd_dev_remove_dev_info_no_lock: "
//              "Static port detected pm_rc[0x%x]",pm_rc);
    }
    else if(CSD_EOK == pm_rc)
    {
      /* Dynamic port management, check for whether all reference were freed */
      if(!freed)
      {
        remove_device = FALSE;
      }
    }
    else
    {
      /* Unknown error */
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_remove_dev_info_no_lock: "
              "Unknown error[0x%x]",pm_rc);
      rc = CSD_EUNEXPECTED;
    }
  }

  if(remove_device)
  {
    if(dev->mask&CSD_DEV_STATUS_REG_MASK)
    {
      /* Device is Running, will remove upon device teardown */
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_remove_dev_info_no_lock: "
              "Attempted to remove an running device [%d], "
              "Current mask[0x%x]", dev_id, dev->mask);
    }
    else
    {
      /* Removing unused record */
      csd_dev_remove_active_record(dev);
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_remove_dev_info_no_lock: "
              "Remove device [%d], Current mask[0x%x]", dev_id, dev->mask);
    }
  }
  return rc;
}

static int32_t csd_dev_reconfig_sr( csd_dev_record_t *a_dev, uint32_t sr )
{
  int32_t rc = CSD_EOK;
  int32_t temp_rc = CSD_EOK;

  rc = csd_dev_manager_down(a_dev, CSD_DEV_MANAGER_TEARDOWN);

  if( CSD_EOK != rc ) 
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_reconfig_sr: "
            "Failed to teardown device(%d) at stage (%d)",
                a_dev->dev_id,a_dev->manager_seq_index);
  }
  else
  {
    /* bring up the device at new sample rate*/
    a_dev->curr_attrib.sample_rate = sr;
   
    /* query avsync hw delay value at new sample rate*/
    temp_rc = csd_acdb_get_device_avsync_hw_delay(a_dev->dev_id,
                 a_dev->curr_attrib.sample_rate,&a_dev->av_sync_hw_delay);
    if( CSD_EOK != temp_rc )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_reconfig_sr: "
              "Unable to get av sync hw delay value "
              "for device(%d) for given sample rate(%d), rc[0x%x]",
              a_dev->dev_id,a_dev->curr_attrib.sample_rate,temp_rc);

      a_dev->av_sync_hw_delay = AVSYNC_HW_DELAY_DEFAULT;
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_reconfig_sr: "
              "Updating device record with default"
              " hw delay value [%d]", a_dev->av_sync_hw_delay);
    }
   
    rc = csd_dev_manager_up(a_dev, CSD_DEV_MANAGER_SETUP);
    if( CSD_EOK != rc ) 
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_reconfig_sr: "
              "Failed to reconfig device(%d) at sr (%d))",
                    a_dev->dev_id,sr);

      /* roll back the sequence */
      csd_dev_manager_down(a_dev, CSD_DEV_MANAGER_TEARDOWN);
      csd_dev_remove_dev_info_no_lock(a_dev->dev_id);
    }
  }
  return rc;
}

static void csd_dev_append_active_record( csd_dev_record_t* dev)
{
  csd_dev_record_t* a_dev = dev_data.active_devs_head;
    
  if( a_dev == NULL) 
  {
    dev_data.active_devs_head = dev;
  }
  while(a_dev) 
  {
    if(a_dev->next == NULL) 
    {
      a_dev->next = dev;
      dev->next = NULL;
      break;
    }
    a_dev = a_dev->next;
  }
}

/* insert at the head function not used commented out*/
/*static void csd_dev_insert_active_record( struct csd_dev_record* dev)
{
  if( dev_data.active_devs_head == NULL) 
  {
    dev_data.active_devs_head = dev;
  }
  else
  {
    dev->next = dev_data.active_devs_head;
    dev_data.active_devs_head = dev;
  }
}*/

/* this function returns the active dev record based on the 
 * device id. It will return match record or the head of the active list if
 * default device id is used. otherwise NULL is returned
 */
csd_dev_record_t* csd_dev_get_active_record( const uint32_t dev_id )
{
  csd_dev_record_t *a_dev = dev_data.active_devs_head;
  bool_t found = FALSE;
  
  while( a_dev ) 
  {
    if(a_dev->dev_id == dev_id) 
    {
      found = TRUE;
      break;
    }
    a_dev = a_dev->next;
  }
  if (!found) 
  {
    a_dev = NULL;
  }
  return a_dev;
}


static int32_t csd_dev_create_record(uint32_t,csd_dev_record_t**);

/*=============================================================================

 FUNCTION      csd_dev_get_dev_info_no_lock

 DESCRIPTION   Obtain record device info for a device id. If no active device
               is present, prefetch record and cache it for future use.

 DEPENDENCIES  Expected to be called locally. Lock across dev_data must be
               present before calling. 
               *Compliemented by csd_dev_remove_dev_info_no_lock

 PARAMETERS    @param dev_id : device id

 RETURN VALUE  csd_dev_record_t*

 SIDE EFFECTS  none

 =============================================================================*/
static csd_dev_record_t* csd_dev_get_dev_info_no_lock(uint32_t dev_id)
{
  int32_t rc = CSD_EOK;

  csd_dev_record_t* dev = csd_dev_get_active_record(dev_id);

  if( NULL == dev)
  {
    /* Prefetch device record and obtain active record */
    rc = csd_dev_create_record(dev_id, &dev);
  }

  if(CSD_EOK != rc || NULL == dev)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_get_dev_info_no_lock: "
            "Unable to fetch devid [%d], rc[0x%x]",
            dev_id, rc);
    dev = NULL;
  }
  else
  {
    if(ACDB_DEVICE_TYPE_SLIMBUS_PARAM == dev->dev_info.interface_type)
    {
      /* Increment if a dynamic port was used */
      csd_dev_pm_sb_port_inc_ref_count(dev->dev_info.afe_port_id);
    }
  }
  return dev;
}

/* \brief This function handles device configuration for RT Proxy devices.  */
static int32_t csd_dev_config(csd_dev_config_param_t *config_param,
                              uint32_t mask_bit)
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* dev = NULL;

//  CSD_MSG(CSD_PRIO_LOW, "csd_dev_config: "
//          "Start configuring device %d !",config_param->dev_id);

  /* Removed record management at this level */
  /* Do everythin in csd_dev_get_dev_info */
  /* No increment here */
  dev = csd_dev_get_active_record(config_param->dev_id);
  if(dev == NULL)
  {
    /*Creat a new device record*/
    rc = csd_dev_create_record(config_param->dev_id,&dev);
    if( CSD_EOK == rc )
    {
      //Update new device record.
      dev->dev_id = config_param->dev_id;

      if ((dev->dev_info.interface_type == ACDB_DEVICE_TYPE_RT_PROXY_PARAM))
      {
        csd_memscpy( &dev->cfg_param, sizeof(dev->cfg_param),
                     &config_param->csd_dev_cfg, sizeof(config_param->csd_dev_cfg) );
      }
//      CSD_MSG(CSD_PRIO_LOW,"csd_dev_config: "
//              "Configured Successfully %d", dev->dev_id);
    }/*end if csd_acdb_get_dev_info was successful*/

    /*
     * Not incrementing dynamic afe port reference as csd_dev_config has
     * no supplement/conjugate function to decrement the reference
     */
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_config: Device (%d) already Configured!",
        config_param->dev_id);
    rc = CSD_EALREADY;
  }
  return rc;
}

/*****************************************************************************
 * Internal Interface Functions
 *****************************************************************************/
/* \brief This function handles the regular device bring up */
int32_t csd_dev_setup(struct csd_dev_entry *new_dev, uint32_t mask_bit)
{
  int32_t rc = CSD_EOK;
  int32_t temp_rc = CSD_EOK;
  csd_dev_record_t* a_dev = NULL;
  bool_t dev_setup_flag = FALSE;

//  CSD_MSG(CSD_PRIO_LOW, "Set Up device %d at sample rate (%d)!",
//      new_dev->dev_id,new_dev->dev_attrib.sample_rate);

  a_dev = csd_dev_get_active_record(new_dev->dev_id);
  if(a_dev == NULL) 
  {
    /* No record present, creating new record */
    rc = csd_dev_create_record(new_dev->dev_id,&a_dev);
    if( (CSD_EOK == rc) &&
        (a_dev->dev_info.interface_type == ACDB_DEVICE_TYPE_RT_PROXY_PARAM) )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
              "This device need to get configured first!! (%d)",
              new_dev->dev_id);
      rc = CSD_EFAILED;
    }
      
    if( CSD_EOK == rc )
    {
      rc = csd_dev_validate_sr(a_dev, new_dev->dev_attrib.sample_rate);
      if (CSD_EOK != rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
                "Invalid sample rate(%d) specified for dev_id(%d),",
                new_dev->dev_attrib.sample_rate, new_dev->dev_id);
      }
    }
      
    if( CSD_EOK == rc )
    {
      rc = csd_dev_validate_bps(a_dev, new_dev->dev_attrib.bits_per_sample);
      if (CSD_EOK != rc)
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
                "Invalid bits per sample(%d) specified for dev_id(%d),",
                new_dev->dev_attrib.bits_per_sample, new_dev->dev_id);
      }
    }

    if( CSD_EOK == rc)
    {
      /*Get the HW delay value from acdb*/
      temp_rc = csd_acdb_get_device_avsync_hw_delay(new_dev->dev_id,
                  new_dev->dev_attrib.sample_rate,&a_dev->av_sync_hw_delay);
      if(CSD_EOK != temp_rc)
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
                "Unable to get av sync hw delay value "
                "for device(%d) for given sample rate(%d), rc[0x%x]",
                new_dev->dev_id,new_dev->dev_attrib.sample_rate,temp_rc);

        a_dev->av_sync_hw_delay = AVSYNC_HW_DELAY_DEFAULT;
        CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_setup: "
                "Updating device record with default "
                "hw delay value [%d]", a_dev->av_sync_hw_delay);
      }
    }
   
    if( CSD_EOK == rc )
    {
      /*Update new device record. */
      a_dev->curr_attrib.sample_rate =  new_dev->dev_attrib.sample_rate;
      a_dev->curr_attrib.bits_per_sample = new_dev->dev_attrib.bits_per_sample;
      /*If new device record is successfully created, set dev_setup_flag.*/
      dev_setup_flag = TRUE;
    }
    else
    {
      /* Remove faulty device from record list */
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_setup: "
              "Removing faulty record due to rc[%x]",rc);
      if ( NULL != a_dev )
      {
        csd_dev_remove_dev_info_no_lock(a_dev->dev_id);
      }
    }
  }
  else
  {
//    CSD_MSG(CSD_PRIO_LOW,"csd_dev_setup: "
//            "Current status [0x%x] and requested status [0x%x]",
//            a_dev->mask, mask_bit);
    if(a_dev->dev_info.interface_type != ACDB_DEVICE_TYPE_RT_PROXY_PARAM &&
       ((a_dev->mask & mask_bit) == CSD_DEV_STATUS_REG_MASK))
    {
      /* If device not null and device type is not proxy and was enabled before */
      if ( (a_dev->mask & CSD_DEV_STATUS_SUSPEND_MASK) && (mask_bit == CSD_DEV_STATUS_REG_MASK) )
      {
        /* If the device is in suspended and requested for active setting. */
        csd_dev_cache_dev_config(a_dev);
        rc = csd_dev_restore(a_dev->dev_id);
      }
      else if (a_dev->curr_attrib.sample_rate == new_dev->dev_attrib.sample_rate)
      {
        /* If the new device is already active and running at the same sample rate,
         * notify CSD_EALREADY to the caller.  */
//        CSD_MSG( CSD_PRIO_LOW, "csd_dev_setup: Device (%d) already started!", new_dev->dev_id);
        rc = CSD_EALREADY;
      }
      else if ( (mask_bit == CSD_DEV_STATUS_REG_MASK) && (a_dev->mask & CSD_DEV_STATUS_ANC_MASK) )
      {
        /* For special case, the requested TX device for regular operation was enabled earlier
        as ANC device, and running at different sample rate. we need to disable ANC, and reconfig 
           the tx device sample rate.
        */
        csd_dev_record_t* tmp_rx_dev = dev_data.active_devs_head;
        struct csd_cmd_dev_anc anc_ctl;
        uint32_t len = sizeof(struct csd_cmd_dev_anc);

//        CSD_MSG( CSD_PRIO_LOW, "csd_dev_setup: Device (%d) was enabled for ANC, need to "
//            "reconfig device sr and disable ANC!", new_dev->dev_id);
        while (tmp_rx_dev)
        {
          if ( (tmp_rx_dev->mask & CSD_DEV_STATUS_ANC_MASK) &&
              (tmp_rx_dev->dev_info.direction == CSD_DEV_RX_DEVICE))
          {
            anc_ctl.rx_dev_id = tmp_rx_dev->dev_id;
            anc_ctl.enable = CSD_DEV_CODEC_ANC_DISABLE;

            rc = csd_dev_tgt_ioctl(tmp_rx_dev, NULL, NULL, CSD_DEV_CMD_ANC_CONTROL,
                (void *) &anc_ctl, len);
            a_dev->mask &= ~CSD_DEV_STATUS_ANC_MASK;
            a_dev->mask |= CSD_DEV_STATUS_REG_MASK;
            rc = csd_dev_reconfig_sr(a_dev, new_dev->dev_attrib.sample_rate );
            break;
          }
          tmp_rx_dev = tmp_rx_dev->next;
        }/*end while*/
      } /*end if new dev is ANC device and enable cmd maskbit is reg dev status. */
      else
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_dev_setup: "
            "Device (%d) already in use, but running at different sr(%d)!",
            new_dev->dev_id, a_dev->curr_attrib.sample_rate);
        rc = CSD_EFAILED;
      }
    }
    else
    {
      /* Enabling Proxy port Device and Pre-fetched devices*/
      /* Validate the sample rate and bits per sample and enable device if valid */
      rc = csd_dev_validate_sr(a_dev, new_dev->dev_attrib.sample_rate);
      if( CSD_EOK != rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
                "Invalid sample rate(%d) for dev_id(%d)",
            new_dev->dev_attrib.sample_rate,new_dev->dev_id);
      }
      
      if( CSD_EOK == rc )
      {
        rc = csd_dev_validate_bps(a_dev, new_dev->dev_attrib.bits_per_sample);
        if (CSD_EOK != rc)
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
                  "Invalid bits per sample(%d) for dev_id(%d)",
              new_dev->dev_attrib.bits_per_sample,new_dev->dev_id);
        }
      }

      if( CSD_EOK == rc)
      {
        /*Get the HW delay value from acdb*/
        temp_rc = csd_acdb_get_device_avsync_hw_delay(new_dev->dev_id,
                      new_dev->dev_attrib.sample_rate,&a_dev->av_sync_hw_delay);
        if(CSD_EOK != temp_rc)
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_setup: "
                  "Unable to get av sync hw delay value "
                  "for device(%d) for given sample rate(%d), rc[0x%x]",
                  new_dev->dev_id,new_dev->dev_attrib.sample_rate,temp_rc);

          a_dev->av_sync_hw_delay = AVSYNC_HW_DELAY_DEFAULT;
          CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_setup: "
                  "Updating device record with default "
                  "hw delay value [%d]", a_dev->av_sync_hw_delay);
        }
      }
      
      if( CSD_EOK == rc )
      {
        a_dev->curr_attrib.sample_rate = new_dev->dev_attrib.sample_rate;  
        a_dev->curr_attrib.bits_per_sample = new_dev->dev_attrib.bits_per_sample;
        /*If new device is proxy device and both sr and bps are valid,
          set dev_setup_flag.*/
        dev_setup_flag = TRUE;
      }
    }/*end device was enabled and was proxy device */
  }/*end if device is not NULL*/
  
  if ( (TRUE == dev_setup_flag) && ( CSD_EOK == rc ) )
  {
    /* First Cache Device Configuration Params into dev_record and
     * clear contents/Flag from Global Structure */
    csd_dev_cache_dev_config(a_dev);
//    CSD_MSG(CSD_PRIO_LOW,"csd_dev_setup: Start to enable device %d", a_dev->dev_id);
    /* bring up the device */
    rc = csd_dev_manager_up(a_dev, CSD_DEV_MANAGER_SETUP);
    if( CSD_EOK != rc ) 
    {
      CSD_MSG(CSD_PRIO_ERROR, "Failed to enable device(%d))", a_dev->dev_id);
      /* roll back the setup sequence aka teardown*/
      csd_dev_manager_down(a_dev, CSD_DEV_MANAGER_TEARDOWN);
      csd_dev_remove_dev_info_no_lock(new_dev->dev_id);
    }
    else
    {
      /* Device enabled */
      a_dev->mask |= CSD_DEV_STATUS_REG_MASK;
      /* increment flag for dynamic port */
      if(ACDB_DEVICE_TYPE_SLIMBUS_PARAM == a_dev->dev_info.interface_type)
      {
        /* Increment if a dynamic port was used */
        csd_dev_pm_sb_port_inc_ref_count(a_dev->dev_info.afe_port_id);
      }
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_setup: Enable device [%d] Successful!!", a_dev->dev_id);
    }
  }
  return rc;
}


/* \brief This function handles the regular device tear down.  */
int32_t csd_dev_teardown(uint32_t dev_id, uint32_t mask_bit)
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* a_dev = NULL;
  bool_t dev_teardown_flag = FALSE;

//  CSD_MSG(CSD_PRIO_LOW, "csd_dev_teardown: Tear Down device %d!", dev_id);
    
  a_dev = csd_dev_get_active_record(dev_id);
  if(a_dev == NULL) 
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_dev_teardown: "
             "Cannot find device[%d] record to tear down!",
             dev_id);
    return CSD_ENOTFOUND;
  }

  if (mask_bit == CSD_DEV_STATUS_REG_MASK)
  {
    a_dev->mask &= ~mask_bit;
    /* Check if the Device is in Suspended State
     * (i.e Already TornDown with ANC disabled) */
    if ( a_dev->mask & CSD_DEV_STATUS_SUSPEND_MASK )
    {
      /* Clear the Suspend Mask bit */
      a_dev->mask ^= CSD_DEV_STATUS_SUSPEND_MASK;
    }
    else
    {
      /*If ANC still on, turn off ANC first then tear down the device.*/
      if (a_dev->mask & CSD_DEV_STATUS_ANC_MASK)
      {
        struct csd_cmd_dev_anc anc_ctl;
        uint32_t len = sizeof(struct csd_cmd_dev_anc);

        anc_ctl.rx_dev_id = dev_id;
        anc_ctl.enable = CSD_DEV_CODEC_ANC_DISABLE;
        /* tear down ANC device regarless of ANC_CONTROL rc */
        csd_dev_tgt_ioctl(a_dev, NULL, NULL, CSD_DEV_CMD_ANC_CONTROL,
            (void *) &anc_ctl, len);
        /* Clear the ANC Mask bit */
        a_dev->mask ^= CSD_DEV_STATUS_ANC_MASK;
      }
      /*If device is not in suspend state, set dev_teardown_flag.*/
      dev_teardown_flag = TRUE;
    }
  }/*end if mask_bit is CSD_DEV_STATUS_REG_MASK */
  else if (mask_bit == CSD_DEV_STATUS_ANC_MASK)
  {
    /* Check if the Device is in Suspended State
     * (i.e Already TornDown with ANC disabled) */
    if (a_dev->mask & CSD_DEV_STATUS_SUSPEND_MASK)
    {
      /* Disable mask bit after tear down so AFE isn't configured */
      a_dev->mask &= ~mask_bit;
      /* Clear the Suspend Mask bit */
      a_dev->mask ^= CSD_DEV_STATUS_SUSPEND_MASK;
    }
    else
    {
      /*If device is not in suspend state, set dev_teardown_flag.*/
      dev_teardown_flag = TRUE;
      a_dev->mask &= ~mask_bit; /* Disable mask bit after tear down so AFE isn't
                                   configured*/
    }/*end if device is not in suspend state.*/
  }/*end if mask_bit is CSD_DEV_STATUS_ANC_MASK */
  else
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_teardown: "
            "Unsupported mask_bit[%x]",mask_bit);
    rc = CSD_EUNSUPPORTED;
  }

  if ( (CSD_EOK == rc) && (TRUE == dev_teardown_flag) )
  {
    /* Tear down the device. */
    rc = csd_dev_manager_down(a_dev, CSD_DEV_MANAGER_TEARDOWN);
    if( CSD_EOK != rc )
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_teardown: "
              "Failed to teardown device(%d) at stage (%d)",
              dev_id,a_dev->manager_seq_index);

      /* Ignore failure of tear down, and attempt to clear the record
       * but don't over write the tear down failure error code*/
      csd_dev_remove_dev_info_no_lock(dev_id);
    }
    else
    {
      /* Remove from the active Device Records */
      rc = csd_dev_remove_dev_info_no_lock(dev_id);
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_teardown: Teardown Device [%d] Successful!!", dev_id);
    }
  }
  /* no tear down needed, only remove device record */
  if ( (CSD_EOK == rc) && (FALSE == dev_teardown_flag) )
  {
    /* Remove from the active Device Records */
    rc = csd_dev_remove_dev_info_no_lock(dev_id);
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_teardown: Remove Device [%d] Successful!!", dev_id);
  }

  return rc;
}

static int32_t csd_dev_cmd_enable_afe_loopback( uint32_t cmd_code,
                                                void *cmd_buf,
                                                uint32_t cmd_buf_len )
{
  int32_t rc = CSD_EOK;
  csd_dev_afe_loopback_t *loopback_cmd = (csd_dev_afe_loopback_t *) cmd_buf;
  csd_dev_record_t *rx_dev, *tx_dev;

  if ( cmd_buf_len != sizeof(csd_dev_afe_loopback_t) )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_enable_afe_loopback: "
        "Wrong pay load size for CSD_DEV_CMD_AFE_LOOPBACK!");
    rc = CSD_EBADPARAM;
  }
  else
  {
    rx_dev = csd_dev_get_active_record(loopback_cmd->rx_dev_id);
    tx_dev = csd_dev_get_active_record(loopback_cmd->tx_dev_id);
    if ( NULL == rx_dev || NULL == tx_dev )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_afe_loopback: "
          "RX device (%d) or TX devices(%d) has not enabled yet!",
          loopback_cmd->rx_dev_id, loopback_cmd->tx_dev_id);
      rc = CSD_EFAILED;
    }
    else
    {
      rc = csd_dev_tgt_ioctl(rx_dev, tx_dev, NULL, cmd_code, cmd_buf, cmd_buf_len);
      if (CSD_EOK == rc)
      {
        /* Set the AfeLoopBack flag accordingly and Cache the Config Params */
        if (loopback_cmd->enable)
        {
          rx_dev->cache_cfg_param.control_params |= CSD_DEV_AFE_LOOPBACK_SET;
          tx_dev->cache_cfg_param.control_params |= CSD_DEV_AFE_LOOPBACK_SET;
          CSD_MSG(CSD_PRIO_LOW, "csd_dev_cmd_enable_afe_loopback: "
                  "CSD_DEV_CMD_AFE_LOOPBACK Enabled!!");
        }
        else
        {
          /* AfeLoopback is Disabled :: Reset the AFE_LOOPBACK_SETTING FLAG */
          rx_dev->cache_cfg_param.control_params &= (~CSD_DEV_AFE_LOOPBACK_SET);
          tx_dev->cache_cfg_param.control_params &= (~CSD_DEV_AFE_LOOPBACK_SET);
          CSD_MSG(CSD_PRIO_LOW, "csd_dev_cmd_enable_afe_loopback: "
                  "CSD_DEV_CMD_AFE_LOOPBACK Disabled!!");
        }
        csd_memscpy(&rx_dev->cache_cfg_param.afe_loopback,
                    sizeof(rx_dev->cache_cfg_param.afe_loopback),
                    cmd_buf, cmd_buf_len);
        csd_memscpy(&tx_dev->cache_cfg_param.afe_loopback,
                    sizeof(tx_dev->cache_cfg_param.afe_loopback),
                    cmd_buf, cmd_buf_len);
      }
    }/* end if find the valid rx and tx device records. */
  }/* end if validate the pay load size */
  //wcd_ioctl(NULL,WCD_IOCTL_REGISTER_DUMP_TO_LOG,NULL);
  //CSD_SLEEP(3); for dump collection
  
  return rc;
}

static int32_t csd_dev_cmd_enable_anc_control( uint32_t cmd_code,
                                               void *cmd_buf,
                                               uint32_t cmd_buf_len )
{
  int32_t rc = CSD_EOK;
  struct csd_cmd_dev_anc *anc_ctl = (struct csd_cmd_dev_anc*)cmd_buf;
  csd_dev_record_t *rx_dev = NULL;
  csd_dev_record_t  tx_dev = {0,};
  uint32_t tx_dev_id;

  if((NULL == anc_ctl) || (cmd_buf_len != sizeof(struct csd_cmd_dev_anc))) 
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_enable_anc_control: "
        "Wrong payload size for CSD_DEV_CMD_ANC_CONTROL!");
    return CSD_EBADPARAM;
  }
  
  if(NULL == (rx_dev = csd_dev_get_active_record(anc_ctl->rx_dev_id)))
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_anc_control: "
        "RX device (%d) has not enabled yet!",anc_ctl->rx_dev_id);
    rc = CSD_EFAILED;
    return rc;
  }
  
  /* Get the Paired Tx Device: validates if Rx is set up as ANC or not */
  rc = csd_dev_acdb_get_anc_devs(anc_ctl->rx_dev_id, &tx_dev_id);
  if(CSD_EOK != rc)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_anc_control: "
        "RX device (%d) has no ANC Tx Paired Device!!, Error=[0x%x]",
        anc_ctl->rx_dev_id, rc);
    return rc;
  }
  
  /* Get the Requested mode of AANC [Enable/Disable] */
  if(CSD_DEV_CODEC_ANC_ENABLE == anc_ctl->enable)
  {
    if (NULL != csd_dev_get_active_record(tx_dev_id))
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_anc_control: "
        "ANC TX device (%d) Found in active device record!!",tx_dev_id);
      return CSD_EFAILED;
    }
    
    /* Prepare the Tx Device Properties: SR and BPS (same as Rx)
       mask value: same as Rx and append ANC mask ( is dummy, not used) 
    */
    tx_dev.dev_id = tx_dev_id;
    tx_dev.mask   = rx_dev->mask|CSD_DEV_STATUS_ANC_MASK;
    tx_dev.curr_attrib.sample_rate     = rx_dev->curr_attrib.sample_rate;  
    tx_dev.curr_attrib.bits_per_sample = rx_dev->curr_attrib.bits_per_sample;
    tx_dev.av_sync_hw_delay = 0;
    
    /* Get the ACDB Info for Tx Device */
    rc = csd_dev_acdb_get_dev_info(tx_dev_id, &tx_dev.dev_info);
    if(CSD_EOK != rc ) 
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_anc_control: "
        "Can not get Tx dev info from acdb:(%d)",tx_dev_id);
      return rc;
    }
  }
  /* Needs No additioanl processing for Disable */
  
  if(CSD_EOK == rc)
  {
    rc = csd_dev_tgt_ioctl(rx_dev,&tx_dev,NULL,cmd_code,cmd_buf,cmd_buf_len);
  }
    
  if(CSD_EOK == rc)
  {
    if(CSD_DEV_CODEC_ANC_ENABLE == anc_ctl->enable)
    {
      /* Set the flag and Cache the Configured Params */
      rx_dev->cache_cfg_param.control_params |= CSD_DEV_ANC_SET;
      csd_memscpy(&rx_dev->cache_cfg_param.anc_ctl,
                  sizeof(rx_dev->cache_cfg_param.anc_ctl),
                  cmd_buf, cmd_buf_len);
    }
    else
    {
      rx_dev->cache_cfg_param.control_params &= ~CSD_DEV_ANC_SET;
    }
  }
  return rc;
}

static int32_t csd_dev_cmd_enable_companding_control( uint32_t cmd_code,
                                                      void *cmd_buf,
                                                      uint32_t cmd_buf_len )
{
  int32_t rc = CSD_EOK;
  csd_dev_companding_t *cmp_ctl = (struct csd_dev_companding*) cmd_buf;
  csd_dev_record_t* rx_dev = NULL;

  if (cmd_buf_len != sizeof(struct csd_dev_companding))
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_companding_control: "
        "Wrong payload size for CSD_DEV_CMD_COMPANDING_CONTROL!");
    rc = CSD_EBADPARAM;
  }
  else
  {
    rx_dev = csd_dev_get_active_record(cmp_ctl->rx_dev_id);
    if (NULL == rx_dev)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_companding_control: "
              "Wrong device, only RX devices is supported!");
      rc = CSD_EBADPARAM;
    }
    else
    {
      rc = csd_dev_tgt_ioctl(rx_dev, NULL, NULL, cmd_code, cmd_buf, cmd_buf_len);
      if (CSD_EOK == rc)
      {
        /* Set the flag and Cache the Configured Params */
        rx_dev->cache_cfg_param.control_params |= CSD_DEV_COMAPANDING_SET;
        csd_memscpy(&rx_dev->cache_cfg_param.cmp_ctl,
                    sizeof(rx_dev->cache_cfg_param.cmp_ctl),
                    cmd_buf, cmd_buf_len);
      }
    } /* end if find the valid device record */
  } /* end if validate the pay load size */
  return rc;
}

static int32_t csd_dev_cmd_enable_dtmf_control( uint32_t cmd_code,
                                                void *cmd_buf,
                                                uint32_t cmd_buf_len )
{
  int32_t rc = CSD_EOK;
  csd_dev_dtmf_ctrl_t *dtmf_ctrl = (struct csd_dev_dtmf_ctrl *) cmd_buf;

  if ( (cmd_buf_len != sizeof(struct csd_dev_dtmf_ctrl)) || (dtmf_ctrl->p_dev_ids == NULL) )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_enable_dtmf_control: "
        "Wrong payload size for CSD_DEV_CMD_DTMF_CONTROL!");
    rc = CSD_EBADPARAM;
  }
  else
  {
    rc = csd_dev_tgt_ioctl(NULL, NULL, NULL, cmd_code, cmd_buf, cmd_buf_len);
    if (CSD_EOK == rc)
    {
      uint16_t nDevCount = 0;
      uint32_t nDevID = 0;
      csd_dev_record_t* rx_dev = NULL;

      /* Set the flag and Cache the Configured Params for all relevant devices. */
      do
      {
        nDevID = dtmf_ctrl->p_dev_ids[nDevCount++];
        rx_dev = csd_dev_get_active_record(nDevID);
        if (NULL != rx_dev)
        {
          /* DTMF is Enabled (infinite or +ve ms); Set the flag and Cache the Configured Params */
          if (dtmf_ctrl->dtmf_duration_in_ms)
          {
            rx_dev->cache_cfg_param.control_params |= CSD_DEV_DTMF_SET;
            CSD_MSG(CSD_PRIO_LOW, "csd_dev_cmd_enable_dtmf_control: "
                    "CSD_DEV_CMD_DTMF_CONTROL Enabled for Device[%d] !!", nDevID);
            csd_memscpy(&rx_dev->cache_cfg_param.dtmf_ctl,
                        sizeof(rx_dev->cache_cfg_param.dtmf_ctl),
                        cmd_buf, cmd_buf_len);
          }
          else /* DTMF is Disabled :: ReSet the flag and Cache the Configured Params */
          {
            rx_dev->cache_cfg_param.control_params &= (~CSD_DEV_DTMF_SET);
            CSD_MSG(CSD_PRIO_LOW, "csd_dev_cmd_enable_dtmf_control: "
                    "CSD_DEV_CMD_DTMF_CONTROL Disabled for Device[%d]!!", nDevID);
            csd_memscpy(&rx_dev->cache_cfg_param.dtmf_ctl,
                        sizeof(rx_dev->cache_cfg_param.dtmf_ctl) ,
                        cmd_buf, cmd_buf_len);
          }
        }
        else
        {
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_enable_dtmf_control: "
                  "CSD_DEV_CMD_DTMF_CONTROL on Non Active Device[%d]!!",nDevID);
          break;
        }
      } while (nDevCount < dtmf_ctrl->num_devs);
    }
  } /* end if validate the pay load size */
  return rc;
}

static int32_t csd_dev_cmd_enable_sidetone_control( uint32_t cmd_code,
                                                    void *cmd_buf,
                                                    uint32_t cmd_buf_len )
{
  int32_t rc = CSD_EOK;
  struct csd_dev_sidetone_ctrl* sidetone_ctrl = (struct csd_dev_sidetone_ctrl*) cmd_buf;
  csd_dev_record_t *rx_dev, *tx_dev;

  if ( (sidetone_ctrl == NULL) || (sizeof(struct csd_dev_sidetone_ctrl) != cmd_buf_len) )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_enable_sidetone_control: "
        "Wrong payload size for CSD_DEV_CMD_SIDETONE_CONTROL!");
    rc = CSD_EBADPARAM;
  }
  else
  {
    /* check if the devices is active */
    rx_dev = csd_dev_get_active_record(sidetone_ctrl->rx_dev_id);
    if (rx_dev == NULL)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_sidetone_control: "
          "Device id (%d) is not active", sidetone_ctrl->rx_dev_id);
      rc = CSD_EBADPARAM;
    }
    tx_dev = csd_dev_get_active_record(sidetone_ctrl->tx_dev_id);
    if (tx_dev == NULL)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_sidetone_control: "
          "Device id (%d) is not active", sidetone_ctrl->tx_dev_id);
      rc = CSD_EBADPARAM;
    }
    if (CSD_EOK == rc)
    {
      rc = csd_dev_tgt_ioctl(rx_dev, tx_dev, NULL, cmd_code, cmd_buf, cmd_buf_len);
      if (CSD_EOK == rc)
      {
        /* Set the flag and Cache the Configured Params */
        rx_dev->cache_cfg_param.control_params |= CSD_DEV_SIDETONE_SET;
        csd_memscpy(&rx_dev->cache_cfg_param.sidetone_ctl,
                    sizeof(rx_dev->cache_cfg_param.sidetone_ctl),
                    cmd_buf, cmd_buf_len);

        tx_dev->cache_cfg_param.control_params |= CSD_DEV_SIDETONE_SET;
        csd_memscpy(&tx_dev->cache_cfg_param.sidetone_ctl,
                    sizeof(tx_dev->cache_cfg_param.sidetone_ctl),
                    cmd_buf, cmd_buf_len);
      }
    } /* end if find the valid device record */
  } /* end if validate the pay load size */
  return rc;
}

static int32_t csd_dev_cmd_enable_hdmi_multi_channel_config( uint32_t cmd_code,
                                                             void *cmd_buf,
                                                             uint32_t cmd_buf_len )
{
  int32_t rc = CSD_EOK;
  struct csd_dev_hdmi_multi_channel_cfg* hdmi_dev_cfg = (struct csd_dev_hdmi_multi_channel_cfg*) cmd_buf;
  csd_dev_record_t *hdmi_dev = NULL;

  /* Validate the input configuration params */
  if ( (cmd_buf == NULL) || (sizeof(struct csd_dev_hdmi_multi_channel_cfg)!= cmd_buf_len) )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_hdmi_multi_channel_config: "
        "Wrong payload size for CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG!");
    rc = CSD_EBADPARAM;
  }
  else
  {
    hdmi_dev = csd_dev_get_active_record(hdmi_dev_cfg->dev_id);
    /* Check if the Device is in the active record list or not */
    if (hdmi_dev == NULL)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_cmd_enable_hdmi_multi_channel_config: "
          "HDMI Device id [%d] is not active:: Cache Configurations Locally!!",
          hdmi_dev_cfg->dev_id);
      /* Cache to Global Structure :: Need to Re-cache in the dev_record at the time of Device Enable */
      g_dev_cache_config.control_params |= CSD_DEV_HDMI_MULTICH_SET;
      csd_memscpy(&g_dev_cache_config.hdmi_multich_cfg,
                  sizeof(g_dev_cache_config.hdmi_multich_cfg),
                  cmd_buf, cmd_buf_len);
    }
    else
    {
      /* Set the flag and Cache the Configured Params */
      hdmi_dev->cache_cfg_param.control_params |= CSD_DEV_HDMI_MULTICH_SET;
      csd_memscpy(&hdmi_dev->cache_cfg_param.hdmi_multich_cfg,
                  sizeof(hdmi_dev->cache_cfg_param.hdmi_multich_cfg),
                  cmd_buf, cmd_buf_len);
    }
    /* Apply the HDMI configurations to the DSP/AFE */
    rc = csd_dev_tgt_ioctl(hdmi_dev, NULL, NULL, cmd_code, cmd_buf, cmd_buf_len);
  }/* end if validate the pay load size */
  return rc;
}

static int32_t csd_dev_cmd_aanc_control(uint32_t cmd_code, void *cmd_buf,
                                        uint32_t cmd_buf_len)
{
  int32_t rc = CSD_EOK;
  csd_cmd_dev_aanc_t *aanc_ctl = (csd_cmd_dev_aanc_t *)cmd_buf;
  csd_dev_record_t *rx_dev = NULL;
  csd_dev_record_t *tx_dev = NULL;
  csd_dev_record_t *ref_dev = NULL;

  if(cmd_buf_len != sizeof(csd_cmd_dev_aanc_t) ||
     NULL == cmd_buf)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
            "Bad parameters passed for CSD_DEV_CMD_AANC_CONTROL!");
    rc = CSD_EBADPARAM;
    return rc;
  }

  CSD_MSG(CSD_PRIO_HIGH, "csd_dev_cmd_aanc_control: "
          "RX_DevID = [%d] ref_DevID=[%d]",
          aanc_ctl->rx_dev_id, aanc_ctl->ref_dev_id);

  if(CSD_DEV_AFE_AANC_DISABLE == aanc_ctl->aanc_ctrl)
  {
    /*
     * AANC Disable, Record should be in existence as AANC should
     * have been enabled and we need to avoid increasing reference count
     */

    /* Get Rx device record */
    if(NULL == (rx_dev = csd_dev_get_active_record(aanc_ctl->rx_dev_id)))
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
              "Unable to fetch RX dev record for devid[%d]",
              aanc_ctl->rx_dev_id);
      rc = CSD_EFAILED;
      return rc;
    }

    /* Get Ref device record */
    if(NULL == (ref_dev = csd_dev_get_active_record(aanc_ctl->ref_dev_id)))
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
              "Unable to fetch Ref dev record for devid[%d]",
              aanc_ctl->ref_dev_id);
      rc = CSD_EFAILED;
      return rc;
    }

    /* Get Tx device record */
    if(NULL == (tx_dev = csd_dev_get_active_record(aanc_ctl->tx_dev_id)))
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
              "Unable to fetch TX dev record for devid[%d]",
              aanc_ctl->tx_dev_id);
      rc = CSD_EFAILED;
      return rc;
    }
  }
  else
  {
    /*
     * AANC ENABLE or Default needs increasing reference count *
     * If Default is AANC Enable : reference count is increment now and
     *                             decremented during AANC disable
     * If Default is AANC Disable : reference count is decremented below
     */

    /* Get Rx device record */
    if(NULL == (rx_dev = csd_dev_get_dev_info_no_lock(aanc_ctl->rx_dev_id)))
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
              "Unable to fetch RX dev record for devid[%d]",
              aanc_ctl->rx_dev_id);
      rc = CSD_EFAILED;
    }

    /* Get Ref device record */
    if((CSD_EOK == rc ) &&
       (NULL == (ref_dev = csd_dev_get_dev_info_no_lock(aanc_ctl->ref_dev_id))))
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
              "Unable to fetch Ref dev record for devid[%d]",
              aanc_ctl->ref_dev_id);
      rc = CSD_EFAILED;
    }

    /* Get Tx device record */
    if((CSD_EOK == rc ) &&
       (NULL == (tx_dev = csd_dev_get_dev_info_no_lock(aanc_ctl->tx_dev_id))))
    {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
              "Unable to fetch TX dev record for devid[%d]",
              aanc_ctl->tx_dev_id);
      rc = CSD_EFAILED;
    }
  }

  if(CSD_EOK == rc )
  {
    rc = csd_dev_tgt_ioctl(rx_dev, tx_dev, ref_dev, cmd_code,cmd_buf,cmd_buf_len);
  }

  /* Set the flag and Cache the Configured Params */
  if(CSD_EOK == rc && NULL != rx_dev)
  {
    rx_dev->cache_cfg_param.control_params |= CSD_DEV_AANC_SET;
    csd_memscpy(&(rx_dev->cache_cfg_param.aanc_ctl),
                sizeof(rx_dev->cache_cfg_param.aanc_ctl),
                cmd_buf, cmd_buf_len);

    /* TODO: Uncomment below once we finalize on the behavior for the 
      cached params and the way they need be treated when device is disable */
    /*
    tx_dev->dev_params.control_Params  |= CSD_DEV_AANC_SET;
    DALSYS_memcpy(&tx_dev->dev_params.aanc_ctl, cmd_buf,cmd_buf_len);
    */
  }

  if(CSD_EOK != rc || CSD_DEV_AFE_AANC_DISABLE == aanc_ctl->aanc_ctrl)
  {
    uint32_t temp_rc = CSD_EOK;

    /* Free up device record and reserved port for AANC */
    if( NULL != rx_dev )
    {
      temp_rc = csd_dev_remove_dev_info_no_lock(rx_dev->dev_id);
      if( CSD_EOK != temp_rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
                "Error freeing up rx_dev id[%d] rc=[0x%x]",
                rx_dev->dev_id, temp_rc);
        rc = CSD_EOK == rc ? temp_rc : rc;
      }
    }
    if( NULL != ref_dev )
    {
      temp_rc = csd_dev_remove_dev_info_no_lock(ref_dev->dev_id);
      if( CSD_EOK != temp_rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
                "Error freeing up ref_dev id[%d] rc=[0x%x]",
                ref_dev->dev_id, temp_rc);
        rc = CSD_EOK == rc ? temp_rc : rc;
      }
    }
    if( NULL != tx_dev )
    {
      temp_rc = csd_dev_remove_dev_info_no_lock(tx_dev->dev_id);
      if( CSD_EOK != temp_rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_cmd_aanc_control: "
                "Error freeing up tx_dev id[%d] rc=[0x%x]",
                tx_dev->dev_id, temp_rc);
        rc = CSD_EOK == rc ? temp_rc : rc;
      }
    }
  }

  return rc; 
}

static uint32_t csd_dev_set_evt_cb(csd_dev_cb_t *cb)
{
  int32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_set_evt_cb: (new): ENTER");
  csd_lock_enter(dev_data.mutex);

  if( NULL != cb )
  {
    /*save csd client callback*/
    g_dev_cb.cb = cb->cb;
    g_dev_cb.data = cb->data;

    CSD_MSG( CSD_PRIO_MEDIUM, "csd_dev_set_evt_cb: "
             "WCD callback setup is successful, "
             "callback address = [%p]!", ((void*)g_dev_cb.cb)) ;
  }
  else
  {
    rc = CSD_EFAILED;
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_set_evt_cb: failed");
  }

  csd_lock_leave(dev_data.mutex);

  CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_set_evt_cb: LEAVE (rc = [0x%x])", rc);
  return rc;
}

int32_t csd_dev_cmd_enable_fb_spkr_prot_control(
                                   csd_dev_spkr_calib_ctrl_t* fb_spkr_calib_cfg)
{
  int32_t rc = CSD_EOK;
  g_dev_cache_config.control_params |= CSD_DEV_SPKR_PROT_SET;
  csd_memscpy(&g_dev_cache_config.spkr_calib_cfg,
              sizeof(g_dev_cache_config.spkr_calib_cfg),
              fb_spkr_calib_cfg, sizeof(csd_dev_spkr_calib_ctrl_t));
  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_ssr_shutdown_mbhc

 DESCRIPTION   This is to handle the SSR shutdown for MBHC

 DEPENDENCIES  1)Expected to be called from csd_resourcemanager.c, 
                 csd_rm_handle_ssr_reset_complete()
                 If called within the dev module, mutex dead lock could occur.
               2) call tgt ioctl  CSD_DEV_MBHC_CMD_DISABLE to disable mbhc

 PARAMETERS    

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

=============================================================================*/
int32_t csd_dev_ssr_shutdown_mbhc( void )
{
  int32_t rc = CSD_EOK;
  struct csd_dev_mbhc_config mbhc_ctrl = {0};

  CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_ssr_shutdown_mbhc: MBHC SSR shutdown enter");

  mbhc_ctrl.mbhc_type = CSD_DEV_MBHC_CMD_DISABLE;
  rc = csd_dev_tgt_ioctl(NULL, NULL, NULL,CSD_DEV_CMD_CONFIG_MBHC_PARAMS,
                         &mbhc_ctrl,sizeof(mbhc_ctrl));
  if ( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ssr_shutdown_mbhc: "
            "MBHC disable failed rc=0x%x",rc);
  }
  else
  {
    CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_ssr_shutdown_mbhc: MBHC disable Complete");
  }

  csd_lock_enter(dev_data.mutex);   

  /* clear csd client callback */
  if(NULL == g_dev_cb.cb)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ssr_shutdown_mbhc: "
            "MBHC callback is NULL already");
  }
  g_dev_cb.cb = NULL;

  csd_lock_leave(dev_data.mutex);

  CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_ssr_shutdown_mbhc: "
          "MBHC SSR shutdown Complete");

  return rc;
}

/**-------------------------------------------------------------------------*/
/*! \brief   entry functions                                                 */
/**-------------------------------------------------------------------------*/
static int32_t csd_dev_ioctl( uint32_t handle, uint32_t cmd_code,
                              void* cmd_buf, uint32_t cmd_buf_len)
{
  int32_t rc = CSD_EOK;

  csd_lock_enter(dev_data.mutex);
  CSD_MSG(CSD_PRIO_LOW,  "csd_dev_ioctl: "
          "handle=0x%x,ioctl=0x%x,cmd_len=0x%x",
      handle, cmd_code, cmd_buf_len);
  switch(cmd_code) 
  {
    case CSD_DEV_CMD_CONFIGURE:
    {
      uint32_t num_dev = 1;
      struct csd_dev_configure *config_cmd = (struct csd_dev_configure *)cmd_buf;
      struct csd_dev_config_param *config_dev;

      if( cmd_buf_len != sizeof(struct csd_dev_configure) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_ioctl: "
            "Wrong payload size for CSD_DEV_CMD_CONFIGURE!");
        rc = CSD_EBADPARAM;
      }
      else
      {
        num_dev = config_cmd->num_devs;
        while( num_dev-- )
        {
          config_dev = &(config_cmd->devs[num_dev]);
          rc = csd_dev_config(config_dev,CSD_DEV_STATUS_REG_MASK);
          if( rc != CSD_EOK )
          {
            CSD_MSG(CSD_PRIO_ERROR, "csd_dev_ioctl: "
                    "Failed to configure device %d, err[%d]",
                config_dev->dev_id, rc);
            break; /*exit from the while*/
          }
        }/*end while*/
      }
      break;
    }
    case CSD_DEV_CMD_ENABLE:
    {
      uint32_t num_dev = 0;
      uint32_t newly_enabled_num_dev = 0;
      uint32_t newly_enabled_dev_id[CSD_DEVICE_MAX_ENTRIES] = {0};
      csd_dev_enable_t *enable_cmd = (struct csd_dev_enable *)cmd_buf;
      csd_dev_entry_t *new_dev;

      if (cmd_buf_len < sizeof(uint32_t))
      {
        CSD_MSG( CSD_PRIO_ERROR, "csd_dev_ioctl: "
                 "Wrong payload size for CSD_DEV_CMD_ENABLE: "
                 "cmd_buf_len=%d, minimum payload=%d",
                 cmd_buf_len, sizeof(uint32_t));

        rc = CSD_EBADPARAM;
        break;
      }
      num_dev = enable_cmd->num_devs;
      if ( cmd_buf_len <  ( sizeof(uint32_t) + (num_dev * sizeof(struct csd_dev_entry)) ) )
      {

        CSD_MSG( CSD_PRIO_ERROR, "csd_dev_ioctl: "
                  "Wrong payload size for CSD_DEV_CMD_ENABLE: "
                 "num_dev (%d) cmd_buf_len=%d",
                 num_dev, cmd_buf_len );
        rc = CSD_EBADPARAM;
        break;
      }

      while (num_dev--)
      {
        new_dev = &(enable_cmd->devs[num_dev]);

        if(CSD_EOK == csd_dev_fb_spkr_prot_is_fb_device(new_dev->dev_id))
        {
          csd_dev_spkr_calib_ctrl_t fb_spkr_calib_cfg = {0,};
          CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_ioctl: FB SPKR PROTECTION ENABLED");
          
          rc = csd_dev_fb_spkr_prot_init_cfg_params(new_dev, &fb_spkr_calib_cfg);
          
          if (rc == CSD_EOK )
          {
            rc = csd_dev_cmd_enable_fb_spkr_prot_control(&fb_spkr_calib_cfg);
            CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_ioctl: "
                    "FB SPKR PROTECTION - INIT MODE SUCCESS rc = [0x%x]",rc);
          }
          else
          {
            /*TODO: No need to break, if init normal mode fails,we continue
              device bringup without feedback enabled break;*/
            CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_ioctl: "
                    "FB SPKR PROTECTION NOT ENABLED - INIT FAILED rc = [0x%x]",rc);
          }
        }

        rc = csd_dev_setup(new_dev,CSD_DEV_STATUS_REG_MASK);
        if( (rc != CSD_EOK) && (rc != CSD_EALREADY))
        {        
          CSD_MSG(CSD_PRIO_ERROR, "csd_dev_ioctl: "
                  "Failed to set up for device %d, err[%d]",
              new_dev->dev_id, rc);
          break;
        }
        else if( rc == CSD_EOK )
        {
          /*
           * CSD_EOK indicates a newly added device
           * CSD_EALREADY indicates a already existing device which shouldn't
           *              be managed/disturbed in this instance
           */
          newly_enabled_dev_id[ newly_enabled_num_dev++ ] = new_dev->dev_id;
        }
      }/*end of while*/

      /* If the Second or Later Device Setup Failed: Then teardown that got
       * enabled prior in this ioctl call */
      if ((newly_enabled_num_dev > 0) &&
          (rc != CSD_EOK) && (rc != CSD_EALREADY)  )
      {
        int32_t temp_rc = CSD_EOK;
        CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_ioctl: "
                "Tearing down the devices that got enabled in this call!");
        while (--newly_enabled_num_dev)
        {
          temp_rc = csd_dev_teardown(newly_enabled_dev_id[newly_enabled_num_dev],
                                     CSD_DEV_STATUS_REG_MASK );
          CSD_MSG(CSD_PRIO_LOW,"csd_dev_ioctl: "
                  "Teardown of Device[%d] completed with rc=[0x%x] ",
                  newly_enabled_dev_id[newly_enabled_num_dev], temp_rc);
        }
      }
      break;
    }/*end of Switch case*/
    case CSD_DEV_CMD_DISABLE:
    {
      int32_t temp_rc = CSD_EOK;
      csd_dev_disable_t *disable_cmd = (struct csd_dev_disable *)cmd_buf;
      uint32_t num_dev = 1;

      if ( cmd_buf_len < sizeof(uint32_t) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_ioctl: "
                "Wrong payload size for CSD_DEV_CMD_DISABLE: "
                "cmd_buf_len=%d, minimum payload=%d",
                cmd_buf_len, sizeof(uint32_t));
        rc = CSD_EBADPARAM;
        break;
      }
      num_dev = disable_cmd->num_devs;
      if ( cmd_buf_len < ( sizeof(uint32_t) + (num_dev * sizeof(uint32_t)) ) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_ioctl: "
                "Wrong payload size for CSD_DEV_CMD_DISABLE: "
                "num_devs(%d), cmd_buf_len=%d",
                num_dev, cmd_buf_len);
        rc = CSD_EBADPARAM;
        break;
      }
      while (num_dev--)
      {
        temp_rc = csd_dev_teardown(disable_cmd->dev_ids[num_dev],
                                   CSD_DEV_STATUS_REG_MASK);
        if(temp_rc == CSD_ENOTFOUND)
        {
          CSD_MSG(CSD_PRIO_MEDIUM,"csd_dev_ioctl: "
                "Device not currently active, exit !");
          temp_rc = CSD_EOK;
        }
        if(CSD_EOK == rc)
        {
          /* Saving only the first failed rc or avoid success rc re-write */
          rc = temp_rc;
        }
      }
      break;
    }
    case CSD_DEV_CMD_GET_MAX_DEV_NUMS:
    {
      struct csd_dev_max_dev_nums *max_dev = (struct csd_dev_max_dev_nums*)cmd_buf;
      if (cmd_buf_len != sizeof(struct csd_dev_max_dev_nums)) 
      {
          CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ioctl: "
              "Wrong payload size for CSD_DEV_CMD_GET_MAX_DEV_NUMS!");
          rc = CSD_EBADPARAM;
      }
      else
      {
        max_dev->max_num_devs = csd_dev_oem_get_dev_count();
      }
      break;
    }
    case CSD_DEV_CMD_GET_DEV_CAPS:
    {
      struct csd_dev_caps *dev_caps = (struct csd_dev_caps*)cmd_buf;

      if( (cmd_buf_len != sizeof(struct csd_dev_caps)) ||
          (dev_caps->entries == NULL))
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ioctl: "
            "Wrong payload size for CSD_DEV_CMD_GET_DEV_CAPS!");
        rc = CSD_EBADPARAM;
      }
      else
      {
        /*From API definition CSD_DEV_CMD_GET_DEV_CAPS,
         * the client should allocate the buffer size of
         * (num_dev * sizeof(struct csd_dev_caps_entry))
         * How much is num_dev is not known, hence disabling this API with 0
         */
        uint32_t allocated_buf_size = 0;
        rc  = csd_dev_acdb_get_dev_cap(dev_caps,allocated_buf_size);
      }
      break;
    }
    case CSD_DEV_CMD_AFE_LOOPBACK:
    {
      rc = csd_dev_cmd_enable_afe_loopback(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_CONNECT_DEVICE:
    {
      csd_dev_connect_device_t *connect_cmd = (csd_dev_connect_device_t *)cmd_buf;
      csd_dev_record_t *source_dev = NULL;
      csd_dev_record_t *sink_dev = NULL;
      csd_dev_afe_loopback_t afe_loopback;
      uint32_t num_pairs;
      csd_dev_pair_t* temp_pair;

      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_ioctl: "
              "ioctl CSD_DEV_CMD_CONNECT_DEVICE");

      if( cmd_buf_len != sizeof( csd_dev_connect_device_t )) 
      {
        CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ioctl: "
            "Wrong playload size for CSD_DEV_CMD_CONNECT_DEV!");
        rc = CSD_EBADPARAM;
      }
      else
      {
        num_pairs = connect_cmd->num_pairs;
        afe_loopback.afe_mode = 1; /*default mode*/
        afe_loopback.enable = connect_cmd->connect_flag;
        while (num_pairs--)
        {
          temp_pair = (csd_dev_pair_t*) ( connect_cmd->pairs + ( sizeof( csd_dev_pair_t ) * num_pairs ) ) ; 
          source_dev = csd_dev_get_active_record(temp_pair->source_dev_id);        
          if ( NULL == source_dev ) 
          {
            CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ioctl: "
                "Soucrce device (%d) has not been enabled yet!",
                temp_pair->source_dev_id);
            rc = CSD_EFAILED;
          }
          
          sink_dev = csd_dev_get_active_record(temp_pair->sink_dev_id);
          if (NULL == sink_dev )
          {
            CSD_MSG(CSD_PRIO_ERROR,"csd_dev_ioctl: "
                "Sink device (%d) has not been enabled yet!",
                temp_pair->sink_dev_id);
            rc = CSD_EFAILED;
          }
          
          if( source_dev != NULL && sink_dev != NULL )
          {
            afe_loopback.rx_dev_id = temp_pair->source_dev_id;
            afe_loopback.tx_dev_id = temp_pair->sink_dev_id;
            rc = csd_dev_tgt_ioctl(source_dev,sink_dev, NULL, CSD_DEV_CMD_AFE_LOOPBACK ,&afe_loopback, sizeof( struct csd_dev_afe_loopback ) );
            CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_ioctl: "
                    "ioctl CSD_DEV_CMD_CONNECT_DEVICE result = %d ", rc );
          }
        }
      }
      break;
    }
    case CSD_DEV_CMD_ANC_CONTROL:
    {
      rc = csd_dev_cmd_enable_anc_control(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_COMPANDING_CONTROL:
    {
      rc = csd_dev_cmd_enable_companding_control(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_DTMF_CONTROL:
    {
      rc = csd_dev_cmd_enable_dtmf_control(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_SIDETONE_CONTROL:
    {
      rc = csd_dev_cmd_enable_sidetone_control(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG:
    {
      rc = csd_dev_cmd_enable_hdmi_multi_channel_config(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_AANC_CONTROL:
    {
      rc = csd_dev_cmd_aanc_control(cmd_code, cmd_buf, cmd_buf_len);
      break;
    }
    case CSD_DEV_CMD_SET_EVT_CB:
    {
      csd_dev_cb_t *dev_cb = (csd_dev_cb_t *) cmd_buf;

      if( cmd_buf_len != sizeof(csd_dev_cb_t) )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_ioctl: "
                "Wrong playload size for CSD_DEV_SET_EVT_CB!");
        rc = CSD_EBADPARAM;
      }
      else
      {
        rc = csd_dev_set_evt_cb(dev_cb);
      }
      break;
    }
case CSD_DEV_CMD_RESTART:
    {
      csd_dev_record_t* tx_dev = NULL;
      csd_dev_record_t* rx_dev = NULL;
      csd_cmd_dev_restart_t *restart_cmd = (csd_cmd_dev_restart_t *)cmd_buf;

      tx_dev = csd_dev_get_active_record(restart_cmd->tx_dev_id);
      rx_dev = csd_dev_get_active_record(restart_cmd->rx_dev_id);

      if(tx_dev == NULL)
      {
        CSD_MSG(CSD_PRIO_ERROR,
                  "csd_dev_ioctl: Cannot restart device(%d) has not been enabled yet!",
                  restart_cmd->tx_dev_id);
        rc = CSD_EFAILED;
      }
      if(rx_dev == NULL)
      {
        CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_ioctl: Cannot restart device(%d) has not been enabled yet!",
                restart_cmd->rx_dev_id);
        rc = CSD_EFAILED;
      }
      /* verify devices are enabled and of I2s interface*/
      if( tx_dev && rx_dev && 
          ( tx_dev->dev_info.interface_type == ACDB_DEVICE_TYPE_I2S_PARAM ) &&
          ( rx_dev->dev_info.interface_type == ACDB_DEVICE_TYPE_I2S_PARAM ) )
      {
        rc = csd_dev_validate_sr(tx_dev, restart_cmd->sample_rate);
        if( rc == CSD_EOK )
        {
          rc = csd_dev_validate_sr(rx_dev, restart_cmd->sample_rate);
        }
        if( rc == CSD_EOK )
        {
          /* validate sample rate is different then device is currently running at*/
          if( rx_dev->curr_attrib.sample_rate == restart_cmd->sample_rate ||
               tx_dev->curr_attrib.sample_rate == restart_cmd->sample_rate )
          {
            CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_ioctl: Restart failed device is already running at sample rate (%d )!",
                restart_cmd->sample_rate);
            rc = CSD_EALREADY;
          }
          else
          {
            rc = csd_dev_tgt_ioctl(rx_dev,tx_dev,NULL,cmd_code,cmd_buf,cmd_buf_len);
          }
        }
        else
        {
          CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_ioctl: Cannot restart unsupported sample rate(%d)!",
                restart_cmd->sample_rate);
        }
      }
      else
      {
         CSD_MSG(CSD_PRIO_ERROR,
                "csd_dev_ioctl: Cannot restart invalid device(%d), device(%d)!",
                restart_cmd->rx_dev_id, restart_cmd->rx_dev_id);
          rc = CSD_EFAILED;
      }
      break;
    }
  default:
    {
      rc = csd_dev_tgt_ioctl(NULL,NULL,NULL, cmd_code,cmd_buf,cmd_buf_len);
      break;
    }
  }
  
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_ioctl: "
        "csd_dev_ioctl end - handle=0x%x,ioctl=0x%x,rc=0x%x", handle, cmd_code, rc);
  csd_lock_leave(dev_data.mutex);
  return rc;
}

static uint32_t csd_dev_open(enum csd_open_code code, void* open_struct, uint32_t len)
{
  uint32_t handle = 0;
  if( len || open_struct || code != CSD_OPEN_DEVICE_CONTROL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_open: "
            " could not open device control, bad parameter\n");
  }
  else
  {
    csd_handle_set_ssr(g_dev_handle, FALSE );
    handle = g_dev_handle;
    CSD_MSG(CSD_PRIO_LOW, "csd_dev_open: handle=0x%x\n", g_dev_handle);
  }
  return handle;
}

static int32_t csd_dev_close(uint32_t handle)
{
  int32_t rc = CSD_EOK;

  if(!handle)
  {
    rc = CSD_EBADPARAM;
  }
  return rc;
}

int32_t csd_dev_dinit(void)
{
  csd_dev_record_t* dev_node = NULL;
  dev_node = dev_data.active_devs_head;

  while (dev_data.active_devs_head) 
  {
    if( dev_node->manager_seq_index >= CSD_DEV_INIT)
    {
      csd_dev_manager_down(dev_node, CSD_DEV_MANAGER_TEARDOWN);
    }
    dev_node = (dev_data.active_devs_head)->next;
    csd_free(dev_data.active_devs_head);
    dev_data.active_devs_head = dev_node;
  }

  if(dev_data.mutex)
  {
    csd_lock_destroy(dev_data.mutex);
    /*csd dev target de init done*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_RESOURCE_CREATE,0,0); 
  }
  csd_memset(&dev_data, 0, sizeof(dev_data));
  csd_dev_tgt_deinit();
  /*csd dev target de init done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_DEV_INIT_STAGE,
                   //CSD_INIT_DEV_TARGET_INIT,0,0); 

  csd_dev_fb_spkr_prot_deinit();
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_DEV_INIT_STAGE,
                   //CSD_INIT_DEV_FB_SPKR_PROT_INIT,0,0); 

  csd_dev_manager_deinit();
  /*csd dev sm de init done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_DEV_INIT_STAGE,
                   //CSD_INIT_DEV_MANAGER_INIT,0,0); 

  csd_handle_free(g_dev_handle);
  /*csd dev memory free done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,CSD_EOK,CSD_DEV_INIT_STAGE,
                   //CSD_INIT_DEV_HANDLE_MALLOC,0,0); 
  csd_memset(&csd_dev_vtbl, 0, sizeof(csd_dev_vtbl));
  return CSD_EOK;
}

int32_t csd_dev_init(struct csd_vtable ** vtable)
{
  int32_t rc = CSD_EOK;

  g_dev_handle = csd_handle_malloc(CSD_OPEN_DEVICE_CONTROL, &g_dev_handle);
  if (g_dev_handle == 0)
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_init: Failed to allocate device handle!");
    rc = CSD_ENORESOURCE;
  }
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                     //CSD_INIT_DEV_HANDLE_MALLOC,0,0);


  if( CSD_EOK == rc )
  {
    csd_memset(&dev_data, 0, sizeof(dev_data));
    csd_memset(&csd_dev_vtbl, 0, sizeof(csd_dev_vtbl));  
    if( CSD_EOK != csd_lock_create( &dev_data.mutex ) )
    {
      rc = CSD_ENORESOURCE;
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                       //CSD_INIT_DEV_RESOURCE_CREATE,0,0);
  }
  
  if( CSD_EOK == rc )
  {
    csd_dev_vtbl.open_fn = csd_dev_open;
    csd_dev_vtbl.close_fn = csd_dev_close;
    csd_dev_vtbl.write_fn = NULL;
    csd_dev_vtbl.read_fn = NULL;
    csd_dev_vtbl.ioctl_fn = csd_dev_ioctl;
    rc = csd_dev_manager_init();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_MANAGER_INIT,0,0); 
  }
  
  if( CSD_EOK == rc )
  {
    rc = csd_dev_tgt_init();
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_TARGET_INIT,0,0); 
  }

  if( CSD_EOK == rc )
  {
    rc = csd_dev_fb_spkr_prot_init();
    if (rc != CSD_EOK) {
      CSD_MSG(CSD_PRIO_ERROR, "csd_dev_init: Failed Speaker Protection Init!");
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,rc,CSD_DEV_INIT_STAGE,
                         //CSD_INIT_DEV_FB_SPKR_PROT_INIT,0,0); 
  }

  if( CSD_EOK == rc )
  {
    *vtable = &csd_dev_vtbl;
  }
  else
  {    
    csd_dev_dinit();
  }
  return rc;
}

/** Function to cache Device Config from Global to DeviceRecord */
int32_t csd_dev_cache_dev_config( csd_dev_record_t* a_dev )
{
  int32_t rc = CSD_EOK;
  uint32_t control_param = 0;
  uint32_t cmd_dst_len = 0;
  uint32_t cmd_src_len = 0;
  void *cmd_dst_buf = NULL;
  void *cmd_src_buf = NULL;

  if (NULL == a_dev)
  {
    return rc;
  }

#if 0
  /* Currrently ONLY HDMI config supports setting Config before Dev Enable
   * If we need to support any other configs before dev enable then, uncomment this part
   */

  /* Check and Cache AFE LOOPBACK */
  if(g_dev_cache_config.control_params & CSD_DEV_AFE_LOOPBACK_SET)
  {
    if((a_dev->dev_id == g_dev_cache_config.afe_loopback.rx_dev_id )||((a_dev->dev_id == g_dev_cache_config.afe_loopback.tx_dev_id )) )
    {
      cmd_dst_len = sizeof(a_dev->cache_cfg_param.afe_loopback);
      cmd_src_len = sizeof(g_dev_cache_config.afe_loopback);
      cmd_dst_buf = &(a_dev->cache_cfg_param.afe_loopback);
      cmd_src_buf = &(g_dev_cache_config.afe_loopback);

      csd_memscpy(cmd_dst_buf, cmd_dst_len, cmd_src_buf,cmd_src_len);
      a_dev->cache_cfg_param.control_params |= CSD_DEV_AFE_LOOPBACK_SET;
      control_param |= CSD_DEV_AFE_LOOPBACK_SET;
    }
  }
  /* Check and Cache COMAPNDING */
  if(g_dev_cache_config.control_params & CSD_DEV_COMAPANDING_SET)
  {
    if(a_dev->dev_id == g_dev_cache_config.cmp_ctl.rx_dev_id)
    {
      cmd_dst_len = sizeof(a_dev->cache_cfg_param.cmp_ctl);
      cmd_src_len = sizeof(g_dev_cache_config.cmp_ctl);
      cmd_dst_buf = &(a_dev->cache_cfg_param.cmp_ctl);
      cmd_src_buf = &(g_dev_cache_config.cmp_ctl);

      csd_memscpy(cmd_dst_buf, cmd_dst_len, cmd_src_buf,cmd_src_len);
      a_dev->cache_cfg_param.control_params |= CSD_DEV_COMAPANDING_SET;
      control_param |= CSD_DEV_COMAPANDING_SET;
    }
  }
  /* Check and Cache SIDETONE */
  if(g_dev_cache_config.control_params & CSD_DEV_SIDETONE_SET)
  {
    if((a_dev->dev_id == g_dev_cache_config.sidetone_ctl.rx_dev_id)||(a_dev->dev_id == g_dev_cache_config.sidetone_ctl.tx_dev_id))
    {
      cmd_dst_len = sizeof(a_dev->cache_cfg_param.sidetone_ctl);
      cmd_src_len = sizeof(g_dev_cache_config.sidetone_ctl);
      cmd_dst_buf = &(a_dev->cache_cfg_param.sidetone_ctl);
      cmd_src_buf = &(g_dev_cache_config.sidetone_ctl);

      csd_memscpy(cmd_dst_buf, cmd_dst_len, cmd_src_buf,cmd_src_len);
      a_dev->cache_cfg_param.control_params |= CSD_DEV_SIDETONE_SET;
      control_param |= CSD_DEV_SIDETONE_SET;
    }
  }
  /* Check and Cache ANC SET MASK */
  if(g_dev_cache_config.control_params & CSD_DEV_ANC_SET)
  {
    if(a_dev->dev_id == g_dev_cache_config.anc_ctl.rx_dev_id)
    {
      cmd_dst_len = sizeof(a_dev->cache_cfg_param.anc_ctl);
      cmd_src_len = sizeof(g_dev_cache_config.anc_ctl);
      cmd_dst_buf = &(a_dev->cache_cfg_param.anc_ctl);
      cmd_src_buf = &(g_dev_cache_config.anc_ctl);

      csd_memscpy(cmd_dst_buf, cmd_dst_len, cmd_src_buf,cmd_src_len);
      a_dev->cache_cfg_param.control_params |= CSD_DEV_ANC_SET;
      control_param |= CSD_DEV_ANC_SET;
    }
  }
#endif
  /* Check and Cache HDMI Multi Channel Cfg */
  if (g_dev_cache_config.control_params & CSD_DEV_HDMI_MULTICH_SET)
  {
    if (a_dev->dev_id == g_dev_cache_config.hdmi_multich_cfg.dev_id)
    {
      cmd_dst_len = sizeof(a_dev->cache_cfg_param.hdmi_multich_cfg);
      cmd_src_len = sizeof(g_dev_cache_config.hdmi_multich_cfg);
      cmd_dst_buf = &(a_dev->cache_cfg_param.hdmi_multich_cfg);
      cmd_src_buf = &(g_dev_cache_config.hdmi_multich_cfg);

      csd_memscpy(cmd_dst_buf, cmd_dst_len, cmd_src_buf, cmd_src_len);
      a_dev->cache_cfg_param.control_params |= CSD_DEV_HDMI_MULTICH_SET;
      control_param |= CSD_DEV_HDMI_MULTICH_SET;
    }
  }

  /* Check and Cache FB Speaker Protection Cfg */
  if(g_dev_cache_config.control_params & CSD_DEV_SPKR_PROT_SET)
  {
    if(a_dev->dev_id == g_dev_cache_config.spkr_calib_cfg.dev_id)
    {
      cmd_dst_len = sizeof(a_dev->cache_cfg_param.spkr_calib_cfg);
      cmd_src_len = sizeof(g_dev_cache_config.spkr_calib_cfg);
      cmd_dst_buf = &(a_dev->cache_cfg_param.spkr_calib_cfg);
      cmd_src_buf = &(g_dev_cache_config.spkr_calib_cfg);

      csd_memscpy(cmd_dst_buf, cmd_dst_len, cmd_src_buf,cmd_src_len);
      a_dev->cache_cfg_param.control_params |= CSD_DEV_SPKR_PROT_SET;
      control_param |= CSD_DEV_SPKR_PROT_SET;
    }
  }
  /* Now clear the settings in the global struct that were applied to this device */
  g_dev_cache_config.control_params ^= control_param;
  return rc;
}

int32_t csd_dev_apply_pre_dev_config( csd_dev_record_t* a_dev )
{
  int32_t rc = CSD_EOK;
  uint32_t cmd_buf_len = 0;
  uint32_t control_params = 0;
  void *cmd_buf = NULL;

  if (NULL == a_dev)
  {
    return rc;
  }
  control_params = a_dev->cache_cfg_param.control_params;
  /* Check and Apply HDMI Multi Channel Cfg */
  if (control_params & CSD_DEV_HDMI_MULTICH_SET)
  {
    cmd_buf_len = sizeof(a_dev->cache_cfg_param.hdmi_multich_cfg);
    cmd_buf = &(a_dev->cache_cfg_param.hdmi_multich_cfg);
    rc = csd_dev_cmd_enable_hdmi_multi_channel_config(
        CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG, cmd_buf, cmd_buf_len);
  }
  return rc;
}

int32_t csd_dev_apply_post_dev_config( csd_dev_record_t* a_dev )
{
  int32_t rc = CSD_EOK;
  uint32_t cmd_buf_len = 0;
  uint32_t control_params = 0;
  void *cmd_buf = NULL;

  if (NULL == a_dev)
  {
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_dev_apply_post_dev_config: "
              "Null dev passed returning");
    return rc;
  }

  control_params = a_dev->cache_cfg_param.control_params;
  /* Check and Apply AFE LOOPBACK */
  if (control_params & CSD_DEV_AFE_LOOPBACK_SET)
  {
    cmd_buf_len = sizeof(a_dev->cache_cfg_param.afe_loopback);
    cmd_buf = &(a_dev->cache_cfg_param.afe_loopback);
    /*rc = csd_dev_enable_afe_loopback(&(a_dev->cache_cfg_param.afe_loopback));*/
    rc = csd_dev_cmd_enable_afe_loopback(CSD_DEV_CMD_AFE_LOOPBACK, cmd_buf,
        cmd_buf_len);
  }
  /* Check and Apply COMAPNDING */
  if (control_params & CSD_DEV_COMAPANDING_SET)
  {
    cmd_buf_len = sizeof(a_dev->cache_cfg_param.cmp_ctl);
    cmd_buf = &(a_dev->cache_cfg_param.cmp_ctl);
    /*rc = csd_dev_control_device_companding(a_dev->dev_id, (a_dev->cache_cfg_param.cmp_ctl.comp_options));*/
    rc = csd_dev_cmd_enable_companding_control(CSD_DEV_CMD_COMPANDING_CONTROL,
        cmd_buf, cmd_buf_len);
  }
  /* Check and Apply SIDETONE */
  if (control_params & CSD_DEV_SIDETONE_SET)
  {
    cmd_buf_len = sizeof(a_dev->cache_cfg_param.sidetone_ctl);
    cmd_buf = &(a_dev->cache_cfg_param.sidetone_ctl);
    /*rc = csd_dev_control_device_sidetone(&(a_dev->cache_cfg_param.sidetone_ctl));*/
    rc = csd_dev_cmd_enable_sidetone_control(CSD_DEV_CMD_SIDETONE_CONTROL,
        cmd_buf, cmd_buf_len);
  }
  /* Check and Apply ANC SET MASK */
  if (control_params & CSD_DEV_ANC_SET)
  {
    cmd_buf_len = sizeof(a_dev->cache_cfg_param.anc_ctl);
    cmd_buf = &(a_dev->cache_cfg_param.anc_ctl);
    /*rc = csd_dev_control_device_ANC(a_dev->dev_id, &(a_dev->cache_cfg_param.anc_ctl.enable));*/
    rc = csd_dev_cmd_enable_anc_control(CSD_DEV_CMD_ANC_CONTROL, cmd_buf,
        cmd_buf_len);
  }

  /* Check and Apply AANC SET MASK */
  if(control_params & CSD_DEV_AANC_SET)
  {
      cmd_buf_len = sizeof(a_dev->cache_cfg_param.aanc_ctl);
      cmd_buf     = &(a_dev->cache_cfg_param.aanc_ctl);
      rc = csd_dev_cmd_aanc_control( CSD_DEV_CMD_AANC_CONTROL, cmd_buf, cmd_buf_len);
  }

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_ssr_teardown_devices

 DESCRIPTION   SSR teardown device call

 DEPENDENCIES  n/a

 PARAMETERS

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_ssr_teardown_devices()
{
  return csd_dev_force_teardown_all_devices();
}

/*=============================================================================

 FUNCTION      csd_dev_force_teardown_devices

 DESCRIPTION   Forcefully teardown all devices

 DEPENDENCIES  n/a

 PARAMETERS

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_force_teardown_all_devices()
{
  csd_dev_record_t *dev_node = dev_data.active_devs_head;

  csd_lock_enter(dev_data.mutex);
  CSD_MSG( CSD_PRIO_ERROR, "csd_dev_ssr_teardown_devices\n");
  while (dev_data.active_devs_head)
  {
    CSD_MSG( CSD_PRIO_ERROR,
        "csd_dev_ssr_teardown_devices DevID=[%d]\n", dev_node->dev_id );

    csd_dev_manager_down(dev_node, CSD_DEV_MANAGER_TEARDOWN);
    dev_node = (dev_data.active_devs_head)->next;
    csd_free(dev_data.active_devs_head);
    dev_data.active_devs_head = dev_node;
  }
  csd_lock_leave(dev_data.mutex);
  return CSD_EOK;
}
/*=============================================================================

 FUNCTION      csd_dev_create_record

 DESCRIPTION   Query ACDB for record and populate append it to dev data list.
               Detect and dynamici

 DEPENDENCIES  Expects the caller to have lock around the data

 PARAMETERS    @param dev_id : device id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
static int32_t csd_dev_create_record( uint32_t dev_id,
                                      csd_dev_record_t** dev_rec )
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* p_dev;

  if ( NULL == dev_rec )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_create_record: "
            " Null Param, dev_info[%p]", ((void*)dev_rec));
    return CSD_EBADPARAM;
  }

  if ( NULL != *dev_rec )
  {
    /* Avoid Memory leak by checking whether the reference is empty */
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_create_record: "
            "Reference isn't empty *dev_info[%p]", ((void*)*dev_rec));
    return CSD_EBADPARAM;
  }

  /* Create a new device record */
  csd_malloc(sizeof(csd_dev_record_t),(void**)dev_rec);
  if( *dev_rec == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_create_record: "
            "Failed to allocate memory of size (%d)for a new "
            "dev record, dev_id(%d)",sizeof(csd_dev_record_t), dev_id);
    return CSD_ENORESOURCE;
  }

  /* Direct pointer for easy reference */
  p_dev = *dev_rec;

  /* Clearing data */
  csd_memset(p_dev, 0, sizeof( csd_dev_record_t ));

  /* Populating data */
  p_dev->dev_id = dev_id;

  /* Querying ACDB */
  rc = csd_dev_acdb_get_dev_info(dev_id, &(p_dev->dev_info) );
  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_create_record: "
            "Cannot get dev info from acdb for dev id [%d], "
            "rc[%x]", dev_id, rc);
  }

  if( CSD_EOK == rc )
  {
    if((ACDB_DEVICE_TYPE_SLIMBUS_PARAM == p_dev->dev_info.interface_type) &&
       (CSD_DEV_INVALID_AFE_PORT_ID ==  p_dev->dev_info.afe_port_id))
    {
      /* Slimbus dynamic Port requirement meet, allocating a new port */
      rc = csd_dev_pm_alloc_sb_port(p_dev->dev_info.direction,
                                    &(p_dev->dev_info.afe_port_id));
    }
  }

  if( CSD_EOK != rc )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_create_record: "
            "Removing faulty device record, rc=[%x]", rc);
    csd_free(p_dev);
  }
  else
  {
    /* Append the record to list */
    csd_dev_append_active_record(p_dev);
  }

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_get_dev_info

 DESCRIPTION   Obtain record device info for a device id. If no active device
               is present, prefetch record and cache it for future use.

 DEPENDENCIES  Expected to be called by modules outside dev. If called within
               the dev module, mutex dead lock could occur.

 PARAMETERS    @param dev_id : device id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_get_dev_info( uint32_t dev_id, csd_dev_info_t* dev_info )
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* active_dev_info = NULL;

  csd_lock_enter(dev_data.mutex);

  if ( NULL == dev_info )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_get_dev_info: "
            "Null param, dev_info[%p]",((void*)dev_info));
    rc = CSD_EBADPARAM;
  }

  if( CSD_EOK == rc )
  {
    active_dev_info = csd_dev_get_dev_info_no_lock(dev_id);
  }

  if(CSD_EOK == rc && NULL != active_dev_info)
  {
    /* All good, populate data */
    dev_info->afe_port_id = active_dev_info->dev_info.afe_port_id;
    dev_info->direction_mask = active_dev_info->dev_info.direction;
    dev_info->channel_config = active_dev_info->dev_info.channel_config;
    dev_info->dev_interface_type = active_dev_info->dev_info.interface_type;
    /* dev_info->audio_topology_id and dev_info->voice_topology_id are not
     * populated or disturbed here as dev module is not aware of these data */

    switch(active_dev_info->dev_info.interface_type)
    {
      case ACDB_DEVICE_TYPE_SLIMBUS_PARAM:
        dev_info->codec_path_id =
                     active_dev_info->dev_info.u.slimbus_dev_info.codec_path_id;
        break;
      case ACDB_DEVICE_TYPE_I2S_PARAM:
        dev_info->codec_path_id =
                         active_dev_info->dev_info.u.i2s_dev_info.codec_path_id;
        break;
      default:
        dev_info->codec_path_id = CSD_DEV_INVALID_CODEC_PATH_ID;
        break;
    }
  }

  csd_lock_leave(dev_data.mutex);

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_remove_dev_info

 DESCRIPTION   Remove record device info for a device id.

 DEPENDENCIES  Expected to be called by modules outside dev. If called within
               the dev module, mutex dead lock could occur.

 PARAMETERS    @param dev_id : device id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_remove_dev_info(const uint32_t dev_id)
{
  int32_t rc = CSD_EOK;

  csd_lock_enter(dev_data.mutex);
  rc = csd_dev_remove_dev_info_no_lock(dev_id);
  csd_lock_leave(dev_data.mutex);

  return rc;
}


/*=============================================================================

 FUNCTION      csd_dev_get_afe_port_id

 DESCRIPTION   Get afe port id for a device

 DEPENDENCIES  Expected to be called by modules outside dev. If called within
               the dev module, mutex dead lock could occur.

 PARAMETERS    @param dev_id : device id
               @param afe_port_id : return value for afe port id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_get_active_afe_port_id(const uint32_t dev_id,
                                       uint16_t* afe_port_id)
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* active_dev_info;

  if(NULL == afe_port_id)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_get_active_afe_port_id: "
            "Null Param, afe_port_id[%p]",((void*)afe_port_id));
    return CSD_EBADPARAM;
  }

  csd_lock_enter(dev_data.mutex);

  /* Check and obtain active record if any */
  active_dev_info = csd_dev_get_active_record(dev_id);
  if(NULL == active_dev_info)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_get_active_afe_port_id: "
            "No active record for dev_id[%d]",dev_id);
    rc = CSD_ENOTFOUND;
    *afe_port_id = CSD_DEV_INVALID_AFE_PORT_ID;
  }
  else
  {
    *afe_port_id = active_dev_info->dev_info.afe_port_id;
  }

  csd_lock_leave(dev_data.mutex);

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_is_valid_sample_rate

 DESCRIPTION   Valid sample rate wrt to CSD

 DEPENDENCIES  Expected to be called by modules outside dev. If called within
               the dev module, mutex dead lock could occur.

 PARAMETERS    @param dev_id : device id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_is_valid_sample_rate(uint32_t dev_id, uint32_t sample_rate)
{
  int32_t rc = CSD_EOK;
  csd_dev_record_t* active_dev_info;

  csd_lock_enter(dev_data.mutex);

  /* Check and obtain active record if any */
  active_dev_info = csd_dev_get_active_record(dev_id);
  if(NULL == active_dev_info)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_is_valid_sample_rate: "
            "No active record for dev_id[%d]",dev_id);
    rc = CSD_ENOTFOUND;
  }
  else
  {
    rc = csd_dev_validate_sr(active_dev_info, sample_rate);
    if(CSD_EOK != rc)
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_dev_is_valid_sample_rate: "
              "Invalid sample rate[%d] for dev[%d]",
              sample_rate,dev_id);
    }
  }

  csd_lock_leave(dev_data.mutex);

  return rc;
}

/*=============================================================================

 FUNCTION      csd_dev_get_audio_record_device_pair

 DESCRIPTION   Wrapper for csd_acdb_get_audio_record_device_pair

 DEPENDENCIES

 PARAMETERS    @param tx_dev_id : tx device id
               @param rx_dev_id : rx device id

 RETURN VALUE  csd return status @see csd_status.h

 SIDE EFFECTS  none

 =============================================================================*/
int32_t csd_dev_get_audio_record_device_pair(uint32_t tx_dev_id,
                                             uint32_t *rx_dev_id)
{
  uint32_t rc = CSD_EOK;
  if ( NULL == rx_dev_id )
  {
    rc = CSD_EBADPARAM;
  }
  else
  {
    rc = csd_acdb_get_audio_record_device_pair(tx_dev_id, rx_dev_id);
  }
  return rc;
}
