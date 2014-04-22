/*
 * v4.1.testing mqtt/mqttc.h
 * May 26, 2016
 * Distributed per InterNiche ref US2611-2
 */
/*
 * FILENAME: mqttc.h
 *
 * Copyright  2015 By InterNiche Technologies Inc. All rights reserved.
 *
 *
 * MODULE: MQTT
 *
 *
 * PORTABLE: yes
 */
#include "qapi_mqtt.h"
#include "qapi_net_status.h"
#include "ds_log.h"

#ifdef MQTT_CLIENT_SECURITY
#include "qapi_ssl.h" 
#endif

#define MAX_CLIENT_ID_LEN             QAPI_NET_MQTT_MAX_CLIENT_ID_LEN
#define MQTT_PUBACK_TIMEOUT           qurt_timer_convert_time_to_ticks(3, QURT_TIME_SEC)
#define MQTT_PUB_RETRANSMIT_THRESHOLD 3
#define MQTT_PORT                     1883
#define MQTT_SECURE_PORT              8883

/* Default data length to be sent per transaction */
#define MQTT_DEFAULT_SEND_CHUNK_SIZE    2000
/* Default retries for send data chunk failures */ 
#define MQTT_DEFAULT_SEND_CHUNK_RETRIES   10
/* Default delay between send data chunks */
#define MQTT_DEFAULT_SEND_CHUNK_DELAY    400


enum MQTT_CONN_RX_state
{
   MQTT_RX_ST_WAITING_FOR_MSG_PART1,
   MQTT_RX_ST_WAITING_FOR_MSG_PART2,
   MQTT_RX_ST_MSG_COMPLETE,
};

typedef enum MQTT_CONN_RX_state MQTT_CONN_RX_STATE;

struct MQTT_TxCB
{
   struct MQTT_TxCB *next;
   enum msgTypes response;
   uint8_t *buf;
   int length;
   int offset; /* used when only part of the data was written into the socket */
   uint32_t flags;
   uint16_t id;
   uint16_t pub_retransmit_count;
   int qos;
   uint32_t created;
   void *app_id; /* XXX add as parameter to MQTT client APIs */
   void (*callback)(struct MQTT_TxCB *); /* XXX add as parameter to MQTT client APIs */
}; 

typedef struct MQTT_TxCB MQTT_TxCB;
typedef struct MQTT_TxCB *MQTT_TxCB_PTR;

/* XXX does Tx CB require a lifetime? */
#define MQTT_TX_CB_LIFETIME (60*TPS)
#define MQTT_CONN_LIFETIME (60000*TPS)


/* XXX rename - eliminate Subscribe */
struct MQTT_SubscribeRxOp
{
   uint8_t *buf;
   int length;
   int offset; /* used when only part of the data was read out from the socket */
   uint8_t *vhdr; /* start of MQTT variable header */
   int rem_len;
   int id;
   int qos;
   uint32_t expiration;
   int state;
};

/* forward declaration */
struct MQTT_connection;

/* callback function that is invoked when a MQTT subscription request is 
 * granted or denied, or when a message (based on a prior subscription) 
 * is received from the broker 
 */
typedef void (*MQTT_SUBSCRIPTION_CB)(struct MQTT_connection *conn, int reason,
                                     uint8_t *topic, int topic_length,
		                     uint8_t *msg, int msg_length,
                                     int qos, void *sid);

#define MQTT_MAX_DUP_SUBSCRIPTIONS 1
#define MQTT_GRANTED_QOS_OUTSTANDING -1
#define MQTT_PERSISTED_QOS_OUTSTANDING -2


struct MQTT_subscription
{
   uint8_t *topic;
   int length;
   int requested_qos;
   int granted_qos;
   void *sid[MQTT_MAX_DUP_SUBSCRIPTIONS + 1];
   MQTT_SUBSCRIPTION_CB callback[MQTT_MAX_DUP_SUBSCRIPTIONS + 1];
   int status[MQTT_MAX_DUP_SUBSCRIPTIONS + 1];
   int sid_count;
};

