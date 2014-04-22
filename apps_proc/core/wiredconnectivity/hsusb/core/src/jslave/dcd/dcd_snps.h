#ifndef DCD_SNPS
#define DCD_SNPS
/*
===========================================================================

FILE:         dcd_snps.h

DESCRIPTION:  
This is the external interface for the HS-USB core default implementation.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/core/src/jslave/dcd/dcd_snps.h#2 $

===========================================================================

===========================================================================
Copyright © 2010, 2023 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR
===========================================================================
*/

#include <jos_int.h>
#include <jusb_common_int.h>
#include <lists.h>
#include <jusb_core.h>
#include "hsu_common_int.h"
#include "hsu_hwio_dependency.h"
#include "hsu_al_bam_utils.h"
#include "hsu_timer.h"
#include "hsu_log.h"

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
Definitions
==============================================================================*/
// 1 for control and 3 for GSI == 4
#define SNPS_DCD_MAX_NUM_EVENT_BUF    1

/*=============================== Registers Defs =============================*/

#define SNPS_DCD_DSTS_CONNECTSPD_SS                 0x4
#define SNPS_DCD_DSTS_CONNECTSPD_HS                 0x0
#define SNPS_DCD_DSTS_CONNECTSPD_FS                 0x1
#define SNPS_DCD_DSTS_CONNECTSPD_LS                 0x2
#define SNPS_DCD_DSTS_CONNECTSPD_FS_PHY_48MHZ       0x3

#define SNPS_DCD_DCFG_DEVSPD_HS       0x0
#define SNPS_DCD_DCFG_DEVSPD_SS       0x4
#ifdef FEATURE_HS_USB_BURST_MODE
#define SNPS_DCD_DCFG_DEFAULT_NUMP    0x5
#else // ~FEATURE_HS_USB_BURST_MODE
#define SNPS_DCD_DCFG_DEFAULT_NUMP    0x1
#endif // ~FEATURE_HS_USB_BURST_MODE

#define SNPS_DCD_DEVTEN_DISCONNEVTEN   0x00000001
#define SNPS_DCD_DEVTEN_USBRSTEN       0x00000002
#define SNPS_DCD_DEVTEN_CONNECTDONEEN  0x00000004
#define SNPS_DCD_DEVTEN_ULSCNGEN       0x00000008
#define SNPS_DCD_DEVTEN_WKUPEVTEN      0x00000010
#define SNPS_DCD_DEVTEN_HIBERNANTION   0x00000020
#define SNPS_DCD_DEVTEN_U3L2L1         0x00000040
#define SNPS_DCD_DEVTEN_ERRTICERR      0x00000200
#define SNPS_DCD_DEVTEN_DEFAULT \
  SNPS_DCD_DEVTEN_DISCONNEVTEN | SNPS_DCD_DEVTEN_USBRSTEN | SNPS_DCD_DEVTEN_CONNECTDONEEN | \
  /* SNPS_DCD_DEVTEN_ULSCNGEN | */ SNPS_DCD_DEVTEN_WKUPEVTEN | SNPS_DCD_DEVTEN_HIBERNANTION | \
  SNPS_DCD_DEVTEN_U3L2L1 | SNPS_DCD_DEVTEN_ERRTICERR
//  TODO : FEATURE_HS_USB_PRE_SILICON_BU SNPS_DCD_DEVTEN_CMDCMPLEN | SNPS_DCD_DEVTEN_EVNTOVRFLWEN 

#define SNPS_DCD_GUSB2PHYCFG_USBTRDTIM_16BITS    0x5
#define SNPS_DCD_GUSB2PHYCFG_USBTRDTIM_8BITS     0x9

#define SNPS_DCD_GCTL_PRTCAPDIR_DEVICE           0x2
#define SNPS_DCD_GCTL_PWRDNSCALE_DEFAULT         0x3D2

/*====================== Controller Defs & Structs  ==========================*/

#define SNPS_DCD_DEPCMD_CMDTYPE_SETEPCONF       0x1
#define SNPS_DCD_DEPCMD_CMDTYPE_SETEPXFERRES    0x2
#define SNPS_DCD_DEPCMD_CMDTYPE_GETEPSTATE      0x3
#define SNPS_DCD_DEPCMD_CMDTYPE_SETSTALL        0x4
#define SNPS_DCD_DEPCMD_CMDTYPE_CLEARSTALL      0x5
#define SNPS_DCD_DEPCMD_CMDTYPE_STARTTRANSFER   0x6
#define SNPS_DCD_DEPCMD_CMDTYPE_UPDATETRANSFER  0x7
#define SNPS_DCD_DEPCMD_CMDTYPE_ENDTRANSFER     0x8
#define SNPS_DCD_DEPCMD_CMDTYPE_STARTNEWCFG     0x9

