/*=============================================================================

                            BAM DMUX APIs

 BAM Data Multiplexer public interface.

 Copyright  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/api/mproc/bam_dmux.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/16   rv      Initial version.
===========================================================================*/

#ifndef BAM_DMUX_H
#define BAM_DMUX_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/

/* BAM DMUX header size */
#define BAM_DMUX_HEADER_SIZE    (8)

/* Macro to initialize the open options structure */
#define BAM_DMUX_OPEN_OPTIONS_INIT(options) \
  do { \
    memset(options, 0, sizeof(bam_dmux_open_options); \
  } while(0)

/* Signature for validation to see if client has reserved the space for
 * BAM DMUX header in TX data buffer */
#define BAM_DMUX_HEADER_CLIENT_SIGNATURE  (0xF4520BAD)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/

/* The opaque handle returned by bam_dmux_open(). Client uses this handle to
 * call into BAM DMUX APIs for any further activity related to the channel */
typedef void* bam_dmux_handle;

/* The notification events client receive from BAM DMUX */
typedef enum {
    BAM_DMUX_EVENT_NONE           = 0,
  /* Channel is fully opened. Both local and remote have opened channel
   * from their respective ends, and data communication can now take place.
   * The bam_dmux_event_data->open is valid for this event.
   * Client TX data buffer size (including BAM DMUX header) should not exceed
   * the bam_dmux_event_data->open.remote_max_rx_buf_size */
  BAM_DMUX_EVENT_OPEN           = 1,

  /* After a failed bam_dmux_tx() with BAM_DMUX_STATUS_TX_ERROR error,
   * client should wait for this event to occur, so that the client
   * can continue the next transmit operation */
  BAM_DMUX_EVENT_TX_RESUME      = 2,

  /* Remote side channel is closed, no further data transfers are allowed */
  BAM_DMUX_EVENT_REMOTE_CLOSE   = 3,

  /* Channel is closed from both the sides, no further activity on channel.
   * After the client receives this event, the port may be immediately reopened.
   * see bam_dmux_close() */
  BAM_DMUX_EVENT_CLOSE_COMPLETE = 4,

  /* Power event. The bam_dmux_event_data->power is valid and 
   * bam_dmux_event_data->power.state gives the current power state */
  BAM_DMUX_EVENT_POWER          = 5,
} bam_dmux_event;

/* Event specific data */
typedef union {

  /* BAM_DMUX_EVENT_OPEN event Specific data, which is valid only for open event */
  struct {
    /* Max buffer size that remote is capable to receive. Local client
     * TX data size (including BAM DMUX header) should not exceed this size */
    uint16 remote_max_rx_buf_size;
  } open;

  /* BAM_DMUX_EVENT_POWER event Specific data, which is valid only for power event */
  struct {
    /* A2 current power state. TRUE mean power on and FALSE means power off */
    boolean state;
  } power;

} bam_dmux_event_data;

/* BAM DMUX status/error codes */
typedef enum {
  BAM_DMUX_STATUS_SUCESS           = 0,
  BAM_DMUX_STATUS_INVALID_ARGS     = -1,
  BAM_DMUX_STATUS_INVALID_CH_STATE = -2,
  /* TX (UL) is failed, clients need to wait for BAM_DMUX_EVENT_TX_RESUME to
   * send further (UL) data */
  BAM_DMUX_STATUS_TX_ERROR         = -3,
  BAM_DMUX_STATUS_TX_NO_HEADROOM   = -4,
} bam_dmux_status;

/* BAM DMUX channel open options data structure. Client should not directly modify
 * the member of structure. Client should call BAM_DMUX_OPEN_OPTIONS_INIT()
 * to initialize the structure, and use BAM_DMUX_OPEN_OPTIONS* macros
 * to set options */
typedef struct {
  uint8 unused; /* Place holder */
} bam_dmux_open_options;

/*=============================================================================

                     CALLBACK TYPE DEFINITIONS

=============================================================================*/

/*=============================================================================
  CALLBACK FUNCTION  bam_dmux_event_notify_cb
=============================================================================*/
/**
 * This callback function is called by the BAM DMUX to notify
 * the channel events.
 *
 * @param[in]  handle        Channel handle
 * @param[in]  priv          priv client data passed in bam_dmux_open()
 * @param[in]  event         Event (see bam_dmux_event enumeration)
 * @param[in]  event_data    Event specific data, see bam_dmux_event_data union
 *
 * @return  None
 *
 * @note
 * This callback function may be called from interrupt context; clients must
 * not block or call any functions that block. Clients can call BAM DMUX APIs.
 */
/*===========================================================================*/
typedef void (*bam_dmux_event_notify_cb)
(
  bam_dmux_handle     handle,
  void                *priv,
  bam_dmux_event      event,
  bam_dmux_event_data *event_data
);

/*=============================================================================
  CALLBACK FUNCTION  bam_dmux_tx_done_notify_cb
=============================================================================*/
/**
 * This callback function is called by the BAM DMUX to notify
 * that TX (UL) data is transferred to remote.
 *
 * @param[in]  handle     Channel handle
 * @param[in]  priv       priv client data passed in bam_dmux_open()
 * @param[in]  tx_priv    tx_priv client data passed in bam_dmux_tx()
 * @param[in]  data       TX data pointer (starting with BAM DMUX header)
 * @param[in]  data_len   TX data length (excluding BAM DMUX header length)
 * @param[in]  status     Status of the transfer (see bam_dmux_status enumeration)
 *
 * @return  None
 *
 * @note
 * This callback function may be called from interrupt context; clients must
 * not block or call any functions that block. Clients can call BAM DMUX APIs.
 */
/*===========================================================================*/
typedef void (*bam_dmux_tx_done_notify_cb)
(
  bam_dmux_handle handle,
  void            *priv,
  void            *tx_priv,
  void            *data,
  uint16          data_len,
  bam_dmux_status status
);

/*=============================================================================
  CALLBACK FUNCTION  bam_dmux_rx_notify_cb
=============================================================================*/
/**
 * This callback function is called by the BAM DMUX to notify about the
 * incoming RX (DL) data from remote. Client should call bam_dmux_rx_done()
 * once RX data is handled/processed.
 *
 * @param[in]  handle     Channel handle
 * @param[in]  priv       priv client data passed in bam_dmux_open()
 * @param[in]  rx_priv    This is internal to BAM DMUX, clients should
                          pass same rx_priv in bam_dmux_rx_done()
 * @param[in]  data       RX data pointer (excluding BAM DMUX header)
 * @param[in]  data_len   RX data length (excluding BAM DMUX header length)
 *
 * @return  None
 *
 * @note
 * This callback function may be called from interrupt context; clients must
 * not block or call any functions that block. Clients can call BAM DMUX APIs.
 */
/*===========================================================================*/
typedef void (*bam_dmux_rx_notify_cb)
(
  bam_dmux_handle handle,
  void            *priv,
  void            *rx_priv,
  void            *data,
  uint16          data_len
);

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

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
  uint8                      channel_id,
  void                       *priv,
  bam_dmux_open_options      *options,
  bam_dmux_event_notify_cb   event_cb,
  bam_dmux_tx_done_notify_cb tx_done_cb,
  bam_dmux_rx_notify_cb      rx_cb,
  bam_dmux_handle            *handle
);

