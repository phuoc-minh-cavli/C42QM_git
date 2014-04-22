/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ P P P _ T A S K . C

GENERAL DESCRIPTION
  This is the file that defines all of the PPP functions that are tied to
  executing PPP in some task context.

EXTERNALIZED FUNCTIONS
  ppp_task_powerup_init()
    The fucntion that must be called from the executing tasks powerup
    initialization function to initialize PPP and register all of the
    callbacks etc...

Copyright (c) 2002-2019 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/dataiot.mpss/2.0/protocols/linklayer/src/ps_ppp_task.c#2 $ 
  $Author: pwbldsvc $ $DateTime: 2023/04/06 06:44:01 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
04/08/19    ds     Memory optimization related changes.
07/08/14    cx     Fixed the error when reading NV_PPP_CONFIG_DATA_I NV item  
12/14/12    fn     Remove default NV item writes when NV read fails
11/05/12    fn     Optimizing the read for PS PPP CONFIG NV item 
05/01/12    sb     Always On UIM Feature Cleanup
10/17/11    ssh    eHRPD Always On support
06/06/11    ash    Added support for PPP Partial Context
03/23/11    op     Data SU API cleanup
01/28/11    jee    JCDMA feature cleanup
12/05/10    ssh    Added ppp_ncp_abort()
11/18/10    ssh    Added ppp_ncp_resync()
08/31/09    mga    Merged from eHRPD branch
06/09/09    mga    Merged Compiler warnings fixes
05/07/09    pp     CMI Phase-4: SU Level API Effort.
11/10/08    scb    Fixed QTF compilation warnings.
01/22/08    scb    Mainlined features under FEATURE_JCDMA_1X
12/06/07    scb    Swapped the auth_retry and auth_timeout variables in the
                   ppp_task_init() function for consistency.
08/18/07    scb    Featurized the MMGSDI interaction of RUIM code using the
                   FEATURE_RUIM_FUNC_INTERFACE feature.
08/16/07    scb    Added PPP Authentication initialization function to 
                   ppp_task_init()
05/14/07    scb    Changed variable names to avoid compilation error for
                   JCDMA_1X builds
04/27/07    scb    Added code to populate the default values for constants
                   such as LCP/IPCP/AUTH and IPv6CP timeout and retry values
                   during ppp_task_init() under FEATURE_JCDMA_1X
04/02/07    ssh    New, PPP_VENDOR_SEND_CMD. Error checks. More F3 messages.
02/10/05    ifk    Change LCP term timeouts to 3 sec by default.
10/31/04    ifk    Added reading of ppp_ccp_reject NV item
10/14/04    ifk    Added meta info ** to pppi_receive() call
08/18/04    mvl    Changed misspelling in NV item for PPP configuration.
07/16/04    ifk    Added reading of NV_AUTH_REQUIRE_PASSWORD_ENCRYPTION_I
                   nv_item.  Added processing of PPP resync command
04/13/04    ifk    Uncommented setting of IPV6CP config parameters and 
                   featurized under FEATURE_DATA_PS_IPV6
01/30/04    vp     Write back the default values for ppp_config_data to the
                   NV if not already set/unable to read in ppp_task_init()
11/11/03    ifk    Included ps_ppp_auth.h to get the prototype of
                   ppp_auth_set_device_constants()
10/27/03    ifk    Added ppp_task_init()
08/13/03    ifk    PPP cleanup for IPv6.
04/18/03    vsk    enode the right dev & prot fields in 
                   ppp_task_process_inject_pkt()
04/07/03    usb    Added PPP_INJECT_PACKET_CMD processing to replay an out 
                   of order PPP packet for PPP receive processing.
01/27/03    usb    Added PPP_SET_MODE_CMD processing to set PPP mode in PS 
                   context only 
09/23/02    mvl    Modified signal usage.
09/12/02    mvl    removed unused variable. Removed netmodel featurization.
09/02/02    mvl    Removed the PPP rx callback, and registration as this is
                   done in the start command.