// Device Generic Command Descriptors
#define SNPS_DCD_DGCMD_CMDTYPE_SET_PERIODIC_PARAMS                    0x2
#define SNPS_DCD_DGCMD_CMDTYPE_SET_SCRATCHPAD_BUFFER_ARRAY_ADDRESS_LO 0x4
#define SNPS_DCD_DGCMD_CMDTYPE_SET_SCRATCHPAD_BUFFER_ARRAY_ADDRESS_HI 0x5
#define SNPS_DCD_DGCMD_CMDTYPE_TRANSMIT_DEVICE_NOTIFICATION           0x7
#define SNPS_DCD_DGCMD_CMDTYPE_SELECTED_FIFO_FLUSH                    0x9
#define SNPS_DCD_DGCMD_CMDTYPE_ALL_FIFO_FLUSH                         0xA
#define SNPS_DCD_DGCMD_CMDTYPE_SET_ENDPOINT_NDRY                      0xC
#define SNPS_DCD_DGCMD_CMDTYPE_RUN_SOC_BUS_LOOPBACK_TEST              0x10

// +++Command 0x7 notification specifics DGCMDPAR for function wakeup
// DGCMDPAR[3:0] = 1 for function wake
#define SNPS_DCD_DGCMD_FUNCTION_WAKE_MASK       0x1
#define SNPS_DCD_DGCMD_FUNCTION_WAKE_SHFT       0x0

// DGCMDPAR[10:4] = Interface Number for function wake
#define SNPS_DCD_DGCMD_FUNCTION_WAKE_IFACE_MASK 0x7F0
#define SNPS_DCD_DGCMD_FUNCTION_WAKE_IFACE_SHFT 0x4
// ---

/* structures for the commands */
typedef struct
{
  juint32_t IntrNum:5;      /* 0:4 */
  juint32_t reserved1:3;    /* 5:7 */
  juint32_t XferCmplEn:1;   /* 8 */
  juint32_t XferInProgEn:1; /* 9 */
  juint32_t XferNRdyEn:1;   /* 10 */
  juint32_t reserved2:2;    /* 11:12 */
  juint32_t StreamEvtEn:1;  /* 13 */
  juint32_t reserved3:1;    /* 14 */
  juint32_t Ebc_used:1;     /* 15 */
  juint32_t bInterval_m1:8; /* 16:23 */
  juint32_t StrmCap:1;      /* 24 */
  juint32_t EpDir:1;        /* 25 */
  juint32_t LEP:4;          /* 26:29 */
  juint32_t BulkBased:1;    /* 30 */
  juint32_t FifoBased:1;    /* 31 */
} snps_dcd_DEPCFG_P1;  //cannot find def HWIO_DEPCMDPAR2_REGS_p_DEPCMDPAR1_OUTI

typedef struct
{
  juint32_t reserved1:1;    /* 0 */
  juint32_t EPType:2;       /* 1:2 */
  juint32_t MPS:11;         /* 3:13 */
  juint32_t reserved2:3;    /* 14:16 */
  juint32_t FIFONum:5;      /* 17:21 */
  juint32_t BrstSiz:4;      /* 22:25 */
  juint32_t reserved3:4;    /*26:29*/
  juint32_t CfgAction:2;    /* 30:31 */
} snps_dcd_DEPCFG_P0;  //cannot find def  HWIO_DEPCMDPAR2_REGS_p_DEPCMDPAR0_OUTI

typedef struct
{
  juint32_t NumXferRes:16;  /* 0:15 */
  juint32_t reserved:16;    /* 16:31 */
} snps_dcd_DEPXFERCFG_P0;  

typedef struct
{
  juint32_t TDAddrLow:32;
} snps_dcd_DEPSTRTXFER_P1;

typedef struct
{
  juint32_t TDAddrHigh:32;
} snps_dcd_DEPSTRTXFER_P0;

typedef union
{
  juint32_t value;
  snps_dcd_DEPCFG_P1 DEPCFG;
  snps_dcd_DEPSTRTXFER_P1 DEPSTRTXFER;
} snps_dcd_DEPCMD_P1;

typedef union
{
  juint32_t value;
  snps_dcd_DEPCFG_P0 DEPCFG;
  snps_dcd_DEPXFERCFG_P0 DEPXFERCFG;
  snps_dcd_DEPSTRTXFER_P0 DEPSTRTXFER;
} snps_dcd_DEPCMD_P0;

typedef struct
{
  juint32_t SystemExitLatency:10;  /* 0:9 */
  juint32_t resreved:22;           /* 10:31 */
} snps_dcd_DGCMD_SET_PERIODIC_PARAMS; 

typedef union
{
  juint32_t value;
  snps_dcd_DGCMD_SET_PERIODIC_PARAMS DGCMD_SET_PERIODIC_PARAMS;
} snps_dcd_DGCMDPAR;  //HWIO_DGCMDPAR_OUTM

#define SNPS_EP_XFER_RSC_INDEX_BMASK 0x7F
#define SNPS_BUS_TIME_EXPIRY_BMSK   0x2
#define SNPS_BUS_TIME_EXPIRY_SHFT   0x1

// DEPEVT in response to a STARTXFER command
#define SNPS_NO_XFER_RESOURCE_AVAIL_BMSK    0x1
#define SNPS_NO_XFER_RESOURCE_AVAIL_SHFT    0x0