/*=============================================================================
  FUNCTION  bam_dmux_tx
=============================================================================*/
/**
 * Sends (UL) data to remote. Client should allocate and leave space for
 * BAM DMUX header (of size BAM_DMUX_HEADER_SIZE) at the start of the data
 * buffer. Which means the total size of data buffer should be equal to actual
 * payload size plus BAM DMUX header size (BAM_DMUX_HEADER_SIZE).
 *
 * Clients should set BAM_DMUX_HEADER_CLIENT_SIGNATURE at the start of the
 * data buffer (which is start of BAM DMUX header). This is only for integrity
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
 * When this function is fails with BAM_DMUX_STATUS_TX_ERROR error code,
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
);

/*=============================================================================
  FUNCTION  bam_dmux_rx_done
=============================================================================*/
/**
 * Client uses this to indicate the BAM DMUX that it is done with the
 * received RX (DL) data (which was given to client in the bam_dmux_rx_notify_cb()
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
);

/*=============================================================================
  FUNCTION  bam_dmux_close
=============================================================================*/
/**
 * Closes an opened channel.
 *
 * After calling this function, clients must wait for
 * BAM_DMUX_EVENT_CLOSE_COMPLETE event before reopening the port.
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
);

#ifdef __cplusplus
}
#endif

#endif /* BAM_DMUX_H */
