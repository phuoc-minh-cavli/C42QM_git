
/*-----------------------------------------------------------------------------
     Copyright (C) 2014, 2016 Qualcomm Technologies Incorporated.
  All rights reserved.
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/csd/voice/src/csd_voc_common.c#2 $
	$DateTime: 2019/01/03 07:23:13 $
	$Author: pwbldsvc $
	$Change: 17964421 $
	$Revision: #2 $
	=========================================================================*/
 /*=============================================================================
                     
  This section contains comments describing changes made to this module.
  Notice that changes are listed in reverse chronological order.
  when          who       what, where, why
  --------     -----     ---------------------------------------------------
  11/15/16      rk       Abstracting OSAL layer
  08/25/14      jk       added per vocoder calibration changes  
  11/05/14      jk       fallback cal fix

=============================================================================*/
#include "apr_list.h"
#include "aprv2_api_inline.h"

#include "acdb.h"

#include "csd_handle.h"
#include "csd_status.h"
#include "csd_voc_common.h"
#include "csd_dev_common.h"

#include "csd_debug_msg.h"
#include "vss_imemory_public_if.h"
#include "csd_os_dependencies.h"
#include "csd_error_map.h"

#include <stdio.h>
#include <string.h>

#include "csd_debug_info.h"


/*
 * Start Internal data & structures
*/
static csd_lock_t csd_voc_common_task_lock = NULL;          /* Lock management */
static csd_event_t csd_voc_common_event_handle = NULL;      /* Event management */
static csd_thread_t csd_voc_common_task_handle = NULL;      /* Task management */
static csd_event_t csd_voc_common_task_control_event_init = NULL;      /* Event management */
static csd_event_t csd_voc_common_task_control_event_deinit = NULL;
static bool_t csd_voc_common_keep_thread_alive = FALSE;

uint8_t csd_voc_common_thread_stack_base[CSD_VOC_COMMON_THREAD_STACK_SIZE];

static csd_voc_common_session_list_t csd_voc_common_session_pool[ CSD_VOC_COMMON_NUM_SESSIONS ];
static apr_list_t csd_voc_common_free_sess_list;
static apr_list_t csd_voc_common_active_sess_list;

/*
 * End Internal data & structures
*/


/*
 * Start Internal Functions
*/

static void csd_voc_common_task_lock_fn ( void )
{
  ( void ) csd_lock_enter( csd_voc_common_task_lock );
}

static void csd_voc_common_task_unlock_fn ( void )
{
  ( void ) csd_lock_leave( csd_voc_common_task_lock );
}

static int32_t csd_voc_common_service_task( void* param) 
{
  uint32_t apr_rc = APR_EOK;
  csd_voc_common_session_list_t* node, *prev;

  //signal parent thread it can proceed
  if( csd_voc_common_task_control_event_init )
  {
    csd_event_signal(csd_voc_common_task_control_event_init);
  }
  else
  {
    apr_rc = APR_ENOTREADY;
    CSD_MSG(CSD_PRIO_ERROR,"csd_voc_common_service_task: apr_event : csd_voc_common_task_control_event_init"
            "not ready apr_rc[0x%x]", apr_rc );
  }
  
  if( apr_rc == APR_EOK && csd_voc_common_event_handle == NULL )
  { //only create the event if there was no event before.
    apr_rc = csd_event_create( &csd_voc_common_event_handle );
  }
  if ( apr_rc != APR_EOK )
  {
    CSD_MSG(CSD_PRIO_ERROR,"Failed to create event apr_rc[0x%x]", apr_rc );
  }

  if( apr_rc == APR_EOK )
  {
    do
    {
      apr_rc = csd_event_wait( csd_voc_common_event_handle );
      if( apr_rc != APR_EOK && apr_rc != APR_ECONTINUE )
      {
        CSD_MSG( CSD_PRIO_ERROR,
                 "Event wait failed, apr_rc[0x%x]",
                 apr_rc );
        break;
      }
      else if ( apr_rc != APR_ECONTINUE )
      {
      node = NULL; /* reset the head pointer */

      ( void ) csd_voc_common_task_lock_fn();
      apr_rc =  apr_list_peak_head( &csd_voc_common_active_sess_list, ( apr_list_node_t** ) &node);
      do
      {
        if ( node == NULL )
        {
          break;
        }

        if ( ( node->sess.sig & CSD_VOC_COMMON_SIG_CMD) && (node->sess.cmd_fn != NULL) )
        {
          node->sess.sig &= ~CSD_VOC_COMMON_SIG_CMD;
          node->sess.cmd_fn( ( uint32_t )NULL);
        }
        prev = node;
      } /* for all sessions */
      while( apr_list_get_next( &csd_voc_common_active_sess_list,
                                  ( apr_list_node_t* )prev,
                                  ( apr_list_node_t** )&node ) == APR_EOK);

      ( void ) csd_voc_common_task_unlock_fn ();
      //TODO: validate if the thread finished all its tasks before being killed.
      }
    }while ( csd_voc_common_keep_thread_alive );

    apr_rc = csd_event_destroy( csd_voc_common_event_handle );
    if( apr_rc != APR_EOK )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_voc_common_service_task: Failed to destroy event apr_rc[0x%x]", apr_rc );
    }
    else
    {
      csd_voc_common_event_handle = NULL;
    }
  }
  if( csd_voc_common_task_control_event_deinit )
  {
    csd_event_signal(csd_voc_common_task_control_event_deinit);
  }
  //TODO: verify thread is destroyed if we return value 0
  //on DalWorkloop 0 will not destroy the thread.
  return CSD_EOK; 
}

