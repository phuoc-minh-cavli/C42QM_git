/*==============================================================================

USB Serial I/Fs (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB

GENERAL DESCRIPTION
Contains the implementation of the API for the serial functions' adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_init
hsu_al_ser_open
hsu_al_ser_close
hsu_al_ser_transmit  
hsu_al_ser_flush_tx
hsu_al_ser_ioctl

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_init must be called prior to performing any other operations

Copyright (c) 2006 - 2010, 2020 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser.c#2 $

when      who     what, where, why
--------  ---     ------------------------------------------------------
05/12/20  cs      Resolved the issue: USB SIO flush call back not getting called in hsu_al_ser driver
05/08/06  esh     First revision of file

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "hsu_al_ser.h"

#include "jerrno.h"
#include "compiler_int.h"
#include "uw_device_cdc_acm.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"

#include "hsu_al_ser_i.h"
#include "jtypes_int.h"
#include "hsu_al_ser_base_int.h"
#include "hsu_al_ser_acm_int.h"
#include "hsu_al_ser_obex.h"
#include "hsu_common_int.h"

#include "err.h"

#include "diagdsm.h"

#include "hsu_conf_sel_stack_utils_int.h"

#include "hsu_config_selector.h"

#include "hsu_conf_sel_types_int.h"
#include "jos_int.h"
#include "jslave_init.h"
#include "hsu_conf_sel_i.h"

#include "sio.h"
#include "hsu_core_config_int.h"
#include "hsu_conf_sel_core_config.h"

#include "jusb_common_int.h"
#include "jusb_core_fd_int.h"
#include "jusb_core.h"
#include "cdc_fd.h"
#include "hsu_phy_utils_int.h"
#include "hsu_log.h"

#ifdef FEATURE_HS_USB_HEALTH_MONITOR
#include "hsu_health_diag_init.h"
#endif /* FEATURE_HS_USB_HEALTH_MONITOR */
#include "hsu_qurt_util.h"


/*==============================================================================

LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/
/*==============================================================================
Constants and Macros
==============================================================================*/

/* The following are used in hsu_al_ser_close, when no app callback is passed */
#define AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS (100)
#define AL_SER_CLOSE_MAX_TIMEOUT_IN_MSECS \
  (10*AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS)

/*==============================================================================
Typedefs
==============================================================================*/

static struct
{
 
#ifdef FEATURE_HS_USB_CSVT_PORT
  boolean                     mdm_held_by_app[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_ACM_USERS];
  hsu_al_ser_acm_data         mdm_acm_data[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_ACM_USERS];
    /* ACM data struct for Modem I/F, with indication if it is used by app */
  uint8                       num_of_initialized_acm_instances[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS];
  /* 
  * The number of ACM instances that were allocated and initialized (for now, 1 is the 
  * maximal possible value because there's only one instance of the 
  * modem function)
  */

  cdc_handle_t                acm_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS][MAX_NUM_OF_ACM_USERS];
  /* 
  * Keeps the handles or the FDs that the stack initialized.
  * When the configuration will be changed we will use the corresponding handle 
  * for the stack's FD.
  */
#else /* FEATURE_HS_USB_CSVT_PORT */
  /*
  Each of the CDC I/Fs:
  */
  boolean                     mdm_held_by_app;
  hsu_al_ser_acm_data         mdm_acm_data[HSU_MAX_NUM_OF_CORES];
    /* ACM data struct for Modem I/F, with indication if it is used by app */
  uint8                       num_of_initialized_acm_instances[HSU_MAX_NUM_OF_CORES];
  /* 
  * The number of ACM instances that were allocated and initialized (for now, 1 is the 
  * maximal possible value because there's only one instance of the 
  * modem function)
  */

  cdc_handle_t                acm_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS];
  /* 
  * Keeps the handles or the FDs that the stack initialized.
  * When the configuration will be changed we will use the corresponding handle 
  * for the stack's FD.
  */
#endif /* FEATURE_HS_USB_CSVT_PORT */
  boolean                     obex_slot_avail_for_app[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];
  hsu_al_ser_obex_data        data_for_apps_using_obex_if[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];
    /* Array for obex I/Fs, with indication if a slot is avail for usage or not */

  cdc_handle_t                obex_fd_handle[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS][MAX_NUM_OF_OBEX_USERS];
  /* 
  * Keeps the handles or the FDs that the stack initialized.
  * When the configuration will be changed we will use the corresponding handle 
  * for the stack's FD. 
  */
  uint8                       num_of_initialized_obex_instances[HSU_MAX_NUM_OF_CORES][HSU_MAX_NUM_OF_CONFIGS];
  /* 
  * The number of OBEX instances that were allocated and initialized
  */

  /* The following would point to the applicable slot in the above array,
  once hsu_al_ser_open has been issued, and would then be used instead of 
  referencing the above array. */
  hsu_al_ser_acm_data*        mdm_acm_ptr;
#ifdef FEATURE_HS_USB_CSVT_PORT
  hsu_al_ser_acm_data*        csvt_acm_ptr;
#endif /* FEATURE_HS_USB_CSVT_PORT */
  hsu_al_ser_obex_data*       diag_obex_ptr;
#ifdef FEATURE_HS_USB_NMEA    
  hsu_al_ser_obex_data*       nmea_obex_ptr;
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
  hsu_al_ser_obex_data*       ser3_obex_ptr;
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
  hsu_al_ser_obex_data*       ser5_obex_ptr;
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
  hsu_al_ser_obex_data*       ser6_obex_ptr;
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
  hsu_al_ser_obex_data*       dci_obex_ptr;
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
  hsu_al_ser_obex_data*       ser4_obex_ptr;
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
/* This encapsulates the obex data and SPS connection */
  hsu_al_ser_obex_data*       qdss_obex_ptr;
#endif /* FEATURE_HS_USB_QDSS_PORT */


  /* Storage for close and flush_tx callbacks supplied by the app */
  /* Since the app supplies void f(void) callbacks, whereas hsu_al_ser_obex 
  (and later on hsu_al_ser_acm) would like to use AL callbacks with signature 
  void f(hsu_al_ser_base_data_for_ser_if*), then the app callbacks need
  to be saved here and this layer would pass "internal" ones, so once the 
  internal AL callback is called, in the end it will call the app callback, 
  stored here. */
  sio_vv_func_ptr_type        app_close_cb_arr_for_obex[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];
  sio_vv_func_ptr_type        app_flush_tx_cb_arr_for_obex[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_OBEX_USERS];

#ifdef FEATURE_HS_USB_CSVT_PORT
  sio_vv_func_ptr_type        app_close_cb_arr_for_acm[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_ACM_USERS];
  sio_vv_func_ptr_type        app_flush_tx_cb_arr_for_acm[HSU_MAX_NUM_OF_CORES][MAX_NUM_OF_ACM_USERS];  
#else /* FEATURE_HS_USB_CSVT_PORT */
  sio_vv_func_ptr_type        mdm_close_cb;
  sio_vv_func_ptr_type        mdm_flush_tx_cb;  
#endif /* FEATURE_HS_USB_CSVT_PORT */
  uint8                       curr_config_idx[HSU_MAX_NUM_OF_CORES];
} hsu_al_ser_static_data;
  /* This file's variable */


/*---------------------------------------------------------------------------
FORWARD DECLARATIONS
---------------------------------------------------------------------------*/
/* The following functions are for managing the OBEX data array: */
static hsu_al_ser_obex_data* get_avail_obex_data_slot_for_fd(cdc_handle_t handle);

#ifdef FEATURE_HS_USB_CSVT_PORT
static hsu_al_ser_acm_data* get_and_alloc_acm_data_for_app(sio_port_id_type);
#else /* FEATURE_HS_USB_CSVT_PORT */
static hsu_al_ser_acm_data* get_and_alloc_acm_data_for_app(void);
#endif /* FEATURE_HS_USB_CSVT_PORT */

static hsu_al_ser_obex_data* get_and_alloc_obex_data_slot_for_app
(
  sio_port_id_type port_id
);

static void release_obex_data_slot_from_app
(
  hsu_al_ser_obex_data* data_for_obex_if_ptr
);

#ifdef FEATURE_HS_USB_CSVT_PORT
static void release_acm_data_slot_from_app
(
  hsu_al_ser_acm_data* data_for_acm_if_ptr 
    /* Pointer to acm data structure to release */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

/* The following functions are to guarantee OBEX data arrays, as well as ACM data struct
are valid prior to any hsu_al_ser_open: */
static void init_obex_data_arrays_once(void);
#ifdef FEATURE_HS_USB_CSVT_PORT
static void init_acm_data_array_once(void);
#else /* FEATURE_HS_USB_CSVT_PORT */
static void init_acm_data_once(void);
#endif /* FEATURE_HS_USB_CSVT_PORT */

/* The following functions are for managing app supplied flush tx and close callbacks, 
for OBEX I/Fs */
static void add_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
  sio_vv_func_ptr_type app_close_cb
);

#ifdef FEATURE_HS_USB_CSVT_PORT
static void add_app_close_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type app_close_cb
                                                  /* App supplied callback */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

static sio_vv_func_ptr_type get_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);

#ifdef FEATURE_HS_USB_CSVT_PORT
static sio_vv_func_ptr_type get_app_close_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

static void remove_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);

#ifdef FEATURE_HS_USB_CSVT_PORT 
static void remove_app_close_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

static void add_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
  sio_vv_func_ptr_type app_flush_tx_cb
);

#ifdef FEATURE_HS_USB_CSVT_PORT
static void add_app_flush_tx_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type acm_flush_tx_cb
                                                  /* App supplied callback */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

static sio_vv_func_ptr_type get_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);

#ifdef FEATURE_HS_USB_CSVT_PORT
static sio_vv_func_ptr_type get_app_flush_tx_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

static void remove_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
);

#ifdef FEATURE_HS_USB_CSVT_PORT
static void remove_app_flush_tx_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
);
#endif /* FEATURE_HS_USB_CSVT_PORT */

/* The following functions are the callbacks given by hsu_al_ser to either the
OBEX or ACM data structs to be invoked in case of flush_tx or close 
(respectively) */
static void al_ser_on_close(void*);
static void al_ser_on_flush_tx(void*);

static boolean data_ptr_is_freed_from_app
(
  const hsu_al_ser_base_data_for_ser_if* data_if_ptr
);

static void al_ser_internal_flush_tx
(
  sio_port_id_type port_id,
  hsu_al_ser_base_op_complete_cb internal_flush_tx_done_cb
);
/* This function is the flush_tx_done cb when flush is done as part of the close operations.
   It is used as part of hsu_al_ser_close, and performs the actual close operations*/
static void al_ser_complete_close_after_flush_tx
(
  void*
);
/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/

/*===========================================================================
FUNCTION hsu_al_ser_init()
===========================================================================*/
boolean hsu_al_ser_init
(
  void 
)
{
  uint8 core_idx;

  for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
  {
    hsu_al_ser_static_data.curr_config_idx[core_idx] = 0;
  }
  /* Init all serial instances no matter how many, so that if an application 
  wishes to open the corresponding HSU port, it will receive a proper error
  (otherwise, an assert is issued in hsu_al_ser_open) */
#ifdef FEATURE_HS_USB_CSVT_PORT
  init_acm_data_array_once();
#else /* FEATURE_HS_USB_CSVT_PORT */
  init_acm_data_once();
#endif /* FEATURE_HS_USB_CSVT_PORT */
  init_obex_data_arrays_once();

  return TRUE;
}

/*===========================================================================
FUNCTION hsu_al_ser_open()
===========================================================================*/
sio_status_type hsu_al_ser_open
( 
  sio_open_type *open_ptr
)
{
  boolean ret_val;
  hsu_iface_index_enum_type iface;

  if (!hsu_conf_sel_is_hsusb_initialized())
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "usb_not_init, open_ptr 0x%8X", (uint32)open_ptr);
    return SIO_BUSY_S;
  }

  if (open_ptr == NULL)
  {
    HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "open_ptr_null, open_ptr 0x%8X", (uint32)open_ptr);
    return SIO_BADP_S;
  }

  iface = hsu_conf_sel_get_iface_type_from_port_id(open_ptr->port_id);

  switch (open_ptr->port_id)
  {
    case AL_SER_PORT_MDM:
    {
      dsm_mempool_id_enum_type dsm_mempool_id;
      hsu_al_ser_acm_data* mdm_acm_ptr_temp = NULL;
      
      if ( SIO_DM_MODE == open_ptr->stream_mode )
      {
        /* For DIAG port dancing, use DIAG DSM items on the MDM port. */
        dsm_mempool_id = DSM_DIAG_ITEM_POOL;
      }
      else
      {
        dsm_mempool_id = ACM_RX_DSM_ITEM_POOL;
      }

      jsafe_enter();

      #ifdef FEATURE_HS_USB_CSVT_PORT
      mdm_acm_ptr_temp = get_and_alloc_acm_data_for_app(AL_SER_PORT_MDM);
      #else /* FEATURE_HS_USB_CSVT_PORT */
      mdm_acm_ptr_temp = get_and_alloc_acm_data_for_app();
      #endif /* FEATURE_HS_USB_CSVT_PORT */

      if (NULL == mdm_acm_ptr_temp)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "acm_slot_empty");
        jsafe_leave();
        return SIO_UNAVAIL_S;
      }

      hsu_al_ser_static_data.mdm_acm_ptr = mdm_acm_ptr_temp;

      #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_MDM
      hsu_al_ser_static_data.mdm_acm_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
      #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_MDM */

      ret_val = hsu_al_ser_acm_open(
        hsu_al_ser_static_data.mdm_acm_ptr, 
        open_ptr, 
        dsm_mempool_id,
        HS_USB_ACM_RX_BUFF_CNT,
        HS_USB_ACM_RX_TIMEOUT_MS,
        HS_USB_ACM_TX_MAX_PENDING_BUFFERS,
        HS_USB_ACM_TX_TIMEOUT_MS,
        HS_USB_ACM_TX_COMP_TIMER_VAL);

      if (ret_val == FALSE)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "MDM_open_fail");
        jsafe_leave();
        return SIO_BADP_S;
      }

