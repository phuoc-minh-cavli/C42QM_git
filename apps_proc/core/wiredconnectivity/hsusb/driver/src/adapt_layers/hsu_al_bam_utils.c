/*==============================================================================

BAM utilities for High Speed USB Implementation

GENERAL DESCRIPTION
Contains the implementation of the API for the BAM utilities

EXTERNALIZED FUNCTIONS

hsu_al_bam_init
hsu_al_bam_pipe_enable
hsu_al_bam_pipe_disable
hsu_al_bam_allocate_pipes
hsu_al_bam_register_wakeup_event
hsu_al_bam_unregister_wakeup_event
hsu_al_bam_alloc
hsu_al_bam_free

Copyright (c) 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_bam_utils.c#1 $
  $DateTime: 2020/01/30 22:49:35 $

when      who       what, where, why
--------  ---       ------------------------------------------------------
03/03/09  evolfson  First revision of file  
10/07/12  cheunga   Rewrite to support new BAM APIs
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h"
#include "hsu_al_bam_utils.h"
#include "hsu_common_int.h"
#include "hsu_conf_sel_i.h"
#include "memory.h"
//#include "rex.h"
#include "hsu_log.h"
//#include "DALSys.h"
#include "HAL_hsusb.h"
#include "hsu_core_config_int.h"
#include "smem.h"
#include "hsu_phy_utils_int.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_al_task.h"
#include <stringl/stringl.h>
#include "hsu_al_ecm.h"
#include "hsu_conf_sel_stack_utils_int.h"
#include "hsu_os_abstract_bus.h"
#include "hsu_bam_sm.h"
#include "hsu_al_ser_obex.h"
#include "qdss_usb_bam.h"

#ifdef FEATURE_HS_USB_IPA_LOG_EFS
#include "ipa_log_efs.h"
#endif // FEATURE_HS_USB_IPA_LOG_EFS

#ifdef FEATURE_HS_USB_BAM

/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/
#define MAX_BAM_CONNECTIONS 1 /*  3 pairs of hardware accelerated endpoints/6 pipes 
                                  for RMNET and 1 DL pipe for QDSS 
                              */

#define HSU_MAX_IPA_CONNECTIONS    2

#define MAX_BAM_FLUSH_DURATION_MS 50
#define MAX_BAM_FLUSH_ITER 10
#define HSU_AL_BAM_INVALID_PIPE_INDEX 0xFF
#define HSU_AL_BAM_MAX_NUM_BAM    3 /* USB, A2, QDSS */
#define HSU_QDSS_BAM_INDEX (0)

typedef enum
{
  HSU_AL_BAM_OCIMEM               = 0,
  HSU_AL_BAM_SMEM                 = 1,
  HSU_AL_BAM_QDSS_OCIMEM          = 2,
  HSU_AL_BAM_MAX_BUFFER_TYPE         ,
  HSU_AL_BAM_UNDEFINED            = 0xFF,
} hsu_al_bam_buffer_type;

typedef enum 
{
  HSU_AL_BAM_QDSS                 = 0,
  HSU_AL_BAM_NET_1                = 1,
  HSU_AL_BAM_NET_2                = 2,
  HSU_AL_BAM_NET_3                = 3,
  HSU_AL_BAM_MAX_IFACE_TYPE
} hsu_al_bam_iface_enum_type;

typedef struct
{
  hsu_al_bam_buffer_type mem_type;
  uint32 pa_addr;
  uint32 size;
} hsu_al_bam_mem_config_type;

typedef struct
{
  hsu_al_bam_mem_config_type ul_data_fifo;
  hsu_al_bam_mem_config_type ul_desc_fifo;
  hsu_al_bam_mem_config_type dl_data_fifo;
  hsu_al_bam_mem_config_type dl_desc_fifo;
} hsu_al_bam_mem_layout_type;


typedef struct 
{
  boolean bam_allocated;
  boolean ul_pipes_flow_enabled; // Aggregate info of all pipes, it turns true even if 1 of the many pipes are enabled
  boolean dl_pipes_flow_enabled; // Aggregate info of all pipes, it turns true even if 1 of the many pipes are enabled

  uint32 hsu_bam_connection_index; /* index to maintain the number of connections initialized */
  hsu_bam_peripheral_bam_type peripheral_bam; /* only initialized once per core */
  hsu_bam_peripheral_bam_type peripheral_bam_direct_mode;
  /* need to hold a local copy of the pipe parameters as it gets referenced within the structure below */
  /* this is not to be passed around to the clients by design */
  bam_pipe_config_type bam_producer_config[MAX_BAM_CONNECTIONS];
  bam_pipe_config_type bam_consumer_config[MAX_BAM_CONNECTIONS];

  /* all connection information is passed via hsu_bam_connection_type between client and this layer */
  /* glue logic for consumer pipe and producer pipe */
  /* holds a lightweight pointer to each interface */
  hsu_bam_connection_type *connections[MAX_BAM_CONNECTIONS];

  uint32 smem_buf_va_base_addr;
  uint32 smem_buf_pa_base_addr;

  uint32 pa_addr_running_sum[HSU_AL_BAM_MAX_BUFFER_TYPE];

#if 0 //Removing bam dump
  /* +++ new bam dump information upon error fatal */
  void*         bam_dump;
  uint32        bam_dump_size;
  void*         bam_dump_desc_fifo_buf;
  uint32        bam_dump_desc_fifo_size;
  void*         bam_dump_data_fifo_buf;
  uint32        bam_dump_data_fifo_size;
  /* --- new bam dump information upon error fatal */
#endif

  boolean                      ipa_per_drop_active_flag;
  boolean                      ipa_per_clear_data_path_active_flag;
  uint32                       ipa_data_activity_cb_cnt;

  #ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
  boolean                      ci_bam_already_reset;
  #endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
} hsu_al_bam_utils_ctx_type;

static hsu_al_bam_utils_ctx_type hsu_al_bam_utils_ctx;

static hsu_al_bam_mem_layout_type hsu_al_bam_ul_dl_fifo_config[] =
{
  {
    /* HSU_AL_BAM_QDSS */
    { /* UL DATA FIFO */
      HSU_AL_BAM_UNDEFINED,     /* mem_type */
      0,                        /* pa_addr  */
      0,                        /* size     */
    },
    { /* UL DESC FIFO */
      HSU_AL_BAM_UNDEFINED,     /* mem_type */
      0,                        /* pa_addr  */
      0,                        /* size     */
    },
    { /* DL DATA FIFO */
      HSU_AL_BAM_QDSS_OCIMEM,   /* mem_type */
      0,                        /* pa_addr  */
      0x0600,                   /* size     */
    },
    { /* DL DESC FIFO */
      HSU_AL_BAM_QDSS_OCIMEM,   /* mem_type */
      0,                        /* pa_addr  */
      0x0200,                   /* size     */
    },
  },
  {
    /* HSU_AL_BAM_NET_1 */
    { /* UL DATA FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,       /* mem_type */
      0,                     /* pa_addr  */
      0x3000,                /* size     */   // 12288 bytes
    },
    { /* UL DESC FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,       /* mem_type */
      0,                     /* pa_addr  */
      0x1000,                /* size     */   // 4096 bytes == 512 descriptors
    },
    { /* DL DATA FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,     /* mem_type */
      0,                     /* pa_addr  */
      0x6C00,                /* size     */   // 27648 bytes
    },
    { /* DL DESC FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,     /* mem_type */
      0,                     /* pa_addr  */
      0x1C00,                /* size     */   // 7168 bytes == 896 descriptors
    },
  },
  {
    /* HSU_AL_BAM_NET_2 */
    { /* UL DATA FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,       /* mem_type */
      0,                     /* pa_addr  */
      0x3000,                /* size     */   // 12288 bytes
    },
    { /* UL DESC FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,       /* mem_type */
      0,                     /* pa_addr  */
      0x1000,                /* size     */   // 4096 bytes == 512 descriptors
    },
    { /* DL DATA FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,       /* mem_type */
      0,                     /* pa_addr  */
      0x6C00,                /* size     */   // 27648 bytes
    },
    { /* DL DESC FIFO */
      HSU_AL_BAM_UNDEFINED, //HSU_AL_BAM_SMEM,       /* mem_type */
      0,                     /* pa_addr  */
      0x1C00,                /* size     */   // 7168 bytes == 896 descriptors
    },
  },
  {
    /* HSU_AL_BAM_NET_3 */
    { /* UL DATA FIFO */
      HSU_AL_BAM_UNDEFINED,     /* mem_type */
      0,                   /* pa_addr  */
      0,              /* size     */
    },
    { /* UL DESC FIFO */
      HSU_AL_BAM_UNDEFINED,     /* mem_type */
      0,                   /* pa_addr  */
      0,              /* size     */
    },
    { /* DL DATA FIFO */
      HSU_AL_BAM_UNDEFINED,     /* mem_type */
      0,                   /* pa_addr  */
      0,              /* size     */
    },
    { /* DL DESC FIFO */
      HSU_AL_BAM_UNDEFINED,     /* mem_type */
      0,                   /* pa_addr  */
      0,              /* size     */
    },
  },
};

