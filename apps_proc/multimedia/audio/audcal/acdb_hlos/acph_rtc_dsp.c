/** 
  \file **************************************************************************
 *
 *  A U D I O   C A L I B R A T I O N   P A C K E T   H A N D L E R   
 *
 *DESCRIPTION
 * This file contains the implementation of RTC AUDIO VOICE INTERFACES 
 *
 *REFERENCES
 * None.
 *
 *Copyright (c) 2013, 2016 by QUALCOMM Technology, Incorporated.  All Rights Reserved.
 *******************************************************************************
 */
/**
  \file ***************************************************************************
 *
 *                      EDIT HISTORY FOR FILE
 *
 *  This section contains comments describing changes made to this file.
 *  Notice that changes are listed in reverse chronological order.
 *   
 *  $Header: acph.h
 *
 *when         who     what, where, why
 *--------     ---     ----------------------------------------------------------
  11/27/17     pm       Add support for QSEE version 4.0
  11/15/16     rk       Abstracting OSAL layer
  03/15/13     jyuan    Fix for APR resp msg size mismatch
  08/03/11     ernanl   initial draft
 ********************************************************************************
 */ 
/* $Header: //components/rel/audio.tx/2.0/audio/audcal/acdb_hlos/acph_rtc_dsp.c#1 $ */

/*
   -------------------------------
   |Include Files                |
   -------------------------------
   */
#include "acph_rtc_dsp.h"
#include "csd_voc_common.h"
#include "adsp_afe_service_commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IxErrno.h"
#include "scm.h"

int32_t csd_voice_debug_ioctl(uint32_t handle, uint32_t code, void* params, uint32_t size);
CSD_INTERNAL int32_t csd_vc_ioctl(uint32_t handle, uint32_t code, void* params, uint32_t size);
CSD_INTERNAL int32_t csd_vs_ioctl(uint32_t handle, uint32_t code, void* params, uint32_t size);

/*===========================================================================
  STATIC VARIABLES
  ===========================================================================*/
static uint32_t voice_debug_handle = 0;
/* APR Registration */
static uint32_t rtc_apr_handle = 0;
static uint16_t rtc_apr_src_addr = 0;
static char_t rtc_dns[] = "qcom.audio.rtc";

static char_t rtc_dest_dns_cvp[] = "qcom.audio.cvp";
static uint16_t rtc_dest_addr_cvp = 0;

static char_t rtc_dest_dns_cvs[] = "qcom.audio.cvs";
static uint16_t rtc_dest_addr_cvs = 0;

static char_t rtc_dest_dns_afe[] = "qcom.audio.afe";
static uint16_t rtc_dest_addr_afe = 0;

static char_t rtc_dest_dns_mvm[] = "qcom.audio.mvm";
static uint16_t rtc_dest_addr_mvm = 0;

static uint8_t* pRespBuf = NULL;
static uint32_t pRespBufLen = 0;
static uint32_t* pBufLeng = NULL;
static uint32_t nAPRstatus = 0;
static rtcq6intf rtc_main_data;
static rtc_oob_mem_t rtc_mem;

/*===========================================================================
  Macro
  ===========================================================================*/
#define RTC_INBAND_BUFFER_SIZE      0x1000
#define RTC_OOB_BUFFER_SIZE         0x2000   
#define RTC_DESCRIPTOR_TABLE_SIZE   1024
#define RTC_SYNC_TIMEOUT            5000000   //5 sec time out

/*===========================================================================
  INTERNAL FUNCTIONS
  ===========================================================================*/


/**
* FUNCTION : rtc_APR_cbfn
*
* DESCRIPTION : generate apr header
*
* DEPENDENCIES : None
*
* PARAMS:
*   Input - nPayloadSize - Payload size
*   Input - pBufferPtr - APR packet pointer
*
* RETURN VALUE : None
*
* SIDE EFFECTS : None
*/
static void rtc_APR_cbfn( aprv2_packet_t* packet, void* dispatch_data)
{
  if( !packet )
  {
    RTC_MSG( RTC_PRIO_ERROR, "Input arg[*packet] is NULL" );
    return;
  }
  
  csd_lock_enter( rtc_main_data.lock );
  switch( packet->opcode )
  {
    case APRV2_IBASIC_EVT_ACCEPTED:
    break;

    case VSS_ICOMMON_RSP_GET_PARAM:
    {
      uint16_t nBufferSize = 0;
      vss_icommon_rsp_get_param_t *rsp = APRV2_PKT_GET_PAYLOAD(vss_icommon_rsp_get_param_t, packet);

      nAPRstatus = rsp->status;
      RTC_MSG( RTC_PRIO_HIGH, "Received VSS_ICOMMON_RSP_GET_PARAM rsp from DSP, rc[0x%x]", nAPRstatus );
      
      if( nAPRstatus == APR_EOK )
      {        
        // Get the param_size in vss_icommon_param_data_t
        csd_memscpy( &nBufferSize, sizeof(uint16_t), (rtc_mem.pva_data+2*sizeof(uint32_t)), sizeof(uint16_t) );
        RTC_MSG( RTC_PRIO_HIGH, "Param Size[%d]", nBufferSize );
        
        // Copy the data to the resp buffer        
        csd_memscpy( pRespBuf, pRespBufLen, rtc_mem.pva_data, (nBufferSize+3*sizeof(uint32_t)) );        
        *pBufLeng = nBufferSize+3*sizeof(uint32_t);        
      }      
    }
    break;
    
    case AFE_PORT_CMDRSP_GET_PARAM_V2:
    {
      uint32_t nBufferSize = 0;
      afe_port_cmdrsp_get_param_v2_t *rsp = APRV2_PKT_GET_PAYLOAD(afe_port_cmdrsp_get_param_v2_t, packet);

      nAPRstatus = rsp->status;
      RTC_MSG( RTC_PRIO_HIGH, "Received AFE_PORT_CMDRSP_GET_PARAM_V2 rsp from DSP, rc[0x%x]", nAPRstatus );
      
      if( nAPRstatus == APR_EOK )
      {
        csd_memscpy( &nBufferSize, sizeof(uint32_t), ( (uint8_t*)rsp+3*sizeof(uint32_t) ), sizeof(uint16_t) );

        csd_memscpy( pRespBuf, pRespBufLen, ( (uint8_t*)rsp+sizeof(uint32_t) ), nBufferSize+3*sizeof(uint32_t) );
        *pBufLeng = nBufferSize+3*sizeof(uint32_t);            
      }
    }
    break;
    
    case VSS_IMEMORY_RSP_MAP: /* by definition a successful response */
    {
      vss_imemory_rsp_map_t* pmemmap_rsp = (vss_imemory_rsp_map_t*)APRV2_PKT_GET_PAYLOAD( uint8_t, packet );

      RTC_MSG( RTC_PRIO_HIGH, "rtc_APR_cbfn: VSS_IMEMORY_CMD_MAP success, received mem_handle[0x%x]", pmemmap_rsp->mem_handle );
      rtc_mem.descriptor_mapped_handle = pmemmap_rsp->mem_handle;
    }
    break;
    
    case APRV2_IBASIC_RSP_RESULT:
    {
      aprv2_ibasic_rsp_result_t *rsp = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet );
      if( rsp )
      {
        nAPRstatus = rsp->status;
        switch( rsp->opcode )
        {
          case VSS_ICOMMON_CMD_SET_PARAM_V2:
            if( nAPRstatus == APR_EOK )
            {
              RTC_MSG( RTC_PRIO_HIGH, "VSS_ICOMMON_CMD_SET_PARAM_V2 success" );
            }
            else
            {
              RTC_MSG( RTC_PRIO_ERROR, "VSS_ICOMMON_CMD_SET_PARAM_V2 failed, apr_rc[0x%x]", nAPRstatus );
            }            
          break;
          
          case AFE_PORT_CMD_SET_PARAM_V2:
            if( nAPRstatus == APR_EOK )
            {
              RTC_MSG( RTC_PRIO_HIGH, "AFE_PORT_CMD_SET_PARAM_V2 success" );
            }
            else
            {
              RTC_MSG( RTC_PRIO_ERROR, "AFE_PORT_CMD_SET_PARAM_V2 failed, apr_rc[0x%x]", nAPRstatus );
            }
          break;
          
          case VSS_IMEMORY_CMD_MAP_PHYSICAL: /* this is an error case */
            RTC_MSG( RTC_PRIO_ERROR, "rtc_APR_cbfn: VSS_IMEMORY_CMD_MAP_PHYSICAL failed reported by MVM, apr_rc[%d]", nAPRstatus );
          break;
          
          case VSS_IMEMORY_CMD_UNMAP:              
            RTC_MSG( RTC_PRIO_HIGH, "rtc_APR_cbfn: Received VSS_IMEMORY_CMD_UNMAP response, apr_rc[0x%x]", nAPRstatus );            
          break;
          
          default:
            RTC_MSG( RTC_PRIO_ERROR, "Unknown opcode[0x%x]", rsp->opcode );
          break;
        }
      }
    }
    break;
    
    default:
      RTC_MSG( RTC_PRIO_ERROR, "Unrecognized Q6 Response\n");
      nAPRstatus = RTC_INTF_EFAILURE;
    break;
  }

  if( packet->opcode != APRV2_IBASIC_EVT_ACCEPTED )
  {
    csd_event_signal( rtc_main_data.trans_obj.sync_event_handle );
  }
  
  csd_lock_leave(rtc_main_data.lock);
  __aprv2_cmd_free(0,packet);
}