/*
 * Start External Functions
*/
/*
 * FUNCTION: csd_voc_common_init

 * PARAMETERS:  None

 * CONTEXT: Clients context at bootup time

 * SIDE EFFECTS: create a CVC task/thread .

 * RETURN:    csd status
*/
uint32_t csd_voc_common_init() 
{
  int32_t rc = CSD_EOK;
  uint32_t apr_rc = APR_EOK;
  uint32_t i = 0;
  uint32_t checkpoint = 0;
    
  if ( APR_EOK == apr_rc && csd_voc_common_task_control_event_init == NULL )
  {
    apr_rc = csd_event_create( &csd_voc_common_task_control_event_init );
    if ( apr_rc != APR_EOK )
    {
      CSD_MSG(CSD_PRIO_ERROR,"Failed to create event apr_rc[0x%x]", apr_rc );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                      //CSD_INIT_VOC_COMM_CTL_EVT_CREATE,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    csd_voc_common_keep_thread_alive = TRUE;
    apr_rc = csd_thread_create( (void**) &csd_voc_common_task_handle,
              (char_t*) CSD_VOC_COMMON_TASK_NAME,
              (uint32_t) CSD_VOC_COMMON_TASK_PRIORITY,
              (uint8_t*) &csd_voc_common_thread_stack_base,
              (uint32_t)  CSD_VOC_COMMON_THREAD_STACK_SIZE,
              (csd_thread_fn_t)csd_voc_common_service_task, NULL );
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                      //CSD_INIT_VOC_COMM_THREAD_CREATE,0,0); 
  }
    
  if ( APR_EOK == apr_rc )
  {
    apr_rc = csd_event_wait( csd_voc_common_task_control_event_init );
    csd_event_destroy( csd_voc_common_task_control_event_init );
    csd_voc_common_task_control_event_init = NULL; //reset memory
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                      //CSD_INIT_VOC_COMM_CTL_EVT_WAIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    apr_rc = csd_lock_create( &csd_voc_common_task_lock );
    checkpoint = 1;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                      //CSD_INIT_VOC_COMM_LOCK_CREATE,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    apr_rc = apr_list_init( &csd_voc_common_free_sess_list, csd_voc_common_task_lock_fn, csd_voc_common_task_unlock_fn );
    checkpoint = 2;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                      //CSD_INIT_VOC_COMM_FREE_SESS_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    apr_rc = apr_list_init( &csd_voc_common_active_sess_list, csd_voc_common_task_lock_fn, csd_voc_common_task_unlock_fn );
    checkpoint = 3;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                      //CSD_INIT_VOC_COMM_ACTIVE_SESS_INIT,0,0); 
  }

  if ( APR_EOK == apr_rc )
  {
    for ( i = 0; i < CSD_VOC_COMMON_NUM_SESSIONS; ++i )
    {
      apr_rc = apr_list_add_tail( &csd_voc_common_free_sess_list,
                              ( ( apr_list_node_t* ) &csd_voc_common_session_pool[i] ) );
    }
    checkpoint = 4;
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_INIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
     //                 CSD_INIT_VOC_COMM_FREE_SESS_INIT,0,0); 
  }

  if ( APR_EOK != apr_rc )
  {
    /* free resources here*/
    switch ( checkpoint ) 
    {
      case 4:
        ( void ) apr_list_destroy ( &csd_voc_common_free_sess_list );
        //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
          //              CSD_INIT_VOC_COMM_FREE_SESS_INIT,0,0); 
      case 3:
        ( void ) apr_list_destroy ( &csd_voc_common_active_sess_list );
        /*csd voc common active session list destroy done*/
        //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
            //            CSD_INIT_VOC_COMM_ACTIVE_SESS_INIT,0,0); 
      case 2:
        ( void ) csd_lock_destroy ( csd_voc_common_task_lock );
        /*csd voc common lock destroy done*/
        //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
              //          CSD_INIT_VOC_COMM_LOCK_CREATE,0,0); 
      case 1:
        ( void ) csd_thread_destroy ( csd_voc_common_task_handle );
        /*csd voc common thread destroy done*/
        //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
                //        CSD_INIT_VOC_COMM_THREAD_CREATE,0,0); 
        break;
      default:
          break;
    }
  }

  rc = csd_map_apr_error_to_csd( apr_rc );
  return rc;
}

/*
 * FUNCTION: csd_voc_common_deinit

 * PARAMETERS: None

 * CONTEXT: Clients context at bootup time

 * SIDE EFFECTS: Destroys thread .

 * RETURN:    csd status
*/
uint32_t csd_voc_common_deinit() 
{
  uint32_t apr_rc = APR_EOK;
  int32_t rc;
  
  if ( APR_EOK == apr_rc && csd_voc_common_task_control_event_deinit == NULL )
  {
    apr_rc = csd_event_create( &csd_voc_common_task_control_event_deinit );
    if ( apr_rc != APR_EOK )
    {
      CSD_MSG(CSD_PRIO_ERROR,"Failed to create event rc[0x%x]", apr_rc );
    }
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                  //    CSD_INIT_VOC_COMM_DEINIT_EVT_CREATE,0,0);
  }
  
  //signal the thread so it initiates the kill procedure
  csd_voc_common_keep_thread_alive = FALSE; 
  if( csd_voc_common_event_handle )
  {
    csd_event_signal( csd_voc_common_event_handle); //send event to the thread so it will end
  }
	
  //destroy all events
  if( csd_voc_common_task_control_event_deinit )
  {
    csd_event_wait( csd_voc_common_task_control_event_deinit );
    /*csd voc common control event wait done*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
                    //CSD_INIT_VOC_COMM_CTL_EVT_WAIT,0,0); 
    csd_event_destroy( csd_voc_common_task_control_event_deinit );
    /*csd voc common control event wait done*/
    //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
                    //CSD_INIT_VOC_COMM_DEINIT_EVT_DESTROY,0,0); 
  }
  
  //once the thread finished running destroy the thread handle.
  apr_rc = csd_event_signal_abortall ( csd_voc_common_event_handle );
  csd_thread_destroy( csd_voc_common_task_handle);
  /*csd voc common thread destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
                  //CSD_INIT_VOC_COMM_THREAD_CREATE,0,0); 
  apr_list_destroy( &csd_voc_common_free_sess_list);
  /*csd voc common free session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
                  //CSD_INIT_VOC_COMM_FREE_SESS_INIT,0,0); 
  apr_list_destroy( &csd_voc_common_active_sess_list);
  /*csd voc common active session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,APR_EOK,CSD_VOC_COMM_INIT_STAGE,
                  //CSD_INIT_VOC_COMM_ACTIVE_SESS_INIT,0,0); 
  apr_rc = csd_lock_destroy( csd_voc_common_task_lock );
  /*csd voc common active session list destroy done*/
  //CSD_DEBUG_INIT_LOG(CSD_DEBUG_DEINIT_STATE,apr_rc,CSD_VOC_COMM_INIT_STAGE,
                    //CSD_INIT_VOC_COMM_LOCK_CREATE,0,0); 

  //reset memory
  csd_voc_common_task_control_event_deinit = NULL;
  csd_voc_common_task_handle = NULL;
  csd_voc_common_task_lock = NULL;
  rc = csd_map_apr_error_to_csd( apr_rc );
  return rc;
}

/*
 * FUNCTION: csd_voc_common_get_session

 * PARAMETERS: state_fn & command fn for the session

 * CONTEXT:  CVC task context

 * SIDE EFFECTS: Reserves a csd_voc_common session for client

 * RETURN:    csd status & csd_voc_common handle
*/
uint32_t csd_voc_common_get_session(
                         csd_voc_common_cmd_fn_t       cmd_fn,
                         uint32_t            *h_csd_voc_common
                         )
{

  uint32_t apr_rc  = APR_EOK;
  int32_t rc = CSD_EOK;
  csd_voc_common_session_list_t* work_item;

  apr_rc = apr_list_remove_head( &csd_voc_common_free_sess_list,
                             ( ( apr_list_node_t** ) &work_item ) );
  if ( apr_rc == APR_EOK )
  {
    ( void ) apr_list_add_tail( &csd_voc_common_active_sess_list, &work_item->link );
    *h_csd_voc_common = (uint32_t)&work_item->sess;
  }
  else
  {
    *h_csd_voc_common = ( uint32_t ) NULL;
  }

  if ( work_item )
  {
    work_item->sess.cmd_fn = cmd_fn;
    work_item->sess.sig = CSD_VOC_COMMON_SIG_NONE;
    rc = CSD_EOK;
  }
  else
  {
    CSD_MSG(CSD_PRIO_ERROR,"Failed " );
    rc = CSD_EFAILED;
  }
  return rc;
}

/*
 * FUNCTION: csd_voc_common_free_session

 * PARAMETERS: csd_voc_common session

 * CONTEXT: CVC Task

 * SIDE EFFECTS: Free csd_voc_common session

 * RETURN:    csd status
*/
uint32_t csd_voc_common_free_session( uint32_t *h_csd_voc_common )
{
  csd_voc_common_session_list_t* psess_pivot;
  csd_voc_common_session_list_t* psess;

  psess = NULL;

  ( void ) csd_voc_common_task_lock_fn ();

  if ( !( apr_list_peak_head ( &csd_voc_common_active_sess_list, ( apr_list_node_t ** )&psess ) ) )
  {
    psess_pivot = psess;

    do
    {
      if ( & ( psess->sess ) == ( csd_voc_common_session_handle_t* )*h_csd_voc_common )
      {
        ( void ) apr_list_delete( &csd_voc_common_active_sess_list, &psess->link);
        ( void ) apr_list_add_tail( &csd_voc_common_free_sess_list, &psess->link );
        ( void ) csd_voc_common_task_unlock_fn ();
        return CSD_EOK;
      }
      else
      {
        psess_pivot = psess; /* update the pivot pointer */
      }

    } while ( ! ( apr_list_get_next ( &csd_voc_common_active_sess_list,
                                    ( apr_list_node_t* )psess_pivot,
                                    ( apr_list_node_t** )&psess ) ) );
  }
  ( void ) csd_voc_common_task_unlock_fn ();

  return CSD_EFAILED;
}

/*
 * FUNCTION: csd_voc_common_queue_cmd

 * PARAMETERS: csd_voc_common handle and signal

 * CONTEXT: any context

 * SIDE EFFECTS: CVC task wakes up

 * RETURN:    csd status
*/
uint32_t csd_voc_common_queue_cmd(
                       uint32_t  h_csd_voc_common,
                       csd_voc_common_sig_enum_t sigs
                       )
{
  uint32_t rc = CSD_EOK;

  while( 1 )
  {
    if ( !h_csd_voc_common )
    {
        CSD_MSG(CSD_PRIO_ERROR,"csd_voc_common_queue_cmd: h_csd_voc_common is NULL" );
        rc = CSD_EFAILED;
        break;
    }

    ( ( csd_voc_common_session_handle_t* ) h_csd_voc_common )->sig |= sigs;
    if( csd_voc_common_event_handle )
    {
      ( void ) csd_event_signal( csd_voc_common_event_handle );
    }
    break;
  }

  return rc;
}
/*
 * FUNCTION: csd_voc_common_check_device_pair

 * PARAMETERS: buffer pointer

 * CONTEXT:  VC Task

 * SIDE EFFECTS: 

 * RETURN:    uint32_t rc
*/
uint32_t csd_voc_common_check_device_pair(
  csd_voc_common_check_dev_pair_t *data )
{
  int32_t acdb_rc = ACDB_SUCCESS;
  uint32_t rc = CSD_EOK;
  AcdbDevicePairType paircmd;
  AcdbDevicePairingResponseType pair_rsp;

  paircmd.nRxDeviceId = data->rx_device_id;
  paircmd.nTxDeviceId = data->tx_device_id;

  CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_check_device_pair: Invoking ACDB_CMD_IS_DEVICE_PAIRED," 
                  "tx_dev_id[0x%x], rx_dev_id[0x%x]",
                  paircmd.nTxDeviceId, paircmd.nRxDeviceId );

  acdb_rc = acdb_ioctl( ACDB_CMD_IS_DEVICE_PAIRED,
                        ( const uint8_t* )&paircmd, 
                        sizeof( AcdbDevicePairType ),
                        ( uint8_t* )&pair_rsp, 
                        sizeof( AcdbDevicePairingResponseType ) ); 

  if( acdb_rc != ACDB_SUCCESS )
  {
    CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_check_device_pair: ACDB_CMD_IS_DEVICE_PAIRED failed, acdb_rc[%d]",
             acdb_rc );
    rc = csd_map_acdb_error_to_csd(acdb_rc);
  }  
  else if( pair_rsp.ulIsDevicePairValid != 0x00000001 )
  {
    CSD_MSG( CSD_PRIO_ERROR, "csd_voc_common_check_device_pair: ACDB_CMD_IS_DEVICE_PAIRED failed, ulIsDevicePairValid[%d]",
             pair_rsp.ulIsDevicePairValid );
    rc = CSD_EFAILED;
  }
  else
  {
    CSD_MSG( CSD_PRIO_MEDIUM,"csd_voc_common_check_device_pair: ACDB_CMD_IS_DEVICE_PAIRED success,"
                     "rx_dev_id[0x%x], tx_dev_id[0x%x]", 
                     paircmd.nRxDeviceId, paircmd.nTxDeviceId );
  }
  return rc;
}
/*
 * FUNCTION: csd_voc_common_get_calibration_size

 * PARAMETERS: buffer pointer

 * CONTEXT:  CVC Task

 * SIDE EFFECTS: reserves memory for this query from static pool

 * RETURN:    uint32_t size
*/