08/13/02    mvl    Session handle support.
07/25/02    mvl    Updates for PPP renaming.
07/24/02    mvl    Now ensuring that is PPP mode is NONE discard before
                   sending to any of the PPP unframing functions or input.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Make the file more independent of PS via MACROs defined in
                   header file
04/01/02    mvl    Created Module
===========================================================================*/



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          INCLUDE FILES FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
/*---------------------------------------------------------------------------
  The following include is the task include that specifies the support
  functions PPP uses to set itself up as a service executing in that task
---------------------------------------------------------------------------*/
#include "ps_svc.h"
#include "ps_ppp_defs.h"

/*---------------------------------------------------------------------------
  The general includes
---------------------------------------------------------------------------*/
#include "amssassert.h"
#include "dsm.h"
#include "ps_ppp_task.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi_fsm.h"
#include "ps_ppp.h"
#include "ps_ppp_config.h"
#include "ps_pppi.h"
#include "ps_utils.h"
#include "ps_ppp_lcp.h"
#include "ps_pppi_lcp.h"
#include "ps_ppp_ipcp.h"
#include "ps_pppi_ipcp.h"
#include "ps_ppp_auth.h"
#include "ps_pppi_auth.h"
#include "ps_pppi_vend.h"

#ifdef FEATURE_DATA_PS_IPV6
#include "ps_ppp_ipv6cp.h"
#endif /* FEATURE_DATA_PS_IPV6 */

#include "ds_Utils_DebugMsg.h"
#include "ds_fwki.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         INTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  NV item path & the variable to store JCDMA mode info
---------------------------------------------------------------------------*/
#define PS_PPP_JCDMA_MODE_FILE_NAME  "/nv/item_files/jcdma/jcdma_mode"

static boolean jcdma_mode = FALSE;

#define PS_PPP_TASK_ASSERT(expression)  \
               ps_ppp_task_assert_wrapper(__LINE__, expression)


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
 /*===========================================================================
 
 FUNCTION ps_ppp_task_assert_wrapper
 
 DESCRIPTION
   Wrapper function for PS_PPP_TASK_ASSERT
  
 DEPENDENCIES 
   None
 
 RETURN VALUE
   None
  
 SIDE EFFECTS
 
 ===========================================================================*/
 static void ps_ppp_task_assert_wrapper
 (
   unsigned int     line_num,
   int              expression
 )
 {
   if ( !expression )
   {
     ERR_FATAL("PS_PPP_TASK_ERR_FATAL at line:%d ",  
                                    line_num,0,0);  
   }
 }/* ps_ppp_task_assert_wrapper */

void ppp_task_process_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_resync
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
);

void ppp_task_process_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_abort
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_set_mode
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_inject_pkt
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_vendor_send
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            EXTERNAL DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PPP_TASK_POWERUP_INIT()

DESCRIPTION
  This function does the powerup initialization for the PPP module.  This
  includes registering handlers with PS (the task executing it) for the PPP
  RX signals.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_powerup_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    register the handlers for the various PPP commands that must be handled
    in the task context.
  -------------------------------------------------------------------------*/
  (void)ps_set_cmd_handler(PPP_START_CMD, ppp_task_process_start);
  (void)ps_set_cmd_handler(PPP_STOP_CMD, ppp_task_process_stop);
  (void)ps_set_cmd_handler(PPP_ABORT_CMD, ppp_task_process_abort);
  (void)ps_set_cmd_handler(PPP_SET_MODE_CMD, ppp_task_process_set_mode);
  (void)ps_set_cmd_handler(PPP_INJECT_PKT_CMD, ppp_task_process_inject_pkt);
  
  /*-------------------------------------------------------------------------
    Call the PPP module initialization funtion
  -------------------------------------------------------------------------*/
  pppi_powerup_init();
  
} /* ppp_task_powerup_init() */