/**
* FUNCTION : rtc_get_q6_cal_data
*
* DESCRIPTION : Get real time calibration data
*
* DEPENDENCIES : DSP must be active
*
* PARAMS:
*   mode - get/set param operation mode
*   req_buf_ptr - pointer to request buffer
*   resp_buf_ptr - pointer to response buffer
*   resp_buf_length - length of the response buffer
*
* RETURN VALUE : None
*
* SIDE EFFECTS : None
*/

int32_t rtc_transfer_q6_cal_data(
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_total_len,
  uint32_t *resp_buf_length,
  uint16_t nCommandId )
{
  int32_t rc = RTC_INTF_SUCCESS;
  uint32_t csd_apr_handle = 0;
  uint16_t dst_addr = 0;
  uint16_t dst_portId = 0;  

  ACPH_CMD_RTC_GET_CAL_DATA_req* GetCalData_Req = (ACPH_CMD_RTC_GET_CAL_DATA_req*)req_buf_ptr;
  ACPH_CMD_RTC_SET_CAL_DATA_req* SetCalData_Req = (ACPH_CMD_RTC_SET_CAL_DATA_req*)req_buf_ptr;
  
  // Preprocessing: Get CVP/CVS handle 
  switch( nCommandId )
  {
    case ACPH_CMD_RTC_GET_CAL_DATA:
    case ACPH_CMD_RTC_SET_CAL_DATA:
    {
      uint32_t service_id = GetCalData_Req->service_id;
      uint32_t handle = GetCalData_Req->pp_id;
      
      switch( service_id )
      {
        case APRV2_IDS_SERVICE_ID_ADSP_VSM_V:
        {          
          csd_debug_voice_ioctl_get_cvs_handle_t param;
          param.csd_vs_handle = handle;
          param.ret_cvs_handle = &dst_portId;
          
          // Get CVS handle 
          RTC_MSG( RTC_PRIO_MEDIUM, "Calling CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE, handle[0x%x]", handle );      
          rc = csd_voice_debug_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE, 
                                      &param, sizeof(param) );
          if( rc != CSD_EOK )
          {
            RTC_MSG( RTC_PRIO_ERROR, "CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE failed, csd_rc[0x%x]", rc );
            break;
          }   
          
          dst_addr = rtc_dest_addr_cvs;
        }
        break;
        
        case APRV2_IDS_SERVICE_ID_ADSP_VPM_V:
        {
          csd_debug_voice_ioctl_get_cvp_handle_t param;
          param.csd_vc_handle = handle;
          param.ret_cvp_handle = &dst_portId;
          
          // Get CVP handle 
          RTC_MSG( RTC_PRIO_MEDIUM, "Calling CSD_DEBUG_VOICE_IOCTL_GET_CVP_HANDLE, handle[0x%x]", handle );
          rc = csd_voice_debug_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_CVP_HANDLE,
                                      &param, sizeof(param) );
          if( rc != CSD_EOK )
          {
            RTC_MSG( RTC_PRIO_ERROR, "CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE failed, csd_rc[0x%x]", rc );
            break;
          }
          
          dst_addr = rtc_dest_addr_cvp;
        }
        break;
        
        default:
          RTC_MSG( RTC_PRIO_ERROR, "Unknown service id[0x%x]", service_id );
          rc = RTC_INTF_EFAILURE;
        break;
      }
    }
    break;
    
    case ACPH_GET_AFE_DATA:
    case ACPH_SET_AFE_DATA:
      dst_addr = rtc_dest_addr_afe;
    break;
    
    default:
      RTC_MSG( RTC_PRIO_ERROR, "Unknown cmdid[0x%x]", nCommandId );
      rc = RTC_INTF_EFAILURE;
    break;
  }

  // Send to DSP 
  if( rc == RTC_INTF_SUCCESS )
  {
    switch( nCommandId )
    {
      case ACPH_CMD_RTC_GET_CAL_DATA:
      {
        vss_icommon_cmd_get_param_v2_t cmd;
        
        cmd.mem_handle = rtc_mem.descriptor_mapped_handle;
        cmd.mem_address = rtc_mem.phy_addr_data;
        cmd.mem_size = RTC_OOB_BUFFER_SIZE;
        cmd.module_id = GetCalData_Req->module_id;
        cmd.param_id = GetCalData_Req->parameter_id;        

        rc = __aprv2_cmd_alloc_send( rtc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     rtc_apr_src_addr, 0,
                                     dst_addr, dst_portId,
                                     0, VSS_ICOMMON_CMD_GET_PARAM_V2,
                                     &cmd, sizeof(vss_icommon_cmd_get_param_v2_t) );
        RTC_MSG( RTC_PRIO_HIGH, "VSS_ICOMMON_CMD_GET_PARAM_V2 sent, apr_rc[0x%x], module_id[0x%x], param_id[0x%x]", 
                 rc, cmd.module_id, cmd.param_id );                                    
      }
      break;
      
      case ACPH_CMD_RTC_SET_CAL_DATA:
      {
        vss_icommon_cmd_set_param_v2_t cmd;
        vss_icommon_param_data_t common_cmd;
        uint8_t* pdata = NULL;
        
        common_cmd.module_id = SetCalData_Req->module_id;
        common_cmd.param_id = SetCalData_Req->parameter_id;
        common_cmd.param_size = SetCalData_Req->length;
        common_cmd.reserved = 0;
        
        cmd.mem_handle = rtc_mem.descriptor_mapped_handle;
        cmd.mem_address = rtc_mem.phy_addr_data;        
        cmd.mem_size = sizeof(vss_icommon_param_data_t) + common_cmd.param_size;
        
        pdata = rtc_mem.pva_data;
        csd_memscpy( pdata, RTC_OOB_BUFFER_SIZE, &common_cmd, sizeof(common_cmd) );
        pdata += sizeof(common_cmd);
        csd_memscpy( pdata, (RTC_OOB_BUFFER_SIZE-sizeof(common_cmd)),((uint8_t*)req_buf_ptr+sizeof(ACPH_CMD_RTC_SET_CAL_DATA_req)), common_cmd.param_size );
        
        rc = __aprv2_cmd_alloc_send( rtc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     rtc_apr_src_addr, 0,
                                     dst_addr, dst_portId,
                                     0, VSS_ICOMMON_CMD_SET_PARAM_V2,
                                     &cmd, sizeof(cmd) );
                  
        RTC_MSG( RTC_PRIO_HIGH, "VSS_ICOMMON_CMD_SET_PARAM_V2 sent, apr_rc[0x%x], module_id[0x%x], param_id[0x%x]", 
                 rc, common_cmd.module_id, common_cmd.param_id );              
      }
      break;
      
      case ACPH_GET_AFE_DATA:
      {
        ACPH_GET_AFE_DATA_req* GetAfeData_Req = (ACPH_GET_AFE_DATA_req*)req_buf_ptr;
        afe_port_cmd_get_param_v2_t cmd;

        cmd.port_id = GetAfeData_Req->afe_port_id;
        cmd.payload_size = RTC_INBAND_BUFFER_SIZE;
        cmd.payload_address_lsw = NULL;
        cmd.payload_address_msw = NULL;
        cmd.mem_map_handle = NULL;
        cmd.module_id = GetAfeData_Req->module_id;
        cmd.param_id = GetAfeData_Req->parameter_id;
        
        rc = __aprv2_cmd_alloc_send( csd_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                     rtc_apr_src_addr, 0,
                                     dst_addr, dst_portId,
                                     0, AFE_PORT_CMD_GET_PARAM_V2,
                                     &cmd, sizeof(afe_port_cmd_get_param_v2_t) );
        RTC_MSG( RTC_PRIO_HIGH, "AFE_PORT_CMD_GET_PARAM_V2 sent, apr_rc[0x%x], afe_port_id[0x%x], module_id[0x%x], param_id[0x%x]", 
                 rc, cmd.port_id, cmd.module_id, cmd.param_id );  
      }
      break;
      
      case ACPH_SET_AFE_DATA:
      {
        ACPH_SET_AFE_DATA_req* SetAfeData_Req = (ACPH_SET_AFE_DATA_req*)req_buf_ptr;
        afe_port_cmd_set_param_v2_t cmd;
        afe_port_param_data_v2_t param_data;
        aprv2_packet_t* papr_packet = NULL;
        
        if( SetAfeData_Req->op_mode == tx )
        {
          cmd.port_id = SetAfeData_Req->tx_afe_port_id;
        }
        else if( SetAfeData_Req->op_mode == rx )
        {
          cmd.port_id = SetAfeData_Req->rx_afe_port_id;
        }
        else
        { 
          RTC_MSG( RTC_PRIO_ERROR, "Unsupported op_mode[%d] for ACPH_SET_AFE_DATA", SetAfeData_Req->op_mode );
          rc = RTC_INTF_EFAILURE;
          break;
        }
        
        cmd.payload_address_lsw = 0;
        cmd.payload_address_msw = 0;
        cmd.mem_map_handle = 0;
        param_data.module_id = SetAfeData_Req->module_id;
        param_data.param_id = SetAfeData_Req->parameter_id;
        param_data.param_size = SetAfeData_Req->length;
        
        rc = __aprv2_cmd_alloc_ext( csd_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                    rtc_apr_src_addr, 0,
                                    dst_addr, dst_portId,
                                    0, AFE_PORT_CMD_SET_PARAM_V2,
                                    sizeof(afe_port_cmd_set_param_v2_t) + param_data.param_size,
                                    &papr_packet );
                                    
        if( rc == APR_EOK && papr_packet )
        {  // copy the param_data into apr buffer 
          uint8_t* pdata = APRV2_PKT_GET_PAYLOAD( uint8_t, papr_packet );
          uint32_t psize = APRV2_PKT_GET_PAYLOAD_BYTE_SIZE( papr_packet->header );
          csd_memscpy( pdata, psize, &cmd, sizeof(afe_port_cmd_set_param_v2_t) ); // copy the cmd structure
          pdata += sizeof(afe_port_cmd_set_param_v2_t);
          csd_memscpy( pdata, psize, (void*)&param_data, sizeof(afe_port_param_data_v2_t) );
          pdata += sizeof(afe_port_param_data_v2_t) ;
          csd_memscpy( pdata, psize, ( (uint8_t*)req_buf_ptr + sizeof(ACPH_SET_AFE_DATA_req) ), param_data.param_size );
          rc = __aprv2_cmd_forward( csd_apr_handle, papr_packet );
          
          RTC_MSG( RTC_PRIO_HIGH, "AFE_PORT_CMD_SET_PARAM_V2 sent, apr_rc[0x%x], module_id[0x%x], param_id[0x%x]", 
                   rc, param_data.module_id, param_data.param_id );      
        }
      }
      
      default:
        RTC_MSG( RTC_PRIO_ERROR, "Unknown cmdid[0x%x]", nCommandId );
        rc = RTC_INTF_EFAILURE;
      break;
    }
  }
  
  // wait for response 
  if( rc == RTC_INTF_SUCCESS )
  {
    rc = csd_event_wait_timed(rtc_main_data.trans_obj.sync_event_handle, (uint32)rtc_main_data.sync_send_timeout);    
  }

  if( rc != CSD_OSAL_EOK )
  {
    RTC_MSG( RTC_PRIO_ERROR, "csd_event_wait_timed failed, rc[0x%x]", rc );
    rc = ACPH_FAILURE; // TODO: Check QACT reaction on differnt error codes 
  }
  else
  {
    rc = nAPRstatus;
  }

  return rc;
}