/* BAM2BAM arrangement */
/* UL1 DATA FIFO | DESC FIFO
 * DL1 DATA FIFO | DESC FIFO
 * UL2 DATA FIFO | DESC FIFO
 * DL2 DATA FIFO | DESC FIFO
 * etc...
 */
#define HSU_AL_BAM_SMEM_BASE_ADDR        (hsu_al_bam_utils_ctx.smem_buf_pa_base_addr)

static void hsu_al_bam_ipa_init(void);
static void hsu_al_bam_pipe_disable(hsu_bam_connection_type* connection, boolean ul, boolean dl);
static void hsu_al_bam_pipe_enable(hsu_bam_connection_type* connection, boolean ul, boolean dl);
static boolean hsu_al_bam_utils_is_ipa_clock_voted(void);


static uint32 hsu_al_bam_utils_get_all_desc_fifo_size(void);
static uint32 hsu_al_bam_utils_get_all_data_fifo_size(void);

static void hsu_al_bam_utils_populate_pa_addr_int(hsu_al_bam_mem_config_type *layout_config)
{
   layout_config->pa_addr = hsu_al_bam_utils_ctx.pa_addr_running_sum[layout_config->mem_type];
   hsu_al_bam_utils_ctx.pa_addr_running_sum[layout_config->mem_type] += layout_config->size;
}

/* main API exposed to caller within this file */
void hsu_al_bam_utils_populate_pa_addr(void) /* ep count starts with 1 */
{
  uint32 i;
  hsu_al_bam_utils_ctx.pa_addr_running_sum[HSU_AL_BAM_OCIMEM] += HSU_AL_BAM_OCIMEM_BASE_ADDR;
  hsu_al_bam_utils_ctx.pa_addr_running_sum[HSU_AL_BAM_SMEM] += HSU_AL_BAM_SMEM_BASE_ADDR;
  hsu_al_bam_utils_ctx.pa_addr_running_sum[HSU_AL_BAM_QDSS_OCIMEM] += HSU_AL_BAM_QDSS_OCIMEM_BASE_ADDR;

  for (i = 0; i < HSU_AL_BAM_MAX_IFACE_TYPE; i++)
  {
    hsu_al_bam_utils_populate_pa_addr_int(&(hsu_al_bam_ul_dl_fifo_config[i].ul_data_fifo));
    hsu_al_bam_utils_populate_pa_addr_int(&(hsu_al_bam_ul_dl_fifo_config[i].ul_desc_fifo));
    hsu_al_bam_utils_populate_pa_addr_int(&(hsu_al_bam_ul_dl_fifo_config[i].dl_data_fifo));
    hsu_al_bam_utils_populate_pa_addr_int(&(hsu_al_bam_ul_dl_fifo_config[i].dl_desc_fifo));
  }
  if ((hsu_al_bam_utils_ctx.pa_addr_running_sum[HSU_AL_BAM_OCIMEM] > HSU_AL_BAM_OCIMEM_BASE_ADDR + HSU_AL_BAM_UTILS_OCIMEM_BUF_SIZE) ||
      (hsu_al_bam_utils_ctx.pa_addr_running_sum[HSU_AL_BAM_SMEM] > HSU_AL_BAM_SMEM_BASE_ADDR + HSU_AL_BAM_UTILS_SMEM_BUF_SIZE) ||
      (hsu_al_bam_utils_ctx.pa_addr_running_sum[HSU_AL_BAM_QDSS_OCIMEM] > HSU_AL_BAM_QDSS_OCIMEM_BASE_ADDR + HSU_AL_BAM_UTILS_QDSS_OCIMEM_BUF_SIZE))
  {
    HSU_ERR_FATAL("UL DL Data Desc FIFO overrun, check calculations", 0, 0, 0);
  }
}

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
Variables
==============================================================================*/

/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
#ifndef CI_MERGE
static hsu_bam_endpoint_type hsu_al_bam_get_usb_endpoint(void);
#endif

void hsu_al_init_bam_config_params(hsu_bam_connection_type* connection);
void hsu_al_init_loopback_bam_config_params(hsu_bam_connection_type* connection);
static void hsu_al_bam_utils_bam_notification_cb(bam_result_type bam_result);
static void hsu_al_bam_utils_revert_to_non_bam_mode(void);


/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_bam_init

DESCRIPTION
Initialize the HSUSB BAM interface.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_init(void)
{
  uint32 total_size_to_alloc = 0;
  bam_info_type *bam_info = NULL;
  hsu_bam_peripheral_bam_type *bam = NULL;

  if (hsu_al_bam_utils_ctx.bam_allocated == FALSE)
  {
    hsu_al_bam_utils_populate_pa_addr();

    #ifndef CI_MERGE
    /* run once when the very first pipe is initialized since this is a top level BAM driver initialization */
    /* Due to core system error, we must configure the BAM from BYPASS mode to BAM DIRECT mode for both cores if enabled */
    if (hsu_conf_sel_hs_main_core_enabled())
    {
       bam = (HSU_BAM_ENDPOINT_USB1 == hsu_al_bam_get_usb_endpoint()) ? 
             (&(hsu_al_bam_utils_ctx.peripheral_bam)) :
             (&(hsu_al_bam_utils_ctx.peripheral_bam_direct_mode));
    
       /* USB */
       bam->bam_config.bam_va = 0;
       bam->bam_config.bam_pa = HSU_AL_BAM_USB_BASE; /* use IPCatalog to lookup USB/HSIC BAM base */
       bam->bam_config.bam_irq = 175; /* IPCatalog */
       bam->bam_config.sum_thresh = HSU_AL_BAM_SUM_THRESHOLD;
       bam->bam_config.bam_irq_mask = (BAM_IRQ_HRESP_ERR_EN | BAM_IRQ_ERR_EN);
       bam->bam_config.bam_mti_irq_pa = 0; /* unused */
       bam->bam_config.options = 0; /* unused */
    
       /* Callback for BAM top level notifications */
       bam->bam_top_level_isr.func = hsu_al_bam_utils_bam_notification_cb;
       bam->bam_top_level_isr.data = (void*)HSU_CORE_CONFIG_USB3_IDX;
    
       bam->bam_handle = bam_init(&(bam->bam_config), &(bam->bam_top_level_isr));
       if (!(bam->bam_handle))
       {
         HSU_ERR_FATAL("bam_init() returned invalid handle", 0, 0, 0);
       }
       HSU_ULOG(NO_MSG, BAM_LOG, "bam_init");
    
       if (BAM_SUCCESS != bam_get_info(bam->bam_handle, &(bam->bam_core_dump_info)))
       {
         HSU_ERR_FATAL("bam_get_info() failed", 0, 0, 0);
       }
     }
    #endif /*CI_MERGE*/
    /* +++ BAM core dump allocations */
    /*
     * As per BAM documentation:
     * BAM dump buffer size: (bam_header_buffer_size + (bam_buffer_size * Nums of different instance of peer bam))
     * BAM pipe dump buffer size: ((bam_pipe_buffer_size * nums of pipe) + (bam_pipe_buffer_size * nums of peer pipe)).
     * Alternate formula from BAM team:
     * bam_header_buffer_size+(bam_buffer_size*num_of_bams)+(bam_pipe_buffer_size*num_of_pipes)
    */
    /* allocate a larger chunk and then subdivide it within this abstraction layer */
  
    /* core dump should only be collected for the main bam, not the bam that is in direct mode */
#if 0 // Removing bam dump
    bam_info = &(hsu_al_bam_utils_ctx.peripheral_bam.bam_core_dump_info);
  
    hsu_al_bam_utils_ctx.bam_dump_size = 
      bam_info->bam_header_buffer_size + (bam_info->bam_buffer_size * HSU_AL_BAM_MAX_NUM_BAM) +
      /* Convert connection to == num of pipes by multipyling 2. */
      /* Another 2x for capturing peer pipe buffers as well */
      ((bam_info->bam_pipe_buffer_size * MAX_BAM_CONNECTIONS * 2) * 2);
    hsu_al_bam_utils_ctx.bam_dump_desc_fifo_size = hsu_al_bam_utils_get_all_desc_fifo_size();
    hsu_al_bam_utils_ctx.bam_dump_data_fifo_size = hsu_al_bam_utils_get_all_data_fifo_size();
  
    total_size_to_alloc =
      hsu_al_bam_utils_ctx.bam_dump_size + 
      hsu_al_bam_utils_ctx.bam_dump_desc_fifo_size +
      hsu_al_bam_utils_ctx.bam_dump_data_fifo_size;
  
    hsu_al_bam_utils_ctx.bam_dump = 
      hsu_conf_sel_alloc_uncached_memory(total_size_to_alloc);
  
    /* increment the pointer with byte offsets */
    hsu_al_bam_utils_ctx.bam_dump_desc_fifo_buf = (uint8*)hsu_al_bam_utils_ctx.bam_dump + hsu_al_bam_utils_ctx.bam_dump_size;
    hsu_al_bam_utils_ctx.bam_dump_data_fifo_buf = (uint8*)hsu_al_bam_utils_ctx.bam_dump_desc_fifo_buf + hsu_al_bam_utils_ctx.bam_dump_desc_fifo_size;
    /* --- BAM core dump allocations */
#endif  
    hsu_al_bam_utils_ctx.bam_allocated = TRUE;
    #ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
      hsu_al_bam_utils_ctx.ci_bam_already_reset = FALSE;
    #endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
  }
}

