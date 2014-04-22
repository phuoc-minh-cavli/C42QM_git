/*
 * Copyright (c) 2017-2023 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/*
 * FILENAME: mqttc.c
 * MODULE: MQTT
 *
 *
 * PORTABLE: yes
 */

#ifdef MQTT_CLIENT

#define __STDC_FORMAT_MACROS
#include "inttypes.h"
#include "stdlib.h"
#include "stringl/stringl.h"
#include "mqtt_port.h"

#include "qapi_socket.h"
#include "qapi_ssl.h"
#include "qapi.h"
#include "qapi_netservices.h"
#include "qapi_timer.h"

#include "MQTTPacket.h"
#include "MQTTConnect.h"
#include "ce.h"
#include "mqtt_stats.h"
#include "mqttc.h"

#include "timer.h"
#include "fs_public.h"
#include "fs_errno.h"
#include "fs_sys_types.h"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define MQTT_MAX_MSG_LEN 4

extern ip6_addr ip6unspecified;

extern bool is_mqtt_ctx_and_conn_pair_exist(qapi_Net_MQTT_Hndl_t hndl, struct MQTT_connection* conn);

#ifdef APPS_PACKET_LOGGING_ENABLE  
extern void dump_buf_to_dpl(int handle, uint8_t * dump_buf, int buf_len, bool recvd);
#endif

extern IN_SEM ts_mqtt;
extern qurt_mutex_t * mqtt_mutex;

const char *MQTT_conn_state_names[] =
{
  "DORMANT",
  "TCP_CONNECTING",
  "TCP_CONNECTED",
  "SSL_CONNECTING",
  "SSL_CONNECTED",
  "MQTT_CONNECTING",
  "MQTT_CONNECTED",
  "MQTT_TERMINATING",
  "SSL_TERMINATING",
  "TCP_TERMINATING",
  "DYING",
  "DEAD"
};

#define MAX_SUB_BUF_LEN 2000
#define MAX_TX_BUF_LEN 5000
#define MAX_SUB_PERSISTED 10
#define MAX_TX_PERSISTED 10
#define MQTT_PERSISTENCE_FILE "/datatx/mqtt/mqtt_restore_persist"
#define MQTT_MAX_RESYNC_THRESHOLD 10

typedef struct sub_persist_info{
  uint8_t sub[MAX_SUB_BUF_LEN];
}sub_persist_info_t;

#if 0
typedef struct tx_persist_info{
  uint8_t tx[MAX_TX_BUF_LEN];
}tx_persist_info_t;
#endif
typedef struct mqtt_persist_info{
  uint8_t client_id[MAX_CLIENT_ID_LEN];
  int subq_count;
  int txq_count;
  sub_persist_info_t subq[MAX_SUB_PERSISTED];
  //tx_persist_info_t txq[MAX_TX_PERSISTED];   /*persistence of TX list is not supported as of now*/
}mqtt_persist_info_t;

#ifdef MQTT_DEBUG
int mqtt_log_level = 0;
#endif

struct MQTT_connection *MQTT_connlist = NULL;

extern  void mqtt_intr_subscribe_callback(struct MQTT_connection* conn,
    int reason,
    uint8_t* topic,
    int topic_length,
    uint8_t* msg,
    int msg_length,
    int qos,
    void* sid);

static int mqtt_conn_establish(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status);
static int mqtt_ssl_cert_load(MQTT_CONN_PTR conn, MQTT_CFG_PTR cfg);
static int mqtt_start_ssl_connection(MQTT_CONN_PTR conn);

const char *mqtt_get_conn_state_name(MQTT_CONN_STATE state_id)
{
  return (MQTT_conn_state_names[state_id]);
}

static void mqtt_config_string_struct(MQTTString *mqtt_str, char * src, int len)
{
  if(NULL == mqtt_str)
    return;
  if (src != NULL)
  {
    mqtt_str->cstring        = NULL;
    mqtt_str->lenstring.data = src;
    mqtt_str->lenstring.len  = len;
  }
  else
  {
    MEMSET(mqtt_str, 0, sizeof(MQTTString));
  }
}

static bool mqtt_config_str_len(uint8_t **dest_p, int *dest_len, uint8_t *src, int src_len)
{
  bool ret = false;

  if(dest_p == NULL || dest_len == NULL)
    return false;

  if(src != NULL && src_len > 0)
  {
    *dest_p = MQTTC_ALLOC(src_len);
    if(*dest_p != NULL)
    {
      memscpy(*dest_p, src_len, src, src_len);
      *dest_len = src_len;
      ret = true;
    }
    else
    {
      MQTT_LOG_DEBUG("mqtt_config_str_len: fail to alloc memory");
      ret = false;
    }
  }
  else
  {
    *dest_p = NULL;
    *dest_len = 0;
    ret = true;
  }
  return ret;
}

void htonl_uint32_t(uint8_t** pptr, uint32_t uvar)
{
  if(pptr==NULL || *pptr == NULL)
    return;
  *((uint32_t *)(*pptr)) = htonl(uvar);
  *pptr += sizeof(uint32_t);
  return;
}

void htonl_int32_t(uint8_t** pptr, int32_t var)
{
  if(pptr==NULL || *pptr == NULL)
    return;
  *((int32_t *)(*pptr)) = htonl(var);
  *pptr += sizeof(int32_t);
  return;
}

void ntohl_int32_t(int32_t* pvar, uint8_t** pptr)
{
  if(pptr==NULL || *pptr == NULL || pvar == NULL)
    return;
  *pvar = ntohl(*((int32_t *)(*pptr)));
  *pptr += sizeof(int32_t);
  return;
}

int mqtt_process_tx_cb(MQTT_CONN_PTR conn, enum msgTypes msgtype, uint16_t *p_pkt_id )
{
  MQTT_TxCB_PTR tx_cb;
  MQTT_TxCB_PTR prev_tx_cb;
  void (*app_cbk)(struct MQTT_TxCB *);
  uint16_t expected_id;
  uint16_t pkt_id;
  uint16_t ix = 0;
  uint16_t max_txq_count = MQTT_MAX_RESYNC_THRESHOLD;
  if(NULL == conn )
    return EFAILURE;

  pkt_id = *(uint16_t *)(conn->rx.buf + 2);
  pkt_id = ntohs(pkt_id);

  prev_tx_cb = tx_cb = conn->txq;

  for(ix = 0; (tx_cb != NULL) && (ix < max_txq_count); tx_cb = tx_cb->next) {
    expected_id = tx_cb->id;
    if((msgtype == tx_cb->response) && (expected_id == pkt_id))
      break;
    prev_tx_cb = tx_cb;
    ix++;
  }

  if(tx_cb)
  {

    if(p_pkt_id != NULL)
      *p_pkt_id = expected_id;
    if(expected_id != pkt_id) {
      MQTT_LOG_ERR2("mqtt_process_tx_cb: mismatch pkt "
            "ids (expected %u, rcvd %u)", expected_id, pkt_id);
      return (EFAILURE);
    } else {
        MQTT_LOG_INFO("mqtt_process_tx_cb: pkt id match %d", pkt_id);
    }

    /* invoke application-specified callback to notify it of the arrival of
     * the matching response
     */
    app_cbk = tx_cb->callback;
    if(app_cbk) {
      app_cbk(tx_cb);
    } else {
      MQTT_LOG_INFO("mqtt_process_tx_cb: application "
            "callback not specified");
    }

    MQTT_LOG_INFO("mqtt_process_tx_cb: found matching "
          "entry, deleting...");

    if(tx_cb == conn->txq) {
      conn->txq = tx_cb->next;
    } else {
      prev_tx_cb->next = tx_cb->next;
    }

    /* free entry, and update queue */
    MQTTC_FREE(tx_cb->buf);
    MQTTC_FREE(tx_cb);
  }
  else
  {
      MQTT_LOG_ERR1("mqtt_process_tx_cb: No match found for pkt_id=%d", pkt_id);
      return (EFAILURE);
  }

  return (ESUCCESS);
}

int mqtt_process_tx_cb_without_pkt_id(MQTT_CONN_PTR conn, enum msgTypes msgtype)
{
  MQTT_TxCB_PTR tx_cb;
  MQTT_TxCB_PTR prev_tx_cb;
  void (*app_cbk)(struct MQTT_TxCB *);
  uint16_t ix;
  uint16_t max_txq_count = MQTT_MAX_RESYNC_THRESHOLD;

  if(!conn)
    return EFAILURE;

  prev_tx_cb = tx_cb = conn->txq;

  for(ix = 0; (tx_cb != NULL) && (ix < max_txq_count); tx_cb = tx_cb->next) {
    if(msgtype == tx_cb->response)
      break;
    prev_tx_cb = tx_cb;
    ix++;
  }

  if(tx_cb)
  {
    /* invoke application-specified callback to notify it of the arrival of
     * the matching response
     */
    app_cbk = tx_cb->callback;
    if(app_cbk) {
      app_cbk(tx_cb);
    } else {
      MQTT_LOG_INFO("mqtt_process_tx_cb_without_pkt_id: application "
            "callback not specified");
    }

    MQTT_LOG_INFO("mqtt_process_tx_cb_without_pkt_id: found matching "
          "entry, deleting...");

    if(tx_cb == conn->txq) {
      conn->txq = tx_cb->next;
    } else {
      prev_tx_cb->next = tx_cb->next;
    }

    /* free entry, and update queue */
    MQTTC_FREE(tx_cb->buf);
    MQTTC_FREE(tx_cb);
  }
  return (ESUCCESS);
}


/* caller must provide the necessary locking */
static MQTT_CONN_PTR
mqtt_find_conn(struct sockaddr *local, struct sockaddr *remote, 
    uint8_t *client_id, int client_id_len)
{
  uint16_t family;
  MQTT_CONN_PTR conn;

  if (local == NULL || remote == NULL || client_id == NULL || client_id_len <= 0)
  {
    return (NULL);
  }

  for (conn = MQTT_connlist; conn != NULL; conn = conn->next)
  {
    family = conn->cfg.local.sa_family;
    if (family != local->sa_family)
      continue;

    switch (family)
    {
#ifdef IP_V4
      case AF_INET:
        {
          struct sockaddr_in *sin1, *sin2;

          sin1 = (struct sockaddr_in *)local;
          sin2 = (struct sockaddr_in *)&conn->cfg.local;
          if (((sin1->sin_addr.s_addr != 0) && (sin1->sin_addr.s_addr != sin2->sin_addr.s_addr)) ||
              ((sin1->sin_port != 0) && (sin1->sin_port != sin2->sin_port)))
          {
            continue;
          }
          sin1 = (struct sockaddr_in *)remote;
          sin2 = (struct sockaddr_in *)&conn->cfg.remote;
          if ((sin1->sin_addr.s_addr != sin2->sin_addr.s_addr) ||
              (sin1->sin_port != sin2->sin_port))
          {
            continue;
          }
        }
#endif

#ifdef IP_V6
      case AF_INET6:
        {
          struct sockaddr_in6 *sin1, *sin2;

          sin1 = (struct sockaddr_in6 *)local;
          sin2 = (struct sockaddr_in6 *)&conn->cfg.local;
          if ((!(IP6EQ(&sin1->sin_addr, &ip6unspecified))) && (!IP6EQ(&sin1->sin_addr, &sin2->sin_addr)) ||
              ((sin1->sin_port != 0) && (sin1->sin_port != sin2->sin_port)))
          {
            continue;
          }
          sin1 = (struct sockaddr_in6 *)remote;
          sin2 = (struct sockaddr_in6 *)&conn->cfg.remote;
          if (!IP6EQ(&sin1->sin_addr, &sin2->sin_addr) ||
              sin1->sin_port != sin2->sin_port)
          {
            continue;
          }
        }
#endif
    }

    if ((conn->cfg.client_id_len != client_id_len) || 
        (MEMCMP(conn->cfg.client_id, client_id, client_id_len + 1)) != 0)
    {
      continue;
    }
    else
    {
      return (conn);
    }
  }

  return (NULL);
}

static void
mqtt_add_conn(MQTT_CONN_PTR conn)
{
  if (conn == NULL)
  {
    return;
  }

  MQTT_LOCK_MUTEX(mqtt_mutex);

  conn->prev = NULL;
  conn->next = MQTT_connlist;
  if (MQTT_connlist != NULL)
  {
    MQTT_connlist->prev = conn;
  }
  MQTT_connlist = conn;
  MQTT_UNLOCK_MUTEX(mqtt_mutex);

}

static void
mqtt_unlink_conn(MQTT_CONN_PTR conn, bool_t lock_required)
{
  if (conn == NULL)
  {
    return;
  }

  if (lock_required)
  {
    MQTT_LOCK_MUTEX(mqtt_mutex);
  }
  if (MQTT_connlist == conn)
  {
    MQTT_connlist = conn->next;
    if (conn->next)
    {
      conn->next->prev = NULL;
    }
  }
  else
  {
    conn->prev->next = conn->next;
    if (conn->next)
    {
      conn->next->prev = conn->prev;
    }
  }
  if (lock_required)
  {
     MQTT_UNLOCK_MUTEX(mqtt_mutex);
  }
}

bool_t
mqtt_is_secure_conn(MQTT_CONN_PTR conn)
{
  uint16_t family;
  uint16_t port = 0;
  MQTT_CFG cfg;

  if (conn == NULL)
  {
    return (FALSE);
  }

  family = conn->cfg.local.sa_family;
  cfg = conn->cfg;
  switch (family)
  {
#ifdef IP_V4
    case AF_INET:
      {
        struct sockaddr_in *sin;
        sin = (struct sockaddr_in *)&conn->cfg.remote;
        port = ntohs(sin->sin_port);
        break;
      }
#endif
#ifdef IP_V6
    case AF_INET6:
      {
        struct sockaddr_in6 *sin6;
        sin6 = (struct sockaddr_in6 *)&conn->cfg.remote;
        port = ntohs(sin6->sin_port);
        break;
      }
#endif
  }

  if (port == MQTT_SECURE_PORT)
  {
    return (TRUE);
  }
  else if(port == MQTT_PORT)
  {
    return (FALSE);
  }
  else if(cfg.ssl_cfg.protocol == QAPI_NET_SSL_PROTOCOL_TLS_1_0 ||
          cfg.ssl_cfg.protocol == QAPI_NET_SSL_PROTOCOL_TLS_1_1 ||
          cfg.ssl_cfg.protocol == QAPI_NET_SSL_PROTOCOL_TLS_1_2 ||
          cfg.ssl_cfg.protocol == QAPI_NET_SSL_PROTOCOL_TLS_1_3)
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
  }
}

void
mqtt_free_conn(MQTT_CONN_PTR conn)
{
  MQTT_TxCB_PTR txcb;
  MQTT_TxCB_PTR next_txcb;
  MQTT_SubRxCB_PTR subcb;
  MQTT_SubRxCB_PTR next_subcb;
  int i;

  if (conn == NULL)
  {
    return;
  }

  MQTTC_FREE(conn->rx.buf);

  MQTT_LOG_INFO("mqtt_free_conn: Freeing queues");
  conn->flags &= ~MQTT_FL_RESTORE_FAILED;
  for (txcb = conn->txq; txcb != NULL; txcb = next_txcb)
  {
    next_txcb = txcb->next;
    MQTTC_FREE(txcb->buf);
    MQTTC_FREE(txcb);
  }

  for (subcb = conn->subq; subcb != NULL; subcb = next_subcb)
  {
    next_subcb = subcb->next;
    for (i = 0; i < subcb->count; ++i)
    {
      MQTTC_FREE(subcb->sub[i].topic);
    }
    MQTTC_FREE(subcb);
  }
  MQTTC_FREE(conn->cfg.username);
  MQTTC_FREE(conn->cfg.password);
  MQTTC_FREE(conn->cfg.will_topic);
  MQTTC_FREE(conn->cfg.will_message);

#ifdef MQTT_CLIENT_SECURITY

  if (mqtt_is_secure_conn(conn))
  {
    #ifdef MQTT_ENABLE_LOG_INFO
    MQTT_CONN_STATE state = conn->state;
    #endif
    //bool_t bidirectional_shutdown;
    conn->state = MQTT_ST_SSL_TERMINATING;
    MQTT_LOG_INFO_STRF("mqtt_free_conn: conn %p %s -> %s",
                   conn,mqtt_get_conn_state_name(state), mqtt_get_conn_state_name(conn->state));
    qapi_Net_SSL_Shutdown(conn->ssl);
    qapi_Net_SSL_Obj_Free(conn->sslctx);
  }
#endif

  if (conn->sock != 0 && conn->sock != INVALID_SOCKET)
  {
    #ifdef MQTT_ENABLE_LOG_INFO
    MQTT_CONN_STATE state = conn->state;
    #endif
    conn->state = MQTT_ST_TCP_TERMINATING;
    MQTT_LOG_INFO_STRF("mqtt_free_conn: conn %p %s -> %s",
                   conn, mqtt_get_conn_state_name(state), mqtt_get_conn_state_name(conn->state));
    qapi_socketclose(conn->sock);
  }
#ifndef SUPERLOOP
  MUTEX_FREE(conn->mutex);
#endif
  MQTTC_FREE(conn);
}

#ifdef NET_STATS
/* XXX add #ifdef...#endif directive? */
void
mqtt_decr_active_sess_count(bool_t secure_conn)
{
  if (secure_conn == FALSE)
  {
    NSDEC2(mqtt_stats, active_clear_sessions);
  }
  else
  {
    NSDEC2(mqtt_stats, active_secure_sessions);
  }
}
#endif

void
mqtt_destroy_conn(MQTT_CONN_PTR conn, bool_t lock_required)
{
  if (conn != NULL)
  {
    MQTT_LOG_DEBUG("mqtt_destroy_conn : Destroy MQTT Conn");
    mqtt_unlink_conn(conn, lock_required);
#ifdef NET_STATS
    if (conn->flags & MQTT_FL_SESS_ESTAB)
    {
      mqtt_decr_active_sess_count(mqtt_is_secure_conn(conn));
    }
#endif
    mqtt_free_conn(conn);
  }
}

int
mqtt_rd_sock(MQTT_CONN_PTR conn, uint8_t *buf, int length, int flags, 
    MQTT_ERR_TYPE *status)
{
  SOCKTYPE sock;
  int rc;
  int sock_err = 0;

  if (conn == NULL || buf == NULL || length <= 0 || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_INFO("mqtt_rd_sock: parameter err");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	
	if(conn != NULL)
	{
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }
  MQTT_LOG_DEBUG5("mqtt_rd_sock: conn %p buf %p len %d flags %d status %p", conn, buf, length, flags, status);
  sock = conn->sock;

  if (!mqtt_is_secure_conn(conn))
  {
    rc = qapi_recv(sock, (char *)buf, length, flags);
    MQTT_LOG_DEBUG2("mqtt_rd_sock: t_recv() return %d for conn %p", rc, conn);
  }
#ifdef MQTT_CLIENT_SECURITY
  else
  {
    rc = qapi_Net_SSL_Read(conn->ssl, buf, length); 
    MQTT_LOG_DEBUG2("mqtt_rd_sock: sslclnt_recv() return %d for conn %p", rc, conn);
  }
#endif

  if ( rc < QAPI_OK  ) { 
    MQTT_LOG_ERR1("mqtt_rd_sock: SSL_Read()/qapi_recv() err: [ %d ]", rc);
    rc = qapi_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_err, sizeof(int));
    MQTT_LOG_ERR3("mqtt_rd_sock: qapi_getsockopt:return %d (sock_err %d) for conn %p", rc, sock_err, conn);
	conn->cfg.last_error = sock_err;
	conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;

    if ((rc != ESUCCESS) || (sock_err != EWOULDBLOCK))
    {
      NSINC2(mqtt_stats, socket_failures);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (EFAILURE);
    }

    conn->cfg.last_error = MQTT_ERR_NO_DATA;
	conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;
    *status = MQTT_ERR_NO_DATA;
    rc = 0;
  } else { 
    switch ( rc ) 
    { 
      case QAPI_OK : 
        NSINC2(mqtt_stats, conn_closed_by_srv);
        *status = MQTT_ERR_CONN_CLOSED;
        MQTT_LOG_DEBUG1("mqtt_rd_sock: read 0 bytes rc %d ", rc);
        break;
      default :
        *status = MQTT_ERR_NO_ERR;
        break;
    }
  }     
  return rc;
}