/**
 * FUNCTION : query_voc_all_active_streams
 *
 * DESCRIPTION : query for all active voice streams
 *
 * DEPENDENCIES : CSD needs to be available and initialized
 *
 * PARAMS:
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : None
 *
 * SIDE EFFECTS : None
 *
 * Expected data input: None
 *
 * Expected data output:
 *              number of streams (32-bit)
 *              voice_stream_array[n]
 *     Each element of voice_stream_array is:
 *              cvs_handle (32-bit)
 *              voc_vs_handle (32-bit)
 *
 */
 
static int32_t query_voc_all_active_streams(
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_total_len,
  uint32_t *resp_buf_length )
{  
  csd_debug_voice_ioctl_get_stream_list_t get_stream_list;
  uint8_t* pOutputBuffer = (uint8_t*)(resp_buf_ptr);
  uint32_t result_flag = 0;
  uint32_t total_size = 0, write_size = 0;
  void* temp_buffer = NULL;

  csd_malloc( RTC_INBAND_BUFFER_SIZE, &temp_buffer );
  if( !temp_buffer )
  {
    RTC_MSG( RTC_PRIO_ERROR, "query_voc_all_active_streams: Memory allocation failed" );
    return ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }
  
  get_stream_list.buffer = (uint32_t*)temp_buffer;
  get_stream_list.buffer_size = RTC_INBAND_BUFFER_SIZE;
  get_stream_list.ret_total_size = &total_size;
  get_stream_list.ret_write_size = &write_size;
  result_flag = csd_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_STREAM_LIST, 
                           &get_stream_list,sizeof(get_stream_list) );

  if( result_flag != 0 )
  {
    RTC_MSG( RTC_PRIO_ERROR, "CSD_DEBUG_VOICE_IOCTL_GET_STREAM_LIST failed, rc[0x%x]", result_flag );
    result_flag = ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }
  else
  {
    ACPH_CMD_QUERY_VOC_ALL_ACTIVE_STREAMS_rsp* pNoOfStreams =
                                     (ACPH_CMD_QUERY_VOC_ALL_ACTIVE_STREAMS_rsp*)pOutputBuffer;
    uint32_t num_entries = total_size/sizeof(uint32_t), i = 0;

    pNoOfStreams->num_of_streams = num_entries;
    pOutputBuffer = pOutputBuffer + sizeof(ACPH_CMD_QUERY_VOC_ALL_ACTIVE_STREAMS_rsp);

    for( i=0; i<num_entries; i++ )
    {
      VOC_ALL_ACTIVE_STREAMS *pActiveStreams = (VOC_ALL_ACTIVE_STREAMS *)pOutputBuffer;
      csd_memscpy( (void*)&pActiveStreams->popp_handle, sizeof(uint32_t), ((uint8_t*)temp_buffer+i*sizeof(uint32_t)), sizeof(uint32_t) );
      csd_memscpy( (void*)&pActiveStreams->popp_id, sizeof(uint32_t), ((uint8_t*)temp_buffer+i*sizeof(uint32_t)), sizeof(uint32_t) );
      pOutputBuffer = pOutputBuffer + sizeof(VOC_ALL_ACTIVE_STREAMS);
    }

    *resp_buf_length = num_entries*sizeof(VOC_ALL_ACTIVE_STREAMS) + sizeof(ACPH_CMD_QUERY_VOC_ALL_ACTIVE_STREAMS_rsp);        
    result_flag = ACPH_SUCCESS;
  }
  
  csd_free(temp_buffer);
  return result_flag;
}

