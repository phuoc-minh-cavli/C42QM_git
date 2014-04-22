#ifndef DS_SOCKI_H
#define DS_SOCKI_H

/*===========================================================================

          S O C K E T S  I N T E R N A L  H E A D E R  F I L E

DESCRIPTION

 The Data Services Internal Socket Header File. Contains shared variables and
 enums, as well as declarations for functions for each protocol.


Copyright (c) 2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef FEATURE_DATA_PS
#ifdef FEATURE_DS_SOCKETS
#include "comdef.h"
#include "customer.h"
#include "dsm.h"
#include "ps_tcp.h"
#include "ps_udp.h"
#include "ps_utils.h"
#include "ps_in.h"
#include "ps_svc.h"
#include "ps_socket_defs.h"
#include "ds_ipfltr_defs.h"
#include "pstimer.h"
#include "ds_ipfltr.h"
#include "ps_icmp.h"
#include "ps_udp_io.h"
#include "ps_in_alias.h"
#include "dss.h"
#include "ds_fwk.h"
#include "ds_ipfltr.h"
#include "data_msg.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Value used to abstract the actual primary phys link instance and default
  flow's instance at DSS level
---------------------------------------------------------------------------*/
#define DSSOCKI_PRIMARY_PHYS_LINK_ID  0xFF
#define DSSOCKI_DEFAULT_FLOW_ID       0xFF

/*---------------------------------------------------------------------------
  Socket allocation constants.
---------------------------------------------------------------------------*/
#define SOCKAVAIL            0                   /* socket is free for use */
#define SOCKFDMAX       0x1FFF                    /* socket descriptor max */
#define APPAVAIL             0              /* application is free for use */
#define DSS_NETHANDLEMAX        0x2FFF                    /* socket descriptor max */
#define PORTBASE        (1025)                      /* ephemeral port base */
#define PORTOFFSET      (0xFFFF  - PORTBASE)      /* ephemeral port offset */

/*---------------------------------------------------------------------------
  Socket Half Open Interval
---------------------------------------------------------------------------*/
#define DSS_HALF_OPEN_INTERVAL 15000          /* Max time to ack a syn/ack */

#define DSS_PROTO_UDP_HDR 20
#define DSS_PROTO_ICMP_HDR 16


#define NETWORK 0
#define NORMAL 1
#define RESET 2
#define TIMEOUT 3


/*---------------------------------------------------------------------------
  Data types for option values. When an entry for a specified option is
  referenced from the options table, the dss_sockopt_data_type field of the
  entry reveals the option's data type.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSI_SO_BOOL            = 0,               /* boolean type               */
  DSSI_SO_INT             = 1,               /* integer type               */
  DSSI_SO_LINGER          = 2,               /* linger type                */
  DSSI_SO_SDB_ACK_CB_TYPE = 3,               /* sdb ack callback type      */
  DSSI_SO_NETPOLICY_TYPE  = 4,               /* dss_netpolicy type         */
  DSSI_SO_IFACE_ID_TYPE   = 5,               /* dss_iface_id_type          */
  DSSI_SO_IP_ADDR_TYPE    = 6,               /* m'cast membership info     */
  DSSI_SO_CB_FCN_TYPE     = 7,               /*Socket callback type        */
  DSSI_SO_MCAST_IP_MREQN_TYPE  = 8,           /* dss_ip_mreqn               */
  DSSI_SO_MCAST_IPV6_MREQN_TYPE = 9
} dssocki_sockopt_data_type;

/*---------------------------------------------------------------------------
  Encapsulation of socket option. Fields such as option name, level and
  option value type are not to be changed once initilzed.
---------------------------------------------------------------------------*/
typedef struct
{
  int optname;                            /* option name                   */
  dssocki_sockopt_data_type optvaltype;   /* option type                   */
  dss_sockopt_levels_type_e level;          /* option level                  */
  uint32 optlen;                          /* legth of the option value     */
  int initval;                            /* initial option value          */
  int minval;                             /* optval lower bound            */
  int maxval;                             /* optval upper bound            */
} dssocki_sockopt_type;

/*---------------------------------------------------------------------------
  Enum for socket state.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_NULL = 0,       /* empty state                                   */
  DSSOCKI_INIT = 1,       /* TCP socket has been created                   */
  DSSOCKI_LISTEN = 2,     /* TCP server socket listening                   */
  DSSOCKI_OPENING = 3,    /* TCP is attempting a connection                */
  DSSOCKI_OPEN = 4,       /* TCP conn established, DGRAMS open for writing */
  DSSOCKI_CLOSING = 5,    /* TCP issued a close                            */
  DSSOCKI_WAIT_FOR_CLOSE = 6, /* TCP close completed                       */
  DSSOCKI_CLOSED = 7      /* connection or network failure                 */
} dssocki_state_enum_type;

/*---------------------------------------------------------------------------
  Enum for state flags. Note that the flags are ORed together.
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_CANTSENDMORE = 0x1,  /* output operations are disallowed on sock */
  DSSOCKI_CANTRECVMORE = 0x2,   /* input operations are disallowed on sock */
  DSSOCKI_LINGER       = 0x4                    /* the socket is lingering */
} dssocki_state_flags_enum_type;

/*---------------------------------------------------------------------------
  Enum for type of caller function
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_CONNECT_ROUTINE  =  0,
  DSSOCKI_BIND_ROUTINE     =  1,
  DSSOCKI_LISTEN_ROUTINE   =  2,
  DSSOCKI_ACCEPT_ROUTINE   =  3,
  DSSOCKI_CLOSE_ROUTINE    =  4,
  DSSOCKI_INPUT_ROUTINE    =  5,
  DSSOCKI_OUTPUT_ROUTINE   =  6,
  DSSOCKI_SHUTDOWN_ROUTINE =  7
} dssocki_caller_enum_type;

/*---------------------------------------------------------------------------
  The values are what would be if 0th, 1st, 2nd etc bits are set. So next
  enum should have a value of 8
---------------------------------------------------------------------------*/
typedef enum
{
  DSSOCKI_NET_UP_EVENT_WAIT = 0x1,
  DSSOCKI_MCAST_EVENT_REG   = 0X2,
  DSSOCKI_APP_PREEMPTED     = 0x4
} dssocki_app_ctrl_flags_enum_type;


/*---------------------------------------------------------------------------
  Application control block structure.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type link;                                    /* Link to next ACB */
  dss_net_cb_fcn net_cb;                     /* network event callback fcn */
  void * net_cb_user_data;       /* User data specfied during registration */
  dss_sock_cb_fcn sock_cb;                    /* socket event callback fcn */
  void * sock_cb_user_data;      /* User data specfied during registration */
  sint15 dss_nethandle;              /* ACB's corresponding application ID */
  boolean notified_cb_flag;             /* Socket callback has been called */
  ds_fwk_s_type  * fw_inst;           /* Ptr to fwk instance for this socket   */
  q_type event_q_array[PDN_EVENT_MAX];
                                                  /* event callbacks array */
  //dss_net_policy_info_type user_netpolicy; /* User specified network policy*/
  uint16  qos_requested;       /* Number of Qos requests by the application*/
  uint16 scb_cnt;               /* Number of SCBs associated with this ACB */
  ds_net_down_reason_type  netdown_reason;    /*Reason for iface going down*/
  uint16                    flags;          /* application control flags   */
  sint15 dss_handle;                    /* DSS Handle returned by Framework*/
} acb_type;


/*---------------------------------------------------------------------------
  This is the type that is placed on the event callback queues.  It is only
  used internally and so should not be referenced outside of this file.
---------------------------------------------------------------------------*/

typedef struct
{
  q_link_type               link;
  sint15                    dss_nethandle;
  ds_fwk_s_type             *fw_inst;
  ds_pdn_ioctl_event_cb     f_ptr;
  void                      *user_info_ptr;
  ds_pdn_event_info_u_type   event_buf_info;
} dssocki_event_buf_type;

#if 0
typedef struct
{
  struct ps_in6_addr ipv6_addr;
  boolean         is_unique;
} dss_iface_ioctl_priv_ipv6_addr_ev_type;


typedef union
{
  dss_iface_ioctl_mcast_handle_type mcast_handle;
  dss_iface_ioctl_priv_ipv6_addr_ev_type priv_ipv6_addr;

} dssocki_event_buf_u_type;


