/*===========================================================================

                       P S _ S O C K _ M E M _ P O O L _ C

DESCRIPTION
 The Protocol Services Socket Memory Pool Header File. Contains declarations
 of functions specifice to sockets memory initialization.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
==============================================================================
when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/08/18    sr              Initial Version
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_PS
#include "ps_mem.h"
#include "ps_meta_info.h"
#include "dssocki.h"
#include "ps_pkt_info.h"
#include "ps_udp.h"
#include "ps_socket_defs.h"
#include "ps_sock_mem_pool.h"
#include "dss_config.h"


/*===========================================================================

                     LOCAL DATA DEFINITIONS

===========================================================================*/
/*---------------------------------------------------------------------------
  Tuning the number of ps meta info buffers needed by this module
---------------------------------------------------------------------------*/
#define PS_TX_META_INFO_BUF_SIZE  ((sizeof(ps_tx_meta_info_type) + 3) & ~3)
#define PS_RX_META_INFO_BUF_SIZE  ((sizeof(ps_rx_meta_info_type) + 3) & ~3)
#define PS_META_INFO_BUF_SIZE  ((sizeof(ps_meta_info_type) + 3) & ~3)

#define PS_RT_META_INFO_BUF_SIZE  (( sizeof( ps_rt_meta_info_type) + 3) & ~3)


#define PS_META_INFO_BUF_NUM       70
#define PS_META_INFO_BUF_HIGH_WM   60
#define PS_META_INFO_BUF_LOW_WM     5


#define PS_RT_META_INFO_BUF_NUM       70
#define PS_RT_META_INFO_BUF_HIGH_WM   60
#define PS_RT_META_INFO_BUF_LOW_WM    5

/*----------------------------------------------------------------------------
  Allocate memory to hold ps_meta_info along with ps_mem header
----------------------------------------------------------------------------*/
static int ps_meta_info_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                (
                                 PS_META_INFO_BUF_NUM,
                                 PS_META_INFO_BUF_SIZE
                                )];

static int ps_rt_meta_info_buf_mem[ PS_MEM_GET_TOT_SIZE_OPT
                                    (
                                      PS_RT_META_INFO_BUF_NUM,
                                      PS_RT_META_INFO_BUF_SIZE
                                    )];

/*----------------------------------------------------------------------------
  Allocate memory to hold different ps_meta_info buffers along with ps_mem
  header
----------------------------------------------------------------------------*/
static int ps_tx_meta_info_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(
                                                    PS_META_INFO_BUF_NUM,
                                                    PS_TX_META_INFO_BUF_SIZE)];

