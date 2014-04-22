/*==============================================================================

QAPIs for User-Space applications

GENERAL DESCRIPTION
This file opens a data channel for the client (user space).
It consists of pure USB data transfers without any protocol.


EXTERNALIZED FUNCTIONS
qapi_USB_Handler
qapi_USB_Open
qapi_USB_Read
qapi_USB_Write
qapi_USB_Ioctl


INITALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2017, 2020  by Qualcomm Technologies, Inc.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/qapi_usb.c#5 $
$DateTime: 2023/03/03 02:43:29 $ $Author: pwbldsvc $
2009/01/13 04:17:02 $ $Author: pwbldsvc $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
09/18/20  cs      Added pointer validation check in QAPI handler instead of actual QAPI.
05/18/20  cs      Changes for FR59756: [DAM module containment] QAPI support for unprivileged module
08/18/17  ay      Disconnect notification to client, new TX.3.0 framework changes
01/25/17  ay      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/

#include "qapi_usb_internal.h"
#include "qapi_usb.h"

#ifdef QAPI_TXM_SOURCE_CODE

#include <tx_api.h>
#include <qurt_txm_qapi_fwk.h>

#include <sio.h>
#include "dsm.h"
#include "hsu_al_ser.h"
#include "hsu_al_ser_i.h"
#include <stringl/stringl.h>
#include "hsu_common_int.h"
#include "hsu_log.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/


/*==============================================================================
Constants and Macros
==============================================================================*/

#define HSU_AL_QAPI_STREAM_ID (1)

/*==============================================================================
Typedefs
=============================================================================*/


typedef struct
{
  qapi_USB_App_Rx_Cb_t  qapi_app_rx_cb_func_ptr; // Client rx cb func
  qapi_USB_App_Disconnect_Cb_t qapi_app_disconnect_cb_func_ptr; // Client disconnect cb func
  dsm_item_type * rx_dsm_ptr;      // DSM item from HW
  sio_open_type   sio_open_ctx;    // open context
  dsm_watermark_type tx_wmq;
  q_type q_item;
  q_type rx_pending_queue;
  q_type * rx_pending_queue_ptr;
  dword lock;
  boolean dte_ready;
  sio_ioctl_param_type ioctl_param;
  boolean is_ser3_open;  // If ser3 channel is opened or not
} hsu_al_qapi_ctx_type;


/*==============================================================================
Variables
==============================================================================*/

static hsu_al_qapi_ctx_type hsu_al_qapi_ctx;

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

#ifdef FEATURE_DSM_WM_CB
void qapi_lo_wm_cb
(
  dsm_watermark_type * ignore,
  void * port_id
)
{
  HSU_ULOG (NO_MSG, DATA_LOG, "low_WM_hit");
};

void qapi_hi_wm_cb
(
  dsm_watermark_type * ignore,
  void * port_id
)
{
  HSU_ULOG (NO_MSG, DATA_LOG, "hi_WM_hit");
};
#else
void qapi_lo_wm_cb
(
  void
)
{
  HSU_ULOG (NO_MSG, DATA_LOG, "low_WM_hit");
};

void qapi_hi_wm_cb
(
  void
)
{
  HSU_ULOG (NO_MSG, DATA_LOG, "hi_WM_hit");
};
#endif


/* Callback function received on any dtr event */
void hsu_al_qapi_dtr_event_cb (void)
{
  hsu_al_qapi_ctx.ioctl_param.dte_ready_asserted = &(hsu_al_qapi_ctx.dte_ready);

  hsu_al_ser_ioctl (HSU_AL_QAPI_STREAM_ID, AL_SER_PORT_USB_SER3, SIO_IOCTL_DTE_READY_ASSERTED, 
    &(hsu_al_qapi_ctx.ioctl_param));

  if (hsu_al_qapi_ctx.dte_ready == FALSE)
  {
    HSU_ULOG (NO_MSG, BUS_LOG, "disconnect event");

    //delete the pending dsm items
    while (hsu_al_qapi_ctx.rx_dsm_ptr != NULL)
    {
      dsm_free_packet (&(hsu_al_qapi_ctx.rx_dsm_ptr));
      hsu_al_qapi_ctx.rx_dsm_ptr = (dsm_item_type *) q_get (hsu_al_qapi_ctx.rx_pending_queue_ptr);
    }

    // Call the client disconnect cb
    if (hsu_al_qapi_ctx.qapi_app_disconnect_cb_func_ptr != NULL)
    {
      hsu_al_qapi_ctx.qapi_app_disconnect_cb_func_ptr();
    }
  }

  return;
}


/* Takes the dsm item from HW and then calls the client rx cb func
   notifying that data is available                               */
