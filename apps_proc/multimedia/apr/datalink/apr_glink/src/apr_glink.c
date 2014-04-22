/*=============================================================================
  Copyright (C) 2015-2016 QUALCOMM Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - QUALCOMM Technologies, Inc.

===============================================================================
                                 Edit History

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia2/apr/apr2/mainline/main/latest/datalink/apr_glink/src/apr_glink.c
  $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ------------------------------------------------------------
08/08/2015   rs    Initial version.
=============================================================================*/

/******************************************************************************
 * Includes                                                                   *
 *****************************************************************************/
#include "mmstd.h"
#include "apr_dbg_msg.h"
#include "apr_errcodes.h"
#include "apr_lock.h"
#include "apr_list.h"
#include "apr_glink.h"
#include "glink.h"

//#define APR_GLINK_QOS_SUPPORT

#ifdef APR_GLINK_QOS_SUPPORT
  #include "glink_qos.h"
#endif

/******************************************************************************
 * Defines                                                                    *
 *****************************************************************************/

#define APR_GLINK_MAX_PORTS ( 16 )  //changing from 8 to 16
  /**< Max number of APR GLINK ports. */

#define APR_GLINK_PORT_HANDLE_BASE  ( 1 )
  /**< Used to make sure port handle are positive ( starting from 1 ). */

#define APR_GLINK_128_BYTES_BUFFER ( 128 )
#define APR_GLINK_512_BYTES_BUFFER ( 512 )
#define APR_GLINK_1024_BYTES_BUFFER ( 1024 )
#define APR_GLINK_4096_BYTES_BUFFER ( 4096 )

#define APR_GLINK_NUM_OF_128_BYTES_INTENTS ( 64 )
#define APR_GLINK_NUM_OF_512_BYTES_INTENTS ( 32 )
#define APR_GLINK_NUM_OF_1024_BYTES_INTENTS ( 16 )
#define APR_GLINK_NUM_OF_4096_BYTES_INTENTS ( 04 )

/******************************************************************************
 * Type Definitions                                                           *
 *****************************************************************************/

typedef uint32_t apr_glink_state_t;

typedef struct apr_glink_port_s {
  apr_glink_state_t state;
  apr_glink_port_handle_t port_handle;
  char_t port_name[ APR_GLINK_MAX_CHANNEL_NAME_SIZE ];
  uint32_t port_name_size;
  uint32_t enable_intents;
  uint32_t qos_latency_us;
  uint32_t qos_pkt_size;
  void *cb_data;
  apr_glink_rx_cb_t rx_cb;
  apr_glink_tx_done_cb_t tx_done_cb;
  apr_glink_state_cb_t state_cb;
  glink_handle_type glink_handle;
} apr_glink_port_t;

typedef struct apr_glink_port_item_s {
  apr_list_node_t link;
  apr_glink_port_t port;
} apr_glink_port_item_t;

/******************************************************************************
 * Variables                                                                  *
 *****************************************************************************/

static apr_list_t apr_glink_free_port_list;
static apr_glink_port_item_t apr_glink_port_pool[ APR_GLINK_MAX_PORTS ];
static apr_lock_t apr_glink_free_port_list_lock;

static char *apr_glink_remote_ss[] = {
  "apss",   /* Application Processor Subsystem */
  "mpss",   /* Modem subsystem */
  "lpass",  /* Low Power Audio Subsystem */
  "dsps",   /* Sensors Processor */
  "wcnss",  /* Wireless Connectivity Subsystem */
  "rpm",    /* Resource Power Manager processor */
};

/******************************************************************************
 * Core Routine Implementations                                               *
 *****************************************************************************/

static void apr_glink_free_port_list_lock_fn ( void )
{
  ( void ) apr_lock_enter( apr_glink_free_port_list_lock );
}

static void apr_glink_free_port_list_unlock_fn ( void )
{
  ( void ) apr_lock_leave( apr_glink_free_port_list_lock );
}

