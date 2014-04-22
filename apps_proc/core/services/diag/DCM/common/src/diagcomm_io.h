#ifndef DIAGCOMM_IO_H 
#define DIAGCOMM_IO_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2010-2014, 2016, 2019, 2022 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagcomm_io.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
03/24/16   gn      Added support for 12k log packet
01/13/16   is      Send feature mask before processing commands
07/23/14   rh      Cleaned up SMDL support structure
04/24/14   tbg     Updated comments for STREAM_LISTENER usage
09/20/13   tbg     Added support for Diag over STM 
06/25/14   rs      removed unused variables from diagcomm_io_conn_type.
06/03/14   rh      Changes to support 8KB cmd/rsp
03/28/14   is      Resolve 4k response issue
03/13/14   is      Fix for 4k commands
03/12/14   xy      Add support for STM status  
02/21/14   is      Command/response separation
02/11/14   xy      Fixed KW issues 
01/23/14   xy      Add support for enable/disable STM command   
01/07/14   xy      Added fusion DCI feature  
12/05/13   sr      Added drain timer for forward channel 
10/04/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
10/01/13   xy      Removed code under FEATURE_WINCE 
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/27/12   sa      Mainlining DIAG_CENTRAL_ROUTING
11/15/12   is      Support for preset masks
10/17/12   rh      Added new api declarations for Tx wmq
03/07/12   rh      Added DIAG_MIN_STREAM_ID to go with DIAG_MAX_STREAM_ID
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   wjg     Added data_pending flag to diagcomm_io_conn_type for use
                     in diagcomm_smdl_buffer_read
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/

#include "customer.h"

#ifdef DIAG_SIO_SUPPORT  
  #include "sio.h"
#endif


#include "dsm.h"                              /* For dsm_item_type */
#include "feature.h"                          /* For DIAG_INT_FEATURE_MASK_LEN */
#include "msgcfg.h"                           /* MSG_MASK_TBL_CNT */
#include "diagpkt.h"                          /* for DIAG_MAX_TX_PKT_SIZ */
#include "log_codes.h"						  /* For LOG_EQUIP_ID_MAX */
#include "diag_cfg.h"

#ifndef DIAG_SIO_SUPPORT  
  typedef int sio_bitrate_type;               /* Defined by sio.h, but used in Diag public API headers */
  typedef int rdm_device_enum_type;
#endif


#if defined(DIAG_SOCKET_SUPPORT)
#include "qsocket_ipcr.h"

typedef struct qsockaddr_ipcr ipcr_addr_t;
#endif

/* ------------------------------------------------------------------------
** Typedefs and defines
** ------------------------------------------------------------------------ */

/* Diag Error codes */
#define DIAG_EOK                              0  /**< Operation successfully performed. */
#define DIAG_EPARAM                          -1  /**< Wrong values for parameters.  */
#define DIAG_EMEM                            -2  /**< Not enough memory to perform operation.*/
#define DIAG_EINVALID                        -3  /**< Invalid argument(s) */
#define DIAG_EUNKNOWN                        -4  /**< Unknown error */
#define DIAG_EFAILED                         -5  /**< Operation failed. */
#define DIAG_ENOTALLOWED                     -6  /**< Operation not allowed. */
#define DIAG_EHEAP                           -7  /**< No heap space available. */
#define DIAG_EUNREACH                        -8  /**< Destination unreachable. */
#define DIAG_ERETRY                          -9  /**< Retry */
#define DIAG_ELIMIT                          -10 /**< Limit reached */

#define DIAGCOMM_IO_NO_STREAM_ID -1 /* SIO_NO_STREAM_ID - Returned when sio_open() fails */
#define NUM_IO_CHANNELS 2

#ifdef DIAG_CONSUMER_API
  #define NUM_SIO_PORTS 3             /* number of sio ports */
  #define NUM_SMD_PORTS 3             /* size of diagcomm_enum_port_type */
#else
  #define NUM_SIO_PORTS 1 			/* number of sio ports */
  #define NUM_SMD_PORTS 1 			/* size of diagcomm_enum_port_type */
#endif

#define NUM_SMD_PORT_TYPES 2        /* size of diagcomm_port_type */

/* For log/event/f3 mask streams */
#define DIAG_MIN_STREAM_ID          1
#ifdef DIAG_CONSUMER_API
  #define DIAG_MAX_STREAM_ID          2
#else
  #define DIAG_MAX_STREAM_ID          1
#endif /* #ifdef DIAG_CONSUMER_API */
#define DIAG_STREAM_ALL             0 /* For all streams */
#define DIAG_STREAM_1               1 /* QXDM stream */
#ifdef DIAG_CONSUMER_API
  #define DIAG_STREAM_2               2 /* DCI stream */
#endif /* #ifdef DIAG_CONSUMER_API */

