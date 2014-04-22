/*-----------------------------------------------------------------------------
   Copyright (C) 2015-2017 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/audio.tx/2.0/audio/csdlite/src/csdlite.c#2 $
  $DateTime: 2019/05/14 21:39:07 $
  $Author: pwbldsvc $
  $Change: 19192970 $
  $Revision: #2 $
  =========================================================================*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
       when        who       what, where, why
      --------    -----      ---------------------------------------------------
      02/01/17     jk        add fix for multiple config calls
      11/15/16     rk        abstracting OSAL layer
      07/10/15     rp        fixed the adversary use cases issues
      04/20/15     jk        creation 
  =========================================================================*/

#include "csdlite.h"
#include "csdlite_private.h"
#include "csd.h"
#include "csd_vm.h"
#include "csd_dev_ioctl.h"
#include "csd_vc_ioctl.h"
#include "csd_vs_ioctl.h"
#include "csd_vm_ioctl.h"
#include "csd_debug_msg.h"
#include "apr_errcodes.h"
#include "csd_os_dependencies.h"
/* global static state vars*/

#define CSDLITE_DRIVER_VER ((CSDLITE_VER_MAIN<<24)+(CSDLITE_VER_MINOR<<16)+CSDLITE_VER_REV)

static csdlite_dev_sess_t csdlite_dev_session;
static csdlite_voice_sess_t csdlite_voice_session;
static csdlite_dtmf_sess_t csdlite_dtmf_session;
static csdlite_afe_loopback_state_t csdlite_afe_loopback_state;
static csdlite_feature_cache_t csdlite_feature_cache;
static uint32_t csdlite_version = CSDLITE_DRIVER_VER;

static csd_event_t csdlite_event = NULL;
static csd_timer_t csdlite_dtmf_gen_timer = NULL;
static csd_lock_t csdlite_lock = NULL;
static bool_t csdlite_inited = FALSE;

/*thread info*/
#define CSDLITE_TASK_PRIORITY ( 212 )
#define CSDLITE_THREAD_STACK_SIZE 8192
static char_t csdlite_thread_name[] = "CSDLITE";
static csd_thread_t csdlite_thread;
static csd_event_t csdlite_event_handle = NULL;      /* Event management */
static csd_event_t csdlite_event_init = NULL;      
static bool_t csdlite_keep_thread_alive = FALSE;

static csdlite_cmd_node_t csdlite_cmd_pool[ CSDLITE_NUM_CMD ];
static apr_list_t csdlite_free_cmd_list;
static apr_list_t csdlite_active_cmd_list;

static void csdlite_task_lock_fn ( void )
{
  ( void ) csd_lock_enter( csdlite_lock );
}

static void csdlite_task_unlock_fn ( void )
{
  ( void ) csd_lock_leave( csdlite_lock );
}

static int32_t csdlite_service_task( void* param) 
{
  uint32_t apr_rc = APR_EOK;

  //signal parent thread it can proceed
  if( csdlite_event_init )
  {
    csd_event_signal(csdlite_event_init);
  }
  else
  {
    apr_rc = APR_ENOTREADY;
    CSD_MSG(CSD_PRIO_ERROR,"csdlite_service_task: apr_event : csdlite_event_init"
            "not ready apr_rc[0x%x]", apr_rc );
  }
  if( apr_rc == APR_EOK && csdlite_event_handle == NULL )
  { //only create the event if there was no event before.
    apr_rc = csd_event_create( &csdlite_event_handle );
  }
  if ( apr_rc != APR_EOK )
  {
    CSD_MSG(CSD_PRIO_ERROR,"Failed to create event apr_rc[0x%x]", apr_rc );
  }

  if( apr_rc == APR_EOK )
  {
    do
    {
      apr_rc = csd_event_wait( csdlite_event_handle );
      if( apr_rc != APR_EOK && apr_rc != APR_ECONTINUE )
      {
        CSD_MSG( CSD_PRIO_ERROR,
                 "Event wait failed, apr_rc[0x%x]",
                 apr_rc );
        break;
      }
      else if ( apr_rc != APR_ECONTINUE )
      {
        csdlite_cmd_node_t* cmd;
        while( !( apr_list_remove_head( &csdlite_active_cmd_list, ( apr_list_node_t** )&cmd ) ) )
        {
          if( cmd )
          {
            switch( cmd->cmd )
            {
              case CSDLITE_CMD_DISABLE_DEV:
                CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_service_task: device disabling due to no activity");
                csdlite_disable_device();
              break;
            }

            ( void ) apr_list_add_tail ( &csdlite_free_cmd_list, &cmd->link );
          } 
        }
      }

    }while ( csdlite_keep_thread_alive );
  }
  return apr_rc;
}

