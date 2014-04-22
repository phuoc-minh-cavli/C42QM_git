
#include "cmsnsmgr.h"
#include "cmsnsutil.h"
#include "cmpsm.h"
#include "cm_psm_uart.h"
#include "pmapp_pon.h"
#include "pmapp_usb.h"
#include "DDIGPIOInt.h"
#include "DALDeviceId.h"

#ifdef FEATURE_REF_TRACKER 

cm_sns_data_collection_s_type cm_sns_info_local;

cm_sns_motion_event_e_type event_debug ;

#define MAX_SNS_RSP_TIME 10

#ifdef FEATURE_MMODE_TX_21
cm_sns_phy_test_config_s_type cm_sns_test_data_global;
#endif

#ifndef FEATURE_MMODE_TX_21

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
	  cm_snsmgr_state_e_type next_sns_state_g = CM_SNS_COND_CHECK_MOTION_STATE;
#else
	  cm_snsmgr_state_e_type next_sns_state_g = CM_SNS_COND_CHECK_POLICY_MET_STATE;
#endif

#else
  cm_snsmgr_state_e_type next_sns_state_g = CM_SNS_COND_INTR_CHECK_STATE;

#endif

cm_snsmgr_state_mach_e_type state_fptr[CM_SNS_SENSOR_MAX_STATE -1 ] =  {

#ifdef FEATURE_MMODE_TX_21
                                                                                {CM_SNS_COND_INTR_CHECK_STATE,&cmsns_intr_check_state_func},
#endif
	                                                                            {CM_SNS_COND_CHECK_MOTION_STATE,&cmsns_motion_state_func},
																			    {CM_SNS_COND_CHECK_POLICY_MET_STATE,&cmsns_cond_met_state_func},
																			    {CM_SNS_COND_CHECK_BATCH_FULL_MET_STATE,&cmsns_cond_met_state_func},
                                      
                                      {CM_SNS_COND_CHECK_SNS_FAIL_STATE,&cmsns_cond_met_state_func}, 
																			    {CM_SNS_EVALUATION_STATE,&cmsns_evaluation_state_func},
																			    {CM_SNS_SENSOR_REQ_STATE,&cmsns_sensor_req_state_func},
																			    {CM_SNS_SENSOE_DONE_STATE,&cmsns_sensor_done_state_func},
																			   };


/*===========================================================================

FUNCTION cmpsm_wait_for_msec

DESCRIPTION
   wait for msec

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

void cmpsm_wait_for_msec(uint32 msec);


/*===========================================================================
FUNCTION cmsns_get_data_info_ptr

DESCRIPTION
  return the global datastructure

DEPENDENCIES
  none

RETURN VALUE
  pointer to cm_sns_data_collection_s_type

SIDE EFFECTS
  none

===========================================================================*/


cm_sns_data_collection_s_type* cmsns_get_data_info_ptr(void)
{

  return &cm_sns_info_local;
}


#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
/*===========================================================================
FUNCTION cmsns_send_psm_auto_ready_ext_ind

DESCRIPTION
 It will send the auto ready indication to CM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_send_psm_auto_ready_ext_ind(uint32 active_timer,uint8 psm_enter_type)
{
  cm_psm_u_type msgr_rpt_u;
  // return if PSM process in prgress
  if( !((cmsns_get_data_info_ptr()->sns_ready_rsp_state == CM_SNS_PSM_READY_RSP_NONE) 
     || (cmsns_get_data_info_ptr()->sns_ready_rsp_state == CM_SNS_PSM_AUTO_READY_STARTED)))
  {
	 return; 
  }

  memset((void*)&msgr_rpt_u,0,sizeof(cm_psm_u_type));
  msgr_rpt_u.cm_client_psm_auto_ready_ext_ind.msg_hdr.id = MM_CM_PSM_AUTO_READY_EXT_IND;
  msgr_rpt_u.cm_client_psm_auto_ready_ext_ind.activate_timer = active_timer;
  msgr_rpt_u.cm_client_psm_auto_ready_ext_ind.psm_entry_type = psm_enter_type;
  cmsns_get_data_info_ptr()->soi_psm_entry_type = psm_enter_type;
  CM_MSG_HIGH_2("  SNS MGR sending ext auto ready with enter type = %d and timer = %u",
                                                psm_enter_type,active_timer);
  cmsns_get_data_info_ptr()->sns_ready_rsp_state = CM_SNS_PSM_AUTO_READY_SENT;
 
  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= cmsns_send_psm_auto_ready_ext_ind send with PSM enter type  = %d ",
                                                                                      psm_enter_type);
  }	
  cm_msgr_send(MM_CM_PSM_AUTO_READY_EXT_IND,MSGR_MM_CM_PSM,(msgr_hdr_s*)&msgr_rpt_u,sizeof(cm_psm_u_type));
  CM_SERIAL_LOG_2(" =CM= SNS auto_ready_ext_ind timer = %d enter type = %d \n\r",active_timer,psm_enter_type);
  //cmpsm_msgr_cmd_proc((void *) &msgr_rpt_u);
}
#endif


#ifndef FEATURE_BUILD_SENSORONLY_IMAGE

/*===========================================================================
FUNCTION cmsns_send_psm_auto_ready_ind

DESCRIPTION
 It will send the auto ready indication to CM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_send_psm_auto_ready_ind(uint32 active_timer)
{
  cm_psm_u_type msgr_rpt_u;
  memset((void*)&msgr_rpt_u,0,sizeof(cm_psm_u_type));
  msgr_rpt_u.cm_client_psm_auto_ready_ind.activate_timer= active_timer;
  msgr_rpt_u.cm_client_psm_auto_ready_ind.module = CM_PSM_MODULE_SNS_MGR;
  CM_MSG_HIGH_1("  SNS MGR sending auto ready with timer = %d ",active_timer);
  cm_msgr_send(MM_CM_PSM_AUTO_READY_IND,MSGR_MM_CM_PSM,(msgr_hdr_s*)&msgr_rpt_u,sizeof(cm_psm_u_type));
}



/*===========================================================================
FUNCTION cmsns_send_psm_ready_rsp

DESCRIPTION
  it will send psm ready rsp

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/



void cmsns_send_psm_ready_rsp(sys_psm_status_e_type  status,uint32 activate_timer)
{
  cm_psm_u_type msgr_rpt_u;
  cm_sns_data_collection_s_type*	  sns_ptr = cmsns_get_data_info_ptr();
  memset((void*)&msgr_rpt_u,0,sizeof(cm_psm_u_type));
  msgr_rpt_u.cm_client_psm_ready_rsp.msg_hdr.id = MM_CM_PSM_READY_RSP;
  msgr_rpt_u.cm_client_psm_ready_rsp.activate_timer = activate_timer;
  msgr_rpt_u.cm_client_psm_ready_rsp.status = status;
  msgr_rpt_u.cm_client_psm_ready_rsp.module = CM_PSM_MODULE_SNS_MGR;  
  CM_MSG_HIGH_2("  SNS MGR sending ready rsp status = %d and timer = %lu",
                                                status,activate_timer);
  cm_sns_update_psm_state( sns_ptr,CM_SNS_PSM_READY_RSP_SEND);
  cm_msgr_send(MM_CM_PSM_READY_RSP,MSGR_MM_CM_PSM,(msgr_hdr_s*)&msgr_rpt_u,sizeof(cm_psm_u_type));

}

/*===========================================================================
FUNCTION cmsns_send_psm_ready_ind

DESCRIPTION
 It will send the ready ind to CM.
 it is the case where sensor manger rejected the ready request,
 but now ok to proceed. 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_send_psm_ready_ind(uint32 activate_timer)
{

  cm_psm_u_type msgr_rpt_u;
  cm_sns_data_collection_s_type*	  sns_ptr = cmsns_get_data_info_ptr();
  memset((void*)&msgr_rpt_u,0,sizeof(cm_psm_u_type));
 


  memset((void*)&msgr_rpt_u,0,sizeof(cm_psm_u_type));
  msgr_rpt_u.cm_client_psm_ready_ind.msg_hdr.id = MM_CM_PSM_READY_IND;
  msgr_rpt_u.cm_client_psm_ready_ind.activate_timer = activate_timer;
  msgr_rpt_u.cm_client_psm_ready_ind.module = CM_PSM_MODULE_SNS_MGR;

  CM_MSG_HIGH_1("  SNS MGR sending ready ind with timer = %d",
												  activate_timer);


  cm_sns_update_psm_state( sns_ptr,CM_SNS_PSM_READY_RSP_SEND);
 // cmpsm_msgr_cmd_proc((void *) &msgr_rpt_u);
  cm_msgr_send(MM_CM_PSM_READY_IND,MSGR_MM_CM_PSM,(msgr_hdr_s*)&msgr_rpt_u,sizeof(cm_psm_u_type));
}


/*===========================================================================
FUNCTION cmsns_send_psm_ready_req_to_snsmgr

DESCRIPTION
 It will process the ready request sent to sensor manager

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/


boolean  cmsns_send_psm_ready_req_to_snsmgr(void)
{
  cm_sns_data_collection_s_type*	  sns_ptr = cmsns_get_data_info_ptr();
  cm_sns_update_psm_state( sns_ptr,CM_SNS_PSM_READY_RSP_STARTED);
  return TRUE;
}

/*===========================================================================
FUNCTION cmsns_client_cmd_check

DESCRIPTION
  it validate command for any error

DEPENDENCIES
  none

RETURN VALUE
  cm_sns_cmd_err_e_type

SIDE EFFECTS
  none

===========================================================================*/