typedef struct
{
  ps_iface_event_info_u_type  event_info;
  ps_iface_event_enum_type    event;
  ps_iface_type             * iface_ptr;
  ps_phys_link_type          *phys_link_ptr;
} dssocki_iface_event_info_u_type;


typedef struct
{
  q_link_type                        link;
//  ps_flow_type                     * flow_ptr;
//  dss_iface_ioctl_event_enum_type    event;
//  dss_iface_id_type                  iface_id;
  dss_qos_handle_type                qos_handle;
//  dss_iface_ioctl_event_cb           f_ptr;
  void                             * user_info_ptr;
  sint15                             dss_nethandle;
} dssocki_flow_event_buf_type;

typedef struct
{
  ps_flow_type                * flow_ptr;
  ps_iface_event_info_u_type    event_info;
  ps_iface_event_enum_type      event;
  uint8                         flow_cookie;
} dssocki_flow_event_info_type;
#endif

/*---------------------------------------------------------------------------
  protocol control block union type - contains pointers to protocol
  specific control blocks
---------------------------------------------------------------------------*/
typedef union
{
  struct icmp_cb *icb;                                             /* ICMP */
  struct tcb *tcb;                                                  /* TCP */
  struct udp_cb *ucb;                                               /* UDP */
  void *p;                                                       /* dunno? */
} protocol_ctl_block_type;

typedef struct
{
  struct  connection conn;
  uint32  mss;
  uint32  cwind;
  struct tcp_options_s  options;
} tcp_info_type;

typedef struct
{
  struct connection conn;
} udp_info_type;

typedef struct
{
  uint8  type;
  uint8  code;
} icmp_info_type;

typedef struct
{
  ds_fwk_s_type*      routing_cache;
  uint32              flow_label;
  uint64              options_mask;
  uint32              tx_flags;
  uint8               ttl;
  uint8               tos;
  uint8               tclass;
//  ps_route_scope_type route_scope;
} ip_info_type;

/*---------------------------------------------------------------------------
  The congestion window and sendcount updates to the upper layer during
  data transmission. Member of data union of ps_event_info_type structure.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32 cwind;
  uint32 sndcnt;
}tx_upcall_info_type;

/*---------------------------------------------------------------------------
  When the socket state changes due to some TCP state change
  sock_state_change_info_type has the new state for the sockt and the close
  reason in the case the socket is going into CLOSED or WAIT_FOR_CLOSE state.
  Member of data union of ps_event_info_type structure.
---------------------------------------------------------------------------*/
typedef struct
{
  dssocki_state_enum_type new_state;
  sint15                  reason;
}sock_state_change_info_type;

typedef enum
{
  PS_TCP_EVENT = 0,
  PS_UDP_EVENT = 1,
  PS_IP_EVENT  = 2,
  PS_SOCKET_EVENT = 3,
  PS_DATA_AVAILABLE_EVENT = 4,
  PS_TX_UPCALL_EVENT = 5,
  PS_SOCK_STATE_CHANGE_EVENT = 6,
  PS_ICMP_ERROR_EVENT = 7
}ps_event_enum_type;

typedef struct ps_event_info_s
{
  int16               sockfd;
  ps_event_enum_type  id;                  /* protocol type TCP_PTCL, etc. */

  union
  {
    tcp_info_type               tcp;
    udp_info_type               udp;
    ip_info_type                ip;
    boolean                     data_available;
    tx_upcall_info_type         t_upcall;
    sock_state_change_info_type state;
    uint32                      icmp_error;
  } data;
} ps_event_info_type;

typedef struct
{
  union
  {
    tcp_info_type   tcp;
    udp_info_type   udp;
    icmp_info_type  icmp;
  } pcb;

  ip_info_type      ip;
} sock_cache_type;


/*---------------------------------------------------------------------------
  Information about filters that are installed on a socket
---------------------------------------------------------------------------*/
typedef struct
{
  /* Iface on which IP filters are installed */
//  ps_iface_type                * iface_ptr;
  ds_fwk_s_type                  * fw_inst;
  /* Handle to IP filters that are installed */
//  ps_iface_ipfltr_handle_type    ipfltr_handle;
  ds_ipfltr_handle_type           ipfltr_handle;
} dssocki_fltr_info_type;

/*---------------------------------------------------------------------------
  socket control block structure.
---------------------------------------------------------------------------*/
typedef struct scb_s
{
  q_link_type link;                                    /* Link to next SCB */
  acb_type *acb_ptr;          /* Ptr to application ctl block for this scb */
  ds_flow_type          *flow_ptr;          /* Pointer to flow being used  */
  struct dss_fcn_ptr_table *fcn_ptr_table;  /* protocol function ptr table */
  protocol_ctl_block_type protocol_ctl_blk;      /* protocol control block */
  sint31 requested_event_mask;       /* Events the socket is interested in */
  sint31 event_happened_mask;                 /* Events that have occured  */
  struct scb_s *so_head;                  /* back pointer to accept socket */
  struct scb_s *so_partial;    /* queue of unconnected/partial connections */
  sint15 so_qlimit;                                 /* max length of queue */
  sint15 closed_state_reason;      /* reason socket went into CLOSED state */
  sint15 sockfd;                  /* SCB's corresponding socket descriptor */
  dssocki_state_enum_type socket_state;             /* state of the socket */
  uint16 state_flags;            /* flags associated with the socket_state */
  byte family;                                           /* address family */
  byte type;                                                /* socket type */
  byte protocol;                                          /* protocol type */
  boolean data_available;                 /* Something to read -or- accept */
  ip_pkt_info_type  pkt_info_cache;     /* Address cache for UDP routing   */
  dssocki_fltr_info_type  fltr_info; /* Info regarding filters of a socket */
  uint64 options_mask;                      /* bit-mask for socket options */
  uint32 rcvbuf;      /* maximum number of bytes allowed to be in the rcvq */
  uint32 sndbuf;      /* maximum number of bytes allowed to be in the sndq */
  int    so_linger;            /* time to linger for, in ms, while closing */
                                          /* timer id for the linger timer */
  ps_timer_handle_type    linger_timer;
  acb_type *linger_acb_ptr;               /* ptr to the acb while lingering */
  void* sdb_ack_user_data; /* the user data to be passed to sdb ack cb fcn */
  dss_sock_cb_fcn sock_cb;                    /* socket event callback fcn */
  void * sock_cb_user_data;      /* User data specfied during registration */
  sock_cache_type cached_info;
  boolean         dirty_cache_flag;     /* flag to indicate cache is dirty */
  uint32 sndcnt;                                          /* tcp snd count */
  uint32 rx_iface_id;                     /* iface id on which pkt was rx'd */
  uint32  so_error;                /* Stores ICMP(v6) errors on this socket */
  uint32 max_backoff_time;        /*TCP Max backoff timer*/
  uint8 icmp_code;                /*ICMP Code*/
  uint8 icmp_type;                 /*ICMP type*/
  struct ps_sock_extended_err sock_err;   /* Stores the ICMP(v6) error information */
  struct ps_in6_addr v6_offender;     /* IP address of the node generating err */
  boolean error_available;         /* Shows if there is ICMP error to fetch */
  uint32 qos_subset_id;            /* QOS subset id                        */
  boolean is_write_blocked;    /* Boolean to detect whether TCP socket is write */
                               /* blocked or not. */
} scb_type;

typedef void
  (*ps_upcall_fcn_ptr) (void * , ps_event_info_type *);