boolean hsu_al_bam_is_ipa_rdy_for_pipe_connect(void)
{
  return FALSE;
}



/*===========================================================================

FUNCTION hsu_al_bam_utils_bam_notification_cb

DESCRIPTION
This is BAM's top level callback to notify user of errors

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_bam_utils_bam_notification_cb(bam_result_type bam_result)
{
  uint32 core_index = (uint32)bam_result.cb_data;
  
  if (BAM_EVENT_ERROR == bam_result.event)
  {
    HSU_ULOG_1(NO_MSG, BUS_LOG, "bam_top_level_error, core_idx %u", core_index);

#if 0 /* 9x35 BU - TODO: Need to find equivalent BAM SM API */
    if ( (FALSE == hsu_sps_sm_is_a2_ready()) &&
         (core_index == hsu_hal_os_bus_core_id(HSU_CORE_IDX_PRIMARY)) )
    {
      /* Do not crash in case of HSIC stack restart or BAM RESET scenario for 
      ** Primay core, as resetting the BAM and core will recover the system.
      */
      hsu_event_log(EVT_BAM_TOP_LEVEL_ERROR_IGNORED, core_index);
    }
    else
#endif
    {
      HSU_ERR_FATAL("Bam Top Level Error\n", 0, 0, 0);
    }
  }
  else
  {
    HSU_ERR_FATAL("Other Bam Errors.", 0, 0, 0);
  }
}



/*===========================================================================

FUNCTION hsu_al_bam_pipe_enable

DESCRIPTION
Function is safe when called multiple times.
Creates top level BAM if this is the very first pipe allocated.
Creates pipe if not allocated already.
Enables the BAM producer and consumer pipe specified.

DEPENDENCIES
None

RETURN VALUE
TRUE if BAM mode is enabled, FALSE otherwise.

SIDE EFFECTS
None

===========================================================================*/
static void hsu_al_bam_pipe_enable
(
  hsu_bam_connection_type *connection, boolean ul, boolean dl
)
{
  bam_status_type hsu_bam_status;

  if (ul && connection->producer_pipe)
  {
    hsu_bam_status = bam_pipe_enable(connection->producer_pipe);
    if (BAM_SUCCESS != hsu_bam_status)
    {
      HSU_ERR_FATAL("hsu_al_bam_pipe_enable: Unable to enable BAM pipe", 0, 0, 0);
    }
    HSU_ULOG_1(NO_MSG, BAM_LOG, "ul, producer_pipe 0x%08X", (uint32)connection->producer_pipe);
  }

  if (dl && connection->consumer_pipe)
  {
    hsu_bam_status = bam_pipe_enable(connection->consumer_pipe);
    if (BAM_SUCCESS != hsu_bam_status)
    {
      HSU_ERR_FATAL("hsu_al_bam_pipe_enable: Unable to enable BAM pipe", 0, 0, 0);
    }
    HSU_ULOG_1(NO_MSG, BAM_LOG, "dl, consumer_pipe 0x%08X", (uint32)connection->consumer_pipe);
  }
  return;
}

/*===========================================================================

FUNCTION hsu_al_bam_pipe_disable_and_deinit

DESCRIPTION
Deallocates both the read and write pipes of a connection
If this is the last connection to be disabled, also deallocates the top level BAM handle

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_bam_pipe_disable_and_deinit
(
  hsu_bam_connection_type *connection
)
{
  bam_status_type hsu_bam_status;
  int index_to_be_deallocated = 0; /* track hsu_bam_connections position of array to be deallocated */
  uint32 i = 0;
  boolean are_all_pipes_deallocated = TRUE; /* assume true unless otherwise determined */

  if (connection)
  {
    if (0 == hsu_al_bam_utils_ctx.hsu_bam_connection_index)
    {
      HSU_ERR_FATAL("hsu_al_bam_pipe_deinit: no more deallocation possible, check upper layer for consistency", 0, 0 ,0);
    }

    /* scan and ensure the appropriate connection is deallocated */
    for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
    {
      if (hsu_al_bam_utils_ctx.connections[i] == connection)
      {
        index_to_be_deallocated = i;
        break;
      }
    }

    if (connection->producer_pipe)
    {
      hsu_bam_status = bam_pipe_disable(connection->producer_pipe);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("hsu_al_bam_pipe_deinit: bam_pipe_disable failed", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "disable, producer_pipe 0x%08X", (uint32)connection->producer_pipe);
    }

    if (connection->consumer_pipe)
    {
      hsu_bam_status = bam_pipe_disable(connection->consumer_pipe);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("hsu_al_bam_pipe_deinit: bam_pipe_disable failed", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "disable, consumer_pipe 0x%08X", (uint32)connection->consumer_pipe);
    }

    if (connection->producer_pipe)
    {
      hsu_bam_status = bam_pipe_deinit(connection->producer_pipe);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("hsu_al_bam_pipe_deinit: bam_pipe_deinit failed", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "deinit, producer_pipe 0x%08X", (uint32)connection->producer_pipe);
    }

    if (connection->consumer_pipe)
    {
      hsu_bam_status = bam_pipe_deinit(connection->consumer_pipe);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("hsu_al_bam_pipe_deinit: bam_pipe_deinit failed", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "deinit, consumer_pipe 0x%08X", (uint32)connection->consumer_pipe);
    }

    connection->producer_pipe = NULL;
    connection->consumer_pipe = NULL;
    hsu_al_bam_utils_ctx.connections[index_to_be_deallocated] = NULL;

    /* scan to see if it's last pipe that's allocated */
    for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
    {
      if (NULL != hsu_al_bam_utils_ctx.connections[i])
      {
        are_all_pipes_deallocated = FALSE;
        break;
      }
    }
    /* this should only be called if it's for the last pipe that is deallocated */
    if (are_all_pipes_deallocated)
    {
      hsu_bam_status = bam_deinit(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 1);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("hsu_al_bam_pipe_deinit: bam_deinit failed", 0, 0, 0);
      }
      HSU_ULOG(NO_MSG, BAM_LOG, "deinit, all_pipes_deallocated");
      hsu_al_bam_utils_ctx.hsu_bam_connection_index = 0;
      hsu_al_bam_utils_ctx.peripheral_bam.bam_handle = NULL;
      hsu_al_bam_utils_ctx.bam_allocated = FALSE;
    }
  }
}

/*===========================================================================

FUNCTION hsu_al_init_bam_config_params

DESCRIPTION
Abstraction for the complex initialization parameters.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_init_bam_config_params(hsu_bam_connection_type* connection)
{
  HSU_ASSERT(connection != NULL);

  switch (connection->sio_port_id)
  {
#if 0 // Not required for Twizy
    case SIO_PORT_USB_NET_WWAN:
      /**************BEGIN
       Populate this section for both producer and consumer BAM configuration */
      memset(&hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1], 0, sizeof(bam_pipe_config_type));
      memset(&hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1], 0, sizeof(bam_pipe_config_type));
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].options            = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].dir                = BAM_DIR_PRODUCER;

      if (connection->cdc_ctx_ptr && (connection->cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS))
      {
        hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].mode             = BAM_MODE_SYSTEM;
      }
      else
      {
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].mode               = BAM_MODE_BAM2BAM;
      }
      
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].ul_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].ul_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].peer_base_pa       = HSU_AL_BAM_IPA_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].peer_pipe_num      = 0; /* default val, will be overwritten */
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].ul_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_base_va       = 0;
      if (connection->cdc_ctx_ptr && (connection->cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS))
      {
        hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_size        = hsu_conf_sel_get_ecm_rx_dsm_item_size();
      }
      else
      {
        hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_size        = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].ul_data_fifo.size;
      }

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].options            = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].dir                = BAM_DIR_CONSUMER;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].peer_base_pa       = HSU_AL_BAM_IPA_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].peer_pipe_num      = 1; /* default val, will be overwritten */
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_data_fifo.size;
      //connection->producer_pipe_index                = 1; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->producer_pipe_config               = &hsu_al_bam_utils_ctx.bam_producer_config[0];
      //connection->consumer_pipe_index                = 0; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->consumer_pipe_config               = &hsu_al_bam_utils_ctx.bam_consumer_config[0];
      /**************END */
      break;

    case SIO_PORT_USB_RMNET2:
    case SIO_PORT_USB_DPL:
      /**************BEGIN
       Populate this section for both producer and consumer BAM configuration */
      memset(&hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2], 0, sizeof(bam_pipe_config_type));
      memset(&hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2], 0, sizeof(bam_pipe_config_type));
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].options            = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].dir                = BAM_DIR_PRODUCER;

      if (connection->cdc_ctx_ptr && (connection->cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS))
      {
        hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].mode             = BAM_MODE_SYSTEM;
      }
      else
      {
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].mode               = BAM_MODE_BAM2BAM;
      }