uint32_t csd_voc_common_get_calibration_size(
  csd_voc_common_get_oemdb_size_enum_t  type,
  csd_voc_common_get_oemdb_t       *data )
{
  uint32_t cal_size = 0;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbSizeResponseType size_rsp;

  switch ( type )
  {
    case CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_SIZE:
    {
      AcdbVocProcCmnTblSizeCmdType cmd;
      
      if ( data == NULL ) 
      {
        break;
      }

      cmd.nTxDeviceId = data->common.tx_device_id;
      cmd.nRxDeviceId = data->common.rx_device_id;
      cmd.nTxDeviceSampleRateId	= data->common.tx_samp_rate;
      cmd.nRxDeviceSampleRateId = data->common.rx_samp_rate;
            
      CSD_MSG(CSD_PRIO_MEDIUM,"csd_voc_common_get_calibration_size: trying ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE"
                   ",tx_dev_id[0x%x], rx_dev_id[0x%x], tx_sr[%d], rx_sr[%d]",
                   cmd.nTxDeviceId, cmd.nRxDeviceId, cmd.nTxDeviceSampleRateId,
                   cmd.nRxDeviceSampleRateId );

        /* try to get the size of the static table if this fails fall back*/
      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE,
                           ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&size_rsp,
                            sizeof( size_rsp ) );
                       
      if( acdb_rc == ACDB_SUCCESS && size_rsp.nSize > 0 )
      {
        cal_size = size_rsp.nSize;
        CSD_MSG( CSD_PRIO_MEDIUM,"csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE"
                     "success, size[0x%x]", cal_size );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_PROC_STATIC_TABLE_SIZE"
                     "failed, acdb_rc[%d]", acdb_rc );
      }
    }
    break;
  
    case CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_SIZE:
    {
      AcdbVocProcGainDepVolTblSizeV2CmdType cmd;  
      
      if( data == NULL ) 
      {
        break;
      } 
      
      cmd.nTxDeviceId = data->volume.tx_device_id;
      cmd.nRxDeviceId = data->volume.rx_device_id;
      cmd.nFeatureId = data->volume.feature_id;
            
         /* try to get the size of the static table if this fails fall back*/
      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE,
                           ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&size_rsp,
                            sizeof( size_rsp ) );
  
      if( acdb_rc == ACDB_SUCCESS && size_rsp.nSize > 0 ) 
      {
        cal_size = size_rsp.nSize;
        CSD_MSG( CSD_PRIO_MEDIUM, 
                    "csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE success,"
                    "size[0x%x]", cal_size );
      }
      else
      {
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE_SIZE"
                    "failed, acdb_rc[%d]", acdb_rc );
      }
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_SIZE:
    {
      /* try to get the size of the static table if this fails fall back*/
      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE,
                            NULL,
                            0,
                            ( uint8_t* )&size_rsp,
                            sizeof( size_rsp ) );
  
      if( acdb_rc == ACDB_SUCCESS && size_rsp.nSize > 0 ) 
      {
        cal_size = size_rsp.nSize;
        CSD_MSG( CSD_PRIO_MEDIUM, 
                      "csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE success,"
                      "size[0x%x]", cal_size );
      }
      else 
      {
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE_SIZE "
                      "failed, acdb_rc[%d]", acdb_rc );
      }
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_DEV_CFG_SIZE:
    {
      AcdbVocProcDevCfgSizeCmdType cmd;

      cmd.nRxDeviceId = data->dev_cfg.rx_device_id;
      cmd.nTxDeviceId = data->dev_cfg.tx_device_id;

      CSD_MSG(CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration_size: Invoking ACDB_CMD_GET_VOC_PROC_DEVICE_CFG_SIZE"
                     ",tx_dev_id[0x%x], rx_dev_id[0x%x]",
                     cmd.nTxDeviceId, cmd.nRxDeviceId );

      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_PROC_DEVICE_CFG_SIZE,
                            ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&size_rsp,
                            sizeof( size_rsp ) );
                            
      if( acdb_rc == ACDB_SUCCESS && size_rsp.nSize > 0 ) 
      {
        cal_size = size_rsp.nSize;
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_PROC_DEVICE_CFG_SIZE"
                       "success, size[0x%x]", cal_size );
      }
      else 
      {
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOC_PROC_DEVICE_CFG_SIZE"
                      "failed, acdb_rc[%d]", acdb_rc );
      }
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_VOL_TABLE_STEP_SIZE:
    {
      AcdbVolTblStepSizeRspType vol_table_size;

      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOL_TABLE_STEP_SIZE,
                            NULL,
                            0,
                            ( uint8_t* )&vol_table_size,
                            sizeof( vol_table_size ) );
                          
      if( acdb_rc == ACDB_SUCCESS && vol_table_size.VocProcVolTblStepSize > 0 )
      {
        cal_size = vol_table_size.VocProcVolTblStepSize;
        CSD_MSG( CSD_PRIO_MEDIUM, 
                  "csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOL_TABLE_STEP_SIZE success, size[0x%x]", 
                         cal_size );
      }
      else 
      {
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration_size: ACDB_CMD_GET_VOL_TABLE_STEP_SIZE failed,"
                   "acdb_rc[%d]", acdb_rc );
      }
    }
    break;

    default:
      CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration_size: type[0x%x] unsupported", type );
    break;
  }

  return cal_size;

};

