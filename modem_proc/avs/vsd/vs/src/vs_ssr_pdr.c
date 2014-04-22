/**
  @file vs_ssr_pdr.c
  @brief This file contains the definitions of the public interfaces of VS.
  This file also includes some common function defintions.

*/

/* 
   Copyright (C) 2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_ssr_pdr.c#1 $
   $Author: pwbldsvc $
*/

/******************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#if ( !defined AVS_MPSS_TEST )
#include "rcinit.h"
#include "sys_m_messages.h"
#include "rcecb.h"

#ifdef SERVREG_STUB
#include "vs_servreg_stub.h"
#else 

#include "servreg_common.h"
#include "qurt_anysignal.h"
#include "servreg_locator_qurt.h"
#include "servreg_monitor.h"
#include "servreg_monitor_qurt.h"

#endif //SERVREG_STUB

#endif

#include "vs_task.h"
#include "vs_i.h"


/****************************************************************************
 * DEFINE
 ****************************************************************************/

/* CVD remote service status signal mask */
#define CVD_REMOTE_SERVICE_STATUS_MASK  ( 0x2 )
/* CVD remote service status signal mask */
#define SERVREG_LOCATOR_STATUS_MASK ( 0x1 )

/****************************************************************************
 * EXTERN variables across VS should be defined here.
 ****************************************************************************/

extern apr_event_t vs_thread_control_event;

#if ( !defined AVS_MPSS_TEST )
/****************************************************************************
 * STATIC variables
 ****************************************************************************/
static qurt_anysignal_t servreg_pdr_cvd_status_signal;
static qurt_anysignal_t servreg_locator_avail_signal;
static SERVREG_DL_HANDLE remote_cvd_mon_handle = NULL;

/* CVD service name define */
static char vs_audio_svc_name[32] = "avs/audio";
apr_thread_t vs_pdr_handle;
vs_thread_state_enum_t vs_pdr_state = VS_THREAD_STATE_ENUM_INIT;
static uint8_t vs_pdr_task_stack[ VS_PDR_TASK_STACK_SIZE ];

bool_t vs_pdr_fwk_is_available = FALSE;

bool_t vs_registered_with_pdr = FALSE;

bool_t vs_registered_with_ssr = FALSE;

/* flag that indicates the state of servreg locator service (MPSS/APSS).
 * audio service up/down notifications would be received only when 
 * servreg locator service is up.
 */
SERVREG_SERVICE_STATE vs_servreg_locator_service_state = SERVREG_SERVICE_STATE_UNINIT;

/* Number of possible configurations for audio services (PD) across MSM,
 * MDM, and fusion architectures.
 */
#define VS_NO_OF_AUDIO_SERVICE_CONFIGS (3)

/* Preference order of audio services for VS to register in MDM/Fusion. 
 * - Scope of VS is MDM.
 * - service present at index 0 has highest priority and index 2 has lowest priority.
 * - VS will register/listen only for the first matched service 
 */
SERVREG_NAME vs_audio_svc_preferences_mdm[VS_NO_OF_AUDIO_SERVICE_CONFIGS] = { 
                                      "mdm/modem/root_pd",
                                      "mdm/modem/audio_pd",
                                      "msm/adsp/audio_pd"
                                   };

/* Preference order of audio services for VS to register in MSM/Fusion. 
 * - Scope of VS is MSM.
 * - service present at index 0 has highest priority and index 2 has lowest priority.
 * - VS will register/listen only for the first matched service 
 */
SERVREG_NAME vs_audio_svc_preferences_msm[VS_NO_OF_AUDIO_SERVICE_CONFIGS] = { 
                                      "msm/adsp/audio_pd",
                                      "mdm/modem/root_pd",
                                      "mdm/modem/audio_pd"
                                   };

/****************************************************************************
 * STATIC FUNCTIONS
 ****************************************************************************/
