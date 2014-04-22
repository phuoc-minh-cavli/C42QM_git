/*=============================================================================

                            RSP COMMUNICATION APIs

 
 Copyright  2018 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/api/mproc/rspcom.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/07/18   rv      Initial release of RSPCOM APIs
===========================================================================*/

#ifndef RSPCOM_H
#define RSPCOM_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "stdlib.h"
#include "stdint.h"

/*=============================================================================

                         MACRO DECLARATIONS

=============================================================================*/
#define RSPCOM_OPEN_CFG_INIT(open_cfg_ptr) \
  do { \
    memset(open_cfg_ptr, 0, sizeof(rspcom_open_cfg_type));\
  } while(0)

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
/**
 * Opaque handle returned by rspcom_open(). Client uses this handle to call into
 * RSPCOM API for any further operations.
 *
 * Client must not modify or try to interpret this value.
 */
typedef void* rspcom_handle_type;

/** RSPCOM status/return codes */
typedef enum
{
  /* Success */
  RSPCOM_ERR_NONE               = 0,
  
  /* Invalid parameter is passed to RSPCOM functions */
  RSPCOM_ERR_INVALID_PARAM      = -1,

  RSPCOM_ERR_NOT_INITED         = -2,
  RSPCOM_ERR_XPORT_DISCONNECTED = -3,
  RSPCOM_ERR_OUT_OF_RESOURCES   = -4,
  RSPCOM_ERR_NOT_SUPPORTED      = -5
  
} rspcom_err_type;

/** RSPCOM connection state */
typedef enum
{
  /* Debugger client is disconnected */
  RSPCOM_CONN_STATE_DISCONNECTED = 0,
  
  /* Debugger client is connected */
  RSPCOM_CONN_STATE_CONNECTED    = 1,
} rspcom_conn_state_type;

/** Packet type sending from debug server to client */
typedef enum 
{
  /* Packet contains char type data */
  RSPCOM_PKT_FMT_CHAR = 0,
  /* Packet contains binary type data */
  RSPCOM_PKT_FMT_BIN = 1,
} rspcom_pkt_fmt_type;

/** RSPCOM configurations */
typedef enum
{
  RSPCOM_CFG_NONE = 0,
  
  /* Disable the normal '+'/'-' protocol acknowledgments  */
  RSPCOM_CFG_DISABLE_ACK = 1,
} rspcom_cfg_type;

/** RSPCOM connection state callback function type */
typedef void (*rspcom_state_callback_type)
(
  rspcom_handle_type     handle,    /* Handle for the RSPCOM */
  void                   *priv,     /* priv client data passed in rspcom_open */
  rspcom_conn_state_type conn_state /* Connection state */
);

/** RSPCOM RX command callback function type */
typedef void (*rspcom_rx_cmd_pkt_callback_type)
(
  rspcom_handle_type handle,       /* Handle for the RSPCOM */
  void               *priv,        /* priv client data passed in rspcom_open */
  void               *cmd_pkt_buf, /* Pointer to the command packet received from debug client. 
                                    * Pointer is only valid till the return from 
                                    * this callback function */
  size_t             cmd_pkt_size  /* Received command packet size */
);

/** RSPCOM Open configuration  */
typedef struct
{
  /* Private data to maintain context. 
   * This is passed back in the callbacks */
  void *priv; 
  
  /* Session name or PD name or DAM applications  name */
  /* TODO: For initial design considering only one processor/subsystem 
   * running multiple applications and applications are separated 
   * by the name or name with ID */
  const char *app_name;
  /* TODO: App id */
  
  /* Callback functions */
  rspcom_state_callback_type      state_cb;
  rspcom_rx_cmd_pkt_callback_type rx_cmd_cb;

  /* Max TX packet size */
  size_t max_tx_pkt_size;
  
  /* Max RX packet size */
  size_t max_rx_pkt_size;
} rspcom_open_cfg_type;

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  rspcom_open
=============================================================================*/
/**
 * Opens a handle to interact with RSPCOM.
 *
 * @param[in]  open_config   Pointer to the open configuration structure.
 *                           This structure must filled by the clients.
 *                           Use RSPCOM_OPEN_CFG_INIT() macro function to 
 *                           initialize the structure before setting any 
 *                           member of structure.
 * @param[out]  handle       The handle to be used to interact with RSPCOM.
 *
 * @return
 * Sets the handle and returns RSPCOM_ERR_NONE if function is successful,
 * otherwise returns status code (see rspcom_err_type enumeration).
 *
 */
