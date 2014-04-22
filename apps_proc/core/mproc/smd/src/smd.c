/*===========================================================================

                    Shared Memory Driver Source File


 Copyright (c) 2005-2013 by Qualcomm Technologies, Incorporated.  All Rights
 Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/13   rv      SMEM Partition backwards compatibility is removed.
07/24/13   bt      Make SMD use secure SMEM partitions.
05/29/13   pa      Remove check on data_read flag in smd_notify_write.
04/15/13   rv/bt   Do not queue a redundant Process Port cmd in 
                   smd_notify_write if data_read flag is already set.
02/22/13   sm      Added other_host in smd_stream_info_type struct.
02/11/13   bt      Replace string copy macro with smd_string_copy.
03/22/12   bm      Use new macros to access SMD channel header struct.
04/05/12   bt      Remove smd_target.h, smd_open_memcpy() for B-family.
12/07/11   bm      Update smdl_ioctl() header with newly supported ioctl - 
                   SMD_IOCTL_REGISTER_MEMCPY_RESET
04/20/11   bm      Include file smd_taget.h which defines SMD_(APPS/MODEM)_PROC
                   needed for compilation of smd_open_memcpy()
04/01/11   bm      Updated functions
                    + smd_open_memcpy() - added param xfrflow_type when calling
                      smdi_alloc_channel_info.
                    + smd_open_memcpy_3() - handling smd_xfrflow_type param as 
                      part of smd_open_param_type input param struct.
04/01/11   bt      Featurize out smd_open_memcpy if not Modem/Apps.
01/06/11   bt      Removed SMD Block and Data Mover protocols.
12/22/10   hwu     Removed component services SMD port.
10/25/20   bt      Fixed port_name argument bugs in all smd open functions.
09/27/10   tl/ih   Send messages straight to the port for smd_notify_write(),
                   rather than letting the SMD task iterate through all open
                   ports
07/21/10   tl      Implemented SMD API for SMD priority.
06/08/10   tl      Split SMD task into multiple tasks with different priorities
09/29/09   bfc/rs  Added variable sized FIFO API support.
09/24/09   rs      Fixed typo.
07/17/09   tl      Added CTS/RTS callback
06/10/09   bfc     CMI updates.
04/01/09   ih      fixed error fataling on boot
03/10/09   jlk     fixed stream state check in smd_notify_read and 
                   smd_notify_write
06/10/08   sa      Smd_target.h cleanup. 
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
04/17/08   hwu     Fixed the channel_type.
02/21/08   bfc     First SMD Block port check in.
01/29/08   hwu     Added smd_api_version.
01/14/08   bfc     Created a SMD base struct.
11/09/07   bfc     Removed strncpy.
10/25/07   hwu     Changed tx/rx/flowctl callback prototype.
10/08/07   hwu     Added smd_open_memcpy_2.
07/18/07   hwu     Moved assert.h and err.h to smd_target.h.
07/05/07   hwu     Added the channel_type when open a stream.
04/26/07   ptm     Change "stub" port to SMD PORT DUMMY
03/20/07   taw     Include smd_target.h instead of customer.h and target.h.
01/09/07   bfc     Added SMD using DM
12/18/06   bfc     Added the capability to close stubed ports.
12/06/06   bfc     Fixed the location where we were getting the data_read
                   flag from for transmit operations
11/29/06   taw     Use macros to access shared memory data.
11/13/06   hwu     combined smd_update_sig, smd_receive_sig and
                   smd_transmit_sig into smd_event_sig.
11/03/06   ptm     Fixed a race condition that existed between opening a port
                   and then calling smd_notify_read/write before SMD had a
                   chance to process the open. And change SMD_SMD_... symbols
                   to SMD_... symbols.
10/05/06   bfc     Added new ioctl's that enable/disable Full Rcv Buffer
                   control and modified some comments
09/28/06   taw     Convert to using SMD symbols instead of SIO symbols.
09/28/06   bfc     Added an ioctl to support additional functionality while
                   a port is in the Opening state
09/05/06   ptm     Remove references to REX.
09/05/06   ptm     Add code to support getting the buffer size.
08/22/06   ptm     Merge WinMobile changes to main line.
05/24/06   bfc     Added ioctl's to get the CD and RI state
05/05/06   ptm     Make interface signals synchronous and add pending IOCTL
                   counter.
04/28/06   ptm     Make interface signals asynchronous.
04/28/06   ptm     Added flow control callback.
03/27/06   bfc     Modified SMD to use the new sio extended function callback
                   parameters
11/22/05   ptm     Initial version (based on smd_interface.c) API restructured.
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smd_os.h"
#include "smd.h"
#include "smd_v.h"
#include "smd_internal.h"
#include "smd_main.h"
#include "smd_os_common.h"

/*===========================================================================
                              GLOBAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_open_memcpy_2

DESCRIPTION   Opens a shared memory driver memcpy stream for the given port.
              Must be called before any other operations are done on that port.

ARGUMENTS     port name  - The name of the SMD port to open. The same
                           name must be used by both processors for a
                           particular port. The length of the name is
                           defined as SMD_CHANNEL_NAME_SIZE_MAX, where the 
                           last character is always null. Do not use 
                           whitespace in port_name.

              channel_type   - The port connection type.

              tx_callback_fn - This function is called when the previous
                               transmit buffer (if any) has been copied to
                               shared memory. The callback should provide the
                               next buffer (and the buffer size) to send.

                               If no buffer is provided, then smd_notify_write
                               must be called when a new transmit buffer is
                               available. The call to smd_notify_write will
                               cause the SMD task to call this callback to
                               get the buffer.

                               This callback is called in the SMD task context
                               and MUST NOT BLOCK OR WAIT.

              tx_callback_data - A pointer to the client callback data
                                 for tx_callback_fn

              rx_callback_fn - This function is called when SMD has finished
                               writing data into the previous receive buffer
                               (if any). It is called with the number of bytes
                               written into the previous buffer. The callback
                               should provide the next read buffer and the
                               space available in the buffer.

                               If the number of bytes written is zero, then
                               there was no previous buffer.

                               If no buffer is provided, then smd_notify_read
                               must be called when a new receive buffer is
                               available. The call to smd_notify_read will
                               cause the SMD task to call this callback
                               to get the buffer.

                               This call back is called in the SMD task context
                               and MUST NOT BLOCK OR WAIT.

              rx_callback_data - A pointer to the client callback data
                                 for rx_callback_fn

              rx_flowctl_fn -  This function is called when rx_callback_fn
                               returns a NULL buffer and flow control is not
                               enabled. Flow control not enabled means that
                               pending data should be thrown away if there's
                               no where to put the data.

                               rx_flowctl_fn is called with the number of bytes
                               pending and returns the number of bytes to
                               skip.

                               For packet mode streams, this can be used to
                               keep packet sync. by skipping bytes to the next
                               header.

                               For stream mode stream, you typically skip all
                               of the pending bytes.

                               This call back is called in the SMD task context
                               and MUST NOT BLOCK OR WAIT.

              rx_flowctl_data - A pointer to the client callback data
                                for rx_flowctl_fn

RETURN VALUE  Returns the SMD port id.

SIDE EFFECTS  Allocates an smd stream info structure.
===========================================================================*/
smd_port_id_type smd_open_memcpy_2
(
  const char                *port_name,
  smd_channel_type           channel_type,
  smd_memcpy_tx_callback_fn  tx_callback_fn,
  void                      *tx_callback_data,
  smd_memcpy_rx_callback_fn  rx_callback_fn,
  void                      *rx_callback_data,
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn,
  void                      *rx_flowctl_data
)
{
  /* Enforce new callback data semantics: One data pointer for all three
   * callback functions. */
  ASSERT(tx_callback_data == rx_callback_data);
  ASSERT(tx_callback_data == rx_flowctl_data);

  return smd_open_memcpy_3(port_name,
                           channel_type,
                           tx_callback_data,
                           tx_callback_fn,
                           rx_callback_fn,
                           rx_flowctl_fn,
                           NULL);
}