/** Data receive notification callback */  
static void apr_glink_rx_cb (
  glink_handle_type handle,     /* handle for the glink channel */
  const void        *priv,      /* priv client data passed in glink_open */
  const void        *pkt_priv,  /* private client data assiciated with the
                                   rx intent that client queued earlier */
  const void        *ptr,       /* pointer to the received buffer */
  size_t            size,       /* size of the packet */
  size_t            intent_used /* size of the intent used for this packet */
)
{
  int32_t rc;
  apr_glink_port_item_t *port_item;
  apr_glink_port_t *port;
  glink_err_type glink_rc;

 // APR_DBG_1(APR_DBG_HIGH, "APPS apr_glink_rx_cb(): rcvd with APR packet 0x%x", ptr);

  if (NULL == ptr)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_rx_cb(): Rcvd NULL buffer ptr");
    return;
  }

  if (NULL == priv)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_rx_cb(): Rcvd NULL priv pointer");
    return;
  }

  port_item = (apr_glink_port_item_t *)priv;
  port = &port_item->port;
  if (NULL == port)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_rx_cb(): port is NULL");
    return;
  }

  for ( ;; )
  {
    if (NULL == port->rx_cb)
    {
      APR_DBG_0(APR_DBG_ERROR, "apr_glink_rx_cb(): rx_cb is NULL");
      break;
    }

    rc = port->rx_cb(port->cb_data, (void *)ptr, size);
    if (rc)
    {
      APR_DBG_1(APR_DBG_ERROR,
              "apr_glink_rx_cb(): Client cb returned error %d", rc);
      break;
    }

    return;
  }

  /* Failed to deliver the packet. Release the packet */
  if (GLINK_STATUS_SUCCESS !=
                 (glink_rc = glink_rx_done(port->glink_handle, ptr, TRUE)))
  {
    APR_DBG_1(APR_DBG_ERROR, "glink_rx_done() failed with rc = %d", glink_rc);
  }

  return;
}

/** Data transmit notification callback */  
static void apr_glink_tx_done_cb (
  glink_handle_type handle,    /* handle for the glink channel */
  const void        *priv,     /* priv client data passed in glink_open */
  const void        *pkt_priv, /* private client data assiciated with the
                                  tx pkt that client queued earlier */
  const void        *ptr,      /* pointer to the transmitted buffer */
  size_t            size       /* size of the packet */
)
{
  int32_t rc;
  apr_glink_port_item_t *port_item;
  apr_glink_port_t *port;

  //APR_DBG_1(APR_DBG_HIGH, "APPS apr_glink_tx_done_cb() rcvd with APR packet 0x%x", ptr);

  if (NULL == ptr)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_tx_done_cb(): Rcvd NULL buffer ptr");
    return;
  }

  if (NULL == priv)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_tx_done_cb(): Rcvd NULL priv pointer");
    return;
  }

  port_item = (apr_glink_port_item_t *)priv;
  port = &port_item->port;
  if (NULL == port)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_tx_done_cb(): port is NULL");
    return;
  }

  if (NULL == port->tx_done_cb)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_tx_done_cb(): tx_done_cb is NULL");
    return;
  }

  rc = port->tx_done_cb(port->cb_data, (void *)ptr, size);
  if (rc)
  {
    APR_DBG_1(APR_DBG_ERROR,
            "apr_glink_tx_done_cb(): Client cb returned error %d", rc);
  }

  return;
}

APR_INTERNAL glink_err_type apr_glink_queue_rx_intents (
  uint32_t intent_size, 
  uint32_t num_of_intents,
  glink_handle_type glink_handle
)
{
  uint32_t index;
  glink_err_type glink_rc;

  /* Queue rx intents */
  for (index = 0; index < num_of_intents; index++)
  {
    if (GLINK_STATUS_SUCCESS != (glink_rc =
         glink_queue_rx_intent(glink_handle, NULL, intent_size)))
    {
        APR_DBG_2(APR_DBG_ERROR,
        "glink_queue_rx_intent() failed for size = %d @ count = %d.",
                                                   intent_size, index+1);
       return glink_rc;
     }
  }
  return GLINK_STATUS_SUCCESS;
  
}