/* TRB types */
typedef enum
{
  TRB_CONTROL_NORMAL           = 1,
  TRB_CONTROL_CONTROL_SETUP    = 2,
  TRB_CONTROL_CONTROL_STATUS_2 = 3,
  TRB_CONTROL_CONTROL_STATUS_3 = 4,
  TRB_CONTROL_CONTROL_DATA     = 5,
  TRB_CONTROL_ISOC_FIRST       = 6,
  TRB_CONTROL_ISOC             = 7,
  TRB_CONTROL_LINK             = 8
} snps_dcd_trb_type;

typedef enum
{
  SNPS_CTRL_STATE_UNKNOWN = 0,
  SNPS_CTRL_STATE_SETUP_START,
  SNPS_CTRL_STATE_SETUP_RCVD,
  SNPS_CTRL_STATE_DATA_OR_STATUS2,
  SNPS_CTRL_STATE_DATA_INTERPRETE,
  SNPS_CTRL_STATE_STATUS3_IN,
  SNPS_CTRL_STATE_STATUS3_OUT,
} snps_dcd_ctrl_state_type;

#define HSU_DBM_BPTRH_EP_NUM_BMSK     0x1F000000
#define HSU_DBM_BPTRH_EP_NUM_SHFT     24
#define HSU_DBM_BPTRH_TRB_DMA_BMSK    0x20000000
#define HSU_DBM_BPTRH_TRB_DMA_SHFT    29
#define HSU_DBM_BPTRH_DBM_BMSK        0x80000000
#define HSU_DBM_BPTRH_DBM_SHFT        31

/* The structure of the TRB */
typedef struct
{
  struct
  {
    juint32_t BPTRL:32;
  } WORD0;
  struct
  {
    juint32_t BPTRH:32;
  } WORD1;
  struct
  {
    juint32_t BUFSIZE:24;   /* 0:23 */
    juint32_t PCM1:2;       /* 24:25 */
    juint32_t reserved:2;   /* 26:27 */
    juint32_t TRBSTS:4;     /* 28:31 */
  } WORD2;
  struct
  {
    juint32_t HWO:1;        /* 0 */
    juint32_t LST:1;        /* 1 */
    juint32_t CHN:1;        /* 2 */
    juint32_t CSP:1;        /* 3 */
    juint32_t TRBCTL:6;     /* 4:9 */
    juint32_t ISP:1;        /* 10 */
    juint32_t IOC:1;        /* 11 */
    juint32_t reserved1:2;  /* 12:13 */
    juint32_t StrmID_SOFNum:16; /* 14:29 */
    juint32_t reserved2:2;  /* 30:31 */
  } WORD3;
} snps_dcd_trb;

/* Event structures */
typedef union
{
  juint32_t value;
  struct
  {
    juint32_t EPEvt:1;        /* 0 */
    juint32_t PhysicalEPNum:5;/* 1:5 */
    juint32_t type:4;         /* 6:9 */
    juint32_t reserved1:2;    /* 10:11 */
    juint32_t EventStatus:4;  /* 12:15 */
    juint32_t EventParam:16;  /* 16:31 */
  } DEPEVT;
  struct
  {
    juint32_t NonEPEvt:1;     /* 0 */
    juint32_t DEVEvt:7;       /* 1:7 */
    juint32_t type:4;         /* 8:11 */
    juint32_t reserved1:4;    /* 12:15 */
    juint32_t EvtInfo:9;      /* 16:24 */
    juint32_t reserved2:7;    /* 25:31 */
  } DEVT;
} snps_dcd_event_t;

/* Types of events */
#define SNPS_DCD_DEVT_DISCONNEVTEN    0
#define SNPS_DCD_DEVT_USBRSTEN        1
#define SNPS_DCD_DEVT_CONNECTDONEEN   2
#define SNPS_DCD_DEVT_ULSCNGEN        3
#define SNPS_DCD_DEVT_WKUPEVTEN       4
#define SNPS_DCD_DEVT_HIBERREQEN      5
#define SNPS_DCD_DEVT_SUSPENTEVTEN    6
#define SNPS_DCD_DEVT_SOF             7
#define SNPS_DCD_DEVT_ERRTICERR       9
#define SNPS_DCD_DEVT_CMDCMPLEN       10
#define SNPS_DCD_DEVT_EVNTOVRFLWEN    11
#define SNPS_DCD_DEVT_TSTLMPRECVD     12

typedef enum
{
  SNPS_CTRL_EVT_UNKNOWN = 0,
  SNPS_CTRL_EVT_XFER_COMPLETE,
  SNPS_CTRL_EVT_XFER_NOTREADY,
  SNPS_CTRL_EVT_XFER_ERROR,
  SNPS_CTRL_EVT_CORE_REQ,
}snps_dcd_ep_event_type;

#define SNPS_DCD_DEPVT_ISOC_MISSED_INTRVL_MASK  0x8


#define SNPS_DCD_DEPVT_XFER_COMPLETE          1
#define SNPS_DCD_DEPVT_XFER_INPROGRESS        2
#define SNPS_DCD_DEPVT_XFER_NOTREADY          3
//#define SNPS_DCD_DEPVT_FIFO_UNDERRUN_OVERRUN  4
#define SNPS_DCD_DEPVT_STREAM                 6
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE        7