#ifndef FEATURE_HS_USB_CSVT_PORT
      hsu_al_ser_static_data.mdm_held_by_app = TRUE;
#endif /* FEATURE_HS_USB_CSVT_PORT */ 

      jsafe_leave();
      break;
    }

#ifdef FEATURE_HS_USB_CSVT_PORT
    case AL_SER_PORT_MDM2:
    {
      dsm_mempool_id_enum_type dsm_mempool_id;
      hsu_al_ser_acm_data* csvt_acm_ptr_temp = NULL;
      
      if ( SIO_DM_MODE == open_ptr->stream_mode )
      {
        /* For DIAG port dancing, use DIAG DSM items on the MDM port. */
        dsm_mempool_id = DSM_DIAG_ITEM_POOL;
      }
      else
      {
        dsm_mempool_id = ACM_RX_DSM_ITEM_POOL;
      }

      jsafe_enter();

      csvt_acm_ptr_temp = get_and_alloc_acm_data_for_app(AL_SER_PORT_MDM2);

      if (NULL == csvt_acm_ptr_temp)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "csvt_slot_empty");
        jsafe_leave();
        return SIO_UNAVAIL_S;
      }

      hsu_al_ser_static_data.csvt_acm_ptr = csvt_acm_ptr_temp;

      #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_MDM
      hsu_al_ser_static_data.csvt_acm_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
      #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_MDM */

      ret_val = hsu_al_ser_acm_open(
        hsu_al_ser_static_data.csvt_acm_ptr, 
        open_ptr, 
        dsm_mempool_id,
        HS_USB_ACM_RX_BUFF_CNT,
        HS_USB_ACM_RX_TIMEOUT_MS,
        HS_USB_ACM_TX_MAX_PENDING_BUFFERS,
        HS_USB_ACM_TX_TIMEOUT_MS,
        HS_USB_ACM_TX_COMP_TIMER_VAL);

      if (ret_val == FALSE)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "CSVT_open_fail");

        /* TODO: invalidate fields in mdm_acm_data? */
        jsafe_leave();
        return SIO_BADP_S;
      }

      jsafe_leave();
      break;
    }

#endif /* FEATURE_HS_USB_CSVT_PORT */

    case AL_SER_PORT_DIAG:
    {
      hsu_al_ser_obex_data* diag_obex_ptr_temp = NULL;

      jsafe_enter();
      diag_obex_ptr_temp = 
        get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_DIAG);

      if (diag_obex_ptr_temp == NULL)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DIAG_slot_alloc_fail");
        jsafe_leave();
        return SIO_BADP_S;
      }

      hsu_al_ser_static_data.diag_obex_ptr = diag_obex_ptr_temp;

      if (open_ptr->open_param == NULL)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DIAG_open_param_null");
        jsafe_leave();
        return SIO_BADP_S;
      }

      if (open_ptr->open_param->dsm_pool_param.dsm_mempool_id == NULL)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DIAG_dsm_null");
        jsafe_leave();
        return SIO_BADP_S;
      }

      #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_DIAG
      hsu_al_ser_static_data.diag_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
      #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_DIAG */

      ret_val = hsu_al_ser_base_open(
        &(hsu_al_ser_static_data.diag_obex_ptr->base_data),
        open_ptr, 
        open_ptr->open_param->dsm_pool_param.dsm_mempool_id,
        HS_USB_DIAG_RX_BUFF_CNT,
        HS_USB_DIAG_RX_TIMEOUT_MS,
        HS_USB_DIAG_TX_MAX_PENDING_BUFFERS,
        HS_USB_DIAG_TX_TIMEOUT_MS,
        HS_USB_DIAG_TX_COMP_TIMER_VAL,
        hsu_al_ser_static_data.diag_obex_ptr);

      if (ret_val == FALSE)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DIAG_open_fail");

        release_obex_data_slot_from_app(hsu_al_ser_static_data.diag_obex_ptr);

        hsu_al_ser_static_data.diag_obex_ptr = NULL;
        jsafe_leave();
        return SIO_BADP_S;
      }

      /* If DIAG is over HSUSB (not HSIC), the packet needs to be dropped
      ** during the USB DISCONNECT case to allow XO shutdown if there was
      ** pending DIAG TX packet.
      */
      hsu_al_ser_static_data.diag_obex_ptr->base_data.tx_info.recover_dropped_packet = TRUE;
      
      jsafe_leave();
      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      hsu_al_ser_obex_data* nmea_obex_ptr_temp = NULL;

      jsafe_enter();
      nmea_obex_ptr_temp = 
        get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_NMEA);
   
      if (nmea_obex_ptr_temp == NULL)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "NMEA_slot_alloc_fail");
        jsafe_leave();
        return SIO_BADP_S;
      }

      hsu_al_ser_static_data.nmea_obex_ptr = nmea_obex_ptr_temp;

      #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_NMEA
      hsu_al_ser_static_data.nmea_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
      #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_NMEA */

      ret_val = hsu_al_ser_base_open(
        &(hsu_al_ser_static_data.nmea_obex_ptr->base_data), 
        open_ptr, 
        DSM_DS_LARGE_ITEM_POOL,
        HS_USB_NMEA_RX_BUFF_CNT,
        HS_USB_NMEA_RX_TIMEOUT_MS,
        HS_USB_NMEA_TX_MAX_PENDING_BUFFERS,
        HS_USB_NMEA_TX_TIMEOUT_MS,
        HS_USB_NMEA_TX_COMP_TIMER_VAL,
        hsu_al_ser_static_data.nmea_obex_ptr);

      if (ret_val == FALSE)
      {
        HSU_ULOG(ERROR_MSG, DBG_E_LOG, "NMEA_open_fail");

        release_obex_data_slot_from_app(hsu_al_ser_static_data.nmea_obex_ptr);

        hsu_al_ser_static_data.nmea_obex_ptr = NULL;
        jsafe_leave();
        return SIO_BADP_S;
      }
      jsafe_leave();
      break;      
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        hsu_al_ser_obex_data* ser3_obex_ptr_temp = NULL;

#ifdef T_UNDP 
        if (hsu_is_undp_ser3_port_enabled() == FALSE)
        {
          /* Since SER port is not enabled, we should not open SER3 */
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER3_not_enabled");
          return SIO_UNAVAIL_S;
        }
#endif /* T_UNDP */

		HSU_ULOG(NO_MSG, DATA_LOG, "SER3_OPEN_ENTER");

        jsafe_enter();
        ser3_obex_ptr_temp = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_SER3);

        if (ser3_obex_ptr_temp == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER3_slot_alloc_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }

        hsu_al_ser_static_data.ser3_obex_ptr = ser3_obex_ptr_temp;

        #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER3
        hsu_al_ser_static_data.ser3_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
        #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER3 */

        // Doing this for ser3 being called from QAPI module (currently this is the only
        // module that is using ser3)
        // Need to revisit this if someone else uses ser3
        hsu_al_ser_static_data.ser3_obex_ptr->base_data.tx_info.recover_dropped_packet = FALSE;

        ret_val = hsu_al_ser_base_open(
          &(hsu_al_ser_static_data.ser3_obex_ptr->base_data), 
          open_ptr,
          DSM_DS_LARGE_ITEM_POOL,
          HS_USB_SER3_RX_BUFF_CNT,
          HS_USB_SER3_RX_TIMEOUT_MS,
          HS_USB_SER3_TX_MAX_PENDING_BUFFERS,
          HS_USB_SER3_TX_TIMEOUT_MS,
          HS_USB_SER3_TX_COMP_TIMER_VAL,
          hsu_al_ser_static_data.ser3_obex_ptr);

        if (ret_val == FALSE)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER3_open_fail");

          release_obex_data_slot_from_app(hsu_al_ser_static_data.ser3_obex_ptr);

          hsu_al_ser_static_data.ser3_obex_ptr = NULL;
          jsafe_leave();
          return SIO_BADP_S;
        }
        jsafe_leave();
        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
      {
        hsu_al_ser_obex_data* ser5_obex_ptr_temp = NULL;
        jsafe_enter();
		ser5_obex_ptr_temp = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_SER5);

        if (ser5_obex_ptr_temp == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER5_slot_alloc_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }
		
		hsu_al_ser_static_data.ser5_obex_ptr = ser5_obex_ptr_temp;

        #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER5
        hsu_al_ser_static_data.ser5_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
        #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER5 */

        ret_val = hsu_al_ser_base_open(
          &(hsu_al_ser_static_data.ser5_obex_ptr->base_data), 
          open_ptr,
          DSM_DS_LARGE_ITEM_POOL,
          HS_USB_SER5_RX_BUFF_CNT,
          HS_USB_SER5_RX_TIMEOUT_MS,
          HS_USB_SER5_TX_MAX_PENDING_BUFFERS,
          HS_USB_SER5_TX_TIMEOUT_MS,
          HS_USB_SER5_TX_COMP_TIMER_VAL,
          hsu_al_ser_static_data.ser5_obex_ptr);

        if (ret_val == FALSE)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER5_open_fail");

          release_obex_data_slot_from_app(hsu_al_ser_static_data.ser5_obex_ptr);

          hsu_al_ser_static_data.ser5_obex_ptr = NULL;
          jsafe_leave();
          return SIO_BADP_S;
        }
        jsafe_leave();
        break;
      }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
      {
        hsu_al_ser_obex_data* ser6_obex_ptr_temp = NULL;
        jsafe_enter();
        ser6_obex_ptr_temp = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_SER6);

        if (ser6_obex_ptr_temp == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER6_slot_alloc_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }
		
		hsu_al_ser_static_data.ser6_obex_ptr = ser6_obex_ptr_temp;

        #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER6
        hsu_al_ser_static_data.ser6_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
        #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER6 */

        ret_val = hsu_al_ser_base_open(
          &(hsu_al_ser_static_data.ser6_obex_ptr->base_data), 
          open_ptr,
          DSM_DS_LARGE_ITEM_POOL,
          HS_USB_SER6_RX_BUFF_CNT,
          HS_USB_SER6_RX_TIMEOUT_MS,
          HS_USB_SER6_TX_MAX_PENDING_BUFFERS,
          HS_USB_SER6_TX_TIMEOUT_MS,
          HS_USB_SER6_TX_COMP_TIMER_VAL,
          hsu_al_ser_static_data.ser6_obex_ptr);

        if (ret_val == FALSE)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER6_open_fail");

          release_obex_data_slot_from_app(hsu_al_ser_static_data.ser6_obex_ptr);

          hsu_al_ser_static_data.ser6_obex_ptr = NULL;
          jsafe_leave();
          return SIO_BADP_S;
        }
        jsafe_leave();
        break;
      }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
      {
        hsu_al_ser_obex_data* dci_obex_ptr_temp = NULL;
        jsafe_enter();
        dci_obex_ptr_temp = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_DCI);

        if (dci_obex_ptr_temp == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCI_slot_alloc_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }

        hsu_al_ser_static_data.dci_obex_ptr = dci_obex_ptr_temp;
        
        #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_DCI
        hsu_al_ser_static_data.dci_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
        #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_DCI */

        ret_val = hsu_al_ser_base_open(
          &(hsu_al_ser_static_data.dci_obex_ptr->base_data), 
          open_ptr,
          open_ptr->open_param->dsm_pool_param.dsm_mempool_id,
          HS_USB_DCI_RX_BUFF_CNT,
          HS_USB_DCI_RX_TIMEOUT_MS,
          HS_USB_DCI_TX_MAX_PENDING_BUFFERS,
          HS_USB_DCI_TX_TIMEOUT_MS,
          HS_USB_DCI_TX_COMP_TIMER_VAL,
          hsu_al_ser_static_data.dci_obex_ptr);

        if (ret_val == FALSE)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "DCI_open_fail");

          release_obex_data_slot_from_app(hsu_al_ser_static_data.dci_obex_ptr);

          hsu_al_ser_static_data.dci_obex_ptr = NULL;
          jsafe_leave();
          return SIO_BADP_S;
        }
        jsafe_leave();
        break;
      }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
    {
        hsu_al_ser_obex_data* ser4_obex_ptr_temp = NULL;

        jsafe_enter();
        ser4_obex_ptr_temp = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_SER4);

        if (ser4_obex_ptr_temp == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER4_alloc_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }

        hsu_al_ser_static_data.ser4_obex_ptr = ser4_obex_ptr_temp;
    
        if (open_ptr->open_param == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER4_open_param_null");
          jsafe_leave();
          return SIO_BADP_S;
        }

        if (open_ptr->open_param->dsm_pool_param.dsm_mempool_id == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER4_dsm_null");
          jsafe_leave();
          return SIO_BADP_S;
        }

        #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER4
        hsu_al_ser_static_data.ser4_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
        #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_SER4 */

        hsu_al_ser_static_data.ser4_obex_ptr->base_data.uses_dsm_app_ptr = TRUE;

        ret_val = hsu_al_ser_base_open(
          &(hsu_al_ser_static_data.ser4_obex_ptr->base_data), 
          open_ptr,
          open_ptr->open_param->dsm_pool_param.dsm_mempool_id,
          HS_USB_SER4_RX_BUFF_CNT,
          HS_USB_SER4_RX_TIMEOUT_MS,
          HS_USB_SER4_TX_MAX_PENDING_BUFFERS,
          HS_USB_SER4_TX_TIMEOUT_MS,
          HS_USB_SER4_TX_COMP_TIMER_VAL,
          hsu_al_ser_static_data.ser4_obex_ptr);

        if (ret_val == FALSE)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "SER4_open_fail");

          /*
           * SER4 client may always try to open the port even when composition does not 
           * support SER4.  The fd is not initialized, so we cannot correctly map the core.
           * In this case, if we try to release it for HSIC core.
           */
          /* release_obex_data_slot_from_app(hsu_al_ser_static_data.ser4_obex_ptr); */

          hsu_al_ser_static_data.ser4_obex_ptr = NULL;
          jsafe_leave();
          return SIO_BADP_S;
        }
        jsafe_leave();
        break;
      }
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
      {
        hsu_al_ser_obex_data* qdss_obex_ptr_temp = NULL;

        jsafe_enter();
        qdss_obex_ptr_temp = 
          get_and_alloc_obex_data_slot_for_app(AL_SER_PORT_USB_QDSS);

        if (qdss_obex_ptr_temp == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "QDSS_slot_alloc_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }

        hsu_al_ser_static_data.qdss_obex_ptr = qdss_obex_ptr_temp;

        if (open_ptr->open_param == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "QDSS_open_param_null");
          jsafe_leave();
          return SIO_BADP_S;
        }

        if (open_ptr->open_param->dsm_pool_param.dsm_mempool_id == NULL)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "QDSS_dsm_fail");
          jsafe_leave();
          return SIO_BADP_S;
        }

        #ifdef FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_QDSS
        hsu_al_ser_static_data.qdss_obex_ptr->base_data.tx_info.drop_packet_on_suspend = TRUE;
        #endif /* FEATURE_HS_USB_DROP_PKT_ON_SUSPEND_QDSS */
     
        hsu_al_ser_static_data.qdss_obex_ptr->base_data.uses_dsm_app_ptr = TRUE;

        ret_val = hsu_al_ser_obex_open(
          hsu_al_ser_static_data.qdss_obex_ptr, 
          open_ptr,
          open_ptr->open_param->dsm_pool_param.dsm_mempool_id,
          HS_USB_QDSS_RX_BUFF_CNT,
          HS_USB_QDSS_RX_TIMEOUT_MS,
          HS_USB_QDSS_TX_MAX_PENDING_BUFFERS,
          HS_USB_QDSS_TX_TIMEOUT_MS,
          HS_USB_QDSS_TX_COMP_TIMER_VAL);

        if (ret_val == FALSE)
        {
          HSU_ULOG(ERROR_MSG, DBG_E_LOG, "QDSS_open_fail");

          release_obex_data_slot_from_app(hsu_al_ser_static_data.qdss_obex_ptr); 

          hsu_al_ser_static_data.qdss_obex_ptr = NULL;
          jsafe_leave();
          return SIO_BADP_S;
        }

        hsu_al_ser_static_data.qdss_obex_ptr->base_data.tx_info.recover_dropped_packet = FALSE;

        jsafe_leave();
        break;
      }