/** GLink channel state change notification callback */  
static void apr_glink_state_cb (
  glink_handle_type         handle, /* handle for the glink channel */
  const void                *priv,  /* priv client data passed in glink_open */
  glink_channel_event_type  event   /* Notification event */
)
{
  int32_t rc;
  apr_glink_port_item_t *port_item;
  apr_glink_port_t *port;
  glink_err_type glink_rc;

  APR_DBG_1(APR_DBG_HIGH, "APPS apr_glink_state_cb() rcvd with event = 0x%x", event);

  if (NULL == priv)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_state_cb(): Rcvd NULL priv pointer");
    return;
  }

  port_item = (apr_glink_port_item_t *)priv;
  port = &port_item->port;
  if (NULL == port)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_state_cb(): port is NULL");
    return;
  }

  switch(event)
  {
    case GLINK_CONNECTED:
      port->state = APR_GLINK_CONNECTED;

	  
      /* Queue rx intents */

	  if ( port->enable_intents )
	  {
	    glink_rc = apr_glink_queue_rx_intents ( APR_GLINK_128_BYTES_BUFFER, 
			                                    APR_GLINK_NUM_OF_128_BYTES_INTENTS, port->glink_handle );
		if ( glink_rc != GLINK_STATUS_SUCCESS )
		{
          (void) glink_close(port->glink_handle);
          port->state = APR_GLINK_DISCONNECTING;
          return;			
		}
		
		glink_rc = apr_glink_queue_rx_intents ( APR_GLINK_512_BYTES_BUFFER, 
			                                    APR_GLINK_NUM_OF_512_BYTES_INTENTS, port->glink_handle);
		if ( glink_rc != GLINK_STATUS_SUCCESS )
		{
          (void) glink_close(port->glink_handle);
          port->state = APR_GLINK_DISCONNECTING;
          return;			
		}
		
		glink_rc = apr_glink_queue_rx_intents ( APR_GLINK_1024_BYTES_BUFFER, 
			                                    APR_GLINK_NUM_OF_1024_BYTES_INTENTS, port->glink_handle );
		if ( glink_rc != GLINK_STATUS_SUCCESS )
		{
          (void) glink_close(port->glink_handle);
          port->state = APR_GLINK_DISCONNECTING;
          return;			
		}
		
		glink_rc = apr_glink_queue_rx_intents ( APR_GLINK_4096_BYTES_BUFFER, 
			                                    APR_GLINK_NUM_OF_4096_BYTES_INTENTS, port->glink_handle );
		if ( glink_rc != GLINK_STATUS_SUCCESS )
		{
          (void) glink_close(port->glink_handle);
          port->state = APR_GLINK_DISCONNECTING;
          return;			
		}		
	  }

 
#ifdef APR_GLINK_QOS_SUPPORT
      /* Set QoS Latency requirements */
      if (0 != port->qos_latency_us && 0 != port->qos_pkt_size)
      {
        if (GLINK_STATUS_SUCCESS != (glink_rc = glink_qos_latency(
                port->glink_handle, port->qos_latency_us, port->qos_pkt_size)))
        {
          APR_DBG_1(APR_DBG_ERROR,
                           "glink_qos_latency() failed with rc = %d.", glink_rc);
          (void) glink_close(port->glink_handle);
          port->state = APR_GLINK_DISCONNECTING;
          return;
        }
      }
#endif

      break;

    case GLINK_REMOTE_DISCONNECTED:
      port->state = APR_GLINK_REMOTE_DISCONNECTED;
      break;

    case GLINK_LOCAL_DISCONNECTED:
      port->state = APR_GLINK_DISCONNECTED;
      (void) apr_list_add_tail(&apr_glink_free_port_list,
                                   (apr_list_node_t *)&port_item->link);
      break;

    default:
      return;
  }

  if (NULL == port->state_cb)
  {
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_state_cb(): state_cb is NULL");
    return;
  }

  rc = port->state_cb(port->port_handle, port->state);
  if (rc)
  {
    APR_DBG_1(APR_DBG_ERROR,
            "apr_glink_state_cb(): Client cb returned error %d", rc);
  }

  APR_DBG_0(APR_DBG_HIGH,
            "apr_glink_state_cb(): Done CB" );

  return;
}

APR_INTERNAL int32_t apr_glink_init ( glink_link_state_notif_cb link_state_cb )
{
  int32_t rc;
  uint32_t checkpoint = 0;
  uint32_t index;
  glink_link_id_type link_id; 
  glink_err_type err;

  APR_DBG_0(APR_DBG_ERROR, "apr_glink_init(): APPS Inside apr_glink_init ");

  for ( ;; )
  {
    rc = apr_lock_create(APR_LOCK_TYPE_MUTEX, &apr_glink_free_port_list_lock);
    if (rc) break;
    checkpoint = 1;

    rc = apr_list_init(&apr_glink_free_port_list,
         apr_glink_free_port_list_lock_fn, apr_glink_free_port_list_unlock_fn);
    if (rc) break;

    for (index = 0; index < APR_GLINK_MAX_PORTS; index++)
    {
      apr_glink_port_pool[index].port.port_handle = index;
      apr_glink_port_pool[index].port.state = APR_GLINK_DISCONNECTED;
      (void) apr_list_add_tail(&apr_glink_free_port_list,
                                             &apr_glink_port_pool[index].link);
    }

    APR_DBG_0(APR_DBG_ERROR, "apr_glink_init(): APPS Before registration to link state ");
    /* Initialize the structure */
    GLINK_LINK_ID_STRUCT_INIT(link_id);
    link_id.xport = "SMEM";
    link_id.remote_ss = "mpss";
    link_id.link_notifier = link_state_cb;

    err = glink_register_link_state_cb(&link_id, NULL);
    if (err == GLINK_STATUS_SUCCESS)
    {
      /*TODO: Store the handle here*/
       APR_DBG_1(APR_DBG_ERROR, "apr_glink_init(): APPS Registration success %d ", err);
    }
    else
    {
      APR_DBG_1(APR_DBG_ERROR, "apr_glink_init(): APPS Registration failure %d ", err);
    }
    APR_DBG_0(APR_DBG_ERROR, "apr_glink_init(): APPS After registration to link state ");

    return APR_EOK;
  }

  switch (checkpoint)
  {
    case 1:
      (void)apr_lock_destroy(apr_glink_free_port_list_lock);
      /*-fallthru */
    default:
      break;
  }

  APR_DBG_1(APR_DBG_ERROR,
              "apr_glink_init: error after checkpoint %d.", checkpoint);

  return rc;
}