static cm_sns_cmd_err_e_type  cmsns_client_cmd_check(cm_sns_cmd_s_type * sns_cmd_ptr)
{

  cm_sns_cmd_err_e_type      cmd_err  = CM_SNS_CMD_ERR_NOERR;
  /* Initialize command error to NO-ERROR
  */
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  CM_ASSERT( sns_cmd_ptr != NULL );
  CM_ASSERT( sns_cmd_ptr->cmd_type == CM_CMD_TYPE_SNS );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  CM_ASSERT( BETWEEN(sns_cmd_ptr->cmd, CM_SNS_CMD_NONE, CM_SNS_CMD_MAX ));
  CM_MSG_HIGH_1("START cmsns_client_cmd_check(), cmd=%d",
      sns_cmd_ptr->cmd);
  
  switch( sns_cmd_ptr->cmd )
  {
    
    case CM_SNS_CMD_SET_CONFIG:
    /* check condition if the config allowed
       we can add other condition if and when required*/
      if(cm_sns_config_allowed(sns_cmd_ptr) == FALSE)
      {
        CM_MSG_HIGH_0("CMSNS error");
        cmd_err = CM_SNS_CMD_ERR_CONFIG_INVALID;
      }
      break;
    case CM_SNS_CMD_DATA_REQ:
      /* check condition if the update allowed/update
      will be used for enable/disable of the sensor
       we can add other condition if and when required*/
      if(cm_sns_data_req_allowed(sns_cmd_ptr) == FALSE)
      {
        cmd_err = CM_SNS_CMD_ERR_DATA_REQ_NOT_ALLOW;
      }
      break;
     case CM_SNS_CMD_RESET_CONFIG:
        cmd_err = CM_SNS_CMD_ERR_NOERR;
       break;
	 case CM_SNS_CMD_OD_SNS_REQ:
	   if(cm_sns_od_sns_req_allowed(sns_cmd_ptr) == FALSE)
	   {
         cmd_err = CM_SNS_CMD_ERR_DATA_REQ_NOT_ALLOW;
	   }
       break;
	  case CM_SNS_PHY_TEST:
      case CM_SNS_PHY_TEST_GET_DATA:
      case CM_SNS_GET_SERIAL_NUM:
      #ifdef FEATURE_MMODE_TX_21
      case CM_SNS_SET_LOGGING_LEVEL:
      break;
      case CM_SNS_CMD_GET_CONFIG :
      break;
      #endif
      break;  

      default:    
      cmd_err = CM_SNS_CMD_ERR_OTHER;
      CM_ERR_1( "Bad SNS command %d", (uint8) sns_cmd_ptr->cmd);    
  }
  
  return cmd_err;
}


/*===========================================================================
FUNCTION cmsns_client_cmd_err

DESCRIPTION
  This function process the client command error

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


static void cmsns_client_cmd_err(

  const cm_sns_cmd_s_type    *cmsns_cmd_ptr,
    /* Pointer to a CM command
    */

  cm_sns_cmd_err_e_type    cmd_err
    /* Indicate the BCMCS command error
    */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmsns_cmd_ptr != NULL );
  CM_ASSERT( cmsns_cmd_ptr->cmd_type == CM_CMD_TYPE_SNS );
  CM_ASSERT( INRANGE( cmd_err, CM_SNS_CMD_ERR_NOERR, CM_SNS_CMD_ERR_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point atsns command component
  */

  /* If commanding client supplied a callback function,
  ** notify this client of command error status.
  */
  if( cmsns_cmd_ptr->cmd_cb_func != NULL )
  {
    cmsns_cmd_ptr->cmd_cb_func( cmsns_cmd_ptr->cmd,
                                cmd_err );
  }

}

/*===========================================================================
FUNCTION cmsns_client_cmd_forward_mmll

DESCRIPTION
   it will check basic sanity and forward the command to process

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


static boolean cmsns_client_cmd_forward_mmll(cm_sns_cmd_s_type *  sns_cmd_ptr )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  CM_ASSERT( sns_cmd_ptr != NULL );
  CM_ASSERT( sns_cmd_ptr->cmd_type == CM_CMD_TYPE_SNS );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Point at SNS command component
  */
  CM_MSG_LOW_1("  START cmsns_client_cmd_forward_mmll(),cmd=%d",
              sns_cmd_ptr->cmd);
  
  switch( sns_cmd_ptr->cmd )
  {
    case CM_SNS_CMD_SET_CONFIG:
      cm_sns_process_config_cmd(sns_cmd_ptr);
      break;
    case CM_SNS_CMD_DATA_REQ:
      cm_sns_process_data_request_cmd(sns_cmd_ptr);
      break;
    case CM_SNS_CMD_RESET_CONFIG:
      cm_sns_process_reset_config_cmd(sns_cmd_ptr);
      break;
    	case CM_SNS_CMD_OD_SNS_REQ:
      cm_sns_process_od_sns_req_cmd(sns_cmd_ptr);
      break;
#ifdef FEATURE_MMODE_TX_21
	case CM_SNS_PHY_TEST:
      cm_sns_process_phy_test_req_cmd(sns_cmd_ptr);
	  break;
	case CM_SNS_PHY_TEST_GET_DATA:
	  cm_sns_process_phy_test_get_data_req_cmd(sns_cmd_ptr);	
    case CM_SNS_SET_LOGGING_LEVEL:
      cm_sns_set_logging_level_cmd(sns_cmd_ptr);
	break;
	case CM_SNS_GET_SERIAL_NUM:
	  cm_sns_process_get_serial_num(sns_cmd_ptr);
	  break;
        case CM_SNS_CMD_GET_CONFIG:
        cm_sns_get_registry_config(sns_cmd_ptr);
        break;
#endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
    default:  
      CM_ERR_1( "Bad SNS command %d", (uint8) sns_cmd_ptr->cmd);
       break;
  }  
  /* Inform the caller of this function that SNSMGR is not
  ** waiting for the reply from  sensor module
  */
  return FALSE;

}

/*===========================================================================
FUNCTION cmsns_client_cmd_mmll_reply

DESCRIPTION
  it check and set if need to wait for reply

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


static void cmsns_client_cmd_mmll_reply( cm_sns_cmd_s_type * sns_cmd_ptr,
                                                     cm_sns_cmd_err_e_type cmd_err)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  CM_ASSERT( sns_cmd_ptr != NULL );
  CM_ASSERT( cmd_err == CM_SNS_CMD_ERR_NOERR );
  CM_ASSERT( sns_cmd_ptr->cmd_type == CM_CMD_TYPE_SNS );
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Point at BCMCS command component
  */
  CM_MSG_LOW_2("  START cmsns_client_cmd_mmll_reply(), cmd=%d, err=%d",
                       sns_cmd_ptr->cmd, cmd_err );
  
  switch( sns_cmd_ptr->cmd )
  {
    case CM_SNS_CMD_SET_CONFIG:
    case CM_SNS_CMD_DATA_REQ:
	case CM_SNS_CMD_OD_SNS_REQ:
	case CM_SNS_PHY_TEST:
	case CM_SNS_PHY_TEST_GET_DATA:
	case CM_SNS_GET_SERIAL_NUM:
    #ifdef FEATURE_MMODE_TX_21
    case CM_SNS_SET_LOGGING_LEVEL: 
    break;
   case CM_SNS_CMD_GET_CONFIG:
     break;
    #endif   
       break;
    default:
    CM_ERR_1( "Bad SNSMGR command %d", (uint8) sns_cmd_ptr->cmd);
     break;
  }

}

/*===========================================================================
FUNCTION cmsns_client_cmd_proc

DESCRIPTION
   it does the sanity check for client command

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_client_cmd_proc(cm_sns_cmd_s_type * sns_cmd_ptr)
{

  cm_sns_cmd_err_e_type    cm_sns_cmd_err;
  boolean                  wait_for_reply = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  
  CM_ASSERT( sns_cmd_ptr != NULL );
  CM_ASSERT( sns_cmd_ptr->cmd_type == CM_CMD_TYPE_SNS);
  
  /* Start by checking whether this is a valid command
  ** relative to the current state of the system.
  */
  cm_sns_cmd_err = cmsns_client_cmd_check( sns_cmd_ptr );
  CM_MSG_HIGH_1("The CMD err = %d ",cm_sns_cmd_err);
  cmsns_client_cmd_err(sns_cmd_ptr,cm_sns_cmd_err);
  
  if( cm_sns_cmd_err != CM_SNS_CMD_ERR_NOERR )
  {
    return;
  }
   /* If we got here, no errors were found,
  ** so forward the client command to Lower Layer.
  */
  wait_for_reply = cmsns_client_cmd_forward_mmll( sns_cmd_ptr );
  
  if(!wait_for_reply )
  {
    cmsns_client_cmd_mmll_reply( sns_cmd_ptr,
                                  CM_SNS_CMD_ERR_NOERR );
  }

}