/* Alias used for STM Mode (indicates which stream is piped to STM, if Diag STM is active) */
#define DIAG_STREAM_STM DIAG_STREAM_1
/* 'Listener' stream ID is used indicate packet is needed by a listener. */
/* (Streams 1 and 2 should not be used to designate listern usage) */
#define DIAG_STREAM_LISTENER 0x80

#define DIAG_PRESET_MASK_ALL        0 /* For all presets */
#define DIAG_MIN_PRESET_ID          1
#ifdef DIAG_PRESET_MASK_2_SUPPORT
  #define DIAG_MAX_PRESET_ID          2
#else
  #define DIAG_MAX_PRESET_ID          1
#endif /* DIAG_PRESET_MASK_2_SUPPORT */

#define DIAG_PRESET_MASK_1          1

#ifdef DIAG_PRESET_MASK_2_SUPPORT
  #define DIAG_PRESET_MASK_2          2
#endif

#ifdef DIAG_CONSUMER_API
  #define DIAG_DCI_MASK_1             1
  #define DIAG_MIN_DCI_ID             1
  #define DIAG_MAX_DCI_ID             1
#endif /* #ifdef DIAG_CONSUMER_API */

/* -------------------------------------------------------------------------
** Diagcomm defines for TX/RX Max Chain Size
** ------------------------------------------------------------------------- */

  /* These are here to avoid circular include problems. */
#define DIAG_TX_MAX_CHAIN_SIZ  ((DSM_DIAG_SIO_TX_ITEM_SIZ) * 2)    /* Total dsm item chain size including the first dsm item is now 16KB*/
#define DIAG_TX_MAX_CHAINED_ITEMS_SIZ  (DIAG_TX_MAX_CHAIN_SIZ - DSM_DIAG_SIO_TX_ITEM_SIZ )
#define DIAG_RX_RSP_MAX_CHAIN_SIZ ((DIAG_MAX_RX_PKT_SIZ) * 2)
#define DIAG_RX_RSP_MAX_CHAINED_ITEMS_SIZ (DIAG_RX_RSP_MAX_CHAIN_SIZ - DSM_DIAG_CMD_SIO_TX_ITEM_SIZ)

/* Max Chained Number of DSM Items */
#define DIAG_CMD_MAX_CHAINED_DSM_ITEM         (9)

typedef void (*diagcomm_close_cb_fnc_ptr_type)( void );

typedef enum
{
  DIAGCOMM_PORT_SMD = 0,
  DIAGCOMM_PORT_SIO = 1
} diagcomm_port_type;

/*lint -save -e{641} */
typedef enum
{
  DIAGCOMM_PORT_NONE = -1, /*Used to flush data fwd channel*/
  DIAGCOMM_PORT_NUM_FIRST = 0,
  DIAGCOMM_PORT_1 = 0,  
  #ifdef DIAG_CONSUMER_API
    DIAGCOMM_PORT_2 = 1,
    DIAGCOMM_PORT_FUSION_DCI = 2,
    DIAGCOMM_PORT_NUM_LAST = 2,
  #else
    DIAGCOMM_PORT_NUM_LAST = 0,
  #endif /* #ifdef DIAG_CONSUMER_API */
} diagcomm_enum_port_type;
/*lint -restore */

typedef enum
{
  DIAGCOMM_IO_DATA = 0,
  DIAGCOMM_IO_CTRL = 1, 
  DIAGCOMM_IO_CMD = 2,
  #ifdef DIAG_CONSUMER_API
    DIAGCOMM_IO_DCI = 3
  #endif /* #ifdef DIAG_CONSUMER_API */
  
} diagcomm_io_channel_type;

typedef enum
{ 
  DIAGCOMM_IO_TYPE_INVALID = -1,
  DIAGCOMM_IO_TYPE_SIO = 0,
  DIAGCOMM_IO_TYPE_SMDL = 1,
  DIAGCOMM_IO_TYPE_IPCR_UDP = 2
} diagcomm_io_type;


typedef union 
{  
#ifdef DIAG_SIO_SUPPORT
  sio_stream_id_type  sio_handle;  /* int2 */
#endif


} diagcomm_io_handle_type;


typedef union
{
#ifdef DIAG_SIO_SUPPORT
  struct
  {
    sio_open_type                  params;
    sio_open_param_type            open_param;
    diagcomm_close_cb_fnc_ptr_type close_func_ptr;
  } sio_params;
#endif
 
    

} diagcomm_io_open_params_type;


typedef union
{
  dsm_item_type      *dsm_ptr;               /* Both uses DSMs for now */

#ifdef DIAG_SMDL_SUPPORT  
  struct
  {
    uint32            len;
    void             *buffer;
    uint32            flags;
  } smdl_params;                             /* Unused now */
#endif

} diagcomm_io_tx_params_type;