// For DEPVT_EP_CMD_COMPLETE
// The EventParam are already shifted right by 16 by reading from the bitfield; hence, the mask definitions are updated as follows
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__COMMAND_TYPE_BMSK    0xF00
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__COMMAND_TYPE_SHFT    0x8
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__XFER_RSC_IDX_BMSK    0x7F
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__XFER_RSC_IDX_SHFT    0x0   

// For DEPVT_EP_CMD_COMPLETE
// EndXfer
// type is already shifted by 6 due to bitfield.  The mask definitions are updated as follows
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__DMA_STOPPED_VAL      0x7
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__DMA_STOPPED_BMSK     0x15
#define SNPS_DCD_DEPVT_EP_CMD_COMPLETE__DMA_STOPPED_SHFT     0x0

#define SNPS_DCD_LINK_STATE_U0                0
#define SNPS_DCD_LINK_STATE_U1                1
#define SNPS_DCD_LINK_STATE_U2                2

#define SNPS_DCD_LINK_STATE_L1                2

#define SNPS_DCD_LINK_STATE_SUSPEND_U3        3

#define SNPS_DCD_EVENT_INFO_MASK              0xF

#define SNPS_DCD_TRB_STATUS_SETUP_PENDING     0x2

#define SNPS_DCD_PMIC_VBUS_DISCONNECT  0x1
#define SNPS_DCD_PMIC_VBUS_CONNECT     0x2

/*========================= Driver structures ==========================*/

/* General DCD definitions */
#define SNPS_DCD_SOFT_RESET_RETRIES           100
#define SNPS_DCD_SOFT_RESET_DELAY_US          10
#define SNPS_DCD_RX_FIFO_DRAIN_RETRIES        100
#define SNPS_DCD_RX_FIFO_DRAIN_DELAY_US       10
#define SNPS_DCD_MAX_CMD_DELAY_MS             20
#define SNPS_DCD_DEFAULT_STREAM               0
#define SNPS_DCD_EP0_OUT_ADDR                 0
#define SNPS_DCD_EP0_IN_ADDR                  1
#define SNPS_DCD_DEFAULT_EVT_BUF_ID           0
#define SNPS_DCD_RES_NUM_CONNECTED            0
#define SNPS_DCD_RES_NUM_CONFIGURED           2
#define SNPS_DCD_EVNT_BUF_SIZE_FACTOR         20
#define SNPS_DCD_MAX_SYSTEM_EXIT_LATENCY_US   125
#define SNPS_DCD_MAX_TRBS_PER_STREAM          2
#define SNPS_DCD_MAX_IN_TRBS_PER_STREAM       3 // +2 due to ZLP + LINK
#define SNPS_DCD_MAX_BULK_IN_TRBS_PER_STREAM  12 // +2 due to ZLP + LINK
#define SNPS_DCD_MAX_AUDIO_TRBS_PER_STREAM    61

// Size of RX buffer is linked with number of TRBs that will be queued at EP.
// Due to 50kB internal RX cache, 25 TRBs each pointing to 2kB buffer is optimal.
// Additional 1 is for link TRB
#define SNPS_DCD_MAX_OUT_TRBS_PER_STREAM    (HSU_SNPS_OUT_TRB_ARRAY_SIZE_MAX+1) 

#define SNPS_DCD_MAX_TRBS_FOR_INFINITE_MODE   2

/* EP types */
#define SNPS_DCD_TYPE_CTRL 0x1
#define SNPS_DCD_TYPE_ISOC 0x2
#define SNPS_DCD_TYPE_BULK 0x4
#define SNPS_DCD_TYPE_INTR 0x8
#define SNPS_DCD_TYPE_ALL \
  (SNPS_DCD_TYPE_CTRL | SNPS_DCD_TYPE_ISOC | SNPS_DCD_TYPE_INTR | SNPS_DCD_TYPE_BULK)

/* Connection Status */
typedef enum
{
  SNPS_DCD_STATE_DETACHED,
  SNPS_DCD_STATE_ATTACHED,
  SNPS_DCD_STATE_ENABLED,
  SNPS_DCD_STATE_IN_RESET ,
  SNPS_DCD_STATE_CONNECTED,
  SNPS_DCD_STATE_ADRESSED,
  SNPS_DCD_STATE_CONFIGURED
} snpd_dcd_connection_status;

/* TRB list */
typedef struct
{
  snps_dcd_trb *virt;
  snps_dcd_trb *phys;
  void *cookie;
  juint32_t trb_array_size;
} snps_dcd_soft_trb_array;

/* Streams list */
typedef struct
{
  /* Pointer to TRB array for the stream */
  snps_dcd_soft_trb_array soft_trb_array; 
  /* Pointer to next TRB SW should use */
  snps_dcd_trb *trb_soft_ptr;
  /* Pointer to next TRB SW should check for completion */
  snps_dcd_trb *trb_hw_ptr;
  /* Pointer to pending request list for the stream */
  request_t *cur_req;
  /* Transfre Resource Index for this stream */
  juint16_t xfer_rsc_idx;
  /* Flag that indicates whether this stream structure is valid */
  jbool_t stream_initialized;
} snps_dcd_stream;

