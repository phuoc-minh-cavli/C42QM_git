/*
 * Copyright (c) 2012-2013 Qualcomm Technologies, Inc. 

 * All Rights Reserved. 

 * Qualcomm Technologies, Inc. Confidential and Proprietary. 

  $Header: //components/rel/audio.tx/2.0/apr/custom/src/aprv2_ipc.c#2 $
  $Author: pwbldsvc $
*/

#include "mmstd.h"
#include "apr_comdef.h"
#include "apr_errcodes.h"
#include "aprv2_rtable_api_i.h"
#include "aprv2_api_inline.h"
#include "apr_glink.h"
#include "apr_log.h"
#include "aprv2_ipc_i.h"
#include "msg.h"
#include "apr_dbg_msg.h"
#include "aprv2_ids_services.h"
#include "aprv2_api.h"

/*****************************************************************************
 * Definitions                                                               *
 ****************************************************************************/

#define APRV2_IPC_AUDIO_PORT_NAME ( "apr_voice_svc" )

#define APRV2_IPC_ENABLE_INTENTS          ( 1 )
#define APRV2_IPC_QOS_LATENCY_US          ( 550 )
#define APRV2_IPC_QOS_PKT_SIZE            ( /*160*/ 544 )

/*****************************************************************************
 * Variables                                                                 *
 ****************************************************************************/

static apr_glink_port_handle_t aprv2_ipc_modem_port = NULL;
  
/*****************************************************************************
 * Routines                                                                  *
 ****************************************************************************/

static int32_t aprv2_ipc_rx_cb (
  void* cb_data,
  void* rx_read_buf,
  uint32_t size
)
{
  int32_t handle;
  uint32_t packet_size ;
  aprv2_packet_t *packet;

  if (NULL == rx_read_buf || size < 1)
  {
    APR_DBG_0(APR_DBG_ERROR,
           "APPS aprv2_ipc_rx_cb(): Rcvd NULL buffer or buffer size < 1");
    return APR_EBADPARAM;
  }

  packet = (aprv2_packet_t *)rx_read_buf;
  packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);

  if (size != packet_size)
  {
    APR_DBG_0(APR_DBG_ERROR,
                 "APPS aprv2_ipc_rx_cb(): Rcvd invalid buffer size");
    return APR_EFAILED;
  }

  handle = ((uint32_t)
             (((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->dst_addr)) << 8)
                   | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, packet->dst_addr))));

  (void) __aprv2_cmd_forward(handle, packet);

  return APR_EOK;
}

static int32_t aprv2_ipc_tx_done_cb (
  void* cb_data,
  void* rx_write_buf,
  uint32_t size
)
{
  int32_t  handle ;
  uint32_t packet_size;
  aprv2_packet_t *packet;

  if (NULL == rx_write_buf || size < 1)
  {
    APR_DBG_0(APR_DBG_ERROR,
          "APPS aprv2_ipc_tx_done_cb(): Rcvd NULL buffer or buffer size < 1");
    return APR_EBADPARAM;
  }

  packet = (aprv2_packet_t *)rx_write_buf;
  packet_size = APRV2_PKT_GET_PACKET_BYTE_SIZE(packet->header);

  if (size != packet_size)
  {
    APR_DBG_0(APR_DBG_ERROR,
                 "APPS aprv2_ipc_tx_done_cb(): Rcvd invalid buffer size");
    return APR_EFAILED;
  }

  handle = ((uint32_t)
             (((APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->dst_addr)) << 8)
                   | (APR_GET_FIELD(APRV2_PKT_SERVICE_ID, packet->dst_addr))));

  (void) __aprv2_cmd_free(handle, packet);

  return APR_EOK;
}

static int32_t aprv2_ipc_state_cb (apr_glink_port_handle_t port_handle, uint32_t event)
{

  APR_DBG_1(APR_DBG_MEDIUM,
                "APPS aprv2_ipc_state_cb(): Rcvd event = 0x%x", event);

  switch ( event )
  {
  case APR_GLINK_REMOTE_DISCONNECTED:
   {
      APR_DBG_0(APR_DBG_HIGH, "APPS calling apr_glink_close");      
	  (void)apr_glink_close(port_handle);      
  return APR_EOK;
}
    case APR_GLINK_DISCONNECTED:   
	{
      APR_DBG_0(APR_DBG_HIGH, "APPS closed."); 		
    }
  }
  return APR_EOK;
}