static void vs_pdr_adsp_before_shutdown_handler ( void )
{
  uint32_t rc = VS_EOK;
  SERVREG_RESULT ret = VS_EOK;
  uint32_t transaction_id = 0;

  /* For hanlding Back to Back SSR we should make sure to skip it if the
   * first one is still not processed. */
  if ( vs_cvd_cleanup_is_done == FALSE )
  {
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VS: ignoring B2B PDR DOWN noifications!!" );
    return;
  }

  vs_cvd_cleanup_is_done = FALSE;
  vs_cvd_service_is_up = FALSE;
  vs_adsp_timeout_is_triggered = FALSE;

  transaction_id = servreg_get_transaction_id( remote_cvd_mon_handle );
  ret = servreg_set_ack( remote_cvd_mon_handle, transaction_id );
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS: AUDIO_PDR_DOWN_HANDLING: transactionId[%d], acknowledgement status[0x%08x]",
         transaction_id, ret );

  rc = vs_prepare_and_dispatch_cmd_packet( VS_VOC_CMDI_CLEANUP_CVD_REFERENCES, NULL, 0 );
  VS_REPORT_FATAL_ON_ERROR ( rc );
  return;
}

static void vs_pdr_adsp_after_powerup_handler ( void )
{
  uint32_t transaction_id = 0;
  SERVREG_RESULT ret;

  /**
   * SSR UP event does not requires to restore the vocoder state of each VS
   * session as per the call state as of now.
   *
   * NOTE: This would be required when voice call continuity feature needs to be
   * supported and target supports PD-Restart and APR Service UP/DOWN events.
   *
   * Indicates CVD Services are actually up post PDR hence made TRUE
   *
   */
  vs_cvd_service_is_up = TRUE;

  transaction_id = servreg_get_transaction_id(remote_cvd_mon_handle);
  ret = servreg_set_ack(remote_cvd_mon_handle, transaction_id);
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS: AUDIO_PDR_UP_HANDLING: transactionId[%d], acknowledgement status[0x%08x]",
         transaction_id, ret );

  return;
}

