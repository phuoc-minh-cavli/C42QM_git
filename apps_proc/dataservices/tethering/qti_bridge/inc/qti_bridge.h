#ifndef QTI_BRIDGE_H
#define QTI_BRIDGE_H
/*===========================================================================
                         QTI_AT_DUN_BRIDGE.H

DESCRIPTION
  This header file contains definitions used internally by the QTI AT DUN
  Module.

  Copyright (c)2016-2020 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
08/02/2016   sr     created file
==============================================================================*/

#ifndef FEATURE_DSM_WM_CB
#define FEATURE_DSM_WM_CB
#endif

#include "comdef.h"
#include "dsm.h"
#ifndef MOB_TX
#include "sio.h"
#include "rdevmap.h"
#else
#include "sio_mob.h"
#include "rdevmap_mob.h"
#endif

#include "stdlib.h"
#include "err.h"
#include "msgcfg.h"
#include "amssassert.h"
#include "rcinit.h"
#include "stdint.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "ds_mux_defs.h"
#include "at_fwd_ext_api.h"
#include "qapi_atfwd.h"
#include "access_terminal_service_v01.h"
#include "qapi_diag.h"

#define QTI_BRIDGES_MAX 2
#define QTI_BRIDGES_DS_MUX_CHANNEL_MAX 3
#define QTI_BRIDGE_MAX_AT_TOKENS 500

#define QTI_BRIDGE_AT_MASK_EMPTY  0  /**<  Denotes a feed back mechanism for AT reg and de-reg */
#define QTI_BRIDGE_AT_MASK_NA_V01 0x01
#define QTI_BRIDGE_AT_MASK_EQ_V01 0x02
#define QTI_BRIDGE_AT_MASK_QU_V01 0x04
#define QTI_BRIDGE_AT_MASK_AR_V01 0x08

#define QTI_BRIDGE_CMUX "+CMUX"
#define QTI_BRIDGE_IPR "+IPR"
#define QTI_BRIDGE_QCEMBP "$QCEMBP"

#define QTI_BRIDGE_AT_CMD_CMUX 1
#define QTI_BRIDGE_AT_CMD_IPR  2
#define QTI_BRIDGE_AT_CMD_QCEMBP 3

//#define QTI_LOGGING_ENABLED 1

/*-----------------------------------------------------------------------------
  Baud Rate Macros
-----------------------------------------------------------------------------*/
#define QTI_BRIDGE_BAUD_RATE_300                    300
#define QTI_BRIDGE_BAUD_RATE_600                    600
#define QTI_BRIDGE_BAUD_RATE_1200                   1200
#define QTI_BRIDGE_BAUD_RATE_2400                   2400
#define QTI_BRIDGE_BAUD_RATE_4800                   4800
#define QTI_BRIDGE_BAUD_RATE_9600                   9600
#define QTI_BRIDGE_BAUD_RATE_14400                  14400
#define QTI_BRIDGE_BAUD_RATE_19200                  19200
#define QTI_BRIDGE_BAUD_RATE_38400                  38400
#define QTI_BRIDGE_BAUD_RATE_57600                  57600
#define QTI_BRIDGE_BAUD_RATE_115200                 115200
#define QTI_BRIDGE_BAUD_RATE_230400                 230400
#define QTI_BRIDGE_BAUD_RATE_460800                 460800
#define QTI_BRIDGE_BAUD_RATE_921600                 921600
#define QTI_BRIDGE_BAUD_RATE_1000000                1000000
#define QTI_BRIDGE_BAUD_RATE_1200000                1200000
#define QTI_BRIDGE_BAUD_RATE_1209677                1209677
#define QTI_BRIDGE_BAUD_RATE_1250000                1250000
#define QTI_BRIDGE_BAUD_RATE_1293103                1293103
#define QTI_BRIDGE_BAUD_RATE_1339286                1339286
#define QTI_BRIDGE_BAUD_RATE_1388889                1388889
#define QTI_BRIDGE_BAUD_RATE_1442308                1442308
#define QTI_BRIDGE_BAUD_RATE_1500000                1500000
#define QTI_BRIDGE_BAUD_RATE_1562500                1562500
#define QTI_BRIDGE_BAUD_RATE_1630435                1630435
#define QTI_BRIDGE_BAUD_RATE_1704545                1704545
#define QTI_BRIDGE_BAUD_RATE_1785714                1785714
#define QTI_BRIDGE_BAUD_RATE_2000000                2000000
#define QTI_BRIDGE_BAUD_RATE_3000000                3000000
#define QTI_BRIDGE_BAUD_RATE_3200000                3200000
#define QTI_BRIDGE_BAUD_RATE_3686400                3686400
#define QTI_BRIDGE_BAUD_RATE_4000000                4000000

