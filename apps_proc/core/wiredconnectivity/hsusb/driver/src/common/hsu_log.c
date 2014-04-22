/*==============================================================================

  HSU Generic Logging APIs

  GENERAL DESCRIPTION

  EXTERNALIZED FUNCTIONS

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_log.c#1 $
  $DateTime: 2020/01/30 22:49:35 $
    
  when      who       what, where, why
  --------  ------    ------------------------------------------------------------
  11/15/13  kameya    support for snps circular buffer logging.
  02/05/12  ac        Initial version
==============================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include "customer.h"
#include "timetick.h"
#include "assert.h"
#include "jdebug.h"
#include "hsu_log.h"
#include "hsu_common.h"
#include "hsu_timer.h"
#include "hsu_conf_sel_i.h"
#include "hsu_common_int.h"
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_conf_sel_stack_utils_int.h"
#include "jusb_core.h"
#include "hsu_al_ecm_int.h"
#include "hsu_al_dsm_utils.h"
#include "DDITimetick.h"
#include <stringl/stringl.h>
#include "hsu_platform.h"
#include "hsu_conf_sel_core_config.h"

//#include "DALSys.h"
#include "DALStdErr.h"
#include "CoreTime.h"
#include "qurt_interrupt.h"

static unsigned long hsu_log_intlock_flag;

#define PACK4(qmi_data, data, index) (qmi_data = ((((unsigned char *)data)[index+0] << 24) | \
                                    (((unsigned char *)data)[index+1] << 16) | \
                                    (((unsigned char *)data)[index+2] << 8) | \
                                    ((unsigned char *)data)[index+3]))

static uint32 hsu_log_timetick_clk_khz;
static uint64 hsu_log_timetick_19p2M;
static uint32 hsu_log_timetick_32K;
static hsu_dun_log_config_info_type hsu_dun_log_config_info;


// DEBUG: Enable uncached buffer logging
#define FEATURE_HS_USB_LOG_UNCACHED

#ifdef FEATURE_HS_USB_LOG_UNCACHED


#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF
  static hsu_audio_log_type *hsu_audio_cir_buff;
  static hsu_snps_ctrl_log_type *hsu_snps_ctrl_cir_buff;
  static hsu_snps_log_type *hsu_snps_cir_buff;
  static hsu_generic_log_type *hsu_cir_buff;
  static hsu_generic_log_type *hsu_bus_cir_buff;
  static hsu_generic_log_type *hsu_data_cir_buff;
  static hsu_qmi_log_type *hsu_qmi_cir_buff;
  static hsu_qmi_data_log_type *hsu_qmi_data_cir_buff;
  static hsu_ser4_log_type *hsu_ser4_cir_buff;
  static hsu_ser4_data_log_type *hsu_ser4_data_cir_buff;
  static hsu_reg_log_type *hsu_reg_cir_buff;
  static hsu_func_log_type *hsu_func_cir_buff;
  static hsu_dbg_e_log_type *hsu_dbg_e_cir_buff;  
#endif

static hsu_dun_log_type *hsu_dun_cir_buff;
static hsu_perf_log_type *hsu_perf_cir_buff;
static hsu_vddmin_log_type *hsu_vddmin_cir_buff;

static hsu_ep0_log_type *hsu_ep0_cir_buff;
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
static hsu_task_schedule_log_type *hsu_task_schedule_cir_buff;
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
#ifdef FEATURE_HS_USB_DEBUG_DTD
static hsu_dtd_log_type *hsu_dtd_cir_buff;
#endif /* FEATURE_HS_USB_DEBUG_DTD */
#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
static hsu_mem_log_type *hsu_mem_cir_buff;
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */
static hsu_timer_log_type *hsu_timer_cir_buff;
static hsu_reg_dump_log_type *hsu_reg_dump_cir_buff;

/* frequency log type */
static hsu_event_log_type *hsu_event_buf;
#else /* FEATURE_HS_USB_LOG_UNCACHED */

#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF
static hsu_audio_log_type hsu_audio_cir_buff[HSU_AUDIO_LOG_SIZE];
static hsu_snps_ctrl_log_type hsu_snps_ctrl_cir_buff[HSU_SNPS_CTRL_LOG_SIZE];
static hsu_snps_log_type hsu_snps_cir_buff[HSU_SNPS_LOG_SIZE];
static hsu_generic_log_type hsu_cir_buff[HSU_GENERIC_LOG_SIZE];
static hsu_generic_log_type hsu_bus_cir_buff[HSU_BUS_LOG_SIZE];
static hsu_generic_log_type hsu_data_cir_buff[HSU_DATA_LOG_SIZE];
static hsu_qmi_log_type hsu_qmi_cir_buff[HS_USB_NUM_OF_ECM_INST * HSU_QMI_LOG_SIZE]; /* 1D representation to be compatible with uncached representation */
static hsu_qmi_data_log_type hsu_qmi_data_cir_buff[HSU_QMI_DATA_LOG_SIZE];
static hsu_ser4_log_type hsu_ser4_cir_buff[HSU_SER4_LOG_SIZE];
static hsu_ser4_data_log_type hsu_ser4_data_cir_buff[HSU_SER4_DATA_LOG_SIZE];
static hsu_reg_log_type hsu_reg_cir_buff[HSU_REG_LOG_SIZE];
static hsu_func_log_type hsu_func_cir_buff[HSU_FUNC_LOG_SIZE];
#endif

static hsu_dun_log_type hsu_dun_cir_buff[HSU_DUN_LOG_SIZE];
static hsu_perf_log_type hsu_perf_cir_buff[HSU_PERF_LOG_SIZE];
static hsu_vddmin_log_type hsu_vddmin_cir_buff[HSU_VDDMIN_LOG_SIZE];
static hsu_dbg_e_log_type hsu_dbg_e_cir_buff[HSU_DBG_E_LOG_SIZE];
static hsu_ep0_log_type hsu_ep0_cir_buff[HSU_EP0_LOG_SIZE];
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
static hsu_task_schedule_log_type hsu_task_schedule_cir_buff[HSU_TASK_SCHEDULE_LOG_SIZE];
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
#ifdef FEATURE_HS_USB_DEBUG_DTD
static hsu_dtd_log_type hsu_dtd_cir_buff[HSU_DTD_LOG_SIZE];
#endif /* FEATURE_HS_USB_DEBUG_DTD */
#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
static hsu_mem_log_type hsu_mem_cir_buff[HSU_MEM_LOG_SIZE];
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */
static hsu_timer_log_type hsu_timer_cir_buff[HSU_TIMER_LOG_SIZE];
static hsu_reg_dump_log_type hsu_reg_dump_cir_buff[HSU_REG_DUMP_LOG_SIZE];

/* frequency log type */
static hsu_event_log_type hsu_event_buf[NUMBER_OF_CORES * HSU_EVENT_LOG_NUM_ITEMS + 1];
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF
static boolean is_reg_log_on = TRUE;
static boolean is_func_log_on = TRUE;
static uint32 hsu_audio_cir_buff_index;
static uint32 hsu_snps_cir_buff_index;
static uint32 hsu_snps_ctrl_cir_buff_index;
static uint32 hsu_cir_buff_index;
static uint32 hsu_bus_cir_buff_index;
static uint32 hsu_data_cir_buff_index;
static uint32 hsu_qmi_data_cir_buff_index;
static uint32 hsu_ser4_cir_buff_index;
static uint32 hsu_ser4_data_cir_buff_index;
static uint32 hsu_reg_cir_buff_index;
static uint32 hsu_func_cir_buff_index;
static uint32 hsu_dbg_e_cir_buff_index;
#endif /*FEATURE_HS_USB_USE_LEGACY_CIR_BUF*/

static uint32 hsu_dun_cir_buff_index;
static uint32 hsu_perf_cir_buff_index;
static uint32 hsu_vddmin_cir_buff_index;
static uint32 hsu_ep0_cir_buff_index;
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
static uint32 hsu_task_schedule_cir_buff_index;
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
#ifdef FEATURE_HS_USB_DEBUG_DTD
static uint32 hsu_dtd_cir_buff_index;
#endif /* FEATURE_HS_USB_DEBUG_DTD */
#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
static uint32 hsu_mem_cir_buff_index;
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */
static uint32 hsu_timer_cir_buff_index;
static uint32 hsu_reg_dump_cir_buff_index;


#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF
static const uint32 hsu_audio_cir_buff_size     = HSU_AUDIO_LOG_SIZE;
static const uint32 hsu_snps_ctrl_cir_buff_size= HSU_SNPS_CTRL_LOG_SIZE;
static const uint32 hsu_snps_cir_buff_size     = HSU_SNPS_LOG_SIZE;
static const uint32 hsu_cir_buff_size          = HSU_GENERIC_LOG_SIZE;
static const uint32 hsu_bus_cir_buff_size      = HSU_BUS_LOG_SIZE;
static const uint32 hsu_data_cir_buff_size     = HSU_DATA_LOG_SIZE;
static const uint32 hsu_qmi_cir_buff_size      = HSU_QMI_LOG_SIZE;
static const uint32 hsu_qmi_data_cir_buff_size = HSU_QMI_DATA_LOG_SIZE;
static const uint32 hsu_ser4_cir_buff_size     = HSU_SER4_LOG_SIZE;
static const uint32 hsu_ser4_data_cir_buff_size= HSU_SER4_DATA_LOG_SIZE;
static const uint32 hsu_reg_cir_buff_size      = HSU_REG_LOG_SIZE;
static const uint32 hsu_func_cir_buff_size     = HSU_FUNC_LOG_SIZE;
static const uint32 hsu_dbg_e_cir_buff_size    = HSU_DBG_E_LOG_SIZE;

/* lookup table of interface number to hsu_qmi_cir_buff's ecm instance number */
/* content of array represents the interface number */
typedef struct
{
  hsu_qmi_log_type*   iface_cir_buff; /* points to interface's circular buffer section */
  uint32              qmi_cir_buff_index;
  uint32              qmi_cir_buff_max_size;
  uint8               iface_num;
  uint8               intr_in_ep;
  uint8               bulk_out_ep;
  uint8               bulk_in_ep;
} hsu_qmi_iface_type;

#define HSU_LOG_MAX_NUM_OF_ENDPTS    32

/* Lookup table for: index in RMNET INTERFACE with direct mapping to circular buffer pointer */
static hsu_qmi_iface_type* hsu_qmi_endpt_iface_table[HSU_LOG_MAX_NUM_OF_ENDPTS];

/* iface number will be matched to a zero based index array */
static hsu_qmi_iface_type qmhsu_qmi_iface_table[HS_USB_NUM_OF_ECM_INST];

#endif

/* these constants are required for JTAG script to decipher the size of the buffers */
static const uint32 hsu_dun_cir_buff_size     = HSU_DUN_LOG_SIZE;
static const uint32 hsu_perf_cir_buff_size     = HSU_PERF_LOG_SIZE;
static const uint32 hsu_vddmin_cir_buff_size     = HSU_VDDMIN_LOG_SIZE;
static const uint32 hsu_ep0_cir_buff_size      = HSU_EP0_LOG_SIZE;
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
static const uint32 hsu_task_schedule_cir_buff_size = HSU_TASK_SCHEDULE_LOG_SIZE;
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
#ifdef FEATURE_HS_USB_DEBUG_DTD
static const uint32 hsu_dtd_cir_buff_size      = HSU_DTD_LOG_SIZE;
#endif /* FEATURE_HS_USB_DEBUG_DTD */
#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
static const uint32 hsu_mem_cir_buff_size      = HSU_MEM_LOG_SIZE;
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */
static const uint32 hsu_timer_cir_buff_size    = HSU_TIMER_LOG_SIZE;
static const uint32 hsu_reg_dump_cir_buff_size = HSU_REG_DUMP_LOG_SIZE;
static const uint32 hsu_event_num_items = HSU_EVENT_LOG_NUM_ITEMS;