int
mqtt_wr_sock(MQTT_CONN_PTR conn, uint8_t *buf, int length, int flags, 
    MQTT_ERR_TYPE *status)
{
  bool_t secure_conn;
  SOCKTYPE sock;
  MQTT_CFG_PTR sess = NULL;

  uint32_t buffer_offset = 0;
  uint32_t bytes_to_send = length;
  uint16_t max_data_chunk = MQTT_DEFAULT_SEND_CHUNK_SIZE; 
  int bytes_sent = 0;
  uint8_t retries = MQTT_DEFAULT_SEND_CHUNK_RETRIES;
  uint16_t delay = MQTT_DEFAULT_SEND_CHUNK_DELAY;
  sess = &conn->cfg;
  
  if (conn == NULL || buf == NULL || length <= 0 || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_wr_sock: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	
	if(conn != NULL)
	{
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

#ifdef APPS_PACKET_LOGGING_ENABLE  

  MQTT_LOG_PRINT5("mqtt_wr_sock: conn %p buf %p len %d flags %d status %p", conn, buf, length, flags, status);
  dump_buf_to_dpl(conn->sock, buf,  length, FALSE);

#endif

  secure_conn = mqtt_is_secure_conn(conn);
  sock = conn->sock;

  
  /* Configure the application specific paramters */
  if (sess)
  {
    max_data_chunk = (sess->max_chunk) ? sess->max_chunk : MQTT_DEFAULT_SEND_CHUNK_SIZE; 
    retries = (sess->max_chunk_retries) ? sess->max_chunk_retries : MQTT_DEFAULT_SEND_CHUNK_RETRIES;
    delay = (sess->max_chunk_delay_ms) ?  sess->max_chunk_delay_ms : MQTT_DEFAULT_SEND_CHUNK_DELAY;
  }

  while(1)
  {
    bytes_sent = 0;
	
    if(bytes_to_send <= max_data_chunk)
      max_data_chunk = bytes_to_send;
	
    if (secure_conn)
      bytes_sent = qapi_Net_SSL_Write(conn->ssl, &buf[buffer_offset], max_data_chunk);
    else
      bytes_sent = qapi_send(sock, (char *)&buf[buffer_offset], max_data_chunk, flags);
	
    if (bytes_sent > 0)
    {	
      MQTT_LOG_INFO2("mqtt_wr_sock: qapi_Net_SSL_Write()/qapi_send() returned %d for conn %p\n", bytes_sent, conn);
	
      if (bytes_sent == bytes_to_send)
      {
        *status = MQTT_ERR_NO_ERR;
         conn->last_tx = CTICKS;
         return length;
      }
	
      bytes_to_send -= bytes_sent;
      buffer_offset += bytes_sent;	 
	
      /* Sleep between consecutive send data chunks */
      qapi_Timer_Sleep(delay, QAPI_TIMER_UNIT_MSEC, TRUE);
    }
    else
    {
      int errnum;
  	  
      MQTT_LOG_ERR1("mqtt_wr_sock: qapi_Net_SSL_Write()/qapi_send() error %d", bytes_sent);
      errnum = qapi_errno(sock);
  
      MQTT_LOG_ERR1("mqtt_wr_sock:  errnum %d", errnum);
  	
      if ((errnum != EWOULDBLOCK) && (errnum != ENOBUFS))
      {
        NSINC2(mqtt_stats, socket_failures);
        *status = MQTT_ERR_SOCKET_FATAL_ERROR;
		conn->cfg.last_error = errnum;
		conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;
        return (EFAILURE);
      }
      else if ((errnum == ENOBUFS) && ((retries--) <= 0))
      {
        NSINC2(mqtt_stats, socket_failures);
        *status = MQTT_ERR_SOCKET_FATAL_ERROR;
		conn->cfg.last_error = errnum;
		conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;
        return (EFAILURE);
      }
  
      /* If we hit ENOBUFF scenario we will retry 10 times to send the data after that we
      will bail out of the send while loop. */
      qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, TRUE); 
    }	
  }
}

static bool_t
mqtt_chk_wr_pending(MQTT_CONN_PTR conn)
{
  MQTT_TxCB_PTR txcb;

  if (conn == NULL) 
  {
    return (FALSE);
  }

  if ((txcb = conn->txq) == NULL)
  {
    /* transmit queue is empty */
    return (FALSE);
  }

  do
  {
    if (txcb->length != txcb->offset)
    {
      return (TRUE);
    }
    txcb = txcb->next;
  } while (txcb != NULL);

  return (FALSE);
}

/**
 * Determines the length of the MQTT connect packet that would be produced using the supplied connect options.
 * @param options the options to be used to build the connect packet
 * @return the length of buffer needed to contain the serialized version of the packet
 */
int MQTTSerialize_connectLength(MQTTPacket_connectData* options);

  uint8_t *
mqtt_create_connect_msg(MQTT_CONN_PTR conn, int *len, MQTT_ERR_TYPE *status)
{
  MQTTPacket_connectData conn_data;
  int reqd_len;
  uint8_t *buf;
  int rc;

  if (conn == NULL || len == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_create_connect_msg: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	
	if(conn != NULL)
	{
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (NULL);
  }

  conn_data.MQTTVersion = MQTT_VERSION;
  conn_data.keepAliveInterval = conn->cfg.keepalive_duration/TPS;
  conn_data.cleansession = conn->cfg.clean_session;

  conn_data.willFlag = conn->cfg.will_topic == NULL? FALSE : TRUE;
  conn_data.will.retained = conn->cfg.will_retained;
  conn_data.will.qos = conn->cfg.will_qos;

  mqtt_config_string_struct(&conn_data.clientID,       (char *)conn->cfg.client_id,    conn->cfg.client_id_len);
  mqtt_config_string_struct(&conn_data.will.topicName, (char *)conn->cfg.will_topic,   conn->cfg.will_topic_len);
  mqtt_config_string_struct(&conn_data.will.message,   (char *)conn->cfg.will_message, conn->cfg.will_message_len);
  mqtt_config_string_struct(&conn_data.username,       (char *)conn->cfg.username,     conn->cfg.username_len);
  mqtt_config_string_struct(&conn_data.password,       (char *)conn->cfg.password,     conn->cfg.password_len);

  reqd_len = MQTTPacket_len(MQTTSerialize_connectLength(&conn_data));
  MQTT_LOG_DEBUG1("mqtt_create_connect_msg: required length of buf %d", reqd_len);

  buf = MQTTC_ALLOC(reqd_len);
  if (buf == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_create_connect_msg: fail to alloc buf");
    NSINC2(mqtt_stats, alloc_failures);
    *status = MQTT_ERR_ALLOC_FAILURE;
	conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    return (NULL);
  }
  /*
   * Serializes the connect options into the buffer.
   * @param buf the buffer into which the packet will be serialized
   * @param len the length in bytes of the supplied buffer
   * @param options the options to be used to build the connect packet
   * @return serialized length, or error if 0
   */
  rc = MQTTSerialize_connect(buf, reqd_len, &conn_data);
  MQTT_LOG_DEBUG1("mqtt_create_connect_msg: MQTTSerialize_connect return %d", rc);
  if (rc == 0)
  {
    MQTT_LOG_ERR1("mqtt_create_connect_msg: fail for conn %p", conn);
    MQTTC_FREE(buf);
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (NULL);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS1("mqtt_create_connect_msg: CONNECT (%d bytes):", rc);
    gio_hexdump(NULL, buf, rc, TRUE, TRUE);
  }
#endif

  *len = rc;
  *status = MQTT_ERR_NO_ERR;
  return (buf);
}

void
mqtt_add_txcb(MQTT_CONN_PTR conn, MQTT_TxCB_PTR tx_cb)
{
  MQTT_TxCB_PTR ptr;
  
  if (conn == NULL || tx_cb == NULL)
    return;
    
  if(tx_cb->response >= INVALID_RESP) {
  	conn->cfg.last_error = tx_cb->response;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    MQTT_LOG_ERR("mqtt_add_txcb: Invalid rsp number");
    return;
  }
    
  if (conn->txq == NULL)
  {
    conn->txq = tx_cb; 
  }
  else
  {
    for (ptr = conn->txq; ptr->next != NULL; ptr = ptr->next)
    {
      ;
    }

    /* insert at tail end */
    ptr->next = tx_cb;
  } 
 
}

int
mqtt_tx_connect(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  uint8_t        *tx_buf;
  int             len;
  int             rc = ESUCCESS;
  MQTT_TxCB      *tx_cb;
  #ifdef MQTT_ENABLE_LOG_INFO
  MQTT_CONN_STATE old_state;
  #endif
  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_tx_connect: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }

	if(conn != NULL)
	{
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  state = conn->state;
  if (!(!mqtt_is_secure_conn(conn) && state == MQTT_ST_TCP_CONNECTED) &&
      !(mqtt_is_secure_conn(conn)  && state == MQTT_ST_SSL_CONNECTED))
  {
    MQTT_LOG_DEBUG_STRF("mqtt_tx_connect: reject to send CONN in state %d[%s]", state, mqtt_get_conn_state_name(state));
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  /* create the CONNECT message */
  tx_buf = mqtt_create_connect_msg(conn, &len, status);
  if(tx_buf == NULL)
    return EFAILURE;

  /* attempt to send the data now if the tx queue is empty */
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status);
    if (rc == EFAILURE)
    {
      MQTT_LOG_ERR1("mqtt_tx_connect: conn %p fatal err on sock", conn);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      /* increment statistics in the event of a partial write */
      NSINC2(mqtt_stats, connect_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required) and to complete the CONNECT/CONNACK exchange) 
   */
  tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
  if (tx_cb == NULL)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_tx_connect: fail to alloc MQTT_TxCB");
    NSINC2(mqtt_stats, alloc_failures);
    NSINC2(mqtt_stats, connect_fails);
    *status = MQTT_ERR_ALLOC_FAILURE;
    return (EFAILURE);
  }
  tx_cb->next   = NULL;
  tx_cb->buf    = tx_buf;
  tx_cb->length = len;
  /* keep track of what has been written into the socket */
  tx_cb->offset = rc;
  tx_cb->created = CTICKS;
  tx_cb->response = CONNACK;
  mqtt_add_txcb(conn, tx_cb);
  #ifdef MQTT_ENABLE_LOG_INFO
  old_state = conn->state;
  #endif
  conn->state = MQTT_ST_MQTT_CONNECTING;
  MQTT_LOG_INFO_STRF("mqtt_tx_connect: conn %p %s -> MQTT_CONNECTING",conn, mqtt_get_conn_state_name(old_state));

  MQTT_LOG_INFO1("mqtt_tx_connect: queued CONNECT req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
}

#ifdef NET_STATS
void
mqtt_incr_sess_estab_count(MQTT_CONN_PTR conn)
{
  if (!mqtt_is_secure_conn(conn))
  {
    NSINC2(mqtt_stats, clear_sessions_established);
    NSINC2(mqtt_stats, active_clear_sessions);
  }
  else
  {
    NSINC2(mqtt_stats, secure_sessions_established);
    NSINC2(mqtt_stats, active_secure_sessions);
  }
}
#endif

void *mqtt_config_conn(MQTT_CONN_PTR conn, MQTT_CFG_PTR cfg, MQTT_RESTORE_PTR restore, MQTT_ERR_TYPE *status)
{
  /* copy socket address configuration into MQTT_CONN data structure */
  conn->cfg.local = cfg->local;
  conn->cfg.remote = cfg->remote;
  conn->cfg.allow_unsub_pub = cfg->allow_unsub_pub;

  conn->cfg.client_id_len = cfg->client_id_len;
  conn->callback = cfg->callback;
  conn->cfg.nonblocking_connect = cfg->nonblocking_connect;
  memscpy(conn->cfg.options, sizeof(conn->cfg.options), cfg->options,
          sizeof(cfg->options));
  conn->cfg.security_mode= cfg->security_mode;
  conn->flags = 0;
  conn->cfg.last_error = 0;
  conn->cfg.module_cat = QAPI_NET_NO_MODULE_CAT;

  if(
     /*username check*/
     ((cfg->username     != NULL && (cfg->username_len     <= 0 || cfg->username_len > QAPI_NET_MQTT_MAX_USERNAME_LEN)) ||
       (cfg->username     == NULL && cfg->username_len     != 0))    ||
     /*password check*/
     ((cfg->password     != NULL && (cfg->password_len     <= 0 || cfg->password_len > QAPI_NET_MQTT_MAX_PASSWORD_LEN)) ||
       (cfg->password     == NULL && cfg->password_len     != 0))    ||
     /*"If the User Name Flag is set to 0, the Password Flag MUST be set to 0 [MQTT-3.1.2-22]"*/
     (cfg->username      == NULL && cfg->password      != NULL)      ||
     /*will topic check*/
     ((cfg->will_topic   != NULL && (cfg->will_topic_len   <= 0 || cfg->will_topic_len > QAPI_NET_MQTT_MAX_WILL_TOPIC_LEN)) ||
       (cfg->will_topic   == NULL && cfg->will_topic_len   != 0))    ||
     /*will message check*/
     ((cfg->will_message != NULL && (cfg->will_message_len <= 0 || cfg->will_message_len > QAPI_NET_MQTT_MAX_WILL_MESSAGE_LEN)) ||
       (cfg->will_message == NULL && cfg->will_message_len != 0))    ||
     /*client id length check only, no need to check cfg->client_id as it's an array, no chance to be NULL*/
     (cfg->client_id_len > MAX_CLIENT_ID_LEN || cfg->client_id_len < 0) ||
     /*If the Client supplies a zero-byte ClientId with CleanSession set to 0,the Server MUST
       respond to the CONNECT Packet with a CONNACK return code 0x02 (Identifier rejected) and
       then close the Network Connection [MQTT-3.1.3-8]
     */
     (cfg->client_id_len == 0 && cfg->clean_session == FALSE)        ||
     /*will_qos check*/
     (cfg->will_qos != QOS_0 && cfg->will_qos != QOS_1 && cfg->will_qos != QOS_2)||
	 /*keepalive check*/
     (cfg->keepalive_duration > 65535))
  {
    MQTT_LOG_ERR("mqtt_config_conn: bad parameter");
    NSINC2(mqtt_stats, parameter_error);
    NSINC2(mqtt_stats, connect_fails);
	cfg->last_error = MQTT_ERR_BAD_PARAM;
	cfg->module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_PARAM;
    return NULL;
  }

  memscpy(conn->cfg.client_id, cfg->client_id_len, cfg->client_id, cfg->client_id_len);

  /* copy username and password fields */
  if(!mqtt_config_str_len(&conn->cfg.username,     &conn->cfg.username_len,     cfg->username,     cfg->username_len))
    goto exit;
  if(!mqtt_config_str_len(&conn->cfg.password,     &conn->cfg.password_len,     cfg->password,     cfg->password_len))
    goto exit;
  if(!mqtt_config_str_len(&conn->cfg.will_topic,   &conn->cfg.will_topic_len,   cfg->will_topic,   cfg->will_topic_len))
    goto exit;
  if(!mqtt_config_str_len(&conn->cfg.will_message, &conn->cfg.will_message_len, cfg->will_message, cfg->will_message_len))
    goto exit;

  if (cfg->sock_options != NULL)
  {
    conn->cfg.sock_options = MQTTC_ALLOC(sizeof(qapi_Net_MQTT_Sock_Opts_t));
    if (conn->cfg.sock_options != NULL)
    {
        conn->cfg.sock_options = cfg->sock_options;
        conn->cfg.sock_options_cnt = cfg->sock_options_cnt;
    }
    else
    {
      MQTT_LOG_DEBUG("mqtt_config_conn: fail to alloc will_message");
      goto exit;
    }
  }
  conn->cfg.keepalive_duration = cfg->keepalive_duration*TPS;
  conn->cfg.connack_timed_out_sec = cfg->connack_timed_out_sec;

  if (cfg->clean_session == TRUE && restore != NULL)
  {
    MQTT_LOG_DEBUG("mqtt_config_conn: inconsistent config (clean session and restore state)");
    NSINC2(mqtt_stats, parameter_error);
    NSINC2(mqtt_stats, connect_fails);
    *status = MQTT_ERR_BAD_PARAM;
    return (NULL);
  }
  conn->restore = restore;
  conn->cfg.clean_session = cfg->clean_session;

  conn->cfg.will_retained = cfg->will_retained;
  conn->cfg.will_qos = cfg->will_qos;
  
  conn->cfg.pass_msg_hdr = cfg->pass_msg_hdr;
  conn->cfg.max_chunk= cfg->max_chunk;
  conn->cfg.max_chunk_delay_ms= cfg->max_chunk_delay_ms;
  conn->cfg.max_chunk_retries= cfg->max_chunk_retries;
  if(cfg->pub_ack_timeout == 0)
  {
  	conn->cfg.pub_ack_timeout = 5;
  }
  else
  {
  	conn->cfg.pub_ack_timeout = cfg->pub_ack_timeout;
  }
  
  /* initialize the MQTT receiver-related state variables */
  conn->rx.state = MQTT_RX_ST_WAITING_FOR_MSG_PART1;
  conn->rx.buf = NULL;
  conn->rx.offset = 0;
  return conn;
exit:
  MQTT_LOG_DEBUG("mqtt_config_conn: fail to alloc memory");
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, connect_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  return (NULL);
}

/* request MQTT to create a connection */
MQTT_CONN *
mqtt_connect(qapi_Net_MQTT_Hndl_t hndl, MQTT_CFG_PTR cfg, MQTT_RESTORE_PTR restore, MQTT_ERR_TYPE *status, uint8_t *session_present)
{
  MQTT_CONN_PTR conn;
  uint16_t family;
  SOCKTYPE sock;
  int non_blocking;
  int len = 0;
  uint8_t *txbuf;

  uint8_t rxbuf[MQTT_HDR_SZ]; /* first two bytes of CONNACK message */
  int offset;
  uint8_t *rx_msg; /* entire CONNACK message */
  uint8_t msg_type;
  #ifdef MQTT_ENABLE_LOG_INFO
  uint8_t msg_flags;
  #endif
  int total_length;
  uint8_t connack_rc;
  int rc;
  struct timeval timeout;
  uint32_t cntr;
  int32_t errnum;
  //int ssl_conn_status;
#ifdef MQTT_CLIENT_SECURITY
  int result;
#endif

  NSINC2(mqtt_stats, connect_requests);

  if(cfg != NULL)
  {
  	cfg->last_error = 0;
	cfg->module_cat = QAPI_NET_NO_MODULE_CAT;
  }
  if (cfg == NULL || status == NULL || session_present == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_connect: parameter error");
	
	if(cfg != NULL)
	{
		cfg->last_error = MQTT_ERR_BAD_PARAM;
		cfg->module_cat = QAPI_NET_MQTT_CAT;
	}
	
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
    return (NULL);
  }

  MQTT_LOCK_MUTEX(mqtt_mutex);

  conn = mqtt_find_conn(&cfg->local, &cfg->remote, 
      cfg->client_id, cfg->client_id_len);
  if (conn == NULL)
  {
    MQTT_UNLOCK_MUTEX(mqtt_mutex);
    MQTT_LOG_INFO("mqtt_connect: couldn't find existing conn; creating new one...");
    /* create a new connection */
    conn = MQTTC_ALLOC(sizeof(MQTT_CONN));
    if (conn == NULL)
    {
      MQTT_LOG_DEBUG("mqtt_connect: fail to create CONN");
      goto exit_alloc_failure;
    }
#ifndef SUPERLOOP
    conn->mutex = MUTEX_ALLOC();
    if (conn->mutex == NULL)
    {
      MQTT_LOG_DEBUG("mqtt_connect: fail to create mutex for CONN");
      MQTTC_FREE(conn);
      goto exit_alloc_failure;
    }
#endif
    if(mqtt_config_conn(conn, cfg, restore, status) == NULL)
    {
      mqtt_free_conn(conn);
      MQTT_LOG_DEBUG1("mqtt_connect: fail config for conn %p", conn);
      return (NULL);
    }
    
    non_blocking = conn->cfg.nonblocking_connect;
    family = conn->cfg.local.sa_family;

    if ((conn->sock = qapi_socket(family, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
      cfg->last_error = INVALID_SOCKET;
	  cfg->module_cat = QAPI_NET_SOCKET_CAT;
      mqtt_free_conn(conn);
      MQTT_LOG_DEBUG1("mqtt_connect: fail to create sock for conn %p", conn);
      goto exit_socket_failure;
    }

#ifdef IP_V4
    if (family == AF_INET)
    {
      len = sizeof(struct sockaddr_in);
    }
#endif
#ifdef IP_V6
    if (family == AF_INET6)
    {
      len = sizeof(struct sockaddr_in6);
    }
#endif

    /* Configure the application specific socket options */
    for (cntr = 0; cntr < conn->cfg.sock_options_cnt; cntr++)
    {
      int32_t level = conn->cfg.sock_options[cntr].level;
      int32_t opt_name = conn->cfg.sock_options[cntr].opt_name;
      void *opt_value = conn->cfg.sock_options[cntr].opt_value;
      uint32_t opt_len = conn->cfg.sock_options[cntr].opt_len;
    
      rc = qapi_setsockopt(conn->sock, level, opt_name, opt_value, opt_len);
      if (rc != ESUCCESS)
      {
        MQTT_LOG_ERR1("mqtt_connect: fail config sock %lx "
                      "for linger ", conn->sock);
		errnum = qapi_errno(conn->sock);
		cfg->last_error = errnum;
	  	cfg->module_cat = QAPI_NET_SOCKET_CAT;
        mqtt_free_conn(conn);
        *status = MQTT_ERR_SOCKET_FATAL_ERROR;
        goto exit_socket_failure;
      }
      else
      {
        MQTT_LOG_PRINT2("mqtt_connect: config sock %lx for %d", conn->sock, opt_name);
      }
    }

    rc = qapi_bind(conn->sock, &cfg->local, len);
    MQTT_LOG_ERR2("mqtt_connect: qapi_bind return %d for conn %p", rc, conn);
    if (rc != ESUCCESS)
    {
      int sock_err;
      errnum = qapi_errno(conn->sock);
	  cfg->last_error = errnum;
	  cfg->module_cat = QAPI_NET_SOCKET_CAT;
      *status = MQTT_ERR_TCP_BIND_FAILED;
      rc = qapi_getsockopt(conn->sock, SOL_SOCKET, SO_ERROR, &sock_err, 
          sizeof(int));
      MQTT_LOG_ERR2("mqtt_connect: qapi_getsockopt return %d for conn %p", rc, conn);
      if (rc == ESUCCESS)
      {
        MQTT_LOG_ERR2("mqtt_connect: errno for fail bind: %d "
              "for conn %p", sock_err, conn);
      }
      mqtt_free_conn(conn);
      goto exit_socket_failure;
    }

    conn->state = MQTT_ST_DORMANT;
    MQTT_LOG_INFO1("mqtt_connect: conn %p <none> -> DORMANT", conn);
    conn->refcount = 1;
    TK_MUTEX_PEND(conn->mutex, 0);
  }
  else
  {
    TK_MUTEX_PEND(conn->mutex, 0);
    //only increase refcount when different handle/context share the connection
    if(!is_mqtt_ctx_and_conn_pair_exist(hndl, conn))
    {
      ++conn->refcount;
      MQTT_LOG_INFO2("mqtt_connect: conn %p refcount increased to %d", conn, conn->refcount);
    }
    else
    {
      MQTT_LOG_INFO1("mqtt_connect: the same handle calling multiple connection, refcount is : %d", conn->refcount);
    }

    if(conn->state == MQTT_ST_DYING)
    {
      MQTT_ERR_TYPE status_temp = MQTT_ERR_NO_ERR;
      MQTT_LOG_INFO("mqtt_connect: conn state is DYING "
            "Re-establish conn");
      /*Connection with the server will be re-established*/
      conn->state = MQTT_ST_TCP_CONNECTING;
      if(EFAILURE == mqtt_conn_establish(conn, &status_temp))
      {
        *status = status_temp;
        TK_MUTEX_POST(conn->mutex);
        MQTT_UNLOCK_MUTEX(mqtt_mutex);
        return NULL;
      }
    }

    TK_MUTEX_POST(conn->mutex);
    MQTT_UNLOCK_MUTEX(mqtt_mutex);
    *status = MQTT_ERR_NO_ERR;
    MQTT_LOG_PRINT("mqtt_connect: Conn exist");
    return (conn);
  }

  /* add newly created connection to list of connections */
  mqtt_add_conn(conn);

  sock = conn->sock;
  if (non_blocking)
  {
    rc = qapi_setsockopt(sock, SOL_SOCKET, SO_NONBLOCK, &non_blocking, sizeof(int));
    if (rc != ESUCCESS)
    {
      mqtt_destroy_conn(conn, TRUE);
      MQTT_LOG_DEBUG1("mqtt_connect: fail to config sock %lx for non-blocking mode", sock);

	  errnum = qapi_errno(conn->sock);
	  cfg->last_error = errnum;
	  cfg->module_cat = QAPI_NET_SOCKET_CAT;
	  
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      goto exit_socket_failure;
    }
  }
  
#ifdef IP_V4
  if (family == AF_INET)
  {
    len = sizeof(struct sockaddr_in);
  }
#endif
#ifdef IP_V6
  if (family == AF_INET6)
  {
    len = sizeof(struct sockaddr_in6);
  }
#endif

  rc = qapi_connect(sock, &conn->cfg.remote, len);
  MQTT_LOG_ERR2("mqtt_connect: qapi_connect return %d for conn %p", rc, conn);
        
  if (rc != ESUCCESS)
  {
    int sock_err;

    MQTT_LOG_ERR1("mqtt_connect: qapi_connect fail for conn %p", conn);
          
    rc = qapi_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_err, sizeof(int));
    MQTT_LOG_ERR2("mqtt_connect: qapi_getsockopt:"
          "return %d for conn %p", rc, conn);

    if (rc != ESUCCESS || sock_err != EINPROGRESS)
    {
      if(rc == ESUCCESS)
      {
        MQTT_LOG_ERR2("mqtt_connect: errno for fail conn: %d "
          "for conn %p", sock_err, conn);
	    cfg->last_error = sock_err;
		cfg->module_cat = QAPI_NET_SOCKET_CAT;
      }
      mqtt_destroy_conn(conn, TRUE);
      *status = MQTT_ERR_TCP_CONN_FAILED;
      goto exit_socket_failure;
    }
  }

  MQTT_LOG_INFO("mqtt_connect: qapi_connect succeed");

  /* qapi_connect() has succeeded.  For a blocking connect, this implies that 
   * we have completed the TCP connection process; for a non-blocking 
   * socket, this implies that a TCP connection request has been initiated.
   */
  conn->state = non_blocking? MQTT_ST_TCP_CONNECTING : MQTT_ST_TCP_CONNECTED;
  MQTT_LOG_INFO(("mqtt_connect: conn %p DORMANT -> %s\n", conn, mqtt_get_conn_state_name(conn->state)));
  
  /*copying this field here to access ssl_cfg.protocol field so that we can decide conn is secure or not*/
  memscpy(&(conn->cfg.ssl_cfg), sizeof(cfg->ssl_cfg), &(cfg->ssl_cfg), sizeof(cfg->ssl_cfg));
  memscpy(&(conn->cfg.ca_list), sizeof(cfg->ca_list), &(cfg->ca_list), sizeof(cfg->ca_list));
  memscpy(&(conn->cfg.cert), sizeof(cfg->cert), &(cfg->cert), sizeof(cfg->cert));
  memscpy(&(conn->cfg.psk), sizeof(cfg->psk), &(cfg->psk), sizeof(cfg->psk));
  
  MQTT_LOG_INFO_STRF("mqtt_connect: conn %p DORMANT -> %s\n", conn, mqtt_get_conn_state_name(conn->state));
  if (non_blocking)
  {
    TK_MUTEX_POST(conn->mutex);
    *status = MQTT_ERR_NO_ERR;
    SignalMqtt();
    return (conn);
  }

#ifdef MQTT_CLIENT_SECURITY
  if (mqtt_is_secure_conn(conn))
  {
#ifdef MQTT_ENABLE_LOG_INFO
    MQTT_CONN_STATE old_state = conn->state;
#endif
    if(!QAPI_MQTT_IS_OPTION(&(conn->cfg), QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE))
    {
      /*if no security mode is set, regard it as cert mode as the default value*/
      conn->cfg.security_mode =QAPI_MQTT_CERTIFICATE;
      QAPI_MQTT_SET_OPTION(&(conn->cfg), QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE);
    }
    conn->state = MQTT_ST_SSL_CONNECTING;
    MQTT_LOG_INFO_STRF("mqtt_connect: conn %p %s -> SSL_CONNECTING",conn, mqtt_get_conn_state_name(old_state));
    result = mqtt_ssl_cert_load(conn, cfg);
    if(result < QAPI_OK)
   	{
      cfg->last_error = result;
	  cfg->module_cat = QAPI_NET_SSL_CAT;
      goto exit_ssl_failure;
    }

    result = mqtt_start_ssl_connection(conn);
    if((result < QAPI_OK) && (result != QAPI_SSL_OK_HS))
    {
      cfg->last_error = result;
	  cfg->module_cat = QAPI_NET_SSL_CAT;
      goto exit_ssl_failure;
    }
  }
#endif

  /* create the CONNECT message */
  txbuf = mqtt_create_connect_msg(conn, &len, status);
  if (txbuf == NULL)
  {
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, connect_fails);
    return (NULL);
  }

  /* send the CONNECT message to the server */
  rc = mqtt_wr_sock(conn, txbuf, len, 0, status);
  if (rc != len)
  {
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, connect_fails);
    return (NULL);
  }
  NSINC2(mqtt_stats, connect_out);
  MQTTC_FREE(txbuf);

  /* set socket timeout */
  timeout.tv_sec = conn->cfg.connack_timed_out_sec;
  timeout.tv_usec = 0;

  if (qapi_setsockopt (conn->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == -1)
  {
      MQTT_LOG_DEBUG("mqtt_connect: setsockopt fail");
	  cfg->last_error = EFAILURE;
	  cfg->module_cat = QAPI_NET_SOCKET_CAT;
      mqtt_destroy_conn(conn, TRUE);
      NSINC2(mqtt_stats, connect_fails);
      return (NULL);
  }

  /* wait to receive the CONNACK message from the server until timeout.
   * read minimum length of message from server.
   */
  rc = mqtt_rd_sock(conn, &rxbuf[0], 2, 0, status);
  if (rc != 2)
  {
    MQTT_LOG_ERR2("mqtt_connect: mqtt_rd_sock return %d for conn %p", rc, conn);
	cfg->last_error = conn->cfg.last_error;
	cfg->module_cat = conn->cfg.module_cat;
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, connect_fails);
    return (NULL);
  }

  msg_type = MQTT_MSG_TYPE(rxbuf);
  #ifdef MQTT_ENABLE_LOG_INFO
  msg_flags = MQTT_MSG_FLAG(rxbuf);
  #endif
  if (msg_type != CONNACK)
  {
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, connect_fails);
    *status = MQTT_ERR_UNEXPECTED_MSG;
    return (NULL);
  }
  NSINC2(mqtt_stats, connack_in);

  MQTT_LOG_INFO2("mqtt_connect: msg %u flags %u", msg_type, msg_flag);

  {
    uint8_t msg_len[MQTT_MAX_MSG_LEN];
    int index = 0;
    int i;
    int multiplier;

    msg_len[index++] = rxbuf[1] & 0x7f;
    if ((rxbuf[1] & 0x80) == 0)
    {
      MQTT_LOG_DEBUG1("mqtt_connect: length %u bytes (specified in one byte)", msg_len[0]);
      total_length = msg_len[0];

    }
    else
    {
      bool_t more = TRUE;

      do
      {
        rc = mqtt_rd_sock(conn, &msg_len[index], 1, 0, status);
        if (rc != 1)
        {
          cfg->last_error = conn->cfg.last_error;
	      cfg->module_cat = conn->cfg.module_cat;
          mqtt_destroy_conn(conn, TRUE);
          NSINC2(mqtt_stats, connect_fails);
          return (NULL);
        }
        more = msg_len[index] & 0x80;
        if (more)
        {
          msg_len[index++] &= 0x7f;
        }
      } while ((more) && (index < MQTT_MAX_MSG_LEN));
    }

#ifdef MQTT_DEBUG
    if (mqtt_log_level & DUMP_LOG_MSGS)
    {
      MQTT_LOG_MSGS("mqtt_connect: Length data:");
      gio_hexdump(NULL, msg_len, index, FALSE, FALSE);
    }
#endif
    /* compute the total length of the message
     *
     * total_length = msg_len[0] + msg_len[1] * 128 + 
     *                msg_len[2] * 128^2 + msg_len[3] * 128^3;
     */
    for (i = 0, total_length = 0, multiplier = 1; i < index; i++)
    {
      total_length += msg_len[i] * multiplier;
      multiplier *= 128;
    }
    MQTT_LOG_DEBUG2("mqtt_connect: length %u bytes (specified in %d bytes)", total_length, index);

    offset = index + 1;
    total_length += offset;
    MQTT_LOG_INFO("mqtt_connect: Total length of CONNACK: %d bytes", total_length);
    rx_msg = MQTTC_ALLOC(total_length);
    if (rx_msg == NULL)
    {
      MQTT_LOG_DEBUG("mqtt_connect: fail to alloc rx buf for CONNACK");
      mqtt_destroy_conn(conn, TRUE);
      goto exit_alloc_failure;
    }
    rx_msg[0] = rxbuf[0];
    memscpy(rx_msg + 1,index, msg_len, index);
  }


  /* wait to read the entire CONNACK message from the socket */
  rc = mqtt_rd_sock(conn, rx_msg + offset, total_length - offset, 0, status);

  if (rc != total_length - offset)
  {
    cfg->last_error = conn->cfg.last_error;
	cfg->module_cat = conn->cfg.module_cat;
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, connect_fails);
    *status = MQTT_ERR_SOCKET_FATAL_ERROR;
    return (NULL);
  }
  
