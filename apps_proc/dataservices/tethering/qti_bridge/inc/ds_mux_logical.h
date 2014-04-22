#ifndef DS_MUX_LOGICAL_H
#define DS_MUX_LOGICAL_H
/*===========================================================================

                                 D S   M U X   L O G I C A L

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX Task. This file
  contains all the functions, definitions and data types needed for other
  tasks to interface to the MUX Ports over SIO.

  Copyright (c) 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux_logical.h#1 $
  $DateTime: 2019/11/26 00:18:17 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux_defs.h"
#include "ds_mux_phys.h"
#include "ds_mux_io.h"

#ifndef MOB_TX
#include "rdevmap.h"
#else
#include "rdevmap_mob.h"
#endif

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define DS_MUX_BASE_LOGICAL_RDM_DEVICE      RDM_MUX_DEV_1
#define DS_MUX_LOGICAL_SERVICE              RDM_DATA_SRVC
#define DS_MUX_MAX_RDM_OPR_RETRY            10  //TBD
#define DS_MUX_RDM_OPR_RETRY_TIMER          100 //TBD

typedef enum
{
  DS_MUX_LOGICAL_RDM_OPRT_MIN     = 0,
  DS_MUX_LOGICAL_RDM_ASSIGN_PORT  = 1,
  DS_MUX_LOGICAL_RDM_CLOSE_PORT   = 2,
}ds_mux_logical_rdm_oprt_enum_type;

typedef enum
{
  DS_MUX_LOGICAL_PORT_MIN       = 0,
  DS_MUX_LOGICAL_PORT_CLOSED    = DS_MUX_LOGICAL_PORT_MIN,
  DS_MUX_LOGICAL_PORT_ASSIGN    = 1,
  DS_MUX_LOGICAL_PORT_ASSIGNED  = 2,
  DS_MUX_LOGICAL_PORT_OPEN      = 3,
  DS_MUX_LOGICAL_PORT_CONNECTED = 4,
  DS_MUX_LOGICAL_PORT_CLOSING   = 5,

}ds_mux_logical_port_status_enum_type;


/*--------------------------------------------------------------------------
   logical connection control block: Data types to hold logical SIO connection info  
 ---------------------------------------------------------------------------*/
typedef struct
{
  struct ds_mux_logical_conn_info_type
  {
    uint8                                 rdm_oprt_retry;
    /* retry RDM operation*/
    qurt_timer_t                          timer_handle;
    qurt_timer_attr_t                     timer_attr;
    /* retry  timer*/
    ds_mux_logical_port_status_enum_type  port_status;
    rdm_device_enum_type                  rdm_device;
    rdm_service_enum_type                 service;
    sio_port_id_type                      port_id;
    /* SIO Port*/
    uint8                                 dlci_id;
    /* DLCI ID*/
    dsm_watermark_type                   *rx_queue;           
    /* Received Data Queue.         */
    dsm_watermark_type                   *tx_queue;           
    /* Transmit Data Queue.         */
    void                                  (*close_cb_ptr)(void);
    /* Function to call when  close is complete. */
    
    struct 
    {
      sio_ioctl_enable_dtr_event_ext_type dtr_evt_info;
      sio_vv_func_ptr_type                dtr_backward;
      sio_ioctl_enable_cb_event_ext_type  cd_evt_info;
      sio_ioctl_enable_ri_event_ext_type  ri_evt_info;

      boolean                             in_bound_flow_disabled;
      boolean                             rtc_asserted;
      boolean                             cd_asserted;
      boolean                             dte_ready_asserted;
      boolean                             ri_asserted;
      struct 
      {
        boolean                             data_valid;
        boolean                             rtc;
        boolean                             fc;
        boolean                             ic;
      }remote_sig_state;
      
    } ioctl_data;

    
  }logical_cons[DS_MUX_MAX_LOGICAL_CONNECTION];
  
}ds_mux_logical_sio_iface_info_type;



/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

void ds_mux_logical_init(void);

void ds_mux_logical_notify_port_open(void);


uint32 ds_mux_logicali_driver_init
( 
  sio_register_device_driver_cb_type register_driver_cb_fn
);

ds_mux_result_enum_type ds_mux_logical_process_rx_data_frames
(
  ds_mux_io_frame_type    *frame
);

void ds_mux_logical_notify_disconneted_dlci 
(
  uint8                   dlci_id
);


void ds_mux_logical_notify_conneted_dlci 
(
  uint8                   dlci_id
);


dsm_watermark_type*      ds_mux_logical_get_tx_wmk
(
  uint8                   dlci_id
);

dsm_watermark_type*      ds_mux_logical_get_rx_wmk
(
  uint8                   dlci_id
);

dsm_item_type* ds_mux_logical_get_dsm_item_from_wmk
(
  uint8                   dlci_id
);


ds_mux_result_enum_type ds_mux_logical_input_rx_data
(
  uint8                  dlci_id,
  dsm_item_type         *item_ptr
);

ds_mux_result_enum_type  ds_mux_logical_process_v24_signal
(
  uint8                  dlci_id,
  uint8                  v24_sig
);



#endif /* DS_MUX_LOGICAL_H */