// Macro to string expansion
#define HSU_ENUM_TO_STR(enum_val) #enum_val

// Event log to string
#define HSU_EVT_LOG_TO_MSG_HIGH(evt_enum, core_id) \
  HSU_DIAG_MSG_HIGH_2("%8u :%15u : " HSU_ENUM_TO_STR(evt_enum), \
    hsu_event_buf[((uint32)(evt_enum) - 1) + (HSU_EVENT_LOG_NUM_ITEMS) * core_id].total_cnt, \
    hsu_event_buf[((uint32)(evt_enum) - 1) + (HSU_EVENT_LOG_NUM_ITEMS) * core_id].last_event_timetick)




/* Debug Timer definitions */
hsu_timer_handle_type  hsu_log_dbg_timer_1, hsu_log_dbg_timer_2;
timetick_type   timeout_tmr_1_value, timeout_tmr_2_value;

static void hsu_log_dbg_timer_init(void);

#ifdef FEATURE_HS_USB_TRACE
hsu_trace_info hsu_trace;
#endif /* FEATURE_HS_USB_TRACE */

static char* HSU_ULOG_ENUM_STR[HSU_ULOG_ENUM_SIZE] =
{
    /*==============================================================================
  **
  ** HSUSB BUS NOTIFICATIONS
  **
  *=============================================================================*/
  /* ++HSU BUS NOTIFY      */
  /* HSU_BUS_NOTIFY_OFFSET */
  "EMPTY",
  "NOTIFY_DEVICE_CONNECT",
  "NOTIFY_DEVICE_DISCONNECT",
  "NOTIFY_DEVICE_SUSPEND",
  "NOTIFY_DEVICE_RESUME",
  "NOTIFY_DEVICE_RESUME_COMPLETED",
  "NOTIFY_DEVICE_REMOTE_WAKEUP",
  "NOTIFY_DEVICE_CONFIGURED",
  "NOTIFY_DEVICE_UNCONFIGURED",
  "NOTIFY_DEVICE_RESET",
  "NOTIFY_DEVICE_SPEED_CHANGE",
  /* --HSU BUS NOTIFY */

  /* +++ EVENTS */
  "HSU_BAM_SM_NIL_EVT", /* required as event definition starts with 1 */

  /* HSU_AP_SIDEBAND_SIG */
  "HSU_HOST_READY_RISE_EVT",
  "HSU_HOST_READY_FALL_EVT",

  /* HSU_USB_BUS_SIG */
  "HSU_BUS_SUSPEND_EVT",
  "HSU_BUS_RESUME_EVT",
  "HSU_BUS_RESET_EVT",
  "HSU_BUS_CONFIGURED_EVT",

  /* HSU_IPA_SIG */
  "HSU_IPA_SUSPEND_COMPLETE_IND_EVT",
  "HSU_IPA_DATA_ACTIVITY_EVT",

  /* HSU_FRAMEWORK_SIG */
  "HSU_TIMER_EVT",  

  /* +++ STATES */
  // Must update hsu_generic_log_enum, hsu_event_log_enum, hsu_bam_sm_state_enum (for both BAM and NO ECM BAM state machines),
  // HSU_ULOG_ENUM_STR, hsu_ulog_usb_enum. 
  "HSU_BAM_SM_NIL_STATE", /* required as state definition starts with 1 */
  
  "FLOW_ENABLED_STATE",
  "FLOW_DISABLING_FROM_FORCE_SHUTDOWN",
  "FLOW_DISABLED_FROM_FORCE_SHUTDOWN",

  "DATA_ACTIVITY_WHILE_IN_FORCE_SHUTDOWN",
  "ENUMERATION_WITH_AP",

  "FLOW_DISABLING_FROM_SUSPEND",
  "FLOW_DISABLED_FROM_SUSPEND",
  "DATA_ACTIVITY_WHILE_SUSPENDED",
  "BOOTUP_FLOW_DISABLED",
  /* --- STATES */

  /* HSU_SPEED_OFFSET */
  "SPEED_LOW",
  "SPEED_FULL",
  "SPEED_HIGH",
  "SPEED_SUPER",
  "SPEED_UNKNOWN",



  /*==============================================================================
  **
  ** USB Interface Offset
  **
  *=============================================================================*/ 
  
  /* HSU_IFACE_OFFSET */
  "NO_HSU_IFACE",
  
  "MS_HSU_IFACE",
  
  "DIAG_HSU_IFACE",
  "NMEA_HSU_IFACE",
  "SER3_HSU_IFACE",
  "SER4_HSU_IFACE",
  "SER5_HSU_IFACE",
  "SER6_HSU_IFACE",
  "DCI_HSU_IFACE",
  "QDSS_HSU_IFACE",
  
  "MDM_HSU_IFACE",
  "CSVT_HSU_IFACE",
  
  "NET1_HSU_IFACE",
  "NET2_HSU_IFACE",
  "NET3_HSU_IFACE",
  "NET4_HSU_IFACE",
  "NET5_HSU_IFACE",
  "NET6_HSU_IFACE",
  "NET7_HSU_IFACE",
  "NET8_HSU_IFACE",
  
  "MBIM_HSU_IFACE",
  "GNSS_HSU_IFACE",
  "IPCR_HSU_IFACE",
  
  "AUDIO_HSU_IFACE",
  "NUM_OF_HSU_IFACE",



  /*==============================================================================
  **
  ** CLKMGR LOG
  **
  *=============================================================================*/   
  "HSU_CLKMGR__CLK_OFF",
  "HSU_CLKMGR__CLK_ON",
    
  "HSU_CLKMGR__NPA__CLK_OFF",
  "HSU_CLKMGR__NPA__CLK_ON",


  /* HSU_CORE_OFFSET */
  "USB2_CI_CORE",     //< -- USB2.0 core.
  "UNKNOWN_CORE",       //< -- Uknown, Number of core types.

  // HSU_PWR_EVNT_OFFSET 
  "PWR_EVNT_SW",
  "PWR_EVNT_PHY_L2",
  "PWR_EVNT_PHY_P3",
  "QMP_PHY_LFPS_IRQ",
  "QUSB2_PHY_IRQ",
  "GENERIC_WAKEUP_EVNT",   


  /* Chapter 9 CMD's */
  "HSU_CHP9_REQ_GET_STATUS",
  "HSU_CHP9_REQ_CLEAR_FEATURE",
  "HSU_DUMMY_1",
  "HSU_CHP9_REQ_SET_FEATURE",
  "HSU_DUMMY_2",
  "HSU_CHP9_REQ_SET_ADDRESS",
  "HSU_CHP9_REQ_GET_DESCRIPTOR",
  "HSU_CHP9_REQ_SET_DESCRIPTOR",
  "HSU_CHP9_REQ_GET_CONFIGURATION",
  "HSU_CHP9_REQ_SET_CONFIGURATION",
  "HSU_CHP9_REQ_GET_INTERFACE",
  "HSU_CHP9_REQ_SET_INTERFACE",
  "HSU_CHP9_REQ_SYNCH_FRAME",
  "HSU_CHP9_OUT_OF_CMD",

  /* Get Descriptor CMD's */
  /* 0*/ "GET_DESCRIPTOR_INVALID",
  /* 1*/ "GET_DEVICE_DESCRIPTOR",
  /* 2*/ "GET_CONFIG_DESCRIPTOR",
  /* 3*/ "GET_STRING_DESCRIPTOR",
  /* 4*/ "GET_INTERFACE_DESCRIPTOR",
  /* 5*/ "GET_ENDPOINT_DESCRIPTOR",
  /* 6*/ "GET_DEVICE_QUALIFIER_DESCRIPTOR",
  /* 7*/ "GET_OTHER_SPEED_CONFIG_DESCRIPTOR",
  /* 8*/ "GET_INTERFACE_POWER_DESCRIPTOR",
  /* 9*/ "GET_OTG_DESCRIPTOR",
  /* A*/ "GET_DEBUG_DESCRIPTOR",
  /* B*/ "GET_INTERFACE_ASSOCIATION_DESCRIPTOR",
  /* C*/ "GET_0x0C",
  /* D*/ "GET_0x0D",
  /* E*/ "GET_0x0E",
  /* F*/ "GET_BOS_DESCRIPTOR",
  /*10*/ "GET_DEVICE_CAPABILITY",
  /*11*/ "GET_DEVICE_DESCRIPTOR_OUT_OF_ULOG",

    
  /*==============================================================================
  **
  ** TX/RX CDC Transfer Status
  **
  *=============================================================================*/ 
  "TX_CDC_TRANSFER_ERROR",
  "TX_CDC_TRANSFER_OK",        /* HSU_TX_CDC_TRANSFER_OFFSET */
  "TX_CDC_TRANSFER_CANCELLED",

  "RX_CDC_TRANSFER_ERROR",
  "RX_CDC_TRANSFER_OK",        /* HSU_RX_CDC_TRANSFER_OFFSET */
  "RX_CDC_TRANSFER_CANCELLED",  

  /*==============================================================================
  **
  ** snps_dcd_ep_event_type
  **
  *=============================================================================*/ 
  "SNPS_CTRL_EVT_UNKNOWN",
  "SNPS_CTRL_EVT_XFER_COMPLETE",
  "SNPS_CTRL_EVT_XFER_NOTREADY",
  "SNPS_CTRL_EVT_XFER_ERROR",
  "SNPS_CTRL_EVT_CORE_REQ"
};

#define HSU_ULOG_SIZE_ADJUST 16
//This table need to match 1-1 withhsu_ulog_log_type_enum in hsu_log.h
static hsu_ulog_ctx_type hsu_ulog_ctx[HSU_LOG_TYPE_SIZE] = 
{
  //{handle ptr, buf_size, buf_name}
  //handle ptr is dynamic setup, so init to 0.
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_0"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_1"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_2"},    
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_3"},    
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_4"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_5"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_6"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_7"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_8"},  
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_9"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_10"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_11"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_12"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_13"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_14"},
  {0, HSU_QMI_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_qmi_if_15"},
      
  {0, HSU_BAM_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_bam"},
  {0, HSU_BUS_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_bus"},
  {0, HSU_DATA_LOG_SIZE      * HSU_ULOG_SIZE_ADJUST, "usb_data"},
  {0, HSU_SNPS_CTRL_LOG_SIZE * HSU_ULOG_SIZE_ADJUST, "usb_snps_ctrl"},
  {0, HSU_SNPS_LOG_SIZE      * HSU_ULOG_SIZE_ADJUST, "usb_snps"},
  {0, HSU_SER4_LOG_SIZE      * HSU_ULOG_SIZE_ADJUST, "usb_ser4"},
  {0, HSU_REG_LOG_SIZE       * HSU_ULOG_SIZE_ADJUST, "usb_reg"},
  {0, HSU_AUDIO_LOG_SIZE     * HSU_ULOG_SIZE_ADJUST, "usb_audio"},
  {0, HSU_DBG_E_LOG_SIZE     * HSU_ULOG_SIZE_ADJUST, "usb_dbg_e"},
  {0, HSU_BUS_LOG_SIZE*4     * HSU_ULOG_SIZE_ADJUST, "usb_eng"},
};
static uint32 hsu_ulog_total_size = 0;


static uint64 hsu_timetick_ulog_get64(void)
{
/*
  uint64 timestamp = 0;
  ULogHandle h = hsu_ulog_ctx[BAM_LOG].handle;
  if(((ULOG_TYPE*)h->altTimestampSrcFn != NULL && !HANDLE_IS_A_TEE(((ULOG_TYPE*)h))) && (h != null))
  {
    timestamp = ((ULOG_TYPE*)h)->altTimestampSrcFn();
  }
  else
  {
  */
  return CoreTimetick_Get64();
  //}
}


  
/*============================================================================
  hsu_dun_log_init
==============================================================================*/
static void hsu_dun_log_init
(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_dun_cir_buff = (hsu_dun_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_dun_log_type) * (HSU_DUN_LOG_SIZE + 1));
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_dun_cir_buff, 0, sizeof(hsu_dun_log_type) * HSU_DUN_LOG_SIZE);
  hsu_dun_cir_buff_index = 0;

  hsu_dun_log_config_info.rx_internal 
    = hsu_dun_log_config_info.tx_internal 
    = hsu_dun_log_config_info.rx_f3 
    = hsu_dun_log_config_info.tx_f3 
    = FALSE;
  
  switch((hsu_dun_efs_config_type)hsu_conf_sel_nv_efs_get(HSU_EFS_DUN_LOG_ENABLED))
  {
    case HSU_DUN_LOG_RX_TX_INTERNAL_ONLY:
    {
      hsu_dun_log_config_info.rx_internal = hsu_dun_log_config_info.tx_internal = TRUE;
    }
    break;
   
    case HSU_DUN_LOG_RX_TX_F3_ONLY:
    {
      hsu_dun_log_config_info.rx_f3 = hsu_dun_log_config_info.tx_f3 = TRUE;
    }
    break;
   
    case HSU_DUN_LOG_RX_INTERNAL_TX_F3:
    {
      hsu_dun_log_config_info.rx_internal = hsu_dun_log_config_info.tx_f3 = TRUE;
    }
    break;

    case HSU_DUN_LOG_RX_INTERNAL_ONLY:
    {
      hsu_dun_log_config_info.rx_internal = TRUE;
    }
    break;
   
    case HSU_DUN_LOG_TX_INTERNAL_ONLY:
    {
      hsu_dun_log_config_info.tx_internal = TRUE;
    }
    break;
   
    case HSU_DUN_LOG_TX_INTERNAL_RX_F3:
    {
      hsu_dun_log_config_info.tx_internal = hsu_dun_log_config_info.rx_f3 = TRUE;
    }
    break;
   
    case HSU_DUN_LOG_RX_TX_INTERNAL_AND_F3:
    {
      hsu_dun_log_config_info.rx_internal 
        = hsu_dun_log_config_info.tx_internal 
        = hsu_dun_log_config_info.rx_f3 
        = hsu_dun_log_config_info.tx_f3 
        = TRUE;
    }
    break;

    // Do not log
    case HSU_DUN_LOG_DISABLED:
    default:
    break;
  }
}