/**
 * FUNCTION : query_voc_vs_copp_handles
 *
 * DESCRIPTION : query for voice copp handles with a given voc_vs_handle
 *
 * DEPENDENCIES : CSD needs to be available and initialized
 *
 * PARAMS:
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : None
 *
 * SIDE EFFECTS : None
 *
 * Expected data input: voc_vs_handle (32-bit)
 *
 * Expected data output:
 *              number of voice copp handles (32-bit)
 *              voice_copp_handle_array[n]
 *      Each element of voice_copp_handle_array is:
 *              cvp_handle (32-bit)
 *              voc_vc_handle (32-bit)
 *
 */

 static int32_t query_voc_vs_copp_handles(
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_total_len,
  uint32_t *resp_buf_length )
{   
  csd_debug_voice_ioctl_get_context_list_t get_context_list;
  uint8_t* pOutputBuffer = (uint8_t*)(resp_buf_ptr);
  uint8_t* pInputBuffer = (uint8_t*)(req_buf_ptr);
  uint32_t result_flag = 0;
  uint32_t total_size = 0, write_size = 0;
  void* temp_buffer = NULL;
  ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_req *pHandle = (ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_req *)pInputBuffer;

  if( sizeof(ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_req) > req_buf_len )
  {    
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }

  csd_malloc( RTC_INBAND_BUFFER_SIZE, &temp_buffer );
  if( !temp_buffer )
  {
    RTC_MSG( RTC_PRIO_ERROR, "query_voc_vs_copp_handles: Memory allocation failed" );
    return ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }
     
  get_context_list.buffer = (uint32_t*)temp_buffer;
  get_context_list.buffer_size = RTC_INBAND_BUFFER_SIZE;
  get_context_list.ret_total_size = &total_size;
  get_context_list.ret_write_size = &write_size;
  get_context_list.csd_vs_handle = pHandle->popp_handle;
  
  result_flag = csd_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_CONTEXT_LIST, 
                           &get_context_list,sizeof(get_context_list) );
  
  if( result_flag != 0 )
  {
    RTC_MSG( RTC_PRIO_ERROR, "CSD_DEBUG_VOICE_IOCTL_GET_CONTEXT_LIST failed, rc[0x%x]", result_flag );
    result_flag = ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }
  else
  {    
    uint32_t i;
    ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_rsp *pNoOfCopps = (ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_rsp*)pOutputBuffer;

    pNoOfCopps->num_of_copps = total_size/sizeof(uint32_t);
    pOutputBuffer = pOutputBuffer + sizeof(ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_rsp);

    for( i = 0; i < pNoOfCopps->num_of_copps; i++ )
    {
      VOC_VS_COPP_HANDLES *pCoppHandles = (VOC_VS_COPP_HANDLES*)pOutputBuffer;
      csd_memscpy((void*)&pCoppHandles->copp_handle, sizeof(uint32_t), ((uint8_t*)temp_buffer+i*sizeof(uint32_t)), sizeof(uint32_t));
      csd_memscpy((void*)&pCoppHandles->copp_id, sizeof(uint32_t), ((uint8_t*)temp_buffer+i*sizeof(uint32_t)), sizeof(uint32_t));
      pOutputBuffer += sizeof(VOC_VS_COPP_HANDLES);
    }
    *resp_buf_length = pNoOfCopps->num_of_copps*sizeof(VOC_VS_COPP_HANDLES) + sizeof(ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES_rsp);
        
    result_flag = ACPH_SUCCESS;
  }
  
  csd_free(temp_buffer);
  return result_flag;
}

