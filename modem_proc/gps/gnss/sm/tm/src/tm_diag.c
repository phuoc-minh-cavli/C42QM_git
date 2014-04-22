
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            TM Diag Interface

General Description
  This file contains implementations for TM Diag Interface
  
  Copyright (c) 2002 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2018 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_diag.c#1 $  
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/27/16   rn      Updated gpsdiag_EfsFileRespHandler to include filename for EFS Quota.
4/17/15    jv      Added QZSS Support
11/20/14   am      Added masking of configurations for TM_DIAG_NAV_CONFIG_CMD
09/01/09   gk      Added a check to validate the directory for EFS operation
07/25/09    jlp    Added diag command to force a dump of various data
                   structures.
06/16/08   jlp     Added handling of SV No Exist list diag command.
03/13/08   jw      Fixed encoding of the gps time for setting PM position using
                   diag.
02/14/08   jw      Support for Optimistic Punc
06/18/07   jd      Implemented diag support for GPS File System support.
05/09/07   jw      Added CGPS Intelliceiver session ctrl command
02/22/07   jw      Removed early hack for measurement combining
02/01/07   cl      Add feature to check if CDMA is supported
01/29/07   jw      Added proper featurization for PPM specific code.
12/20/06   ank     Changes to send PRM and post-fix PPM together.
12/13/06   jw      Added temporary code to set the time_ref of the Post Fix PPMs.
07/12/06   cl      Initail Release
===========================================================================*/
#include "gps_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <stdlib.h>
#include "diag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "msg.h"

#include "aries_os_api.h"
#include "tm_diag.h"
#include "tm_data.h"
#include "tm_prtl_iface.h"
#include "lm_api.h"
#include "pdapibuf.h"
#include "gps_common.h"
#include "tm_common.h"
#include "mgp_api.h"
#include "navhw_api.h"
#include "gts_loc_api.h"
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
void trigger_gnss_unload(uint8 event);
#endif


tm_prtl_cb_s_type          tm_diag_prtl_table;

#ifdef FEATURE_QDSP6
/* 
   This union helps convert uint32 data coming in diag to float
   Casting the data directly to float results in warnings on gcc
 */
typedef union 
{
  uint32 q_uint32temp;
  FLT    f_flttemp;
} tm_diag_uint_flt_type;
#endif

/* Packet buffer to hold incoming diag pket for asynchronous processing */
/* Make it 1960 bytes TODO: [an][del]converted to dynamic alloc*/
#define TM_DIAG_PACKET_BUFFER_SIZE  1960
byte tm_diag_pkt_buffer[TM_DIAG_PACKET_BUFFER_SIZE];
uint16 tm_diag_pkt_buffer_length = 0;

tm_sess_req_param_u_type   tm_diag_sess_req_param;
tm_post_data_e_type        tm_diag_post_data_type;
lm_request_info_s_type     tm_diag_aiding_data_delete_info;

boolean tm_diag_in_ftest_mode = FALSE;

/* For cached WBIQ info */
sm_ReportWBIQInfoStructType tm_diag_wbiq_info;
/* This is just a counter of the time when wbiq info is updated. */
uint8 tm_diag_wbiq_info_cnt = 0;
#define TM_DIAG_WBIQ_INFO_VERSION (1);

/* For cached XO offset info */
static sm_ReportXOOffsetInfoStructType smz_XOOffsetInfo = {0};

/* Structure to save info for delayed response. Everything else
   for the delayed response is cached in the tm_diag_wbiq_info
   defined above. */
typedef struct{
  boolean v_FactoryIqTestRequested;
  diagpkt_subsys_delayed_rsp_id_type w_DelayedRspId;
  uint8 u_Version;
} tm_diag_factory_iq_test_type;
tm_diag_factory_iq_test_type z_tm_diag_factory_iq_test = {FALSE, 0, 0};

/* Structure to store info for delayed response of Nav config 
   change diag command. */
typedef struct{
  diagpkt_subsys_delayed_rsp_id_type w_DelayedRspId;
  uint8 u_GnssDesiredConfig;
  uint8 u_HwGnssSupportedConfig;
} tm_diag_NavConfigCommandType;
tm_diag_NavConfigCommandType z_NavConfigCommand = {0, 0, 0};
boolean me_DataDump( uint32 q_DumpMask );

/* Default supported IQ test mask (see e_CollectModeTypeVal for details) */
#define C_TM_DIAG_SARF_IQ_TEST_DEFAULT 0x0007812A
static uint32 tm_diag_sarf_iq_test_mask = C_TM_DIAG_SARF_IQ_TEST_DEFAULT;

/* Default supported RF Dev Multichannel Track constellation mask, all L1 */
static gnss_RfDevTrackConstellationMaskType 
  tm_diag_rfdev_track_constellation_mask = ((1 << MultiChan_Track_L1_ENABLED) |
                                            (1 << MultiChan_Track_G1_ENABLED) |
                                            (1 << MultiChan_Track_B1_ENABLED) |
                                            (1 << MultiChan_Track_E1_ENABLED));

/*===========================================================================

FUNCTION tm_diag_send_ipc 

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_send_ipc(uint32 diag_command)
{
  os_IpcMsgType              *ipc_msg_ptr;
  
  ipc_msg_ptr = os_IpcCreate(sizeof(uint32),IPC_ALLOCATION_DYNAMIC, (uint32)THREAD_ID_SM_TM);

  if (ipc_msg_ptr != NULL)
  {
    /* Set the msg id and init the command which failed. */
    ipc_msg_ptr->q_MsgId = TM_CORE_MSG_ID_DIAG;

    *((uint32 *)ipc_msg_ptr->p_Data)= diag_command; /*lint !e826 */

    if (!os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM))
    {
      (void)os_IpcDelete(ipc_msg_ptr);   
    }
  } /* End ipc_msg_ptr */
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Failed to create IPC mesage",0,0,0); 
  } /* End ipc_msg_ptr */
}

