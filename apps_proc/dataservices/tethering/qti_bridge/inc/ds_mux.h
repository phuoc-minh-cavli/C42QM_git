#ifndef DS_MUX_H
#define DS_MUX_H
/*===========================================================================

                                 D S   M U X  

                                 H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the MUX Task. This file
  contains all the functions, definitions and data types needed for 
  MUX processing

  DS MUX 27.010 core protocol and logic is implemented in this file

  Copyright (c) 2016,2021 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataservices.tx/6.0/tethering/qti_bridge/inc/ds_mux.h#2 $
  $DateTime: 2021/05/05 04:35:02 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/16   ad      initial version
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
#include "ds_mux_defs.h"
#include "ds_mux_svc.h"
#include "ds_mux_phys.h"
#include "ds_mux_logical.h"
#include "ds_mux_io.h"
#include "qurt_timer.h"
#include "qti_bridge.h"
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/



/*--------------------------------------------------------------------------
   Outstanding commands and messages queue.
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 DLCI control block: Data types to hold dlci config  
---------------------------------------------------------------------------*/

typedef struct
{
   uint8                     dlci_id;
  /**< Self-reference ID   */
  
  uint32                     flow_control_mask;
  /**< Flow control mask   */
  
  ds_mux_dlci_state_enum_type state;
  /**< Currnet connection sate   */
  /*--------------------------------------------------------------------------
    DLCI Parameters, Parameters will be set through PN messages, 
    AT +CMUX command or having default values
  --------------------------------------------------------------------------*/
  ds_mux_subset_enum_type  frame_type;                         
   /**< 0-UIH, 1-UI, 2-I frames.            */
  uint8                    priprity;                    
   /**< The priority is a number in the range 0-63.
      Not supported, All DLCIs will be having same priority*/
  uint16                   frame_size_N1;            
   /**< Max frame size (1-32768).           */
  uint16                   response_timer_T1;            
   /**<  Time UE waits for an acknowledgement before
   resorting to other action (e.g. transmitting a frame)
   default 100ms min value 10 ms.
 */
  uint8                     re_transmissions_N2;      
   /**< Max re-tries N2 (0-255).            */

  uint8                     curr_transmissions_N2; 
   
  uint16                    response_timer_T2;            
   /**< Time mux control channel waits before 
 re-transmitting a command default 300ms and
 min value 20 ms.
 */
  uint32                     wake_up_timer_T3;            
   /**<  Time UE waits for response for a 
 power wake-up command default 10ms and min
 value 1 ms.
 */
  uint8                       window_size_k;                
  /**< Window size default 2, range (1-7). 
     Not supported*/

  ds_mux_cl_type_enum_type    cl_type;
  /**< Convergence layer type*/
  ds_mux_io_frame_struct_type *curr_msg_frame;
  /**< MSG is pending for this message */
  ds_mux_io_frame_struct_type *curr_cmd_frame;
   /**< UA is pending for this command */
  q_type                      outstanding_cmd_frame_q;
  /**< queue is holding outstanding commands pending to be sent */
  q_type                      outstanding_msg_frame_q;
  /**< queue is holding outstanding messages pending to be sent */

  qurt_timer_t     t1_timer_handle;
  /* retry  timer for T1*/
  qurt_timer_t     t2_timer_handle;
  /* retry  timer for T2*/
  qurt_timer_t     t3_timer_handle;
  /* retry  timer for T2*/

  qurt_timer_attr_t t1_timer_attr;

  qurt_timer_attr_t t2_timer_attr;

  qurt_timer_attr_t t3_timer_attr;

  /**< Convergence layer type*/
  dsm_item_type                *pending_rx_cl4_data_ptr;

} ds_mux_dlci_param_type;

typedef struct 
{
  
  ds_mux_state_enum_type       ds_mux_state;
  boolean                      is_initiator;
}ds_mux_state_info_type;

/*---------------------------------------------------------------------------
  Request and Release call back declarations for MUX.
---------------------------------------------------------------------------*/
typedef void (*ds_mux_event_cb_fcn)
(
  ds_mux_ext_client_event_enum_type event_name,     /* Event name  */
  void *                            event_payload,  /* Info assosiated to event: If Any else NULL  */
  void *                            cb_user_data    /* User data specified during registration */
);

/*--------------------------------------------------------------------------
  External client event information.
---------------------------------------------------------------------------*/
typedef struct
{
  ds_mux_event_cb_fcn   event_cb_fn;
  void                 *client_data;
}ds_mux_ext_client_event_info_type;


/*--------------------------------------------------------------------------
  External client event and device einformation.
---------------------------------------------------------------------------*/
typedef struct
{
  sio_stream_id_type                  stream_id;
  ds_mux_ext_client_event_info_type   *event_info;
}ds_mux_ext_client_info_type;