uint32_t csdlite_init( void )
{
  uint32_t apr_rc = APR_EOK;
  uint32_t checkpoint = 0;
  uint32_t i = 0;

  if ( csdlite_inited )
  {
    apr_rc = CSD_EALREADY;
  }
  else
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_init");

    /*thread creation*/
    if ( csdlite_event_init == NULL )
    {
      apr_rc = csd_event_create( &csdlite_event_init );
      if ( apr_rc != APR_EOK )
      {
        CSD_MSG(CSD_PRIO_ERROR,"csdlite_init: Failed to create event apr_rc[0x%x]", apr_rc );
      }
    }
    if ( APR_EOK == apr_rc )
    {
      csdlite_keep_thread_alive = TRUE;
      apr_rc = csd_thread_create( &csdlite_thread, 
                                  csdlite_thread_name, 
                                  CSDLITE_TASK_PRIORITY,
                                  NULL, 
                                  (uint32_t) CSDLITE_THREAD_STACK_SIZE,
                                  csdlite_service_task, 
                                  NULL );
    }
    if ( APR_EOK == apr_rc )
    {
      apr_rc = csd_event_wait( csdlite_event_init );
      csd_event_destroy( csdlite_event_init );
      csdlite_event_init = NULL; //reset memory
      checkpoint = 1;
    }

    if ( csdlite_lock == NULL )
    {
      apr_rc = csd_lock_create( &csdlite_lock );
      checkpoint = 2;
    }

    if ( APR_EOK == apr_rc )
    {
      apr_rc = apr_list_init( &csdlite_free_cmd_list, csdlite_task_lock_fn, csdlite_task_unlock_fn ); 
      checkpoint = 3;
    }

    if ( APR_EOK == apr_rc )
    {
      apr_rc = apr_list_init( &csdlite_active_cmd_list, csdlite_task_lock_fn, csdlite_task_unlock_fn );
      checkpoint = 4;
    }

    if ( APR_EOK == apr_rc )
    {
      for ( i = 0; i < CSDLITE_NUM_CMD; ++i )
      {
        apr_rc = apr_list_add_tail( &csdlite_free_cmd_list,
                              ( ( apr_list_node_t* ) &csdlite_cmd_pool[i].link ) );
      }
    }
    
    /*initialize global structures*/
    csdlite_dev_session.rx_dev.dev_id = CSDLITE_INVALID_DEV;
    csdlite_dev_session.rx_dev.bits_per_sample = 0;
    csdlite_dev_session.rx_dev.sample_rate = 0;
    csdlite_dev_session.tx_dev.dev_id = CSDLITE_INVALID_DEV;
    csdlite_dev_session.tx_dev.bits_per_sample = 0;
    csdlite_dev_session.tx_dev.sample_rate = 0;
    csdlite_dev_session.ec_ref_dev.dev_id = CSDLITE_INVALID_DEV;
    csdlite_dev_session.ec_ref_dev.bits_per_sample = 0;
    csdlite_dev_session.ec_ref_dev.sample_rate = 0;
    csdlite_dev_session.dev_handle = CSDLITE_INVALID_HANDLE;
    csdlite_dev_session.device_enabled = FALSE;

    csdlite_dtmf_session.detect_cb_func = NULL;
    csdlite_dtmf_session.gen_cb_func = NULL;

    memset( &csdlite_voice_session, 0, 
                       sizeof(csdlite_voice_sess_t) );

    csdlite_afe_loopback_state = CSDLITE_AFE_LOOPBACK_IDLE;

    memset( &csdlite_feature_cache, 0, 
                       sizeof(csdlite_feature_cache_t) );

    if ( APR_EOK != apr_rc )
    {
      /* free resources here*/
      switch ( checkpoint ) 
      {
        case 4:
          ( void ) apr_list_destroy ( &csdlite_active_cmd_list );

        case 3:
          ( void ) apr_list_destroy ( &csdlite_free_cmd_list );

        case 2:
          ( void ) csd_lock_destroy ( csdlite_lock );
          csdlite_lock = NULL;

        case 1:
          ( void ) csd_thread_destroy ( csdlite_thread );


          break;
        default:
          break;
      }
    }
    else
    {
      csdlite_inited = TRUE;
    }

  }
  return apr_rc;
}

uint32_t csdlite_deinit( void ){

  csd_event_destroy(csdlite_event);
  csd_timer_destroy(csdlite_dtmf_gen_timer);
  csd_lock_destroy(csdlite_lock);
  apr_list_destroy ( &csdlite_active_cmd_list );
  apr_list_destroy ( &csdlite_free_cmd_list );
  csd_thread_destroy ( csdlite_thread );

   csdlite_inited = FALSE;

  return CSD_EOK;
}

/*****************************************************************************/
/* helper functions                                                          */
/*****************************************************************************/
uint32_t csdlite_cleanup_loopback( void )
{
  uint32_t rc = CSD_EOK;
  csd_dev_afe_loopback_t csdlite_afe_loopback_info;

  csdlite_afe_loopback_info.afe_mode = 1;
  csdlite_afe_loopback_info.enable = FALSE;
  csdlite_afe_loopback_info.rx_dev_id = csdlite_dev_session.rx_dev.dev_id;
  csdlite_afe_loopback_info.tx_dev_id = csdlite_dev_session.tx_dev.dev_id;

  rc = csd_ioctl( csdlite_dev_session.dev_handle, CSD_DEV_CMD_AFE_LOOPBACK, &csdlite_afe_loopback_info, sizeof(csd_dev_afe_loopback_t) );

  return rc;
}

uint32_t csdlite_disable_device( void )
{
  uint32_t rc = CSD_EOK;
  csd_dev_disable_t csdlite_dev_disable_info;
  uint32_t num_devs = 0;

  csd_lock_enter(csdlite_lock);
  CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_disable_device rx_dev[%08X] tx_dev[%08X] ec_ref[%08X]",
                csdlite_dev_session.rx_dev.dev_id, csdlite_dev_session.tx_dev.dev_id, csdlite_dev_session.ec_ref_dev.dev_id);
  if ( csdlite_dev_session.rx_dev.dev_id != CSDLITE_INVALID_DEV  )
  {
    csdlite_dev_disable_info.num_devs = ++num_devs;
    csdlite_dev_disable_info.dev_ids[num_devs-1] = csdlite_dev_session.rx_dev.dev_id;
  }
  if ( csdlite_dev_session.tx_dev.dev_id != CSDLITE_INVALID_DEV  )
  {
    csdlite_dev_disable_info.num_devs = ++num_devs;
    csdlite_dev_disable_info.dev_ids[num_devs-1] = csdlite_dev_session.tx_dev.dev_id;
  }
  /* if ec_ref exsists*/
  if ( csdlite_dev_session.ec_ref_dev.dev_id != CSDLITE_INVALID_DEV )
  {
    csdlite_dev_disable_info.num_devs = ++num_devs;
    csdlite_dev_disable_info.dev_ids[num_devs-1] = csdlite_dev_session.ec_ref_dev.dev_id;
  }
  rc = csd_ioctl( csdlite_dev_session.dev_handle, CSD_DEV_CMD_DISABLE, &csdlite_dev_disable_info, sizeof(csd_dev_disable_t));
  if ( rc == CSD_EOK )
  {
    csdlite_dev_session.device_enabled = FALSE;
    csdlite_dev_session.rx_dev.dev_id = 0xFFFFFFFF;
    csdlite_dev_session.tx_dev.dev_id = 0xFFFFFFFF;
    csdlite_dev_session.ec_ref_dev.dev_id = 0xFFFFFFFF;

    /*disable vc if enabled*/
    if ( csdlite_voice_session.vc_state == CSDLITE_VC_ENABLED )
    {
      uint32_t csdlite_disable_cmd_token = 0;
      rc = csd_ioctl( csdlite_voice_session.vc_handle, CSD_VC_IOCTL_DISABLE, &csdlite_disable_cmd_token, sizeof(uint32_t));
      if (rc == CSD_EOK )
      {
        csd_event_wait(csdlite_event);
        rc = csdlite_voice_session.cb_response;
      }
      csdlite_voice_session.vc_state = CSDLITE_VC_ATTACHED;
    }
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: CSD_DEV_CMD_DISABLE rx_dev[%08X] tx_dev[%08X] ec ref[%08X] failed",
                csdlite_dev_session.rx_dev.dev_id, csdlite_dev_session.tx_dev.dev_id, csdlite_dev_session.ec_ref_dev.dev_id);
  }

  csd_lock_leave(csdlite_lock);
  return rc;
}