/*-----------------------------------------------------------------------------
Macros for  QTI_Bridge instances
-----------------------------------------------------------------------------*/
#define QTI_BRIDGE_INSTANCE_0 0
#define QTI_BRIDGE_INSTANCE_1 1

/*-----------------------------------------------------------------------------
  Internal Macros
-----------------------------------------------------------------------------*/
#define QTI_BRIDGE_CNTL_LO_WM_LEVEL (1024)
#define QTI_BRIDGE_CNTL_HI_WM_LEVEL (4 * QTI_BRIDGE_CNTL_LO_WM_LEVEL)

#define QTI_BRIDGE_DATA_LO_WM_LEVEL (4 * 1024)
#define QTI_BRIDGE_DATA_HI_WM_LEVEL (4 * QTI_BRIDGE_DATA_LO_WM_LEVEL)

#define QTI_BRIDGE_DNE_WM_LEVEL (2 * QTI_BRIDGE_DATA_HI_WM_LEVEL)

#ifdef CHILTEPIN_UART_PORT
#define QTI_BRIDGE_UART_PORT SIO_PORT_UART_MAIN
#else
#define QTI_BRIDGE_UART_PORT SIO_PORT_UART_AUX
#endif 

/* Macro to clear the unused warning while documenting the non use. */
#ifndef UNUSED
#define UNUSED(x) (void)x;
#endif


typedef enum
{
  QTI_BRIDGE_MIN_CMD_EVT = -1,
  QTI_BRIDGE_RDM_OPEN_EVT,
  QTI_BRIDGE_RDM_CLOSE_EVT,
  QTI_BRIDGE_SMD_QTI_RECV_EVT,
  QTI_BRIDGE_PHY_QTI_RECV_EVT,
  QTI_BRIDGE_SMD_QTI_DTR_EVT,
  QTI_BRIDGE_PHYS_QTI_DTR_EVT,
  QTI_BRIDGE_SMD_QTI_CD_EVT,
  QTI_BRIDGE_SMD_QTI_RI_EVT,
  QTI_BRIDGE_PHY_LOW_WM_EVT,
  QTI_BRIDGE_PHY_HIGH_WM_EVT,
  QTI_BRIDGE_SMD_LOW_WM_EVT,
  QTI_BRIDGE_SMD_HIGH_WM_EVT,
  QTI_BRIDGE_ATFWD_EVT,
  QTI_BRIDGE_AT_POST_PROCESS_EVT,
  QTI_BRIDGE_DS_MUX_SMD_QTI_RECV_EVT,
  QTI_BRIDGE_DS_MUX_LOGICAL_LOW_WM_EVT,
  QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_WM_EVT,
  QTI_BRIDGE_DS_MUX_SMD_QTI_DTR_EVT,
  QTI_BRIDGE_DS_MUX_SMD_QTI_CD_EVT,
  QTI_BRIDGE_DS_MUX_SMD_QTI_RI_EVT,
  QTI_BRIDGE_DS_MUX_LOGICAL_QTI_DTR_EVT,
  QTI_BRIDGE_DS_MUX_LOGICAL_QTI_CD_EVT,
  QTI_BRIDGE_DS_MUX_LOGICAL_QTI_RI_EVT,
  QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE,
  QTI_BRIDGE_ATFWD_QMI_SERVICE_UP_ENT,
  QTI_BRIDGE_CMUX_POST_PROCESS_EVT,
  QTI_BRIDGE_MAX_CMD_EVT
} qti_bridge_cmd_id_e;


typedef enum
{
  QTI_BRIDGE_PHYS_PORT,
  QTI_BRIDGE_SMD_PORT,
  QTI_BRIDGE_DS_MUX_LOGICAL_PORT
}qti_bridge_port_type;


typedef enum
{
  QTI_BRIDGE_DS_SMD = 6,
  QTI_BRIDGE_SMD_DATA1 = 7,
  QTI_BRIDGE_SMD_DATA2 = 8,
  QTI_BRIDGE_SMD_DATA3 = 9,
  QTI_BRIDGE_SMD_DATA4 = 10,
  QTI_BRIDGE_SMD_DATA11 = 11
}qti_bridge_smd_channel;