/*===========================================================================

FUNCTION tm_diag_process_rf_dev_cmd

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_process_rf_dev_cmd(void)
{
  aries_diag_sess_ctrl_req_type *rf_dev_req_ptr = (aries_diag_sess_ctrl_req_type *)&tm_diag_pkt_buffer;
  navhw_DiagCmdStruct z_NavHwDiagCmd;
  byte dont_care = 0;
 
  (void) dont_care;
  
  switch(rf_dev_req_ptr->gps_control)
  {

  case TM_DIAG_SESS_CGPS_SUB_COMMAND:
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"CGPS commands: 0x%x",rf_dev_req_ptr->supporting_data[0],0,0);

    switch(rf_dev_req_ptr->supporting_data[0])
    {
    case TM_DIAG_SESS_CGPS_SET_PM_POS_SUB_COMMAND:
      /*
        supporting_data[1] - position type
        supporting_data[2] - lsb lat
        supporting_data[3]
        supporting_data[4]
        supporting_data[5] - msb lat
        supporting_data[6] - lsb lon
        supporting_data[7]
        supporting_data[8]
        supporting_data[9] - msb lon
        supporting_data[10] - lsb punc
        supporting_data[11]
        supporting_data[12]
        supporting_data[13] - msb punc
        supporting_data[14] - lsb wk
        supporting_data[15] - msb wk
        supporting_data[16] - lsb ms
        supporting_data[17]
        supporting_data[18]
        supporting_data[19] - msb ms
     */
      {
        uint8 u_Temp;
        uint16 w_Temp;
        uint32 q_Temp;
  #ifdef FEATURE_QDSP6
  tm_diag_uint_flt_type z_uint_flt_temp;
  #endif

        FLT *p_FltTemp;
        char b_Buffer[200];

        mgp_CellDBPosInfoStruct z_PosInject;

        /* Position Type */
        u_Temp = rf_dev_req_ptr->supporting_data[1];
        z_PosInject.e_PosInfoSrc = (mgp_PosInfoSrcType) u_Temp;

        /* Lat */
  #ifndef FEATURE_QDSP6
        q_Temp = rf_dev_req_ptr->supporting_data[2] | (rf_dev_req_ptr->supporting_data[3] << 8) | 
        (rf_dev_req_ptr->supporting_data[4] << 16) | (rf_dev_req_ptr->supporting_data[5] << 24);
        p_FltTemp = (FLT *)(&q_Temp);
    #else
        z_uint_flt_temp.q_uint32temp = rf_dev_req_ptr->supporting_data[2] | (rf_dev_req_ptr->supporting_data[3] << 8) | 
                (rf_dev_req_ptr->supporting_data[4] << 16) | (rf_dev_req_ptr->supporting_data[5] << 24);
        p_FltTemp = (&z_uint_flt_temp.f_flttemp);
    #endif
        z_PosInject.d_PosLLa[0] = (DBL) (*p_FltTemp);

        /* Lon */
    #ifndef FEATURE_QDSP6
        q_Temp = rf_dev_req_ptr->supporting_data[6] | (rf_dev_req_ptr->supporting_data[7] << 8) | 
                (rf_dev_req_ptr->supporting_data[8] << 16) | (rf_dev_req_ptr->supporting_data[9] << 24);
        p_FltTemp = (FLT *)(&q_Temp);
    #else
        z_uint_flt_temp.q_uint32temp = rf_dev_req_ptr->supporting_data[6] | (rf_dev_req_ptr->supporting_data[7] << 8) | 
                (rf_dev_req_ptr->supporting_data[8] << 16) | (rf_dev_req_ptr->supporting_data[9] << 24);
        p_FltTemp = (&z_uint_flt_temp.f_flttemp);
    #endif

        z_PosInject.d_PosLLa[1] = (DBL) (*p_FltTemp);

        /* Pos Unc */
    #ifndef FEATURE_QDSP6
        q_Temp = rf_dev_req_ptr->supporting_data[10] | (rf_dev_req_ptr->supporting_data[11] << 8) | 
                   (rf_dev_req_ptr->supporting_data[12] << 16) | (rf_dev_req_ptr->supporting_data[13] << 24);
        z_PosInject.f_PosUnc = (FLT) q_Temp;
    #else
        z_uint_flt_temp.q_uint32temp = rf_dev_req_ptr->supporting_data[10] | (rf_dev_req_ptr->supporting_data[11] << 8) | 
                   (rf_dev_req_ptr->supporting_data[12] << 16) | (rf_dev_req_ptr->supporting_data[13] << 24);
        z_PosInject.f_PosUnc = z_uint_flt_temp.f_flttemp ;
    #endif

        /* GPS wk */
        w_Temp = rf_dev_req_ptr->supporting_data[14] | (rf_dev_req_ptr->supporting_data[15] << 8);
        z_PosInject.w_GpsWeek = w_Temp;

        /* GPS ms */
        q_Temp = rf_dev_req_ptr->supporting_data[16] | (rf_dev_req_ptr->supporting_data[17] << 8) | 
                   (rf_dev_req_ptr->supporting_data[18] << 16) | (rf_dev_req_ptr->supporting_data[19] << 24);
        z_PosInject.q_GpsMsec = q_Temp;

        z_PosInject.u_PosInfoValid = TRUE;
        z_PosInject.u_GpsTimeValid = TRUE;
        
        /* Alt unc */
        z_PosInject.f_AltUnc = (FLT)(500.0);
        
        if ( msg_status( MSG_SSID_GPSSM, MSG_LEGACY_HIGH ) )
        {
          MSG_6( MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Type %d, Wk %d, Ms %lu, Lat %8.6f, Lon %8.6f, Punc %8.2f",
                           z_PosInject.e_PosInfoSrc,
                           z_PosInject.w_GpsWeek,
                           z_PosInject.q_GpsMsec, 
                           z_PosInject.d_PosLLa[0],
                           z_PosInject.d_PosLLa[1],
                           z_PosInject.f_PosUnc);
        }

        /* Convert to Rad */
        z_PosInject.d_PosLLa[0] *= DEG2RAD;
        z_PosInject.d_PosLLa[1] *= DEG2RAD;
        /* Alt */
        z_PosInject.d_PosLLa[2] = (DBL) 0.0;

        mgp_InjectPosEstimate(&z_PosInject, TRUE);
      }
      break;


        #ifndef FEATURE_GNSS_LEAN_BUILD
        case TM_DIAG_SESS_GNSS_NAV_HW_SUB_COMMAND:
          memscpy((void *)&z_NavHwDiagCmd.u_DiagData[0],
		  	     sizeof(z_NavHwDiagCmd.u_DiagData),
                 (void *)&rf_dev_req_ptr->supporting_data[0], 
                 (sizeof(uint8) * TM_DIAG_SUPPORTING_DATA_LENGTH) );
          navhw_ProcessDiagCommands( &z_NavHwDiagCmd ); 
        break;
        #endif
        
      } /* rf_dev_req_ptr->supporting_data[0] switch ends */
    break;

    case TM_DIAG_SESS_LTE_RF_DEV_COMMAND:
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"LTE TTr: Enable RF dev test command %d %d", 
               TM_DIAG_SESS_LTE_RF_DEV_COMMAND, rf_dev_req_ptr->supporting_data[0], 0);
      /* call GTS function only for SS change */
      #ifdef FEATURE_LOC_GTS_TTR_SUPPORT
      gts_LteTtrProcRfDevCmd( (uint8)rf_dev_req_ptr->supporting_data[0] );
      #else
      mgp_ProcLteRfDevCmd( (uint8)rf_dev_req_ptr->supporting_data[0] );
      #endif /* #ifdef FEATURE_LOC_GTS_TTR_SUPPORT */
      break;
    }
    #ifndef FEATURE_GNSS_SA 
    #ifndef FEATURE_GPS_LM_STANDALONE
    #ifdef FEATURE_LOC_GTS_SUPPORT
    case TM_DIAG_SESS_GTS_COMMAND:
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GTS Commands %d %d",
               TM_DIAG_SESS_GTS_COMMAND, rf_dev_req_ptr->supporting_data[0], 0);

      gts_ProcDiagCmd( (uint8 *)&rf_dev_req_ptr->supporting_data[0],TM_DIAG_SUPPORTING_DATA_LENGTH );
      break;
    } 
    #endif /* FEATURE_LOC_GTS_SUPPORT */
    #endif /*FEATURE_GPS_LM_STANDALONE */
    #endif /* ! FEATURE_GNSS_SA */

    case TM_DIAG_GEN9_ME_COMMANDS:
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"GEN9 ME Diag Command %d %d",
               TM_DIAG_GEN9_ME_COMMANDS, rf_dev_req_ptr->supporting_data[0], 0);

      mgp_ProcMeDiagCommands( (uint8 *)&rf_dev_req_ptr->supporting_data[0],TM_DIAG_SUPPORTING_DATA_LENGTH );
      break;
    } 
  
    default:
    {
      ERR("Unknown RF Dev DIAG command %d", rf_dev_req_ptr->gps_control, 0, 0);
    }    
    break;    
  }
}

/*===========================================================================

FUNCTION tm_diag_msg_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_msg_handler(const void *data)
{
  uint32 diag_cmd;

  
  if(data != NULL)
  {
    diag_cmd = *((uint32 *)data);

    switch(diag_cmd)
    {
      case TM_DIAG_SESS_REQ_RF_DEV_COMMAND:
        tm_diag_process_rf_dev_cmd();
      break;
      
      case TM_DIAG_SESS_REQ_START_COMMAND:
        
        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_START,
                       &tm_diag_sess_req_param))
        {
          tm_diag_in_ftest_mode = TRUE; /*lint !e506 */
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Start Sess Req Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Start Sess Req Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_REQ_STOP_COMMAND:
        
        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_STOP,
                       &tm_diag_sess_req_param))
        {
          tm_diag_in_ftest_mode = FALSE;                
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Stop Sess Req Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Stop Sess Req Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_REQ_CONTINUE_COMMAND:

        if(tm_sess_req(TM_PRTL_TYPE_DIAG,
                       TM_SESS_HANDLE_FTEST,
                       TM_SESS_REQ_CONTINUE,
                       &tm_diag_sess_req_param))
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Continue Sess Req Succeeded", 0, 0, 0);  
        }
        else
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Continue Sess Req Failed", 0, 0, 0);
        }
      break;

      case TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND:
        tm_diag_aiding_data_delete_info.e_req_type = LM_REQUEST_DELETE_MGP_INFO;
        tm_diag_aiding_data_delete_info.z_request.z_delete_request.delete_all = TRUE;

        /* Deletes GPS and Glonass data */
        tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.e_GnssType = MGP_GNSS_TYPE_ALL;

        (void)lm_request(&tm_diag_aiding_data_delete_info);
      break;
      
      default:
      break;      
    }
  }
}

/*===========================================================================

FUNCTION tm_diag_sess_start_req_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_start_req_cmd_handler (
                                                   PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                   word pkt_len               /* length of request packet   */
                                                 )
{
  tm_diag_sess_start_req_pkt_type *cmd_ptr = (tm_diag_sess_start_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_start_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_sess_req_param.start_param.op_mode   = (tm_sess_operation_mode_e_type)cmd_ptr->operation_mode;
  tm_diag_sess_req_param.start_param.op_req    = cmd_ptr->req_op;
  tm_diag_sess_req_param.start_param.num_fixes = cmd_ptr->num_fixes;
  tm_diag_sess_req_param.start_param.tbf_ms    = cmd_ptr->time_between_fixes * 1000;
  tm_diag_sess_req_param.start_param.ppm_qos   = cmd_ptr->ppm_qos;
  tm_diag_sess_req_param.start_param.prm_qos   = cmd_ptr->prm_qos;  
  tm_diag_sess_req_param.start_param.lr_qos    = cmd_ptr->lr_qos;    
  tm_diag_sess_req_param.start_param.accuracy_threshold = cmd_ptr->accuracy_threshold;   
  tm_diag_sess_req_param.start_param.dynamic_qos_enabled = TRUE;   

  rsp_ptr = (tm_diag_sess_start_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_START_COMMAND,
                                  sizeof (tm_diag_sess_start_rsp_pkt_type) );
  
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_START_COMMAND);
 
  return rsp_ptr;
}/*end tmDiag_SessStartReqCmdHandler*/