APR_INTERNAL int32_t apr_glink_deinit ( void )
{
  (void) apr_list_destroy(&apr_glink_free_port_list);
  (void) apr_lock_destroy(apr_glink_free_port_list_lock);

  APR_DBG_0(APR_DBG_HIGH, "apr_glink_deinit() Inside DEINIT");

  return APR_EOK;
}

APR_INTERNAL int32_t apr_glink_open (
  char_t *port_name,
  uint32_t port_name_size,
  uint32_t remote_ssid,
  uint32_t enable_intents,
  uint32_t qos_latency_us,
  uint32_t qos_pkt_size,
  void *cb_data,
  apr_glink_rx_cb_t rx_cb,
  apr_glink_tx_done_cb_t tx_done_cb,
  apr_glink_state_cb_t state_cb,
  apr_glink_port_handle_t *port_handle
)
{
  int32_t rc;
  uint32_t checkpoint = 0;
  uint32_t remote_ss_index;
  apr_glink_port_item_t *port_item = NULL;
  apr_glink_port_t *port = NULL;
  glink_open_config_type glink_cfg; 
  glink_err_type glink_rc;

  if (NULL == port_name || NULL == rx_cb || NULL == tx_done_cb ||
          NULL == port_handle || port_name_size < 2 /*1 char + '\0'*/ ||
                              port_name_size > APR_GLINK_MAX_CHANNEL_NAME_SIZE)
  {
    return APR_EBADPARAM;
  }

  APR_DBG_1(APR_DBG_ERROR, "apr_glink_open(): APPS Inside glink open with remote ssid %d", remote_ssid);

  mmstd_memset(&glink_cfg, 0, sizeof(glink_cfg));
  switch(remote_ssid)
  {
    case APR_GLINK_APPS_REMOTE_SS_ID:
      remote_ss_index = 0;
      glink_cfg.transport = "SMEM";
      break;

    case APR_GLINK_MODEM_REMOTE_SS_ID:
      remote_ss_index = 1;
      glink_cfg.transport = "SMEM";
      break;

    case APR_GLINK_QDSP_REMOTE_SS_ID:
      remote_ss_index = 2;
      glink_cfg.transport = "SMEM";
      break;

    default:
      return APR_EBADPARAM;
  }

  for ( ;; )
  {
    rc = apr_list_remove_head(&apr_glink_free_port_list,
                                               (apr_list_node_t **)&port_item);
    if (rc) break;
    checkpoint = 1;

    port = &port_item->port;
    mmstd_strlcpy(port->port_name, port_name,
                           MMSTD_MIN(sizeof(port->port_name), port_name_size));
    port->port_name_size = port_name_size;
	port->enable_intents = enable_intents;
    port->qos_latency_us = qos_latency_us;
    port->qos_pkt_size = qos_pkt_size;
    port->cb_data = cb_data;
    port->rx_cb = rx_cb;
    port->tx_done_cb = tx_done_cb;
    port->state_cb = state_cb;

    glink_cfg.remote_ss = apr_glink_remote_ss[remote_ss_index];
    glink_cfg.name = port_name;
    glink_cfg.options = GLINK_OPT_INITIAL_XPORT;
    glink_cfg.priv = port_item;
    glink_cfg.notify_rx = apr_glink_rx_cb;
    glink_cfg.notify_tx_done = apr_glink_tx_done_cb;
    glink_cfg.notify_state = apr_glink_state_cb;

    if (GLINK_STATUS_SUCCESS !=
                  (glink_rc = glink_open(&glink_cfg, &port->glink_handle)))
    {
      APR_DBG_1(APR_DBG_ERROR, "glink_open() failed with rc = %d.", glink_rc);
      rc = APR_EFAILED;
      break;
    }
    checkpoint = 2;

    port->state = APR_GLINK_CONNECTING;
    *port_handle = port->port_handle + APR_GLINK_PORT_HANDLE_BASE;

    APR_DBG_1(APR_DBG_HIGH,
            "apr_glink_open() with port handle %d DONE.", *port_handle);

    return APR_EOK;
  }

  switch(checkpoint)
  {
    case 2:
      (void) glink_close(port->glink_handle);
      port->state = APR_GLINK_DISCONNECTING;
      /*-fallthru */
    case 1:
      apr_list_add_tail(&apr_glink_free_port_list,
                                   (apr_list_node_t *)&port_item->link);
      /*-fallthru */
    default:
      break;
  }

  *port_handle = 0;

  APR_DBG_1(APR_DBG_ERROR, "apr_glink_open() failed with rc = %d.", rc);
  return rc;
}

