/*==============================================================================

High Speed USB Ethernet Networking control Model (CDC/ECM) Adaptation Layer

GENERAL DESCRIPTION
Implementation of the High Speed USB CDC/ECM Adaptation Layer.
This includes implementation of the API functions defined by the CDC/ECM 
function driver in ecm_fd_api.h (such as callback functions), and the API 
exposed to the Applications (sio_usb like API).

The API exposed to the application is protected by Giant mutex. An internal 
critical section is not used in order to prevent deadlocks that might occur
due to inconsistent mutex lock order (between ECM crit section and the Giant mutex)

EXTERNALIZED FUNCTIONS
hsu_al_ecm_control_open
hsu_al_ecm_data_open
hsu_al_ecm_control_close
hsu_al_ecm_data_close
hsu_al_ecm_control_transmit
hsu_al_ecm_data_transmit
hsu_al_ecm_ioctl
hsu_al_ecm_data_flush_tx

INITALIZATION AND SEQUENCING REQUIREMENTS
The stack must be initialized first (which causes ecm_init to be called), 
before all other functions in this header file can be used.

Copyright (c) 2007 - 2017 by Qualcomm Technologies, Inc.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ecm.c#1 $
$DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
2009/01/13 04:17:02 $ $Author: pwbldsvc $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
05/17/06  ke      Initial version

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_al_ecm_int.h"
#include "customer.h" /* For FEATURE_HSU_TEST to work */
#include "hsu_common_int.h"
#include "err.h"
//#include "rex.h"

#include "hsu_al_dsm_utils.h"
#include "hsu_al_task_int.h"
#include "hsu_cdc_fd_api.h"
#include "port_int.h"

#include "hsu_conf_sel_stack_utils_int.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_conf_sel_comp_utils.h"

#if defined(FEATURE_HSU_ECM_APP_STUB)
#include "hsu_al_ecm_qmi_msg_parser.h"
#include "hsu_al_unit_test_utils.h"
#endif

#include "jerrno.h"
#include "jtypes_int.h"
#include "jusb_common_int.h"
#include "jusb_int.h"
#include "jusb_core_fd_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "jusb_core.h"
#include "cdc_fd.h"

#include "hsu_log.h"

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
#include "hsu_health_diag_init.h"
#endif /* FEATURE_HS_USB_HEALTH_MONITOR */

#include <stdio.h>

#include "hsu_core_config_int.h"
#include "hsu_conf_sel_core_config.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_conf_sel_i.h"

#include "uw_device_cdc.h"
#include "uw_device_cdc_ecm.h"

#include "hsu_al_bam_utils.h"
#include "queue.h"

#include "HAL_hsusb.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_platform.h"

#include "bam.h"
#include "hsu_timer.h"

#include "hsu_bam_sm.h"
#include <stringl/stringl.h>

#include "hsu_qurt_util.h"

#include "hsu_hwio_dependency.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

static void hsu_al_ecm_handle_tx_complete
(
  unsigned long arg
);

/*==============================================================================
Constants and Macros
==============================================================================*/

#define GET_CHIP_SERIAL_FOR_MAC_ADDR(x)  (x << 12)
#define SEND_FLAG_AS_BOOLEAN(x)          (x >> 8)
/*Increment the last chaaracter of MAC Address by 1 to form a new MAC Address to be given to the network*/
#define MAC_ADDR_FOR_NETWORK(x)          (x[11] = x[11] + 1)

#define USB_DATA_CHANNEL  0x8
    
#define HSU_ECM_RMNET_BUF_SIZE  (2040 + BAM_DMUX_HEADER_SIZE)
#define HSU_ECM_RMNET_DL_MAX_PKT_LENGTH 0x4000
#define HSU_ECM_MAX_ETHERNET_FRAME_LENGTH 2048
/* The maximal length of an Ethernet frame, sent by the application 2K + 28 byte overhead */ 
#ifdef FEATURE_DATA_QMI_QOS
#define HSU_AL_ECM_QOS_HEADER_SIZE 6 
/* Header size added when QOS is enabled*/
#endif /*FEATURE_DATA_QMI_QOS*/


#ifdef FEATURE_DATA_QMI_QOS
  #define HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES HSU_ECM_MAX_ETHERNET_FRAME_LENGTH + \
          HSU_AL_ECM_QOS_HEADER_SIZE
    /* The maximal length of an Ethernet frame, sent by the application + 6 bytes
        header for QoS */
#else /* FEATURE_DATA_QMI_QOS */
  #define HSU_AL_ECM_RX_DATA_BUFFER_SIZE_IN_BYTES HSU_ECM_MAX_ETHERNET_FRAME_LENGTH
    /* The maximal length of an Ethernet frame, sent by the application */
#endif  /*FEATURE_DATA_QMI_QOS*/

#define QMI_ETH_VENDOR_ID  (0x0250F3)
/* Needed for MAC address generation. See ds_rmnet_sm.c, line 1173 */

#define HSU_ECM_AL_CONTROL_DSM_POOL_ID  DSM_DS_LARGE_ITEM_POOL
#define HSU_ECM_AL_DSM_POOL_ID          UNCACHED_DSM_ITEM_POOL

#define AL_SER_ECM_DTR_ASSERTED_MASK (0x1)
/* See CDC Spec. Section 6.2.14 */

#define HSU_AL_ECM_SUBCLASS_STANDARD   0x06
#define HSU_AL_ECM_SUBCLASS_PROP       0xFF

#define HSU_AL_ECM_PROTOCOL         0xFF

#define HSU_ROUND_DOWN_TO_512(size) ((size) & ~511)

// Each Rx buffer provided to the HW must be multiple of 512 bytes
#define ECM_ROUNDED_DSM_ITEM_SIZE  HSU_ROUND_DOWN_TO_512(UNCACHED_DSM_ITEM_SIZE)

// Size of ECM RX buffer is linked with number of TRBs that will be queued at EP and size of DSM item.
#define ECM_RX_BUF_DEFAULT_SIZE    (HSU_SNPS_OUT_TRB_ARRAY_SIZE_MAX * ECM_ROUNDED_DSM_ITEM_SIZE)


// Max number of DSM items that will be provided to HW in single cdc_read_multiple 
// Should be same as no of TDs/TRBs queued at USB HW.

#define ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ (HSU_SNPS_OUT_TRB_ARRAY_SIZE_MAX)

#ifndef HS_USB_ECM_RX_TIMEOUT_MS
#define HS_USB_ECM_RX_TIMEOUT_MS  8
#endif

#ifndef HS_USB_ECM_TX_TIMEOUT_MS
#define HS_USB_ECM_TX_TIMEOUT_MS 0
  /* zero TX timeout significantly improve CPU utilization with a low
     risk of delayed freeing of DSM items */
#endif

/* This is the Manufacturer ID. It is is used to construct a MAC address
   when working in standard ECM mode */
#define HS_USB_MANUFACTURER_ID "00A0C6"

#ifdef FEATURE_HS_USB_STANDARD_ECM
/* The following defines the sizes of different strings/places in the string used to 
   construct a MAC address from ESN/IMEI when working in standard ECM
 */
/*The MAC address is 6 octets long. The first 3 are constant and hold the manufacturer 
ID (HS_USB_MANUFACTURER_ID). The last 3 are defined by the application (3 bytes)
In order to support Multiple RMNet instances in the future we need to assign each 
RmNet instance a unique MAC address. Thus we'll generate a 20 bit (5 digits) 
unique string from the ESN/IMEI  and use the last 4 bits to identify each RmNet instance.
These 5 digits are taken from the serial number from the ESN/IMEI
*/
#define HS_USB_SERIAL_NUMBER_BUF_SIZE 5
/* ESN is 32 bits long. The last 24 bits are a unique serial number. So in the ESN str
   the index of the serial number is 1 */
#define HS_USB_INDX_OF_SERIAL_NUM_IN_ESN 1
/* The format of IMEI is AA-BBBBBB-CCCCCC-EE where CCCCCC is the serial number.
   So the index of the serial number in the IMEI string is 8*/
#define HS_USB_INDX_OF_SERIAL_NUM_IN_IMEI 8
/*!NOTE: 
MAC_ADDR_STR_SIZE >= sizeof(HS_USB_MANUFACTURER_ID) + HS_USB_SERIAL_NUMBER_BUF_SIZE + 1 
*/
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

/* A timer for calling the stack cdc_handle_tx_completion function.
When this API is called, the stack will notify us on completed transfers. 
Needed when HS_USB_ECM_TX_TIMEOUT_MS is set to 0, otherwise when we reach the 
High WM the application stops calling the transmit function and usb_stack will 
not check for completed transfers and we will never go back to the Low WM. */
#ifndef HS_USB_ECM_TX_HANDLE_COMPLETE_TIMER_MS
  #define HS_USB_ECM_TX_HANDLE_COMPLETE_TIMER_MS 50
#elif   (HS_USB_ECM_TX_HANDLE_COMPLETE_TIMER_MS < 1)
  #error "HS_USB_ECM_TX_HANDLE_COMPLETE_TIMER_MS must be at least 1"
#endif /* ! HS_USB_ECM_TX_HANDLE_COMPLETE_TIMER_MS */


// HS_USB_NUM_OF_ECM_INST value is determined by the combination of feature 
// flags.  So, new array size that is bigger than the max possible ECM_USERS 
// is required
#define HSU_ECM_STRING_ARRAY_SIZE 8

#if (HSU_ECM_STRING_ARRAY_SIZE < HS_USB_NUM_OF_ECM_INST)
  #error "insufficient HSU_ECM_STRING_ARRAY_SIZE < HS_USB_NUM_OF_ECM_INST"
#endif

#ifndef HSU_ECM_CONTROL_STRING_0
#define HSU_ECM_CONTROL_STRING_0 NULL
#endif /* HSU_ECM_CONTROL_STRING_0 */
#ifndef HSU_ECM_DATA_STRING_0
#define HSU_ECM_DATA_STRING_0 NULL
#endif /* HSU_ECM_DATA_STRING_0 */
#ifndef HSU_ECM_CONTROL_STRING_1
#define HSU_ECM_CONTROL_STRING_1 NULL
#endif /* HSU_ECM_CONTROL_STRING_1 */
#ifndef HSU_ECM_DATA_STRING_1
#define HSU_ECM_DATA_STRING_1 NULL
#endif /* HSU_ECM_DATA_STRING_1 */
#ifndef HSU_ECM_CONTROL_STRING_2
#define HSU_ECM_CONTROL_STRING_2 NULL
#endif /* HSU_ECM_CONTROL_STRING_2 */
#ifndef HSU_ECM_DATA_STRING_2
#define HSU_ECM_DATA_STRING_2 NULL
#endif /* HSU_ECM_DATA_STRING_2 */
#ifndef HSU_ECM_CONTROL_STRING_3
#define HSU_ECM_CONTROL_STRING_3 NULL
#endif /* HSU_ECM_CONTROL_STRING_3 */
#ifndef HSU_ECM_DATA_STRING_3
#define HSU_ECM_DATA_STRING_3 NULL
#endif /* HSU_ECM_DATA_STRING_3 */
#ifndef HSU_ECM_CONTROL_STRING_4
#define HSU_ECM_CONTROL_STRING_4 NULL
#endif /* HSU_ECM_CONTROL_STRING_4 */
#ifndef HSU_ECM_DATA_STRING_4
#define HSU_ECM_DATA_STRING_4 NULL
#endif /* HSU_ECM_DATA_STRING_4 */
#ifndef HSU_ECM_CONTROL_STRING_5
#define HSU_ECM_CONTROL_STRING_5 NULL
#endif /* HSU_ECM_CONTROL_STRING_5 */
#ifndef HSU_ECM_DATA_STRING_5
#define HSU_ECM_DATA_STRING_5 NULL
#endif /* HSU_ECM_DATA_STRING_5 */
#ifndef HSU_ECM_CONTROL_STRING_6
#define HSU_ECM_CONTROL_STRING_6 NULL
#endif /* HSU_ECM_CONTROL_STRING_6 */
#ifndef HSU_ECM_DATA_STRING_6
#define HSU_ECM_DATA_STRING_6 NULL
#endif /* HSU_ECM_DATA_STRING_6 */
#ifndef HSU_ECM_CONTROL_STRING_7
#define HSU_ECM_CONTROL_STRING_7 NULL
#endif /* HSU_ECM_CONTROL_STRING_7 */
#ifndef HSU_ECM_DATA_STRING_7
#define HSU_ECM_DATA_STRING_7 NULL
#endif /* HSU_ECM_DATA_STRING_7 */

#ifdef T_UNDP
#define NORMAL_INTERFACE_NO 0
#define ALTERNATE_NET_INTERFACE_NO 5
#endif /* T_UNDP */

#ifdef FEATURE_HS_USB_ECM_BAM
#define HSU_ECM_NUM_OF_HW_ACC_INST 2
static const uint32 hsu_ecm_num_of_hw_acc_inst = HSU_ECM_NUM_OF_HW_ACC_INST; //to be removed when runtime modification is not necessary 

#endif /* FEATURE_HS_USB_ECM_BAM */

/*==============================================================================
Typedefs
=============================================================================*/
/* UL data transfer callback  */
typedef  bam_dmux_status(* bam_dmux_rx_ptr)
(
    bam_dmux_handle handle,
    void            *tx_priv,
    void            *data,
    uint16          data_len
        );
    /* DL done callback */
typedef  bam_dmux_status(* bam_dmux_tx_ptr)
(
    bam_dmux_handle handle,
    void            *rx_priv
);



/*
* Enum representing the current state of an adaptation layer instance.
*/
typedef enum
{
  HSU_ECM_INSTANCE_STATE_NONE,
  /* initial value, before hsu_al_ecm_init was called */
  HSU_ECM_INSTANCE_STATE_INITIALIZED,
  /* hsu_al_ecm_init was called, but neither hsu_al_ecm_control_open nor 
  hsu_al_ecm_data_open were called */
  HSU_ECM_INSTANCE_STATE_HALF_OPENED,
  /* hsu_al_ecm_init was called, and one of either and hsu_al_ecm_control_open 
  or hsu_al_ecm_data_open (but not both yet). This state means there's one
  more call to hsu_al_ecm_*_open() expected before the instance is 
  considered fully OPENED. */
  HSU_ECM_INSTANCE_STATE_OPENED
  /* hsu_al_ecm_init, hsu_al_ecm_control_open and hsu_al_ecm_data_open were 
  called */
} hsu_al_ecm_instance_states_enum;

/*
* This struct contains the application level CB, that should be called 
* from the hsu_al_ecm callback invoked by the FD. 
*/
typedef struct
{
  /* Host to device request handlers  */
  /************************************/
  sio_vpu_func_ptr_type enable_disable_cb_ext;
  /* the application CB to be called when the enabled/disabled state of the 
  instance changes */
  void *                enable_disable_ext_user_data;
  /* the user data to be passed when the application CB above is invoked */

#ifdef FEATURE_HS_USB_STANDARD_ECM
  sio_enable_set_packet_filter_event_func_ptr_type set_packet_filter_cb;
  /* the application CB to be called when "Set Ethernet Packet Filter" request is received */
  void *                set_packet_filter_user_data;
  /* the user data to be passed when the application set_packet_filter CB above is invoked */
#endif /*FEATURE_HS_USB_STANDARD_ECM*/
} hsu_al_ecm_app_cb_type;

struct hsu_al_ecm_instance_info_type; /* forward declaration */
typedef struct hsu_al_ecm_instance_info_type hsu_al_ecm_instance_info_type;

/*
* This structure contains information related to a buffer, given to the stack
* for control write operations.
*/
typedef struct 
{
  #ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
  /* WARNING: 
   * Fist 4-bytes will be used for sequence number.
   * Do not add any new members front of ctrl_tx_buffer as it could cause
   * alignment issue, when trying to read first 4-byte as juint32_t */
  uint8   ctrl_tx_buffer[sizeof(juint32_t) + HSU_ENCAPS_CMD_MAX_SIZE];
  #else  /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
  uint8   ctrl_tx_buffer[HSU_ENCAPS_CMD_MAX_SIZE];
  #endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
  uint32  ctrl_tx_num_of_bytes; /* the number of bytes actually used in the buffer */
} hsu_al_ecm_control_write_buffer_data_type;
 

/*
* This struct contains information related to the control plane of a specific
* network function instance
*/
typedef struct 
{
  sio_stream_id_type                stream_id;
  /* the unique identifier of this instance's control plane
  The unique ID is allocated by sio_control_open() */
  dsm_watermark_type *              rx_queue_ptr;      
  /* the RX WM Q provided by the application */
  dsm_watermark_type *              tx_queue_ptr; 
  /* the TX WM Q provided by the application */
  sio_rx_func_ptr_type              rx_func_ptr;
  /* the RX notification callback provided by the application */
  hsu_al_ecm_control_write_buffer_data_type write_buffer_data;
  /* Details of a buffer given to the stack using cdc_send_encap_response */ 
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  dsm_item_type * pending_dsm_ptr;
  boolean need_resending_pending_dsm;
  /* pending dsm chain pointer */
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
} hsu_al_ecm_control_info_type;

/* This struct contains information needed for re-sending a Connection Speed 
Changed Notification to the host. The resending might be needed in case when
the instance was disabled or the device was suspended. */
typedef struct 
{
  boolean                           resend;
  ecm_conn_speed_t                  connection_speed_args;
} hsu_al_ecm_conn_speed_mem_type;

typedef struct  
{
  /* Indication whether or not inbound flow is enabled, which means more 
  read operations can be done. If disabled, no further reads should be made. */
  boolean inbound_flow_is_enabled;
  /* when inbound flow is disabled num_buf_waiting will hold number of
  buffers waiting to be forwarded to the application upon enabling 
  inbound flow back */
  uint32 num_buf_waiting;

  /* current buffer chain used for Rx operation */
  rb_t        rx_chain[ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ];

  /* current buffer chain used for Rx operation */
  rb_t        *rx_chain_ptr;
  /* array size should be ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ but 
  ECM_ROUNDED_DSM_ITEM_SIZE is not known at compile time so assume worst
  case of 512 bytes*/
  /* status, buf pointer and size for each Rx buffer */
  cdc_iostatus_t        buf_status_array[ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ];
  uint8*                buf_array[ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ];
  uint32                buf_size_array[ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ];
  /* number of Rx buffers given in each cdc_read_multiple call */
  uint32                array_size;
  /* number of Rx buffers already forwarded to the application */
  uint32                num_buf_already_forwarded;
  /* if rx_done is called with partial Ethernet packet it will be stored here.
  when rx_done is later called with the rest of the packet it will get 
  appended to the stored part */
  dsm_item_type* partial_packet_ptr;
  /* rx timeout value (for  multiple read)*/
  uint32                rx_timeout;
  /* determines whether a transfer is pending */
  boolean is_pending;
} ecm_data_rx_info_type;

typedef struct  
{
  /* DSM chain of pending Tx requests. append this chain each time there is a
  call to cdc_write_multiple() */
  rb_t*        pending_chain_ptr;

  /* current buffer chain used for Tx operation */
  rb_t        tx_chain[ECM_MAX_NUM_RB_ITEMS_FOR_WRITE];
  
  /* number of DSM items in above chain */
  uint32                pending_chain_len;
  /* Timer struct for Tx handle complete timer */
  hsu_timer_handle_type tx_handle_complete_timer;
  /* tx timeout value (for  multiple write)*/
  uint32                tx_timeout;
  /* A flag to notify if tx_handle_complete_timer was already set. we would like 
     to set the timer only if it is not set, otherwise it may never invoke (we
     will keep setting it) */ 
  boolean               tx_handle_comp_scheduled;

  q_type              tx_queue;
  q_type              *tx_queue_ptr;

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  q_type tx_pending_queue;
  q_type *tx_pending_queue_ptr;
  q_type tx_resending_queue;
  q_type *tx_resending_queue_ptr;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
} ecm_data_tx_info_type;


/*
* This struct contains information related to the data plane of a specific
* network function instance
*/
typedef struct
{
  sio_stream_id_type                stream_id;
  /* the unique identifier of this instance's data plane. 
  The unique ID is allocated by sio_open() */

  ecm_data_rx_info_type rx_info;
  ecm_data_tx_info_type tx_info;
  dsm_watermark_type* rx_queue_ptr;      
  /* the TX WM Q provided by the application - queue itself will not be used but 
     only high/low callbacks */
  bam_dmux_rx_ptr             rx_func_ptr;
   /* the UL/RX notification callback provided by the application */
  bam_dmux_tx_ptr             tx_done_ptr;
  /* the DL/TX done notification callback for the application */
#ifdef FEATURE_HS_USB_STANDARD_ECM
  hsu_al_ecm_usb_iNiche_rx_cb_type         rx_func_ptr_std;
  hsu_al_ecm_usb_iNiche_tx_done_cb_type    tx_done_ptr_std;
#endif

  boolean              channel_opened;
  bam_dmux_handle      dmux_handle;
  /* state of data channel opening */
#ifdef FEATURE_HS_USB_STANDARD_ECM
  /* parameters for sending a Connection Speed Changed Notification to the host.
     Used only in Standard ECM*/
  ecm_conn_speed_t                  connection_speed_args;
  hsu_al_ecm_conn_speed_mem_type    conn_speed_mem;
#endif /*FEATURE_HS_USB_STANDARD_ECM*/
} hsu_al_ecm_data_info_type;

typedef struct  
{
  uint32 rx_zlp_count;
  uint32 rx_total_bytes;
  uint32 rx_timeout_cnt;
  uint32 rx_transfer_error_cnt;
  uint32 rx_zero_dsm_cnt;

  uint32 tx_total_bytes;
  uint32 tx_reached_max_pending_cnt;
  uint32 tx_max_num_pending;
  uint32 tx_dropped_packet_cnt;
  uint32 tx_transfer_error_cnt;
  uint32 tx_pkt_cnt;

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  uint32 tx_control_dropped_packet_inst_disabled_cnt;
  uint32 tx_control_dropped_packet_empty_queue_cnt;
  uint32 tx_control_dropped_packet_error_cnt;
  uint32 tx_control_dropped_packet_esuspended_cnt;
  uint32 tx_control_dropped_packet_encap_not_ready_cnt;
  uint32 tx_control_resent_notification_cnt;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  uint32 mem_allocation_fails;
} ecm_statistics_type;

#ifdef FEATURE_HS_USB_ECM_BAM
typedef struct
{
  boolean is_bam_mode;
  boolean ul_bam_flow_enabled;
  boolean dl_bam_flow_enabled;
  boolean bam_pipes_allocated;

  hsu_bam_connection_type       bam_connection;
} hsu_al_ecm_bam_info_type;

#endif /* FEATURE_HS_USB_ECM_BAM */

/*
* This struct contains information of a specific network function instance
*/
struct hsu_al_ecm_instance_info_type
{
  cdc_handle_t                    handle;
  /* A unique handle for this instance, that is used for the various stack 
  APIs (CDC's or ECM's) */
  hsu_al_ecm_instance_states_enum state;
  /* the current state of this instance */
  boolean                         instance_is_enabled;
  /* Indicates whether this instance is enabled (TRUE) or disabled (FALSE) */
  hsu_al_ecm_app_cb_type          app_callbacks;
  /* the application level CB, to be called from the hsu_al_ecm callbacks, 
  that are invoked by the FD. */
  hsu_al_ecm_control_info_type    control_info;
  /* information related to the control plane of this instance */
  hsu_al_ecm_data_info_type       data_info;
  /* information related to the data plane of this instance */
  sio_port_id_type      port_id;  
  /* SIO port id. this will identify the application instance */
  boolean             ctrl_line_state;
  /* Control Line State: See CDC Spec. Section 6.2.14 */

  char mac_address[MAC_ADDR_STR_SIZE];
  /* When working in Standard ECM the MAC address is calculated according to the ESN/IMEI.
     When not in Standard ECM this value is fake.
     The calculated value is stored here. */

#ifdef FEATURE_HS_USB_ECM_BAM
  hsu_al_ecm_bam_info_type bam_info;
#endif /* FEATURE_HS_USB_ECM_BAM */

  ecm_statistics_type statistics;
  boolean debug_mode;
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  boolean ignore_disconnect;
  boolean is_core_enabled;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
  uint32 usb_ecm_packet_index;
#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
};

/*==============================================================================
Variables
==============================================================================*/
/* 
* The instances information 
*/
static hsu_al_ecm_instance_info_type ecm_instance_info[HS_USB_NUM_OF_ECM_INST];

/* 
* Keeps the handles or the FDs that the stack initialized.
* When the configuration will be changed we will use the corresponding handle 
* for the stack's FD.
* The number of FD handles that will be initialized by the stack is:
* for (i=0; i<HSU_MAX_NUM_OF_CONFIGS)
* {
*    num_of_initialized_fds += num_of_ecm_instances_in_curr_config
* }
*/
  cdc_handle_t ecm_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS][HS_USB_NUM_OF_ECM_INST];

/* 
* The number of ECM instances that were allocated and initialized by FD.
* This value can be larger than  HS_USB_NUM_OF_ECM_INST. See documentation of
* ecm_fd_handle for more details
*/
  static uint8 hsu_al_ecm_num_of_initialized_instances[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS] = {0};

static hsu_bus_mode_enum hsu_al_ecm_suspend_resume_mode = HSU_BUS_MODE_SUSPENDED;


/* Boolean flag that indicates if ECM AL is registered with the bus mode 
  notifications */
static boolean bus_mode_notif_registered = FALSE;

/* String descriptors for control and data interfaces */
static struct 
{
  char *control, *data;
} hsu_al_ecm_strings[HSU_ECM_STRING_ARRAY_SIZE] = 
{
  {HSU_ECM_CONTROL_STRING_0 , HSU_ECM_DATA_STRING_0},
  {HSU_ECM_CONTROL_STRING_1 , HSU_ECM_DATA_STRING_1},
  {HSU_ECM_CONTROL_STRING_2 , HSU_ECM_DATA_STRING_2},
  {HSU_ECM_CONTROL_STRING_3 , HSU_ECM_DATA_STRING_3},
  {HSU_ECM_CONTROL_STRING_4 , HSU_ECM_DATA_STRING_4},
  {HSU_ECM_CONTROL_STRING_5 , HSU_ECM_DATA_STRING_5},
  {HSU_ECM_CONTROL_STRING_6 , HSU_ECM_DATA_STRING_6},
  {HSU_ECM_CONTROL_STRING_7 , HSU_ECM_DATA_STRING_7},
};

#ifdef FEATURE_HS_USB_ECM_BAM
/* this is an aggregate count for all ECM instances */
static uint32 ecm_num_of_allocated_bam_connections = 0;

#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM // This table will be used later
static const sio_port_id_type hsu_sio_port_id_table[] = 
{
  SIO_PORT_USB_NET_WWAN,
//  SIO_PORT_USB_RMNET2, // 2nd ECM will always be DPL
  SIO_PORT_USB_DPL,
  SIO_PORT_USB_RMNET3,
  SIO_PORT_USB_RMNET4,
  SIO_PORT_USB_RMNET5,
  SIO_PORT_USB_RMNET6,
  SIO_PORT_USB_RMNET7,
  SIO_PORT_USB_RMNET8,
};
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM

#endif /* FEATURE_HS_USB_ECM_BAM */

#ifdef FEATURE_HS_USB_MBIM
static hsu_al_ecm_register_mbim_cb_list_type mbim_cb_list;
static boolean is_mbim_cb_registered = FALSE;
#endif /* FEATURE_HS_USB_MBIM */

/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/
static void hsu_al_ecm_encap_resp_comp_cb
(
 cdc_iostatus_t  status,
 juint32_t       bytes_sent,
 cdc_appctx_t    ctx
 );

#ifdef FEATURE_HS_USB_STANDARD_ECM
static void remember_conn_speed_notif
(
 hsu_al_ecm_instance_info_type * context_ptr
);
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

void hsu_al_ecm_handle_set_configuration
(
 hsu_al_ecm_instance_info_type *context_ptr
);

void 
hsu_al_ecm_dmux_event_notify(
 bam_dmux_handle     handle,
 void                *priv,
 bam_dmux_event      event,
 bam_dmux_event_data *event_data
);


#ifdef FEATURE_HS_USB_MBIM
/*==============================================================================

FUNCTION      HSU_AL_ECM_SET_NTB_INPUT_SIZE_DONE

DESCRIPTION   
Informs HS USB that SetNTBSize is done in QBI Layer 

DEPENDENCIES  
None.

RETURN VALUE  
None

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_set_ntb_input_size_done
(
  cdc_handle_t handle,
  jresult_t wstatus
)
{
	jsafe_enter();
	/*This has to be handled in ECM_FD*/
	set_ntb_input_size_complete(handle,wstatus);

	jsafe_leave();
	
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_RESET_FUNCTION_DONE