#endif /* FEATURE_HS_USB_QDSS_PORT */

    default:
    {
      HSU_ULOG_1(ERROR_MSG, DBG_E_LOG, "unknown, port_id 0x%8X", (uint32)open_ptr->port_id);
      return SIO_BADP_S;
    }
  }

  HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "hsu_al_ser_open, success", iface, HSU_IFACE_OFFSET2);

  return SIO_DONE_S;
}

/*===========================================================================
FUNCTION hsu_al_ser_close()
===========================================================================*/
void hsu_al_ser_close
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  sio_vv_func_ptr_type close_done_cb
)
{
  uint32 remaining_msecs_to_wait_for_close = AL_SER_CLOSE_MAX_TIMEOUT_IN_MSECS;
  hsu_al_ser_base_data_for_ser_if* common_data_ptr = NULL;

  HSU_USE_PARAM(stream_id);

  jsafe_enter();

  HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "hsu_al_ser_close", hsu_conf_sel_get_iface_type_from_port_id(port_id), HSU_IFACE_OFFSET2);

  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      common_data_ptr = 
        &hsu_al_ser_static_data.mdm_acm_ptr->base_data;
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
#ifdef FEATURE_HS_USB_CSVT_PORT
        add_app_close_cb_for_acm(hsu_al_ser_static_data.mdm_acm_ptr, close_done_cb);
#else /* FEATURE_HS_USB_CSVT_PORT */
        hsu_al_ser_static_data.mdm_close_cb = close_done_cb;
#endif /* FEATURE_HS_USB_CSVT_PORT */
      }
      else 
      {
        /* abort immediately since there's no close_cb => no need to flush */
        hsu_al_ser_acm_close(hsu_al_ser_static_data.mdm_acm_ptr, 
          al_ser_on_close, TRUE);
      }
      break;
    }
#ifdef FEATURE_HS_USB_CSVT_PORT
    case AL_SER_PORT_MDM2:
    {
      common_data_ptr = 
        &hsu_al_ser_static_data.csvt_acm_ptr->base_data;
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
        add_app_close_cb_for_acm(hsu_al_ser_static_data.csvt_acm_ptr, close_done_cb);
      }
      else 
      {
        /* abort immediately since there's no close_cb => no need to flush */
        hsu_al_ser_acm_close(hsu_al_ser_static_data.csvt_acm_ptr, al_ser_on_close, TRUE);
      }
      break;
    }
#endif /* FEATURE_HS_USB_CSVT_PORT */
    case AL_SER_PORT_DIAG:
    {
      common_data_ptr = &hsu_al_ser_static_data.diag_obex_ptr->base_data;
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
        add_app_close_cb_for_obex(hsu_al_ser_static_data.diag_obex_ptr, close_done_cb);
      }
      else
      {
        hsu_al_ser_obex_close(hsu_al_ser_static_data.diag_obex_ptr, al_ser_on_close, TRUE);
      }

      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      common_data_ptr = &hsu_al_ser_static_data.nmea_obex_ptr->base_data;
      if (close_done_cb != NULL)
      {
        /* Save original App callback: */
        add_app_close_cb_for_obex(hsu_al_ser_static_data.nmea_obex_ptr, close_done_cb);
      }
      else
      {
        hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
      }
      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        common_data_ptr = &hsu_al_ser_static_data.ser3_obex_ptr->base_data;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.ser3_obex_ptr, close_done_cb);
        }
        else
        {
          hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
        }
        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
      {
        common_data_ptr = &hsu_al_ser_static_data.ser5_obex_ptr->base_data;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.ser5_obex_ptr, close_done_cb);
        }
        else
        {
          hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
        }
        break;
      }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
      {
        common_data_ptr = &hsu_al_ser_static_data.ser6_obex_ptr->base_data;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.ser6_obex_ptr, close_done_cb);
        }
        else
        {
          hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
        }
        break;
      }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
      {
        common_data_ptr = &hsu_al_ser_static_data.dci_obex_ptr->base_data;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.dci_obex_ptr, close_done_cb);
        }
        else
        {
          hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
        }
        break;
      }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
      {
        common_data_ptr = &hsu_al_ser_static_data.ser4_obex_ptr->base_data;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.ser4_obex_ptr, close_done_cb);
        }
        else
        {
          hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
        }
        break;
      }
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
      {
        common_data_ptr = &hsu_al_ser_static_data.qdss_obex_ptr->base_data;
        if (close_done_cb != NULL)
        {
          /* Save original App callback: */
          add_app_close_cb_for_obex(hsu_al_ser_static_data.qdss_obex_ptr, close_done_cb);
        }
        else
        {
          hsu_al_ser_base_close(common_data_ptr, al_ser_on_close, TRUE);
        }
        break;
      }
#endif /* FEATURE_HS_USB_QDSS_PORT */
    default:
    {
      HSU_ERR_FATAL("hsu_al_ser_close: Port: %d not supported", 
        port_id, 0, 0);
    }
  }
  if (close_done_cb != NULL)
  {
    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "hsu_al_ser_close, flush_start", common_data_ptr->iface_type, HSU_IFACE_OFFSET2);
    
    /* Flush the tx queue. Close will be called from the flush callback */
    al_ser_internal_flush_tx(port_id, al_ser_complete_close_after_flush_tx);
  }
  
  /* If close_done_cb == NULL, there is a need for a rex_sleep loop, 
  to simulate synchronous call 
  (without modifying the code of the upper apps) */

  if (close_done_cb == NULL)
  {
    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "hsu_al_ser_close, no_cb_wait_start", common_data_ptr->iface_type, HSU_IFACE_OFFSET2);

    while (remaining_msecs_to_wait_for_close > 0)
    {
      if (data_ptr_is_freed_from_app(common_data_ptr) == TRUE)
      {
        break;
      }
      /* else: sleep for a certain interval and retry: */

      /*Release the Giant mutex before sleep to allow other tasks to run*/
      jsafe_leave();
      hsu_qurt_sleep_ms(AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS);
      jsafe_enter();

      remaining_msecs_to_wait_for_close -= 
        AL_SER_CLOSE_SLEEP_INTERVAL_TIMEOUT_IN_MSECS;
    }

    /* Since there is no return value, this is the only way to indicate that 
    despite waiting sufficient time, this layer has yet to finish the close
    operation. */
    HSU_ASSERT(data_ptr_is_freed_from_app(common_data_ptr) == TRUE);

    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "hsu_al_ser_close, no_cb_wait_finish", common_data_ptr->iface_type, HSU_IFACE_OFFSET2);
  }
  jsafe_leave();
}