/*===========================================================================
FUNCTION cmsns_send_app_sample_data_rsp

DESCRIPTION

 The function will send the response to app for sns data request

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/



boolean  cmsns_send_app_sample_data_rsp(void)
{
  errno_enum_type msgr_error = E_SUCCESS;
  cm_qmi_sns_data_collection_rsp_s_type  *sns_data_rsp = (cm_qmi_sns_data_collection_rsp_s_type*)cm_mem_malloc(sizeof(cm_qmi_sns_data_collection_rsp_s_type));
  
  if(sns_data_rsp != NULL)
  {
     memset(sns_data_rsp, 0, sizeof(cm_qmi_sns_data_collection_rsp_s_type));
     cm_sns_copy_sns_sample_data(sns_data_rsp->sns_arr,&sns_data_rsp->sns_len);
    
	 
	 if((cmsns_get_data_info_ptr()->policy_met_mask & (1<<CM_SNS_TYPE_FMD)) != 0)
     {
       sns_data_rsp->fmd_status  = cmsns_get_data_info_ptr()->sns_odr_data.fmd_data.flight_status;
     }
	 if((cmsns_get_data_info_ptr()->policy_met_mask & (1<<CM_SNS_TYPE_TILT)) != 0)
     {
       sns_data_rsp->tilt_angle= cmsns_get_data_info_ptr()->sns_intr_data.tilt_data.tilt_angle_val;
     }
   
	 CM_MSG_HIGH_2("  Copied no of sensors  = %d fmd_status = %d ",sns_data_rsp->sns_len, sns_data_rsp->fmd_status );	  

     msgr_error = cm_msgr_send(MM_CM_SNS_MGR_DATA_RSP,MSGR_MM_CM_SNS_MGR,
                  (msgr_hdr_s*)sns_data_rsp,sizeof(cm_qmi_sns_data_collection_rsp_s_type));
     cm_mem_free(sns_data_rsp);
     if(msgr_error != E_SUCCESS)
	 {
      return  FALSE;
	   //CM_ERR_FATAL_0("Crash for debug");
	 }
	 return TRUE;
     
  }
  return FALSE;
}
#ifdef FEATURE_MMODE_TX_21

/*===========================================================================
FUNCTION cmsns_send_app_logging_data_rsp

DESCRIPTION

 The function will send the response to app for sns data request

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/



boolean  cmsns_send_app_logging_data_rsp(cm_sns_data_req_info_s_type *  data_req_ptr)
{
  errno_enum_type msgr_error = E_SUCCESS;
  cm_qmi_sns_data_collection_rsp_s_type  *sns_data_rsp = (cm_qmi_sns_data_collection_rsp_s_type*)cm_mem_malloc(sizeof(cm_qmi_sns_data_collection_rsp_s_type));
  
  if(sns_data_rsp != NULL)
  {
     memset(sns_data_rsp, 0, sizeof(cm_qmi_sns_data_collection_rsp_s_type));
     cm_sns_copy_sns_logging_data(sns_data_rsp->sns_arr,&sns_data_rsp->sns_len, data_req_ptr);
     CM_MSG_HIGH_1("  Copied no of sensors  = %d ",sns_data_rsp->sns_len);	   
     msgr_error = cm_msgr_send(MM_CM_SNS_MGR_DATA_RSP,MSGR_MM_CM_SNS_MGR,
                  (msgr_hdr_s*)sns_data_rsp,sizeof(cm_qmi_sns_data_collection_rsp_s_type));
     cm_mem_free(sns_data_rsp);
     if(msgr_error != E_SUCCESS)
	 {
      return  FALSE;
	   //CM_ERR_FATAL_0("Crash for debug");
	 }
	 return TRUE;     
  }
  return FALSE;
}
#endif
/*===========================================================================
FUNCTION cmsns_send_app_sensor_evt_ind

DESCRIPTION
  it sends indication to app related to critical event raised by 
  sesnors

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_send_app_sensor_evt_ind(uint32 policy_met_mask,uint32 batch_met_mask,uint32 sns_fail_mask)
{

  cm_qmi_sensor_event_ind_s_type  *sns_app_evt_ind = (cm_qmi_sensor_event_ind_s_type*)cm_mem_malloc(sizeof(cm_qmi_sensor_event_ind_s_type));
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  if(sns_ptr == NULL)
  {
    return;
  }

  if(sns_app_evt_ind != NULL)
  {
     memset(sns_app_evt_ind, 0, sizeof(cm_qmi_sensor_event_ind_s_type));
     sns_app_evt_ind->sns_event_info.policy_met_sns_mask = policy_met_mask;
     sns_app_evt_ind->sns_event_info.batch_full_sns_mask = batch_met_mask;
     sns_app_evt_ind->sns_event_info.sns_fail_mask = sns_fail_mask;
     
     CM_MSG_HIGH_3("  SNS MGR sending pol cond = %d batch cond = %d sns_fail_cond = %d",
                                 policy_met_mask,batch_met_mask,sns_fail_mask);
     cm_msgr_send(MM_CM_SNS_MGR_SNS_APP_EVENT_IND,MSGR_MM_CM_SNS_MGR,
                  (msgr_hdr_s*)sns_app_evt_ind,sizeof(cm_qmi_sensor_event_ind_s_type));
     cm_mem_free(sns_app_evt_ind);    
	 if((sns_ptr->policy_met_mask &(1 << CM_SNS_TYPE_FALL_DETECT)) != 0)
	 {
	   sns_ptr->policy_met_mask &= ~(1 << CM_SNS_TYPE_FALL_DETECT);
	 } 
  }
  
}


/*===========================================================================
FUNCTION cmsns_get_event_info

DESCRIPTION
  Check if any sensor is still waiting for the request/response

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_get_event_info(cm_sns_event_info_s_type * sns_event_info)
{
  cm_sns_data_collection_s_type*  sns_ptr = cmsns_get_data_info_ptr();

  CM_ASSERT(sns_ptr != NULL);
  sns_event_info->policy_met_sns_mask = sns_ptr->policy_met_mask;
  sns_event_info->batch_full_sns_mask = sns_ptr->batch_full_mask;
  sns_event_info->sns_fail_mask = sns_ptr->sns_fail_mask;
  sns_ptr->sns_fail_mask = 0;
  CM_MSG_HIGH_0("resetting the fail mask as app query the fail mask");	
  sns_ptr->policy_met_mask = sns_ptr->policy_met_mask & (~(1<<CM_SNS_TYPE_SMD));

}


/*===========================================================================
FUNCTION cmsns_set_force_motion_enable

DESCRIPTION
  This should be used for testing purpose only, it simulate if APPS has 
  configured to enable motion or not, without load app
  should not be enable for commercialize build

  arguement --> motion enable 
                    TRUE --> enable motion in SOI and MD interrupt configure
                    FALSE --> disable motion in SOI and MD in

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_set_force_motion_enable(uint8 motion_enable)
{
  cm_sns_data_collection_s_type*	sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  if(motion_enable &  0x01 )
  {
    sns_ptr->force_motion_enable = TRUE;	
  }
  
}

#endif

/*===========================================================================
FUNCTION cmsns_set_boot_up_time

DESCRIPTION
  it will set up the boot time for sensors

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_set_boot_up_time( uint32 boot_up_time)
{

  cm_sns_data_collection_s_type*  sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  sns_ptr->sns_bootup_time = boot_up_time;
  if(sns_ptr->sns_bootup_time == 0)
  {
	sns_ptr->sns_bootup_time = 7000;  
  }
  
}
#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cmsns_set_smd_time_out_value

DESCRIPTION
  It will set up the SMD time out value, this value can be used 
  to decide incase of false motion,how much time UE should 
  wait before moving to PSM

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_set_smd_time_out_value(uint8 time_in_sec)
{
  cm_sns_data_collection_s_type*	   sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);  

   /* time value illegal like 0 or less than 15 or greater than 60 */
  if(time_in_sec == 0 || time_in_sec < CM_MIN_SMD_TIME_OUT_VALUE || time_in_sec > CM_DEFAULT_MOTION_WAIT_TIME)
  {
    time_in_sec = CM_DEFAULT_MOTION_WAIT_TIME;
  }

  sns_ptr->motion_sns_data.sns_smd_wait_time = time_in_sec;

}