#if 0
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].ul_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].ul_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].peer_base_pa       = HSU_AL_BAM_IPA_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].peer_pipe_num      = 2;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].ul_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_base_va       = 0;
      if (connection->cdc_ctx_ptr && (connection->cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS))
      {
        hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_size        = hsu_conf_sel_get_ecm_rx_dsm_item_size();
      }
      else
      {
        hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_size        = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].ul_data_fifo.size;
      }
#endif

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].options            = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].dir                = BAM_DIR_CONSUMER;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].peer_base_pa       = HSU_AL_BAM_IPA_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].peer_pipe_num      = 3;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_data_fifo.size;
      /**************END */
#if 0
      //connection->producer_pipe_index                = 3; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->producer_pipe_config               = &hsu_al_bam_utils_ctx.bam_producer_config[1];
#endif

      connection->producer_pipe_index                = HSU_AL_BAM_INVALID_PIPE_INDEX;
      //connection->consumer_pipe_index                = 2; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->consumer_pipe_config               = &hsu_al_bam_utils_ctx.bam_consumer_config[1];
      break;
    case SIO_PORT_USB_RMNET3:
      /**************BEGIN
       Populate this section for both producer and consumer BAM configuration */
      memset(&hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3], 0, sizeof(bam_pipe_config_type));
      memset(&hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3], 0, sizeof(bam_pipe_config_type));
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].options            = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].dir                = BAM_DIR_PRODUCER;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].ul_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].ul_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].peer_base_pa       = HSU_AL_BAM_IPA_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].peer_pipe_num      = 4;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].ul_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_3].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].ul_data_fifo.size;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].options            = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].dir                = BAM_DIR_CONSUMER;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].peer_base_pa       = HSU_AL_BAM_IPA_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].peer_pipe_num      = 5;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_3].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_3].dl_data_fifo.size;
      /**************END */
      //connection->producer_pipe_index                = 5; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->producer_pipe_config               = &hsu_al_bam_utils_ctx.bam_producer_config[2];
      //connection->consumer_pipe_index                = 4; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->consumer_pipe_config               = &hsu_al_bam_utils_ctx.bam_consumer_config[2];
      break;
#endif

     case SIO_PORT_USB_QDSS:
       /**************BEGIN */
      
      memset(&hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS], 0, sizeof(bam_pipe_config_type));
      
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].options            = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].dir                = BAM_DIR_CONSUMER;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_QDSS].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_QDSS].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].evt_thresh         = HSU_AL_BAM_EVENT_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].lock_group         = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].peer_base_pa       = HSU_AL_BAM_QDSS_BAM_BASE_ADDR;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].peer_pipe_num      = HSU_AL_BAM_QDSS_BAM_PIPE_NUM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_QDSS].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_QDSS].dl_data_fifo.size;
      /**************END */
      /* QDSS has only consumer pipe. Set the producer pipe index to magic number */
      connection->producer_pipe_index                = HSU_AL_BAM_INVALID_PIPE_INDEX;

      /* consumer pipe only for QDSS */
      connection->consumer_pipe_index                = HSU_QDSS_BAM_INDEX;
      connection->consumer_pipe_config               = &hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS];
      break;

    case SIO_PORT_USB_MDM:
      /* ACM will go on 3rd hardware accelerated interface by design */
      break;

    default:
      HSU_ERR_FATAL("hsu_al_init_bam_config_params: invalid connection type", 0 , 0, 0);
      break;
  }
  hsu_al_bam_utils_ctx.connections[(hsu_al_bam_utils_ctx.hsu_bam_connection_index)++] = connection;
}

#ifdef FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK
/*===========================================================================

FUNCTION hsu_al_init_loopback_bam_config_params

DESCRIPTION
Abstraction for the complex initialization parameters.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_init_loopback_bam_config_params(hsu_bam_connection_type* connection)
{
  switch (connection->sio_port_id)
  {
#if 0 // Not required for Twizy
    case SIO_PORT_USB_NET_WWAN:
      memset(&hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1], 0, sizeof(bam_pipe_config_type));
      memset(&hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1], 0, sizeof(bam_pipe_config_type));
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].options            = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].dir                = BAM_DIR_PRODUCER;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].lock_group         = 0;

      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].peer_base_pa       = HSU_AL_BAM_USB_BASE;

      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].peer_pipe_num      = connection->consumer_pipe_index;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_1].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_data_fifo.size;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].options            = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].dir                = BAM_DIR_CONSUMER;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].lock_group         = 0;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].peer_base_pa       = HSU_AL_BAM_USB_BASE;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].peer_pipe_num      = connection->producer_pipe_index;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_1].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_1].dl_data_fifo.size;
      //connection->producer_pipe_index                = 1; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->producer_pipe_config               = &hsu_al_bam_utils_ctx.bam_producer_config[0];
      //connection->consumer_pipe_index                = 0; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->consumer_pipe_config               = &hsu_al_bam_utils_ctx.bam_consumer_config[0];
      break;
      
    case SIO_PORT_USB_RMNET2:
      memset(&hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2], 0, sizeof(bam_pipe_config_type));
      memset(&hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2], 0, sizeof(bam_pipe_config_type));
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].options            = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].dir                = BAM_DIR_PRODUCER;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].lock_group         = 0;

      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].peer_base_pa       = HSU_AL_BAM_USB_BASE;

      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].peer_pipe_num      = connection->consumer_pipe_index;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_producer_config[HSU_AL_BAM_NET_2].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_data_fifo.size;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].options            = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].dir                = BAM_DIR_CONSUMER;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].mode               = BAM_MODE_BAM2BAM;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].desc_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].desc_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_desc_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].desc_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_desc_fifo.size;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].evt_thresh         = HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].lock_group         = 0;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].peer_base_pa     = HSU_AL_BAM_USB_BASE;

      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].peer_pipe_num      = connection->producer_pipe_index;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].data_base_pa       = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_data_fifo.pa_addr;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].data_base_va       = 0;
      hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_NET_2].data_size          = hsu_al_bam_ul_dl_fifo_config[HSU_AL_BAM_NET_2].dl_data_fifo.size;
      /**************END */
      //connection->producer_pipe_index                = 3; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->producer_pipe_config               = &hsu_al_bam_utils_ctx.bam_producer_config[1];
      //connection->consumer_pipe_index                = 2; /* populated systematically by hsu_al_ecm_suspend_resume_mode_changed_cb() */
      connection->consumer_pipe_config               = &hsu_al_bam_utils_ctx.bam_consumer_config[1];
      break;
#endif
    case SIO_PORT_USB_RMNET3:
    case SIO_PORT_USB_QDSS:
    case SIO_PORT_USB_MDM:
      /* do nothing to configure loopback */
      break;

    default:
      HSU_ERR_FATAL("hsu_al_init_loopback_bam_config_params: invalid connection type", 0 , 0, 0);
      break;
  }
  hsu_al_bam_utils_ctx.connections[(hsu_al_bam_utils_ctx.hsu_bam_connection_index)++] = connection;
}
#endif /* FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK */

void hsu_al_bam_init_pipes_for_ipa_and_bam(hsu_bam_connection_type* connection)
{
  bam_pipe_config_type peer_bam_info;

  bam_handle qdss_pipe_handle = NULL;
  uint32  qdss_pipe_num;

  switch (connection->sio_port_id)
  {
#if 0 // Not required for Twizy
    case SIO_PORT_USB_NET_WWAN:
      /* producer pipe */
      connection->producer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                                connection->producer_pipe_index, 
                                                connection->producer_pipe_config,
                                                NULL); /* no callback required for producer type */
      if (!connection->producer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG,BAM_LOG, "net_wwan, pro_idx %u", connection->producer_pipe_index);

      /* consumer pipe */
      connection->consumer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                                connection->consumer_pipe_index,
                                                connection->consumer_pipe_config,
                                                &(connection->consumer_pipe_wakeup_event_cb));
      if (!connection->consumer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG,BAM_LOG, "net_wwan, con_idx %u", connection->consumer_pipe_index);
      break;

    case SIO_PORT_USB_RMNET2:
    case SIO_PORT_USB_DPL:
      /* consumer pipe */
      connection->consumer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle,
                                                connection->consumer_pipe_index,
                                                connection->consumer_pipe_config,
                                                &(connection->consumer_pipe_wakeup_event_cb));
      if (!connection->consumer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG,BAM_LOG, "DPL|RMNET2, con_idx %u", connection->consumer_pipe_index);
      break;

    case SIO_PORT_USB_RMNET3:
      /* producer pipe */
      connection->producer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                                connection->producer_pipe_index, 
                                                connection->producer_pipe_config,
                                                NULL); /* no callback required for producer type */
      if (!connection->producer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG,BAM_LOG, "RMNET3, pro_idx %u", connection->producer_pipe_index);

      /* consumer pipe */
      connection->consumer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                                connection->consumer_pipe_index,
                                                connection->consumer_pipe_config,
                                                &(connection->consumer_pipe_wakeup_event_cb));
      if (!connection->consumer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG,BAM_LOG, "RMNET3, con_idx %u", connection->consumer_pipe_index);
      break;