static int ps_rx_meta_info_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(
                                                    PS_META_INFO_BUF_NUM,
                                                    PS_RX_META_INFO_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ps_meta_info_type_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type	 * ps_tx_meta_info_buf_hdr[PS_META_INFO_BUF_NUM];
static ps_tx_meta_info_type  * ps_tx_meta_info_buf_ptr[PS_META_INFO_BUF_NUM];
	
static ps_mem_buf_hdr_type	 * ps_rx_meta_info_buf_hdr[PS_META_INFO_BUF_NUM];
static ps_rx_meta_info_type  * ps_rx_meta_info_buf_ptr[PS_META_INFO_BUF_NUM];

static ps_mem_buf_hdr_type   * ps_meta_info_buf_hdr[PS_META_INFO_BUF_NUM];
static ps_meta_info_type     * ps_meta_info_buf_ptr[PS_META_INFO_BUF_NUM];

static ps_mem_buf_hdr_type   * ps_rt_meta_info_buf_hdr[PS_RT_META_INFO_BUF_NUM];
static ps_rt_meta_info_type  * ps_rt_meta_info_buf_ptr[PS_RT_META_INFO_BUF_NUM];


#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ds sockets internal event buffers needed by this module
---------------------------------------------------------------------------*/
#define DSSOCKI_EVT_BUF_SIZE  ((sizeof(dssocki_event_buf_type) + 3) & ~3)

#define DSSOCKI_EVT_BUF_NUM        70
#define DSSOCKI_EVT_BUF_HIGH_WM    60
#define DSSOCKI_EVT_BUF_LOW_WM     5


/*----------------------------------------------------------------------------
  Allocate memory to hold dssocki_event_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int dssocki_evt_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                               (
                                DSSOCKI_EVT_BUF_NUM,
                                DSSOCKI_EVT_BUF_SIZE
                               )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to dssocki_evt_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type     * dssocki_evt_buf_hdr[DSSOCKI_EVT_BUF_NUM];
static dssocki_event_buf_type  * dssocki_evt_buf_ptr[DSSOCKI_EVT_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of dssocki app to ps command buffers needed by this module
---------------------------------------------------------------------------*/
#define DSSOCKI_APP_TO_PS_CMD_BUF_SIZE  ((sizeof(dssocki_app_to_ps_cmd_type) + 3) & ~3)

#ifndef FEATURE_DATA_PS_LOW_MEM_CHIPSET

#define DSSOCKI_APP_TO_PS_CMD_BUF_NUM       100
#define DSSOCKI_APP_TO_PS_CMD_BUF_HIGH_WM    95
#define DSSOCKI_APP_TO_PS_CMD_BUF_LOW_WM     5

#else

#define DSSOCKI_APP_TO_PS_CMD_BUF_NUM       50
#define DSSOCKI_APP_TO_PS_CMD_BUF_HIGH_WM   40
#define DSSOCKI_APP_TO_PS_CMD_BUF_LOW_WM     5
#endif /* FEATURE_DATA_PS_LOW_MEM_CHIPSET */

/*----------------------------------------------------------------------------
  Allocate memory to hold dssocki_app_to_ps_cmd_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int dssocki_app_to_ps_cmd_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                                         (
                                           DSSOCKI_APP_TO_PS_CMD_BUF_NUM,
                                           DSSOCKI_APP_TO_PS_CMD_BUF_SIZE
                                         )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to dssocki_app_to_ps_cmd_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type         * dssocki_app_to_ps_cmd_buf_hdr[DSSOCKI_APP_TO_PS_CMD_BUF_NUM];
static dssocki_app_to_ps_cmd_type  * dssocki_app_to_ps_cmd_buf_ptr[DSSOCKI_APP_TO_PS_CMD_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of ip packet info buffers needed by this module
---------------------------------------------------------------------------*/
#define IP_PKT_INFO_BUF_SIZE  ((sizeof(ip_pkt_info_type) + 3) & ~3)


#define IP_PKT_INFO_BUF_NUM       70
#define IP_PKT_INFO_BUF_HIGH_WM   60
#define IP_PKT_INFO_BUF_LOW_WM     5

/*----------------------------------------------------------------------------
  Allocate memory to hold ip_pkt_info_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int ip_pkt_info_buf_mem[PS_MEM_GET_TOT_SIZE_OPT
                               (
                                IP_PKT_INFO_BUF_NUM,
                                IP_PKT_INFO_BUF_SIZE
                               )];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to ip_pkt_info_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type   * ip_pkt_info_buf_hdr[IP_PKT_INFO_BUF_NUM];
static ip_pkt_info_type      * ip_pkt_info_buf_ptr[IP_PKT_INFO_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */


/*---------------------------------------------------------------------------
  Tuning the number of scb buffers needed by this module
---------------------------------------------------------------------------*/
#define SCB_BUF_SIZE  ((sizeof(scb_type) + 3) & ~3)

#define SCB_BUF_NUM       DSS_MAX_SOCKS
#define SCB_BUF_HIGH_WM   (DSS_MAX_SOCKS - 1)
#define SCB_BUF_LOW_WM    1

/*----------------------------------------------------------------------------
  Allocate memory to hold scb_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int scb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(SCB_BUF_NUM, SCB_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to scb_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * scb_buf_hdr[SCB_BUF_NUM];
static scb_type            * scb_buf_ptr[SCB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

/*---------------------------------------------------------------------------
  Tuning the number of acb buffers needed by this module
---------------------------------------------------------------------------*/
#define ACB_BUF_SIZE  ((sizeof(acb_type) + 3) & ~3)

#define ACB_BUF_NUM         DSS_MAX_APPS
#define ACB_BUF_HIGH_WM     (DSS_MAX_APPS - 1)
#define ACB_BUF_LOW_WM      1

/*----------------------------------------------------------------------------
  Allocate memory to hold acb_buf along with ps_mem header
----------------------------------------------------------------------------*/
static int acb_buf_mem[PS_MEM_GET_TOT_SIZE_OPT(ACB_BUF_NUM, ACB_BUF_SIZE)];

#ifdef FEATURE_DATA_PS_MEM_DEBUG
/*----------------------------------------------------------------------------
  Array of pointers used to facilitate easy debugging.  The first one points
  to the ps_mem header and the latter ponts to acb_buf
----------------------------------------------------------------------------*/
static ps_mem_buf_hdr_type * acb_buf_hdr[ACB_BUF_NUM];
static acb_type            * acb_buf_ptr[ACB_BUF_NUM];
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

#endif /*FEATURE_DATA_PS*/

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
#define PS_MEM_POOL_INIT ps_mem_pool_init
/*===========================================================================
FUNCTION PS_SOCK_MEM_POOL_INIT()

DESCRIPTION
  Initializes memory for sockets module

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_sock_mem_pool_init
(
  void
)
{
  ps_mem_pool_enum_type pool_id;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Initialize Pools
  -------------------------------------------------------------------------*/
  pool_id = PS_MEM_TX_META_INFO_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
					  ps_tx_meta_info_buf_mem,
					  PS_TX_META_INFO_BUF_SIZE,
					  PS_META_INFO_BUF_NUM,
					  PS_META_INFO_BUF_HIGH_WM,
					  PS_META_INFO_BUF_LOW_WM,
					  NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
						 (int *) ps_tx_meta_info_buf_hdr,
						 (int *) ps_tx_meta_info_buf_ptr
#else
						  NULL,
						  NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
						  ) == -1)
	  {
			goto bail;
	  }
  pool_id = PS_MEM_RX_META_INFO_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
						  ps_rx_meta_info_buf_mem,
						  PS_RX_META_INFO_BUF_SIZE,
						  PS_META_INFO_BUF_NUM,
						  PS_META_INFO_BUF_HIGH_WM,
						  PS_META_INFO_BUF_LOW_WM,
						  NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
						 (int *) ps_rx_meta_info_buf_hdr,
						 (int *) ps_rx_meta_info_buf_ptr