#endif
/*===========================================================================
FUNCTION cmsns_send_psm_enter_rsp

DESCRIPTION
  it will send the psm enter rsp

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_send_psm_enter_rsp(void)
{
 
    cm_psm_u_type msgr_rpt_u;
	uint32	context_size = 0;
	uint8*	context_ptr = NULL;

	cm_sns_data_collection_s_type*	   sns_ptr = cmsns_get_data_info_ptr();
	CM_ASSERT(sns_ptr != NULL);  
#ifndef CM_DEBUG
    cm_sns_write_context();
#endif

  memset((void*)&msgr_rpt_u,0,sizeof(cm_psm_u_type));
  
  
  msgr_rpt_u.cm_client_psm_enter_rsp.msg_hdr.id = MM_CM_PSM_ENTER_RSP;
  msgr_rpt_u.cm_client_psm_enter_rsp.context_size = 0;
  msgr_rpt_u.cm_client_psm_enter_rsp.context = NULL;
  msgr_rpt_u.cm_client_psm_enter_rsp.is_context_changed = TRUE;
  msgr_rpt_u.cm_client_psm_enter_rsp.module = CM_PSM_MODULE_SNS_MGR;
  CM_MSG_HIGH_0("  SNS MGR psm enter rsp ");
  cm_sns_update_psm_state( sns_ptr,CM_SNS_PSM_ENTER_RSP_SENT); 
 
 cm_msgr_send(MM_CM_PSM_ENTER_RSP,MSGR_MM_CM_PSM,(msgr_hdr_s*)&msgr_rpt_u,sizeof(cm_psm_u_type));
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

 CM_SERIAL_LOG_1("=CM= SNS PSM enter RSP with type = %d  \n\r ",sns_ptr->soi_psm_entry_type);
#endif

}


#ifndef FEATURE_MMODE_TX_21
/*===========================================================================
FUNCTION cmsns_process_motion_detect_enable

DESCRIPTION
  it will send enter rsp

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_process_motion_detect_enable(void)
{

 CM_MSG_HIGH_0(" Process MD interrupt");  

}

#endif
/*===========================================================================
FUNCTION cmsns_send_psm_enter_req_to_snsmgr

DESCRIPTION
it will process the psm enter request and send response back to CM

DEPENDENCIES
  none

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/

boolean  cmsns_send_psm_enter_req_to_snsmgr(void)
{
  cm_sns_data_collection_s_type* 	 sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);  
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  cm_disable_smd_stream();
#endif
 cm_sns_update_psm_state( sns_ptr,CM_SNS_PSM_ENTER_REQ_STARTED);
 cmsns_get_data_info_ptr()->sns_psm_full_boot_time = cmpsm_ptr()->psm_full_boot_uptime;

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  CM_SERIAL_LOG_2(" =CM= SNS PSM enter req with type = %d psmfullboot = %d  \n\r",
                                sns_ptr->soi_psm_entry_type,(uint32)cmpsm_ptr()->psm_full_boot_uptime);

  if(sns_ptr->soi_psm_entry_type == CM_PSM_TYPE_RESET )
  {
     CM_MSG_HIGH_0(" It is reset Not enable MD INTR");  
  }
  else 
#endif
  {
#ifndef FEATURE_MMODE_TX_21
   if(cm_sns_check_if_motion_possible(sns_ptr) == TRUE )
   {
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

     CM_SERIAL_LOG_0("=CM= SNS sending request for SNS MD \n\r ");
#endif
    cm_sns_enable_motion_detect_intr();
    cmpsm_wait_for_msec(200);   
  }
#endif
      cmsns_send_psm_enter_rsp();
   }
  return TRUE;
}

/*===========================================================================
FUNCTION cm_snsmgr_init

DESCRIPTION
 it will initialize the sensor manager and register the call back for ready
 request and enter request

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cm_snsmgr_init(void)
{
#if !defined( FEATURE_BUILD_SENSORONLY_IMAGE) && !defined(FEATURE_BUILD_PAGEONLY_IMAGE)
  cm_client_psm_ready_reg(cmsns_send_psm_ready_req_to_snsmgr,CM_PSM_MODULE_SNS_MGR);  
#endif
  cm_client_psm_enter_reg(cmsns_send_psm_enter_req_to_snsmgr,CM_PSM_MODULE_SNS_MGR);
#ifndef CM_DEBUG
  cm_sns_init_sns_data_collection();
#endif
}




/*===========================================================================
FUNCTION cmsns_snsmod_send_sns_sample_rsp

DESCRIPTION
  It send the environment sensor sample rsp to CM task

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_snsmod_send_sns_sample_rsp(cm_sns_id_e_type sns_id,float arr[],int len,uint64 timestamp)
{
  cm_sns_env_sensor_sample_rsp_s_type* sns_sample_rsp = NULL;
  errno_enum_type msgr_error = E_SUCCESS;
  if((len < 1 || len > CM_MAX_SNS_DIMENSION) )
  {
    CM_MSG_HIGH_2(" Invalid length = %d  or sns id = %d  return",len,sns_id);
    return;
  }
  
  sns_sample_rsp =(cm_sns_env_sensor_sample_rsp_s_type*)cm_mem_malloc(sizeof(cm_sns_env_sensor_sample_rsp_s_type));
  if(sns_sample_rsp != NULL)
  {
    memset(sns_sample_rsp,0,sizeof(cm_sns_env_sensor_sample_rsp_s_type));
    sns_sample_rsp->sensor_id = sns_id;
    memscpy(sns_sample_rsp->sample_buffer.sns_dimension_data,len * sizeof(float),arr,len * sizeof(float));
    sns_sample_rsp->sample_buffer.dimension_data_len = len;
    sns_sample_rsp->sample_buffer.timestamp = timestamp;
    msgr_error = cm_msgr_send(MM_CM_SNS_MGR_SNS_MOD_SAMPLE_RSP,MSGR_MM_CM_SNS_MGR,
                (msgr_hdr_s*)sns_sample_rsp,sizeof(cm_sns_env_sensor_sample_rsp_s_type));
    cm_mem_free(sns_sample_rsp);
    CM_MSG_HIGH_1("  cmsns_snsmod_send_sns_sample_rsp  msgr send resp = %d",msgr_error);  
  }
}


/*===========================================================================
FUNCTION cmsns_snsmod_send_sns_sample_rsp

DESCRIPTION
  It send the environment sensor sample rsp to CM task

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_snsmod_send_sns_qmi_cmd_rsp(void *user_handle,
                                      unsigned int msg_id,
                                      void *ind_buf,
                                      unsigned int ind_buf_len)
{
  cm_sns_qmi_sns_cmd_rsp_s_type* sns_qmi_rsp = NULL;
  errno_enum_type msgr_error = E_SUCCESS;
  
  sns_qmi_rsp =(cm_sns_qmi_sns_cmd_rsp_s_type*)cm_mem_malloc(sizeof(cm_sns_qmi_sns_cmd_rsp_s_type));
  if(sns_qmi_rsp != NULL)
  {
    memset(sns_qmi_rsp,0,sizeof(cm_sns_qmi_sns_cmd_rsp_s_type));
    sns_qmi_rsp->user_handle = user_handle;
	sns_qmi_rsp->msg_id = msg_id;
	sns_qmi_rsp->ind_buf = (void *)cm_mem_malloc(ind_buf_len);
	memscpy(sns_qmi_rsp->ind_buf,ind_buf_len,ind_buf,ind_buf_len);
	sns_qmi_rsp->ind_buf_len = ind_buf_len;
    msgr_error = cm_msgr_send(MM_CM_SNS_MGR_SNS_QMI_CMD_RSP,MSGR_MM_CM_SNS_MGR,
                (msgr_hdr_s*)sns_qmi_rsp,sizeof(cm_sns_qmi_sns_cmd_rsp_s_type));
    cm_mem_free(sns_qmi_rsp);
    CM_MSG_HIGH_1("  cmsns_snsmod_send_sns_qmi_cmd_rsp  msgr send resp = %d",msgr_error);  
  }
}



/*===========================================================================
FUNCTION cmsns_snsmod_send_motion_sns_rsp

DESCRIPTION
  it send the motion sensor rsp to CM task

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_snsmod_send_motion_sns_rsp(cm_sns_motion_event_e_type event,uint32 psm_time)
{
  cm_sns_mod_sensor_cmd_u_type* sns_sample_rsp = NULL;
  errno_enum_type msgr_error = E_SUCCESS;
  CM_ASSERT(INRANGE(event,CM_SNS_MOTION_EVENT_TYPE_UNKNOWN,CM_SNS_MOTION_EVENT_MAX));
  
  sns_sample_rsp =(cm_sns_mod_sensor_cmd_u_type*)cm_mem_malloc(sizeof(cm_sns_mod_sensor_cmd_u_type));
  if(sns_sample_rsp != NULL)
  {
    memset(sns_sample_rsp,0,sizeof(cm_sns_mod_sensor_cmd_u_type));
    sns_sample_rsp->cm_sns_mcd_rsp.mcd_rsp.event = event;
	sns_sample_rsp->cm_sns_mcd_rsp.mcd_rsp.psm_time = psm_time;
	event_debug = event;
    msgr_error = cm_msgr_send(MM_CM_SNS_MGR_SNS_MOTION_RSP,MSGR_MM_CM_SNS_MGR,
                (msgr_hdr_s*)sns_sample_rsp,sizeof(cm_sns_mod_sensor_cmd_u_type));
    cm_mem_free(sns_sample_rsp);
    CM_MSG_HIGH_1("  cmsns_snsmod_send_motion_sns_rsp  msgr send resp = %d",msgr_error);
  
  }
}


#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

/*===========================================================================

FUNCTION cm_sns_usb_intrpt_callback

DESCRIPTION
   intrpt callback

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

void cm_sns_usb_intrpt_callback(void)
{
	cm_sns_mod_sensor_cmd_u_type* pmic_intr = NULL;
	 errno_enum_type msgr_error = E_SUCCESS;
	
	 
	 pmic_intr =(cm_sns_mod_sensor_cmd_u_type*)cm_mem_malloc(sizeof(cm_sns_mod_sensor_cmd_u_type));
	 if(pmic_intr != NULL)
	 {
	   memset(pmic_intr,0,sizeof(cm_sns_mod_sensor_cmd_u_type));
	   msgr_error = cm_msgr_send(MM_CM_SNS_MGR_SNS_USB_INTRPT_IND,MSGR_MM_CM_SNS_MGR,
				   (msgr_hdr_s*)pmic_intr,sizeof(cm_sns_mod_sensor_cmd_u_type));
	   cm_mem_free(pmic_intr);
	   CM_MSG_HIGH_1("cm_sns_usb_intrpt_callback  msgr send resp = %d",msgr_error);
	 
	 }

}

/*===========================================================================

FUNCTION cm_sns_key_press_intrpt_callback

DESCRIPTION
   intrpt callback for key pressed

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

void cm_sns_key_press_intrpt_callback(boolean key_status)
{
	cm_sns_mod_sensor_cmd_u_type* pmic_intr = NULL;
	errno_enum_type msgr_error = E_SUCCESS;
	if(key_status == 0 )
		return;
	 
	 pmic_intr =(cm_sns_mod_sensor_cmd_u_type*)cm_mem_malloc(sizeof(cm_sns_mod_sensor_cmd_u_type));
	 if(pmic_intr != NULL)
	 {
	   memset(pmic_intr,0,sizeof(cm_sns_mod_sensor_cmd_u_type));
	   pmic_intr->key_prss_intrp_ind.key_press = key_status;
	   msgr_error = cm_msgr_send(MM_CM_SNS_MGR_SNS_KEY_PRSS_INTRPT_IND,MSGR_MM_CM_SNS_MGR,
				   (msgr_hdr_s*)pmic_intr,sizeof(cm_sns_mod_sensor_cmd_u_type));
	   cm_mem_free(pmic_intr);
	   CM_MSG_HIGH_2("cm_sns_usb_intrpt_callback  msgr send resp = %d key_status = %d ",msgr_error,key_status);
	 
	 }

}

/*===========================================================================

FUNCTION cmsns_handle_key_press_intrpt_ind

DESCRIPTION
   intrpt handle the key press press indication

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/


void cmsns_handle_key_press_intrpt_ind(boolean key_status)
{
  cm_sns_data_collection_s_type*  sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  
  sns_ptr->intrpt_status = CM_SNS_HW_INTRPT_TYPE_KEY_PRESSED;

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

 CM_SERIAL_LOG_1("=CM= SNS Key pressed intrpt status %d \n\r ",sns_ptr->intrpt_status);
#endif
  cm_sns_set_context_change(TRUE);
}


/*===========================================================================

FUNCTION cmsns_handle_usb_intrpt_ind

DESCRIPTION
   intrpt handle for usb intrpt indication

DEPENDENCIES
  none

RETURN VALUE
 none
  
SIDE EFFECTS
  none
===========================================================================*/