/*============================================================================
  hsu_perf_log_init
==============================================================================*/
static void hsu_perf_log_init
(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_perf_cir_buff = (hsu_perf_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_perf_log_type) * (HSU_PERF_LOG_SIZE + 1));
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_perf_cir_buff, 0, sizeof(hsu_perf_log_type) * HSU_PERF_LOG_SIZE);
  hsu_perf_cir_buff_index = 0;
}



#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF
/*============================================================================
  hsu_audio_log_init
==============================================================================*/
static void hsu_audio_log_init
(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_audio_cir_buff = (hsu_audio_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_perf_log_type) * (HSU_AUDIO_LOG_SIZE + 1));
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_audio_cir_buff, 0, sizeof(hsu_audio_log_type) * HSU_AUDIO_LOG_SIZE);
  hsu_audio_cir_buff_index = 0;
}

/*============================================================================
  hsu_snps_ctrl_log_init
==============================================================================*/
static void hsu_snps_ctrl_log_init
(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_snps_ctrl_cir_buff = (hsu_snps_ctrl_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_snps_ctrl_log_type) * (HSU_SNPS_CTRL_LOG_SIZE + 1));
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_snps_ctrl_cir_buff, 0, sizeof(hsu_snps_ctrl_log_type) * HSU_SNPS_CTRL_LOG_SIZE);
  hsu_snps_ctrl_cir_buff_index = 0;
}

/*============================================================================
  hsu_snps_log_init
==============================================================================*/
static void hsu_snps_log_init
(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_snps_cir_buff = (hsu_snps_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_snps_log_type) * (HSU_SNPS_LOG_SIZE + 1));
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_snps_cir_buff, 0, sizeof(hsu_snps_log_type) * HSU_SNPS_LOG_SIZE);
  hsu_snps_cir_buff_index = 0;
}


/*============================================================================
  hsu_bus_log_init
==============================================================================*/
static void hsu_bus_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_bus_cir_buff = (hsu_generic_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_generic_log_type) * (HSU_BUS_LOG_SIZE + 1));
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_bus_cir_buff, 0, sizeof(hsu_generic_log_type) * HSU_BUS_LOG_SIZE);
  hsu_bus_cir_buff_index = 0;
}

/*============================================================================
  hsu_data_log_init
==============================================================================*/
static void hsu_data_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_data_cir_buff = (hsu_generic_log_type *) 
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_generic_log_type) * (HSU_DATA_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_data_cir_buff, 0, sizeof(hsu_generic_log_type) * HSU_DATA_LOG_SIZE);
  hsu_data_cir_buff_index = 0;
}

/*============================================================================
  hsu_qmi_log_init
==============================================================================*/
static void hsu_qmi_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_qmi_cir_buff = (hsu_qmi_log_type *) 
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_qmi_log_type) * (HSU_QMI_LOG_SIZE + 1) * HS_USB_NUM_OF_ECM_INST);  

  hsu_qmi_data_cir_buff = (hsu_qmi_data_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_qmi_data_log_type) * (HSU_QMI_DATA_LOG_SIZE + 1));
  
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_qmi_cir_buff, 0, sizeof(hsu_qmi_log_type) * HSU_QMI_LOG_SIZE * HS_USB_NUM_OF_ECM_INST);
  memset(hsu_qmi_data_cir_buff, 0, sizeof(hsu_qmi_data_log_type) * HSU_QMI_DATA_LOG_SIZE);
  hsu_qmi_data_cir_buff_index = 0;
}

/*============================================================================
  hsu_ser4_log_init
==============================================================================*/
static void hsu_ser4_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_ser4_cir_buff = (hsu_ser4_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
       sizeof(hsu_ser4_log_type) * (HSU_SER4_LOG_SIZE + 1));

  hsu_ser4_data_cir_buff = (hsu_ser4_data_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_ser4_data_log_type) * (HSU_SER4_DATA_LOG_SIZE + 1));
  
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_ser4_cir_buff, 0, sizeof(hsu_ser4_log_type) * HSU_SER4_LOG_SIZE);
  memset(hsu_ser4_data_cir_buff, 0, sizeof(hsu_ser4_data_log_type) * HSU_SER4_DATA_LOG_SIZE);
  hsu_ser4_cir_buff_index = 0;
  hsu_ser4_data_cir_buff_index = 0;
}

/*============================================================================
  hsu_reg_log_init
==============================================================================*/
void hsu_reg_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_reg_cir_buff = (hsu_reg_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_reg_log_type) * (HSU_REG_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_reg_cir_buff, 0, sizeof(hsu_reg_log_type) * HSU_REG_LOG_SIZE);
  hsu_reg_cir_buff_index = 0;
}

/*============================================================================
  hsu_func_log_init
==============================================================================*/
void hsu_func_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_func_cir_buff = (hsu_func_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_func_log_type) * (HSU_FUNC_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_func_cir_buff, 0, sizeof(hsu_func_log_type) * HSU_FUNC_LOG_SIZE);
  hsu_func_cir_buff_index = 0;
}

/*============================================================================
  hsu_dbg_e_log_init
==============================================================================*/
void hsu_dbg_e_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED
  hsu_dbg_e_cir_buff = (hsu_dbg_e_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_dbg_e_log_type) * (HSU_DBG_E_LOG_SIZE + 1));      
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_dbg_e_cir_buff, 0, sizeof(hsu_dbg_e_log_type) * HSU_DBG_E_LOG_SIZE);
  hsu_dbg_e_cir_buff_index = 0;
}


#endif  /*FEATURE_HS_USB_USE_LEGACY_CIR_BUF*/





/*============================================================================
  hsu_vddmin_log_init
==============================================================================*/
void hsu_vddmin_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_vddmin_cir_buff = (hsu_vddmin_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_vddmin_log_type) * (HSU_VDDMIN_LOG_SIZE + 1));  

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_vddmin_cir_buff, 0, sizeof(hsu_vddmin_log_type) * HSU_VDDMIN_LOG_SIZE);
  hsu_vddmin_cir_buff_index = 0;
}

/*============================================================================
  hsu_ep0_log_init
==============================================================================*/
void hsu_ep0_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_ep0_cir_buff = (hsu_ep0_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_ep0_log_type) * (HSU_EP0_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_ep0_cir_buff, 0, sizeof(hsu_ep0_log_type) * HSU_EP0_LOG_SIZE);
  hsu_ep0_cir_buff_index = 0;
}

#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
/*============================================================================
  hsu_task_schedule_log_init
==============================================================================*/
void hsu_task_schedule_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_task_schedule_cir_buff = (hsu_task_schedule_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_task_schedule_log_type) * (HSU_TASK_SCHEDULE_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_task_schedule_cir_buff, 0, sizeof(hsu_task_schedule_log_type) * HSU_TASK_SCHEDULE_LOG_SIZE);
  hsu_task_schedule_cir_buff_index = 0;
}
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */


#ifdef FEATURE_HS_USB_DEBUG_DTD
/*============================================================================
  hsu_dtd_log_init
==============================================================================*/
void hsu_dtd_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_dtd_cir_buff = (hsu_dtd_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_dtd_log_type) * (HSU_DTD_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_dtd_cir_buff, 0, sizeof(hsu_dtd_log_type) * HSU_DTD_LOG_SIZE);
  hsu_dtd_cir_buff_index = 0;
}
#endif /* FEATURE_HS_USB_DEBUG_DTD */

#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
/*============================================================================
  hsu_mem_log_init
==============================================================================*/
void hsu_mem_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_mem_cir_buff = (hsu_mem_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_mem_log_type) * (HSU_MEM_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_mem_cir_buff, 0, sizeof(hsu_mem_log_type) * HSU_MEM_LOG_SIZE);
  hsu_mem_cir_buff_index = 0;
}
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */

/*============================================================================
  hsu_timer_log_init
==============================================================================*/
void hsu_timer_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_timer_cir_buff = (hsu_timer_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_timer_log_type) * (HSU_TIMER_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_timer_cir_buff, 0, sizeof(hsu_timer_log_type) * HSU_TIMER_LOG_SIZE);
  hsu_timer_cir_buff_index = 0;
}

/*============================================================================
  hsu_reg_dump_log_init
==============================================================================*/
void hsu_reg_dump_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_reg_dump_cir_buff = (hsu_reg_dump_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_reg_dump_log_type) * (HSU_REG_DUMP_LOG_SIZE + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_reg_dump_cir_buff, 0, sizeof(hsu_reg_dump_log_type) * HSU_REG_DUMP_LOG_SIZE);
  hsu_reg_dump_cir_buff_index = 0;
}

/*============================================================================
  hsu_event_log_init
==============================================================================*/
void hsu_event_log_init(void)
{
#ifdef FEATURE_HS_USB_LOG_UNCACHED

  hsu_event_buf = (hsu_event_log_type *)
    hsu_conf_sel_alloc_uncached_memory(
      sizeof(hsu_event_log_type) * (NUMBER_OF_CORES * HSU_EVENT_LOG_NUM_ITEMS + 1));

#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_event_buf, 0, sizeof(hsu_event_log_type) * (NUMBER_OF_CORES * HSU_EVENT_LOG_NUM_ITEMS + 1));
}

