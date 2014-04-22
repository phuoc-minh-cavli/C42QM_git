#ifndef _SMD_LOOPBACK_CLIENT_H_
#define _SMD_LOOPBACK_CLIENT_H_

/*===========================================================================

                S M D   L O O P B A C K   C L I E N T   A P I

===========================================================================*/
/**
  @file smd_loopback_client.h
*/
/* 
  Declares the private API and structures used to implement the N-way shared 
  memory driver loopback clients.
*/

/*===========================================================================
    Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_loopback_client.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/12   bt      Add required smd.h include.
05/04/11   bt      Initial implementation.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "smd_loopback_n_way.h"
#include "smd.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

/* Used by the function \c smd_loopback_signal to indicate which signal is 
 * to be modified. */
typedef enum
{
  SMD_LOOPBACK_SIGNAL_DTR,
  SMD_LOOPBACK_SIGNAL_CTS,
  SMD_LOOPBACK_SIGNAL_CD,
  SMD_LOOPBACK_SIGNAL_RI
} smd_loopback_signal_type;

/* All loopback functions must use a callback following this prototype (unless
 * it is NULL).  The value of the argument \c success indicates whether the 
 * loopback function was ultimately successful (true) or not (false). */
typedef void (*smd_loopback_cb_type) (boolean success, void *data);

struct smd_loopback_handle_struct;

/* Handle pointing to the internal data structures.
 * Loopback clients must not take advantage of the internal structure of this 
 * handle. */
typedef struct smd_loopback_handle_struct * smd_loopback_handle_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
  FUNCTION  smd_loopback_client_update
===========================================================================*/
/**
  Checks every existing N-way SMD Loopback edge and appropriately performs a 
  client state transition or calls a client callback.
  
  Called from smd_main() after all other events have been serviced, if the 
  host processor is a valid client for any interprocessor edge.  Will exit 
  without doing anything if no outstanding response from any server is 
  expected.

  @return
  None.

  @dependencies
  None.
*/
/*==========================================================================*/
void smd_loopback_client_update( void );

/*===========================================================================
  FUNCTION  smd_loopback_open
===========================================================================*/
/**
  Tells the server to open an SMD loopback port identified by the name and 
  edge, with the provided characteristics (buffer size, API, transfer type, 
  and loopback mode).

  When the client host is notified that the server has finished the open 
  operation, the client host will call the client-provided cb_func, using 
  cb_data as a parameter.

  @param[in] name        The name of the SMD loopback port to open.  The 
                         length of the name is defined as
                         SMD_CHANNEL_NAME_SIZE_MAX, where the last character 
                         is always null. Do not use whitespace in port_name.
  @param[in] edge        Indicates which two processors will be communicating
                         on this port.
  @param[in] fifo_size   The SMD FIFO size, in bytes.  Must be a multiple
                         of 32 and must not be greater than #SMD_MAX_FIFO.
                         Some processors may have certain other restrictions.
  @param[in] server_api  Indicates whether SMD memcpy, SMD Lite, or SMD DSM 
                         should be used to implement the server's open, close, 
                         signaling, and read/write functionality.
  @param[in] xfr_type    Packet, Streaming, or legacy mode, if supported by 
                         the server_api.
  @param[in] mode        The server will Echo, Source, or Sink data.
  @param[in] cb_func     The client callback to run once the open finishes 
                         (on failure or success).
  @param[in] cb_data     The client data to pass to the callback, if necessary.

  @return       A handle pointing to the internal data structures used by the
                loopback code on the client side.

  @sideeffect   Initializes a loopback command structure in shared memory.

  @dependencies
              - Shared memory must have been initialized on this processor by
                calling smem_init().
              - The Shared Memory Driver must be up and running. The startup
                process is platform-specific.
*/
/*=========================================================================*/
smd_loopback_handle_type smd_loopback_open
(
  const char               *name,
  smd_channel_type          edge,
  uint32                    fifo_size,
  smd_loopback_api_type     server_api,
  smd_xfrflow_type          xfr_type,
  smd_loopback_mode_type    mode,
  smd_loopback_cb_type      cb_func,
  void                     *cb_data
);

/*===========================================================================
  FUNCTION  smd_loopback_close
===========================================================================*/
/**
  Tells the server to close an SMD loopback port identified by the handle.

  When the client host is notified that the server has finished the close 
  operation, the client host will call the client-provided cb_func, using 
  cb_data as a parameter.

  @param[in] handle   The handle returned for this port by smd_loopback_open().
  @param[in] cb_func  The client callback to run once the close finishes.
  @param[in] cb_data  The client data to pass to the callback, if necessary.

  @return             True if the command is successfully sent to the server.
                      False if the port is in an inappropriate state to close.

  @dependencies
  The port must have been opened on the server side already.  Both the client
  and server-side loopback states should be in SMD_LOOPBACK_STATE_OPEN.
*/
/*=========================================================================*/
boolean smd_loopback_close
(
  smd_loopback_handle_type  handle,
  smd_loopback_cb_type      cb_func,
  void                     *cb_data
);

/*===========================================================================
  FUNCTION  smd_loopback_signal
===========================================================================*/
/**
  Tells the server to set a particular signal on its side of an SMD loopback 
  port identified by the handle.  The value may not necessarily be different 
  from the current server signal's value.

  When the client host is notified that the server has finished the signaling 
  operation, the client host will call the client-provided cb_func, using 
  cb_data as a parameter.

  @param[in] handle   The handle returned for this port by smd_loopback_open().
  @param[in] signal   The signal that should be set.
  @param[in] value    The value the signal should be set to.
  @param[in] cb_func  The client callback to run once the signal finishes.
  @param[in] cb_data  The client data to pass to the callback, if necessary.

  @return             True if the command is successfully sent to the server.
                      False if the port is in an inappropriate state to close.

  @dependencies
  The port must have been opened on the server side already.  Both the client
  and server-side loopback states should be in SMD_LOOPBACK_STATE_OPEN.
*/
/*=========================================================================*/
boolean smd_loopback_signal
(
  smd_loopback_handle_type  handle,
  smd_loopback_signal_type  signal,
  boolean                   value,
  smd_loopback_cb_type      cb_func,
  void                     *cb_data
);

#ifdef __cplusplus
}
#endif

#endif /* _SMD_LOOPBACK_CLIENT_H_ */