#ifdef APPS_PACKET_LOGGING_ENABLE  
  dump_buf_to_dpl(conn->sock, rx_msg, total_length,TRUE);
#endif

  /**
   * Deserializes the supplied (wire) buffer into connack data - return code
   * @param sessionPresent the session present flag returned (only for MQTT 3.1.1)
   * @param connack_rc returned integer value of the connack return code
   * @param buf the raw buffer data, of the correct length determined by the remaining length field
   * @param len the length in bytes of the data in the supplied buffer
   * @return error code.  1 is success, 0 is failure
   */
  rc = MQTTDeserialize_connack(session_present, &connack_rc, rx_msg, 
      total_length);
  MQTTC_FREE(rx_msg);
  MQTT_LOG_DEBUG1("mqtt_connect:  MQTTDeserialize_connack() return %d", rc);
  if (rc == 0)
  {
    /* in the event of a failure, close underlying TCP connection */
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, badconnack_in);
    NSINC2(mqtt_stats, connect_fails);
	cfg->last_error = MQTT_ERR_MSG_DESERIALIZATION_FAILURE;
	cfg->module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_MSG_DESERIALIZATION_FAILURE;
    return (NULL);
  }


  MQTT_LOG_PRINT2("mqtt_connect: session present %u connack_rc %u", *session_present, connack_rc);

  /* return back to caller; use information from server's response to 
   * determine return value
   */
  if (connack_rc != 0)
  {
    if(connack_rc == 1)
       conn->callback(conn, MQTT_CONNECT_PROTOCOL_VER_ERROR);
    else if(connack_rc == 2)
       conn->callback(conn, MQTT_CONNECT_IDENTIFIER_ERROR);
    else if(connack_rc == 3)
       conn->callback(conn, MQTT_CONNECT_SERVER_UNAVAILABLE);
    else if(connack_rc == 4)
       conn->callback(conn, MQTT_CONNECT_MALFORMED_DATA);
    else if(connack_rc == 5)
       conn->callback(conn, MQTT_CONNECT_UNAUTHORIZED_CLIENT);
    else
       conn->callback(conn, MQTT_MQTT_CONNECT_FAILED);

    /* connack_rc values of 1 thru' 5 */
    mqtt_destroy_conn(conn, TRUE);
    NSINC2(mqtt_stats, badconnack_in);
    NSINC2(mqtt_stats, connect_fails);
    *status = MQTT_ERR_NEGATIVE_CONNACK; 
	cfg->last_error = MQTT_ERR_NEGATIVE_CONNACK;
	cfg->module_cat = QAPI_NET_MQTT_CAT;
    return (NULL);
  }
  
  MQTT_LOG_PRINT("mqtt_connect : CONNACK recevied");
  
  /* configure socket for non-blocking mode for subsequent I/O */
  non_blocking = TRUE;
  /* use portable version of this option (check refs) */
  rc = qapi_setsockopt(sock, SOL_SOCKET, SO_NONBLOCK, &non_blocking, sizeof(int));
  if (rc == SOCKET_ERROR)
  {
    MQTT_LOG_DEBUG1("mqtt_connect: fail to config socket %lx for non-blocking mode", sock);

	errnum = qapi_errno(sock);
	cfg->last_error = errnum;
	cfg->module_cat = QAPI_NET_SOCKET_CAT;
    mqtt_destroy_conn(conn, TRUE);
    *status = MQTT_ERR_SOCKET_FATAL_ERROR;
    goto exit_socket_failure;
  }

  conn->state = MQTT_ST_MQTT_CONNECTED;
  conn->flags |= MQTT_FL_SESS_ESTAB;
#ifdef NET_STATS
  mqtt_incr_sess_estab_count(conn);
#endif
  if (conn->cfg.clean_session == FALSE && conn->restore != NULL)
  {
    if(*session_present)
    {
      if(mqtt_conn_restore_state(conn) != ESUCCESS) 
      {
        MQTT_LOG_ERR1("mqtt_connect: fail to restore conn %lx", conn);
		cfg->last_error = MQTT_ERR_RESTORE_FAILED;
		cfg->module_cat = QAPI_NET_MQTT_CAT;
        mqtt_destroy_conn(conn, TRUE);
        NSINC2(mqtt_stats, connect_fails);
        *status = MQTT_ERR_RESTORE_FAILED;
        return (NULL);    
      }
    }
    else
    {
      mqtt_store_conn_context_to_persistent(conn);
    }
  }
  if(conn->cfg.clean_session == TRUE)
  {
    mqtt_store_conn_context_to_persistent(conn);
  }
  /* callback not invoked for blocking mode configuration */

  TK_MUTEX_POST(conn->mutex);

  conn->cfg.last_error = 0;
  conn->cfg.module_cat = QAPI_NET_NO_MODULE_CAT;
  *status = MQTT_ERR_NO_ERR;
  SignalMqtt();
  return (conn);
#ifdef MQTT_CLIENT_SECURITY
exit_ssl_failure:
  mqtt_destroy_conn(conn, TRUE);
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, connect_fails);
  *status = MQTT_ERR_SSL_CONN_FAILURE;
  return (NULL);
#endif
exit_alloc_failure:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, connect_fails);
  cfg->last_error = MQTT_ERR_ALLOC_FAILURE;
  cfg->module_cat = QAPI_NET_MQTT_CAT;
  *status = MQTT_ERR_ALLOC_FAILURE;
  return (NULL);
exit_socket_failure:
  NSINC2(mqtt_stats, socket_failures);
  NSINC2(mqtt_stats, connect_fails);
  return (NULL);
}
int
mqtt_disconnect(MQTT_CONN_PTR conn, MQTT_RESTORE_PTR *restore, MQTT_ERR_TYPE *status)
{
  uint8_t *tx_buf;
  int reqd_len;
  int rc = 0;
  MQTT_TxCB *tx_cb;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_disconnect: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
    return (EFAILURE);
  }

  TK_MUTEX_PEND(conn->mutex, 0);
  if (--conn->refcount > 0)
  {
    TK_MUTEX_POST(conn->mutex);
    *status = MQTT_ERR_NONZERO_REFCOUNT;
    return (ESUCCESS);
  }

  /* destroy connection after it has encountered fatal error */
  if (conn->state == MQTT_ST_DYING)
  {
    mqtt_destroy_conn(conn, TRUE);
    *status = MQTT_ERR_NO_ERR;
    return (ESUCCESS);
  }

  reqd_len = MQTT_HDR_SZ;
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_disconnect: fail to alloc buf for DISCONNECT");
    goto exit;
  }

  MQTT_SET_DISCONN_HDR_BITS(tx_buf);

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS ("mqtt_disconnect: DISCONNECT");
    gio_hexdump(NULL, tx_buf, reqd_len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if tx queue is empty */
  //sock = conn->sock;
  conn->state = MQTT_ST_MQTT_TERMINATING;

  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, reqd_len, 0, status); 
    if (rc == EFAILURE)
    {
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      MQTT_LOG_ERR1("mqtt_disconnect: conn %p fatal err on sock", conn);
	  
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      mqtt_destroy_conn(conn, TRUE);
      return (rc);
    }
  }

  /* save the state of the transmission request (to complete transmission) */
  if (rc < reqd_len)
  {
    tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
    if (tx_cb == NULL)
    {
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      MQTT_LOG_DEBUG("mqtt_disconnect: fail to alloc MQTT_TxCB");
      goto exit;
    }
    tx_cb->next   = NULL;
    tx_cb->buf    = tx_buf;
    tx_cb->length = reqd_len;
    /* keep track of what has been written into the socket */
    tx_cb->offset = rc;
    tx_cb->created = CTICKS;
    tx_cb->response = MQTT_NO_RESPONSE_MSG_REQD;
    mqtt_add_txcb(conn, tx_cb);
    TK_MUTEX_POST(conn->mutex);
  }
  else
  {
    NSINC2(mqtt_stats, disconnect_out);
    mqtt_destroy_conn(conn, TRUE);
    MQTTC_FREE(tx_buf);
  }

  MQTT_LOG_INFO1("mqtt_disconnect: queued DISCONNECT req (%d bytes) for transmission", reqd_len);

  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  *status = MQTT_ERR_ALLOC_FAILURE;
  mqtt_destroy_conn(conn, TRUE);
  return (EFAILURE);
}

uint16_t
mqtt_rand16(void)

{
  uint16_t num = RANDOM_FN()%65536; 

  /* XXX add check to ensure uniqueness (among other ongoing transactions)? */
  return (num);
}

/*
 * 
 * Serializes the supplied publish data into the supplied buffer, ready for sending
 * @param buf the buffer into which the packet will be serialized
 * @param buflen the length in bytes of the supplied buffer
 * @param dup integer - the MQTT dup flag
 * @param qos integer - the MQTT QoS value
 * @param retained integer - the MQTT retained flag
 * @param packetid integer - the MQTT packet identifier
 * @param topicName MQTTString - the MQTT topic in the publish
 * @param payload byte buffer - the MQTT publish payload
 * @param payloadlen integer - the length of the MQTT payload
 * @return the length of the serialized data.  <= 0 indicates error
 int MQTTSerialize_publish(unsigned char* buf, int buflen, unsigned char dup, int qos, 
                           unsigned char retained, unsigned short packetid, MQTTString topicName, 
                           unsigned char* payload, int payloadlen);
 */
int
mqtt_publish2(MQTT_CONN_PTR conn, uint8_t *topic, int topic_len, uint8_t *msg, 
    int msg_len, int qos, bool_t retained, MQTT_ERR_TYPE *status,  MQTT_PUBLISH_CB cbk,
    uint16_t *msg_id)
{
  MQTT_CONN_STATE state;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  MQTTString topic_str;
  uint16_t packet_id;
  int rc = 0;

  if ((conn == NULL) ||
      (topic == NULL || (topic_len <= 0 || topic_len > QAPI_NET_MQTT_MAX_TOPIC_LEN)) ||
      ((msg == NULL && msg_len > 0 ) || msg_len < 0) ||
      (qos != QOS_0 && qos != QOS_1 && qos != QOS_2) ||
      (status == NULL))
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_publish2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }

	if(conn != NULL)
	{
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  TK_MUTEX_PEND(conn->mutex, 0);
  state = conn->state;

  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    TK_MUTEX_POST(conn->mutex);
	ds_iot_log_strf_err("mqtt_publish2: publish reject in state %d[%s]", state, mqtt_get_conn_state_name(state));
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    NSINC2(mqtt_stats, publish_fails);
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  /*Update the publish callback in the MQTT connection*/
  conn->pub_cb = cbk;

  /* create message */
  mqtt_config_string_struct(&topic_str, (char *)topic, topic_len);

  reqd_len = MQTTPacket_len(MQTTSerialize_publishLength(qos, topic_str, msg_len));
  MQTT_LOG_DEBUG1("mqtt_publish2: MQTTPacket_len() return %d", reqd_len);
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_publish2: fail to alloc buf for PUBLISH message");
    goto exit;
  }

  packet_id = mqtt_rand16();
  *msg_id = packet_id;
  /* dup = FALSE (always) */
  len = MQTTSerialize_publish(tx_buf, reqd_len, FALSE, qos, (unsigned char)retained, 
      packet_id, topic_str, msg, msg_len);
  MQTT_LOG_DEBUG1("mqtt_publish2: MQTTSerialize_publish() return %d", len);
  if (len <= 0)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTTC_FREE(tx_buf);
    MQTT_LOG_ERR("mqtt_publish2: failure");
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    NSINC2(mqtt_stats, publish_fails);
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS ("mqtt_publish2: PUBLISH");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if the tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    MQTT_LOG_PRINT3("mqtt_publish2: Publish buf %p payload len %d and qos=%d", tx_buf, len, qos);
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {

      MQTT_LOG_ERR1("mqtt_publish2: conn %p fatal err on sock", conn);
      conn->state = MQTT_ST_DYING;

      MQTT_LOG_INFO_STRF("mqtt_publish2: conn %p %s -> DYING", conn, mqtt_get_conn_state_name(state));
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      NSINC2(mqtt_stats, publish_fails);
      NSINC2(mqtt_stats, transitioned_to_dying);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      switch (qos)
      {
        case QOS_0:
          NSINC2(mqtt_stats, publish0_out);
          break;
        case QOS_1:
          NSINC2(mqtt_stats, publish1_out);
          break;
        case QOS_2:
          NSINC2(mqtt_stats, publish2_out);
          break;
      }
    }
    else 
    {
      MQTT_LOG_DEBUG2("mqtt_publish2: sent %d of %d bytes", rc, len);
    }
  }

  /* queue the data for subsequent transmission if none or a part of the 
   * data was written into the socket, or if the QoS for the PUBLISH 
   * transaction is greater than zero
   */
  if (rc < len || qos > 0)
  {
    MQTT_TxCB *tx_cb;

    tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
    if (tx_cb == NULL)
    {
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      MQTT_LOG_DEBUG("mqtt_publish2: fail to alloc MQTT_TxCB");
      goto exit;
    }
    tx_cb->next   = NULL;
    tx_cb->buf    = tx_buf;
    tx_cb->length = len;
    /* keep track of what has been written into the socket */
    tx_cb->offset = rc;
    tx_cb->id = packet_id;
    tx_cb->created = CTICKS;
    tx_cb->pub_retransmit_count = 0;
    switch (qos)
    {
      case QOS_0:
        tx_cb->qos = QOS_0;
        tx_cb->response = MQTT_NO_RESPONSE_MSG_REQD;
        break;
      case QOS_1:
        tx_cb->qos = QOS_1;
        tx_cb->response = PUBACK;
        break;
      case QOS_2:
        tx_cb->qos = QOS_2;
        tx_cb->response = PUBREC;
        break;
    }
    mqtt_add_txcb(conn, tx_cb);
  }
  else
  {
    MQTTC_FREE(tx_buf);
  }

  TK_MUTEX_POST(conn->mutex);
  MQTT_LOG_INFO1("mqtt_publish2: queued PUBLISH req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, publish_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

/**
 * Serializes a pubrel packet into the supplied buffer.
 * @param buf the buffer into which the packet will be serialized
 * @param buflen the length in bytes of the supplied buffer
 * @param dup integer - the MQTT dup flag
 * @param packetid integer - the MQTT packet identifier
 * @return serialized length, or error if 0
 */
int MQTTSerialize_pubrel(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid);

/* note the absence of mutual exclusion (locking) primitives */
int
mqtt_pubrel2(MQTT_CONN_PTR conn, uint16_t pkt_id, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  int rc = 0;
  MQTT_TxCB *tx_cb;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_pubrel2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
    return (EFAILURE);
  }


  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    ds_iot_log_strf_err("mqtt_pubrel2: reject attempt to send PUBREL in state %d[%s]", state, mqtt_get_conn_state_name(conn->state));
    NSINC2(mqtt_stats, pubrel_fails);
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  reqd_len = MQTT_PUB_HDR_SZ;
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_pubrel2: fail to alloc buf for PUBREL");
    goto exit;
  }

  len = MQTTSerialize_pubrel(tx_buf, reqd_len, FALSE, pkt_id);
  MQTT_LOG_DEBUG1("mqtt_pubrel2: MQTTSerialize_pubrel() return %d", len);
  if (len <= 0)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_pubrel2: fail to create PUBREL");
    NSINC2(mqtt_stats, pubrel_fails);
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS("mqtt_pubrel2: PUBREL:");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if the tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      MQTTC_FREE(tx_buf);
      MQTT_LOG_ERR1("mqtt_pubrel2: conn %p fatal err on sock", conn);
      NSINC2(mqtt_stats, pubrel_fails);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, pubrel_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required) and to complete the PUBREL/PUBCOMP exchange) 
   */
  tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
  if (tx_cb == NULL)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_pubrel2: fail to alloc MQTT_TxCB");
    goto exit;
  }
  tx_cb->next   = NULL;
  tx_cb->buf = tx_buf;
  tx_cb->length = len;
  /* keep track of what has been written into the socket */
  tx_cb->offset = rc;
  tx_cb->id = pkt_id;
  tx_cb->created = CTICKS;
  tx_cb->qos = QOS_2;
  tx_cb->response = PUBCOMP;
  tx_cb->pub_retransmit_count = 0;
  mqtt_add_txcb(conn, tx_cb);

  MQTT_LOG_INFO1("mqtt_pubrel2: queued PUBREL req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, pubrel_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

MQTT_CHK_SUB_RESULT
mqtt_chk_subscription(MQTT_CONN_PTR conn, uint8_t *topic, int length,
    int requested_qos, void *sid, MQTT_SUBSCRIPTION_CB cbk)
{
  MQTT_SubRxCB_PTR ptr;
  int i, j;
  int sid_count;
  MQTT_SubRxCB_PTR next_ptr;
  MQTT_SubRxCB_PTR prev;

  if (conn == NULL || topic == NULL || length <= 0 || 
      (requested_qos != QOS_0 && requested_qos != QOS_1 && requested_qos != QOS_2))
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_chk_subscription: parameter error");
	if(conn != NULL)
	{
	    if(requested_qos > QOS_2)
	   {
	   		conn->cfg.last_error = MQTT_ERR_REJECTED_SUBSCRIPTION_HIGHER_QOS;
       		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	   }
	   else
	   {
	   		conn->cfg.last_error = MQTT_ERR_SUBSCRIPTION_BAD_PARAM;
       		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	   }
	}
    return (MQTT_SUBSCRIPTION_BAD_PARAM);
  }

   prev = NULL;
   next_ptr = NULL;
  /* iterate thru' list of subscriptions on connection */
  for (ptr = conn->subq; ptr; ptr = next_ptr)
  {
      next_ptr = ptr->next;
    /* iterate thru' all topics in a single subscription request */
    for (i = 0; i < ptr->count; i++)
    {
      /* compare length of topic */
      if (ptr->sub[i].length != length)
      {
        continue;
      }
      /* compare topic content */
      if (MEMCMP(ptr->sub[i].topic, topic, length) != 0)
      {
        continue;
      }
      /* compare SIDs for equality - if equal, error */
      for (j = 0; j < ptr->sub[i].sid_count; j++)
      {
        if ((ptr->sub[i].sid[j] == sid) && (ptr->sub[i].requested_qos == requested_qos))
        {
          /* error - duplicate subscription by same subscriber */
		  MQTT_LOG_ERR("mqtt_chk_subscription: reject duplicate subscription by same subscriber");
		  conn->cfg.last_error = MQTT_ERR_REJECTED_SUBSCRIPTION_DUP_SID;
          conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
          return (MQTT_REJECTED_SUBSCRIPTION_DUP_SID);
        }
      }

      if (ptr->sub[i].granted_qos == MQTT_GRANTED_QOS_OUTSTANDING)
      {
        /* error - subscription request is in progress */
        MQTT_LOG_ERR("mqtt_chk_subscription: reject duplicate subscription as init subscription is in progress");
		conn->cfg.last_error = MQTT_ERR_REJECTED_SUBSCRIPTION_REQ_OUTSTANDING;
        conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
        return (MQTT_REJECTED_SUBSCRIPTION_REQ_OUTSTANDING);
      }
      /* compare requested QoS with the granted QoS */
      if (ptr->sub[i].granted_qos >= requested_qos)
      {
          sid_count = ptr->sub[i].sid_count;
          if (sid_count < MQTT_MAX_DUP_SUBSCRIPTIONS + 1)
          {
              /* subscription request can be accepted; add entry to array */
              ptr->sub[i].sid[sid_count] = sid;
              ptr->sub[i].callback[sid_count] = cbk;
              ptr->sub[i].status[sid_count++] = MQTT_SUBSCRIPTION_STATUS_TEMPORARY;
              MQTT_LOG_ERR1("mqtt_chk_subscription: add duplicate subscription at slot %d", sid_count);
              return (MQTT_ADD_TO_EXISTING_SUBSCRIPTION);
          }
          else
          {
              MQTT_LOG_ERR("mqtt_chk_subscription: reject duplicate subscription due to lack of available slot in table");
			  conn->cfg.last_error = MQTT_ERR_REJECTED_SUBSCRIPTION_TBL_FULL;
              conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
              return (MQTT_REJECTED_SUBSCRIPTION_TBL_FULL);
          }
      }
      else
      {
          /* subscription request can be accepted remove the old entry*/
          MQTT_LOG_ERR2("mqtt_chk_subscription: duplicate subscritpion and granted_qos %d less than the requested_qos %d", ptr->sub[i].granted_qos, requested_qos);

          MQTTC_FREE(ptr->sub[i].topic);
          MEMSET(&ptr->sub[i], 0, sizeof(struct MQTT_subscription));
          if (prev != NULL)
          {
              prev->next = ptr->next;
          }
          else
          {
              conn->subq = ptr->next;
          }
          MQTTC_FREE(ptr);
          return (MQTT_CREATE_NEW_SUBSCRIPTION);
      }
    }
    prev = ptr;
  }

  /* did not find an existing subscription that matches; create a new 
   * subscription control block
   */
  MQTT_LOG_INFO("mqtt_chk_subscription: new subscription required");
  return (MQTT_CREATE_NEW_SUBSCRIPTION);
}

