/*=============================================================================

                            BAM DMUX STUB

 BAM Data Multiplexer stub.

 Copyright  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/bam_dmux/src/bam_dmux_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "bam_dmux.h"


/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DEFINATION

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                     LOCAL FUNCTION DEFINATION

=============================================================================*/


/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  bam_dmux_init
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void bam_dmux_init(void)
{
}

/*=============================================================================
  FUNCTION  bam_dmux_open
=============================================================================*/
/**
 * Opens a logical channel.
 *
 * @param[in]  channel_id    The logical channel id
 * @param[in]  priv          Private data for client to maintain context
 *                           This data is passed back to client in the
 *                           notification callbacks
 * @param[in]  options       Channel open options (see BAM_DMUX_OPEN_OPTIONS*)
 * @param[in]  event_cb      Client notification callback function, which will
                             be called to notify about the channel events
                             (see bam_dmux_event enumeration)
 * @param[in]  tx_done_cb    Client notification callback function, which will
                             be called when TX transfer is completed.
 * @param[in]  rx_cb         Client notification callback function, which will
                             be called when there is an incoming (RX) data
                             from remote
 * @param[out] handle        The handle to be used to interact with this
 *                           channel on a successful open
 *
 * @return
 * Sets the handle and returns BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_open
(
  uint8                       channel_id,
  void	                     *priv,
  bam_dmux_open_options      *options,
  bam_dmux_event_notify_cb    event_cb,
  bam_dmux_tx_done_notify_cb  tx_done_cb,
  bam_dmux_rx_notify_cb       rx_cb,
  bam_dmux_handle            *handle
)
{
  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_tx
=============================================================================*/
/**
 * Sends data to remote. Client should allocate and leave space for
 * BAM DMUX header (of size BAM_DMUX_HEADER_SIZE) at the start of the data
 * buffer. Which means the total size of data buffer should be equal to actual
 * payload size plus BAM DMUX header size (BAM_DMUX_HEADER_SIZE).
 *
 * Clients should set BAM_DMUX_HEADER_CLIENT_SIGNATURE at the start of the
 * data point (which is start of BAM DMUX header). This is only for integrity
 * check to ensure client has reserved the space for BAM DMUX header.
 * BAM DMUX will check this signature before start sending the data.
 *
 * Client TX data buffer size (including BAM DMUX header) should not exceed
 * the remote max RX buffer size. BAM DMUX shares/notify the max remote RX
 * buffer size with BAM_DMUX_EVENT_OPEN event data (see bam_dmux_event_data union)
 * to clients.
 *
 * |-----------------|---------------------------------------------|
 * | BAM DMUX header | Client TX data                              |
 * |-----------------|---------------------------------------------|
 *
 *
 * @param[in]  handle    Channel handle returned in the bam_dmux_open()
 * @param[in]  tx_priv   Client private data for this TX transfer
 * @param[in]  data      TX data pointer (starting with BAM DMUX header)
 *                       buffer should be allocated with 4 bytes aligned
 * @param[in]  data_len  TX data length (excluding BAM DMUX header length)
 *
 * @return
 * BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 * BAM DMUX calls bam_dmux_tx_done_notify_cb() notification callback once
 * data is transferred to remote.
 *
 * When this function is fails with BAM_DMUX_STATUS_TX_NO_SPACE error code,
 * BAM DMUX calls bam_dmux_event_notify_cb() callback function with
 * BAM_DMUX_EVENT_TX_RESUME event when further bam_dmux_tx() are allowed.
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_tx
(
  bam_dmux_handle handle,
  void            *tx_priv,
  void            *data,
  uint16          data_len
)
{
  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_rx_done
=============================================================================*/
/**
 * Client uses this to indicate the BAM DMUX that it is done with the
 * received(RX) data (which was given to client in the bam_dmux_rx_notify_cb()
 * notification callback).
 *
 * @param[in]  handle    Channel handle returned in the bam_dmux_open()
 * @param[in]  rx_priv   BAM DMUX private data for the RX transfer
 *
 * @return
 * BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_rx_done
(
  bam_dmux_handle handle,
  void            *rx_priv
)
{
  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_close
=============================================================================*/
/**
 * Closes an opened channel.
 *
 * @param[in]  handle    Channel handle returned in the bam_dmux_open()
 *
 * @return
 * BAM_DMUX_STATUS_SUCESS if function is successful,
 * Otherwise returns error code (see bam_dmux_status enumeration).
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_close
(
  bam_dmux_handle handle
)
{
  return BAM_DMUX_STATUS_SUCESS;
}

/*=============================================================================
  FUNCTION  bam_dmux_clock_vote
=============================================================================*/
/**
 * TODO
 *
 * @param[in]
 *
 * @return
 *
 * @note
 */
/*===========================================================================*/
bam_dmux_status bam_dmux_clock_vote
(
  bam_dmux_handle handle,
  boolean         vote
)
{
  return BAM_DMUX_STATUS_SUCESS;
}