#endif
     case SIO_PORT_USB_QDSS:
     memscpy(&peer_bam_info, sizeof(bam_pipe_config_type), &hsu_al_bam_utils_ctx.bam_consumer_config[HSU_AL_BAM_QDSS], sizeof(bam_pipe_config_type));
     peer_bam_info.dir = BAM_DIR_PRODUCER;

     peer_bam_info.peer_base_pa   = HSU_AL_BAM_USB_BASE;

     peer_bam_info.peer_pipe_num  = HSU_QDSS_BAM_INDEX;
     peer_bam_info.evt_thresh     = HSU_AL_BAM_EVENT_THRESHOLD;
    

     if(qdss_per_request_pipe(&peer_bam_info,
                                    &qdss_pipe_handle,
                                    &qdss_pipe_num))
     {
       HSU_ERR_FATAL("qdss_per_request_pipe: QDSS BAM init failed", 0, 0, 0);
     }

     connection->consumer_pipe_config->peer_pipe_num = qdss_pipe_num;
     
     connection->consumer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                               connection->consumer_pipe_index,
                                               connection->consumer_pipe_config,
                                               &(connection->consumer_pipe_wakeup_event_cb));
      if (!connection->consumer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG,BAM_LOG, "qdss, con_idx %u", connection->consumer_pipe_index);
      break;

    case SIO_PORT_USB_MDM:
      /* ACM will go on 3rd hardware accelerated interface by design */
      break;

    default:
      HSU_ERR_FATAL("hsu_al_bam_allocate_pipes: invalid connection type", 0 , 0, 0);
      break;
  }

  hsu_al_bam_utils_ctx.ul_pipes_flow_enabled = TRUE;
  hsu_al_bam_utils_ctx.dl_pipes_flow_enabled = TRUE;
}

#ifdef FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK
void hsu_al_bam_init_loopback_pipes(hsu_bam_connection_type* connection)
{
  switch (connection->sio_port_id)
  {
#if 0 // Not required for Twizy
    case SIO_PORT_USB_NET_WWAN:
    case SIO_PORT_USB_RMNET2:
      /* producer pipe */
      connection->producer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                                connection->producer_pipe_index, 
                                                connection->producer_pipe_config,
                                                NULL); /* no callback required for producer type */
      if (!connection->producer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "producer_pipe_index %u", connection->producer_pipe_index);

      /* consumer pipe */
      connection->consumer_pipe = bam_pipe_init(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, 
                                                connection->consumer_pipe_index,
                                                connection->consumer_pipe_config,
                                                &(connection->consumer_pipe_wakeup_event_cb));
      if (!connection->consumer_pipe)
      {
        HSU_ERR_FATAL("bam_pipe_init: BAM pipe init failure", 0, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "consumer_pipe_index %u", connection->consumer_pipe_index);
      break;
#endif
    case SIO_PORT_USB_RMNET3:
    case SIO_PORT_USB_QDSS:
    case SIO_PORT_USB_MDM:
      /* Do nothing to init the bam pipes */
      break;
    default:
      HSU_ERR_FATAL("hsu_al_bam_init_loopback_pipes: invalid connection type", 0 , 0, 0);
      break;
  }
}
#endif /* FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK */

/*===========================================================================

FUNCTION hsu_al_bam_allocate_pipes

DESCRIPTION
Allocate IN and OUT BAM pipes.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_allocate_pipes
(
  hsu_bam_connection_type *connection
)
{
  HSU_ULOG_1(NO_MSG, BAM_LOG, "connection 0x%08X", (uint32)connection);
  /* make the function multiple calls safe */
  if (!connection->producer_pipe && !connection->consumer_pipe)
  {
#ifndef FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK
    hsu_al_init_bam_config_params(connection);
#else /* FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK */
    hsu_al_init_loopback_bam_config_params(connection);
#endif /* FEATURE_HS_USB_BAM2BAM_SELF_LOOPBACK */
  }

  return TRUE;
} /* hsu_al_bam_allocate_pipes */

/*===========================================================================

FUNCTION hsu_al_bam_register_wakeup_event

DESCRIPTION
Enable pipe wakeup event for the given BAM pipe and register the given event 
object for wakeup event.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful registration. FALSE otherwise.

SIDE EFFECTS
Enable pipe wakeup interrupt in BAM HW

===========================================================================*/
boolean hsu_al_bam_register_wakeup_event
(
  hsu_bam_connection_type* connection
)
{
  bam_status_type status;

  if (connection && connection->consumer_pipe)
  {
    status = bam_pipe_setirqmode(connection->consumer_pipe, 1, BAM_O_WAKEUP | BAM_O_WAKEUP_IS_ONESHOT);
    if (BAM_SUCCESS != status)
    {
      HSU_ERR_FATAL("hsu_al_bam_register_wakeup_event failed", 0, 0, 0);
    }
    HSU_ULOG_1(NO_MSG, BAM_LOG, "bam_pipe_setirqmode_reg_wakeup, consumer_pipe 0x%08X", (uint32)connection->consumer_pipe);
  }

  return TRUE;
} /* hsu_al_bam_register_wakeup_event */

/*===========================================================================

FUNCTION hsu_al_bam_unregister_wakeup_event

DESCRIPTION
Disable pipe wakeup event for the given BAM pipe. This will unregister any 
event object registered for wakeup event.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful unregistration. FALSE otherwise.

SIDE EFFECTS
Disable pipe wakeup interrupt in BAM HW

===========================================================================*/
boolean hsu_al_bam_unregister_wakeup_event
(
  hsu_bam_connection_type* connection
)
{
  bam_status_type status;
  if (connection && connection->consumer_pipe)
  {
    status = bam_pipe_setirqmode(connection->consumer_pipe, 0, BAM_EVENT_INVALID);
    if (BAM_SUCCESS != status)
    {
      HSU_ERR_FATAL("hsu_al_bam_unregister_wakeup_event failed", 0, 0, 0);
    }
    HSU_ULOG_1(NO_MSG, BAM_LOG, "consumer_pipe 0x%08X", (uint32)connection->consumer_pipe);
  }
  return TRUE;
} /* hsu_al_bam_unregister_wakeup_event */

/*===========================================================================

FUNCTION hsu_al_bam_alloc

DESCRIPTION
Allocate a buffer of pipe memory.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_alloc
(
  uint32 uBytes, 
  uint32 align,
  void** virt_address, 
  void** phys_address, 
  void** handle
)
{

//  DAL_bam_MemBufferType bam_buf;
//  uint32 align_offset;
//  DALSYSMemInfo dalmeminfo;
//  uint32 desc_fifo_size = 0x100;
//
//  /* Allocate more than the required size, to allow alignment */
//  bam_buf.uSize = uBytes + align;
//  bam_buf.uMinSize = 0;
//
//  // Allocate a physically contiguous buffer for Descriptor FIFO
//  if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
//        DALSYS_MEM_PROPS_PHYS_CONT,
//        DALSYS_MEM_ADDR_NOT_SPECIFIED,
//        DALSYS_MEM_ADDR_NOT_SPECIFIED,
//        desc_fifo_size,
//        &bam_buf,
//        NULL) )
//  {
//    *bam_buf = NULL;
//    return;
//  }
//  DALSYS_MemInfo(bam_buf, &dalmeminfo);
//
//  (void)memset(bam_buf.pBase, 0, bam_buf.uSize);
//
//  align_offset = (align - bam_buf.uPhysBase % align) % align;
//
//  *handle = (void*)bam_buf.uPhysBase;
//  /* Re-align the buffer to ensure alignment requirement is met */
//  *phys_address = (void*)(bam_buf.uPhysBase + align_offset);
//  *virt_address = (void*)((uint32)bam_buf.pBase + align_offset);
//
//  HSU_MSG_MED_3("DalBam_AllocMem, virt: %d phys: %d handle: %d", 
//    *virt_address, *phys_address, *handle);
//
//  hsu_al_bam_utils_system_read_pipe.handle = *handle;
//  hsu_log(HSU_AL_BAM_ALLOC, HSU_AL_BAM_OFFSET, HSU_AL_BAM_DAL_API__ALLOC_MEM, HSU_AL_BAM_OFFSET, dal_res);
  
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_bam_free