/*---------------------------------------------------------------------------
  protocol specific function pointer table - contains function pointers
  to "common" socket functions, that are implemented differently, depending
  on the protocol being used.
---------------------------------------------------------------------------*/
struct dss_fcn_ptr_table
{
  int protocol;                                             /* Socket type */
  sint15 (*socket)(struct scb_s *scb_ptr, sint15 *dss_errno);/* alloc sckt */
  sint15 (*connect_setup)(struct scb_s *scb_ptr,
                          struct ps_sockaddr_in6* dest_ptr,
                          sint15 *dss_errno);         /* set-up connection */
  sint15 (*ds_connect)(protocol_ctl_block_type pcb,
                    struct ps_sockaddr_in6 *dest_ptr,
                    ps_meta_info_type *meta_info_ptr);  /* perform connect */
  sint15 (*listen_setup) (struct scb_s *scb_ptr,
              sint15 backlog,
              sint15 * dss_errno);           /* set-up sockets */
  sint15 (*listen) ( protocol_ctl_block_type pcb,
                     uint16 rcv_window);/* perform passive open */
  sint15 (*ds_accept) (struct scb_s *scb_ptr,
            struct ps_sockaddr * remote_addr,
            uint16 * addr_len,
            sint15 * dss_errno);        /* accept a new connection */
  sint15 (*write)(protocol_ctl_block_type pcb,
                  struct ps_sockaddr_in6 *ta_ptr,
                  dsm_item_type *dsm_item,
                  ps_tx_meta_info_type *meta_info_ptr);/* byte output routine */
  sint15 (*read)( struct scb_s * scb_ptr, struct ps_iovec * iov,
                  uint16 iovcount, struct ps_sockaddr *fromaddr,
                  uint16 * addrlen,
                  int flags,
                  sint15* dss_errno);                /* byte input routine */
  sint15 (*read_dsm_chain)( struct scb_s* scb_ptr,
                            dsm_item_type **item_ptr,
                            struct ps_sockaddr *fromaddr,
                            uint16 * addrlen,
                            sint15* dss_errno);  /* dsm item input routine */
  sint15 (*shutdown)( protocol_ctl_block_type pcb,
                      int how,
                      sint15* dss_errno);       /* socket shutdown routine */
  sint15 (*close)(protocol_ctl_block_type pcb,
                  boolean silent_close_flag,
                  sint15 *dss_errno); /* cls con*/
  void (*bind)(protocol_ctl_block_type pcb, struct ps_sockaddr_in6 *la);
                                                               /* bind sock*/
  int (*is_writeable)(struct scb_s * scb_ptr);
  /*-------------------------------------------------------------------------
    register an upcall to get notification of protocol specific information
    change
  -------------------------------------------------------------------------*/
  void (*register_upcall)( struct scb_s *scb_ptr,
                           ps_upcall_fcn_ptr upcall_fcn );

  void (*setopt) (protocol_ctl_block_type pcb, int level, int optname,
      void* optval, int optlen);       /* set ptcl options */

  /* Processing reopening of window */
  int16 (*window_reopen) (protocol_ctl_block_type pcb, int16* dss_errno);

//  void (*set_rscope) (protocol_ctl_block_type pcb,
//                     ps_route_scope_type rscope);       /* set route scope */

  /* Processing flow enable on socket's routing cache/flow cache */
  void (*flow_enabled_ind) (protocol_ctl_block_type pcb);

  void (*set_route_meta_info)( protocol_ctl_block_type pcb,
                               int32                   sockfd,
                               ps_rt_meta_info_type *  rt_mi_ptr);

  void (*set_linger_reset)( protocol_ctl_block_type pcb,
                            int32    sockfd,
                            boolean  is_linger_on,
                            int32    time_in_sec);
};


/*--------------------------------------------------------------------------
  enum for types of PS commands.
 -------------------------------------------------------------------------*/
typedef enum
{
  APP_TO_PS_SOCKET_CMD_MIN_VAL       = 0,
  APP_TO_PS_SOCKET_PAYLOAD           = 1,
  APP_TO_PS_SOCKET_CONNECT           = 2,
  APP_TO_PS_SOCKET_CLOSE             = 3,
  APP_TO_PS_SOCKET_SHUTDOWN          = 4,
  APP_TO_PS_SOCKET_LISTEN            = 5,
  APP_TO_PS_SOCKET_SETOPT            = 6,
  APP_TO_PS_SOCKET_BIND              = 7,
  APP_TO_PS_SOCKET_WINDOW_REOPEN     = 8,
  APP_TO_PS_SOCKET_RSCOPE            = 9,
  APP_TO_PS_SOCKET_FLOW_ENABLED_IND  = 10,
  APP_TO_PS_SOCKET_SET_ROUTE_META_INFO =11,
  APP_TO_PS_SOCKET_LINGER_RESET          =12,
  APP_TO_PS_SOCKET_CMD_MAX_VAL
} dssocki_app_to_ps_cmd_kind_type;

/*---------------------------------------------------------------------------
  PS command
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type                        link; /* field for put/get to/from Qs */
  dssocki_app_to_ps_cmd_kind_type    kind;          /* application command */
  protocol_ctl_block_type            pcb;            /* protocol ctl block */
  int                                protocol;            /* TCP/UDP/ICMP? */
  struct ps_sockaddr_in6                ta;            /* destination address */
  sint15                             sockfd;                  /* Socket fd */
  void                             * user_data;      /* Kind specific data */
  boolean                          is_linger_on;
  int32                            time_in_sec;
  ps_rt_meta_info_type             *rt_meta_info_ptr;
} dssocki_app_to_ps_cmd_type;

typedef enum
{
  DSSOCKI_DSM_ITEMS_FEW =0,
  DSSOCKI_DSM_ITEMS_MANY
}dssocki_dsm_item_kind_type;

typedef struct
{
  dssocki_dsm_item_kind_type  kind;
}dssocki_dsm_item_cmd_type;

/*---------------------------------------------------------------------------
  protocol specific function pointer table - array of function tables,
  on a per protocol basis.
---------------------------------------------------------------------------*/
extern struct dss_fcn_ptr_table dssock_fcn_ptr_table[];

extern const dssocki_sockopt_type dss_sock_opts_default[];
/*---------------------------------------------------------------------------
  Application Identifier Base.
   NOTE THAT THE  DSS_NETHANDLEBASE_MIN_VAL SHOULD NOT EQUAL ZERO.
   Note, these should be within range of dss_nethandlebase type.
---------------------------------------------------------------------------*/
#define DSS_NETHANDLEBASE_MIN_VAL  100
#define DSS_NETHANDLEBASE_MAX_VAL  ((1<<15) - 1 - DSS_MAX_APPS)


extern ps_meta_info_type_ex    * dss_meta_info_ex_ptr;
extern ps_tx_meta_info_type    * dss_tx_meta_info_ptr;


/*===========================================================================

                      PUBLIC MACRO DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO DSSOCKI_ID_GET_IFACE()

DESCRIPTION
  This macro returns the iface ptr corresponding to the dss iface id given.

PARAMETERS
  iface_id: dss_iface_id used by apps to access/control the iface

RETURN VALUE
  A valid iface ptr or NULL if id is DSS_IFACE_INVALID_ID or no iface
  matches the iface_id.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
//#define DSSOCKI_ID_GET_IFACE(iface_id)                                     \
//   ((iface_id != DSS_IFACE_INVALID_ID ) ?                                  \
//      ps_iface_get_handle((ps_iface_id_type)iface_id) : NULL)



/*===========================================================================
MACRO DSSOCKI_GET_QOS_HANDLE()

DESCRIPTION
  This macro returns the opaque id for a QOS instance given an iface and
  flow which represents the QOS instance

PARAMETERS
  iface_ptr : iface on which QOS instance is supported
  flow_ptr  : flow which represents the QOS instance

RETURN VALUE
  A valid QOS handle or NULL

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define DSSOCKI_GET_QOS_HANDLE(iface_ptr, flow_ptr)                      \
  ((PS_IFACE_IS_VALID(iface_ptr) && PS_FLOW_IS_VALID(flow_ptr))          \
      ? (PS_IFACE_GET_ID3(iface_ptr->name,                               \
                          iface_ptr->instance,                           \
                          PS_FLOWI_GET_COOKIE(flow_ptr)))                \
      : NULL)


/*===========================================================================
MACRO DSSOCKI_GET_FLOW_COOKIE_FROM_QOS_HANDLE()

DESCRIPTION
  This macro returns cookie that corresponds to a QOS instance given its
  handle

PARAMETERS
  qos_handle : QOS handle of a QOS instance

RETURN VALUE
  Cookie of the flow which reprsents QOS instance of a QOS handle

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define DSSOCKI_GET_FLOW_COOKIE_FROM_QOS_HANDLE(qos_handle)              \
  (qos_handle & 0x000000FF)

/*==========================================================================
MACRO      DSSOCKI_BIND_ACB_TO_SCB()

DESCRIPTION  This macro binds an ACB to a SCB.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_BIND_ACB_TO_SCB(scb_ptr, bind_acb_ptr)                   \
  (scb_ptr)->acb_ptr = bind_acb_ptr;                                     \
  /*lint -save -e774 */                                                  \
  if (NULL != bind_acb_ptr)                                              \
  /*lint -restore */                                                     \
  {                                                                      \
    /*lint -save -e571 */  		                                        \
    (scb_ptr)->qos_subset_id = (uint32) ((bind_acb_ptr)->dss_nethandle); \
    /*lint -restore */                                                   \
    (bind_acb_ptr)->scb_cnt++;                                           \
  }