void cmsns_handle_usb_intrpt_ind(void)
{
  cm_sns_data_collection_s_type*  sns_ptr = cmsns_get_data_info_ptr();
  CM_ASSERT(sns_ptr != NULL);
  sns_ptr->intrpt_status = CM_SNS_HW_INTRPT_TYPE_USB_DETECTED;
  cm_sns_set_context_change(TRUE);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

 CM_SERIAL_LOG_1("=CM= SNS USB inserted intrpt_status %d \n\r ",sns_ptr->intrpt_status);
#endif
}


#endif

/*===========================================================================
FUNCTION cmsns_msgr_cmd_proc

DESCRIPTION
  it is the main router function, to process command 
  received through msgr router interface
DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_msgr_cmd_proc(cm_sns_mod_sensor_cmd_u_type * sns_smpl_rsp)
{
  CM_ASSERT(sns_smpl_rsp != NULL)
  
  switch(sns_smpl_rsp->msg_hdr.id)
  {
    case MM_CM_SNS_MGR_SNS_MOD_SAMPLE_RSP:
	  cm_disable_sns_stream(sns_smpl_rsp->env_sns_sample_rsp.sensor_id);
      cm_sns_handle_sample_rsp(sns_smpl_rsp->env_sns_sample_rsp.sensor_id,sns_smpl_rsp->env_sns_sample_rsp.sample_buffer);              
    break;
    case MM_CM_SNS_MGR_SNS_MOTION_RSP:
	  CM_MSG_HIGH_0("  cm_sns_handle_motion_sensor_rsp ");
      cm_sns_handle_motion_sensor_rsp(sns_smpl_rsp->motion_sns_rsp.event,sns_smpl_rsp->cm_sns_mcd_rsp.mcd_rsp.psm_time);
    break;
	case MM_CM_SNS_MGR_SNS_QMI_CMD_RSP:
	  CM_MSG_HIGH_0("  cm_sns_process_qmi_sns_cmd_rsp ");
      cm_sns_process_qmi_sns_cmd_rsp(sns_smpl_rsp->cm_sns_qmi_rsp.user_handle,
                                     sns_smpl_rsp->cm_sns_qmi_rsp.msg_id,
                                     sns_smpl_rsp->cm_sns_qmi_rsp.ind_buf,
                                     sns_smpl_rsp->cm_sns_qmi_rsp.ind_buf_len
	                                 );
      
	 break;
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

	case MM_CM_SNS_MGR_SNS_KEY_PRSS_INTRPT_IND:
       cmsns_handle_key_press_intrpt_ind(sns_smpl_rsp->key_prss_intrp_ind.key_press);
	 break;
	 case MM_CM_SNS_MGR_SNS_USB_INTRPT_IND:
       cmsns_handle_usb_intrpt_ind();
	  break;
#endif

#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
#ifndef FEATURE_MMODE_TX_21
    case MM_CM_SNS_MGR_SNS_MOTION_CONFIG_REQ:
      cm_sns_handle_motion_sensor_config_req(sns_smpl_rsp->config_request.motion_enable);
    break;
#endif
#endif
#ifdef FEATURE_MMODE_TX_21
    case MM_CM_SNS_MGR_REGISTRY_IND:
     cmsns_mgr_run_state_machine();
    break;
#endif
    default:
      CM_MSG_HIGH_1("  SNS not supported id = %d ",sns_smpl_rsp->msg_hdr.id);
    break;
  }

}

/*===========================================================================

DESCRIPTION
  This function used to run the function as per interrupt check  state 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
state_fptr;

===========================================================================*/

#ifdef FEATURE_MMODE_TX_21
cm_snsmgr_state_e_type cmsns_intr_check_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont)
{
  cm_snsmgr_state_e_type next_sns_state = next_sns_state_g;
  boolean  loop_cont = TRUE;
  CM_ASSERT(sns_ptr != NULL); 

  if((sns_ptr->sns_registry_mask & (1<<CM_SNS_TYPE_MCD))!=0)
  {
      loop_cont =FALSE;
      sns_ptr->sns_registry_mask = sns_ptr->sns_registry_mask  & (~(1<<CM_SNS_TYPE_MCD));
      cm_send_sns_req(CM_SNS_TYPE_REGISTRY);
  }
  else if((sns_ptr->sns_registry_mask & (1<<CM_SNS_TYPE_TILT))!=0)
  {
      loop_cont =FALSE;
      sns_ptr->sns_registry_mask = sns_ptr->sns_registry_mask  & (~(1<<CM_SNS_TYPE_TILT));
      cm_send_sns_req(CM_SNS_TYPE_REGISTRY);
  }

  else if((sns_ptr->sns_registry_mask & (1<<CM_SNS_TYPE_FMD))!=0)
  {
      loop_cont =FALSE;
      sns_ptr->sns_registry_mask = sns_ptr->sns_registry_mask  & (~(1<<CM_SNS_TYPE_FMD)); 
      cm_send_sns_req(CM_SNS_TYPE_REGISTRY); 
  }
	
  else if((sns_ptr->sns_registry_mask & (1<<CM_SNS_TYPE_MOTION_DETECT))!=0)
  {
     loop_cont =FALSE;
     sns_ptr->sns_registry_mask = sns_ptr->sns_registry_mask  & (~(1<<CM_SNS_TYPE_MOTION_DETECT));
     cm_send_sns_req(CM_SNS_TYPE_REGISTRY);	  
  }

 else if((sns_ptr->sns_registry_mask & (1<<CM_SNS_TYPE_SMD))!=0)
   {
     loop_cont =FALSE;
     sns_ptr->sns_registry_mask = sns_ptr->sns_registry_mask  & (~(1<<CM_SNS_TYPE_SMD));
     cm_send_sns_req(CM_SNS_TYPE_REGISTRY);	  
   }

 else if(sns_ptr->motion_sns_data.mcd_enable == TRUE && ((sns_ptr->intr_sns_prog_mask & (1<<CM_SNS_TYPE_MCD)) ==0 ) )
 {
    sns_ptr->intr_sns_prog_mask = (1<<CM_SNS_TYPE_MCD);
    loop_cont = FALSE;
    cm_send_sns_req(CM_SNS_TYPE_MCD);
  }

  else if(sns_ptr->sns_intr_data.fall_data.is_enable == TRUE && ((sns_ptr->intr_sns_prog_mask & (1<<CM_SNS_TYPE_FALL_DETECT)) ==0 ))
  {
    sns_ptr->intr_sns_prog_mask =  sns_ptr->intr_sns_prog_mask | (1<<CM_SNS_TYPE_FALL_DETECT);
     loop_cont = FALSE;
     cm_send_sns_req(CM_SNS_TYPE_FALL_DETECT);
   }

#if 0  
/**** we can tilt here  **/
if( sns_ptr->sns_intr_data.tilt_data.is_enable == TRUE && ((sns_ptr->intr_sns_prog_mask & (1<<CM_SNS_TYPE_TILT)) == 0 ))
{
     sns_ptr->intr_sns_prog_mask =  sns_ptr->intr_sns_prog_mask | (1<<CM_SNS_TYPE_TILT);
     loop_cont = FALSE;
     cm_send_sns_req(CM_SNS_TYPE_TILT);
}
#endif
/*****  **/

/* when moving to next state move as per image*/
if( loop_cont == TRUE )
{
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
	 next_sns_state = CM_SNS_COND_CHECK_MOTION_STATE;
    #else
     next_sns_state = CM_SNS_COND_CHECK_POLICY_MET_STATE;
#endif

if(sns_ptr->motion_sns_data.mcd_enable == TRUE)
{
  sns_ptr->intr_sns_prog_mask = (1<<CM_SNS_TYPE_MCD);
}
else 
{
 //   sns_ptr->intr_sns_prog_mask = 0;
}
  sns_ptr->sns_registry_mask = 0;

}
*is_cont = loop_cont;
return next_sns_state;
}
cm_sns_phy_test_config_s_type* cm_sns_get_phy_test_config(void)
{
  return &cm_sns_test_data_global;
}