VS_INTERNAL uint32_t vs_pdr_init ( void )
{
  SERVREG_DL_HANDLE pdr_handle = NULL;
  SERVREG_NAME domain = SERVREG_NULL;
  SERVREG_NAME temp_domain = SERVREG_NULL;
  SERVREG_NAME soc_scope = SERVREG_NULL;
  SERVREG_RESULT result;
  uint32_t domain_index = 0;
  uint32_t domain_count = 0;
  bool_t expected_domain_found = FALSE;
  uint32_t no_of_domains = 0;
  

  /* 1. Assigning domain list handle. 
   * 2. handle is needed to query the list of domains in which a given service resides.
   * 3. Once VS client notices its intended domain, it can continue to register as listener and
   *    free up this handle. the  client handle would be used until VS client de-registers
   *
   * Valid scenarios:
   *   1. If the local soc scope is MSM, then VS registers to monitor the status of avs/audio in userPD of ADSP.
   *   2. If the local soc scope is MDM, then VS registers to monitor the status of avs/audio in rootPD of MDSP - 1st priority (OR)
   *                                          VS registers to monitor the status of avs/audio in userPD of MDSP - 2nd priority (OR)
   *                                          VS registers to monitor the status of avs/audio in userPD of ADSP - 3rd priority
   *
   */
  pdr_handle = servreg_alloc_domainlist_handle();
  if( SERVREG_NULL == pdr_handle )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
         "VS: PDR_INIT: servreg_alloc_domainlist_handle() failed \n" );
    return VS_EFAILED;
  }

  result = servreg_get_domainlist( vs_audio_svc_name, pdr_handle );
  if( SERVREG_SUCCESS != result ) 
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VS: PDR_INIT: servreg_get_domainlist() failed" );
    result = servreg_free_domainlist_handle( pdr_handle );
    return VS_EFAILED;
  }

  /* soc_scope can be either msm or mdm */
  soc_scope = servreg_get_local_soc_scope();
  /* domain can be either modem or adsp, and subdomain can be either root_pd or audio_pd */
  no_of_domains = servreg_get_num_entries(pdr_handle);

  if ( 0 == strncmp( "msm", soc_scope, strlen("msm") ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vs_pdr_init(): soc_scope is MSM; no_of_domains that has audio service present = %d", no_of_domains );

    for ( domain_index = 0; domain_index < VS_NO_OF_AUDIO_SERVICE_CONFIGS; domain_index++)
    {
      temp_domain = vs_audio_svc_preferences_msm[domain_index];
      for ( domain_count = 1; ( domain_count <= no_of_domains) && (!expected_domain_found);  domain_count++ )
      {
        result = servreg_get_entry(pdr_handle, domain_count, &domain);
        if ( ( SERVREG_SUCCESS != result ) || ( SERVREG_NULL == domain ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VS: PDR_INIT: servreg_get_entry() failed \n" );
          return VS_EFAILED;
        }
        if( 0 == (strncmp(temp_domain, domain, strlen(temp_domain))) )
        {
          expected_domain_found = TRUE;
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "scope:MSM; domain found; sub-domain:%d", domain_index);  
          break;
        }
      }
    }

  }
  else if ( 0 == strncmp("mdm", soc_scope, strlen("mdm") ) )
  {

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "vs_pdr_init(): soc_scope is MDM; no_of_domains that has audio service present = %d", no_of_domains );

    for ( domain_index = 0; domain_index < VS_NO_OF_AUDIO_SERVICE_CONFIGS; domain_index++)
    {
      temp_domain = vs_audio_svc_preferences_mdm[domain_index];
      for ( domain_count = 1; ( domain_count <= no_of_domains) && (!expected_domain_found) ;  domain_count++ )
      {
        result = servreg_get_entry(pdr_handle, domain_count, &domain);
        if ( ( SERVREG_SUCCESS != result ) || ( SERVREG_NULL == domain ) )
        {
          MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VS: PDR_INIT: servreg_get_entry() failed \n" );
          return VS_EFAILED;
        }
        if( 0 == (strncmp(temp_domain, domain, strlen(temp_domain))) )
        {
          expected_domain_found = TRUE;
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "scope:MDM; domain found; sub-domain:%d", domain_index);  
          break;
        }
      }
    }

  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "vs_pdr_init(): soc_scope is neither MSM nor MDM" );
    return VS_EFAILED;
  }

  if ( FALSE == expected_domain_found )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS: PDR REGISTRATION failed to get expected domain.", servreg_get_num_entries( pdr_handle ) );
    result = servreg_free_domainlist_handle( pdr_handle );
    return VS_EFAILED;
  }

  remote_cvd_mon_handle = servreg_alloc_monitor_handle(domain, vs_audio_svc_name);
  if ( SERVREG_NULL == remote_cvd_mon_handle )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VS: PDR_INIT: servreg_alloc_monitor_handle failed" );
    result = servreg_free_domainlist_handle( pdr_handle );
    return VS_EFAILED;
  }

  /* Free the domain list handle once monitor allocation is done */
  result = servreg_free_domainlist_handle( pdr_handle );
  if(SERVREG_SUCCESS != result )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VS: PDR_INIT: pdr_handle free failed after finding expected domain" );
    return VS_EFAILED;
  }

  result = servreg_register_listener_qurt( remote_cvd_mon_handle, &servreg_pdr_cvd_status_signal,
                                           CVD_REMOTE_SERVICE_STATUS_MASK );
  if( SERVREG_SUCCESS != result )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VS: PDR_INIT: Client registration with servreg failed via QDI." );
    result = servreg_free_monitor_handle( remote_cvd_mon_handle );
    return VS_EFAILED;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "VS: PDR_INIT: Client registeration successful with servreg." );
  vs_registered_with_pdr = TRUE;

  return VS_EOK;
}