/*===========================================================================
FUNCTION      smd_open_memcpy_sz

DESCRIPTION   Opens a shared memory driver memcpy stream with a variable sized
              fifo for the given port.  Must be called before any other 
              operations are done on that port.

ARGUMENTS     port name  - The name of the SMD port to open. The same
                           name must be used by both processors for a
                           particular port. The length of the name is
                           defined as SMD_CHANNEL_NAME_SIZE_MAX, where the 
                           last character is always null. Do not use 
                           whitespace in port_name.

              channel_type   - The port connection type.
              
              tx_callback_fn - This function is called when the previous
                               transmit buffer (if any) has been copied to
                               shared memory. The callback should provide the
                               next buffer (and the buffer size) to send.

                               If no buffer is provided, then smd_notify_write
                               must be called when a new transmit buffer is
                               available. The call to smd_notify_write will
                               cause the SMD task to call this callback to
                               get the buffer.

                               This callback is called in the SMD task context
                               and MUST NOT BLOCK OR WAIT.

              tx_callback_data - A pointer to the client callback data
                                 for tx_callback_fn
              
              rx_callback_fn - This function is called when SMD has finished
                               writing data into the previous receive buffer
                               (if any). It is called with the number of bytes
                               written into the previous buffer. The callback
                               should provide the next read buffer and the
                               space available in the buffer.

                               If the number of bytes written is zero, then
                               there was no previous buffer.

                               If no buffer is provided, then smd_notify_read
                               must be called when a new receive buffer is
                               available. The call to smd_notify_read will
                               cause the SMD task to call this callback
                               to get the buffer.
    
                               This call back is called in the SMD task context
                               and MUST NOT BLOCK OR WAIT.

              rx_callback_data - A pointer to the client callback data
                                 for rx_callback_fn

              rx_flowctl_fn -  This function is called when rx_callback_fn
                               returns a NULL buffer and flow control is not
                               enabled. Flow control not enabled means that
                               pending data should be thrown away if there's
                               no where to put the data.

                               rx_flowctl_fn is called with the number of bytes
                               pending and returns the number of bytes to
                               skip.

                               For packet mode streams, this can be used to
                               keep packet sync. by skipping bytes to the next
                               header.

                               For stream mode stream, you typically skip all
                               of the pending bytes.

                               This call back is called in the SMD task context
                               and MUST NOT BLOCK OR WAIT.

              rx_flowctl_data - A pointer to the client callback data
                                 for rx_flowctl_fn

              fifo_sz - The size of each FIFO.  The entire FIFO size allocated
                        will be 2*fifo_sz.  The fifo must be 32 byte aligned.
                                 
RETURN VALUE  Returns the SMD port id.

SIDE EFFECTS  Allocates an smd stream info structure.
===========================================================================*/
smd_port_id_type smd_open_memcpy_sz
(
  const char                *port_name,
  smd_channel_type           channel_type,
  smd_memcpy_tx_callback_fn  tx_callback_fn,
  void                      *tx_callback_data,
  smd_memcpy_rx_callback_fn  rx_callback_fn,
  void                      *rx_callback_data,
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn,
  void                      *rx_flowctl_data,
  uint32                     fifo_sz
)
{
  smd_open_param_type param;

  param.fields = SMD_OPEN_PARAM_FIFO_SIZE;
  param.fifo_size = fifo_sz;

  /* Enforce new callback data semantics: One data pointer for all three
   * callback functions. */
  ASSERT(tx_callback_data == rx_callback_data);
  ASSERT(tx_callback_data == rx_flowctl_data);

  return smd_open_memcpy_3(port_name,
                           channel_type,
                           tx_callback_data,
                           tx_callback_fn,
                           rx_callback_fn,
                           rx_flowctl_fn,
                           &param);
}