/*===========================================================================
FUNCTION hsu_al_ser_transmit()
===========================================================================*/
void hsu_al_ser_transmit
( 
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  dsm_item_type *tx_ptr
)
{
  hsu_al_ser_base_data_for_ser_if* common_data_ptr = NULL;

  HSU_USE_PARAM(stream_id);

  if (tx_ptr == NULL)
  {
    HSU_ERR_FATAL("hsu_al_ser_transmit: tx_ptr is NULL", 0, 0, 0);
  }

  hsu_chk_ist_crit_failure();

  jsafe_enter();
  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      HSU_ASSERT(hsu_al_ser_static_data.mdm_acm_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.mdm_acm_ptr->base_data;

      hsu_al_ser_acm_transmit(hsu_al_ser_static_data.mdm_acm_ptr, tx_ptr);
      jsafe_leave();
      return;
    }
#ifdef FEATURE_HS_USB_CSVT_PORT
    case AL_SER_PORT_MDM2:
    {
      HSU_ASSERT(hsu_al_ser_static_data.csvt_acm_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.csvt_acm_ptr->base_data;

      hsu_al_ser_acm_transmit(hsu_al_ser_static_data.csvt_acm_ptr, tx_ptr);
      jsafe_leave();
      return;
    }
#endif /* FEATURE_HS_USB_CSVT_PORT */
    case AL_SER_PORT_DIAG:
    {
      HSU_ASSERT(hsu_al_ser_static_data.diag_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.diag_obex_ptr->base_data;

      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      HSU_ASSERT(hsu_al_ser_static_data.nmea_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.nmea_obex_ptr->base_data;

      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
    {
      HSU_ASSERT(hsu_al_ser_static_data.ser3_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.ser3_obex_ptr->base_data;

      break;
    }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
    {
      HSU_ASSERT(hsu_al_ser_static_data.ser5_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.ser5_obex_ptr->base_data;

      break;
    }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
    {
      HSU_ASSERT(hsu_al_ser_static_data.ser6_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.ser6_obex_ptr->base_data;

      break;
    }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
    {
      HSU_ASSERT(hsu_al_ser_static_data.dci_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.dci_obex_ptr->base_data;

      break;
    }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
    {
      HSU_ASSERT(hsu_al_ser_static_data.ser4_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.ser4_obex_ptr->base_data;
      HSU_ULOG(NO_MSG, SER4_LOG, "ser4_transfer_initiated");

      break;
    }
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
     case AL_SER_PORT_USB_QDSS:
    {
      HSU_ASSERT(hsu_al_ser_static_data.qdss_obex_ptr != NULL);
      common_data_ptr = &hsu_al_ser_static_data.qdss_obex_ptr->base_data;
      hsu_al_ser_obex_transmit(hsu_al_ser_static_data.qdss_obex_ptr, tx_ptr);
      jsafe_leave();
      return;
    }
#endif /* FEATURE_HS_USB_QDSS_PORT */
    default:
    {
      HSU_ERR_FATAL("hsu_al_ser_transmit: Port: %d not supported", 
        port_id, 0, 0);
    }
  }

  HSU_ASSERT(common_data_ptr != NULL);

  hsu_al_ser_base_transmit(common_data_ptr, tx_ptr);
  jsafe_leave();
}

/*===========================================================================
FUNCTION hsu_al_ser_flush_tx()
===========================================================================*/
void hsu_al_ser_flush_tx
( 
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  sio_vv_func_ptr_type flush_tx_done_cb
)
{
  HSU_USE_PARAM(stream_id);

  jsafe_enter();
  /* Save original App callback- for every port */
  switch (port_id)
  {
  case AL_SER_PORT_MDM:
    {
      #ifdef FEATURE_HS_USB_CSVT_PORT
      add_app_flush_tx_cb_for_acm(hsu_al_ser_static_data.mdm_acm_ptr, flush_tx_done_cb);
      #else /* FEATURE_HS_USB_CSVT_PORT */
      hsu_al_ser_static_data.mdm_flush_tx_cb = flush_tx_done_cb;
      #endif /* FEATURE_HS_USB_CSVT_PORT */
      break;
    }
#ifdef FEATURE_HS_USB_CSVT_PORT
  case AL_SER_PORT_MDM2:
    {
      add_app_flush_tx_cb_for_acm(hsu_al_ser_static_data.csvt_acm_ptr, flush_tx_done_cb);	  
      break;
    }
#endif /* FEATURE_HS_USB_CSVT_PORT */

  case AL_SER_PORT_DIAG:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.diag_obex_ptr, flush_tx_done_cb);
      break;
    }
#ifdef FEATURE_HS_USB_NMEA
  case AL_SER_PORT_NMEA:
    {      
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.nmea_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
  case AL_SER_PORT_USB_SER3:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.ser3_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
  case AL_SER_PORT_USB_SER5:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.ser5_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
  case AL_SER_PORT_USB_SER6:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.ser6_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
  case AL_SER_PORT_USB_DCI:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.dci_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
  case AL_SER_PORT_USB_SER4:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.ser4_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
    {
      add_app_flush_tx_cb_for_obex(hsu_al_ser_static_data.qdss_obex_ptr, flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_QDSS_PORT */


  default:
    {
      HSU_ERR_FATAL("hsu_al_ser_flush_tx: Port: %d not supported", 
        port_id, 0, 0);
    }
  }/*end of switch*/

  al_ser_internal_flush_tx(port_id, al_ser_on_flush_tx);

  jsafe_leave();
}
/*===========================================================================
FUNCTION hsu_al_ser_ioctl()
===========================================================================*/
void hsu_al_ser_ioctl
(
  sio_stream_id_type stream_id,
  sio_port_id_type port_id,
  sio_ioctl_cmd_type ioctl_cmd,
  sio_ioctl_param_type* ioctl_param_ptr
)
{
  hsu_al_ser_obex_data* obex_data_ptr = NULL;

  HSU_USE_PARAM(stream_id);

  hsu_chk_ist_crit_failure();

  jsafe_enter();
  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      if (hsu_al_ser_acm_ioctl(hsu_al_ser_static_data.mdm_acm_ptr,
        ioctl_cmd, ioctl_param_ptr) == FALSE)
      {
        HSU_ERR_FATAL("hsu_al_ser_ioctl: hsu_al_ser_acm_ioctl returned FALSE",
          0, 0, 0);
      }
      jsafe_leave();
      return;
    }
#ifdef FEATURE_HS_USB_CSVT_PORT
    case AL_SER_PORT_MDM2:
    {
      if (hsu_al_ser_acm_ioctl(hsu_al_ser_static_data.csvt_acm_ptr,
        ioctl_cmd, ioctl_param_ptr) == FALSE)
      {
        HSU_ERR_FATAL("hsu_al_ser_ioctl: hsu_al_ser_acm_ioctl returned FALSE",
          0, 0, 0);
      }
      jsafe_leave();
      return;
    }
#endif /* FEATURE_HS_USB_CSVT_PORT */
    case AL_SER_PORT_DIAG:
    {
      obex_data_ptr = hsu_al_ser_static_data.diag_obex_ptr;
      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      obex_data_ptr = hsu_al_ser_static_data.nmea_obex_ptr;
      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
      {
        obex_data_ptr = hsu_al_ser_static_data.ser3_obex_ptr;
        break;
      }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
      {
        obex_data_ptr = hsu_al_ser_static_data.ser5_obex_ptr;
        break;
      }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
      {
        obex_data_ptr = hsu_al_ser_static_data.ser6_obex_ptr;
        break;
      }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
      {
        obex_data_ptr = hsu_al_ser_static_data.dci_obex_ptr;
        break;
      }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
      {
        obex_data_ptr = hsu_al_ser_static_data.ser4_obex_ptr;
        break;
      }
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
    {
        obex_data_ptr = hsu_al_ser_static_data.qdss_obex_ptr;
        break;
    }
#endif /* FEATURE_HS_USB_QDSS_PORT */

    default:
    {
      HSU_ERR_FATAL("hsu_al_ser_ioctl: Port: %d not supported", 
        port_id, 0, 0);
    }
  }

  HSU_ASSERT(obex_data_ptr != NULL);

  /* TODO: How to verify if an I/O Ctl indeed came from an SMD? 
  Because, otherwise commands such as ASSERT_CD are an error.*/
  if (hsu_al_ser_obex_ioctl(obex_data_ptr, ioctl_cmd, ioctl_param_ptr) == FALSE)
  {
    HSU_ULOG_ENUM_1(ERROR_MSG, DBG_E_LOG, 
      "hsu_al_ser_ioctl, ioctl_not_supported, cmd 0x%8X", 
      obex_data_ptr->base_data.iface_type, HSU_IFACE_OFFSET2, ioctl_cmd);
  }
  jsafe_leave();
}

/*==============================================================================

EXTERNAL FUNCTION DEFINITIONS FOR CDC FDs (acm_fd_api.h & obex_fd_api.h)

==============================================================================*/
/*==============================================================================

FUNCTION      GET_AND_ALLOC_ACM_INSTANCE_INFO_FOR_FD

DESCRIPTION   
Called from acm_init, to find an instance_info_ptr to be used for the specific 
FD instance. 
Currently, since only one instance of the modem driver is supported, the 
function simply returns the address of hsu_al_ser_static_data.mdm_acm_data.
In the future, this implementation will have to change. 

DEPENDENCIES  
Should be called when the Giant mutex is locked. 

RETURN VALUE  
A pointer to an ecm_instance_info.

SIDE EFFECTS  
None.

==============================================================================*/
static hsu_al_ser_acm_data* get_and_alloc_acm_instance_info_for_fd
(
 cdc_handle_t      handle 
 /* handle to this instance of the FD, to be used in further API calls */
)
{
  hsu_al_ser_acm_data* returned_value = NULL;
  core_t *core = ((cdc_ctx_t*)handle)->core_ctx;
  uint8 core_index = core->core_number;
  #ifdef FEATURE_HS_USB_CSVT_PORT
    uint8 cur_config_index = core->curr_config;
  #endif /* FEATURE_HS_USB_CSVT_PORT */
  uint8 cur_acm_user;

#ifdef FEATURE_HS_USB_CSVT_PORT
  cur_acm_user = hsu_al_ser_static_data.num_of_initialized_acm_instances[core_index][cur_config_index-1];
#else /* FEATURE_HS_USB_CSVT_PORT */
  cur_acm_user = hsu_al_ser_static_data.num_of_initialized_acm_instances[core_index];
#endif /* FEATURE_HS_USB_CSVT_PORT */
  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("get_and_alloc_acm_instance_info_for_fd: Should be called "
          " with Giant mutex locked!", 0, 0, 0);
     return NULL;
  }

#ifdef FEATURE_HS_USB_CSVT_PORT
  returned_value = &(hsu_al_ser_static_data.mdm_acm_data[core_index][cur_acm_user]);
  
  /* We save the handle of all the FDs that the stack initialized. 
  When the configuration will be changed we will use the corresponding handle 
  for the stack's FD. */

  hsu_al_ser_static_data.acm_fd_handle[core_index][cur_config_index - 1][cur_acm_user]= handle;
  
  returned_value->base_data.instance_of_fd_used = handle;
  returned_value->modem_if_data.core_idx = core_index;
  
  ++hsu_al_ser_static_data.num_of_initialized_acm_instances[core_index][cur_config_index-1];
  
  return returned_value;
#else
  hsu_al_ser_static_data.mdm_acm_data[core_index].base_data.instance_of_fd_used = handle;

  /* We save the handle of all the FDs that the stack initialized. 
  When the configuration will be changed we will use the corresponding handle 
  for the stack's FD. */
  hsu_al_ser_static_data.acm_fd_handle[core_index][cur_acm_user] = handle;
  hsu_al_ser_static_data.mdm_acm_data[core_index].modem_if_data.core_idx = core_index;

  ++hsu_al_ser_static_data.num_of_initialized_acm_instances[core_index];
  returned_value = &(hsu_al_ser_static_data.mdm_acm_data[core_index]);

  return returned_value;
#endif /* FEATURE_HS_USB_CSVT_PORT */
}


/*===========================================================================
FUNCTION obex_init()
===========================================================================*/
jresult_t obex_init
(
  cdc_handle_t handle, 
  obex_callbacks_t* cbs, 
  obex_init_info_t* info, 
  cdc_appctx_t* ctx
)
{
  hsu_al_ser_obex_data* obex_data_instance_ptr = NULL;
  uint8 config_index;
  uint8 instance_number;
  core_t *core = NULL; 
  uint8 core_index = 0;

  HSU_MSG_HIGH("obex_init: entered");

  /* Make sure data structs are valid, prior to working on them: */
  init_obex_data_arrays_once();

  if (handle == NULL || cbs == NULL || info == NULL || ctx == NULL)
  {
    HSU_MSG_ERROR_3("obex_init: At least one of the args is invalid:\
                  handle=0x%x, cbs=0x%x, info=0x%x", 
                  handle, cbs, info);
    HSU_MSG_ERROR_1("obex_init: At least one of the args is invalid (cont): ctx=0x%x",
      ctx);
    return JEINVAL;
  }

  core = ((cdc_ctx_t*)handle)->core_ctx;
  HSU_ASSERT(core);
  core_index = core->core_number;

  /* Allocate a data slot for this OBEX FD instance */
  obex_data_instance_ptr = get_avail_obex_data_slot_for_fd(handle);

  /* Must not happen: There should be enough slots to begin with... */
  HSU_ASSERT(obex_data_instance_ptr != NULL);

  /* Let hsu_al_ser_obex fill the data, as well as marking obex_data_instance_ptr 
  as belonging to handle. */
  config_index = core_get_curr_configuration(core_index)-1;/* config_index starts from 0 and curr_config starts from 1 */
  HSU_ASSERT(config_index < HSU_MAX_NUM_OF_CONFIGS);

  instance_number = hsu_al_ser_static_data.num_of_initialized_obex_instances[core_index][config_index];
  hsu_al_ser_obex_populate_obex_specific_data(obex_data_instance_ptr, handle, 
    cbs, info, ctx, instance_number);

  HSU_MSG_HIGH("obex_init: exited");

  return HSU_JSUCCESS;
}

/*===========================================================================
FUNCTION acm_init()
===========================================================================*/
jresult_t acm_init
(
  cdc_handle_t handle, 
  acm_callbacks_t* cbs, 
  acm_init_info_t* info, 
  cdc_appctx_t* ctx
)
{
  hsu_al_ser_acm_data* acm_data_instance_ptr = NULL;
#ifdef FEATURE_HS_USB_CSVT_PORT
    uint8 config_index;
    uint8 instance_number;
    core_t *core = NULL;
    uint8 core_index = 0;
#endif /* FEATURE_HS_USB_CSVT_PORT */
  

  /* Make sure data struct is valid, prior to working on it: */
#ifdef FEATURE_HS_USB_CSVT_PORT
  init_acm_data_array_once();
#else /* FEATURE_HS_USB_CSVT_PORT */  
  init_acm_data_once();
#endif /* FEATURE_HS_USB_CSVT_PORT */

  if (handle == NULL || cbs == NULL || info == NULL || ctx == NULL)
  {
    HSU_MSG_ERROR_3("acm_init: At least one of the args is invalid:\
                  handle=0x%x, cbs=0x%x, info=0x%x", 
                  handle, cbs, info);
    HSU_MSG_ERROR_1("acm_init: At least one of the args is invalid (cont): ctx=0x%x",
      ctx);
    return JEINVAL;
  }

  core = ((cdc_ctx_t*)handle)->core_ctx;
  HSU_ASSERT(core); 
  core_index = core->core_number;

#ifdef T_UNDP
    /* 
    ** In UNDP (Gobi 2 or later version), 
    ** - the only acm interface (MDM) should be set as 2 in static interfaces
    */
    if (hsu_is_undp_using_static_interface () == TRUE) 
    {
      info->basic.interface_number = 2;
    }
#endif

  acm_data_instance_ptr = get_and_alloc_acm_instance_info_for_fd(handle);

#ifdef FEATURE_HS_USB_CSVT_PORT
  /* Must not happen: There should be enough slots to begin with... */
  HSU_ASSERT(acm_data_instance_ptr != NULL);

  config_index = core_get_curr_configuration(core_index)-1;/* config_index starts from 0 and curr_config starts from 1 */
  HSU_ASSERT(config_index < HSU_MAX_NUM_OF_CONFIGS);
  
  instance_number = hsu_al_ser_static_data.num_of_initialized_acm_instances[core_index][config_index];
  /* Let hsu_al_ser_acm fill the data, as well as marking acm_data_instance_ptr 
  as belonging to handle. */
  hsu_al_ser_acm_populate_acm_specific_data(acm_data_instance_ptr, handle, cbs, info, ctx, instance_number);

  return HSU_JSUCCESS;
#else
  
  /* Let hsu_al_ser_acm fill the data, as well as marking acm_data_instance_ptr 
  as belonging to handle. */
  hsu_al_ser_acm_populate_acm_specific_data(acm_data_instance_ptr, handle, 
    cbs, info, ctx);

  return HSU_JSUCCESS;
#endif /* FEATURE_HS_USB_CSVT_PORT */
}


/*=============================================================================
PUBLIC FUNCTION DEFINITIONS FOR MODULE UNIT TESTS (ut)
=============================================================================*/


/*==============================================================================

INTERNAL FUNCTION DEFINITIONS FOR MODULE

==============================================================================*/
/*===========================================================================

FUNCTION get_avail_obex_data_slot_for_fd

DESCRIPTION
Called whenever obex_init is called. Allows association between OBEX FD and
a data slot. 
In order to mark the returned value us unavailable, the caller is responsible 
for setting the field: instance_of_fd_used. Once the caller has done that,
it "owns" this slot (so if another obex_init is called, the other caller 
will not use that slot).

DEPENDENCIES
Should be called when the Giant mutex is locked. 

RETURN VALUE
hsu_al_ser_obex_data* - Pointer to the next available struct.
NULL - if no more available structs.

SIDE EFFECTS
None

===========================================================================*/
static hsu_al_ser_obex_data* get_avail_obex_data_slot_for_fd
(
 cdc_handle_t handle
)
{
  hsu_al_ser_obex_data* returned_value = NULL;
  core_t *core = ((cdc_ctx_t*)handle)->core_ctx;
  uint8 cur_core_index = core->core_number;
  uint8 cur_config_index = core->curr_config;
 
  uint8 cur_obex_user;

  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("get_avail_obex_data_slot_for_fd: Should be called "
       " with Giant mutex locked!", 0, 0, 0);
     return NULL;
  }

  cur_obex_user = hsu_al_ser_static_data.
    num_of_initialized_obex_instances[cur_core_index][cur_config_index-1];

  /* The same data_for_apps_using_obex_if is used for all the configurations, 
     so we assume that the same obex interface number is used in all the
     configurations. For example - DIAG is always first etc. 
     An example for a case that won't work will be:
     Config 1 - Diag, NMEA, Ser3, Config 2 - only NMEA. In this case we will use
     data_for_apps_using_obex_if of DIAG of the first config for NMEA of the second 
     config and we assume that the same application uses this instance (we have a port 
     open with DIAG and we didn't open a new port for NMEA when setting to config 2). */
  returned_value = 
    &(hsu_al_ser_static_data.data_for_apps_using_obex_if[cur_core_index][cur_obex_user]);

  /* We save the handle for all the stack's initialized FDs. 
     When the configuration will be changed we will use the corresponding handle 
     for the stack's FD */
  hsu_al_ser_static_data.obex_fd_handle[cur_core_index][cur_config_index-1][cur_obex_user] = handle;

  /* We set only the handle of the first configuration. If a different configuration  
    will be set we will switch to the corresponding handle for the stack's FD */
  returned_value->base_data.instance_of_fd_used = handle;
  returned_value->obex_if_data.core_idx = cur_core_index;
  ++hsu_al_ser_static_data.
    num_of_initialized_obex_instances[cur_core_index][cur_config_index-1];

  return returned_value;
}

/*===========================================================================

FUNCTION get_and_alloc_obex_data_slot_for_app

DESCRIPTION

Called whenever hsu_al_ser_open is called. 
Allows association between application (DIAG or NMEA) and a data slot. 
The returned value will be used by the adaptation layer to represent the app 
which is using the OBEX FD and called hsu_al_ser_open.

DEPENDENCIES
Should be called when the Giant mutex is locked. 

RETURN VALUE
hsu_al_ser_obex_data* - Pointer to the next available struct.
NULL - if no more available structs or no initialized obex instance.

SIDE EFFECTS
obex_slot_avail_for_app[MATCHING_INDEX][MATCHING_INDEX] is set to FALSE

===========================================================================*/
static hsu_al_ser_obex_data* get_and_alloc_obex_data_slot_for_app
(
 sio_port_id_type port_id /* SIO port id representing the app using the
                          OBEX FD */
)
{
  uint32 obex_data_slot_index;
  uint32 core_index;
  hsu_al_ser_obex_data* returned_value = NULL;
  uint8 config_index;
  uint32 total_num_of_initialized_obex_instances = 0;

  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("get_and_alloc_obex_data_slot_for_app: Should be called "
        " with Giant mutex locked!", 0, 0, 0);
     return NULL;
  }

  obex_data_slot_index = al_ser_get_obex_slot_idx_for_sio_port(port_id, &core_index);

  if(obex_data_slot_index == HSU_IFACE_ABSENT_IN_COMP)
  {
    HSU_ULOG_1(NO_MSG, DATA_LOG, "port_id_not_present_in_comp, port_id %u", port_id);
    return NULL;
  }

  /* Verify that there is at least one obex instance that was already initialized */
  for (config_index=0; config_index < HSU_MAX_NUM_OF_CONFIGS; config_index++)
  {
    total_num_of_initialized_obex_instances += 
      hsu_al_ser_static_data.num_of_initialized_obex_instances[core_index][config_index];
  }
  if (total_num_of_initialized_obex_instances == 0)
  {
    HSU_MSG_ERROR_1("get_and_alloc_obex_data_slot_for_app: Core index %d"
      "has no initialized obex instances", core_index);
    return NULL;
  }

  if (hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_data_slot_index] == FALSE)
  {
    HSU_MSG_ERROR_1("get_and_alloc_obex_data_slot_for_app: already allocated for"
      "port_id: %d", port_id);
    return NULL;
  }

  hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_data_slot_index] = FALSE;
  returned_value = 
    &(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]);

  return returned_value;
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION release_acm_data_slot_from_app

DESCRIPTION
TBD

DEPENDENCIES
Should be called when the Giant mutex is locked. 

RETURN VALUE
None

SIDE EFFECTS
mdm_held_by_app[MATCHING_INDEX][MATCHING_INDEX] is set to False

===========================================================================*/
static void release_acm_data_slot_from_app
(
  hsu_al_ser_acm_data* data_for_acm_if_ptr 
    /* Pointer to acm data structure to release */
)
{
  boolean found_match_for_if_ptr = FALSE;
  int acm_data_slot_index;
  uint8 core_index;

  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("release_obex_data_slot_from_app: Should be called "
       " with Giant mutex locked!", 0, 0, 0);
     return;
  }

  HSU_ASSERT(data_for_acm_if_ptr != NULL);

  /* No need for critical section, even if the following scenario can occur  */
  
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    core_index = data_for_acm_if_ptr->modem_if_data.core_idx;

    if (data_for_acm_if_ptr ==
      &(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]))
    {
      hsu_al_ser_static_data.mdm_held_by_app[core_index][acm_data_slot_index] = FALSE;
      
      found_match_for_if_ptr = TRUE;
      break;
    }
  }

  if (found_match_for_if_ptr == FALSE)
  {
    HSU_ERR_FATAL("release_acm_data_slot_from_app: No match for 0x%x", 
      data_for_acm_if_ptr, 0, 0);
  }
}
#endif /* FEATURE_HS_USB_CSVT_PORT */