typedef enum
{
  DS_MUXI_MIN_CMD               = -1,
  DS_MUXI_PHY_PORT_OPEN         =  0,
  DS_MUXI_LOGICAL_PORT_OPEN     =  1,
  DS_MUXI_LOGICAL_PORT_CLOSE    =  2,
  DS_MUXI_LOGICAL_PORT_IOCTL    =  3,
  /* Always must be last */
  DS_MUXI_MAX_CMD_ID
} ds_muxi_cmd_id_enum_type;

/*--------------------------------------------------------------------------
  DS MUX  PHY PORT RDM open call back paramters
---------------------------------------------------------------------------*/
typedef struct
{
  sio_port_id_type port_id;
}ds_mux_phys_rdm_open_info_type;

/*--------------------------------------------------------------------------
  DS MUX  PHY PORT RDM open call back paramters
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  dlci_id;
}ds_mux_logical_open_info_type;
/*--------------------------------------------------------------------------
  DS MUX close  paramters
---------------------------------------------------------------------------*/
typedef struct
{
  uint8  dlci_id;
}ds_mux_logical_close_info_type;

/*--------------------------------------------------------------------------
  DS MUX  IOCTL  paramters
---------------------------------------------------------------------------*/
typedef struct
{
  uint8                     dlci_id;
  boolean                   data_valid;
  boolean                   rtc;
  boolean                   fc;
  boolean                   ic;
}ds_mux_logical_ioctl_info_type;


/*--------------------------------------------------------------------------
  DS MUX command info type
---------------------------------------------------------------------------*/
typedef struct
{
  ds_muxi_cmd_id_enum_type    cmd_id;

  union
  {
    ds_mux_phys_rdm_open_info_type    phy_port_open_info;
    ds_mux_logical_open_info_type     logical_port_open_info;
    ds_mux_logical_close_info_type    logical_port_close_info;
    ds_mux_logical_ioctl_info_type    logical_port_ioctl_info;
  } cmd_info;

} ds_muxi_cmd_info_type;


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

void ds_muxi_init(void);
void ds_muxi_deinit (void);

ds_mux_result_enum_type   ds_mux_initialize_cmux_param
(
  dlci_cmux_param_type    *cmux_param
);


ds_mux_result_enum_type  ds_muxi_passive_init
(
  dlci_cmux_param_type    *dlci_param
);

ds_mux_result_enum_type  ds_mux_set_client_param
(
  sio_stream_id_type                   sio_port_id,
  ds_mux_ext_client_event_info_type *event_info
);

ds_mux_result_enum_type  ds_muxi_active_init
(
  boolean                            is_cmux_supported,
  sio_stream_id_type                 stream_id,
  ds_mux_ext_client_event_info_type *event_info
);

void ds_mux_prepare_and_send_msc_message
(
  uint8                                  dlci_id,
  uint8                                  fc_flag,
  uint8                                  rtc_asserted_flag,
  uint8                                  cd_asserted_flag,
  uint8                                  ic_asserted_flag,
  uint8                                  rtr_asserted_flag
);

ds_mux_state_enum_type ds_mux_get_mux_state(void);

void ds_mux_set_mux_state
(
  ds_mux_state_enum_type  state
);

void ds_mux_update_phy_port_open_status
(
  ds_mux_result_enum_type  port_open_status
);

ds_mux_result_enum_type ds_mux_process_dlci_rx_ctl_frames
(
  ds_mux_io_frame_type    *frame
);

ds_mux_cl_type_enum_type ds_mux_get_cl_type
(
  uint8                       dlci
);

ds_mux_result_enum_type ds_mux_process_rx_frag_data_frames
(
  ds_mux_io_frame_type    *frame
);

boolean ds_mux_is_initiator(void);

void ds_mux_set_is_initiator
(
  boolean  is_initiator
);

void ds_mux_prepare_ua_response
(
  ds_mux_io_frame_type    *input_frame,
  ds_mux_io_frame_type    *output_frame
);

ds_mux_dlci_param_type* ds_mux_get_dlci_conn_ptr
(
  uint32                   dlci_id 
);

ds_mux_sig_enum_type ds_mux_get_sig_by_id
(
  uint32  dlci_id
);

void  ds_mux_disable_flow_mask
(
  uint8                    dlci_id,
  ds_mux_flow_ctl_mask_enum_type fc_msk
);

void  ds_mux_enable_flow_mask
(
    uint8                        dlci_id,
  ds_mux_flow_ctl_mask_enum_type fc_msk
);

ds_mux_result_enum_type ds_mux_get_msg_type 
(
  byte                          msg_byte,
  ds_mux_msg_type_enum_type    *msg_type
);

boolean   ds_mux_is_flow_enabled
(
  uint8                           dlci_id
);

void ds_mux_disconnect_dlci
(
  ds_mux_dlci_param_type  *dlci_conn
);

#endif /* DS_MUX_H */