/*===========================================================================

FUNCTION tm_diag_sess_stop_req_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_stop_req_cmd_handler (
                                                   PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                   word pkt_len               /* length of request packet   */
                                                )
{
  tm_diag_sess_stop_req_pkt_type *cmd_ptr = (tm_diag_sess_stop_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_stop_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_sess_req_param.stop_param.stop_type    = (tm_sess_stop_e_type)cmd_ptr->stop_type;
  tm_diag_sess_req_param.stop_param.stop_reason  = (tm_sess_stop_reason_e_type)cmd_ptr->stop_reason;
  tm_diag_sess_req_param.stop_param.op_to_stop   = cmd_ptr->op_to_stop;
   
  rsp_ptr = (tm_diag_sess_stop_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_STOP_COMMAND,
                                  sizeof (tm_diag_sess_stop_rsp_pkt_type) );
  
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_STOP_COMMAND);   
  
  return rsp_ptr;
}/*end tmDiag_SessStopReqCmdHandler*/


/*===========================================================================

FUNCTION tm_diag_sess_continue_req_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_continue_req_cmd_handler (
                                                      PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                      word pkt_len               /* length of request packet   */
                                                    )
{
  tm_diag_sess_continue_req_pkt_type *cmd_ptr = (tm_diag_sess_continue_req_pkt_type *) req_pkt_ptr;
  tm_diag_sess_continue_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;
  
  tm_diag_sess_req_param.continue_param.op_req    = cmd_ptr->req_op;
  tm_diag_sess_req_param.continue_param.ppm_qos   = cmd_ptr->ppm_qos;
  tm_diag_sess_req_param.continue_param.prm_qos   = cmd_ptr->prm_qos;  
  tm_diag_sess_req_param.continue_param.lr_qos    = cmd_ptr->lr_qos;      
  tm_diag_sess_req_param.continue_param.dynamic_qos_enabled    = TRUE;      
  tm_diag_sess_req_param.start_param.accuracy_threshold = cmd_ptr->accuracy_threshold;   
  tm_diag_sess_req_param.start_param.dynamic_qos_enabled = TRUE;   
  
  rsp_ptr = (tm_diag_sess_continue_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_CONTINUE_COMMAND,
                                  sizeof (tm_diag_sess_continue_rsp_pkt_type) );
  
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_CONTINUE_COMMAND);
  
  return rsp_ptr;
}/*end tmDiag_SessContinueReqCmdHandler*/

/*===========================================================================

FUNCTION tm_diag_sess_post_data_cmd_handler

used only for post data, which is deprecated
FUNCTION tm_diag_sess_del_aiding_data_cmd_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_del_aiding_data_cmd_handler (
                                                         PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                         word  pkt_len              /* length of request packet   */
                                                       )
{
  tm_diag_delete_aiding_data_req_pkt_type *cmd_ptr = NULL;
  tm_diag_delete_aiding_data_rsp_pkt_type *rsp_ptr = NULL;

  if( NULL == req_pkt_ptr )
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,
      "tm_diag_sess_del_aiding_data_cmd_handler received NULL packet");
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt_ptr, pkt_len));
  }

  cmd_ptr = (tm_diag_delete_aiding_data_req_pkt_type *) req_pkt_ptr;
  
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteClockMask    = cmd_ptr->del_clock_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeletePosMask      = cmd_ptr->del_pos_mask;

  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteGpsEphMask   = cmd_ptr->del_gps_eph_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteGloEphMask   = cmd_ptr->del_glo_eph_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteQzssEphMask  = cmd_ptr->del_qzss_eph_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteGpsAlmMask   = cmd_ptr->del_gps_alm_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteGloAlmMask   = cmd_ptr->del_glo_alm_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteQzssAlmMask  = cmd_ptr->del_qzss_alm_mask;

  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvDirMask    = cmd_ptr->del_sv_dir_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.t_DeleteSvSteerMask  = cmd_ptr->del_sv_steer_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteGpsSvHealthMask = cmd_ptr->del_gps_sv_health_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteGloSvHealthMask = cmd_ptr->del_glo_sv_health_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteQzssSvHealthMask = cmd_ptr->del_qzss_sv_health_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteSaDataMask   = cmd_ptr->del_aa_data_mask;
  tm_diag_aiding_data_delete_info.z_request.z_delete_request.z_mgp_info.q_DeleteRtiMask      = cmd_ptr->del_rti_mask;

  rsp_ptr = (tm_diag_delete_aiding_data_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND,
                                  sizeof (tm_diag_delete_aiding_data_rsp_pkt_type));

  tm_diag_send_ipc(TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND);

  return rsp_ptr;
}/*end tmDiag_SessDelAidingDataCmdHandler*/

/*===========================================================================

FUNCTION tm_diag_sess_req_sw_ver_cmd_handler            

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_sess_req_sw_ver_cmd_handler (
                                                         PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                         word  pkt_len              /* length of request packet   */
                                                       )
{
  tm_diag_sess_req_sw_ver_rsp_pkt_type *rsp_ptr = NULL;

  (void)req_pkt_ptr;
  (void)pkt_len;
  
  rsp_ptr = (tm_diag_sess_req_sw_ver_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_SESS_REQ_SW_VER_COMMAND,
                                  sizeof (tm_diag_sess_req_sw_ver_rsp_pkt_type));

  if(rsp_ptr != NULL)
  {
    rsp_ptr->pkt_ver = 0;
    rsp_ptr->cgps_major_sw_version = 1;
    rsp_ptr->cgps_minor_sw_version = 0;
    rsp_ptr->cgps_build_sw_version = 1;
    rsp_ptr->cgps_comm_state = 0;
    rsp_ptr->cgps_sess_state = 0;
    rsp_ptr->cgps_wireless_mode = 0;
    rsp_ptr->cgps_pdapi_rev_id = 0;
  }

  return rsp_ptr;
}/*end tm_diag_sess_req_sw_ver_cmd_handler*/