/* Event Buffer */
typedef struct
{
  /* Pointers to the Event Buffer DMA buffer */
  void              *vaddr;
  void              *paddr;
  void              *cookie;
  /* Size of the event buffer */ 
  juint32_t         buffer_size;
  /* The high dword of the event address */
  snps_dcd_event_t* high_addr;
  /* The current event address for the software */
  snps_dcd_event_t* sw_current_addr;
  /* The current event address for the hardware */
  snps_dcd_event_t* hw_current_addr;
} snps_dcd_event_buffer_t;

/* EP Dscriptor */
typedef struct {

  /* End Point physical number (hardware address) */
  juint8_t              hw_addr;
  /* End Point logical number (USB EP address) */
  juint8_t              soft_addr;
  juint8_t              fifo_number;
  /* Which config claimed this ep */
  juint8_t              claimed_config;
  /* Which iface claimed this ep */
  juint8_t              claimed_iface;
  /* Which alt-iface of the FD claimed the ep */
#define TYPE_CTRL 0x1
#define TYPE_ISOC 0x2
#define TYPE_BULK 0x4
#define TYPE_INTR 0x8
#define TYPE_ALL (TYPE_CTRL | TYPE_ISOC | TYPE_INTR | TYPE_BULK)
  juint8_t              claimed_alt;
  pipe_desc_t           *pipe;
  jbool_t               is_in;         /* Direction */
  juint8_t              type_map;      /* Control/Isync/Bulk/Interrupt */
  juint16_t             packet_size;   /* Actual size, not 2^size */
  snps_dcd_stream*      streams;       /* Array of streams */
  juint16_t             max_streams;   /* Size of stream array */
  juint8_t              max_burst;     /* Max burst size (Super-Speed only) */
  juint8_t              mult;          /* Mult (Super-Speed only) */
  jtask_h               timeout_task;  /* Task for handling transfer timeouts */
  jbool_t               abort_timeout_task;  /* Abort timeout when xfer is completed */
  struct snps_dcd_soft_dc_t *sc;
  juint8_t              max_trbs;
  request_t             debug_req;       // Debug only: Keeps copy of head node for current request linked list
  juint32_t             isoc_interval;
  jbool_t               xfer_nrdy_elapsed;
  juint32_t             xfer_in_progress;
} snps_dcd_ep_desc_t;

/* DCD Software context */
typedef struct snps_dcd_soft_dc_t 
{
    jdevice_t                dev;
    device_speed_t           speed;
    juint8_t                 max_available_eps;
    juint8_t                 max_available_in_eps;
    snps_dcd_ep_desc_t       *ep_desc_table;
    jspinlock_h              lock;
    /* resources */
    jbus_resource_h          reg_res;
    jbus_resource_h          irq_res;
    void                    *interrupt_h;
    void                    *core_ctx;
    snps_dcd_event_buffer_t event_buffer[SNPS_DCD_MAX_NUM_EVENT_BUF];
    core_events_t*          events;
    juint16_t               snpsid;
    snpd_dcd_connection_status conn_status;
    juint8_t                intr_to_handle;
    request_t               ep0_request;
    snps_dcd_trb_type       ep0_trb_type;
    request_t*                  ctrl_pending_request;
    jbool_t                     xfer_nrdy_rcvd;
    snps_dcd_ctrl_state_type    ctrl_state;
    // +++ HSU Addition
    jbool_t enumeration_is_allowed;
    jbool_t enumeration_is_pending;
    jbool_t is_remote_wakeup_pending;
    jbool_t is_power_on_or_soft_reset; // Used for setting the max packet size for control endpoints
    // --- HSU Addition
} snps_dcd_soft_dc_t;

#define HSU_MAX_NUM_OF_EP  32

/* Structure is needed as snps_dcd_dbm_init() cannot be placed in dcd_enable() 
 * due to the DBM initialization of DBM_GEN_CFG requiring enumeration as a pre-requisite
 *
 * Structure is need to maintain knowledge of BAM resets leading to DBM resets */

//#ifdef FEATURE_HS_USB_BAM
typedef struct
{
  uint8   bam_pipe_num;
  uint8   hw_ep_num;
  boolean is_bam_pipe_producer;
  void    *connection;
  uint32  data_fifo_addr;
  uint32  data_fifo_size;
  uint32  desc_fifo_addr;
  uint32  desc_fifo_size;
} dbm_bam_info_type;

#define HSU_UNDEFINED_EP_VAL 0xFF
typedef struct
{
  boolean           is_dbm_initialized;
  boolean           is_bam_initialized;
  boolean           is_dbm_ep_initialized[HWIO_DBM_EP_CFG_ADDRn_MAXn+1];
  boolean           is_bam_ep_initialized[HWIO_DBM_EP_CFG_ADDRn_MAXn+1];
  dbm_bam_info_type dbm_to_usb_hw_ep_map[HWIO_DBM_EP_CFG_ADDRn_MAXn+1];
} dcd_snps_eps_dbm_ctx_type;

#define HSU_CHK_EP_STREAM_ID(ep, stream_id) \
  if ((NULL == (ep)) || (NULL == (ep)->streams) || ((stream_id) >= (((ep)->max_streams)))) \
    { HSU_ERR_FATAL("ep (0x%8X) stream (%u) invalid", ep, stream_id, 0); }