VS_INTERNAL uint32_t vs_pdr_deinit ( void )
{
  uint32_t result = VS_EOK;

  if (NULL != remote_cvd_mon_handle)
  { 
    /* Unregister for service register PDR signal. */
    SERVREG_RESULT ret;

    ret = servreg_deregister_listener_qurt( remote_cvd_mon_handle, &servreg_pdr_cvd_status_signal,
                                            CVD_REMOTE_SERVICE_STATUS_MASK );
    if ( SERVREG_SUCCESS != ret )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: PDR_DEINIT: servreg_deregister_listener_qurt() failed", ret );
      result = VS_EFAILED;
    }

    ret = servreg_free_monitor_handle( remote_cvd_mon_handle );
    if( SERVREG_SUCCESS != ret )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS: PDR_DEINIT: could not free the monitor handle" );
      result = VS_EFAILED;
    }
  }

  qurt_anysignal_destroy(&servreg_locator_avail_signal);
  qurt_anysignal_destroy(&servreg_pdr_cvd_status_signal);

  vs_pdr_fwk_is_available = FALSE;
  vs_registered_with_pdr = FALSE;

  return result;
}

static int32_t vs_pdr (
  void* param
)
{
  uint32_t result = VS_EFAILED; 
  uint32_t signal = 0;
  uint32_t signal_wait_mask = SERVREG_LOCATOR_STATUS_MASK;
  SERVREG_SERVICE_STATE servreg_state = SERVREG_SERVICE_STATE_UNINIT;
  SERVREG_SERVICE_STATE cvd_state = SERVREG_SERVICE_STATE_UNINIT;

  vs_pdr_state = VS_THREAD_STATE_ENUM_READY;
  apr_event_signal( vs_thread_control_event );
  servreg_state = servreg_get_servloc_availability();
  vs_servreg_locator_service_state = servreg_state;

  do
  {
    if( SERVREG_SERVICE_STATE_UP == servreg_state )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_pdr():service locator state [0x%08x] UP", servreg_state);
      vs_servreg_locator_service_state = servreg_state;
      result = vs_pdr_init();
      if( result )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VS: PDR registartion failed:: %d", result );
        /* (void)vs_voc_ssr_init(); */
      }
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_pdr(): service locator state [0x%08x] not UP, waiting for signal[0x%08x]",
           servreg_state, signal_wait_mask );

    signal = (uint32_t) qurt_anysignal_wait( &servreg_locator_avail_signal, signal_wait_mask );
    if ( SERVREG_LOCATOR_STATUS_MASK & signal )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_pdr():service registry available. signal[0x%08x]", signal);
      qurt_anysignal_clear(&servreg_locator_avail_signal,signal_wait_mask);
      servreg_state = servreg_get_servloc_availability();
      vs_servreg_locator_service_state = servreg_state;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_pdr():servreg locator service available. signal[0x%08x], state [0x%08x]",
              signal, servreg_state );
    }
  } while ( TRUE );

  if( FALSE == vs_registered_with_pdr )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS: PDR registration failed, bailing out!!!" );
    vs_pdr_state = VS_THREAD_STATE_ENUM_EXIT;
    return VS_EOK;
  }

  if ( SERVREG_NULL != remote_cvd_mon_handle )
  {
    cvd_state = servreg_get_service_curr_state( remote_cvd_mon_handle );
  }

  switch (cvd_state)
  {
    case SERVREG_SERVICE_STATE_UP:
      vs_cvd_service_is_up = TRUE;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "PDR registartion succesfull, curent audio service state is UP" );
      break;

    case SERVREG_SERVICE_STATE_DOWN:
      vs_cvd_service_is_up = FALSE;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "PDR registartion succesfull, curent audio service state is DOWN" );
      break;

    case SERVREG_SERVICE_STATE_UNINIT:
    default: 
      vs_cvd_service_is_up = FALSE;
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, 
             "PDR registartion succesfull, curent audio service state is UNINITIALIZED");
      break;
  }

  for (;;)
  {
    signal_wait_mask = CVD_REMOTE_SERVICE_STATUS_MASK;
    signal = ( uint32_t )qurt_anysignal_wait( &servreg_pdr_cvd_status_signal, signal_wait_mask );

    if ( CVD_REMOTE_SERVICE_STATUS_MASK & signal )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "VS: service registry notified audio PD status" );
      qurt_anysignal_clear(&servreg_pdr_cvd_status_signal,CVD_REMOTE_SERVICE_STATUS_MASK);

      cvd_state = servreg_get_service_curr_state(remote_cvd_mon_handle);
      switch (cvd_state)
      {
        case SERVREG_SERVICE_STATE_DOWN:
          MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: recieved SERVREG_SERVICE_STATE_DOWN for CVD. " );
          vs_pdr_adsp_before_shutdown_handler();
          break;

        case SERVREG_SERVICE_STATE_UP:
          MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: recieved SERVREG_SERVICE_STATE_UP for CVD. " );
          vs_pdr_adsp_after_powerup_handler();
          break;

        default:
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                  "VS: recieved invalid state[0x%08x] for CVD", cvd_state );
          break;
      }
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: servreg notified wrong signal[0x%08x], bailing out!!",signal );
      break;
    }
  }

  vs_pdr_state = VS_THREAD_STATE_ENUM_EXIT;
  return VS_EOK;
}