APR_INTERNAL int32_t apr_glink_close ( apr_glink_port_handle_t port_handle )
{
  apr_glink_port_t *port;

  port_handle -= APR_GLINK_PORT_HANDLE_BASE;
  if (port_handle >= APR_GLINK_MAX_PORTS)
  {
    return APR_EBADPARAM;
  }

  port = &apr_glink_port_pool[port_handle].port;
  if (NULL == port)
  {
    return APR_EBADPARAM;
  }

  if (APR_GLINK_DISCONNECTED == port->state ||
                                   APR_GLINK_DISCONNECTING == port->state)
  {
    return APR_EALREADY;
  }

#ifdef APR_GLINK_QOS_SUPPORT
  (void) glink_qos_cancel(port->glink_handle);
#endif
  (void) glink_close(port->glink_handle);
  port->state = APR_GLINK_DISCONNECTING;

  APR_DBG_0(APR_DBG_HIGH, "apr_glink_close() called APR_GLINK_DISCONNECTING");


  return APR_EOK;
}

APR_INTERNAL int32_t apr_glink_tx (
  apr_glink_port_handle_t port_handle,
  void *buf,
  uint32_t size
)
{
  apr_glink_port_t *port;
  glink_err_type glink_rc;

  //APR_DBG_1(APR_DBG_HIGH, "apr_glink_tx() called with APR packet 0x%x", buf);

  port_handle -= APR_GLINK_PORT_HANDLE_BASE;
  if (port_handle >= APR_GLINK_MAX_PORTS || NULL == buf || size < 1)
  {
    return APR_EBADPARAM;
  }

  port = &apr_glink_port_pool[port_handle].port;
  if (NULL == port)
  {
    return APR_EBADPARAM;
  }

  if (APR_GLINK_CONNECTED != port->state)
  {
    APR_DBG_1(APR_DBG_ERROR,
                "apr_glink_tx(): port not connected, state=0x%x", port->state);
    return APR_ENOTREADY;
  }

  if (GLINK_STATUS_SUCCESS !=
          (glink_rc = glink_tx(port->glink_handle, NULL, buf, size, TRUE)))
  {
    APR_DBG_1(APR_DBG_ERROR, "glink_tx() failed with rc = %d.", glink_rc);
    return APR_EFAILED;
  }

  return APR_EOK;
}

APR_INTERNAL int32_t apr_glink_rx_done (
  apr_glink_port_handle_t port_handle,
  void *buf
)
{
  apr_glink_port_t *port;
  glink_err_type glink_rc;

  //APR_DBG_1(APR_DBG_HIGH, "apr_glink_rx_done() called with APR packet 0x%x", buf);

  port_handle -= APR_GLINK_PORT_HANDLE_BASE;
  if (port_handle >= APR_GLINK_MAX_PORTS || NULL == buf)
  {
    return APR_EBADPARAM;
  }

  port = &apr_glink_port_pool[port_handle].port;
  if (NULL == port)
  {
    return APR_EBADPARAM;
  }

  if (APR_GLINK_CONNECTED != port->state)
  {
    return APR_ENOTREADY;
  }

  if (GLINK_STATUS_SUCCESS !=
          (glink_rc = glink_rx_done(port->glink_handle, buf, TRUE)))
  {
    APR_DBG_1(APR_DBG_ERROR, "glink_rx_done() failed with rc = %d", glink_rc);
    return APR_EFAILED;
  }

  return APR_EOK;
}