/*===========================================================================
  FUNCTION  smd_open_memcpy_3
===========================================================================*/
/*!
@brief
  Opens a shared memory port identified by the name and edge with optional
  parameters.

  This function must be called before any other operations are done on that
  port. 

@param[in]  port_name           The name of the SMD port. The same name must
                                be used by both processors for a particular
                                port. The length of the name is defined as
                                SMD_CHANNEL_NAME_SIZE_MAX, where the last 
                                character is always null. Do not use 
                                whitespace in port_name.

@param[in]  channel_type        The port connection type, specifies the
                                connection direction.  

@param[in]  callback_data       A pointer to the client data passed to all
                                three client callback functions.

@param[in]  tx_callback_fn      This function is called when the previous
                                transmit buffer (if any) has been copied to
                                shared memory. The callback should provide the
                                next buffer(and the buffer size)to send.  If
                                no buffer is provided, then smd_notify_write
                                must be called when a new transmit buffer is
                                available. The call to smd_notify_write will
                                cause the SMD task to call this callback to
                                get the buffer.  This callback is called in
                                the SMD task context and MUST NOT BLOCK OR
                                WAIT.

@param[in]  rx_callback_fn      This function is called when SMD has finished
                                writing data into the previous receive buffer
                                (if any). It is called with the number of
                                bytes written into the previous buffer. The
                                callback should provide the next read buffer
                                and the space available in the buffer.If the
                                number of bytes written is zero, then there
                                was no previous buffer. If no buffer is
                                provided, then smd_notify_read() must be
                                called when a new receive buffer is available.
                                The call to smd_notify_read() will cause the
                                SMD task to call this callback to get the
                                buffer.  This callback is called in the SMD
                                task context and MUST NOT BLOCK OR WAIT.

@param[in]  rx_flowctl_fn       This function is called when rx_callback_fn
                                returns a NULL buffer and flow control is not
                                enabled. Flow control not enabled means that
                                pending data should be throw away if there's
                                nowhere to put the data. rx_flowctl_fn is
                                called with the number of bytes pending and
                                returns the number of bytes to skip.  For
                                packet mode streams, this can be used to keep
                                packet sync. by skipping bytes to the next
                                header.  For stream mode stream, you typically
                                skip all of the pending bytes.  This callback
                                is called in the SMD task context and MUST NOT
                                BLOCK OR WAIT.

@param[in]  smd_param           Optional paramaters used to configure the SMD
                                port. If this pointer is NULL, the default
                                values will be used instead. See \a
                                smd_open_param_type for detailed documentation
                                on using the optional paramaters.

@return       The SMD port id of the newly-opened port

@sideeffect   Allocates an smd stream info structure

@dependencies
              - Shared memory must have been initialized on this processor by
                calling smem_init().
              - The Shared Memory Driver must be up and running. The startup
                process is platform-specific.

@ingroup smd_buffer
*/
/*=========================================================================*/
smd_port_id_type smd_open_memcpy_3
(
  const char                *port_name,
  smd_channel_type           channel_type,
  void                      *callback_data,
  smd_memcpy_tx_callback_fn  tx_callback_fn,
  smd_memcpy_rx_callback_fn  rx_callback_fn,
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn,
  const smd_open_param_type *smd_param
)
{
  smd_cmd_type          cmd;
  smd_info_type        *info;
  smd_stream_info_type *sinfo;
  smd_port_id_type      port_id = 0;
  uint32                fifo_sz;
  smd_priority_type     priority;
  smd_xfrflow_type      xfrflow;

  ASSERT(tx_callback_fn != NULL);
  ASSERT(rx_callback_fn != NULL);
  ASSERT(rx_flowctl_fn != NULL);

  /* Use FIFO size from the optional parameters if it is specified; otherwise,
   * use the default. */
  if((smd_param != NULL) && (smd_param->fields & SMD_OPEN_PARAM_FIFO_SIZE))
  {
    fifo_sz = smd_param->fifo_size;
  }
  else
  {
    fifo_sz = SMD_STANDARD_FIFO;
  }

  /* Validate size: multiple of 32, and within the min and max sizes */
  if( ( fifo_sz & 0x1F ) != 0 || (fifo_sz < SMD_MIN_FIFO) ||
      (fifo_sz > SMD_MAX_FIFO) )
  {
    ERR_FATAL("Invalid SMD FIFO SZ: %i",fifo_sz,0,0);
  }

  /* Use the channel priority from the optional parameters if it is specified;
   * otherwise, use the default. */
  if((smd_param != NULL) && (smd_param->fields & SMD_OPEN_PARAM_PRIORITY))
  {
    priority = smd_param->priority;
  }
  else
  {
    priority = SMD_PRIORITY_DEFAULT;
  }
  ASSERT(priority < SMD_PRIORITY_COUNT);

  /* Use xfrflow type from the optional parameters if it is specified; otherwise
   * use the default. */
  if((smd_param != NULL) && (smd_param->fields & SMD_OPEN_PARAM_XFRFLOW))
  {
    xfrflow = smd_param->xfrflow;
  }
  else
  {
    xfrflow = SMD_NO_XFRFLOW;
  }
  ASSERT(xfrflow < SMD_INVALID_TYPE);

  if (port_name == NULL || !strcmp(port_name, ""))
  {
    ERR_FATAL("%s: Invalid port name.", __FUNCTION__, 0, 0);
  }
  
  /* Allocate the channel info */
  port_id = smdi_alloc_channel_info(port_name,
                                    channel_type,
                                    xfrflow);

  info = smdi_alloc_info( port_id );
 
  /* Initialize the fields of the smd info structure */
  smd_string_copy(info->port_name, port_name, SMD_CHANNEL_NAME_SIZE_MAX);
  info->channel_type = channel_type;

  /* Initialize "simple" fields of stream info structure */   
  sinfo = &info->info.stream;
  sinfo->port_id = port_id;
  sinfo->channel_type = channel_type;

  sinfo->fifo_sz = fifo_sz;

  sinfo->tx_flow_control_method = SMD_CTSRFR_FCTL;
  sinfo->rx_flow_control_method = SMD_CTSRFR_FCTL;

  sinfo->tx_fctl_enabled = TRUE;
  sinfo->rx_fctl_enabled = TRUE;

  sinfo->dataxfr_mode = SMD_MEMCPY_MODE;

  sinfo->mode.memcpy.tx_callback_fn   = tx_callback_fn;
  sinfo->mode.memcpy.tx_callback_data = callback_data;
  sinfo->mode.memcpy.rx_callback_fn   = rx_callback_fn;
  sinfo->mode.memcpy.rx_callback_data = callback_data;
  sinfo->mode.memcpy.rx_flowctl_fn    = rx_flowctl_fn;
  sinfo->mode.memcpy.rx_flowctl_data  = callback_data;

  smd_assign_context(info, priority);

  /* Send command to smd task */
  cmd.cmd = SMD_CMD_OPEN_MEMCPY;
  cmd.data.open_memcpy.port_id = port_id;

  if(!strcmp(port_name, "LOOPBACK"))
  {
     cmd.data.open_memcpy.is_loopback = TRUE;
  }
  else
  {
    cmd.data.open_memcpy.is_loopback = FALSE;
  }

  smd_cmd( &cmd );

  return(port_id);
}