struct MQTT_publish
{
   uint8_t *topic;
   int topic_length;
   int dup;
   int qos;
   int retained;
   uint16_t id;
   uint8_t *payload;
   int payload_length;
};

typedef struct MQTT_publish MQTT_PUBLISH;
typedef struct MQTT_publish *MQTT_PUBLISH_PTR;

struct MQTT_SubscribeRxCB
{
   struct MQTT_SubscribeRxCB *next;
   uint16_t id; /* MQTT Packet Identifier */
   int count;
   struct MQTT_subscription sub[1];
};

typedef struct MQTT_SubscribeRxCB MQTT_SubRxCB;
typedef struct MQTT_SubscribeRxCB *MQTT_SubRxCB_PTR;



struct MQTT_restore
{
  struct MQTT_TxCB *txq; /* list of pending transmit transactions (conn, publish) */
  struct MQTT_SubscribeRxCB *subq; /* list of current subscriptions */
  struct MQTT_SubscribeRxOp rx; /* currently active rx transaction */
};

typedef struct MQTT_restore MQTT_RESTORE;
typedef struct MQTT_restore *MQTT_RESTORE_PTR;

/* reason codes for subscription callback */
enum MQTT_SUBSCRIPTION_CBK_MSG
{
   MQTT_SUBSCRIPTION_DENIED, /* subscription denied by broker */
   MQTT_SUBSCRIPTION_GRANTED, /* subscription granted by broker */
   MQTT_SUBSCRIPTION_MSG /* message received from broker */
};

typedef enum MQTT_SUBSCRIPTION_CBK_MSG MQTT_SUBSCRIPTION_CBK_MSG;

/* reason codes for connect callback */
enum MQTT_CONNECT_CBK_MSG
{
   MQTT_MQTT_CONNECT_SUCCEEDED, /* MQTT connect succeeded */
   MQTT_TCP_CONNECT_FAILED, /* TCP connect failed */
   MQTT_SSL_CONNECT_FAILED, /* SSL connect failed */
   MQTT_MQTT_CONNECT_FAILED, /**< MQTT connect failed. */
   MQTT_CONNECT_PROTOCOL_VER_ERROR, /**< MQTT connect failed-unacceptable protocol version. */
   MQTT_CONNECT_IDENTIFIER_ERROR,/**< MQTT connect failed-Identifire rejected. */
   MQTT_CONNECT_SERVER_UNAVAILABLE,/**< MQTT connect failed-Server unavailable. */
   MQTT_CONNECT_MALFORMED_DATA,/**< MQTT connect failed-Data in user name or password is malformed. */
   MQTT_CONNECT_UNAUTHORIZED_CLIENT /**< MQTT connect failed-Client is not authorized to connect. */
};

typedef enum MQTT_CONNECT_CBK_MSG MQTT_CONNECT_CBK_MSG;

typedef void (*MQTT_CONNECT_CB)(struct MQTT_connection *conn, int reason);

typedef void (*MQTT_PUBLISH_CB)(struct MQTT_connection *conn,
                                   enum msgTypes msgtype,
                                   int qos,
                                   uint16_t msg_id);


/* several fields (e.g., keepalive_duration, clean_session, etc.) are 
 * only valid for initial session, and are ignored for all connect 
 * requests that (re)use existing session
 * XXX alignment issues
 */