/*===========================================================================
FUNCTION  PPP_IS_CURR_MODE_JCDMA

DESCRIPTION   Called to check if it is in JCDMA mode. 

DEPENDENCIES  This parameter will be set based on UIM API

RETURN VALUE  TRUE  - JCDMA mode enabled
              FALSE - JCDMA mode disabled
  
SIDE EFFECTS
===========================================================================*/
boolean ppp_is_curr_mode_jcdma(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    JCDMA mode is enabled if both the JCDMA feature is defined & the NV item 
    is set to TRUE
  -------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_1X
  if(TRUE == jcdma_mode)
  {
    return TRUE;
  }
#endif

  return FALSE;
} /* ppp_is_curr_mode_jcdma() */

/*===========================================================================
FUNCTION PPP_TASK_INIT()

DESCRIPTION
  This function does the startup initialization for the PPP module.  This
  includes configuring the PPP configurable parameters

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_init( void )
{
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  int  init_check = 0;
#endif /* FEATURE_RUIM_FUNC_INTERFACE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ppp_task_nv_init();

  /*-----------------------------------------------------------------------
    Initialize the PPP authentication framework.
  -----------------------------------------------------------------------*/
#ifdef FEATURE_RUIM_FUNC_INTERFACE
  init_check = ppp_auth_init();
  if( 0 != init_check)
  {
    /*("Unable to initialize authentication module for PPP.");*/
    PS_PPP_TASK_ASSERT(0);
    return;
  } /* Unable to initialize authentication framework */
#endif /* FEATURE_RUIM_FUNC_INTERFACE  */

  /* Initialize EAP_AKA, makes usage faster */
#ifdef FEATURE_DATA_PS_PPP_EAP
  eap_aka_powerup_init();  
#endif /*FEATURE_DATA_PS_PPP_EAP*/

  return;
} /* ppp_task_init() */