/**
 * FUNCTION : query_voc_vc_devices
 *
 * DESCRIPTION : query for voice devices with a given voc_vc_handle
 *
 * DEPENDENCIES : CSD needs to be available and initialized
 *
 * PARAMS:
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : None
 *
 * SIDE EFFECTS : None
 *
 * Expected data input: voc_vc_handle (32-bit)
 *
 * Expected data output:
 *              number of voice devices (32-bit)
 *              voice_device_array[n]
 *      Each element of voice_device_array is:
 *              voice_rx_device_id (32-bit)
 *              voice_tx_device_id (32-bit)
 *
 */

 static int32_t query_voc_vc_devices(
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_total_len,
  uint32_t *resp_buf_length )
{
  csd_debug_voice_ioctl_get_device_ids_t  get_device_ids;

  uint8_t* pOutputBuffer = (uint8_t*)(resp_buf_ptr);
  uint8_t* pInputBuffer = (uint8_t*)(req_buf_ptr);
  uint32_t result_flag = 0, rx_device_id = 0, tx_device_id = 0;
  uint32_t num_entries = 1;
  ACPH_CMD_QUERY_VOC_VC_DEVICES_req *voc_handles = (ACPH_CMD_QUERY_VOC_VC_DEVICES_req*)pInputBuffer;

  ACPH_CMD_QUERY_VOC_VC_DEVICES_rsp *num_devices = (ACPH_CMD_QUERY_VOC_VC_DEVICES_rsp*)pOutputBuffer;
  VOC_VC_DEVICES *vcDevices = (VOC_VC_DEVICES *)(pOutputBuffer + sizeof(ACPH_CMD_QUERY_VOC_VC_DEVICES_rsp));

  memset( (void*)&get_device_ids, 0, sizeof(get_device_ids));
    
  if( sizeof(ACPH_CMD_QUERY_VOC_VC_DEVICES_req) > req_buf_len )
  {    
    return ACPH_FAILURE;
  }

  get_device_ids.csd_vc_handle = voc_handles->copp_handle;
  get_device_ids.ret_rx_dev_id = &rx_device_id;
  get_device_ids.ret_tx_dev_id = &tx_device_id;
  result_flag = csd_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_DEVICE_IDS, 
                           &get_device_ids, sizeof(get_device_ids) );
  if( 0 != result_flag )
  {
    return ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }

  num_devices->num_of_device_pairs = num_entries;
  if( get_device_ids.ret_rx_dev_id == NULL || get_device_ids.ret_tx_dev_id == NULL )
  {
    return ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }

  vcDevices->rx_device_id = *(get_device_ids.ret_rx_dev_id);
  vcDevices->tx_device_id = *(get_device_ids.ret_tx_dev_id);
  *resp_buf_length = sizeof(ACPH_CMD_QUERY_VOC_VC_DEVICES_rsp) + sizeof(VOC_VC_DEVICES);
  
  return ACPH_SUCCESS;
}

/**
 * FUNCTION : query_voc_afe_active_port_ids
 *
 * DESCRIPTION : query for voice afe port ids with a given vc_handle
 *
 * DEPENDENCIES : CSD needs to be available and initialized
 *
 * PARAMS:
 *   req_buf_ptr - pointer to request buffer
 *   resp_buf_ptr - pointer to response buffer
 *   resp_buf_length - length of the response buffer
 *
 * RETURN VALUE : None
 *
 * SIDE EFFECTS : None
 *
 * Expected data input: vc_handle (32-bit)
 *
 * Expected data output:
 *              tx_afe_port_id (32-bit)
 *              rx_afe_port_id (32-bit)
 *
 */
 
static int32_t query_voc_afe_active_port_ids(
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_total_len,
  uint32_t *resp_buf_length )
{
  uint32_t  result_flag = ACPH_SUCCESS;  
  csd_debug_voice_ioctl_get_afe_port_t voice_debug_get_afe_port;
  ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_rsp *pOutputBuffer = (ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_rsp*)resp_buf_ptr;
  ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_req *pInputBuffer = (ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_req*)req_buf_ptr;
  uint16_t ret_tx_afe_port = 0;
  uint16_t ret_rx_afe_port = 0;
  uint16_t ret_ec_afe_port = 0;
  
  if( sizeof(ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_req) > req_buf_len ||
      sizeof(ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_rsp) > resp_buf_total_len )
  {
    RTC_MSG( RTC_PRIO_ERROR, "Invalid input param" );
    return ACPH_FAILURE;
  }

  voice_debug_get_afe_port.csd_vc_handle = pInputBuffer->voc_copp_handle;
  voice_debug_get_afe_port.ret_rx_afe_port = &ret_rx_afe_port;
  voice_debug_get_afe_port.ret_tx_afe_port = &ret_tx_afe_port;
  voice_debug_get_afe_port.ret_ec_afe_port = &ret_ec_afe_port;
  result_flag = csd_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_AFE_PORT, 
                           &voice_debug_get_afe_port, sizeof(voice_debug_get_afe_port) );
  if( result_flag != CSD_EOK )
  {
    return ACPH_ERR_CSD_VOC_CMD_FAILURE;
  }

  pOutputBuffer->tx_afe_port_id = *voice_debug_get_afe_port.ret_tx_afe_port;
  pOutputBuffer->rx_afe_port_id = *voice_debug_get_afe_port.ret_rx_afe_port;

  *resp_buf_length = sizeof(ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS_rsp);

  return result_flag;
}