/*===========================================================================
FUNCTION      smd_close

DESCRIPTION   Close a shared memory port.

              The callback is called when the port has been closed. The port
              may be reopened in the close callback routine. The port may not
              be reused until after the callback has been called.

ARGUMENTS     port_id - SMD port to close

              close_cb - callback that is called when the port is actually
                         closed.

              cb_data - user data passed to the close callback routine.

              flush - flush the port before closing. If TRUE, the close
                      callback will be called when all of the pending data
                      has been received by the other processor. The flush
                      operation will not complete until the SMD task calls
                      the tx_callback_fn and does NOT get a another transmit
                      buffer. (That is, the higher level transmit queue is
                      empty as well.)

                      If the other side of the port closes before the flush
                      is complete, any data pending in the shared memory pipe
                      is lost.

                      If FALSE, the close callback is called as soon as
                      possible and there may be data remaining in transmit
                      buffers and in the shared memory pipe (and in any
                      higher level queues).

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void smd_close
(
  smd_port_id_type      port_id,
  smd_close_callback_fn close_cb,
  void                  *cb_data,
  boolean               flush
)
{
  smd_cmd_type cmd;

  SMD_VALIDATE_PORT_ID( port_id );

  cmd.cmd = SMD_CMD_CLOSE;
  cmd.data.close.port_id = port_id;
  cmd.data.close.close_cb = close_cb;
  cmd.data.close.cb_data = cb_data;
  cmd.data.close.flush = flush;

  smd_cmd( &cmd );
} /* smd_close */