struct MQTT_config
{
   struct sockaddr local;
   struct sockaddr remote;
   bool_t nonblocking_connect;
   uint8_t client_id[MAX_CLIENT_ID_LEN];
   int client_id_len;
   MQTT_CONNECT_CB callback;
   /* zero = disable keepalive */
   uint32_t keepalive_duration;
   uint8_t clean_session;
   uint8_t *will_topic;
   int will_topic_len;
   uint8_t *will_message;
   int will_message_len;
   uint8_t will_retained;
   uint8_t will_qos;
   uint8_t *username;
   int username_len;
   uint8_t *password;
   int password_len;
   bool_t allow_unsub_pub;
   uint32_t connack_timed_out_sec;
   uint32_t sock_options_cnt;
   qapi_Net_MQTT_Sock_Opts_t *sock_options;
   uint8_t options[QAPI_NET_MQTT_EXTENDED_CONFIG_MAX / QAPI_MQTT_OPTION_MAP_SIZE + 1]; /* Bitmap to check if option is set */
   qapi_Net_Mqtt_Sec_Mode security_mode;
   int pass_msg_hdr;
   uint16_t max_chunk;
   /**< Maximum send data chunk per transaction. */
   uint16_t max_chunk_delay_ms;
   /**< Maximum delay between send data chunks (msec). */
   uint8_t max_chunk_retries; 
   /**< Maximum send data chunk retries. */
   int last_error;
   qapi_Net_Error_Category_t module_cat;
   /* zero = default timeout will be assigned. 5 sec */
   uint8_t pub_ack_timeout;
#ifdef MQTT_CLIENT_SECURITY
   qapi_Net_SSL_Config_t ssl_cfg;
   qapi_Net_SSL_CAList_t ca_list;
   qapi_Net_SSL_Cert_t cert;
   qapi_Net_SSL_PSKTable_t psk; /**< MQTT security mode. */
#endif
};

typedef struct MQTT_config MQTT_CFG;
typedef struct MQTT_config *MQTT_CFG_PTR;

enum MQTT_conn_state
{
   MQTT_ST_DORMANT,
   MQTT_ST_TCP_CONNECTING,
   MQTT_ST_TCP_CONNECTED,
   MQTT_ST_SSL_CONNECTING,
   MQTT_ST_SSL_CONNECTED,
   MQTT_ST_MQTT_CONNECTING,
   MQTT_ST_MQTT_CONNECTED,
   MQTT_ST_MQTT_TERMINATING,
   MQTT_ST_SSL_TERMINATING,
   MQTT_ST_TCP_TERMINATING,
   MQTT_ST_DYING,
   MQTT_ST_DEAD
};

typedef enum MQTT_conn_state MQTT_CONN_STATE;

/* number of consecutive PINGREQs that have not been responded to before
 * connection is marked as unusable
 */
#define MQTT_PINGRESP_FAILURE_THRESHOLD 2

/* flag bit indicating that a MQTT session has been fully established (i.e.,
 * a CONNECT/CONNACK exchange has occurred).
 */
#define MQTT_FL_SESS_ESTAB          (0x1 << 0)
#define MQTT_FL_INFORM_SESS_BROKEN  (0x1 << 1)
#define MQTT_FL_TXCB_RESTORED       (0x1 << 2)
#define MQTT_FL_RESTORE_FAILED      (0x1 << 3)

/* XXX add a MQTT_CFG data structure to MQTT_CONN? */
struct MQTT_connection
{
   SOCKTYPE sock;
   MQTT_CONN_STATE state;
   int refcount;
   uint32_t flags;
   MQTT_RESTORE_PTR restore;
   MQTT_CONNECT_CB callback; /* callback notifying application of completion of connection process or XXX */
   MQTT_PUBLISH_CB pub_cb;/*Callback notifying application of Publish status*/
   struct MQTT_TxCB *txq; /* list of pending transmit transactions (conn, publish) */
   struct MQTT_SubscribeRxCB *subq; /* list of current subscriptions */
   struct MQTT_SubscribeRxOp rx; /* currently active rx transaction */
   struct MQTT_connection *prev;
   struct MQTT_connection *next;
   bool_t pingresp_pending;
#ifndef SUPERLOOP
   IN_MUTEX mutex;
#endif
   uint64_t last_tx;
   MQTT_CFG cfg;
#ifdef MQTT_CLIENT_SECURITY
   qapi_Net_SSL_Obj_Hdl_t sslctx; /* ssl context index */
   qapi_Net_SSL_Con_Hdl_t ssl;  /* ssl  connection index */
#endif
};

