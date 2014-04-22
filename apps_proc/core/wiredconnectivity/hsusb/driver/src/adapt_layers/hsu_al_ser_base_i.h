#ifndef _HSU_AL_SER_BASE_I_H_
#define _HSU_AL_SER_BASE_I_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
"Base Class" Internal Header File

GENERAL DESCRIPTION
Contains common API for any serial I/F extending the "base class".
The base class header file or client of the HS-USB serial layer should not 
include this file.

EXTERNALIZED FUNCTIONS
hsu_al_ser_base_i_uninit
hsu_al_ser_base_i_enabled
hsu_al_ser_base_i_disabled
hsu_al_ser_base_i_resumed
hsu_al_ser_base_i_rx_done
hsu_al_ser_base_i_rx_done_from_flow_enable
hsu_al_ser_base_i_rx_setup_read
hsu_al_ser_base_i_tx_done
hsu_al_ser_base_i_ctx_is_valid
hsu_al_ser_base_i_close_clear_data
hsu_al_ser_base_i_open_set_data
hsu_al_ser_base_i_internal_close
hsu_al_ser_base_i_has_already_been_closed


Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_base_i.h#1 $
  $DateTime: 2020/01/30 22:49:35 $


when     who  what, where, why
-------- ---  ---------------------------------------------------------
07/30/06 esh  First revision of file  
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "jtypes_int.h"
#include "hsu_al_ser_base_int.h"

#include "uw_device_cdc.h"

/*==============================================================================
Constants and Macros
==============================================================================*/
/*===========================================================================

FUNCTION hsu_al_ser_base_i_ctx_is_valid

DESCRIPTION
Checks if the received context, typically received when a callback is invoked 
by the CDC FD, is a valid hsu_al_ser_base_data_for_ser_if and not a bad pointer 
cast.

DEPENDENCIES
None

RETURN VALUE
TRUE - context is a valid hsu_al_ser_base_data_for_ser_if. FALSE - Otherwise.

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_i_ctx_is_valid
(
  const hsu_al_ser_base_data_for_ser_if* ctx         
                                      /* Pointer to serial I/F data struct,
                                      as the context to be tested          */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_uninit

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized, or as an initializer for the given context when the very first 
hsu_al_ser_base_init is called.

DEPENDENCIES
See cdc_uninit_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
The data structure corresponding to the given context is invalidated.

===========================================================================*/
jresult_t hsu_al_ser_base_i_uninit
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_rx_done

DESCRIPTION
Rx Callback whenever the serial CDC FD wishes to notify a buffer has been 
received over the data class I/F.

PARAMETERS
status                  : Operation status
buffers                 : Ptr to buffer used
bytes_transferred       : Buffer size in bytes
req_num                 : Number of requests
ctx                     : Context passed in callback This is actuallp pointer 
                          to serial I/F data struct