uint32_t csdlite_vc_config( csdlite_device_info_t dev_info )
{
  uint32_t rc = CSD_EOK;
  csd_vc_ioctl_set_device_config_v2_t csdlite_vc_set_device_args;
  uint32_t csdlite_vc_enable_args;

  CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_vc_config");

  /*create event for voice*/
  if ( csdlite_event == NULL )
  {
    rc = csd_event_create( &csdlite_event );
  }

  if ( !csdlite_voice_session.vc_handle )
  {
    CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_vc_config failed invalid handle");
    rc = CSD_EFAILED;
  }
  else if ( rc == CSD_EOK )
  { 
    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_vc_config: config vc");
    csdlite_vc_set_device_args.rx_dev_id = dev_info.rx_dev.dev_id;
    csdlite_vc_set_device_args.tx_dev_id = dev_info.tx_dev.dev_id;
    csdlite_vc_set_device_args.rx_dev_sr = dev_info.rx_dev.sample_rate;
    csdlite_vc_set_device_args.tx_dev_sr = dev_info.tx_dev.sample_rate;
    csdlite_vc_set_device_args.ec_ref_dev_id = dev_info.ec_ref_dev.dev_id;
    csdlite_vc_set_device_args.cmd_token = CSDLITE_VC_CONFIG;
    rc = csd_ioctl( csdlite_voice_session.vc_handle, 
                      CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2,
                      &csdlite_vc_set_device_args,
                      sizeof( csd_vc_ioctl_set_device_config_v2_t ) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
  }
  if ( rc == CSD_EOK )
  {
    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_vc_config: enabling vc");  
    csdlite_vc_enable_args = CSDLITE_VC_ENABLE;
    rc = csd_ioctl( csdlite_voice_session.vc_handle, 
                    CSD_VC_IOCTL_ENABLE,
                    &csdlite_vc_enable_args,
                    sizeof(csdlite_vc_enable_args));
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
    if ( rc == CSD_EOK )
    {
      csdlite_voice_session.vc_state = CSDLITE_VC_ENABLED;
    }
  }
  return rc;
}

uint32_t csdlite_enable_device( csdlite_device_info_t new_dev )
{
  uint32_t rc = CSD_EOK;

  csd_lock_enter(csdlite_lock);
  CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_enable_device rx_dev[%08X] tx_dev[%08X] ec_ref[%08X]",
                new_dev.rx_dev.dev_id, new_dev.tx_dev.dev_id, new_dev.ec_ref_dev.dev_id);
  /*get dev handle if not already avialable */
  /*Added for twizy - check dev_handle each time*/
  //if ( !csdlite_dev_session.dev_handle )
 // {
    csdlite_dev_session.dev_handle = csd_open( CSD_OPEN_DEVICE_CONTROL, NULL, 0 );
  //}

  /*check if device change is needed*/
  if ( ((csdlite_dev_session.rx_dev.dev_id == new_dev.rx_dev.dev_id) && (new_dev.rx_dev.dev_id != CSDLITE_INVALID_DEV)) &&
       ((csdlite_dev_session.tx_dev.dev_id == new_dev.tx_dev.dev_id) && (new_dev.tx_dev.dev_id !=CSDLITE_INVALID_DEV)) &&
       ((csdlite_dev_session.ec_ref_dev.dev_id == new_dev.ec_ref_dev.dev_id)))
  {
    /*perform dev restart on sample rate change only*/
    if ( csdlite_dev_session.device_enabled && csdlite_dev_session.rx_dev.sample_rate != new_dev.rx_dev.sample_rate )
    {
      /*dev restart*/
      csd_cmd_dev_restart_t csdlite_csd_dev_restart; 
      csdlite_csd_dev_restart.rx_dev_id = new_dev.rx_dev.dev_id;
      csdlite_csd_dev_restart.tx_dev_id = new_dev.tx_dev.dev_id;
      csdlite_csd_dev_restart.sample_rate = new_dev.rx_dev.sample_rate;
      CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: restart rx_dev[%08X] tx_dev[%08X] to sr [%d]",
                new_dev.rx_dev.dev_id, new_dev.tx_dev.dev_id, new_dev.rx_dev.sample_rate);
      rc = csd_ioctl (csdlite_dev_session.dev_handle, CSD_DEV_CMD_RESTART, &csdlite_csd_dev_restart, sizeof(csd_cmd_dev_restart_t));
      if ( rc == CSD_EOK )
      {
        csdlite_dev_session.rx_dev.sample_rate = new_dev.rx_dev.sample_rate;
        csdlite_dev_session.tx_dev.sample_rate = new_dev.rx_dev.sample_rate;
      }
    }
    else
    {
       CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_enable_device already configed for rx_dev[%08X] tx_dev[%08X] ec_ref[%08X]",
                new_dev.rx_dev.dev_id, new_dev.tx_dev.dev_id, new_dev.ec_ref_dev.dev_id);
	   rc = CSD_EALREADY;
    }
  }
  else
  {
    /*verify atleast rx device is valid*/
    if ( new_dev.rx_dev.dev_id != CSDLITE_INVALID_DEV )
    { 
      /*tear down old device if device is enabled*/
      if ( csdlite_dev_session.device_enabled )
      {
        CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: re-enabling device rx_dev[%08X] tx_dev[%08X]",
                csdlite_dev_session.rx_dev.dev_id, csdlite_dev_session.tx_dev.dev_id );
        rc = csdlite_disable_device();
      }
      if ( rc == CSD_EOK )
      {
        csd_dev_enable_t csdlite_dev_enable_info;
        uint32_t num_devs = 0;

        /*add rx device*/
        csdlite_dev_enable_info.num_devs = ++num_devs;
        csdlite_dev_enable_info.devs[num_devs-1].dev_id = new_dev.rx_dev.dev_id;
        csdlite_dev_enable_info.devs[num_devs-1].dev_attrib.bits_per_sample = new_dev.rx_dev.bits_per_sample;
        csdlite_dev_enable_info.devs[num_devs-1].dev_attrib.sample_rate = new_dev.rx_dev.sample_rate;

         /* if tx dev exsists*/
        if ( new_dev.tx_dev.dev_id != CSDLITE_INVALID_DEV )
        {
          csdlite_dev_enable_info.num_devs = ++num_devs;
          csdlite_dev_enable_info.devs[num_devs-1].dev_id = new_dev.tx_dev.dev_id;
          csdlite_dev_enable_info.devs[num_devs-1].dev_attrib.bits_per_sample = new_dev.tx_dev.bits_per_sample;
          csdlite_dev_enable_info.devs[num_devs-1].dev_attrib.sample_rate = new_dev.tx_dev.sample_rate;
        
          /* if ec_ref exsists*/
          if ( new_dev.ec_ref_dev.dev_id != CSDLITE_INVALID_DEV )
          {
            csdlite_dev_enable_info.num_devs = num_devs;
            csdlite_dev_enable_info.devs[num_devs-1].dev_id = new_dev.ec_ref_dev.dev_id;
            csdlite_dev_enable_info.devs[num_devs-1].dev_attrib.bits_per_sample = new_dev.ec_ref_dev.bits_per_sample;
            csdlite_dev_enable_info.devs[num_devs-1].dev_attrib.sample_rate = new_dev.ec_ref_dev.sample_rate;
          }
        }
        rc = csd_ioctl( csdlite_dev_session.dev_handle, CSD_DEV_CMD_ENABLE, &csdlite_dev_enable_info, sizeof(csd_dev_enable_t));
        /*reconfig vc if created*/
        if ( rc  == CSD_EOK  )
        {
          CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: CSD_DEV_CMD_ENABLE rx_dev[%08X] tx_dev[%08X] ec ref[%08X] success",
                new_dev.rx_dev.dev_id, new_dev.tx_dev.dev_id, new_dev.ec_ref_dev.dev_id);

          csdlite_dev_session.tx_dev.dev_id = new_dev.tx_dev.dev_id;
          csdlite_dev_session.tx_dev.bits_per_sample = new_dev.tx_dev.bits_per_sample;
          csdlite_dev_session.tx_dev.sample_rate = new_dev.rx_dev.sample_rate;
          csdlite_dev_session.rx_dev.dev_id = new_dev.rx_dev.dev_id;
          csdlite_dev_session.rx_dev.bits_per_sample = new_dev.rx_dev.bits_per_sample;
          csdlite_dev_session.rx_dev.sample_rate = new_dev.tx_dev.sample_rate;
          csdlite_dev_session.ec_ref_dev.dev_id = new_dev.ec_ref_dev.dev_id;
          csdlite_dev_session.ec_ref_dev.bits_per_sample = new_dev.ec_ref_dev.bits_per_sample;
          csdlite_dev_session.ec_ref_dev.sample_rate = new_dev.ec_ref_dev.sample_rate;
          csdlite_dev_session.device_enabled = TRUE;

          if( csdlite_voice_session.vc_handle )
          {
            rc = csdlite_vc_config( new_dev );
          }
        }
        else
        {
          CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: CSD_DEV_CMD_ENABLE rx_dev[%08X] tx_dev[%08X] ec ref[%08X] failed",
                new_dev.rx_dev.dev_id, new_dev.tx_dev.dev_id, new_dev.ec_ref_dev.dev_id);
        }
      }     
    }
    else
    {
      rc = CSD_EBADPARAM;
    }
  }
  csd_lock_leave(csdlite_lock);
  return rc;
}