DESCRIPTION   
Called from QBI to let HS-USB know that Reset is done successfully 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE/ FALSE

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_reset_function_done
(
  cdc_handle_t handle,
  jresult_t wstatus
)
{
	jsafe_enter();
	/*This has to be handled in ECM_FD*/
	handle_reset_function_complete(handle,wstatus);

	jsafe_leave();
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_REGISTER_CTL_INTRCPT_CALLBACKS

DESCRIPTION   
Register intercept functions of control messages between SIO and HSU_AL_ECM

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
void hsu_al_ecm_register_control_intercept_cb
( 
  hsu_al_ecm_control_intercept_cb_type control_intercept_cb
)
{
	if(NULL != mbim_cb_list.hsu_al_ecm_usb_to_qbi.hsu_al_ecm_control_intercept_cb)
	{
		mbim_cb_list.hsu_al_ecm_usb_to_qbi.hsu_al_ecm_control_intercept_cb
		= control_intercept_cb;
	}
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_REGISTER_MBIM_CB

DESCRIPTION   
Registers the MBIM function Callbacks between QBI and HSUSB

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
void hsu_al_ecm_register_mbim_cb
( 
	hsu_al_ecm_register_mbim_cb_list_type *cb_list
)
{
	HSU_ASSERT(cb_list != NULL);
	/*Map the callbacks received from QBI layer*/
	memscpy((void*)&mbim_cb_list, sizeof(hsu_al_ecm_register_mbim_cb_list_type),
	  cb_list,sizeof(hsu_al_ecm_register_mbim_cb_list_type));
	/*There are two callbacks which we have to map*/

	/*These two are completion callbacks which will be triggered from QBI*/
	cb_list->hsu_al_ecm_qbi_to_usb.hsu_al_ecm_set_ntb_input_size_complete_cb 
	= hsu_al_ecm_set_ntb_input_size_done;

	/* No need to complete cb for rest function as status's returned already */
	cb_list->hsu_al_ecm_qbi_to_usb.hsu_al_ecm_reset_mbim_complete_cb 
	  = NULL;

	/*After successfully registering the callbacks, set the flag true*/
	is_mbim_cb_registered = TRUE;
}
#endif /* FEATURE_HS_USB_MBIM */

static void perform_control_transmit
( 
 hsu_al_ecm_instance_info_type *  context_ptr, 
 dsm_item_type *                  tx_ptr
);

#ifdef FEATURE_HS_USB_BAM_RESET
/* marker: to create CB for BAM consumer wakeup */

void hsu_al_ecm_bam_enable_cb(void *app_data);
#endif /* FEATURE_HS_USB_BAM_RESET */

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
/*==============================================================================

FUNCTION      HSU_GET_DIAG_HEALTH_ECM_STATISTICS_TYPE

DESCRIPTION   
ALLOWS ACCESS TO ecm_statistics_type OUTSIDE OF FUNCTION

DEPENDENCIES  
None.

RETURN VALUE  
POINTER TO ARRAY

SIDE EFFECTS  
None.
==============================================================================*/

void hsu_get_diag_health_ecm_statistics_type(uint32* temp[])
{
  int i = 0;
  int j = 0;
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
	j=i*10;
	temp[j]=(uint32*)&ecm_instance_info[i].statistics.rx_zlp_count;
	temp[j+1]=(uint32*)&ecm_instance_info[i].statistics.rx_total_bytes;
	temp[j+2]= (uint32*)&ecm_instance_info[i].statistics.rx_timeout_cnt;
	temp[j+3]= (uint32*)&ecm_instance_info[i].statistics.rx_transfer_error_cnt;
	temp[j+4]= (uint32*)&ecm_instance_info[i].statistics.tx_total_bytes;
	temp[j+5]= (uint32*)&ecm_instance_info[i].statistics.tx_reached_max_pending_cnt;
	temp[j+6]= (uint32*)&ecm_instance_info[i].statistics.tx_max_num_pending;
	temp[j+7]= (uint32*)&ecm_instance_info[i].statistics.tx_dropped_packet_cnt;
	temp[j+8]= (uint32*)&ecm_instance_info[i].statistics.tx_transfer_error_cnt;
	temp[j+9]= (uint32*)&ecm_instance_info[i].statistics.mem_allocation_fails;
  }
  return;
} /* hsu_get_diag_health_ecm_statistics_type */
#endif /* FEATURE_HS_USB_HEALTH_MONITOR */

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
/*===========================================================================

FUNCTION hsu_al_ecm_transmit_pending_control_pkt

DESCRIPTION
The function is supposed to be called in HSU AL task. 
It is to send the dsm in control tx_queue_ptr after ECM FD is enabled.


DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_ecm_transmit_pending_control_pkt(void * param)
{
  hsu_al_ecm_instance_info_type * context_ptr = 
    (hsu_al_ecm_instance_info_type *)param;

  dsm_item_type * curr_dsm_ptr;

  if(context_ptr->control_info.tx_queue_ptr)
  {
    /* check if an active NOTIFY_RESPONSE_AVAILABLE and GET_ENCAPSULATED_RESPONSE
         is still in progress */
    if (cdc_get_encapsulated_response_state(context_ptr->handle) != CDC_ENCAP_READY)
    {
      HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)((cdc_ctx_t*)context_ptr->handle)->desc->interfaces->number,
        "cdc_not_ready");

      context_ptr->statistics.tx_control_dropped_packet_encap_not_ready_cnt++;
      return;
    }

    if(context_ptr->control_info.need_resending_pending_dsm)
    {
      curr_dsm_ptr = context_ptr->control_info.pending_dsm_ptr;
      context_ptr->control_info.need_resending_pending_dsm = FALSE;
    }
    else
    {
      curr_dsm_ptr = dsm_dequeue(context_ptr->control_info.tx_queue_ptr);
    }

    if(curr_dsm_ptr == NULL)
    {
      context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
      return;
    }
    
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 
      dsm_length_packet(curr_dsm_ptr);
    
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)((cdc_ctx_t*)context_ptr->handle)->desc->interfaces->number,
        "app_field %u", curr_dsm_ptr->app_field);
    
    perform_control_transmit(context_ptr, curr_dsm_ptr);
  }
}

/*===========================================================================

FUNCTION hsu_al_ecm_invoke_resending_droped_packet

DESCRIPTION
invoke resending dropped packets.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_ecm_invoke_resending_droped_packet
(
  hsu_al_ecm_instance_info_type * context_ptr
)
{
  if(context_ptr->state == HSU_ECM_INSTANCE_STATE_OPENED)
  {
    /* start transmition of pending data */
    HSU_MSG_HIGH_1("hsu_al_ecm_invoke_resending_droped_packet %d", context_ptr);
  
    /* trigger resending data packet */
    //hsu_al_ecm_set_handle_complete_timer(context_ptr);
  
    /* trigger resending control packet */
    hsu_al_task_enqueue_cmd(hsu_al_ecm_transmit_pending_control_pkt, context_ptr);
  }
}
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */


/*==============================================================================

FUNCTION      HSU_AL_ECM_IS_CDC_APP_CONTEXT_VALID

DESCRIPTION   
Verifies that the cdc_appctx_t given by CDC stack functions contains a valid 
value. 

Goes over all existing instances and verifies that there's one that matches the input. 
Called from Jungo cb functions when the Giant mutex is locked. 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE if the context is valid, FALSE if not.

SIDE EFFECTS  
None.
==============================================================================*/
static boolean hsu_al_ecm_is_cdc_app_context_valid
(
 cdc_appctx_t  context
 )
{
  int i = 0;
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
    if(context == &ecm_instance_info[i])
  {
    return TRUE;
  }
  }
  HSU_MSG_ERROR_1(
    "hsu_al_ecm_is_cdc_app_context_valid(): Context is invalid context=0x%04x",
    context);
  return FALSE;
} /* hsu_al_ecm_is_cdc_app_context_valid */

#ifdef FEATURE_HS_USB_STANDARD_ECM
/*==============================================================================

FUNCTION      DEFER_CALL_TO_NOTIF_CON_SPEED_CH

DESCRIPTION
This function will try to send CONN_SPEED_CHANGE notification by calling
ecm_notify_conn_speed_change API.
Right now it's called only from HSUSB context so the Mutex is already locked. 

DEPENDENCIES  
This function should be called when the Giant Mutex is locked

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void defer_call_to_notif_con_speed_ch(hsu_al_ecm_instance_info_type* context_ptr)
{
  jresult_t jresult;
  /* 
  * Verify that the instance is not disabled. If it is disabled, do not issue
  * a new cdc_read 
  */

  if(context_ptr->instance_is_enabled == FALSE)
  {
    HSU_MSG_HIGH_1(
      "defer_call_to_notif_con_speed_ch(): Instance is disabled (handle=%d). Ignoring.",
      context_ptr->handle);

    /* If the notification is disable_inbound_flow then we need to 
       remember and resend it. We don't need to check upstream since it's 
       always up*/
    if (context_ptr->data_info.connection_speed_args.downstream_bitrate == 0)
    {
      remember_conn_speed_notif(context_ptr);
    }
    return;
  }

  jresult = ecm_notify_conn_speed_change(context_ptr->handle,
    &context_ptr->data_info.connection_speed_args);

  if(jresult == ESUSPENDED)
  {
    /* The device is suspended (for example - the USB cable was disconnected). */
    HSU_MSG_HIGH_1(
      "defer_call_to_notif_con_speed_ch(): ecm_notify_conn_speed_change "
      "returned ESUSPENDED. Remembering notification (handle=%d)",
      context_ptr->handle);

    /* We need to remember the notification and resend upon resume since the host
      remembers the flow state before suspension.*/
    remember_conn_speed_notif(context_ptr);
    return;
  }

  if(jresult != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL(
      "defer_call_to_notif_con_speed_ch(): "
      "ecm_notify_conn_speed_change returned error %d."
      "connection.down=0x%x, handle=%d", 
      jresult,
      context_ptr->data_info.connection_speed_args.downstream_bitrate,
      context_ptr->handle);
  }
} /* defer_call_to_notif_con_speed_ch */
#endif /*FEATURE_HS_USB_STANDARD_ECM*/


/*==============================================================================

FUNCTION      INVOKE_APP_ENABLE_DISABLE_CBS

DESCRIPTION   
Invokes the application DTR callbacks, if they were registered.
Called when the Giant mutex is locked:
1. On enable\disable\line_state_cb mutex locked by HSUSB0 task
2. On SIO_IOCTL_ENABLE_DTR_EVENT_EXT mutex locked by app in hsu_al_ecm_ioctl() 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void invoke_app_enable_disable_cbs
(
 hsu_al_ecm_instance_info_type * context_ptr
 )
{
  HSU_ASSERT(context_ptr != NULL);

  if(context_ptr->app_callbacks.enable_disable_cb_ext != NULL)
  {
    HSU_MSG_MED_1(
      "invoke_app_enable_disable_cbs(): calling application ext CB (handle=%d)",
      context_ptr->handle);
    context_ptr->app_callbacks.enable_disable_cb_ext(
      context_ptr->app_callbacks.enable_disable_ext_user_data);
  }
} /* invoke_app_enable_disable_cbs */

/*===========================================================================

FUNCTION hsu_al_ecm_setup_read

DESCRIPTION
Sets up a new read operation for all idle buffers
 
DEPENDENCIES
This function is assumed to be called in stack context, so the giant mutex 
is already locked. No need to re-lock it. 

RETURN VALUE
None.

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_ecm_setup_read
(
  hsu_al_ecm_instance_info_type* context_ptr
)
{

  uint32 i;
  void *buffer;
  jresult_t cdc_read_res;
  rb_t *rx_rb_chain_ptr = NULL, *cur_rb_ptr = NULL;
  ecm_data_rx_info_type* rx_info_ptr = &context_ptr->data_info.rx_info;

  HSU_ASSERT(rx_info_ptr);

  HSU_ULOG(NO_MSG, DATA_LOG, " Entry ");
  
  /* inbound_flow_is_enabled check is not required in this API.
   * The API that gets invoked when RX data is recieved, 
   * hsu_al_ecm_rx_done_internal, will check for inbound flow control
   * and make decision whether to call this API again for setting up
   * RX buffer in HW.
   */
  if (FALSE == context_ptr->instance_is_enabled)
  {
    HSU_MSG_HIGH("hsu_al_ecm_setup_read: instance is not enabled");
    return;
  }

  hsu_perf_log( PERF(HSU_AL_ECM_SETUP_READ, Entry), 0);

  HSU_ASSERT(rx_info_ptr->array_size <= ECM_MAX_NUM_DSM_ITEMS_IN_CDC_READ);
  
  /* rb queue has to be empty for the queuing the next request, if not then a few of  previous 
        items might still be pending to be sent */
  if(FALSE == rb_queue_empty(rx_info_ptr->rx_chain, rx_info_ptr->array_size))
  {
    HSU_ULOG(NO_MSG, DATA_LOG, "No free rb's");
    return;
  }
  
  cur_rb_ptr = rx_info_ptr->rx_chain;
  rx_info_ptr->rx_chain_ptr = cur_rb_ptr;
  rx_rb_chain_ptr = cur_rb_ptr;
  
  for (i=0; i<rx_info_ptr->array_size; ++i)
  {
    if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0 )
    {
      *((uint32 *)cur_rb_ptr->buffer_pa) = BAM_DMUX_HEADER_CLIENT_SIGNATURE;
      buffer = (uint8 *)cur_rb_ptr->buffer_pa + BAM_DMUX_HEADER_SIZE ;
    }
    else
    {
      buffer = (uint8 *)cur_rb_ptr->buffer_pa;
    }
	
    rx_info_ptr->buf_array[i] = buffer;
    rx_info_ptr->buf_size_array[i] = ECM_ROUNDED_DSM_ITEM_SIZE;
    cur_rb_ptr->free = FALSE;
    cur_rb_ptr = cur_rb_ptr->next;
  }

  hsu_trace_iface_set(NET1_HSU_IFACE);
  hsu_trace_msg2_set(HSU_AL_ECM_SETUP_READ__TRC);

  cdc_read_res = cdc_read_multiple(context_ptr->handle, 
    (void**)rx_info_ptr->buf_array,
    rx_info_ptr->buf_size_array,
    rx_info_ptr->array_size, rx_info_ptr->rx_timeout);

  hsu_trace_iface_clear();
  hsu_trace_msg2_clear();

  switch(cdc_read_res)
  {
  case ESUSPENDED:
    HSU_MSG_HIGH_3("cdc_read_multiple returned ESUSPENDED", 0, 0, 0);
    /* a new read operation will be invoked upon resume */
    rb_queue_free(rx_rb_chain_ptr, FALSE);
    break;

  case HSU_JSUCCESS:
    HSU_ULOG(NO_MSG, DATA_LOG, "Rx buffer submitted");
    break;

  default:
    rb_queue_free(rx_rb_chain_ptr, FALSE);
    ERR_FATAL("cdc_read_multiple failed: %d", cdc_read_res, 0, 0);
  }

}

#ifdef FEATURE_HS_USB_ECM_BAM
/*===========================================================================

FUNCTION is_in_bam_mode

DESCRIPTION
This function return TRUE if the current ECM instance is BAM. FALSE otherwise.

DEPENDENCIES
None.

RETURN VALUE
TRUE / FALSE.

SIDE EFFECTS
None.

===========================================================================*/
static boolean is_in_bam_mode(const hsu_al_ecm_instance_info_type* context_ptr)
{
  return context_ptr->bam_info.is_bam_mode;
}

/*===========================================================================

FUNCTION hsu_al_ecm_wakeup_host

DESCRIPTION
This function will trigger remote wakeup

DEPENDENCIES
Called in HSU_AL_TASK context upon pipe wakeup event from SPS

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_ecm_wakeup_host(void* cmd_param)
{
  hsu_al_ecm_instance_info_type* context_ptr = (hsu_al_ecm_instance_info_type*)cmd_param;

  HSU_ULOG_1(NO_MSG, BUS_LOG, "instance_is_enabled %u", context_ptr->instance_is_enabled);

  if (context_ptr->instance_is_enabled == FALSE)
  {
    return;
  }
 
  hsu_al_task_signal_remote_wakeup();
} /* hsu_al_ecm_wakeup_host */ 


/*===========================================================================

FUNCTION hsu_al_ecm_pipe_wakeup_cb

DESCRIPTION
This function is the callback function for SPS pipe wakeup event.
This callback will be invoked when USB is in suspend and SPS pipe now has data
to send to host. We need to trigger remote wakeup so SPS data can be delivered
to host.
We let HSU_AL_TASK take care of remote wakeup so that SPS driver will not get
delayed

DEPENDENCIES
None.

RETURN VALUE
always return NULL

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_ecm_pipe_wakeup_cb(bam_result_type bam_result)
{  
   HSU_ULOG_1(HIGH_MSG, BUS_LOG, "bam_result_event %u", bam_result.event);
 
   if (bam_result.event != BAM_EVENT_WAKEUP)
   {
     HSU_ERR_FATAL("hsu_al_ecm_pipe_wakeup_cb: unexpected event %d", bam_result.event, 0, 0);
     return;
   }
 
   hsu_al_ecm_wakeup_host(&bam_result.cb_data);
   return;
}

/*===========================================================================

  FUNCTION hsu_al_ecm_enable_flow_all_bam

  ===========================================================================*/
void hsu_al_ecm_enable_flow_all_bam(boolean ul, boolean dl)
{
  hsu_al_ecm_instance_info_type *context_ptr;
  uint32 i;
  for (i = 0; (i < hsu_get_num_of_net_instances()) && (i < hsu_ecm_num_of_hw_acc_inst); i++)
  {
    context_ptr = &ecm_instance_info[i];
    if (context_ptr && context_ptr->bam_info.is_bam_mode)
    {
      hsu_al_ecm_enable_flow_with_bam(context_ptr, ul, dl);
    }
  }
}

/*===========================================================================

  FUNCTION hsu_al_ecm_enable_read_write_infinite_all_bam_ep

  DESCRIPTION
  This API should only be used for the bootup case where BAM pipe INIT implicitly
  performs a BAM pipe ENABLE
  ===========================================================================*/
void hsu_al_ecm_enable_read_write_infinite_all_bam_ep(void)
{
#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
  hsu_al_ecm_instance_info_type *context_ptr;
  uint32 i;
  for (i = 0; (i < hsu_get_num_of_net_instances()) && (i < hsu_ecm_num_of_hw_acc_inst); i++)
  {
    context_ptr = &ecm_instance_info[i];
    if (context_ptr)
    {
      // This is created to accomodate the design where bam_pipe_init( ) implicitly enabling the BAM pipe
      if (cdc_sps_read_write_infinite(context_ptr->handle,
          (juint8_t)context_ptr->bam_info.bam_connection.producer_pipe_index, HSU_AL_BAM_UL, /* DIRECTION OUT data from AP */
          (juint8_t)context_ptr->bam_info.bam_connection.consumer_pipe_index, HSU_AL_BAM_DL)) /* DIRECTION IN data from MODEM */
      {
        HSU_ERR_FATAL("read write infinite failed", 0, 0, 0);
      }
      context_ptr->bam_info.ul_bam_flow_enabled = TRUE;
      context_ptr->bam_info.dl_bam_flow_enabled = TRUE;
      HSU_ULOG_FUNC(BUS_LOG);
    }
  }
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
}

/*===========================================================================

FUNCTION hsu_al_ecm_enable_flow_with_bam

  DESCRIPTION
  This function configures the eTD and performs the read/write infinite to allow
  data to flow between the host controller and device controller.

  ===========================================================================*/
boolean hsu_al_ecm_enable_flow_with_bam(void* ctx_param, boolean ul, boolean dl)
{
  jresult_t cdc_res;
  boolean ret = FALSE;
  hsu_al_ecm_instance_info_type *context_ptr = (hsu_al_ecm_instance_info_type*)ctx_param;

  if (!context_ptr)
  {
    /* invalid instance context passed */
    HSU_ERR_FATAL("hsu_al_ecm_enable_flow_with_bam invoked with invalid context ptr", 0, 0, 0);
    return FALSE;
  }

  if (!context_ptr->instance_is_enabled)
  {
    HSU_ULOG(HIGH_MSG, BAM_LOG, "xcp instance_not_enabled");
    return FALSE;
  }

  if (!context_ptr->bam_info.is_bam_mode)
  {
    HSU_ULOG(HIGH_MSG, BAM_LOG, "xcp non_accelerated_pipe");
    return FALSE;
  }

  HSU_MSG_HIGH("hsu_al_ecm_enable_flow_with_bam: Enter");

  if(ecm_num_of_allocated_bam_connections > hsu_ecm_num_of_hw_acc_inst)
  {
    HSU_ERR_FATAL("Error allocating BAM pipes (%u) > hsu_ecm_num_of_hw_acc_inst (%u)",
      ecm_num_of_allocated_bam_connections, hsu_ecm_num_of_hw_acc_inst, 0);
  }


  if((FALSE == ul) || (context_ptr->bam_info.ul_bam_flow_enabled == TRUE))
  {
    HSU_ULOG(HIGH_MSG, BAM_LOG, "enable_ul, xcp bam_already_enabled");
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "enable_ul");
    // The definitions of read/write pipes may be confusing.
    // Here the context refers to USB HW perspective with the AP */
    cdc_res = cdc_sps_read_write_infinite(context_ptr->handle,
        (juint8_t)context_ptr->bam_info.bam_connection.producer_pipe_index, ul, /* DIRECTION OUT data from AP */
        (juint8_t)context_ptr->bam_info.bam_connection.consumer_pipe_index, FALSE); /* DIRECTION IN data from MODEM */
    HSU_ULOG_1(NO_MSG, BUS_LOG, "after cdc_sps_read_write_infinite, ul, cdc_res %u", (uint32)cdc_res);

    switch(cdc_res)
    {
      case HSU_JSUCCESS:
        {
          context_ptr->bam_info.ul_bam_flow_enabled = TRUE;
        }
        break;
  
      default:
        HSU_MSG_ERROR_1("hsu_al_ecm_enable_flow_with_bam: cdc_sps_read_write_infinite UL failed %d",
            cdc_res);
        HSU_ERR_FATAL("cdc_sps_read_write_infinite UL failed", cdc_res, 0, 0);
    }
  }

  if((FALSE == dl) || (context_ptr->bam_info.dl_bam_flow_enabled == TRUE))
  {
    HSU_ULOG(HIGH_MSG, BAM_LOG, "enable_dl, xcp dl_bam_already_enabled");
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "enable_dl");
    // The definitions of read/write pipes may be confusing.
    // Here the context refers to USB HW perspective with the AP */
    cdc_res = cdc_sps_read_write_infinite(context_ptr->handle,
        (juint8_t)context_ptr->bam_info.bam_connection.producer_pipe_index, FALSE, /* DIRECTION OUT data from AP */
        (juint8_t)context_ptr->bam_info.bam_connection.consumer_pipe_index, dl); /* DIRECTION IN data from MODEM */
    HSU_ULOG_1(NO_MSG, BUS_LOG, "after cdc_sps_read_write_infinite, dl, cdc_res %u", (uint32) cdc_res);

    switch(cdc_res)
    {
      case HSU_JSUCCESS:
        {
          context_ptr->bam_info.dl_bam_flow_enabled = TRUE;
        }
        break;
  
      default:
        HSU_MSG_ERROR_1("hsu_al_ecm_enable_flow_with_bam: cdc_sps_read_write_infinite DL failed %d",
            cdc_res);
        HSU_ERR_FATAL("cdc_sps_read_write_infinite DL failed", cdc_res, 0, 0);
    }
  }

  if (ul && dl)
  {
    ret = context_ptr->bam_info.ul_bam_flow_enabled && context_ptr->bam_info.dl_bam_flow_enabled;
  }
  if (ul)
  {
    ret = context_ptr->bam_info.ul_bam_flow_enabled;
  }
  if (dl)
  {
    ret = context_ptr->bam_info.dl_bam_flow_enabled;
  }

  HSU_MSG_HIGH("hsu_al_ecm_enable_flow_with_bam: Exit");
  return ret;
}

#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
/*=============================================================================
FUNCTION    hsu_al_ecm_disable_bam
=============================================================================*/
/**
Keeps track of which BAM is disabled via count.
Actual hooks to disconnect with IPA and the BAM is not implemented.

@param context_ptr [IN] -- ECM context

@dependencies
None.

@sideeffects
None.

@return
None.
*/
static void hsu_al_ecm_disable_bam
(
  hsu_al_ecm_instance_info_type* context_ptr, boolean ul, boolean dl
)
{
  HSU_MSG_HIGH("hsu_al_ecm_disable_bam: Enter");

  if (is_in_bam_mode(context_ptr))
  {
    if (ul)
    {
      context_ptr->bam_info.ul_bam_flow_enabled = FALSE;
    }
    if (dl)
    {
      context_ptr->bam_info.dl_bam_flow_enabled = FALSE;
    }

    // Never decrement ecm_num_of_allocated_bam_connections because the same bam pipes are used across bus resets
      // if(ecm_num_of_allocated_bam_connections > 0)
      // {
      //   ecm_num_of_allocated_bam_connections--;
      // }
      // else
      // {
      //   HSU_MSG_ERROR_1("hsu_al_ecm_disable_bam: Illegal number of BAM pipes allocated - %d", 
      //     ecm_num_of_allocated_bam_connections);
      // }
  }
  
  HSU_MSG_HIGH("hsu_al_ecm_disable_bam: Exit");
}
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
#endif /* FEATURE_HS_USB_ECM_BAM */

/*==============================================================================

FUNCTION      HSU_AL_ECM_SUSPEND_RESUME_MODE_CHANGED_CB

DESCRIPTION   
  Invoked when the USB stack gets suspended/resumed from HSUSB0 context
  so the Giant mutex is locked.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
  None.

==============================================================================*/
static void hsu_al_ecm_suspend_resume_mode_changed_cb
(
  hsu_bus_mode_enum new_mode,
  void* app_data             /*Not used*/
)
{
  uint32 i = 0;
  hsu_al_ecm_instance_info_type* context_ptr;

  HSU_USE_PARAM(app_data);
  if (hsu_al_ecm_suspend_resume_mode == new_mode)
  {
    HSU_MSG_ERROR("*_suspend_resume_mode_changed_cb: mode not changed");
    return;
  }

  hsu_al_ecm_suspend_resume_mode = new_mode;

  /* Go over all of RmNet instances (that are initialized) */
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
    context_ptr = &ecm_instance_info[i];
    /*If the instance is not initialized by fd do nothing */
    if (!context_ptr->handle)
    {
      continue;
    }

    /* IPA interaction is dependent on enumeration.
     * Keep all existing logic by adding this exclusion check based on CONFIGURED
     */
    if (new_mode != HSU_BUS_MODE_CONFIGURED)
    {
      if ((context_ptr->instance_is_enabled == FALSE) ||
          (context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED))
      {
        HSU_MSG_LOW_1("*_ecm_suspend_resume_mode_changed_cb: instance %d not enabled/opened", i);
        continue;
      }

    }

    switch (new_mode)
    {
      case HSU_BUS_MODE_RESUMED:
        /* if we are resumed we need to issue cdc_read for the buffers that were
           canceled upon suspend */

        hsu_al_ecm_setup_read(context_ptr);

#ifdef FEATURE_HS_USB_STANDARD_ECM
        if (context_ptr->data_info.conn_speed_mem.resend == TRUE)
        {
          /* resend connection speed notification */
          context_ptr->data_info.conn_speed_mem.resend = FALSE;
          context_ptr->data_info.connection_speed_args = 
            context_ptr->data_info.conn_speed_mem.connection_speed_args;
          defer_call_to_notif_con_speed_ch((void*)(context_ptr));
        }
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
        /* resending dropped packet */
        hsu_al_ecm_invoke_resending_droped_packet(context_ptr);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
        break;

#ifdef FEATURE_HS_USB_ECM_BAM
      case HSU_BUS_MODE_SUSPENDED:
        /* marker: to handle suspend/resume with the state machine */
        /* code logic must consider consumer pipe wakeup events */
        // no check required here
//        hsu_al_ecm_disable_bam(context_ptr, HSU_AL_BAM_UL, HSU_AL_BAM_DL);
        break;
#endif /* FEATURE_HS_USB_ECM_BAM */
    }
  }
}/*hsu_al_ecm_suspend_resume_mode_changed_cb*/

#ifdef FEATURE_HS_USB_ECM_BAM
// Input parameters are sanitized by caller
void hsu_al_ecm_configure_bam_and_dpm(hsu_al_ecm_instance_info_type *context_ptr)
{
#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
  uint32 bam_pipe_num = ecm_num_of_allocated_bam_connections * 2; /* assign bam pipe numbers and dbm ep numbers systematically */
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM

#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
  /* Note: DBM is already configured before this CB is exercised when EPs are initialized */
  if ((ecm_num_of_allocated_bam_connections < hsu_ecm_num_of_hw_acc_inst) &&
      (FALSE == context_ptr->bam_info.bam_pipes_allocated))
  {
    cdc_ctx_t *ctx;
    core_t *core;

    ctx = (cdc_ctx_t*)context_ptr->handle;
    HSU_ASSERT(ctx);
    core = (core_t*)ctx->core_ctx;
    HSU_ASSERT(core);

    // The hsu_sio_port_id_table is used to setup contiguous RMNET instances via offsets
    context_ptr->bam_info.bam_connection.sio_port_id = hsu_sio_port_id_table[ecm_num_of_allocated_bam_connections];
    context_ptr->bam_info.bam_connection.if_num = (uint32)(((cdc_ctx_t*)(context_ptr->handle))->in_pipe.if_num); /* use BULK IN if # */
    context_ptr->bam_info.bam_connection.producer_pipe_index = bam_pipe_num; /* follow historic convention of producer odd */
    context_ptr->bam_info.bam_connection.consumer_pipe_index = bam_pipe_num+1; /* follow historic convention of consumer even */
    HSU_ASSERT(context_ptr->bam_info.bam_connection.cdc_ctx_ptr == ctx);
    context_ptr->bam_info.bam_connection.cdc_ctx_ptr = ctx;

    /* this only configures the parameters without initializing the pipes */
    if (hsu_al_bam_allocate_pipes(&(context_ptr->bam_info.bam_connection)) == FALSE)
    {
      HSU_ERR_FATAL("hsu_al_bam_allocate_pipes() pipes failed", 0, 0, 0);
    }
    
    context_ptr->bam_info.bam_pipes_allocated = TRUE;
    context_ptr->bam_info.is_bam_mode = TRUE;
    ++ecm_num_of_allocated_bam_connections;
  }
#else // ~ FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
  if (ecm_num_of_allocated_bam_connections < hsu_ecm_num_of_hw_acc_inst)
  {
    ++ecm_num_of_allocated_bam_connections;
  }
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
}
#endif // FEATURE_HS_USB_ECM_BAM

