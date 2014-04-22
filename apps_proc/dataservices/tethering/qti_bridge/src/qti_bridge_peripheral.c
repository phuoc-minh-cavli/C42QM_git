/*===========================================================================
                         QTI_BRIDGE_PERIPHERAL.c

DESCRIPTION
  This  file contains definitions used  by the QTI AT DUN  Module.

  Copyright (c)2016 , 2021 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

#include "qti_bridge.h"
#ifdef QTI_LOGGING_ENABLED
#include "ULogFront.h"
#endif
#include "ds_log.h"


extern qti_bridge_info_t qti_bridge_info_table[QTI_BRIDGES_MAX];
extern qti_bridge_qurt_t  qti_bridge_qurt;
#ifdef QTI_LOGGING_ENABLED
extern ULogHandle qti_ulog;
#endif

void qti_bridge_enable_phys_0(dsm_watermark_type *ignore,
                                         void *bridge_id)
{
  qti_bridge_enable_phys(ignore, QTI_BRIDGE_INSTANCE_0);
}

void qti_bridge_enable_phys_1(dsm_watermark_type *ignore,
                                         void *bridge_id)
{
  qti_bridge_enable_phys(ignore, QTI_BRIDGE_INSTANCE_1);
}

void qti_bridge_disable_phys_0(dsm_watermark_type *ignore,
                                          void *bridge_id)
{
  qti_bridge_disable_phys(ignore, QTI_BRIDGE_INSTANCE_0);
}

void qti_bridge_disable_phys_1(dsm_watermark_type *ignore,
                                          void *bridge_id)
{
  qti_bridge_disable_phys(ignore, QTI_BRIDGE_INSTANCE_1);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_FORWARD_PHYS_TO_QTI

DESCRIPTION
  Receives a dsm packet from  the physical device  and forwards it to QTI

DEPENDENCIES
  None.

PARAMETER
  The dsm item to be forwarded
  Bridge_id - bridge id of the port
  
RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

static void qti_bridge_recv_phys_to_qti
(
  dsm_item_type      **item,
  uint32             bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "p2q");
#endif
 
  ds_iot_log_high ("qti_bridge_recv_phys_to_qti() " );

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_PHY_QTI_RECV_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;
    cmd_ptr->data.dsm_item_ptr     = *item;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_FORWARD_PHYS_TO_SMD

DESCRIPTION
  Register callbacks for each bridge channel for communication from Physical
  interconnect to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_recv_phys_to_qti_0 (dsm_item_type **item)
{
  qti_bridge_recv_phys_to_qti(item, QTI_BRIDGE_INSTANCE_0);
}

/*==============================================================================

FUNCTION qti_bridge_recv_phys_to_qti_1

DESCRIPTION
  Register callbacks for each bridge channel for communication from Physical
  interconnect to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_recv_phys_to_qti_1 (dsm_item_type **item)
{
  qti_bridge_recv_phys_to_qti(item, QTI_BRIDGE_INSTANCE_1);
}


/*==============================================================================

FUNCTION QTI_BRIDGE_HANDLE_PHYS_QTI_DTR

DESCRIPTION
  Handles the DTR Callbacks

DEPENDENCIES
  None.

PARAMETER
  Bridge_id - bridge id of the port

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

static void qti_bridge_handle_phys_qti_dtr (uint32 bridge_id)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "pqdtr");
#endif
  ds_iot_log_high ("qti_bridge_handle_phys_qti_dtr() " );

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_PHYS_QTI_DTR_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}


/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_PHYS_TO_QTI_0

DESCRIPTION
  Register callbacks for each bridge channel for handling DTR status changes

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_dtr_phys_to_qti_0(void)
{
  qti_bridge_handle_phys_qti_dtr(QTI_BRIDGE_INSTANCE_0);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_PHYS_TO_QTI_1

DESCRIPTION
  Register callbacks for each bridge channel for handling DTR status changes

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_dtr_phys_to_qti_1(void)
{
  qti_bridge_handle_phys_qti_dtr(QTI_BRIDGE_INSTANCE_1);
}

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_PHYS

DESCRIPTION
  dsm callback that enables the physical port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_enable_phys
(
  dsm_watermark_type *ignore, 
  int              bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

#ifdef QTI_LOGGING_ENABLED
  ULogFront_RealTimePrintf(qti_ulog, 0, "ep");
#endif

  ds_iot_log_high1 ("qti_bridge_enable_phys: qti_bridge id:%d",bridge_id);

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_PHY_LOW_WM_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_PHYS

DESCRIPTION
  dsm callback that disables the physical port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disable_phys
(
  dsm_watermark_type *ignore, 
  int              bridge_id
)
{
  qti_bridge_cmd_t*             cmd_ptr = NULL;

#ifdef QTI_LOGGING_ENABLED  
  ULogFront_RealTimePrintf(qti_ulog, 0, "dp");
#endif
  ds_iot_log_high ("qti_bridge_disable_phys() ");

  if((cmd_ptr = qti_bridge_get_cmd_buffer()) == NULL)
  {
    ds_iot_log_err("Cannot obtain command buffer. Assert");
    ASSERT(0);
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_id        = QTI_BRIDGE_PHY_HIGH_WM_EVT;
    cmd_ptr->data.bridge_id        = bridge_id;

    //Enqueue the command into the command queue.
    qti_bridge_enqueue_cmd(cmd_ptr);
  }

  qti_bridge_set_signal();
  return;
}
