/*!
  @file
  http_client.h

  @brief
  HTTP client service provides a collection of API functions that allow 
  the application to enable and configure HTTP client services. The HTTP 
  client can be configured to support IPv4, IPv6, as well as HTTP mode, 
  HTTPS mode (secure) or both. 
*/

/*===========================================================================

  Copyright (c) 2008-2023 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25Jan2017 vpulimi  Created the module. First version of the file.
===========================================================================*/
#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_    1

#include "nsport.h"
#include "qapi_httpc.h"

#define HTTPCLIENT_MAX_TASK_PRIORITY  110 
#define HTTPCLIENT_MAX_TASK_SIZE     (1024 * 8)

#define HTTPCLIENT_DEFAULT_CON_SUPPORT   04 /* Maximum HTTP sessions allowed */

/* Blocks the response thread for any data on the sockets  */ 
#define HTTPC_RESPONSE_WAIT_MS           200
/* Transitions the response thread to sleep state allowing any OEM requests to be performed */ 
#define HTTPC_CHECK_INTERVAL_MS          05
/* Clean-up stale sessions */
#define HTTPC_TIMER_TIMEOUT              NS_GET_TICKS_MSEC(500) /* 500ms */
/* Tick counter (since start) */
#define HTTPCTICKS                       NS_NOW() 
/* Minimum default response thread timeout interval */
#define HTTPC_MIN_DEFAULT_RX_TIMEOUT_MS  3000
/* Default data length to be sent per transaction */
#define HTTPC_DEFAULT_SEND_CHUNK_SIZE    2000
/* Default retries for send data chunk failures */ 
#define HTTPC_DEFAULT_SEND_CHUNK_RETRIES   10
/* Default delay between send data chunks */
#define HTTPC_DEFAULT_SEND_CHUNK_DELAY    400
/* Default connect poll count */
#define HTTPC_DEFAULT_CONNECT_POLL_CNT     15
/* Default connect poll interval */
#define HTTPC_DEFAULT_CONNECT_POLL_INTERVAL_MS   500
/* DNS resolve interface name size */
#define HTTPC_DNS_RESOLVE_IFACE_NAME       30

#define HTTPC_ALLOC_FAIL 5
#define HTTPC_INVALID_HANDLE (-1)

#define MAX_DATA_BUFFER_FOR_CHUNK_LEN            16
#define HTTPCLIENT_MAX_PAGE_SIZE                 8000
#define HTTPCLIENT_MAX_HEADER_SIZE               350
#define HTTPCLIENT_MAX_BUFFER_SIZE              (HTTPCLIENT_MAX_PAGE_SIZE + HTTPCLIENT_MAX_HEADER_SIZE)
#define HTTPCLIENT_ADDITIONAL_URL_LENGTH         256
#define HTTPCLIENT_MAX_HOST_LENGTH               256
#define HTTPCLIENT_MAX_DATA_LENGTH               128
#define HTTPCLIENT_MAX_LOCATION_LENGTH           64
#define HTTPCLIENT_DEFAULT_REMAIN_BUFFER_LENGTH  1024

/* HTTP client response codes */
#define HTTP_RESPONSE_CODE_MIN    (100)
#define HTTP_RESPONSE_CODE_MAX    (999)

/* HTTP client flags */
#define HCF_CLOSED_BY_PEER         0x0001
#define HCF_RECV_HEADER            0x0002
#define HCF_CHUNKED                0x0004

/* HTTP event mask */
#define HTTPC_PENDING_RQST_EVENT   0x0001

/* HTTP client states */
typedef enum 
{
  HCS_INIT = 1,
  HCS_CONNECT_IN_PROG,
  HCS_CONNECTED,
  HCS_SENTREQ,
  HCS_RECVRESP,
  HCS_ENDING
} HTTPCLIENT_STATE;

/* HTTP client request methods */
typedef enum 
{
  HTTP_CLIENT_GET_CMD = 1,  
  HTTP_CLIENT_POST_CMD,
  HTTP_CLIENT_PUT_CMD,
  HTTP_CLIENT_PATCH_CMD,
  HTTP_CLIENT_HEAD_CMD,
  HTTP_CLIENT_CONNECT_CMD,
  HTTP_CLIENT_DELETE_CMD
} HTTPC_REQUEST_CMD;

/* HTTP client handle */
typedef uint32_t httpc_hdl_t;

/* HTTP client configuration */
typedef qapi_Net_HTTPc_Config_t HTTPc_CFG;
typedef qapi_Net_HTTPc_Extended_Config_Options_e HTTPc_EXT_CFG_OPTION;