uint32_t csd_voc_common_get_calibration(
  csd_voc_common_get_oemdb_enum_t  type,
  csd_voc_common_get_oemdb_t       *data,
  uint8_t                           *buf_ptr,  /* [ in ] Buffer pointer */
  uint32_t                         buf_size,   /* [ in ] Buffer size */
  uint32_t                         *write_size /* [ out ]size of calibration written */
)
/* TO-DO! check NULL and also check size*/
{
  AcdbQueryResponseType rsp;
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  uint32_t cmd_type;

  switch ( type )
  {
    case CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC:
    {
      AcdbQueryCmdType  cmd;

      cmd.nBufferLength = ( uint32_t )buf_size ;
      cmd.pBufferPointer = ( uint8_t * )buf_ptr;

      CSD_MSG( CSD_PRIO_MEDIUM, 
               "csd_voc_common_get_calibration: Invoking ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE");
      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE,
                            ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&rsp,
                            sizeof( rsp ) );

      if( ( acdb_rc == ACDB_SUCCESS ) && ( rsp.nBytesUsedInBuffer > 0 ) )  
      {
        *write_size = rsp.nBytesUsedInBuffer;
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE"
                     "success, buffer size used[0x%x]", *write_size );
      }
      else 
      {
        *write_size = 0;
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_STREAM_STATIC_TABLE failed,"
                       "acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd( acdb_rc );
      }
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC:
    {
      AcdbVocProcCmnTblCmdType  cmd;

      if ( data == NULL ) 
      {
        return CSD_EFAILED;
      }

      cmd.nTxDeviceId = data->common.tx_device_id;
      cmd.nRxDeviceId = data->common.rx_device_id;
      cmd.nTxDeviceSampleRateId	= data->common.tx_samp_rate;
      cmd.nRxDeviceSampleRateId = data->common.rx_samp_rate;
      cmd.nBufferPointer = ( uint8_t* )buf_ptr;
      cmd.nBufferLength = ( uint32_t )buf_size;

      cmd_type = ACDB_CMD_GET_VOC_PROC_STATIC_TABLE;
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration: Invoking ACDB_CMD_GET_VOC_PROC_STATIC_TABLE,"
               "tx_dev_id[0x%x], rx_dev_id[0x%x], tx_sr[%d], rx_sr[%d]",
               cmd.nTxDeviceId, cmd.nRxDeviceId, cmd.nTxDeviceSampleRateId,
               cmd.nRxDeviceSampleRateId );

      acdb_rc = acdb_ioctl( cmd_type,
                            ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&rsp,
                            sizeof( rsp ) );

      if( ( acdb_rc == ACDB_SUCCESS ) && ( rsp.nBytesUsedInBuffer > 0 ) ) 
      {
        *write_size = rsp.nBytesUsedInBuffer;
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_PROC_STATIC_TABLE success,"
                          " buffer size used[0x%x]", *write_size );
      }
      else 
      {
        *write_size = 0;
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_PROC_STATIC_TABLE failed,"
                           "acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd(acdb_rc);
      }

    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC:
    {
      AcdbVocProcGainDepVolTblV2CmdType  cmd;

      if ( data == NULL ) 
      {
        return CSD_EFAILED;
      }

      cmd.nTxDeviceId = data->volume.tx_device_id;
      cmd.nRxDeviceId  = data->volume.rx_device_id;
      cmd.nFeatureId = data->volume.feature_id;
      cmd.nBufferPointer = ( uint8_t* )buf_ptr;
      cmd.nBufferLength = ( uint32_t )buf_size;

      cmd_type = ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE;
      CSD_MSG(CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration: Invoking ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE"
                       ",tx_dev_id[0x%x], rx_dev_id[0x%x], FeatureId[0x%x]",
                       cmd.nTxDeviceId, cmd.nRxDeviceId, cmd.nFeatureId );

      acdb_rc = acdb_ioctl( cmd_type,
                            ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&rsp,
                            sizeof( rsp ) );

      if( ( acdb_rc == ACDB_SUCCESS ) && ( rsp.nBytesUsedInBuffer > 0 ) ) 
      {
        *write_size = rsp.nBytesUsedInBuffer;
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE"
                         "success, buffer size used[0x%x]", *write_size );
      }
      else
      {
        *write_size = 0;
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_PROC_DYNAMIC_TABLE failed,"
                         "acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd(acdb_rc);
      }
    }
    break;
    
    case CSD_VOC_COMMON_GET_OEMDB_DEV_CFG:
    {
      AcdbVocProcDevCfgCmdType  cmd;
      
      if ( data == NULL ) 
      {
        return CSD_EFAILED;
      }

      cmd.nTxDeviceId = data->dev_cfg.tx_device_id;
      cmd.nRxDeviceId = data->dev_cfg.rx_device_id;
      cmd.pBuff = ( uint8_t* )( buf_ptr );
      cmd.nBufferLength = ( uint32_t )( buf_size );

      CSD_MSG( CSD_PRIO_MEDIUM, 
                    "csd_voc_common_get_calibration: Invoking ACDB_CMD_GET_VOC_PROC_DEVICE_CFG, tx_dev_id[0x%x],"
                    "rx_dev_id[0x%x]", cmd.nTxDeviceId, cmd.nRxDeviceId );

      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_PROC_DEVICE_CFG,
                            ( const uint8_t* )&cmd,
                            sizeof( cmd ),
                            ( uint8_t* )&rsp,
                            sizeof( rsp ) );

      if( ( acdb_rc == ACDB_SUCCESS ) && ( rsp.nBytesUsedInBuffer > 0 ) )  
      {
        *write_size = rsp.nBytesUsedInBuffer;
        CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_PROC_DEVICE_CFG success,"
                       "buffer size used[0x%x]", *write_size );
      }
      else 
      {
        *write_size = 0;
        CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration: ACDB_CMD_GET_VOC_PROC_DEVICE_CFG failed,"
                          "acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd(acdb_rc);
      }
    }
    break;
    
    default:
      CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_calibration: type[0x%x] unsupported ", type );
      rc = CSD_EUNSUPPORTED;
    break;
  } /* switch type */

  return rc;
}