/*===========================================================================

FUNCTION release_obex_data_slot_from_app

DESCRIPTION
Typically called whenever hsu_al_ser_close is called, or serious error encountered.
This will set the given hsu_al_ser_obex_data entry as available 
(obex_slot_avail_for_app[MATCHING_INDEX][MATCHING_INDEX] = TRUE).

DEPENDENCIES
Should be called when the Giant mutex is locked. 

RETURN VALUE
None

SIDE EFFECTS
obex_slot_avail_for_app[MATCHING_INDEX][MATCHING_INDEX] is set to TRUE

===========================================================================*/
static void release_obex_data_slot_from_app
(
  hsu_al_ser_obex_data* data_for_obex_if_ptr 
    /* Pointer to OBEX data structure to release */
)
{
  boolean found_match_for_if_ptr = FALSE;
  int obex_data_slot_index;
  uint8 core_index;

  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("release_obex_data_slot_from_app: Should be called "
       " with Giant mutex locked!", 0, 0, 0);
     return;
  }

  HSU_ASSERT(data_for_obex_if_ptr != NULL);

  /* No need for critical section, even if the following scenario can occur:
  DIAG calls hsu_al_ser_close and in the middle, NMEA calls hsu_al_ser_open.
  Then this is only a boolean 
  (doesn't matter which of them checked it first or set it to TRUE).
  */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    core_index = data_for_obex_if_ptr->obex_if_data.core_idx;

    if (data_for_obex_if_ptr ==
      &(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]))
    {
      hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_data_slot_index] = TRUE;
      
      found_match_for_if_ptr = TRUE;
      break;
    }
  }

  if (found_match_for_if_ptr == FALSE)
  {
    HSU_ERR_FATAL("release_obex_data_slot_from_app: No match for 0x%x", 
      data_for_obex_if_ptr, 0, 0);
  }
}

/*===========================================================================

FUNCTION init_obex_data_arrays_once

DESCRIPTION
Sets the OBEX data structs to default values. This process is done only once, 
and may be invoked either from hsu_al_ser_init or the very first call to 
obex_init (whichever comes first).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void init_obex_data_arrays_once
(
  void
)
{
  int obex_data_slot_index;
  uint8 core_idx = 0;
  uint8 config_idx = 0;

  static boolean performed_init_of_obex_slots = FALSE;

  /* Make sure this is done only once (step 0). 
  In case the stack goes down, uninit callbacks will be called, 
  so when the stack goes back up - the values are as if init was called. */
  if (performed_init_of_obex_slots == TRUE)
  {
    HSU_MSG_HIGH("init_obex_data_arrays_once: was already called. Doing nothing...");
    return ;
  }

  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
    {
      hsu_al_ser_static_data.obex_slot_avail_for_app[core_idx][obex_data_slot_index] = TRUE;

      hsu_al_ser_obex_init(
        &(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_idx][obex_data_slot_index]));

      hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_idx][obex_data_slot_index] = NULL;
      hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_idx][obex_data_slot_index] = NULL;
    }
  }

  /* hsu_al_ser_open will do the "real" binding between OBEX instance and app */
  hsu_al_ser_static_data.diag_obex_ptr = NULL;
#ifdef FEATURE_HS_USB_NMEA
  hsu_al_ser_static_data.nmea_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_NMEA */

#ifdef FEATURE_HS_USB_SER3_PORT
  hsu_al_ser_static_data.ser3_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
  hsu_al_ser_static_data.ser5_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
  hsu_al_ser_static_data.ser6_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
  hsu_al_ser_static_data.dci_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
  hsu_al_ser_static_data.ser4_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_QDSS_PORT
  hsu_al_ser_static_data.qdss_obex_ptr = NULL;
#endif /* FEATURE_HS_USB_QDSS_PORT */

  for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
  {
    for(config_idx = 0 ; config_idx < HSU_MAX_NUM_OF_CONFIGS; ++config_idx)
    {
      hsu_al_ser_static_data.num_of_initialized_obex_instances[core_idx][config_idx] = 0;
    }
  }

  memset(hsu_al_ser_static_data.obex_fd_handle, 0, sizeof (hsu_al_ser_static_data.obex_fd_handle));
  performed_init_of_obex_slots = TRUE;
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION init_acm_data_array_once

This function is for CSVT Enableb composition only.

DESCRIPTION
Sets the ACM data struct to default values. This process is done only once, 
and may be invoked either from hsu_al_ser_init or acm_init (whichever comes first).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void init_acm_data_array_once
(
  void
)
{
  int acm_data_slot_index;
  uint8 core_idx = 0;
  uint8 config_idx = 0;
  static boolean performed_init_of_acm_data_array = FALSE;
  
  /* Make sure this is done only once (step 0). 
  In case the stack goes down, uninit callbacks will be called, 
  so when the stack goes back up - the values are as if init was called. */
  if (performed_init_of_acm_data_array == TRUE)
  {
    HSU_MSG_HIGH("init_acm_data_once: was already called. Doing nothing...");
    return ;
  }

  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
    {
      hsu_al_ser_static_data.mdm_held_by_app[core_idx][acm_data_slot_index] = FALSE;

      hsu_al_ser_acm_init(
        &(hsu_al_ser_static_data.mdm_acm_data[core_idx][acm_data_slot_index]));

      hsu_al_ser_static_data.app_close_cb_arr_for_acm[core_idx][acm_data_slot_index] = NULL;
      hsu_al_ser_static_data.app_flush_tx_cb_arr_for_acm[core_idx][acm_data_slot_index] = NULL;
    }
  }

  hsu_al_ser_static_data.mdm_acm_ptr= NULL;
  hsu_al_ser_static_data.csvt_acm_ptr= NULL;


  for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
  {
    for(config_idx = 0 ; config_idx < HSU_MAX_NUM_OF_CONFIGS; ++config_idx)
    {
      hsu_al_ser_static_data.num_of_initialized_acm_instances[core_idx][config_idx] = 0;
    }
  }

  memset(hsu_al_ser_static_data.acm_fd_handle, 0, sizeof (hsu_al_ser_static_data.acm_fd_handle));

  performed_init_of_acm_data_array = TRUE;
}