void
mqtt_update_subscriptions(MQTT_CONN_PTR conn, bool_t error)
{
  MQTT_SubRxCB_PTR ptr;
  int i, j;

  /* iterate thru' list of subscriptions on connection to undo or finalize 
   * temporary additions
   */
  for (ptr = conn->subq; ptr; ptr = ptr->next)
  {
    /* iterate thru' all topics in a single subscription request */
    for (i = 0; i < ptr->count; i++)
    {
      for (j = 0; j < ptr->sub[i].sid_count; j++)
      {
        if (ptr->sub[i].status[j] == MQTT_SUBSCRIPTION_STATUS_TEMPORARY)
        {
          if (error)
          {
            ptr->sub[i].status[j] = MQTT_SUBSCRIPTION_STATUS_NONEXISTENT;
            ptr->sub[i].sid[j] = NULL;
            ptr->sub[i].callback[j] = NULL;
            ptr->sub[i].sid_count--;
          }
          else
          {
            ptr->sub[i].status[j] = MQTT_SUBSCRIPTION_STATUS_PENDING;
          }
        }
      }
    }
  }
}

void
mqtt_free_topics(MQTT_SubRxCB_PTR subs_cb, int count)
{
  int i;

  for (i = 0; i < count; i++)
  {
    MQTTC_FREE(subs_cb->sub[i].topic);
  }
}

/**
 * Serializes the supplied subscribe data into the supplied buffer, ready for sending
 * @param buf the buffer into which the packet will be serialized
 * @param buflen the length in bytes of the supplied bufferr
 * @param dup integer - the MQTT dup flag
 * @param packetid integer - the MQTT packet identifier
 * @param count - number of members in the topicFilters and reqQos arrays
 * @param topicFilters - array of topic filter names
 * @param requestedQoSs - array of requested QoS
 * @return the length of the serialized data.  <= 0 indicates error
 */
int MQTTSerialize_subscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid, int count,
    MQTTString topicFilters[], int requestedQoSs[]);