uint32_t csd_voc_common_get_old_cal_column_info_size(
  csd_voc_common_get_oemdb_column_info_size_enum_t  type,
  AcdbSizeResponseType *size_rsp
)
{
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbVocColumnsInfoSizeCmdType cmd;
  
  switch ( type )
  {
    case CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO_SIZE:
    {           
      cmd.nTableId = ACDB_VOC_PROC_TABLE;
    }
    break;
  
    case CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO_SIZE:
    {
      cmd.nTableId = ACDB_VOC_PROC_VOL_TABLE;
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO_SIZE:
    {
      cmd.nTableId = ACDB_VOC_STREAM_TABLE;
    }
    break;

    default:
    break;
  }

  CSD_MSG( CSD_PRIO_MEDIUM,"csd_voc_common_get_old_cal_column_info_size: Invoking ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE,"
           "table_id[%d]", cmd.nTableId );

  acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE,
                        ( const uint8_t* )&cmd,
                        sizeof( cmd ),
                        ( uint8_t* )size_rsp,
                        sizeof( AcdbSizeResponseType ) );
  return  acdb_rc;
}

uint32_t csd_voc_common_get_cal_column_info_size(
  csd_voc_common_get_oemdb_column_info_size_enum_t  type,
  uint32_t rx_dev_id,
  uint32_t tx_dev_id
)
{
  int32_t acdb_rc = ACDB_SUCCESS;
  uint32_t cal_size = 0;
  AcdbSizeResponseType size_rsp;

  AcdbVocColumnsInfoSizeCmdType_v2 cmd;

  cmd.nRxDeviceId = rx_dev_id;
  cmd.nTxDeviceId = tx_dev_id;
  
  switch ( type )
  {
    case CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO_SIZE:
    {           
      cmd.nTableId = ACDB_VOC_PROC_STAT_TABLE_V2;
    }
    break;
  
    case CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO_SIZE:
    {
      cmd.nTableId = ACDB_VOC_PROC_DYN_TABLE_V2 ;
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO_SIZE:
    {
      cmd.nTableId = ACDB_VOC_STREAM2_TABLE_V2;
    }
    break;

    default:
    break;
  }

  CSD_MSG( CSD_PRIO_MEDIUM,"csd_voc_common_get_cal_column_info_size: Invoking ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE_V2,"
           "table_id[%d]", cmd.nTableId );

  acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE_V2,
                        ( const uint8_t* )&cmd,
                        sizeof( cmd ),
                        ( uint8_t* )&size_rsp,
                        sizeof( size_rsp ) );
  
                        
  if( acdb_rc == ACDB_SUCCESS )
  {
    cal_size = size_rsp.nSize;
    CSD_MSG( CSD_PRIO_MEDIUM,"csd_voc_common_get_cal_column_info_size: ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE success,"
                 "size[0x%x]", cal_size );
  }
  else
  {
    CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_cal_column_info_size: ACDB_CMD_GET_VOC_COLUMNS_INFO_SIZE failed,"
                 "acdb_rc[%d]", acdb_rc );
  }

  return ( cal_size - sizeof ( uint32_t ) );

};

