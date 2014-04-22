/*==============================================================================
 @file csd_dev_manager.c

 The objective of this source file is to call into RX/TX device sequences
 to setup/tear down the requested devices.

 Copyright (c) 2008-13, 2016 QUALCOMM Technologies Incorporated.
 All rights reserved.
 Qualcomm Confidential and Proprietary
 ==============================================================================*/
/*===========================================================================
 EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to this module.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/audio.tx/2.0/audio/csd/dev/common/src/csd_dev_manager.c#1 $
 $DateTime: 2018/12/25 22:39:07 $
 $Author: pwbldsvc $
 $Change: 17929171 $
 $Revision: #1 $

 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/15/16   rk      Abstracting OSAL layer
 02/15/13   suns    Device module Unification.
 05/05/10   suns    Initial draft
 ============================================================================*/
#include "csd_dev_manager.h"
#include "csd_debug_msg.h"
#include "DALSys.h"

/* global sequence manager machine structure */
static csd_dev_manager** g_csd_dev_manager[CSD_DEV_TYPE_MAX];

extern int32_t csd_dev_manager_rx_init(csd_dev_manager*** sm_db);
extern int32_t csd_dev_manager_rx_deinit(void);
extern int32_t csd_dev_manager_tx_init(csd_dev_manager*** sm_db);
extern int32_t csd_dev_manager_tx_deinit(void);

int32_t csd_dev_manager_up( csd_dev_record_t* a_dev,
    enum csd_dev_manager_type type)
{
  int32_t rc = CSD_EOK;
  csd_dev_manager* entry = NULL;
  csd_dev_manager** table = g_csd_dev_manager[a_dev->dev_info.direction];

  if( table == NULL || table[type] == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_manager_up: "
            "No table defined for dev id %d, seq type %d",
        a_dev->dev_id, type);
    rc = CSD_EBADPARAM;
  }

  if( CSD_EOK == rc )
  {
    a_dev->manager_seq_index = CSD_DEV_INIT;
    while( a_dev->manager_seq_index < CSD_DEV_STAGE_MAX )
    {
      entry = &(table[type][a_dev->manager_seq_index]);
      if( entry->action )
      {
        rc = entry->action(a_dev);
      }
      if( CSD_EOK != rc )
      {
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_manager_up: "
            "Failed to bring up dev (%d) at stage (%d)",
            a_dev->dev_id,entry->stage);
        break;
      }
      /*
       * manager_seq_index: holds the current sequence index,
       * All previous index were successfully enabled
       */
      a_dev->manager_seq_index++;
    }
  }

  CSD_MSG(CSD_PRIO_LOW, "csd_dev_manager_up: "
      "Exiting dev=0x%x,final index=0x%x,rc=0x%x,max_stage=0x%x\n",
      a_dev->dev_id, a_dev->manager_seq_index, rc, CSD_DEV_STAGE_MAX);

  return rc;
}

int32_t csd_dev_manager_down( csd_dev_record_t* a_dev,
    enum csd_dev_manager_type type)
{
  int32_t rc = CSD_EOK;
  int32_t temp_rc = CSD_EOK;
  csd_dev_manager* entry = NULL;
  csd_dev_manager** table = g_csd_dev_manager[a_dev->dev_info.direction];

  if( table == NULL || table[type] == NULL )
  {
    CSD_MSG(CSD_PRIO_ERROR, "csd_dev_manager_down: "
        "Table defined for dev id %d, seq type %d",
        a_dev->dev_id, type);
    rc = CSD_EBADPARAM;
  }

  if( rc == CSD_EOK)
  {
    if(a_dev->manager_seq_index >= CSD_DEV_STAGE_MAX)
    {
      /* Complete tear down */
      a_dev->manager_seq_index = CSD_DEV_STAGE_MAX - 1;
      CSD_MSG(CSD_PRIO_LOW, "csd_dev_manager_down: Graceful tear down");
    }
    else
    {
      /* Tearing down only successfully completed stages.
       * NOTE: manager_seq_index points to last attempted stage
       */
      a_dev->manager_seq_index-- ;
      CSD_MSG(CSD_PRIO_HIGH, "csd_dev_manager_down: "
          "Forced/Abrupt Tear down from stage (%d)" ,
          a_dev->manager_seq_index);
    }

    while( a_dev->manager_seq_index >= CSD_DEV_INIT )
    {
      entry = &(table[type][a_dev->manager_seq_index]);
      if( entry->action )
      {
        temp_rc = entry->action(a_dev);
      }
      if( CSD_EOK != temp_rc )
      {
        if(CSD_EOK == rc)
        {
          /* Saving only the first failed rc or avoid success rc re-write */
          rc = temp_rc;
        }
        CSD_MSG(CSD_PRIO_ERROR, "csd_dev_manager_down: "
                "Failed to tear down dev (%d) at stage (%d)"
                "rc=0x%x", a_dev->dev_id, entry->stage,rc);
        /* continue on error */
      }
      a_dev->manager_seq_index--;
    }
  }
  CSD_MSG(CSD_PRIO_LOW, "csd_dev_manager_down: "
          "Exiting - dev=0x%x,final manager_seq_index=0x%x,rc=0x%x\n",
          a_dev->dev_id, a_dev->manager_seq_index, rc);
  return rc;
}

int32_t csd_dev_manager_init(void)
{
  csd_memset(g_csd_dev_manager, 0,
                CSD_DEV_TYPE_MAX*sizeof(csd_dev_manager**));
  csd_dev_manager_rx_init(g_csd_dev_manager);
  csd_dev_manager_tx_init(g_csd_dev_manager);
  return CSD_EOK;
}

int32_t csd_dev_manager_deinit(void)
{
  csd_dev_manager_rx_deinit();
  csd_dev_manager_tx_deinit();
  return CSD_EOK;
}