/* HTTP client response */
typedef struct http_rsp_cont
{
  uint32_t  totalLen;
  uint32_t  chunk_rx_flag;
  uint8_t   data[1];
}HTTP_RSP_CONT;

/* HTTP client callback response */
typedef struct 
{
  uint32_t     length;
  uint32_t     resp_code;
  const void  *data;
  const void  *rsp_hdr;
  uint32_t     rsp_hdr_len;
} http_client_cb_resp_t;

/* HTTP client user callback */
typedef void (*http_client_cb_t)(void* arg, int32_t state, void* http_resp);

/* HTTP client response state */
typedef enum
{
  HTTPC_RX_ERROR_SERVER_CLOSED = -8, /* Will reset session and reconnect before next request */
  HTTPC_RX_ERROR_RX_PROCESS = -7,
  HTTPC_RX_ERROR_RX_HTTP_HEADER = -6,
  HTTPC_RX_ERROR_INVALID_RESPONSECODE = -5,
  HTTPC_RX_ERROR_CLIENT_TIMEOUT = -4,
  HTTPC_RX_ERROR_NO_BUFFER = -3,
  HTTPC_RX_CONNECTION_CLOSED = -2,
  HTTPC_RX_ERROR_CONNECTION_CLOSED = -1,
  HTTPC_RX_FINISHED = 0,
  HTTPC_RX_MORE_DATA = 1,
} http_client_rx_cb_state;

/* HTTP client error codes */
typedef enum
{
  HTTPC_ERR_SSL_CFG = -10,
  HTTPC_ERR_SOCKET_OPEN = -9,
  HTTPC_ERR_SSL_CONN = -8,
  HTTPC_ERR_INVALID_PARAM = -7,
  HTTPC_ERR_BUSY = -6,
  HTTPC_ERR_NO_MEMORY = -5,
  HTTPC_ERR_SEND = -4,
  HTTPC_ERR_CONN = -3,
  HTTPC_ERR_NONE_SESS = -2,
  HTTPC_ERROR = -1,
  HTTPC_OK = 0,
} http_client_error_code_e;

typedef struct http_client_rx_cb_s
{
  uint8_t pending_req;
  int32_t state;
  uint32_t resp_code;
} rx_cb_t;

/* HTTP client session */
typedef struct httpclient_sess_s 
{
  uint32_t            id; 
  uint8_t             index;
  uint32_t            module_uid;
  uint32_t            module_instance;
  /* Session configuration information */
  uint8_t             dns_resolve_iface_name[HTTPC_DNS_RESOLVE_IFACE_NAME];
  struct ip46addr     bind_iface_address;
  HTTPc_CFG          *httpc_cfg;
  /* Connection related information */
  HTTPCLIENT_STATE    hcs_state;
  int32_t             hcs_socket;
  uint32_t            hcs_flags;
  struct ip46addr     hcs_addr;
  uint32_t            hcs_port;
  uint8_t             hcs_host[HTTPCLIENT_MAX_HOST_LENGTH];
  uint8_t             hcs_hostisipv6addr;
  uint32_t            hcs_command;  
  uint32_t            timeout;
  uint64_t            hcs_lasttime;
  uint8_t             proxy;
  uint8_t             secure_proxy;
  uint8_t             disconnect_req;
  /* SSL related information */  
  SSL_CON             ssl_conn;
  SSL_CTX             sslCtx;
  SSL_CFG            *ssl_cfg; 
  /* HTTP request related information */
  uint32_t            hcs_contentlength;
  uint32_t            hcs_headersize;
  uint8_t            *hcs_headerstart;
  uint32_t            hcs_rxbufoffset;
  uint8_t            *hcs_buffer;
  uint32_t            hcs_bufoffset;
  uint32_t            hcs_buf_len;
  uint8_t            *hcs_headerbuffer;    
  uint32_t            hcs_headerbufoffset;
  uint32_t            hcs_headerbuf_len;
  /* HTTP response related information */  
  uint8_t            *hcs_rxbuffer;
  uint32_t            hcs_rxbuf_len;  
  int32_t             chunked_len;
  uint32_t            offset;
  uint8_t             data_buf[MAX_DATA_BUFFER_FOR_CHUNK_LEN];
  uint32_t            data_buf_len;
  rx_cb_t             cb;
  http_client_cb_t    http_client_cb;
  void               *cb_arg;
  HTTP_RSP_CONT      *http_rsp_cont;
}httpclient_sess;