#else
						  NULL,
						  NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
								) == -1)
	  {
		goto bail;
	  }


  pool_id = PS_MEM_META_INFO_TYPE_EX;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
                       ps_meta_info_buf_mem,
                       PS_META_INFO_BUF_SIZE,
                       PS_META_INFO_BUF_NUM,
                       PS_META_INFO_BUF_HIGH_WM,
                       PS_META_INFO_BUF_LOW_WM,
                       NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                       (int *) ps_meta_info_buf_hdr,
                       (int *) ps_meta_info_buf_ptr
#else
                       NULL,
                       NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                      ) == -1)
  {
    goto bail;
  }

  pool_id = PS_MEM_RT_META_INFO_TYPE;
  if (PS_MEM_POOL_INIT_OPT( pool_id,
                         ps_rt_meta_info_buf_mem,
                         PS_RT_META_INFO_BUF_SIZE,
                         PS_RT_META_INFO_BUF_NUM,
                         PS_RT_META_INFO_BUF_HIGH_WM,
                         PS_RT_META_INFO_BUF_LOW_WM,
						 NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
						 (int *) ps_rt_meta_info_buf_hdr,
						 (int *) ps_rt_meta_info_buf_ptr
#else
                         NULL,
                         NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
						) == -1)
 {
    DS_MSG0(MSG_LEGACY_ERROR,"MemManager::MemPoolInit(): "
		"Can't init the module");	
		goto bail;
 }


  pool_id = PS_MEM_DSS_IOCTL_EVT_CB_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
                           dssocki_evt_buf_mem,
                           DSSOCKI_EVT_BUF_SIZE,
                           DSSOCKI_EVT_BUF_NUM,
                           DSSOCKI_EVT_BUF_HIGH_WM,
                           DSSOCKI_EVT_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) dssocki_evt_buf_hdr,
                           (int *) dssocki_evt_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  pool_id = PS_MEM_PKT_META_INFO_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
                           ip_pkt_info_buf_mem,
                           IP_PKT_INFO_BUF_SIZE,
                           IP_PKT_INFO_BUF_NUM,
                           IP_PKT_INFO_BUF_HIGH_WM,
                           IP_PKT_INFO_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) ip_pkt_info_buf_hdr,
                           (int *) ip_pkt_info_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  pool_id = PS_MEM_APP_TO_PS_CMD_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
                            dssocki_app_to_ps_cmd_buf_mem,
                            DSSOCKI_APP_TO_PS_CMD_BUF_SIZE,
                            DSSOCKI_APP_TO_PS_CMD_BUF_NUM,
                            DSSOCKI_APP_TO_PS_CMD_BUF_HIGH_WM,
                            DSSOCKI_APP_TO_PS_CMD_BUF_LOW_WM,
                            NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) dssocki_app_to_ps_cmd_buf_hdr,
                           (int *) dssocki_app_to_ps_cmd_buf_ptr
#else
                            NULL,
                            NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
							) == -1)
	{
	  goto bail;
	}

  pool_id = PS_MEM_DSS_SCB_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
                           scb_buf_mem,
                           SCB_BUF_SIZE,
                           SCB_BUF_NUM,
                           SCB_BUF_HIGH_WM,
                           SCB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) scb_buf_hdr,
                           (int *) scb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  pool_id = PS_MEM_DSS_ACB_TYPE;
  if (PS_MEM_POOL_INIT_OPT(pool_id,
                           acb_buf_mem,
                           ACB_BUF_SIZE,
                           ACB_BUF_NUM,
                           ACB_BUF_HIGH_WM,
                           ACB_BUF_LOW_WM,
                           NULL,
#ifdef FEATURE_DATA_PS_MEM_DEBUG
                           (int *) acb_buf_hdr,
                           (int *) acb_buf_ptr
#else
                           NULL,
                           NULL
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
                          ) == -1)
  {
    goto bail;
  }

  /*-------------------------------------------------------------------------
    Register for memory available callback with PS_MEM
  -------------------------------------------------------------------------*/
  ps_mem_reg_mem_avail_cb ( PS_MEM_APP_TO_PS_CMD_TYPE,
                            dssocki_ps_mem_low_wm_cback );

  return;

  /*-------------------------------------------------------------------------
    Common Error Handling.
  -------------------------------------------------------------------------*/
  bail:
  {
    ERR_FATAL("Can't init the module. pool_id = %d", pool_id, 0, 0);
  }
} /* ps_sock_mem_pool_init() */