typedef struct MQTT_connection MQTT_CONN; 
typedef struct MQTT_connection *MQTT_CONN_PTR;



/* list of MQTT connection */
extern struct MQTT_connection *MQTT_connlist;

enum MQTT_subscription_check_result
{
   MQTT_SUBSCRIPTION_UNKNOWN_STATUS,
   MQTT_CREATE_NEW_SUBSCRIPTION,
   MQTT_ADD_TO_EXISTING_SUBSCRIPTION,
   MQTT_REJECTED_SUBSCRIPTION_DUP_SID,
   MQTT_REJECTED_SUBSCRIPTION_TBL_FULL,
   MQTT_REJECTED_SUBSCRIPTION_HIGHER_QOS,
   MQTT_REJECTED_SUBSCRIPTION_REQ_OUTSTANDING,
   MQTT_REJECTED_UTF8_VALIDATION_FAILED,
   MQTT_SUBSCRIPTION_BAD_PARAM
};

typedef enum MQTT_subscription_check_result MQTT_CHK_SUB_RESULT;

enum MQTT_subscription_status
{
   MQTT_SUBSCRIPTION_STATUS_NONEXISTENT, /* slot is empty (e.g., at creation or when client application has unsubscribed) */
   MQTT_SUBSCRIPTION_STATUS_TEMPORARY, /* interim state during creation; subscription may be discarded later */
   MQTT_SUBSCRIPTION_STATUS_PENDING, /* waiting for a response from server */
   MQTT_SUBSCRIPTION_STATUS_ACCEPTED, /* server has accepted subscription */
   MQTT_SUBSCRIPTION_STATUS_REJECTED /* server has accepted subscription */
};

typedef enum MQTT_subscription_status MQTT_SUBSCRIPTION_STATUS;

enum MQTT_ERR_type
{
   MQTT_ERR_NO_ERR                          = 0,
   MQTT_ERR_ALLOC_FAILURE                   = 1,
   MQTT_ERR_BAD_PARAM                       = 2,
   MQTT_ERR_BAD_STATE                       = 3,
   MQTT_ERR_CONN_CLOSED                     = 4,
   MQTT_ERR_MSG_DESERIALIZATION_FAILURE     = 5,
   MQTT_ERR_MSG_SERIALIZATION_FAILURE       = 6,
   MQTT_ERR_NEGATIVE_CONNACK                = 7,
   MQTT_ERR_NO_DATA                         = 8,
   MQTT_ERR_NONZERO_REFCOUNT                = 9,
   MQTT_ERR_PINGREQ_MSG_CREATION_FAILED     = 10,
   MQTT_ERR_PUBACK_MSG_CREATION_FAILED      = 11,
   MQTT_ERR_PUBCOMP_MSG_CREATION_FAILED     = 12,
   MQTT_ERR_PUBLISH_MSG_CREATION_FAILED     = 13,
   MQTT_ERR_PUBREC_MSG_CREATION_FAILED      = 14,
   MQTT_ERR_PUBREL_MSG_CREATION_FAILED      = 15,
   MQTT_ERR_RX_INCOMPLETE                   = 16,
   MQTT_ERR_SOCKET_FATAL_ERROR              = 17,
   MQTT_ERR_TCP_BIND_FAILED                 = 18,
   MQTT_ERR_TCP_CONN_FAILED                 = 19,
   MQTT_ERR_SSL_CONN_FAILURE                = 20,
   MQTT_ERR_SUBSCRIBE_MSG_CREATION_FAILED   = 21,
   MQTT_ERR_SUBSCRIBE_UNKNOWN_TOPIC         = 22,
   MQTT_ERR_UNSUBSCRIBE_MSG_CREATION_FAILED = 23,
   MQTT_ERR_UNEXPECTED_MSG                  = 24,
   MQTT_ERR_PARTIAL_SUBSCRIPTION_FAILURE    = 25,
   MQTT_ERR_RESTORE_FAILED                  = 26,
   MQTT_ERR_REJECTED_SUBSCRIPTION_DUP_SID   = 27,
   MQTT_ERR_REJECTED_SUBSCRIPTION_HIGHER_QOS	  = 28,
   MQTT_ERR_REJECTED_SUBSCRIPTION_REQ_OUTSTANDING = 29,
   MQTT_ERR_REJECTED_UTF8_VALIDATION_FAILED  	  = 30,
   MQTT_ERR_REJECTED_SUBSCRIPTION_TBL_FULL   	  = 31,
   MQTT_ERR_SUBSCRIPTION_BAD_PARAM   			  = 32,
   MQTT_ERR_UNKNOWN = 255
};

