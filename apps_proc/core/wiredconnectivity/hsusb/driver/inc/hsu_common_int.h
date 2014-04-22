/*==============================================================================

            hsu_common   ---   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains common definitions for high speed usb.

Copyright (c) 2008-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/inc/hsu_common_int.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
08/25/09  rg  Creation

==============================================================================*/

#ifndef _HSU_COMMON_INT_H
#define _HSU_COMMON_INT_H
#include "hsu_common.h"
#include "customer.h"
#include "msg.h"
#include "assert.h"
#include "hsu_os_abstract_bus_ext_int.h"

#ifdef FEATURE_L4
#include "cache_mmu.h" /* for dcache_inval_region */
#else 
//#include "cache_mmu.h" /* for dcache_inval_region */
//#include "armmmu.h"    /* for mmu_dcache_inval_region */
#endif /* FEATURE_L4 */

#include "hsu_timer.h"
#include "qurt_memory.h"
#include "qurt_interrupt.h"

/*==============================================================================
**
** USB Configuration Parameters
**
==============================================================================*/

/* Offset or DLOAD INFO for USB
*/
#define HS_USB_DLOAD_INFO_USB_OFFSET     (0xC8)
   
/* IMEM code ram region used to pass PID and Serial number string.
** Physical Address: 0xFE807800 (MAGIC NUMBER IMEM BASE) + 0xC8 (USB OFFSET)
** MMU Mapping: 02000000--02007FFF|    A:FE800000--FE807FFF
*/
#define HS_USB_DLOAD_INFO_IMEM_ADDR ((SCL_SHARED_IMEM_BASE) + (HS_USB_DLOAD_INFO_USB_OFFSET))

/*
** 9x25 PHY Caliberation Default Values 
*/
#define HSU_DISCONN_THRESHOLD_DEFAULT_VALUE 4
#define HSU_VBUS_VALID_THRESHOLD_DEFAULT_VALUE 4
#define HSU_SQUELCH_THRESHOLD_DEFAULT_VALUE 6
#define HSU_DC_VOLTAGE_DEFAULT_VALUE 8
#define HSU_PREEMP_ADJ_DEFAULT_VALUE 0
#define HSU_PREEMP_AMP_DEFAULT_VALUE 2
#define HSU_RISE_FALL_DEFAULT_VALUE 1
#define HSU_CROSSOVER_DEFAULT_VALUE 0

#define HSU_SOURCE_IMPEDANCE_DEFAULT_VALUE 1
#define HSU_FS_LS_SOURCE_IMPEDANCE_DEFAULT_VALUE 3

// Size of ECM RX buffer is linked with number of TRBs that will be queued at EP.
// It has been found that due to 50kB internal RX cache, 25 TRBs each pointing to 2kB buffer is optimal.
#define HSU_SNPS_OUT_TRB_ARRAY_SIZE_MAX   (8)

//for intlock and intfree
// TX_INTERRUPT_SAVE_AREA


/** The number of all cores (host and peripheral) in all possible
configurations.\ This constant is used for local array allocation
to be supplied to Jungo on stack startup.
*/
typedef enum 
{
    HSU_CORE_CONFIG_USB3_IDX = 0,
    HSU_MAX_NUM_OF_CORES
} usb_hal_core_type;

#define HSU_CORE_CONFIG_USB_IDX hsu_conf_sel_config_get_core_idx()

/*==============================================================================

FUNCTION          hsu_conf_sel_config_get_core_idx

DESCRIPTION
This function returns value of the primary usb core index.

DEPENDENCIES
None.

RETURN VALUE
Returns value of the primary usb core index.

SIDE EFFECTS
None.

==============================================================================*/
usb_hal_core_type hsu_conf_sel_config_get_core_idx(void);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_USB_CORE_SNPS

DESCRIPTION
This API tells if we EFS item indicates SNPS core to be used.

DEPENDENCIES
None.

RETURN VALUE
Returns TRUE if SNPS core is used else FALSE.

SIDE EFFECTS
None.