/* HTTP client context */
typedef struct 
{
  uint8_t   valid; 
  uint8_t   ref_count;  
  uint64_t  timer_lasttime;
  NS_EVENT  rx_event;
  uint32_t *httpc_sess[HTTPCLIENT_DEFAULT_CON_SUPPORT];  
} httpc_ctxt_t;

/* Global declarations */
extern httpc_ctxt_t g_httpc_ctxt;
extern NS_MUTEX  g_httpc_mutex;
extern NS_MUTEX  g_httpc_session_mutex[HTTPCLIENT_DEFAULT_CON_SUPPORT];     


/* Utility Macros */
#define HTTPC_MOVETOSTATE(sess, state)  (sess)->hcs_state = (state)
#define HTTPC_SET_FLAG(sess, flag)      (sess)->hcs_flags |= (flag)
#define HTTPC_RESET_FLAG(sess, flag)    (sess)->hcs_flags &= ~(flag)
#define HTTPC_ISFLAGSET(sess, flag)     (((sess)->hcs_flags & (flag))?TRUE:FALSE)
#define HTTPC_ISFLAGRESET(sess, flag)   (!((sess)->hcs_flags & (flag))?TRUE:FALSE)
#define HTTPC_IS_SESSION_VALID(sess)    ((sess->index < HTTPCLIENT_DEFAULT_CON_SUPPORT) && (sess == (httpclient_sess *)g_httpc_ctxt.httpc_sess[sess->index]))

#define rsp_cont(sess) (sess->http_rsp_cont)
#define chunk_len(sess) (sess->chunked_len)
#define sess_buffer(sess) (sess->hcs_buffer)
#define sess_bufoffset(sess) (sess->hcs_bufoffset)
#define sess_buflen(sess) (sess->hcs_buf_len)
#define sess_rxbuffer(sess) (sess->hcs_rxbuffer)
#define sess_rxbuflen(sess) (sess->hcs_rxbuf_len)
#define sess_rxbufoffset(sess) (sess->hcs_rxbufoffset)
#define sess_headerbuffer(sess) (sess->hcs_headerbuffer)
#define sess_headerbufoffset(sess) (sess->hcs_headerbufoffset)
#define sess_headerbuflen(sess) (sess->hcs_headerbuf_len)
#define sess_sslcfg(sess) (sess->ssl_cfg)
#define sess_httpcfg(sess) (sess->httpc_cfg)
#define sess_sslconn(sess) (sess->ssl_conn)



/* HTTP client APIs */
/**
 * @brief Initialize HTTP client module
 *
 * @details Function is invoked to initialize the HTTP client module. Need to 
 *          called prior to starting the HTTP client. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
int32_t http_client_init(void);

/**
 * @brief Start/Re-start HTTP client module
 *
 * @details Function is invoked to start or restart the HTTP client after it is
 *          stopped via call to qapi_Net_HTTPc_Stop().
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_start(void);

/**
 * @brief Stop HTTP client module
 *
 * @details Function is invoked to stop the HTTP client after it was started via 
            call to qapi_Net_HTTPc_Start().
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_stop(void);

/**
 * @brief Creates new HTTP client session 
 *
 * @details In order to create a client session the caller needs to invoke this function 
 *          and the handle to the newly created context is returned if success. As part 
 *          of the function call, an user callback function is registered with the HTTP 
 *          client module that gets invoked for that particular session if there is some
 *          response data from the HTTP server. Passing in the SSL context information
 *          would ensure that a secure session would get created.           
 *
 * @param[in] timeout   Timeout(ms) on session method (zero is not recommended)
 * @param[in] ssl_ctx   SSL context for HTTPs connect (zero for no HTTPs session support)
 * @param[in] callback  Register callback function, NULL for not support callback
 * @param[in] arg       User data payload to be returned by the callback function
 * @param[in] httpc_max_body_length    Max body length on this session
 * @param[in] httpc_max_header_length  Max header length on this session
 *
 * @return On success, qapi_Net_HTTPc_handle_t is returned. NULL otherwise.
 */
extern httpclient_sess* http_client_newsess(uint32_t timeout, uint32_t ssl_ctx, http_client_cb_t callback, void* arg, uint32_t httpc_max_body_length, uint32_t httpc_max_header_length);