/*===========================================================================

FUNCTION add_app_close_cb_for_acm

DESCRIPTION
Based on the given OBEX/ACM data struct, adds the supplied callback which will
be invoked when the supplied data struct had been freed from the app.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void add_app_close_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type app_close_cb
                                                  /* App supplied callback */
)
{
  int acm_data_slot_index;
  uint8 core_index;

  /* Assuming caller made sure the close callback isn't NULL */
  HSU_ASSERT((data_for_ser_if_ptr != NULL) && (app_close_cb != NULL));

  /* No need critical section, it isn't possible that both the acm_init 
  of MDM and CSVT can race, as they are sequentially called */
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {

      if (&(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_close_cb_arr_for_acm[core_index][acm_data_slot_index] = app_close_cb;
        return;
      }
    }
  }
  HSU_ERR_FATAL("add_app_close_cb_for_acm: couldn't find slot (%u)", 
    acm_data_slot_index, 0, 0);
}
#else /* FEATURE_HS_USB_CSVT_PORT */

/*===========================================================================

FUNCTION init_acm_data_once

DESCRIPTION
Sets the ACM data struct to default values. This process is done only once, 
and may be invoked either from hsu_al_ser_init or acm_init (whichever comes first).

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void init_acm_data_once
(
  void
)
{
  static boolean performed_init_of_acm_data = FALSE;
  uint8 core_idx;

  /* Make sure this is done only once (step 0). 
  In case the stack goes down, uninit callbacks will be called, 
  so when the stack goes back up - the values are as if init was called. */
  if (performed_init_of_acm_data == TRUE)
  {
    HSU_MSG_HIGH("init_acm_data_once: was already called. Doing nothing...");
    return ;
  }

  for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
  {
    hsu_al_ser_acm_init(&(hsu_al_ser_static_data.mdm_acm_data[core_idx]));
  }

  for(core_idx = 0 ; core_idx < HSU_MAX_NUM_OF_CORES; ++core_idx)
  {
    hsu_al_ser_static_data.num_of_initialized_acm_instances[core_idx] = 0;
  }
  hsu_al_ser_static_data.mdm_acm_ptr = NULL;
  hsu_al_ser_static_data.mdm_flush_tx_cb = NULL;
  hsu_al_ser_static_data.mdm_close_cb = NULL;

  memset(hsu_al_ser_static_data.acm_fd_handle, 0, sizeof (hsu_al_ser_static_data.acm_fd_handle));

  performed_init_of_acm_data = TRUE;
}
#endif /* FEATURE_HS_USB_CSVT_PORT */

/*===========================================================================

FUNCTION add_app_close_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, adds the supplied callback which will
be invoked when the supplied data struct had been freed from the app.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void add_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type app_close_cb
                                                  /* App supplied callback */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  /* Assuming caller made sure the close callback isn't NULL */
  HSU_ASSERT((data_for_ser_if_ptr != NULL) && (app_close_cb != NULL));

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {

      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][obex_data_slot_index] = app_close_cb;
        return;
      }
    }
  }
  HSU_ERR_FATAL("add_app_close_cb_for_obex: couldn't find slot (%u)",
    obex_data_slot_index, 0, 0);
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION get_app_close_cb_for_acm

DESCRIPTION
Based on the given OBEX/ACM data struct, returns the supplied callback which will
be invoked when the supplied data struct had been freed from the app.

DEPENDENCIES
None

RETURN VALUE
The app close callback, corresponding to the given OBEX/ACM data struct.

SIDE EFFECTS
None

===========================================================================*/
static sio_vv_func_ptr_type get_app_close_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int acm_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the acm_init 
  of MDM and CSVT can race, as they are sequentially called */
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        return hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][acm_data_slot_index];
      }
    }
  }
  HSU_ERR_FATAL("get_app_close_cb_for_obex: couldn't find slot (%u)", 
    acm_data_slot_index, 0, 0);
  return NULL;
}
#endif /* FEATURE_HS_USB_CSVT_PORT */
/*===========================================================================

FUNCTION get_app_close_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, returns the supplied callback which will
be invoked when the supplied data struct had been freed from the app.

DEPENDENCIES
None

RETURN VALUE
The app close callback, corresponding to the given OBEX/ACM data struct.

SIDE EFFECTS
None

===========================================================================*/
static sio_vv_func_ptr_type get_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        return hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][obex_data_slot_index];
      }
    }
  }
  HSU_ERR_FATAL("get_app_close_cb_for_obex: couldn't find slot (%u)", 
    obex_data_slot_index, 0, 0);
  return NULL;
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION remove_app_close_cb_for_acm

DESCRIPTION
Based on the given OBEX/ACM data struct, removes the stored callback which 
would have been invoked when the supplied data struct had been freed from 
the app.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void remove_app_close_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int acm_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the acm_init 
  of MDM & CSVT can race, as they are sequentially called */
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_close_cb_arr_for_acm[core_index][acm_data_slot_index] = NULL;
        return;
      }
    }
  }
  HSU_ERR_FATAL("remove_app_close_cb_for_acm: couldn't find slot (%u)", 
    acm_data_slot_index, 0, 0);
}
#endif /* FEATURE_HS_USB_CSVT_PORT */

/*===========================================================================

FUNCTION remove_app_close_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, removes the stored callback which 
would have been invoked when the supplied data struct had been freed from 
the app.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void remove_app_close_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_close_cb_arr_for_obex[core_index][obex_data_slot_index] = NULL;
        return;
      }
    }
  }
  HSU_ERR_FATAL("remove_app_close_cb_for_obex: couldn't find slot (%u)", 
    obex_data_slot_index, 0, 0);
}

/*===========================================================================

FUNCTION add_app_flush_tx_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, adds the supplied callback which will
be invoked when the supplied data struct has transmitted all pending data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void add_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type app_flush_tx_cb
                                                  /* App supplied callback */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][obex_data_slot_index] = app_flush_tx_cb;
        return;
      }
    }
  }
  HSU_ERR_FATAL("add_app_flush_tx_cb_for_obex: couldn't find slot (%u)",
    obex_data_slot_index, 0, 0);
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION add_app_flush_tx_cb_for_acm

DESCRIPTION
Based on the given OBEX/ACM data struct, adds the supplied callback which will
be invoked when the supplied data struct has transmitted all pending data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void add_app_flush_tx_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr,
             /* Data structure, which the supplied callback corresponds to */
  sio_vv_func_ptr_type acm_flush_tx_cb
                                                  /* App supplied callback */
)
{
  int acm_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of MDM and CSVT can race, as they are sequentially called */
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_flush_tx_cb_arr_for_acm[core_index][acm_data_slot_index] = acm_flush_tx_cb;
        return;
      }
    }
  }
  HSU_ERR_FATAL("add_app_flush_tx_cb_for_acm: couldn't find slot (%u)",
    acm_data_slot_index, 0, 0);
}

/*===========================================================================

FUNCTION get_app_flush_tx_cb_for_acm

DESCRIPTION
Based on the given OBEX/ACM data struct, returns the supplied callback which will
be invoked when the supplied data struct has transmitted all pending data.

DEPENDENCIES
None

RETURN VALUE
The app flush tx callback, corresponding to the given OBEX/ACM data struct.

SIDE EFFECTS
None

===========================================================================*/
static sio_vv_func_ptr_type get_app_flush_tx_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int acm_data_slot_index;
  uint8 core_index;

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES ; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        return hsu_al_ser_static_data.app_flush_tx_cb_arr_for_acm[core_index][acm_data_slot_index];
      }
    }
  }
  HSU_ERR_FATAL("get_app_flush_tx_cb_for_acm: couldn't find slot (%u)",
    acm_data_slot_index, 0, 0);
  return NULL;
}
#endif /* FEATURE_HS_USB_CSVT_PORT */

/*===========================================================================

FUNCTION get_app_flush_tx_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, returns the supplied callback which will
be invoked when the supplied data struct has transmitted all pending data.

DEPENDENCIES
None

RETURN VALUE
The app flush tx callback, corresponding to the given OBEX/ACM data struct.

SIDE EFFECTS
None

===========================================================================*/
static sio_vv_func_ptr_type get_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES ; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        return hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][obex_data_slot_index];
      }
    }
  }
  HSU_ERR_FATAL("get_app_flush_tx_cb_for_obex: couldn't find slot (%u)",
    obex_data_slot_index, 0, 0);
  return NULL;
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION remove_app_flush_tx_cb_for_acm

DESCRIPTION
Based on the given OBEX/ACM data struct, removes the stored callback which 
would have been invoked when the supplied data struct has transmitted all 
pending data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void remove_app_flush_tx_cb_for_acm
(
  const hsu_al_ser_acm_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int acm_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the acm_init 
  of MDM and CSVT can race, as they are sequentially called */
  for (acm_data_slot_index = 0 ; acm_data_slot_index < MAX_NUM_OF_ACM_USERS ; 
    ++acm_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES ; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][acm_data_slot_index] = NULL;
        return;
      }
    }
  }
  HSU_ERR_FATAL("remove_app_flush_tx_cb_for_acm: couldn't find slot (%u)",
    acm_data_slot_index, 0, 0);
}

#endif /* FEATURE_HS_USB_CSVT_PORT */
/*===========================================================================

FUNCTION remove_app_flush_tx_cb_for_obex

DESCRIPTION
Based on the given OBEX/ACM data struct, removes the stored callback which 
would have been invoked when the supplied data struct has transmitted all 
pending data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void remove_app_flush_tx_cb_for_obex
(
  const hsu_al_ser_obex_data* data_for_ser_if_ptr
            /* Data structure, which the requested callback corresponds to */
)
{
  int obex_data_slot_index;
  uint8 core_index;

  HSU_ASSERT(data_for_ser_if_ptr != NULL);

  /* No need critical section, it isn't possible that both the obex_init 
  of DIAG and NMEA can race, as they are sequentially called */
  for (obex_data_slot_index = 0 ; obex_data_slot_index < MAX_NUM_OF_OBEX_USERS ; 
    ++obex_data_slot_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES ; 
      ++core_index)
    {
      if (&(hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_data_slot_index]) == 
        data_for_ser_if_ptr)
      {
        hsu_al_ser_static_data.app_flush_tx_cb_arr_for_obex[core_index][obex_data_slot_index] = NULL;
        return;
      }
    }
  }
  HSU_ERR_FATAL("remove_app_flush_tx_cb_for_obex: couldn't find slot (%u)", 
    obex_data_slot_index, 0, 0);
}

/*===========================================================================

FUNCTION al_ser_on_close

DESCRIPTION
Serves as an internal close callback for the OBEX/ACM data structs.
Once invoked, will do all the needed cleanup and invoke the app close callback.

DEPENDENCIES
Should be called when the Giant mutex is locked. 

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_on_close
(
  void* arg_cb
                   /* Data structure, which this callback was invoked from */
)
{
  sio_vv_func_ptr_type app_close_cb = NULL;

  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("al_ser_on_close: Should be called "
       " with Giant mutex locked!", 0, 0, 0);
     return;
  }

  /* Check that arg_cb is a valid pointer */
  if ((arg_cb == (void*)hsu_al_ser_static_data.diag_obex_ptr) 
#ifdef FEATURE_HS_USB_NMEA
    ||(arg_cb == (void*)hsu_al_ser_static_data.nmea_obex_ptr)
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    ||(arg_cb == (void*)hsu_al_ser_static_data.ser3_obex_ptr)
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
    ||(arg_cb == (void*)hsu_al_ser_static_data.ser5_obex_ptr)
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
    ||(arg_cb == (void*)hsu_al_ser_static_data.ser6_obex_ptr)
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
    ||(arg_cb == (void*)hsu_al_ser_static_data.dci_obex_ptr)
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
    ||(arg_cb == (void*)hsu_al_ser_static_data.ser4_obex_ptr)
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
    ||(arg_cb == (void*)hsu_al_ser_static_data.qdss_obex_ptr)
#endif /* FEATURE_HS_USB_QDSS_PORT */

    )
  {
    hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)arg_cb;

    release_obex_data_slot_from_app(obex_data_ptr);

    /* Finally call app callback: */
    app_close_cb = get_app_close_cb_for_obex(obex_data_ptr);
    remove_app_close_cb_for_obex(obex_data_ptr);

    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "al_ser_on_close, success", obex_data_ptr->base_data.iface_type, HSU_IFACE_OFFSET2);
  }
  else if ((arg_cb == (void*)hsu_al_ser_static_data.mdm_acm_ptr)
#ifdef FEATURE_HS_USB_CSVT_PORT
|| (arg_cb == (void*)hsu_al_ser_static_data.csvt_acm_ptr)
#endif /*  FEATURE_HS_USB_CSVT_PORT */
    )
  {
#ifdef FEATURE_HS_USB_CSVT_PORT
    hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)arg_cb;

    release_acm_data_slot_from_app(acm_data_ptr);

    /* Finally call app callback: */
    app_close_cb = get_app_close_cb_for_acm(acm_data_ptr);
    remove_app_close_cb_for_acm(acm_data_ptr);
#else /* FEATURE_HS_USB_CSVT_PORT */
    /* Release mdm acm from app */
    hsu_al_ser_static_data.mdm_held_by_app = FALSE;

    app_close_cb = hsu_al_ser_static_data.mdm_close_cb;
    hsu_al_ser_static_data.mdm_close_cb = NULL;
#endif /* FEATURE_HS_USB_CSVT_PORT */

    HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "al_ser_on_close, success", acm_data_ptr->base_data.iface_type, HSU_IFACE_OFFSET2);
  }
  else
  {
    HSU_ERR_FATAL("al_ser_on_close: no match on context ptr (%u)", 
      arg_cb, 0, 0);
  }

  if (app_close_cb != NULL)
  {
    app_close_cb();
  }
}