==============================================================================*/
boolean hsu_conf_sel_is_usb_core_snps (void);

#define HSU_ASSERT(condition) \
    if( !(condition) ) \
    { \
      ERR_FATAL( "hsu " #condition, 0, 0, 0 ); \
    }

#define HSU_ERR(condition) \
    if( condition ) \
    { \
      ERR_FATAL( "hsu " #condition, 0, 0, 0 ); \
    }

// No-op the DIAG LOGGING since ULOG is used instead
#define HSU_MSG_ERROR(x_fmt)
#define HSU_MSG_ERROR_1(x_fmt, a)
#define HSU_MSG_ERROR_2(x_fmt, a, b)
#define HSU_MSG_ERROR_3(x_fmt, a, b, c)
#define HSU_MSG_HIGH(x_fmt)
#define HSU_MSG_HIGH_1(x_fmt, a)
#define HSU_MSG_HIGH_2(x_fmt, a, b) 
#define HSU_MSG_HIGH_3(x_fmt, a, b, c)
#define HSU_MSG_MED(x_fmt)
#define HSU_MSG_MED_1(x_fmt, a)
#define HSU_MSG_MED_2(x_fmt, a, b) 
#define HSU_MSG_MED_3(x_fmt, a, b, c)
#define HSU_MSG_LOW(x_fmt)
#define HSU_MSG_LOW_1(x_fmt, a)
#define HSU_MSG_LOW_2(x_fmt, a, b)
#define HSU_MSG_LOW_3(x_fmt, a, b, c)
#define HSU_SPRINTF_3(x_fmt, a, b, c)
#define HSU_ERR_FATAL(x_fmt, a, b, c) \
  ERR_FATAL(x_fmt, (a), (b), (c))

#define HSU_DIAG_MSG_HIGH(x_fmt)               MSG(MSG_SSID_HSUSB, MSG_LEGACY_HIGH, x_fmt)
#define HSU_DIAG_MSG_HIGH_1(x_fmt, a)          MSG_1(MSG_SSID_HSUSB, MSG_LEGACY_HIGH, x_fmt, (a))
#define HSU_DIAG_MSG_HIGH_2(x_fmt, a, b)       MSG_2(MSG_SSID_HSUSB, MSG_LEGACY_HIGH, x_fmt, (a), (b))
#define HSU_DIAG_MSG_HIGH_3(x_fmt, a, b, c)    MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_HIGH, x_fmt, (a), (b), (c))

#define HSU_DIAG_MSG_ERROR(x_fmt)              MSG(MSG_SSID_HSUSB, MSG_LEGACY_ERROR, x_fmt)
#define HSU_DIAG_MSG_ERROR_1(x_fmt, a)         MSG_1(MSG_SSID_HSUSB, MSG_LEGACY_ERROR, x_fmt, (a))
#define HSU_DIAG_MSG_ERROR_2(x_fmt, a, b)      MSG_2(MSG_SSID_HSUSB, MSG_LEGACY_ERROR, x_fmt, (a), (b))
#define HSU_DIAG_MSG_ERROR_3(x_fmt, a, b, c)   MSG_3(MSG_SSID_HSUSB, MSG_LEGACY_ERROR, x_fmt, (a), (b), (c))

#define HSU_DIAG_SPRINTF_3(x_fmt, a, b, c)     MSG_SPRINTF_3(MSG_SSID_HSUSB, MSG_LEGACY_HIGH, x_fmt, (a), (b), (c))

/* HSUSB giant mutex behavior becomes undefined when certain APIs are called in IST context.
** The following warns the caller of such violation and suggests a resolution method
*/
#define hsu_chk_ist_crit_failure() \
  if (rex_is_in_irq_mode()) \
  { \
    HSU_ERR_FATAL("CRITICAL FAILURE: Function cannot be called from IST context.", 0, 0, 0); \
  }

/* with the addition of HW Queuing, TLP is not supported anymore */
#ifdef FEATURE_HS_USB_TLP
#error FEATURE_HS_USB_TLP not supported
#endif

/* with the addition of HW Queuing, ACM DL aggregation is not supported anymore */
#ifdef HS_USB_AGGREGATION_TIMER_FOR_MDM_MSECS
#error HS_USB_AGGREGATION_TIMER_FOR_MDM_MSECS not supported
#endif

/* SPS mode support */
#if defined(FEATURE_HS_USB_FAST_CSW) && defined(FEATURE_HS_USB_MS_BAM)
#error FEATURE_HS_USB_FAST_CSW and FEATURE_HS_USB_MS_BAM are not allowed to be defined together
#endif /* FEATURE_HS_USB_FAST_CSW && FEATURE_HS_USB_MS_BAM */

#ifdef FEATURE_L4
#define hsu_dcache_inval_region(ptr, len) dcache_inval_region(ptr, len);
#else
#define hsu_dcache_inval_region(ptr, len) qurt_mem_cache_clean(\
    (qurt_addr_t)ptr,\
    (qurt_size_t)len,\
    QURT_MEM_CACHE_INVALIDATE,\
    QURT_MEM_DCACHE);
// #define hsu_dcache_inval_region(ptr, len) mmu_dcache_inval_region((uint32 *)(ptr), (uint32)(len));
#endif /* FEATURE_L4 */

#ifdef FEATURE_L4
#define hsu_dcache_clean_region(ptr, len) dcache_clean_region(ptr, len);
#else
#define hsu_dcache_clean_region(ptr, len) qurt_mem_cache_clean(\
    (qurt_addr_t)ptr,\
    (qurt_size_t)len,\
    QURT_MEM_CACHE_FLUSH,\
    QURT_MEM_DCACHE);
