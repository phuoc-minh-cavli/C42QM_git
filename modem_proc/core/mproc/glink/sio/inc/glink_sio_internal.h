#ifndef GLINK_SIO_INTERNAL_H
#define GLINK_SIO_INTERNAL_H
/*===========================================================================

      GLINK SIO Interface Header File for use within GLINK SIO

 Copyright (c) 2015-2016,2018-2019 by Qualcomm Technologies, Incorporated.  
 All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/19   ab      callback notification changes for CTS/RTS signals
02/10/16   ap      Rework to move all handling into worker threads
02/10/16   ap      Do not process any commands till the link is up.
07/24/15   db      Enable packet dropping
07/15/15   db      Added support for DUN call
01/30/14   db      Added local_open flag in info structure to prevent
                   opening same port multiple times. This is side-effect 
                   of calling glink_open from LOCAL_DISCONNECTED event
09/02/14   db     initial version of glink_sio_internal.h
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h"   /* sio data types used in prototypes */
#include "dsm.h"   /* dsm data types used in prototypes */
#include "timer.h" /* clock services definition         */

/*===========================================================================
                     CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/
/*---------------------------------------------------------------------------
 Constants used in '+++' escape code detection.
---------------------------------------------------------------------------*/

/* Guard time that is needed before and after the Escape Characters in order
** for the escape characters to be detected as escape sequence.  This time
** is also an upper bound on the amount of time that can pass between the
** escape characters.  Typically guard-time is 1s, but expire in .9s so that
** if application's timing is off (and/or phone's timing is off) escape 
** detection still works.  Phone's timing should not be off by more than 5ms,
** unless there are some extreme performance problems.
*/
#define SIOSMEM_ESC_SEQ_GUARD_TIME         900

/* Escape sequence character
*/
#define ESC_SEQ_CHAR          '+'

/* timer functions used in smd_sio */
#define GLINK_SIO_TIMER_TYPE timer_type

#define GLINK_SIO_TIMER_DEF(timer) timer_def2(timer, NULL)

#define GLINK_SIO_TIMER_CLR(timer) timer_clr(timer, T_NONE)

#define GLINK_SIO_TIMER_UNDEF(timer) timer_undef(timer)

#define GLINK_SIO_TIMER_REG(timer, func, data, time, repeat) \
        timer_reg(timer, func, data, time, repeat)
	 
typedef enum
{
  SIOSMEM_ESC_SEQ_NO_GUARDTIME,   /* no 1st guardtime                        */
  SIOSMEM_ESC_SEQ_1ST_GUARDTIME,  /* 1st Guardtime occurred                  */
  SIOSMEM_ESC_SEQ_1ST_PLUS,       /* Received 1st plus sign before guardtime */
  SIOSMEM_ESC_SEQ_2ND_PLUS,       /* Received 2nd plus sign before guardtime */
  SIOSMEM_ESC_SEQ_3RD_PLUS,       /* Received 3rd plus sign before guardtime */
  SIOSMEM_ESC_SEQ_DETECTED,       /* Escape Sequence detected                */
  SIOSMEM_ESC_SEQ_MAX             /* For Bounds checking only                */
} glink_sio_esc_seq_enum_type;

/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      glink_sio_open

DESCRIPTION   This function opens a shared memory stream.

ARGUMENTS     open_ptr - SIO open structure containing open params.

DEPENDENCIES  glink_sio_dev_init must have been called.
 
RETURN VALUE  SIO_DONE_S:     SIO stream as specified in open_ptr has been
                              successfully opened.
              SIO_BADP_S:     Bad Parameter
              SIO_UNAVAIL_S:  All available streams have been allocated.
    
SIDE EFFECTS  None
===========================================================================*/
sio_status_type glink_sio_open( sio_open_type *open_ptr );

/*===========================================================================
FUNCTION      glink_sio_close

DESCRIPTION   This function closes the port and stores callback function
              which gets called when glink core notifies sio layer of closing
              the port.

ARGUMENTS     stream_id - the SIO stream id of the port (not used)

              port_id - SIO port if of the port to close

              close_cb - callback to call when the stream is closed and all
                        of the queued data has been received by the other
                        processor.
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called when all pending data has been received
              by the other processor
===========================================================================*/
void glink_sio_close
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void               (*close_func_ptr)(void)
);

/*===========================================================================
FUNCTION      glink_sio_flush_tx
                                                                             
DESCRIPTION   This function flushes the TX.

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to flush

              flush_cb - callback to call when all of the queued data has
                         been received by the other processor.

DEPENDENCIES  User should not queue more data until after the registered
              callback is executed.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_sio_flush_tx
( 
    sio_stream_id_type stream_id,
    sio_port_id_type   port_id,
    void               (*flush_func_ptr) (void)
);

/*===========================================================================
FUNCTION      glink_sio_dev_transmit

DESCRIPTION   This function queues the dsm chain and signals the smd task that
              data is pending.

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to send the data on

              tx_ptr - pointer to DSM item to send

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_sio_dev_transmit
( 
    sio_stream_id_type stream_id,           /* Stream ID                     */
    sio_port_id_type   port_id,
    dsm_item_type     *tx_ptr               /* Packet for transmission       */
);

/*===========================================================================
FUNCTION     glink_sio_ioctl

DESCRIPTION   Handles data port ioctls

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to control

              cmd - SIO IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void glink_sio_ioctl
(
    sio_stream_id_type    stream_id,
    sio_port_id_type      port_id,
    sio_ioctl_cmd_type    cmd,
    sio_ioctl_param_type *param
);

/*===========================================================================
FUNCTION      glink_sio_disable_device

DESCRIPTION   Does nothing, there's no way to disable this device.

RETURN VALUE  None
===========================================================================*/
void glink_sio_disable_device( sio_port_id_type port_id );