/*------------------------------------------------------------------------------
  Signal Related Macros
------------------------------------------------------------------------------*/
#define QTI_BRIDGE_SIGNAL_MASK       0x00000001

#define QTI_BRIDGE_TASK_IS_SIG_SET(signal, signal_mask)  ((signal & signal_mask) != 0)

#define QTI_BRIDGE_TASK_CLEAR_SIG(signal, signal_mask) \
                                    qurt_signal_clear(&signal,signal_mask)

#define QTI_BRIDGE_TASK_SET_SIG(signal, signal_mask) \
                                    qurt_signal_set(&signal, signal_mask)


/*------------------------------------------------------------------------------
  Command Queue Related Macros
------------------------------------------------------------------------------*/

#define QTI_CMD_ID_RANGE_CHECK(cmd_id) \
                                       ( (cmd_id > QTI_BRIDGE_MIN_CMD_EVT) && \
                                         (cmd_id < QTI_BRIDGE_MAX_CMD_EVT) )


/*------------------------------------------------------------------------------
  Typedef Declaration
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
  This is the command queue that will be used to enqueue commands from
  external entities
------------------------------------------------------------------------------*/
typedef q_type qti_bridge_task_cmd_q;

typedef void (*qti_bridge_cb_func_ptr_type)(void);

typedef void (*sio_close_func_ptr_type)(void);


/*------------------------------------------------------------------------------
  Structure declaration
------------------------------------------------------------------------------*/

/* This structure holds all of the information on the bridge */
typedef struct
{
  /* smd port information */
  sio_port_id_type     smd_port_id;       /* sio port id of the smd port */
  sio_stream_id_type   smd_stream_id;     /* sio stream id of the smd port */
  dsm_watermark_type   smd_tx_wm;         /* tx wm queue for the smd port */
  q_type               smd_tx_queue;      /* scratch queue for the dsm items */
  uint32               smd_fifo_size;     /* FIFO size for SMD port */
  boolean              usb_sps;           /* USB pipe is in SPS(BAM) mode */

  /* physical port information */
  sio_port_id_type     phys_port_id;      /* sio port id of the physical port */
  sio_stream_id_type   phys_stream_id;    /* sio stream id of the physical port */
  dsm_watermark_type   phys_tx_wm;        /* tx wm queue for the physical port */
  q_type               phys_tx_queue;     /* scratch queue for the dsm items */

  boolean              control;           /* specifies this as a control port */
  void                (*close_cb)(void*); /* the client close cb function;
                                             currently unused */

  sio_stream_id_type   uart_stream_id;    /* sio stream id of the UART port */
  sio_bitrate_type     uart_baud_rate;         /* Baud rate of the UART port*/
}qti_bridge_info_t;

/* This structure holds all of the information on the bridge */
typedef struct
{
  /* smd port information */
  sio_port_id_type     smd_port_id[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];       /* sio port id of the smd port */
  sio_stream_id_type   smd_stream_id[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];     /* sio stream id of the smd port */
  dsm_watermark_type   smd_tx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];         /* tx wm queue for the smd port */
  q_type               smd_tx_queue[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];      /* scratch queue for the dsm items */
  uint32               smd_fifo_size;     /* FIFO size for SMD port */
  boolean              usb_sps;           /* USB pipe is in SPS(BAM) mode */

  /*Logical Channel information*/
  sio_port_id_type     logical_port_id[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];       /* sio port id of the smd port */
  sio_stream_id_type   logical_stream_id[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];     /* sio stream id of the smd port */
  dsm_watermark_type   logical_tx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];         /* tx wm queue for the smd port */
  q_type               logical_tx_queue[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];      /* scratch queue for the dsm items */
  dsm_watermark_type   logical_rx_wm[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];         /* tx wm queue for the smd port */
  q_type               logical_rx_queue[QTI_BRIDGES_DS_MUX_CHANNEL_MAX];      /* scratch queue for the dsm items */

  boolean              control;           /* specifies this as a control port */
  void                (*close_cb)(void*); /* the client close cb function;
                                             currently unused */
}qti_bridge_ds_mux_info_t;


typedef struct
{
  qurt_signal_t                qti_bridge_signal;
  qurt_mutex_t                 qti_bridge_mutex;
}qti_bridge_qurt_t;

typedef struct{
  q_link_type                 link;
  qti_bridge_cmd_id_e         cmd_id;
} qti_bridge_cmd_hdr_t;