/*==============================================================================

FUNCTION      HSU_AL_ECM_ENABLED_CB

DESCRIPTION   
Called by the FD when the device has successfully enumerated. 

DEPENDENCIES  
ecm_init() must have been called for this instance.

RETURN VALUE  
jresult_t, currently only HSU_JSUCCESS is possible (failures cause an ASSERT).

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_enabled_cb
(
 cdc_appctx_t ctx
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  cdc_ctx_t *cdc_ctx_ptr = NULL;

  if (hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_enabled_cb(): ctx (%u) param invalid",
      ctx, 0, 0);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)(ctx);
  HSU_MSG_HIGH_1("hsu_al_ecm_enabled_cb(): Enter (handle=%d)", context_ptr->handle);

  /* 
  * instance_is_enabled doesn't require a critical section protection, because
  * the only access to it in the application's context, is from 
  * hsu_al_ecm_ioctl(), which performs a simple read. Since this is a boolean, 
  * the read operation is "atomic". 
  */ 
  if (context_ptr->instance_is_enabled)
  {
    HSU_MSG_ERROR_1(
      "hsu_al_ecm_enabled_cb(): the instance is already enabled (handle=%d)",
      context_ptr->handle);
    return HSU_JSUCCESS; 
  }

  HSU_ULOG_1(NO_MSG, BUS_LOG, "context_ptr 0x%08X", (uint32)context_ptr);

  /* Set the FD handle to the current configuration's FD */
  hsu_al_ecm_handle_set_configuration(context_ptr);

  cdc_ctx_ptr = (cdc_ctx_t*)(context_ptr->handle); /* context_ptr already sanitized */
  HSU_ASSERT(cdc_ctx_ptr);

  /* use "real" ECM interface interface number and not the endpoint sharing interrupt number */
  if ((cdc_ctx_ptr->desc) &&
     (cdc_ctx_ptr->desc->interfaces) &&
     (cdc_ctx_ptr->intr_pipe.desc) &&
     (cdc_ctx_ptr->out_pipe.desc) &&
     (cdc_ctx_ptr->in_pipe.desc))
  {
    hsu_ulog_type_init((hsu_ulog_log_type_enum)cdc_ctx_ptr->desc->interfaces->number);
    HSU_ULOG_3(NO_MSG, (hsu_ulog_log_type_enum)cdc_ctx_ptr->desc->interfaces->number,
                "intr_addr 0x%x, out_addr 0x%X, in_addr 0x%X", 
                               cdc_ctx_ptr->intr_pipe.desc->address,
                               cdc_ctx_ptr->out_pipe.desc->address,
                               cdc_ctx_ptr->in_pipe.desc->address);
  }

 // if (cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS)
 // {
 //   hsu_al_uncached_dsm_init();
 //   hsu_al_dsm_utils_init(HSU_ECM_AL_DSM_POOL_ID);
 // }

  context_ptr->instance_is_enabled = TRUE;
  context_ptr->ctrl_line_state = FALSE;
  hsu_al_ecm_suspend_resume_mode = HSU_BUS_MODE_RESUMED;

#ifdef FEATURE_HS_USB_STANDARD_ECM
  /* Check if there is connection speed notification that needs to be resent. */
  if (context_ptr->data_info.conn_speed_mem.resend == TRUE)
  {
    context_ptr->data_info.conn_speed_mem.resend = FALSE;
    context_ptr->data_info.connection_speed_args = 
      context_ptr->data_info.conn_speed_mem.connection_speed_args;
    defer_call_to_notif_con_speed_ch((void*)context_ptr);
  }
  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) != 0)
  {
    context_ptr->ctrl_line_state = TRUE;
	context_ptr->data_info.rx_info.inbound_flow_is_enabled = TRUE;
  }
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

  if (cdc_ctx_ptr->info.basic.flags & (CDC_INIT_STD_ECM | CDC_INIT_INTR_ONLY_ECM_PIPE | CDC_INIT_DPL))
  {
    invoke_app_enable_disable_cbs(context_ptr);
  }

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  if((context_ptr->ignore_disconnect == TRUE) && 
     (context_ptr->is_core_enabled == FALSE))
  {
    /* resending dropped packet */
    hsu_al_ecm_invoke_resending_droped_packet(context_ptr);
    
    /* update core state */
    context_ptr->is_core_enabled = TRUE;
  }

  if (context_ptr->data_info.rx_info.inbound_flow_is_enabled)
  {
    /* Setup a new read, but only if open already issued (or if interface 
     * is using read) and if the inbound flow is enabled:*/
    hsu_al_ecm_setup_read(context_ptr);
  }
  
  /* Trigger sending control packet that was recieved in the middle of 
  ** enumeration */
  else if (context_ptr->state == HSU_ECM_INSTANCE_STATE_OPENED)
  {   
    hsu_al_task_enqueue_cmd(hsu_al_ecm_transmit_pending_control_pkt, context_ptr);
  }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  
  return HSU_JSUCCESS;
} /*hsu_al_ecm_enabled_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_DISABLED_CB

DESCRIPTION   
Called by the FD when the stack identifies that the device has been detached 
from the host. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_enabled_cb() must have been called for this 
instance.

RETURN VALUE  
jresult_t, currently only HSU_JSUCCESS is possible (failures cause an ASSERT).

SIDE EFFECTS  
All pending read and writes on Bulk are aborted.

==============================================================================*/
static jresult_t hsu_al_ecm_disabled_cb
(
  cdc_appctx_t ctx
)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  ecm_data_tx_info_type* tx_info_ptr = NULL;

  //cdc_ctx_t *cdc_ctx_ptr = NULL;

  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_disabled_cb(): ctx (%u) param invalid", 
      ctx, 0, 0);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)ctx;
  tx_info_ptr = &context_ptr->data_info.tx_info;

  HSU_MSG_HIGH_1("hsu_al_ecm_disabled_cb: Enter (handle=%d)", context_ptr->handle);

  HSU_ULOG_1(HIGH_MSG, BAM_LOG, "context_ptr 0x%08X", (uint32)context_ptr);

  //cdc_ctx_ptr = (cdc_ctx_t*)(context_ptr->handle);

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  if(context_ptr->ignore_disconnect == TRUE)
  {
    HSU_MSG_HIGH_1("hsu_al_ecm_disabled_cb:ignore_disconnect enabled %d", context_ptr);
    
    context_ptr->instance_is_enabled = FALSE;
    
    /* disable the core */
    context_ptr->is_core_enabled = FALSE;

    #ifdef FEATURE_HS_USB_ECM_BAM
    #ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
    hsu_al_ecm_disable_bam(context_ptr, HSU_AL_BAM_UL, HSU_AL_BAM_DL);
    #endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
    #endif /* FEATURE_HS_USB_ECM_BAM */

    return HSU_JSUCCESS;
  }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */


#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  if(context_ptr->control_info.pending_dsm_ptr)
  {
    dsm_free_packet(&context_ptr->control_info.pending_dsm_ptr);
    context_ptr->control_info.pending_dsm_ptr = NULL;
  }

  context_ptr->control_info.need_resending_pending_dsm = FALSE;  
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  if(context_ptr->instance_is_enabled == FALSE)
  {
    /* This CB should be called only for REAL state changes */
    HSU_MSG_ERROR_1(
      "hsu_al_ecm_disabled_cb(): the instance is already disabled (handle=%d)",
      context_ptr->handle);
    return HSU_JSUCCESS;
  }

  hsu_al_dsm_utils_uninit(HSU_ECM_AL_DSM_POOL_ID);

#ifdef FEATURE_HS_USB_ECM_BAM
#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
  hsu_al_ecm_disable_bam(context_ptr, HSU_AL_BAM_UL, HSU_AL_BAM_DL);
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
#endif /* FEATURE_HS_USB_ECM_BAM */


  /* TX control cleanup */
  if (context_ptr->control_info.tx_queue_ptr != NULL)
  {
    dsm_empty_queue(context_ptr->control_info.tx_queue_ptr);
  }

  /* Clear the Data channel request handles */
  rb_queue_free(context_ptr->data_info.rx_info.rx_chain, FALSE);
  rb_queue_free(context_ptr->data_info.tx_info.tx_chain, FALSE);

  /*Cancel the timer, if active*/
  if (tx_info_ptr->tx_handle_comp_scheduled == TRUE)
  {
	hsu_timer_cancel(&tx_info_ptr->tx_handle_complete_timer);
  }

  context_ptr->instance_is_enabled = FALSE;
  context_ptr->ctrl_line_state = FALSE;

  invoke_app_enable_disable_cbs(context_ptr);

  HSU_MSG_HIGH("hsu_al_ecm_disabled_cb: Exit");

  return HSU_JSUCCESS;
} /*hsu_al_ecm_disabled_cb*/

#ifdef FEATURE_HS_USB_MBIM
/*==============================================================================

FUNCTION      HSU_AL_ECM_ENCAP_COMMAND_LAST_PRE_OPEN

DESCRIPTION   
  Called by either 
     hsu_al_ecm_encap_command_cb() - to save control msg due to no
        control open [yet]   
  OR 
    hsu_al_ecm_control_open() - Enqueue to RX last control msg saved
       before the control open was executed
   
  This is to solve the dropping of the first MBIM_OPEN msg from the host 
  which results in extra 5 seconds before the next MBIM_OPEN. This 5 
  second delay has an impact on meeting startup requirements (<10 seconds). 

DEPENDENCIES  

RETURN VALUE  

SIDE EFFECTS  
None.
==============================================================================*/
static void hsu_al_ecm_encap_command_last_pre_open
(
  hsu_al_ecm_instance_info_type *  context_ptr,
  dsm_item_type                 *  dsm_item_ptr
)
{
  static dsm_item_type * dsm_item_last_ptr = NULL;

  if (dsm_item_ptr == NULL) 
  {
    if (dsm_item_last_ptr != NULL)
    {
       /* Forward last command to control RX */
       HSU_MSG_MED_2(
         "hsu_al_ecm_encap_command_last_pre_open(): enqueuing dsm_item=0x%x to CTRL RX WMQ (handle=%d)",
         dsm_item_last_ptr, context_ptr->handle);

       if (context_ptr->control_info.rx_func_ptr)
       {
         context_ptr->control_info.rx_func_ptr(&dsm_item_last_ptr);
       }
       else if (context_ptr->control_info.rx_queue_ptr)
       {
         dsm_enqueue(context_ptr->control_info.rx_queue_ptr, &dsm_item_last_ptr);
       }
       else
       {
         HSU_ERR_FATAL("hsu_al_ecm_encap_command_last_pre_open(): Neither RX "
           "WM nor rx_func_ptr is defined", 0, 0, 0);
         dsm_free_packet(&dsm_item_last_ptr);
       }

       dsm_item_last_ptr = NULL;;
    }
    return;
  }

  /* Save current dsm_item */
  if (dsm_item_last_ptr != NULL)
  {
    dsm_free_packet(&dsm_item_last_ptr);
  }
  dsm_item_last_ptr =  dsm_item_ptr;
}
#endif
/*==============================================================================

FUNCTION      HSU_AL_ECM_ENCAP_COMMAND_CB

DESCRIPTION   
Called by the FD when a "Send Encapsulated Command" request has arrived from 
the host. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
jresult_t: HSU_JSUCCESS for success, currently all failures cause an ASSERT
(When the temporary ASSERT is removed, the value JENOMEM will be returned for 
DSM allocation failure, and JEINVAL for scattering failure)

SIDE EFFECTS  
None.
==============================================================================*/
static jresult_t hsu_al_ecm_encap_command_cb
(
 void *        command_ptr, 
 juint16_t     size,
 cdc_appctx_t  ctx
 )
{
  dsm_item_type * dsm_chain_head_ptr;
  boolean bool_result;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_ASSERT(command_ptr != NULL);
  HSU_ASSERT(size != 0);
  HSU_PARAM_COULD_BE_CONST(command_ptr);

  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_encap_command_cb(): ctx param invalid",
      ctx, 0, 0);
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)ctx;

  HSU_MSG_HIGH_2("hsu_al_ecm_encap_command_cb(): called with size=%d (handle=%d)",
    size, context_ptr->handle);
#ifdef FEATURE_HS_USB_MBIM
  if (!(((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_MBIM_ECM ))
#endif
   {
    if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
    {
    /* This is possible and legitimate if the ECM is part of the composition, 
    * but the application isn't using the USB link, and hasn't called sio_open(s) 
    */
    HSU_MSG_HIGH_1(
      "hsu_al_ecm_encap_command_cb(): Instance is not opened, ignoring CTRL msg (handle=%d)",
      context_ptr->handle);
    return HSU_JSUCCESS;
    }
   }

  if(context_ptr->instance_is_enabled == FALSE)
  {
    HSU_MSG_ERROR_1(
      "hsu_al_ecm_encap_command_cb(): instance is disabled! (handle=%d)",
      context_ptr->handle);

#ifndef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
    /* This is corner case with ENCAP CMD pending, but loses enumerate at 
    ** same time. In this case, drop the packet 
    */
    return HSU_JSUCCESS;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  }

  /* Scatter buffer into DSM items */
  dsm_chain_head_ptr = 
    hsu_al_dsm_utils_generate_dsm_chain(HSU_ECM_AL_CONTROL_DSM_POOL_ID, size);
  if(dsm_chain_head_ptr == NULL)
  {
    HSU_MSG_ERROR("hsu_al_ecm_encap_command_cb - large DSM items allocation failed. "  
                  "Falling back to small DSM items");
    dsm_chain_head_ptr = 
        hsu_al_dsm_utils_generate_dsm_chain(DSM_DS_SMALL_ITEM_POOL, size);
    if(dsm_chain_head_ptr == NULL)
    {
      HSU_MSG_ERROR("hsu_al_ecm_encap_command_cb(): failed allocating "
                    "both large and small DSM items. Dropping packet");
      context_ptr->statistics.mem_allocation_fails++;
      return JENOMEM;
    }
  }

#ifdef FEATURE_HSU_ECM_APP_STUB
  hsu_al_ecm_parse_qmi_msg(
    (byte *)command_ptr, 
    size);
#endif /* defined(FEATURE_HSU_ECM_APP_STUB) */

  /* For control channel messages, the DMOV is not used. */
  bool_result = hsu_al_dsm_utils_scatter_buffer_without_dmov(command_ptr, 
    size, dsm_chain_head_ptr);
  if(bool_result == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ecm_encap_command_cb(): "
      "hsu_al_dsm_utils_scatter_buffer_without_dmov() failed");
    context_ptr->statistics.tx_dropped_packet_cnt++;
    return JEINVAL;
  }
#ifdef FEATURE_HS_USB_MBIM
  if (((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_MBIM_ECM )
  {  
   if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
   {
    /* This is possible and legitimate if the ECM is part of the composition, 
    * but the application isn't using the USB link, and hasn't called sio_open(s) 
    */
    HSU_MSG_HIGH_1(
      "hsu_al_ecm_encap_command_cb(): Instance is not opened, saving CTRL msg (handle=%d)",
      context_ptr->handle);
    hsu_al_ecm_encap_command_last_pre_open(context_ptr, dsm_chain_head_ptr);
    return HSU_JSUCCESS;
   }
  }
#endif
  /* Enqueue DSM chain to Control RX WMQ */
  HSU_ULOG_2(MED_MSG, (hsu_ulog_log_type_enum)((cdc_ctx_t*)(context_ptr->handle))->desc->interfaces->number,
     "enqueu_dsm_chain_to_CTRL_RX_WRMQ, dsm_chain 0x%08X, handle 0x%08X", (uint32) dsm_chain_head_ptr, (uint32) context_ptr->handle);
  HSU_ULOG_DATA_5((hsu_ulog_log_type_enum)((cdc_ctx_t*)(context_ptr->handle))->desc->interfaces->number, (void*)dsm_chain_head_ptr->data_ptr, (uint8)dsm_chain_head_ptr->used);

  if ((context_ptr->control_info.rx_func_ptr)
   || (context_ptr->control_info.rx_queue_ptr))
  {
    /* Defer the handling to HSU AL TASK without Giant mutex locked 
    ** It will use DSM item's app_ptr to pass hsu_al_ecm_instance_info_type
    */
    dsm_chain_head_ptr->app_ptr = context_ptr;
    hsu_al_task_forward_ecm_control_rx_dsm(dsm_chain_head_ptr);
  }
  else
  {
    HSU_ERR_FATAL("hsu_al_ecm_encap_command_cb(): Neither RX WM nor "
      "rx_func_ptr is defined", 0, 0, 0);
  }

  return HSU_JSUCCESS;
} /*hsu_al_ecm_encap_command_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_SET_PKT_FILTER_CB

DESCRIPTION   
Called by the FD when a "Set Ethernet Packet Filter" request has arrived from 
the host. 
This is a mandatory request, so a CB must be supplied.
In non-standard ECM, this request is not handled by the AMSS.

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS in case of a standard ECM or JEOPNOTSUPP - operation not supported 
for non standard ECM 

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_set_pkt_filter_cb
(
 juint16_t     pf_bitmask,
 cdc_appctx_t  ctx
 )
{
#ifndef FEATURE_HS_USB_STANDARD_ECM
  HSU_MSG_ERROR("hsu_al_ecm_set_pkt_filter_cb(): not supported");
  return JEOPNOTSUPP;
#else /*FEATURE_HS_USB_STANDARD_ECM*/
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ecm_set_pkt_filter_cb(): ctx param invalid");
    return JEINVAL;
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)(ctx);

  HSU_MSG_LOW_1("hsu_al_ecm_set_pkt_filter_cb(): pf_bitmask = 0x%04x", pf_bitmask);

  if(!(((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM ))
  {
    HSU_MSG_ERROR("hsu_al_ecm_set_pkt_filter_cb(): not supported");
    return JEOPNOTSUPP;
  }

  if (context_ptr->app_callbacks.set_packet_filter_cb != NULL)
  {
    context_ptr->app_callbacks.set_packet_filter_cb(
      pf_bitmask, context_ptr->app_callbacks.set_packet_filter_user_data);
  }
  else
  {
    HSU_MSG_HIGH("set_packet_filter_cb is NULL");
  }
  return HSU_JSUCCESS;
#endif /*FEATURE_HS_USB_STANDARD_ECM*/
} /*hsu_al_ecm_set_pkt_filter_cb*/

/*==============================================================================

FUNCTION      PERFORM_CONTROL_TRANSMIT

DESCRIPTION   
Called by either hsu_al_ecm_control_transmit() (application context) or by
hsu_al_ecm_encap_resp_comp_cb() (FD context), when it is allowed to  
send a DSM chain to the stack for transmission. 
When the function is called, the transmit buffer is already marked as full 
(the field ctrl_tx_num_of_bytes != 0), to ensure that further calls by the  
application to hsu_al_ecm_control_transmit() will cause the DSM chain to be 
enqueued to the control TX WM Q.

the function gathers the DSM chain into a preallocated buffer, and gives it to
the cdc_send_encap_response() function of the USB stack. 

The stack is responsible to generate a "response available" notification, and 
then send the message after the host sends an IN token on the control channel
following the notification. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.
the control_info.write_buffer_data.ctrl_tx_num_of_bytes field must have 
been set to the DSM chain's length prior to the call to this function.
Should be called with Giant mutex locked

RETURN VALUE  
None.

SIDE EFFECTS  
None. 

==============================================================================*/
static void perform_control_transmit
( 
 hsu_al_ecm_instance_info_type *  context_ptr, 
 dsm_item_type *                  tx_ptr
 )
{
  jresult_t result;
  uint32 tx_ptr_chain_length;

#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
  juint32_t *seq_num_addr;
#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  core_t *core;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  HSU_ASSERT(context_ptr != NULL);
  HSU_ASSERT(tx_ptr != NULL);

  tx_ptr_chain_length = dsm_length_packet(tx_ptr);

  /* Gather the DSM chain into a contiguous buffer */
  /************************************************/
  /* 
  * this is done using memscpy operations, and not by using the DMOV, since 
  * control messages are rather small, and it's probably not worth using the 
  * DMOV 
  */
  if(hsu_al_dsm_utils_gather_buffer_without_dmov(
    tx_ptr, /* chain_to_gather */
    tx_ptr_chain_length,
    #ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
    /* First 4-bytes are reserved for sequence number */
    context_ptr->control_info.write_buffer_data.ctrl_tx_buffer 
      + sizeof(juint32_t),
    #else /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
    context_ptr->control_info.write_buffer_data.ctrl_tx_buffer,
    #endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
    HSU_ENCAPS_CMD_MAX_SIZE) 
    == 0)
  {
    HSU_MSG_ERROR("perform_control_transmit(): "
      "hsu_al_dsm_utils_gather_buffer_without_dmov failed");
    HSU_MSG_ERROR_2("  dsm_length_packet=%d, buf size=%d", tx_ptr_chain_length,
      HSU_ENCAPS_CMD_MAX_SIZE);

    dsm_free_packet(&tx_ptr);
    context_ptr->statistics.tx_dropped_packet_cnt++;
    return;
  }

#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
  /* Assign sequence number to first 4-bytes */
  seq_num_addr = (juint32_t *)
    (context_ptr->control_info.write_buffer_data.ctrl_tx_buffer);

  *seq_num_addr = tx_ptr->app_field;
#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  context_ptr->control_info.pending_dsm_ptr = tx_ptr;
#else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  /* Now that the message is in the buffer, no need for the DSM chain... */
  dsm_free_packet(&tx_ptr); 
#endif /* !FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
#ifdef FEATURE_HS_USB_MBIM
  if (!hsu_conf_sel_is_diag_over_mbim())
#endif /* FEATURE_HS_USB_MBIM */
  {
    HSU_MSG_MED_3(
      "perform_control_transmit(): calling cdc_send_encap_response, len=%d, tx_ptr=%d (handle=%d)",
      context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes, 
      tx_ptr,
      context_ptr->handle);
  }

#if defined(FEATURE_HSU_ECM_APP_STUB)
  hsu_al_ecm_parse_qmi_msg(
    context_ptr->control_info.write_buffer_data.ctrl_tx_buffer, 
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes);
#endif /* defined(FEATURE_HSU_ECM_APP_STUB) */

  /* Send the buffer to the stack, to be sent on the control EP, to the host */
  /***************************************************************************/
  if(context_ptr->instance_is_enabled == FALSE)
  {
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
    context_ptr->control_info.need_resending_pending_dsm = TRUE;
    context_ptr->statistics.tx_control_dropped_packet_empty_queue_cnt++;
#else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
    dsm_empty_queue(context_ptr->control_info.tx_queue_ptr);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

    HSU_MSG_HIGH_1(
      "perform_control_transmit(): Instance is disabled (handle=%d), Ignoring.",
      context_ptr->handle);
    return;
  }

  result = hsu_cdc_fd_api_send_encap_response(context_ptr->handle, 
    (void *)context_ptr->control_info.write_buffer_data.ctrl_tx_buffer,
    (juint32_t)context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes);

  if(result == ESUSPENDED)
  {
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
    /* Anything other than HSIC, we should not recover drop packet if 
    ** REMOTE WAKEUP FEATURE is not enabled from host
    */
    core = ((cdc_ctx_t*)context_ptr->handle)->core_ctx;
    HSU_ASSERT(core);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    {
      /* 
       * When Remote Wakeup is enabled, ESUSPENDED will be returned when 
       * the Remote Wakeup failed. control TX DSM qeueue will be cleared out 
       * and the call will return without trying to send remaining data on the 
       * TX queue. 
       *
       * The call to this must be performed when the giant mutex is locked, as
       * would happen if the FD was the one calling this function. 
       */

      context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
      dsm_empty_queue(context_ptr->control_info.tx_queue_ptr);

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      /* Need to clean up additional structure */
      dsm_free_packet(&context_ptr->control_info.pending_dsm_ptr); 
      context_ptr->control_info.pending_dsm_ptr = NULL;
      context_ptr->control_info.need_resending_pending_dsm = FALSE;

      HSU_MSG_ERROR_1("perform_control_transmit(): hsu_cdc_fd_api_send_encap_response "
        "returned ESUSPENDED. cleared all TX (handle=%d)",
        context_ptr->handle);

      context_ptr->statistics.tx_control_dropped_packet_esuspended_cnt++;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    }

    HSU_MSG_HIGH_1("perform_control_transmit(): hsu_cdc_fd_api_send_encap_response "
      "returned ESUSPENDED. Cancelled current TX (handle=%d)",
      context_ptr->handle);
    return;
  }

  if(result == JEBUSY)
  {
    HSU_ERR_FATAL("perform_control_transmit(): Ctl Tx already in progress, "
      "  write_buffer_data.ctrl_tx_num_of_bytes=%d, tx_ptr=0x%x",
      context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes, 
      tx_ptr, 0);
  }
  else if(result != HSU_JSUCCESS)
  {
    HSU_ERR_FATAL(
      "perform_control_transmit(): cdc_send_encap_response failed with %d",
      result, 0, 0);
  }
  /* Note: if one of the asserts is replaced with error handling, consider 
  adding the following section:     
  context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
  to the callers' functions. 
  If we were to do this in this function, and there would be another message in 
  the WM Q, the app. context might take over and send ANOTHER control message, 
  which will go to the host BEFORE the one already queued. 
  */
} /* perform_control_transmit */

/*==============================================================================

FUNCTION      HSU_AL_ECM_ENCAP_RESP_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a request made through the 
function cdc_send_encap_response().
The sequence completed includes the "Response Available" notification and the 
"Get Encapsulated command" request. 

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_encap_resp_comp_cb
(
 cdc_iostatus_t  status, 
 juint32_t       bytes_sent, 
 cdc_appctx_t    ctx
 )
{
  dsm_item_type * dsm_chain_ptr;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_encap_resp_comp_cb(): ctx (%u) param invalid",
      ctx, 0, 0);
  }
  context_ptr = (hsu_al_ecm_instance_info_type *)ctx;

  switch(status)
  {
  case CDC_TRANSFER_ERROR:
    {
      /* We assume that a failure at this stage (asynchronous) is due to a non-fatal 
      error, such as: temporarily out of resources (TD), bus error, etc...
      and therefore we assume it is OK to send the next control message */
      HSU_MSG_ERROR_1(
        "hsu_al_ecm_encap_resp_comp_cb(): status=CDC_TRANSFER_ERROR, bytes_sent=%d",
        bytes_sent);
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      context_ptr->statistics.tx_control_dropped_packet_error_cnt++;
      /* Fall through and handle ERROR same as CANCELLED */
#else
      break; /* code continues after the end of the switch */
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    }
  case CDC_TRANSFER_CANCELLED:
    {
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      /* Re-transmit the packet lost in suspend. If the event is not from suspend, 
       * , eg disconnect, the pending dsm items will be freeed in 
       * perform_control_transmit. 
       */
      context_ptr->control_info.need_resending_pending_dsm = TRUE;
      /* trigger resending control packet on the next execution to avoid cancellation upon entering suspend*/
      hsu_al_task_enqueue_cmd(hsu_al_ecm_transmit_pending_control_pkt, context_ptr);
      return;
#else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
      /* 
      * The device is suspended (for example - the USB cable is disconnected). 
      * We should keep TXing as long as we did not get a disabled_cb.
      */
      HSU_MSG_HIGH_2(
        "hsu_al_ecm_encap_resp_comp_cb(): status=CANCELLED, bytes_sent=%d (handle=%d)",
        bytes_sent, context_ptr->handle);
      break; /* code continues after the end of the switch */
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */  
    }
  case CDC_TRANSFER_OK:
    {
#ifdef FEATURE_HS_USB_MBIM      
      if (!hsu_conf_sel_is_diag_over_mbim())
#endif /* FEATURE_HS_USB_MBIM */        
      {
        HSU_MSG_HIGH_2("hsu_al_ecm_encap_resp_comp_cb():status=OK, bytes_sent=%d (handle=%d)",
          bytes_sent, context_ptr->handle);
      }
      HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)((cdc_ctx_t*)(context_ptr->handle))->desc->interfaces->number, "");
      break; /* code continues after the end of the switch */
    }
  default:
    {
      HSU_MSG_ERROR_3(
        "hsu_al_ecm_encap_resp_comp_cb(): unknown status=%d, bytes_sent=%d (handle=%d)",
        status, bytes_sent, context_ptr->handle);
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      context_ptr->statistics.tx_control_dropped_packet_error_cnt++;
      HSU_ERR_FATAL("hsu_al_ecm_encap_resp_comp_cb(): unknown status (%u)", 
        status, 0, 0);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    }
  } /* switch */

  /* no need to notify the application */

  /* 
  * the write_buffer_data structure is accessed both from the FD task's 
  * context and from the application context when hsu_al_ecm_control_transmit 
  * is called. Therefore it is protected by the Giant mutex. Note that we 
  * protect the ctrl_tx_num_of_bytes field, as a means to control access to the 
  * buffer itself as well. When the field is != 0, the structure cannot be 
  * changed in the application task's context (unless the transfer failed), but  
  * only in the FD task context, when the control transfer completes. 
  */

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  if(context_ptr->control_info.pending_dsm_ptr)
  {
    dsm_free_packet(&context_ptr->control_info.pending_dsm_ptr);
    context_ptr->control_info.pending_dsm_ptr = NULL;
  }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  /* Initiate the next control TX transmission if there is such pending */
  dsm_chain_ptr = dsm_dequeue(context_ptr->control_info.tx_queue_ptr);
  if(dsm_chain_ptr == NULL)
  {
    context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 0;
    return;
  }

  context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 
    dsm_length_packet(dsm_chain_ptr);