void hsu_al_qapi_app_rx_cb_func (dsm_item_type ** rx_ptr)
{
  if ((rx_ptr == NULL) || ((*rx_ptr) == NULL))
  {
    HSU_ULOG (NO_MSG, DATA_LOG, "received NULL DSM");
    return;
  }

  jsafe_enter();

  if (hsu_al_qapi_ctx.qapi_app_rx_cb_func_ptr == NULL)
  {
    HSU_ULOG (NO_MSG, DATA_LOG, "qapi_app_cb_func not registered, dropping the DSM");
    dsm_free_packet (rx_ptr);
    jsafe_leave();
    return;
  }

  // rx_dsm_ptr is NULL means there is no previous data here from the HW, so take the received DSM
  if (hsu_al_qapi_ctx.rx_dsm_ptr == NULL)
  {
    //storing the DSM in QAPI structure
    hsu_al_qapi_ctx.rx_dsm_ptr = *rx_ptr;
  }
  else //queue the received DSM
  {
    q_put ((hsu_al_qapi_ctx.rx_pending_queue_ptr), &((*rx_ptr)->link));
  }

  hsu_al_qapi_ctx.qapi_app_rx_cb_func_ptr();

  jsafe_leave();
  return;  
}


/*==============================================================================

FUNCTION      qapi_USB_Handler

DESCRIPTION
This function is used to register the USB API's with the application layer.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==============================================================================*/
UINT qapi_USB_Handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e,
						UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l)	
{
  qapi_USB_Status_t retval = QAPI_ERROR;
  void* manager_cb = NULL;

  switch(id)
  {
    case TXM_QAPI_USB_OPEN:
      retval = qapi_USB_Open();
      break;
    case TXM_QAPI_USB_READ:
      retval = qapi_USB_Read((void **)a, (uint16)b);
      break;
    case TXM_QAPI_USB_WRITE:
      retval = qapi_USB_Write((void *)a, (uint16)b);
      break;
    case TXM_QAPI_USB_IOCTL:
      if (qurt_check_if_module_address((qurt_addr_t)b, sizeof(b)) == FALSE)
	  {
		HSU_ULOG (ERROR_MSG, ENG_LOG, "Address doesn't belong to current module");
		retval = QAPI_ERR_INVALID_PARAM;
		break;
	  }
	  if ((qapi_USB_Ioctl_Cmd_t)a == QAPI_USB_RX_CB_REG_E)
      {
        if (!qurt_qapi_callback_configure(TXM_QAPI_USB_RX_CB, // type
                     (void*)0xDEADCAFE, // dummy obj_ptr
                     0,	// id 
                     (void *)b, // example_api1 - a2: app_cb
                     (void *)c, // example_api1 - a3: app_cb_dispatcher
                     (void**)&manager_cb,
                     NULL))
        {
          if (manager_cb == NULL)
          {
            retval = QAPI_ERROR;
            break;
          }
        }
      }
      else if ((qapi_USB_Ioctl_Cmd_t)a == QAPI_USB_DISCONNECT_CB_REG_E)
      {
        if (!qurt_qapi_callback_configure(TXM_QAPI_USB_DISC_CB, // type
                     (void*)0xDEADCAFE, // dummy obj_ptr
                     0,	// id 
                     (void *)b, // example_api1 - a2: app_cb
                     (void *)c, // example_api1 - a3: app_cb_dispatcher
                     (void**)&manager_cb,
                     NULL))
        {
          if (manager_cb == NULL)
          {
            retval = QAPI_ERROR;
            break;
          }
        }
      }
      retval = qapi_USB_Ioctl((qapi_USB_Ioctl_Cmd_t)a, (void *)manager_cb);
      break;
    default:
      break;
  }
  return retval;
}