/*===========================================================================
FUNCTION      smd_ioctl

DESCRIPTION

  This function is used to control various features of the SMD interface. The
  behaviour of this function depends on the "command" issued.

  The command parameter is used to control the behaviour of the stream.

  The following commands are handled immediately:

    SMD_IOCTL_DTE_READY_ASSERTED:    Retrieve the state of the DTR signal
    SMD_IOCTL_GET_CURRENT_RTS:       Retrieve the state of the RTS signal
    SMD_IOCTL_GET_FLOW_CTL:          Retrieve the flow control method for the
                                     port
    SMD_IOCTL_GET_CURRENT_CD:        Retrieve the state of the CD signal
    SMD_IOCTL_GET_CURRENT_RI:        Retrieve the state of the RI signal
    SMD_IOCTL_GET_BUFFER_SIZE:       Retrieve the state of the Buffer Size
                                     control
    SMD_IOCTL_GET_DROP_PEND:         Retrieve the state of the drop pending
                                     control
    SMD_IOCTL_GET_FULL_RCV_BUFS:     Retrieve the state the Full Rcv Buffers
                                     control

  The following commands are handled by the smd_task:

    SMD_IOCTL_CD_ASSERT:             Assert the Carrier Detect signal
    SMD_IOCTL_CD_DEASSERT:           De-assert the carrier detect signal
    SMD_IOCTL_RI_ASSERT:             Assert the Ring Indicator signal
    SMD_IOCTL_RI_DEASSERT:           De-assert the Ring Indicator signal
    SMD_IOCTL_ENABLE_DTR_EVENT:      Register DTR-Toggle callback function
    SMD_IOCTL_DISABLE_DTR_EVENT:     Deregister the DTR-Toggle callback fn
    SMD_IOCTL_INBOUND_FLOW_ENABLE:   Enable inbound flow
    SMD_IOCTL_INBOUND_FLOW_DISABLE:  Disable inbound flow
    SMD_IOCTL_DSR_ASSERT:            Assert DSR signal
    SMD_IOCTL_DSR_DEASSERT:          De-assert DSR signal
    SMD_IOCTL_SET_FLOW_CTL:          Allows setting of flow control method
    SMD_IOCTL_SET_XFR_MODE:          Set the transfer type for device
    SMD_IOCTL_ENABLE_DTR_EVENT_EXT:  Register extended DTR-Toggle callback func
    SMD_IOCTL_DISABLE_DTR_EVENT_EXT: Dereg extended DTR-Toggle callback func
    SMD_IOCTL_ENABLE_CD_EVENT_EXT:   Register extended CD-Toggle callback func
    SMD_IOCTL_DISABLE_CD_EVENT_EXT:  Dereg extended CD-Toggle callback func
    SMD_IOCTL_ENABLE_RI_EVENT_EXT:   Register extended RI-Toggle callback func
    SMD_IOCTL_DISABLE_RI_EVENT_EXT:  Dereg extended RI-Toggle callback func
    SMD_IOCTL_ENABLE_CTS_EVENT_EXT:  Register extended CTS-Toggle callback func
    SMD_IOCTL_DISABLE_CTS_EVENT_EXT: Dereg extended CTS-Toggle callback func
    SMD_IOCTL_SET_BUFFER_SIZE:       Set the state of the Buffer Size control
    SMD_IOCTL_SET_DROP_PEND:         Set the state of the drop pending control
    SMD_IOCTL_SET_FULL_RCV_BUFS:     Set the state the Full Rcv Buffers control
    SMD_IOCTL_REGISTER_MEMCPY_RESET  Register a memcpy stream reset callback.    

ARGUMENTS     port_id - SMD port to control

              command - SMD IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

RETURN VALUE  In *param as required by the command

SIDE EFFECTS  None
===========================================================================*/
void smd_ioctl
(
  smd_port_id_type      port_id,
  smd_ioctl_cmd_type    command,
  smd_ioctl_param_type *param
)
{
  smd_stream_info_type *info;
  smd_cmd_type          cmd;

  SMD_VALIDATE_PORT_ID( port_id );
  info = smdi_get_stream_info( port_id );

  /*----------------------------------------------------------------------
   * Be careful when adding more commands. Commands that return a value
   * must be handled here (in the client context), other commands can be
   * handled here or in the SMD task. However, care must be taken for race
   * conditions with the SMD task since it also accesses the info structure.
   *----------------------------------------------------------------------*/

  switch ( command )
  {
    case SMD_IOCTL_DTE_READY_ASSERTED: /* Retrieve the state of the DTR signal */
      *(param->dte_ready_asserted) = info->prev_dtr;
      break;

    case SMD_IOCTL_GET_CURRENT_RTS: /* Retrieve the state of the RTS signal */
      *(param->rts_asserted) = info->prev_rts;
      break;

    case SMD_IOCTL_GET_FLOW_CTL:
      param->flow_ctl.tx_flow = info->tx_flow_control_method;
      param->flow_ctl.rx_flow = info->rx_flow_control_method;
      break;

    case SMD_IOCTL_GET_CURRENT_CD: /* Retrieve the state of the CD signal */
      *(param->cd_asserted) = info->prev_cd;
      break;

    case SMD_IOCTL_GET_CURRENT_RI: /* Retrieve the state of the RI signal */
      *(param->ri_asserted) = info->prev_ri;
      break;

    case SMD_IOCTL_GET_BUFFER_SIZE:
      param->buffer_size = info->max_queued_data;
      break;

    case SMD_IOCTL_GET_DROP_PEND:
      param->drop_asserted = info->tx_opening_drop_enabled;
      break;

    case SMD_IOCTL_GET_FULL_RCV_BUFS:
      param->full_bufs_asserted = info->ret_full_rcv_bufs_en;
      break;

    default:
      cmd.cmd = SMD_CMD_IOCTL;
      cmd.data.ioctl.port_id = port_id;
      cmd.data.ioctl.cmd = command;
      if( param != NULL )
      {
        cmd.data.ioctl.param = *param;
      }

      SMD_IOCTL_LOCK();
      info->pending_ioctl_cnt++;
      SMD_IOCTL_UNLOCK();

      smd_cmd( &cmd );
      break;
  }
} /* smd_ioctl */

