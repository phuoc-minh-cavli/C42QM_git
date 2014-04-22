#ifndef SMD_SIO_INTERNAL_H
#define SMD_SIO_INTERNAL_H
/*===========================================================================

      Shared Memory Driver SIO Interface Header File for use within SMD

 Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_sio_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/08   jlk     initial version split from smd_sio.h
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "sio.h" /* sio data types used in prototypes */
#include "dsm.h" /* dsm data types used in prototypes */


/*===========================================================================
                      PUBLIC FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_sio_open

DESCRIPTION   This function opens a shared memory stream.

ARGUMENTS     open_ptr - SIO open structure containing open params.

DEPENDENCIES  smd_sio_dev_init must have been called.
 
RETURN VALUE  SIO_DONE_S:     SIO stream as specified in open_ptr has been
                              successfully opened.
              SIO_BADP_S:     Bad Parameter
              SIO_UNAVAIL_S:  All available streams have been allocated.
    
SIDE EFFECTS  None
===========================================================================*/
sio_status_type smd_sio_open( sio_open_type *open_ptr );

/*===========================================================================
FUNCTION      smd_sio_close
                                                                             
DESCRIPTION   This function starts closing the port. It saves the client's
              callback function which will be called when the port is closed.

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port to close

              close_cb - callback to call when the stream is closed and all
                        of the queued data has been received by the other
                        processor.
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called when all pending data has been received
              by the other processor
===========================================================================*/
void smd_sio_close
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void               (*close_func_ptr)(void)
);

/*===========================================================================
FUNCTION      smd_sio_flush_tx
                                                                             
DESCRIPTION   This function flushes the TX.

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port to flush

              flush_cb - callback to call when all of the queued data has
                         been received by the other processor.
                  
DEPENDENCIES  User should not queue more data until after the registered
              callback is executed.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_flush_tx
( 
    sio_stream_id_type stream_id,
    sio_port_id_type   port_id,
    void               (*flush_func_ptr) (void)
);

/*===========================================================================
FUNCTION      smd_sio_dev_transmit

DESCRIPTION   This function queues the dsm chain and signals the smd task that
              data is pending.

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port to send the data on

              tx_ptr - pointer to DSM item to send
                  
DEPENDENCIES  None
 
RETURN VALUE  None
 
SIDE EFFECTS  None
===========================================================================*/
void smd_sio_dev_transmit
( 
    sio_stream_id_type stream_id,           /* Stream ID                     */
    sio_port_id_type   port_id,
    dsm_item_type     *tx_ptr               /* Packet for transmission       */
);

/*===========================================================================
FUNCTION      smd_sio_ioctl

DESCRIPTION   This function is used to control various features of the SMD
              interface. The behaviour of this function depends on the
              "command" issued.

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port to control

              cmd - SIO IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_ioctl
(
    sio_stream_id_type    stream_id,
    sio_port_id_type      port_id,
    sio_ioctl_cmd_type    cmd,
    sio_ioctl_param_type *param
);

/*===========================================================================
FUNCTION      smd_sio_disable_device

DESCRIPTION   Does nothing, there's no way to disable this device.

RETURN VALUE  None
===========================================================================*/
void smd_sio_disable_device( sio_port_id_type port_id );

/*===========================================================================
FUNCTION      smd_sio_control_open

DESCRIPTION   This function opens a control shared memory stream.

ARGUMENTS     open_ptr - SIO open structure containing open params.

DEPENDENCIES  smd_sio_dev_init must have been called.
 
RETURN VALUE  SIO_DONE_S:     SIO stream as specified in open_ptr has been
                              successfully opened.
              SIO_BADP_S:     Bad Parameter
              SIO_UNAVAIL_S:  All available streams have been allocated.
    
SIDE EFFECTS  None
===========================================================================*/
sio_status_type smd_sio_control_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
);

/*===========================================================================
FUNCTION      smd_sio_control_close
                                                                             
DESCRIPTION   This function starts closes a control port.  The closure
              of the control section of this 

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port to close

              close_cb - callback to call when the stream is closed
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called regardless of if there is pending data
              for this control port in the stream
===========================================================================*/
void smd_sio_control_close
(
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void             (*close_func_ptr)(void)
);

/*===========================================================================
FUNCTION      smd_sio_control_transmit

DESCRIPTION   This function pushes the port number on the tail of the dsm_item
              chain and then multiplexes the transmit to the SIO_PORT_SMD_CONTROL
              port

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port to send the data on

              tx_ptr - pointer to DSM item to send

DEPENDENCIES  None
 
RETURN VALUE  None
 
SIDE EFFECTS  None
===========================================================================*/
void smd_sio_control_transmit
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type     *tx_ptr
);

/*===========================================================================
FUNCTION      smd_sio_control_ioctl

DESCRIPTION   Place holder funciton for handling control port ioctls

ARGUMENTS     stream_id - the sio stream id of the port

              port_id - SIO port being flushed

              cb_data - a pointer to the info structure associated with the
                        port_id

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void smd_sio_control_ioctl
(
  sio_stream_id_type stream_id,
  sio_port_id_type      port_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
);

#endif /* SMD_SIO_INTERNAL_H */