typedef enum MQTT_ERR_type MQTT_ERR_TYPE;

#define QOS_NONE -1
#define QOS_0 0
#define QOS_1 1
#define QOS_2 2

enum MQTT_Remaining_Length
{
   MQTT_MAX_REMLEN1 = 127,
   MQTT_MAX_REMLEN2 = 16383,
   MQTT_MAX_REMLEN3 = 2097151,
   MQTT_MAX_REMLEN4 = 268435455
};

/* (maximum) length of message type, message flags, and remaining length fields */
#define MQTT_FIXED_HDR_MAX_LEN 5

#ifdef SUPERLOOP
#define MQTT_SELECT_TIMEOUT 0 /* non-blocking operation */
#else
#define MQTT_SELECT_TIMEOUT 3000 /*secs*/
#endif

/* externs - XXX move elsewhere */
extern int MQTTSerialize_connect(unsigned char* buf, int buflen, MQTTPacket_connectData* options);
extern int MQTTDeserialize_connack(unsigned char* sessionPresent, unsigned char* connack_rc, unsigned char* buf, int buflen);
extern int MQTTSerialize_publishLength(int qos, MQTTString topicName, int payloadlen);
extern int MQTTSerialize_pubrec(unsigned char* buf, int buflen, unsigned short packetid);
extern int MQTTSerialize_subscribeLength(int count, MQTTString topicFilters[]);
extern int MQTTSerialize_unsubscribeLength(int count, MQTTString topicFilters[]);
extern int MQTTSerialize_pingreq(unsigned char* buf, int buflen);
const char *MQTTPacket_getName(unsigned short packetid);

/* MQTT client public APIs */
MQTT_CONN *mqtt_connect(qapi_Net_MQTT_Hndl_t hndl, MQTT_CFG_PTR cfg, MQTT_RESTORE_PTR restore, MQTT_ERR_TYPE *status, uint8_t *session_present);
int mqtt_disconnect(MQTT_CONN_PTR conn, MQTT_RESTORE_PTR *restore, MQTT_ERR_TYPE *status);
int mqtt_publish2(MQTT_CONN_PTR conn, uint8_t *topic, int topic_len, 
                  uint8_t *msg, int msg_len, int qos, bool_t retained, 
                  MQTT_ERR_TYPE *status, MQTT_PUBLISH_CB cbk, uint16_t *msg_id);
int
mqtt_subscribe2(MQTT_CONN_PTR conn, uint8_t *topics[], int topics_len[], 
                int topics_qos[], MQTT_CHK_SUB_RESULT topics_status[], 
                int num_topics, MQTT_SUBSCRIPTION_CB cbk, void *sid, 
                MQTT_ERR_TYPE *status);
int
mqtt_unsubscribe2(MQTT_CONN_PTR conn, uint8_t *topics[], int topics_len[], 
                  int count, void *sid, MQTT_ERR_TYPE *status);