uint32_t csdlite_voice_reconfig( csdlite_voice_config_t* params )
{
  uint32_t rc = CSD_EOK;

  /*if already configued and session id diffs */
  if ( csdlite_voice_session.state == CSDLITE_VOICE_CONFIG &&
        strncmp( csdlite_voice_session.session_id, params->session_id, csdlite_voice_session.session_id_size )!= 0 )
  {
    rc = CSD_EBADPARAM;
  }
  /*check if device info is there*/
  else if ( params->dev_info.rx_dev.dev_id != CSDLITE_INVALID_DEV && 
            params->dev_info.tx_dev.dev_id != CSDLITE_INVALID_DEV )
  {
    rc = csdlite_enable_device( params->dev_info );
  }

  return rc;
}

uint32_t csdlite_apply_volume( void )
{
  uint32_t rc = CSD_EOK;

  if ( csdlite_feature_cache.volume.applied )
  {
    csd_vc_ioctl_set_rx_volume_step_t csdlite_volume_param;

    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_apply_volume");
    csdlite_volume_param.ramp_duration = csdlite_feature_cache.volume.ramp_duration;
    csdlite_volume_param.vol_step = csdlite_feature_cache.volume.step;
    csdlite_volume_param.cmd_token = CSDLITE_VC_VOLUME;

    rc = csd_ioctl( csdlite_voice_session.vc_handle, CSD_VC_IOCTL_SET_RX_VOLUME_STEP, &csdlite_volume_param, sizeof(csd_vc_ioctl_set_rx_volume_step_t) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
    if ( rc == CSD_EOK )
    {
      csdlite_feature_cache.volume.applied = FALSE;
    }
  }

  return rc;
}

uint32_t csdlite_apply_hdvoice( void )
{
  uint32_t rc = CSD_EOK;
  if ( csdlite_feature_cache.hd_voice.applied )
  {
    csd_vm_ioctl_set_hdvoice_mode_t hd_voice;

    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_apply_hdvoice");

    hd_voice.cmd_token = CSDLITE_VM_HDVOICE;
    hd_voice.mode = csdlite_feature_cache.hd_voice.hd_voice;
    rc = csd_ioctl( csdlite_voice_session.vm_handle, CSD_VM_IOCTL_SET_HDVOICE_MODE, &hd_voice, sizeof(csd_vm_ioctl_set_hdvoice_mode_t) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
    if ( rc == CSD_EOK )
    {
      csdlite_feature_cache.hd_voice.applied = FALSE;
    }
  }
  return rc;
}

uint32_t csdlite_apply_tty( void )
{
  uint32_t rc = CSD_EOK;
  if ( csdlite_feature_cache.tty.applied )
  {
    csd_vm_ioctl_set_tty_mode_t tty;

    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_apply_tty mode %d", csdlite_feature_cache.tty.tty_mode ); 
    
    tty.cmd_token = CSDLITE_VM_TTY_MODE;
    tty.mode = csdlite_feature_cache.tty.tty_mode;
    rc = csd_ioctl( csdlite_voice_session.vm_handle, CSD_VM_IOCTL_SET_TTY_MODE, &tty, sizeof(csd_vm_ioctl_set_tty_mode_t) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
    if ( rc == CSD_EOK )
    {
      csdlite_feature_cache.tty.applied = FALSE;
    }
  }
  return rc;
}

uint32_t csdlite_apply_cache( void ){
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_apply_cache");

  rc = csdlite_apply_hdvoice();
  rc = csdlite_apply_tty();
  rc = csdlite_apply_volume();

  return rc;
}

uint32_t csdlite_voice_teardown( void )
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_voice_teardown");
   
  if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING )
  {
    uint32_t csdlite_stop_token = CSDLITE_VM_STOP;
    rc = csd_ioctl( csdlite_voice_session.vm_handle, CSD_VM_IOCTL_STOP_VOICE, &csdlite_stop_token, sizeof(uint32_t) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
    if ( rc == CSD_EOK )
    {
        csdlite_voice_session.state = CSDLITE_VOICE_CONFIG;
    }
  }
  if ( csdlite_voice_session.vc_state == CSDLITE_VC_ATTACHED && rc == CSD_EOK )
  {
    csd_vm_ioctl_detach_context_t detach_param;
    /*dettach vc*/
    detach_param.cmd_token = CSDLITE_VM_DETACH;
    detach_param.context_handle = csdlite_voice_session.vc_handle;
    rc = csd_ioctl( csdlite_voice_session.vm_handle, CSD_VM_IOCTL_DETACH_CONTEXT, &detach_param, sizeof(csd_vm_ioctl_detach_context_t) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
    if ( rc == CSD_EOK )
    {
      csdlite_voice_session.vc_state = CSDLITE_VC_IDLE;
    }  
  }
 
  /*close all handles*/
  if ( rc == CSD_EOK )
  {
    if ( csdlite_voice_session.vc_handle )
    {
      rc = csd_close( csdlite_voice_session.vc_handle );
      csdlite_voice_session.vc_handle = CSDLITE_INVALID_HANDLE;
    }
    if ( csdlite_voice_session.vs_handle)
    {
      rc = csd_close( csdlite_voice_session.vs_handle );
      csdlite_voice_session.vs_handle = CSDLITE_INVALID_HANDLE;
    }
    if ( csdlite_voice_session.vm_handle )
    {
      rc = csd_close( csdlite_voice_session.vm_handle );
      csdlite_voice_session.vm_handle = CSDLITE_INVALID_HANDLE;
    }
  }
  if ( rc == CSD_EOK  )
  {
    csdlite_voice_session.state = CSDLITE_VOICE_IDLE;
  }

  /*clear cache*/
  memset( &csdlite_feature_cache, 0, sizeof(csdlite_feature_cache_t) );

  return rc;
}

/*****************************************************************************/
/* API defs                                                                  */
/*****************************************************************************/

void csdlite_cleanup( void )
{
  CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_cleanup");
  /*cleanup device*/
  if ( csdlite_dev_session.device_enabled )
  {
    csdlite_disable_device();
  }
  /*cleanup voice*/
   csdlite_voice_teardown();

}

uint32_t csdlite_get_version( void )
{
  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_get_version called");
  return CSDLITE_DRIVER_VER;
}

uint32_t csdlite_voice_config(csdlite_voice_config_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_voice_config called" );

  /*verify session is not already started*/
  if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING )
  {
    rc = CSD_EALREADY;
  }
  /* verfiy we are not in loopback mode*/
  if ( csdlite_afe_loopback_state !=  CSDLITE_AFE_LOOPBACK_IDLE )
  {
    rc = csdlite_cleanup_loopback();
  }
  if ( rc == CSD_EOK )
  {
    /*cleanup session if in configed state and there is a new session name*/
    if ( csdlite_voice_session.state == CSDLITE_VOICE_CONFIG &&
         strncmp( csdlite_voice_session.session_id, params->session_id, csdlite_voice_session.session_id_size )!= 0)
    {
      csdlite_voice_teardown();
    }

    rc = csdlite_enable_device( params->dev_info );

    if ( rc == CSD_EOK && csdlite_voice_session.state == CSDLITE_VOICE_IDLE )
    {
      /*setup voice path*/
      csd_vc_open_full_control_t csdlite_open_vc_info;
      csd_vm_open_t csdlite_open_vm_info;
      csd_vs_open_t csdlite_open_vs_info;
      csd_vm_ioctl_attach_context_t csdlite_vm_attach_context_args;

      csdlite_voice_session.session_id_size = strlcpy(csdlite_voice_session.session_id, params->session_id , sizeof(csdlite_voice_session.session_id));

      csdlite_open_vc_info.callback_fn = csdlite_vc_callback_fn;
      csdlite_open_vc_info.cb_data = (void*) &csdlite_version;
      csdlite_open_vc_info.direction = (uint16_t)CSDLITE_DIR_TX_RX;
      csdlite_open_vc_info.name = csdlite_voice_session.session_id; 
      csdlite_open_vc_info.name_size =  csdlite_voice_session.session_id_size+1;
      csdlite_open_vc_info.network_id = CSD_NETWORK_ID_DEFAULT;
      csdlite_open_vc_info.ret_status = &rc;


      csdlite_voice_session.vc_handle = csd_open( CSD_OPEN_VOICE_CONTEXT, &csdlite_open_vc_info, 
                            sizeof(csd_vc_open_full_control_t) );
      if ( rc != CSD_EOK )
      {
        CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_voice_config open vc failed with rc 0x%x ", rc  );
      }
      else
      {
        csdlite_open_vm_info.open_id = CSD_VM_OPENID_PASSIVE_CONTROL;
        csdlite_open_vm_info.ret_status = &rc;
        csdlite_open_vm_info.u.passive_control.name_size = csdlite_voice_session.session_id_size+1;
        csdlite_open_vm_info.u.passive_control.name = csdlite_voice_session.session_id; 
        csdlite_open_vm_info.u.passive_control.callback_fn = csdlite_vm_callback_fn;
        csdlite_open_vm_info.u.passive_control.cb_data = (void*) &csdlite_version;
        csdlite_voice_session.vm_handle = csd_open( CSD_OPEN_VOICE_MANAGER, &csdlite_open_vm_info, 
                              sizeof(csd_vm_open_passive_control_t) );
      }
      if ( rc != CSD_EOK )
      {
        CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_voice_config open vm failed with rc 0x%x ", rc  );
      }
      else
      {
        csdlite_open_vs_info.open_id = CSD_VS_OPENID_PASSIVE_CONTROL;
        csdlite_open_vs_info.ret_status = &rc;
        csdlite_open_vs_info.u.passive_control.name_size = csdlite_voice_session.session_id_size+1;
        csdlite_open_vs_info.u.passive_control.name = csdlite_voice_session.session_id; 
        csdlite_open_vs_info.u.passive_control.callback_fn = csdlite_vs_callback_fn;
        csdlite_open_vs_info.u.passive_control.cb_data = (void*) &csdlite_version;
        csdlite_voice_session.vs_handle = csd_open( CSD_OPEN_VOICE_STREAM, &csdlite_open_vs_info, 
                              sizeof(csd_vs_open_passive_control_t) );
      }
      if ( rc != CSD_EOK )
      {
        CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_voice_config open vm failed with rc 0x%x ", rc  );
      }

      /*attach vc*/
      if ( rc == CSD_EOK )
      {
        csdlite_vm_attach_context_args.cmd_token = CSDLITE_VM_ATTACH;
        csdlite_vm_attach_context_args.context_handle = csdlite_voice_session.vc_handle;

        /*create event for voice*/
        if ( csdlite_event == NULL )
        {
          CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_voice_config: creating the csdlite_event event ");
          rc = csd_event_create( &csdlite_event );
        }
        rc = csd_ioctl( csdlite_voice_session.vm_handle, 
                      CSD_VM_IOCTL_ATTACH_CONTEXT,
                      &csdlite_vm_attach_context_args,
                      sizeof( csd_vm_ioctl_attach_context_t ));
        if (rc == CSD_EOK)
        {
          csd_event_wait(csdlite_event);
          rc = csdlite_voice_session.cb_response;
        }
        if ( rc == CSD_EOK )
        {
          csdlite_voice_session.vc_state = CSDLITE_VC_ATTACHED;
          csdlite_voice_session.state = CSDLITE_VOICE_CONFIG;
        }
      }
      
      if ( rc == CSD_EOK )
      {
        /*enable and config vc*/
        rc = csdlite_vc_config( params->dev_info );
      }
    }
  }

  if ( rc != CSD_EOK && rc != CSD_EALREADY )
  {
      CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_voice_config failed with rc 0x%x", rc  );
      /*if failed cleanup*/
      csdlite_cleanup();
  }

  return rc;
}

uint32_t csdlite_voice_start(csdlite_voice_config_t* params)
{
  uint32_t rc = CSD_EOK;

   CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_voice_start called" );

  if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING )
  {
    rc = CSD_EALREADY;
    CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_voice_start already in voice call" );
  }
  else if ( csdlite_voice_session.state == CSDLITE_VOICE_IDLE )
  {
    rc = csdlite_voice_config(params);
  }
  else 
  {
    rc = csdlite_voice_reconfig( params );
  }

  if (  rc == CSD_EOK || rc == CSD_EALREADY)
  {
    uint32_t cmd_token = CSDLITE_VM_START;

    rc = csdlite_apply_cache();
    /*start voice*/
    rc = csd_ioctl( csdlite_voice_session.vm_handle, 
                    CSD_VM_IOCTL_START_VOICE,
                    &cmd_token, 
                    sizeof ( cmd_token ) );
    if (rc == CSD_EOK)
    {
      csd_event_wait(csdlite_event);
      rc = csdlite_voice_session.cb_response;
    }
     if ( rc == CSD_EOK )
     {
       csdlite_voice_session.state = CSDLITE_VOICE_RUNNING;
     }
  }

  if ( rc != CSD_EOK )
  {
      CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_voice_start failed with rc 0x%x", rc  );
  }
  return rc;
}

uint32_t csdlite_voice_end(csdlite_voice_end_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_voice_end called" );

  if ( csdlite_voice_session.state != CSDLITE_VOICE_IDLE  )
  {
    /*disable device*/
    if ( csdlite_dev_session.device_enabled )
    {
      rc = csdlite_disable_device();
    }
    
    if ( rc == CSD_EOK )
    {
      rc = csdlite_voice_teardown();
    }
   }
   else
   {
     /*fail if voice session is idle*/
     rc = CSD_EFAILED;
     CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_voice_end failed no session to end" );
   }

   return rc;
}

uint32_t csdlite_dev_switch(csdlite_dev_switch_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_dev_switch called" );
  if ( !csdlite_dev_session.device_enabled )
  {
    rc = CSD_EFAILED;
  }
  else
  {
      rc = csdlite_enable_device( params->dev_info );
  }
  return rc;
}

uint32_t csdlite_afe_loopback(csdlite_afe_loopback_t* params)
{
  uint32_t rc = CSD_EOK;
  csd_dev_afe_loopback_t csdlite_afe_loopback_info;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_afe_loopback called" );

  if ( csdlite_voice_session.state != CSDLITE_VOICE_IDLE )
  {
    CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_afe_loopback cannot start duirng voice call");
    rc = CSD_EBUSY;
  }
  else if ( csdlite_afe_loopback_state == CSDLITE_AFE_LOOPBACK_RUNNING && params->enable )
  {
    CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_afe_loopback already started" );
    rc = CSD_EALREADY;
  }
  else
  {
    /*enable dev on enable*/
    if ( params->enable )
    {
      rc = csdlite_enable_device( params->dev_info);
    }
  
    if ( rc == CSD_EOK )
    {
      csdlite_afe_loopback_info.afe_mode = params->afe_mode;
      csdlite_afe_loopback_info.enable = params->enable;
      csdlite_afe_loopback_info.rx_dev_id = params->dev_info.rx_dev.dev_id;
      csdlite_afe_loopback_info.tx_dev_id = params->dev_info.tx_dev.dev_id;

      rc = csd_ioctl( csdlite_dev_session.dev_handle, CSD_DEV_CMD_AFE_LOOPBACK, &csdlite_afe_loopback_info, sizeof(csd_dev_afe_loopback_t) );

    }
    if ( rc == CSD_EOK && params->enable )
    {
      csdlite_afe_loopback_state = CSDLITE_AFE_LOOPBACK_RUNNING;
    }
    else
    {
      /*teardown dev on disable*/
      csdlite_disable_device();
      csdlite_afe_loopback_state = CSDLITE_AFE_LOOPBACK_IDLE;
    }

  }

  return rc;
}


uint32_t csdlite_dtmf_generation(csdlite_dtmf_gen_t* params)
{
  uint32_t rc = CSD_EOK;
  uint64_t timer;
  bool_t timer_enabled = FALSE;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_dtmf_generation called");

  csdlite_dtmf_session.gen_cb_func = params->cb_func;

  /* if device */
  if ( !csdlite_dev_session.device_enabled )
  {
    rc = csdlite_enable_device(params->dev_info);

    timer_enabled = TRUE;
  }
  /*send dtmf command*/
  if ( params->direction == CSDLITE_DIR_TX )
  {
    csd_vs_ioctl_set_dtmf_generation_t voice_gen;

    if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING  )
    {
      voice_gen.cmd_token = CSDLITE_VS_DTMF_GEN;
      voice_gen.direction = params->direction;
      voice_gen.duration = params->dtmf_duration_in_ms;
      voice_gen.gain = params->dtmf_gain;
      voice_gen.mix_flag = params->mix_flag;
      voice_gen.tone_1 = params->dtmf_low_freq;
      voice_gen.tone_2 = params->dtmf_high_freq;

      CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_dtmf_generation calling CSD_VS_IOCTL_SET_DTMF_GENERATION");
      rc = csd_ioctl( csdlite_voice_session.vs_handle, 
                          CSD_VS_IOCTL_SET_DTMF_GENERATION,
                          &voice_gen, 
                          sizeof ( csd_vs_ioctl_set_dtmf_generation_t ) );
      if (rc == CSD_EOK)
      {
        csd_event_wait(csdlite_event);
        rc = csdlite_voice_session.cb_response;
      }
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,"CSDLITE: csdlite_dtmf_generation must be in voice call for TX generation");
      rc = CSD_EBADPARAM;
    }
  }
  else if ( params->direction == CSDLITE_DIR_RX )
  {
    csd_dev_dtmf_ctrl_t dev_gen;
    dev_gen.dtmf_duration_in_ms = params->dtmf_duration_in_ms;
    dev_gen.dtmf_gain = params->dtmf_gain;
    dev_gen.dtmf_high_freq = params->dtmf_high_freq;
    dev_gen.dtmf_low_freq = params->dtmf_low_freq;
    dev_gen.num_devs = 1;
    dev_gen.p_dev_ids = &csdlite_dev_session.rx_dev.dev_id;

    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_dtmf_generation calling CSD_DEV_CMD_DTMF_CONTROL");
    rc = csd_ioctl( csdlite_dev_session.dev_handle, 
                        CSD_DEV_CMD_DTMF_CONTROL,
                        &dev_gen, 
                        sizeof ( csd_dev_dtmf_ctrl_t ) );
    if ( rc == CSD_EOK )
    {
       CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_dtmf_generation CSD_DEV_CMD_DTMF_CONTROL success");
    }
  }
  else
  {
    rc = CSD_EBADPARAM;
  }

  /*start timer if needed*/
  if ( timer_enabled && params->dtmf_duration_in_ms != -1 && params->dtmf_duration_in_ms != 0 )
  {
    /*create timer if not already created*/
    if ( csdlite_dtmf_gen_timer == NULL )
    {
      rc = csd_timer_create( &csdlite_dtmf_gen_timer, csdlite_dtmf_gen_timer_cb, &csdlite_version );
    }
    timer = ( ( params->dtmf_duration_in_ms*1000000 < 0xB2D05E00 ) ? 0xB2D05E00 : params->dtmf_duration_in_ms*1000000 );
    CSD_MSG(CSD_PRIO_MEDIUM,"CSDLITE: csdlite_dtmf_generation timer set to 0x%x", timer);
    /*stop timer if currently running*/
    csd_timer_stop(csdlite_dtmf_gen_timer);
    csd_timer_start( csdlite_dtmf_gen_timer, timer );
  }

  /*tear down device if not being used and a 0 is passed*/
  if ( params->dtmf_duration_in_ms == 0 && 
            csdlite_voice_session.state == CSDLITE_VOICE_IDLE && 
            csdlite_afe_loopback_state == CSDLITE_AFE_LOOPBACK_IDLE )
  {
    rc = csdlite_disable_device();
  }

  return rc;
}