#ifdef FEATURE_HS_USB_MBIM
  if (!hsu_conf_sel_is_diag_over_mbim())
#endif /* FEATURE_HS_USB_MBIM */    
  {
    HSU_MSG_MED_2(
      "hsu_al_ecm_encap_resp_comp_cb(): another CTRL TX, dsm_chain_ptr=0x%x (handle=%d)",
      dsm_chain_ptr, context_ptr->handle);
  }
  perform_control_transmit(context_ptr, dsm_chain_ptr);
} /*hsu_al_ecm_encap_resp_comp_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_NOTIF_NET_CONN_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a 
cdc_notify_network_connection() (which is used to send the "Network 
Connection" notification. 
This is a mandatory notification, so a CB must be supplied, but this 
notification is not used by the AMSS.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_notif_net_conn_comp_cb
(
 cdc_iostatus_t  status, 
 cdc_appctx_t    ctx
 )
{
  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_notif_net_conn_comp_cb(): ctx (%u) param invalid",
      ctx, 0, 0);
  }

  HSU_USE_PARAM(status);
} /*hsu_al_ecm_notif_net_conn_comp_cb*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_NOTIF_CONN_SPEED_CH_COMP_CB

DESCRIPTION   
Called by the FD to notify of the completion of a 
ecm_notify_conn_speed_change() (which is used to send the "Connection Speed 
Change" notification). 

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_notif_conn_speed_ch_comp_cb
(
 cdc_iostatus_t  status, 
 cdc_appctx_t    ctx
 )
{
  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_notif_conn_speed_ch_comp_cb(): ctx (%u) param ",
      ctx, 0, 0);
  }

  if(status != CDC_TRANSFER_OK)
  {
    HSU_MSG_ERROR_1(
      "hsu_al_ecm_notif_conn_speed_ch_comp_cb(): failed with status=%d",
      status);
  }
  else
  {
    HSU_MSG_LOW("hsu_al_ecm_notif_conn_speed_ch_comp_cb(): CDC_TRANSFER_OK");
  }

  /* nothing else to do... */
} /*hsu_al_ecm_notif_conn_speed_ch_comp_cb*/


/*===========================================================================

FUNCTION hsu_al_ecm_forward_rx_to_app

DESCRIPTION
Function calls either the Rx callback or enqueue in WMQ for the DSM item received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS  
None

===========================================================================*/
void hsu_al_ecm_forward_rx_to_app(rb_t *rb)
{
  
  hsu_al_ecm_instance_info_type * context_ptr;
  bam_dmux_rx_ptr rx_func_ptr;
  bam_dmux_status tx_status;

#ifdef FEATURE_HS_USB_STANDARD_ECM
  hsu_al_ecm_usb_iNiche_rx_cb_type	   rx_func_ptr_std;
#endif

  HSU_ASSERT(rb);
      
  context_ptr = (hsu_al_ecm_instance_info_type *)rb->app_ptr;

  HSU_ASSERT(context_ptr);
  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
  {
    rx_func_ptr = context_ptr->data_info.rx_func_ptr;
	HSU_ASSERT(rx_func_ptr);
  }
  else
  {
    rx_func_ptr_std = context_ptr->data_info.rx_func_ptr_std;
	HSU_ASSERT(rx_func_ptr_std);
  }

  HSU_ULOG_1(NO_MSG, DATA_LOG, "rb 0x%X",(uint32)rb);
  
  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
  {
    if (rx_func_ptr &&
		context_ptr->data_info.dmux_handle &&
		context_ptr->instance_is_enabled)
    {
    tx_status = rx_func_ptr(context_ptr->data_info.dmux_handle, 
                            NULL, rb->buffer_va, rb->used);
    if(BAM_DMUX_STATUS_TX_ERROR == tx_status)
    {
      HSU_ULOG(NO_MSG, DATA_LOG, "Flow disabled ");

      jsafe_enter();
      
      context_ptr->data_info.rx_info.inbound_flow_is_enabled = FALSE;
      //Drop the packet
      free_single_rb(rb, FALSE);
      context_ptr->statistics.tx_dropped_packet_cnt++;

      jsafe_leave();
    }
    else if (BAM_DMUX_STATUS_SUCESS == tx_status)
    {
      context_ptr->data_info.rx_info.num_buf_already_forwarded++;
    }  
    else
    {
      HSU_ERR_FATAL("bam_dmux_tx failed with err=%d! buffer=0x%X, used=%d.", tx_status, rb->buffer_va, rb->used);
    }
  }
  else
  {
    //Drop the packet 
    HSU_ULOG(NO_MSG, DATA_LOG, " rx_func_ptr or dmux handle is NULL or instance is disabled ");
    hsu_al_ecm_rx_complete(NULL, context_ptr, NULL, rb->buffer_va, rb->used, (bam_dmux_status)0);
	}
  }
  else
  {
    if(rx_func_ptr_std &&
	   context_ptr->instance_is_enabled)
    {
      rx_func_ptr_std(rb->buffer_va, rb->used);
    }
	else
	{
	  //Drop the packet
      HSU_ULOG(NO_MSG, DATA_LOG, " rx_func_ptr_std is NULL or instance is disabled ");
      hsu_al_ecm_rx_complete(NULL, context_ptr, NULL, rb->buffer_va, rb->used, (bam_dmux_status)0);
	}
  }

} /* hsu_al_ecm_forward_rx_to_app */

/*===========================================================================

FUNCTION hsu_al_ecm_forward_control_rx_to_app

DESCRIPTION
Function calls either the control Rx callback or enqueue in WMQ for the DSM 
item received.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS  
None

===========================================================================*/
void hsu_al_ecm_forward_control_rx_to_app(dsm_item_type *dsm_ptr)
{
  /*
  ** It will use DSM item's app_ptr to pass hsu_al_ecm_instance_info_type
  */
  hsu_al_ecm_instance_info_type * context_ptr = (hsu_al_ecm_instance_info_type *)dsm_ptr->app_ptr;
  sio_rx_func_ptr_type rx_func_ptr = context_ptr->control_info.rx_func_ptr;
  dsm_watermark_type *rx_queue_ptr = context_ptr->control_info.rx_queue_ptr;
  
  HSU_ULOG(NO_MSG, (hsu_ulog_log_type_enum)((cdc_ctx_t*)(context_ptr->handle))->desc->interfaces->number, "");
  
  if (rx_func_ptr)
  {
    rx_func_ptr(&dsm_ptr);
  }
  else if(rx_queue_ptr)
  {
    dsm_enqueue(rx_queue_ptr, &dsm_ptr);
  }
  else
  {
    /* Dropping packet assuming that rx_func_ptr and rx_queue got cleared
     * due to port closing from the point HS-USB0 task enqeueuing
     * to HSU_AL_TASK invoking this function. */
    dsm_free_packet(&dsm_ptr);
  }
} /* hsu_al_ecm_forward_control_rx_to_app */

/*===========================================================================

FUNCTION HSU_AL_ECM_RX_DONE_FORWARD_TO_APP

DESCRIPTION   
forward the received data to the application by calling its rx callback 
or by enqueing the data into the RX Q
Should be called with mutex locked:
1. If from rx_done_cb then it's locked by HSUSB0 task
2. If from enable_inbound_flow then it's called by app

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_al_ecm_rx_done_forward_to_app
(
  hsu_al_ecm_instance_info_type *         context_ptr, 
  rb_t* rx_rb_chain_ptr
)
{
  rb_t *rb_for_app;
  //ecm_data_rx_info_type* rx_info_ptr;

  HSU_ASSERT(context_ptr != NULL);
  HSU_ASSERT(rx_rb_chain_ptr != NULL);
  
  //rx_info_ptr = &context_ptr->data_info.rx_info;

  rb_for_app = rx_rb_chain_ptr;

  while (rb_for_app)
  {
    if ((rb_for_app->used < ECM_ROUNDED_DSM_ITEM_SIZE)&(rb_for_app->done == TRUE))
    {
      if (rb_for_app->used == 0)
      {
        HSU_MSG_HIGH_3("discarding DSM item with zero length",0,0,0);
        HSU_ULOG_1(NO_MSG, DATA_LOG, "zero length rb 0x%X", (uint32)rb_for_app);
        rb_for_app->done = FALSE;
        free_single_rb(rb_for_app, FALSE);
      }
      else if ((context_ptr->data_info.rx_func_ptr != NULL) ||
	  	       (context_ptr->data_info.rx_func_ptr_std != NULL))
      {
        /* The RX callback \ dsm_enqueue might try to lock a mutex so call it from AL task 
                    context, where we don't hold the giant-mutex, to avoid possible deadlock. */
        rb_for_app->done = FALSE;
        rb_for_app->app_ptr = context_ptr;
        HSU_ULOG_1(NO_MSG, DATA_LOG, "forward rb 0x%X ",(uint32)rb_for_app);
        hsu_al_task_forward_rx_dsm(rb_for_app);
      }
      else
      {
        HSU_ULOG_1(NO_MSG, DATA_LOG, "No Rx callback AND no Rx Q. Packet dropped rb 0x%X ",(uint32)rb_for_app);
        HSU_MSG_ERROR_3("No Rx callback AND no Rx WMQ. Packet dropped",0,0,0);
        /* dsm_free_packet might try to lock a mutex so call it from AL task 
           context, where we don't hold the giant-mutex, to avoid possible deadlock. */
        rb_for_app->done = FALSE;
        free_single_rb(rb_for_app, FALSE);
        //rx_info_ptr->rx_chain_ptr = NULL;
        return;
      }

      rb_for_app = rb_for_app->next;
    }
    else
    {
      rb_for_app = rb_for_app->next;
    }
  }
} /* hsu_al_ecm_rx_done_forward_to_app */

/*===========================================================================

FUNCTION HSU_AL_ECM_RX_DONE_WITH_FLOW_DISABLED

DESCRIPTION
Called by rx_done to handle the case of inbound flow disabled.

Inbound flow may be disabled due to one of the following:
1. Dmux not enabled the flow yet.
2. Dmux has run into flow cotrol.

In either case, we'll drop the packet

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS  
None

===========================================================================*/
static void hsu_al_ecm_rx_done_with_flow_disabled
(
  hsu_al_ecm_instance_info_type* context_ptr,
  const cdc_iostatus_t *status,        /* array of statuses for each transfer */
  const juint32_t *bytes_transferred,  /* array of sizes for each buffer */
  juint32_t req_num                    /* number of request in current callback (if 
                                       timeout occurred it is not the same as the
                                       number passed to the cdc_read_multiple) */
)
{
  uint32 i;
  ecm_data_rx_info_type* rx_info_ptr = &context_ptr->data_info.rx_info;
  rb_t* cur_rb_ptr = rx_info_ptr->rx_chain_ptr;

  HSU_ULOG_1(NO_MSG, DATA_LOG, "rb 0x%X",(uint32)cur_rb_ptr);

  for (i=0; (i<req_num && cur_rb_ptr!= NULL); ++i,cur_rb_ptr = cur_rb_ptr->next)
  {
    free_single_rb(cur_rb_ptr, FALSE);
    context_ptr->statistics.tx_dropped_packet_cnt++;
  }

  rx_info_ptr->rx_chain_ptr = cur_rb_ptr;
} /* hsu_al_ecm_rx_done_with_flow_disabled */



/*===========================================================================

FUNCTION HSU_AL_ECM_RX_DONE_INTERNAL

DESCRIPTION   
Rx Callback whenever the ECM FD wishes to notify some buffers has been received
over the data class I/F.

DEPENDENCIES  
This function assume Giant Mutex is locked

RETURN VALUE    
None

SIDE EFFECTS  
None

===========================================================================*/
static void hsu_al_ecm_rx_done_internal
(
  cdc_iostatus_t *status,        /* array of statuses for each transfer */
  void **buffers,                /* array of buffers with data */
  juint32_t *bytes_transferred,  /* array of sizes for each buffer */
  juint32_t req_num,             /* number of request in current callback (if 
                                 timeout occurred it is not the same as the
                                 number passed to the cdc_read_multiple) */
  cdc_appctx_t ctx,
  boolean due_to_flow_enabled    /* simulated rx_done due to inbound flow enabled */
)
{
  hsu_al_ecm_instance_info_type* context_ptr = (hsu_al_ecm_instance_info_type *)ctx;
  ecm_data_rx_info_type* rx_info_ptr = &context_ptr->data_info.rx_info;
  uint32 i, cur_idx;
  rb_t* cur_rb_ptr = rx_info_ptr->rx_chain_ptr;

//  cdc_ctx_t *cdc_ctx_ptr;
//  cdc_ctx_ptr = (cdc_ctx_t*)context_ptr->handle;

  HSU_USE_PARAM(buffers);
  HSU_ASSERT(req_num>0);

  hsu_perf_log(PERF(HSU_AL_ECM_RX_DONE_INTERNAL, Entry), 0);

  if (context_ptr->instance_is_enabled == FALSE)
  {
    /* rx_done is sometimes called after hsu_al_ecm_disabled_cb() 
       (as part of pipe aborting, with status CDC_TRANSFER_CANCELLED) */
    HSU_MSG_MED_3(
      "hsu_al_ecm_rx_done_internal(): instance is disabled", 0, 0, 0);
    return;
  }

  /* mutual exclusion is achieved through giant mutex */
  if (rx_info_ptr->inbound_flow_is_enabled == FALSE)
  {
    HSU_ULOG_1(NO_MSG, DATA_LOG, "flow disbaled, req_num %u", req_num);

    hsu_al_ecm_rx_done_with_flow_disabled(context_ptr, 
      status, bytes_transferred, req_num);
    
    return;
  }

  if (context_ptr->debug_mode)
  {
    HSU_MSG_MED_3("rx_done: req_num=%d", req_num, 0, 0);
  }

  HSU_ULOG_1(NO_MSG, DATA_LOG, "req_num %u", req_num);

  /* iterate through all items in the received DSM chain */
  for (i=0, cur_idx=rx_info_ptr->num_buf_already_forwarded;
       (i<req_num && (cur_rb_ptr != NULL));
       ++i, ++cur_idx, cur_rb_ptr = cur_rb_ptr->next)
  {
    switch(status[i])
    {
    case CDC_TRANSFER_OK:
      {
        if (context_ptr->debug_mode)
        {
          HSU_MSG_MED_3("CDC_TRANSFER_OK. length=%d", bytes_transferred[i], 0, 0);
        }

        HSU_ULOG_2(LOW_MSG, DATA_LOG, "rb 0x%X, len %u",(uint32)cur_rb_ptr, (uint16)bytes_transferred[i]);

        cur_rb_ptr->used = (uint16)bytes_transferred[i];
        cur_rb_ptr->done = TRUE;

        if (bytes_transferred[i] != 0)
        {
          /* Invalidate cache before we further process the buffer to make sure
             whoever reads data gets it from the system memory instead of cache.
          */
          // Cache invalidation is not needed as we are using un-cached DSM items.
          //hsu_dcache_inval_region(cur_rb_ptr->data_ptr, ECM_ROUNDED_DSM_ITEM_SIZE);
        
          context_ptr->statistics.rx_total_bytes += bytes_transferred[i];
        }
        else
        {
          /* ZLP mark the end of Ethernet packet which is multiple of 
             max USB packet size */
          context_ptr->statistics.rx_zlp_count++;
        }

        break;
      }
    case CDC_TRANSFER_ERROR:
      {
        HSU_MSG_ERROR_3("_ecm_rx_done_internal: CDC_TRANSFER_ERROR", 0, 0, 0);
        HSU_ULOG_1(NO_MSG, DATA_LOG, " CDC_TRANSFER_ERROR rb 0x%X",(uint32)cur_rb_ptr);

        cur_rb_ptr->used = 0;
        rb_queue_free(cur_rb_ptr, FALSE);
        
        context_ptr->statistics.rx_transfer_error_cnt++;
        
        break;
      }
    case CDC_TRANSFER_CANCELLED:
      {
        HSU_MSG_HIGH_3("_ecm_rx_done_internal: CDC_TRANSFER_CANCELLED", 0, 0, 0);
        HSU_ULOG_1(NO_MSG, DATA_LOG, " CDC_TRANSFER_CANCELLED rb 0x%X",(uint32)cur_rb_ptr);
        /* assuming all remaining items are CANCELLED */
        free_single_rb(cur_rb_ptr, FALSE);
        
        //TODO: need to verify this
        if (cur_rb_ptr != NULL)
        {
          /* all items before this canceled one are OK */
          hsu_al_ecm_rx_done_forward_to_app(context_ptr, rx_info_ptr->rx_chain_ptr);
        }
        rx_info_ptr->num_buf_already_forwarded = 0;

        if (due_to_flow_enabled)
        {
          hsu_al_ecm_setup_read(context_ptr);
        }
        /* else this is due to suspend - setup_read will happen upon resume */

        if (cur_rb_ptr->next == NULL)
          return;

        break;
      }

    default:
      HSU_MSG_ERROR_3("_ecm_rx_done_internal: unexpected status %d, i=%d, cur_idx=%d", status[i], i, cur_idx);
      break;
    }

  }

  /* need to forward rx_info_ptr->dsm_chain_ptr[0 - (req_num-1)] to app */

  if (cur_rb_ptr == NULL)
  {
    HSU_ULOG_1(NO_MSG, DATA_LOG, "cur rb is NULL, req_num %u", req_num);
    
    /* all buffers are exhausted */
    hsu_al_ecm_rx_done_forward_to_app(context_ptr, rx_info_ptr->rx_chain_ptr);
    rx_info_ptr->num_buf_already_forwarded = 0;

    hsu_al_ecm_setup_read(context_ptr);
  }
  else
  {
    hsu_al_ecm_rx_done_forward_to_app(context_ptr, rx_info_ptr->rx_chain_ptr);
    rx_info_ptr->rx_chain_ptr = cur_rb_ptr;
    rx_info_ptr->num_buf_already_forwarded += req_num;
    context_ptr->statistics.rx_timeout_cnt++;
  }
} /* hsu_al_ecm_rx_done_internal */

/*===========================================================================

FUNCTION HSU_AL_ECM_RX_COMPLETE

DESCRIPTION
Rx Callback whenever the Dmux wishes to notify some buffers has been received
over the data class I/F.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ecm_rx_complete
(
  bam_dmux_handle handle,
  void            *priv,
  void            *tx_priv,
  void            *data,
  uint16          data_len,
  bam_dmux_status status
)
{
  hsu_al_ecm_instance_info_type* context_ptr = (hsu_al_ecm_instance_info_type *)priv;
  ecm_data_rx_info_type* rx_info_ptr = &context_ptr->data_info.rx_info;
  rb_t  *cur_rb_ptr = NULL;

  jsafe_enter();
  
  cur_rb_ptr = find_rb_from_buffer(data, rx_info_ptr->rx_chain);
  free_single_rb(cur_rb_ptr, FALSE);

  /* We have finished all the pending requests, so try queing for the next iteration */
  if((cur_rb_ptr != NULL) && (cur_rb_ptr->next == NULL))
  {
    hsu_al_ecm_setup_read(context_ptr);
  }

  jsafe_leave();
 
}


/*===========================================================================

FUNCTION HSU_AL_ECM_RX_DONE

DESCRIPTION
Rx Callback whenever the ECM FD wishes to notify some buffers has been received
over the data class I/F.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_ecm_rx_done
(
  cdc_iostatus_t *status,        /* array of statuses for each transfer */
  void **buffers,                /* array of buffers with data */
  juint32_t *bytes_transferred,  /* array of sizes for each buffer */
  juint32_t req_num,             /* number of request in current callback (if 
                                 timeout occurred it is not the same as the
                                 number passed to the cdc_read_multiple) */
  cdc_appctx_t ctx
)
{
  hsu_al_ecm_rx_done_internal(status, buffers, bytes_transferred, req_num, ctx, FALSE);
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_TX_DONE

DESCRIPTION   
Called by the FD to notify of the completion of write operations. 
The function frees any buffers (DSM item or contiguous_buf) associated with 
the completed write operations.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_tx_done
(
  cdc_iostatus_t *status,
  void **buffers, 
  juint32_t *buf_sizes,
  juint32_t req_num,
  cdc_appctx_t ctx
)
{
  hsu_al_ecm_instance_info_type *context_ptr = (hsu_al_ecm_instance_info_type *)ctx;
  rb_t *cur_rb_ptr = NULL;
  uint32 i;
  ecm_data_tx_info_type* tx_info_ptr = &context_ptr->data_info.tx_info;
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  boolean need_resend = FALSE; 

  //HSU_USE_PARAM(prev_rb_ptr);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  HSU_ULOG_1(NO_MSG, DATA_LOG, "req_num %u ",req_num);

  if (req_num == 0)
  {
    HSU_MSG_ERROR_3("hsu_al_ecm_tx_done: req_num == 0", 0, 0, 0);
    return;
  }
  
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET

  for (i=0; i < req_num; i++)
  {
   cur_rb_ptr = (rb_t *) q_get(tx_info_ptr->tx_pending_queue_ptr);
   
   if (cur_rb_ptr == NULL)
   {
     if (!(context_ptr->ignore_disconnect))
     {
       /* simply return as queue is already freed and no further action is required in this completion api */
       return;
     }
     /* else when ignore_disconnect is enabled, the queue should be intact and would be an error fatal condition */

     HSU_MSG_ERROR_3("curr_dsm_item is NULL: req_num (%d)", req_num, 0, 0);

     HSU_ULOG_1(NO_MSG, DATA_LOG, "curr_dsm_item is NULL: req_num %u ",req_num);

     /* Assert to prevent page fault */
     HSU_ASSERT(FALSE);
   }

   tx_info_ptr->pending_chain_len--;

   switch (status[i])
   {
   case CDC_TRANSFER_CANCELLED:
     need_resend = TRUE;
     //q_put(tx_info_ptr->tx_resending_queue_ptr, &cur_rb_ptr->link);
     HSU_MSG_HIGH_3("hsu_al_ecm_tx_done: CDC_TRANSFER_CANCELLED", 0, 0, 0);
     HSU_ULOG(NO_MSG, DATA_LOG, "CDC_TRANSFER_CANCELLED ");
     break;

   case CDC_TRANSFER_ERROR:
     need_resend = TRUE;
     //q_put(tx_info_ptr->tx_resending_queue_ptr, &cur_rb_ptr->link);
     HSU_MSG_ERROR_3("hsu_al_ecm_tx_done: CDC_TRANSFER_ERROR", 0, 0, 0);
     HSU_ULOG(NO_MSG, DATA_LOG, "CDC_TRANSFER_ERROR ");
     context_ptr->statistics.tx_transfer_error_cnt++;
     break;

   case CDC_TRANSFER_OK:
     if (context_ptr->debug_mode)
     {
       HSU_MSG_MED_3("CDC_TRANSFER_OK. length=%d, buf[0]=%x, buf[1]=%x",
         buf_sizes[i], ((uint8*)(buffers[i]))[0], ((uint8*)(buffers[i]))[1]);
     }

     if (context_ptr->debug_mode)
     {
       HSU_MSG_MED_3("hsu_al_ecm_tx_done. cur_rb_ptr=%x",
         cur_rb_ptr, 0, 0);
       if (cur_rb_ptr->app_ptr != NULL)
       {
         HSU_MSG_MED_3("ecm_tx_done. cur_rb_ptr->app_ptr=%x, buffers[i]=%x, cur_rb_ptr->pkt_ptr=%x",
           cur_rb_ptr->app_ptr, buffers[i], cur_rb_ptr->pkt_ptr);
       }
       else
       {
         HSU_MSG_MED_3("_ecm_tx_done. cur_rb_ptr->data_ptr=%x, buffers[i]=%x",
           cur_rb_ptr->data_ptr, buffers[i], 0);
       }
     }

     HSU_ULOG_1(NO_MSG, DATA_LOG, "CDC_TRANSFER_OK 0x%X ",(uint32)cur_rb_ptr);

     /* free current rb item */
     free_single_rb(cur_rb_ptr, FALSE);
	 if (tx_info_ptr->tx_handle_comp_scheduled == TRUE)
	 {
	   hsu_timer_cancel(&tx_info_ptr->tx_handle_complete_timer);
	 }
	 
	 if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
	 {
       context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, cur_rb_ptr->app_ptr);
	 }
	 else
	 {
	   context_ptr->data_info.tx_done_ptr_std(cur_rb_ptr->app_ptr);
	 }
     
     break;

   default:
     HSU_MSG_ERROR_3("hsu_al_ecm_tx_done: unexpected status %d", status[i], 0, 0);
     HSU_ASSERT((uint32)"hsu_al_ecm_tx_done: unexpected status" == NULL);

     break;
   }

   if(need_resend == TRUE)
   {
     free_single_rb(cur_rb_ptr, FALSE);
     if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
     {
       context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, cur_rb_ptr->app_ptr);
     }
     else
     {
       context_ptr->data_info.tx_done_ptr_std(cur_rb_ptr->app_ptr);
     }
   }  
   
  }
 
  if (context_ptr->debug_mode)
  {
   HSU_MSG_LOW_3("hsu_al_ecm_tx_done. tx_pending_queue_ptr=%x, cur_rb_ptr=0x%x",
     tx_info_ptr->tx_pending_queue_ptr, cur_rb_ptr, 0);
  }

  if(need_resend)
  {
    HSU_MSG_HIGH_3("hsu_al_ecm_tx_done. tx was suspended ",0,0,0);
	HSU_ULOG(ERROR_MSG, DATA_LOG, "Tx ERR/CNCL");
	if (tx_info_ptr->tx_handle_comp_scheduled == TRUE)
	{
	  hsu_timer_cancel(&tx_info_ptr->tx_handle_complete_timer);
	}
  }
#else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
  for (i=0, cur_rb_ptr = q_get(tx_info_ptr->tx_queue_ptr);
      (i < req_num) && cur_rb_ptr;
      ++i, cur_rb_ptr = q_get(tx_info_ptr->tx_queue_ptr))
  {
    if (context_ptr->debug_mode)
    {
      HSU_MSG_MED_3("hsu_al_ecm_tx_done. cur_rb_ptr=%x",
        cur_rb_ptr, 0, 0);
      if (cur_rb_ptr->app_ptr != NULL)
      {
        HSU_MSG_MED_3("ecm_tx_done. cur_rb_ptr->app_ptr=%x, buffers[i]=%x, cur_rb_ptr->pkt_ptr=%x",
          cur_rb_ptr->app_ptr, buffers[i], cur_rb_ptr->pkt_ptr);
      }
      else
      {
        HSU_MSG_MED_3("_ecm_tx_done. cur_rb_ptr->data_ptr=%x, buffers[i]=%x",
          cur_rb_ptr->data_ptr, buffers[i], 0);
      }
    }
    switch (status[i])
    {
    case CDC_TRANSFER_CANCELLED:
     HSU_MSG_HIGH_3("hsu_al_ecm_tx_done: CDC_TRANSFER_CANCELLED", 0, 0, 0);
      break;

    case CDC_TRANSFER_ERROR:
      HSU_MSG_ERROR_3("hsu_al_ecm_tx_done: CDC_TRANSFER_ERROR", 0, 0, 0);
      context_ptr->statistics.tx_transfer_error_cnt++;
      break;

    case CDC_TRANSFER_OK:
      if (context_ptr->debug_mode)
      {
        HSU_MSG_MED_3("CDC_TRANSFER_OK. length=%d, buf[0]=%x, buf[1]=%x",
          buf_sizes[i], ((uint8*)(buffers[i]))[0], ((uint8*)(buffers[i]))[1]);
      }

      HSU_ULOG_1(NO_MSG, DATA_LOG, "done: rb 0x%X ",(uint32)cur_rb_ptr);

      break;

    default:
      HSU_MSG_ERROR_3("hsu_al_ecm_tx_done: unexpected status %d", status[i], 0, 0);
      HSU_ASSERT((uint32)"hsu_al_ecm_tx_done: unexpected status" == NULL);

      break;
    }

    if (cur_rb_ptr)
    {
      HSU_ULOG_1(NO_MSG, DATA_LOG, "done: rb : %u ",cur_rb_ptr);
      free_single_rb(cur_rb_ptr, FALSE);
	  if (tx_info_ptr->tx_handle_comp_scheduled == TRUE)
	  {
	    hsu_timer_cancel(&tx_info_ptr->tx_handle_complete_timer);
	  }
	  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
	  {
            context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, cur_rb_ptr->app_ptr);
	  }
	  else
	  {
	    context_ptr->data_info.tx_done_ptr_std(cur_rb_ptr->app_ptr);
	  }
    }

    //prev_rb_ptr = cur_rb_ptr;
  }

  if (context_ptr->debug_mode)
  {
    HSU_MSG_LOW_3("hsu_al_ecm_tx_done. pending_chain_ptr=%x, cur_rb_ptr=0x%x",
      tx_info_ptr->pending_chain_ptr, cur_rb_ptr, 0);
  }

  /* free the DSM items that were sent. pending_chain_ptr will hold the 
  remaining items that were not yet sent */
  if (tx_info_ptr->pending_chain_ptr)
  {
    //dsm_free_packet(&tx_info_ptr->pending_chain_ptr);
    tx_info_ptr->pending_chain_ptr = cur_rb_ptr;
    tx_info_ptr->pending_chain_len -= req_num;
  }