#define HSU_INC_TRB_PTR(TRB, EP, STREAM_ID, LINK_TRB) \
  /* Advance the running hw pointer */ \
  (TRB)++; \
  /* For non-control and non-ISOC, allow TRB to loop around once LINK TRB is reached */ \
  if (((EP)->pipe) && (PIPE_ISOC != ((EP)->pipe->type))) \
  { \
    if ((TRB) >= (LINK_TRB)) \
    { \
      (TRB) = (EP)->streams[(STREAM_ID)].soft_trb_array.virt; \
    } \
  }

#ifdef FEATURE_HS_USB_LOG_TRB_POINTERS
#define HSU_INC_HW_TRB_PTR(HW_TRB, EP, STREAM_ID, LINK_TRB, TRB_BASE) \
  HSU_ULOG_5(NO_MSG, SNPS_LOG, "hw_trb 0x%8X, idx %u, buf 0x%8X, remain_sz %u, EP 0x%X", (uint32)HW_TRB, ((uint32)(HW_TRB)-(uint32)(TRB_BASE))/16, (HW_TRB)->WORD0.BPTRL, (HW_TRB)->WORD2.BUFSIZE, (EP)->hw_addr); \
  HSU_INC_TRB_PTR(HW_TRB, EP, STREAM_ID, LINK_TRB)
#else // ~FEATURE_HS_USB_LOG_TRB_POINTERS
#define HSU_INC_HW_TRB_PTR(HW_TRB, EP, STREAM_ID, LINK_TRB, TRB_BASE) \
  HSU_INC_TRB_PTR(HW_TRB, EP, STREAM_ID, LINK_TRB)
#endif // ~FEATURE_HS_USB_LOG_TRB_POINTERS

#ifdef FEATURE_HS_USB_LOG_TRB_POINTERS
#define HSU_INC_SW_TRB_PTR(SW_TRB, EP, STREAM_ID, LINK_TRB, TRB_BASE) \
  HSU_ULOG_5(NO_MSG, SNPS_LOG, "soft_trb 0x%8X, idx %u, buf 0x%8X, size %u, EP 0x%X", (uint32)SW_TRB,  ((uint32)(SW_TRB)-(uint32)(TRB_BASE))/16, (SW_TRB)->WORD0.BPTRL, (SW_TRB)->WORD2.BUFSIZE, (EP)->hw_addr); \
  HSU_INC_TRB_PTR(SW_TRB, EP, STREAM_ID, LINK_TRB)
#else // ~FEATURE_HS_USB_LOG_TRB_POINTERS
#define HSU_INC_SW_TRB_PTR(SW_TRB, EP, STREAM_ID, LINK_TRB, TRB_BASE) \
  HSU_INC_TRB_PTR(SW_TRB, EP, STREAM_ID, LINK_TRB)
#endif // ~FEATURE_HS_USB_LOG_TRB_POINTERS

// Caller must ensure EP NULL check already verified
#define HSU_GET_PIPE_TYPE(EP) (EP->pipe ? EP->pipe->type : PIPE_CONTROL)


/*==============================================================================
Forward Declarations
==============================================================================*/

/*=================== DCD Operations Interface (CORE->DCD) ===================*/

