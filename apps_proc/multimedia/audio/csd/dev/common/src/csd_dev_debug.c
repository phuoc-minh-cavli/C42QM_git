/*==============================================================================
  @file csd_dev_debug.c

  The objective of this source file is to define debug functions related to 
  csd_dev. This should act like csd_dev_main but should comply with csd general
  debug source files. We have a init which return function local static table  
  consisting of static function pointers. @Unni-Please consider delete or rework

  Copyright (c) 2008-13, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/src/csd_dev_debug.c#1 $
  $DateTime: 2018/12/25 22:39:07 $
  $Author: pwbldsvc $
  $Change: 17929171 $
  $Revision: #1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  11/15/16   rk      Abstracting OSAL layer
  02/15/13   suns    Device module unification
  09/21/12   unni    B Family
                     -Reformatting to a common format
  05/06/10   suns    Initial Creation

==============================================================================*/


#include "csd_dev_main.h"
#include "csd_dev_common.h"
#include "csd_dev_manager.h"
#include "csd_debug_msg.h"
#include "csd_intf.h"
#include "csd_handle.h"
#include "csd_dev_debug.h"

static uint32_t csd_dev_debug_control_handle = 0;


/* 
 * This function returns the active dev record count
 */
static uint32_t csd_dev_count_active_record_with_lock( void )
{
  struct csd_dev_record *a_dev;
  uint32_t count = 0;

  csd_lock_enter(dev_data.mutex);
  a_dev = dev_data.active_devs_head;
  while( a_dev )
  {
    count++;
    a_dev = a_dev->next;
  }
  csd_lock_leave(dev_data.mutex);
  return count;
}

static uint32_t csd_dev_debug_open( enum csd_open_code code, void* open_struct,
                                    uint32_t len )
{
  uint32_t rc = 0;

  if (len || open_struct)
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_debug_open: Bad input param for len[%d],open_struct[%p]!",
                           len, open_struct);
    rc = 0;
  }
  /* for debug control open we only need to return the as control handle */
  if(CSD_OPEN_DEBUG_DEVICE_CONTROL == code)
  {
    csd_dev_debug_control_handle = csd_handle_malloc(CSD_OPEN_DEBUG_DEVICE_CONTROL,
                                                     &csd_dev_debug_control_handle);
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_debug_open: open handle [0x%x]",
      csd_dev_debug_control_handle);
    if(csd_dev_debug_control_handle == 0)
    {
      rc = 0;
    }
    else
    {
      rc = csd_dev_debug_control_handle;
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_debug_open: Not recognized open code(%d) for !",code);
    rc = 0;
  }
  return rc;
}

static int32_t csd_dev_debug_close(uint32_t handle)
{
  int32_t rc = CSD_EOK;

  if( !handle )
  {
    CSD_MSG(CSD_PRIO_ERROR,"csd_dev_debug_close: Bad input param for handle[%x]!",handle);
    rc = CSD_EBADPARAM;
  }
  else
  {
    CSD_MSG(CSD_PRIO_HIGH,"csd_dev_debug_close: close handle [0x%x]", handle);
    csd_handle_free(handle);
    handle = 0;
    rc =  CSD_EOK;
  }
  return rc;
}