/*===========================================================================

FUNCTION al_ser_on_flush_tx

DESCRIPTION
Serves as an internal flush tx callback for the OBEX/ACM data structs.
Once invoked, will invoke the app flush tx callback.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_on_flush_tx
(
  void* arg_cb
                   /* Data structure, which this callback was invoked from */
)
{
  sio_vv_func_ptr_type app_flush_tx_cb = NULL;

  HSU_PARAM_COULD_BE_CONST(arg_cb);

    /* Check that arg_cb is a valid pointer */
    if ((arg_cb == (void*)hsu_al_ser_static_data.diag_obex_ptr) 
#ifdef FEATURE_HS_USB_NMEA
        ||(arg_cb == (void*)hsu_al_ser_static_data.nmea_obex_ptr)
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
        ||(arg_cb == (void*)hsu_al_ser_static_data.ser3_obex_ptr)
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
        ||(arg_cb == (void*)hsu_al_ser_static_data.ser5_obex_ptr)
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
        ||(arg_cb == (void*)hsu_al_ser_static_data.ser6_obex_ptr)
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
        ||(arg_cb == (void*)hsu_al_ser_static_data.dci_obex_ptr)
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
        ||(arg_cb == (void*)hsu_al_ser_static_data.ser4_obex_ptr)
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
        ||(arg_cb == (void*)hsu_al_ser_static_data.qdss_obex_ptr)
#endif /* FEATURE_HS_USB_QDSS_PORT */
       )
    {
      hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)arg_cb;

      app_flush_tx_cb = get_app_flush_tx_cb_for_obex(obex_data_ptr);
      if (app_flush_tx_cb != NULL)
      {
        remove_app_flush_tx_cb_for_obex(obex_data_ptr);
      }

    }
  else if ((arg_cb == (void*)hsu_al_ser_static_data.mdm_acm_ptr)
#ifdef FEATURE_HS_USB_CSVT_PORT
	  || (arg_cb == (void*)hsu_al_ser_static_data.csvt_acm_ptr)
#endif	/*  FEATURE_HS_USB_CSVT_PORT */
		)
    {
#ifdef FEATURE_HS_USB_CSVT_PORT
      hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)arg_cb;

      app_flush_tx_cb = get_app_flush_tx_cb_for_acm(acm_data_ptr);
      if (app_flush_tx_cb != NULL)
    {
        remove_app_flush_tx_cb_for_acm(acm_data_ptr);
      }
#else	/*  FEATURE_HS_USB_CSVT_PORT */
      app_flush_tx_cb = hsu_al_ser_static_data.mdm_flush_tx_cb;
      hsu_al_ser_static_data.mdm_flush_tx_cb = NULL;
#endif	/*  FEATURE_HS_USB_CSVT_PORT */
    }
    else
    {
      HSU_ERR_FATAL("al_ser_on_flush_tx: no match on context ptr (%u)", 
        arg_cb, 0, 0);
    }

    if (app_flush_tx_cb !=NULL)
    {
      /* Finally call app callback: */
      app_flush_tx_cb();
    }
}

/*===========================================================================

FUNCTION data_ptr_freed_from_app

DESCRIPTION
Checks if this data pointer is no longer taken by an app, 
used by hsu_al_ser_close, to determine whether or not to continue rex_sleep 
(when a callback is not supplied).

DEPENDENCIES
None

RETURN VALUE
TRUE - Data ptr no longer held by app. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
static boolean data_ptr_is_freed_from_app
(
  const hsu_al_ser_base_data_for_ser_if* data_if_ptr
                                                /* Data structure to check */
)
{
  int obex_index = 0;
  uint8 core_index;
#ifdef FEATURE_HS_USB_CSVT_PORT
  int acm_index = 0;
#endif /* FEATURE_HS_USB_CSVT_PORT */

  HSU_ASSERT(data_if_ptr != NULL);

  for (obex_index = 0 ; obex_index < MAX_NUM_OF_OBEX_USERS ; ++obex_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; ++core_index)
    {
      if (data_if_ptr == 
        &hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_index].base_data)
      {
        if (hsu_al_ser_static_data.obex_slot_avail_for_app[core_index][obex_index] == TRUE)
        {
          return TRUE;
        }
        /* No point in continuing the search*/
        return FALSE;
      }
    }
  }

#ifdef FEATURE_HS_USB_CSVT_PORT
  for (acm_index = 0 ; acm_index < MAX_NUM_OF_ACM_USERS ; ++acm_index)
  {
    for (core_index = 0 ; core_index < HSU_MAX_NUM_OF_CORES; ++core_index)
    {
      if (data_if_ptr == 
        &hsu_al_ser_static_data.mdm_acm_data[core_index][acm_index].base_data)
      {
        if (hsu_al_ser_static_data.mdm_held_by_app[core_index][acm_index] == FALSE)
        {
          return TRUE;
        }
        return FALSE;
      }
    }
  }

#else /* FEATURE_HS_USB_CSVT_PORT */

  if (data_if_ptr == 
    &hsu_al_ser_static_data.mdm_acm_ptr->base_data)
  {
    if (hsu_al_ser_static_data.mdm_held_by_app == FALSE)
    {
      return TRUE;
    }
    return FALSE;
  }
#endif /* FEATURE_HS_USB_CSVT_PORT */
  
  /* No match found */
  HSU_ERR_FATAL("data_ptr_is_freed_from_app: no match on context ptr (%u)", 
    data_if_ptr, 0, 0);

  /* (To eliminate VC++ warning)*/
  return FALSE;
}

/*===========================================================================

FUNCTION al_ser_get_obex_slot_idx_for_sio_port

DESCRIPTION
Returns the index of the proper OBEX slot for the given port. 
It is assumed that port is using OBEX FD.

DEPENDENCIES
None

RETURN VALUE
Index corresponding to the OBEX slot for the given SIO port ID.

SIDE EFFECTS
None

===========================================================================*/
uint32 al_ser_get_obex_slot_idx_for_sio_port
(
  sio_port_id_type port_id, /* SIO port id representing the app using the
                           OBEX FD */
  uint32 *core_index        /* The core index for this SIO port */         
)
{
  uint32 obex_slot_id = HSU_IFACE_ABSENT_IN_COMP;
  *core_index = HSU_CORE_IDX_PRIMARY;
  
  /* For now, implementing a hard coded switch case */
  switch (port_id)
  {
    case AL_SER_PORT_DIAG:
    {
#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && \
    (defined(FEATURE_HS_USB_LEGACY_DEVICE) || defined(FEATURE_HS_USB_2ND_HS_CORE))
      /* Only if we are running two cores on the same processor, use NV item to
         decide where to take DIAG from */
      if (hsu_conf_sel_diag_is_on_legacy_port())
      {
        *core_index = HSU_CORE_IDX_SECONDARY;
      }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && 
          (FEATURE_HS_USB_LEGACY_DEVICE || FEATURE_HS_USB_2ND_HS_CORE) */
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(DIAG_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_diag, obex_slot_id %u", obex_slot_id);
      break;
    }
    case AL_SER_PORT_NMEA:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(NMEA_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_nmea, obex_slot_id %u", obex_slot_id);
      break;
    }
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(SER3_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_ser3, obex_slot_id %u", obex_slot_id);
      break;
    }
#endif /* FEATURE_HS_USB_SER3_PORT */

#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(SER4_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_ser4, obex_slot_id %u", obex_slot_id);
      break;
    }
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(SER5_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_ser5, obex_slot_id %u", obex_slot_id);
      break;
    }
#endif /* FEATURE_HS_USB_SER5_PORT */

#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(SER6_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_ser6, obex_slot_id %u", obex_slot_id);
      break;
    }
#endif /* FEATURE_HS_USB_SER6_PORT */

#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(DCI_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_dci, obex_slot_id %u", obex_slot_id);
      break;
    }
#endif /* FEATURE_HS_USB_DCI_PORT */


#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
    {
      obex_slot_id = hsu_conf_sel_get_iface_index_comp(QDSS_HSU_IFACE);
      HSU_ULOG_1(NO_MSG, DATA_LOG, "slot_index_for_qdss, obex_slot_id %u", obex_slot_id);
      break;
    }
#endif /* FEATURE_HS_USB_QDSS_PORT */

    default: 
    {
      HSU_ERR_FATAL("al_ser_get_obex_slot_idx_for_sio_port: Port: %d not supported", 
        port_id, 0, 0);      
    }
  }

  if ((*core_index >= HSU_MAX_NUM_OF_CORES)
    || (obex_slot_id >= MAX_NUM_OF_OBEX_USERS && obex_slot_id != HSU_IFACE_ABSENT_IN_COMP))
  {
    HSU_ERR_FATAL("obex_slot_idx_for_sio_port: out of bound - core_index (%u),"
      " obex_data_slot_index (%u / %u)",
      core_index, obex_slot_id, MAX_NUM_OF_OBEX_USERS);
    
    obex_slot_id = HSU_IFACE_ABSENT_IN_COMP;
  }

  return obex_slot_id;
}

#ifdef FEATURE_HS_USB_CSVT_PORT
/*===========================================================================

FUNCTION get_and_alloc_acm_data_for_app

===========================================================================*/
static hsu_al_ser_acm_data* get_and_alloc_acm_data_for_app 
(
 sio_port_id_type port_id /* SIO port id representing the app using the
                          MDM FD */
)
{
  uint8 acm_data_slot_index = 0;
  uint8 config_index;
  uint32 total_num_of_initialized_acm_instances = 0;
  hsu_al_ser_acm_data* returned_value = NULL;
  uint8 core_index = HSU_CORE_IDX_PRIMARY;

  /* For now, implementing a hard coded switch case */
#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && \
    (defined(FEATURE_HS_USB_LEGACY_DEVICE) || defined(FEATURE_HS_USB_2ND_HS_CORE))
      /* Only if we are running two cores on the same processor, use NV item to
         decide where to take DIAG from */
      if (hsu_conf_sel_diag_is_on_legacy_port())
      {
        core_index = HSU_CORE_IDX_SECONDARY;
      }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && 
          (FEATURE_HS_USB_LEGACY_DEVICE || FEATURE_HS_USB_2ND_HS_CORE) */

  switch (port_id)
  {
  	case AL_SER_PORT_MDM:
		{
			acm_data_slot_index = 0;
			break;
		}
	case AL_SER_PORT_MDM2:
		{
			acm_data_slot_index = 1;
			break;
		}
  }
  	
	
  /* Verify that there is at least one acm instance that was already initialized */
  for (config_index=0; config_index < HSU_MAX_NUM_OF_CONFIGS; config_index++)
  {
    total_num_of_initialized_acm_instances += 
  	hsu_al_ser_static_data.num_of_initialized_acm_instances[core_index][config_index];
  }
  if (total_num_of_initialized_acm_instances == 0)
  {
    HSU_MSG_ERROR_1("get_and_alloc_acm_data_slot_for_app: Core index %d"
      "has no initialized acm instances", core_index);
    return NULL;
  }
  
  if (hsu_al_ser_static_data.mdm_held_by_app[core_index][acm_data_slot_index] == TRUE)
  {
    HSU_MSG_ERROR_1("get_and_alloc_acm_data_slot_for_app: already allocated for"
      "port_id: %d", port_id);
    return NULL;
  }
  
  hsu_al_ser_static_data.mdm_held_by_app[core_index][acm_data_slot_index] = TRUE;


  returned_value = 
    &(hsu_al_ser_static_data.mdm_acm_data[core_index][acm_data_slot_index]);
	
	return returned_value;

}
#else /* FEATURE_HS_USB_CSVT_PORT */
/*===========================================================================

FUNCTION get_and_alloc_acm_data_for_app

===========================================================================*/
static hsu_al_ser_acm_data* get_and_alloc_acm_data_for_app (void)
{
  hsu_al_ser_acm_data* returned_value = NULL;
  uint8 core_index = HSU_CORE_IDX_PRIMARY;

  /* For now, implementing a hard coded switch case */
#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && \
    (defined(FEATURE_HS_USB_LEGACY_DEVICE) || defined(FEATURE_HS_USB_2ND_HS_CORE))
      /* Only if we are running two cores on the same processor, use NV item to
         decide where to take DIAG from */
      if (hsu_conf_sel_diag_is_on_legacy_port())
      {
        core_index = HSU_CORE_IDX_SECONDARY;
      }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && 
          (FEATURE_HS_USB_LEGACY_DEVICE || FEATURE_HS_USB_2ND_HS_CORE) */
  returned_value = 
    &(hsu_al_ser_static_data.mdm_acm_data[core_index]);
	
	return returned_value;

}
#endif /* FEATURE_HS_USB_CSVT_PORT */