/*===========================================================================
FUNCTION tm_diag_sarf_cmd_handler

DESCRIPTION
  Handles the group of commands that execute in Standalone-RF mode (MGP).
  SingleSv_Standalone is a lone exception. This can run even when MGP is
  NOT in this mode. Mode_switch cmd triggers MGP to switch to this mode.

  All four cmds that this function receives has a version number followed by
  payload. We take adv of this fact by extracting just the payload and sending
  it to LM. 
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

PACKED void* tm_diag_sarf_cmd_handler (PACKED void* p_PktReq, uint16 w_PktLength)
{
  tm_diag_sarf_cmds_common_req_pkt_type* p_Req = (tm_diag_sarf_cmds_common_req_pkt_type*) p_PktReq;
  tm_diag_sarf_common_rsp_pkt_type* p_CommonRsp = NULL;
  tm_diag_sarf_rfdevfeature_rsp_pkt_type *p_RfDevRsp = NULL;
  tm_diag_sarf_wbiq_info_rsp_pkt_type *p_RFDevWbiqRsp = NULL;
  tm_ipc_msg_id_type lm_dstn_ipc_msg_id;
  tm_diag_sarf_cmds_iqtest_req_pkt_type * p_IqTestReq = NULL;

  /* Function parameters Sanity Check */
  if (p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  if( p_Req->z_DiagCmdCommon.u_GpsCmdCode >= TM_DIAG_GPS_CMD_CODE_LAST )
  {
    return (diagpkt_err_rsp(DIAG_BAD_CMD_F, p_PktReq, w_PktLength));
  }

  /* Get diag cmd id */
  lm_dstn_ipc_msg_id = (tm_ipc_msg_id_type) 
  (((uint32) p_Req->z_DiagCmdCommon.u_GpsCmdCode - (uint32)TM_DIAG_GPS_CMD_CODE_START) 
                                                + (uint32)ARIES_MSG_ID_TM_LM_FIRST + 1);

  if (lm_dstn_ipc_msg_id == ARIES_MSG_ID_TM_LM_RF_DEV_FEATURE_CAPABILITIES)
  {
    /* Allocate space through diag for response packet */
    p_RfDevRsp = (tm_diag_sarf_rfdevfeature_rsp_pkt_type*)
             diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_SESS_SARF_COMMAND,
                                   sizeof(tm_diag_sarf_rfdevfeature_rsp_pkt_type) );

    if(p_RfDevRsp != NULL)
    {
      p_RfDevRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
      p_RfDevRsp->q_RfDevFeatureEnum = mgp_GetRfDevFeatureNum();
    }

    return p_RfDevRsp;

  }
  else if (lm_dstn_ipc_msg_id == ARIES_MSG_ID_TM_LM_IQ_TEST)
  {
    /* Check the IQ test request against the device capabilities */
    p_IqTestReq = (tm_diag_sarf_cmds_iqtest_req_pkt_type *)p_Req;

    if ((tm_diag_sarf_iq_test_mask & (1 << p_IqTestReq->u_CollectMode)) != 0)
    {
      /* Send the IQ test request down to LM */
      p_CommonRsp = (tm_diag_sarf_common_rsp_pkt_type *)
        diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                             TM_DIAG_SESS_SARF_COMMAND,
                             sizeof(tm_diag_sarf_common_rsp_pkt_type));

      if (p_CommonRsp != NULL)
      {
        p_CommonRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
        p_CommonRsp->u_Status = (uint8)lm_send_sarf_cmd(&p_Req->u_data[0], lm_dstn_ipc_msg_id);
        p_CommonRsp->q_ConstellationMask = tm_diag_rfdev_track_constellation_mask;
      }
      return(p_CommonRsp);
    }
    else 
    { /* IQ test is not supported */
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
          "tm_diag_sarf_cmd_handler: IQ Test Collect Mode %d not supported (mask 0x%08X)", 
          p_IqTestReq->u_CollectMode, tm_diag_sarf_iq_test_mask);

      return(diagpkt_err_rsp(DIAG_BAD_MODE_F, p_PktReq, w_PktLength));
    }
  }
  else if (lm_dstn_ipc_msg_id == ARIES_MSG_ID_TM_LM_GET_LATEST_WB_FFT_RESULTS)
  {
    /* Allocate space through diag for response packet */
    p_RFDevWbiqRsp = (tm_diag_sarf_wbiq_info_rsp_pkt_type*)
             diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_SESS_SARF_COMMAND,
                                   sizeof(tm_diag_sarf_wbiq_info_rsp_pkt_type) );

    if(p_RFDevWbiqRsp != NULL)
    {
      p_RFDevWbiqRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
      p_RFDevWbiqRsp->u_Ver = TM_DIAG_WBIQ_INFO_VERSION;
      p_RFDevWbiqRsp->u_Cnt = tm_diag_wbiq_info_cnt;
      if(tm_diag_wbiq_info.u_GnssConfig & C_RCVR_GNSS_CONFIG_GPS_ENABLED)
      {
        p_RFDevWbiqRsp->q_Cno = (uint32)(10*tm_diag_wbiq_info.f_GpsL1CNoDBHz);
        p_RFDevWbiqRsp->l_Freq = (int32)tm_diag_wbiq_info.f_GpsL1FreqHz; 
      }
      else if(tm_diag_wbiq_info.u_GnssConfig & C_RCVR_GNSS_CONFIG_GLO_ENABLED)
      {
        p_RFDevWbiqRsp->q_Cno = (uint32)(10*tm_diag_wbiq_info.f_GloG1CNoDBHz);
        p_RFDevWbiqRsp->l_Freq = (int32)tm_diag_wbiq_info.f_GloG1FreqHz; 
      }
      else if(tm_diag_wbiq_info.u_GnssConfig & C_RCVR_GNSS_CONFIG_BDS_ENABLED)
      {
        p_RFDevWbiqRsp->q_Cno = (uint32)(10*tm_diag_wbiq_info.f_BdsB1CNoDBHz);
        p_RFDevWbiqRsp->l_Freq = (int32)tm_diag_wbiq_info.f_BdsB1FreqHz; 
      }
      else if(tm_diag_wbiq_info.u_GnssConfig & C_RCVR_GNSS_CONFIG_GAL_ENABLED)
      {
        p_RFDevWbiqRsp->q_Cno = (uint32)(10*tm_diag_wbiq_info.f_GalE1CNoDBHz);
        p_RFDevWbiqRsp->l_Freq = (int32)tm_diag_wbiq_info.f_GalE1FreqHz; 
      }
      else if (tm_diag_wbiq_info.q_MultiBandConfig & 
               (C_RCVR_GNSS_CONFIG_L5_ENABLED | C_RCVR_GNSS_CONFIG_E5A_ENABLED))
      {
        p_RFDevWbiqRsp->q_Cno = (uint32)(10*tm_diag_wbiq_info.f_GpsL5GalE5aCNoDBHz);
        p_RFDevWbiqRsp->l_Freq = (int32)tm_diag_wbiq_info.f_GpsL5GalE5aFreqHz;
      }
      else
      { /* Default to GPS results */
        p_RFDevWbiqRsp->q_Cno = (uint32)(10*tm_diag_wbiq_info.f_GpsL1CNoDBHz);
        p_RFDevWbiqRsp->l_Freq = (int32)tm_diag_wbiq_info.f_GpsL1FreqHz; 
      }
    }

    return p_RFDevWbiqRsp;

  }
  else
  {
    /* Allocate space through diag for response packet */
    p_CommonRsp = (tm_diag_sarf_common_rsp_pkt_type*)
             diagpkt_subsys_alloc( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_SESS_SARF_COMMAND,
                                   sizeof(tm_diag_sarf_common_rsp_pkt_type) );
  
    if(p_CommonRsp != NULL)
    {
      p_CommonRsp->z_DiagCmdCommon.u_GpsCmdCode = p_Req->z_DiagCmdCommon.u_GpsCmdCode;
      p_CommonRsp->u_Status = (uint8) lm_send_sarf_cmd(&p_Req->u_data[0], lm_dstn_ipc_msg_id);
      p_CommonRsp->q_ConstellationMask = tm_diag_rfdev_track_constellation_mask;
    }

    return p_CommonRsp;
  }



} /* gpsdiag_RfVerifCmdHandler */

/*===========================================================================

FUNCTION tm_diag_port_control_cmd_handler

DESCRIPTION
  This function handles port control command

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * tm_diag_port_control_cmd_handler (
                                                 PACKED void *req_pkt_ptr,  /* pointer to request packet  */
                                                 word pkt_len               /* length of request packet   */
                                               )
{
  tm_diag_port_control_req_pkt_type *cmd_ptr = (tm_diag_port_control_req_pkt_type *) req_pkt_ptr;
  tm_diag_port_control_rsp_pkt_type *rsp_ptr = NULL;

  (void)pkt_len;

  rsp_ptr = (tm_diag_port_control_rsp_pkt_type *) 
            diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_GPS,
                                  TM_DIAG_PORT_CONTROL_COMMAND,
                                  sizeof (tm_diag_port_control_rsp_pkt_type) );

  if(cmd_ptr != NULL)
  {
    /* Service type 0 => NMEA */
   if(cmd_ptr->service_type == 1)  /* Service type 1 => Serial PD API */
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Received Serial PD API Port Control Command", 0, 0, 0);
    }

    if(rsp_ptr != NULL)
    {
      rsp_ptr->result = 1;
    }    
  }
  else
  {
    if(rsp_ptr != NULL)
    {
      rsp_ptr->result = 0;
    }  
  }

  return rsp_ptr;
}/*end tm_diag_port_control_cmd_handler*/

/*===========================================================================

FUNCTION tm_diag_forward_pkt

DESCRIPTION
  This procedure receives Diag packets from AriesDiag
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_forward_pkt(
  const PACKED void *req_pkt_ptr,
  word pkt_len
)
{
  aries_diag_sess_ctrl_req_type *req_ptr = (aries_diag_sess_ctrl_req_type *) req_pkt_ptr;

  if( pkt_len > TM_DIAG_PACKET_BUFFER_SIZE )
{
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Diag packet too big: %d > %d",
              pkt_len, TM_DIAG_PACKET_BUFFER_SIZE, 0);
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Bad packet Cmd code %d GPS cmd %d GPS Subcmd %d",
              req_ptr->cmd_code, req_ptr->gps_control, req_ptr->supporting_data[0]);

    pkt_len = TM_DIAG_PACKET_BUFFER_SIZE;
  }
  
  memscpy((void *)&tm_diag_pkt_buffer,sizeof(tm_diag_pkt_buffer) , (void *)req_pkt_ptr, pkt_len);
  tm_diag_pkt_buffer_length = pkt_len;
  tm_diag_send_ipc(TM_DIAG_SESS_REQ_RF_DEV_COMMAND);
  return;
}

/*===========================================================================

FUNCTION ariesDiag_SessCtrl

DESCRIPTION
  This procedure starts retrieval of the GPS search grid and decide where 
  (TM or LM) to forward the request to.
  
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * ariesDiag_SessCtrl (
  PACKED void *req_pkt_ptr,
  word pkt_len
)
{
  aries_diag_sess_ctrl_req_type *req_ptr = (aries_diag_sess_ctrl_req_type *) req_pkt_ptr;
  aries_diag_sess_ctrl_rsp_type *rsp_ptr = NULL;
  byte gps_cmd = req_ptr->gps_control;
  boolean lm_command = FALSE;
  boolean test_command = FALSE;
  
  /* Dispatch legacy requests to LM or TM */
  /* Find out what needs to be sent to LM, others should be sent to TM */

  /* AFTL and GPS control commands share the same gps_cmd code */
  /* We need to figure out what goes to LM and what goes to TM */
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Received diag control Command %d", gps_cmd, 0, 0);
  if(gps_cmd == 7)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Received diag Command %d", req_ptr->supporting_data[0], 0, 0);
    switch(req_ptr->supporting_data[0])
    {
    
#ifdef TEST_UNLOAD_TRIGGER_FROM_INTERNAL_CODE_LOCATIONS
      case 0x01:   /* trigger unload */
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"triggered unload from diag %d", req_ptr->supporting_data[0], 0, 0);
        trigger_gnss_unload(99);
        test_command=TRUE;
        break;
      }