DESCRIPTION
Free an allocated buffer of pipe memory.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_free
(
   void* handle
)
{
//  DALResult dal_res;
//  DAL_bam_MemBufferType bam_buf;
//
//  bam_buf.uPhysBase = (uint32)handle;
//
//
//  dal_res = DalBam_FreeMem(hsu_al_bam_utils_system_read_pipe.handle, &bam_buf);
//  dal_res = DALSYS_Free(&bam_buf);
//  hsu_log(HSU_AL_BAM_FREE, HSU_AL_BAM_OFFSET, HSU_AL_BAM_DAL_API__FREE_MEM, HSU_AL_BAM_OFFSET, dal_res);
//  if (dal_res != DAL_SUCCESS)
//  {
//    HSU_MSG_ERROR_1("DalBam_FreeMem failed %d", dal_res, 0, 0);
//  }
}

#ifndef CI_MERGE
static hsu_bam_endpoint_type hsu_al_bam_get_usb_endpoint(void)
{
  return HSU_BAM_ENDPOINT_USB1;
}
#endif
/*===========================================================================

FUNCTION hsu_al_bam_reset

DESCRIPTION
reset BAM pipe

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_reset(void)
{
  bam_status_type hsu_bam_status;
  if (hsu_al_bam_utils_ctx.peripheral_bam.bam_handle)
  {
  #ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
    if (FALSE == hsu_al_bam_utils_ctx.ci_bam_already_reset)
    {
      hsu_al_bam_utils_ctx.ci_bam_already_reset = TRUE;
  #endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
      // flow disabled is explicitly set here to ensure the reversion to non-bam mode occurs
      hsu_al_bam_utils_ctx.ul_pipes_flow_enabled = FALSE;
      hsu_al_bam_utils_ctx.dl_pipes_flow_enabled = FALSE;
      hsu_al_bam_utils_revert_to_non_bam_mode();

      hsu_bam_status = bam_reset(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("bam_reset returned an error", 0, 0, 0);
      }
      HSU_ULOG_FUNC(BAM_LOG);
  #ifndef CI_MERGE
      if (HSU_CORE_IDX_PRIMARY == HSU_CORE_CONFIG_USB3_IDX)
      {
        hsu_phy_utils_enable_BAM();
      }
  #endif
  #ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
    }
  #endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
  }
}


/*===========================================================================

FUNCTION hsu_al_bam_pipe_disable

DESCRIPTION
Disables the BAM pipe (this is a lightweight version that does not require full pipe deinit)

DEPENDENCIES
None.

RETURN VALUE
DAL_bam_ConnectType

SIDE EFFECTS
None.

===========================================================================*/
static void hsu_al_bam_pipe_disable
(
  hsu_bam_connection_type* connection, boolean ul, boolean dl
)
{
  bam_status_type hsu_bam_status;
  if (connection)
  {
    if (ul && connection->producer_pipe)
    {
      hsu_bam_status = bam_pipe_disable(connection->producer_pipe);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("bam_pipe_disable returned an error", hsu_bam_status, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "ul, producer_pipe 0x%08X", (uint32)connection->producer_pipe); 
    }

    if (dl && connection->consumer_pipe)
    {
      hsu_al_bam_unregister_wakeup_event(connection);

      hsu_bam_status = bam_pipe_disable(connection->consumer_pipe);
      if (BAM_SUCCESS != hsu_bam_status)
      {
        HSU_ERR_FATAL("bam_pipe_disable returned an error", hsu_bam_status, 0, 0);
      }
      HSU_ULOG_1(NO_MSG, BAM_LOG, "dl, consumer_pipe 0x%08X", (uint32)connection->consumer_pipe); 
    }
  }
}

/*===========================================================================

FUNCTION hsu_al_bam_pipe_connect

DESCRIPTION
Connect to IPA pipe (run once upon bootup)

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_ipa_pipe_connect
(
  hsu_bam_connection_type* connection
)
{
  /* the guard against multiple instances of this call must be implemented in the caller (in this case ECM) */
  HSU_ULOG_1(NO_MSG, BAM_LOG, "connection 0x%08X", (uint32)connection);
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_disable_all_pipe

DESCRIPTION
Lightweight version that disables all BAM pipes opened
DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_disable_all_pipe(boolean ul, boolean dl)
{
  int i;

  if ((FALSE == ul) || (FALSE == hsu_al_bam_utils_ctx.ul_pipes_flow_enabled))
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "filtered, ul");
  }
  else
  {
    hsu_al_bam_utils_ctx.ul_pipes_flow_enabled = FALSE;
    for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
    { 
      if (hsu_al_bam_utils_ctx.connections[i]->producer_pipe)
      {
        hsu_al_bam_pipe_disable(hsu_al_bam_utils_ctx.connections[i], ul, FALSE);
      }
    }
  }

  if ((FALSE == dl) || (FALSE == hsu_al_bam_utils_ctx.dl_pipes_flow_enabled))
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "filtered, dl");
  }
  else
  {
    hsu_al_bam_utils_ctx.dl_pipes_flow_enabled = FALSE;
    for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
    {
      if (hsu_al_bam_utils_ctx.connections[i]->consumer_pipe)
      {
        hsu_al_bam_pipe_disable(hsu_al_bam_utils_ctx.connections[i], FALSE, dl);
      }
    }
  }
}

#ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
/*===========================================================================
FUNCTION hsu_al_bam_utils_allow_subsequent_reset_bam
===========================================================================*/
void hsu_al_bam_utils_allow_subsequent_reset_bam(void)
{
  HSU_ULOG_FUNC(BAM_LOG);
  hsu_al_bam_utils_ctx.ci_bam_already_reset = FALSE;
}

/*===========================================================================
FUNCTION hsu_al_bam_utils_reset_bam
===========================================================================*/
void hsu_al_bam_utils_reset_bam(void)
{
  if (HSU_CORE_CONFIG_USB3_IDX == HSU_CORE_IDX_PRIMARY)
  {
    if (FALSE == hsu_al_bam_utils_ctx.ci_bam_already_reset)
    {
      // BAM Pipe Reset is resetting the HSIC BAM HW.  It only needs to be done 
      //  once for all pipes on HSIC BAM.  Use the first instance's handle to 
      //  reset the HSIC BAM. 
      if (hsu_al_bam_utils_ctx.hsu_bam_connection_index > 0)
      {  
        hsu_al_bam_reset(); 
      }
    }
  }
}
#endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER


#ifndef CI_MERGE
/*===========================================================================

FUNCTION hsu_al_bam_utils_enable_all_pipes

DESCRIPTION
Reset all BAM pipes and then enable them (lightweight without pipe de-init/init)

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_reset_bam_and_enable_all_pipe(boolean ul, boolean dl)
  {
    if (HSU_CORE_CONFIG_USB3_IDX == HSU_CORE_IDX_PRIMARY)
    {
#ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
      if (FALSE == hsu_al_bam_utils_ctx.ci_bam_already_reset)
      {
        hsu_al_bam_utils_ctx.ci_bam_already_reset = TRUE;
#endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
  
        if (ul && dl)
        {
          // For CI core, there isn't any use case calling for asymmetric UL/DL pipe reset and enable;
          // thus it is safe to only apply the reset ONCE in the former part of the sequence.
      
          // This section of the code should never be duplicated in the latter section.
          // This BAM reset is safe as long as it's not used on SNPS core
  
          // BAM Pipe Reset is resetting the HSIC BAM HW.  It only needs to be done 
          //  once for all pipes on HSIC BAM.  Use the first instance's handle to 
          //  reset the HSIC BAM. 
          if (hsu_al_bam_utils_ctx.hsu_bam_connection_index > 0)
          {  
            hsu_al_bam_reset(); 
          }
        }
        else
        {
          HSU_ERR_FATAL("CI Core should never have asymmetric pipe reset/enable", 0, 0, 0);
        }
#ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
      }
#endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
    }
    hsu_al_bam_utils_enable_all_pipes(ul,dl);
}
#endif /*CI_MERGE*/

void hsu_al_bam_utils_enable_all_pipes(boolean ul, boolean dl)
{
  int i;

  if ((FALSE == ul) || hsu_al_bam_utils_ctx.ul_pipes_flow_enabled)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "filtered, rst_bam, enable_all_ul_pipe");
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "rst_bam, enable_all_ul_pipe");
    for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
    {
      /* BAM API will implictly perform a pipe reset.  BAM reset is no longer required */
      if (hsu_al_bam_utils_ctx.connections[i]->producer_pipe) 
      {
        hsu_al_bam_pipe_enable(hsu_al_bam_utils_ctx.connections[i], ul, FALSE);
      }
    }
    hsu_al_bam_utils_ctx.ul_pipes_flow_enabled = TRUE;
  }

  if ((FALSE == dl) || hsu_al_bam_utils_ctx.dl_pipes_flow_enabled)
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "filtered, rst_bam, enable_all_dl_pipe");
  }
  else
  {
    HSU_ULOG(NO_MSG, BAM_LOG, "rst_bam, enable_all_dl_pipe");
    for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
    {
      /* BAM API will implictly perform a pipe reset.  BAM reset is no longer required */
      if (hsu_al_bam_utils_ctx.connections[i]->consumer_pipe)
      {
        hsu_al_bam_pipe_enable(hsu_al_bam_utils_ctx.connections[i], FALSE, dl);
      }
    }
    hsu_al_bam_utils_ctx.dl_pipes_flow_enabled = TRUE;
  }
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_get_pipes_status