/*===========================================================================

FUNCTION al_ser_internal_flush_tx

DESCRIPTION
actual flush tx function.

DEPENDENCIES
Should be called when the Giant mutex is locked. 

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void al_ser_internal_flush_tx
(
 sio_port_id_type port_id,
 hsu_al_ser_base_op_complete_cb flush_tx_done_cb
)
{
  /*Verify that the Giant is locked by current task */
  if (!hsu_os_is_giant_locked_by_curr_task())  
  {
     HSU_ERR_FATAL("al_ser_internal_flush_tx: Should be called "
       " with Giant mutex locked!", 0, 0, 0);
     return ;
  }

  HSU_ULOG_ENUM(HIGH_MSG, DATA_LOG, "al_ser_internal_flush_tx", hsu_conf_sel_get_iface_type_from_port_id(port_id), HSU_IFACE_OFFSET2);

  switch (port_id)
  {
    case AL_SER_PORT_MDM:
    {
      /* call specific acm function - because it uses HW queuing.
      After the flush- it will complete the last write operation explicitly*/
      hsu_al_ser_acm_flush_tx(hsu_al_ser_static_data.mdm_acm_ptr,
        flush_tx_done_cb);
      break;
    }
#ifdef FEATURE_HS_USB_CSVT_PORT
    case AL_SER_PORT_MDM2:
    {
      /* call specific acm function - because it uses HW queuing.
      After the flush- it will complete the last write operation explicitly*/
      hsu_al_ser_acm_flush_tx(hsu_al_ser_static_data.csvt_acm_ptr,
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_CSVT_PORT */
    case AL_SER_PORT_DIAG:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.diag_obex_ptr->base_data, 
        flush_tx_done_cb);

      break;
    }
#ifdef FEATURE_HS_USB_NMEA
    case AL_SER_PORT_NMEA:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.nmea_obex_ptr->base_data, 
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
    case AL_SER_PORT_USB_SER3:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.ser3_obex_ptr->base_data,
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
    case AL_SER_PORT_USB_SER5:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.ser5_obex_ptr->base_data,
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
    case AL_SER_PORT_USB_SER6:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.ser6_obex_ptr->base_data,
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
    case AL_SER_PORT_USB_DCI:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.dci_obex_ptr->base_data,
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
    case AL_SER_PORT_USB_SER4:
    {
      hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.ser4_obex_ptr->base_data,
        flush_tx_done_cb);
      break;
    }
#endif /* FEATURE_HS_USB_SER4_PORT */

#ifdef FEATURE_HS_USB_QDSS_PORT
    case AL_SER_PORT_USB_QDSS:
    {
       hsu_al_ser_base_flush_tx(&hsu_al_ser_static_data.qdss_obex_ptr->base_data,
	flush_tx_done_cb);
        break;
    }
#endif /* FEATURE_HS_USB_QDSS_PORT */

    default:
    {
      HSU_ERR_FATAL("al_ser_internal flush_tx: Port: %d not supported", 
        port_id, 0, 0);
    }
  }
}
/*===========================================================================

FUNCTION al_ser_complete_close_after_flush_tx

DESCRIPTION
This is the cb given to flush_tx, when it's called from the "al_ser_close" function.
So this cb will end the close port process.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
static void al_ser_complete_close_after_flush_tx
(
  void* arg_cb
)
{
  HSU_MSG_HIGH("al_ser_complete_close_after_flush_tx: Entered");

  /* First check that there is flush tx callback pending.  If so, remove it
  ** from the array and invoke it */
  al_ser_on_flush_tx(arg_cb);

  /* Check that data_for_ser_if_ptr is a valid pointer */
  if (arg_cb == (void*)hsu_al_ser_static_data.diag_obex_ptr)
  {
    hsu_al_ser_obex_close(hsu_al_ser_static_data.diag_obex_ptr, al_ser_on_close, 
      TRUE);
  }
#ifdef FEATURE_HS_USB_NMEA
  else if (arg_cb == (void*)hsu_al_ser_static_data.nmea_obex_ptr)
  {
     hsu_al_ser_base_close(&hsu_al_ser_static_data.nmea_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_NMEA */
#ifdef FEATURE_HS_USB_SER3_PORT
  else if(arg_cb == (void*)hsu_al_ser_static_data.ser3_obex_ptr)
  {
    hsu_al_ser_base_close(&hsu_al_ser_static_data.ser3_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
  else if(arg_cb == (void*)hsu_al_ser_static_data.ser5_obex_ptr)
  {
    hsu_al_ser_base_close(&hsu_al_ser_static_data.ser5_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
  else if(arg_cb == (void*)hsu_al_ser_static_data.ser6_obex_ptr)
  {
    hsu_al_ser_base_close(&hsu_al_ser_static_data.ser6_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
  else if(arg_cb == (void*)hsu_al_ser_static_data.dci_obex_ptr)
  {
    hsu_al_ser_base_close(&hsu_al_ser_static_data.dci_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
  else if(arg_cb == (void*)hsu_al_ser_static_data.ser4_obex_ptr)
  {
    hsu_al_ser_base_close(&hsu_al_ser_static_data.ser4_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
  else if(arg_cb == (void*)hsu_al_ser_static_data.qdss_obex_ptr)
  {
    hsu_al_ser_base_close(&hsu_al_ser_static_data.qdss_obex_ptr->base_data, al_ser_on_close, TRUE);
  }
#endif /* FEATURE_HS_USB_QDSS_PORT */
  else if (arg_cb == (void*)hsu_al_ser_static_data.mdm_acm_ptr)
  {
    /* acm_close called with: abort_write_operations =FALSE since flush_tx was completed,
       and thus write operations are taken care of*/
    hsu_al_ser_acm_close( hsu_al_ser_static_data.mdm_acm_ptr,
                          al_ser_on_close, FALSE);
  }
#ifdef FEATURE_HS_USB_CSVT_PORT
  else if (arg_cb == (void*)hsu_al_ser_static_data.csvt_acm_ptr)
  {
    /* acm_close called with: abort_write_operations =FALSE since flush_tx was completed,
       and thus write operations are taken care of*/
    hsu_al_ser_acm_close( hsu_al_ser_static_data.csvt_acm_ptr,
                          al_ser_on_close, FALSE);
  }
#endif /* FEATURE_HS_USB_CSVT_PORT */
  else
  {
    HSU_ERR_FATAL("al_ser_complete_close_after_flush_tx: no match (%u)",
      arg_cb, 0, 0);
  }

  HSU_MSG_HIGH("al_ser_complete_close_after_flush_tx: Exit");
}

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_SET_CONFIGURATION

DESCRIPTION
This function is called when the enabled callback is received.
This function sets the FD handle according to the current configuration FD, for
a given core_index.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Note that data_for_apps_using_obex_if of the first configuration is used for all 
configurations, and that its variables are not cleared when switching to a new 
configuration.

==============================================================================*/

void hsu_al_ser_handle_set_configuration(uint8 core_index)
{
  uint8 obex_users_idx = 0;
  uint8 config_idx = 0;

  config_idx = core_get_curr_configuration(core_index);
  HSU_MSG_LOW_1("hsu_al_ser_handle_set_configuration(): config_idx = %d.", config_idx);

  /* KW: config_idx could go out of bounds in theory */
  if ((config_idx == 0) || (config_idx > HSU_MAX_NUM_OF_CONFIGS) ||(config_idx == hsu_al_ser_static_data.curr_config_idx[core_index]))
  {
    HSU_MSG_LOW_2("hsu_al_ser_handle_set_configuration():config_idx=%d,core_index=%d,nothing to do.", 
      config_idx, core_index);
    return;
  }

  /* Set OBEX FD instances according to new configuration */
  for (obex_users_idx = 0; obex_users_idx < MAX_NUM_OF_OBEX_USERS; ++obex_users_idx)
  {
    hsu_al_ser_static_data.data_for_apps_using_obex_if[core_index][obex_users_idx].base_data.instance_of_fd_used = 
      hsu_al_ser_static_data.obex_fd_handle[core_index][config_idx-1][obex_users_idx];
  }
  
  hsu_al_ser_static_data.curr_config_idx[core_index] = config_idx;
}

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_OBEX_UNINIT

DESCRIPTION
This function is called when OBEX CDC FD notifies it is being 
un-initialized

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ser_handle_obex_uninit
( 
 cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  hsu_al_ser_obex_data* obex_data_ptr = (hsu_al_ser_obex_data*)ctx;
  uint8 config_index;

  HSU_ASSERT(obex_data_ptr->obex_if_data.core_idx < HSU_MAX_NUM_OF_CORES);

  /* Assume that if this function is called for core_idx, for the FIRST configuration,
     it will be called for ALL configurations of that core_idx. So clean data structures 
     for this core_idx on the FIRST call only. */ 
  if (hsu_al_ser_static_data.num_of_initialized_obex_instances[obex_data_ptr->obex_if_data.core_idx][HSU_CONFIG_IDX_0] != 0)
  {
    for (config_index=0; config_index < HSU_MAX_NUM_OF_CONFIGS; config_index++)
    {
      hsu_al_ser_static_data.num_of_initialized_obex_instances[obex_data_ptr->obex_if_data.core_idx][config_index] = 0;
    }

    memset(hsu_al_ser_static_data.obex_fd_handle[obex_data_ptr->obex_if_data.core_idx], 
      0, sizeof (hsu_al_ser_static_data.obex_fd_handle) / HSU_MAX_NUM_OF_CORES);
  }
}

/*==============================================================================

FUNCTION          HSU_AL_SER_HANDLE_ACM_UNINIT

DESCRIPTION
This function is called when ACM CDC FD notifies it is being 
un-initialized

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ser_handle_acm_uninit
(
 cdc_appctx_t ctx                           /* Context passed in callback */
)
{
  uint8 config_index=0;
  hsu_al_ser_acm_data* acm_data_ptr = (hsu_al_ser_acm_data*)ctx;

  HSU_USE_PARAM(config_index);

  HSU_ASSERT(acm_data_ptr->modem_if_data.core_idx < HSU_MAX_NUM_OF_CORES);

#ifdef FEATURE_HS_USB_CSVT_PORT
  if (hsu_al_ser_static_data.num_of_initialized_acm_instances[acm_data_ptr->modem_if_data.core_idx][HSU_CONFIG_IDX_0] != 0)
  {
    for (config_index=0; config_index < HSU_MAX_NUM_OF_CONFIGS; config_index++)
    {
      hsu_al_ser_static_data.num_of_initialized_acm_instances[acm_data_ptr->modem_if_data.core_idx][config_index] = 0;
    }

    memset(hsu_al_ser_static_data.acm_fd_handle[acm_data_ptr->modem_if_data.core_idx], 
      0, sizeof (hsu_al_ser_static_data.acm_fd_handle) / HSU_MAX_NUM_OF_CORES);
  }
#else /* FEATURE_HS_USB_CSVT_PORT */
  if (hsu_al_ser_static_data.num_of_initialized_acm_instances[acm_data_ptr->modem_if_data.core_idx] != 0)
  {
    hsu_al_ser_static_data.num_of_initialized_acm_instances[acm_data_ptr->modem_if_data.core_idx] = 0;

    memset(hsu_al_ser_static_data.acm_fd_handle[acm_data_ptr->modem_if_data.core_idx], 
      0, sizeof (hsu_al_ser_static_data.acm_fd_handle) / HSU_MAX_NUM_OF_CORES);
  }
#endif /* FEATURE_HS_USB_CSVT_PORT */
}

/*===========================================================================

FUNCTION      hsu_al_ser_pool_many_cb

DESCRIPTION
This function invokes the appropriate actions when the serial memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_pool_many_cb
(
 dsm_mempool_id_enum_type pool_id
)
{
  if ((hsu_al_ser_static_data.diag_obex_ptr!= NULL) && 
      (pool_id == hsu_al_ser_static_data.diag_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.diag_obex_ptr->base_data);
  }

  if ((hsu_al_ser_static_data.mdm_acm_ptr != NULL) && 
      (pool_id == hsu_al_ser_static_data.mdm_acm_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.mdm_acm_ptr->base_data);
  }

#ifdef FEATURE_HS_USB_CSVT_PORT
  if ((hsu_al_ser_static_data.csvt_acm_ptr != NULL) && 
      (pool_id == hsu_al_ser_static_data.csvt_acm_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.csvt_acm_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_CSVT_PORT */

#ifdef FEATURE_HS_USB_NMEA
  if ((hsu_al_ser_static_data.nmea_obex_ptr != NULL) && 
      (pool_id == hsu_al_ser_static_data.nmea_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.nmea_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_NMEA */

#ifdef FEATURE_HS_USB_SER3_PORT
  if ((hsu_al_ser_static_data.ser3_obex_ptr != NULL) &&
      (pool_id == hsu_al_ser_static_data.ser3_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.ser3_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_SER3_PORT */
#ifdef FEATURE_HS_USB_SER5_PORT
  if ((hsu_al_ser_static_data.ser5_obex_ptr != NULL) &&
      (pool_id == hsu_al_ser_static_data.ser5_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.ser5_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_SER5_PORT */
#ifdef FEATURE_HS_USB_SER6_PORT
  if ((hsu_al_ser_static_data.ser6_obex_ptr != NULL) &&
      (pool_id == hsu_al_ser_static_data.ser6_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.ser6_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_SER6_PORT */
#ifdef FEATURE_HS_USB_DCI_PORT
  if ((hsu_al_ser_static_data.dci_obex_ptr != NULL) &&
      (pool_id == hsu_al_ser_static_data.dci_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.dci_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_DCI_PORT */
#ifdef FEATURE_HS_USB_SER4_PORT
  if ((hsu_al_ser_static_data.ser4_obex_ptr != NULL) &&
      (pool_id == hsu_al_ser_static_data.ser4_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.ser4_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_SER4_PORT */
#ifdef FEATURE_HS_USB_QDSS_PORT
  if ((hsu_al_ser_static_data.qdss_obex_ptr != NULL) &&
      (pool_id == hsu_al_ser_static_data.qdss_obex_ptr->base_data.dsm_pool_id))
  {
    hsu_al_ser_base_pool_many_cb(pool_id, 
      &hsu_al_ser_static_data.qdss_obex_ptr->base_data);
  }
#endif /* FEATURE_HS_USB_QDSS_PORT */

}