/*===========================================================================*/
rspcom_err_type rspcom_open
(
  rspcom_open_cfg_type *open_cfg,
  rspcom_handle_type   *handle
);

/*=============================================================================
  FUNCTION  rspcom_close
=============================================================================*/
/**
 * Closes the specified handle. No further operation are allowed with RSPCOM.
 *
 * @param[in/out]  handle     Handle return by rspcom_open()..
 *
 * @return
 * Sets the handle to NULL and returns RSPCOM_ERR_NONE if function is
 * successful, otherwise returns status code (see rspcom_err_type enumeration).
 *
 */
/*===========================================================================*/
rspcom_err_type rspcom_close
(
  rspcom_handle_type *handle
);

/*=============================================================================
  FUNCTION  rspcom_tx_resp_pkt
=============================================================================*/
/**
 * Sends response packet to the debug client. 
 * Response packet form: "$ data # checksum"
 *     'data' is the resp_pkt_buf content.
 *     RSPCOM adds the $, # and checksum.
 *
 * @param[in]  handle           Handle return by rspcom_open()..
 * @param[in]  pkt_fmt          Packet format (see rspcom_pkt_fmt_type)
 * @param[in]  resp_pkt_buf     Pointer to the response packet buffer. 
 * @param[in]  resp_pkt_size    Response packet buffer size.
 *
 * @return
 * RSPCOM_ERR_NONE if function is successful,
 * Otherwise returns error code (see rspcom_err_type enumeration).
 *
 * @sideeffects
 * "$#00" packet will be sent if resp_pkt_buf is NULL or resp_pkt_size is zero.
 *
 */
/*===========================================================================*/
rspcom_err_type rspcom_tx_resp_pkt
(
  rspcom_handle_type  handle,
  rspcom_pkt_fmt_type pkt_fmt,
  void                *resp_pkt_buf,
  size_t              resp_pkt_size
);

/*=============================================================================
  FUNCTION  rspcom_tx_notify_pkt
=============================================================================*/
/**
 * Sends notification packet to the debug client. 
 * Notification packet form: "% data # checksum"
 *     'data' is the notify_pkt_buf content.
 *     RSPCOM adds the %, # and checksum.
 *
 * @param[in]  handle           Handle return by rspcom_open()..
 * @param[in]  pkt_fmt          Packet format (see rspcom_pkt_fmt_type)
 * @param[in]  notify_pkt_buf   Pointer to the notification packet buffer. 
 * @param[in]  notify_pkt_size  Notification packet buffer size.
 *
 * @return
 * RSPCOM_ERR_NONE if function is successful,
 * Otherwise returns error code (see rspcom_err_type enumeration).
 *
 * @sideeffects
 * "%#00" packet will be sent if notify_pkt_buf is NULL or 
 * notify_pkt_size is zero.
 *
 */
/*===========================================================================*/
rspcom_err_type rspcom_tx_notify_pkt
(
  rspcom_handle_type  handle,
  rspcom_pkt_fmt_type pkt_fmt,
  void                *notify_pkt_buf,
  size_t              notify_pkt_size
);

/*=============================================================================
  FUNCTION  rspcom_set_cfg
=============================================================================*/
/**
 * Configures the RSPCOM connection.
 *
 * @param[in]  handle         Handle return by rspcom_open()..
 * @param[in]  cfg            Configuration type (see rspcom_cfg_type)
 * @param[in]  cfg_data       Configuration data buffer if any (based on rspcom_cfg_type)
 * @param[in]  cfg_data_size  Configuration data buffer size.
 *
 * @return
 * RSPCOM_ERR_NONE if function is successful,
 * Otherwise returns error code (see rspcom_err_type enumeration).
 *
 */
/*===========================================================================*/
rspcom_err_type rspcom_set_cfg
(
  rspcom_handle_type handle,
  rspcom_cfg_type    cfg,
  void               *cfg_data,
  size_t             cfg_data_size
);

#ifdef __cplusplus
}
#endif

#endif /* RSPCOM_H */