DEPENDENCIES
See cdc_read_complete_cb for more information

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_i_rx_done
(
  cdc_iostatus_t       *status,
  void                **buffers,
  juint32_t            *bytes_transferred,
  juint32_t             req_num,
  cdc_appctx_t          ctx
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_tx_done

DESCRIPTION
Tx Callback whenever the serial CDC FD wishes to notify a buffer has been 
sent over the data class I/F.

PARAMETERS
status                  : Operation status
buffers                 : Ptr to buffer used
bytes_transferred       : Buffer size in bytes
req_num                 : Number of requests
ctx                     : Context passed in callback This is actuallp pointer 
                          to serial I/F data struct

DEPENDENCIES
See cdc_write_complete_cb for more information

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_base_i_tx_done
(
  cdc_iostatus_t       *status,
  void                **buffers, 
  juint32_t            *buf_sizes,
  juint32_t             req_num,
  cdc_appctx_t          ctx
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_enabled

DESCRIPTION
Enabled callback whenever the serial CDC FD wishes to notify that the interface
is enabled.

DEPENDENCIES
See cdc_enabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_base_i_enabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_disabled

DESCRIPTION
Disabled callback whenever the serial CDC FD wishes to notify that the interface
is disabled.

DEPENDENCIES
See cdc_disabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_base_i_disabled
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_resumed

DESCRIPTION
Callback invoked whenever the serial CDC FD wishes to notify that the interface
can resume operations.

DEPENDENCIES
See cdc_resumed_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_base_i_resumed
(
  cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_internal_close

DESCRIPTION
Reverts various fields of the given data for serial I/F struct to default values,
and calls the app's close callback.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_base_i_internal_close
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
  /* Serial I/F data struct used in operation */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_has_already_been_closed

DESCRIPTION
Checks if this serial I/F data struct has already been closed by this layer. 
This means that the client's close cb had been called before, and therefore
had set the field stream_id to SIO_NO_STREAM_ID.

DEPENDENCIES
None

RETURN VALUE
TRUE - Close has been issued. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_base_i_has_already_been_closed
(
  hsu_al_ser_base_data_for_ser_if* ser_if_data_ptr
  /* Serial I/F data struct used in operation */
);

/*===========================================================================

FUNCTION hsu_al_ser_base_i_rx_done_from_flow_enable

DESCRIPTION
Rx Callback whenever the serial CDC FD wishes to notify a buffer has been 
received over the data class I/F when Rx flow has been enabled.

PARAMETERS
status                  : Operation status
buffers                 : Ptr to buffer used
bytes_transferred       : Buffer size in bytes
req_num                 : Number of requests
ctx                     : Context passed in callback This is actuallp pointer 
                          to serial I/F data struct

DEPENDENCIES
See cdc_read_complete_cb for more information

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_base_i_rx_done_from_flow_enable
(
  cdc_iostatus_t       *status,
  void                **buffers,
  juint32_t            *bytes_transferred,
  juint32_t             req_num,
  cdc_appctx_t          ctx
);

/*==============================================================================

FUNCTION hsu_al_ser_base_i_rx_setup_read

DESCRIPTION
Tx Callback whenever the serial CDC FD wishes to notify a buffer has been 
sent over the data class I/F.

PARAMETERS
status                  : Operation status
buffers                 : Ptr to buffer used
bytes_transferred       : Buffer size in bytes
req_num                 : Number of requests
ctx                     : Context passed in callback

DEPENDENCIES
See cdc_read_complete_cb for more information

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_base_i_rx_setup_read
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);


/*==============================================================================

FUNCTION hsu_al_ser_base_i_close_clear_data

DESCRIPTION
Clear various fields in the given ser_if_data_ptr common in close operations. 
In close operation, the specified params shall have the default values.

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct 

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_base_i_close_clear_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);

/*==============================================================================

FUNCTION hsu_al_ser_base_i_open_set_data

DESCRIPTION
Sets various fields in the given ser_if_data_ptr common in open operations. 
In close operation, the specified params shall have the default values.

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct 
stream_id               : SIO stream id
stream_mode             : SIO steam mode
tx_wmq_ptr              : Pointer to Tx watermark queue
rx_wmq_ptr              : Pointer to Rx watermark queue
app_rx_cb_func          : Application Rx callback function that will get
                          invoked when a Rx transfer is complete
dsm_pool_id             : DSM pool id that the application uses
rx_buff_size_for_read   : Rx buffer size that should be allocated for read
                          Internally, DSM items will be allocated totally up to
                          this size
rx_timeout              : Rx timeout
tx_max_pending_chain_cnt: Maximum HS-USB HW queue pending Tx items
tx_timeout              : Tx timeout
tx_complete_timer_val   : Tx complete timer value, this value is only used if 
                          tx_timeout is set to 0
rx_flow_ctl             : SIO Rx flow control
tx_flow_ctl             : SIO Tx flow control
arg_cb                  : Callbackk argument

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_base_i_open_set_data
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr, 
  sio_port_id_type      port_id,
  sio_stream_id_type    stream_id,  
  sio_stream_mode_type  stream_mode,
  dsm_watermark_type   *tx_wmq_ptr,
  dsm_watermark_type   *rx_wmq_ptr,
  sio_rx_func_ptr_type  app_rx_cb_func,
  dsm_mempool_id_enum_type dsm_pool_id,
  juint8_t              rx_buff_cnt,
  uint32                rx_timeout,
  uint32                tx_max_pending_chain_cnt,
  uint32                tx_timeout,
  uint32                tx_complete_timer_val,
  sio_flow_ctl_type     rx_flow_ctl,
  sio_flow_ctl_type     tx_flow_ctl,
  void                 *arg_cb
);

/*===========================================================================

FUNCTION      hsu_al_ser_acm_pool_many_cb

DESCRIPTION
This function invokes the appropriate actions when the memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_base_pool_many_cb
(
  dsm_mempool_id_enum_type pool_id, 
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);

/*==============================================================================
FUNCTION hsu_al_ser_i_base_tx_set_handle_complete_timer

DESCRIPTION
This function sets the handle complete timer if needed

PARAMETERS
ser_if_data_ptr         : Pointer to serial I/F data struct

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
==============================================================================*/
void hsu_al_ser_i_base_tx_set_handle_complete_timer
(
  hsu_al_ser_base_data_for_ser_if *ser_if_data_ptr
);

#endif /* _HSU_AL_SER_BASE_I_H_ */