static void vs_ssr_adsp_after_powerup_handler (
  void
)
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "vs_voc_ssr_adsp_after_powerup_handler is called." );

  /**
   * SSR UP event does not requires to restore the vocoder state of each VS
   * session as per the call state as of now.
   *
   * NOTE: This would be required when voice call continuity feature needs to be
   * supported and target supports PD-Restart and APR Service UP/DOWN events.
   *
   * For Target that do not have Multi-PD, there is no method to
   * detect if CVD Services are actually up post SSR.
   *
   * this variable is made default TRUE post SSR cleanup.
   */
  vs_cvd_service_is_up = TRUE;

  return;
}

static void vs_ssr_adsp_before_shutdown_handler (
  void
)
{
  uint32_t rc = VS_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "VS: handler for ssr before adsp shutdown called." );

  /* For hanlding Back to Back SSR we should make sure to skip 
   * it if the first one is still not processed. */
  if ( vs_cvd_cleanup_is_done == FALSE )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VS: ignoring B2B SSR DOWN noifications!!" );
    return;
  }

  vs_cvd_cleanup_is_done = FALSE;
  vs_cvd_service_is_up = FALSE;
  vs_adsp_timeout_is_triggered = FALSE;

  rc = vs_prepare_and_dispatch_cmd_packet( VS_VOC_CMDI_CLEANUP_CVD_REFERENCES, NULL, 0 );
  VS_REPORT_FATAL_ON_ERROR ( rc );
  return;
}

static uint32_t vs_ssr_init (
  void
)
{
  RCECB_HANDLE ssr_handle = NULL;

  if( vs_registered_with_ssr )
  {
    return VS_EOK;
  }

  ssr_handle = rcecb_register_name( SYS_M_SSR_ADSP_BEFORE_SHUTDOWN,
                                    vs_ssr_adsp_before_shutdown_handler );
  if ( NULL == ssr_handle )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VS: SSR_INIT: Failed to register for SYS_M_SSR_ADSP_BEFORE_SHUTDOWN callback." );
  }

  ssr_handle = rcecb_register_name( SYS_M_SSR_ADSP_AFTER_POWERUP,
                                    vs_ssr_adsp_after_powerup_handler );
  if ( NULL == ssr_handle )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "VS: SSR_INIT: Failed to register for SYS_M_SSR_ADSP_AFTER_POWERUP callback." );
  }

  vs_registered_with_ssr = TRUE;
  vs_cvd_cleanup_is_done = TRUE;  
  /* service registry or system monitor won't explicitly notify 
   * audio PD (ADSP) up in SSR framework. So, setting the flag explicitly
   */
  vs_cvd_service_is_up = TRUE;

  return VS_EOK;
}