static jresult_t snps_dcd_ops_enable(jdevice_t dev, jbool_t unused);
static void snps_dcd_ops_disable(jdevice_t dev);
static jresult_t snps_dcd_ops_wake_the_host(jdevice_t dev);
jresult_t snps_dcd_ops_vbus_connect(jdevice_t dev);
static jresult_t snps_dcd_ops_vbus_disconnect(jdevice_t dev);
static int snps_dcd_connect(jdevice_t dev);
static jresult_t snps_dcd_ops_enable_ep(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t snps_dcd_ops_enable_ep_data_struct_only(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t snps_dcd_ops_disable_ep(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t snps_dcd_ops_disable_ep_hw_accel(jdevice_t dev, pipe_desc_t *pipe);
       jresult_t snps_dcd_ops_abort_ep(jdevice_t dev, pipe_desc_t *pipe);
       jresult_t snps_dcd_ops_abort_ep_hw_accel(jdevice_t dev, pipe_desc_t *pipe);

       jresult_t snps_dcd_ops_stall_ep(jdevice_t dev, pipe_desc_t *pipe, jbool_t flag);
static jresult_t snps_dcd_ops_send_io_request(jdevice_t dev, pipe_desc_t *pipe, request_t *core_req);
static jresult_t snps_dcd_ops_begin_hw_accel_xfer(jdevice_t dev, pipe_desc_t *pipe, void* trb_ring_pa_addr, juint32_t gsi_interrupter_idx, boolean is_ep_resume);
static jresult_t snps_dcd_ops_gsi_post_channel_connect_init(jdevice_t dev, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx, void* gsi_doorbell_reg, void* trb_ring_pa_addr);
static jresult_t snps_dcd_ops_gsi_general_cfg(void);
static jresult_t snps_dcd_ops_alloc_pipe(jdevice_t dev, pipe_desc_t *pipe, 
                                         juint8_t config_index, juint8_t iface_index, 
                                         juint8_t alt_index);
static jresult_t snps_dcd_ops_free_pipe(jdevice_t dev, pipe_desc_t *pipe);
static jresult_t snps_dcd_ops_get_ep_status(jdevice_t dev, juint8_t *status, 
                                            juint8_t ep_num);
static jresult_t snps_dcd_ops_set_address_state(jdevice_t dev, juint16_t address,
                                                jbool_t wait_for_status_stage);
static jresult_t snps_dcd_ops_get_device_address(jdevice_t dev, juint16_t* address_ptr);
static jresult_t snps_dcd_ops_set_config_state(jdevice_t dev, juint8_t config);
static jresult_t snps_dcd_ops_handle_feature(jdevice_t dev, juint16_t feature, 
                                             juint16_t index, juint8_t set);
static juint8_t snps_dcd_ops_get_max_packet0(jdevice_t dev);
static void snps_dcd_ops_handle_tx_completion(jdevice_t dev, pipe_desc_t *pipe);

static void snps_dcd_ops_set_test_mode(jdevice_t dev, juint16_t test_mode);
static void snps_dcd_ops_allow_enumeration(jdevice_t dev);
static void snps_dcd_ops_disallow_enumeration(jdevice_t dev);
static void snps_dcd_ops_register_events(jdevice_t dev, const void* events);
static jresult_t snps_usb3_dcd_ioctl(jdevice_t self, drv_ioctl_t ioctl, void *arg);
void snps_dcd_ops_dbm_ep_init(jdevice_t dev, uint32 dbm_ep_num); /* needed in another snps file */
void snps_dcd_ops_dbm_ep_reset(jdevice_t dev, uint32 dbm_ep_num);
void snps_dcd_ops_dbm_init(jdevice_t dev);
void snps_dcd_ops_dbm_init_qdss(jdevice_t dev, uint32 consumer_pipe_index, uint32 consumer_data_fifo_addr, uint32 consumer_data_fifo_size);
static void snps_dcd_ops_get_iface_index(pipe_desc_t *pipe, uint8* iface_ptr);

// GSI specific
void snps_dcd_setup_event_buf_and_gsi_interrupters(jdevice_t dev, uint32 idx, uint32 num_of_trbs);
void snps_dcd_reset_event_buffer(jdevice_t dev, uint32 idx);
void snps_dcd_usb_gsi_endpoint_config(jdevice_t dev, pipe_desc_t *pipe, juint32_t gsi_interrupter_idx);
jresult_t snps_dcd_ops_init_ep_trbs_hw_accel(jdevice_t dev, pipe_desc_t *pipe, 
  void **trb_ring_cookie, void **trb_ring_va_addr, void **trb_ring_pa_addr, juint32_t n);
jresult_t snps_dcd_ops_populate_ep_trb_hw_accel(jdevice_t dev, pipe_desc_t *pipe, 
  juint32_t n, void **next_free_pa_running_ptr, juint32_t client_buf_size,
  void* trb_ring_va_addr, void* trb_ring_pa_addr, void **link_trb_pa_addr, juint32_t usb_gsi_interrupter_index);
jresult_t snps_dcd_ops_set_hwo_bit__updatexfer_ul(jdevice_t dev, pipe_desc_t *pipe, juint32_t n, void *trb_ring_va_addr);
jresult_t snps_dcd_ops_get_ep_num(jdevice_t dev, pipe_desc_t *pipe, juint8_t *ep_hw_addr);
request_t* snps_dcd_ops_get_ep_cur_req(pipe_desc_t *pipe);

/*=============================== Events Utils ===============================*/

void snps_dcd_set_event_buffer_address(snps_dcd_soft_dc_t *sc, 
                                              juint8_t event_buffer_index, 
                                              void* address);
void snps_dcd_set_event_buffer_address_gsi(void *ctx, juint8_t event_buffer_index, void* address);

void snps_dcd_set_event_buffer_size(snps_dcd_soft_dc_t *sc, 
                                           juint8_t event_buffer_index, 
                                           juint16_t size);
void snps_dcd_set_event_buffer_interrupt(snps_dcd_soft_dc_t *sc, 
                                                juint8_t event_buffer_index, 
                                                boolean enable);
void snps_dcd_set_event_buffer_count(snps_dcd_soft_dc_t *sc, 
                                            juint8_t event_buffer_index, 
                                            juint16_t count);
juint16_t snps_dcd_get_event_buffer_count(snps_dcd_soft_dc_t *sc, 
                                                 juint8_t event_buffer_index);

/*============================== Commands Utils ==============================*/

jresult_t snps_dcd_write_ep_command(snps_dcd_soft_dc_t *sc, juint8_t ep_index,
                                           juint32_t cmd, juint32_t p0, juint32_t p1,
                                           juint32_t p2, juint16_t* cmd_params);
jresult_t snps_dcd_write_general_command(snps_dcd_soft_dc_t *sc,
                                         juint32_t cmd, juint32_t parameter);
void snps_dcd_function_wakeup_request(jdevice_t dev, uint8 iface_num);
void snps_dcd_ops_accept_u1_u2(boolean set_value);

/*==================== General core configuration utils ======================*/

jresult_t snps_dcd_read_general_params(snps_dcd_soft_dc_t *sc);
jresult_t snps_dcd_reset_core(snps_dcd_soft_dc_t *sc);
jresult_t snps_dcd_config_phy(snps_dcd_soft_dc_t *sc);
jresult_t snps_dcd_config_core(snps_dcd_soft_dc_t *sc);
jresult_t snps_dcd_config_events(snps_dcd_soft_dc_t *sc, juint8_t event_buffer_index);
jresult_t snps_dcd_config_device_resources(snps_dcd_soft_dc_t* sc, juint16_t res_num);
jresult_t snps_dcd_init_event_buffer(snps_dcd_soft_dc_t *sc, juint8_t event_buffer_index, juint32_t event_count);
jresult_t snps_dcd_set_U1(snps_dcd_soft_dc_t *sc, jbool_t flag);
jresult_t snps_dcd_set_U2(snps_dcd_soft_dc_t *sc, jbool_t flag);
jresult_t snps_dcd_handle_set_SEL_request(snps_dcd_soft_dc_t *sc, 
                                          struct usb_set_SEL_request *set_SEL_request);

/*============================== Transfer utils ==============================*/

jresult_t snps_dcd_submit_isoc_request(snps_dcd_ep_desc_t* ep, juint16_t event_params, jbool_t new_req);

jresult_t snps_dcd_add_request_to_stream(
                                   snps_dcd_ep_desc_t* ep, 
                                   request_t* request,
                                   juint16_t stream_id);

jresult_t snps_dcd_enqueue_request(snps_dcd_ep_desc_t* ep, 
                                   request_t* request);
jresult_t snps_dcd_dequeue_request(snps_dcd_ep_desc_t* ep, 
                              request_t* request);
jresult_t snps_dcd_abort_ep0(snps_dcd_soft_dc_t *sc);
jresult_t snps_dcd_send_setup_request(snps_dcd_soft_dc_t* sc);
jresult_t snps_dcd_send_status_request(snps_dcd_soft_dc_t* sc);
void      snps_dcd_handle_timeout(void *arg);
jresult_t snps_dcd_abort_xfer(snps_dcd_ep_desc_t* ep);


/*=========================== Transfer Completion ============================*/

jresult_t snps_dcd_handle_xfer_completion(snps_dcd_ep_desc_t* ep, juint16_t stream_id);

/*=========================== Control Transfer State Machine ============================*/

jresult_t snps_dcd_ctrl_sm_notify( snps_dcd_ep_desc_t* ep, request_t* request, snps_dcd_ep_event_type event);

/*========================== EP configuration utils ==========================*/

snps_dcd_ep_desc_t* snps_dcd_get_ep_desc(snps_dcd_soft_dc_t *sc, juint8_t usb_ep_address);
jresult_t snps_dcd_init_endpoints(snps_dcd_soft_dc_t *sc);
jresult_t snps_dcd_config_ep(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_dalepena_ep(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_config_ep_cfg(snps_dcd_ep_desc_t *ep, jbool_t ignore_seq, jbool_t is_modify_cfg_action);
jresult_t snps_dcd_config_ep_xfer(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_config_tx_fifo_size(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_unconfig_ep(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_enable_ep0(snps_dcd_soft_dc_t* sc);
jresult_t snps_dcd_disable_ep0(snps_dcd_soft_dc_t* sc);
jresult_t snps_dcd_stall_ep(snps_dcd_ep_desc_t *ep, jbool_t flag);
jresult_t snps_dcd_handle_fifo_overrun_underrun(snps_dcd_ep_desc_t* ep, juint16_t stream_id);
jresult_t snps_dcd_init_ep_streams(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_uninit_ep_streams(snps_dcd_ep_desc_t *ep);
jresult_t snps_dcd_uninit_stream_trbs(snps_dcd_ep_desc_t *ep, snps_dcd_stream* stream);
jresult_t snps_dcd_init_stream_trbs(snps_dcd_ep_desc_t *ep, juint16_t stream_id);

/*================================ ISR & DSR ================================*/

jresult_t snps_dcd_config_interrupts(snps_dcd_soft_dc_t *sc);

/*================================== Other ==================================*/

jresult_t snps_usb3_dcd_detach(jdevice_t dev);
void snps_dcd_dbm_get_bam_info(uint8 dbm_ep_num, dbm_bam_info_type **info);
uint8 snps_dcd_get_dbm_ep_from_hw_ep(uint32 hw_ep_num);
void snps_dcd_dbm_init(snps_dcd_soft_dc_t *sc);
void snps_dcd_dbm_reset(snps_dcd_soft_dc_t *sc);
dcd_snps_eps_dbm_ctx_type* snps_dcd_get_snps_eps_dbm_ctx_hnd(void);
void snps_dcd_dbm_set_event_buffer_address_and_size(snps_dcd_soft_dc_t *sc);

void snps_dcd_function_wakeup_cb(void *param);

/* TODO: Remove */
void snps_dcd_debug_timer_cb(unsigned long callback_param);
jresult_t snps_dcd_handle_resume(snps_dcd_soft_dc_t *sc);
void snps_dcd_set_usb_link_state_change_event_enable(jbool_t enable);

#endif /* DCD_SNPS */