uint32_t csdlite_dtmf_detection(csdlite_dtmf_detect_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_dtmf_detection called");
   /*only allow detection during voice call*/
  if ( csdlite_voice_session.state != CSDLITE_VOICE_IDLE )
  {
    csd_vc_ioctl_set_tx_dtmf_detection_t tx_detection;
    csd_vs_ioctl_set_rx_dtmf_detection_t rx_detection;

    csdlite_dtmf_session.detect_cb_func = params->cb_func;
    tx_detection.cmd_token = CSDLITE_VC_DTMF_DETECT;
    tx_detection.enable = params->enable;
    rx_detection.cmd_token = CSDLITE_VS_DTMF_DETECT;
    rx_detection.enable = params->enable;
    switch ( params->direction )
    {
      case  CSDLITE_DIR_TX:      
        rc = csd_ioctl( csdlite_voice_session.vc_handle, 
                        CSD_VC_IOCTL_SET_TX_DTMF_DETECTION,
                        &tx_detection, 
                        sizeof ( csd_vc_ioctl_set_tx_dtmf_detection_t ) );
        if (rc == CSD_EOK)
        {
          csd_event_wait(csdlite_event);
          rc = csdlite_voice_session.cb_response;
        }
        break;

      case CSDLITE_DIR_RX:
        rc = csd_ioctl( csdlite_voice_session.vs_handle, 
                        CSD_VS_IOCTL_SET_RX_DTMF_DETECTION,
                        &rx_detection, 
                        sizeof ( csd_vs_ioctl_set_rx_dtmf_detection_t ) );

        if (rc == CSD_EOK)
        {
          csd_event_wait(csdlite_event);
          rc = csdlite_voice_session.cb_response;
        }
        break;

      case CSDLITE_DIR_TX_RX:
        rc = csd_ioctl( csdlite_voice_session.vc_handle, 
                        CSD_VC_IOCTL_SET_TX_DTMF_DETECTION,
                        &tx_detection, 
                        sizeof ( csd_vc_ioctl_set_tx_dtmf_detection_t ) );
        if (rc == CSD_EOK)
        {
          csd_event_wait(csdlite_event);
          rc = csdlite_voice_session.cb_response;
        }

        rc = csd_ioctl( csdlite_voice_session.vs_handle, 
                        CSD_VS_IOCTL_SET_RX_DTMF_DETECTION,
                        &rx_detection, 
                        sizeof ( csd_vs_ioctl_set_rx_dtmf_detection_t ) );
        if (rc == CSD_EOK)
        {
          csd_event_wait(csdlite_event);
          rc = csdlite_voice_session.cb_response;
        }
        break; 
    }
  }
  else
  {
    rc = CSD_EFAILED;
  }

  return rc;
}