/*===========================================================================
MACRO      DSSOCKI_CACHE_GET_LOCAL()

DESCRIPTION  This macro gets the local endpoint (addr, port) of a socket
             from the cached information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_GET_LOCAL(scb_ptr, lname)          \
{                                                        \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);   \
  if ((scb_ptr)->type == (byte)PS_IPPROTO_TCP)                    \
  {                                                      \
    (lname) = (scb_ptr)->cached_info.pcb.tcp.conn.local; \
  }                                                      \
  else                                                   \
  {                                                      \
    (lname) = (scb_ptr)->cached_info.pcb.udp.conn.local; \
  }                                                      \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);   \
}

/*===========================================================================
MACRO      DSSOCKI_CACHE_GET_REMOTE()

DESCRIPTION  This macro gets the remote endpoint (addr, port) of a socket
             from the cached information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_GET_REMOTE(scb_ptr, fname)          \
{                                                         \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);         \
  if ((scb_ptr)->type == (byte)PS_IPPROTO_TCP)               \
  {                                                       \
    (fname) = (scb_ptr)->cached_info.pcb.tcp.conn.remote; \
  }                                                       \
  else                                                    \
  {                                                       \
    (fname) = (scb_ptr)->cached_info.pcb.udp.conn.remote; \
  }                                                       \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);    \
}

/*===========================================================================
MACRO      DSSOCKI_CACHE_SET_LOCAL()

DESCRIPTION  This macro sets the local endpoint (addr, port) of a socket
             in the cached information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_SET_LOCAL(scb_ptr, lname)         \
{                                                       \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);  \
  if ((scb_ptr)->type == (byte)PS_IPPROTO_TCP)                   \
  {                                                     \
    (scb_ptr)->cached_info.pcb.tcp.conn.local = lname;  \
  }                                                     \
  else                                                  \
  {                                                     \
    (scb_ptr)->cached_info.pcb.udp.conn.local = lname;  \
  }                                                     \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  \
}

/*===========================================================================
MACRO      DSSOCKI_CACHE_SET_REMOTE()

DESCRIPTION  This macro sets the remote endpoint (addr, port) of a socket
             in the cached information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_SET_REMOTE(scb_ptr, fname)        \
{                                                       \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);       \
  if ((scb_ptr)->type == (byte)PS_IPPROTO_TCP)             \
  {                                                     \
    (scb_ptr)->cached_info.pcb.tcp.conn.remote = fname; \
  }                                                     \
  else                                                  \
  {                                                     \
    (scb_ptr)->cached_info.pcb.udp.conn.remote = fname; \
  }                                                     \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);  \
}

/*===========================================================================
MACRO      DSSOCKI_GET_TX_IFACE()

DESCRIPTION  This macro gets the transmit interface of a socket from the
             cached information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_GET_TX_FWK_INST(scb_ptr, tx_iface)     \
{                                                         \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);    \
  (tx_iface) = (scb_ptr)->cached_info.ip.routing_cache;   \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);    \
}

/*===========================================================================
MACRO      DSSOCKI_SET_TX_IFACE()

DESCRIPTION  This macro sets the transmit interface of a socket in the
             cached information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_SET_TX_FWK_INST(scb_ptr, tx_iface)     \
{                                                         \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);    \
  (scb_ptr)->cached_info.ip.routing_cache = (tx_iface);   \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);    \
}

/*===========================================================================
MACRO      DSSOCKI_CACHE_GET_FLOW_LABEL()

DESCRIPTION  This macro gets the flow label.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_GET_FLOW_LABEL( scb_ptr)                           \
  (scb_ptr)->cached_info.ip.flow_label


/*===========================================================================
MACRO      DSSOCKI_CACHE_SET_FLOW_LABEL()

DESCRIPTION  This macro sets the flow label.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_SET_FLOW_LABEL(scb_ptr, flow_info)      \
{                                                             \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);        \
  (scb_ptr)->cached_info.ip.flow_label = (flow_info);         \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);        \
}

/*===========================================================================
MACRO      DSSOCKI_CACHE_GET_TX_FLAGS()

DESCRIPTION  This macro gets the tx flags

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_GET_TX_FLAGS( scb_ptr)                           \
  (scb_ptr)->cached_info.ip.tx_flags


/*===========================================================================
MACRO      DSSOCKI_CACHE_SET_TX_FLAGS()

DESCRIPTION  This macro sets the tx flags

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_SET_TX_FLAGS(scb_ptr, cached_tx_flags)             \
  {                                                                      \
    DS_ENTER_CRIT_SECTION(&global_ps_crit_section);                      \
    (scb_ptr)->cached_info.ip.tx_flags = (cached_tx_flags);              \
    DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);                      \
  }

/*===========================================================================
MACRO      DSSOCKI_CACHE_GET_ROUTE_SCOPE()

DESCRIPTION  This macro gets the route scope.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_GET_ROUTE_SCOPE(scb_ptr, rscope)        \
{                                                             \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);        \
  (rscope) = (scb_ptr)->cached_info.ip.route_scope;           \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);        \
}

/*===========================================================================
MACRO      DSSOCKI_CACHE_SET_ROUTE_SCOPE()

DESCRIPTION  This macro sets the route scope.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_CACHE_SET_ROUTE_SCOPE(scb_ptr, rscope)        \
{                                                             \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);        \
  (scb_ptr)->cached_info.ip.route_scope = (rscope);           \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);        \
}

/*===========================================================================
MACRO        TCP_INFO_CHANGE_UPCALL()

DESCRIPTION  This macro makes the info change upcall to indicate to the
             upper layers of a change in the TCP related information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define TCP_INFO_CHANGE_UPCALL(tcb_ptr)                   \
{                                                         \
  ps_event_info_type  info;                               \
  if (tcb_ptr->info_change_upcall != NULL)                \
  {                                                       \
    info.id = PS_TCP_EVENT;                               \
    info.sockfd = tcb_ptr->sockfd;                        \
    info.data.tcp.conn = tcb_ptr->conn;                   \
    info.data.tcp.options = tcb_ptr->options;             \
    info.data.tcp.mss = tcb_ptr->mss;                     \
    info.data.tcp.cwind = tcb_ptr->cwind;                 \
    tcb_ptr->info_change_upcall(tcb_ptr->scb_ptr, &info); \
  }                                                       \
}

/*===========================================================================
MACRO        UDP_INFO_CHANGE_UPCALL()

DESCRIPTION  This macro makes the info change upcall to indicate to the
             upper layers of a change in the UDP related information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define UDP_INFO_CHANGE_UPCALL(ucb_ptr)                   \
{                                                         \
  ps_event_info_type  info;                               \
  if (ucb_ptr->info_change_upcall != NULL)                \
  {                                                       \
    info.id = PS_UDP_EVENT;                               \
    info.sockfd = ucb_ptr->sockfd;                        \
    info.data.udp.conn = ucb_ptr->conn;                   \
    ucb_ptr->info_change_upcall(ucb_ptr->scb_ptr, &info); \
  }                                                       \
}

/*===========================================================================
MACRO        IP_INFO_CHANGE_UPCALL()

DESCRIPTION  This macro makes the info change upcall to indicate to the
             upper layers of a change in the IP related information.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define IP_INFO_CHANGE_UPCALL(ipcb_ptr, sock_fd, user_data,               \
                              info_change_upcall)                         \
{                                                                         \
  ps_event_info_type  info;                                               \
  if(info_change_upcall != NULL)                                          \
  {                                                                       \
    info.id = PS_IP_EVENT;                                                \
    info.sockfd = sock_fd;                                                \
    info.data.ip.routing_cache = (ipcb_ptr)->routing_cache;               \
    info.data.ip.options_mask = (ipcb_ptr)->options_mask;                 \
    info.data.ip.ttl = (ipcb_ptr)->ttl;                                   \
    info_change_upcall(user_data, &info);                                 \
  }                                                                       \
}

/*===========================================================================
MACRO      DSSOCKI_SOCKET_CANTRECVMORE()

DESCRIPTION  This macro updates the state_flags of the given scb to
             indicate that the socket has been shut down for the read-half

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Modifies state_flags in the scb
===========================================================================*/
#define DSSOCKI_SOCKET_CANTRECVMORE(scb_ptr)                  \
{                                                             \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);        \
  scb_ptr->state_flags |= (uint16)DSSOCKI_CANTRECVMORE;  \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);        \
}