static int32_t dsp_rtc_get_version(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  ACPH_CMD_QUERY_DSP_RTC_VERSION_rsp* pDspRtcVersion = (ACPH_CMD_QUERY_DSP_RTC_VERSION_rsp*)resp_buf_ptr;
  pDspRtcVersion->dsp_rtc_major_version = ACPH_SERVICE_MAJOR_VERSION_1;
  pDspRtcVersion->dsp_rtc_minor_version = ACPH_SERVICE_MINOR_VERSION_2;
  *resp_buf_bytes_filled = sizeof(ACPH_CMD_QUERY_DSP_RTC_VERSION_rsp);

  RTC_MSG( RTC_PRIO_HIGH, "Major version[%d], minor version[%d]", ACPH_SERVICE_MAJOR_VERSION_1, ACPH_SERVICE_MINOR_VERSION_2 );
  
  return ACPH_SUCCESS;
}
/*===========================================================================
  FUNCTION acph_rtc_mem_map

  DESCRIPTION Map 8K memory to DSP

  PARAMS:    

  RETURN VALUE ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS
  ===========================================================================*/
static int32_t acph_rtc_mem_map()
{
  int32_t rc = RTC_INTF_SUCCESS;
  vss_imemory_cmd_map_physical_t cvp_cmd;
  vss_imemory_table_t* vss_mem_tbl = NULL;
  vss_imemory_block_t* vss_mem_block = NULL;
  
  vss_mem_tbl = (vss_imemory_table_t*)rtc_mem.pva_descriptor;
  vss_mem_tbl->next_table_descriptor.mem_address = 0;
  vss_mem_tbl->next_table_descriptor.mem_size = 0; 

  vss_mem_block = (vss_imemory_block_t*)( rtc_mem.pva_descriptor + sizeof(vss_imemory_table_descriptor_t) );
  vss_mem_block->mem_address = rtc_mem.phy_addr_data;
  vss_mem_block->mem_size = RTC_OOB_BUFFER_SIZE;
  
  cvp_cmd.table_descriptor.mem_address = rtc_mem.phy_addr_descriptor;
  cvp_cmd.table_descriptor.mem_size = sizeof(vss_imemory_block_t) + sizeof(vss_imemory_table_descriptor_t);
  cvp_cmd.is_cached = TRUE;
  cvp_cmd.cache_line_size = 128;
  cvp_cmd.access_mask = 1;
  cvp_cmd.page_align = 4096;
  cvp_cmd.min_data_width = 8;
  cvp_cmd.max_data_width = 64;
  
  rc = __aprv2_cmd_alloc_send( rtc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                               rtc_apr_src_addr, 0,
                               rtc_dest_addr_mvm, APR_NULL_V,
                               0, VSS_IMEMORY_CMD_MAP_PHYSICAL,
                               &cvp_cmd, sizeof(vss_imemory_cmd_map_physical_t) );
  RTC_MSG( RTC_PRIO_HIGH, "acph_rtc_mem_map: VSS_IMEMORY_CMD_MAP_PHYSICAL sent, apr_rc[0x%x], mem_addr[0x%X], mem_size[0x%X]",
           rc, cvp_cmd.table_descriptor.mem_address, cvp_cmd.table_descriptor.mem_size );
           
  if( rc == RTC_INTF_SUCCESS )
  {
    rc = csd_event_wait_timed( rtc_main_data.trans_obj.sync_event_handle, (uint32)rtc_main_data.sync_send_timeout);
  }

  if( rc != CSD_OSAL_EOK && nAPRstatus != APR_EOK )
  {
    RTC_MSG( RTC_PRIO_ERROR, "csd_event_wait_timed failed, rc[0x%x]", rc );
    rc = ACPH_FAILURE; 
  }
  
  return rc;
}
/*===========================================================================
  FUNCTION acph_rtc_mem_unmap

  DESCRIPTION Unmap 8K memory to DSP

  PARAMS:    

  RETURN VALUE ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS
  ===========================================================================*/
static int32_t acph_rtc_mem_unmap()
{
  int32_t rc = RTC_INTF_SUCCESS;
  vss_imemory_cmd_unmap_t cvp_cmd;

  cvp_cmd.mem_handle = rtc_mem.descriptor_mapped_handle;
  
  rc = __aprv2_cmd_alloc_send( rtc_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                               rtc_apr_src_addr, 0,
                               rtc_dest_addr_mvm, APR_NULL_V,
                               0, VSS_IMEMORY_CMD_UNMAP,
                               &cvp_cmd, sizeof(vss_imemory_cmd_unmap_t) );

  RTC_MSG( RTC_PRIO_HIGH, "acph_rtc_mem_unmap: VSS_IMEMORY_CMD_UNMAP sent, apr_rc[0x%x], mem_handle[0x%X]",
           rc, cvp_cmd.mem_handle );
           
  if( rc == RTC_INTF_SUCCESS )
  {
    rc = csd_event_wait_timed( rtc_main_data.trans_obj.sync_event_handle, (uint32)rtc_main_data.sync_send_timeout );
  }

  if( rc != CSD_OSAL_EOK && nAPRstatus != APR_EOK )
  {
    RTC_MSG( RTC_PRIO_ERROR, "csd_event_wait_timed failed, rc[0x%x]", rc );
    rc = ACPH_FAILURE; 
  }
  
  return rc;
}
/*===========================================================================
  FUNCTION acph_oob_mem_alloc

  DESCRIPTION Alloc memory for acph buffer and descriptor table

  PARAMS:    

  RETURN VALUE ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS
  ===========================================================================*/