/*===========================================================================
FUNCTION      smd_flush_tx

DESCRIPTION   Flushes the pending transmit data to the other processor.

              The callback is only called after the other processor has
              received all of the pending data. The flush operation will not
              complete until the SMD task calls the tx_callback_fn and does
              NOT get a another transmit buffer. (That is, the higher level
              transmit queue is empty as well.)

              If the other side of the port closes before the flush is
              complete, any data pending in the shared memory pipe is lost.

ARGUMENTS     port_id - SMD port to flush

              flush_cb - callback to call when other processor has received
                         all pending data

              cb_data - user data passed to the flush callback routine

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_flush_tx
(
  smd_port_id_type      port_id,
  smd_flush_callback_fn flush_cb,
  void                  *cb_data
)
{
  smd_cmd_type cmd;

  SMD_VALIDATE_PORT_ID( port_id );
  ASSERT( flush_cb != NULL );

  cmd.cmd = SMD_CMD_FLUSH;
  cmd.data.flush.port_id = port_id;
  cmd.data.flush.flush_cb = flush_cb;
  cmd.data.flush.cb_data = cb_data;

  smd_cmd( &cmd );
} /* smd_flush_tx */

/*===========================================================================
FUNCTION      smd_notify_read

DESCRIPTION   Notifies SMD task that a read buffer is now available for the
              given port_id.

ARGUMENTS     port_id - SMD port that should try to get a read buffer

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_notify_read( smd_port_id_type port_id )
{
  smd_stream_info_type *info;
  uint32 stream_state;

  SMD_VALIDATE_PORT_ID( port_id );

  info = smdi_get_stream_info( port_id );

  if(( info->tx_shared_info_ptr == NULL ) || 
     ( info->rx_shared_info_ptr == NULL ))
  { /* smd_allocate_stream_channel has not run yet */
    info->read_pending = TRUE;
  }
  else
  {
    stream_state = SMD_READ_CH_HDR(info, SMD_RX_HDR, stream_state );
    if((stream_state == SMD_SS_OPENED) || (stream_state == SMD_SS_FLUSHING))
    {/* the state is valid do the transmit */
      SMD_WRITE_IF_SIG( info, SMD_RX_HDR, data_written, TRUE );
      smd_event_send_smd( port_id );
    }
    else
    {/* in a transitional phase, set the pending flag*/
      info->read_pending = TRUE;
    }
  }
} /* smd_notify_read */