/*===========================================================================
MACRO      DSSOCKI_SOCKET_CANTSENDMORE()

DESCRIPTION  This macro updates the state_flags of the given scb to
             indicate that the socket has been shut down for the write-half

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  Modifies state_flags in the scb
===========================================================================*/
#define DSSOCKI_SOCKET_CANTSENDMORE(scb_ptr)                  \
{                                                             \
  DS_ENTER_CRIT_SECTION(&global_ps_crit_section);        \
  scb_ptr->state_flags |= (uint16)DSSOCKI_CANTSENDMORE;  \
  DS_LEAVE_CRIT_SECTION(&global_ps_crit_section);        \
}

/*===========================================================================
MACRO         IS_SOCKOPT_SET()

DESCRIPTION   Check if the specified socket option is set,

DEPENDENCIES  None

RETURN VALUE  True, if the option is set
              False, otherwise

SIDE EFFECTS  None
===========================================================================*/
#define IS_SOCKOPT_SET(options_mask, optname) \
   ((options_mask & ((uint64)1 << optname))? TRUE : FALSE)


/*===========================================================================
MACRO         SOCKOPT_SET()

DESCRIPTION   Set the specified socket option,

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define SOCKOPT_SET(options_mask, optname) \
   ( (options_mask) |= ( (uint64)1 << (optname) ) )


/*===========================================================================
MACRO         SOCKOPT_RESET()

DESCRIPTION   Reset the specified socket option

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define SOCKOPT_RESET(options_mask, optname) \
   ( (options_mask) &= ~((uint64)1 << optname))

/*===========================================================================
MACRO         DSSOCKI_RESET_ERROR_AVAILABLE()

DESCRIPTION   Set error_available flag to FALSE if IP_RECVERR and IPV6_ERCVERR
              flags are not set.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_RESET_ERROR_AVAILABLE(scb_ptr) \
  if(!IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,(uint64)DSS_IP_RECVERR) &&   \
     !IS_SOCKOPT_SET(scb_ptr->cached_info.ip.options_mask,(uint64)DSS_IPV6_RECVERR))   \
  {                                                                                    \
    scb_ptr->error_available = FALSE;                                                  \
  }


/*===========================================================================
MACRO         DSSOCKI_IS_APP_CTRL_FLAG_SET

DESCRIPTION   Check if the specified control flag is set,

DEPENDENCIES  None

RETURN VALUE  True, if the option is set
              False, otherwise

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_IS_APP_CTRL_FLAG_SET(acb_ptr, optname) \
   (((acb_ptr)->flags & (optname))? TRUE : FALSE)


/*===========================================================================
MACRO         DSSOCKI_SET_APP_CTRL_FLAG

DESCRIPTION   Set the specified control flag,

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_SET_APP_CTRL_FLAG(acb_ptr, optname) \
   ((acb_ptr)->flags |= (optname))


/*===========================================================================
MACRO          DSSOCKI_RESET_APP_CTRL_FLAG

DESCRIPTION   Reset the specified control flag.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#define DSSOCKI_RESET_APP_CTRL_FLAG(acb_ptr, optname) \
   ((acb_ptr)->flags &= ~( optname))

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSSOCKI_GENERATE_PKT_INFO()

DESCRIPTION
  Generates pkt info containing header and misc info

PARAMETERS
  scb_ptr        : socket which is transmitting data
  v6_sockaddr    : remote socket's address
  pkt_info_ptr   : OUT PARAM. pkt info is populated in this parameter
  dss_errno      : errno passed back if pkt info can't be generated

RETURN VALUE
  On error, DSS_ERROR is returned; DSS_SUCCESS otherwise.

DEPENDENCIES
  pointers must be valid

SIDE EFFECTS
  None
===========================================================================*/
int32 dssocki_generate_pkt_info
(
  scb_type             * scb_ptr,
  struct ps_sockaddr_in6  * v6_sockaddr,
  ip_pkt_info_type     * pkt_info_ptr,
  int16                * dss_errno
);

/*===========================================================================

FUNCTION DSSOCKI_ALLOCSCB

DESCRIPTION
  Allocates socket control block from SCB array.  Calculates socket
  descriptor and sets it in the SCB.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.

===========================================================================*/
extern struct scb_s *dssocki_allocscb
(
  byte protocol
);

/*===========================================================================
FUNCTION DSSOCKI_INIT_SCB()

DESCRIPTION
  Initializes the given scb

DEPENDENCIES
  None.

RETURN VALUE
  TRUE:  If the initialization was successful
  FALSE: Otherwise

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean
dssocki_init_scb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_FREESCB

DESCRIPTION
  Frees a socket control block in the SCB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_freescb
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCKFDTOSCB

DESCRIPTION
  Given a socket descriptor, returns a socket control block from SCB array.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.

===========================================================================*/
extern struct scb_s *dssocki_sockfdtoscb
(
  int sockfd                                          /* socket descriptor */
);


/*===========================================================================
FUNCTION DSSOCKI_INDEXTOSCB()

DESCRIPTION
  Given an index, returns a socket control block from SCB array.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to socket control block structure.

SIDE EFFECTS
  None.
===========================================================================*/
struct scb_s *dssocki_indextoscb
(
  uint16 index
);



/*===========================================================================

FUNCTION DSSOCKI_ALLOCACB

DESCRIPTION
  Allocates application control block from ACB array.  Assigns application
  ID and sets the network and socket callback functions.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.  If none are
  available, returns NULL.

SIDE EFFECTS
  None.

===========================================================================*/
extern acb_type *dssocki_allocacb
(
  void
);

/*===========================================================================

FUNCTION DSSOCKI_FREEACB

DESCRIPTION
  Frees an application control block in the ACB array for re-use.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_freeacb
(
  acb_type *acb_ptr          /* ptr to application control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_DSS_NETHANDLETOACB

DESCRIPTION
  Given an application ID, returns an application control block from ACB
  array.

DEPENDENCIES
  None.

RETURN VALUE
  Returns pointer to application control block structure.

SIDE EFFECTS
  None.

===========================================================================*/

#define dssocki_appidtoacb    dssocki_dss_nethandletoacb

extern acb_type *dssocki_dss_nethandletoacb
(
  sint15 dss_nethandle                                          /* application ID */
);

/*===========================================================================
FUNCTION DSSOCKI_IS_ADDR_IN_USE()

DESCRIPTION
  Determines if the specified address, in network byte order, is in use for
  the given protocol.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - The address is in use
  FALSE - The address is not in use

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean dssocki_is_addr_in_use
(
  struct scb_s *       scb_ptr,
  byte proto,
  struct ps_sockaddr_in6* laddr
);

/*===========================================================================

FUNCTION DSSOCKI_GETEMPHEM_PORT

DESCRIPTION
  Returns an ephemeral port, for the specified protocol, between PORTBASE and
  PORTBASE+PORTOFFSET after checking that it is not in use by another socket.

DEPENDENCIES
  None.

RETURN VALUE
  Port number: in network byte order, not in use by another socket
  0: In case there is no ephemeral port available

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16 dssocki_getephem_port
(
  byte proto       /* protocol for which ephemeral port is to be generated */
);