#endif
/*===========================================================================

DESCRIPTION
  This function used to run the function as per motion state 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
state_fptr;

===========================================================================*/


cm_snsmgr_state_e_type cmsns_motion_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont)
{
  cm_snsmgr_state_e_type next_sns_state = next_sns_state_g;
  boolean  loop_cont = TRUE;
  CM_ASSERT(sns_ptr != NULL); 
  
  if(sns_ptr->no_of_sns_enabled == 0 )
  {
    next_sns_state = CM_SNS_SENSOE_DONE_STATE;
  }
  else
  {
    next_sns_state = CM_SNS_EVALUATION_STATE;
  }
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
  CM_SERIAL_LOG_2(" =CM= cmsns_motion_state_func  no of sns = %d and next state  = %d  \n\r ", sns_ptr->no_of_sns_enabled,next_sns_state);
#endif
#endif
#ifndef FEATURE_MMODE_TX_21 
  if(cm_sns_check_if_motion_possible(sns_ptr))
  {
    cm_send_sns_req(CM_SNS_TYPE_SMD);
    sns_ptr->motion_sns_data.motion_sns_state = CM_SNS_STATE_REQ_IN_PROGRESS;
    sns_ptr->motion_sns_data.motion_sns_uptime = time_get_secs()+  sns_ptr->motion_sns_data.sns_smd_wait_time;
    CM_MSG_HIGH_1("settings motion uptime = 0x %x", sns_ptr->motion_sns_data.motion_sns_uptime);
    if(cm_get_ulog_handle() != NULL)
    {
       ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= cmsns_mgr_run_state_machine  NV SMD time = %d ",
                                                         sns_ptr->motion_sns_data.sns_smd_wait_time);
    }  
    /* wait for response either from the sensor or from timer to move to next state
      out of loop wait for other interrupt*/ 
    loop_cont = FALSE;
  }
#endif
  /* By default UE should move to next state */
  *is_cont = loop_cont;
  return next_sns_state;
}


/*===========================================================================
FUNCTION cmsns_cond_met_state_func

DESCRIPTION
  This function used to run the function as per cond_met state 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
state_fptr;

===========================================================================*/


cm_snsmgr_state_e_type cmsns_cond_met_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont)
{
  cm_snsmgr_state_e_type next_sns_state = next_sns_state_g;
  boolean  loop_cont = TRUE;
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
  cm_sns_id_e_type sns_id;
  int index = 0;
#endif
  CM_ASSERT(sns_ptr != NULL); 
  next_sns_state = CM_SNS_EVALUATION_STATE;
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  CM_MSG_HIGH_0("  In sensor only imgae  in policy met/Batch met state move to next state");
#else
  if(sns_ptr->policy_met_mask != 0 || sns_ptr->batch_full_mask != 0 || sns_ptr->sns_fail_mask !=0)
  {
     /*  Go in loop to find the mask to update the 
     sensor state*/
     for(sns_id = CM_SNS_TYPE_ALS; sns_id <= CM_SNS_TYPE_ENV_SENSOR_MAX;sns_id++)
     {
        if((sns_ptr->policy_met_mask &(1 << sns_id)) ||
            (sns_ptr->batch_full_mask &(1 << sns_id)) ||
            (sns_ptr->sns_fail_mask &(1 << sns_id)))
        {
           for(index =0;index < sns_ptr->no_of_sns_enabled; index++)
           {
              if( sns_ptr->sns_data_arr[index].sns_config_data.sns_id == sns_id )
              {
                 cm_sns_update_sns_current_state(sns_ptr,CM_SNS_STATE_IND_SENT_TO_APP,index);
                 break;
              }
           }
        }
     }
                      CM_MSG_HIGH_0("=SNSDBG= sending event");  
     cmsns_send_app_sensor_evt_ind(sns_ptr->policy_met_mask ,sns_ptr->batch_full_mask, sns_ptr->sns_fail_mask);
  
  }
         
#endif
  /* by default move to next state else it will be updated in above condition */
  *is_cont = loop_cont;
  return next_sns_state;
   
}


/*===========================================================================
FUNCTION cmsns_evaluation_state_func

DESCRIPTION
  This function used to run the function as per evaluation state 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
state_fptr;

===========================================================================*/

cm_snsmgr_state_e_type cmsns_evaluation_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont)
{
  cm_snsmgr_state_e_type next_sns_state = next_sns_state_g;
  boolean  loop_cont = TRUE;
  boolean result = FALSE;
  CM_ASSERT(sns_ptr != NULL); 

   /* return true if any sensor has uptime reached */
  result =  cm_sns_find_sesnors_polling_time_reached(sns_ptr);  
  if(result == TRUE)
  {
   next_sns_state = CM_SNS_SENSOR_REQ_STATE;
  }
  else
  {
   next_sns_state = CM_SNS_SENSOE_DONE_STATE;
  }
  
  /* by default move to next state else it will be updated in above condition */
  *is_cont = loop_cont;
  return next_sns_state;
}


/*===========================================================================
FUNCTION cmsns_sensor_req_state_func

DESCRIPTION
  This function used to run the function as per req state 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
state_fptr;

===========================================================================*/

cm_snsmgr_state_e_type cmsns_sensor_req_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont)
{
  cm_snsmgr_state_e_type next_sns_state = next_sns_state_g;
  boolean  loop_cont = TRUE;
  boolean result = FALSE;
  CM_ASSERT(sns_ptr != NULL); 
  
  result = cm_sns_send_sensor_request(sns_ptr);
  if(result == FALSE)
  {
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
     /* Now set state to done   In sensor only no app 
      so no processing required*/
    next_sns_state = CM_SNS_SENSOE_DONE_STATE;
#else
    if(sns_ptr->batch_full_mask == 0 && sns_ptr->policy_met_mask == 0 && sns_ptr->sns_fail_mask == 0)	
    {
      /* Now set state to done   */
      next_sns_state = CM_SNS_SENSOE_DONE_STATE;
    }
    else if (sns_ptr->policy_met_mask != 0)
    {
      next_sns_state = CM_SNS_COND_CHECK_POLICY_MET_STATE;
    }
    else if(sns_ptr->sns_fail_mask != 0)
    {    
      next_sns_state = CM_SNS_COND_CHECK_SNS_FAIL_STATE;
    }
    else
    {
      next_sns_state = CM_SNS_COND_CHECK_BATCH_FULL_MET_STATE;
    }
#endif
     /*Only all sensor related data has been processed
      move to next state as per above condition  
      by default it will move to next state*/
     
  }
  else 
  {
    sns_ptr->sns_sensor_rsp_timer = time_get_secs() + MAX_SNS_RSP_TIME;
   /*keep in current state until it is until all SNS data req are done 
    come out of loog and wait for other interrupt*/
    loop_cont = FALSE;
  }
  
  *is_cont = loop_cont;
  return next_sns_state;

}

/*===========================================================================
FUNCTION cmsns_sensor_done_state_func

DESCRIPTION
  This function used to run the function as per done state 

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
state_fptr;

===========================================================================*/

cm_snsmgr_state_e_type cmsns_sensor_done_state_func(cm_sns_data_collection_s_type *sns_ptr,boolean *is_cont)
{
  cm_snsmgr_state_e_type next_sns_state = next_sns_state_g;
  boolean  loop_cont = TRUE;
  CM_ASSERT(sns_ptr != NULL); 
           /* This state is last state after evaluating all condition   */
  if(cm_sns_evaluate_and_send_ready_rsp(sns_ptr) == TRUE)
   {            /* if we have sent the ready or auto ready
               then all processing realted to current sensors  is done*/
#ifndef   FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_MMODE_TX_21
     next_sns_state = CM_SNS_COND_INTR_CHECK_STATE;
#endif
#endif
#ifdef   FEATURE_BUILD_SENSORONLY_IMAGE

     /* Release the Sensor resource */
     cm_disable_smd_stream();
#endif
   }
    /* no need to move to next state now 
      wait for other interrupt*/
  loop_cont = FALSE;
 
  *is_cont = loop_cont;
  return next_sns_state;

}