#endif
    
      case 0x10:   /* Set gps search mode: high */
      case 0x11:   /* Set gps search mode: Fast TCal */
      case 0x13:   /* Set gps search mode: high to DeepMedium */
      case 0x1b:   /* Set gps search mode: high to SuperLow */
      case 0x23:   /* Turn off Jammer Detection Alg */
      case 0x24:   /* Turn on Jammer Detection Alg */
      case 0xA0:   /* Set GPS Intelliceiver mode */
        lm_command = TRUE; /*lint !e506 */
      break;

      default:
      break;      
    }
  }

  if(lm_command == TRUE) /*lint !e506  !e731 */
  {
    (void)lmDiag_ForwardGSCPkt(req_pkt_ptr, pkt_len);
  }
  else if(test_command == FALSE)
  {
    tm_diag_forward_pkt(req_pkt_ptr, pkt_len);  
  }  
  
  rsp_ptr = (aries_diag_sess_ctrl_rsp_type *)diagpkt_alloc (DIAG_GPS_SESS_CTRL_F, sizeof (aries_diag_sess_ctrl_rsp_type));
  
  if(rsp_ptr != NULL)
  {
    rsp_ptr->gps_control = req_ptr->gps_control;
  }

  return rsp_ptr;
}


/*===========================================================================
FUNCTION tm_diag_query_blanking_cmd_handler

DESCRIPTION
  This function is used to query the blanking state (as configured by RF)
  and the current blanking count.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_query_blanking_cmd_handler(PACKED void* p_PktReq, uint16 w_PktLength)
{
  gpsdiag_QueryBlankingReqType* p_Req = (gpsdiag_QueryBlankingReqType*) p_PktReq;
  gpsdiag_QueryBlankingRspType* p_Rsp = NULL;
  mgp_QueryBlankingType z_QueryBlanking = {0};
  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_QueryBlankingRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                TM_DIAG_QUERY_BLANKING_CMD,
                                sizeof(gpsdiag_QueryBlankingRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_diag_query_blanking_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
     /* Set the status in the response, 0 => Success */
    diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
 
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);

    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));
    /* Query the blanking config and counter from MGP */
    mgp_QueryBlanking(&z_QueryBlanking);

    /* Fill out the response */
    p_Rsp->b_Enabled = z_QueryBlanking.b_Enabled;
    p_Rsp->q_Rx1Counter = z_QueryBlanking.q_Rx1Counter;
    p_Rsp->q_Rx2Counter = z_QueryBlanking.q_Rx2Counter;
  }

  return p_Rsp;
} 

/*===========================================================================
FUNCTION tm_api_nav_config_change_resp_handler

DESCRIPTION
  This function is used to send the delayed response back to the diag for 
  the last NAV config change command request. It sends the current config
  along with the response.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/

void tm_api_nav_config_change_resp_handler(uint8 u_CurrGnssConfig)
{
  gpsdiag_NavConfigRspType *p_DelayedRsp = NULL;
  
  p_DelayedRsp = (gpsdiag_NavConfigRspType *)
        diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type) DIAG_SUBSYS_GPS,
                                       TM_DIAG_NAV_CONFIG_CMD,
                                       z_NavConfigCommand.w_DelayedRspId,
                                       sizeof (gpsdiag_NavConfigRspType));
  if(p_DelayedRsp != NULL)
  {
	/* Set the response count to 1 */
	diagpkt_subsys_set_rsp_cnt(&(p_DelayedRsp->z_DiagCmdHdrV2), 1);
    if(z_NavConfigCommand.u_GnssDesiredConfig == u_CurrGnssConfig)
    {
      /* Set the status to 0 => Success */
      diagpkt_subsys_set_status(&(p_DelayedRsp->z_DiagCmdHdrV2), 0); 
    }
    else
    {
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
           "Requested Config 0x%x, Actual Config 0x%x", 
           z_NavConfigCommand.u_GnssDesiredConfig, u_CurrGnssConfig);
      /* Set the status to 1 => Failure */
      diagpkt_subsys_set_status(&(p_DelayedRsp->z_DiagCmdHdrV2), 1); 
    }
    /* Fill in the rest of the delayed response */
    p_DelayedRsp->u_GnssSupportedConfig = z_NavConfigCommand.u_HwGnssSupportedConfig &
                                         ( C_RCVR_GNSS_CONFIG_GPS_ENABLED |
                                           C_RCVR_GNSS_CONFIG_GLO_ENABLED |
                                           C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |
                                           C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE );
    p_DelayedRsp->u_GnssActualConfig =  u_CurrGnssConfig &
                                        ( C_RCVR_GNSS_CONFIG_GPS_ENABLED |
                                          C_RCVR_GNSS_CONFIG_GLO_ENABLED |
                                          C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |
                                          C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE );

    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
       "Sending delayed response for Nav config change cmd");

    /* Send it! */
    diagpkt_delay_commit((void *)p_DelayedRsp);
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
        "Unable to allocate memory for the delayed response");
  }
  /** Zero out the z_NavConfigCommand structure **/
  z_NavConfigCommand.u_GnssDesiredConfig = 0;
  z_NavConfigCommand.w_DelayedRspId = 0; 
  z_NavConfigCommand.u_HwGnssSupportedConfig = 0;
}

/*===========================================================================
FUNCTION tm_diag_NavConfigRequest

DESCRIPTION
  This function is used to send the NavConfigRequest to MGP through LM if 
  the desired and actual config are different. It also saves the state for 
  a delayed response when the config change completes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Nav config change could take some time and hence we send an immediate
  response to acknowledge the command and then a delayed response when the 
  process completes.
===========================================================================*/
static void tm_diag_NavConfigRequest(gpsdiag_NavConfigReqType *p_Req,
                                     gpsdiag_NavConfigRspType *p_ImmedRsp)
{ 
  lm_NavConfigReqType z_NavConfig;
  uint8  u_GnssActualConfig;
  uint8  u_HwGnssSupportedConfig;
  uint8  u_GnssDesiredConfig;
  
  if( (p_Req != NULL) && (p_ImmedRsp != NULL) )
  {
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_ImmedRsp->z_DiagCmdHdrV2), 0);
    diagpkt_subsys_set_status(&(p_ImmedRsp->z_DiagCmdHdrV2), 0); 
	
    /* Obtain the current receiver configuration */
    mgp_GetActiveGnssRcvrConfiguration(&u_GnssActualConfig);
   
    /* Obtain the HWsupported receiver configuration */
    mgp_GetHwSupportedRcvrConfiguration(&u_HwGnssSupportedConfig);

    u_GnssDesiredConfig = p_Req->u_GnssDesiredConfig;

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "GNSS Nav config 13, %2x, %2x, %2x ", u_GnssActualConfig,
              p_Req->u_GnssDesiredConfig, u_HwGnssSupportedConfig);

    /* Fill out the response */
    p_ImmedRsp->u_GnssSupportedConfig = u_HwGnssSupportedConfig &
                                      ( C_RCVR_GNSS_CONFIG_GPS_ENABLED |
                                        C_RCVR_GNSS_CONFIG_GLO_ENABLED |
                                        C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |
                                        C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE );
    p_ImmedRsp->u_GnssActualConfig =  u_GnssActualConfig &
                                    ( C_RCVR_GNSS_CONFIG_GPS_ENABLED |
                                      C_RCVR_GNSS_CONFIG_GLO_ENABLED |
                                      C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE |
                                      C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE );

    /** If DesiredConfig is 0, then it is a read request. 
     *  If the DesiredConfig matches the current configuration,
     *  we just return an immediate response.
     */
    if ( (u_GnssDesiredConfig == 0)  || 
         (u_GnssDesiredConfig == u_GnssActualConfig) )
    {
       /* Nothing more to follow */
	    diagpkt_subsys_reset_delayed_rsp_id(&(p_ImmedRsp->z_DiagCmdHdrV2));
    }
    else
    {
      /* Save the DIAG config */
      z_NavConfigCommand.w_DelayedRspId = 
	   diagpkt_subsys_get_delayed_rsp_id(&(p_ImmedRsp->z_DiagCmdHdrV2));
      z_NavConfigCommand.u_GnssDesiredConfig = p_Req->u_GnssDesiredConfig;
      z_NavConfigCommand.u_HwGnssSupportedConfig = u_HwGnssSupportedConfig;
      
      /* Send the config to MGP through LM */
      z_NavConfig.u_GnssDesiredConfig = p_Req->u_GnssDesiredConfig;
      
      /** IPC to LM is unsuccessful */
      if( lm_send_sarf_cmd((uint8 *)&z_NavConfig, 
                            ARIES_MSG_ID_TM_LM_NAV_CONFIG_CMD) == TRUE )
      {
        /** There won't be a delayed response */
		 diagpkt_subsys_set_status(&(p_ImmedRsp->z_DiagCmdHdrV2), 1);
        diagpkt_subsys_reset_delayed_rsp_id(&(p_ImmedRsp->z_DiagCmdHdrV2));
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
            "tm_diag_NavConfigRequest: Failed to send IPC to LM");
      }
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
        "tm_diag_NavConfigRequest: Input parameter check failed");
  }
}