/*===========================================================================
FUNCTION PPP_TASK_NV_INIT()

DESCRIPTION
  This function configures the PPP configurable parameters

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Sets configurable protocol specific device constants
===========================================================================*/
void ppp_task_nv_init
(
  void
)
{
  ppp_dev_enum_type     device;
  nv_item_type          ppp_nv_item;
  boolean               require_encrypted_passwd = FALSE;
  ppp_ccp_config_e_type ccp_config = PPP_CCP_REJECT;
  ppp_type             *ppp_cb_array = NULL;
  char                  item_file_path[] = PS_PPP_JCDMA_MODE_FILE_NAME;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-----------------------------------------------------------------------
    Configure CCP
  -----------------------------------------------------------------------*/
  if( NV_DONE_S == ds_get_nv_item( NV_PPP_CCP_REJECT_I,
                                   &ppp_nv_item ) )
  {
    ccp_config = (ppp_ccp_config_e_type)(ppp_nv_item.ppp_ccp_reject);
  }
  else
  {
    ppp_nv_item.ppp_ccp_reject = (uint8)ccp_config;
  } /* end else */

  /*-----------------------------------------------------------------------
    Check if encrypted passwords required
  -----------------------------------------------------------------------*/
  if( NV_DONE_S == ds_get_nv_item( NV_AUTH_REQUIRE_PASSWORD_ENCRYPTION_I,
                                   &ppp_nv_item ) )
  {
    require_encrypted_passwd = 
      ppp_nv_item.auth_require_password_encryption;
  }
  else
  {
    ppp_nv_item.auth_require_password_encryption = 
      require_encrypted_passwd;
  } /* end else */

  /*-------------------------------------------------------------------------
    Read the NV item to know if JCDMA feature is enabled or not
  -------------------------------------------------------------------------*/  
  jcdma_mode = FALSE;
  if (0 != ps_read_efs_nv(item_file_path, &jcdma_mode, sizeof(jcdma_mode)))
  {
    LOG_MSG_ERROR_0("Unable to read JCDMA mode NV item, using the default value");
  }

  /*-----------------------------------------------------------------------
    Retrieve PPP_CONFIG_DATA from NV, NV_PPP_CONFIG_DATA_I
  -----------------------------------------------------------------------*/
  for(device = PPP_MIN_DEV; device < PPP_MAX_DEV; device++)
  {
    memset(&ppp_nv_item, 0, sizeof(ppp_nv_item));

    /*---------------------------------------------------------------------
       Specify the index to get the correspoding parameters from NV item
    ---------------------------------------------------------------------*/
    ppp_nv_item.ppp_config_data.index = (uint8)device;

    if( NV_DONE_S != ds_get_nv_item( NV_PPP_CONFIG_DATA_I, &ppp_nv_item ) )
    {
      /*---------------------------------------------------------------------
        Set the NV item to default values for all its components if unable
        to read from NV_PPP_CONFIG_DATA_I
      ---------------------------------------------------------------------*/
      if(TRUE == ppp_is_curr_mode_jcdma())
      {
        ppp_nv_item.ppp_config_data.lcp_term_timeout = LCP_TERM_TO;
      }
      else
      {
        ppp_nv_item.ppp_config_data.lcp_term_timeout = LCP_TIMEOUT *
                                                       LCP_TERM_MULT;
      }
  
      ppp_nv_item.ppp_config_data.lcp_ack_timeout  = LCP_TIMEOUT;
      ppp_nv_item.ppp_config_data.lcp_req_try      = LCP_REQ_TRY;
      ppp_nv_item.ppp_config_data.lcp_nak_try      = LCP_NAK_TRY;
      ppp_nv_item.ppp_config_data.lcp_term_try     = LCP_TERM_TRY;
  
      ppp_nv_item.ppp_config_data.auth_retry   = PPP_AUTH_RETRY_COUNTER_VAL;
      ppp_nv_item.ppp_config_data.auth_timeout = PPP_AUTH_TIMER_VAL;
  
      ppp_nv_item.ppp_config_data.ipcp_term_timeout       = IPCP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipcp_ack_timeout        = IPCP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipcp_req_try            = IPCP_REQ_TRY;
      ppp_nv_item.ppp_config_data.ipcp_nak_try            = IPCP_NAK_TRY;
      ppp_nv_item.ppp_config_data.ipcp_term_try           = IPCP_TERM_TRY;
      ppp_nv_item.ppp_config_data.ipcp_compression_enable = 
          (uint8)PPP_NCP_COMPRESS_IGNORE;
  
      ppp_nv_item.ppp_config_data.ipv6cp_term_timeout     = IPV6CP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipv6cp_ack_timeout      = IPV6CP_TIMEOUT;
      ppp_nv_item.ppp_config_data.ipv6cp_req_try            = IPV6CP_REQ_TRY;
      ppp_nv_item.ppp_config_data.ipv6cp_nak_try            = IPV6CP_NAK_TRY;
      ppp_nv_item.ppp_config_data.ipv6cp_term_try         = IPV6CP_TERM_TRY;
      ppp_nv_item.ppp_config_data.ipv6cp_compression_enable =
          (uint8)PPP_NCP_COMPRESS_IGNORE;
    }

    /*-----------------------------------------------------------------------
      Initialize LCP and IPCP device constants
    -----------------------------------------------------------------------*/
    lcp_set_device_constants(device, &ppp_nv_item.ppp_config_data);
    ipcp_set_device_constants(device, &ppp_nv_item.ppp_config_data);
#ifdef FEATURE_DATA_PS_IPV6
    ipv6cp_set_device_constants(device, &ppp_nv_item.ppp_config_data);
#endif /* FEATURE_DATA_PS_IPV6 */
    ppp_auth_set_device_constants(device, &ppp_nv_item.ppp_config_data);

    /*-----------------------------------------------------------------------
      Populate the device constants structure from the NV item
    -----------------------------------------------------------------------*/
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_LCP_ACK_TIMEOUT,
                               ppp_nv_item.ppp_config_data.lcp_ack_timeout))
    {
        /* "ppp_task_init: Failed to set default constant values"
        "  for device <device> constant <DEFAULT_LCP_ACK_TIMEOUT>"*/
        DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,DEFAULT_LCP_ACK_TIMEOUT);
        PS_PPP_TASK_ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_LCP_REQUEST_TRY,
                                   ppp_nv_item.ppp_config_data.lcp_req_try))
    {
        /*"ppp_task_init: Failed to set default constant values"
          "  for device <device> constant <DEFAULT_LCP_REQUEST_TRY>"*/
        DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,
                               DEFAULT_LCP_ACK_TIMEOUT);
        PS_PPP_TASK_ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_IPCP_ACK_TIMEOUT,
                              ppp_nv_item.ppp_config_data.ipcp_ack_timeout))
    {
        /*"ppp_task_init: Failed to set default constant values"
          "  for device <device> constant <DEFAULT_IPCP_ACK_TIMEOUT>" */
        DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,
                               DEFAULT_IPCP_ACK_TIMEOUT);
        PS_PPP_TASK_ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_IPCP_REQUEST_TRY,
                                   ppp_nv_item.ppp_config_data.ipcp_req_try))
    {
        /*"ppp_task_init: Failed to set default constant values"
          " for device <device> constant <DEFAULT_IPCP_REQUEST_TRY>"*/
        DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,
                               DEFAULT_IPCP_REQUEST_TRY);
        PS_PPP_TASK_ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_AUTH_TIMEOUT,
                                   ppp_nv_item.ppp_config_data.auth_timeout))
    {
      /* "ppp_task_init: Failed to set default constant values"
         "  for device <device> constant <DEFAULT_AUTH_TIMEOUT>"*/
      DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,
                             DEFAULT_AUTH_TIMEOUT);
      PS_PPP_TASK_ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                   DEFAULT_AUTH_RETRY,
                                   ppp_nv_item.ppp_config_data.auth_retry))
    {
      /*"ppp_task_init: Failed to set default constant values"
         "  for device <device> constant <DEFAULT_AUTH_RETRY>"*/
      DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,DEFAULT_AUTH_RETRY);
      PS_PPP_TASK_ASSERT(0);
      return;
    }