int
mqtt_subscribe2(MQTT_CONN_PTR conn, uint8_t *topics[], int topics_len[],
    int topics_qos[], MQTT_CHK_SUB_RESULT topics_status[], 
    int num_topics, MQTT_SUBSCRIPTION_CB cbk, void *sid, 
    MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  int count; /* number of new (unique) subscriptions */
  int error_count; /* number of erroneous subscriptions */
  int i, j;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  MQTTString *topics_strs;
  uint16_t packet_id;
  int rc = 0;
  MQTT_TxCB_PTR tx_cb;
  MQTT_SubRxCB_PTR subs_cb;
  MQTT_SubRxCB_PTR next_ptr;
  MQTT_SubRxCB_PTR ptr;
  int sub_count = 0;

  if ((conn == NULL) ||
      (topics == NULL || (topics_len == NULL || (topics_len[0] <= 0 || topics_len[0] > QAPI_NET_MQTT_MAX_TOPIC_LEN))) ||
      (topics_qos == NULL || (topics_qos[0] != QOS_0 && topics_qos[0] != QOS_1 && topics_qos[0] != QOS_2)) ||
      num_topics < 1 || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_subscribe2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	
	if(conn != NULL)
	{
	   if(topics_qos != NULL && topics_qos[0] > QOS_2)
	   {
	   		conn->cfg.last_error = MQTT_ERR_REJECTED_SUBSCRIPTION_HIGHER_QOS;
       		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	   }
	   else
	   {
	   		conn->cfg.last_error = MQTT_ERR_SUBSCRIPTION_BAD_PARAM;
       		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	   }
	}
	
    return (EFAILURE);
  }
  for (ptr = conn->subq; ptr; ptr = next_ptr)
  {
      next_ptr = ptr->next;
	  sub_count += ptr->count;
  }
  if((sub_count + num_topics) > MAX_SUB_PERSISTED)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_subscribe2: parameter error: exceeds max topic number to subscribe");
	conn->cfg.last_error = MQTT_ERR_REJECTED_SUBSCRIPTION_TBL_FULL;
    conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_PARAM;

    return (EFAILURE);
  }

  TK_MUTEX_PEND(conn->mutex, 0);
  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_ERR_STRF("mqtt_subscribe2: reject attempt to subscribe in state %d[%s]", state, mqtt_get_conn_state_name(conn->state));
    NSINC2(mqtt_stats, subscribe_fails);
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
    conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  count = 0;
  error_count = 0;
  for (i = 0; i < num_topics; i++)
  {
    MQTT_LOG_INFO1("mqtt_subscribe2: checking subscription table for topic match", i);
#ifdef MQTT_DEBUG
    if (mqtt_log_level & MQTT_LOG_DEBUG)
    {
      MQTT_LOG_DEBUG1("mqtt_subscribe2: topic filter #%d name", i);
      gio_hexdump(NULL, topics[i], topics_len[i], TRUE, TRUE);
    }
#endif

    topics_status[i] = mqtt_chk_subscription(conn, topics[i], topics_len[i], 
            topics_qos[i], sid, cbk);

    switch (topics_status[i])
    {
      case MQTT_CREATE_NEW_SUBSCRIPTION:
        {
          ++count;
          break;
        }
      case MQTT_ADD_TO_EXISTING_SUBSCRIPTION:
        {
          /* this is not considered a new subscription, and is not an error case either */
          break;
        }
      case MQTT_REJECTED_SUBSCRIPTION_DUP_SID:
      case MQTT_REJECTED_SUBSCRIPTION_HIGHER_QOS:
      case MQTT_REJECTED_SUBSCRIPTION_REQ_OUTSTANDING:
      case MQTT_REJECTED_UTF8_VALIDATION_FAILED:   
      case MQTT_SUBSCRIPTION_BAD_PARAM:
      default: /*Default case is error scenario*/
        {
          ++error_count;
          break;
        }
    }
  }

  if (error_count > 0)
  {
    mqtt_update_subscriptions(conn, error_count);
    NSINC2(mqtt_stats, subscribe_fails);
    TK_MUTEX_POST(conn->mutex);
    *status = MQTT_ERR_PARTIAL_SUBSCRIPTION_FAILURE; /* one (or more) subscription(s) failed */
    return (EFAILURE);
  }

  topics_strs = MQTTC_ALLOC(count*sizeof(MQTTString));
  if (topics_strs == NULL)
  {
    mqtt_update_subscriptions(conn, error_count);
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_subscribe2: fail to alloc buf for topic strings");
    goto exit;
  }

  subs_cb = MQTTC_ALLOC(sizeof(struct MQTT_SubscribeRxCB) + 
      (count - 1) * sizeof(struct MQTT_subscription));
  if (subs_cb == NULL)
  {
    MQTTC_FREE(topics_strs);
    mqtt_update_subscriptions(conn, error_count);
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_subscribe2: fail to alloc buf for MQTT_SubscribeRxCB");
    goto exit;
  }

  subs_cb->id = packet_id = mqtt_rand16();
  subs_cb->count = count;

  for (i = 0, j = 0; i < num_topics; i++)
  {
    topics_strs[j].cstring = NULL;
    /* include new subscriptions in just-created subscription control block */
    if (topics_status[i] == MQTT_CREATE_NEW_SUBSCRIPTION)
    {
      uint8_t *topic;

      /* make a copy of the caller's topic filter */
      topic = MQTTC_ALLOC(topics_len[i]);
      if (topic == NULL)
      {
        MQTTC_FREE(topics_strs);
        /* free previously allocated topics */
        mqtt_free_topics(subs_cb, j);
        MQTTC_FREE(subs_cb);
        mqtt_update_subscriptions(conn, error_count);
        TK_MUTEX_POST(conn->mutex);
        *status = MQTT_ERR_ALLOC_FAILURE;
        return (EFAILURE);
      }
      memscpy(topic, topics_len[i], topics[i], topics_len[i]);
      subs_cb->sub[j].topic = topic;
      topics_strs[j].lenstring.data = (char *)topic;
      subs_cb->sub[j].length = topics_strs[j].lenstring.len = topics_len[i];
      subs_cb->sub[j].requested_qos = topics_qos[i];
      subs_cb->sub[j].granted_qos = MQTT_GRANTED_QOS_OUTSTANDING;
      subs_cb->sub[j].sid[0] = sid;
      subs_cb->sub[j].callback[0] = cbk;
      subs_cb->sub[j].status[0] = MQTT_SUBSCRIPTION_STATUS_PENDING;
      subs_cb->sub[j++].sid_count = 1;
    }
  }

  reqd_len = MQTTPacket_len(MQTTSerialize_subscribeLength(count, topics_strs));
  MQTT_LOG_DEBUG1("mqtt_subscribe2: MQTTPacket_len return %d", reqd_len);
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    MQTTC_FREE(topics_strs);
    mqtt_free_topics(subs_cb, j);
    MQTTC_FREE(subs_cb);
    mqtt_update_subscriptions(conn, error_count);
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_subscribe2: fail to alloc buf for SUBSCRIBE msg");
    goto exit;
  }

  /* dup = FALSE (always) */
  len = MQTTSerialize_subscribe(tx_buf, reqd_len, FALSE, packet_id, count, topics_strs, topics_qos);
  MQTT_LOG_DEBUG1("mqtt_subscribe2: MQTTSerialize_subscribe return %d", len);
  if (len <= 0)
  {
    MQTTC_FREE(topics_strs);
    mqtt_free_topics(subs_cb, j);
    MQTTC_FREE(subs_cb);
    mqtt_update_subscriptions(conn, error_count);
    TK_MUTEX_POST(conn->mutex);
    MQTTC_FREE(tx_buf);
    MQTT_LOG_ERR("mqtt_subscribe2: failure");
    conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    NSINC2(mqtt_stats, subscribe_fails);
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS("mqtt_subscribe2: SUBSCRIBE");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if the tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      MQTT_LOG_ERR1("mqtt_subscribe2: conn %p fatal err on sock", conn);
      conn->state = MQTT_ST_DYING;
      MQTT_LOG_INFO_STRF("mqtt_subscribe2: conn %p %s -> DYING", __FUNCTION__, conn, mqtt_get_conn_state_name(state));
      MQTTC_FREE(topics_strs);
      mqtt_free_topics(subs_cb, j);
      MQTTC_FREE(subs_cb);
      mqtt_update_subscriptions(conn, error_count);
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      NSINC2(mqtt_stats, subscribe_fails);
      NSINC2(mqtt_stats, transitioned_to_dying);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, subscribe_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required) and to complete the SUBSCRIBE/SUBACK exchange) 
   */
  tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
  if (tx_cb == NULL)
  {
    MQTTC_FREE(topics_strs);
    mqtt_free_topics(subs_cb, j);
    MQTTC_FREE(subs_cb);
    mqtt_update_subscriptions(conn, error_count);
    TK_MUTEX_POST(conn->mutex);
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_subscribe2: fail to alloc MQTT_TxCB");
    goto exit;
  }


  MQTTC_FREE(topics_strs);
  tx_cb->next   = NULL;
  tx_cb->buf = tx_buf;
  tx_cb->length = len;
  /* keep track of what has been written into the socket */
  tx_cb->offset = rc;
  tx_cb->id = packet_id;
  tx_cb->created = CTICKS;
  tx_cb->response = SUBACK;
  mqtt_add_txcb(conn, tx_cb);

  /* mark all shared subscriptions as active */
  mqtt_update_subscriptions(conn, error_count);

  /* add subscription to connection */
  subs_cb->next = conn->subq;
  conn->subq = subs_cb;

  /* update the total number of topics that have been subscribed to include 
   * those in the current request 
   */
  NSINCn(mqtt_stats, topic_subscription_count, count);

  TK_MUTEX_POST(conn->mutex);
  MQTT_LOG_INFO1("mqtt_subscribe2: queued SUBSCRIBE req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, subscribe_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

bool_t
mqtt_find_subscription(MQTT_CONN_PTR conn, uint8_t *topic, int length, void *sid, int *single_subscriber_count)
{
  MQTT_SubRxCB_PTR ptr;
  int i, j;

  if (conn == NULL || topic == NULL || length <= 0)
  {
    return (FALSE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & MQTT_LOG_DEBUG)
  {
    MQTT_LOG_DEBUG("mqtt_find_subscription: searching for topic filter name");
    gio_hexdump(NULL, topic, length, TRUE, TRUE);
  }
#endif

  /* iterate thru' list of subscriptions on connection */
  for (ptr = conn->subq; ptr; ptr = ptr->next)
  {
    /* iterate thru' all topics in a single subscription request */
    for (i = 0; i < ptr->count; i++)
    {
      /* compare length of topic */
      if (ptr->sub[i].length != length)
      {
        continue;
      }
      /* compare topic content */
      if (MEMCMP(ptr->sub[i].topic, topic, length) != 0)
      {
        continue;
      }
      /* compare subscriber id for equality */
      for (j = 0; j < ptr->sub[i].sid_count; j++)
      {
        if (ptr->sub[i].sid[j] == sid)
        {
          MQTT_LOG_DEBUG("mqtt_find_subscription: found matching "
                "subscription in subcb %p slot %d:%d (SID %p)", ptr, i, j, sid);
          if (ptr->sub[i].sid_count == 1)
          {
            ++*single_subscriber_count;
          }
          return (TRUE);
        }
      }
    }
  }

  /* did not find an existing subscription that matches */
  MQTT_LOG_DEBUG("mqtt_find_subscription: could not find matching "
        "subscription");
  return (FALSE);
}

void
mqtt_del_subscription(MQTT_CONN_PTR conn, uint8_t *topic, int length, void *sid)
{
  MQTT_SubRxCB_PTR ptr;
  int i, j;

  if (conn == NULL || topic == NULL || length <= 0)
  {
    return;
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & MQTT_LOG_DEBUG)
  {
    MQTT_LOG_DEBUG("mqtt_del_subscription: searching for "
          "topic filter name");
    gio_hexdump(NULL, topic, length, TRUE, TRUE);
  }
#endif

  /* iterate thru' list of subscriptions on connection */
  for (ptr = conn->subq; ptr; ptr = ptr->next)
  {
    /* iterate thru' all topics in a single subscription request */
    for (i = 0; i < ptr->count; i++)
    {
      /* compare length of topic */
      if (ptr->sub[i].length != length)
      {
        continue;
      }
      /* compare topic content */
      if (MEMCMP(ptr->sub[i].topic, topic, length) != 0)
      {
        continue;
      }
      /* compare subscriber id for equality */
      for (j = 0; j < ptr->sub[i].sid_count; j++)
      {
        if (ptr->sub[i].sid[j] == sid)
        {
          MQTT_LOG_DEBUG2("mqtt_del_subscription: found matching "
                "subscription (SID %p)", i, sid);
          ptr->sub[i].sid[j] = NULL;
          ptr->sub[i].callback[j] = NULL;
          ptr->sub[i].status[j] = MQTT_SUBSCRIPTION_STATUS_NONEXISTENT;
          ptr->sub[i].sid_count--;
          return;
        }
      }
    }
  }

  /* did not find an existing subscription that matches */
  conn->cfg.last_error = MQTT_SUBSCRIPTION_STATUS_NONEXISTENT;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  MQTT_LOG_ERR("mqtt_del_subscription: failure");
}

/**
 * Serializes the supplied unsubscribe data into the supplied buffer, ready for sending
 * @param buf the raw buffer data, of the correct length determined by the remaining length field
 * @param buflen the length in bytes of the data in the supplied buffer
 * @param dup integer - the MQTT dup flag
 * @param packetid integer - the MQTT packet identifier
 * @param count - number of members in the topicFilters array
 * @param topicFilters - array of topic filter names
 * @return the length of the serialized data.  <= 0 indicates error
 */
int MQTTSerialize_unsubscribe(unsigned char* buf, int buflen, unsigned char dup, unsigned short packetid,
    int count, MQTTString topicFilters[]);

int
mqtt_unsubscribe2(MQTT_CONN_PTR conn, uint8_t *topics[], int topics_len[], 
    int count, void *sid, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  int i;
  int single_subscriber_count;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  MQTTString *topics_strs;
  uint16_t packet_id;
  int rc = 0;
  MQTT_TxCB *tx_cb;
  MQTT_SubRxCB_PTR ptr;
  MQTT_SubRxCB_PTR next_ptr;
  MQTT_SubRxCB_PTR prev;
  int total_count;

  if (conn == NULL ||
      (topics == NULL || (topics_len == NULL || (topics_len[0] <= 0 || topics_len[0] > QAPI_NET_MQTT_MAX_TOPIC_LEN))) ||
      count < 1 || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_unsubscribe2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	if (conn != NULL)
	{
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
      conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  TK_MUTEX_PEND(conn->mutex, 0);
  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    TK_MUTEX_POST(conn->mutex);
    ds_iot_log_strf_err("mqtt_unsubscribe2: reject to unsubscribe in state %d[%s]", state, mqtt_get_conn_state_name(conn->state));
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    NSINC2(mqtt_stats, unsubscribe_fails);
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  /* check to ensure that all topics being unsubscribed from currently exist 
   * in the subscription table 
   */
  single_subscriber_count = 0;
  for (i = 0; i < count; i++)
  {
    if (mqtt_find_subscription(conn, topics[i], topics_len[i], sid, 
          &single_subscriber_count) == FALSE)
    {
      TK_MUTEX_POST(conn->mutex);
      MQTT_LOG_ERR1("mqtt_unsubscribe2: not find subscription "
            "for topic filter #%d; reject to unsubscribe", i);
	  
	  conn->cfg.last_error = MQTT_ERR_SUBSCRIBE_UNKNOWN_TOPIC;
      conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
      NSINC2(mqtt_stats, unsubscribe_fails);
      *status = MQTT_ERR_SUBSCRIBE_UNKNOWN_TOPIC;
      return (EFAILURE);
    }
  }

  /* update subscription table */
  for (i = 0; i < count; i++)
  {
    mqtt_del_subscription(conn, topics[i], topics_len[i], sid);
  }

  /* only unsubscribe for topics for which we are the only interested entity; 
   * other subscriptions will remain in place
   */
  if (single_subscriber_count == 0)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_INFO("mqtt_unsubscribe2: all subscriptions were shared; "
          "an UNSUBSCRIBE no need to send");
    return (ESUCCESS);
  }

  topics_strs = MQTTC_ALLOC(count*sizeof(MQTTString));
  if (topics_strs == NULL)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_unsubscribe2: fail to alloc buf for "
          "topic strings");
    goto exit;
  }

  for (i = 0; i < count; i++)
  {
    topics_strs[i].cstring = NULL;
    topics_strs[i].lenstring.data = (char *)topics[i];
    topics_strs[i].lenstring.len = topics_len[i];
  }

  reqd_len = MQTTPacket_len(MQTTSerialize_unsubscribeLength(count, topics_strs));
  MQTT_LOG_DEBUG1("mqtt_unsubscribe2: MQTTPacket_len return %d", reqd_len);
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    MQTTC_FREE(topics_strs);
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_unsubscribe2: fail to alloc buf for "
          "UNSUBSCRIBE msg");
    goto exit;
  }

  packet_id = mqtt_rand16();
  /* dup = FALSE (always) */
  len = MQTTSerialize_unsubscribe(tx_buf, reqd_len, FALSE, packet_id, count, 
      topics_strs);
  MQTT_LOG_DEBUG1("mqtt_unsubscribe2: MQTTSerialize_unsubscribe "
        "return %d", len);
  if (len <= 0)
  {
    MQTTC_FREE(topics_strs);
    TK_MUTEX_POST(conn->mutex);
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_unsubscribe2: fail to create UNSUBSCRIBE");
    NSINC2(mqtt_stats, unsubscribe_fails);
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS ("mqtt_unsubscribe2: UNSUBSCRIBE");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if there is tx queue is empty */
  // sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      MQTTC_FREE(topics_strs);
      MQTT_LOG_ERR1("mqtt_unsubscribe2: conn %p fatal error on "
            "sock", conn);
      conn->state = MQTT_ST_DYING;
      MQTT_LOG_INFO_STRF("mqtt_unsubscribe2: conn %p %s -> DYING", conn, mqtt_get_conn_state_name(state));
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      NSINC2(mqtt_stats, unsubscribe_fails);
      NSINC2(mqtt_stats, transitioned_to_dying);
	  *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, unsubscribe_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required) and to complete the UNSUBSCRIBE/UNSUBACK exchange) 
   */
  tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
  if (tx_cb == NULL)
  {
    MQTTC_FREE(topics_strs);
    TK_MUTEX_POST(conn->mutex);
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_unsubscribe2: fail to alloc MQTT_TxCB");
    goto exit;
  }
  tx_cb->next   = NULL;
  tx_cb->buf = tx_buf;
  tx_cb->length = len;
  /* keep track of what has been written into the socket */
  tx_cb->offset = rc;
  tx_cb->id = packet_id;
  tx_cb->created = CTICKS;
  tx_cb->response = UNSUBACK;
  mqtt_add_txcb(conn, tx_cb);

  /* XXX should we do this after we receive an UNSUBACK from the server? */
  /* iterate thru' list of subscriptions on connection to determine if there 
   * are any subscription control blocks with zero subscribers 
   */
  prev = NULL;
  for (ptr = conn->subq; ptr; ptr = next_ptr)
  {
    next_ptr = ptr->next; 
    for (i = 0, total_count = 0; i < ptr->count; i++)
    {
      total_count += ptr->sub[i].sid_count;
      if (ptr->sub[i].sid_count == 0)
      {
        MQTTC_FREE(ptr->sub[i].topic);
        MEMSET(&ptr->sub[i], 0, sizeof(struct MQTT_subscription));
      }
    }
    if (total_count == 0)
    {
      if (prev != NULL)
      {
        prev->next = ptr->next;
      }
      else
      {
        conn->subq = ptr->next;
      }
      MQTTC_FREE(ptr);
    }
    else
    {
      prev = ptr;
    }
  }

  MQTTC_FREE(topics_strs);

  TK_MUTEX_POST(conn->mutex);
  MQTT_LOG_INFO1("mqtt_unsubscribe2: queued UNSUBSCRIBE req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, unsubscribe_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

/**
 * Serializes a disconnect packet into the supplied buffer, ready for writing to a socket
 * @param buf the buffer into which the packet will be serialized
 * @param buflen the length in bytes of the supplied buffer, to avoid overruns
 * @return serialized length, or error if 0
 */
int MQTTSerialize_pingreq(unsigned char* buf, int buflen);

int
mqtt_ping2(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  //  SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  int rc = 0;
  MQTT_TxCB *tx_cb;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_ping2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }

	if (conn != NULL)
	{	
	  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  TK_MUTEX_PEND(conn->mutex, 0);
  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG_STRF("mqtt_ping2: reject attempt to ping in state "
          "%d (%s)", state, mqtt_get_conn_state_name(conn->state));
    NSINC2(mqtt_stats, pingreq_fails);
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  reqd_len = MQTT_HDR_SZ;
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTT_LOG_DEBUG("mqtt_ping2: fail to alloc buf for PINGREQ");
    goto exit;
  }

  len = MQTTSerialize_pingreq(tx_buf, reqd_len);
  MQTT_LOG_DEBUG1("mqtt_ping2: MQTTSerialize_pingreq return %d", len);
  if (len <= 0)
  {
    TK_MUTEX_POST(conn->mutex);
    MQTTC_FREE(tx_buf);
    MQTT_LOG_ERR("mqtt_ping2: failure" );
    NSINC2(mqtt_stats, pingreq_fails);
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS ("mqtt_ping2: PINGREQ");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if there is tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      MQTT_LOG_ERR1("mqtt_ping2: conn %p fatal err on sock", conn);
      NSINC2(mqtt_stats, pingreq_fails);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, pingreq_out);
    }
  }

  /*Comment:Not required to add all ping req in TX_CB,
   * so adding only 1 and will be deleted once we get pingresp. 
   * If application registers a callback then it triggers once ping resp is received.
   * pingresp_pending only will be incremented for further ping requests sent to network */
  if(conn->pingresp_pending == 0){
    /* save the state of the transmission request (to complete transmission (if 
     * required) and to complete the PINGREQ/PINGRESP exchange) 
     */
    tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
    if (tx_cb == NULL)
    {
      TK_MUTEX_POST(conn->mutex);
      MQTTC_FREE(tx_buf);
      MQTT_LOG_DEBUG("mqtt_ping2: fail to alloc MQTT_TxCB");
      goto exit;
    }
    tx_cb->next   = NULL;
    tx_cb->buf = tx_buf;
    tx_cb->length = len;
    /* keep track of what has been written into the socket */
    tx_cb->offset = rc;
    tx_cb->created = CTICKS;
    tx_cb->response = PINGRESP;
    mqtt_add_txcb(conn, tx_cb);
  } else {
    MQTTC_FREE(tx_buf);
  }
  conn->pingresp_pending++;
  TK_MUTEX_POST(conn->mutex);
  MQTT_LOG_INFO1("mqtt_ping2: queued PINGREQ req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, pingreq_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

/* returns 
 * -1 (EFAILURE) upon error, or
 *  0 if all of type, flags, and remaining length have not been received (max 5 bytes), or
 *  remaining length
 */
int
mqtt_read_msg1(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status)
{
  //SOCKTYPE sock;
  uint8_t *rxbuf;
  int rc;
  int offset;
  int i;
  bool_t more;
  int remaining_length;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_read_msg1: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	if(conn != NULL)
	{
		conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  //sock = conn->sock;
  rxbuf = conn->rx.buf;
  offset = conn->rx.offset;

  for (;;)
  {

    rc = mqtt_rd_sock(conn, &rxbuf[offset], 1, 0, status);
    if (rc == 0) 
    {

      if (*status == MQTT_ERR_CONN_CLOSED)
      {
        MQTT_LOG_INFO1("mqtt_read_msg1: conn %p sock closed", conn);
		conn->cfg.last_error = MQTT_ERR_CONN_CLOSED;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
        return (EFAILURE);
      }
      else if (*status == MQTT_ERR_NO_DATA)
      {
        MQTT_LOG_INFO1("mqtt_read_msg1: conn %p no data", conn);
        *status = offset == conn->rx.offset? MQTT_ERR_NO_DATA : 
          MQTT_ERR_RX_INCOMPLETE;
        conn->rx.offset = offset;
		conn->cfg.last_error = *status;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
        return (0);
      }
    }
    else if (rc == EFAILURE)
    {
      MQTT_LOG_ERR1("mqtt_read_msg1: conn %p fatal err on sock", conn);
      return (rc);
    }
    else
    {
      int multiplier;

#ifdef MQTT_DEBUG
      if (offset == 0)
      {
        if (mqtt_log_level & DUMP_LOG_MSGS)
        {
          uint8_t msg_type, msg_flags;

          msg_type = MQTT_MSG_TYPE(rxbuf);
          msg_flags = MQTT_MSG_FLAG(rxbuf);
          MQTT_LOG_MSGS2("mqtt_read_msg1: msg type %u flags %u", msg_type, msg_flags);
        }
      }
      else
#endif
        if (offset >= 1)
        {
          more = rxbuf[offset] & 0x80;
          if (more)
          {
              if (offset == MQTT_FIXED_HDR_MAX_LEN - 1)
              {
                  *status = MQTT_ERR_BAD_PARAM;
                  MQTT_LOG_INFO("mqtt_read_msg1: 'more' bit of last byte of 'Remaining length' is on");
				  
				  conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
				  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
                  return (EFAILURE);
              }

              /* clear the 'more' bit */
              rxbuf[offset] &= 0x7f;
          }
          else
          {
            /* compute the total length of the message
             *
             * total_length = msg_len[0] + msg_len[1] * 128 + 
             *                msg_len[2] * 128^2 + msg_len[3] * 128^3;
             */
            for (i = 1, remaining_length = 0, multiplier = 1; i <= offset; i++)
            {
              remaining_length += rxbuf[i] * multiplier;
              multiplier *= 128;
              /* restore Remaining Length field to its original value */
              if (i < offset)
              {
                rxbuf[i] |= 0x80;
              }
            }
            MQTT_LOG_INFO3("mqtt_read_msg1: offset %d "
                  "remaining_length %d; total_length of rcvd "
                  "msg: %d", offset, remaining_length, 
                  1 + offset + remaining_length);

            /* update Remaining Length-related statistics */
            if (remaining_length <= MQTT_MAX_REMLEN1)
            {
              NSINC2(mqtt_stats, rx_rem_length1);
            }
            else if (remaining_length <= MQTT_MAX_REMLEN2)
            {
              NSINC2(mqtt_stats, rx_rem_length2);
            }
            else if (remaining_length <= MQTT_MAX_REMLEN3)
            {
              NSINC2(mqtt_stats, rx_rem_length3);
            }
            else if (remaining_length <= MQTT_MAX_REMLEN4)
            {
              NSINC2(mqtt_stats, rx_rem_length4);
            }

            if (remaining_length == 0)
            {
              conn->rx.state = MQTT_RX_ST_MSG_COMPLETE;
            }
            else
            {
              conn->rx.state = MQTT_RX_ST_WAITING_FOR_MSG_PART2;
            }
            conn->rx.offset = offset + 1;
            conn->rx.vhdr = conn->rx.buf + conn->rx.offset;
            conn->rx.rem_len = remaining_length;
            *status = MQTT_ERR_NO_ERR;
            return (remaining_length);
          }
        }
      offset += rc;
    }
  }
}

int
mqtt_read_msg2(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status)
{
  //SOCKTYPE sock;
  uint8_t *rxbuf;
  int offset;
  int rc;
  int len;
  int rem_len;
  int rd_len;
  int space;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_read_msg2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	
	if(conn != NULL)
	{
		conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  //sock = conn->sock;
  rxbuf = conn->rx.buf;
  len = conn->rx.length;
  offset = conn->rx.offset;
  rem_len = conn->rx.rem_len;
  space = len - offset;
  rd_len = MIN(rem_len, space);

  rc = mqtt_rd_sock(conn, &rxbuf[offset], rd_len, 0, status);
  if (rc == 0) 
  {
    if (*status == MQTT_ERR_CONN_CLOSED)
    {
      MQTT_LOG_INFO1("mqtt_read_msg2: conn %p sock closed", conn);
	  conn->cfg.last_error = MQTT_ERR_CONN_CLOSED;
  	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
      return (EFAILURE);
    }
    else if (*status == MQTT_ERR_NO_DATA)
    {
      MQTT_LOG_INFO1("mqtt_read_msg2: conn %p no data", conn);
      *status = offset == conn->rx.offset? MQTT_ERR_NO_DATA : 
        MQTT_ERR_RX_INCOMPLETE;
	  conn->cfg.last_error = *status;
  	  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
      conn->rx.offset = offset;
      return (0);
    }/*Fixed*/
    else
    {
      return (ESUCCESS);
    }
  }
  else if (rc == EFAILURE)
  {
    MQTT_LOG_ERR("mqtt_read_msg2: conn %p fatal err on sock");
    return (rc);
  }
  else
  {
    offset += rc;
    conn->rx.offset = offset;
    conn->rx.rem_len -= rc;
    if (conn->rx.rem_len == 0)
    {
      conn->rx.state = MQTT_RX_ST_MSG_COMPLETE;
      *status = MQTT_ERR_NO_ERR;
      return (ESUCCESS);
    }
    *status = MQTT_ERR_RX_INCOMPLETE;
	conn->cfg.last_error = MQTT_ERR_RX_INCOMPLETE;
  	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    /* receiver state stays unchanged */
    return (ESUCCESS);
  }
}

int
mqtt_process_rxmsg_pingresp(MQTT_CONN_PTR conn)
{
  int rc;

  if (conn == NULL)
  {
    return (EFAILURE);
  }

  MQTT_LOG_INFO("mqtt_process_rxmsg_pingresp: found matching entry, deleting...");

  rc = mqtt_process_tx_cb_without_pkt_id(conn, PINGRESP);       
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  if(rc == EFAILURE) 
  {
    /* cleanup of receiver state, etc. in the event of a failure */
    NSINC2(mqtt_stats, badpingresp_in);
    return EFAILURE;
  }
  conn->pingresp_pending = 0;

  return (ESUCCESS);
}

/* 'filter' (topic filter) may contain wild cards; 'name' (topic name) 
 * does not contain wild cards 
 */
static bool_t
mqtt_topic_match(uint8_t *filter, int flen, uint8_t *name, int nlen)
{
  int foff;
  int noff;

  if (filter == NULL || flen <= 0 || name == NULL || nlen <= 0)
  {
    return (FALSE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & MQTT_LOG_DEBUG)
  {
    MQTT_LOG_DEBUG("mqtt_topic_match: Topic filter:");
    gio_hexdump(NULL, filter, flen, TRUE, TRUE);

    MQTT_LOG_DEBUG("mqtt_topic_match: Topic name:\n");
    gio_hexdump(NULL, name, nlen, TRUE, TRUE);
  }
#endif

  for (foff = 0, noff = 0; foff < flen && noff < nlen; foff++, noff++) 
  {
    switch (filter[foff])
    {
      case '/':
        {
          if (name[noff] != '/')
          {
            MQTT_LOG_DEBUG("mqtt_topic_match: [S1] mismatch");
            return (FALSE);
          }
          break;
        }
      case '+':
        {
          do
          {
            if (name[noff] != '/')
            {
              noff++;
            }
            else
            {
              noff--;
              break;
            }
          } while (noff < nlen);
          if ((foff == flen - 1) && (noff == nlen))
          {
            MQTT_LOG_DEBUG("mqtt_topic_match: [P1] match");
            return (TRUE);
          }
          break;
        }
      case '#':
        {
          if (foff == flen - 1)
          {
            MQTT_LOG_DEBUG("mqtt_topic_match: [H1] match");
            return (TRUE);
          }
          else
          {
            /* syntax error in filter specification */
            MQTT_LOG_DEBUG("mqtt_topic_match: [H2] mismatch");
            return (FALSE);
          }
          //break;
        }
      default:
        {
          if (name[noff] != filter[foff])
          {
            MQTT_LOG_DEBUG("mqtt_topic_match: [D1] mismatch");
            return (FALSE);
          }
          break;
        }
    }
  }

  if (noff == nlen)
  {
    if ((foff == flen - 1) && (filter[foff] == '+' || filter[foff] == '#'))
    {
      MQTT_LOG_DEBUG("mqtt_topic_match: [E1] match");
      return (TRUE);
    }
    else if (foff == flen)
    {
      MQTT_LOG_DEBUG("mqtt_topic_match: [E2] match");
      return (TRUE);
    }
  }

  MQTT_LOG_DEBUG("mqtt_topic_match: [E3] mismatch");
  return (FALSE);
}

/* returns the number of subscribers to whom the message was delivered */
int
mqtt_deliver_published_msg(MQTT_CONN_PTR conn, uint8_t *topic, int topic_length, 
    uint8_t *msg, int msg_length, int qos)
{
  MQTT_SubRxCB_PTR sub_cb;
  int i, j;
  int subscriber_count = 0;
  bool_t allow_unsub_message;
  bool_t topic_is_subscribed = false;
  
  if (conn == NULL || topic == NULL || topic_length <= 0  ||
      (msg_length < 0 || (msg_length > 0 && msg == NULL)) ||
      (qos != QOS_0 && qos != QOS_1 && qos != QOS_2))
  {
    MQTT_LOG_DEBUG("mqtt_deliver_published_msg: parameter error");
	if(conn != NULL)
	{
		conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (0);
  }

  allow_unsub_message = conn->cfg.allow_unsub_pub;
  for (sub_cb = conn->subq; sub_cb; sub_cb = sub_cb->next)
  {
    int count;

    count = sub_cb->count;
    for (i = 0; i < count; i++)
    {
      /* skip failed subscriptions */
      if (sub_cb->sub[i].granted_qos == 0x80)
        continue;
      if (sub_cb->sub[i].topic != NULL && 
          mqtt_topic_match(sub_cb->sub[i].topic, sub_cb->sub[i].length, 
            topic, topic_length) == TRUE)
      {
        for (j = 0; j < sub_cb->sub[i].sid_count; j++)
        {
          if (sub_cb->sub[i].status[j] == MQTT_SUBSCRIPTION_STATUS_ACCEPTED &&
              sub_cb->sub[i].callback[j] != NULL)
          {
            sub_cb->sub[i].callback[j](conn, MQTT_SUBSCRIPTION_MSG, topic, 
                topic_length, msg, msg_length, qos, 
                sub_cb->sub[i].sid[j]);
            ++subscriber_count;
            topic_is_subscribed = true;
          }
        }
      }
    }
  }
   if(allow_unsub_message && (topic_is_subscribed == false))
  {
    sub_cb = conn->subq;
    if(sub_cb == NULL)
    {
      MQTT_LOG_INFO("mqtt_deliver_published_msg: Client not subscribed any topic ");
    }
    else if(sub_cb->sub[0].callback[0] != NULL)
    {
      sub_cb->sub[0].callback[0](conn, MQTT_SUBSCRIPTION_MSG, topic, 
                topic_length, msg, msg_length, qos, 
                sub_cb->sub[0].sid[0]);
            ++subscriber_count;
    }
  }

  MQTT_LOG_INFO2("mqtt_deliver_published_msg: delivered msg to %d recipients", subscriber_count);
  if (subscriber_count == 0)
  {
    NSINC2(mqtt_stats, zero_recipient_count);
  }
  return (subscriber_count);
}

int
mqtt_puback2(MQTT_CONN_PTR conn, uint16_t pkt_id, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  int rc = 0;
  MQTT_TxCB *tx_cb;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_puback2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	if(conn != NULL)
	{
		conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    ds_iot_log_strf_err("mqtt_puback2: reject attempt to send PUBACK in state %d[%s]", state, mqtt_get_conn_state_name(conn->state));
    NSINC2(mqtt_stats, puback_fails);
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
  	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  reqd_len = MQTT_PUB_HDR_SZ;
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_puback2: fail to alloc buf for PUBACK");
    goto exit;
  }

  len = MQTTSerialize_puback(tx_buf, reqd_len, pkt_id);
  MQTT_LOG_DEBUG1("mqtt_puback2: MQTTSerialize_puback return %d", len);
  if (len <= 0)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_puback2: fail to create PUBACK msg");
    NSINC2(mqtt_stats, puback_fails);
	conn->cfg.last_error = MQTT_ERR_PUBACK_MSG_CREATION_FAILED;
  	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_PUBACK_MSG_CREATION_FAILED;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS("mqtt_puback2: PUBACK");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if there is tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      MQTT_LOG_ERR1("mqtt_puback2: conn %p fatal err on socK", conn);
      conn->state = MQTT_ST_DYING;

      MQTT_LOG_INFO_STRF("mqtt_puback2: conn %p %s -> DYING", conn, mqtt_get_conn_state_name(state));
      MQTTC_FREE(tx_buf);
      NSINC2(mqtt_stats, puback_fails);
      NSINC2(mqtt_stats, transitioned_to_dying);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, puback_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required)
   */
  if (rc < len)
  {
    tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
    if (tx_cb == NULL)
    {
      MQTTC_FREE(tx_buf);
      MQTT_LOG_DEBUG("mqtt_puback2: fail to alloc MQTT_TxCB");
      goto exit;
    }
    tx_cb->next   = NULL;
    tx_cb->buf = tx_buf;
    tx_cb->length = len;
    /* keep track of what has been written into the socket */
    tx_cb->offset = rc;
    tx_cb->id = pkt_id;
    tx_cb->created = CTICKS;
    tx_cb->qos = QOS_1;
    tx_cb->response = MQTT_NO_RESPONSE_MSG_REQD;
    mqtt_add_txcb(conn, tx_cb);
  }
  else
  {
    MQTTC_FREE(tx_buf);
  }
  MQTT_LOG_INFO1("mqtt_puback2: queued PUBACK req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, puback_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;
  return (EFAILURE);
}

int
mqtt_pubrec2(MQTT_CONN_PTR conn, uint16_t pkt_id, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  int rc = 0;
  MQTT_TxCB *tx_cb;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_pubrec2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	if(conn != NULL)
	{
		conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    ds_iot_log_strf_err("mqtt_pubrec2: reject attempt to send PUBREC in state %d[%s]", state, mqtt_get_conn_state_name(conn->state));
    NSINC2(mqtt_stats, pubrec_fails);
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
  	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  reqd_len = MQTT_PUB_HDR_SZ;
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_pubrec2: fail to alloc buf for PUBREC");
    goto exit;
  }

  len = MQTTSerialize_pubrec(tx_buf, reqd_len, pkt_id);
  MQTT_LOG_DEBUG1("mqtt_pubrec2: MQTTSerialize_pubrec return %d", len);
  if (len <= 0)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_pubrec2: fail to create PUBREC");
    NSINC2(mqtt_stats, pubrec_fails);
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
  	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS("mqtt_pubrec2: PUBREC");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if there is tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      MQTT_LOG_ERR1("mqtt_pubrec2: conn %p fatal err on "
            "sock", conn);
      conn->state = MQTT_ST_DYING;
      MQTT_LOG_INFO_STRF("%s: conn %p %s -> DYING", conn, mqtt_get_conn_state_name(state));
      MQTTC_FREE(tx_buf);
      NSINC2(mqtt_stats, pubrec_fails);
      NSINC2(mqtt_stats, transitioned_to_dying);
      *status = MQTT_ERR_SOCKET_FATAL_ERROR;
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, pubrec_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required) and to complete the PUBREC/PUBREL exchange
   */
  tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
  if (tx_cb == NULL)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_pubrec2: fail to alloc MQTT_TxCB");
    goto exit;
  }
  tx_cb->next   = NULL;
  tx_cb->buf = tx_buf;
  tx_cb->length = len;
  /* keep track of what has been written into the socket */
  tx_cb->offset = rc;
  tx_cb->id = pkt_id;
  tx_cb->created = CTICKS;
  tx_cb->qos = QOS_2;
  tx_cb->response = PUBREL;
  mqtt_add_txcb(conn, tx_cb);

  MQTT_LOG_INFO1("mqtt_pubrec2: queued PUBREC req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, pubrec_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

int
mqtt_pubcomp2(MQTT_CONN_PTR conn, uint16_t pkt_id, MQTT_ERR_TYPE *status)
{
  MQTT_CONN_STATE state;
  //SOCKTYPE sock;
  uint8_t *tx_buf;
  int reqd_len;
  int len;
  int rc = 0;
  MQTT_TxCB *tx_cb;

  if (conn == NULL || status == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_DEBUG("mqtt_pubcomp2: parameter error");
    if (status != NULL)
    {
      *status = MQTT_ERR_BAD_PARAM;
    }
	if(conn != NULL)
	{
		conn->cfg.last_error = MQTT_ERR_BAD_PARAM;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
	}
    return (EFAILURE);
  }

  state = conn->state;
  if (state != MQTT_ST_MQTT_CONNECTED)
  {
    ds_iot_log_strf_err("mqtt_pubcomp2: reject attempt to send PUBCOMP in state %d[%s]", state, mqtt_get_conn_state_name(conn->state));
    NSINC2(mqtt_stats, pubcomp_fails);
	conn->cfg.last_error = MQTT_ERR_BAD_STATE;
  		conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_BAD_STATE;
    return (EFAILURE);
  }

  reqd_len = MQTT_PUB_HDR_SZ;
  tx_buf = MQTTC_ALLOC(reqd_len);
  if (tx_buf == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_pubcomp2: fail to alloc buf for PUBCOMP");
    goto exit;
  }

  len = MQTTSerialize_pubcomp(tx_buf, reqd_len, pkt_id);
  MQTT_LOG_DEBUG1("mqtt_pubcomp2: MQTTSerialize_pubcomp return %d", len);
  if (len <= 0)
  {
    MQTTC_FREE(tx_buf);
    MQTT_LOG_DEBUG("mqtt_pubcomp2: fail to create PUBCOMP");
    NSINC2(mqtt_stats, pubcomp_fails);
	conn->cfg.last_error = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
	conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
    *status = MQTT_ERR_MSG_SERIALIZATION_FAILURE;
    return (EFAILURE);
  }

#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS ("mqtt_pubcomp2: PUBCOMP");
    gio_hexdump(NULL, tx_buf, len, TRUE, TRUE);
  }
#endif

  /* attempt to send the data now if there is tx queue is empty */
  //sock = conn->sock;
  if (mqtt_chk_wr_pending(conn) == FALSE)
  {
    rc = mqtt_wr_sock(conn, tx_buf, len, 0, status); 
    if (rc == EFAILURE)
    {
      MQTT_LOG_ERR1("mqtt_pubcomp2: conn %p fatal err on sock", conn);
      conn->state = MQTT_ST_DYING;

      MQTT_LOG_INFO_STRF("mqtt_pubcomp2: conn %p %s -> DYING", conn, mqtt_get_conn_state_name(state));
      MQTTC_FREE(tx_buf);
      NSINC2(mqtt_stats, pubcomp_fails);
      NSINC2(mqtt_stats, transitioned_to_dying);
      return (rc);
    }
    else if (rc == len)
    {
      NSINC2(mqtt_stats, pubcomp_out);
    }
  }

  /* save the state of the transmission request (to complete transmission (if 
   * required))
   */
  if (rc < len)
  {
    tx_cb = MQTTC_ALLOC(sizeof(MQTT_TxCB));
    if (tx_cb == NULL)
    {
      MQTTC_FREE(tx_buf);
      MQTT_LOG_DEBUG("mqtt_pubcomp2: fail to alloc MQTT_TxCB");
      goto exit;
    }
    tx_cb->next   = NULL;
    tx_cb->buf = tx_buf;
    tx_cb->length = len;
    /* keep track of what has been written into the socket */
    tx_cb->offset = rc;
    tx_cb->id = pkt_id;
    tx_cb->created = CTICKS;
    tx_cb->qos = QOS_2;
    tx_cb->response = MQTT_NO_RESPONSE_MSG_REQD;
    mqtt_add_txcb(conn, tx_cb);
  }
  else
  {
    MQTTC_FREE(tx_buf);
  }

  MQTT_LOG_INFO1("mqtt_pubcomp2: queued PUBCOMP req (%d bytes) for transmission", len);
  *status = MQTT_ERR_NO_ERR;
  return (ESUCCESS);
exit:
  NSINC2(mqtt_stats, alloc_failures);
  NSINC2(mqtt_stats, pubcomp_fails);
  *status = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.last_error = MQTT_ERR_ALLOC_FAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (EFAILURE);
}

int
mqtt_process_rxmsg_connack(MQTT_CONN_PTR conn)
{
  uint8_t session_present;
  uint8_t connack_rc;
  int rc;
  uint8_t connack_rc_client = MQTT_MQTT_CONNECT_FAILED;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_connack: parameter error");
    return (EFAILURE);
  }

  rc = mqtt_process_tx_cb_without_pkt_id(conn, CONNACK); 
  if(rc == EFAILURE) 
  {
    /* cleanup of receiver state, etc. in the event of a failure */
    goto exit;
  }

  /**
   * Deserializes the supplied (wire) buffer into connack data - return code
   * @param sessionPresent the session present flag returned (only for MQTT 3.1.1)
   * @param connack_rc returned integer value of the connack return code
   * @param buf the raw buffer data, of the correct length determined by the remaining length field
   * @param len the length in bytes of the data in the supplied buffer
   * @return error code.  1 is success, 0 is failure
   */
  rc = MQTTDeserialize_connack(&session_present, &connack_rc, conn->rx.buf, 
      conn->rx.offset);
  MQTT_LOG_DEBUG1("mqtt_process_rxmsg_connack: MQTTDeserialize_connack return %d", rc);
  if (rc == 0)
  {
    goto exit;
  }

  MQTT_LOG_MSGS2("mqtt_process_rxmsg_connack: session present %u "
        "connack_rc %u", session_present, connack_rc);

  /* return back to caller; use information from server's response to 
   * determine return value
   */
  if (connack_rc != 0)
  {
    if(connack_rc == 1)
       connack_rc_client = MQTT_CONNECT_PROTOCOL_VER_ERROR;
    else if(connack_rc == 2)
       connack_rc_client = MQTT_CONNECT_IDENTIFIER_ERROR;
    else if(connack_rc == 3)
       connack_rc_client = MQTT_CONNECT_SERVER_UNAVAILABLE;
    else if(connack_rc == 4)
       connack_rc_client = MQTT_CONNECT_MALFORMED_DATA;
    else if(connack_rc == 5)
       connack_rc_client = MQTT_CONNECT_UNAUTHORIZED_CLIENT;

    goto exit;
  }

  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));

  conn->state = MQTT_ST_MQTT_CONNECTED;
  conn->flags |= MQTT_FL_SESS_ESTAB;
#ifdef NET_STATS
  mqtt_incr_sess_estab_count(conn);
#endif
  if (conn->cfg.clean_session == FALSE && conn->restore != NULL) 
  {
    if(session_present)
    {
      MQTT_SubRxCB_PTR sub_cb = NULL;
	  MQTT_SubRxCB_PTR next_sub = NULL;
	  int i = 0;
	  int j = 0;
	  int granted;
    if(mqtt_conn_restore_state(conn) != ESUCCESS) 
    {
      //mqtt_conn_free_saved_state(conn->restore);
      return (EFAILURE);
    }
	  for(sub_cb = conn->subq; sub_cb; sub_cb = next_sub)
	  {
	    next_sub = sub_cb->next;
		for(i = 0; i < sub_cb->count; i++)
		{
		  for (j = 0; j < sub_cb->sub[i].sid_count; j++)
          {
            if (sub_cb->sub[i].callback[j] != NULL)
            {
              granted = (sub_cb->sub[i].status[j] == MQTT_SUBSCRIPTION_STATUS_ACCEPTED)?MQTT_SUBSCRIPTION_GRANTED: MQTT_SUBSCRIPTION_DENIED;
              sub_cb->sub[i].callback[j](conn, granted, sub_cb->sub[i].topic,
              sub_cb->sub[i].length, NULL, 0,
              sub_cb->sub[i].granted_qos,
              sub_cb->sub[i].sid[j]);
              if(granted == MQTT_SUBSCRIPTION_DENIED)
              {
                sub_cb->sub[i].requested_qos = MQTT_PERSISTED_QOS_OUTSTANDING;
                sub_cb->sub[i].granted_qos = MQTT_PERSISTED_QOS_OUTSTANDING;
              }
            }
          }  	
		}
	  }
	  if (conn->callback != NULL)
      {
        conn->callback(conn, MQTT_MQTT_CONNECT_SUCCEEDED);
      }
    }
	else
	{
	  MQTT_SubRxCB_PTR sub_cb = NULL;
	  MQTT_SubRxCB_PTR next_sub = NULL;
	  int i = 0;
	  int j = 0;
	  int granted;
	  
	  for(sub_cb = conn->restore->subq; sub_cb; sub_cb = next_sub)
	  {
	    next_sub = sub_cb->next;
		for(i = 0; i < sub_cb->count; i++)
		{
		  for (j = 0; j < sub_cb->sub[i].sid_count; j++)
          {
            if (sub_cb->sub[i].callback[j] != NULL)
            {
              granted = MQTT_SUBSCRIPTION_DENIED;
              sub_cb->sub[i].callback[j](conn, granted, sub_cb->sub[i].topic,
              sub_cb->sub[i].length, NULL, 0,
              sub_cb->sub[i].granted_qos,
              sub_cb->sub[i].sid[j]);
        
            }
          }  	
		}
	  }
	  if (conn->callback != NULL)
      {
        conn->callback(conn, MQTT_MQTT_CONNECT_SUCCEEDED);
      }
	  mqtt_store_conn_context_to_persistent(conn);
	}
	MQTT_LOG_INFO1("mqtt_process_rxmsg_connack: conn %p MQTT_CONNECTING -> MQTT_CONNECTED", conn);
	return (ESUCCESS);
  }
  if(conn->cfg.clean_session == TRUE)
  {
    mqtt_store_conn_context_to_persistent(conn);
  }
  if (conn->callback != NULL)
  {
    conn->callback(conn, MQTT_MQTT_CONNECT_SUCCEEDED);
  }
  MQTT_LOG_INFO1("mqtt_process_rxmsg_connack: conn %p MQTT_CONNECTING -> MQTT_CONNECTED", conn);

  return (ESUCCESS);
exit:
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  NSINC2(mqtt_stats, badconnack_in);
  if (conn->callback != NULL)
  {
   conn->callback(conn, connack_rc_client);
   conn->flags |= MQTT_FL_INFORM_SESS_BROKEN;
  }
  return (EFAILURE);
}

int
mqtt_process_rxmsg_pubrel(MQTT_CONN_PTR conn)
{
  MQTT_TxCB_PTR tx_cb;
  //uint8_t *buf;
  uint16_t expected_id = 0;
  MQTT_ERR_TYPE status;
  int rc;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_pubrel: parameter error");
    return (EFAILURE);
  }

  tx_cb = conn->txq;

  /* check QoS type of transaction */
  if ((tx_cb != NULL) && (tx_cb->qos != QOS_2))
  {
    NSINC2(mqtt_stats, mismatched_qos_in);
    NSINC2(mqtt_stats, badpubrel_in);
    /* clean receiver state */
    MQTTC_FREE(conn->rx.buf);
    MEMSET(&conn->rx, 0, sizeof(conn->rx));   
    return (EFAILURE);
  }
  else
  {
    MQTT_LOG_INFO("mqtt_process_rxmsg_pubrel: QoS types match");
  }

  rc = mqtt_process_tx_cb(conn, PUBREL,&expected_id); 
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));   
  if(rc == EFAILURE) 
  {
    NSINC2(mqtt_stats, mismatched_pktid_in);
    NSINC2(mqtt_stats, badpubrel_in);
    /*Ignore the pkt id mis-match failure*/
    return (ESUCCESS);
  }

  /* create and send out a PUBCOMP */
  rc = mqtt_pubcomp2(conn, expected_id, &status);
  MQTT_LOG_DEBUG1("mqtt_process_rxmsg_pubrel: mqtt_pubcomp return %d", rc);

  return (rc);
}