/*===========================================================================
FUNCTION tm_diag_nav_config_cmd_handler

DESCRIPTION
  This function is used to configure the Nav Core, which will then
  configure Nav RF as needed and notify upper layers like MC.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_nav_config_cmd_handler(PACKED void* p_PktReq, 
                                            uint16 w_PktLength)
{

  gpsdiag_NavConfigReqType *p_Req = (gpsdiag_NavConfigReqType *)p_PktReq; 
  gpsdiag_NavConfigRspType *p_ImmedRsp = NULL; 

  /* Function parameters sanity check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  /* Allocate space through diag for response packet */
  p_ImmedRsp = (gpsdiag_NavConfigRspType *)
      diagpkt_subsys_alloc_v2((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                               TM_DIAG_NAV_CONFIG_CMD, 
                               sizeof(gpsdiag_NavConfigRspType)); 

  /* NAV Config request will fill in the immediate response */
  tm_diag_NavConfigRequest(p_Req, p_ImmedRsp);  

  return(p_ImmedRsp);
}

/*===========================================================================
FUNCTION tm_diag_query_adc_iq_sat_per_cmd_handler

DESCRIPTION
  This function is used to do query adc iq saturation percentage

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_query_adc_iq_sat_per_cmd_handler(PACKED void* p_PktReq, 
                                                      uint16 w_PktLength)
{
  gpsdiag_QueryAdcIqSatPerReqType *p_Req = (gpsdiag_QueryAdcIqSatPerReqType *)p_PktReq; 
  gpsdiag_QueryAdcIqSatPerRspType *p_Rsp = NULL; 
  mgp_QueryAdcIqSatPerType z_AdcIqSatParams;
  

  /* Function parameters Sanity Check */
  if ( p_Req == NULL )
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return (diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }

  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_QueryAdcIqSatPerRspType *)
  diagpkt_subsys_alloc_v2((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                          TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD, 
                            sizeof(gpsdiag_QueryAdcIqSatPerRspType)); 
  
  /* Fill in response packet */
  if ( p_Rsp == NULL )
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_diag_query_adc_iq_sat_per_cmd_handler: Rsp packet is NULL!!", 0, 0, 0); 
  }
  else
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_diag_query_adc_iq_sat_per_cmd_handler: Rsp packet is not NULL!!", 0, 0, 0);

    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);
	
    /* Nothing more to follow */
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));
	
    /* Query ADC IQ saturation status from MGP */
    mgp_QueryAdcIqSatPer(&z_AdcIqSatParams);
    if ( z_AdcIqSatParams.u_AdcIqSatPerStatus  == C_MGP_ADC_IQ_SAT_PER_VALID ) 
    {
      /* Set the status in the response, 0 => Success */
      diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
      diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }

    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_diag_query_adc_iq_sat_per_cmd_handler Status %u ISat %d QSat %d",
             z_AdcIqSatParams.u_AdcIqSatPerStatus,
             z_AdcIqSatParams.q_ISatPer,
             z_AdcIqSatParams.q_QSatPer);
    /* Fill out the response */
    p_Rsp->u_AdcIqSatPerStatus = z_AdcIqSatParams.u_AdcIqSatPerStatus;
    p_Rsp->q_ISatPer = z_AdcIqSatParams.q_ISatPer;
    p_Rsp->q_QSatPer = z_AdcIqSatParams.q_QSatPer;     

  }

  return p_Rsp; 
}

/*===========================================================================
FUNCTION tm_diag_ioctl_cmd_handler

DESCRIPTION
  This function is used to run multiple ftm tests.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_ioctl_cmd_handler(PACKED void* p_PktReq, 
                                            uint16 w_PktLength)
{
  gpsdiag_NavIoctlReqType* p_Req = (gpsdiag_NavIoctlReqType*) p_PktReq;
  gpsdiag_NavIoctlRspType* p_Rsp = NULL;
  mgp_NavIoctlType z_NavIoctl = {0};

  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_NavIoctlRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_NAV_IOCTL_CMD,
                                   sizeof(gpsdiag_NavIoctlRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_diag_nav_set_pga_backoff_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    /* Set the first (0) response count */
	 diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);	
    /* Nothing more to follow */
     diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));
    /*Set the request params*/
    z_NavIoctl.u_RequestCode = p_Req->u_RequestCode;
    z_NavIoctl.l_RequestParam1 = p_Req->l_RequestParam1;
    z_NavIoctl.l_RequestParam2 = p_Req->l_RequestParam2;    
      /*Send the config to LM */
    if ( mgp_NavIoctl(&z_NavIoctl)== TRUE )
    {
       /* Set the status in the response, 0 => Success */
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }
     /*Set the response params*/
     p_Rsp->u_RequestCode = z_NavIoctl.u_RequestCode;
     p_Rsp->l_ResponseParam1 = z_NavIoctl.l_ResponseParam1;
     p_Rsp->l_ResponseParam2 = z_NavIoctl.l_ResponseParam2;
  }
  return(p_Rsp);
} 
 
/*===========================================================================
FUNCTION tm_diag_ext_wb_fft_results_cmd_handler

DESCRIPTION
  This function is used to return extended wideband FFT results back
  to the test tool.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_ext_wb_fft_results_cmd_handler(PACKED void* p_PktReq, 
                                                    uint16 w_PktLength)
{
  gpsdiag_ExtendedWbFFtResultsReqType* p_Req = 
    (gpsdiag_ExtendedWbFFtResultsReqType*) p_PktReq;	
  gpsdiag_ExtendedWbFFtResultsRspType* p_Rsp = NULL;
  int i;
  /* Function parameters Sanity Check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_ExtendedWbFFtResultsRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_EXT_WB_FFT_RESULTS_CMD,
                                   sizeof(gpsdiag_ExtendedWbFFtResultsRspType) );

  /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_diag_ext_wb_fft_results_cmd_handler: Rsp packet is NULL!!", 0, 0, 0);
  }
  else
  {
    /* Set the first (0) response count */
    diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);
    /* Nothing more to follow */    
    diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));
	 /* Sanity check to see if there are any WB FFT results to return */
    if(tm_diag_wbiq_info_cnt > 0)
    {
       /* Set the status in the response, 0 => Success */
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
       diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }
    p_Rsp->u_Version = 3;
    p_Rsp->u_Cnt = tm_diag_wbiq_info_cnt;
    p_Rsp->q_GpsL1CnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_GpsL1CNoDBHz);
    p_Rsp->l_GpsL1FreqHz = (int32)tm_diag_wbiq_info.f_GpsL1FreqHz; 
    p_Rsp->q_GloG1CnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_GloG1CNoDBHz);
    p_Rsp->l_GloG1FreqHz = (int32)tm_diag_wbiq_info.f_GloG1FreqHz; 
    p_Rsp->q_BdsB1CnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_BdsB1CNoDBHz);
    p_Rsp->l_BdsB1FreqHz = (int32)tm_diag_wbiq_info.f_BdsB1FreqHz; 
    p_Rsp->q_GalE1CnoP1DBHz = (uint32)(10*tm_diag_wbiq_info.f_GalE1CNoDBHz);
    p_Rsp->l_GalE1FreqHz = (int32)tm_diag_wbiq_info.f_GalE1FreqHz;
    p_Rsp->u_GnssConfig = tm_diag_wbiq_info.u_GnssConfig;
    p_Rsp->l_AdcMeanIp1mV = tm_diag_wbiq_info.l_AdcMeanIp1mV;
    p_Rsp->l_AdcMeanQp1mV = tm_diag_wbiq_info.l_AdcMeanQp1mV;
    p_Rsp->q_AdcAmpIp1mV = tm_diag_wbiq_info.q_AdcAmpIp1mV;
    p_Rsp->q_AdcAmpQp1mV = tm_diag_wbiq_info.q_AdcAmpQp1mV;
    p_Rsp->u_AdcIqSatPerStatus = tm_diag_wbiq_info.u_AdcIqSatPerStatus; 
    p_Rsp->q_ISatPer = tm_diag_wbiq_info.q_ISatPer; 
    p_Rsp->q_QSatPer = tm_diag_wbiq_info.q_QSatPer; 
    p_Rsp->l_AdcIqWbJammerPwrEstDb = tm_diag_wbiq_info.l_AdcIqWbJammerPwrEstDb;  
    p_Rsp->l_GpsL1NoiseFloorEstDb = tm_diag_wbiq_info.l_GpsL1NoiseFloorEstDb;
    p_Rsp->l_GloG1NoiseFloorEstDb = tm_diag_wbiq_info.l_GloG1NoiseFloorEstDb;
    p_Rsp->l_BdsB1NoiseFloorEstDb = tm_diag_wbiq_info.l_BdsB1NoiseFloorEstDb;
    p_Rsp->l_GalE1NoiseFloorEstDb = tm_diag_wbiq_info.l_GalE1NoiseFloorEstDb;
    for(i=0; i<N_GLO_FREQ; i++)
    {
      p_Rsp->l_GloG1FreqNoiseFloorEstDb[i] =
        tm_diag_wbiq_info.l_GloG1FreqNoiseFloorEstDb[i];
    }
    p_Rsp->q_MultiBandConfig = tm_diag_wbiq_info.q_MultiBandConfig;
    p_Rsp->q_GpsL5GalE5aCnoP1DBHz = 
      (uint32)(10*tm_diag_wbiq_info.f_GpsL5GalE5aCNoDBHz);
    p_Rsp->l_GpsL5GalE5aFreqHz = (int32)tm_diag_wbiq_info.f_GpsL5GalE5aFreqHz; 
  }

  return(p_Rsp);
} 