#ifdef FEATURE_DATA_PS_IPV6
    if(0 > ppp_set_device_constant(
               device,
               DEFAULT_IPV6CP_ACK_TIMEOUT,
               ppp_nv_item.ppp_config_data.ipv6cp_ack_timeout)
    )
    {
        /* "ppp_task_init: Failed to set default constant values"
        "  for device <device> constant <DEFAULT_IPV6CP_ACK_TIMEOUT>" */
        DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,
                               DEFAULT_IPV6CP_ACK_TIMEOUT);
        PS_PPP_TASK_ASSERT(0);
      return;
    }
    if(0 > ppp_set_device_constant(device, 
                                DEFAULT_IPV6CP_REQUEST_TRY,
                                ppp_nv_item.ppp_config_data.ipv6cp_req_try))
    {
        /* "ppp_task_init: Failed to set default constant values"
         "  for device <device> constant <DEFAULT_IPV6CP_REQUEST_TRY>"*/
        DATA_PROTO_ERROR_LOG_2(PS_BAD_ARG_OR_NULL_PTR,device,
                               DEFAULT_IPV6CP_REQUEST_TRY);
        PS_PPP_TASK_ASSERT(0);
      return;
    }
#endif /* FEATURE_DATA_PS_IPV6 */

    /*-----------------------------------------------------------------------
      Initialize password encryption
    -----------------------------------------------------------------------*/
    ppp_cb_array = fsm_get_ppp_cb_array();
    ppp_cb_array[device].auth.require_encrypted_passwd = 
      require_encrypted_passwd;

    /*-----------------------------------------------------------------------
        Initialize CCP
    -----------------------------------------------------------------------*/
    ppp_cb_array[device].ccp_config = ccp_config;
  } /* for(all PPP devices) */
} /* ppp_task_nv_init() */