uint32_t csdlite_volume(csdlite_volume_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_volume called");
  /*cache volume*/
  csdlite_feature_cache.volume.ramp_duration = params->ramp_duration;
  csdlite_feature_cache.volume.step = params->vol_step;
  csdlite_feature_cache.volume.applied = TRUE;

  /*if runnnig apply volume*/
  if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING )
  {
    rc = csdlite_apply_volume();
  }

  return rc;
}


uint32_t csdlite_mute(csdlite_mute_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_mute called");
  if (  csdlite_voice_session.state != CSDLITE_VOICE_IDLE )
  {
    if ( params->mode == CSDLITE_MUTE_STREAM )
    {
      csd_vs_ioctl_set_mute_v2_t mute;

      mute.cmd_token = CSDLITE_VS_MUTE;
      mute.direction = params->direction;
      mute.mute_flag = params->mute_flag;
      mute.ramp_duration = params->ramp_duration;

      rc = csd_ioctl( csdlite_voice_session.vs_handle, 
                    CSD_VS_IOCTL_SET_MUTE_V2,
                    &mute, 
                    sizeof ( csd_vs_ioctl_set_mute_v2_t ) );
      if (rc == CSD_EOK)
      {
        csd_event_wait(csdlite_event);
        rc = csdlite_voice_session.cb_response;
      }
    }
    else if ( params->mode == CSDLITE_MUTE_DEV )
    {
      csd_vc_ioctl_set_mute_v2_t mute;

      mute.cmd_token = CSDLITE_VC_MUTE;
      mute.direction = params->direction;
      mute.mute_flag = params->mute_flag;
      mute.ramp_duration = params->ramp_duration;

      rc = csd_ioctl( csdlite_voice_session.vc_handle, 
                    CSD_VC_IOCTL_SET_MUTE_V2,
                    &mute, 
                    sizeof ( csd_vc_ioctl_set_mute_v2_t ) );
      if (rc == CSD_EOK)
      {
        csd_event_wait(csdlite_event);
        rc = csdlite_voice_session.cb_response;
      }
    }
    else
    {
      rc = CSD_EBADPARAM;
    }

  }
  else
  {
    rc = CSD_EFAILED;
  }

  return rc;
}