DESCRIPTION
return FALSE when hsu_al_bam_utils_disconnect_all_pipe has been called, but
hsu_al_bam_utils_reset_all_pipe has not been called. 

DEPENDENCIES
None.

RETURN VALUE
boolean.

SIDE EFFECTS
None.

===========================================================================*/
boolean  hsu_al_bam_utils_get_dl_pipes_status(void)
{
  // This API is used for registering wakeup due to DL traffic from BAM pipes.
  // Thus returning only DL pipe status is safe
  return hsu_al_bam_utils_ctx.dl_pipes_flow_enabled;
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_flush_all_pipe

DESCRIPTION
wait until all of BAM pipes get emptied. It will wait for 
MAX_BAM_FLUSH_DURATION_MS ms for flushing in worst case.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_flush_all_pipe(void)
{
  /* trigger pipe API no longer supported */
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_are_DL_pipes_empty

DESCRIPTION
Check if all BAM DL pipes are empty

DEPENDENCIES
None.

RETURN VALUE
TRUE if all BAM DL pipes are empty, and FALSE if any BAM DL pipe is found to
be not empty.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_utils_are_DL_pipes_empty(void)
{
  int i;

  for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {
    if (hsu_al_bam_utils_ctx.connections[i] && hsu_al_bam_utils_ctx.connections[i]->consumer_pipe)
    {
      if (SIO_PORT_USB_QDSS == hsu_al_bam_utils_ctx.connections[i]->sio_port_id)
      {
        // Do not perform IPA handshake for SNPS USB 3.0 QDSS 
        // since BAM will get entire RESET if it is done breaking QDSS
        HSU_ULOG_1(NO_MSG, BAM_LOG, "tigger_remote_wakeup_for_qdss, connection_index %u", i);
        return FALSE;
      }
      
      if (!bam_pipe_isempty(hsu_al_bam_utils_ctx.connections[i]->consumer_pipe))
      {
        HSU_ULOG_1(NO_MSG, BAM_LOG, "false_log, connection_index %u", i);
        return FALSE;
      }
    }
  }
  HSU_ULOG_1(NO_MSG, BAM_LOG, "true_log, connection_index %u", i);
  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_are_UL_pipes_empty

DESCRIPTION
Check if all BAM UL pipes are empty

DEPENDENCIES
None.

RETURN VALUE
TRUE if all BAM UL pipes are empty, and FALSE if any BAM UL pipe is found to
be not empty.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_utils_are_UL_pipes_empty(void)
{
  boolean ret = TRUE;
  //If no IPA then return TRUE
  return ret;
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_print_all_pipe

DESCRIPTION
Print BAM pipe related info to F3

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_print_all_pipe(void)
{
  uint32 is_empty; 
  int i;
  uint32 consumer_pipe_not_empty_bitfield = 0;
  uint32 producer_pipe_not_empty_bitfield = 0;

  for(i=0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {
    if (hsu_al_bam_utils_ctx.connections[i] && hsu_al_bam_utils_ctx.connections[i]->consumer_pipe)
    {
      is_empty = bam_pipe_isempty(hsu_al_bam_utils_ctx.connections[i]->consumer_pipe);
      consumer_pipe_not_empty_bitfield |= (is_empty ? 0 : 1) << i;
    }

    if (hsu_al_bam_utils_ctx.connections[i] && hsu_al_bam_utils_ctx.connections[i]->producer_pipe)
    {
      is_empty = bam_pipe_isempty(hsu_al_bam_utils_ctx.connections[i]->producer_pipe);
      producer_pipe_not_empty_bitfield |= (is_empty ? 0 : 1) << i;
    }
  }
  HSU_ULOG_2(HIGH_MSG, BAM_LOG, "consumer_pipe_not_empty_bitfield 0x%08X, producer_pipe_not_empty_bitfield 0x%08X",
    consumer_pipe_not_empty_bitfield, producer_pipe_not_empty_bitfield);  
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_get_all_desc_fifo_size

DESCRIPTION
Helper function to iterate through and obtain all desc fifo sizes

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static uint32 hsu_al_bam_utils_get_all_desc_fifo_size(void)
{
  uint32 i = 0;
  uint32 size_result = 0;
  for (i = 0; i < HSU_AL_BAM_MAX_IFACE_TYPE; i++)
  {
    size_result += hsu_al_bam_ul_dl_fifo_config[i].ul_desc_fifo.size;
    size_result += hsu_al_bam_ul_dl_fifo_config[i].dl_desc_fifo.size;
  }
  return size_result;
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_get_all_data_fifo_size

DESCRIPTION
Helper function to iterate through and obtain all data fifo sizes

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static uint32 hsu_al_bam_utils_get_all_data_fifo_size(void)
{
  uint32 i = 0;
  uint32 size_result = 0;
  for (i = 0; i < HSU_AL_BAM_MAX_IFACE_TYPE; i++)
  {
    size_result += hsu_al_bam_ul_dl_fifo_config[i].ul_data_fifo.size;
    size_result += hsu_al_bam_ul_dl_fifo_config[i].dl_data_fifo.size;
  }
  return size_result;
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_build_active_pipe_mask

DESCRIPTION
Helper function to build a mask that identifies active pipes

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
static uint32 hsu_al_bam_utils_build_active_pipe_mask(void)
{
  uint32 i = 0;
  uint32 mask_result = 0;
  for (i = 0; i < MAX_BAM_CONNECTIONS; i++)
  {
    if (hsu_al_bam_utils_ctx.connections[i] && hsu_al_bam_utils_ctx.connections[i]->producer_pipe)
    {
      mask_result |= 1 << hsu_al_bam_utils_ctx.connections[i]->producer_pipe_index;
    }
    if (hsu_al_bam_utils_ctx.connections[i] && hsu_al_bam_utils_ctx.connections[i]->consumer_pipe)
    {
      mask_result |= 1 << hsu_al_bam_utils_ctx.connections[i]->consumer_pipe_index;
    }
  }
  return mask_result;
}


/*===========================================================================

FUNCTION hsu_al_bam_utils_core_dump

DESCRIPTION
External function to collect BAM related data in coredump.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_core_dump(void)
{
#if 0 // Removing bam dump
  if (hsu_al_bam_utils_ctx.bam_allocated && hsu_al_bam_utils_ctx.bam_dump)
  {
    bam_coredump_config_type *bam_core_dump_config = &hsu_al_bam_utils_ctx.peripheral_bam.bam_coredump_config;
    bam_core_dump_config->user_buffer = (void*)hsu_al_bam_utils_ctx.bam_dump;
    bam_core_dump_config->buf_size = hsu_al_bam_utils_ctx.bam_dump_size;
    bam_core_dump_config->pipe_mask = hsu_al_bam_utils_build_active_pipe_mask(); /* bit mask from all connection->consumer_pipe_index */
    
    if (hsu_al_bam_utils_is_ipa_clock_voted())
    {
      bam_core_dump_config->peer_pipe_mask = hsu_al_bam_utils_build_active_pipe_mask(); /* bit mask of pipe mask to include or exclude dumping of peer pipe */
    }
    else
    {
      /* Do not dump peer pipe when not in DEVICE READY state */
      bam_core_dump_config->peer_pipe_mask = 0;

      HSU_ULOG(NO_MSG, BUS_LOG, "bam_core_dump_for_peer_pipe_skipped_due_to_device_not_ready");
    }

    bam_core_dump_config->descfifo_buf = hsu_al_bam_utils_ctx.bam_dump_desc_fifo_buf;
    bam_core_dump_config->descfifo_size = hsu_al_bam_utils_ctx.bam_dump_desc_fifo_size;
    bam_core_dump_config->datafifo_buf = hsu_al_bam_utils_ctx.bam_dump_data_fifo_buf;
    bam_core_dump_config->datafifo_size = hsu_al_bam_utils_ctx.bam_dump_data_fifo_size;
    bam_core_dump_config->option = 0;

    if (BAM_SUCCESS != bam_core_dump(hsu_al_bam_utils_ctx.peripheral_bam.bam_handle, bam_core_dump_config))
    {
      /* do not ERR FATAL */
    }
  }
#endif
  /* else
   * Do nothing since bam not even allocated. 
   * Do not err fatal again as it will result in a double throw. */
}

/*===========================================================================

FUNCTION hsu_al_bam_utils_peer_all_pipe_disable

DESCRIPTION
External function to disable all IPA pipes

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_peer_all_pipe_disable(boolean ul, boolean dl)
{
  uint32 i;
  hsu_bam_connection_type *bam_conn_ptr;

  for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {
    bam_conn_ptr = hsu_al_bam_utils_ctx.connections[i];
    if(bam_conn_ptr && (bam_conn_ptr->sio_port_id == SIO_PORT_USB_QDSS))
    { 
      HSU_ULOG(NO_MSG, BAM_LOG, "qdss_bam_pipe_disable");
      qdss_bam_pipe_disable();
    }
  }
  return;
}

void hsu_al_bam_utils_peer_all_pipe_enable(boolean ul, boolean dl)
{
  uint32 i;
  hsu_bam_connection_type *bam_conn_ptr;

  for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {    
    bam_conn_ptr = hsu_al_bam_utils_ctx.connections[i];
    if(bam_conn_ptr && (bam_conn_ptr->sio_port_id == SIO_PORT_USB_QDSS))
    { 
      HSU_ULOG(NO_MSG, BAM_LOG, "qdss_bam_pipe_enable");
      qdss_bam_pipe_enable();
    }
  }
  return;
}


/*===========================================================================
hsu_al_bam_utils_ipa_state_chg_suspend
===========================================================================*/
boolean hsu_al_bam_utils_ipa_state_chg_suspend(void)
{
  //NO IPA return TRUE
  return TRUE;
}

/*===========================================================================
hsu_al_bam_utils_ipa_state_chg_resume
===========================================================================*/
void hsu_al_bam_utils_ipa_state_chg_resume(void)
{
  return;
}

/*===========================================================================
hsu_al_bam_utils_ipa_pm_release_clock
===========================================================================*/
void hsu_al_bam_utils_ipa_pm_release_clock(void)
{
  return;
}

/*===========================================================================
hsu_al_bam_utils_ipa_pm_request_clk
===========================================================================*/
void hsu_al_bam_utils_ipa_pm_request_clk(void)
{
  return;
}

/*===========================================================================
hsu_al_bam_utils_dbm_init
===========================================================================*/
void hsu_al_bam_utils_dbm_init(void)
{
  // TODO fix when ECM working on seconcdary core (HSU_CORE_IDX_PRIMARY)
  cdc_dbm_init(hsu_al_bam_utils_ctx.connections[0]->cdc_ctx_ptr);
}

/*===========================================================================
hsu_al_bam_utils_all_dbm_ep_init
===========================================================================*/
void hsu_al_bam_utils_all_dbm_ep_init(boolean ul, boolean dl)
{
  cdc_ctx_t *cdc_ctx_ptr;
  uint32 i;
  
  for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {
    cdc_ctx_ptr = hsu_al_bam_utils_ctx.connections[i]->cdc_ctx_ptr;

    HSU_ASSERT(cdc_ctx_ptr != NULL);
    
    if(ul && (hsu_al_bam_utils_ctx.connections[i]->producer_pipe_index != HSU_AL_BAM_INVALID_PIPE_INDEX)) 
    {
      if ( !(cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS) ) 	
      {
        cdc_dbm_ep_init(cdc_ctx_ptr, hsu_al_bam_utils_ctx.connections[i]->producer_pipe_index);
      }
    }

    if(dl && (hsu_al_bam_utils_ctx.connections[i]->consumer_pipe_index != HSU_AL_BAM_INVALID_PIPE_INDEX))
    {
      cdc_dbm_ep_init(cdc_ctx_ptr, hsu_al_bam_utils_ctx.connections[i]->consumer_pipe_index);
    }
  }
}

/*===========================================================================
hsu_al_bam_utils_all_dbm_ep_reset
===========================================================================*/
void hsu_al_bam_utils_all_dbm_ep_reset(boolean ul, boolean dl)
{
  cdc_ctx_t *cdc_ctx_ptr;
  
  uint32 i;

  for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {
    cdc_ctx_ptr = hsu_al_bam_utils_ctx.connections[i]->cdc_ctx_ptr;

    HSU_ASSERT(cdc_ctx_ptr != NULL);

    if ( !(cdc_ctx_ptr->info.basic.flags & CDC_INIT_SSUSB_UL_BAM_BYPASS) 
      && (SIO_PORT_USB_QDSS != hsu_al_bam_utils_ctx.connections[i]->sio_port_id))
    {
      if (ul && (hsu_al_bam_utils_ctx.connections[i]->producer_pipe_index != HSU_AL_BAM_INVALID_PIPE_INDEX))
      {
        cdc_dbm_ep_reset(cdc_ctx_ptr, hsu_al_bam_utils_ctx.connections[i]->producer_pipe_index);
      }
    }
    if (dl && (hsu_al_bam_utils_ctx.connections[i]->consumer_pipe_index != HSU_AL_BAM_INVALID_PIPE_INDEX))
    {
      cdc_dbm_ep_reset(cdc_ctx_ptr, hsu_al_bam_utils_ctx.connections[i]->consumer_pipe_index);
    }
  }
}


/*===========================================================================
hsu_al_bam_enable_read_write_infinite_qdss_bam_ep
===========================================================================*/
void hsu_al_bam_enable_read_write_infinite_qdss_bam_ep(void)
{
  uint32 i;
  hsu_al_ser_obex_data *  obex_app_ptr = NULL;
  
  for (i = 0; i < hsu_al_bam_utils_ctx.hsu_bam_connection_index; i++)
  {
    if(hsu_al_bam_utils_ctx.connections[i]->sio_port_id == SIO_PORT_USB_QDSS)
    {
      obex_app_ptr = (hsu_al_ser_obex_data *) hsu_al_bam_utils_ctx.connections[i]->cdc_ctx_ptr->app_ctx;

      if(obex_app_ptr->obex_if_data.bam_info.is_bam_mode != TRUE)
      {
        HSU_ULOG(NO_MSG, BAM_LOG, "not_bam_mode");
      }
      else if (obex_app_ptr->obex_if_data.bam_info.bam_flow_enabled)
      {
        HSU_ULOG(NO_MSG, BAM_LOG, "already_done");
      }
      else if (FALSE == obex_app_ptr->base_data.instance_enabled_by_cdc_fd)
      {
        HSU_ULOG(NO_MSG, BAM_LOG, "instance_not_enabled");      
      }
      else if (cdc_sps_read_write_infinite(hsu_al_bam_utils_ctx.connections[i]->cdc_ctx_ptr, 
        (juint8_t)hsu_al_bam_utils_ctx.connections[i]->producer_pipe_index, HSU_AL_BAM_UL,
        (juint8_t)hsu_al_bam_utils_ctx.connections[i]->consumer_pipe_index, HSU_AL_BAM_DL)) 
      {
        HSU_ERR_FATAL("read write infinite failed", 0, 0, 0);
      }
      else
      {
        HSU_ULOG_FUNC(BAM_LOG);
        obex_app_ptr->obex_if_data.bam_info.bam_flow_enabled = TRUE;
      }
    }
  }
  
}

#ifndef CI_MERGE
void hsu_al_bam_utils_revert_to_non_bam_mode(void)
{
  if (HSU_CORE_CONFIG_USB3_IDX == HSU_CORE_IDX_PRIMARY)
  {
    if (FALSE == hsu_al_bam_utils_ctx.ul_pipes_flow_enabled &&
        FALSE == hsu_al_bam_utils_ctx.dl_pipes_flow_enabled)
    {
      // Disable BAM hardware to prevent hsusb/hsic core from going out of sync with BAM hardware.
      hsu_phy_utils_disable_BAM();
    }
  }
}
#endif


/*===========================================================================
hsu_al_bam_utils_ipa_per_drop
===========================================================================*/
void hsu_al_bam_utils_ipa_per_drop(void)
{
}

/*===========================================================================
hsu_al_bam_utils_ipa_per_undrop
===========================================================================*/
void hsu_al_bam_utils_ipa_per_undrop(void)
{
}

/*===========================================================================
hsu_al_bam_utils_ipa_per_enable_clear_data_path
===========================================================================*/
void hsu_al_bam_utils_ipa_per_enable_clear_data_path(void)
{
}

/*===========================================================================
hsu_al_bam_utils_ipa_per_disable_clear_data_path
===========================================================================*/
void hsu_al_bam_utils_ipa_per_disable_clear_data_path(void)
{
}

/*===========================================================================
hsu_al_bam_utils_is_ipa_clock_voted
===========================================================================*/
static boolean hsu_al_bam_utils_is_ipa_clock_voted(void)
{
  return FALSE;
}

/*===========================================================================
hsu_al_bam_utils_ipa_notify_core_speed
===========================================================================*/
void hsu_al_bam_utils_ipa_notify_core_speed(void)
{
}

#endif /* FEATURE_HS_USB_BAM */