/*===========================================================================
FUNCTION PPP_TASK_PROCESS_START()

DESCRIPTION
  This function handles the ppp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_start
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_START_CMD)
  {
    PS_PPP_TASK_ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing START cmd");
  ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(),"ppp_task_process_start(): "
                   "Configbuf_ptr 0x%x recieved.", 
                   data_ptr);
  pppi_start((ppp_config_buffer_s_type *)data_ptr);

} /* ppp_task_process_start() */

/*===========================================================================
FUNCTION PPP_TASK_PROCESS_ABORT()

DESCRIPTION
  This function handles the ppp_abort command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_abort
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  pppi_abort_info_type abort_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_ABORT_CMD)
  {
    /*("Invalid Arguments");*/
    PS_PPP_TASK_ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing ABORT cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_abort()
    abort_info:
      device         : extract device from high 16-bits 
      session_handle : extract session_handle from low 16-bits 
  -------------------------------------------------------------------------*/
  abort_info.device         = 
    (uint16)(((uint32)(data_ptr) & 0xffff0000) >> 16 ); 
  abort_info.session_handle = (uint16)((uint32)(data_ptr) & 0x0000ffff);

  pppi_abort(abort_info);

} /* ppp_task_process_abort() */



/*===========================================================================
FUNCTION PPP_TASK_PROCESS_STOP()

DESCRIPTION
  This function handles the ppp_close command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_stop
(
  ps_cmd_enum_type  cmd,
  void             *data_ptr
)
{
  pppi_stop_info_type stop_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_STOP_CMD)
  {
    /*("Invalid Arguments");*/
    PS_PPP_TASK_ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing STOP cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_stop()
    stop_info:
      device:   extract device from high 16-bits 
      protocol: extract protocol from low 16-bits 
  -------------------------------------------------------------------------*/
  stop_info.device = 
    (uint16)(((uint32)(data_ptr)& 0xffff0000) >> 16 );
  stop_info.protocol = (uint16)((uint32)(data_ptr)&(0x0000ffff));
  pppi_stop(stop_info);

} /* ppp_task_process_stop() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_SET_MODE()

DESCRIPTION
  This function handles the ppp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_set_mode
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  pppi_mode_info_type mode_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_SET_MODE_CMD)
  {
    /*("Invalid Arguments");*/
    PS_PPP_TASK_ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing SET_MODE cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_set_mode() 
    mode_info:
      device : extract device from high 16-bits 
      mode   : extract mode from low 16-bits 
  -------------------------------------------------------------------------*/
  mode_info.device   = 
    (uint16)(((uint32)(data_ptr) & 0xffff0000) >> 16 );
  mode_info.mode = (uint16)((uint32)(data_ptr) & 0x0000ffff);

  pppi_set_mode(mode_info);

} /* ppp_task_process_set_mode() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_INJECT_PKT()

DESCRIPTION
  This function handles the ppp_inject_pkt command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_inject_pkt
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  ppp_dev_enum_type device;
  uint16 protocol;
  dsm_item_type *pkt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_INJECT_PKT_CMD)
  {
    PS_PPP_TASK_ASSERT(0);
    return;
  }

  LOG_MSG_INFO2_0("PPP processing INJECT_PKT cmd");
  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_receive()
  -------------------------------------------------------------------------*/
  pkt_ptr = (dsm_item_type*) data_ptr;
  device = (ppp_dev_enum_type)(pkt_ptr->app_field & 0xFFFF);
  protocol = (uint16)((pkt_ptr->app_field >> 16) & 0xFFFF);

  pppi_receive(device, protocol, &pkt_ptr, NULL);

} /* ppp_task_process_inject_pkt() */