/*===========================================================================

FUNCTION DSSOCKI_FCNPTR_SETUP

DESCRIPTION
  Sets up the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Function pointer table is assinged.

===========================================================================*/
extern sint15 dssocki_fcnptr_setup
(
  struct scb_s *sock_ptr,                /* socket control block structure */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================

FUNCTION DSSOCKI_FCNPTR_SETUP

DESCRIPTION
  Sets up the protocol specific function pointer table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Function pointer table is assinged.

===========================================================================*/
extern struct dss_fcn_ptr_table * dssocki_get_fcnptr_table
(
 int protocol                                        /* transport protocol */
);

/*===========================================================================
FUNCTION DSSOCKI_SEND_PS_CMD_BUFFER()

DESCRIPTION
  Sends the PS command to the socket send q

DEPENDENCIES
  The PS command should be first memory allocated and its kind be
  set using dssocki_alloc_ps_cmd_buffer

RETURN VALUE
  DSS_ERROR if any of the arguments passed are invalid
  DSS_SUCCESS otherwise

SIDE EFFECTS

===========================================================================*/
void dssocki_send_ps_cmd_buffer
(
  struct scb_s                   *scb_ptr,  /* ptr to socket control block */
  void                           *user_data,/* Ptr to payload to be sent   */
  struct ps_sockaddr_in6            *ta_ptr,   /* Ptr to destination info     */
  dssocki_app_to_ps_cmd_type     *ps_cmd    /* ps command buffer           */
);

/*===========================================================================
FUNCTION DSSOCKI_ALLOC_PS_CMD_BUFFER()

DESCRIPTION
  Allocates memory for the command buffer and sets its kind.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the ps command buffer, if memory can be allocated for it
  Else return NULL

SIDE EFFECTS
  This is a malloc kind of function. The allocated memory should be
  freed appropriately by the user.
===========================================================================*/
dssocki_app_to_ps_cmd_type *
dssocki_alloc_ps_cmd_buffer
(
  dssocki_app_to_ps_cmd_kind_type kind                  /* PS command kind */
);

/*===========================================================================

FUNCTION DSSOCKI_SEND_TO_PS

DESCRIPTION
  Sends the application's dsm_item data to the PS task, which will send it
  out using the protocol for the given socket. This function first calls
  dssocki_alloc_ps_cmd_buffer to get the ps memory for the command to send.
  If successful, it calls dssocki_send_ps_cmd_buffer.

  This function is called from the context of the socket application task.

DEPENDENCIES
  None.

RETURN VALUE
  DSS_ERROR if no memory or invalid arguments
  DSS_SUCCESS on success

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dssocki_send_to_ps
(
  struct scb_s  *scb_ptr,                   /* ptr to socket control block */
  void * user_data,                           /* Ptr to payload to be sent */
  struct ps_sockaddr_in6* ta_ptr,   /* Ptr to structure holding the dest info */
  dssocki_app_to_ps_cmd_kind_type  kind     /* Type of item to be enqueued */
);

/*===========================================================================

FUNCTION DSSOCKI_COPY_BUF_TO_DSM

DESCRIPTION
  Copies application data buffer vector to data services memory pool.

DEPENDENCIES
  None.

RETURN VALUE
  pointer to dsm_item_type

SIDE EFFECTS
  None.

===========================================================================*/
extern sint15 dssocki_copy_buf_to_dsm
(
  struct ps_iovec *iov,      /* array of data buffers from which to copy data */
  sint15 iovcount,                                /* number of array items */
  sint15 max_bytes,                     /* maximum number of bytes to copy */
  dsm_item_type **buf_ptr_ptr          /* pointer to dsm item to copy into */
);

/*===========================================================================
FUNCTION DSSOCKI_GENERATE_META_INFO()

DESCRIPTION
  Generates PS meta info packet for TX flags and socket opetions
  set and attaches it to the packet.

DEPENDENCIES
  Packet head pointer has to include all headers above IP.

RETURN VALUE
  DSS_SUCCESS or DSS_ERROR (dss_errno contains cause).

SIDE EFFECTS
  None.
===========================================================================*/
ps_tx_meta_info_type * dssocki_generate_meta_info
(
  scb_type*       scb_ptr,
  ds_flow_type*   flow_ptr,
  uint32          tx_flags,
  boolean         *ipcb_meta_info_valid,
  sint15*         dss_errno
);


/*===========================================================================

FUNCTION DSSOCKI_SET_SOCK_STATE

DESCRIPTION
  This function sets the socket state. It can be NULL, INIT, OPENING, OPEN,
  CLOSING, WAIT_FOR_CLOSE, or CLOSED.

  NULL: Is a pseudo-state, that is set prior to initial socket
           creation.  A NULL socket cannot be accessed by any API
           function.

  INIT: Is specific to TCP, and is set when a TCP socket is first
           created, through a call to socket().  Note this is an
           invalid state for UDP sockets.

  LISTEN: This socket it listening.  This is specific to TCP, as such
           is an invalid state for UDP.  accept() may be called on
           this socket.

  OPENING: the socket is not writable and read() and write() would
           return DS_EWOULDBLOCK.
           A connected socket is in the OPENING state after dss_connect()
           has been called and before the socket is completly connected.
           A connectionless socket is never in this state.

  OPEN: This means the socket is writable if there is enough memory
           available.  A connected socket is in the OPEN state when
           TCP is in the ESTABLISHED state.  A connectionless socket
           is in OPEN state in the dss_socket() call.

  CLOSED:  means the socket is writable and readable  and read() and write()
           would return DS_ECONNABORTED, DS_ECONNRESET, DS_IPADDRCHANGED or
           DS_ENETDOWN. A connected socket is in the CLOSED state after it
           lost its connection to the server or the network connectivity was
           lost any time after the dss_connect() was called.
           A connectionless socket is in this state, only when the network
           subsystem has experienced a failure.

  CLOSING: means that a TCP socket has issued a close(), but the TCP close
           is still in progress.  The initial call to close() for TCP
           sockets would return DS_EWOULDBLOCK.  This corresponds to the
           active close, and the socket is waiting for TCP to fully
           negotiate the close.
           This state is invalid for connectionless sockets.

  WAIT_FOR_CLOSE: means that TCP has fully closed, or the TCP close
           timer has expired.  In this state, the application may call
           close() again to clean up the socket and free it for
           re-use.  This state is invalid for connectionless sockets.

  This should be called whenever the state changes and is the only function
  which modifies the socket_state member of the scb_s.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_set_sock_state
(
  struct scb_s* scb_ptr,     /* Ptr to socket control block for the socket */
  dssocki_state_enum_type new_state                        /* Socket state */
);


/*===========================================================================

FUNCTION DSSOCKI_SOCKET_EVENT_CHECK()

DESCRIPTION
  This function performs a real-time check, to determine if any of the socket
  events of interest have occurred.  If so, it returns the event mask of the'
  events that have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  sint31 event mask of the socket events that have occurred.

SIDE EFFECTS
  None.

===========================================================================*/
sint31 dssocki_socket_event_check
(
  struct scb_s* scb_ptr,          /* ptr to socket control block structure */
  sint31 requested_event_mask        /* Events the socket is interested in */
);

/*===========================================================================

FUNCTION DSSOCKI_NOTIFY_APP

DESCRIPTION
  This function calls the application callback function to alert the
  application that a socket event has occurred.  Note that the application
  callback will not be called if the application has already been notified.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The application callback function is invoked.

===========================================================================*/
extern void dssocki_notify_app
(
  scb_type * scb_ptr,                       /* ptr to socket control block */
  uint32     event_mask                                  /* Occurred event */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCKET_EVENT_OCCURRED

DESCRIPTION
  This function notifies all interested sockets that a socket event
  has occurred.  If a scb_ptr is specified, this function notifies only that
  socket. If scb_ptr is NULL and acb_ptr is specified, this function notifies
  all sockets for that app. If both scb_ptr and acb_ptr are specified, the
  function assumes it should provide notification for all sockets.  The
  function then loops through the socket control block array and checks each socket for events
  that may have occurred.  The application is notified upon any events
  having occurred.  If a valid scb_ptr is specified, the function checks
  that particular socket for any events that may have occurred.  Again,
  the application is notified if any of the events that it is interested
  in, have occurred.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void dssocki_socket_event_occurred
(
  struct scb_s    *scb_ptr,                 /* ptr to socket control block */
  acb_type *acb_ptr                         /* ptr to app control block    */
);

/*===========================================================================

FUNCTION DSSOCKI_SOCK_SET_STATE_CLOSED

DESCRIPTION
  This function sets the socket state to DSSOCKI_CLOSED, however, only if
  the current socket state is set to DSSOCKI_OPEN or DSSOCKI_OPENING.  If a
  NULL parameter is passed in, it will change all DSSOCKI_OPEN or
  DSSOCKI_OPENING sockets to DSSOCKI_CLOSED.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets socket state to DSSOCKI_CLOSED.
===========================================================================*/
extern void  dssocki_sock_set_state_closed
(
  struct scb_s *scb_ptr           /* ptr to socket control block structure */
);

/*===========================================================================

FUNCTION DSSOCKI_SET_CLOSED_STATE_REASON

DESCRIPTION
  Sets the closed_state_reason field in the SCB with the appropriate
  reason for the socket transitioning into the CLOSED state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the closed_state_reason field.
===========================================================================*/
extern void  dssocki_set_closed_state_reason
(
  struct scb_s *scb_ptr,          /* ptr to socket control block structure */
  sint15   reason                /* reason for socket tranistion to CLOSED */
);

/*===========================================================================

FUNCTION DSSOCKI_GET_ERR_VAL

DESCRIPTION
  Gets the appropriate error value based on current socket state and
  type of operation (connect(), close(), read(), write()).  This function
  follows the TCP/UDP socket state machines.

DEPENDENCIES
  None.

RETURN VALUE
  Error condition.

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15 dssocki_get_err_val
(
  struct scb_s *scb_ptr,               /* ptr to socket control block type */
  dssocki_caller_enum_type type,   /* type of function that is calling this
                                      function.                            */
  sint15 *dss_errno                               /* error condition value */
);