/**
 * @brief Release HTTP client session
 *
 * @details HTTP client session that is connected to the HTTP server is disconnected prior 
            to releasing the resources associated with that session. 
 *
 * @param[in] sess         Handle to the HTTP client session
 * @param[in] lock_state   Whether to acquire mutex or not 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_freesess(httpclient_sess *sess, boolean lock_state);

/**
 * @brief Configure HTTP client session 
 *
 * @details HTTP client session configuration information to be configured. 
 *
 * @param[in] sess   Handle to the HTTP client session
 * @param[in] cfg    HTTP client session configuration 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_configure(httpclient_sess *sess, HTTPc_CFG *cfg);

/**
 * @brief Extended HTTP client session configuration  
 *
 * @details Additional HTTP client session configuration information to be configured. 
 *
 * @param[in] handle        Handle to the HTTP client session.
 * @param[in] option        HTTP client extended configuration option. 
 * @param[in] option_value  HTTP client extended configuration option info.
 * @param[in] option_size   HTTP client extended configuration option size. 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_extended_configure(httpclient_sess *sess, HTTPc_EXT_CFG_OPTION option, void *option_value, uint32_t option_size);

/**
 * @brief Configure HTTP client SSL session 
 *
 * @details HTTP client SSL session configuration information to be configured. 
 *
 * @param[in] sess   Handle to the HTTP client session
 * @param[in] cfg    HTTP client SSL session configuration 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_sslconfigure(httpclient_sess *sess, SSL_CFG *cfg);

/**
 * @brief Connect HTTP client session to the HTTP server
 *
 * @details HTTP client session is connected to the HTTP server in blocking mode. 
 *
 * @param[in] sess        Handle to the HTTP client session
 * @param[in] URL         Server URL informtion  
 * @param[in] port        Server port information 
 * @param[in] lock_state  Whether to acquire mutex or not 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_connect(httpclient_sess *sess, const char *server, uint16_t port, boolean lock_state);

/**
 * @brief Connect HTTP client session to the HTTP proxy server
 *
 * @details HTTP client session is connected to the HTTP proxy server in blocking mode. 
 *
 * @param[in] sess          Handle to the HTTP client session
 * @param[in] URL           Server URL informtion  
 * @param[in] port          Server port information 
 * @param[in] secure_proxy  Secure proxy connection 
 * @param[in] lock_state    Whether to acquire mutex or not 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_proxy_connect(httpclient_sess *sess, const char *server, uint16_t port, uint8_t secure_proxy, boolean lock_state);

/**
 * @brief Disconnect HTTP client session from the HTTP server
 *
 * @details HTTP client session that is connected to the HTTP server is disconnected 
            from the HTTP server. 
 *
 * @param[in] sess        Handle to the HTTP client session
 * @param[in] lock_state  Whether to acquire mutex or not 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_disconnect(httpclient_sess *sess, boolean lock_state);

/**
 * @brief Process the HTTP client session requests
 *
 * @details HTTP client session requests are processed and sent to the HTTP server. 
 *
 * @param[in] sess       Handle to the HTTP client session
 * @param[in] cmd        HTTP request method information 
 * @param[in] url_info   Server URL information
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_request(httpclient_sess *sess, HTTPC_REQUEST_CMD cmd, const char *url_info);

/**
 * @brief Set HTTP client session body
 *
 * @details Multiple invocations of this function would result in overwriting the 
            internal data buffer with content of the last call.  
 *
 * @param[in] sess      Handle to the HTTP client session
 * @param[in] body      HTTP body related information buffer
 * @param[in] bodylen   HTTP body buffer length
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_set_body(httpclient_sess* sess, const char *body, uint32_t bodylen);

/**
 * @brief Set HTTP client session parameter
 *
 * @details Multiple invocations of this function would result in appending the 
            parameter key-value pair information to the internal data buffer. 
 *
 * @param[in] sess    Handle to the HTTP client session
 * @param[in] key     HTTP key related inforamtion 
 * @param[in] value   HTTP value associated with the key 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_set_param(httpclient_sess* sess, const char *key,const char *value);

/**
 * @brief Add HTTP client session header field
 *
 * @details Multiple invocations of this function would result in appending the 
            header type-value pair information to the internal header buffer. 
 *
 * @param[in] sess    Handle to the HTTP client session
 * @param[in] type    HTTP header type related inforamtion 
 * @param[in] value   HTTP value associated with the header type 
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_add_header_field(httpclient_sess* sess, const char *header,const char *value);

/**
 * @brief Clear HTTP client session header
 *
 * @details Invocation to this function would clear the entire content associated 
            with the internal header buffer. 
 *
 * @param[in] sess    Handle to the HTTP client session
 *
 * @return On success, 0 is returned. Other value on error.
 */
extern int32_t http_client_clear_header(httpclient_sess* sess);
 
#endif /* _HTTP_CLIENT_H_ */