typedef struct{
  uint32                      bridge_id;
  sio_port_id_type            port_id;
  dsm_item_type               *dsm_item_ptr;
  ds_mux_ext_client_event_enum_type  event_name;
} qti_bridge_cmd_data_t;


typedef struct{
  qti_bridge_cmd_hdr_t               cmd_hdr;
  qti_bridge_cmd_data_t              data;
} qti_bridge_cmd_t;

/*==============================================================================

FUNCTION qti_bridge_enable_phys_0

DESCRIPTION
  dsm callback that enables the physical port for qti_bridge instance 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_enable_phys_0(dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION qti_bridge_enable_phys_1

DESCRIPTION
  dsm callback that enables the physical port for qti_bridge instance 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_enable_phys_1(dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_SMD_0

DESCRIPTION
  Dsm callback that enables the SMD port for instance of qti_bridge 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_enable_smd_0 (dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_SMD_1

DESCRIPTION
  Dsm callback that enables the SMD port for instance of qti_bridg 1
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/

void qti_bridge_enable_smd_1 (  dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_SMD_0

DESCRIPTION
  Dsm callback that disables the SMD port by queuing for instance of qti_bridge 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disable_smd_0 (  dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_SMD_1

DESCRIPTION
  Dsm callback that disables the SMD port for instance of qti_bridge 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disable_smd_1 (  dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION qti_bridge_disable_phys_0

DESCRIPTION
  dsm callback that disables the physical port for qti_bridge instance 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_disable_phys_0(dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION qti_bridge_disable_phys_1

DESCRIPTION
  dsm callback that disables the physical port for qti_bridge instance 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_disable_phys_1(dsm_watermark_type *ignore, void *bridge_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_SMD_TO_QTI_CB_0

DESCRIPTION
  Callback for handling DTR signal from SMD to QTI for qti_bridge instance 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_dtr_smd_to_qti_cb_0(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_SMD_TO_QTI_CB_1

DESCRIPTION
  Callback for handling DTR signal from SMD to QTI for qti_bridge instance 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_dtr_smd_to_qti_cb_1(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_CD_SMD_TO_QTI_CB_0

DESCRIPTION
  Callback for handling CD (Carrier Detect) signal from SMD to QTI for
  qti_bridge instance 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_cd_smd_to_qti_cb_0(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_CD_SMD_TO_QTI_CB_1

DESCRIPTION
  Callback for handling CD (Carrier Detect) signal from SMD to QTI for
  qti_bridge instance 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_cd_smd_to_qti_cb_1(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_RI_SMD_TO_QTI_CB_0

DESCRIPTION
  Callback for handling RI (Ring Indicator)signal from SMD to QTI for
  qti_bridge instance 0
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ri_smd_to_qti_cb_0(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_RI_SMD_TO_QTI_CB_1

DESCRIPTION
  Callback for handling RI (Ring Indicator)signal from SMD to QTI for
  qti_bridge instance 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ri_smd_to_qti_cb_1(void *info);


/*==============================================================================

FUNCTION QTI_BRIDGE_RDM_OPEN_CB

DESCRIPTION
  RDM invokes this callback when it has enumerated devices associated
  with DUN service transport.

DEPENDENCIES
  None.

RETURN VALUE
  uint32 to indicate Init API successfully invoked

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_rdm_open_cb(sio_port_id_type port_id, void *data);

/*==============================================================================

FUNCTION QTI_BRIDGE_RDM_CLOSE_CB

DESCRIPTION
  RDM invokes this callback when it wants to close SIO port
  associated with DUN service transport.

DEPENDENCIES
  None.

RETURN VALUE
  uint32 to indicate Init API successfully invoked

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_rdm_close_cb(sio_port_id_type port_id, void *data);

/*==============================================================================

FUNCTION QTI_BRIDGE_INIT

DESCRIPTION
  Init API called from SIO layer to initialize the QTI AT DUN Bridge

DEPENDENCIES
  None.

RETURN VALUE
  uint32 to indicate Init API successfully invoked

SIDE EFFECTS
  None

==============================================================================*/
uint32 qti_bridge_init(sio_register_device_driver_cb_type unused);