#endif /* !FEATURE_HS_USB_RECOVER_DROPPED_PACKET */ 


} /*hsu_al_ecm_tx_done*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_GET_FAKE_MAC_ADDR_AS_STR

DESCRIPTION   
Creates a fake MAC address. 

The real MAC address in the application is formulated based on information 
sent by the host on the control channel.

However, in ECM, the MAC address should be supplied for the descriptors, 
before there's a QMI link, and therefore we cannot use the same method.

Therefore it was decided that the ECM AL will "make up" a MAC address for the 
descriptors, but the host driver will ignore this address, and use the current
protocol to negotiate the MAC address that will be used for the session.

This function is called only if we're working in a non standard ECM mode, for each
initialized (by the stack) ecm instance

DEPENDENCIES  
None.

RETURN VALUE  
TRUE/FALSE

SIDE EFFECTS  
None.

==============================================================================*/
static boolean hsu_al_ecm_get_fake_mac_addr_as_str
(
 char *  mac_addr_as_str_ptr, 
 uint8   size
 )
{
  HSU_ASSERT(mac_addr_as_str_ptr != NULL);

  if(size < MAC_ADDR_STR_SIZE)
  {
    return FALSE;
  }

  (void)snprintf(mac_addr_as_str_ptr, 
    size, 
    "%02x%02x%02x000000", 
    ((QMI_ETH_VENDOR_ID >> 16) & 0xFF), 
    ((QMI_ETH_VENDOR_ID >> 8) & 0xFF), 
    (QMI_ETH_VENDOR_ID & 0xFF));

  return TRUE;
} /* hsu_al_ecm_get_fake_mac_addr_as_str */ 

/*==============================================================================

FUNCTION      HSU_AL_ECM_CLEAR

DESCRIPTION   
Clears internal data of the HSU ECM AL that are not specific for a certain 
instance, and also, the first time this is called, zeros all the instance 
specific fields. 
Called when the instance counter hits zero. 
This happens on power-up, when ecm_init is called for the first time ever, and 
then whenever hsu_al_ecm_uninit is called (number of calls to ecm_init equals 
number of calls to hsu_al_ecm_uninit).

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_clear(uint8 core_index)
{
  static boolean was_already_called = FALSE;
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  hsu_al_ecm_control_info_type* control_info_ptr = NULL;
  hsu_al_ecm_data_info_type* data_info_ptr = NULL;
  int i = 0;

  if(was_already_called == FALSE)
  {
    was_already_called = TRUE;

    /* Initialize instance data */
    /****************************/
    for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
    {
      context_ptr = &ecm_instance_info[i];
      control_info_ptr = &ecm_instance_info[i].control_info;
      data_info_ptr = &ecm_instance_info[i].data_info;


#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      q_init(&data_info_ptr->tx_info.tx_pending_queue);
      data_info_ptr->tx_info.tx_pending_queue_ptr = 
        &data_info_ptr->tx_info.tx_pending_queue;

      q_init(&data_info_ptr->tx_info.tx_resending_queue);
      data_info_ptr->tx_info.tx_resending_queue_ptr = 
        &data_info_ptr->tx_info.tx_resending_queue;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

    /* General data */
      context_ptr->handle = NULL;
      context_ptr->state = HSU_ECM_INSTANCE_STATE_NONE;
      context_ptr->instance_is_enabled = FALSE;
      context_ptr->app_callbacks.enable_disable_cb_ext = NULL;
      context_ptr->app_callbacks.enable_disable_ext_user_data = NULL;
      memset((void*)&(context_ptr->statistics), 0, sizeof(ecm_instance_info[i].statistics));
      context_ptr->debug_mode = FALSE;

    /* Control Info */
      control_info_ptr->stream_id = SIO_NO_STREAM_ID;
      control_info_ptr->rx_queue_ptr = NULL;
      control_info_ptr->tx_queue_ptr = NULL;
      control_info_ptr->rx_func_ptr = NULL;
      control_info_ptr->write_buffer_data.ctrl_tx_num_of_bytes = 0;
      
    /* Data Info */
      data_info_ptr->stream_id = SIO_NO_STREAM_ID;
      data_info_ptr->rx_info.inbound_flow_is_enabled = FALSE; 
      data_info_ptr->rx_info.rx_chain_ptr = NULL;
      data_info_ptr->channel_opened = FALSE;
      data_info_ptr->dmux_handle = NULL;
      
      q_init(&data_info_ptr->tx_info.tx_queue);
      data_info_ptr->tx_info.tx_queue_ptr = &data_info_ptr->tx_info.tx_queue;
      data_info_ptr->tx_info.pending_chain_len = 0;

      // Optimization for ping delay/latency.
      data_info_ptr->rx_info.rx_timeout = HS_USB_ECM_RX_TIMEOUT_MS;
      
      data_info_ptr->rx_info.num_buf_waiting = 0;
      data_info_ptr->rx_info.array_size = HSU_SNPS_OUT_TRB_ARRAY_SIZE_MAX;
      data_info_ptr->rx_info.num_buf_already_forwarded = 0;
      data_info_ptr->rx_info.partial_packet_ptr = NULL;
      data_info_ptr->rx_info.is_pending = FALSE;
      data_info_ptr->tx_info.pending_chain_ptr = NULL;
      data_info_ptr->tx_info.pending_chain_len = 0;
      data_info_ptr->tx_info.tx_handle_comp_scheduled = FALSE;
      data_info_ptr->tx_info.tx_timeout = HS_USB_ECM_TX_TIMEOUT_MS;

      data_info_ptr->rx_queue_ptr = NULL;
      //data_info_ptr->tx_info.tx_queue_ptr = NULL;
      data_info_ptr->rx_func_ptr = NULL;

#ifdef FEATURE_HS_USB_STANDARD_ECM    
      data_info_ptr->connection_speed_args.downstream_bitrate = 0;
      data_info_ptr->connection_speed_args.upstream_bitrate = 0;
      data_info_ptr->conn_speed_mem.resend = FALSE;
	  data_info_ptr->rx_func_ptr_std = NULL;
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

#ifdef FEATURE_HS_USB_ECM_BAM
      /* BAM Info */
      context_ptr->bam_info.ul_bam_flow_enabled = FALSE;
      context_ptr->bam_info.dl_bam_flow_enabled = FALSE;
      // Never set to false since the same bam pipes will be used across bus resets
        // context_ptr->bam_info.is_bam_mode = FALSE;
        // context_ptr->bam_info.bam_pipes_allocated = FALSE;
#endif /* FEATURE_HS_USB_ECM_BAM */

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      context_ptr->ignore_disconnect = FALSE;
      context_ptr->is_core_enabled = FALSE;
      control_info_ptr->need_resending_pending_dsm = FALSE;
      control_info_ptr->pending_dsm_ptr = NULL;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */      

    }
  }

#ifdef FEATURE_HS_USB_ECM_BAM
  // Never decrement since the same bam pipes will be used across bus resets
    // ecm_num_of_allocated_bam_connections = 0;
#endif /* FEATURE_HS_USB_ECM_BAM */
} /* hsu_al_ecm_clear */

/*==============================================================================

FUNCTION      HSU_AL_ECM_UNINIT_CB

DESCRIPTION   
Called by the FD when a certain instance of the network function should be 
un-initialized.

DEPENDENCIES  
ecm_init() for the same instance must have been called.

RETURN VALUE  
JEINVAL if the context is invalid, HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_uninit_cb(cdc_appctx_t ctx)
{
  hsu_bus_mode_multi_core_cb_info_struct suspend_resume_callback_info;
  uint8 i;

  hsu_al_ecm_instance_info_type *app_ctx_ptr = (hsu_al_ecm_instance_info_type*)ctx;
  cdc_ctx_t *handle = app_ctx_ptr->handle;
  /* Since we clean all data structures at the first call handle might be null */ 
  core_t *core = NULL;
  uint8 core_index = 0;
  HSU_USE_PARAM(ctx);
  HSU_MSG_HIGH_1("hsu_al_ecm_uninit_cb(): Enter (handle=%d)", handle);    //pohanw_Q

  if (handle == NULL) 
  {
    /* When handle is NULL, this means that first call cleared subsequent 
    ** instances' handle 
    */
    return HSU_JSUCCESS;
  }

  HSU_ULOG_1(NO_MSG, BAM_LOG, "app_ctx_ptr 0x%08X", (uint32)app_ctx_ptr);
  
  core = handle->core_ctx;
  core_index = core->core_index;

  /* Assume that if this function is called for the FIRST configuration, it will 
     be called for ALL configurations. So clean data structures on the FIRST call 
     only. */ 
  if (hsu_al_ecm_num_of_initialized_instances[core_index][HSU_CONFIG_IDX_0] > 0)
  {
    for (i=0; i < HSU_MAX_NUM_OF_CONFIGS; i++)
    {
      hsu_al_ecm_num_of_initialized_instances[core_index][i] = 0;
    }
    memset(ecm_fd_handle,0, sizeof (ecm_fd_handle));


  /* uninit only FD related members. Application related members are
  untouched */
    for (i=0; i<HS_USB_NUM_OF_ECM_INST; i++)
    {
      
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      if(app_ctx_ptr->ignore_disconnect == FALSE)
      {
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */       
        if(ecm_instance_info[i].instance_is_enabled == TRUE)
        {
          HSU_ERR_FATAL("hsu_al_ecm_uninit_cb(): "
            "called when instance is enabled", 0, 0, 0);
        }
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */       
      ecm_instance_info[i].handle = NULL;
#ifdef FEATURE_HS_USB_ECM_BAM
      ecm_instance_info[i].bam_info.bam_connection.cdc_ctx_ptr = NULL;
#endif
    }

    /*Unregister bus_mode_notif */
    suspend_resume_callback_info.core_index = (uint32)(core->core_number); /* HAL core index */
    suspend_resume_callback_info.bus_mode_cb = hsu_al_ecm_suspend_resume_mode_changed_cb;
    suspend_resume_callback_info.app_data = NULL;
    hsu_unregister_bus_mode_multi_core_notif_cb(suspend_resume_callback_info);
    bus_mode_notif_registered = FALSE;
  } 
  return HSU_JSUCCESS;
} /* hsu_al_ecm_uninit_cb */

/*==============================================================================

FUNCTION      GET_AND_ALLOC_ECM_INSTANCE_INFO_FOR_FD

DESCRIPTION   
Called from ecm_init, to find an instance_info_ptr to be used for the specific 
FD instance. Giant mutex already locked.

DEPENDENCIES  
None.

RETURN VALUE  
A pointer to an ecm_instance_info.

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ecm_instance_info_type * get_and_alloc_ecm_instance_info_for_fd
(
 cdc_handle_t      handle,
 /* handle to this instance of the FD, to be used in further API calls */
 uint8*            cur_conf_inst_num
 /* pointer to the FD instance number */
)
{
  core_t *core = ((cdc_ctx_t*)handle)->core_ctx;
  uint8 core_index = core->core_index;
  uint8 cur_config_index = core->curr_config;
  uint32 num_of_intiialized_instance;
  *cur_conf_inst_num = 0;

  if ((core_index >= HSU_MAX_NUM_OF_CORES) 
    || ((cur_config_index-1) >= HSU_MAX_NUM_OF_CONFIGS))
  {
    HSU_ERR_FATAL("ecm_instance_info_for_fd: array out of bound"
      "core_index (%u), cur_config_index-1 (%u)", 
      core_index, cur_config_index-1, 0);
    return NULL;
  }
  
  num_of_intiialized_instance = hsu_al_ecm_num_of_initialized_instances[core_index][cur_config_index-1];

  /*Find an available slot in ecm_fd_handle[]*/
  if (num_of_intiialized_instance >= HS_USB_NUM_OF_ECM_INST)
  {
    HSU_MSG_ERROR_2("get_and_alloc_ecm_instance_info_for_fd: no more availiable"
      " instances for this configurations (config = %d, num_instances = %d)",
      cur_config_index,hsu_al_ecm_num_of_initialized_instances[core_index][cur_config_index-1]);
    return NULL;
  }

  /* If next available slot is empty, the assign the next slot immediately, 
  ** otherwise, run a loop to search the next available slot. */
  if (ecm_fd_handle[core_index][cur_config_index-1][num_of_intiialized_instance] == NULL)
  {
    *cur_conf_inst_num = num_of_intiialized_instance;
  }
  else
  { 
    uint8 i =  0;

    while (i < HS_USB_NUM_OF_ECM_INST)
    {
      if (ecm_fd_handle[core_index][cur_config_index-1][i] == NULL)
      {
        *cur_conf_inst_num = i;
        break;
      }  
      i++;
    }
    if (i == HS_USB_NUM_OF_ECM_INST)
    {
      /* We can't get here since if num_of_intiialized_instance < HS_USB_NUM_OF_ECM_INST
        there should be an available ecm_instance*/
      HSU_ERR_FATAL("get_and_alloc_ecm_instance_info_for_fd: Imposible to get here!", 0, 0, 0);
    }
  }

  /* We save the handle of all the FDs that the stack initialized. 
  When the configuration will be changed we will use the corresponding handle 
  for the stack's FD */
  ecm_fd_handle[core_index][cur_config_index-1][*cur_conf_inst_num] = handle;
  hsu_al_ecm_num_of_initialized_instances[core_index][cur_config_index-1]++;

  /*If the configuration will be switched we will update the handle in hsu_al_ecm_handle_set_configuration*/
  ecm_instance_info[*cur_conf_inst_num].handle = handle;
#ifdef FEATURE_HS_USB_ECM_BAM
  ecm_instance_info[*cur_conf_inst_num].bam_info.bam_connection.cdc_ctx_ptr = handle;
#endif
  if (ecm_instance_info[*cur_conf_inst_num].state == HSU_ECM_INSTANCE_STATE_NONE)
  {
      ecm_instance_info[*cur_conf_inst_num].state = HSU_ECM_INSTANCE_STATE_INITIALIZED;
  }
  return &ecm_instance_info[*cur_conf_inst_num];
} /* get_and_alloc_ecm_instance_info_for_fd */

/*==============================================================================

FUNCTION      GET_AND_ALLOC_ECM_INSTANCE_INFO_FOR_APP

DESCRIPTION   
Called from hsu_al_ecm_control_open() and hsu_al_ecm_data_open(), 
to find an instance_info_ptr to be used for the specific APP instance. 

DEPENDENCIES  
Should be called with Giant mutex locked.

RETURN VALUE  
A pointer to an ecm_instance_info.

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ecm_instance_info_type * get_and_alloc_ecm_instance_info_for_app
(
 sio_port_id_type      port_id
 )
{
  int i = 0;

  
  /* Else we check if the application already began opening this port:
     state == HSU_ECM_INSTANCE_STATE_HALF_OPENED
     ecm_instance_info.port_id = port_id 
   */
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
    if ((ecm_instance_info[i].port_id == port_id) &&
        (ecm_instance_info[i].state == HSU_ECM_INSTANCE_STATE_HALF_OPENED))
  {
      return &ecm_instance_info[i];
    }
  }
  /*Allocate new ecm instance */
  for (i = 0 ; i < HS_USB_NUM_OF_ECM_INST ; i++)
  {
    /*Only instance that was initialized by the stack can be allocated to the 
      application*/
    if(ecm_instance_info[i].state == HSU_ECM_INSTANCE_STATE_INITIALIZED)
    {
      return &ecm_instance_info[i];
    }
  }
    HSU_MSG_ERROR(
    "get_and_alloc_ecm_instance_info_for_app(): no avaliable instance.");
  return NULL;
} /* get_and_alloc_ecm_instance_info_for_app */

/*===========================================================================

FUNCTION hsu_al_ecm_set_control_line_state_cb

DESCRIPTION
Called whenever the ECM FD receives a SET_CONTROL_LINE_STATE from the host.
It will simply check if the DTR (Data Terminal Ready) is different than what 
was previously saved. If so, will update the new DTR and invoke the RmNet's 
DTR flag changed callback.

DEPENDENCIES
None

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
static jresult_t hsu_al_ecm_set_control_line_state_cb
(
 juint16_t ctrl_sig_bitmap, /* Bitmap representing the control line's 
                            state */
 cdc_appctx_t ctx           /* Context provided to the ECM FD, in ecm_init*/
)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  boolean is_dtr_asserted; 

  if ((ctrl_sig_bitmap & AL_SER_ECM_DTR_ASSERTED_MASK) == AL_SER_ECM_DTR_ASSERTED_MASK)
  {
    is_dtr_asserted = TRUE;
  }
  else
  {
    is_dtr_asserted = FALSE;
  }
  
  if(hsu_al_ecm_is_cdc_app_context_valid(ctx) == FALSE)
  {
    HSU_ERR_FATAL("hsu_al_ecm_set_control_line_state_cb(): ctx (%u) param invalid",
      ctx, 0, 0);
  }

  context_ptr = (hsu_al_ecm_instance_info_type *)(ctx);

  if (((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM )
  {
    HSU_MSG_ERROR("hsu_al_ecm_set_control_line_state_cb: not supported in standard ECM!");
    return JENOTSUP;
  }
  
  /* Check if the DTR bit is different between the given ctrl_sig_bitmap and
  the current ctrl_line_state: */
  if (context_ptr->ctrl_line_state != is_dtr_asserted)
  {
    context_ptr->ctrl_line_state = !context_ptr->ctrl_line_state;
    invoke_app_enable_disable_cbs(context_ptr);
  }

  return HSU_JSUCCESS;
}


/*==============================================================================

FUNCTION      HSU_AL_ECM_REGISTER_BUS_MODE_NOTIFICATIONS

DESCRIPTION   
Called by the ecm_init upon stack initialization. Registers via
hsu_register_bus_mode_multi_core_notif_cb() only once (when the first instance is 
initialized)

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
static jresult_t hsu_al_ecm_register_bus_mode_notifications(cdc_handle_t handle)
{
  hsu_bus_mode_multi_core_cb_info_struct suspend_resume_callback_info;

  if (bus_mode_notif_registered == FALSE)
  {
    core_t *core = ((cdc_ctx_t *)handle)->core_ctx;

    HSU_USE_PARAM(core);
    bus_mode_notif_registered = TRUE;

    suspend_resume_callback_info.core_index = (uint32)(core->core_number); /* HAL core index */
    suspend_resume_callback_info.bus_mode_cb = hsu_al_ecm_suspend_resume_mode_changed_cb;
    suspend_resume_callback_info.app_data = NULL;
    /*Since the mode will change for all RmNet instances there is no use for the app_data param.
      When the cb will be called we will go over all of the initialized RmNet instances */
    if (hsu_register_bus_mode_multi_core_notif_cb(suspend_resume_callback_info)
      == FALSE)
    {
      HSU_ERR_FATAL("ecm_init(): "
        "hsu_register_bus_mode_multi_core_notif_cb failed", 0, 0, 0);
      return EUNKNOWN;
    }
  }
  return HSU_JSUCCESS;
}