/**
 * Deserializes the supplied (wire) buffer into publish data
 * @param dup returned integer - the MQTT dup flag
 * @param qos returned integer - the MQTT QoS value
 * @param retained returned integer - the MQTT retained flag
 * @param packetid returned integer - the MQTT packet identifier
 * @param topicName returned MQTTString - the MQTT topic in the publish
 * @param payload returned byte buffer - the MQTT publish payload
 * @param payloadlen returned integer - the length of the MQTT payload
 * @param buf the raw buffer data, of the correct length determined by the remaining length field
 * @param buflen the length in bytes of the data in the supplied buffer
 * @return error code.  1 is success
 */
int MQTTDeserialize_publish2(int* dup, int* qos, int* retained, unsigned short* packetid, MQTTString* topicName,
    unsigned char** payload, int* payloadlen, int* total_len, unsigned char* buf, int buflen);

int
mqtt_process_rxmsg_publish(MQTT_CONN_PTR conn)
{
  uint8_t *buf;
  int buf_len;
  MQTT_PUBLISH pub;
  MQTTString topic;
  uint8_t *msg;
  int msg_len;
  MQTT_ERR_TYPE status;
  int rc;
  int total_len;
  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_publish: parameter error");
    return (EFAILURE);
  }

  buf = conn->rx.buf;
  buf_len = conn->rx.offset;

  MQTT_LOG_INFO3("mqtt_process_rxmsg_publish: rx.buf %p rx.vhdr %p "
        "buf_len %d", conn->rx.buf, conn->rx.vhdr, buf_len);

  rc = MQTTDeserialize_publish2(&pub.dup, &pub.qos, &pub.retained, &pub.id, 
      &topic, &pub.payload, &pub.payload_length, &total_len, buf, buf_len);
      
  MQTT_LOG_DEBUG1("mqtt_process_rxmsg_publish: MQTTDeserialize_publish return %d", rc);

  if (rc != 1)
  {
    /* clean receiver state */
    NSINC2(mqtt_stats, badpublish_in);
    rc = EFAILURE;
    goto exit;
  }
  MQTT_LOG_INFO3("mqtt_process_rxmsg_publish: DUP %d QoS %d retained "
        "%d", pub.dup, pub.qos, pub.retained);

  msg = pub.payload;
  msg_len = pub.payload_length;
#ifdef MQTT_DEBUG
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS ("mqtt_process_rxmsg_publish: Topic");
    gio_hexdump(NULL, topic.lenstring.data, topic.lenstring.len, TRUE, TRUE);
    MQTT_LOG_MSGS ("mqtt_process_rxmsg_publish: Message");
    gio_hexdump(NULL, msg, msg_len, TRUE, TRUE);
  }
#endif

  /*Validte the msg id*/
  if((pub.qos > QOS_0) && (0 == pub.id))
  {
      MQTT_LOG_ERR("mqtt_process_rxmsg_publish: for qos > 0, the msg id "
                  "should be non-zero");
      NSINC2(mqtt_stats, badpublish_in);
      rc = EFAILURE;
      goto exit;
  }

  switch(pub.qos)
  {
    case QOS_0:
      NSINC2(mqtt_stats, publish0_in);
      break;

    case QOS_1:
      {
        NSINC2(mqtt_stats, publish1_in);
        /* transmit a PUBACK */
        rc = mqtt_puback2(conn, pub.id, &status);
        MQTT_LOG_DEBUG1("mqtt_process_rxmsg_publish: mqtt_puback return %d", rc);
        if (rc == EFAILURE)
        {
          /* clean receiver state */
          goto exit;
        }
        break;
      }

    case QOS_2:
      {
        NSINC2(mqtt_stats, publish2_in);
        /* transmit a PUBREC */
        rc = mqtt_pubrec2(conn, pub.id, &status);
        MQTT_LOG_DEBUG1("mqtt_process_rxmsg_publish: mqtt_pubrec return %d", rc);
        if (rc == EFAILURE)
        {
          /* clean receiver state */
          goto exit;
        }
        break;
      }
    default:
      {
        MQTT_LOG_ERR1("mqtt_process_rxmsg_publish: invalid QOS %d", pub.qos);
        /* clean receiver state */
        NSINC2(mqtt_stats, badpublish_in);
        rc = EFAILURE;
        goto exit;
      }
  }
  if(conn->cfg.pass_msg_hdr == 1)
    mqtt_deliver_published_msg(conn, (uint8_t *)topic.lenstring.data, topic.lenstring.len, 
      buf, total_len, pub.qos);
  else
    mqtt_deliver_published_msg(conn, (uint8_t *)topic.lenstring.data, topic.lenstring.len, 
      msg, msg_len, pub.qos);
 
  MQTT_LOG_PRINT2("mqtt_process_rxmsg_publish dup = %d, retain = %d", pub.dup, pub.retained);
  rc = ESUCCESS;
exit:
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  return rc;
}


int
mqtt_process_rxmsg_puback(MQTT_CONN_PTR conn)
{
  int rc;
  uint16_t expected_id;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_puback: parameter error");
    return (EFAILURE);
  }

  rc = mqtt_process_tx_cb(conn, PUBACK,&expected_id);
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  if(rc == EFAILURE) 
  {
    NSINC2(mqtt_stats, mismatched_pktid_in);
    NSINC2(mqtt_stats, badpuback_in);
    /*Ignore the pkt mis-matchi id failure*/
    return ESUCCESS;
  }

  if (conn->pub_cb != NULL)
  {
    conn->pub_cb(conn, PUBACK, QOS_1, expected_id);
  }
  return (ESUCCESS);
}


int
mqtt_process_rxmsg_pubrec(MQTT_CONN_PTR conn)
{
  MQTT_TxCB_PTR tx_cb;
  //uint8_t *buf;
  uint16_t expected_id;
  MQTT_ERR_TYPE status;
  int rc;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_pubrec: parameter error");
    return (EFAILURE);
  }

  tx_cb = conn->txq;

  /* check QoS type of transaction */
  if ((tx_cb != NULL ) && (tx_cb->qos != QOS_2))
  {
    NSINC2(mqtt_stats, mismatched_qos_in);
    NSINC2(mqtt_stats, badpubrec_in);
    rc = EFAILURE;
    goto exit;
  }
  else
  {
    MQTT_LOG_INFO("mqtt_process_rxmsg_pubrec: QoS types match");
  }

  rc = mqtt_process_tx_cb(conn, PUBREC,&expected_id);
  if(rc == EFAILURE) 
  {
    NSINC2(mqtt_stats, mismatched_pktid_in);
    NSINC2(mqtt_stats, badpubrec_in);
    /*Ignore the pkt mis-matchi id failure*/
    rc = ESUCCESS;
    goto exit;
  }

  /* create and send out a PUBREL */
  rc = mqtt_pubrel2(conn, expected_id, &status);
  MQTT_LOG_DEBUG1("mqtt_process_rxmsg_pubrec: mqtt_pubrel return %d", rc);

  if(conn->pub_cb != NULL)
  {
    conn->pub_cb(conn, PUBREC, QOS_2, expected_id);
  }
  rc = ESUCCESS;
exit:
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  return (rc);
}


int
mqtt_process_rxmsg_pubcomp(MQTT_CONN_PTR conn)
{
  MQTT_TxCB_PTR tx_cb;
  //uint8_t *buf;
  int rc;
  uint16_t expected_id;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_pubcomp: parameter error");
    return (EFAILURE);
  }

  tx_cb = conn->txq;

  /* check QoS type of transaction */
  if ((tx_cb != NULL) && (tx_cb->qos != QOS_2))
  {
    NSINC2(mqtt_stats, mismatched_qos_in);
    NSINC2(mqtt_stats, badpubcomp_in);
    rc = EFAILURE;
    goto exit;
  }
  else
  {
    MQTT_LOG_INFO("mqtt_process_rxmsg_pubcomp: QoS types match");
  }

  rc = mqtt_process_tx_cb(conn, PUBCOMP,& expected_id);     

  if(rc == EFAILURE) 
  {
    NSINC2(mqtt_stats, mismatched_pktid_in);
    NSINC2(mqtt_stats, badpubcomp_in);
    /*Ignore the pkt mis-matchi id failure*/
    rc = ESUCCESS;
    goto exit;
  }

  if(conn->pub_cb != NULL)
  {
    conn->pub_cb(conn, PUBCOMP, QOS_2, expected_id);
  }
  rc = ESUCCESS;
exit:
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  return rc;
}

/* search by packet id */
MQTT_SubRxCB_PTR
mqtt_find_subcb_by_id(MQTT_CONN_PTR conn, uint16_t pkt_id)
{
  MQTT_SubRxCB_PTR sub_cb;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_find_subcb_by_id: parameter error");
    return (NULL);
  }

  for (sub_cb = conn->subq; sub_cb; sub_cb = sub_cb->next)
  {
    if (sub_cb->id == pkt_id)
    {
      MQTT_LOG_DEBUG1("mqtt_find_subcb_by_id: found MQTT_SubRxCB "
            "for pkt id %u", pkt_id);
      return (sub_cb);
    }
  }

  MQTT_LOG_ERR1("mqtt_find_subcb_by_id: fail to find MQTT_SubRxCB "
        "for pkt id %u", pkt_id);
  conn->cfg.last_error = EFAILURE;
  conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
  return (NULL);
}


int
mqtt_process_rxmsg_suback(MQTT_CONN_PTR conn)
{
  uint8_t *buf;
  uint16_t pkt_id;
  MQTT_SubRxCB *sub_cb;
  uint8_t *granted_qos;
  int i, j;
  int granted;
  int rc;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_suback: parameter error");
    return (EFAILURE);
  }

  /* compare packet identifier in received message with expected value */
  buf = conn->rx.buf;
  rc = mqtt_process_tx_cb(conn, SUBACK,&pkt_id);
  if(rc == EFAILURE) 
  {
    NSINC2(mqtt_stats, mismatched_pktid_in);
    NSINC2(mqtt_stats, badsuback_in);
    rc = ESUCCESS;
    goto exit;
  }

  sub_cb = mqtt_find_subcb_by_id(conn, pkt_id);
  if (sub_cb == NULL)
  {
    /* clean receiver state */
    rc = EFAILURE;
    goto exit;
  }

  granted_qos = buf + MQTT_SUBACK_GRANTED_QOS_OFFSET;
  for (i = 0; i < sub_cb->count; ++i)
  {
    sub_cb->sub[i].granted_qos = granted_qos[i];
    granted = granted_qos[i] == 0x80? MQTT_SUBSCRIPTION_DENIED :
      MQTT_SUBSCRIPTION_GRANTED;
    if (granted == MQTT_SUBSCRIPTION_GRANTED)
    {
      MQTT_LOG_PRINT("mqtt_process_rxmsg_suback: Subscription granted");
      NSINC2(mqtt_stats, topic_subscription_granted);
    }
    else
    {
      MQTT_LOG_PRINT("mqtt_process_rxmsg_suback: Subscription rejected");
      NSINC2(mqtt_stats, topic_subscription_denied);
    }
    for (j = 0; j < sub_cb->sub[i].sid_count; j++)
    {
      if (sub_cb->sub[i].status[j] == MQTT_SUBSCRIPTION_STATUS_PENDING &&
          sub_cb->sub[i].callback[j] != NULL)
      {
        sub_cb->sub[i].callback[j](conn, granted, sub_cb->sub[i].topic,
            sub_cb->sub[i].length, NULL, 0,
            sub_cb->sub[i].granted_qos,
            sub_cb->sub[i].sid[j]);
        sub_cb->sub[i].status[j] = granted == MQTT_SUBSCRIPTION_GRANTED?
          MQTT_SUBSCRIPTION_STATUS_ACCEPTED :
          MQTT_SUBSCRIPTION_STATUS_REJECTED;
      }
    }
  }

  if (conn->cfg.clean_session == FALSE)
  {
    mqtt_store_conn_context_to_persistent(conn);
  }
  rc = ESUCCESS;
exit:
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  return rc;
}


int
mqtt_process_rxmsg_unsuback(MQTT_CONN_PTR conn)
{
  int rc;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxmsg_unsuback: parameter error");
    return (EFAILURE);
  }

  rc = mqtt_process_tx_cb(conn, UNSUBACK, NULL);
  if (conn->cfg.clean_session == FALSE)
  {
    mqtt_store_conn_context_to_persistent(conn);
  }
  /* clean receiver state */
  MQTTC_FREE(conn->rx.buf);
  MEMSET(&conn->rx, 0, sizeof(conn->rx));
  if(rc == EFAILURE) 
  {
    NSINC2(mqtt_stats, mismatched_pktid_in);
    NSINC2(mqtt_stats, badunsuback_in);
  }
  return (ESUCCESS);
}