static int32_t csd_dev_debug_ioctl( uint32_t handle, uint32_t cmd,
                                    void* payload, uint32_t len )
{
  int32_t rc = CSD_EOK;
  int32_t i;

  if( !handle )
  {
    return CSD_EBADPARAM;
  }
  switch( cmd )
  {
    case CSD_DEBUG_CMD_ACTIVE_DEV_QUERY:
    {
      struct csd_debug_active_dev_query* tmp =
          (struct csd_debug_active_dev_query*) payload;

      if( sizeof(struct csd_debug_active_dev_query) != len )
      {
        rc = CSD_EBADPARAM;
        break;
      }
      tmp->num_devs = csd_dev_count_active_record_with_lock();
        CSD_MSG(CSD_PRIO_HIGH,"csd_dev_debug_ioctl: CSD_DEBUG_CMD_ACTIVE_DEV_QUERY "
             "dev debug handle=0x%x, num_devs=%d",
             handle, tmp->num_devs);
      break;
    }
    case CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY:
    {
      struct csd_debug_active_dev_info_query* tmp =
          (struct csd_debug_active_dev_info_query*) payload;
      struct csd_dev_record *a_dev = NULL;

      if( sizeof(struct csd_debug_active_dev_info_query) != len )
      {
        rc = CSD_EBADPARAM;
        break;
      }
      csd_lock_enter(dev_data.mutex);
      a_dev = dev_data.active_devs_head;
      if( a_dev != NULL )
      {
        rc = csd_dev_tgt_debug_fill_dev_info(a_dev, tmp);
        CSD_MSG(CSD_PRIO_HIGH, "csd_dev_debug_ioctl: dev debug handle=0x%x, num_entries=%d",
          handle, tmp->num_entries);
        /* print out active device info */
        for(i = 0; i < tmp->num_entries; i++) {
          CSD_MSG(CSD_PRIO_HIGH, "csd_dev_debug_ioctl: "
            "dev debug handle=0x%x, entry=%d, dev_id=%d, sample_rate=0x%x, "
            "bits_per_sample=%d, port_id=%d, is_virtual_dev=%d, timing_mode=%d",
            handle, i, tmp->entries[i].dev_id, tmp->entries[i].sample_rate,
            tmp->entries[i].bits_per_sample, tmp->entries[i].port_id,
            tmp->entries[i].is_virtual_dev, tmp->entries[i].timing_mode);
        }
      }
      else
      {
        rc = CSD_ENOTFOUND;
      }
      csd_lock_leave(dev_data.mutex);
      break;
    }
    case CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY_V2:
    {
        struct csd_debug_active_dev_info_query_v2* tmp_v2 =
            (struct csd_debug_active_dev_info_query_v2 *) payload;
        struct csd_dev_record *a_dev = NULL;
  
        if( sizeof(struct csd_debug_active_dev_info_query_v2) != len )
        {
          rc = CSD_EBADPARAM;
          break;
        }
        csd_lock_enter(dev_data.mutex);
        a_dev = dev_data.active_devs_head;
        if( a_dev != NULL )
        {
          rc = csd_dev_tgt_debug_fill_dev_info_v2(a_dev, tmp_v2);
          CSD_MSG(CSD_PRIO_LOW,"CSD:DEV DEBUG => csd_dev_debug_ioctl CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY_V2 dev debug handle=0x%x, num_entries=%d",
            handle, tmp_v2->num_entries);
          /* print out active device info */
          for(i = 0; i < tmp_v2->num_entries; i++) {
            CSD_MSG(CSD_PRIO_LOW, "CSD:DEV DEBUG => csd_dev_debug_ioctl CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY_V2 dev entry=%d, dev_id=%d, adie_handle = 0x%x, sample_rate=0x%x, bits_per_sample=%d, port_id=%d, is_virtual_dev=%d, timing_mode=%d",
            i, tmp_v2->entries[i].dev_id, tmp_v2->entries[i].adie_handle, tmp_v2->entries[i].sample_rate, tmp_v2->entries[i].bits_per_sample, tmp_v2->entries[i].port_id, tmp_v2->entries[i].is_virtual_dev, tmp_v2->entries[i].timing_mode);
         }
       }
       else
       {
         rc = CSD_ENOTFOUND;
       }
       csd_lock_leave(dev_data.mutex);
       break;
    }
    default:
      rc = CSD_EUNSUPPORTED;
      break;
  }
  return rc;
}

int32_t csd_dev_debug_init(struct csd_vtable **vtable)
{
  int32_t rc = CSD_EOK;
  static struct csd_vtable vtbl = { csd_dev_debug_open,
                                    csd_dev_debug_close,
                                    NULL,
                                    NULL,
                                    csd_dev_debug_ioctl };

  *vtable = &vtbl;
  return rc;
}

int32_t csd_dev_debug_dinit(void)
{
  return CSD_EOK;
}

