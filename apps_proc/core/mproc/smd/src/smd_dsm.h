#ifndef SMD_DSM_H
#define SMD_DSM_H

/*===========================================================================

                    Shared Memory Driver Watermark Header File

============================================================================*/

/**
  @file smd_dsm.h
*/

/* NOTE: For the output PDF generated using Doxygen and Latex, all file and group 
         descriptions are maintained in the SMD_mainpage file. To change any of the 
         the file/group text for the PDF, edit the SMD_mainpage file, or contact 
         Tech Pubs.
*/

/*===========================================================================
Copyright (c) 2006-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_dsm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/13   bt      Make SMD use secure SMEM partitions.
10/05/12   bt      Add smd.h for SMD typedefs.
04/01/11   bm      Updated prototype for smd_wm_open_3() which now takes 
                   smd_xfrflow_type as part of smd_open_param_type input param
                   struct and not as a separate input param.
10/25/10   bt      Fixed port_name argument bugs in all smd open functions.
08/12/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
07/21/10   tl      Added SMD API for SMD priority. Deprecated other open APIs.
02/16/10   tl/esh  Added Doxygen tags
10/13/09   bfc/rs  Removed control port muxing.
09/29/09   bfc/rs  Added variable sized FIFO API support.
06/10/09   bfc     CMI updates.
04/20/09   bfc     Only declare AU public items here.
03/31/09   hwu     Merge in the changes to enable data stacking in smd_sio.
11/12/08   jlk     Added include of dsm.h
10/08/07   hwu     Added smd_wm_open_2.
12/18/06   bfc     Moved control port logic to the smd_wm_layer
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
06/22/06   bfc     Added the client capability to specify a receive DSM
                   WM queue
03/27/06   bfc     Modified SMD to use the new sio extended funcion callback
                   parameters
01/03/06   ptm     Initial version. 
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "dsm.h"  /* ensure this is included for types in prototypes below */
#include "smd.h"


/** @addtogroup smd_dsm
@{ */

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
CALLBACK      smd_wm_read_callback_fn
===========================================================================*/
/**
  Reads callback notifications for SMD DSM ports.

  This function is called from the SMD task when data has been read from a 
  port. If the port was opened in packet mode, the whole packet written into the
  other end of the port is recreated in the output.

  @param[in] port_id    SMD port on which data was read.
  @param[in] packet     DSM item containing the data read from the port.
  @param[in] cb_data    User data provided when the port was opened.

  @return
  None. 

  @dependencies
  None.
*/
/*=========================================================================*/
typedef void (*smd_wm_read_callback_fn)( smd_port_id_type  port_id,
                                         dsm_item_type    *packet,
                                         uint32            cb_data );

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smd_wm_init
===========================================================================*/
/**
  Initializes the data structures internal to this subsystem. It must be called 
  before any other functions in this subsystem are called.
  
  This function is typically called by the SMD itself during initialization. 
 
  @return
  None. 
 
  @dependencies
  The SMD must have been initialized using the platform-specific initialization 
  mechanism.
*/
/*=========================================================================*/
void smd_wm_init( void );

/*===========================================================================
FUNCTION      smd_wm_open_2
===========================================================================*/
/**
  Initializes a named shared memory port for use by a client. It must be called 
  before any other port operations are called.

  @param[in] port_name      Name of the SMD port to be opened.  Do not use 
                            whitespace in port_name.
  @param[in] channel_type   Specifies the connection type.
  @param[in] tx_queue       Transmit watermark queue.
  @param[in] rx_queue       Client Receive watermark queue. If non-NULL, the 
                            driver enqueues the data read from the buffer. If 
                            \a xfrflow_type is packet, only complete data chains 
                            are enqueued.
  @param[in] read_cb        Client read callback. If non-NULL, the driver calls 
                            this callback when it is finished filling a read
                            buffer. If \a xfrflow_type is packet, \a read_cb
                            is called once for each packet received.
  @param[in] cb_data        User data passed to \a read_cb.
  @param[in] dataxfr_mode   Data transfer mode: memory copy or data mover.
  @param[in] xfrflow_type   Data flow type: packet or stream.
  @param[in] dsm_pool_id    DSM pool ID to use when allocating read buffers.
 
  @return       
  Port ID of the open SMD DSM port.
 
  @dependencies 
  The SMD DSM must have been initialized by calling smd_wm_init(). This is 
  typically performed as part of the platform-specific SMD initialization.

@deprecated   This function is deprecated; new code should use smd_wm_open_3()
              instead. This function will be removed as soon as practical.
*/
/*=========================================================================*/
smd_port_id_type smd_wm_open_2
(
  char                    *port_name,
  smd_channel_type         channel_type,
  dsm_watermark_type      *tx_queue,
  dsm_watermark_type      *rx_queue,
  smd_wm_read_callback_fn  read_cb,
  uint32                   cb_data,
  smd_dataxfr_mode_type    dataxfr_mode,
  smd_xfrflow_type         xfrflow_type,
  dsm_mempool_id_enum_type dsm_pool_id
);