/*===========================================================================
FUNCTION      smd_notify_write

DESCRIPTION   Notifies SMD task that a transmit buffer is now available for
              the given port_id.

ARGUMENTS     port_id - SMD port that should try to get a transmit buffer

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_notify_write( smd_port_id_type port_id )
{
  smd_stream_info_type *info;
  uint32 stream_state;

  SMD_VALIDATE_PORT_ID( port_id );

  info = smdi_get_stream_info( port_id );

  if(( info->tx_shared_info_ptr == NULL ) || 
     ( info->rx_shared_info_ptr == NULL ))
  { /* smd_allocate_stream_channel has not run yet */
    info->transmit_pending = TRUE;
  }
  else
  {
    stream_state = SMD_READ_CH_HDR(info, SMD_TX_HDR, stream_state);
    if((stream_state == SMD_SS_OPENED) ||
       (stream_state == SMD_SS_FLUSHING))
    { /* The state is valid, do the transmit. */
      smd_cmd_type cmd;

      SMD_WRITE_IF_SIG( info, SMD_RX_HDR, data_read, TRUE );

      cmd.cmd = SMD_CMD_PROCESS_PORT;
      cmd.data.process_port.port_id = port_id;
      smd_cmd(&cmd);
    }
    else
    { /* In a transitional phase, set the pending flag*/
      info->transmit_pending = TRUE;
    }
  }
} /* smd_notify_write */

/*===========================================================================
FUNCTION      smd_api_version

DESCRIPTION   Returns the SMD API version. 

ARGUMENTS     None

RETURN VALUE  a 32-bit SMD version ID.

SIDE EFFECTS  None
===========================================================================*/
uint32 smd_api_version(void)
{
  return(SMD_API_VERSION);
}/* smd_api_version */

/*===========================================================================
  FUNCTION      smd_is_port_id_valid
===========================================================================*/
/**
  Verify that all the fields of the passed port_id are indicative of a 
  potentially-valid SMD port.

  Does not determine if such an SMD port has been allocated or is open.
  Clients and internal code can use this function to determine if the port_id
  is valid before passing it as an argument to other SMD functions.

  @param[in]  port_id    The port_id handle

  @return
  0     The port_id is not valid for a valid SMD port.
  1     The port_id is valid.
*/
/*=========================================================================*/
boolean smd_is_port_id_valid( smd_port_id_type port_id )
{
  if (PORT_GET_IDX(port_id) >= SMD_NUM_CHANNELS || 
      PORT_GET_REMOTE(port_id) >= SMEM_NUM_HOSTS)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