int mqtt_ping2(MQTT_CONN_PTR conn, MQTT_ERR_TYPE *status);

/* utility functions */
const char *mqtt_get_subscription_cbk_reason_name(MQTT_SUBSCRIPTION_CBK_MSG reason);

/* internal */
#if 0
MQTT_RESTORE *mqtt_conn_save_state(MQTT_CONN_PTR conn);
#endif
int mqtt_conn_restore_state(MQTT_CONN_PTR conn);
int mqtt_store_conn_context_to_persistent(MQTT_CONN_PTR conn);
int mqtt_load_restore_pointer_from_persistent(MQTT_RESTORE_PTR *restore, uint8_t *client_id);

#ifdef MQTT_CLIENT_SECURITY
extern int mqtt_ssl_cfg(void);
#endif

#ifdef MQTT_DEBUG
/* logging level */
extern int mqtt_log_level;
#endif

#define MQTT_CLIENT_ID_MAX_LEN 23


#define MQTTC_ALLOC(size) npalloc(size)
#define MQTTC_FREE(ptr) do{if(ptr != NULL)npfree(ptr);ptr=NULL;}while(0)

/* starting offset of granted QoS field */
#define MQTT_SUBACK_GRANTED_QOS_OFFSET 4

#define MQTT_VERSION 4
#define MQTT_HDR_SZ 2
#define MQTT_PUB_HDR_SZ 4
#define MQTT_MSG_TYPE_BIT (0xf0)

#define MQTT_MSG_TYPE(hdr) ((hdr[0] & MQTT_MSG_TYPE_BIT) >> 4)
#define MQTT_MSG_FLAG(hdr) (hdr[0] & 0x0f)
#define MQTT_SET_DISCONN_HDR_BITS(hdr) do{hdr[0] = 0xe0;hdr[1]=0x0;}while(0)