int
mqtt_process_rxdata(MQTT_CONN_PTR conn)
{
  
  uint8_t *buf;

  uint8_t msg_type;
#ifdef MQTT_DEBUG
  int length;
  const char *msg_name;
  uint8_t msg_flags;
#endif

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_rxdata: parameter error");
    return (EFAILURE);
  }

  buf = conn->rx.buf;

#ifdef APPS_PACKET_LOGGING_ENABLE  

#ifdef MQTT_DEBUG
  length = conn->rx.offset;
  if (mqtt_log_level & DUMP_LOG_MSGS)
  {
    MQTT_LOG_MSGS1("mqtt_process_rxdata: Received msg (%d bytes)", length);
    gio_hexdump(NULL, buf, length, TRUE, TRUE);
  }
  
#endif

    dump_buf_to_dpl(conn->sock, buf,  conn->rx.offset, TRUE);

#endif

  msg_type = MQTT_MSG_TYPE(buf);

#ifdef MQTT_DEBUG
  msg_flags = MQTT_MSG_FLAG(buf);
  msg_name = MQTTPacket_getName(msg_type);
  MQTT_LOG_INFO_STRF("mqtt_process_rxdata: rcvd %u (%s) msg: %d", msg_type, msg_name, msg_flags);
#endif

  /* process message - response to our request, or message from broker */
  switch (msg_type)
  {
    case CONNACK: 
      {
        NSINC2(mqtt_stats, connack_in);
        return (mqtt_process_rxmsg_connack(conn));
      }
    case PUBLISH:
      {
        /* publish statistics are maintained on a per-QoS basis */
        return (mqtt_process_rxmsg_publish(conn));
      }
    case PUBACK:
      {
        NSINC2(mqtt_stats, puback_in);
        return (mqtt_process_rxmsg_puback(conn));
      }
    case PUBREC:
      {
        NSINC2(mqtt_stats, pubrec_in);
        return (mqtt_process_rxmsg_pubrec(conn));
      }
    case PUBREL:
      {
        NSINC2(mqtt_stats, pubrel_in);
        return (mqtt_process_rxmsg_pubrel(conn));
      }
    case PUBCOMP:
      {
        NSINC2(mqtt_stats, pubcomp_in);
        return (mqtt_process_rxmsg_pubcomp(conn));
      }
    case SUBACK:
      {
        NSINC2(mqtt_stats, suback_in);
        return (mqtt_process_rxmsg_suback(conn));
      }
    case UNSUBACK:
      {
        NSINC2(mqtt_stats, unsuback_in);
        return (mqtt_process_rxmsg_unsuback(conn));
      }
    case PINGRESP:
      {
        NSINC2(mqtt_stats, pingresp_in);
        return (mqtt_process_rxmsg_pingresp(conn));
      }

      /* we should never receive any of the following message types */
    case CONNECT:
    case SUBSCRIBE:
    case UNSUBSCRIBE:
    case PINGREQ:
    case DISCONNECT:
    default:
      {
        MQTT_LOG_DEBUG_STRF("mqtt_process_rxdata: ignor %s msg", 
             MQTTPacket_getName(msg_type));
        NSINC2(mqtt_stats, unexpected_msg_in);
        break;
      }
  }

  return (ESUCCESS);
}

/* return code is ignored by caller */
int
mqtt_process_conn_rx(MQTT_CONN_PTR conn)
{
  MQTT_CONN_STATE state;
  MQTT_ERR_TYPE status;
  int rc = EFAILURE;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_conn_rx: parameter error");
    return (EFAILURE);
  }

  state = conn->state;
  status = MQTT_ERR_UNKNOWN;


  if (state != MQTT_ST_MQTT_CONNECTING && state != MQTT_ST_MQTT_CONNECTED)
  {
    return (ESUCCESS);
  }


  switch (conn->rx.state)
  {
    case MQTT_RX_ST_WAITING_FOR_MSG_PART1:
      {
        if (conn->rx.buf == NULL)
        {
          conn->rx.buf = MQTTC_ALLOC(MQTT_FIXED_HDR_MAX_LEN);
          if (conn->rx.buf == NULL)
          {
            /* try again in the next iteration */
            NSINC2(mqtt_stats, alloc_failures);
            break;
          }
          conn->rx.offset = 0;
          conn->rx.length = MQTT_FIXED_HDR_MAX_LEN;
        }
        rc = mqtt_read_msg1(conn, &status);
        if (rc != EFAILURE)
        {
          if (status == MQTT_ERR_NO_ERR)
          {
            uint8_t *ptr;
            int vhdr_offset;
            int new_length = conn->rx.offset + rc;

            /* fixed header has been completely read out from socket; check 
             * to see if there is additional unread data in message (and if 
             * it does not fit in existing buffer).
             */
            if (rc > 0) 
            {
              if (new_length > conn->rx.length)
              {
                /* allocate and setup buffer for use in the next 
                 * (MQTT_RX_ST_WAITING_FOR_MSG_PART2) state.  Skip 
                 * (re)allocating if requirement can be met by current 
                 * allocation.
                 */
                ptr = MQTTC_ALLOC(new_length);
                if (ptr == NULL)
                {
                  NSINC2(mqtt_stats, alloc_failures);
                  break;
                }
                memscpy(ptr, conn->rx.offset, conn->rx.buf, conn->rx.offset);
                vhdr_offset = conn->rx.vhdr - conn->rx.buf;
                MQTTC_FREE(conn->rx.buf);
                conn->rx.buf = ptr;
                conn->rx.vhdr = conn->rx.buf + vhdr_offset;
                conn->rx.length = new_length;
              }
            }
            else
            {
              rc = mqtt_process_rxdata(conn);
            }
          }
        }
        break;
      }

    case MQTT_RX_ST_WAITING_FOR_MSG_PART2:
      {
        rc = mqtt_read_msg2(conn, &status);
        if (rc != EFAILURE)
        {
          if (conn->rx.state == MQTT_RX_ST_MSG_COMPLETE)
          {
            /* process complete message */
            rc = mqtt_process_rxdata(conn);
          }
        }
        break;
      }

    case MQTT_RX_ST_MSG_COMPLETE:
      {
        rc = mqtt_process_rxdata(conn);
        break;
      }
  }

  /* handle failure by setting connection state to DYING */
  if (rc == EFAILURE)
  {
    conn->state = MQTT_ST_DYING;
    ds_iot_log_strf_err("mqtt_process_conn_rx: conn %p %s -> DYING", conn,mqtt_get_conn_state_name(state));   
    NSINC2(mqtt_stats, transitioned_to_dying);
    return (EFAILURE);
  }

  return (ESUCCESS);
}

int mqtt_conn_establish(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status)
{
  SOCKTYPE      sock;
  int           rc;
  int           sock_err;
  MQTT_ERR_TYPE status1;
#ifdef MQTT_CLIENT_SECURITY
  int           result;
#endif

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_conn_establish: parameter error");
    if (status != NULL)
      *status = MQTT_ERR_BAD_PARAM;

    return EFAILURE;
  }

  sock = conn->sock;

  switch (conn->state)
  {
    case MQTT_ST_TCP_CONNECTING:
      {
        /* check to see if the TCP connection process is complete before transmitting 
         * the CONNECT message
         */
        rc = qapi_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_err, sizeof(int));
        MQTT_LOG_DEBUG2("mqtt_conn_establish: qapi_getsockopt(SOL_SOCKET:SO_ERROR) return %d for conn %p", rc, conn);
        if (rc == ESUCCESS && sock_err == QAPI_OK)
        {
          if (!mqtt_is_secure_conn(conn))
          {
            conn->state = MQTT_ST_TCP_CONNECTED;
            MQTT_LOG_INFO1("mqtt_conn_establish: conn %p TCP_CONNECTING -> TCP_CONNECTED", conn);
            /* send the CONNECT message */
            rc = mqtt_tx_connect(conn, &status1);
            MQTT_LOG_DEBUG3("mqtt_conn_establish: mqtt_tx_connect return %d (status %d) for conn %p", rc, status1, conn);
            if(status != NULL)
              *status = status1;

            return rc;
          }
#ifdef MQTT_CLIENT_SECURITY
          else
          {
            //not break out and continue to do SSL connect in case MQTT_ST_SSL_CONNECTING.
            conn->state = MQTT_ST_SSL_CONNECTING;
            MQTT_LOG_INFO1("mqtt_conn_establish: conn %p TCP_CONNECTING -> SSL_CONNECTING", conn);
          }
#endif
        }
        else
        {
          if ((rc != ESUCCESS) || (sock_err != EINPROGRESS))
          {
            conn->state = MQTT_ST_DYING;
            NSINC2(mqtt_stats, socket_failures);
            NSINC2(mqtt_stats, connect_fails);
            NSINC2(mqtt_stats, transitioned_to_dying);
            MQTT_LOG_ERR1("mqtt_conn_establish: conn %p TCP_CONNECTING -> DYING", conn);
            if (rc == ESUCCESS)
            {
              MQTT_LOG_ERR2("mqtt_conn_establish: errno for fail "
                    "conn: %d for conn %p", sock_err, conn);
			  conn->cfg.last_error = sock_err;
	          conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;
            }
            if(conn->callback != NULL)
              conn->callback(conn, MQTT_TCP_CONNECT_FAILED);

            if(status != NULL)
                *status = MQTT_ERR_TCP_CONN_FAILED;

            return EFAILURE;
          }
          break;
        }
      }
#ifdef MQTT_CLIENT_SECURITY
    case MQTT_ST_SSL_CONNECTING: 
      {
        if(!QAPI_MQTT_IS_OPTION(&(conn->cfg), QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE))
        {
          /*if no security mode is set, regard it as cert mode as the default value*/
          conn->cfg.security_mode = QAPI_MQTT_CERTIFICATE;
          QAPI_MQTT_SET_OPTION(&(conn->cfg), QAPI_NET_MQTT_EXTENDED_CONFIG_SEC_MODE);
        }
        result = mqtt_ssl_cert_load(conn, NULL);
        if(result < QAPI_OK)
        {
            conn->cfg.last_error = result;
	        conn->cfg.module_cat = QAPI_NET_SSL_CAT;
            goto exit_ssl_failure;
        }

        result = mqtt_start_ssl_connection(conn);
        if(result == QAPI_SSL_OK_HS)
        {
          /* send the CONNECT message */
          rc = mqtt_tx_connect(conn, &status1);
          MQTT_LOG_DEBUG3("mqtt_conn_establish: mqtt_tx_connect "
               "return %d (status %d) for conn %p", rc, status1, conn);
          if(status != NULL)
            *status = status1;

          return rc;
        }
        else if(result < QAPI_OK)
        {
          conn->cfg.last_error = result;
	      conn->cfg.module_cat = QAPI_NET_SSL_CAT;
          goto exit_ssl_failure;
        }
exit_ssl_failure:
        conn->state = MQTT_ST_DYING;
        NSINC2(mqtt_stats, connect_fails);
        if(status != NULL)
          *status = MQTT_ERR_SSL_CONN_FAILURE;

        return EFAILURE;
      }
#endif /*MQTT_CLIENT_SECUIRTY*/
    default:
      //should not happen
      break;
  }

    if(status != NULL)
      *status = MQTT_ERR_NO_ERR;

    return ESUCCESS;
}

static void
mqtt_incr_tx_stats(uint8_t type, int qos)
{
  switch (type)
  {
    case CONNECT:
      {
        NSINC2(mqtt_stats, connect_out);
        break;
      }
    case PUBLISH:
      {
        switch (qos)
        {
          case QOS_0:
            NSINC2(mqtt_stats, publish0_out);
            break;
          case QOS_1:
            NSINC2(mqtt_stats, publish1_out);
            break;
          case QOS_2:
            NSINC2(mqtt_stats, publish2_out);
            break;
        }
        break;
      }
    case PUBACK:
      {
        NSINC2(mqtt_stats, puback_out);
        break;
      }
    case PUBREC:
      {
        NSINC2(mqtt_stats, pubrec_out);
        break;
      }
    case PUBREL:
      {
        NSINC2(mqtt_stats, pubrel_out);
        break;
      }
    case PUBCOMP:
      {
        NSINC2(mqtt_stats, pubcomp_out);
        break;
      }
    case SUBSCRIBE:
      {
        NSINC2(mqtt_stats, subscribe_out);
        break;
      }
    case UNSUBSCRIBE:
      {
        NSINC2(mqtt_stats, unsubscribe_out);
        break;
      }
    case PINGREQ:
      {
        NSINC2(mqtt_stats, pingreq_out);
        break;
      }
    case DISCONNECT:
      {
        NSINC2(mqtt_stats, disconnect_out);
        break;
      }
    default:
      {
        NSINC2(mqtt_stats, unexpected_msg_out);
        break;
      }
  }
}

void
mqtt_ping_server(MQTT_CONN_PTR conn)
{
  int kal;
  MQTT_ERR_TYPE status;
  int rc;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_ping_server: parameter error");
    return;
  }

  kal = conn->cfg.keepalive_duration;

  if (kal > 0)
  {
    if (TIME_EXP(conn->last_tx + kal, CTICKS))
    {
      if (conn->pingresp_pending == MQTT_PINGRESP_FAILURE_THRESHOLD)
      {
        MQTT_LOG_ERR1("mqtt_ping_server: reached PINGRESP "
              "failure threshold; conn %p MQTT_CONNECTED -> DYING", conn);
        conn->state = MQTT_ST_DYING;
		conn->cfg.last_error = MQTT_ERR_CONN_CLOSED;
	    conn->cfg.module_cat = QAPI_NET_MQTT_CAT;
        NSINC2(mqtt_stats, ping_failures);
        NSINC2(mqtt_stats, transitioned_to_dying);
        return;
      }
      rc = mqtt_ping2(conn, &status);
      MQTT_LOG_DEBUG1("mqtt_ping_server: mqtt_ping return %d", rc);
      if (rc == ESUCCESS)
      {
        //conn->pingresp_pending++;
      }
    }
  }
}

/* handle errors */
int
mqtt_process_conn_tx(MQTT_CONN_PTR conn)
{
  MQTT_CONN_STATE state;
  int length = 0;
  MQTT_ERR_TYPE status;
  MQTT_TxCB_PTR tx_cb;
  MQTT_TxCB_PTR next_tx_cb;
  MQTT_TxCB_PTR prev_tx_cb = NULL;
  //SOCKTYPE sock;
  int offset;
  int rc;

  if (conn == NULL)
  {
    MQTT_LOG_DEBUG("mqtt_process_conn_tx: parameter error");
    return (EFAILURE);
  }

  /* if we were in the process of establishing a TCP connection and the socket 
   * has become writable, the TCP connection process is complete
   */
  state = conn->state;
  if (state == MQTT_ST_TCP_CONNECTING || state == MQTT_ST_SSL_CONNECTING)
  {
    mqtt_conn_establish(conn, NULL);
    return (ESUCCESS);
  }

  /* send PINGREQ message to server if appropriate */
  if (state == MQTT_ST_MQTT_CONNECTED)
  {
    mqtt_ping_server(conn);
    if(conn->state != MQTT_ST_MQTT_CONNECTED) {
      return (EFAILURE);
    }
  }
  /* http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/csprd02/mqtt-v3.1.1-csprd02.html
   *
   * When a Client reconnects with CleanSession set to 0, both the Client and 
   * Server MUST re-send any unacknowledged PUBLISH Packets (where QoS > 0) 
   * and PUBREL Packets using their original Packet Identifiers [MQTT-4.4.0-1].
   */
  tx_cb = conn->txq;
  

  /* XXX add code to skip following under certain conditions... */

  /* check for pending transmits */
  //sock = conn->sock;
  for (tx_cb = conn->txq; tx_cb; tx_cb = next_tx_cb)
  {
    next_tx_cb = tx_cb->next;
    offset = tx_cb->offset;
    length = tx_cb->length;
    
    if(tx_cb->response == PUBACK ||
       tx_cb->response == PUBREC ||
       tx_cb->response == PUBCOMP) 
    {
        if (tx_cb->flags & MQTT_FL_TXCB_RESTORED )
        {
            offset = 0;
            tx_cb->flags &= ~MQTT_FL_TXCB_RESTORED;
        } 
        else 
        {
            if((offset == length) && (tx_cb->pub_retransmit_count < MQTT_PUB_RETRANSMIT_THRESHOLD))
            {
                int timer_val = tx_cb->created + (conn->cfg.pub_ack_timeout * TPS);
                if(TIME_EXP(timer_val, CTICKS))
                {
                    offset = tx_cb->offset = 0;
                    if((tx_cb->buf != NULL) && (tx_cb->response != PUBCOMP)) /*DUP flag should be false in PUBREL retransmit.Filter out PUBREL*/
                        tx_cb->buf[0] = ((1 << 3) | tx_cb->buf[0]); /* Setting DUP flag to true in PUBLISH retransmit*/
                    tx_cb->pub_retransmit_count++;
                    tx_cb->created = CTICKS;
                    MQTT_LOG_PRINT3("mqtt_process_conn_tx: Re-transmit Publish buf %p with qos %d count %d", tx_cb->buf, tx_cb->qos, tx_cb->pub_retransmit_count);
                }
            }
        }
    }
    
    /* check to see if this transaction has not been completely written into 
     * the socket
     */
    if (offset < length)
    {
      rc = mqtt_wr_sock(conn, tx_cb->buf + offset, length - offset, 0, &status);
      if (rc == EFAILURE)
      {
        MQTT_LOG_ERR1("mqtt_process_conn_tx: conn %p fatal err on sock", conn);
        conn->state = MQTT_ST_DYING;

        MQTT_LOG_INFO_STRF("mqtt_process_conn_tx: conn %p %s -> DYING", conn, mqtt_get_conn_state_name(state));
        return (EFAILURE);
      }
      else if (rc == 0)
      {
        /* offset stays unchanged */
        return (ESUCCESS);
      }
      else
      {
        tx_cb->offset += rc;
        if (tx_cb->offset < tx_cb->length)
        {
          return (ESUCCESS);
        }
        else if (tx_cb->offset == tx_cb->length)
        {
          uint8_t type;

          type = MQTT_MSG_TYPE(tx_cb->buf);
          mqtt_incr_tx_stats(type, tx_cb->qos);

          /* delete QoS 0 transaction (since it has been completely written 
           * into the socket)
           */
          if (tx_cb->response == MQTT_NO_RESPONSE_MSG_REQD)
          {
            if (prev_tx_cb != NULL)
            {
              prev_tx_cb->next = tx_cb->next;
            }
            else
            {
              conn->txq = tx_cb->next;
            }
            MQTTC_FREE(tx_cb->buf);
            MQTTC_FREE(tx_cb);
          }
        }
      }
    }
    prev_tx_cb = tx_cb;
  }

  return (ESUCCESS);
}

int
mqtt_process_conn_err(MQTT_CONN_PTR conn)
{
  int rc = 0;
  int sock_err = 0;
  /* handle exceptional condition (indicated via qapi_select()) */
  rc = qapi_getsockopt(conn->sock, SOL_SOCKET, SO_ERROR, &sock_err, sizeof(int));
  MQTT_LOG_ERR3("mqtt_process_conn_err: qapi_getsockopt: return %d sock_err %d for conn %p", rc, sock_err, conn);

  if(rc != ESUCCESS || (sock_err != EWOULDBLOCK && sock_err != EINPROGRESS)) {
    NSINC2(mqtt_stats, socket_failures);
	if(sock_err != 0)
	{
		conn->cfg.last_error = sock_err;
        conn->cfg.module_cat = QAPI_NET_SOCKET_CAT;
	}
    conn->state = MQTT_ST_DYING;
    return (EFAILURE);
  }

  return (ESUCCESS);
}

void
tk_mqttc_v2(void)
{
  int i;
  MQTT_CONN_PTR conn;
  MQTT_CONN_PTR next_conn;

  fd_set rd_set;
  fd_set wr_set;
  fd_set ex_set;
  long wait_duration = MQTT_SELECT_TIMEOUT; /* milli secs */
  int num_ready_fds = 0;

  uint8_t mutex_taken[FD_SETSIZE] = {0};

  FD_ZERO(&rd_set);
  FD_ZERO(&wr_set);
  FD_ZERO(&ex_set);
  
  MQTT_LOCK_MUTEX(mqtt_mutex);

  for (i = 0, conn = MQTT_connlist; conn != NULL; i++, conn = next_conn)
  {
    next_conn = conn->next;


#ifndef SUPERLOOP
    /* non-blocking behavior consistent across all operating environments? */
    if (TK_MUTEX_PEND(conn->mutex, (TPS * 10)) == TK_TIMEOUT)
    { 
      continue;
    }
#endif

    if (conn->sock != INVALID_SOCKET)
    {
      FD_SET(conn->sock, &rd_set);
      FD_SET(conn->sock, &wr_set);
      FD_SET(conn->sock, &ex_set);
    }
    else
    {
      mqtt_destroy_conn(conn, FALSE);
      continue;
    }

    mutex_taken[i] = TRUE;
  }

  num_ready_fds = qapi_select(&rd_set, &wr_set, &ex_set, wait_duration);

  if (num_ready_fds <= 0) 
  {
    /* no descriptors ready for I/O or exception; timeout occurred */
    for (i = 0, conn = MQTT_connlist; conn != NULL; i++, conn = conn->next)
    {
      if (mutex_taken[i]) 
      {
        TK_MUTEX_POST(conn->mutex);
      }
    }
    MQTT_UNLOCK_MUTEX(mqtt_mutex);
    return;
  }

  for (i = 0, conn = MQTT_connlist; conn != NULL; i++, conn = next_conn)
  {
    next_conn = conn->next;
    if (mutex_taken[i])
    {
      if (conn->sock != INVALID_SOCKET)
      {
        /* perform rx-related processing */
        if ((FD_ISSET(conn->sock, &rd_set)))
        {
          mqtt_process_conn_rx(conn);
        }
        /* perform tx-related processing */
        if ((FD_ISSET(conn->sock, &wr_set)))
        {
          mqtt_process_conn_tx(conn);
          if ((mqtt_chk_wr_pending(conn) == FALSE) && 
              (conn->state == MQTT_ST_MQTT_TERMINATING))
          {
            mqtt_destroy_conn(conn, FALSE);
            continue;
          }
        }

        if ((FD_ISSET(conn->sock, &ex_set)))
        {
          mqtt_process_conn_err(conn);
        }
        
        if(conn->state == MQTT_ST_DYING) {
            if((conn->callback != NULL) && (!(conn->flags & MQTT_FL_INFORM_SESS_BROKEN))) {
              conn->callback(conn, MQTT_MQTT_CONNECT_FAILED);
              conn->flags |= MQTT_FL_INFORM_SESS_BROKEN;
            }
        }
      }
      TK_MUTEX_POST(conn->mutex);
    }
  }

  MQTT_UNLOCK_MUTEX(mqtt_mutex);
}

int mqtt_conn_restore_state(MQTT_CONN_PTR conn)
{
  struct MQTT_TxCB *tx      = NULL;
  struct MQTT_TxCB *next_tx = NULL;
  struct MQTT_TxCB *prev_tx = NULL;
  
  if(conn->restore != NULL) {
    
    TK_MUTEX_PEND(conn->mutex, 0);
    conn->txq   = conn->restore->txq;
    conn->subq  = conn->restore->subq;
    prev_tx = tx = conn->txq;

    while (tx)
    {
      next_tx = tx->next;
      if((tx->response != PUBACK) && (tx->response != PUBREC) && (tx->response != PUBCOMP)) {
        if(prev_tx == conn->txq) {
            conn->txq = next_tx;
        } else {
            prev_tx->next = next_tx;
        }
        MQTTC_FREE(tx->buf);
        MQTTC_FREE(tx);
      } else {
        tx->pub_retransmit_count = 0;
        prev_tx = tx;
      }
      tx = next_tx;
    }
    
    TK_MUTEX_POST(conn->mutex);
    return ESUCCESS;
  } else {
    conn->flags |= MQTT_FL_RESTORE_FAILED;
    return EFAILURE;
  }
}


