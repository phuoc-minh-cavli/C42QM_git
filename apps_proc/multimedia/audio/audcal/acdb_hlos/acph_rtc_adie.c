/*! \file acph_rtc_adie.c
\brief ADIE RTC Client of ACPH

Copyright (c) 2014, 2016 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Proprietary and Confidential.
*/

/*===========================================================================
EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.

$Header: 
$DateTime: 
$Author: 
$Change: 
$Revision: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/15/16    rk      Abstracting OSAL layer
02/24       jy      Initial version
============================================================================*/

/*===========================================================================
Include Files
===========================================================================*/
#include "acph_rtc_adie.h"
#include "wcd_intf.h"
#include "acdb_os_includes.h"
#include "csd.h"
#include "csd_dev_debug.h"
#include "csd_voice_debug.h"
#include "csd_voice_debug_ioctl.h"
#include "csd_os_dependencies.h"

/*============================================================================
Function Prototype
============================================================================*/

 /*===========================================================================
  FUNCTION       adie_rtc_get_version
  
  DESCRIPTION    get adie rtc version

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_rtc_get_version(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
	ACPH_CMD_QUERY_ADIE_RTC_VERSION_rsp* pAdieRtcVersion = (ACPH_CMD_QUERY_ADIE_RTC_VERSION_rsp*)resp_buf_ptr;
	pAdieRtcVersion->adie_rtc_major_version = ACPH_SERVICE_MAJOR_VERSION_1;
	pAdieRtcVersion->adie_rtc_minor_version = ACPH_SERVICE_MINOR_VERSION_1;
	*resp_buf_bytes_filled = sizeof(ACPH_CMD_QUERY_ADIE_RTC_VERSION_rsp);
  
  RTC_MSG( RTC_PRIO_HIGH, "Major version[%d], minor version[%d]", ACPH_SERVICE_MAJOR_VERSION_1, ACPH_SERVICE_MINOR_VERSION_1 );
	return ACPH_SUCCESS;
}
 /*===========================================================================
  FUNCTION       adie_set_adie_register
  
  DESCRIPTION    set adie register value

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_set_adie_register(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_SUCCESS;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  int32_t nRespBufLen = 0;
  wcd_ioctl_cmd_register_access sAdieSetRegister;
  ACPH_CMD_SET_ADIE_REGISTER_req *pSetAdieRegReq = (ACPH_CMD_SET_ADIE_REGISTER_req*)input_buf_ptr;

  if( sizeof(ACPH_CMD_SET_ADIE_REGISTER_req) != req_buf_len)
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_set_adie_register: length mismatch, req_buf_len[%d], req_size[%d]", req_buf_len, sizeof(ACPH_CMD_SET_ADIE_REGISTER_req) );
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }
  
  csd_memset( &sAdieSetRegister, 0, sizeof(sAdieSetRegister) );  /* ctrl_bus_intf needs to be zerod out for I2S interface */
  sAdieSetRegister.reg = (uint16_t)pSetAdieRegReq->register_id;
  sAdieSetRegister.mask = (uint8_t)pSetAdieRegReq->register_mask;
  sAdieSetRegister.value = (uint8_t)pSetAdieRegReq->register_value;
  RTC_MSG( RTC_PRIO_MEDIUM, "adie_set_adie_register: Calling WCD_IOCTL_REGISTER_WRITE, reg[%d], mask[%d], value[%d]", sAdieSetRegister.reg, sAdieSetRegister.mask, sAdieSetRegister.value );
  
  wcd_rc = wcd_ioctl( NULL, WCD_IOCTL_REGISTER_WRITE, &sAdieSetRegister);
  if( wcd_rc == E_WCD_SUCCESS )
  {
    *resp_buf_bytes_filled = nRespBufLen;
  }
  else
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_set_adie_register: WCD_IOCTL_REGISTER_WRITE failed, wcd_rc[%d]", wcd_rc );
    rc = ACPH_ERR_ADIE_SET_CMD_FAILURE;
  }
      
  return rc;
  
}
 /*===========================================================================
  FUNCTION       adie_get_adie_register
  
  DESCRIPTION    get adie register value

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_get_adie_register(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_SUCCESS;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  uint8_t *pInputBuf = input_buf_ptr;
  wcd_ioctl_cmd_register_access sAdieGetRegister;
  ACPH_CMD_GET_ADIE_REGISTER_req *pGetAdieRegReq = (ACPH_CMD_GET_ADIE_REGISTER_req*)pInputBuf;
  ACPH_CMD_GET_ADIE_REGISTER_rsp *pGetAdieRegRsp = (ACPH_CMD_GET_ADIE_REGISTER_rsp*)resp_buf_ptr;

  if( sizeof(ACPH_CMD_GET_ADIE_REGISTER_req) != req_buf_len )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_adie_register: length mismatch, req_buf_len[%d], req_size[%d]", req_buf_len, sizeof(ACPH_CMD_GET_ADIE_REGISTER_req) );
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }

  csd_memset( &sAdieGetRegister, 0, sizeof(sAdieGetRegister) );  /* ctrl_bus_intf needs to be zerod out for I2S interface */
  sAdieGetRegister.reg = (uint16_t)pGetAdieRegReq->register_id;
  sAdieGetRegister.mask = (uint8_t)pGetAdieRegReq->register_mask;
  RTC_MSG( RTC_PRIO_MEDIUM, "adie_get_adie_register: Calling WCD_IOCTL_REGISTER_READ, reg[%d], mask[%d]", sAdieGetRegister.reg, sAdieGetRegister.mask );
  
  wcd_rc = wcd_ioctl( NULL, WCD_IOCTL_REGISTER_READ, &sAdieGetRegister );
  if( wcd_rc == E_WCD_SUCCESS )
  {
    pGetAdieRegRsp->value = (uint32_t)sAdieGetRegister.value;
    RTC_MSG( RTC_PRIO_MEDIUM, "adie_get_adie_register: Got value[0x%x]", sAdieGetRegister.value );
    *resp_buf_bytes_filled = sizeof(ACPH_CMD_GET_ADIE_REGISTER_rsp);    
  }
  else
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_adie_register: WCD_IOCTL_REGISTER_READ failed, wcd_rc[%d]", wcd_rc );
    rc = ACPH_ERR_ADIE_GET_CMD_FAILURE;
  }
  
  return rc;
}
 /*===========================================================================
  FUNCTION       adie_set_multiple_adie_registers
  
  DESCRIPTION    set multiple adie register values

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_set_multiple_adie_registers(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{  
  int32_t rc = ACPH_SUCCESS;
  wcd_result wcd_rc = E_WCD_SUCCESS;  
  char_t *pInputBuf = (char_t *)input_buf_ptr;
  ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS_req* pSetMultAdieRegReq = (ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS_req*)pInputBuf;
  uint32_t nTotalRegisters = pSetMultAdieRegReq->num_of_registers;
  uint32_t i = 0;
  uint32_t req_buf_size = 0;

  if( nTotalRegisters == 0 )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_set_multiple_adie_registers: [0] registers to set, exit" );
    return ACPH_ERR_UNKNOWN_REASON;
  }
  
  req_buf_size = nTotalRegisters*sizeof(ACPH_SET_ADIE) + sizeof(ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS_req);
  if( req_buf_len != req_buf_size )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_set_multiple_adie_registers: length mismatch, req_buf_len[%d], req_buf_size[%d]", req_buf_len, req_buf_size );
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }
          
  pInputBuf += sizeof(ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS_req);
  for( i=0; i<nTotalRegisters; i++ )
  {
    wcd_ioctl_cmd_register_access sAdieSetRegister;
    ACPH_SET_ADIE* pSetAdieReg = (ACPH_SET_ADIE*)pInputBuf;    
    
    csd_memset( &sAdieSetRegister, 0, sizeof(sAdieSetRegister) );  /* ctrl_bus_intf needs to be zerod out for I2S interface */
    sAdieSetRegister.reg = (uint16_t)pSetAdieReg->register_id;
    sAdieSetRegister.mask = (uint8_t)pSetAdieReg->register_mask;
    sAdieSetRegister.value = (uint8_t)pSetAdieReg->register_value;
    RTC_MSG( RTC_PRIO_MEDIUM, "adie_set_multiple_adie_registers: Calling WCD_IOCTL_REGISTER_WRITE, reg[%d], mask[%d], value[%d]", sAdieSetRegister.reg, sAdieSetRegister.mask, sAdieSetRegister.value );

    wcd_rc = wcd_ioctl( NULL, WCD_IOCTL_REGISTER_WRITE, &sAdieSetRegister );
    if( wcd_rc != E_WCD_SUCCESS )
    {
      RTC_MSG( RTC_PRIO_ERROR, "adie_set_multiple_adie_registers: WCD_IOCTL_REGISTER_WRITE failed, wcd_rc[%d]", wcd_rc );
      rc = ACPH_ERR_ADIE_SET_CMD_FAILURE;
      break;
    }
    pInputBuf += sizeof(ACPH_SET_ADIE);
  }

  if( rc == ACPH_SUCCESS )
  { /* Valid output buffer length is returned as 0 */
    *resp_buf_bytes_filled = 0;
  }
   
  return rc;      
}
 /*===========================================================================
  FUNCTION       adie_get_multiple_adie_registers
  
  DESCRIPTION    get multiple adie register values

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_get_multiple_adie_registers(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_SUCCESS;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  uint32_t pOutputBuf = (uint32_t)resp_buf_ptr;
  uint8_t *pInputBuf = input_buf_ptr;
  ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS_req *pGetMultAdieRegReq = (ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS_req *)pInputBuf;
  uint32_t req_buf_size = 0; 
  uint32_t i = 0;
  uint32_t nTotalRegisters = pGetMultAdieRegReq->num_of_registers;;      
	  
  if( nTotalRegisters == 0 )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_multiple_adie_registers: [0] registers to get, exit" );
    return ACPH_ERR_UNKNOWN_REASON;
  }
  
  req_buf_size = nTotalRegisters*sizeof(ACPH_ADIE_req) + sizeof(ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS_req);
  if( req_buf_len != req_buf_size )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_multiple_adie_registers: length mismatch, req_buf_len[%d], req_buf_size[%d]", req_buf_len, req_buf_size );
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }
  
  pInputBuf += sizeof(ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS_req);   
  for( i=0; i<nTotalRegisters; i++ )
  {
    uint32_t adie_register_value = 0;
    wcd_ioctl_cmd_register_access sAdieGetRegister;    
    ACPH_ADIE_req *pAdieReg = (ACPH_ADIE_req*)pInputBuf;
    
    csd_memset( &sAdieGetRegister, 0, sizeof(sAdieGetRegister) );  /* ctrl_bus_intf needs to be zerod out for I2S interface */
    sAdieGetRegister.reg = (uint16_t)pAdieReg->register_id;
    sAdieGetRegister.mask = (uint8_t)pAdieReg->register_mask;
    RTC_MSG( RTC_PRIO_MEDIUM, "adie_get_multiple_adie_registers: Calling WCD_IOCTL_REGISTER_READ, reg[%d], mask[%d]", sAdieGetRegister.reg, sAdieGetRegister.mask );
    
    wcd_rc = wcd_ioctl( NULL, WCD_IOCTL_REGISTER_READ, &sAdieGetRegister );
    if( wcd_rc != E_WCD_SUCCESS )
    {
      RTC_MSG( RTC_PRIO_ERROR, "adie_get_multiple_adie_registers: WCD_IOCTL_REGISTER_READ failed, wcd_rc[%d]", wcd_rc );
      rc = ACPH_ERR_ADIE_GET_CMD_FAILURE;
      break;
    }
        
    adie_register_value = (uint32_t)sAdieGetRegister.value;
    csd_memscpy( (void *)pOutputBuf, ( resp_buf_length - i*sizeof(adie_register_value) ), &adie_register_value, sizeof(uint32_t) );
    
    pInputBuf += sizeof(ACPH_ADIE_req);    
    pOutputBuf += sizeof(uint32_t);
  }

  if( rc == ACPH_SUCCESS )
  {
    *resp_buf_bytes_filled = sizeof(uint32_t) * nTotalRegisters;
  }
                  
  return rc;
}
 /*===========================================================================
  FUNCTION       adie_get_sidetone_iir_data
  
  DESCRIPTION    get adie sidetone iir data

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_get_sidetone_iir_data(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_SUCCESS;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA_req *pGetAdieSidetoneIirDataReq = (ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA_req*)input_buf_ptr;
  ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA_rsp *pGetAdieSidetoneIirDataRsp = (ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA_rsp*)resp_buf_ptr;
  wcd_ioctl_cmd_sidetone_iir_config sSidetoneIirConfig;
  
  if( sizeof(ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA_req) != req_buf_len )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_sidetone_iir_data: length mismatch, req_buf_len[%d], req_size[%d]", req_buf_len, sizeof(ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA_req) );
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }
        
  sSidetoneIirConfig.acdb_configuration.module_id = pGetAdieSidetoneIirDataReq->module_id;
  sSidetoneIirConfig.acdb_configuration.parameter_id = pGetAdieSidetoneIirDataReq->parameter_id;
  sSidetoneIirConfig.acdb_configuration.length = sizeof(wcd_adie_codec_db_sidetone_iir_config);
  RTC_MSG( RTC_PRIO_MEDIUM, "adie_get_sidetone_iir_data: Calling WCD_IOCTL_GET_SIDETONE_IIR_CONFIG, mid[%d], pid[%d], length[%d]", 
           sSidetoneIirConfig.acdb_configuration.module_id, sSidetoneIirConfig.acdb_configuration.parameter_id, sSidetoneIirConfig.acdb_configuration.length );
  
  wcd_rc = wcd_ioctl( (void*)pGetAdieSidetoneIirDataReq->rx_codec_handle, WCD_IOCTL_GET_SIDETONE_IIR_CONFIG, &sSidetoneIirConfig );
  if( wcd_rc == E_WCD_SUCCESS )
  {
    csd_memscpy( pGetAdieSidetoneIirDataRsp, resp_buf_length, ( (uint8_t *)&(sSidetoneIirConfig.acdb_configuration)+sizeof(uint32_t) ), ( sizeof(wcd_adie_codec_db_sidetone_iir_config)+3*sizeof(uint32_t) ) );                                
    *resp_buf_bytes_filled = sizeof(wcd_adie_codec_db_sidetone_iir_config) + 3*sizeof(uint32_t);    
  }
  else if( wcd_rc == E_WCD_SIDETONE_NOT_ENABLED )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_sidetone_iir_data: Sidetone module not enabled" );
    rc = ACPH_ERR_MODULE_DISABLED;
  }
  else
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_get_sidetone_iir_data: WCD_IOCTL_GET_SIDETONE_IIR_CONFIG failed, wcd_rc[%d]", wcd_rc );         
    rc = ACPH_ERR_UNKNOWN_REASON;
  }
  
  return rc;
}
 /*===========================================================================
  FUNCTION       adie_set_sidetone_iir_data
  
  DESCRIPTION    set adie sidetone iir data

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
static int32_t adie_set_sidetone_iir_data(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_SUCCESS;
  wcd_result wcd_rc = E_WCD_SUCCESS;
  ACPH_CMD_SET_ADIE_SIDETONE_IIR_DATA_req *pSetAdieSidetoneIirDataReq = (ACPH_CMD_SET_ADIE_SIDETONE_IIR_DATA_req*)input_buf_ptr;
  wcd_ioctl_cmd_sidetone_iir_config sSidetoneIirConfig;

  if( sizeof(ACPH_CMD_SET_ADIE_SIDETONE_IIR_DATA_req) > req_buf_len )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_set_sidetone_iir_data: length mismatch, req_buf_len[%d], req_size[%d]", req_buf_len, sizeof(ACPH_CMD_SET_ADIE_SIDETONE_IIR_DATA_req) );
    return ACPH_ERR_LENGTH_NOT_MATCH;
  }
  
  csd_memscpy( &(sSidetoneIirConfig.acdb_configuration), sizeof(sSidetoneIirConfig.acdb_configuration), 
                  ( (uint8_t *)pSetAdieSidetoneIirDataReq+sizeof(uint32_t) ), sizeof(wcd_acdb_iir_config) );       
    
  wcd_rc = wcd_ioctl(( void*)pSetAdieSidetoneIirDataReq->rx_codec_handle, WCD_IOCTL_SET_SIDETONE_IIR_CONFIG, &sSidetoneIirConfig );       

  if( wcd_rc != E_WCD_SUCCESS )
  {
    RTC_MSG( RTC_PRIO_ERROR, "adie_set_sidetone_iir_data: WCD_IOCTL_SET_SIDETONE_IIR_CONFIG failed, wcd_rc[%d]", wcd_rc );
    rc = ACPH_ERR_UNKNOWN_REASON;
  }

  return rc;
}
 /*===========================================================================
  FUNCTION       acph_query_get_voc_adie_handle
  
  DESCRIPTION    get adie handle

  PARAMS:        input_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_query_get_voc_adie_handle(
  uint8_t *input_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_length,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_SUCCESS;
  uint32_t csd_rc = CSD_EOK;
  uint32_t dev_debug_handle;
  uint32_t voice_debug_handle;
  csd_debug_voice_open_control_t voice_debug_open_info;
  csd_debug_active_dev_query_t active_dev_rsp_buf;
  csd_debug_active_dev_info_query_v2_t active_dev_info_rsp_buf;
  ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_req *pInputBuffer = (ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_req*)input_buf_ptr;
  ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_rsp *pOutputBuffer = (ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_rsp*)resp_buf_ptr;
  uint32_t rx_device_id, tx_device_id;
  bool_t found_tx = FALSE;
  bool_t found_rx = FALSE;
  uint8_t *temp_buffer = NULL;

  if( !input_buf_ptr )
  {
    RTC_MSG( RTC_PRIO_ERROR, "acph_query_get_voc_adie_handle: input_buf_ptr is NULL" );
    return ACPH_FAILURE;
  }
  
  if( sizeof(ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_req) != req_buf_len )
  {
    RTC_MSG( RTC_PRIO_ERROR, "acph_query_get_voc_adie_handle: length mismatch, req_buf_len[%d], req_size[%d]", req_buf_len, sizeof(ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_req) );
    return ACPH_FAILURE;
  }

  /* 1. Get voice/device debug handles */
  voice_debug_open_info.ret_status = &csd_rc;
  voice_debug_handle = csd_open( CSD_OPEN_DEBUG_VOICE_CONTROL, &voice_debug_open_info, sizeof(voice_debug_open_info) );
  dev_debug_handle = csd_open( CSD_OPEN_DEBUG_DEVICE_CONTROL, NULL, 0 );
  if( !voice_debug_handle || !dev_debug_handle )
  { 
    RTC_MSG( RTC_PRIO_ERROR, "acph_query_get_voc_adie_handle: Not able to get voice debug handle" );
    rc = ACPH_ERR_CSD_OPEN_HANDLE;     
  }
  
  if( rc == ACPH_SUCCESS )
  { /* 2. Get Tx/Rx device ids from VC handle */
    csd_debug_voice_ioctl_get_device_ids_t get_device_ids;
    
    get_device_ids.csd_vc_handle = pInputBuffer->voc_copp_handle;
    get_device_ids.ret_rx_dev_id = &rx_device_id;
    get_device_ids.ret_tx_dev_id = &tx_device_id;
    rc = csd_ioctl( voice_debug_handle, CSD_DEBUG_VOICE_IOCTL_GET_DEVICE_IDS, &get_device_ids, sizeof(get_device_ids) );
  }
  
  if( rc == ACPH_SUCCESS )
  { /* 3. Get number of active devices */
    rc = csd_ioctl( dev_debug_handle, CSD_DEBUG_CMD_ACTIVE_DEV_QUERY, (void *)&active_dev_rsp_buf, sizeof(struct csd_debug_active_dev_query) );
  }
  
  if( rc == ACPH_SUCCESS )
  { /* 4. Get all active device info */    
    csd_malloc( 0x1000, (void**)&temp_buffer );    
    if( temp_buffer )
    {    
      active_dev_info_rsp_buf.num_entries = active_dev_rsp_buf.num_devs;
      active_dev_info_rsp_buf.entries = (struct csd_debug_dev_info_v2*)temp_buffer;
      rc = csd_ioctl(dev_debug_handle, CSD_DEBUG_CMD_ACTIVE_DEV_INFO_QUERY_V2, &active_dev_info_rsp_buf, sizeof(struct csd_debug_active_dev_info_query_v2) );
    }
    else
    {
      RTC_MSG( RTC_PRIO_ERROR, "acph_query_get_voc_adie_handle: Unable to allocate memory of size 0x1000" );
      rc = ACPH_FAILURE;
    }
  }
  
  if( rc == ACPH_SUCCESS )
  { /* 5. Get corresponding adie handle */
    uint16_t i = 0;
   
    for( i=0; i<active_dev_info_rsp_buf.num_entries; i++ )
    {        
      if( rx_device_id == active_dev_info_rsp_buf.entries[i].dev_id )
      {
        pOutputBuffer->rx_codec_handle = active_dev_info_rsp_buf.entries[i].adie_handle;
        found_rx = TRUE;
      }
      if( tx_device_id == active_dev_info_rsp_buf.entries[i].dev_id)
      {
        pOutputBuffer->tx_codec_handle = active_dev_info_rsp_buf.entries[i].adie_handle;
        found_tx = TRUE;
      }
    }      
  }
  
  if( found_tx && found_rx )
  {
    *resp_buf_bytes_filled = sizeof(ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES_rsp);
  }
  else
  {
    rc = ACPH_FAILURE;
  }
  
  csd_free( temp_buffer );
  
  return rc;
}
 /*===========================================================================
  FUNCTION       acph_rtc_adie_ioctl
  
  DESCRIPTION    acph adie rtc function call

  PARAMS:        nCommandId - ACPH cmd ID
                 req_buf_ptr - pointer to request buffer
                 req_buf_len - length of the request buffer
                 resp_buf_ptr - pointer to response buffer
                 resp_buf_length - length of the response buffer
                 resp_buf_bytes_filled - pointer of the length of the data filled in resp_buf_ptr
  
  RETURN VALUE   ACPH_SUCCESS/ACPH_FAILURE/ACPH_ERR_INVALID_COMMAND

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_rtc_adie_ioctl(
  uint16_t nCommandId,
  uint8_t *req_buf_ptr,
  uint32_t req_buf_len,
  uint8_t *resp_buf_ptr,
  uint32_t resp_buf_len,
  uint32_t *resp_buf_bytes_filled )
{
  int32_t rc = ACPH_FAILURE;  

  if( !resp_buf_ptr || !resp_buf_len )
  {
    RTC_MSG( RTC_PRIO_ERROR, "acph_rtc_adie_ioctl: Invalid input arg [resp_buf_ptr] [resp_buf_len]" );
    return ACPH_FAILURE;
  }

  RTC_MSG( RTC_PRIO_MEDIUM, "Enter with Command ID[0x%x]", nCommandId );
  
  switch( nCommandId )
  {
    case ACPH_CMD_QUERY_ADIE_RTC_VERSION:
      rc = adie_rtc_get_version(req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_SET_ADIE_REGISTER:
      rc = adie_set_adie_register( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_GET_ADIE_REGISTER:
      rc = adie_get_adie_register( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_SET_MULTIPLE_ADIE_REGISTERS:
      rc = adie_set_multiple_adie_registers( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_GET_MULTIPLE_ADIE_REGISTERS:
      rc = adie_get_multiple_adie_registers( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_GET_ADIE_SIDETONE_IIR_DATA:
      rc = adie_get_sidetone_iir_data( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_SET_ADIE_SIDETONE_IIR_DATA:
      rc = adie_set_sidetone_iir_data( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
    
    case ACPH_CMD_QUERY_GET_VOC_ADIE_HANDLES:    
      rc = acph_query_get_voc_adie_handle( req_buf_ptr, req_buf_len, resp_buf_ptr, resp_buf_len, resp_buf_bytes_filled );
    break;
      
    default:
      RTC_MSG( RTC_PRIO_ERROR, "acph_rtc_adie_ioctl: Unknown cmd[%d]", nCommandId );
      rc = ACPH_ERR_INVALID_COMMAND;
    break;
  }
   
   return rc;
}


 /*===========================================================================
  FUNCTION       acph_rtc_adie_init
  
  DESCRIPTION    Initialize adie RTC

  PARAMS:        
  
  RETURN VALUE   ACPH_SUCCESS or ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_rtc_adie_init()
{
  int32_t rc = acph_register_command( ACPH_ADIE_RTC_REG_SERVICEID,
                                          (ACPH_CALLBACK_PTR)acph_rtc_adie_ioctl );   
  if( rc != ACPH_SUCCESS )
  {
    RTC_MSG( RTC_PRIO_ERROR, "acph_register_command failed for service ACPH_ADIE_RTC_REG_SERVICEID" );  
  }

  return rc;
}
 /*===========================================================================
  FUNCTION       acph_rtc_adie_deinit
  
  DESCRIPTION    De-initialize adie RTC

  PARAMS:        
  
  RETURN VALUE   ACPH_SUCCESS or ACPH_FAILURE

  SIDE EFFECTS   None
  ===========================================================================*/
int32_t acph_rtc_adie_deinit()
{
  int32_t rc = acph_deregister_command( ACPH_ADIE_RTC_REG_SERVICEID );
  if( rc != ACPH_SUCCESS )
  {
    RTC_MSG( RTC_PRIO_ERROR, "acph_deregister_command failed for service ACPH_ADIE_RTC_REG_SERVICEID" );
  }
  
  return rc;
}