typedef struct 
{
  diagcomm_io_type io_type;                  /* IO_TYPE_SIO or IO_TYPE_SMDL */
  diagcomm_enum_port_type port_num;          /* PORT_1, PORT_2, etc */
  diagcomm_port_type port_type;              /* PORT_SMD or PORT_SIO */
  diagcomm_io_channel_type channel_type;     /* IO_DATA or IO_CTRL */
  boolean connected;                         /* Port connection state */
  boolean channel_connected;                 /* Channel (both ports) connection state */
  diagcomm_io_handle_type handle;            /* SIO port ID / SMDL edge */
  diagcomm_io_open_params_type open_params;  /* Params to diagcomm_io_open() */
  boolean data_pending;                       /* Connection waiting to be read from */
  boolean allow_flow;
  /* Needed for CTRL channel only */
#if defined(DIAG_MP_MASTER)
  uint8 num_presets;                                                        /* Number of preset masks supported by remote proc */
  dword mask_preset_update[DIAG_MAX_PRESET_ID];                             /* Bitmask to keep track of which masks needs to be sent to Slaves */
  #ifdef DIAG_CONSUMER_API
    dword mask_dci_update[DIAG_MAX_DCI_ID];
    boolean log_mask_dci_update[DIAG_MAX_DCI_ID][LOG_EQUIP_ID_MAX+1];
  #endif /* DIAG_CONSUMER_API */
  boolean * log_mask_update[DIAG_MAX_STREAM_ID];
  boolean log_mask_preset_update[DIAG_MAX_PRESET_ID][LOG_EQUIP_ID_MAX+1];         /* Keeps track of which equip id log mask has been updated for Stream 1 */
#endif  
  uint8 stm_status; 
  byte feature_mask[DIAG_INT_FEATURE_MASK_LEN];                             /* Diag feature mask of remote proc */
  boolean feature_mask_sent;                                                /* True if feature mask has been sent. */
  dsm_watermark_type * rx_wmq_ptr;
  dsm_watermark_type * tx_wmq_ptr;

boolean diag_prev_tx_pending;
  boolean diag_resend_data;         /*Flag to track if we are trying to resend the data after SMDL signalled us*/

  void *buf;                        /* send buffer pointer */
  uint32 buf_length;                /* length of the send buffer */
  uint32 used;                      /* number of used bytes in the buffer */
  osal_mutex_arg_t buf_mutex;       /* mutex to protect the "used" field */
  diagcomm_io_type change_to_type;  /* connection type to be changed to */

#if defined(DIAG_SOCKET_SUPPORT)
  boolean isServer;                 /* set if this end point in socket connection is a server*/
  ipcr_addr_t addr;                 /* socket connection address*/
  int sock_handle;                  /* socket handle*/
  int send_flags;                   /* set to QMSG_DONTWAIT : Send will block on flow control, recv will block if there are no packets to receive */
  int recv_flags;                   /* receieve flags; not used currently*/
#endif
  int last_error;                   /* last error while transmitting data*/
} diagcomm_io_conn_type;

/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */
void diagcomm_io_init( void );
void diagcomm_io_open_init( diagcomm_io_conn_type * conn );
boolean diagcomm_io_open( diagcomm_io_conn_type * conn );
void diagcomm_io_close( diagcomm_io_conn_type * conn );
#if defined(DIAG_FLASH_LOGGING_SUPPORT)
  boolean diagcomm_flash_transmit(diagcomm_io_tx_params_type * tx_param);
#endif
int diagcomm_io_transmit( diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param );
void diagcomm_io_ioctl( diagcomm_io_conn_type * conn, uint32 cmd, void * param );
void diagcomm_io_read( diagcomm_io_conn_type * conn );
boolean diagcomm_io_status( diagcomm_io_conn_type * conn );
void diagcomm_process_data( void );
#ifdef DIAG_CONSUMER_API
  void diagcomm_process_data_dci( void );
#endif /* #ifdef DIAG_CONSUMER_API */


void diagcomm_io_enable_flow( diagcomm_io_conn_type * conn );
void diagcomm_io_disable_flow( diagcomm_io_conn_type * conn );
boolean diagcomm_is_ondevice_mode( diagcomm_io_conn_type * conn );

void diagcomm_io_set_port( diagcomm_io_conn_type * conn );
void diagcomm_io_flush_tx( diagcomm_io_conn_type * conn, void (*flush_cb) (void),unsigned int timeoutVal );

uint32 diagcomm_io_get_wmq_cnt( dsm_watermark_type * wmq_ptr );

dsm_item_type * diagcomm_io_dequeue_rx_wmq( diagcomm_io_conn_type * conn );



dsm_item_type * diagcomm_io_dequeue_tx_wmq( diagcomm_io_conn_type * conn );
void diag_io_detect_connection( diagcomm_io_conn_type * conn );
void diagcomm_io_conn_changed( diagcomm_io_conn_type * conn, boolean connected );
void diagcomm_io_notify_rx(diagcomm_io_conn_type *conn);

#if defined(DIAG_MP_MASTER)
void diagcomm_mask_update( dword mask, uint8 stream_id, uint8 preset_id, boolean send_now );
#endif

#endif //DIAGCOMM_IO_H