static int32_t acph_oob_mem_alloc()
{
  uint32_t nContigLen  = 0;
  int32_t rc = E_SUCCESS;
  uint32_t tz_resp = E_FAILURE;
  #define TZ_MEMORY_PROTECTION_CMD_ID 0x00000005


  
  csd_mem_alloc( RTC_OOB_BUFFER_SIZE, &rtc_mem.data_memory_handle, &rtc_mem.pva_data );        
  csd_mem_alloc( RTC_DESCRIPTOR_TABLE_SIZE, &rtc_mem.descriptor_memory_handle, &rtc_mem.pva_descriptor );
  if( !rtc_mem.pva_data || !rtc_mem.pva_descriptor )
  {
    RTC_MSG( RTC_PRIO_ERROR, "Unable to allocate memory for RTC buffer/descriptor table" );
    csd_mem_free( rtc_mem.data_memory_handle );
    csd_mem_free( rtc_mem.descriptor_memory_handle );
    
    return RTC_INTF_ENORESOURCE;
  }
  
  memset( rtc_mem.pva_data, 0, RTC_OOB_BUFFER_SIZE );
  memset( rtc_mem.pva_descriptor, 0, RTC_DESCRIPTOR_TABLE_SIZE );
  
  csd_get_mem_info( rtc_mem.data_memory_handle, &rtc_mem.phy_addr_data, &nContigLen );
  csd_get_mem_info( rtc_mem.descriptor_memory_handle, &rtc_mem.phy_addr_descriptor, &nContigLen );  
  
  if (!scm_is_armv8()) 
  {
    typedef struct {
    uint32_t mem_start;
    uint32_t mem_size;
    }csd_tz_cmd_buf_t;
    csd_tz_cmd_buf_t csd_tz_cmd;
  csd_tz_cmd.mem_start = ( uint32_t )rtc_mem.phy_addr_data;
  csd_tz_cmd.mem_size = RTC_OOB_BUFFER_SIZE;
  
  rc = scm_call( TZ_SVC_MEMORY_PROTECTION , TZ_MEMORY_PROTECTION_CMD_ID, ( void* )&csd_tz_cmd,
         sizeof( csd_tz_cmd ), ( void* )&tz_resp, sizeof( tz_resp ) );
  }
  else
  {
    scm_desc_t scm_desc_csd_tz_buf;
    memset(&scm_desc_csd_tz_buf,0, sizeof(scm_desc_t));
    scm_desc_csd_tz_buf.arginfo = TZ_SVC_MEMORY_PROTECTION;
    scm_desc_csd_tz_buf.args[0] = ( uint32_t )rtc_mem.phy_addr_data;
    scm_desc_csd_tz_buf.args[1] = RTC_OOB_BUFFER_SIZE;
    rc = scm_call2( TZ_MEMORY_PROTECTION_CMD_ID, &scm_desc_csd_tz_buf);
  }
  if ( E_SUCCESS != rc )
  {
	RTC_MSG( RTC_PRIO_ERROR, "acph_oob_mem_alloc: register memory with tz rc = %d, tz_resp = %d ", rc, tz_resp );
  }

  return RTC_INTF_SUCCESS;
}
 /*===========================================================================
  FUNCTION       acph_rtc_dsp_ioctl
  
  DESCRIPTION    acph rtc dsp function call

  PARAMS:        nCommandId - command Id;
                 req_buf_ptr - pointer to request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE/ACPH_ERR_INVALID_COMMAND

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_rtc_dsp_ioctl(
  uint16_t nCommandId,
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_total_len,
  uint32_t *resp_buf_length )
{
  int32_t result = ACPH_SUCCESS;
  RTC_MSG( RTC_PRIO_LOW, "Enter with Command ID[0x%x], resp_buf[0x%p], size[%d]", nCommandId, resp_buf_ptr, resp_buf_total_len );
  
  if( !resp_buf_ptr || resp_buf_total_len == 0 )
  {
    RTC_MSG( RTC_PRIO_ERROR, "Null input arg, resp_buf_ptr[0x%X], resp_buf_total_len[%d]", resp_buf_ptr, resp_buf_total_len );
    return ACPH_FAILURE;
  }
  
  // RTC init check
  if( !rtc_mem.data_memory_handle )
  {
    RTC_MSG( RTC_PRIO_ERROR, "RTC service not initialized yet" );
    return ACPH_FAILURE;
  }
  
  // Get voice debug handle
  if( voice_debug_handle == 0 )
  {
    csd_debug_voice_open_control_t voice_debug_open_info;
    uint32_t result_flag = 0;
        
    voice_debug_open_info.ret_status = &result_flag;
    voice_debug_handle = csd_open( CSD_OPEN_DEBUG_VOICE_CONTROL, &voice_debug_open_info, sizeof(voice_debug_open_info) );
    if( voice_debug_handle == 0 )
    {
      RTC_MSG( RTC_PRIO_ERROR, "Unable to get voice debug handle" );
      return ACPH_FAILURE;
    }
  }  

  // Map 8K buffer to CVD
  if( rtc_mem.descriptor_mapped_handle == 0 )
  {
    result = acph_rtc_mem_map();
    if( result == ACPH_FAILURE )
    {
      RTC_MSG( RTC_PRIO_ERROR, "RTC service is unable to map memory to DSP" );
      return ACPH_FAILURE;
    }
  }
      
  nAPRstatus = APR_EOK;
  pRespBuf = resp_buf_ptr;
  pBufLeng = resp_buf_length;
  pRespBufLen = resp_buf_total_len;
  
  switch( nCommandId )
  {  
    case ACPH_CMD_QUERY_DSP_RTC_VERSION:      
      result = dsp_rtc_get_version( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_total_len, resp_buf_length );
    break;

    case ACPH_CMD_QUERY_VOC_ALL_ACTIVE_STREAMS:
      result = query_voc_all_active_streams( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_total_len, resp_buf_length );
    break;

    case ACPH_CMD_QUERY_VOC_VS_COPP_HANDLES:
      result = query_voc_vs_copp_handles( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_total_len, resp_buf_length );
    break;

    case ACPH_CMD_QUERY_VOC_VC_DEVICES:
      result = query_voc_vc_devices( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_total_len, resp_buf_length );
    break;
    
    case ACPH_CMD_QUERY_VOC_AFE_ACTIVE_PORT_IDS:
      result = query_voc_afe_active_port_ids( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_total_len, resp_buf_length );
    break;    

    case ACPH_CMD_RTC_GET_CAL_DATA:
    case ACPH_CMD_RTC_SET_CAL_DATA:
    case ACPH_GET_AFE_DATA:
    case ACPH_SET_AFE_DATA:                     
      result = rtc_transfer_q6_cal_data( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_total_len, 
                                         resp_buf_length, nCommandId );          
    break;

    case ACPH_CMD_QUERY_AUD_DEVICE_COPP_HANDLES:
    case ACPH_CMD_QUERY_AUD_COPP_STREAM_HANDLES:
    case ACPH_CMD_QUERY_AUD_COPP_STREAM_HANDLES_V2:
    case ACPH_CMD_QUERY_AUD_AFE_ACTIVE_PORT_ID:
      RTC_MSG( RTC_PRIO_LOW, "Received Audio API, not supported", nCommandId );
      result = ACPH_ERR_INVALID_COMMAND;
    break;
  
    case ACPH_GET_AFE_SIDETONE_GAIN:
    case ACPH_SET_AFE_SIDETONE_GAIN:
      RTC_MSG( RTC_PRIO_LOW, "Received AFE sidetone API, not supported", nCommandId );      
      result = ACPH_ERR_MODULE_DISABLED;                
    break;
    
    default:
      RTC_MSG( RTC_PRIO_ERROR, "Unknown cmdId[0x%X]", nCommandId );
      result = ACPH_ERR_INVALID_COMMAND;
    break;
  }

  RTC_MSG( RTC_PRIO_LOW, "Exit with rc[0x%x]", result );
  return result;
}
/*===========================================================================
  FUNCTION rtc_q6_intf_init

  DESCRIPTION

  RETURN VALUE

  SIDE EFFECTS
  ===========================================================================*/