/*===========================================================================
FUNCTION tm_diag_query_xo_offset_info_cmd_handler

DESCRIPTION
  This function is used to handle the diag command which queries the XO offset
  information. It returns XO offset, XO rate and FUNC in the diag response.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void* tm_diag_query_xo_offset_info_cmd_handler(PACKED void* p_PktReq, 
                                                      uint16 w_PktLength)
{
  gpsdiag_QueryXoOffsetInfoReqType* p_Req = (gpsdiag_QueryXoOffsetInfoReqType*) p_PktReq;	
  gpsdiag_QueryXoOffsetInfoRspType* p_Rsp = NULL;  
  
  /* Function parameters sanity check */
  if(p_Req == NULL)
  {
    /* Return a diag error packet indicating bad parameters were passed */
    return(diagpkt_err_rsp(DIAG_BAD_PARM_F, p_PktReq, w_PktLength));
  }
  /* Allocate space through diag for response packet */
  p_Rsp = (gpsdiag_QueryXoOffsetInfoRspType*)
          diagpkt_subsys_alloc_v2( (diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                   TM_DIAG_QUERY_XO_OFFSET_INFO_CMD,
                                   sizeof(gpsdiag_QueryXoOffsetInfoRspType) );
   /* Fill in response packet */
  if(p_Rsp == NULL)
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_diag_query_xo_offset_info_cmd_handler: Rsp packet is NULL!!");
  }
  else
  {
     /* Set the first (0) response count */
     diagpkt_subsys_set_rsp_cnt(&(p_Rsp->z_DiagCmdHdrV2), 0);
    /* Nothing more to follow */
     diagpkt_subsys_reset_delayed_rsp_id(&(p_Rsp->z_DiagCmdHdrV2));
	 
    /* Fill out the response */
    p_Rsp->u_XoOffsetInfoStatus = smz_XOOffsetInfo.v_IsDataValid;
    p_Rsp->l_RefOffsetPpb = smz_XOOffsetInfo.l_RefOffsetPpb;
    p_Rsp->l_ClkFreqBiasPpb = smz_XOOffsetInfo.l_ClkFreqBiasPpb;
    p_Rsp->l_ClkFreqBiasUncPpb = smz_XOOffsetInfo.l_ClkFreqBiasUncPpb;
     if ( p_Rsp->u_XoOffsetInfoStatus == TRUE ) 
    {
      /* Set the status in the response, 0 => Success */
      diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 0);
    }
    else
    {
      diagpkt_subsys_set_status(&(p_Rsp->z_DiagCmdHdrV2), 1);
    }
  }
  return(p_Rsp);
}

/*  Diag table for the session control function */
static const diagpkt_user_table_entry_type ariesOneDiag_LegacyTbl[] =
{
  {DIAG_GPS_SESS_CTRL_F, DIAG_GPS_SESS_CTRL_F, ariesDiag_SessCtrl}
};

/*  Diag table for the new subsystem commands */
static const diagpkt_user_table_entry_type tm_DiagTable[] =
{
  {TM_DIAG_SESS_REQ_START_COMMAND,           TM_DIAG_SESS_REQ_START_COMMAND,           tm_diag_sess_start_req_cmd_handler},
  {TM_DIAG_SESS_REQ_STOP_COMMAND,            TM_DIAG_SESS_REQ_STOP_COMMAND,            tm_diag_sess_stop_req_cmd_handler},
  {TM_DIAG_SESS_REQ_CONTINUE_COMMAND,        TM_DIAG_SESS_REQ_CONTINUE_COMMAND,        tm_diag_sess_continue_req_cmd_handler},
  {TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND,  TM_DIAG_SESS_DELETE_AIDING_DATA_COMMAND,  tm_diag_sess_del_aiding_data_cmd_handler},
  {TM_DIAG_SESS_REQ_SW_VER_COMMAND,          TM_DIAG_SESS_REQ_SW_VER_COMMAND,          tm_diag_sess_req_sw_ver_cmd_handler},
  {TM_DIAG_SESS_SARF_COMMAND,                TM_DIAG_SESS_SARF_COMMAND,                tm_diag_sarf_cmd_handler},
  {TM_DIAG_PORT_CONTROL_COMMAND,             TM_DIAG_PORT_CONTROL_COMMAND,             tm_diag_port_control_cmd_handler}
};

static const diagpkt_user_table_entry_type gpsdiag_SubsysCmdTable_v2[] = 
{
  {TM_DIAG_QUERY_BLANKING_CMD, TM_DIAG_QUERY_BLANKING_CMD, tm_diag_query_blanking_cmd_handler},
  {TM_DIAG_NAV_CONFIG_CMD, TM_DIAG_NAV_CONFIG_CMD, tm_diag_nav_config_cmd_handler},
  {TM_DIAG_NAV_IOCTL_CMD,TM_DIAG_NAV_IOCTL_CMD, tm_diag_ioctl_cmd_handler},
  {TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD, TM_DIAG_QUERY_ADC_IQ_SAT_PER_CMD, tm_diag_query_adc_iq_sat_per_cmd_handler },
  {TM_DIAG_EXT_WB_FFT_RESULTS_CMD, TM_DIAG_EXT_WB_FFT_RESULTS_CMD, tm_diag_ext_wb_fft_results_cmd_handler },
  {TM_DIAG_QUERY_XO_OFFSET_INFO_CMD, TM_DIAG_QUERY_XO_OFFSET_INFO_CMD, tm_diag_query_xo_offset_info_cmd_handler}
};

/*===========================================================================

FUNCTION tm_diag_start_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  prtl_start_sess_req_result_e_type

SIDE EFFECTS
  None.

===========================================================================*/
prtl_start_sess_req_result_e_type tm_diag_start_sess_req_handler
(
  tm_sess_handle_type               sess_handle,
  prtl_start_sess_req_param_s_type  *start_sess_req_param,
  tm_sess_req_start_param_s_type    *actions
)
{
  (void)sess_handle;  
  (void)start_sess_req_param;
  (void)actions;  

  return PRTL_START_SESS_REQ_ALLOWED;
}

/*===========================================================================

FUNCTION tm_diag_stop_sess_req_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_stop_sess_req_handler
(
  tm_sess_handle_type           sess_handle,
  prtl_sess_stop_type           stop_type,  
  tm_sess_stop_reason_e_type    stop_reason
)
{
   tm_sess_req_param_u_type tm_sess_req_param;
   
  (void)stop_type;

   tm_sess_req_param.stop_param.stop_type   = TM_STOP_TYPE_SESSION;
   tm_sess_req_param.stop_param.stop_reason = stop_reason;
   
   (void)tm_sess_req(TM_PRTL_TYPE_DIAG,
                     TM_SESS_HANDLE_FTEST,
                     TM_SESS_REQ_STOP,
                     &tm_sess_req_param);
                  
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_diag_sess_req_data_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_sess_req_data_handler
(
  tm_sess_handle_type  session_handle,
  prtl_data_req_type   req_data
)
{
  (void)session_handle;  
  (void)req_data;

  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_diag_sess_info_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_sess_info_handler
(
  tm_sess_handle_type                session_handle,
  prtl_sess_info_e_type              sess_info_type,
  prtl_sess_info_param_u_type       *sess_info_param
)
{
  if(sess_info_param == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Null sess_info_param pointer", 0, 0, 0);   
    return FALSE;
  }
  
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Invalid handle received", 0, 0, 0);
    return FALSE;
}
/*===========================================================================

FUNCTION tm_diag_timer_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_timer_cb_handler
(
  void *timer_data
)
{
  (void)timer_data;  

  return;
}

/*===========================================================================

FUNCTION tm_diag_event_cb_handler

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean tm_diag_event_cb_handler
(
  prtl_event_e_type  event_type,
  prtl_event_u_type  *event_payload
)
{
  return TRUE;
}
/*===========================================================================

FUNCTION: tm_diag_init

DESCRIPTION:
  
PARAMETERS:
  
===========================================================================*/
void tm_diag_init(void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, ariesOneDiag_LegacyTbl);
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_GPS, tm_DiagTable); /*lint !e641 */
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F,(uint8)DIAG_SUBSYS_GPS, gpsdiag_SubsysCmdTable_v2);
}