#endif /* FEATURE_L4 */

#ifdef FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION 
typedef struct
{
  uint32 tx_zlp_seq_num; // ZLP sequence number incremented only for send encapsulated command
  uint32 rx_zlp_seq_num; // ZLP sequence number incremented only for get encapsulated response

  /* SEND ENCAPSULATED COMMAND statistics */
    /* in cdc_handle_encapsulated_command */
    uint32 send_encap_cmd_cnt;
  
    /* in encapsulated_command_complete */
    uint32 cdc_get_io_status_failure_cnt;
    uint32 cdc_send_status_cnt; /* this also logs number of times TX ZLP is created */

    /* in cdc_send_status (TX ZLP) */
    uint32 fail_to_create_ep0_write_cnt;
    uint32 fail_to_send_status_cnt;
  
    /* in status_complete (TX ZLP completion stage) */
    uint32 status_complete_cnt;
    uint32 cdc_status_complete__transfer_error_cnt;

    /* in encapsulated_response_complete */
    uint32 encap_response_complete_insufficient_bytes_sent_cnt;

    /* in create_rx_zlp_request */
    uint32 new_zlp_create_while_last_rx_zlp_has_not_completed_cnt;

    /* in create_request */
    uint32 overlapping_replies_cnt;

    uint32 zlp_not_created_for_zldp_cnt;

  /* in dcd_tdi_4x */
  uint32 abort_ep0_cnt;

#ifdef FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION
  hsu_timer_handle_type encap_cmd_tx_zlp_timer_id; /* unique timer ID  */
  hsu_timer_handle_type encap_resp_rx_zlp_timer_id; /* unique timer ID */
#endif /* FEATURE_HS_USB_ERR_FATAL_UPON_TIMER_EXPIRY_FOR_RX_TX_ZLP_COMPLETION */
} hsu_missing_zlp_debug_type;
#endif /* FEATURE_HS_USB_VERIFY_RX_TX_ZLP_COMPLETION */

#define HSU_EP0_AGGRESSIVE_ERR_CHK_MASK            (1 << 0)
#define HSU_EP0_TIMER_ON_RX_TX_ZLP_ERR_CHK_MASK    (1 << 1)
#define HSU_DTD_MEM_LOGGING_ENABLE_MASK            (1 << 2)
typedef uint8 hsu_efs_ep0_error_capturing_bit_field;

#endif