static uint32_t vs_ssr_deinit (
  void
)
{
  if( TRUE == vs_registered_with_ssr)
  {
    ( void ) rcecb_unregister_name( SYS_M_SSR_ADSP_AFTER_POWERUP,
                                    vs_ssr_adsp_after_powerup_handler );
    ( void ) rcecb_unregister_name( SYS_M_SSR_ADSP_BEFORE_SHUTDOWN,
                                    vs_ssr_adsp_before_shutdown_handler );
  }

  vs_registered_with_ssr = FALSE;
  vs_cvd_cleanup_is_done = FALSE;  
  vs_cvd_service_is_up = FALSE;

  return VS_EOK;
}

uint32_t vs_ssr_pdr_deinit (
  void
)
{
  uint32_t result = VS_EOK;

  ( void ) vs_ssr_deinit();
  ( void ) vs_pdr_deinit();

  return result;
}

uint32_t vs_ssr_pdr_init (
  void
)
{
  int32_t rc = VS_EOK;
  SERVREG_RESULT result;
  RCINIT_INFO info_handle = NULL;
  RCINIT_PRIO priority = 0;
  unsigned long stack_size = 0;

  /* Register for service registry PDR signals. */
  qurt_anysignal_init ( &servreg_pdr_cvd_status_signal );
  qurt_anysignal_init ( &servreg_locator_avail_signal );

  /* register with service registry. 
   * We will get notified when service registry locator is UP. */
  result = servreg_register_servloc_availability_signal_qurt(
             &servreg_locator_avail_signal, SERVREG_LOCATOR_STATUS_MASK );
  if(SERVREG_SUCCESS != result)
  {
    vs_pdr_fwk_is_available = FALSE;
    MSG_1 ( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
            "VS: SSR_PDR_INIT: service locator registration failed[0x%08x], "
            "registring for SSR", result );
    ( void ) vs_ssr_init( );
  }
  else
  {
    vs_pdr_fwk_is_available = TRUE;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS: SSR_PDR_INIT:service locator registration passed[0x%08x]\n", result );

    info_handle = rcinit_lookup( VS_PDR_TASK_NAME );
    if( info_handle == NULL )
    {
      /* Use the default priority & stack_size*/
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "vs_init(): VS_PDR task not registered with RCINIT" );
      priority = VS_PDR_TASK_PRIORITY;
      stack_size = VS_PDR_TASK_STACK_SIZE;
    }
    else
    {
      priority = rcinit_lookup_prio_info( info_handle );
      stack_size = rcinit_lookup_stksz_info( info_handle );
    }

    if( ( priority > 255 ) || ( stack_size == 0 ) )
    {
      ERR_FATAL( "vs_init(): Invalid priority: %d or stack size: %d",
                 priority, stack_size, 0 );
    }

    rc = apr_thread_create( &vs_pdr_handle, VS_PDR_TASK_NAME, TASK_PRIORITY( priority ),
                            vs_pdr_task_stack, stack_size, vs_pdr, NULL );
    VS_PANIC_ON_ERROR( rc );

    /* Wait for service thread to be created. */
    ( void ) apr_event_wait( vs_thread_control_event );
  }

  vs_cvd_cleanup_is_done = TRUE;

  return rc;
}

#else /* ( !defined AVS_MPSS_TEST ) */

uint32_t vs_ssr_pdr_init (
  void
)
{
  int32_t rc = VS_EOK;

  vs_cvd_service_is_up = TRUE;
  vs_cvd_cleanup_is_done = TRUE;

  return rc;
}

uint32_t vs_ssr_pdr_deinit (
  void
)
{
  return VS_EOK;
}

#endif /* ( !defined AVS_MPSS_TEST ) */