uint32_t csdlite_set_voice_feature( csdlite_feature_t* params)
{
  uint32_t rc = CSD_EOK;

  CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_set_voice_feature called");
  switch ( params->feature )
  {
    case CSDLITE_HDVOICE:
      csdlite_feature_cache.hd_voice.applied = TRUE;
      csdlite_feature_cache.hd_voice.hd_voice = params->mode;
      if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING )
      {
        rc = csdlite_apply_hdvoice();
      }
      break;
    case CSDLITE_SLOWTALK:
      rc = CSD_EUNSUPPORTED;
      break;
    case CSDLITE_SET_TTY_MODE:
      csdlite_feature_cache.tty.applied = TRUE;
      csdlite_feature_cache.tty.tty_mode = params->mode;
      if ( csdlite_voice_session.state == CSDLITE_VOICE_RUNNING )
      {
        rc = csdlite_apply_tty();
      }
      break;
  }

  return rc;
}

/*****************************************************************************/
/*                               call back functions                         */
/*****************************************************************************/
uint32_t csdlite_vc_callback_fn(uint32_t cmd_token,uint32_t event_id, 
                                void* param, uint32_t param_size,
                                void* client_data)
{
  uint32_t rc = CSD_EOK;
  csd_vc_event_command_rsp_t* rsp = param;

   /*if dtmf detect event send callback*/
  if ( event_id == CSD_VC_EVENTID_TX_DTMF_DETECTED )
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_vc_callback_fn: recieved CSD_VC_EVENTID_TX_DTMF_DETECTED sending event cb" );
    csdlite_dtmf_session.detect_cb_func( CSDLITE_VC_DTMF_DETECT , CSDLITE_TX_DTMF_DETECTED,
                               param, param_size, client_data );
  }
  else if (event_id == CSD_VC_EVENTID_COMMAND_RSP) 
  {
    csdlite_voice_session.cb_response = rsp->status_id;
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_vc_callback_fn: recieved CSD_VC_EVENTID_COMMAND_RSP callback %d with rc = %d", cmd_token, rsp->status_id );
    csd_event_signal( csdlite_event );
  }

  return rc;
}