#ifdef FEATURE_HS_USB_STANDARD_ECM
/*==============================================================================

FUNCTION          HSU_AL_ECM_CALCULATE_MAC_ADDRESS

DESCRIPTION
This function calculates the MAC address according to the IMEI / ESN.
The MAC address is 6 octets long. The first 3 are constant and hold the manufacturer 
ID (HS_USB_MANUFACTURER_ID). The last 3 are defined by the application (3 bytes)
In order to support Multiple RMNet instances in the future we need to assign each 
RmNet instance a unique MAC address. Thus we'll generate a 20 bit (5h digits) unique
string from the ESN/IMEI  and use the last 4 bits to identify each RmNet instance.

The calculated MAC address is returned as a string so each digit (4 bits) is 
represented by char = 1 byte. 

DEPENDENCIES
None.

RETURN VALUE
TRUE on success FALSE otherwise 

SIDE EFFECTS
None. 

==============================================================================*/
static boolean hsu_al_ecm_get_mac_addr_as_str
(
 char *  mac_addr_as_str_ptr, 
 uint8   size
)
{
  char      esn_imei_buff_ptr[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE];
  char      serial_number_buf[HS_USB_SERIAL_NUMBER_BUF_SIZE+1] = {0};
  juint32_t chip_serial;

  HSU_MSG_HIGH("hsu_al_ecm_get_mac_addr_as_str: Enter");
  if((size < MAC_ADDR_STR_SIZE) || 
    !mac_addr_as_str_ptr)
  {
    HSU_MSG_ERROR("*_get_mac_addr_as_str: Invalid input!");
    return FALSE;
  }

  if (hsu_config_selector_is_standard_ecm_supported() == TRUE)
  {
    chip_serial = HWIO_SERIAL_NUM_IN;
	chip_serial = GET_CHIP_SERIAL_FOR_MAC_ADDR(chip_serial);
	(void)snprintf(serial_number_buf, HS_USB_SERIAL_NUMBER_BUF_SIZE+1, 
    "%08X",chip_serial);
	
	if (chip_serial == 0)
	{
	  /*Try reading the ESN first*/   
	  if (!hsu_conf_sel_nv_get_esn(esn_imei_buff_ptr,sizeof(esn_imei_buff_ptr)))
	  {
		/* If we didn't manage to read the ESN - try to read the IMEI */
		if (!hsu_conf_sel_nv_get_imei(esn_imei_buff_ptr,sizeof(esn_imei_buff_ptr)))
		{
		  /* We didn't manage to read either the ESN or the IMEI. In this case we will use 
			  a constant (hardcoded) MAC address. 
		  */
		  HSU_MSG_ERROR(
			  "*_esn_imei_read(): Couldn't read NV_ESN_I and NV_UE_IMEI_I from NV");
		  memset(serial_number_buf,'0',HS_USB_SERIAL_NUMBER_BUF_SIZE);
		}
		else
		{
		  // Construct the MAC address from the IMEI: 
		  // We will use the Serial number of the model as a unique value to construct the MAC address. 
		  // See documentation of HS_USB_INDX_OF_SERIAL_NUM_IN_IMEI for more details
		  memscpy(serial_number_buf,HS_USB_SERIAL_NUMBER_BUF_SIZE,
		  &esn_imei_buff_ptr[HS_USB_INDX_OF_SERIAL_NUM_IN_IMEI],HS_USB_SERIAL_NUMBER_BUF_SIZE);
		}
	  }
	  else
	  { 
		// Construct the MAC address from the ESN. Take only 5 digits of the ESN = 20 bits. 
		// See documentation of  HS_USB_SERIAL_NUMBER_BUF_SIZE for more details.
		memscpy(serial_number_buf,HS_USB_SERIAL_NUMBER_BUF_SIZE,
		  &esn_imei_buff_ptr[HS_USB_INDX_OF_SERIAL_NUM_IN_ESN],HS_USB_SERIAL_NUMBER_BUF_SIZE);
	  }
	}
  }
  /*Right now we have only 1 RmNet instance. the following sets the RmNet instance in the 
  MAC address. Will be changed when multiple RMNet is supported*/
  (void)snprintf(mac_addr_as_str_ptr, 
    size, 
    "%s%s0", HS_USB_MANUFACTURER_ID,serial_number_buf);

  HSU_MSG_HIGH("hsu_al_ecm_get_mac_addr_as_str: Exit");
  return TRUE;
}
#endif /*FEATURE_HS_USB_STANDARD_ECM*/
#ifdef FEATURE_HS_USB_MBIM
/*==============================================================================

FUNCTION      HSU_AL_ECM_RETURN_MBIM_CB_STATUS

DESCRIPTION   
Informs whether MBIM-QBI Callbacks have been sucessfully registered 

DEPENDENCIES  
None.

RETURN VALUE  
NONE

SIDE EFFECTS  
None.

==============================================================================*/
static boolean hsu_al_ecm_return_mbim_cb_status()
{
	return(is_mbim_cb_registered);
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_RESET_FUNCTION_RECEIVED

DESCRIPTION   
Informs the QBI Layer the RESET_FUNCTION received their registerd callback. 

DEPENDENCIES  
None.

RETURN VALUE  
NONE

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_handle_reset_function_received
(
  cdc_handle_t handle
)
{
  if ((((cdc_ctx_t*)handle)->info.basic.flags & CDC_INIT_MBIM_ECM)
  &&(mbim_cb_list.hsu_al_ecm_usb_to_qbi.hsu_al_ecm_hsu_al_ecm_reset_mbim_cb != NULL))
  {
    /*Just call their call back that it is received*/
    mbim_cb_list.hsu_al_ecm_usb_to_qbi.hsu_al_ecm_hsu_al_ecm_reset_mbim_cb(handle);
  }
  else
  {
    /*This means that callbacks have not been registered*/
    is_mbim_cb_registered = FALSE;
  }
}
/*==============================================================================

FUNCTION      HSU_AL_ECM_SET_NTB_INPUT_SIZE_RECEIVED

DESCRIPTION   
Informs the QBI Layer the NTB size through their registerd callback.

DEPENDENCIES  
None.

RETURN VALUE  
None

SIDE EFFECTS  
None.

==============================================================================*/
static void hsu_al_ecm_set_ntb_input_size_received
(
  cdc_handle_t handle,
  juint32_t ntb_input_size,
  juint16_t max_datagrams_in_ntb
)
{
  if ((((cdc_ctx_t*)handle)->info.basic.flags & CDC_INIT_MBIM_ECM)
  &&	(mbim_cb_list.hsu_al_ecm_usb_to_qbi.hsu_al_ecm_hsu_al_ecm_set_ntb_input_size_cb != NULL))
  {
    mbim_cb_list.hsu_al_ecm_usb_to_qbi.hsu_al_ecm_hsu_al_ecm_set_ntb_input_size_cb(
      handle,ntb_input_size,max_datagrams_in_ntb);
  }
  else
  {
    /*This means that the callbacks have not been registered*/
    is_mbim_cb_registered = FALSE;
  }

}
#endif

/*==============================================================================

FUNCTION      ECM_INIT

DESCRIPTION   
Called by the FD upon stack initialization. 

DEPENDENCIES  
None.

RETURN VALUE  
HSU_JSUCCESS for success.

SIDE EFFECTS  
None.

==============================================================================*/
jresult_t ecm_init
(
 cdc_handle_t      handle, 
 /* handle to this instance of the FD, to be used in further API calls */
 ecm_callbacks_t * cbs_ptr,
 /* OUT parameter - adaptation layer CBs to be invoked by the FD */
 ecm_init_info_t * info_ptr, 
 /* OUT parameter - application / adaptation layer information to be used by 
 * the FD for descriptor building */
 cdc_appctx_t *    ctx_ptr
 /* OUT parameter - adaptation layer context to be passed by the FD when CBs 
 * are invoked */
 )
{  
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  core_t *core = ((cdc_ctx_t*)handle)->core_ctx;
  uint8 core_index = core->core_index;

  /* Below value is intended to be set by return by param.
  ** Initialize to invalid value to catch error if it is not set to correct
  ** value by that function.
  */
  uint8 cur_conf_inst_num = HS_USB_NUM_OF_ECM_INST;
  /* Boolean flag indicating if ECM-AL is operating in standard ECM mode*/
  boolean hsu_al_ecm_is_standard = FALSE;
#ifdef FEATURE_HS_USB_MBIM
  /* Boolean flag indicating if ECM-AL is operating in MBIM mode*/
  boolean hsu_al_ecm_is_mbim_present = FALSE;
#endif /* FEATURE_HS_USB_MBIM */
  jresult_t return_val = HSU_JSUCCESS;

  hsu_al_ecm_is_standard = hsu_config_selector_is_standard_ecm_supported();

  /*****************************************************************************
  * Initialize the instance's info_ptr structure
  ****************************************************************************/ 
  /* This function has a static flag to verify that the clear is done only once */
  hsu_al_ecm_clear(core_index);

  context_ptr = get_and_alloc_ecm_instance_info_for_fd(handle, &cur_conf_inst_num);
  
#ifdef FEATURE_HS_USB_MBIM
  if(hsu_conf_sel_is_instance_mbim(cur_conf_inst_num))
  {
    hsu_al_ecm_is_mbim_present = TRUE;
  }

  /* MBIM on ECM uses part of standard ECM implementation,
  ** Therefore, any composition that supports MBIM should support
  ** standard ECM
  */
  hsu_al_ecm_is_standard |= hsu_al_ecm_is_mbim_present;
#endif /* FEATURE_HS_USB_MBIM */

#ifndef FEATURE_HS_USB_STANDARD_ECM
  if (hsu_al_ecm_is_standard)
  {
    HSU_MSG_ERROR("ecm_init(): FEATURE_HS_USB_STANDARD_ECM is not defined"
      " but hsu_al_ecm_is_standard = TRUE!");
    hsu_al_ecm_is_standard = FALSE;
  }
#else
  if ((hsu_al_ecm_is_standard)
#ifdef FEATURE_HS_USB_MBIM
     && !(hsu_al_ecm_is_mbim_present)
#endif /* FEATURE_HS_USB_MBIM */
	 )
  {
    /* Prevent D+ from going high until the any of the ECM port is opened.
    */
    core_disallow_enumeration(HSU_CORE_IDX_PRIMARY);    
  }
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

  HSU_ASSERT(cbs_ptr != NULL);
  HSU_ASSERT(info_ptr != NULL);  

  HSU_MSG_HIGH_1("ecm_init(): Enter (handle=%d)", handle);

#ifdef T_UNDP
    /* 
    ** In UNDP (Gobi 3 or later version), 
    ** - the ECM interface (NET) should be set as 0 in static interfaces
    ** - if NET alternate is enabled, should be set as 5 in static interfaces  
    */
    if (hsu_is_undp_using_static_interface () == TRUE) 
    {
      info_ptr->basic.interface_number = NORMAL_INTERFACE_NO;

      if (hsu_is_alternate_net_enabled () == TRUE)
      {
        info_ptr->basic.interface_number = ALTERNATE_NET_INTERFACE_NO;
      }
    }
#endif /* T_UNDP */

  /*****************************************************************************
  * Info
  ****************************************************************************/ 
  info_ptr->basic.flags = (juint32_t)(CDC_INIT_BUFFER_DMABLE);

  if (!hsu_al_ecm_is_standard)
  {
    info_ptr->basic.flags |= (juint32_t)(CDC_INIT_VENDOR_SPECIFIC_CLASS |
      CDC_INIT_SINGLE_INTERFACE |
      CDC_INIT_NO_ALT_SETTINGS |
      CDC_INIT_NO_CLASS_DESC);

#ifdef FEATURE_HS_USB_INTR_ONLY_ECM_PIPE
    if(hsu_conf_sel_is_curr_instance_intr_in_only_ecm(cur_conf_inst_num))
    {
      info_ptr->basic.flags |= (juint32_t)( CDC_INIT_INTR_ONLY_ECM_PIPE);
    }
#endif /* FEATURE_HS_USB_INTR_ONLY_ECM_PIPE */

    if (hsu_conf_sel_is_curr_instance_dpl(cur_conf_inst_num))
    {
      info_ptr->basic.flags |= (juint32_t)(CDC_INIT_DPL);
    }

    if(hsu_al_ecm_get_fake_mac_addr_as_str(
      info_ptr->mac_addr, 
      sizeof(info_ptr->mac_addr)) != TRUE)
    {
      HSU_ERR_FATAL("ecm_init(): hsu_al_ecm_get_fake_mac_addr_as_str failed",
        0, 0, 0);
    }
    info_ptr->basic.cci_subclass = HSU_AL_ECM_SUBCLASS_PROP;
  }
#ifdef FEATURE_HS_USB_STANDARD_ECM
  else /*hsu_al_ecm_is_standard = true*/
  {
    /* Since hsu_al_ecm_is_standard is a local variable we have to save the value in 
      context info*/
    if(hsu_al_ecm_is_standard)
    {
      info_ptr->basic.flags |= (juint32_t)( CDC_INIT_STD_ECM );
    }

#ifdef FEATURE_HS_USB_MBIM
    /* MBIM supports IAD so this check needs to be added */
    if (!hsu_al_ecm_is_mbim_present)
    {
    /* Union descriptor is supported instead of IAD */
    info_ptr->basic.flags |= (juint32_t)CDC_INIT_NO_IAD;
    }
#endif /* FEATURE_HS_USB_MBIM */
    if(hsu_al_ecm_get_mac_addr_as_str(
      info_ptr->mac_addr, 
      sizeof(info_ptr->mac_addr)) != TRUE)
    {
      HSU_ERR_FATAL("ecm_init(): hsu_al_ecm_get_mac_addr_as_str failed", 0, 0, 0);
    }

#ifdef FEATURE_HS_USB_MBIM
    if(hsu_al_ecm_is_mbim_present)
    {
      info_ptr->basic.flags |= (juint32_t)( CDC_INIT_MBIM_ECM );
    }
#else
    info_ptr->basic.cci_subclass = HSU_AL_ECM_SUBCLASS_STANDARD;  
#endif
  }
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

  /* Only for SNPS core allow UL BAM BYPPASS */
  //if (hsu_conf_sel_ul_bam_bypassed() && core_index == USB3_CORE_INDEX)
  {
    info_ptr->basic.flags |= CDC_INIT_SSUSB_UL_BAM_BYPASS;
  }
  //else

  //{
  //  info_ptr->basic.flags &= ~CDC_INIT_SSUSB_UL_BAM_BYPASS;
  //}

  info_ptr->basic.max_segment_size = MAX(HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE,UNCACHED_DSM_ITEM_SIZE) ;

  info_ptr->basic.cci_protocol = HSU_AL_ECM_PROTOCOL;
  info_ptr->eth_statistics = 0;
  /* Flags defined in ecm_enum_stats_t, 0 means no capabilities */


  /* 
  * The num_power_filters field contains the number of pattern filters that are
  * available for causing wake-up of the host. 
  * It is related to the ECM requestSetEthernetPowerManagementPatternFilter, 
  * which is optional, and not supported by us. 
  * From the CDC/ECM spec: Some hosts are able to conserve energy and stay 
  * quiet in a “sleeping” state while not being used. USB Networking devices   
  * may provide special pattern filtering hardware that enables it to wake up  
  * the attached host on demand when something is attempting to contact the  
  * host (e.g., an incoming web browser connection). 
  * Primitives are needed in management plane to negotiate the setting of these 
  * special filters.
  */
  info_ptr->num_power_filters = 0;  /* no power filters */

  /* mc_filters - Contains the number of multi cast filters that can be 
  configured by the host. */ 
  /* 
  * From the CDC/ECM Spec:
  * 0  - The device performs perfect multi cast address filtering (no hashing).
  * 1 - The device uses imperfect multi cast address filtering (hashing). 
  *      Here, the host software driver must perform further qualification 
  *      itself to achieve perfect filtering.
  */
  info_ptr->mc_filters.perfect_filtering = 1; 
  /* bit field can have the values 0 or 1, otherwise Lint is unhappy */

  /* 
  * From the CDC/ECM Spec:
  * The num_filters field contains the number of multi cast filters that
  * can be configured by the host.
  * If this value is 0, the device does not support the ECM request
  * SetEthernetMulticastFilters.
  */
  info_ptr->mc_filters.num_filters = 0; 

  /* Enable support for ZLP */
  info_ptr->basic.zlp_terminator = TRUE;

  /*****************************************************************************
  * Callbacks
  ****************************************************************************/ 
  
  cbs_ptr->basic.enabled_h = hsu_al_ecm_enabled_cb;
  cbs_ptr->basic.disabled_h = hsu_al_ecm_disabled_cb;
  cbs_ptr->basic.uninit_h = hsu_al_ecm_uninit_cb;
  cbs_ptr->basic.encap_command_h = hsu_al_ecm_encap_command_cb;
  cbs_ptr->set_multicast_filter_h = NULL;
  cbs_ptr->set_power_filter_h = NULL;
  cbs_ptr->get_power_filter_h = NULL;
  cbs_ptr->set_packet_filter_h = hsu_al_ecm_set_pkt_filter_cb;
  cbs_ptr->get_stats_h = NULL;
  cbs_ptr->set_control_line_state_h = hsu_al_ecm_set_control_line_state_cb;

  /* Request completion handlers */
  cbs_ptr->basic.encap_response_complete_h = 
    hsu_al_ecm_encap_resp_comp_cb;
  cbs_ptr->get_power_filter_complete_h = NULL;
  cbs_ptr->get_stats_complete_h = NULL;

  /* Notification completion handlers */
  cbs_ptr->notify_net_conn_complete_h = hsu_al_ecm_notif_net_conn_comp_cb;
  cbs_ptr->notify_speed_change_complete_h = 
    hsu_al_ecm_notif_conn_speed_ch_comp_cb;

  /* Transfer completion handlers */
  cbs_ptr->basic.read_complete_h = NULL;
  cbs_ptr->basic.read_multi_complete_h = hsu_al_ecm_rx_done;
  cbs_ptr->basic.write_complete_h = NULL;
  cbs_ptr->basic.write_multi_complete_h = hsu_al_ecm_tx_done;

  cbs_ptr->basic.resumed_h = NULL;

#ifdef FEATURE_HS_USB_MBIM
  /* MBIM Related callbacks to pass back data to QBI*/
  cbs_ptr->handle_reset_function_h = hsu_al_ecm_handle_reset_function_received;
  cbs_ptr->set_ntb_input_size_h = hsu_al_ecm_set_ntb_input_size_received;
  cbs_ptr->get_mbim_cb_status_h = hsu_al_ecm_return_mbim_cb_status;
#else
  cbs_ptr->handle_reset_function_h = NULL;
  cbs_ptr->set_ntb_input_size_h = NULL;
  cbs_ptr->get_mbim_cb_status_h = NULL;
#endif

  /* The context pointer cannot be NULL at this point */
  if (!context_ptr)
  {
    HSU_ERR_FATAL("ecm_instance_info conext is NULL: core_index(%u), instance_number (%u)", 
      core_index, cur_conf_inst_num, 0);
  }

  memscpy(&(context_ptr->mac_address), sizeof(info_ptr->mac_addr), info_ptr->mac_addr, 
    sizeof(info_ptr->mac_addr));

  *ctx_ptr = context_ptr;

  /* This function has a static flag to verify that the clear is done only once */
    return_val = hsu_al_ecm_register_bus_mode_notifications(handle);

  /* hsu_al_ecm_strings array size for entry should be same as 
  ** HS_USB_NUM_OF_ECM_INST.  Check for array index out of bound.
  */
  HSU_ASSERT(cur_conf_inst_num < HSU_ECM_STRING_ARRAY_SIZE);

  /* Communication interface string descriptor */
  info_ptr->basic.comm_string = hsu_al_ecm_strings[cur_conf_inst_num].control;
  /* data interface string descriptor */
  info_ptr->basic.data_string = hsu_al_ecm_strings[cur_conf_inst_num].data;

  return return_val;
} /* ecm_init */ 

/*==============================================================================

FUNCTION      HSU_AL_ECM_OPEN_PTR_IS_VALID

DESCRIPTION   
Validates that certain fields of the sio_open_type parameter passed to 
hsu_al_ecm_control_open() and hsu_al_ecm_data_open() are as expected. 

DEPENDENCIES  
None.

RETURN VALUE  
TRUE/ FALSE

SIDE EFFECTS  
None.

==============================================================================*/
static boolean hsu_al_ecm_open_ptr_is_valid
(
  const sio_open_type * open_ptr,
  boolean validate_data_parameters
)
{
  HSU_ASSERT(open_ptr != NULL);

  if(open_ptr->stream_id == SIO_NO_STREAM_ID)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_open_ptr_is_valid(): stream_id==SIO_NO_STREAM_ID");
    return FALSE;
  }

  if (validate_data_parameters)
  {
    if(open_ptr->tx_queue == NULL) 
    {
      HSU_MSG_ERROR(
        "hsu_al_ecm_open_ptr_is_valid(): TX WM Q is not defined");
      return FALSE;
    }

    /* sio_open and sio_control_open allow the open_ptr->rx_queue to be NULL if 
    * open_ptr->rx_func_ptr is not NULL. 
    */
    if((open_ptr->rx_queue == NULL) && (open_ptr->rx_func_ptr == NULL))
    {
      HSU_MSG_ERROR(
        "hsu_al_ecm_open_ptr_is_valid(): Neither RX WM nor rx_func_ptr is defined");
      return FALSE;
    }

    if(open_ptr->stream_mode != SIO_GENERIC_MODE)
    {
      HSU_MSG_ERROR_1(
        "hsu_al_ecm_open_ptr_is_valid(): stream_mode!=SIO_GENERIC_MODE (%d)", 
        open_ptr->stream_mode);
      return FALSE;
    }

    if(open_ptr->rx_bitrate != SIO_BITRATE_BEST)
    {
      HSU_MSG_ERROR_1(
        "hsu_al_ecm_open_ptr_is_valid(): rx_bitrate!=SIO_BITRATE_BEST (%d)", 
        open_ptr->rx_bitrate);
      return FALSE;
    }

    if(open_ptr->tx_bitrate != SIO_BITRATE_BEST)
    {
      HSU_MSG_ERROR_1(
        "hsu_al_ecm_open_ptr_is_valid(): tx_bitrate!=SIO_BITRATE_BEST (%d)", 
        open_ptr->tx_bitrate);
      return FALSE;
    }

    if(open_ptr->tail_char_used != FALSE)
    {
      HSU_MSG_ERROR(
        "hsu_al_ecm_open_ptr_is_valid(): tail_char_used!=FALSE");
      return FALSE;
    }

    if(open_ptr->rx_flow != SIO_FCTL_BEST)
    {
      HSU_MSG_ERROR_1(
        "hsu_al_ecm_open_ptr_is_valid(): rx_flow!=SIO_FCTL_BEST", 
        open_ptr->rx_flow);
      return FALSE;
    }

    if(open_ptr->tx_flow != SIO_FCTL_BEST)
    {
      HSU_MSG_ERROR_1(
        "hsu_al_ecm_open_ptr_is_valid(): tx_flow!=SIO_FCTL_BEST", 
        open_ptr->tx_flow);
      return FALSE;
    }
  }

  return TRUE;
} /* hsu_al_ecm_open_ptr_is_valid */


/*==============================================================================

FUNCTION      HSU_AL_ECM_OPEN_DATA

DESCRIPTION   
Called by the application to allocate and configure the data plane for the 
network function.
This API is no longer used to initialize the data plane of USB-IPA BAM2BAM connection
and is unhooked accordingly.
Most of the functionality of this API must remain intact due to existance of BAM2BAM connections
that are not with the IPA.

DEPENDENCIES  
ecm_init() must have been called for this instance.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter

SIDE EFFECTS  
This causes Rx buffers to be given to the USB stack, to be used when OUT 
transfers arrive. 

==============================================================================*/
void hsu_al_ecm_open_data(hsu_al_ecm_instance_info_type *context_ptr)
{
  bam_dmux_status               status;

  HSU_MSG_HIGH("hsu_al_ecm_data_open(): Enter");
  HSU_ULOG(NO_MSG, DATA_LOG, "Enter");

  if (!hsu_conf_sel_is_hsusb_initialized())
  {
    HSU_MSG_ERROR("hsu_al_ecm_data_open: HSUSB is not initialized!");
    return;
  }

  jsafe_enter();

  if(context_ptr == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_data_open(): get_and_alloc_ecm_instance_info_for_app failed");
    jsafe_leave();
    return;
  }

  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
  {
    /* Open the BAM Dmux channel */
    status = bam_dmux_open(USB_DATA_CHANNEL,(void *)context_ptr,
                  NULL,                        //bam_dmux_open_options * options,
                  hsu_al_ecm_dmux_event_notify,//bam_dmux_event_notify_cb event_cb,
                  hsu_al_ecm_rx_complete,      //bam_dmux_tx_done_notify_cb tx_done_cb/UL done
                  hsu_al_ecm_tx_data,          //bam_dmux_rx_notify_cb/DL
                  &(context_ptr->data_info.dmux_handle));

    if((status != BAM_DMUX_STATUS_SUCESS) || (context_ptr->data_info.dmux_handle == NULL))
    {
      HSU_MSG_ERROR("hsu_al_ecm_data: Dmux can not be opened!");
      HSU_ULOG(NO_MSG, DATA_LOG,"Dmux can not be opened");
      jsafe_leave();
      return;
    }
  }

  if(context_ptr->state == HSU_ECM_INSTANCE_STATE_HALF_OPENED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_OPENED;
  }
  else if(context_ptr->state == HSU_ECM_INSTANCE_STATE_INITIALIZED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
  }
  else
  {
    HSU_ERR_FATAL(
        "hsu_al_ecm_data_open(): state = %d, expected INITIALIZED or HALF_OPENED",
        context_ptr->state, 0, 0);
  }

  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
  {
    context_ptr->data_info.rx_func_ptr = bam_dmux_tx;
    context_ptr->data_info.tx_done_ptr = bam_dmux_rx_done;

    if((context_ptr->data_info.rx_func_ptr == NULL) ||
      (context_ptr->data_info.tx_done_ptr == NULL))
    {
      HSU_MSG_ERROR("hsu_al_ecm_data_open(): Invalid bam dmux tx function ");
      jsafe_leave();
      return;
    }
  }

  /* Initialize the Tx/DL request handler*/  
  if(context_ptr->data_info.tx_info.pending_chain_ptr == NULL)
  {
    if( FALSE == rb_queue_init(context_ptr->data_info.tx_info.tx_chain, 
                             HSU_ECM_RMNET_BUF_SIZE,
                             ECM_MAX_NUM_RB_ITEMS_FOR_WRITE,
                             FALSE))
    {
      HSU_ERR_FATAL("hsu_al_ecm_open_data: Tx  req queue init failed", 0,0,0);
      jsafe_leave();
      return;
    }
  
    context_ptr->data_info.tx_info.pending_chain_ptr = &context_ptr->data_info.tx_info.tx_chain[0];
  }

  /* Initialize the Rx/UL request handler and buffer*/  
  if(context_ptr->data_info.rx_info.rx_chain_ptr == NULL)
  {
    if (FALSE == rb_queue_init(context_ptr->data_info.rx_info.rx_chain,
                               HSU_ECM_RMNET_BUF_SIZE,
                               context_ptr->data_info.rx_info.array_size,
                               TRUE)
                              )
    {
      HSU_ERR_FATAL("hsu_al_ecm_open_data: Rx Memory alloc failed",0,0,0);
      jsafe_leave();
      return;
    }

    context_ptr->data_info.rx_info.rx_chain_ptr = &context_ptr->data_info.rx_info.rx_chain[0];
  }

  if (context_ptr->data_info.tx_info.tx_timeout == 0)
  {
    /* Tx handle complete timer */
    context_ptr->data_info.tx_info.tx_handle_complete_timer = hsu_timer_init(
      HSU_TIMER_CID__ECM_TX_HANDLE_COMPLETE,
      HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
      hsu_al_ecm_handle_tx_complete,
      (timer_cb_data_type)context_ptr);
  }
  
  if (context_ptr->data_info.rx_info.inbound_flow_is_enabled == TRUE &&
		(((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM ) != 0)
  {
    /* Setup a new read, but only  in case of STD_ECM and the inbound flow is enabled:*/
    hsu_al_ecm_setup_read(context_ptr);
  }
    
  HSU_MSG_HIGH("hsu_al_ecm_data_open(): Exit");

  jsafe_leave();
  return;
} /* hsu_al_ecm_data*/



/*==============================================================================

FUNCTION      HSU_AL_ECM_CONTROL_OPEN

DESCRIPTION   
Called by the application to allocate and configure the control plane for the 
network device.

DEPENDENCIES  
ecm_init() must have been called for this instance.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter

SIDE EFFECTS  
None.

==============================================================================*/
sio_status_type hsu_al_ecm_control_open
(
  sio_open_type * open_ptr
)
{
  hsu_al_ecm_instance_info_type * context_ptr;

  HSU_MSG_HIGH("hsu_al_ecm_control_open(): Enter");
  HSU_ULOG(NO_MSG, DATA_LOG, "Enter");

  if (!hsu_conf_sel_is_hsusb_initialized())
  {
    HSU_MSG_ERROR("hsu_al_ecm_control_open: HSUSB is not initialized!");
    return SIO_BUSY_S;
  }
  if(!open_ptr)
  {
    HSU_MSG_ERROR("hsu_al_ecm_control_open: open_ptr = null");
    return SIO_BADP_S;
  }

  if(hsu_al_ecm_open_ptr_is_valid(open_ptr, TRUE) == FALSE)
  {
    HSU_MSG_ERROR("hsu_al_ecm_control_open(): open_ptr has some invalid values");
    return SIO_BADP_S;
  }

  jsafe_enter();
  context_ptr = get_and_alloc_ecm_instance_info_for_app(open_ptr->port_id);
  if(context_ptr == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_ecm_control_open(): get_and_alloc_ecm_instance_info_for_app failed");
    jsafe_leave();
    return SIO_UNAVAIL_S;
  }

  HSU_ASSERT(context_ptr->control_info.stream_id == SIO_NO_STREAM_ID);

  context_ptr->control_info.stream_id = open_ptr->stream_id;
  context_ptr->port_id = open_ptr->port_id;
  /* This is the instance's unique id */ 
  context_ptr->control_info.rx_queue_ptr = open_ptr->rx_queue;
  context_ptr->control_info.tx_queue_ptr = open_ptr->tx_queue;
  context_ptr->control_info.rx_func_ptr = open_ptr->rx_func_ptr;

  if(context_ptr->state == HSU_ECM_INSTANCE_STATE_HALF_OPENED)
  {
    context_ptr->state = HSU_ECM_INSTANCE_STATE_OPENED;
  }
  else if(context_ptr->state == HSU_ECM_INSTANCE_STATE_INITIALIZED)
  {
      context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
    }

  HSU_MSG_HIGH("hsu_al_ecm_control_open(): Exit");

  jsafe_leave();

  hsu_al_ecm_open_data(context_ptr);
  
  if(hsu_config_selector_is_standard_ecm_supported() == TRUE)
  {
    core_allow_enumeration(0);
  }

  return SIO_DONE_S;
} /* hsu_al_ecm_control_open */

/*==============================================================================

FUNCTION      GET_INSTANCE_INFO_FROM_STREAM_ID

DESCRIPTION   
Returns a pointer to the ecm_instance_info_type that matches the given stream 
id.

DEPENDENCIES  
Should be called with Giant mutex locked.

RETURN VALUE  
Pointer to the ecm_instance_info_type that matches the given stream id.

SIDE EFFECTS  
None.
==============================================================================*/
static hsu_al_ecm_instance_info_type * get_instance_info_from_stream_id
(
 sio_stream_id_type  stream_id
 )
{
  int i = 0;
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST ; i++)
  {
    if((stream_id == ecm_instance_info[i].data_info.stream_id) ||
      (stream_id == ecm_instance_info[i].control_info.stream_id))
    {
      return &ecm_instance_info[i];
    }
  }
  HSU_MSG_ERROR_1(
    "get_instance_info_from_stream_id(): unknown stream_id=%d ",
    stream_id);
  return NULL;
} /* get_instance_info_from_stream_id */

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONTROL_TRANSMIT

DESCRIPTION   
Called by the application, when it wishes to send a message on the control
channel. The message is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer, and gives it to
the cdc_send_encap_response() function of the USB stack. 

The stack is responsible to generate a "response available" notification, and 
then send the message after the host sends an IN token on the control channel
following the notification. 

Only one control message can be sent per instance at a time. 
Meaning, from the moment cdc_send_encap_response was called, and until the 
hsu_al_ecm_encap_resp_comp_cb was called, additional calls to 
cdc_send_encap_response() will fail!
therefore, additional calls to hsu_al_ecm_control_transmit cause the DSM chain
to be queued to the control TX WM Q.

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
None.

SIDE EFFECTS  
None. 
==============================================================================*/
void hsu_al_ecm_control_transmit
( 
 sio_stream_id_type  stream_id, 
 dsm_item_type *     tx_ptr
 )
{
#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
  uint32 i = 0;
#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  cdc_ctx_t *ctx = NULL;
  core_t    *core;
  hsu_bus_mode_enum current_bus_mode;

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  jresult_t jresult;
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  HSU_ASSERT(tx_ptr != NULL); /* as done in usbdc_control_write() */

  hsu_chk_ist_crit_failure();

  jsafe_enter();
  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL || context_ptr->handle == NULL)
  {
    HSU_ERR_FATAL("*_control_transmit(): Unknown stream_id contex_ptr (%u)", 
      context_ptr, 0, 0);
    dsm_free_packet(&tx_ptr);
  }

#ifdef FEATURE_HS_USB_MBIM
  if (!hsu_conf_sel_is_diag_over_mbim())
#endif /* FEATURE_HS_USB_MBIM */    
  {
    HSU_MSG_HIGH_2("*_control_transmit(): Enter. tx_ptr=0x%x (handle=%d)", 
      tx_ptr, context_ptr->handle);
  }

#ifdef FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
    if (context_ptr == &ecm_instance_info[i])
    {
      tx_ptr->app_field = context_ptr->usb_ecm_packet_index;
      tx_ptr->app_ptr = (void*)i;

      /* Increment index RESPONSE AVAILABLE */
      ++context_ptr->usb_ecm_packet_index;
    }
  }

#endif /* FEATURE_HS_USB_ECM_CONTROL_SEQ_NUM */

  if(context_ptr->handle)
  {
    ctx = (cdc_ctx_t*)(context_ptr->handle);
    HSU_ULOG_1(NO_MSG, (hsu_ulog_log_type_enum)ctx->desc->interfaces->number, "app_field %u", tx_ptr->app_field);
  }

  if (ctx && (ctx->info.basic.flags & CDC_INIT_DPL))
  {
    core = (core_t *)(ctx->core_ctx);

    // always drop TX packets if DPL
    hsu_event_log(EVT_DPL_DROP_TX_CTRL_PKTS, core->core_number);
    dsm_free_packet(&tx_ptr);
    jsafe_leave();
    return;
  }

  if(hsu_conf_sel_ignore_dtr_drop() && 
    (context_ptr->instance_is_enabled == FALSE))
  {
    HSU_MSG_HIGH_1("hsu_al_ecm_data_transmit:dsm_free_packet 0x%x in ignore_disconnect", tx_ptr);

    /* If the instance is not enabled, drop the packet */
    dsm_free_packet(&tx_ptr);
    jsafe_leave();
    return;
  }

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  if (context_ptr->ignore_disconnect)
  {
    if(!context_ptr->is_core_enabled)
    {
      if (context_ptr->debug_mode)
      {
        HSU_MSG_HIGH_1("hsu_al_ecm_data_transmit:dsm_enqueue 0x%x in ignore_disconnect", tx_ptr);
      }

      /* Enqueue the received DSM chain */
      dsm_enqueue(context_ptr->control_info.tx_queue_ptr, &tx_ptr);
      jsafe_leave();
      return;
    }
  }

  /* check if an active NOTIFY_RESPONSE_AVAILABLE and GET_ENCAPSULATED_RESPONSE
       is still in progress */
  if (cdc_get_encapsulated_response_state(context_ptr->handle) != CDC_ENCAP_READY)
  {
    dsm_enqueue(context_ptr->control_info.tx_queue_ptr,&tx_ptr);

    if (context_ptr->instance_is_enabled)
    {
      /* 
      ** There is high chance that the host may have missed the previous
      ** notificaion.  Resending notification reminder to recover from
      ** QMI TX (CONTROL IN) data stall.   This issue was only reproduced in
      ** HS-USB testing over various PCs.  This workaround is not applied to
      ** HSIC because HSIC host used for chip-to-chip should reliably handle 
      ** notification.  Another reason for not enabling for HSIC, is to reduce
      ** extra notification when there are many QMI indications in a burst.
      */
      if (context_ptr->handle)
      {
        ctx = (cdc_ctx_t*)(context_ptr->handle);
        core = (core_t *)(ctx->core_ctx);

        if (core->core_number == HSU_CORE_CONFIG_USB_IDX)
        {
          context_ptr->statistics.tx_control_resent_notification_cnt++;

          jresult = cdc_resend_response_available(context_ptr->handle);

          if (jresult)
          {
            HSU_MSG_ERROR_2("hsu_al_ecm_data_transmit: resending response available "
              "failed: instance = %u, jresult = %i", context_ptr, jresult);
          }
          else
          {
            HSU_MSG_MED_1("hsu_al_ecm_data_transmit: resending response available "
              "success: instance = %u", context_ptr);
          }
        }
      }
    }
    
    jsafe_leave();
    return;
  }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
  {
    dsm_free_packet(&tx_ptr);

    HSU_ERR_FATAL(
      "*_control_transmit(): Instance is not opened (state=%d, handle=%d)",
      context_ptr->state, context_ptr->handle, 0);
  }

  /* 
  * if the disabled CB was invoked, there's no point in sending data to the 
  * host, since we're not connected. 
  */
  if(context_ptr->instance_is_enabled == FALSE)
  {
    /* Disabled CB was called. This should not cause an assert, since it is a 
    * common scenario - 
    * Following the call to the Application's dtr_cb, a QMI message is sent to 
    * notify the host that the packet data session was ended. But the host is 
    * not connected...
    */
    HSU_MSG_MED_2("*_control_transmit: Instance is disabled (handle=%d). "
      "Discarding message 0x%x.", context_ptr->handle, tx_ptr);
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
    context_ptr->statistics.tx_control_dropped_packet_inst_disabled_cnt++;
    dsm_enqueue(context_ptr->control_info.tx_queue_ptr, &tx_ptr);
#else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    dsm_free_packet(&tx_ptr);
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    jsafe_leave();
    return;
  }

  core = (core_t *)(ctx->core_ctx);
  HSU_ASSERT(core);  
  current_bus_mode = hsu_get_current_bus_mode((uint32)core->core_number);
  
  if (current_bus_mode == HSU_BUS_MODE_SUSPENDED)
  {
    HSU_ULOG_1(NO_MSG, BUS_LOG, "hsu_bus_mode_suspend, context_handle 0x%08X", (uint32)(context_ptr->handle));
  }
  /* If there's already a control transmit in progress,*/ 
  /* enqueue the message to the control TX WM Q        */
  /*****************************************************/
  if((context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes != 0) 
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
    || context_ptr->control_info.pending_dsm_ptr
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    )
  {
    /* There's already a control transmit in progress */
    /* Enqueue DSM chain to Control RX WMQ */
    HSU_MSG_MED(
      "*_control_transmit: CTRL TX already in progress, enqueuing to WMQ");
    HSU_ASSERT(context_ptr->control_info.tx_queue_ptr != NULL);
    dsm_enqueue(context_ptr->control_info.tx_queue_ptr, &tx_ptr);

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
    if ((context_ptr->control_info.need_resending_pending_dsm)
     && (cdc_get_encapsulated_response_state(context_ptr->handle)
          == CDC_ENCAP_READY))
    {
      /* If there is pending packet with pipe in ready state, 
      ** trigger resending control packet to send the data 
      */
      hsu_al_task_enqueue_cmd(
        hsu_al_ecm_transmit_pending_control_pkt, 
        context_ptr);
    }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

    jsafe_leave();
    return;
  }

  context_ptr->control_info.write_buffer_data.ctrl_tx_num_of_bytes = 
    dsm_length_packet(tx_ptr);

  perform_control_transmit(context_ptr, tx_ptr);
  jsafe_leave();
} /* hsu_al_ecm_control_transmit */