/*==============================================================================

FUNCTION      qapi_USB_Open

DESCRIPTION   
Called by the application layer to open the ser3 interface.

INPUT PARAMETERS
None.

DEPENDENCIES  
None.

RETURN VALUE  
0 if success else 1.

SIDE EFFECTS  
None.
==============================================================================*/
qapi_USB_Status_t qapi_USB_Open
(
  void
)
{
  qapi_USB_Status_t status = QAPI_ERROR;
  sio_status_type  status_ser_open = SIO_DONE_S;

  jsafe_enter();

  if (hsu_al_qapi_ctx.sio_open_ctx.port_id == AL_SER_PORT_USB_SER3)
  {
    HSU_ULOG (NO_MSG, DATA_LOG, "ser3_channel_already_open");
    jsafe_leave();
    return QAPI_ERR_ALREADY_DONE;
  }

  //TRUE- When called by ser_obex_disabled
  //FALSE- When called by ser_obex_enabled
  hsu_al_qapi_ctx.dte_ready= FALSE;

  hsu_al_qapi_ctx.tx_wmq.lock = hsu_al_qapi_ctx.lock;
  dsm_queue_init (&(hsu_al_qapi_ctx.tx_wmq), (int)hsu_al_qapi_ctx.tx_wmq.dont_exceed_cnt, &(hsu_al_qapi_ctx.q_item));

  // Initialize the Tx watermark queue
  hsu_al_qapi_ctx.tx_wmq.current_cnt               = 0;
  hsu_al_qapi_ctx.tx_wmq.hi_watermark              = 0x3C00; // DSM_LARGE_ITEM_SIZE*15
  hsu_al_qapi_ctx.tx_wmq.lo_watermark              = 0x2000; // DSM_LARGE_ITEM_SIZE*8
  hsu_al_qapi_ctx.tx_wmq.dont_exceed_cnt           = 0x4000; // DSM_LARGE_ITEM_SIZE*16
  hsu_al_qapi_ctx.tx_wmq.dne_q_cnt                 = 0x10;   // Max 16 DSM items can be queued
  hsu_al_qapi_ctx.tx_wmq.gone_empty_func_ptr       = NULL;
  hsu_al_qapi_ctx.tx_wmq.non_empty_func_ptr        = NULL;
  hsu_al_qapi_ctx.tx_wmq.lowater_func_ptr          = qapi_lo_wm_cb;
  hsu_al_qapi_ctx.tx_wmq.hiwater_func_ptr          = qapi_hi_wm_cb;
  hsu_al_qapi_ctx.tx_wmq.lowater_func_data         = &(hsu_al_qapi_ctx.sio_open_ctx.port_id);
  hsu_al_qapi_ctx.tx_wmq.hiwater_func_data         = &(hsu_al_qapi_ctx.sio_open_ctx.port_id);

  hsu_al_qapi_ctx.sio_open_ctx.port_id      = AL_SER_PORT_USB_SER3;
  hsu_al_qapi_ctx.sio_open_ctx.stream_id    = HSU_AL_QAPI_STREAM_ID;
  hsu_al_qapi_ctx.sio_open_ctx.rx_func_ptr  = hsu_al_qapi_app_rx_cb_func;
  hsu_al_qapi_ctx.sio_open_ctx.tx_queue     = &(hsu_al_qapi_ctx.tx_wmq);

  status_ser_open = hsu_al_ser_open (&(hsu_al_qapi_ctx.sio_open_ctx));

  if(status_ser_open == SIO_DONE_S)
  {
    status = QAPI_OK;
  }
  else
  {
    HSU_ULOG_1 (NO_MSG, REG_LOG, "ser open failed", status_ser_open);
    jsafe_leave();
    return status;
  }

  // Init the rx_pending_queue
  hsu_al_qapi_ctx.rx_pending_queue_ptr = q_init (&(hsu_al_qapi_ctx.rx_pending_queue));

  hsu_al_qapi_ctx.is_ser3_open = TRUE;

  hsu_al_qapi_ctx.ioctl_param.enable_dte_ready_event = (sio_vv_func_ptr_type)hsu_al_qapi_dtr_event_cb;

  jsafe_leave();

  hsu_al_ser_ioctl (HSU_AL_QAPI_STREAM_ID, AL_SER_PORT_USB_SER3, SIO_IOCTL_ENABLE_DTR_EVENT, 
    &(hsu_al_qapi_ctx.ioctl_param));

  return status;
}


/*==============================================================================

FUNCTION      qapi_USB_Read

DESCRIPTION   
Called by the user application when it wishes to read some data.

INPUT PARAMETERS
Pointer to pointer of buffer where data is to be written.
Max size of data that the above buffer can take.

DEPENDENCIES  
None.

RETURN VALUE  
number of bytes of data given to client

SIDE EFFECTS  
None.
==============================================================================*/
qapi_USB_Status_t qapi_USB_Read
(
  void ** buffer,
  uint16 max_size
)
{
  uint16 bytes_to_send = 0;
  qapi_USB_Status_t status = QAPI_OK;

  if (hsu_al_qapi_ctx.is_ser3_open == FALSE)
  {
    HSU_ULOG (ERROR_MSG, DBG_E_LOG, "User called read without opening the channel");
    return QAPI_ERR_NOT_SUPPORTED;
  }

  HSU_ASSERT ((buffer != NULL) && ((*buffer) != NULL));

  jsafe_enter();

  if (hsu_al_qapi_ctx.rx_dsm_ptr == NULL)
  {
    HSU_ULOG (NO_MSG, DATA_LOG, "no_data");

    jsafe_leave();
    return QAPI_ERR_NO_DATA;
  }

  bytes_to_send = hsu_al_qapi_ctx.rx_dsm_ptr->used;

  HSU_ASSERT (max_size >= bytes_to_send);

  if (qurt_copy_to_user ((qurt_addr_t*)*buffer, (qurt_size_t)bytes_to_send, (qurt_addr_t*)hsu_al_qapi_ctx.rx_dsm_ptr->data_ptr, (qurt_size_t)bytes_to_send) != QURT_EOK)
  {
    HSU_ULOG_1 (ERROR_MSG, DATA_LOG, "unable to send data to user %d bytes", bytes_to_send);
	status = QAPI_ERROR;
  }

  dsm_free_packet (&(hsu_al_qapi_ctx.rx_dsm_ptr));

  hsu_al_qapi_ctx.rx_dsm_ptr = (dsm_item_type *) q_get (hsu_al_qapi_ctx.rx_pending_queue_ptr);

  jsafe_leave();

  return status;
}