/*============================================================================
 hsu_timetick_get64
==============================================================================*/
/*
static DalTimetickTime64Type hsu_timetick_get64(void)
{  
  DalTimetickTime64Type ticks;  
  static DalDeviceHandle *pTickHandle = NULL;  

  if (NULL == pTickHandle)  
  {    
    HSU_ASSERT(DAL_SUCCESS == DalTimetick_Attach( "SystemTimer", &pTickHandle));    
    HSU_ASSERT(pTickHandle);  
  }    

  HSU_ASSERT(DAL_SUCCESS == DalTimetick_GetTimetick64(pTickHandle, &ticks));    
  return ticks;
}
*/
/*============================================================================
 hsu_log_init 
==============================================================================*/
void hsu_log_init(void)
{
  uint32 timetick_ms;
  hsu_ulog_init();

#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF

#ifdef FEATURE_HS_USB_LOG_UNCACHED
  
    hsu_cir_buff = (hsu_generic_log_type *) 
      hsu_conf_sel_alloc_uncached_memory(
        sizeof(hsu_generic_log_type) * (HSU_GENERIC_LOG_SIZE + 1));
         
#endif /* FEATURE_HS_USB_LOG_UNCACHED */

  memset(hsu_cir_buff, 0, sizeof(hsu_generic_log_type) * HSU_GENERIC_LOG_SIZE);
  hsu_cir_buff_index = 0;

  hsu_snps_log_init();
  hsu_snps_ctrl_log_init();
  hsu_audio_log_init();
  hsu_bus_log_init();
  hsu_data_log_init();
  hsu_qmi_log_init();
  hsu_ser4_log_init();
  hsu_reg_log_init();
  hsu_func_log_init();
  hsu_dbg_e_log_init();
  
  HSU_USE_PARAM(hsu_audio_cir_buff_size);
  HSU_USE_PARAM(hsu_snps_ctrl_cir_buff_size);
  HSU_USE_PARAM(hsu_snps_cir_buff_size);
  HSU_USE_PARAM(hsu_cir_buff_size);
  HSU_USE_PARAM(hsu_bus_cir_buff_size);
  HSU_USE_PARAM(hsu_data_cir_buff_size);
  HSU_USE_PARAM(hsu_qmi_cir_buff_size);
  HSU_USE_PARAM(hsu_qmi_data_cir_buff_size);
  HSU_USE_PARAM(hsu_ser4_cir_buff_size);
  HSU_USE_PARAM(hsu_ser4_data_cir_buff_size);
  HSU_USE_PARAM(hsu_reg_cir_buff_size);
  HSU_USE_PARAM(hsu_func_cir_buff_size);
  HSU_USE_PARAM(hsu_dbg_e_cir_buff_size);
#endif


  hsu_dun_log_init();
  hsu_perf_log_init();
  hsu_vddmin_log_init();
  hsu_log_dbg_timer_init();
  hsu_ep0_log_init();
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  hsu_task_schedule_log_init();
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
  if (hsu_conf_sel_is_dtd_mem_logging_enabled())
  {
#ifdef FEATURE_HS_USB_DEBUG_DTD
    hsu_dtd_log_init();
#endif /* FEATURE_HS_USB_DEBUG_DTD */
#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
    hsu_mem_log_init();
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */
    //hsu_timer_log_init();
  }
  hsu_timer_log_init();
  hsu_reg_dump_log_init();
  hsu_event_log_init();


  HSU_USE_PARAM(hsu_perf_cir_buff_size);
  HSU_USE_PARAM(hsu_dun_cir_buff_size);

  HSU_USE_PARAM(hsu_vddmin_cir_buff_size);
  HSU_USE_PARAM(hsu_ep0_cir_buff_size);
#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
  HSU_USE_PARAM(hsu_task_schedule_cir_buff_size);
#endif /* FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */
#ifdef FEATURE_HS_USB_DEBUG_DTD
  HSU_USE_PARAM(hsu_dtd_cir_buff_size);
#endif /* FEATURE_HS_USB_DEBUG_DTD */
#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
  HSU_USE_PARAM(hsu_mem_cir_buff_size);
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */
  HSU_USE_PARAM(hsu_timer_cir_buff_size);
  HSU_USE_PARAM(hsu_reg_dump_cir_buff_size);
  HSU_USE_PARAM(hsu_event_num_items);

  /* Calculate and update timetick clock referece, i.e. 32Khz or 19.2Mhz */
  timetick_ms = timetick_get_ms();
  if (timetick_ms)
  {
    hsu_log_timetick_clk_khz = hsu_timetick_ulog_get64() / timetick_ms;
  }
  HSU_USE_PARAM(hsu_log_timetick_clk_khz);  

  hsu_log_timetick_32K = timetick_get_safe();
  hsu_log_timetick_19p2M = hsu_timetick_ulog_get64();//hsu_timetick_get64();
  HSU_USE_PARAM(hsu_log_timetick_32K);
  HSU_USE_PARAM(hsu_log_timetick_19p2M);
}

/*============================================================================
  hsu_dun_log
==============================================================================*/
void hsu_dun_log(uint32 id_0, void* buff_ptr, uint32 size)
{
  void* mem_ptr;
  uint32 alloc_size = size;
  //uint64 timetick64;
  if(buff_ptr && size)
  {
    if ((hsu_dun_log_config_info.rx_internal && (id_0 == DUN__RX__LOG)) 
        || (hsu_dun_log_config_info.tx_internal && (id_0 == DUN__TX__LOG)))
  {
    INTLOCK_SAVE(hsu_log_intlock_flag)
    hsu_dun_cir_buff[hsu_dun_cir_buff_index].timetick = hsu_timetick_ulog_get64();
    hsu_dun_cir_buff[hsu_dun_cir_buff_index].id_0 = (hsu_dun_log_enum)id_0;

    // Free up old data
    mem_ptr = (void *)hsu_dun_cir_buff[hsu_dun_cir_buff_index].buff_ptr;
    if (mem_ptr)
      jfree(mem_ptr);

    // Log first 256 bytes for raw data
    if(alloc_size > HSU_DUN_MAX_DATA_SIZE)
      alloc_size = HSU_DUN_MAX_DATA_SIZE;

    mem_ptr = jmalloc(alloc_size, M_ZERO); // Allocate new memory to store buffer data
    HSU_ASSERT(NULL != mem_ptr);
    memscpy(mem_ptr, alloc_size, buff_ptr, alloc_size); // Copy buffer into newly allocated memory

    hsu_dun_cir_buff[hsu_dun_cir_buff_index].buff_ptr = (char *)mem_ptr;
    hsu_dun_cir_buff[hsu_dun_cir_buff_index].buff_size = size;
    hsu_dun_cir_buff_index = (hsu_dun_cir_buff_index + 1) % HSU_DUN_LOG_SIZE;
    INTFREE_RESTORE(hsu_log_intlock_flag)
  }
    //timetick64 = hsu_timetick_ulog_get64();
    //HSU_SPRINTF_3("0x%08X%08X: hsu_dun_log (conti)", (uint32) (timetick64>>32), (uint32) timetick64, 0);
    if (hsu_dun_log_config_info.rx_f3 && (id_0 == DUN__RX__LOG))
    {
      HSU_SPRINTF_3("        RX   %4d   %s", (int)size, (char*)buff_ptr, 0);
    }

    if (hsu_dun_log_config_info.tx_f3 && (id_0 == DUN__TX__LOG))
    {
      HSU_SPRINTF_3("        TX   %4d   %s", (int)size, (char*)buff_ptr, 0);
    }
  }
}