/*===========================================================================

FUNCTION hsu_al_ecm_write

DESCRIPTION
This function will invoke cdc_write_multiple in order to send one Ethernet
packet, either in a single DSM item or in a contiguous buffer

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_ecm_write
(
  hsu_al_ecm_instance_info_type* context_ptr,
  rb_t *tx_ptr,
  uint8* buf_ptr,
  uint32 buf_size,
  boolean contiguous_buf_used
)
{
  jresult_t res;
  ecm_data_tx_info_type* tx_info_ptr = &context_ptr->data_info.tx_info;

  hsu_trace_iface_set(NET1_HSU_IFACE);
  hsu_trace_msg2_set(HSU_AL_ECM_WRITE__TRC);

  ++context_ptr->statistics.tx_pkt_cnt;

  HSU_ULOG_1(NO_MSG, DATA_LOG, "pkt_cnt %u ",context_ptr->statistics.tx_pkt_cnt);

  res = cdc_write_multiple(context_ptr->handle,
    (void**)&buf_ptr, &buf_size, 1, HS_USB_ECM_TX_TIMEOUT_MS);

  hsu_trace_iface_clear();
  hsu_trace_msg2_clear();

  switch(res)
  {
  case HSU_JSUCCESS:
    if (context_ptr->debug_mode)
    {
      HSU_MSG_MED_3("cdc_write_multiple called with %d bytes. buf[0]=%x, buf[1]=%x", 
        buf_size, buf_ptr[0], buf_ptr[1]);
    }
    
  #ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET       
    q_put(tx_info_ptr->tx_pending_queue_ptr, &tx_ptr->link);
  #else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    q_put(&tx_info_ptr->tx_queue_ptr, &tx_ptr->link);
  #endif /* !FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
    ++tx_info_ptr->pending_chain_len;

    HSU_ULOG_1(NO_MSG, DATA_LOG, "pending_len %u ",tx_info_ptr->pending_chain_len);

    if (tx_info_ptr->pending_chain_len > context_ptr->statistics.tx_max_num_pending)
    {
      context_ptr->statistics.tx_max_num_pending = tx_info_ptr->pending_chain_len;
    }
    break;

  case ESUSPENDED:
  /* FEATURE_HS_USB_ECM_PKT_DROP_ON_ESUSPENDED will make ECM to drop packets on 
  ** ESUSPEND case where bus is in SUSPEND and REMOTE WAKEUP is not enabled. 
  ** This is to prevent USB SW indefinately trying to send the packet when the 
  ** device have no capability to driver REMOTE WAKEUP to the Host.
  */
  #if defined(FEATURE_HS_USB_RECOVER_DROPPED_PACKET) && !defined(FEATURE_HS_USB_ECM_PKT_DROP_ON_ESUSPENDED)
    tx_ptr->app_ptr = NULL;
    q_put(tx_info_ptr->tx_resending_queue_ptr, &tx_ptr->link);
  #else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET && !FEATURE_HS_USB_ECM_PKT_DROP_ON_ESUSPENDED */
    HSU_MSG_HIGH_3("hsu_al_ecm_write: cdc_write_multiple returned ESUSPENDED, dropping packet", 0, 0, 0);
    
    free_single_rb(tx_ptr, FALSE);
	if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
	{
	  context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, tx_ptr->app_ptr);
	}
	else
	{
	  context_ptr->data_info.tx_done_ptr_std(tx_ptr->app_ptr);
	}
    context_ptr->statistics.tx_dropped_packet_cnt++;
  #endif /* !FEATURE_HS_USB_RECOVER_DROPPED_PACKET & !FEATURE_HS_USB_ECM_PKT_DROP_ON_ESUSPENDED */
    break;

  default:
    HSU_MSG_ERROR_3("hsu_al_ecm_write: cdc_write_multiple returned %d", res, 0, 0);
    HSU_ASSERT((uint32)"hsu_al_ecm_write: unexpected result from cdc_write_multiple" == NULL);

    context_ptr->statistics.tx_dropped_packet_cnt++;
    free_single_rb(tx_ptr, TRUE);

    if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
	{
	   context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, tx_ptr->app_ptr);
	}
	else
	{
	  context_ptr->data_info.tx_done_ptr_std(tx_ptr->app_ptr);
	}

    break;
  }

  res = cdc_handle_tx_completion(context_ptr->handle);
  if (res != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR_1("cdc_handle_tx_completion failed. res=%d", res);
  }

}


/*===========================================================================

FUNCTION hsu_al_ecm_send_tx_buffer

DESCRIPTION
Sends a TX buffer (contiguous buffer or a single DSM item).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
static void hsu_al_ecm_send_tx_buffer
(
  hsu_al_ecm_instance_info_type * context_ptr,
  rb_t *tx_ptr,
  uint8* contiguous_buf_ptr
)
{
  uint32 bytes_to_send;
  uint8* buf_ptr;

  HSU_ASSERT(context_ptr);

  if (tx_ptr == NULL)
  {
    HSU_MSG_ERROR_3("_ecm_send_tx_buffer: NULL chain_to_gather_ptr", 0, 0, 0); 
    return;
  }  

  bytes_to_send = tx_ptr->used;

  /* HS-USB should send only one Ethernet frame per USB transfer. 
        HS-USB expects to receive only one Ethernet frame from RmNET at a time. */ 
  if((bytes_to_send > HSU_ECM_RMNET_DL_MAX_PKT_LENGTH) || (bytes_to_send == 0))
  {
    HSU_MSG_ERROR_3("_ecm_send_tx_buffer: packet too large or zero (%d). dropping packet",
      bytes_to_send, 0, 0); 
    context_ptr->statistics.tx_dropped_packet_cnt++;
    free_single_rb(tx_ptr, FALSE);
	HSU_ULOG_1(NO_MSG, DATA_LOG, "dropping the packet, Len = %u", bytes_to_send);
    if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0)
    {
	  context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, tx_ptr->app_ptr);
    }
	else
	{
	  context_ptr->data_info.tx_done_ptr_std(tx_ptr->app_ptr);
	}
	
    return;
  }

  buf_ptr = (void*) tx_ptr->buffer_va;

  context_ptr->statistics.tx_total_bytes += bytes_to_send;
  
  if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) != 0)
  {
    hsu_dcache_clean_region(buf_ptr, bytes_to_send);
  }
  
  buf_ptr = (void*) tx_ptr->buffer_pa;

  if(buf_ptr == NULL)
  {
    HSU_ULOG(ERROR_MSG, DBG_E_LOG, "Error: Addr passed by QTI layer is not mapped");
	free_single_rb(tx_ptr, FALSE);
	context_ptr->data_info.tx_done_ptr_std(tx_ptr->app_ptr);
	return;
  }

  HSU_ULOG_1(NO_MSG, DATA_LOG, "rb 0x%X ",(uint32)tx_ptr);

  hsu_al_ecm_write(context_ptr, tx_ptr, buf_ptr, bytes_to_send, FALSE);
} /* hsu_al_ecm_send_tx_buffer */

void hsu_al_ecm_data_transmit(sio_stream_id_type stream_id,sio_port_id_type port_id,dsm_item_type * tx_ptr)
{

}

sio_status_type hsu_al_ecm_data_open(sio_open_type * open_ptr)
{
    return SIO_DONE_S;
}


/*===========================================================================
FUNCTION hsu_al_ecm_set_handle_complete_timer

DESCRIPTION
This function sets the handle complete timer when DL buffer count
pending with the HW reaches the max value

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_al_ecm_set_handle_complete_timer
(
  hsu_al_ecm_instance_info_type * context_ptr
)
{
  ecm_data_tx_info_type* tx_info_ptr = &context_ptr->data_info.tx_info;
  HSU_ASSERT(tx_info_ptr != NULL);

  /* Set the handle complete timer */
  if ((tx_info_ptr->tx_timeout == 0)  &&
      (tx_info_ptr->tx_handle_comp_scheduled == FALSE))
  {
    hsu_timer_set(
      &tx_info_ptr->tx_handle_complete_timer,
      HS_USB_ECM_TX_HANDLE_COMPLETE_TIMER_MS,
      T_MSEC);

    tx_info_ptr->tx_handle_comp_scheduled = TRUE;
  }
} /* hsu_al_ecm_set_handle_complete_timer */


/*==============================================================================

FUNCTION                  HSU_AL_ECM_DATA_TRANSMIT 

DESCRIPTION
Called by the application, when it wishes to send data on the bulk end point.
The data (Ethernet frame) is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer using the DMOV, 
and then gives it to the cdc_write() function of the USB stack. 

DEPENDENCIES
ecm_init() and hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have 
been called for this instance.

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
void hsu_al_ecm_tx_data
( 
  bam_dmux_handle   handle,
  void            *priv,
  void            *rx_priv,
  void            *data,
  uint16          data_len
)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  cdc_ctx_t *ctx;
  core_t    *core;
  rb_t      *tx_rb = NULL;

//hsu_bus_mode_enum current_bus_mode;
//  boolean res;

  if (data == NULL)
  {
    HSU_ULOG(NO_MSG, DATA_LOG, "tx buffer is NULL");
    return;
  }

  hsu_chk_ist_crit_failure();

  jsafe_enter();

  context_ptr = (hsu_al_ecm_instance_info_type *) priv;
  if(context_ptr == NULL)
  {
    HSU_ERR_FATAL(
      "hsu_al_ecm_tx_data: Unknown context (%u)", context_ptr, 0, 0);
    jsafe_leave();
    return;
  } 

  if(context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
  {
    HSU_ERR_FATAL(
      "hsu_al_ecm_data_transmit(): Instance state=%d (expected opened) (handle=%d)",
      context_ptr->state, context_ptr->handle, 0);
    context_ptr->statistics.tx_dropped_packet_cnt++;
    jsafe_leave();
    return;
  }

  /* Get the free rb from the rb pool for the DL data handling */
  tx_rb = get_free_rb(context_ptr->data_info.tx_info.tx_chain, ECM_MAX_NUM_RB_ITEMS_FOR_WRITE);
  if(tx_rb == NULL)
  {
    /* Ran out of buffers. Most likely Host is not pulling data, drop the packet until Host recovers
	   and start the Tx complete timer */
    if((context_ptr->statistics.tx_dropped_packet_cnt % ECM_MAX_NUM_RB_ITEMS_FOR_WRITE) == 0)
    {         
       HSU_ULOG(NO_MSG, DATA_LOG, "DL No free rb, dropping packet ");
    } 

   if ( (((cdc_ctx_t*)context_ptr->handle) != NULL ) &&
       ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM) == 0))
    {
      context_ptr->data_info.tx_done_ptr(context_ptr->data_info.dmux_handle, rx_priv);
    }
    else
    {
      context_ptr->data_info.tx_done_ptr_std(rx_priv);
    }	
    context_ptr->statistics.tx_dropped_packet_cnt++;
    hsu_al_ecm_set_handle_complete_timer(context_ptr);
    jsafe_leave();
    return;
  }

  tx_rb->buffer_va = data;
  tx_rb->buffer_pa = (void*)(qurt_lookup_physaddr((qurt_addr_t)data));
  tx_rb->free   = FALSE;
  tx_rb->used = data_len;
  tx_rb->app_ptr = rx_priv;

  //q_put(context_ptr->data_info.tx_info.tx_queue_ptr, &tx_&rb->link)

  HSU_ULOG_2(NO_MSG, DATA_LOG, "rb to send 0x%X, len %u", (uint32)tx_rb,(uint16)data_len);

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
  if (context_ptr->ignore_disconnect)
  {
    if(!context_ptr->is_core_enabled)
    {
      if (context_ptr->debug_mode)
      {
        HSU_MSG_HIGH_3("hsu_al_ecm_data_transmit:dsm_enqueue 0x%x in ignore_disconnect", tx_ptr, 0, 0);
      }

      /* Enqueue the received DSM chain */
      q_put(context_ptr->data_info.tx_info.tx_queue_ptr, &tx_rb->link);
      jsafe_leave();
      return;
    }
  }
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

   /* 
    * if the disabled CB was invoked, there's no point in sending data to the 
    * host, since we're not connected. 
    */
    if(context_ptr->instance_is_enabled == FALSE)
  {
#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
      /* The instance may be disabled during core restart up to re-enumeation
       * Enqueue the received packet, so it can be resent on 
       * hsu_al_ecm_enabled_cb() */
    q_put(context_ptr->data_info.tx_info.tx_queue_ptr, &tx_rb->link);    
#else /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
      HSU_MSG_MED_3(
        "hsu_al_ecm_data_transmit: Instance is disabled (handle=%d). Discarding frame 0x%x.",
        context_ptr->handle, tx_ptr, 0);
      context_ptr->statistics.tx_dropped_packet_cnt++;
    
    //TODO: call Tx done for dmux  
    free_single_rb(tx_rb, FALSE);
     
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */
      jsafe_leave();
      return;
    }

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
#if 0
    if(q_cnt(context_ptr->data_info.tx_info.tx_resending_queue_ptr) != 0)
    {
      if (context_ptr->debug_mode)
      {
        HSU_MSG_HIGH_3("hsu_al_ecm_data_transmit: tx is pending : dsm_enqueue 0x%x", tx_rb, 0, 0);
      }
      /* Enqueue the received DSM chain */
      q_put(context_ptr->data_info.tx_info.tx_queue_ptr, &tx_rb->link);
      jsafe_leave();
      return;
   }
#endif    
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */

  ctx = (cdc_ctx_t*)(context_ptr->handle);
  
  core = (core_t *)(ctx->core_ctx);
  HSU_ASSERT(core);  
  //current_bus_mode = hsu_get_current_bus_mode((uint32)core->core_number);
#if 0
  /* If we have something else to send, it means that there are no resources
  for the received packet - enqueue the received DSM chain 
  (handle_comp_timer should be active already ??) */
  if (q_cnt(context_ptr->data_info.tx_info.tx_queue_ptr) > HS_USB_ECM_TX_MAX_PENDING_BUFFERS )
  {
    if (context_ptr->debug_mode)
  {
      HSU_MSG_HIGH_3("hsu_al_ecm_data_transmit: enqueue 0x%x", tx_rb, 0, 0);
    }

    /* Enqueue the received DSM chain */
    q_put(context_ptr->data_info.tx_info.tx_queue_ptr, &tx_rb->link);
    jsafe_leave();
    return;
  }
#endif
  /* This function is responsible for releasing the contiguous buffer memory */
  hsu_al_ecm_send_tx_buffer(context_ptr, tx_rb, NULL);

  jsafe_leave();
} /* hsu_al_ecm_data_transmit */


/*==============================================================================

FUNCTION      ENABLE_INBOUND_FLOW

DESCRIPTION   
This function performs the required operations for enabling inbound flow:
- Marks the flag 'inbound_flow_is_enabled' accordingly
- call hsu_al_ecm_rx_done_internal() to forward any waiting 
  buffers to the application and provide new buffers to the HW if needed.

Note: It is called from the application's context 

DEPENDENCIES  
Should be called when Giant mutex is locked

RETURN VALUE  
None.

SIDE EFFECTS  
In previous design, Connection Speed Changed notification was sent as part of 
enable/disable inbound flow. This is not done anymore.

==============================================================================*/
static void enable_inbound_flow
(
  hsu_al_ecm_instance_info_type * context_ptr
)
{
  ecm_data_rx_info_type* rx_info_ptr = &context_ptr->data_info.rx_info;

  HSU_ULOG_1(NO_MSG, DATA_LOG, " flow_status %u ",rx_info_ptr->inbound_flow_is_enabled);

#ifdef FEATURE_HS_USB_ECM_BAM
  if (context_ptr->bam_info.is_bam_mode)
  {
    HSU_MSG_ERROR("enable_inbound_flow: called in BAM mode");
    return;
  }
#endif /* FEATURE_HS_USB_ECM_BAM */

  if (rx_info_ptr->inbound_flow_is_enabled == TRUE)
  {
    HSU_MSG_LOW_3(
      "enable_inbound_flow: inbound flow already enabled", 0, 0, 0);
    return;
  }      

  HSU_MSG_HIGH_3("enable_inbound_flow: enter",0,0,0);

  rx_info_ptr->inbound_flow_is_enabled = TRUE;

  if (rx_info_ptr->num_buf_waiting != 0)
  {
    HSU_MSG_HIGH_3("enable_inbound_flow: forwarding %d waiting buffers",
      rx_info_ptr->num_buf_waiting, 0, 0);

    hsu_al_ecm_rx_done_internal(
      &rx_info_ptr->buf_status_array[rx_info_ptr->num_buf_already_forwarded],
      (void**)&rx_info_ptr->buf_array[rx_info_ptr->num_buf_already_forwarded],
      &rx_info_ptr->buf_size_array[rx_info_ptr->num_buf_already_forwarded],
      rx_info_ptr->num_buf_waiting,
      (cdc_appctx_t)context_ptr, TRUE);

    rx_info_ptr->num_buf_waiting = 0;
  }
  else  /* There are no buffers waiting to be sent to the application.
           Check if we need to issue cdc_read.
           This can occur in the following scenario:
           1. Suspend occurs
           2. Application disables inbound flow
           3. Resume occurs (remote wakeup to send the disable inbound flow)
           4. cdc_read is not issued in resume because flow is disabled
           5. Application enables inbound flow
         */
  {
    if ((context_ptr->instance_is_enabled))     
    {
      HSU_ULOG(NO_MSG, DATA_LOG, " setup UL ");
      hsu_al_ecm_setup_read(context_ptr);
    }
  }


  HSU_MSG_HIGH("enable_inbound_flow: exit");
} /* enable_inbound_flow */

/*==============================================================================

FUNCTION      DISABLE_INBOUND_FLOW

DESCRIPTION   
This function performs the required operations for disabling inbound flow:
- Marks the flag 'inbound_flow_is_enabled' to FALSE

Note: It is called from the application's context

DEPENDENCIES  
Should be called when Giant mutex is locked

RETURN VALUE  
None.

SIDE EFFECTS  
In previous design, Connection Speed Changed notification was sent as part of 
enable/disable inbound flow. This is not done anymore.

==============================================================================*/
static void disable_inbound_flow
(
  hsu_al_ecm_instance_info_type * context_ptr
)
{
#ifdef FEATURE_HS_USB_ECM_BAM
  if (context_ptr->bam_info.is_bam_mode)
  {
    HSU_MSG_ERROR("disable_inbound_flow: called in BAM mode");
    return;
  }
#endif /* FEATURE_HS_USB_ECM_BAM */


  HSU_MSG_HIGH("disable_inbound_flow: exit");
} /* disable_inbound_flow */