static int32_t rtc_q6_intf_init(void)
{
  int32_t rc = RTC_INTF_SUCCESS;
  uint8_t checkpoint = 0;

  memset( &rtc_mem, 0, sizeof(rtc_mem) );
  memset( &rtc_main_data, 0, sizeof(rtc_main_data) );
  rtc_main_data.sync_send_timeout = RTC_SYNC_TIMEOUT;
  
  rc |= __aprv2_cmd_local_dns_lookup( rtc_dest_dns_cvp, sizeof(rtc_dest_dns_cvp), &rtc_dest_addr_cvp );
  rc |= __aprv2_cmd_local_dns_lookup( rtc_dest_dns_cvs, sizeof(rtc_dest_dns_cvs), &rtc_dest_addr_cvs );
  rc |= __aprv2_cmd_local_dns_lookup( rtc_dest_dns_afe, sizeof(rtc_dest_dns_afe), &rtc_dest_addr_afe );
  rc |= __aprv2_cmd_local_dns_lookup( rtc_dest_dns_mvm, sizeof(rtc_dest_dns_mvm), &rtc_dest_addr_mvm );
  rc |= __aprv2_cmd_register2( &rtc_apr_handle, rtc_dns,
                               sizeof(rtc_dns), 0,
                               (aprv2_dispatch_fn_t)rtc_APR_cbfn, 
                               NULL, &rtc_apr_src_addr );  

  if( rc == RTC_INTF_SUCCESS )
  {
    checkpoint = 1;
    rc = csd_event_create( &(rtc_main_data.trans_obj.sync_event_handle) );
  }

  if( rc == RTC_INTF_SUCCESS )
  {
    checkpoint = 2;
    rc = csd_lock_create( &rtc_main_data.lock );
  }  
  
  if( rc == RTC_INTF_SUCCESS )
  {
    checkpoint = 3;
    rc = acph_oob_mem_alloc();        
  }

  if( rc != RTC_INTF_SUCCESS )
  { /* cleanup */
    RTC_MSG( RTC_PRIO_ERROR, "Init failed with checkpoint[%d]", checkpoint );
    switch( checkpoint )
    {
      case 3:
        csd_lock_destroy( rtc_main_data.lock );
      case 2:
        csd_event_destroy( rtc_main_data.trans_obj.sync_event_handle );
      case 1:
        __aprv2_cmd_deregister( rtc_apr_handle );
      default:
        memset( &rtc_main_data, 0, sizeof(rtc_main_data) );
        memset( &rtc_mem, 0, sizeof(rtc_mem) );
      break;
    }
  }
  
  return rc;
}
 /*===========================================================================
  FUNCTION       acph_rtc_dsp_init
  
  DESCRIPTION    Initialize audio and voice RTC

  PARAMS:        req_buf_ptr - pointer to request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
  
  RETURN VALUE   ACPH_SUCCESS or ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_rtc_dsp_init()
{
  int32_t rc = ACPH_SUCCESS;

  rc = acph_register_command( ACPH_DSP_RTC_REG_SERVICEID,
                              (ACPH_CALLBACK_PTR)acph_rtc_dsp_ioctl );
  if( rc != ACPH_SUCCESS )
  {
    RTC_MSG( RTC_PRIO_ERROR, "register command[acph_rtc_dsp_ioctl] failed" );
    return rc;
  }

  rc = rtc_q6_intf_init();
  if( rc != ACPH_SUCCESS )
  {
    RTC_MSG( RTC_PRIO_ERROR, "[rtc_q6_intf_init] failed, rc[0x%x]", rc );
  }  

  return rc;
}
/*===========================================================================
  FUNCTION rtc_q6_intf_deinit

  DESCRIPTION

  RETURN VALUE

  SIDE EFFECTS
  ===========================================================================*/
static int32_t rtc_q6_intf_deinit(void)
{  
  if( voice_debug_handle == 0 )
  {
    return RTC_INTF_SUCCESS;
  }
  
  if( rtc_mem.descriptor_mapped_handle != 0 )    
    acph_rtc_mem_unmap();
  
  csd_close( voice_debug_handle );
  voice_debug_handle = 0;
  
  csd_lock_destroy(rtc_main_data.lock);
  csd_event_destroy(rtc_main_data.trans_obj.sync_event_handle);
  ( void ) __aprv2_cmd_deregister( rtc_apr_handle );

  csd_mem_free( rtc_mem.data_memory_handle );
  csd_mem_free( rtc_mem.descriptor_memory_handle );
  
  memset( &rtc_main_data, 0, sizeof(rtc_main_data) );
  memset( &rtc_mem, 0, sizeof(rtc_mem) );

  return RTC_INTF_SUCCESS;
}
 /*===========================================================================
  FUNCTION       acph_rtc_dsp_deinit
  
  DESCRIPTION    De-initialize audio and voice RTC

  PARAMS:        req_buf_ptr - pointer to request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
  
  RETURN VALUE   ACPH_SUCCESS or ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_rtc_dsp_deinit()
{
  int32_t rc = ACPH_SUCCESS;
  
  (void) rtc_q6_intf_deinit();

  rc = acph_deregister_command(ACPH_DSP_RTC_REG_SERVICEID);
  if( rc != ACPH_SUCCESS)
  {
    RTC_MSG( RTC_PRIO_ERROR, "Deregister command[acph_rtc_dsp_ioctl] failed, rc[0x%x]", rc );
  }

  return rc;
}