/*==============================================================================

FUNCTION QTI_BRIDGE_RDM_REGISTRATION_INIT

DESCRIPTION
  Register callbacks with RDM framework to detect the peripheral availability

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_rdm_registration_init(void);


/*===========================================================================

FUNCTION QTI_BRIDGE_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  qti_rmnet_config_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void qti_bridge_cmdq_init (void);

/*===========================================================================

FUNCTION QTI_BRIDGE_GET_CMD_BUFFER

DESCRIPTION
  This function is used to allocate a command buffer that the client can
  then enqueue into the command queue.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the command

SIDE EFFECTS
  None

=========================================================================*/
qti_bridge_cmd_t* qti_bridge_get_cmd_buffer(void);


/*===========================================================================

FUNCTION  QTI_BRIDGE_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the QTI queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void qti_bridge_enqueue_cmd(qti_bridge_cmd_t* cmd_ptr);

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_COMMANDS()

DESCRIPTION
  This function dequeues all outstanding commands from and dispatches the
  processor functions.

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_process_commands(void);



/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_OPEN()

DESCRIPTION
  This function opens the peripheral and SMD SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_open(sio_port_id_type port_id);

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_CLOSE()

DESCRIPTION
  This function Closes the SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_close(sio_port_id_type port_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_RECV_SMD_TO_QTI_0

DESCRIPTION
  Register callbacks for each bridge channel for communication from SMD to QTI
  for qti_bridge instnce 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_recv_smd_to_qti_0(dsm_item_type **item);

/*==============================================================================

FUNCTION QTI_BRIDGE_RECV_SMD_TO_QTI_1

DESCRIPTION
  Register callbacks for each bridge channel for communication from SMD to QTI
  for qti_bridge instance 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_recv_smd_to_qti_1(dsm_item_type **item);


/*==============================================================================

FUNCTION QTI_BRIDGE_FORWARD_PHYS_TO_QTI_0

DESCRIPTION
  Register callbacks for each bridge channel for communication from Physical
  interconnect to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_recv_phys_to_qti_0 (dsm_item_type **item);

/*==============================================================================

FUNCTION QTI_BRIDGE_FORWARD_PHYS_TO_QTI_1

DESCRIPTION
  Register callbacks for each bridge channel for communication from Physical
  interconnect to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_recv_phys_to_qti_1 (dsm_item_type **item);

/*==============================================================================
FUNCTION QTI_BRIDGE_FORWARD()

DESCRIPTION
  This function forwards the DSM items to the next hop (SMD/Physical interconnect)

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_forward
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id,
  dsm_item_type *dsm_item_ptr
);


/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_PHYS_TO_QTI_0

DESCRIPTION
  Register callbacks for each bridge channel for handling DTR status changes
   for qti_bridge instance 0

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_dtr_phys_to_qti_0(void);

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_PHYS_TO_QTI_1

DESCRIPTION
  Register callbacks for each bridge channel for handling DTR status changes
   for qti_bridge instances 1

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_dtr_phys_to_qti_1(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_SET_DTR()

DESCRIPTION
  This function sets the DTR signal

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_set_dtr
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_SET_CD()

DESCRIPTION
  This function sets the CD signal on physical interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_set_cd (uint32 bridge_id);

/*==============================================================================
FUNCTION QTI_BRIDGE_SIO_SET_RI()

DESCRIPTION
  This function sets the RI signal on physical interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_sio_set_ri (uint32 bridge_id);

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_SMD_TO_QTI_CB

DESCRIPTION
  Callback for handling DTR signal from SMD to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_dtr_smd_to_qti_cb(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_DTR_SMD_TO_QTI_CB

DESCRIPTION
  Callback for handling CD (Carrier Detect) signal from SMD to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_cd_smd_to_qti_cb(void *info);

/*==============================================================================

FUNCTION QTI_BRIDGE_RI_SMD_TO_QTI_CB

DESCRIPTION
  Callback for handling RI signal from SMD to QTI

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ri_smd_to_qti_cb(void *info);

/*==============================================================================
FUNCTION QTI_BRIDGE_SET_FLOW_CTL

DESCRIPTION
  Set the flow control on the given port

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_set_flow_ctl
(
  uint32                      bridge_id,
  qti_bridge_port_type        port_type,
  boolean                     enable
);

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_PHYS

DESCRIPTION
  dsm callback that enables the physical port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_enable_phys
(
  dsm_watermark_type *ignore,
  int                 bridge_id
);
/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_PHYS

DESCRIPTION
  dsm callback that disables the physical port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disable_phys
(
  dsm_watermark_type *ignore,
  int              bridge_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_ENABLE_SMD

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_enable_smd
(
  dsm_watermark_type *ignore,
  int                 bridge_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_SMD

DESCRIPTION
  Dsm callback that enables the SMD port by queueing a deferred
  procedure call.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disable_smd
(
  dsm_watermark_type *ignore,
  int                 bridge_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_DISCONNECT_SMD_0

DESCRIPTION
  Register callbacks for each bridge channel which is called when the SIO port
  is closed

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disconnect_smd_0(void);

/*==============================================================================

FUNCTION QTI_BRIDGE_DISCONNECT_SMD_1

DESCRIPTION
  Register callbacks for each bridge channel which is called when the SIO port
  is closed

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_disconnect_smd_1(void);

/*==============================================================================

FUNCTION QTI_BRIDGE_SET_SIGNAL

DESCRIPTION
  Set the SIGNAL for QTI Bridge

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_set_signal(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_AT_CMDS

DESCRIPTION
  This function processes the various AT cmds handled in QTI Bridge
  
PARAMETERS
  NONE

RETURN VALUE
  None
  
DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_process_at_cmds(uint32 at_cmd_id);

/*==============================================================================
FUNCTION QTI_BRIDGE_VALIDATE_ATFWD_CMUX_PARAMS

DESCRIPTION
  This function validates all DS MUX AT command parameters
  received from AT-FWD and stores them  and sends validation status to
  AT-FWD

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_validate_atfwd_cmux_params(void);

/*==============================================================================
FUNCTION qti_bridge_tokenize_cmux_input()

DESCRIPTION
  This function tokenizes the AT+CMUX parameter input to individual cmux
  parameters

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_tokenize_cmux_input(void);

/*==============================================================================
FUNCTION qti_bridge_get_current_cmux_params()

DESCRIPTION
  This function gets the current CMUX parameters

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_get_current_cmux_params(void);


/*==============================================================================
FUNCTION qti_bridge_get_supported_cmux_params()

DESCRIPTION
  This function gets the supported CMUX parameters values

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_get_supported_cmux_params(void);


/*==============================================================================
FUNCTION qti_bridge_validate_cmux_params()

DESCRIPTION
  This function validates all DS MUX AT command parameters for their values
  received from AT-FWD and stores them if all are valid

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
boolean qti_bridge_validate_cmux_params(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SMD_CH_OPEN

DESCRIPTION
  Open the SMD SIO channels for DS MUX operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_smd_ch_open(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_CH_OPEN

DESCRIPTION
  Open the LOGICAL SIO channels for DS MUX operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_logical_ch_open(void);


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SMD_SIO_OPEN

DESCRIPTION
  Open the SMD SIO channels for DS MUX operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_smd_sio_open
(
  uint8_t index,
  sio_port_id_type smd_port_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_FORWARD_SMD_TO_QTI

DESCRIPTION
  Register callbacks for each bridge channel for communication from SMD to
  DS MUX Logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_smd_to_qti_0(dsm_item_type **item);

void qti_bridge_ds_mux_recv_smd_to_qti_1(dsm_item_type **item);

void qti_bridge_ds_mux_recv_smd_to_qti_2(dsm_item_type **item);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_ENABLE_LOGICAL

DESCRIPTION
  dsm callback that enables the logical port

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_enable_logical_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_enable_logical_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_enable_logical_2(dsm_watermark_type *item, void* unused);


/*==============================================================================

FUNCTION QTI_BRIDGE_DISABLE_LOGICAL

DESCRIPTION
  dsm callback that Disable the logical port

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_disable_logical_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_disable_logical_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_disable_logical_2(dsm_watermark_type *item, void* unused);


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_FORWARD()

DESCRIPTION
  This function opens the peripheral and SMD SIO ports

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_forward
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id,
  dsm_item_type *dsm_item_ptr
);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SET_FLOW_CTL

DESCRIPTION
  Set the flow control on the given port

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_set_flow_ctl
(
  uint32                      bridge_id,
  qti_bridge_port_type        port_type,
  boolean                     enable
);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_SIO_OPEN

DESCRIPTION
  Open the LOGICAL SIO channels for DS MUX operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_logical_sio_open
(
  uint8_t index,
  sio_port_id_type smd_port_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_LOW_TX_WM

DESCRIPTION
  Call back which will be invoked when LOW WM is hit on logical MUX channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_low_tx_wm_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_low_tx_wm_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_low_tx_wm_2(dsm_watermark_type *item, void* unused);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_TX_WM

DESCRIPTION
  Call back which will be invoked when HIGH WM is hit on TX MUX channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_high_tx_wm_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_high_tx_wm_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_high_tx_wm_2(dsm_watermark_type *item, void* unused);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_LOW_RX_WM

DESCRIPTION
  Call back which will be invoked when LOW WM is hit on RX logical MUX channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_low_rx_wm_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_low_rx_wm_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_low_rx_wm_2(dsm_watermark_type *item, void* unused);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_HIGH_RX_WM

DESCRIPTION
  Call back which will be invoked when HIGH WM is hit on RX logical MUX channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_high_rx_wm_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_high_rx_wm_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_high_rx_wm_2(dsm_watermark_type *item, void* unused);


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_RX_WM_NON_EMPTY_FUNC

DESCRIPTION
  Call back which will be invoked when there is a DSM item to be processed in
  the RX direction

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_rx_wm_non_empty_func_0(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_rx_wm_non_empty_func_1(dsm_watermark_type *item, void* unused);

void qti_bridge_ds_mux_logical_rx_wm_non_empty_func_2(dsm_watermark_type *item, void* unused);


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_SEND_DTR_SIGNAL

DESCRIPTION
  Process the DTR callback from SMD and send to DS MUX logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_dtr_signal
(
  uint8 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_SET_DTR()

DESCRIPTION
  This function sets the DTR signal on the appropriate DS MUX LogicalStream ID

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_set_dtr
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_DTR_SMD_TO_QTI_CB

DESCRIPTION
  Callback to handle change in DTR from SMD channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_dtr_smd_to_qti_cb_0 (void* cb_data);

void qti_bridge_ds_mux_dtr_smd_to_qti_cb_1 (void* cb_data);

void qti_bridge_ds_mux_dtr_smd_to_qti_cb_2 (void* cb_data);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_DTR_LOGICAL_TO_QTI_CB

DESCRIPTION
  Callback to handle change in DTR from DS MUX Logical channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_dtr_logical_to_qti_cb_0 (void* cb_data);

void qti_bridge_ds_mux_dtr_logical_to_qti_cb_1 (void* cb_data);

void qti_bridge_ds_mux_dtr_logical_to_qti_cb_2 (void* cb_data);


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_SEND_CD_SIGNAL

DESCRIPTION
  Process the CD callback from SMD and send to DS MUX logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_cd_signal
(
  uint8 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_SET_CD()

DESCRIPTION
  This function sets the CD signal on DS MUX logical channel interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_set_cd
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_CD_SMD_TO_QTI_CB

DESCRIPTION
  Callback to handle change in CD from SMD channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_cd_smd_to_qti_cb_0 (void* cb_data);

void qti_bridge_ds_mux_cd_smd_to_qti_cb_1 (void* cb_data);

void qti_bridge_ds_mux_cd_smd_to_qti_cb_2 (void* cb_data);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_CD_LOGICAL_TO_QTI_CB

DESCRIPTION
  Callback to handle change in CD from DS MUX Logical channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_cd_logical_to_qti_cb_0 (void* cb_data);

void qti_bridge_ds_mux_cd_logical_to_qti_cb_1 (void* cb_data);

void qti_bridge_ds_mux_cd_logical_to_qti_cb_2 (void* cb_data);


/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_SEND_CD_SIGNAL

DESCRIPTION
  Process the RI callback from SMD and send to DS MUX logical channel

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_recv_ri_signal
(
  uint8 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_SET_RI()

DESCRIPTION
  This function sets the RI signal on DS MUX Logical interconnect

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_set_ri
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_RI_SMD_TO_QTI_CB

DESCRIPTION
  Callback to handle change in RI from SMD channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_ri_smd_to_qti_cb_0 (void* cb_data);

void qti_bridge_ds_mux_ri_smd_to_qti_cb_1 (void* cb_data);

void qti_bridge_ds_mux_ri_smd_to_qti_cb_2 (void* cb_data);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_RI_LOGICAL_TO_QTI_CB

DESCRIPTION
  Callback to handle change in RI from DS MUX Logical channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_ri_logical_to_qti_cb_0 (void* cb_data);

void qti_bridge_ds_mux_ri_logical_to_qti_cb_1 (void* cb_data);

void qti_bridge_ds_mux_ri_logical_to_qti_cb_2 (void* cb_data);


/*==============================================================================
FUNCTION DS_MUX_UPDATE_PHYS_RX_WMK()

DESCRIPTION
  This function will fill the DSM Item received from Physical interconnect
  into the DS MUX PHYS RX water mark

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_mux_update_phys_rx_wmk(dsm_item_type *dsm_item_ptr);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_ENTER_MUX_MODE()

DESCRIPTION
  1. Set DTR low on the DS SMD channel
  2. Open the SMD_DATA1, SMD_DATA2 and SMD_DATA3 channels
  3. Open the DATA_MUX1, DATA_MUX2 and DATA_MUX3 logical channels
  4. Set Global variable to indicate DS MUX mode of operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_enter_mux_mode(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_ENTER_MUX_MODE()

DESCRIPTION
  1. Call DS MUX De-init
  2. Set DTR Low on the SMD_DATA1, SMD_DATA2 and SMD_DATA3 channels
  3. Set DTR High on the DS SMD channel
  4. ReSet Global variable to switch to legacy mode of operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_leave_mux_mode(void);


/*==============================================================================
FUNCTION QTI_BRIDGE_CHANGE_DTR()

DESCRIPTION
  Change the DTR status on the appropriate channel

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_change_dtr
(
  boolean              dtr_status,
  sio_stream_id_type   stream_id
);


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_EVENT_CB()

DESCRIPTION
  This callback function will handle the events set by the DS MUX module

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/

void qti_bridge_ds_mux_event_cb
(
  ds_mux_ext_client_event_enum_type event_name, 	/* Event name  */
  void *                            event_payload,	/* Info assosiated to event: If Any else NULL  */
  void *                            cb_user_data	/* User data specified during registration */
);