/*===========================================================================
FUNCTION DSSOCKI_CHECK_SOCK_EXISTS()

DESCRIPTION

  This API check's if there are any sockets linked with the ACB

DEPENDENCIES
  None.

RETURN VALUE
      0  -- if no sockets are linked with ACB
     -1 -- if sockets are linked with ACB

SIDE EFFECTS
  None
==============================================================================*/
int dssocki_check_sock_exists
(
  acb_type *acb_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_REG_EV()

DESCRIPTION
  This function registers an application callback for the specified event.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If registration is successful.
  FALSE     If registration failed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_reg_ev
(
  sint15                           dss_nethandle,
  ds_pdn_ioctl_event_cb            f_ptr,
  ds_pdn_event_enum_type            event,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_DEREG_EV()

DESCRIPTION
  This function deregisters the application callback for the specified event.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If deregistration is successful.
  FALSE     If deregistration failed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_dereg_ev
(
  sint15                           dss_nethandle,
  ds_pdn_ioctl_event_cb            f_ptr,
  ds_pdn_event_enum_type           event,
  void                             *user_info_ptr
);


/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_PDN_EV_CBACK()

DESCRIPTION   Called when the PDN event happens. This calls any app
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_pdn_ev_cback
(
  ds_pdn_event_enum_type     event,
  ds_pdn_event_info_u_type   event_info
);


/*===========================================================================
FUNCTION      DSSOCKI_INVOKE_APP_EV_CBACKS()

DESCRIPTION   This function invokes callbacks registered by the app (using
              the IOCTL interface) for the specified event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_invoke_app_ev_cbacks
(
  acb_type                         *acb_ptr,
  ds_pdn_event_enum_type                 event,
  ds_pdn_event_info_u_type          event_info
);



#if 0

/*===========================================================================
FUNCTION DSSOCKI_FLOW_EV_CBACK()

DESCRIPTION
  Called when any event happens on a flow.
  This posts a command to PS and is processed in PS task context.

PARAMETERS
  flow_ptr      : flow on which event occured
  event         : event being reported
  event_info    : additinal info on event
  user_info_ptr : data passed back

RETURN VALUE
  None.

DEPENDENCIES
  None

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_flow_ev_cback
(
  ps_flow_type                * flow_ptr,
  ps_iface_event_enum_type      event,
  ps_iface_event_info_u_type    event_info,
  void                        * user_info_ptr
);


/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_IFACE_UP_EV_CBACK()

DESCRIPTION   Called when the IFACE goes up. This calls the
              net_callback_fcn() for the specified application.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_iface_up_ev_cback
(
  ps_iface_type * iface_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_PROCESS_IFACE_EV_CBACK()

DESCRIPTION   Called when the IOCTL event happens. This calls any app
              registered callbacks for this event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_process_iface_ev_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_PROCESS_FLOW_EV_CBACK()

DESCRIPTION
  Called when an event happens on a flow. This calls any app registered
  callbacks for this event.

PARAMETERS
  cmd_name      : command to be processed
  user_info_ptr : data that is passed back in

RETURN VALUE
  None.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_process_flow_ev_cback
(
  ps_cmd_enum_type    cmd_name,
  void              * user_info_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_IS_IFACE_IN_USE()

DESCRIPTION  This function determines if any other app is using the interface
             used by the specified app.

DEPENDENCIES  None

RETURN VALUE  boolean - TRUE if atleast one other app is using iface.
                        FALSE if no other app is using iface.

SIDE EFFECTS  None.
===========================================================================*/
boolean dssocki_is_iface_in_use
(
  acb_type                *acb_ptr
);


/*===========================================================================
FUNCTION DSSOCKI_QOS_REG_EV()

DESCRIPTION
  This function registers an application callback for both implicitly and
  explicitly registered QOS events

DEPENDENCIES
  None

RETURN VALUE
   0 : If registration is successful.
  -1 : If registration failed.

SIDE EFFECTS
  None
===========================================================================*/
int32 dssocki_qos_reg_ev
(
  acb_type                         * acb_ptr,
  dss_iface_ioctl_event_enum_type    event,
  dss_iface_id_type                  iface_id,
  ps_flow_type                     * flow_ptr,
  dss_qos_handle_type                qos_handle,
  dss_iface_ioctl_event_cb           f_ptr,
  void                             * user_info_ptr,
  int16                            * dss_errno
);

/*===========================================================================
FUNCTION DSSOCKI_PRIMARY_QOS_DEREG_EV()

DESCRIPTION
  This function deregisters an application callback for both implicit and
  explicit QOS events on default ps_flow

PARAMETERS
  acb_ptr  : app whose QoS events need to be deregistered
  event    : event to be deregistered
  flow_ptr : ps_flow for which events need to be deregistered

DEPENDENCIES
  This function MUST NOT be used to deregister QoS events on secondary ps_flow

RETURN VALUE
  TRUE  : if event is successfully deregistered
  FALSE : otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean dssocki_primary_qos_dereg_ev
(
  acb_type                         * acb_ptr,
  dss_iface_ioctl_event_enum_type    event,
  ps_flow_type                     * flow_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_IPV6_REG_EV()

DESCRIPTION
  This function registers an application callback for IPv6 privacy events.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If registration is successful.
  FALSE     If registration failed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssocki_ipv6_reg_ev
(
  acb_type                         * acb_ptr,
  ps_iface_type                    * iface_ptr,
  dss_iface_id_type                  iface_id,
  dss_iface_ioctl_event_cb           f_ptr,
  dss_iface_ioctl_event_enum_type    event,
  void                             * user_info_ptr,
  dssocki_event_buf_u_type         * event_buf_data_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_RELEASE_QOS()

DESCRIPTION
  This function releases QOS resources and deregisters the application
  callbacks for QOS events.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If deregistration is successful.
  FALSE     If deregistration failed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_release_qos
(
  acb_type  *acb_ptr
);


/*===========================================================================
FUNCTION DSSOCKI_DEREG_IPV6_EV()

DESCRIPTION
  This function deregs IPv6 event callbacks for Privacy events.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If deregistration is successful.
  FALSE     If deregistration failed.

SIDE EFFECTS
  None.
===========================================================================*/
boolean dssocki_dereg_ipv6_ev
(
  acb_type                         *acb_ptr
);

/*===========================================================================
FUNCTION      DSSOCKI_INVOKE_IPV6_CBACKS()

DESCRIPTION   This function invokes IPv6 Privacy extension callbacks registered
              by the app (using the IOCTL interface).

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void dssocki_invoke_ipv6_cbacks
(
  ps_iface_type                        * iface_ptr,
  dss_iface_ioctl_event_enum_type        event,
  dss_iface_ioctl_event_info_union_type  event_info
);
#endif

/*===========================================================================
FUNCTION DSSOCKI_REG_IP_FILTER()

DESCRIPTION
  This function registers IP filters for the specified iface. The filters are
  passed down to the iface to be added to iface control block while a mapping
  of the iface is added to SCB of the specified socket. This mapping is used
  to deregister these filters when the socket is closed.

DEPENDENCIES
  None.

PARAMETERS
  sockfd      - Descriptor of the Socket requesting filtering
  iface_ptr   - Iface to add filters to
  filter_type - Type of filter
  filters     - Ptr to list of filters (total num_filters) where each
                filter is of type specified above -
                  => ip_filter_type for IP_FILTER_DEFAULT_TYPE
  num_filters - Number of filters passed in
  dss_errno   - Ptr to contain error code on the return

RETURN VALUE
   0 on success
  -1 for error (sets dss_errno appropriately)

SIDE EFFECTS
  None.
===========================================================================*/
int dssocki_reg_ip_filter
(
  sint15                     sockfd,
  uint8                      num_filters,
  void                       *filters,
  uint32                     *filter_handle,
  sint15                     *dss_errno
);

/*===========================================================================
FUNCTION DSSOCKI_SENDMSG()

DESCRIPTION
  This function is a common write function for all the socket output
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and destination address for unconnected udp sockets.
  The function writes data from the scattered buffers over the transport
  specified by the socket descriptor.
DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, in case of tcp it could be less
  than the number of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ---------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EADDRREQ         destination address required
  DS_ENETDOWN         network subsystem unavailable
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EMSGSIZE         the msg is too large to be sent all at once
  DS_EISCONN          if the socket is connected and the destination
                      address is other than it is connected to.

SIDE EFFECTS
  None.
===========================================================================*/

sint15 dssocki_sendmsg
(
  sint15                  sockfd,  /* socket descriptor                    */
  struct dss_msghdr     * msg,     /* Header containing data and dest addr */
  dsm_item_type        ** msg_ptr, /* Optional payload already in DSMs     */
  int                     flags,   /* flags used for SDB (if enabled)      */
  sint15                * dss_errno /* error condition value               */
);

/*===========================================================================
FUNCTION DSSOCKI_RECVMSG()
DESCRIPTION
  This function is a common read function for all the socket input
  functions. The message header contains an array of scattered buffers, a
  socket descriptor and an address field for filling the source address
  of the received packet.The function reads data into the scattered buffers
  over the transport specified by the socket descriptor

DEPENDENCIES
  None.

RETURN VALUE
  n - the number of bytes to be written, which could be less than the number
      of bytes specified.

  On error, return DSS_ERROR and places the error condition value in
  *dss_errno.

  dss_errno Values
  ----------------
  DS_EBADF            invalid socket descriptor is specfied
  DS_EAFNOSUPPORT     address family not supported
  DS_EWOULDBLOCK      operation would block
  DS_EFAULT           bad memory address
  DS_EOPNOTSUPP       option not supported
  DS_EINVAL           can't recv from a listen socket.

SIDE EFFECTS
  None.
===========================================================================*/
sint15 dssocki_recvmsg
(
  sint15                  sockfd,   /* socket descriptor                   */
  struct dss_msghdr     * msg,      /* Message header for filling in data  */
  dsm_item_type        ** msg_ptr, /* Optional payload already in DSMs     */
  int                     flags,    /* flags from dss_recvfrom             */
  sint15                * dss_errno /* error condition value               */
);

/*===========================================================================
FUNCTION DSSOCKI_SOCK_CB()
DESCRIPTION
  This function is a common socket call back function for application
  using old opennetlib function.

DEPENDENCIES
  None.

RETURN VALUE
 None
SIDE EFFECTS
  None.
===========================================================================*/
void dssocki_sock_cb
(
  dss_sock_callback_info_s *sock_callback_info
);

/*===========================================================================
FUNCTION DSSOCKI_PROCESS_LINGERING_CLOSE()

DESCRIPTION
  This function is called to when a lingering socket is closed. It brings
  down the interface if no other application is using it and frees up the
  scb and the associated acb (a lingering socket's acb is allocated
  internally when the socket enters the lingering state)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Frees up the acb and the scb, passed as a parameter
===========================================================================*/
extern void
dssocki_process_lingering_close
(
  scb_type** scb_ptr_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_INFO_CHANGE_HANDLER()

DESCRIPTION
  Common info_change_upcall function for the socket's layer. This function is
  called by the PS, when information cached by the socket's layer
  has changed. (Executed in PS context)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates the cached values.
===========================================================================*/
extern void
dssocki_info_change_handler
(
  void * user_data,
  ps_event_info_type* pcb_info
);

/*===========================================================================
FUNCTION DSSOCKI_CACHE_SETOPT()

DESCRIPTION
  This function updates the option value in the cache and sends a command
  to PS to change the option value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
 Option value is updated in the cache.
===========================================================================*/
extern sint15
dssocki_cache_setopt
(
  scb_type * scb_ptr,
  int        level,
  int        optname,
  void     * optval,
  uint32   * optlen,
  sint15   * dss_errno
);

/*===========================================================================
FUNCTION DSSOCKI_CACHE_GETOPT()

DESCRIPTION
  This function returns the requested option value from the cache.

DEPENDENCIES
  None

RETURN VALUE
  Returns the option value in the pointer

SIDE EFFECTS
  None.
===========================================================================*/
extern sint15
dssocki_cache_getopt
(
  scb_type * scb_ptr,
  int        level,
  int        optname,
  void     * optval,
  uint32   * optlen,
  sint15   * dss_errno
);

/*===========================================================================
FUNCTION DSSOCKI_GET_SOCKADDR_IN6()

DESCRIPTION
  This function check for validity of sockaddr and if it is of type
  sockaddr_in or sockaddr_in6 it converts them into sockaddr_in6.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes the sockaddr_in6 from sockaddr
===========================================================================*/
sint15 dssocki_get_sockaddr_in6
(
  struct ps_sockaddr_in6 * v6addr,                            /* V6 sockaddr */
  struct ps_sockaddr     * sockaddr,                     /* Generic sockaddr */
  uint16                addrlen,                    /* length of sockaddr */
  sint15              * dss_errno                                /* errno */
);

/*===========================================================================
FUNCTION DSSOCKI_FILL_SOCKADDR()

DESCRIPTION
  This function check for validity of sockaddr and fills the structure with
  either sockaddr_in or sockaddr_in6
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills the sockaddr with the 2 tuple(IPaddr and port).
===========================================================================*/
void dssocki_fill_sockaddr
(
  struct ps_sockaddr     * sockaddr,                      /* Generic sockaddr */
  uint16              * addrlen,                     /* length of sockaddr */
  struct socket         localname,             /* IP addr and port 2 tuple */
  uint8                 family                        /* IP address family */
);

/*===========================================================================
FUNCTION DSSOCKI_PS_MEM_LOW_WM_CBACK

DESCRIPTION
  Low watermark call back for ps cmd items.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Triggers socket events.
===========================================================================*/
void dssocki_ps_mem_low_wm_cback
(
  int16 wm_value
);


/*===========================================================================
FUNCTION DSSOCKI_BIND()

DESCRIPTION
  This function either binds or unbinds the the socket appropriately
  based on the the passed sockaddr_in6 structure.

PARAMETERS
  *scb_ptr      - Pointer to the socket control block.
  *v6_sockaddr  - Pointer to the address to bind to.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 dssocki_bind
(
  scb_type            *scb_ptr,
  struct ps_sockaddr_in6 *v6_sockaddr
);

/*===========================================================================
FUNCTION DSSOCKI_UPDATE_WRITE_EV()

DESCRIPTION
  This function iterates through all the sockets/scb's and invokes the socket
  callback if client who opened the socket is interested on DS_WRITE_EV

PARAMETERS

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_update_write_ev();

/*===========================================================================
FUNCTION DSSOCKI_CLONE_SOCKET_OPTIONS()

DESCRIPTION
   This function clones the socket options set on the original TCP server socket to the sockets 
   created internally when dss_listen is invoked by the clients

PARAMETERS
    scb_ptr - The parent socket control block created when client calls dss_socket to create a TCP server socket
    lscb_ptr - Listner socket created internally by DS when dss_listen is called
RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_clone_socket_options
(
  struct scb_s *scb_ptr,
  struct scb_s *lscb_ptr
);

/*===========================================================================
FUNCTION DSSOCKI_UNLINK_SCB_FROM_PCB()

DESCRIPTION
  When the socket is closed and the scb is being freed, we need to unlink the
  scb set in the UCB/TCB/IPCB

PARAMETERS
  scb_ptr - Socket Control Block

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dssocki_unlink_scb_from_pcb(scb_type *scb_ptr);

/*===========================================================================
FUNCTION DSSOCKI_SET_LINGER_TIMER()

DESCRIPTION
  This API sets the linger timer for the TCP socket when DSS_SO_LINGER or
  DSS_SO_LINGER_RESET with non-zero is set from the application

PARAMETERS
  scb_ptr - Socket Control Block

RETURN VALUE
  void

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
sint15 dssocki_set_linger_timer(scb_type* scb_ptr, sint15 *dss_errno );



#endif /* FEATURE_DS_SOCKETS */
#endif /* FEATURE_DATA_PS */

#endif /* DS_SOCKI_H */