/*============================================================================
  hsu_perf_log
==============================================================================*/
void hsu_perf_log(uint32 id_0, uint32 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_perf_cir_buff[hsu_perf_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_perf_cir_buff[hsu_perf_cir_buff_index].id_0 = (hsu_perf_log_enum)id_0;
  hsu_perf_cir_buff[hsu_perf_cir_buff_index].param_0 = param_0;
  hsu_perf_cir_buff_index = (hsu_perf_cir_buff_index + 1) % HSU_PERF_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}
#ifdef FEATURE_HS_USB_USE_LEGACY_CIR_BUF

/*============================================================================
  hsu_audio_log
==============================================================================*/
void hsu_audio_log(uint32 id_0, uint32 param_0, uint32 param_1)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_audio_cir_buff[hsu_audio_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_audio_cir_buff[hsu_audio_cir_buff_index].id_0 = (hsu_audio_log_enum)id_0;
  hsu_audio_cir_buff[hsu_audio_cir_buff_index].param_0 = param_0;
  hsu_audio_cir_buff[hsu_audio_cir_buff_index].param_1 = param_1;
  hsu_audio_cir_buff_index = (hsu_audio_cir_buff_index + 1) % HSU_AUDIO_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}


/*============================================================================
  hsu_snps_ctrl_log
==============================================================================*/
void hsu_snps_ctrl_log(uint32 id_0, uint32 param_0, uint32 param_1)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_snps_ctrl_cir_buff[hsu_snps_ctrl_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_snps_ctrl_cir_buff[hsu_snps_ctrl_cir_buff_index].id_0 = (hsu_snps_ctrl_log_enum)id_0;
  hsu_snps_ctrl_cir_buff[hsu_snps_ctrl_cir_buff_index].param_0 = param_0;
  hsu_snps_ctrl_cir_buff[hsu_snps_ctrl_cir_buff_index].param_1 = param_1;
  hsu_snps_ctrl_cir_buff_index = (hsu_snps_ctrl_cir_buff_index + 1) % HSU_SNPS_CTRL_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_log
==============================================================================*/
void hsu_log(uint32 id_0, uint32 offset_0, uint32 id_1, uint32 offset_1, uint32 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_cir_buff[hsu_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_cir_buff[hsu_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0 + offset_0);
  hsu_cir_buff[hsu_cir_buff_index].id_1 = (hsu_generic_log_enum)(id_1 + offset_1);
  hsu_cir_buff[hsu_cir_buff_index].param_0 = param_0;
  hsu_cir_buff_index = (hsu_cir_buff_index + 1) % HSU_GENERIC_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
  
  //HSU_MSG_HIGH_3("hsu_log( %u, %u, %u )", (id_0 + offset_0), (id_1 + offset_1), param_0);
}

/*============================================================================
  hsu_bus_log
==============================================================================*/
void hsu_bus_log(uint32 id_0, uint32 offset_0, uint32 id_1, uint32 offset_1, uint32 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_bus_cir_buff[hsu_bus_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_bus_cir_buff[hsu_bus_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0 + offset_0);
  hsu_bus_cir_buff[hsu_bus_cir_buff_index].id_1 = (hsu_generic_log_enum)(id_1 + offset_1);
  hsu_bus_cir_buff[hsu_bus_cir_buff_index].param_0 = param_0;
  hsu_bus_cir_buff_index = (hsu_bus_cir_buff_index + 1) % HSU_BUS_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_data_log
==============================================================================*/
void hsu_data_log(uint32 id_0, uint32 offset_0, uint32 id_1, uint32 offset_1, uint32 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_data_cir_buff[hsu_data_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_data_cir_buff[hsu_data_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0 + offset_0);
  hsu_data_cir_buff[hsu_data_cir_buff_index].id_1 = (hsu_generic_log_enum)(id_1 + offset_1);
  hsu_data_cir_buff[hsu_data_cir_buff_index].param_0 = param_0;
  hsu_data_cir_buff_index = (hsu_data_cir_buff_index + 1) % HSU_DATA_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_qmi_register_iface_log
==============================================================================*/
void hsu_qmi_register_iface_log(uint8 rmnet_interface, uint8 intr_in_ep, uint8 bulk_out_ep, uint8 bulk_in_ep)
{
  uint32 i = 0;
  for (i = 0; i < HS_USB_NUM_OF_ECM_INST; i++)
  {
    if ((rmnet_interface == hsu_qmi_iface_table[i].iface_num) || (0 == hsu_qmi_iface_table[i].iface_num))
    {
      break; /* already populated or found empty element */
    }
  }
  if (i < HS_USB_NUM_OF_ECM_INST)
  {
    if (0 == hsu_qmi_iface_table[i].iface_num) /* populated if not already populated */
    {
      hsu_qmi_iface_table[i].iface_num      = rmnet_interface;
      hsu_qmi_iface_table[i].intr_in_ep     = intr_in_ep;
      hsu_qmi_iface_table[i].bulk_out_ep    = bulk_out_ep;
      hsu_qmi_iface_table[i].bulk_in_ep     = bulk_in_ep;
      hsu_qmi_iface_table[i].iface_cir_buff = hsu_qmi_cir_buff + (i * HSU_QMI_LOG_SIZE);
      hsu_qmi_iface_table[i].qmi_cir_buff_max_size = HSU_QMI_LOG_SIZE;

      hsu_qmi_endpt_iface_table[rmnet_interface] = &hsu_qmi_iface_table[i];
    }
  }
  else
  {
    HSU_ERR_FATAL("hsu_qmi_iface_table allocation out of bounds", 0, 0, 0);
  }
}

/*============================================================================
  hsu_qmi_log
==============================================================================*/
void hsu_qmi_log(uint32 id_0, uint8 rmnet_interface, uint32 param_0)
{
  /* enforce the paradigm where caller must sanitize the input parameters */
  hsu_qmi_log_type *hsu_qmi_iface_cir_buff = NULL; /* 2D array is not equal to pointer-to-pointer */
  uint32 timetick_cache = hsu_timetick_ulog_get64(); /* such that broadcast would have a single timestamp */
  uint32 cir_buf_index = 0;

  INTLOCK_SAVE(hsu_log_intlock_flag)
  if (rmnet_interface >= HSU_LOG_MAX_NUM_OF_ENDPTS)
  {
    /* out of bounds checking before using lookup table */
    HSU_ERR_FATAL("hsu_qmi_log: rmnet_interface OUT OF BOUNDS", 0, 0, 0);
  }
  if (hsu_qmi_endpt_iface_table[rmnet_interface])
  {
    cir_buf_index = (hsu_qmi_endpt_iface_table[rmnet_interface])->qmi_cir_buff_index;
    hsu_qmi_iface_cir_buff = (hsu_qmi_endpt_iface_table[rmnet_interface])->iface_cir_buff;

    hsu_qmi_iface_cir_buff[cir_buf_index].timetick = timetick_cache;
    hsu_qmi_iface_cir_buff[cir_buf_index].id_0 = (hsu_generic_log_enum)(id_0);
    hsu_qmi_iface_cir_buff[cir_buf_index].rmnet_interface = rmnet_interface;
    hsu_qmi_iface_cir_buff[cir_buf_index].param_0 = param_0;
    (hsu_qmi_endpt_iface_table[rmnet_interface])->qmi_cir_buff_index = (cir_buf_index + 1) % HSU_QMI_LOG_SIZE;
  }
  /* else discard messages */
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_qmi_data_log
==============================================================================*/
void hsu_qmi_data_log(uint8 rmnet_interface, void *data, uint8 size)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_qmi_log(HSU_QMI_DATA_LOG, rmnet_interface, hsu_qmi_data_cir_buff_index);

  hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  PACK4(hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].qmi_data[0], data, 0);
  PACK4(hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].qmi_data[1], data, 4);
  PACK4(hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].qmi_data[2], data, 8);
  PACK4(hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].qmi_data[3], data, 12);
  PACK4(hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].qmi_data[4], data, 16);
  hsu_qmi_data_cir_buff[hsu_qmi_data_cir_buff_index].size = size;

  hsu_qmi_data_cir_buff_index = (hsu_qmi_data_cir_buff_index + 1) % HSU_QMI_DATA_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_ser4_log
==============================================================================*/
void hsu_ser4_log(uint32 id_0, uint32 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_ser4_cir_buff[hsu_ser4_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_ser4_cir_buff[hsu_ser4_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0);
  hsu_ser4_cir_buff[hsu_ser4_cir_buff_index].param_0 = param_0;
  hsu_ser4_cir_buff_index = (hsu_ser4_cir_buff_index + 1) % HSU_SER4_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_ser4_data_log
==============================================================================*/
void hsu_ser4_data_log(void *data)
{
  hsu_ser4_log(SER4_PACKET_DATA_LOG, hsu_ser4_data_cir_buff_index);
  
  INTLOCK_SAVE(hsu_log_intlock_flag)
  PACK4(hsu_ser4_data_cir_buff[hsu_ser4_data_cir_buff_index].ser4_data[0], data, 0);
  PACK4(hsu_ser4_data_cir_buff[hsu_ser4_data_cir_buff_index].ser4_data[1], data, 4);
  PACK4(hsu_ser4_data_cir_buff[hsu_ser4_data_cir_buff_index].ser4_data[2], data, 8);
  PACK4(hsu_ser4_data_cir_buff[hsu_ser4_data_cir_buff_index].ser4_data[3], data, 12);
  
  hsu_ser4_data_cir_buff_index = (hsu_ser4_data_cir_buff_index + 1) % HSU_SER4_DATA_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_reg_log
  Address input must be virtual address
  The function to translate virtual to physical address is not implemented
==============================================================================*/
void hsu_reg_log(uint32 addr, uint32 reg_val)
{
  if (is_reg_log_on)
  {
    INTLOCK_SAVE(hsu_log_intlock_flag)
    hsu_reg_cir_buff[hsu_reg_cir_buff_index].timetick = hsu_timetick_ulog_get64();
    hsu_reg_cir_buff[hsu_reg_cir_buff_index].addr = (void*)addr;
    hsu_reg_cir_buff[hsu_reg_cir_buff_index].reg_val = (void*)reg_val;
    hsu_reg_cir_buff_index = (hsu_reg_cir_buff_index + 1) % HSU_REG_LOG_SIZE;
    INTFREE_RESTORE(hsu_log_intlock_flag)
  }
}

/*============================================================================
  hsu_stop_reg_log
==============================================================================*/
void hsu_stop_reg_log(void)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  is_reg_log_on = FALSE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_func_log
==============================================================================*/
void hsu_func_log(uint32 id_0, uint32 id_1)
{
  if (is_func_log_on)
  {
    INTLOCK_SAVE(hsu_log_intlock_flag)
    hsu_func_cir_buff[hsu_func_cir_buff_index].timetick = hsu_timetick_ulog_get64();
    hsu_func_cir_buff[hsu_func_cir_buff_index].id_0 = (hsu_generic_log_enum)id_0;
    hsu_func_cir_buff[hsu_func_cir_buff_index].id_1 = (hsu_generic_log_enum)id_1;
    hsu_func_cir_buff_index = (hsu_func_cir_buff_index + 1) % HSU_FUNC_LOG_SIZE;
    INTFREE_RESTORE(hsu_log_intlock_flag)
  }
}

/*============================================================================
  hsu_stop_func_log
==============================================================================*/
void hsu_stop_func_log(void)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  is_func_log_on = FALSE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_dbg_e_log
==============================================================================*/
/* regular version of debug log requiring a memcpy */
void hsu_dbg_e_log(void *data, uint16 buf_size)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  if (data)
  {
    hsu_dbg_e_cir_buff[hsu_dbg_e_cir_buff_index].timetick = hsu_timetick_ulog_get64();
    memscpy(&(hsu_dbg_e_cir_buff[hsu_dbg_e_cir_buff_index].str_data), buf_size, data, buf_size);
    hsu_dbg_e_cir_buff_index = (hsu_dbg_e_cir_buff_index + 1) % HSU_DBG_E_LOG_SIZE;
  }
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_dbg_e_get_client_buffer_log
==============================================================================*/
/* Optimized version that passes back a buffer location */
void hsu_dbg_e_get_client_buffer_log(void **client_buf_ptr)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_dbg_e_cir_buff[hsu_dbg_e_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  /* reserve a slot in the circular buffer to be filled by the client */
  *client_buf_ptr = &(hsu_dbg_e_cir_buff[hsu_dbg_e_cir_buff_index].str_data);
  hsu_dbg_e_cir_buff_index = (hsu_dbg_e_cir_buff_index + 1) % HSU_DBG_E_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_dbg_e_log_ext
==============================================================================*/
void hsu_dbg_e_log_ext(const char* str, ...)
{
  va_list arg_list;
  char *str_output = NULL;

  hsu_dbg_e_get_client_buffer_log((void*)&str_output);
  /*lint -e{530}
  ** The ARM implementation of va_start raises Lint errors.
  */
  va_start(arg_list, str);
  vsnprintf(str_output, HSU_DBG_E_LOG_STR_SIZE, str, arg_list);
  va_end(arg_list);
}

#else  /*FEATURE_HS_USB_USE_LEGACY_CIR_BUF*/


void hsu_log(uint32 id_0, uint32 offset_0, uint32 id_1, uint32 offset_1, uint32 param_0){};
void hsu_audio_log(uint32 id_0, uint32 param_0, uint32 param_1){};

void hsu_snps_ctrl_log(uint32 id_0, uint32 param_0, uint32 param_1){};
void hsu_bus_log(uint32 id_0, uint32 offset_0, uint32 id_1, uint32 offset_1, uint32 param_0){};
void hsu_data_log(uint32 id_0, uint32 offset_0, uint32 id_1, uint32 offset_1, uint32 param_0){};
void hsu_qmi_register_iface_log(uint8 rmnet_interface, uint8 intr_in_ep, uint8 bulk_out_ep, uint8 bulk_in_ep){};
void hsu_qmi_log(uint32 id_0, uint8 rmnet_interface, uint32 param_0){};
void hsu_qmi_data_log(uint8 rmnet_interface, void *data, uint8 size){};
void hsu_ser4_log(uint32 id_0, uint32 param_0){};
void hsu_ser4_data_log(void *data){};

void hsu_reg_log(uint32 addr, uint32 reg_val){};
void hsu_stop_reg_log(void){};
void hsu_func_log(uint32 id_0, uint32 id_1){};
void hsu_stop_func_log(void){};

#endif /*else FEATURE_HS_USB_USE_LEGACY_CIR_BUF*/


/*============================================================================
  hsu_ep0_log
==============================================================================*/
void hsu_ep0_log(uint32 id_0, uint8 rmnet_interface, uint8 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  if (0 == rmnet_interface) /* log control interface here */
  {
    hsu_ep0_cir_buff[hsu_ep0_cir_buff_index].timetick = hsu_timetick_ulog_get64();//timetick_get_safe();
    hsu_ep0_cir_buff[hsu_ep0_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0);
    hsu_ep0_cir_buff[hsu_ep0_cir_buff_index].param_0 = param_0;
    hsu_ep0_cir_buff_index = (hsu_ep0_cir_buff_index + 1) % HSU_EP0_LOG_SIZE;
  }
  else
  {
    /* simply reroute to appropriate interface */
    hsu_qmi_log(id_0, rmnet_interface, 0);
  }
  INTFREE_RESTORE(hsu_log_intlock_flag)
}


/*============================================================================
  hsu_vddmin_log
==============================================================================*/
void hsu_vddmin_log(uint32 id_0, uint32 param_0)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_vddmin_cir_buff[hsu_vddmin_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_vddmin_cir_buff[hsu_vddmin_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0);
  hsu_vddmin_cir_buff[hsu_vddmin_cir_buff_index].param_0 = param_0;
  hsu_vddmin_cir_buff_index = (hsu_vddmin_cir_buff_index + 1) % HSU_VDDMIN_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

#ifdef FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE
/*============================================================================
  hsu_task_schedule_log
==============================================================================*/
void hsu_task_schedule_log(uint32 id_0, int32 prev_count)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_task_schedule_cir_buff[hsu_task_schedule_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_task_schedule_cir_buff[hsu_task_schedule_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0);
  hsu_task_schedule_cir_buff[hsu_task_schedule_cir_buff_index].prev_count = prev_count;
  hsu_task_schedule_cir_buff[hsu_task_schedule_cir_buff_index].rex_tcb = rex_self();
  hsu_task_schedule_cir_buff_index = (hsu_task_schedule_cir_buff_index + 1) % HSU_TASK_SCHEDULE_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}
#endif /* ~FEATURE_HS_USB_DEBUG_JTASK_SCHEDULE */

#ifdef FEATURE_HS_USB_DEBUG_DTD
/*============================================================================
  hsu_dtd_log
==============================================================================*/
void hsu_dtd_log(uint32 id_0, void* ep, void* dtd)
{
  if (hsu_conf_sel_is_dtd_mem_logging_enabled())
  {
    hsu_dtd_struct_type* hsu_dtd = NULL;
    INTLOCK_SAVE(hsu_log_intlock_flag)
    hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].timetick = hsu_timetick_ulog_get64();
    hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0);
    hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].rex_tcb = rex_self();
    hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].ep = ep;
    if (dtd)
    {
      hsu_dtd = (hsu_dtd_struct_type*)dtd;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd_addr = dtd;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.next_td_ptr = hsu_dtd->next_td_ptr;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.td_header = hsu_dtd->td_header;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[0] = hsu_dtd->buf_ptr[0];
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[1] = hsu_dtd->buf_ptr[1];
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[2] = hsu_dtd->buf_ptr[2];
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[3] = hsu_dtd->buf_ptr[3];
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[4] = hsu_dtd->buf_ptr[4];
    }
    else
    {
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd_addr = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.next_td_ptr = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.td_header = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[0] = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[1] = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[2] = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[3] = 0;
      hsu_dtd_cir_buff[hsu_dtd_cir_buff_index].dtd.buf_ptr[4] = 0;
    }
    hsu_dtd_cir_buff_index = (hsu_dtd_cir_buff_index + 1) % HSU_DTD_LOG_SIZE;
    INTFREE_RESTORE(hsu_log_intlock_flag)
    if ((hsu_dtd != NULL) && (0xAABB00CC == hsu_dtd->next_td_ptr))
    {
      HSU_ERR_FATAL("USB TD corruption detected", 0, 0, 0);
    }
  }
}
#endif /* FEATURE_HS_USB_DEBUG_DTD */

#ifdef FEATURE_HS_USB_DEBUG_MEM_ALLOC
/*============================================================================
  hsu_mem_log
==============================================================================*/
void hsu_mem_log(uint32 id_0, void* mem_loc, uint32 size)
{
  if (hsu_conf_sel_is_dtd_mem_logging_enabled())
  {
    INTLOCK_SAVE(hsu_log_intlock_flag)
    hsu_mem_cir_buff[hsu_mem_cir_buff_index].timetick = hsu_timetick_ulog_get64();
    hsu_mem_cir_buff[hsu_mem_cir_buff_index].id_0 = (hsu_generic_log_enum)(id_0);
    hsu_mem_cir_buff[hsu_mem_cir_buff_index].rex_tcb = rex_self();
    hsu_mem_cir_buff[hsu_mem_cir_buff_index].mem_loc = mem_loc;
    hsu_mem_cir_buff[hsu_mem_cir_buff_index].size = size;
    hsu_mem_cir_buff_index = (hsu_mem_cir_buff_index + 1) % HSU_MEM_LOG_SIZE;
    INTFREE_RESTORE(hsu_log_intlock_flag)
  }
}
#endif /* FEATURE_HS_USB_DEBUG_MEM_ALLOC */

/*============================================================================
  hsu_timer_log
==============================================================================*/
void hsu_timer_log(hsu_timer_type *timer_ptr)
{
  static uint32 hsu_timer_last_entry_ms = 0;
  uint32 current_time_ms;
  uint32 time_diff_ms;
  
  current_time_ms = timetick_get_ms();
  time_diff_ms = current_time_ms - hsu_timer_last_entry_ms;
  hsu_timer_last_entry_ms = current_time_ms;

  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_timer_cir_buff[hsu_timer_cir_buff_index].timetick = hsu_timetick_ulog_get64();
  hsu_timer_cir_buff[hsu_timer_cir_buff_index].timer_ptr = timer_ptr;
  hsu_timer_cir_buff[hsu_timer_cir_buff_index].time_diff_ms = time_diff_ms;
  hsu_timer_cir_buff_index = (hsu_timer_cir_buff_index + 1) % HSU_TIMER_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_reg_dump_log
==============================================================================*/
void hsu_reg_dump_log(uint32 addr, uint32 reg_val)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  hsu_reg_dump_cir_buff[hsu_reg_dump_cir_buff_index].addr = (void*)addr;
  hsu_reg_dump_cir_buff[hsu_reg_dump_cir_buff_index].reg_val = (void*)reg_val;
  hsu_reg_dump_cir_buff_index = (hsu_reg_dump_cir_buff_index + 1) % HSU_REG_DUMP_LOG_SIZE;
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/* Debug Timer */
/*============================================================================
  hsu_log_dbg_timer_1_cb
==============================================================================*/
static void hsu_log_dbg_timer_1_cb(timer_cb_data_type data)
{
  HSU_ERR_FATAL("hsu_log_dbg_timer_1_cb() TIMER EXPIRY: %d", (uint32)data, 0, 0);
}

static void hsu_log_dbg_timer_2_cb(timer_cb_data_type data)
{
  HSU_ERR_FATAL("hsu_log_dbg_timer_2_cb() TIMER EXPIRY: %d", (uint32)data, 0, 0);
}

static void hsu_log_dbg_timer_init(void)
{
  hsu_log_dbg_timer_1 = hsu_timer_init(
    HSU_TIMER_CID__LOG_DBG_1,
    HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
    hsu_log_dbg_timer_1_cb,
    (timer_cb_data_type)NULL);

  hsu_log_dbg_timer_2 = hsu_timer_init(
    HSU_TIMER_CID__LOG_DBG_2,
    HSU_TIMER_CB_IN_HSU_AL_TASK_WITH_GIANT_MUTEX,
    hsu_log_dbg_timer_2_cb,
    (timer_cb_data_type)NULL);  
}

/*============================================================================
  hsu_log_dbg_timer_1_start
==============================================================================*/
void hsu_log_dbg_timer_1_start(timetick_type timeout_tmr_value)
{
  /* Start request timeout timer */
  if (timeout_tmr_1_value)
  {
    HSU_ERR_FATAL("hsu_log_dbg_timer_1_start() Unable to reschedule as timer already in use", 0, 0, 0); /* timer already in use */
  }
  timeout_tmr_1_value = timeout_tmr_value;
  hsu_timer_set(
    &hsu_log_dbg_timer_1,
    timeout_tmr_1_value,
    T_MSEC);
}

/*============================================================================
  hsu_log_dbg_timer_2_start
==============================================================================*/
void hsu_log_dbg_timer_2_start(timetick_type timeout_tmr_value)
{
  /* Start request timeout timer */
  if (timeout_tmr_2_value)
  {
    HSU_ERR_FATAL("hsu_log_dbg_timer_2_start() Unable to reschedule as timer already in use", 0, 0, 0); /* timer already in use */
  }
  timeout_tmr_2_value = timeout_tmr_value;
  hsu_timer_set(
    &hsu_log_dbg_timer_2,
    timeout_tmr_2_value,
    T_MSEC);
}

/*============================================================================
  hsu_log_dbg_timer_1_cancel
==============================================================================*/
void hsu_log_dbg_timer_1_cancel(void)
{
  /* Cancel request timeout timer */
  if (timeout_tmr_1_value != 0)
  {
    hsu_timer_cancel(&hsu_log_dbg_timer_1);
    timeout_tmr_1_value = 0;
  }
  /* else there is no timer to cancel.  Function is safe to be called multiple times */
}

/*============================================================================
  hsu_log_dbg_timer_2_cancel
==============================================================================*/
void hsu_log_dbg_timer_2_cancel(void)
{
  /* Cancel request timeout timer */
  if (timeout_tmr_2_value != 0)
  {
    hsu_timer_cancel(&hsu_log_dbg_timer_2);
    timeout_tmr_2_value = 0;
  }
  /* else there is no timer to cancel.  Function is safe to be called multiple times */
}

/*============================================================================
  hsu_vddmin_log_print_f3
==============================================================================*/
#if 0
void hsu_vddmin_log_print_f3(void)
{
  uint32 index;
  uint32 timetickU32, timetickL32;
  for (index = 0; index < HSU_VDDMIN_LOG_SIZE; index++)
  {
    timetickU32 = (uint32) (hsu_vddmin_cir_buff[index].timetick >> 32);
    timetickL32 = (uint32) hsu_vddmin_cir_buff[index].timetick;
    switch (hsu_vddmin_cir_buff[index].id_0)
    {
      case HSU_VDDMIN_ENTER_LOG:
      {
        HSU_MSG_HIGH_3("%4u : %8u%8u ms:  vdd_min enter",
          index, timetickU32, timetickL32);
      }
      break;

      case HSU_VDDMIN_EXIT_LOG:
      {
        HSU_MSG_HIGH_3("%4u : %8u%8u ms:  vdd_min exit",
          index, timetickU32, timetickL32);

      }
      break;

      case HSU_VDDMIN_RPM_REGISTERED:
      {
        HSU_MSG_HIGH_3("%4u : %8u%8u ms:  vdd_min rpm registration success",
          index, timetickU32, timetickL32);

      }
      break;
 
      case HSU_VDDMIN_ERROR1:
      {
        HSU_MSG_HIGH_3("%4u : %8u%8u ms:  vdd_min error #1",
          index, timetickU32, timetickL32);

      }
      break;
 
      case HSU_VDDMIN_ERROR2:
      {
        HSU_MSG_HIGH_3("%4u : %8u%8u ms:  vdd_min error #2",
          index, timetickU32, timetickL32);

      }
      break;

      default:
      {
        HSU_MSG_HIGH_3("%4u : %8u%8u ms:  vdd_min unknown enum (cont)",
          index, timetickU32, timetickL32);
        HSU_MSG_HIGH_1("vdd_min unknown enum %u",
          hsu_vddmin_cir_buff[index].id_0);
      }
      break;
      
    }/* end of switch */
    
  } /* end of for-loop */
  
}
#endif
/*============================================================================
  hsu_log_print_f3_event_logs
==============================================================================*/
static void hsu_log_print_f3_event_logs(uint32 core_index)
{
  HSU_DIAG_MSG_HIGH("-------------------------------------------------");
  HSU_DIAG_MSG_HIGH("   count : last timetick  : event");
  HSU_DIAG_MSG_HIGH("-------------------------------------------------");

  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_ON___HS_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_OFF__HS_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_ON___FS_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_OFF__FS_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_ON___UNKNOWN_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_OFF__UNKNOWN_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_ON___NOT_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLK_OFF__NOT_CONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_LPM_ENTER_DISCONNECTED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_LPM_ENTER_SUSPEND, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_LPM_EXIT, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_REMOTE_WAKEUP, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_REMOTE_WAKEUP_FAIL, core_index);

  HSU_EVT_LOG_TO_MSG_HIGH(EVT_BAM_TOP_LEVEL_ERROR_IGNORED, core_index);

  HSU_EVT_LOG_TO_MSG_HIGH(EVT_HIGH_DTE_READY, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_LOW_DTE_READY, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_FLOW_ENABLED_STATE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_FLOW_DISABLING_FROM_FORCE_SHUTDOWN, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_FLOW_DISABLED_FROM_FORCE_SHUTDOWN, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_DATA_ACTIVITY_WHILE_IN_FORCE_SHUTDOWN, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_ENUMERATION_WITH_AP, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_FLOW_DISABLING_FROM_SUSPEND, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_FLOW_DISABLED_FROM_SUSPEND, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_DATA_ACTIVITY_WHILE_SUSPENDED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_BOOTUP_FLOW_DISABLED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_HSU_CDC_HANDLE_ENCAPSULATED_COMMAND, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_HSU_NOTIFY_RESPONSE_AVAILABLE_WITH_INT_EP_SHARING, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_HSU_NOTIFY_RESPONSE_AVAILABLE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_HSU_CDC_HANDLE_ENCAPSULATED_RESPONSE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_IPA_PM_REQUSET_CLK, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_IPA_PM_RELEASE_CLK, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_SEND_FUNCTION_WAKE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_ISSUE_REMOTE_WAKEUP_DUE_TO_FUNC_WAKE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_CLIENT_REQUESTED_FUNCTION_WAKE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_RESTART_FUNCTION_WAKE_TNOTIF_TIMEOUT, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_ESUSPENDED_CDC_READ_WRITE_INFINITE_SKIPPED, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_DPL_DROP_TX_CTRL_PKTS, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_SS_PHY_NOT_FUNCTIONAL, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_MISSED_ISOC_INTERVAL, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_WAIT_FOR_ZLP_TO_COMPLETE_IN_REQ, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_INVALID_REQUEST_STATUS, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_UNABLE_TO_STOP_GSI_CHANNEL_DUE_TO_BUS_IN_U0_A, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_UNABLE_TO_STOP_GSI_CHANNEL_DUE_TO_BUS_IN_U0_B, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_LINK_STATE_STILL_IN_U0, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_PMIC_VBUS_HIGH_DUPLICATE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_PMIC_VBUS_LOW_DUPLICATE, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_L1_ENTER, core_index);
  HSU_EVT_LOG_TO_MSG_HIGH(EVT_L1_EXIT, core_index);
}


/*============================================================================
  hsu_log_print_f3_curr_speed
==============================================================================*/
void hsu_log_print_f3_curr_speed(uint32 core_index)
{
  switch (core_get_curr_speed(hsu_os_bus_hal_core_id(core_index)))
  {
    case SPEED_SUPER:
      HSU_DIAG_MSG_HIGH_1("current speed: " HSU_ENUM_TO_STR(SPEED_SUPER) " (%u)", SPEED_SUPER);
      break;

    case SPEED_HIGH:
      HSU_DIAG_MSG_HIGH_1("current speed: " HSU_ENUM_TO_STR(SPEED_HIGH) " (%u)", SPEED_HIGH);
      break;

    case SPEED_FULL:
      HSU_DIAG_MSG_HIGH_1("current speed: " HSU_ENUM_TO_STR(SPEED_FULL) " (%u)", SPEED_FULL);
      break;

    default:
      HSU_DIAG_MSG_HIGH_1("current speed: " HSU_ENUM_TO_STR(SPEED_UNKNOWN) " (%u)", SPEED_UNKNOWN);
      break;
  }  
}


/*============================================================================
  hsu_log_get_pid_from_core_index
==============================================================================*/
uint16 hsu_log_get_pid_from_core_index(uint32 core_index)
{
  uint16                              pid = 0;
  struct hsu_device_params_type     * core_conf_sel_info;
  const hsu_conf_sel_comp_info_type * comp_info;

  core_conf_sel_info = 
    hsu_conf_sel_core_config_get_core_conf_sel_info(hsu_os_bus_hal_core_id(core_index));

  if (core_conf_sel_info)
  {
    comp_info = core_conf_sel_info->configs_arr[HSU_CONFIG_IDX_0].cur_dev_comp_details;
    if (comp_info)
    {
      pid = comp_info->product_id;
    }
  }

  return pid;
}


/*============================================================================
  hsu_log_platform_info
==============================================================================*/
void hsu_log_platform_info(void)
{
  hsu_platform_info_type const * platform = hsu_platform_get_info();

  HSU_ASSERT(platform != NULL);

  switch (platform->hsu_platform_id)
  {
    case HSU_PLATFORM_STANDALONE:
      HSU_DIAG_MSG_HIGH_2("platform: " HSU_ENUM_TO_STR(HSU_PLATFORM_STANDALONE) 
        " (%u), nVer: (0x%X)", platform->hsu_platform_id, platform->nVer);
      break;
    case HSU_PLATFORM_M_FUSION:
      HSU_DIAG_MSG_HIGH_2("platform: " HSU_ENUM_TO_STR(HSU_PLATFORM_M_FUSION) 
        " (%u), nVer: (0x%X)", platform->hsu_platform_id, platform->nVer);
      break;
    case HSU_PLATFORM_LA_FUSION:
      HSU_DIAG_MSG_HIGH_2("platform: " HSU_ENUM_TO_STR(HSU_PLATFORM_LA_FUSION) 
        " (%u), nVer: (0x%X)", platform->hsu_platform_id, platform->nVer);
      break;
    case HSU_PLATFORM_GOBI_FUSION:
      HSU_DIAG_MSG_HIGH_2("platform: " HSU_ENUM_TO_STR(HSU_PLATFORM_GOBI_FUSION) 
        " (%u), nVer: (0x%X)", platform->hsu_platform_id, platform->nVer);
      break;
    case HSU_PLATFORM_RUMI:
      HSU_DIAG_MSG_HIGH_2("platform: " HSU_ENUM_TO_STR(HSU_PLATFORM_RUMI) 
        " (%u), nVer: (0x%X)", platform->hsu_platform_id, platform->nVer);
      break;
    case HSU_PLATFORM_VIRTIO:
      HSU_DIAG_MSG_HIGH_2("platform: " HSU_ENUM_TO_STR(HSU_PLATFORM_VIRTIO) 
        " (%u), nVer: (0x%X)", platform->hsu_platform_id, platform->nVer);
      break;
    default:
      HSU_DIAG_MSG_HIGH_2("platform: unknown (%u), nVer: (0x%X)", 
        platform->hsu_platform_id, platform);
  }

  switch (platform->phy_id)
  {
    case HSU_PHY_AT:
      HSU_DIAG_MSG_HIGH_1("phy_id: " HSU_ENUM_TO_STR(HSU_PHY_AT)
        " phy_id (0x%X)", platform->phy_id);
      break;
    case HSU_PHY_BG:
      HSU_DIAG_MSG_HIGH_1("phy_id: " HSU_ENUM_TO_STR(HSU_PHY_BG)
        " phy_id (0x%X)", platform->phy_id);
      break;
    case HSU_PHY_CS:
      HSU_DIAG_MSG_HIGH_1("phy_id: " HSU_ENUM_TO_STR(HSU_PHY_CS)
        " phy_id (0x%X)", platform->phy_id);
      break;
    case HSU_PHY_DI:
      HSU_DIAG_MSG_HIGH_1("phy_id: " HSU_ENUM_TO_STR(HSU_PHY_DI)
        " phy_id (0x%X)", platform->phy_id);
      break;
    case HSU_PHY_EU:
      HSU_DIAG_MSG_HIGH_1("phy_id: " HSU_ENUM_TO_STR(HSU_PHY_EU)
        " phy_id (0x%X)", platform->phy_id);
      break;
    default:
      HSU_DIAG_MSG_HIGH_1("phy_id: unknown phy_id: (0x%X)", 
        platform->phy_id);
      break;
  }
}

/*============================================================================
  hsu_usb_core_info_log_print_f3
==============================================================================*/
void hsu_usb_core_info_log_print_f3(void)
{
  uint64 timetick64 = hsu_timetick_ulog_get64();
  HSU_DIAG_MSG_HIGH("====================================================");
  hsu_log_platform_info();
  
  HSU_DIAG_MSG_HIGH("====================================================");
  HSU_DIAG_MSG_HIGH_2("USB Core Information:  (current timetick = %u%u)", (uint32)(timetick64>>32), (uint32) timetick64);
  
  if (hsu_conf_sel_hs_main_core_enabled())
  {
    if (hsu_conf_sel_is_usb_core_snps())
    {
      HSU_DIAG_MSG_HIGH("====================================================");

      HSU_MSG_HIGH_2("USB Core: Synopsys (USB3) - Index: %u, PID: 0x%X", 
        USB3_CORE_INDEX, hsu_log_get_pid_from_core_index(USB3_CORE_INDEX));
      HSU_MSG_HIGH("====================================================");
    
      hsu_log_print_f3_curr_speed(USB3_CORE_INDEX);
      hsu_log_print_f3_event_logs(USB3_CORE_INDEX);
    }
  }

  //hsu_al_ecm_print_f3();
  //hsu_al_dsm_utils_f3();

  HSU_DIAG_MSG_HIGH("======================= END ========================");
}

/*============================================================================
  hsu_print_dun_log
==============================================================================*/
void hsu_print_dun_log(void)
{
  uint32 index;
  uint32 i;

  HSU_MSG_HIGH("===================== DUN LOG =======================");
  HSU_MSG_HIGH("Timestamp : ");
  HSU_MSG_HIGH("            Dir  Size   Data");
  for(i=0, index = (hsu_dun_cir_buff_index)% HSU_DUN_LOG_SIZE;
      i < HSU_DUN_LOG_SIZE;
      i++, index = (index + 1)% HSU_DUN_LOG_SIZE)
  {
    switch(hsu_dun_cir_buff[index].id_0)
    {
      case DUN__TX__LOG:
        HSU_SPRINTF_3("0x%08%08X: ", (uint32)(hsu_dun_cir_buff[index].timetick>>32), (uint32)(hsu_dun_cir_buff[index].timetick), 0);
        HSU_SPRINTF_3("        TX    %4d    %s", (int)hsu_dun_cir_buff[index].buff_size, hsu_dun_cir_buff[index].buff_ptr, 0);
      break;

      case DUN__RX__LOG:
        HSU_SPRINTF_3("0x%08%08X: ", (uint32)(hsu_dun_cir_buff[index].timetick>>32), (uint32)(hsu_dun_cir_buff[index].timetick), 0);
        HSU_SPRINTF_3("        RX    %4d    %s", (int)hsu_dun_cir_buff[index].buff_size, hsu_dun_cir_buff[index].buff_ptr, 0);
      break;

      default:
      break;
    }
  }
  HSU_MSG_HIGH("======================= END ========================");
}


/*============================================================================
  hsu_event_log
==============================================================================*/
/* core_id is zero based and uses logical core numbers */
/* HS_USB_CORE_INDEX */
void hsu_event_log(hsu_event_log_enum id_0, uint32 core_id)
{
  INTLOCK_SAVE(hsu_log_intlock_flag)
  HSU_ASSERT(((uint32)(id_0) != 0) && ((uint32)(id_0) <= HSU_EVENT_LOG_NUM_ITEMS));
  if (core_id < NUMBER_OF_CORES)
  {
    /* id_0 requires minus 1 because buffer indices commences with 0 */
    hsu_event_log_type *log_ptr = &(hsu_event_buf[((uint32)(id_0) - 1) + (HSU_EVENT_LOG_NUM_ITEMS) * core_id]);
    log_ptr->timetick[log_ptr->log_depth_index] = log_ptr->last_event_timetick = hsu_timetick_ulog_get64();
    log_ptr->id_0 = id_0;
    log_ptr->core_idx = core_id;
    log_ptr->total_cnt = log_ptr->total_cnt + 1;
    log_ptr->log_depth_index = (log_ptr->log_depth_index + 1) % HSU_EVENT_LOG_DEPTH;
  }
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

/*============================================================================
  hsu_get_event_total_cnt
==============================================================================*/
uint32 hsu_get_event_total_cnt(hsu_event_log_enum id_0, uint32 core_id)
{
  // Since this is a read only API for informational purposes only, synchronization is optional.
  uint32 total_cnt = 0;

  HSU_ASSERT(((uint32)(id_0) != 0) && ((uint32)(id_0) <= HSU_EVENT_LOG_NUM_ITEMS));
  if (core_id < NUMBER_OF_CORES)
  {
    /* id_0 requires minus 1 because buffer indices commences with 0 */
    hsu_event_log_type *log_ptr = &(hsu_event_buf[((uint32)(id_0) - 1) + (HSU_EVENT_LOG_NUM_ITEMS) * core_id]);
    total_cnt = log_ptr->total_cnt;
  } 
  return total_cnt;
}

#ifdef FEATURE_HS_USB_TRACE
void hsu_trace_ep_set(uint8 ep)                   { hsu_trace.ep = ep;        }
void hsu_trace_ep_clear(void)                     { hsu_trace.ep = 0xff;         }
void hsu_trace_msg1_set(hsu_trace_msg_type msg1)  { hsu_trace.msg1 = msg1;    }
void hsu_trace_msg1_clear(void)                   { hsu_trace.msg1 = (hsu_trace_msg_type)0; }
void hsu_trace_msg2_set(hsu_trace_msg_type msg2)  { hsu_trace.msg2 = msg2;    }
void hsu_trace_msg2_clear(void)                   { hsu_trace.msg2 = (hsu_trace_msg_type)0; }
void hsu_trace_iface_set(hsu_iface_index_enum_type iface) {  hsu_trace.iface = iface; }
void hsu_trace_iface_clear(void)                  { hsu_trace.iface = NO_HSU_IFACE; }
void hsu_trace_inf_set(uint8 inf)                 { hsu_trace.inf = inf;      }
void hsu_trace_inf_clear(void)                    { hsu_trace.inf = 0;        }

void hsu_trace_copy(hsu_trace_info *trace_info_ptr)
{
  HSU_ASSERT(trace_info_ptr != NULL);

  trace_info_ptr->ep     = hsu_trace.ep;
  trace_info_ptr->msg1   = hsu_trace.msg1;
  trace_info_ptr->msg2   = hsu_trace.msg2;
  trace_info_ptr->iface  = hsu_trace.iface;
  trace_info_ptr->inf    = hsu_trace.inf;
}
#endif /* FEATURE_HS_USB_TRACE */


#if 0  /* Debug Only */
/* Below function is to be called for debugging purpose of 
** Sparrow Power Collapse logging (i.e. cpu_vdd_pc_no_retention_enter/exit).  
** For CRM build, disable this function since it is never called.
**
*/
void hsu_cpu_vdd_log(boolean enter)
{
  static uint32 hsu_cpu_vdd_log_last_entry_ms = 0;

  uint32 current_time_ms;
  uint32 time_diff_ms;
  
  if (hsu_bus_cir_buff)
  {
    current_time_ms = hsu_timetick_ulog_get64();
    time_diff_ms = current_time_ms - hsu_cpu_vdd_log_last_entry_ms;
    hsu_cpu_vdd_log_last_entry_ms = current_time_ms;
    
    if (enter)
    {
      hsu_bus_log(CPU_VDD_PC_NO_RETENTION_ENTER_LOG,0,0,0,time_diff_ms);
    }
    else
    {
      hsu_bus_log(CPU_VDD_PC_NO_RETENTION_EXIT_LOG,0,0,0,time_diff_ms);
    }
  }
}
#endif /* End of Debug Only */

void hsu_ulog_init (void)
{
  #ifndef USB_THREADX
  DALSYSConfig syscfg = {NULL, 0};
  DALSYS_InitMod(&syscfg);
  #endif
  hsu_ulog_type_init(BAM_LOG);
  hsu_ulog_type_init(BUS_LOG);
  hsu_ulog_type_init(DATA_LOG);
  hsu_ulog_type_init(SNPS_CTRL_LOG);
  hsu_ulog_type_init(SNPS_LOG);
  hsu_ulog_type_init(SER4_LOG);
  hsu_ulog_type_init(REG_LOG);
  hsu_ulog_type_init(DBG_E_LOG);
  hsu_ulog_type_init(ENG_LOG);

  //QMI LOG is enable in fucntion: hsu_al_ecm_enabled_cb in hsu_al_ecm.c
  //hsu_ulog_type_init(QMI_LOG);

  if (HSU_IFACE_ABSENT_IN_COMP != hsu_conf_sel_get_iface_index_comp(AUDIO_HSU_IFACE))
  {
    hsu_ulog_type_init(AUDIO_LOG);
  }

  // Sanity check to confirm that STRING array size matches the ENUM size, 
  // meaning, two parallel arrays is not mismatching on number of entries
  if (HSU_ULOG_ENUM_SIZE != (sizeof(HSU_ULOG_ENUM_STR)/sizeof(char*)))
  {
    HSU_ERR_FATAL("ulog enum (%u) != str array (u%)", HSU_ULOG_ENUM_SIZE, (sizeof(HSU_ULOG_ENUM_STR)/sizeof(char*)), 0);
  }
}

void hsu_ulog_type_init(hsu_ulog_log_type_enum log_type)
{
  ULogResult ulog_result = DAL_SUCCESS;
  HSU_ASSERT(log_type < HSU_LOG_TYPE_SIZE);

  // If the handle is not NULL, this means that the API was called previously
  // to create the handle.  In that case, use the existing handle.
  if (NULL == hsu_ulog_ctx[log_type].handle)
  {
    ulog_result = ULogFront_RealTimeInit(
      &hsu_ulog_ctx[log_type].handle,
      hsu_ulog_ctx[log_type].buf_name,
      hsu_ulog_ctx[log_type].buf_size,
      ULOG_MEMORY_LOCAL,
      ULOG_LOCK_NONE );

    HSU_ASSERT((ulog_result == DAL_SUCCESS) && (hsu_ulog_ctx[log_type].handle != NULL));

    hsu_ulog_total_size += hsu_ulog_ctx[log_type].buf_size;

    HSU_ULOG_STR_1(NO_MSG, log_type, "ulog_type %s, total_ulog %u, init_success", hsu_ulog_ctx[log_type].buf_name, hsu_ulog_total_size);
  }
}

void hsu_ulog(hsu_ulog_log_type_enum log_type, const char* formatStr)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_0(hsu_ulog_ctx[log_type].handle, formatStr);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_1 (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 value1)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_1(hsu_ulog_ctx[log_type].handle, formatStr, value1);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_2 (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 value1, uint32 value2)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_2(hsu_ulog_ctx[log_type].handle, formatStr, value1, value2);
  INTFREE_RESTORE(hsu_log_intlock_flag)

}

void hsu_ulog_3 (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 value1, uint32 value2, uint32 value3)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_3(hsu_ulog_ctx[log_type].handle, formatStr, value1, value2, value3);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}


void hsu_ulog_4 (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 value1, uint32 value2, uint32 value3, uint32 value4)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_4(hsu_ulog_ctx[log_type].handle, formatStr, value1, value2, value3, value4);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_5 (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 value1, uint32 value2, uint32 value3, uint32 value4, uint32 value5)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_5(hsu_ulog_ctx[log_type].handle, formatStr, value1, value2, value3, value4, value5);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_6 (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 value1, uint32 value2, uint32 value3, uint32 value4, uint32 value5, uint32 value6)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_6(hsu_ulog_ctx[log_type].handle, formatStr, value1, value2, value3, value4, value5, value6);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}


void hsu_ulog_str(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_1(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_str_1(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg, uint32 value1)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_2(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg, value1);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_str_2(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg, uint32 value1, uint32 value2)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_3(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg, value1, value2);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_str_3(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg, uint32 value1, uint32 value2, uint32 value3)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_4(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg, value1, value2, value3);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_str_4(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg, uint32 value1, uint32 value2, uint32 value3, uint32 value4)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_5(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg, value1, value2, value3, value4);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_str_5(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg, uint32 value1, uint32 value2, uint32 value3, uint32 value4, uint32 value5)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_6(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg, value1, value2, value3, value4, value5);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}
void hsu_ulog_str_6(hsu_ulog_log_type_enum log_type, const char* formatStr, const char* stringMsg, uint32 value1, uint32 value2, uint32 value3, uint32 value4, uint32 value5, uint32 value6)
{
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (stringMsg != NULL));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_7(hsu_ulog_ctx[log_type].handle, formatStr, stringMsg, value1, value2, value3, value4, value5, value6);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_enum  (hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 enum_id, uint32 enum_offset)
{
  uint32 index = enum_id + enum_offset;
  //HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index < HSU_ULOG_ENUM_SIZE));
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index < HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_1(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index]);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_enum_1(hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 enum_id, uint32 enum_offset, uint32 value1)
{
  uint32 index = enum_id + enum_offset;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index < HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_2(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index], value1);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_enum_2(hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 enum_id, uint32 enum_offset, uint32 value1, uint32 value2)
{
  uint32 index = enum_id + enum_offset;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index < HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_3(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index], value1, value2);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_ulog_enum_3(hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 enum_id, uint32 enum_offset, uint32 value1, uint32 value2, uint32 value3)
{
  uint32 index = enum_id + enum_offset;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index < HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_4(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index], value1, value2, value3);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}


void hsu_ulog_enum2 (hsu_ulog_log_type_enum log_type, const char* formatStr,  uint32 enum_id1, uint32 enum_offset1, uint32 enum_id2, uint32 enum_offset2)
{
  uint32 index1 = enum_id1 + enum_offset1;
  uint32 index2 = enum_id2 + enum_offset2;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index1<HSU_ULOG_ENUM_SIZE) && (index2<HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_2(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index1], HSU_ULOG_ENUM_STR[index2]);
  INTFREE_RESTORE(hsu_log_intlock_flag)

}


void hsu_ulog_enum2_1(hsu_ulog_log_type_enum log_type, const char* formatStr,  uint32 enum_id1, uint32 enum_offset1, uint32 enum_id2, uint32 enum_offset2, uint32 value1)
{
  uint32 index1 = enum_id1 + enum_offset1;
  uint32 index2 = enum_id2 + enum_offset2;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index1<HSU_ULOG_ENUM_SIZE) && (index2<HSU_ULOG_ENUM_SIZE));

  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_3(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index1], HSU_ULOG_ENUM_STR[index2], value1);

  INTFREE_RESTORE(hsu_log_intlock_flag)


}
void hsu_ulog_enum2_2(hsu_ulog_log_type_enum log_type, const char* formatStr,  uint32 enum_id1, uint32 enum_offset1, uint32 enum_id2, uint32 enum_offset2, uint32 value1, uint32 value2)