/*===========================================================================
FUNCTION      smd_wm_open_sz
===========================================================================*/
/**
  Initializes a named shared memory port, with a FIFO size, for use by a client. 
  It must be called before any other port operations are called.

  @param[in] port_name      Name of the SMD port to be opened.  Do not use 
                            whitespace in port_name.
  @param[in] channel_type   Specifies the connection type.
  @param[in] tx_queue       Transmit watermark queue.
  @param[in] rx_queue       Client Receive watermark queue. If non-NULL, 
                            the driver enqueues the data read from the buffer. 
                            If \a xfrflow_type is packet, only complete data 
                            chains are enqueued.
  @param[in] read_cb        Client read callback. If non-NULL, the driver calls 
                            this callback when it is finished filling a read 
                            buffer. If \a xfrflow_type is packet, \a read_cb is 
                            called once for each packet received.
  @param[in] cb_data        User data passed to \a read_cb.
  @param[in] dataxfr_mode   Data transfer mode: memory copy or data mover.
  @param[in] xfrflow_type   Data flow type: packet or stream.
  @param[in] dsm_pool_id    DSM pool ID to use when allocating read buffers.
  @param[in] fifo_sz        FIFO size. It must be >= #SMD_MIN_FIFO, must not be
                            greater than #SMD_MAX_FIFO, and must be cache 
                            aligned.

  @return       
  Port ID of the open SMD DSM port.

  @dependencies 
  The SMD DSM must have been initialized by calling smd_wm_init(). This is 
  typically performed as part of the platform-specific SMD initialization.

@deprecated   This function is deprecated; new code should use smd_wm_open_3()
              instead. This function will be removed as soon as practical.
*/
/*=========================================================================*/
smd_port_id_type smd_wm_open_sz
(
  char                    *port_name,
  smd_channel_type         channel_type,
  dsm_watermark_type      *tx_queue,
  dsm_watermark_type      *rx_queue,
  smd_wm_read_callback_fn  read_cb,
  uint32                   cb_data,
  smd_dataxfr_mode_type    dataxfr_mode,
  smd_xfrflow_type         xfrflow_type,
  dsm_mempool_id_enum_type dsm_pool_id,
  uint32                   fifo_sz
);

/*===========================================================================
FUNCTION      smd_wm_open_3
===========================================================================*/
/*!
@brief        Initialize a named shared memory port, with optional parameters,
              for use by a client.

              Must be called before other port operations are called.

@param[in]    port_name   The name of the SMD port to be opened.  Do not use 
                          whitespace in port_name.

@param[in]    channel_type   Specifies the connection type

@param[in]    tx_queue   transmit watermark queue

@param[in]    rx_queue   Client receive watermark queue.  If non-NULL 
                         the driver will enqueue data read from the buffer.
                         If \a xfrflow_type is packet, then only complete data
                         chains will be enqueued.

@param[in]    read_cb    Client read callback.  If non-NULL the driver will
                         call this callback when it is done filling a read
                         buffer. If \a xfrflow_type is packet, then \a read_cb
                         will be called once for each packet received.

@param[in]    cb_data   user data passed to \a read_cb

@param[in]    dataxfr_mode   the data transfer mode: memcpy or data mover.

@param[in]    dsm_pool_id   the DSM pool id to use when allocating read buffers.

@param[in]    smd_param     Optional paramaters used to configure the SMD
                            port. If this pointer is NULL, the default values
                            will be used instead. See \a smd_open_param_type
                            for detailed documentation on using the optional
                            paramaters.

@return       The port id of the open SMD DSM port.

@dependencies SMD DSM must have been initialized by calling smd_wm_init().
              This is normally done as part of the platform-specific SMD
              initialization.
*/
/*=========================================================================*/
smd_port_id_type smd_wm_open_3
(
  const char                *port_name,
  smd_channel_type           channel_type,
  dsm_watermark_type        *tx_queue,
  dsm_watermark_type        *rx_queue,
  smd_wm_read_callback_fn    read_cb,
  uint32                     cb_data,
  smd_dataxfr_mode_type      dataxfr_mode,
  dsm_mempool_id_enum_type   dsm_pool_id,
  const smd_open_param_type *smd_param
);