#ifdef FEATURE_HS_USB_STANDARD_ECM
/*==============================================================================

FUNCTION      REMEMBER_CONN_SPEED_NOTIF

DESCRIPTION   
This function checks whether we need to remember the current connection
speed notification and to resend it when possible.


DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.
==============================================================================*/
static void remember_conn_speed_notif
(
 hsu_al_ecm_instance_info_type * context_ptr
)
{
  if (((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM )
  {
    context_ptr->data_info.conn_speed_mem.resend = TRUE;
    context_ptr->data_info.conn_speed_mem.connection_speed_args = 
      context_ptr->data_info.connection_speed_args;
  }
  else
  {
    /* If there's already a message "in memory", that is the opposite of this one, 
    forget them both */
    if ((context_ptr->data_info.conn_speed_mem.resend == TRUE) &&
      (context_ptr->data_info.conn_speed_mem.connection_speed_args.downstream_bitrate != 
      context_ptr->data_info.connection_speed_args.downstream_bitrate))
    {
      HSU_MSG_LOW_2("remember_conn_speed_notif: canceling the remembered notif. "
        "remembered_speed=%d, new_speed=%d",
        context_ptr->data_info.conn_speed_mem.connection_speed_args.downstream_bitrate,
        context_ptr->data_info.connection_speed_args.downstream_bitrate);
      context_ptr->data_info.conn_speed_mem.resend = FALSE;
    }
    else /* We need to resend the msg */
    {
      HSU_MSG_LOW_1("remember_conn_speed_notif: remembering notification to send: new_speed=%d",
        context_ptr->data_info.connection_speed_args.downstream_bitrate);
      context_ptr->data_info.conn_speed_mem.resend = TRUE;
      context_ptr->data_info.conn_speed_mem.connection_speed_args = 
        context_ptr->data_info.connection_speed_args;
    }
  }
}
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

void 
hsu_al_ecm_dmux_event_notify(
    bam_dmux_handle     handle,
    void                *priv,
    bam_dmux_event      event,
    bam_dmux_event_data *event_data
)
{
   hsu_al_ecm_instance_info_type *ecm_ctx = (hsu_al_ecm_instance_info_type *) priv; 

   if(handle == NULL)
   {
     HSU_MSG_ERROR("hsu_al_ecm_dmux_event_notify(): invalid Dmux Handle");
     return;
   }

   if(ecm_ctx == NULL)
   {
     HSU_MSG_ERROR("hsu_al_ecm_dmux_event_notify(): invalid Ctx");
     return;
   }
   
   jsafe_enter();
   
   switch(event)
   {
     case BAM_DMUX_EVENT_OPEN:
       {
         ecm_ctx->data_info.channel_opened = TRUE;
         ecm_ctx->data_info.rx_info.inbound_flow_is_enabled = TRUE;

         /* By the time open is ACKed by Dmux, interface should have been enabled
                     then we queue the Rx buffer here, otherwise its queued when interface is enabled          
                 */ 
         if(TRUE == ecm_ctx->instance_is_enabled)
         {
           hsu_al_ecm_setup_read(ecm_ctx);
         }  
         break;
       }
     case BAM_DMUX_EVENT_TX_RESUME:
       {
          /* UL Flow might have been disabled by the client and is trying to enable again */
          enable_inbound_flow(ecm_ctx);
       }
     default:
          break;           
   }
   jsafe_leave();
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONTROL_IOCTL

DESCRIPTION   
This function is used to control various features of the USB function.  
The behavior of this function depends on the 'cmd' issued.
Currently this is just wrapper which will call hsu_al_ecm_ioctl().

DEPENDENCIES  
The instance must be in the OPEN state, meaning the ecm_init(), 
hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have been called for
this instance.
When the command is SIO_IOCTL_ENABLE_DTR_EVENT - 'param' cannot be NULL.

RETURN VALUE  
None.

SIDE EFFECTS  
Depends on the command issued.
==============================================================================*/
void hsu_al_ecm_control_ioctl
(
  sio_stream_id_type      stream_id,
  sio_port_id_type        port_id,
  sio_ioctl_cmd_type      cmd,
  sio_ioctl_param_type *  param
)
{ 
  HSU_MSG_MED_3("hsu_al_ecm_control_ioctl(): stream_id (%u), port_id (%u), cmd (%u)",
    stream_id, port_id, cmd);

  hsu_al_ecm_ioctl(stream_id, port_id, cmd, param);
}


/*==============================================================================

FUNCTION      HSU_AL_ECM_IOCTL

DESCRIPTION   
This function is used to control various features of the USB function.  
The behavior of this function depends on the 'cmd' issued.

DEPENDENCIES  
The instance must be in the OPEN state, meaning the ecm_init(), 
hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have been called for
this instance.
When the command is SIO_IOCTL_ENABLE_DTR_EVENT - 'param' cannot be NULL.

RETURN VALUE  
None.

SIDE EFFECTS  
Depends on the command issued.
==============================================================================*/
void hsu_al_ecm_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;
  boolean dte_ready;
#ifdef FEATURE_HS_USB_STANDARD_ECM
  jresult_t jresult;
#endif /*FEATURE_HS_USB_STANDARD_ECM*/

  HSU_USE_PARAM(port_id);
  HSU_PARAM_COULD_BE_CONST(param);

  hsu_chk_ist_crit_failure();

  jsafe_enter();
  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ERR_FATAL("hsu_al_ecm_ioctl(): Unknown stream_id (%u)", 
      context_ptr, 0, 0);
    return;
  }

  if((context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED) &&
    (context_ptr->state != HSU_ECM_INSTANCE_STATE_HALF_OPENED))
  {
    HSU_ERR_FATAL("hsu_al_ecm_ioctl(): Called while instance's state=%d, not "
      "OPENED/HALF_OPENED (handle=%d)",
      context_ptr->state, context_ptr->handle, 0);
    return;
  }

  switch(cmd)
  {
  case SIO_IOCTL_ENABLE_DTR_EVENT:
  case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
    {
      HSU_MSG_MED_1("hsu_al_ecm_ioctl(): got SIO_IOCTL_ENABLE_DTR_EVENT_EXT (handle=%d)",
        context_ptr->handle);
      /* Register callback which informs the caller of change in the state of 
      of the device (enabled/disabled). 
      The callback function must act quickly since it is runs in the DPC 
      context (perhaps simply issue a signal). */
      HSU_ASSERT(param != NULL);
      context_ptr->app_callbacks.enable_disable_cb_ext = 
        (*(sio_ioctl_enable_cb_event_ext_type *)param).cb_func;
      context_ptr->app_callbacks.enable_disable_ext_user_data = 
        (*(sio_ioctl_enable_cb_event_ext_type *)param).cb_data;
      invoke_app_enable_disable_cbs(context_ptr);
      break;
    }

  case SIO_IOCTL_DISABLE_DTR_EVENT:
  case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
    {
      HSU_MSG_MED_1("hsu_al_ecm_ioctl(): got SIO_IOCTL_DISABLE_DTR_EVENT_EXT (handle=%d)",
        context_ptr->handle);
      /* Unregister call-back for change in state of data terminal ready indication */
      context_ptr->app_callbacks.enable_disable_cb_ext = NULL;
      context_ptr->app_callbacks.enable_disable_ext_user_data = NULL;
      break;
    }
  case SIO_IOCTL_DTE_READY_ASSERTED:
    {
      core_t *core = ((cdc_ctx_t*)context_ptr->handle)->core_ctx;
      if (((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & (CDC_INIT_STD_ECM | CDC_INIT_INTR_ONLY_ECM_PIPE | CDC_INIT_DPL))
      {
        dte_ready = context_ptr->instance_is_enabled;
      }
      else 
      {
        dte_ready = context_ptr->ctrl_line_state; 
      }

      hsu_event_log(dte_ready ? EVT_HIGH_DTE_READY : EVT_LOW_DTE_READY, core->core_index);
     
      if (dte_ready)
      {
        HSU_ULOG_1(MED_MSG, DATA_LOG, "high_dte_ready, ctx_ptr 0x%08X", (uint32)context_ptr);
      }
      else
      {
        HSU_ULOG_1(MED_MSG, DATA_LOG, "low_dte_ready, ctx_ptr 0x%08X", (uint32)context_ptr);
      }
      
      /* Retrieve the state of the DTR signal (1 if the instance is resumed, 
      0 if it is suspended */
      HSU_ASSERT(param != NULL);
      *(((sio_ioctl_param_type *)param)->dte_ready_asserted) = dte_ready;
      break;
    }
  case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    {
      /* 
      * Should allow OUT transfers to arrive and be queued to the application's
      * data RX WM Q. This is done in the application context. 
      * The current behavior of the RmNet application is as described below, 
      * and might change in the future.
      * Called by the application in the following cases:
      * - After hsu_al_ecm_control_open() and hsu_al_ecm_data_open() were called 
      * - When the data RX WM Q low WM threshold was reached (currently at 1514 
      *   bytes)
      * - When a previously active packet data session has disconnected
      *  In order to prevent deadlocks we assume that the application should call
      *  this IOCTL when inbound flow was previosly disabled!
      */

      if(context_ptr->port_id == SIO_PORT_USB_IPCR)
      {
        HSU_MSG_ERROR(
          "IOCTL - INBOUND_FLOW_ENABLE not supported for IPCR port.");
      }
      else
      {
        enable_inbound_flow(context_ptr);
      }
      break;
    }
  case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    {
      /* 
      * Called when the data RX WM Q high WM threshold is reached.This is done in 
      * the DMOV context (interrupt context), from the hsu_al_ecm_dmov_scatter_comp_cb(). 
      * Should prevent additional OUT transfers, until hsu_al_ecm_ioctl() is 
      * called with the command SIO_IOCTL_INBOUND_FLOW_ENABLE.
      */

      if(context_ptr->port_id == SIO_PORT_USB_IPCR)
      {
        HSU_MSG_ERROR(
          "IOCTL - INBOUND_FLOW_DISABLE not supported for IPCR port.");
      }
      else
      {
        disable_inbound_flow(context_ptr);
      }
      break;
    }
  case SIO_IOCTL_SET_FLOW_CTL:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_SET_FLOW_CTL");
      HSU_ASSERT(param != NULL);
      if(((sio_ioctl_param_type *)param)->flow_ctl.rx_flow != SIO_FCTL_BEST)
      {
        HSU_ERR_FATAL("hsu_al_ecm_ioctl(): SIO_IOCTL_SET_FLOW_CTL with "
          "rx_flow != SIO_FCTL_BEST", 0, 0, 0);
      }
      if(((sio_ioctl_param_type *)param)->flow_ctl.tx_flow != SIO_FCTL_BEST)
      {
        HSU_ERR_FATAL("hsu_al_ecm_ioctl(): SIO_IOCTL_SET_FLOW_CTL with "
          "tx_flow != SIO_FCTL_BEST", 0, 0, 0);
      }
      break;
    }
  case SIO_IOCTL_GET_FLOW_CTL:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_GET_FLOW_CTL");
      HSU_ASSERT(param != NULL);
      ((sio_ioctl_param_type *)param)->flow_ctl.tx_flow = SIO_FCTL_BEST;
      ((sio_ioctl_param_type *)param)->flow_ctl.rx_flow = SIO_FCTL_BEST;
      break;
    }
  case SIO_IOCTL_GET_EP_INFO:
    {
      HSU_MSG_MED("hsu_al_ecm_ioctl(): got SIO_IOCTL_GET_EP_INFO");
      HSU_ASSERT(param != NULL);
      ((sio_peripheral_ep_info *)param)->ep_type = DATA_EP_TYPE_BAM_DMUX;
      ((sio_peripheral_ep_info *)param)->peripheral_iface_id = USB_DATA_CHANNEL;
      break;
    }
#ifdef FEATURE_HS_USB_STANDARD_ECM
  case SIO_IOCTL_NETWORK_CONNECTION_NOTIF:
    {
      if (!(((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM ))
      {
        HSU_MSG_HIGH("hsu_al_ecm_ioctl(): SIO_IOCTL_NETWORK_CONNECTION_NOTIF shouldn't "
          "be called in non-standard ECM");
        break;
      }
      jresult = ecm_notify_network_connection(context_ptr->handle, param->link_up);
      if (jresult != HSU_JSUCCESS)
      {
        HSU_MSG_ERROR_3("hsu_al_ecm_ioctl(): ecm_notify_network_connection "
          "returned error %d. handle=%d, link_up = %d", jresult,
          context_ptr->handle, ((sio_ioctl_param_type *)param)->link_up);
      }
      break;
    }
  case SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF:
    {
      if (!(((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM ))
      {
        HSU_MSG_HIGH("hsu_al_ecm_ioctl(): SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF shouldn't "
          "be called in non-standard ECM");
        break;
      }
      context_ptr->data_info.connection_speed_args.upstream_bitrate = ((sio_ioctl_param_type *)param)->connection_speed_change_notif.upstream_bitrate;
      context_ptr->data_info.connection_speed_args.downstream_bitrate = ((sio_ioctl_param_type *)param)->connection_speed_change_notif.downstream_bitrate;
      jresult = ecm_notify_conn_speed_change(context_ptr->handle, &context_ptr->data_info.connection_speed_args);
      if (jresult != HSU_JSUCCESS)
      {
        HSU_MSG_ERROR_2("hsu_al_ecm_ioctl(): ecm_notify_conn_speed_change "
          "returned error %d. handle=%d", jresult, context_ptr->handle);
      }
      /* 4.2.2.	For reference look at the way SIO_IOCTL_INBOUND_FLOW_ENABLE is handled right now @TODO */
      break;
    }
  case SIO_IOCTL_GET_MAC_ADDRESS:
    {
      if ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM ) == 0)
      {
        HSU_MSG_HIGH("hsu_al_ecm_ioctl(): SIO_IOCTL_GET_MAC_ADDRESS shouldn't "
          "be called in non-standard ECM");
        break;
      }
      HSU_MSG_MED_1("hsu_al_ecm_ioctl(): got SIO_IOCTL_GET_MAC_ADDRESS (handle=%d)",
        context_ptr->handle);
	  MAC_ADDR_FOR_NETWORK (context_ptr->mac_address);
      if (sizeof(((sio_ioctl_param_type *)param)->mac_address) < sizeof(context_ptr->mac_address))
      {
        HSU_MSG_ERROR_1("hsu_al_ecm_ioctl(): Provided mac address size is too small = %d",
                      sizeof(((sio_ioctl_param_type *)param)->mac_address));
        memscpy(((sio_ioctl_param_type *)param)->mac_address,
          sizeof(((sio_ioctl_param_type *)param)->mac_address),
          context_ptr->mac_address,
          sizeof(((sio_ioctl_param_type *)param)->mac_address));
        break;
      }
      memscpy(((sio_ioctl_param_type *)param)->mac_address,
             sizeof(context_ptr->mac_address),
             context_ptr->mac_address,
             sizeof(context_ptr->mac_address)); 
      break ;
    }
  case SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT:
    {
      HSU_MSG_MED_1("hsu_al_ecm_ioctl(): got SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT (handle=%d)",
        context_ptr->handle);
      context_ptr->app_callbacks.set_packet_filter_cb = 
        ((sio_ioctl_param_type *)param)->set_packet_filter_event.cb_func;
      context_ptr->app_callbacks.set_packet_filter_user_data = 
        ((sio_ioctl_param_type *)param)->set_packet_filter_event.cb_data;
      break ;
    }
  case SIO_IOCTL_IS_STANDARD_ECM:
    {
      HSU_MSG_MED_1("hsu_al_ecm_ioctl(): got SIO_IOCTL_IS_STANDARD_ECM (is_standard =%d)",
        ((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM);
      ((sio_ioctl_param_type *)param)->is_standard_ecm = 
	    SEND_FLAG_AS_BOOLEAN ((((cdc_ctx_t*)context_ptr->handle)->info.basic.flags & CDC_INIT_STD_ECM));
      break;
    }
#endif /*FEATURE_HS_USB_STANDARD_ECM*/
  default:
    {
      HSU_MSG_ERROR_1("hsu_al_ecm_ioctl(): cmd %d not supported!", cmd);
    }
  } /* switch(cmd) */
  jsafe_leave();
} /* hsu_al_ecm_ioctl */

/*==============================================================================

FUNCTION                HSU_AL_ECM_DATA_FLUSH_TX 

DESCRIPTION
TODO ECM

DEPENDENCIES
TODO ECM

RETURN VALUE
None.

SIDE EFFECTS
TODO ECM

==============================================================================*/
void hsu_al_ecm_data_flush_tx
( 
 sio_stream_id_type  stream_id,             
 sio_port_id_type    port_id,
 void                (*flush_func_ptr)(void)  
 /* Function to call when transmission is complete. */
 )
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  jsafe_enter();
  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ERR_FATAL(
      "hsu_al_ecm_data_flush_tx(): Unknown stream_id (%u)", context_ptr, 0, 0);
    return;
  }

  HSU_USE_PARAM(flush_func_ptr);
  HSU_USE_PARAM(port_id);

  HSU_ERR_FATAL("hsu_al_ecm_data_flush_tx() called, but not yet implemented!",
    0, 0, 0);

  jsafe_leave();
} /* hsu_al_ecm_data_flush_tx */

/*==============================================================================

FUNCTION                    HSU_AL_ECM_CONTROL_CLOSE

DESCRIPTION
Called by the application to deallocate and un-configure the control plane for 
the network device.

DEPENDENCIES
ecm_init() must have been called for this instance.

RETURN VALUE
None.

SIDE EFFECTS
Empties the control plane watermark queues.

==============================================================================*/
void hsu_al_ecm_control_close
( 
  sio_stream_id_type  stream_id,
  void                (*close_func_ptr)(void)  
 /* Function to call when transmission is complete.   */
)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  HSU_USE_PARAM(close_func_ptr);
  HSU_MSG_HIGH("hsu_al_ecm_control_close(): Enter.");

  jsafe_enter();
  context_ptr = get_instance_info_from_stream_id(stream_id);

  if(context_ptr == NULL)
  {
    HSU_ERR_FATAL("hsu_al_ecm_control_close(): Unknown stream_id (%u)", 
      context_ptr, 0, 0);
    jsafe_leave();
    return;
  }

  context_ptr->control_info.stream_id = SIO_NO_STREAM_ID;
  if (context_ptr->control_info.rx_queue_ptr)
  {
    dsm_empty_queue(context_ptr->control_info.rx_queue_ptr);
    context_ptr->control_info.rx_queue_ptr = NULL;
  }

  if (context_ptr->control_info.tx_queue_ptr)
  {
    dsm_empty_queue(context_ptr->control_info.tx_queue_ptr);
    context_ptr->control_info.tx_queue_ptr = NULL;
  }

  /* Clear the Data channel buffers */
  rb_queue_free(context_ptr->data_info.rx_info.rx_chain, TRUE);
  rb_queue_free(context_ptr->data_info.tx_info.tx_chain, FALSE);

  switch (context_ptr->state)
  {
    case HSU_ECM_INSTANCE_STATE_HALF_OPENED:
      context_ptr->state = HSU_ECM_INSTANCE_STATE_INITIALIZED;
      context_ptr->port_id = SIO_PORT_NULL;
      break;
    case HSU_ECM_INSTANCE_STATE_OPENED:
#ifdef FEATURE_HS_USB_ECM_BAM
      if (context_ptr->bam_info.is_bam_mode)
      {
        context_ptr->state = HSU_ECM_INSTANCE_STATE_INITIALIZED;	    
      }
      else
#endif
      {
        context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
      }
      break;
    default:
      HSU_ERR_FATAL(
        "hsu_al_ecm_control_close(): state = %d, expected OPENED or HALF_OPENED",
        context_ptr->state, 0, 0);
      break;
  }  /* switch (context_ptr->state) */

  HSU_MSG_HIGH("hsu_al_ecm_control_close(): Exit.");
  jsafe_leave();
} /* hsu_al_ecm_control_close */ 

/*==============================================================================

FUNCTION          HSU_AL_ECM_DATA_CLOSE

DESCRIPTION
Called by the application to deallocate and un-configure the data plane for the 
network function.

DEPENDENCIES
ecm_init() must have been called for this instance.

RETURN VALUE
None.

SIDE EFFECTS
Empties the data plane watermark queues.

==============================================================================*/
void hsu_al_ecm_data_close
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  void              (*close_func_ptr)(void)    
  /* Function to call when transmission is complete. */
)
{
  hsu_al_ecm_instance_info_type * context_ptr = NULL;

  jsafe_enter();

  HSU_USE_PARAM(close_func_ptr);  
  context_ptr = get_instance_info_from_stream_id(stream_id);
  if(context_ptr == NULL)
  {
    HSU_ERR_FATAL("hsu_al_ecm_data_close(): Unknown stream_id (%u)", 
      context_ptr, 0, 0);
    jsafe_leave();
    return;
  }

  HSU_USE_PARAM(port_id);
  HSU_MSG_HIGH("hsu_al_ecm_data_close(): Enter.");

  context_ptr->data_info.stream_id = SIO_NO_STREAM_ID;

  switch (context_ptr->state)
  {
    case HSU_ECM_INSTANCE_STATE_HALF_OPENED:
      context_ptr->state = HSU_ECM_INSTANCE_STATE_INITIALIZED;
      context_ptr->port_id = SIO_PORT_NULL;
      break;
    case HSU_ECM_INSTANCE_STATE_OPENED:
      context_ptr->state = HSU_ECM_INSTANCE_STATE_HALF_OPENED;
      break;
    default:
      HSU_ERR_FATAL(
        "hsu_al_ecm_data_close(): state = %d, expected OPENED or HALF_OPENED",
        context_ptr->state, 0, 0);
      break;
  }  /* switch (context_ptr->state) */

  jsafe_leave();
} /* hsu_al_ecm_data_close */


/*===========================================================================

FUNCTION hsu_al_ecm_handle_tx_complete

DESCRIPTION
This function is executed in HSU_AL_TASK context and calls the stack 
cdc_handle_tx_completion function.
When this API is called, the stack will notify us on completed transfers. 
Needed when HS_USB_ECM_TX_TIMEOUT_MS is set to 0, otherwise when the number
of pending buffer reaches the max value, then we stop calling tx_done and
the buffers are not freed anymore.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_ecm_handle_tx_complete
(
  unsigned long arg
)
{
  hsu_al_ecm_instance_info_type* context_ptr = (hsu_al_ecm_instance_info_type*)arg;
  ecm_data_tx_info_type* tx_info_ptr = &context_ptr->data_info.tx_info;
  jresult_t res;
  rb_t * tx_rb = NULL;

  HSU_ASSERT ((context_ptr != NULL) && (tx_info_ptr != NULL));

  jsafe_enter();

  /* The timer expired so we set the tx_handle_comp_scheduled to false to indicate that it 
  should be set if there are resources */
  tx_info_ptr->tx_handle_comp_scheduled = FALSE;

  if (context_ptr->instance_is_enabled == FALSE)
  {
    /* Instance not enabled, nothing to do  */
    jsafe_leave();
    return;
  }

  res = cdc_handle_tx_completion(context_ptr->handle);
  if (res != HSU_JSUCCESS)
  {
	HSU_ULOG (ERROR_MSG, DATA_LOG, "failed");
  }

  tx_rb = get_free_rb(context_ptr->data_info.tx_info.tx_chain, ECM_MAX_NUM_RB_ITEMS_FOR_WRITE);
  HSU_ULOG_1(NO_MSG, DATA_LOG, "rb %X", (uint32)tx_rb);
  if(tx_rb == NULL &&
     (context_ptr->data_info.rx_info.inbound_flow_is_enabled == TRUE))
  {
    /* None of the buffers are free yet, so restart the timer */
    hsu_al_ecm_set_handle_complete_timer(context_ptr);
  }

  jsafe_leave();
}


/*==============================================================================

FUNCTION          HSU_AL_ECM_HANDLE_SET_CONFIGURATION

DESCRIPTION
This function is called when the enabled callback is received.
This function sets the FD handle according to the current configuration FD.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ecm_handle_set_configuration
(
 hsu_al_ecm_instance_info_type *context_ptr
)
{
  uint8 i = 0;
  core_t *core = ((cdc_ctx_t*)context_ptr->handle)->core_ctx;
  uint8 core_index = core->core_index;
  uint8 cur_config_index = core->curr_config;

  HSU_MSG_LOW_1("hsu_al_ecm_handle_set_configuration(): config_idx = %d.", cur_config_index);

  while( i < HS_USB_NUM_OF_ECM_INST)
  {
    if (context_ptr == &ecm_instance_info[i])
    {
      if(hsu_conf_sel_ignore_dtr_drop())
      {
        hsu_al_ecm_set_ignore_disconnect(i,TRUE);
      }

      HSU_ULOG_1(NO_MSG, DATA_LOG, "core_index %u", core_index);
      ecm_instance_info[i].handle = ecm_fd_handle[core_index][cur_config_index-1][i];
    return;
  }
    i++;
  }
  HSU_MSG_ERROR("hsu_al_ecm_handle_set_configuration: invalid context");
}


/*===========================================================================

FUNCTION      hsu_al_ecm_pool_many_cb

DESCRIPTION
This function invokes the appropriate actions when the ECM memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.  This function should be invoked
in HSU_AL_TASK.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ecm_pool_many_cb
(
 dsm_mempool_id_enum_type pool_id
)
{
}

#ifdef FEATURE_HS_USB_RECOVER_DROPPED_PACKET
/*===========================================================================

FUNCTION      hsu_al_ecm_is_any_data_pending

DESCRIPTION
This function checkes all of the pending items throught all of the instances 
and return the status.

DEPENDENCIES
None

RETURN VALUE
TRUE :  any pending item found
FALSE : no pending item found

SIDE EFFECTS
None
===========================================================================*/
boolean hsu_al_ecm_is_any_item_pending(void)
{
  int i; 
  for(i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
    //if(ecm_instance_info[i].data_info.tx_info.pending_chain_len != 0) return TRUE;
    if(ecm_instance_info[i].control_info.pending_dsm_ptr != NULL) return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION      hsu_al_ecm_set_ignore_disconnect

DESCRIPTION
Enable/disable ignoreing disable event from CDC.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ecm_set_ignore_disconnect(int instance_index, boolean value)
{
  ecm_instance_info[instance_index].ignore_disconnect = value;  
}
#endif /* FEATURE_HS_USB_RECOVER_DROPPED_PACKET */


#ifdef FEATURE_HS_USB_BAM_RESET

#if 0 /* This function may need to be re-implemented for BAM */
/* Function is called whenever A2 is ready.  Attempt to enable both the HSIC BAM and HSIC PER
 * if all conditions are satisfied */
void hsu_al_ecm_sps_sm_enable_sps_cb(void *app_data)
{
  hsu_al_ecm_instance_info_type *context_ptr = NULL;
  context_ptr = app_data;

  if ((context_ptr->sps_info.is_sps_mode) &&       // SIO OPEN with SPS PARAM
      (hsu_al_ecm_suspend_resume_mode != HSU_BUS_MODE_SUSPENDED) &&
      (hsu_sps_sm_is_a2_ready() == TRUE))
  {
    if (hsu_al_ecm_enable_sps(context_ptr) == FALSE)
    {
      HSU_ERR_FATAL("hsu_al_ecm_data_open: hsu_al_ecm_enable_sps failed", 0, 0, 0);
      /* TODO: fall-back to legacy mode. need to make sure app provided 
      Rx/Tx WMQs*/
      context_ptr->sps_info.is_sps_mode = FALSE;
    }
  }
}
#endif

/*===========================================================================

FUNCTION      HSU_AL_ECM_ABORT_ALL_SPS_PIPE

DESCRIPTION   
  Deprimes the DL or UL endpoints for all SPS IN or OUT pipes.  This is used to 
  guarantee thre would be no more UL or DL data comming into SPS pipe.

  Set abort_write to TRUE to abort write pipe.
  Set abort_read to TRUE to abort read pipe.
  Setting FALSE to both will result in crash since it is misuse of the API.

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_abort_all_sps_pipe(boolean abort_write, boolean abort_read)
{
  uint32 index = 0;
    
  hsu_al_ecm_instance_info_type* context_ptr;
  jresult_t cdc_abort_result;
  cdc_ctx_t *cdc_ctx_ptr;

  /* Either abort_read or abort_write should be set to TRUE */
  if ((FALSE == abort_write) && (FALSE == abort_read))
  {
    HSU_ERR_FATAL("hsu_al_ecm_abort_all_sps_pipe called with invalid param: "
      "abort_write: %u, abort_read: %u", 
      abort_write, abort_read, 0);
  }

  /* Go over all of RmNet instances (that are initialized) */
  for (index = 0; index < hsu_ecm_num_of_hw_acc_inst; index++)
  {
    context_ptr = &ecm_instance_info[index];

    if (!is_in_bam_mode(context_ptr))
    {
      /* Ignore pipe not in BAM mode */
      continue;
    }

    /*If the instance is not initialized by fd do nothing */
    if (context_ptr->handle == NULL)
    {
      HSU_ULOG_1(HIGH_MSG, BAM_LOG, "xcp cdc_handle_null, index %u", index);
      continue;
    }

    if (context_ptr->instance_is_enabled == FALSE)
    {      
      HSU_ULOG_1(LOW_MSG, BAM_LOG, "xcp instance_not_enabled, index %u", index);

      continue;
    }
    
    if (context_ptr->state != HSU_ECM_INSTANCE_STATE_OPENED)
    {
      HSU_ULOG_1(LOW_MSG, BAM_LOG, "xcp port_not_open, index %u", index);
      continue;
    }

#ifndef FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM
    if (abort_write || abort_read)
    {
      hsu_al_ecm_disable_bam(context_ptr, abort_read, abort_write);
    }

    if (abort_write)
    {
      /* Remove buffer for SPS DL pipe */
      cdc_abort_result = hsu_cdc_fd_api_cdc_abort_write(context_ptr->handle);

      if (cdc_abort_result != HSU_JSUCCESS)
      {
        HSU_ERR_FATAL("cdc_abort write failed: code %u, instance %u", 
          cdc_abort_result, index, 0);
      }
    }
#endif // FEATURE_HS_USB_DISABLE_ECM_USING_BAM_AND_DBM

    // When UL BAM is bypassed (not using BAM), assume that UL abort is 
    // already taken care at cdc/core layer.
    cdc_ctx_ptr = (cdc_ctx_t*)context_ptr->handle;

    if ( abort_read
      && !(cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS) )
    {
      /* Remove buffer for SPS UL pipe */
      cdc_abort_result = hsu_cdc_fd_api_cdc_abort_read(context_ptr->handle);
    
      if (cdc_abort_result != HSU_JSUCCESS)
      {
        HSU_ERR_FATAL("cdc_abort_read failed: code %u, instance %u", 
          cdc_abort_result, index, 0);
      }
    }

#ifndef CI_MERGE
    if (HSU_CORE_CONFIG_USB3_IDX == HSU_CORE_IDX_PRIMARY)
    {
      HAL_hsusb_Read32(
        HSU_CORE_IDX_PRIMARY, 
        HAL_HSUSB_ENDPTSTAT_ADDR);
    }
#endif /*CI_MERGE*/



    HSU_ULOG_3(NO_MSG, BAM_LOG, "index %u, abort_write %u, abort_read %u", index, abort_write, abort_read);

  }
}

#endif /* FEATURE_HS_USB_BAM_RESET */

#ifdef FEATURE_HS_USB_ECM_INT_EP_SHARING    
/*===========================================================================

FUNCTION      hsu_al_ecm_get_shared_intr_pipe_handle

DESCRIPTION   
  Returns the handle for first ECM instance which has the Interrupt Pipe enabled,
  this intrrupt ppipe will be shared for all the ECM instances for sending 
  notifications.
  
DEPENDENCIES  
  FEATURE_HS_USB_ECM_INT_EP_SHARING should be enabled else will return void.

RETURN VALUE  
  cdc_handle for teh first ECM instance.

SIDE EFFECTS  
None.

==============================================================================*/
cdc_handle_t hsu_al_ecm_get_shared_intr_pipe_handle(void)
{
  return (cdc_handle_t*)ecm_instance_info[0].handle;
}
#endif /* FEATURE_HS_USB_ECM_INT_EP_SHARING */

boolean hsu_al_ecm_is_appctx_ecm(void *appctx)
{
  // appctx type should be cdc_appctx_t*
  uint32 i = 0;
  uint32 num_inst = hsu_get_num_of_net_instances();

#ifdef FEATURE_HS_USB_ECM_BAM
  if (num_inst > hsu_ecm_num_of_hw_acc_inst)
  {
    num_inst = hsu_ecm_num_of_hw_acc_inst;
  }
#endif

  for (i = 0; i < num_inst; i++)
  {
    if (appctx == &ecm_instance_info[i])
    {
      return TRUE;
    }
  }
  return FALSE;
}
#ifdef FEATURE_HS_USB_ECM_BAM
boolean hsu_al_ecm_is_bam_flow_enabled(void)
{
  // Assumes all interfaces are all enabled/disabled solely based on the first interface.
  // This is used to prevent double initializations during bootup due to 
  //   SET CONFIG -> SET INTERFACE, vs. ECM ENABLE -> SET CONFIG without SET INTERFACE
  return ecm_instance_info[0].bam_info.ul_bam_flow_enabled || ecm_instance_info[0].bam_info.dl_bam_flow_enabled;
}
#endif
/*===========================================================================
FUNCTION      hsu_al_ecm_print_f3
==============================================================================*/
void hsu_al_ecm_print_f3(void)
{
  uint32                          index;
  hsu_al_ecm_instance_info_type  *context_ptr;
  cdc_ctx_t                      *cdc_ctx_ptr;
  //boolean                         is_ul_rx_sw_path = FALSE;
  uint32                         num_inst = hsu_get_num_of_net_instances();

#ifdef FEATURE_HS_USB_ECM_BAM
  if (num_inst > hsu_ecm_num_of_hw_acc_inst)
  {
    num_inst = hsu_ecm_num_of_hw_acc_inst;
  }
#endif

  for (index = 0; index < num_inst ; index++)
  {
    context_ptr = &ecm_instance_info[index];
    cdc_ctx_ptr = (cdc_ctx_t*)(context_ptr->handle);

    if ( (HSU_ECM_INSTANCE_STATE_OPENED == context_ptr->state)
      && (NULL != cdc_ctx_ptr) )
    {
      //is_ul_rx_sw_path = (0 != (cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS));

      HSU_MSG_HIGH("----------------------------------------------------");
      //HSU_MSG_HIGH_2(" ECM instance (%4u), ul_rx_sw_path       (%4u)", index, is_ul_rx_sw_path);
      HSU_MSG_HIGH("----------------------------------------------------");
#ifdef FEATURE_HS_USB_ECM_BAM
      HSU_MSG_HIGH_3("is_bam_mode   (%4u), ul_bam_flow_enabled (%4u), dl_bam_flow_enabled (%4u)",
        context_ptr->bam_info.is_bam_mode,
        context_ptr->bam_info.ul_bam_flow_enabled,
        context_ptr->bam_info.dl_bam_flow_enabled);
#endif
      HSU_MSG_HIGH_3("ul_rx_err_cnt (%4u), ul_rx_zero_dsm_cnt  (%4u), ul_rx_total_bytes   (%4u)",
        context_ptr->statistics.rx_transfer_error_cnt,
        context_ptr->statistics.rx_zero_dsm_cnt,
        context_ptr->statistics.rx_total_bytes);

      HSU_MSG_HIGH_3("ul_rx_timeout (%4u), ul_rx_buf_count     (%4u), ul_rx_buf_size      (%4u)",
        context_ptr->data_info.rx_info.rx_timeout,
        context_ptr->data_info.rx_info.array_size,
        ECM_ROUNDED_DSM_ITEM_SIZE);
    }
  }
}

#ifdef FEATURE_HS_USB_STANDARD_ECM
/*==============================================================================

FUNCTION      HSU_AL_ECM_USB_INICHE_RX_DONE_CB

DESCRIPTION   
Callback function for Rx_done.

DEPENDENCIES  
None.

RETURN VALUE  
None

SIDE EFFECTS  
None.
==============================================================================*/
void hsu_al_ecm_usb_iNiche_rx_done_cb
(
 void * data
)
{
	hsu_al_ecm_rx_complete (NULL, &ecm_instance_info[0], NULL, data, 0, (bam_dmux_status)0);
	return;
}

/*==============================================================================

FUNCTION      HSU_AL_ECM_USB_INICHE_TX_CB

DESCRIPTION   
Callback function for Tx.

DEPENDENCIES  
None.

RETURN VALUE  
None

SIDE EFFECTS  
None.
==============================================================================*/
void hsu_al_ecm_usb_iNiche_tx_cb
(
  void * data,
  void * tx_priv,
  uint16 data_len
)
{
	hsu_al_ecm_tx_data (NULL, &ecm_instance_info[0], tx_priv, data, data_len);
	return;
}

/*==============================================================================

FUNCTION      USB_INICHE_CB_REG

DESCRIPTION   
Register rx and tx callback functions with the USB driver.

DEPENDENCIES  
None.

RETURN VALUE  
SUCCESS                 -- All the callbacks are registered properly.
FAILURE                 -- something is not right or USB driver stack is not up etc.

SIDE EFFECTS  
None.
==============================================================================*/
boolean hsu_al_ecm_usb_iNiche_cb_reg
(
 hsu_al_ecm_usb_iNiche_rx_cb_type      usb_iNiche_rx_cb,
 hsu_al_ecm_usb_iNiche_rx_done_cb_type *usb_iNiche_rx_done_cb,
 hsu_al_ecm_usb_iNiche_tx_cb_type      *usb_iNiche_tx_cb,
 hsu_al_ecm_usb_iNiche_tx_done_cb_type usb_iNiche_tx_done_cb
)
{
	HSU_ULOG(NO_MSG, DATA_LOG, "Enter");
	
	HSU_ASSERT (usb_iNiche_rx_cb &&
				usb_iNiche_rx_done_cb &&
				usb_iNiche_tx_cb &&
				usb_iNiche_tx_done_cb);
	
	ecm_instance_info[0].data_info.rx_func_ptr_std = usb_iNiche_rx_cb;
	ecm_instance_info[0].data_info.tx_done_ptr_std = usb_iNiche_tx_done_cb;

	*usb_iNiche_rx_done_cb = hsu_al_ecm_usb_iNiche_rx_done_cb;
	*usb_iNiche_tx_cb = hsu_al_ecm_usb_iNiche_tx_cb;
	
	HSU_ULOG(NO_MSG, DATA_LOG, "Exit");
	
	return TRUE;
}


/*==============================================================================

FUNCTION      HSU_AL_ECM_IS_STANDARD_ECM_OPENED

DESCRIPTION   
Returns TRUE if the stdECM id opened. Used during allow enumeration.

DEPENDENCIES  
None.

RETURN VALUE  
TRUE                 -- Standard ECM is opened.
FALSE                -- Standard ECM is not opened.

SIDE EFFECTS  
None.
==============================================================================*/
boolean hsu_al_ecm_is_standard_ecm_opened(void)
{
  if (ecm_instance_info[0].state == HSU_ECM_INSTANCE_STATE_OPENED)
  {
  	return TRUE;
  }
  else
  {
  	return FALSE;
  }
}

#endif /*FEATURE_HS_USB_STANDARD_ECM*/