#ifdef MQTT_ENABLE_LOG_ERR
#define MQTT_LOG_ERR(format)  									  ds_iot_log_err(format)
#define MQTT_LOG_ERR1(format, arg1) 							  ds_iot_log_err1(format, arg1)
#define MQTT_LOG_ERR2(format, arg1, arg2) 						  ds_iot_log_err2(format, arg1, arg2)
#define MQTT_LOG_ERR3(format, arg1, arg2, arg3) 				  ds_iot_log_err3(format, arg1, arg2, arg3)
#define MQTT_LOG_ERR_STRF(format, ...)							  	  ds_iot_log_strf_err(format, ## __VA_ARGS__)
#else
#define MQTT_LOG_ERR(...) do{}while(0);
#define MQTT_LOG_ERR1(...) do{}while(0);
#define MQTT_LOG_ERR2(...) do{}while(0);
#define MQTT_LOG_ERR3(...) do{}while(0);
#define MQTT_LOG_ERR_STRF(...)	do{}while(0);
#endif
#ifdef MQTT_ENABLE_LOG_PRINT
#define MQTT_LOG_PRINT(format)  									ds_iot_log_med(format)
#define MQTT_LOG_PRINT1(format, arg1) 								ds_iot_log_med1(format, arg1)
#define MQTT_LOG_PRINT2(format, arg1, arg2) 						ds_iot_log_med2(format, arg1, arg2)
#define MQTT_LOG_PRINT3(format, arg1, arg2, arg3) 					ds_iot_log_med3(format, arg1, arg2, arg3)
#define MQTT_LOG_PRINT4(format, arg1, arg2, arg3, arg4)				ds_iot_log_med4(format, arg1, arg2, arg3, arg4)
#define MQTT_LOG_PRINT5(format, arg1, arg2, arg3, arg4, arg5)		ds_iot_log_med5(format, arg1, arg2, arg3, arg4, arg5)
#define MQTT_LOG_PRINT_STRF(format,...) 							ds_iot_log_strf_med(format, ## __VA_ARGS__)
#else
#define MQTT_LOG_PRINT(...) do{}while(0);
#define MQTT_LOG_PRINT1(...) do{}while(0);
#define MQTT_LOG_PRINT2(...) do{}while(0);
#define MQTT_LOG_PRINT3(...)  do{}while(0);
#define MQTT_LOG_PRINT4(...)  do{}while(0);
#define MQTT_LOG_PRINT5(...)  do{}while(0);
#define MQTT_LOG_PRINT_STRF(...) do{}while(0);
#endif
#ifdef MQTT_ENABLE_LOG_INFO
#define MQTT_LOG_INFO(format)  										ds_iot_log_low(format)
#define MQTT_LOG_INFO1(format, arg1) 								ds_iot_log_low1(format, arg1)
#define MQTT_LOG_INFO2(format, arg1, arg2) 							ds_iot_log_low2(format, arg1, arg2)
#define MQTT_LOG_INFO3(format, arg1, arg2, arg3) 					ds_iot_log_low3(format, arg1, arg2, arg3)
#define MQTT_LOG_INFO_STRF(format,...) 								ds_iot_log_strf_low(format, ## __VA_ARGS__)
#else
#define MQTT_LOG_INFO(...) do{}while(0);
#define MQTT_LOG_INFO1(...) do{}while(0);
#define MQTT_LOG_INFO2(...) do{}while(0);
#define MQTT_LOG_INFO3(...)  do{}while(0);
#define MQTT_LOG_INFO_STRF(...)	do{}while(0);
#endif
#ifdef MQTT_ENABLE_LOG_DEBUG
#define MQTT_LOG_DEBUG(format)  									ds_iot_log_low(format)
#define MQTT_LOG_DEBUG1(format, arg1) 								ds_iot_log_low1(format, arg1)
#define MQTT_LOG_DEBUG2(format, arg1, arg2) 						ds_iot_log_low2(format, arg1, arg2)
#define MQTT_LOG_DEBUG3(format, arg1, arg2, arg3) 					ds_iot_log_low3(format, arg1, arg2, arg3)
#define MQTT_LOG_DEBUG4(format, arg1, arg2, arg3, arg4) 			ds_iot_log_low4(format, arg1, arg2, arg3, arg4)
#define MQTT_LOG_DEBUG5(format, arg1, arg2, arg3, arg4, arg5) 		ds_iot_log_low5(format, arg1, arg2, arg3, arg4, arg5)
#define MQTT_LOG_DEBUG_STRF(format,...) 							ds_iot_log_strf_low(format, ## __VA_ARGS__)
#else
#define MQTT_LOG_DEBUG(...)	do{}while(0);
#define MQTT_LOG_DEBUG1(...) do{}while(0);
#define MQTT_LOG_DEBUG2(...) do{}while(0);
#define MQTT_LOG_DEBUG3(...) do{}while(0);
#define MQTT_LOG_DEBUG4(...) do{}while(0);
#define MQTT_LOG_DEBUG5(...) do{}while(0);
#define MQTT_LOG_DEBUG_STRF(...) do{}while(0);
#endif
#ifdef MQTT_ENABLE_LOG_MSGS
#define MQTT_LOG_MSGS(format)  										ds_iot_log_low(format)
#define MQTT_LOG_MSGS1(format, arg1) 								ds_iot_log_low1(format, arg1)
#define MQTT_LOG_MSGS2(format, arg1, arg2) 							ds_iot_log_low2(format, arg1, arg2)
#define MQTT_LOG_MSGS_STRF(format,...) 								ds_iot_log_strf_low(format, ## __VA_ARGS__)
#else
#define MQTT_LOG_MSGS(...) do{}while(0);
#define MQTT_LOG_MSGS1(...)	do{}while(0);
#define MQTT_LOG_MSGS2(...) do{}while(0);
#define MQTT_LOG_MSGS_STRF(...)	do{}while(0);
#endif