void cm_sns_log_uptime(void)
{
   int index = 0;
   cm_sns_data_collection_s_type*   sns_ptr = cmsns_get_data_info_ptr();
   dword                    uptime = time_get_secs(); 
   CM_ASSERT(sns_ptr != NULL);

   for(index = 0; index < sns_ptr->no_of_sns_enabled;index++)
   {
   CM_MSG_HIGH_3("[Debug]Polling time for the sns id = %d sns_uptime = %d current uptime = %d",
				 sns_ptr->sns_data_arr[index].sns_config_data.sns_id,
        		 sns_ptr->sns_data_arr[index].sns_uptime,
				 uptime);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
  CM_SERIAL_LOG_3("=SNSMGR= Polling time for the sns id = %d sns_uptime = %d current uptime = %d \n\r ",
				 sns_ptr->sns_data_arr[index].sns_config_data.sns_id,
        		 sns_ptr->sns_data_arr[index].sns_uptime,
				 uptime);
  CM_SERIAL_LOG_4("=SNSMGR= fail mask = %d policy met = %d batch met = %d SNSMGR current state = %d \n ",sns_ptr->sns_fail_mask,sns_ptr->policy_met_mask,sns_ptr->batch_full_mask,next_sns_state_g);
#endif
#endif
   CM_MSG_HIGH_3("[Debug]Batch count = %d and interval = %d sensor state = %d", sns_ptr->sns_data_arr[index].sns_config_data.sns_max_batch_cnt,
   	sns_ptr->sns_data_arr[index].sns_config_data.sns_interval,
   	sns_ptr->sns_data_arr[index].current_sns_state);
   }
   CM_MSG_HIGH_4("[Debug] fail mask = %d policy met = %d batch met = %d SNSMGR current state = %d \n ",sns_ptr->sns_fail_mask,sns_ptr->policy_met_mask,sns_ptr->batch_full_mask,next_sns_state_g);
}


/*===========================================================================
FUNCTION cmsns_mgr_run_state_machine

DESCRIPTION
  This run the sensor manager state machine, sensor manager state
  machine responsible to handle/Manage all sesnors events.
  it controls the flow for all sensors

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/

void cmsns_mgr_run_state_machine(void)
{

  boolean  cont_loop = TRUE;
  cm_snsmgr_state_e_type index ;
  state_func_ptr_t  state_func_ptr = NULL;
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  CM_MSG_HIGH_1("  Current State = %d ",next_sns_state_g);
  while(cont_loop == TRUE )
  {
    
     for(index = 0;index < (CM_SNS_SENSOR_MAX_STATE-1);index++)
     {
       if( state_fptr[index].state_name == next_sns_state_g )
       {
         state_func_ptr = state_fptr[index].func_ptr;
         break;
       }
     }
	 if(state_func_ptr != NULL)
	 {
       next_sns_state_g = state_func_ptr(sns_ptr,&cont_loop);
	 
     CM_MSG_HIGH_2("  Next State = %d cont in loop = %d ",next_sns_state_g,cont_loop);
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
  CM_SERIAL_LOG_2(" =CM= SNSMGR state = %d and loop = %d  \n\r ", next_sns_state_g,cont_loop);
#endif
#endif
	}
 }
     cm_sns_log_uptime();

}



/*===========================================================================
FUNCTION cmsns_timer_proc

DESCRIPTION
  after 1 sec timer expiry it will check if some processing need to be done

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none

===========================================================================*/


void cmsns_timer_proc(uint32 event)
{

#ifndef FEATURE_BUILD_SENSORONLY_IMAGE

  uint32 active_timer = 0; 
  sys_psm_status_e_type  status = SYS_PSM_STATUS_NOT_READY_NOT_IDLE;
#endif
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  dword                    uptime = time_get_secs(); 
  int index=0;
#ifdef FEATURE_MMODE_TX_21
  cm_sns_phy_test_config_s_type* config = cm_sns_get_phy_test_config();
#endif
  CM_ASSERT(sns_ptr != NULL);

  cm_sns_log_uptime();

#ifdef FEATURE_MMODE_TX_21

  if(config->cm_sns_phy_test_get_data_req == TRUE && uptime >= config->next_report_time)
  {
    cm_send_sns_enable_req_wrapper();
	config->next_report_time = time_get_secs() + config->report_rate;
  }
#endif
 
  if(sns_ptr->sns_sensor_rsp_timer != 0xFFFFFFFF && sns_ptr->sns_sensor_rsp_timer <= uptime)
  {
    for(index = 0; index < sns_ptr->no_of_sns_enabled; index++)
    {
      if(sns_ptr->sns_data_arr[index].current_sns_state == CM_SNS_STATE_REQ_IN_PROGRESS)
      {
        #ifdef FEATURE_BUILD_SENSORONLY_IMAGE
        #ifdef FEATURE_SERIAL_LOGGING_ENABLE
        CM_SERIAL_LOG_1("=SNSDBG= Sending sns fail on timer expiry for sns = %d",sns_ptr->sns_data_arr[index].sns_config_data.sns_id);
		#endif
		#endif
        cm_sns_sample_data_s_type sample_buffer ;
        memset(&sample_buffer,0,sizeof(cm_sns_sample_data_s_type));
        sns_ptr->sns_fail_mask = (sns_ptr->sns_fail_mask |(1 << sns_ptr->sns_data_arr[index].sns_config_data.sns_id));
        cm_sns_handle_sample_rsp(sns_ptr->sns_data_arr[index].sns_config_data.sns_id,sample_buffer);
        return;
      }
    }
  }
  
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  if((sns_ptr->sns_ready_rsp_state != CM_SNS_PSM_ENTER_RSP_SENT)&&
  	  ((sns_ptr->intrpt_status == CM_SNS_HW_INTRPT_TYPE_USB_DETECTED)||
  	  (sns_ptr->intrpt_status == CM_SNS_HW_INTRPT_TYPE_KEY_PRESSED)))
  {
    cmsns_send_psm_auto_ready_ext_ind(0,CM_PSM_TYPE_RESET);
    sns_ptr->intrpt_status = CM_SNS_HW_INTRPT_TYPE_IN_PROCESS;
	return;
  }
#ifndef FEATURE_MMODE_TX_21

  if( sns_ptr->motion_sns_data.motion_detect_enable == TRUE )
  {
    if( sns_ptr->motion_sns_data.motion_sns_state == CM_SNS_STATE_REQ_IN_PROGRESS )
    {
      /*  remove this and go for uptime */
      if(sns_ptr->motion_sns_data.motion_sns_uptime <=  
                                                 uptime )
      {

	    CM_MSG_HIGH_0(" SNS timer proc enter timer for motion expired");
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

        CM_SERIAL_LOG_0("=CM= SNS SMD timer expired \n\r ");
#endif
              /* Mark it as no rsp detected and assume it false motion */
     sns_ptr->motion_sns_data.motion_sns_state  = CM_SNS_STATE_DONE;
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  	sns_ptr->motion_sns_data.no_motion_timer_expired = TRUE;
#endif        
      /* run the state machine move to next state  */
         cmsns_mgr_run_state_machine();
 #ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  	sns_ptr->motion_sns_data.no_motion_timer_expired = FALSE;
 #endif
      }
      
    }
    
  }
  #endif
   if(sns_ptr->no_of_sns_enabled == 0)
  {
	  
	     /*  it is the bad sate check for PSM entry and response */
     if( (sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED) &&
  	     (sns_ptr->soi_psm_entry_type == CM_PSM_TYPE_RESET ))
     {
       cmsns_send_psm_enter_rsp();
     }

  }
#ifndef FEATURE_MMODE_TX_21
  if(sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_AUTO_READY_STARTED  &&
  	 sns_ptr->motion_sns_data.event == CM_SNS_MOTION_EVENT_TYPE_MOTION)
  {
    cmsns_send_psm_auto_ready_ext_ind(cm_sns_get_active_time_value(),CM_PSM_TYPE_RESET);
  }
 #endif
  
  else if( (sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED) &&
  	   sns_ptr->soi_psm_entry_type == CM_PSM_TYPE_RESET &&
  	   (
	    (sns_ptr->intrpt_status == CM_SNS_HW_INTRPT_TYPE_IN_PROCESS ) ||
	    ( (sns_ptr->policy_met_mask != 0 ) || (sns_ptr->batch_full_mask != 0) || (sns_ptr->sns_fail_mask != 0))))
  {
    cmsns_send_psm_enter_rsp();	
  }
  else 
#endif  

#ifndef FEATURE_MMODE_TX_21
  if((sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED) &&
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
	(sns_ptr->soi_psm_entry_type == CM_PSM_TYPE_SHUTDOWN ) &&
#endif
	(sns_ptr->motion_sns_data.event == CM_SNS_MOTION_EVENT_TYPE_MD_INTR_ENABLE))
 {
   cmsns_send_psm_enter_rsp();

 }

#else

  if((sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED) &&
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
	(sns_ptr->soi_psm_entry_type == CM_PSM_TYPE_SHUTDOWN ) &&
#endif
	(sns_ptr->motion_sns_data.mcd_psm_time!= 0 ))
 {
   cmsns_send_psm_enter_rsp();
 }

#endif

#ifndef FEATURE_BUILD_SENSORONLY_IMAGE

  if(sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_READY_RSP_STARTED )
  {
	if ((cm_sns_ready_to_send_psm_ready_rsp() == TRUE) &&
	   (cm_sns_get_min_sns_uptime() > time_get_secs() + (sns_ptr->sns_bootup_time/1000)))
	{
	  active_timer = cm_sns_get_active_time_value();
	  status = SYS_PSM_STATUS_READY;
	} 
    cmsns_send_psm_ready_rsp(status,active_timer);
	/*  save the reject status for the state handling   */
	if(status != SYS_PSM_STATUS_READY)
	{
      cm_sns_update_psm_state(sns_ptr,CM_SNS_PSM_READY_RSP_REJECTED);
	}
  }
#endif
  
  /* if some sensor timer getting expired during image is up  */
  if(((sns_ptr->sns_ready_rsp_state != CM_SNS_PSM_ENTER_REQ_STARTED) && 
  	  (sns_ptr->sns_ready_rsp_state != CM_SNS_PSM_ENTER_RSP_SENT )) &&
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifndef FEATURE_MMODE_TX_21
  	  ((sns_ptr->motion_sns_data.motion_detect_enable == FALSE) || 
  	   (sns_ptr->motion_sns_data.motion_sns_state != CM_SNS_STATE_REQ_IN_PROGRESS)) &&
#endif  
#endif	  
     ( uptime >= cm_sns_get_min_sns_uptime()) ) 
  {  

    CM_MSG_HIGH_0(" Time function min time expird");
    /* if sensor request is in progress put all sesnor in queue state whose timer 
    has expired*/
    if(cm_sns_is_request_in_progress(sns_ptr))
    {
      cm_sns_find_sesnors_polling_time_reached(sns_ptr);
    }
    else
    {
    /* run the state machine move to next state  */
      cmsns_mgr_run_state_machine();
    }
    
    
  }

}

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