void aprv2_ipc_glink_link_state_cb ( 
  glink_link_info_type *link_info,
  void *priv
)
{
  int32_t rc = APR_EOK;

  APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): APPS Starting to open GLINK channels");

  if ( link_info->link_state != GLINK_LINK_STATE_UP )
  {
    APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): APPS link_info->link_state != GLINK_LINK_STATE_UP ");
    return;
  }

  for ( ;; )
  {
#ifndef SIM_DEFINED 

   if ( mmstd_strncmp( link_info->remote_ss, sizeof( "mpss" ),
                        "mpss", sizeof( "mpss" )) == 0 )
   {
     /* "apr_voice_svc" for communicating with ADSP2. */

    rc = apr_glink_open(
           APRV2_IPC_AUDIO_PORT_NAME,
           sizeof(APRV2_IPC_AUDIO_PORT_NAME),
           APR_GLINK_MODEM_REMOTE_SS_ID,      //changing from  APR_GLINK_MODEM_REMOTE_SS_ID
           APRV2_IPC_ENABLE_INTENTS,
           APRV2_IPC_QOS_LATENCY_US,
           APRV2_IPC_QOS_PKT_SIZE,
          &aprv2_ipc_modem_port,
           aprv2_ipc_rx_cb,
           aprv2_ipc_tx_done_cb,
           aprv2_ipc_state_cb,
           &aprv2_ipc_modem_port
        );

      APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): APPS Starting to open MODEM GLINK channels");
  
      if ( rc ) 
      {
        APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): APPS Failed to open MODEM GLINK channels");
	    ( void ) apr_glink_close( aprv2_ipc_modem_port );
     
	    break;
	  }
   	}
	  
#endif /* SIM_DEFINED */

   APR_DBG_0(APR_DBG_HIGH, "aprv2_ipc_glink_link_state_cb(): APPS APR Over GLINK channels succeed.");
  return;
}
 
  return;
}

APR_INTERNAL int32_t aprv2_ipc_init ( void )
{
  int32_t rc;
  uint32_t checkpoint = 0;

  apr_log_printf( "APPS APRIPC: starting initialization sequence" );

  for ( ;; )
  {

#ifndef SIM_DEFINED
    rc = apr_glink_init(aprv2_ipc_glink_link_state_cb);
    if ( rc ) break;
      checkpoint = 1;

#endif

    APR_DBG_0(APR_DBG_HIGH, "APPS aprv2_ipc_init succeed." );
    return APR_EOK;
  }

  switch ( checkpoint )
  {
    case 1:
    (void) apr_glink_deinit();
    /*-fallthru */
    default:
       break;
  }

  APR_DBG_1(APR_DBG_ERROR, "APPS APRIPC: initialization failed after checkpoint=%d", checkpoint);

  return rc;
}

APR_INTERNAL int32_t aprv2_ipc_deinit ( void )
{
  APR_DBG_0(APR_DBG_HIGH, "APPS aprv2_ipc_deinit start." );

#ifndef SIM_DEFINED
  (void) apr_glink_close(aprv2_ipc_modem_port);
  
  (void) apr_glink_deinit();
#endif

  return APR_EOK;
}

APR_INTERNAL int32_t aprv2_ipc_is_domain_local (uint16_t domain_id)
{
  switch ( domain_id )
  {
  case APRV2_IDS_DOMAIN_ID_APPS_V:
    return APR_EOK;

  default:
    return APR_EFAILED;
  }
}

APR_INTERNAL int32_t aprv2_ipc_send (aprv2_packet_t* packet)
{
  int32_t    rc = APR_EOK;
  uint16_t   dst_domain_id;
  uint32_t packet_len;

  if ( packet == NULL )
  {
    return APR_EBADPARAM;
  }

#ifndef SIM_DEFINED
  if (aprv2_ipc_modem_port == NULL)
  
  {
    return APR_ENOTREADY;
  }
#endif

  packet_len =  APRV2_PKT_GET_PACKET_BYTE_SIZE( packet->header );

  dst_domain_id  = APR_GET_FIELD( APRV2_PKT_DOMAIN_ID, packet->dst_addr );

  switch ( dst_domain_id )
  {
    case APRV2_IDS_DOMAIN_ID_PC_V:
    case APRV2_IDS_DOMAIN_ID_MODEM_V:
	case APRV2_IDS_DOMAIN_ID_ADSP_V:
      rc = apr_glink_tx(aprv2_ipc_modem_port, packet, packet_len);
    
      if (rc)
    {
        APR_DBG_1(APR_DBG_ERROR, "apr_glink_tx() failed with rc = 0x%x", rc);
    }
    break;

    case APRV2_IDS_DOMAIN_ID_APPS_V:
    rc = APR_ELPC;
    break;

    default:
    {
        APR_DBG_1(APR_DBG_ERROR, "APPS apr_ipc_send(0x%08X) sending to invaild domain", packet );
        rc = APR_EUNSUPPORTED;
    }
    break;
  } /* switch domain */

  return rc;
}

APR_INTERNAL int32_t aprv2_ipc_free (aprv2_packet_t *packet)
{
  uint16_t src_domain_id;
  int32_t rc = APR_EOK;

  if (packet == NULL)
  {
    return APR_EBADPARAM;
  }

  src_domain_id  = APR_GET_FIELD(APRV2_PKT_DOMAIN_ID, packet->src_addr);

  switch (src_domain_id)
  {
  case APRV2_IDS_DOMAIN_ID_MODEM_V:
  case APRV2_IDS_DOMAIN_ID_ADSP_V:
    rc = apr_glink_rx_done(aprv2_ipc_modem_port, packet);
    
    break;

  default:
    APR_DBG_1(APR_DBG_ERROR,
            "APPS apr_ipc_free(0x%08X) on invaild src domain", packet);
    rc = APR_EFAILED;
    break;
  }

  return rc;
}