/*==============================================================================

FUNCTION QTI_BRIDGE_LOGICAL_CHANNEL_CLOSE

DESCRIPTION
  Close the DS MUX logical channels opened by QTI Bridge at time of DS MUX
  active init

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_logical_channel_close(void);

/*==============================================================================

FUNCTION QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE_CB

DESCRIPTION
  SIO close CB for DS MUX Logical Channels

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_ds_mux_logical_sio_close_cb_0(void);
void qti_bridge_ds_mux_logical_sio_close_cb_1(void);
void qti_bridge_ds_mux_logical_sio_close_cb_2(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_SIO_CLOSE()

DESCRIPTION
  This function handles the SIO close for DS MUX channels Logical/SMD

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_sio_close
(
  uint32 bridge_id,
  qti_bridge_cmd_id_e  evt_id
);

/*==============================================================================
FUNCTION DS_MUX_GET_CMUX_PARAMS()

DESCRIPTION
  This function get the CMUX params global pointer

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
==============================================================================*/
dlci_cmux_param_type* ds_mux_get_cmux_params(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_IPR()

DESCRIPTION
  This function processes the AT+IPR command

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_process_ipr(void);

/*==============================================================================
FUNCTION QTI_BRIDGE_PROCESS_QCEMBP()

DESCRIPTION
  This function processes the AT$QCEMBP command

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_process_qcembp(void);


/*==============================================================================

FUNCTION QTI_BRIDGE_AT_CMD_POST_PROCESS_CB

DESCRIPTION
  callback function invoked to do post processing of AT cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_at_cmd_post_process_cb (void);

/*==============================================================================

FUNCTION QTI_BRIDGE_POST_PROCESS_AT_CMD

DESCRIPTION
  Function invoked to do post processing of AT cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_post_process_at_cmd (void);


/*==============================================================================

FUNCTION QTI_BRIDGE_QMI_SERVICE_UP_CB

DESCRIPTION
  Function invoked for QMI service UP ind from atfwd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_qmi_service_up_cb(void);

/*==============================================================================

FUNCTION QTI_BRIDGE_REG_AT_CMD

DESCRIPTION
  Function invoked to register AT cmds

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void qti_bridge_reg_at_cmd (void);

/*==============================================================================

  FUNCTION QTI_BRIDGE_SET_DNE_WM_LEVEL

  DESCRIPTION
  Function invoked to set the don't exceed water mark level

  DEPENDENCIES
  None.

  RETURN VALUE
  None

  SIDE EFFECTS
  None

  ==============================================================================*/
void qti_bridge_set_dne_wm_level (void);

/*==============================================================================

FUNCTION QTI_BRIDGE_POST_PROCESS_CMUX_CMD

DESCRIPTION
  Function invoked to do post processing of CMUX cmd

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/

void qti_bridge_post_process_cmux_cmd (void);

#endif