{
  uint32 index1 = enum_id1 + enum_offset1;
  uint32 index2 = enum_id2 + enum_offset2;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index1<HSU_ULOG_ENUM_SIZE) && (index2<HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_4(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index1], HSU_ULOG_ENUM_STR[index2], value1, value2);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}
void hsu_ulog_enum2_3(hsu_ulog_log_type_enum log_type, const char* formatStr, uint32 enum_id1, uint32 enum_offset1, uint32 enum_id2, uint32 enum_offset2, uint32 value1, uint32 value2, uint32 value3)
{
  uint32 index1 = enum_id1 + enum_offset1;
  uint32 index2 = enum_id2 + enum_offset2;
  HSU_ASSERT((log_type < HSU_LOG_TYPE_SIZE) && (formatStr != NULL) && (index1<HSU_ULOG_ENUM_SIZE) && (index2<HSU_ULOG_ENUM_SIZE));
  INTLOCK_SAVE(hsu_log_intlock_flag)
  ULOG_RT_PRINTF_5(hsu_ulog_ctx[log_type].handle, formatStr, HSU_ULOG_ENUM_STR[index1], HSU_ULOG_ENUM_STR[index2], value1, value2, value3);
  INTFREE_RESTORE(hsu_log_intlock_flag)
}

void hsu_diag_enum_str(uint32 enum_id1, uint32 enum_offset1)
{
  uint32 index1 = enum_id1 + enum_offset1;
  HSU_ASSERT(index1<HSU_ULOG_ENUM_SIZE);
  HSU_SPRINTF_3("%s, cont", HSU_ULOG_ENUM_STR[index1], 0, 0);
}

void hsu_diag_enum2_str(uint32 enum_id1, uint32 enum_offset1, uint32 enum_id2, uint32 enum_offset2)
{
  uint32 index1 = enum_id1 + enum_offset1;
  uint32 index2 = enum_id2 + enum_offset2;
  HSU_ASSERT((index1<HSU_ULOG_ENUM_SIZE) && (index2<HSU_ULOG_ENUM_SIZE));
  HSU_SPRINTF_3("%s, %s, cont", HSU_ULOG_ENUM_STR[index1], HSU_ULOG_ENUM_STR[index2], 0);
}