uint32_t csd_voc_common_get_old_cal_column_info(  csd_voc_common_get_oemdb_column_info_enum_t  type,
  uint8_t                                      *buf_ptr,  /* [ in ] Buffer pointer */
  uint32_t                                     buf_size,   /* [ in ] Buffer size */
  uint32_t                                     *write_size, /* [ out ]size of calibration written */
  AcdbQueryResponseType                        *rsp
)
{

  uint32_t acdb_rc = ACDB_SUCCESS;
  AcdbVocColumnsInfoCmdType cmd;

  cmd.pBuff = buf_ptr;
  cmd.nBufferLength = buf_size;

  switch ( type )
  {
    case CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO:
    {
      cmd.nTableId = ACDB_VOC_STREAM_TABLE;
    }
    break;  

    case CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO:
    {
      cmd.nTableId = ACDB_VOC_PROC_TABLE;
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO:
    {
      cmd.nTableId = ACDB_VOC_PROC_VOL_TABLE;
    }
    break;
 
    default:
    break;
  } /* switch type */

  CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_old_cal_column_info: Invoking ACDB_CMD_GET_VOC_COLUMNS_INFO,"
           "table_id[%d]", cmd.nTableId );

  acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_COLUMNS_INFO,
                   ( const uint8_t* )&cmd,
                   sizeof( cmd ),
                   ( uint8_t* )rsp,
                   sizeof( AcdbQueryResponseType ));

  return acdb_rc;

}


uint32_t csd_voc_common_get_cal_column_info(
  csd_voc_common_get_oemdb_column_info_enum_t  type,
  uint8_t                                      *buf_ptr,  /* [ in ] Buffer pointer */
  uint32_t                                     buf_size,   /* [ in ] Buffer size */
  uint32_t                                     *write_size, /* [ out ]size of calibration written */
  uint32_t                                      rx_dev_id, /* rx device id*/
  uint32_t                                      tx_dev_id /* tx device id*/
)
{
  uint32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbQueryResponseType rsp;

  AcdbVocColumnsInfoCmdType_v2 cmd;

  cmd.pBuff = buf_ptr;
  cmd.nBufferLength = buf_size;
  cmd.nRxDeviceId = rx_dev_id;
  cmd.nTxDeviceId = tx_dev_id;

  switch ( type )
  {
    case CSD_VOC_COMMON_GET_OEMDB_STREAM_STATIC_COLUMN_INFO:
    {   
        cmd.nTableId = ACDB_VOC_STREAM2_TABLE_V2;      
    }
    break;  

    case CSD_VOC_COMMON_GET_OEMDB_VOC_STATIC_COLUMN_INFO:
    {
        cmd.nTableId = ACDB_VOC_PROC_STAT_TABLE_V2; 
    }
    break;

    case CSD_VOC_COMMON_GET_OEMDB_VOC_DYNAMIC_COLUMN_INFO:
    {
         cmd.nTableId = ACDB_VOC_PROC_DYN_TABLE_V2;
    }
    break;
 
    default:
    break;
  } /* switch type */

  CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_cal_column_info: Invoking ACDB_CMD_GET_VOC_COLUMNS_INFO_V2,"
           "table_id[%d]", cmd.nTableId );

  acdb_rc = acdb_ioctl( ACDB_CMD_GET_VOC_COLUMNS_INFO_V2,
                   ( const uint8_t* )&cmd,
                   sizeof( cmd ),
                 ( uint8_t* )&rsp,
                 sizeof( rsp ) );
  
  if ( acdb_rc == ACDB_SUCCESS ) 
  {
    *write_size = rsp.nBytesUsedInBuffer;
    CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_common_get_cal_column_info: ACDB_CMD_GET_VOC_COLUMNS_INFO_V2 success,"
                  "buffer size used[0x%x]", *write_size );
  } 
  else
  {
    *write_size = 0;
    CSD_MSG( CSD_PRIO_ERROR,"csd_voc_common_get_cal_column_info: ACDB_CMD_GET_VOC_COLUMNS_INFO_V2 failed,"
                   "acdb_rc[%d]", acdb_rc );
    rc = csd_map_acdb_error_to_csd( acdb_rc );
  }

  return rc;
}