/*===========================================================================
FUNCTION      glink_sio_control_open

DESCRIPTION   This function opens a control shared memory stream.

ARGUMENTS     open_ptr - SIO open structure containing open params.

DEPENDENCIES  smd_sio_dev_init must have been called.
 
RETURN VALUE  SIO_DONE_S:     SIO stream as specified in open_ptr has been
                              successfully opened.
              SIO_BADP_S:     Bad Parameter
              SIO_UNAVAIL_S:  All available streams have been allocated.
    
SIDE EFFECTS  None
===========================================================================*/
sio_status_type glink_sio_control_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
);

/*===========================================================================
FUNCTION      glink_sio_control_close
                                                                             
DESCRIPTION   This function starts closing the control port

ARGUMENTS     stream_id - sio stream id of the port to close (ignored)

              port_id   - SIO port to close

              close_cb  - callback to call when the stream is closed
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called regardless of if there is pending data
              for this control port in the stream
===========================================================================*/
void glink_sio_control_close
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void             (*close_func_ptr)(void)
);

/*===========================================================================
FUNCTION      glink_sio_control_transmit

DESCRIPTION   Get glink_sio_info for control port and call glink core
              transmit

ARGUMENTS     stream_id - sio stream id of the port to close (ignored)

              port_id   - SIO port to send the data on

              tx_ptr    - pointer to DSM item to send

DEPENDENCIES  None
 
RETURN VALUE  None
 
SIDE EFFECTS  None
===========================================================================*/
void glink_sio_control_transmit
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type     *tx_ptr
);

/*===========================================================================
FUNCTION      glink_sio_control_ioctl

DESCRIPTION   Handles control port ioctls

ARGUMENTS     stream_id - sio stream id of the port to close (ignored)

              port_id   - SIO port to control

              cmd       - SIO IOCTL command

              param     - pointer to union containing parameters or results for
                          command - may be NULL if the command does not require
                          any parameters

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void glink_sio_control_ioctl
(
  sio_stream_id_type stream_id,
  sio_port_id_type      port_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
);

/* Control or Data port */
typedef enum
{
  GLINK_SIO_DATA_PORT,
  GLINK_SIO_CONTROL_PORT,
  GLINK_SIO_UNASSIGNED_PORT
} glink_sio_port_type;

/* Transfer flow modes */
typedef enum    
{
  GLINK_SIO_PKTXFR_TYPE,
  GLINK_SIO_STREAMING_TYPE
} glink_sio_xfrflow_type;

/* this structure stores information per channel/per processor */
typedef struct 
{
  /* sio open configuration passed by sio layer */ 
  sio_open_type open;

  /* Open params provided by user */
  sio_open_param_type open_param;

  /* control or data port */
  glink_sio_port_type port_type;

  /* opaque handle returned by glink_open */
  /* stored here to callback into glink api */
  glink_handle_type glink_handle;

  /* close callback function pointer provided by sio layer*/
  sio_vv_func_ptr_type close_callback_fn;

  /* flush callback function pointer provided by sio layer */
  sio_vv_func_ptr_type flush_callback_fn;

  /* DTR callback function pointer provided by sio layer */
  sio_vv_func_ptr_type dtr_callback_fn;

  /* escape callback function pointer provided by sio layer */
  sio_vv_func_ptr_type esc_callback_fn;

  /* function and data pointer for extended DTR callback */
  sio_vpu_func_ptr_type dtr_callback_fn_ext;
  void * dtr_callback_data_ext;
  
  /* function and data pointer for extended CTS callback */
  sio_vpu_func_ptr_type cts_callback_fn_ext;
  void * cts_callback_data_ext;

  /* function and data pointer for extended CD callback */
  sio_vpu_func_ptr_type cd_callback_fn_ext;
  void * cd_callback_ext_data;

  /* function and data pointer for extended RI callback */
  sio_vpu_func_ptr_type ri_callback_fn_ext;
  void * ri_callback_ext_data;
  
  /* variable to keep track if channel is connected */
  boolean connected;  
  
  /* variable to keep track of whether channel is locally opened */
  boolean local_open;

  /* Set to TRUE when the client is initiating a close */
  boolean local_closing;

  /* state variable for '+++' escape code detection  */  
  glink_sio_esc_seq_enum_type esc_seq_state;

  /* Clock call back structure which is used in escape code detection and
     auto-detect. */  
  GLINK_SIO_TIMER_TYPE esc_seq_timer;
  
  /* Function pointer which should be called when detecting a PPP packet */
  sio_vv_func_ptr_type pkt_callback_fn;
  
  /* flag to identify whether we should wait before transmitting packet */
  boolean wait;  
  
  /* flag to identify whether packet can be dropped if channel is not 
     fully opened  */
  boolean drop_enabled;	

  /* dsm item pointer to keep track of last dsm item sent */
  dsm_item_type *dsm_pending_tx;  

  /* Flow control status of the receiver */
  boolean tx_flow_enabled;

  /* Cached local and remote signal values */
  uint32 local_sigs;
  uint32 remote_sigs;

  /* The last outstanding intent request */
  uint32 req_size;

  /* Number of packets waiting to be transmitted */
  uint32 tx_pending_packets;

  /* The xfrflow type stream or packet */
  glink_sio_xfrflow_type xfrflow;

} glink_sio_info_type;

#endif /* GLINK_SIO_INTERNAL_H */