/*===========================================================================

FUNCTION: tm_diag_init_prtl_table

DESCRIPTION:
  
PARAMETERS:
  
===========================================================================*/
void tm_diag_init_prtl_table(void)
{
  tm_diag_prtl_table.start_sess_req_fp   = tm_diag_start_sess_req_handler; 
  tm_diag_prtl_table.stop_sess_req_fp    = tm_diag_stop_sess_req_handler;
  tm_diag_prtl_table.sess_req_data_fp    = tm_diag_sess_req_data_handler;
  tm_diag_prtl_table.sess_info_fp        = tm_diag_sess_info_handler; 
  tm_diag_prtl_table.timer_cb_fp         = tm_diag_timer_cb_handler;
  tm_diag_prtl_table.event_cb_fp         = tm_diag_event_cb_handler;
  tm_diag_prtl_table.gm_event_handle_fp  = NULL;
}

/*===========================================================================

FUNCTION tm_diag_prtl_init

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_prtl_init(void)
{
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_diag_prtl_init", 0, 0, 0);
  
  tm_diag_init_prtl_table();
  
  (void)tm_prtl_reg(TM_PRTL_TYPE_DIAG, &tm_diag_prtl_table);
}

/*===========================================================================

FUNCTION tm_diag_wbiq_info_handler

DESCRIPTION 
  Handles new info received for the WBIQ report.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_wbiq_info_handler(const sm_ReportWBIQInfoStructType *p_ReportWBIQInfo)
{
  gpsdiag_FactoryIqTestDelayedRspType *p_DelayedRsp = NULL;
  int i;

  if(p_ReportWBIQInfo != NULL)
  {
    tm_diag_wbiq_info = *p_ReportWBIQInfo;
    tm_diag_wbiq_info_cnt++;

    /* If a delayed response for Factory I/Q Test was requested,
       send it now */
    if(z_tm_diag_factory_iq_test.v_FactoryIqTestRequested == TRUE)
    {
      p_DelayedRsp = (gpsdiag_FactoryIqTestDelayedRspType *)
        diagpkt_subsys_alloc_v2_delay((diagpkt_subsys_id_type) DIAG_SUBSYS_GPS,
                                       TM_DIAG_FACTORY_IQ_TEST_CMD,
                                       z_tm_diag_factory_iq_test.w_DelayedRspId,
                                       sizeof (gpsdiag_FactoryIqTestDelayedRspType));

      if(p_DelayedRsp != NULL)
      {
        /* Set the response count to 1 */
		diagpkt_subsys_set_rsp_cnt(&(p_DelayedRsp->z_DiagCmdHdrV2), 1);
        /* Set the status to 0 => Success */         
		diagpkt_subsys_set_status(&(p_DelayedRsp->z_DiagCmdHdrV2), 0); 
        /* Fill in the rest of the delayed response */
        p_DelayedRsp->u_Version = z_tm_diag_factory_iq_test.u_Version;
        p_DelayedRsp->u_GnssConfig = p_ReportWBIQInfo->u_GnssConfig;
        p_DelayedRsp->q_GpsL1CnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_GpsL1CNoDBHz));
        p_DelayedRsp->l_GpsL1FreqHz = ((int32)p_ReportWBIQInfo->f_GpsL1FreqHz);
        p_DelayedRsp->q_GloG1CnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_GloG1CNoDBHz));
        p_DelayedRsp->l_GloG1FreqHz = ((int32)p_ReportWBIQInfo->f_GloG1FreqHz);
        p_DelayedRsp->q_BdsB1CnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_BdsB1CNoDBHz));
        p_DelayedRsp->l_BdsB1FreqHz = ((int32)p_ReportWBIQInfo->f_BdsB1FreqHz);
        p_DelayedRsp->q_GalE1CnoP1DBHz = ((uint32)(10.0*p_ReportWBIQInfo->f_GalE1CNoDBHz));
        p_DelayedRsp->l_GalE1FreqHz = ((int32)p_ReportWBIQInfo->f_GalE1FreqHz);
        p_DelayedRsp->l_AdcMeanIp1mV = p_ReportWBIQInfo->l_AdcMeanIp1mV;
        p_DelayedRsp->l_AdcMeanQp1mV = p_ReportWBIQInfo->l_AdcMeanQp1mV;
        p_DelayedRsp->q_AdcAmpIp1mV = p_ReportWBIQInfo->q_AdcAmpIp1mV;
        p_DelayedRsp->q_AdcAmpQp1mV = p_ReportWBIQInfo->q_AdcAmpQp1mV;
        p_DelayedRsp->u_AdcIqSatPerStatus = p_ReportWBIQInfo->u_AdcIqSatPerStatus;
        p_DelayedRsp->q_ISatPer = p_ReportWBIQInfo->q_ISatPer;
        p_DelayedRsp->q_QSatPer = p_ReportWBIQInfo->q_QSatPer;
        p_DelayedRsp->l_AdcIqWbJammerPwrEstDb = p_ReportWBIQInfo->l_AdcIqWbJammerPwrEstDb;
        p_DelayedRsp->l_GpsL1NoiseFloorEstDb = p_ReportWBIQInfo->l_GpsL1NoiseFloorEstDb;
        p_DelayedRsp->l_GloG1NoiseFloorEstDb = p_ReportWBIQInfo->l_GloG1NoiseFloorEstDb;
        p_DelayedRsp->l_BdsB1NoiseFloorEstDb = p_ReportWBIQInfo->l_BdsB1NoiseFloorEstDb;
        p_DelayedRsp->l_GalE1NoiseFloorEstDb = p_ReportWBIQInfo->l_GalE1NoiseFloorEstDb;
        for(i=0; i<N_GLO_FREQ; i++)
        {
          p_DelayedRsp->l_GloG1FreqNoiseFloorEstDb[i] =
            p_ReportWBIQInfo->l_GloG1FreqNoiseFloorEstDb[i];
        }
        p_DelayedRsp->u_GnssConfig = p_ReportWBIQInfo->q_MultiBandConfig;
        p_DelayedRsp->q_GpsL5GalE5aCnoP1DBHz = 
          ((uint32)(10.0*p_ReportWBIQInfo->f_GpsL5GalE5aCNoDBHz));
        p_DelayedRsp->l_GpsL5GalE5aFreqHz = 
          ((int32)p_ReportWBIQInfo->f_GpsL5GalE5aFreqHz);

        MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
          "tm_diag_wbiq_info_handler: Sending delayed response for ID 0x%04X", z_tm_diag_factory_iq_test.w_DelayedRspId); 

        /* Send it! */
        diagpkt_delay_commit((void *)p_DelayedRsp);
      }
      else
      {
        MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_wbiq_info_handler: Unable to allocate delayed response packet"); 
      }

      /* Clear out the cached flag and ID */
      z_tm_diag_factory_iq_test.v_FactoryIqTestRequested = FALSE;
      z_tm_diag_factory_iq_test.w_DelayedRspId = 0;
    }
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
          "tm_diag_wbiq_info_handler: Input pointer is NULL"); 
  }
}

/*===========================================================================

FUNCTION tm_diag_xo_offset_info_handler

DESCRIPTION 
  Caches new info received for the XO offset reporting

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void tm_diag_xo_offset_info_handler(const sm_ReportXOOffsetInfoStructType *pz_ReportXOOffsetInfo)
{
  if(pz_ReportXOOffsetInfo != NULL)
  {
    smz_XOOffsetInfo = *pz_ReportXOOffsetInfo;
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, 
        "tm_diag_xo_offset_info_handler: Input pointer is NULL"); 
    smz_XOOffsetInfo.v_IsDataValid = FALSE;
  }
}/*=============================================================================

FUNCTION
  tm_diag_ReportIqTestCapabilities

DESCRIPTION
  This function is used to send Nav IQ Test capabilities to TM Diag, such
  that unsupported IQ test requests can be rejected.

DEPENDENCIES
  None.

PARAMETERS
  q_IqTestMask - Bit mask of the supported IQ tests, bit shifts defined in
  e_CollectModeTypeVal

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void tm_diag_ReportIqTestCapabilities(uint32 q_IqTestMask)
{
  tm_diag_sarf_iq_test_mask = q_IqTestMask;
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
        "tm_diag_ReportIqTestCapabilities: IQ Test Mask 0x%08X", 
        tm_diag_sarf_iq_test_mask); 
}

/*=============================================================================

FUNCTION
  tm_diag_ReportRfDevTrackCapabilities

DESCRIPTION
  This function is used to send RF Dev Multichannel Track capabilities to
  TM Diag

DEPENDENCIES
  None.

PARAMETERS
  q_ConstellationMask - Bit mask of the Multichannel Track tests

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/
void tm_diag_ReportRfDevTrackCapabilities(gnss_RfDevTrackConstellationMaskType q_ConstellationMask)
{
  tm_diag_rfdev_track_constellation_mask = q_ConstellationMask;
  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, 
        "tm_diag_ReportRfDevTrackCapabilities: RF Dev Constellation Mask 0x%08X", 
        q_ConstellationMask); 
}