void
mqtt_conn_free_saved_bufs(uint8_t *buf)
{
   uint32_t uvar;
   uint8_t *next_buf = buf;
   while (next_buf != NULL)
   {
      uvar = *(uint32_t *)buf;
      uvar = ntohl(uvar);
      next_buf = (uint8_t *)uvar;
      MQTTC_FREE(buf);
      buf = next_buf;
   }
}

void
mqtt_conn_free_subq(struct MQTT_SubscribeRxCB *sub_cb)
{
   struct MQTT_SubscribeRxCB *next_sub;
   int i;
   while (sub_cb)
   {
      next_sub = sub_cb->next;
      for (i = 0; i < sub_cb->count; ++i)
      {
         MQTTC_FREE(sub_cb->sub[i].topic);
      }
      MQTTC_FREE(sub_cb);
      sub_cb = next_sub;
   }
}
/**
 * Serializes subcscription list into a chain of buffers in layout mentioned below
 * @param conn the connection whose subscription list need to be serialized.
 * @param status stores errors if any occured.
 * @return pointer to chain of buffers into which subscription is serialized
 *
 * layout:
 * [next][reqd_len][count][id]
 * [length][topic[length]+padding][requested_qos][granted_qos][sid_count][[sid][callback][status]...[sid][callback][status]]
 * ...
 * [length][topic[length]+padding][requested_qos][granted_qos][sid_count][[sid][callback][status]...[sid][callback][status]]
 */
uint8_t *
mqtt_conn_save_subscriptions(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status)
{
   struct MQTT_SubscribeRxCB *sub_cb;
   int reqd_len;
   uint8_t *prev_sub = NULL;
   uint8_t *buf;
   uint8_t *first_buf = NULL;
   int i, j;
   int padding;
   int count;
   int sid_count;
   int num_entries = 0;
   int offset;
   uint8_t *optr;
   uint32_t uvar;
   UNUSED(offset);
   if (conn == NULL)
   {
      NSINC2(mqtt_stats, parameter_error);
      MQTT_LOG_INFO("mqtt_conn_save_subscriptions: parameter error");
      *status = MQTT_ERR_BAD_PARAM;
      return (NULL);
   }

   for (sub_cb = conn->subq; sub_cb != NULL; sub_cb = sub_cb->next)
   {
      reqd_len = sizeof(uint32_t) + sizeof(int32_t) + sizeof(uint32_t) + sizeof(sub_cb->count);

      count = sub_cb->count;
      for (i = 0; i < count; ++i)
      {
         padding = 4 - sub_cb->sub[i].length%4;
         /* pad message to a multiple of 4 bytes */
         reqd_len += sizeof(sub_cb->sub[i].length) + sub_cb->sub[i].length +
                     padding + sizeof(sub_cb->sub[i].requested_qos) +
                     sizeof(sub_cb->sub[i].granted_qos);
         sid_count = sub_cb->sub[i].sid_count;
         for (j = 0; j < sid_count; ++j)
         {
            reqd_len += sizeof(uint32_t) * 2 + sizeof(sub_cb->sub[i].status[j]) +
                        sizeof(sub_cb->sub[i].sid_count);
         }
      }
      MQTT_LOG_INFO1("mqtt_conn_save_subscriptions: reqd_len %d", reqd_len);

      buf = MQTTC_ALLOC(reqd_len);
      if (buf == NULL)
      {
         mqtt_conn_free_saved_bufs(first_buf);
         MQTT_LOG_INFO("mqtt_conn_save_subscriptions: fail to alloc buf");
         NSINC2(mqtt_stats, alloc_failures);
         *status = MQTT_ERR_ALLOC_FAILURE;
         return (NULL);
      }

      if (first_buf == NULL)
      {
         first_buf = buf;
      }

      optr = buf;

      if (prev_sub != NULL)
      {
         uvar = (uint32_t)buf;
         *(uint32_t *)prev_sub = htonl(uvar);
      }
      optr += sizeof(uint32_t);

      htonl_int32_t(&optr, reqd_len);
      htonl_int32_t(&optr, sub_cb->count);
      htonl_uint32_t(&optr, sub_cb->id);

      for (i = 0; i < count; ++i)
      {
         htonl_int32_t(&optr, sub_cb->sub[i].length);

         MEMCPY(optr, sub_cb->sub[i].topic, sub_cb->sub[i].length);
         padding = 4 - sub_cb->sub[i].length%4;
         optr += sub_cb->sub[i].length + padding;

         htonl_int32_t(&optr, sub_cb->sub[i].requested_qos);
         htonl_int32_t(&optr, sub_cb->sub[i].granted_qos);
         htonl_int32_t(&optr, sub_cb->sub[i].sid_count);

         sid_count = sub_cb->sub[i].sid_count;
         for (j = 0; j < sid_count; ++j)
         {
            htonl_uint32_t(&optr, (uint32_t)sub_cb->sub[i].sid[j]);
            htonl_uint32_t(&optr, (uint32_t)sub_cb->sub[i].callback[j]);
            htonl_int32_t(&optr, sub_cb->sub[i].status[j]);
         }
      }

      offset = optr - buf;
      MQTT_LOG_INFO3("mqtt_conn_save_subscriptions: saved %d bytes for entry #%d in buf %p", offset, num_entries, buf);


      /* prior to start of next iteration of loop */
      prev_sub = buf;
      ++num_entries;
   }
   MQTT_LOG_INFO2("mqtt_conn_save_subscriptions: saved %d entries from conn %p's subscription list", num_entries, conn);

   return (first_buf);
}

/**
 * Restores subcscription list after deserializing list of buffers into a MQTT_RESTORE_PTR
 * @param restore the MQTT_RESTORE_PTR pointer into which subscription list is restored.
 * @param subq buffer containing serialized subscription list.
 * @param subq_count count of buffers containing serialized subscription list.
 * @return 0 SUCCESS, else < 0
 *
 * layout:
 * [next][reqd_len][count][id]
 * [length][topic[length]+padding][requested_qos][granted_qos][sid_count][[sid][callback][status]...[sid][callback][status]]
 * ...
 * [length][topic[length]+padding][requested_qos][granted_qos][sid_count][[sid][callback][status]...[sid][callback][status]]
 */

int
mqtt_conn_restore_subscriptions(MQTT_RESTORE_PTR restore, sub_persist_info_t *subq, int subq_count)
{
  int count;
  struct MQTT_SubscribeRxCB *sub_cb;
  uint8_t *next_sub;
  int i, j;
  int32_t var;
  uint32_t uvar;
  uint8_t *iptr;
  uint8_t *topic;
  int length;
  int padding;
  int sid_count;
  int reqd_len;
  int index = 0;
  UNUSED(next_sub);
  UNUSED(reqd_len);

  if (restore == NULL || subq == NULL)
  {
    NSINC2(mqtt_stats, parameter_error);
    MQTT_LOG_INFO("mqtt_conn_restore_subscriptions: parameter error");
    return (EFAILURE);
  }

  while (index < subq_count)
  {
    iptr = subq[index].sub;
    MQTT_LOG_INFO1("mqtt_conn_restore_subscriptions: restore subscription list entry from %p...", iptr);

    uvar = *(uint32_t *)iptr;
    uvar = ntohl(uvar);
    next_sub = (uint8_t *)uvar;
    iptr += sizeof(uint32_t);

    ntohl_int32_t(&reqd_len, &iptr);
    ntohl_int32_t(&count, &iptr);

    sub_cb = MQTTC_ALLOC(sizeof(struct MQTT_SubscribeRxCB) + count - 1);
    if (sub_cb == NULL)
    {
       mqtt_conn_free_subq(restore->subq);
       MQTT_LOG_INFO("mqtt_conn_restore_subscriptions: fail to alloc buf");
       NSINC2(mqtt_stats, alloc_failures);
       return (EFAILURE);
    }

    if(count <= 0)
    {
      mqtt_conn_free_subq(restore->subq);
      MQTT_LOG_INFO("mqtt_conn_restore_subscriptions: no topics");
      MQTTC_FREE(sub_cb);
      return (EFAILURE);
    }

    sub_cb->count = count;
    uvar = *(uint32_t *)iptr;
    sub_cb->id = (uint16_t)ntohl(uvar);
    iptr += sizeof(uint32_t);

    for (i = 0; i < count; ++i)
    {
       ntohl_int32_t(&length, &iptr);
       sub_cb->sub[i].length = length;
       if (length > 0) 
       {
          topic = sub_cb->sub[i].topic = MQTTC_ALLOC(length);
          if (topic == NULL)
          {
             mqtt_conn_free_subq(restore->subq);
             MQTTC_FREE(sub_cb);
             MQTT_LOG_INFO("mqtt_conn_restore_subscriptions: fail to alloc buf");
             NSINC2(mqtt_stats, alloc_failures);
             return (EFAILURE);
          }
          MEMCPY(topic, iptr, length);
          padding = 4 - length%4;
          iptr += length + padding;
       }
       ntohl_int32_t(&(sub_cb->sub[i].requested_qos), &iptr);
       ntohl_int32_t(&(sub_cb->sub[i].granted_qos), &iptr);
       ntohl_int32_t(&sid_count, &iptr);
       sub_cb->sub[i].sid_count = sid_count;

       for (j = 0; j < sid_count; ++j)
       {
          var = *(uint32_t *)iptr;
          sub_cb->sub[i].sid[j] = (void *)ntohl(var);
          iptr += sizeof(uint32_t);
          var = *(uint32_t *)iptr;
          sub_cb->sub[i].callback[j] = &mqtt_intr_subscribe_callback;
          iptr += sizeof(uint32_t);
          ntohl_int32_t(&(sub_cb->sub[i].status[j]), &iptr);
       }
       /* add subscription to connection */
       sub_cb->next = restore->subq;
       restore->subq = sub_cb;
    }
  index++;
  }

  return (ESUCCESS);
}

/**
 * Stores connection context into persistence file "/datatx/mqtt/mqtt_restore_persist"
 * @param conn the connection pointer whose context needs to be persisted.
 * @return 0 SUCCESS, else < 0
 */
int mqtt_store_conn_context_to_persistent(MQTT_CONN_PTR conn)
{
  int fd = 0, ret = 0, index = 0;
  uint8_t *buf = NULL;
  uint8_t *next_sub = NULL;
  uint8_t *iptr;
  int32_t var;
    uint32_t uvar;
  int reqd_len;
  MQTT_ERR_TYPE status = MQTT_ERR_NO_ERR;
  mqtt_persist_info_t *info = NULL;
  mqtt_persist_info_t *conn_context = NULL;
  int i = 0;
  struct fs_statvfs stat_buf;

  if (NULL == conn)
  {
    MQTT_LOG_INFO("mqtt_store_conn_context_to_persistent: pass null args");
    return EFAILURE;
  }

  info = (mqtt_persist_info_t *)malloc(sizeof(mqtt_persist_info_t));
  if(info == NULL)
  {
    MQTT_LOG_INFO("mqtt_store_conn_context_to_persistent: Memory alloc fail");
    return EFAILURE;
  }
  conn_context = (mqtt_persist_info_t *)malloc(sizeof(mqtt_persist_info_t));
  if(conn_context == NULL)
  {
    MQTT_LOG_INFO("mqtt_store_conn_context_to_persistent: Memory alloc fail");
    free(info);
    return EFAILURE;
  }
  
  /* Check root data EFS folder exist or not. If not, create it*/
  ret = efs_mkdir("/datatx/", 0777);
  if ((ret < 0) && (efs_errno != EEXIST)) {
      MQTT_LOG_DEBUG1("mqtt_store_conn_context_to_persistent: Unable to create /datatx/ folder. efs_errno: %d", efs_errno);
      ret = EFAILURE;
      goto END;
  }

  /* Check root mqtt EFS folder exist or not. If not, create it*/
  ret = efs_mkdir("/datatx/mqtt/", 0777);
  if ((ret < 0) && (efs_errno != EEXIST)) {
      MQTT_LOG_DEBUG1("mqtt_store_conn_context_to_persistent: Unable to create /datatx/mqtt/ folder. efs_errno %d", efs_errno);
      ret = EFAILURE;
      goto END;
  }

  /* open mqtt_restore_persist file.If not already present, create it*/
  fd = efs_open(MQTT_PERSISTENCE_FILE, O_CREAT | O_RDWR, 00644);
  if (EFAILURE == fd)
  {
    MQTT_LOG_INFO("mqtt_store_conn_context_to_persistent: fail to open MQTT persist file");
    ret = EFAILURE;
    goto END;
  }

  /* get stats of opened file */
  efs_fstatvfs(fd, &stat_buf);

  while (1)
  {
    memset(info, 0x00, sizeof(mqtt_persist_info_t));
    ret = efs_read(fd, info, sizeof(mqtt_persist_info_t));
    if (ret <= 0)
    {
      break;
    }
    if (strncmp((char *)info->client_id, (char *)conn->cfg.client_id, strlen((char *)info->client_id)) == 0)
    {
      break;
    }
    index++;
  }

  memset(conn_context, 0x00, sizeof(mqtt_persist_info_t));
  strlcpy((char *)(conn_context->client_id), (char *)(conn->cfg.client_id), MAX_CLIENT_ID_LEN);

  /* get subscription list serialized into a chain of buffers */
  buf = mqtt_conn_save_subscriptions(conn, &status);
  if (buf == NULL && status != MQTT_ERR_NO_ERR)
  {
  	ret = EFAILURE;
  	efs_close(fd);
      goto END;
  }
  iptr = buf;

  /* copy serialized subscription list buffers into mqtt_persist_info structure */
  while(iptr != NULL && i < MAX_SUB_PERSISTED)
  {
    uvar = *(uint32_t *)iptr;
    uvar = ntohl(uvar);
    next_sub = (uint8_t *)uvar;

    var = *((int32_t *)(iptr + sizeof(uint32_t)));
    reqd_len = ntohl(var);
    if(reqd_len < MAX_SUB_BUF_LEN)
    	MEMCPY(&(conn_context->subq[i]), iptr, reqd_len);
    i++;
    free(iptr);
    iptr = next_sub;
  }
  conn_context->subq_count = i;
  i = 0;

  /* lseek to reqd index and write the mqtt_persist_info structure in efs */
  efs_lseek(fd, index * sizeof(mqtt_persist_info_t), SEEK_SET);
  if (EFAILURE == efs_write(fd, conn_context, sizeof(mqtt_persist_info_t)))
  {
    MQTT_LOG_INFO1("mqtt_store_conn_context_to_persistent: fail to store persist value of conn %p", conn);
    efs_close(fd);
    ret = EFAILURE;
    goto END;
  }
  efs_close(fd);
  ret = ESUCCESS;
  END:
  free(info);
  free(conn_context);
  return ret;
}

/**
 * Restores connection context from persistence file "/datatx/mqtt/mqtt_restore_persist"
 * @param restore the MQTT_RESTORE_PTR pointer in which context needs to be restored.
 * @param client_id the client_id whose context needs to be restored.
 * @return 0 SUCCESS, else < 0
 */
int mqtt_load_restore_pointer_from_persistent(MQTT_RESTORE_PTR *restore, uint8_t *client_id)
{
  int fd = 0, ret = 0;
  mqtt_persist_info_t *info = NULL;

  if (client_id == NULL)
  {
    MQTT_LOG_INFO("mqtt_load_restore_pointer_from_persistent: pass null args");
    return EFAILURE;
  }

  fd = efs_open(MQTT_PERSISTENCE_FILE, 00);
  if (EFAILURE == fd)
  {
    MQTT_LOG_INFO("mqtt_load_restore_pointer_from_persistent: fail to open mqtt persist file");
    return EFAILURE;
  }

  info = (mqtt_persist_info_t *)malloc(sizeof(mqtt_persist_info_t));
  if(info == NULL)
  {
    MQTT_LOG_INFO("mqtt_load_restore_pointer_from_persistent: Memory alloc fail");
    return EFAILURE;
  }

  while (1)
  {
    memset(info, 0x00, sizeof(mqtt_persist_info_t));

    ret = efs_read(fd, info, sizeof(mqtt_persist_info_t));
    if (ret <= 0)
    {
      break;
    }
    if (strncmp((char *)info->client_id, (char *)client_id, strlen((char *)info->client_id)) != 0)
    {
      continue;
    }
    else
    {
      *restore = MQTTC_ALLOC(sizeof(MQTT_RESTORE));
      if (NULL == *restore)
      {
        ret = -1;
      }
      else
      {
        mqtt_conn_restore_subscriptions(*restore, info->subq, info->subq_count);
      }
      break;
    }
  }
  if (ret < 0)
  {
    efs_close(fd);
    free(info);
    return EFAILURE;
  }
  efs_close(fd);
  free(info);
  return ESUCCESS;
}

int mqtt_ssl_cert_load(MQTT_CONN_PTR conn, MQTT_CFG_PTR cfg)
{
  int result = QAPI_ERROR;

  if(conn == NULL)
    return result;

  do
  {
    if(conn->cfg.security_mode != QAPI_MQTT_NO_SECURITY_MODE)
    {
      if(conn->sslctx == QAPI_NET_SSL_INVALID_HANDLE)
      {
        conn->sslctx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
        if(conn->sslctx == QAPI_NET_SSL_INVALID_HANDLE) {
          MQTT_LOG_INFO("mqtt_ssl_cert_load: qapi_Net_SSL_Obj_New Invalid Handle");
          break;
        }
      }
      switch(conn->cfg.security_mode)
      {
        case QAPI_MQTT_PSK:
          if(cfg != NULL)
          {
            if((!QAPI_MQTT_IS_OPTION(cfg, QAPI_NET_MQTT_EXTENDED_CONFIG_PSK_TABLE_T)) ||
               (cfg->psk == NULL))
            {
              MQTT_LOG_DEBUG("mqtt_ssl_cert_load: psk mode is not set or psk passed in MQTT cfg is NULL");
              break;
            }
            conn->cfg.psk = cfg->psk;
          }
          if(conn->cfg.psk != NULL)
          {
            result = qapi_Net_SSL_Cert_Load(conn->sslctx, QAPI_NET_SSL_PSK_TABLE_E,
                                            (char *)conn->cfg.psk);
            if(result < QAPI_OK)
            {
              ds_iot_log_strf_err("mqtt_ssl_cert_load: Unable to load %s", (char *)conn->cfg.psk);
              break;
            }
          }
          break;
        case QAPI_MQTT_CERTIFICATE:
          /* ca_list is mandatory for server authentication */
          if((cfg == NULL && conn->cfg.ca_list == NULL) ||(cfg != NULL && cfg->ca_list == NULL))
          {
            MQTT_LOG_DEBUG("mqtt_ssl_cert_load: ca_list passed in MQTT cfg is NULL");
            break;
          }
          else if(cfg != NULL)
          {
            conn->cfg.ca_list = cfg->ca_list;
          }
          if(conn->cfg.ca_list != NULL)
          {
            result = qapi_Net_SSL_Cert_Load(conn->sslctx, QAPI_NET_SSL_CA_LIST_E,
                                            (char *)conn->cfg.ca_list);
            if(result < QAPI_OK) {
              ds_iot_log_strf_err("mqtt_ssl_cert_load: Unable to load %s", (char *)conn->cfg.ca_list);
              break;
            }
          }

          /* certificate is optional */
          if(cfg != NULL && cfg->cert != NULL)
            conn->cfg.cert = cfg->cert;

          if(conn->cfg.cert != NULL)
          {
            result = qapi_Net_SSL_Cert_Load(conn->sslctx, QAPI_NET_SSL_CERTIFICATE_E,
                                            (char *)conn->cfg.cert);
            if(result < QAPI_OK) {
              ds_iot_log_strf_err("mqtt_ssl_cert_load: Unable to load %s", (char *)conn->cfg.cert);
              break;
            }
          }
          else
            result = QAPI_OK;
          break;
        default:
          MQTT_LOG_DEBUG("mqtt_ssl_cert_load: no valid security mode is set.");
          break;
      }
    }
  }while(0);

  return result;
}


int mqtt_start_ssl_connection(MQTT_CONN_PTR conn)
{
  int result = QAPI_ERROR;
  do
  {
    if(conn == NULL)
      break;

    if(conn->ssl == QAPI_NET_SSL_INVALID_HANDLE)
    {
      conn->ssl = qapi_Net_SSL_Con_New(conn->sslctx, QAPI_NET_SSL_TLS_E);
      if(conn->ssl == QAPI_NET_SSL_INVALID_HANDLE)
      {
        MQTT_LOG_DEBUG("mqtt_start_ssl_connection: qapi_Net_SSL_Con_New Fail");
		conn->cfg.last_error = QAPI_NET_SSL_INVALID_HANDLE;
	    conn->cfg.module_cat = QAPI_NET_SSL_CAT;
        break;
      }
      result = qapi_Net_SSL_Configure(conn->ssl, &(conn->cfg.ssl_cfg));
      if( result < QAPI_OK)
      {
        /* put the log message  */
	    conn->cfg.last_error = result;
	    conn->cfg.module_cat = QAPI_NET_SSL_CAT;
        MQTT_LOG_DEBUG("mqtt_start_ssl_connection: qapi_Net_SSL_Configure Fail");
        break;
      }
    }

    result = qapi_Net_SSL_Fd_Set(conn->ssl, conn->sock);
    if(result < QAPI_OK) {
      conn->cfg.last_error = result;
	  conn->cfg.module_cat = QAPI_NET_SSL_CAT;
      MQTT_LOG_INFO("mqtt_start_ssl_connection: qapi_Net_SSL_Fd_Set Fail");
      break;
    }

    // SSL Handshake with server
    result = qapi_Net_SSL_Connect(conn->ssl);
    if(result < QAPI_OK) {
      switch(result) {
        case QAPI_SSL_OK_HS:
          conn->state = MQTT_ST_SSL_CONNECTED;
          MQTT_LOG_INFO1("mqtt_start_ssl_connection: conn %p "
                "SSL_CONNECTING -> SSL_CONNECTED", conn);
          break;
        case QAPI_ERR_SSL_CERT_CN:
          MQTT_LOG_INFO("The peer's SSL cert is trusted,"
               "And CN matches the host name, But time is expired");
		  conn->cfg.last_error = result;
	      conn->cfg.module_cat = QAPI_NET_SSL_CAT;
          break;
        case QAPI_ERR_SSL_CERT_TIME:
          MQTT_LOG_INFO("The peer's SSL cert is trusted,"
               "But CN does NOT match the host name, And time is valid");
		  conn->cfg.last_error = result;
	      conn->cfg.module_cat = QAPI_NET_SSL_CAT;
          break;
    
        case QAPI_ERR_SSL_CERT_NONE:
          MQTT_LOG_INFO("The peer's SSL cert is NOT trusted");
		  conn->cfg.last_error = result;
	      conn->cfg.module_cat = QAPI_NET_SSL_CAT;
          break;
        default:
          MQTT_LOG_ERR("mqtt_start_ssl_connection: failure");
		  conn->cfg.last_error = result;
	      conn->cfg.module_cat = QAPI_NET_SSL_CAT;
          break;
      }
      break;
    }
  }while(0);
  return result;
}

#endif /* MQTT_CLIENT */