void cm_sns_gpio_intctl_isr(uint32 regIsrParam){
  regIsrParam = 0;
  cm_sns_usb_intrpt_callback();
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
  CM_SERIAL_LOG_0(" =CM= cm_sns_gpio_intctl_isr \n\r ");
#endif
#endif
}
#endif


/*===========================================================================
FUNCTION cmsns_initiate_snsmgr

DESCRIPTION
 Initate the basic config

DEPENDENCIES
none

RETURN VALUE
void

SIDE EFFECTS
none

===========================================================================*/



void cmsns_initiate_snsmgr(void )
{

 cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
  uint8  nGpioNum = 52;
  static DalDeviceHandle *hGpioIntctl = NULL;
  DALResult eDalResult = DAL_SUCCESS;
  static GPIOINTISR gIsr;
  gIsr = (GPIOINTISR)cm_sns_gpio_intctl_isr;

#endif

#ifdef FEATURE_MMODE_TX_21
 algo_init_input *input;
 int index,i;
 float *data = NULL;
 uint64 *time = NULL;
#endif



#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE

#ifndef CM_DEBUG
  cm_psm_sio_uart_init(0);
#endif
  CM_SERIAL_LOG_0(" =CM= cmsns_initiate_snsmgr init \n\r");
#endif
#endif
  
  cm_sns_reterieve_context();
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

  pmapp_usb_vbus_status_callback(cm_sns_usb_intrpt_callback);
  pmapp_pon_kypd_register_cb(cm_sns_key_press_intrpt_callback);
#ifndef CM_DEBUG
  eDalResult = DAL_DeviceAttach(DALDEVICEID_GPIOINT, &hGpioIntctl);
#endif
  eDalResult = GPIOInt_RegisterIsr(hGpioIntctl,nGpioNum, GPIOINT_TRIGGER_RISING, gIsr, nGpioNum);
#endif

#ifdef FEATURE_MMODE_TX_21
  data = (float*)cm_mem_malloc(sizeof(float) * CM_MAX_BATCH_SUPPORTED);
  time = (uint64*)cm_mem_malloc(sizeof(uint64) * CM_MAX_BATCH_SUPPORTED);
  input = (algo_init_input*)cm_mem_malloc(sizeof(algo_init_input));
 for(index = 0; index < sns_ptr->no_of_sns_enabled; index++){
 	  memset(data,0, sizeof(float) * CM_MAX_BATCH_SUPPORTED);
	  memset(time,0, sizeof(uint64) * CM_MAX_BATCH_SUPPORTED);
      for(i =0;i< sns_ptr->sns_data_arr[index].max_count; i++){
		data[i]= sns_ptr->sns_data_arr[index].sns_sample_data[i].sns_dimension_data[0];
		time[i] = sns_ptr->sns_data_arr[index].sns_sample_data[i].timestamp;
	 }
   memset(input, 0,sizeof(algo_init_input));
   input->sensor_type = sns_ptr->sns_data_arr[index].sns_config_data.sns_id;
   input->operating_mode = sns_ptr->sns_data_arr[index].sns_config_data.operating_mode;
   input->alarm_thr_high = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->greater_value;
   input->alarm_thr_low = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->lower_value;
   input->pre_alert_thr_high = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->pre_greater_value;
   input->pre_alert_thr_low = sns_ptr->sns_data_arr[index].sns_config_data.sns_policy.sns_policy_elem->pre_lower_value;
   input->performance_mode = sns_ptr->sns_data_arr[index].sns_config_data.sns_performance_mode;
   input->alarm_hysteresis_duration = sns_ptr->sns_data_arr[index].sns_config_data.sns_hysteresis_duration;
   input->HPM_sampling_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_high_perf_interval;
   input->measurement_period = sns_ptr->sns_data_arr[index].sns_config_data.sns_interval;
   input->data_buffer = data;
   input->time_buffer = time;
   input->hysteresis_counter = sns_ptr->sns_data_arr[index].sns_config_data.sns_hysteresis_count;
   input->algorithm_state = sns_ptr->sns_data_arr[index].algorithm_state;
   input->current_count = sns_ptr->sns_data_arr[index].current_batch_count - 1;
   input->max_count = sns_ptr->sns_data_arr[index].max_count;
   input->valid_buffer_count = sns_ptr->sns_data_arr[index].valid_buffer_count;
   sns_env_aware_algo_init_interface(*input);
   CM_MSG_HIGH_0("CMSNS sns_env_aware_algo_init_interface called");
  }

  cm_mem_free(data);
  data = NULL;
  cm_mem_free(time);
  time = NULL;
  cm_mem_free(input);
  input = NULL;
#endif
#ifndef FEATURE_MMODE_TX_21
#ifndef FEATURE_BUILD_SENSORONLY_IMAGE
/*  only for full boot mark as false and wait for indication from GPS to 
     get the actual value, same value will be stored across 
      PSM in SOI cycle*/
   sns_ptr->motion_sns_data.motion_detect_enable = FALSE;
   if(sns_ptr->force_motion_enable == TRUE )
   {
     sns_ptr->motion_sns_data.motion_detect_enable = TRUE;
     cm_sns_set_context_change(TRUE);
   }
#endif
  if(cm_get_ulog_handle() != NULL)
  {
    ULOG_RT_PRINTF_1(cm_get_ulog_handle(), "=CM= cmsns_initiate_snsmgr  Motion enable = %d ",
                                                               sns_ptr->motion_sns_data.motion_detect_enable);
  }		  
#endif
  
  cmsns_mgr_run_state_machine();  
#ifdef FEATURE_BUILD_SENSORONLY_IMAGE
#ifdef FEATURE_SERIAL_LOGGING_ENABLE
  CM_SERIAL_LOG_1(" =CM= cmsns_initiate_snsmgr init : uptime =%d \n\r ", time_get_ms_native());
#endif
#endif
}


/*===========================================================================
FUNCTION cmsnsmgr_is_psm_entry_in_progress

DESCRIPTION
 It return TRUE,if SNS MGR is processing the PSM ENTER request
DEPENDENCIES
none

RETURN VALUE
boolean if SNSMGR processing PSM

SIDE EFFECTS
none

===========================================================================*/


boolean cmsnsmgr_is_psm_entry_in_progress(void)
{
  boolean res = FALSE;
  cm_sns_data_collection_s_type*	 sns_ptr = cmsns_get_data_info_ptr();
  if((sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_REQ_STARTED) ||
  	 (sns_ptr->sns_ready_rsp_state == CM_SNS_PSM_ENTER_RSP_SENT))
  {
    res = TRUE;    
  }
  return res;
}

/*===========================================================================
FUNCTION cmsnsmgr_process_lpm_oos

DESCRIPTION
 It process the LPM and OOS state
DEPENDENCIES
none

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/

void cmsnsmgr_process_lpm_oos(void)
{  
  cmsns_get_data_info_ptr()->sns_psm_full_boot_time = cmpsm_ptr()->psm_full_boot_uptime;
  cm_sns_write_context();  

}


#endif