uint32_t csdlite_vs_callback_fn(uint32_t cmd_token,uint32_t event_id, 
                                void* param, uint32_t param_size,
                                void* client_data)
{
  uint32_t rc = CSD_EOK;
  csd_vs_event_command_rsp_t* rsp = param;

  /*if dtmf detect event send callback*/
  if ( event_id == CSD_VS_EVENTID_RX_DTMF_DETECTED )
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_vs_callback_fn: recieved CSD_VS_EVENTID_RX_DTMF_DETECTED sending event cb" );
    csdlite_dtmf_session.detect_cb_func( CSDLITE_VS_DTMF_DETECT , CSDLITE_RX_DTMF_DETECTED,
                               param, param_size, client_data );
  }
  else if (  event_id == CSD_VS_EVENTID_DTMF_GENERATION_ENDED )
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_vs_callback_fn: recieved CSD_VS_EVENTID_DTMF_GENERATION_ENDED sending event cb" );
    csdlite_dtmf_session.gen_cb_func( CSDLITE_VS_DTMF_GEN, CSDLITE_DTMF_GENERATION_ENDED,
                               param, param_size, client_data );
  }
  else if (event_id == CSD_VS_EVENTID_COMMAND_RSP ) 
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_vs_callback_fn: recieved CSD_VS_EVENTID_COMMAND_RSP callback %d with rc = %d", cmd_token, rsp->status_id );
    csdlite_voice_session.cb_response = rsp->status_id;
    csd_event_signal( csdlite_event );   
  }

  return rc;
}

uint32_t csdlite_vm_callback_fn(uint32_t cmd_token,uint32_t event_id, 
                                void* param, uint32_t param_size,
                                void* client_data)
{
  uint32_t rc = CSD_EOK;
  csd_vm_event_command_rsp_t* rsp = param;

  if (event_id == CSD_VM_EVENTID_COMMAND_RSP)
  {
    CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: csdlite_vm_callback_fn: recieved CSD_VM_EVENTID_COMMAND_RSP callback %d with rc = %d", cmd_token, rsp->status_id );
    csdlite_voice_session.cb_response = rsp->status_id;
    csd_event_signal( csdlite_event );
  }
  
  return rc; 
}


void csdlite_dtmf_gen_timer_cb( void* client_token )
{
   CSD_MSG(CSD_PRIO_HIGH,"CSDLITE: dtmf_gen_timer_cb: recieved");

  /*if not in afe or voice usecase teardown device*/
  if ( csdlite_voice_session.state == CSDLITE_VOICE_IDLE && 
       csdlite_afe_loopback_state == CSDLITE_AFE_LOOPBACK_IDLE )
  {
    csdlite_cmd_node_t *cmd;
    ( void ) apr_list_remove_head( &csdlite_free_cmd_list, (apr_list_node_t**)&cmd );
    cmd->cmd = CSDLITE_CMD_DISABLE_DEV;
    ( void ) apr_list_add_tail( &csdlite_active_cmd_list, &cmd->link);

    csd_event_signal( csdlite_event_handle );
  }
}