int32_t csd_voc_common_process_vp3(
  csd_voc_common_process_vp3_oemdb_enum_t type,
  csd_voc_common_process_oemdb_vp3_t cmd,
  uint8_t* buf_ptr,
  uint32_t mem_size,
  uint32_t* write_size )
{
  int32_t rc = CSD_EOK;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbQueryResponseType rsp; 
  static csd_voc_common_vp3_list_t list = {0};
  
  switch( type )
  {

    case CSD_VOC_COMMON_GET_OEMDB_VP3_INFO:
    { /* Support only 1 pair of mid/pid */
      AcdbVP3MidPidListCmdType acdb_cmd;
      acdb_cmd.nTxDeviceId = cmd.tx_device_id;
      acdb_cmd.nRxDeviceId = cmd.rx_device_id;
      acdb_cmd.nUseCaseId = ACDB_VP3_VOICE_USECASE;
      acdb_cmd.nBufferLength = sizeof(csd_voc_common_vp3_list_t); 
      acdb_cmd.nBufferPointer = (uint8_t*)&list;
      
      CSD_MSG( CSD_PRIO_MEDIUM, "Invoking ACDB_CMD_GET_VP3_MIDPID_LIST, nTxDevId[0x%x], nRxDevId[0x%x]"
               ", UseCaseId[%d]", cmd.tx_device_id, cmd.rx_device_id, ACDB_VP3_VOICE_USECASE );
               
      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VP3_MIDPID_LIST, (const uint8_t*)&acdb_cmd,
                            sizeof(acdb_cmd), (uint8_t*)&rsp, sizeof(rsp) );
                          
      if( acdb_rc != ACDB_SUCCESS )
      {
        CSD_MSG( CSD_PRIO_ERROR, "ACDB_CMD_GET_VP3_MIDPID_LIST failed, acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd( acdb_rc );
      }  
      else if( rsp.nBytesUsedInBuffer == 0 )
      {
        CSD_MSG( CSD_PRIO_ERROR, "VP3 list not available in ACDB" );
        rc = CSD_EFAILED;
      }
      else
      {
        CSD_MSG( CSD_PRIO_MEDIUM, "ACDB_CMD_GET_VP3_MIDPID_LIST success, mid[0x%x], pid[0x%x]", list.mid, list.pid );
      }      
    }
    break;
    
    case CSD_VOC_COMMON_GET_OEMDB_VP3_DATA:
    {
      AcdbVP3CmdType acdb_cmd;
      acdb_cmd.nTxDeviceId = cmd.tx_device_id;
      acdb_cmd.nRxDeviceId = cmd.rx_device_id;
      acdb_cmd.nUseCaseId = ACDB_VP3_VOICE_USECASE;
      acdb_cmd.nModuleId = list.mid;
      acdb_cmd.nParamId = list.pid;
      acdb_cmd.nBufferLength = mem_size; 
      acdb_cmd.nBufferPointer = buf_ptr;
      
      CSD_MSG( CSD_PRIO_MEDIUM,  "Invoking ACDB_CMD_GET_VP3_DATA, nTxDevId[0x%x], nRxDevId[0x%x]"
               ", UseCaseId[%d]", acdb_cmd.nTxDeviceId, acdb_cmd.nRxDeviceId, ACDB_VP3_VOICE_USECASE );
      
      acdb_rc = acdb_ioctl( ACDB_CMD_GET_VP3_DATA, (const uint8_t*)&acdb_cmd,
                            sizeof(acdb_cmd), (uint8_t*)&rsp, sizeof(rsp) );
                          
      if( acdb_rc != ACDB_SUCCESS )
      {
        CSD_MSG( CSD_PRIO_ERROR,"ACDB_CMD_GET_VP3_DATA failed, acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd( acdb_rc );
      }  
      else if( rsp.nBytesUsedInBuffer == 0 )
      {
        CSD_MSG( CSD_PRIO_ERROR, "VP3 data not available in ACDB" );
        rc = CSD_EFAILED;
      }
      else
      {
        *write_size = rsp.nBytesUsedInBuffer;
        CSD_MSG( CSD_PRIO_MEDIUM, "ACDB_CMD_GET_VP3_DATA success, buffer size used[0x%x]", *write_size );
      }      
    }
    break;
    
    case CSD_VOC_COMMON_SET_OEMDB_VP3_DATA:
    {
      AcdbVP3CmdType acdb_cmd;
      acdb_cmd.nTxDeviceId = cmd.tx_device_id;
      acdb_cmd.nRxDeviceId = cmd.rx_device_id;
      acdb_cmd.nUseCaseId = ACDB_VP3_VOICE_USECASE;
      acdb_cmd.nModuleId = list.mid;
      acdb_cmd.nParamId = list.pid;
      acdb_cmd.nBufferLength = mem_size; 
      acdb_cmd.nBufferPointer = buf_ptr;
      
      CSD_MSG( CSD_PRIO_MEDIUM,  "Invoking ACDB_CMD_SET_VP3_DATA, nTxDevId[0x%x], nRxDevId[0x%x]"
               ", UseCaseId[%d]", acdb_cmd.nTxDeviceId, acdb_cmd.nRxDeviceId, ACDB_VP3_VOICE_USECASE );
      
      acdb_rc = acdb_ioctl( ACDB_CMD_SET_VP3_DATA, (const uint8_t*)&acdb_cmd,
                            sizeof(AcdbVP3CmdType), NULL, 0 );
                         
      if( acdb_rc != ACDB_SUCCESS )
      {
        CSD_MSG( CSD_PRIO_ERROR, "ACDB_CMD_SET_VP3_DATA failed, acdb_rc[%d]", acdb_rc );
        rc = csd_map_acdb_error_to_csd( acdb_rc );
      }  
      else
      {
        CSD_MSG( CSD_PRIO_MEDIUM, "ACDB_CMD_SET_VP3_DATA success" );
      }      
    }
    break;

    default:
      CSD_MSG( CSD_PRIO_ERROR, "Not supported type[%d]", type );
      rc = CSD_EUNSUPPORTED;
    break;
  }
  
  return rc;
}

void* csd_voc_common_obtain_session_from_handle ( int32_t csd_handle, apr_objmgr_t* objmgr_obj ) /* to rename */
{
  void* psession = NULL;
  apr_objmgr_object_t* apr_obj = NULL;
  int32_t* papr_handle = NULL;

  papr_handle = csd_handle_get_obj ( csd_handle );

  if ( papr_handle != NULL) {
    ( void ) apr_objmgr_find_object ( objmgr_obj, *papr_handle, &apr_obj );
    if ( apr_obj != NULL ) {
      psession = apr_obj->any.ptr;
    }
  }
  return psession;
}

int32_t csd_voc_commmon_is_valid_sample_rate
(
  uint32_t dev_id, uint32_t sample_rate
)
{
  return csd_dev_is_valid_sample_rate(dev_id,sample_rate);
}

int32_t csd_voc_commmon_remove_device_info
(
  uint32_t dev_id
)
{
  return csd_dev_remove_dev_info(dev_id);
}


int32_t csd_voc_commmon_get_active_afe_port_id
(
  uint32_t dev_id,
  uint16_t *afe_port_id
)
{
  return csd_dev_get_active_afe_port_id(dev_id,afe_port_id);
}

/*
 * FUNCTION: csd_voc_commmon_get_device_info

 * DESCRIPTION: Get device info from active device records or prefetch a device
 * for later use

 * DEPENDENCIES: Has to be complimented with csd_voc_commmon_remove_device_info
 * once device info is no longer valid/required

 * PARAMETERS: dev_id

 * CONTEXT: any context

 * RETURN:  *topology_id, *afe_port_id
*/
int32_t csd_voc_commmon_get_device_info
(
  uint32_t dev_id,
  uint32_t *topology_id,
  uint16_t *afe_port_id
)
{
  int32_t rc = CSD_EBADPARAM;
  int32_t acdb_rc = ACDB_SUCCESS;
  AcdbGetVocProcTopIdCmdType acdb_cmd = {0,};
  AcdbGetTopologyIdRspType acdb_cmd_response = {0,};
  csd_dev_info_t device_info;
  
  /*TODO:Unni-Combine topology_id and afe_port_id NULL check in one shoot*/
  if ( NULL != topology_id )
  {
    /* query the device info */

    acdb_cmd.nDeviceId = dev_id;

    CSD_MSG(CSD_PRIO_MEDIUM,"csd_voc_commmon_get_device_info: Invoking ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID,"
                "dev_id[0x%x]", acdb_cmd.nDeviceId );

    /* query acdb */
    if( ACDB_SUCCESS != ( acdb_rc = acdb_ioctl(ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID,
                       ( const byte* )&acdb_cmd,
                       sizeof( acdb_cmd ),
                       ( uint8_t * )&acdb_cmd_response,
                       sizeof( acdb_cmd_response ) ) ) )
    {
      CSD_MSG(CSD_PRIO_ERROR,"csd_voc_commmon_get_device_info: ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID failed"
              "acdb_rc[%d]", acdb_rc);
    }
    else
    {
      *topology_id = acdb_cmd_response.nTopologyId;
      CSD_MSG( CSD_PRIO_MEDIUM, "csd_voc_commmon_get_device_info: ACDB_CMD_GET_VOCPROC_COMMON_TOPOLOGY_ID"
               "success, topology_id[0x%x]", *topology_id );
    }
    rc = csd_map_acdb_error_to_csd( acdb_rc );
  }
  
  if ( NULL != afe_port_id )
  {
    rc = csd_dev_get_dev_info ( dev_id, &device_info );
    if ( CSD_EOK ==  rc )
    {
      *afe_port_id = device_info.afe_port_id;
    }
    else
    {
      CSD_MSG(CSD_PRIO_ERROR,
        "csd_voc_commmon_get_device_info: Failed to retrieve AFE Port ID\n");
    }
  }

  if( ( NULL != afe_port_id ) && ( NULL != topology_id ) )
  {
    CSD_MSG(CSD_PRIO_LOW,"csd_voc_commmon_get_device_info: dev_id[0x%x], port_id[0x%x], topo_id[0x%x]", dev_id,
                  *afe_port_id,
                  *topology_id );
  }
  
  return rc;
}

int32_t csd_voc_common_populate_memory_table ( uint8_t* pva_memory_table,
                                             uint8_t* p_cal_buf,
                                             uint32_t cal_buf_size,
                                             uint32_t* memory_blocks,
                                             int64_t* pa_cal_buf,
                                             void* dalmem_handle )
{
  int32_t rc = CSD_EOK;
  uint32_t nTraverseSize = 0;
  uint8_t* pTraverse = NULL;
  uint64_t pPhysical = NULL;
  uint32_t nContigLen  = 0;
  uint8_t* pVAMemoryTableTraverse = NULL;
  vss_imemory_table_t* vss_mem_tbl = NULL;
  vss_imemory_block_t* vss_mem_block = NULL;
#if 0  
/* if there are over the maximum number of memory blocks per 4k Page Size */
  uint8_t* pNewVAMemoryTable = NULL;
#endif

  memset( ( void* )pva_memory_table, 0, CSD_MEM_PAGE_SIZE );

  pVAMemoryTableTraverse = pva_memory_table;
  vss_mem_tbl = ( vss_imemory_table_t* ) pVAMemoryTableTraverse;

  /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
  pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_table_descriptor_t );

  /* set the next table description to 0 at the moment */
  vss_mem_tbl->next_table_descriptor.mem_address = 0;
  vss_mem_tbl->next_table_descriptor.mem_size = 0;

  /* determine the amount of memory blocks there are in the non-contiguous memory */
  nTraverseSize = cal_buf_size;
  pTraverse = p_cal_buf;

  while ( nTraverseSize > 0 )/* exit the loop when the entire buffer has been analyzed */
  {
    csd_get_mem_info( dalmem_handle, &pPhysical, &nContigLen );

    if ( *memory_blocks == 0)
    {
      *pa_cal_buf = pPhysical;
    }

    nTraverseSize = nTraverseSize - nContigLen; /* update the left over size to be traversed */
    pTraverse = pTraverse + nContigLen; /* update pointer to the virual address of the next memory block */

    vss_mem_block = ( vss_imemory_block_t* )pVAMemoryTableTraverse;

    /* Populate the memory memory block */
    vss_mem_block->mem_address = pPhysical;
    vss_mem_block->mem_size = nContigLen;

    CSD_MSG( CSD_PRIO_LOW, "PMEM Cal Data Block[#%d], size[%d], address[0x%016llX]",
              *memory_blocks, vss_mem_block->mem_size, vss_mem_block->mem_address );

    pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_block_t ); /* advance to the next memory block */

    (*memory_blocks)++;

#if 0
    /* if there are over the maximum number of memory blocks per 4k Page Size */
    if ( *memory_blocks >= ( ( CSD_MEM_PAGE_SIZE - sizeof ( vss_imemory_table_descriptor_t ) - sizeof ( vss_imemory_table_t* ) / sizeof ( vss_imemory_block_t ) ) ) )
    {
      /* allocate the new memory table */
      csd_mem_alloc( CSD_MEM_PAGE_SIZE, 0, pNewVAMemoryTable );
      /* copy the VA of the next memory table at the end of this current one */
      ( void ) csd_memscpy ( pVAMemoryTableTraverse, sizeof ( vss_imemory_table_t * ), &pNewVAMemoryTable, sizeof ( vss_imemory_table_t * ) );

      /* get the Physical Address for the Memory Table */
      csd_get_mem_info( pNewVAMemoryTable, CSD_MEM_PAGE_SIZE,
              &vss_mem_tbl->next_table_descriptor.mem_address, &nContigLen, dalmem_handle);

      vss_mem_tbl->next_table_descriptor.mem_size = nContigLen;

      pVAMemoryTableTraverse = pNewVAMemoryTable;
      vss_mem_tbl = ( vss_imemory_table_t* ) pVAMemoryTableTraverse;

      /* set the next table description to 0 at the moment */
      vss_mem_tbl->next_table_descriptor.mem_address = 0;
      vss_mem_tbl->next_table_descriptor.mem_size = 0;

      /* advance the Memory Table pointer pass the table descriptor to the vss_imemory_block_t information */
      pVAMemoryTableTraverse = pVAMemoryTableTraverse + sizeof ( vss_imemory_table_descriptor_t );
    }
#endif
  }/* while ( nTraverseSize > 0 ) */
  /* populate the rest of the mapping parameters */

  return rc;
}
/*
 * End External Functions
*/