/*===========================================================================
FUNCTION      smd_wm_close
===========================================================================*/
/**
  Closes a shared memory port.
  
  The callback is called when the port has been closed. The port can be reopened 
  in the close callback routine. The port cannot be reused until the callback 
  has been called.

  @param[in] port_id     SMD port to close.
  @param[in] close_cb    Callback that is called when the port is actually closed.
  @param[in] cb_data     User data passed to the close callback routine.
  @param[in] flush       Flushes the port before closing. The close callback is
                         called when all of the pending data has been received by 
                         the other processor.

  @return
  None.

  @dependencies 
  The port must have been opened by smd_wm_open(), smd_wm_open_2(),
  smd_wm_open_3(), or smd_wm_open_sz().
*/
/*=========================================================================*/
void smd_wm_close
(
  smd_port_id_type      port_id,
  smd_close_callback_fn close_cb,
  void                  *cb_data,
  boolean               flush
);

/*===========================================================================
FUNCTION      smd_wm_ioctl
===========================================================================*/
/**
  Performs general control operations for a specified port.

  See smd_ioctl() for detailed information on the supported IOCTL operations 
  and how they are handled.

  @param[in] port_id    SMD port to control.
  @param[in] cmd        Control operation.
  @param[in] param      Data for control operation.

  @return
  None.

  @dependencies 
  The port must have been opened by smd_wm_open(), smd_wm_open_2(),
  smd_wm_open_3(), or smd_wm_open_sz().
*/
/*=========================================================================*/
void smd_wm_ioctl
(
  smd_port_id_type      port_id,
  smd_ioctl_cmd_type    cmd,
  smd_ioctl_param_type *param
);

/*===========================================================================
FUNCTION      smd_wm_flush
===========================================================================*/
/**
  Flushes an SMD port, sending all pending data to the other processor. 
  The callback is called when all of the data has been received by the other 
  processor.

  @param[in] pid        SMD port to flush.
  @param[in] flush_cb   Callback that is called when the port has been received
                        by the other processor.
  @param[in] cb_data    User data passed to the flush callback routine.

  @return
  None.

  @dependencies 
  The port must have been opened by smd_wm_open(), smd_wm_open_2(),
  smd_wm_open_3(), or smd_wm_open_sz().
*/
/*=========================================================================*/
#define smd_wm_flush(pid, flush_cb, cb_data) smd_flush(pid, flush_cb, cb_data )

/*===========================================================================
FUNCTION      smd_wm_notify_write
===========================================================================*/
/**
  Notifies the SMD that data has been appended to the transmit watermark queue. 
  The SMD asynchronously transmits the queued data into the transmit watermark 
  queue, freeing the DSM items after they are transmitted.

  @param[in] port_id    Shared memory port to notify.

  @return
  None.

  @dependencies 
  The port must have been opened by smd_wm_open(), smd_wm_open_2(),
  smd_wm_open_3(), or smd_wm_open_sz().
*/
/*=========================================================================*/
#define smd_wm_notify_write(port_id)             smd_notify_write(port_id)

/** @} */ /* end_addtogroup smd_dsm */


#ifdef __cplusplus
}
#endif

#endif /* SMD_DSM_H */