/*==============================================================================

FUNCTION      qapi_USB_Write

DESCRIPTION   
Called by the client when it wishes to send some data over USB.

INPUT PARAMETERS
Pointer to data
Length of data

DEPENDENCIES  
None.

RETURN VALUE  
Success 0
Failure -1

SIDE EFFECTS  
None.
==============================================================================*/
qapi_USB_Status_t qapi_USB_Write
(
  void * data_ptr,
  uint16 len
)
{
  dsm_item_type* tx_ptr = NULL;
  qapi_USB_Status_t status = QAPI_OK;

  if (hsu_al_qapi_ctx.is_ser3_open == FALSE)
  {
    HSU_ULOG (ERROR_MSG, ENG_LOG, "User called write without opening the channel");
    return QAPI_ERR_NOT_SUPPORTED;
  }

  HSU_ASSERT (data_ptr != NULL);

  tx_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);

  if ((tx_ptr == NULL) || (len > tx_ptr->size))
  {
    status = QAPI_ERR_NO_MEMORY;
    HSU_ERR_FATAL ("buffer_allocation_failed or received_buffer_length_more_than_max_size", len, 0, 0);
  }
  else
  {
    if (qurt_copy_from_user ((qurt_addr_t*)tx_ptr->data_ptr, (qurt_size_t)len, (qurt_addr_t*)data_ptr, (qurt_size_t)len) != QURT_EOK)
    {
      HSU_ULOG_1 (ERROR_MSG, DATA_LOG, "unable to copy data from user %d bytes", len);
	  status = QAPI_ERROR;
    }

    tx_ptr->used = len;
    hsu_al_ser_transmit (HSU_AL_QAPI_STREAM_ID, AL_SER_PORT_USB_SER3, tx_ptr);
  }

  return status;
}

/*==============================================================================

FUNCTION      qapi_USB_Ioctl

DESCRIPTION   
Used to register application's cb func for read.

INPUT PARAMETERS
Pointer to the cb func.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
qapi_USB_Status_t qapi_USB_Ioctl
(
  qapi_USB_Ioctl_Cmd_t cmd,
  void * param
)
{
  qapi_USB_Status_t status = QAPI_ERROR;

  if (hsu_al_qapi_ctx.is_ser3_open == FALSE)
  {
    HSU_ULOG (ERROR_MSG, ENG_LOG, "User called ioctl without opening the channel");
    return QAPI_ERR_NOT_SUPPORTED;
  }

  HSU_PARAM_COULD_BE_CONST(param);

  switch (cmd)
  {
    case QAPI_USB_RX_CB_REG_E:
  	{
      HSU_ASSERT (param != NULL);
  	  hsu_al_qapi_ctx.qapi_app_rx_cb_func_ptr = (qapi_USB_App_Rx_Cb_t)param;
      HSU_ASSERT (hsu_al_qapi_ctx.qapi_app_rx_cb_func_ptr);
      status = QAPI_OK;
  	}
    break;
    
    case QAPI_USB_DISCONNECT_CB_REG_E:
    {
      HSU_ASSERT (param != NULL);
      hsu_al_qapi_ctx.qapi_app_disconnect_cb_func_ptr = (qapi_USB_App_Disconnect_Cb_t)param;
      HSU_ASSERT (hsu_al_qapi_ctx.qapi_app_disconnect_cb_func_ptr);
      status = QAPI_OK;
    }
    break;
    
    default:
    {
      HSU_MSG_ERROR_1("hsu_al_qapi_ioctl(): cmd %d not supported!", cmd);
      status = QAPI_ERR_INVALID_PARAM;
      break;
    }
  }

  return status;
};

#endif


