/*===========================================================================
                         QTI_BRIDGE_DS_MUX_C

DESCRIPTION
  This source file contains API's which are mainly used for AT-FWD and
  qti_bridge callback handling API's

  Copyright (c)2016-2019, 2021 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

#include "qti_bridge.h"
#include "at_fwd_ext_api.h"
#include "ds_mux_platform.h"
#include "ds_mux_api.h"
#include "ds_log.h"


char* at_resp_buf = NULL;

extern uint8 *input_at_params;

extern uint8  at_mask;

extern at_fwd_cmux_cmd_params_struct *cmux_params;

extern qti_bridge_info_t qti_bridge_info_table[QTI_BRIDGES_MAX];

extern qti_bridge_qurt_t  qti_bridge_qurt;

extern qti_bridge_ds_mux_info_t *qti_bridge_ds_mux_info;

extern boolean ds_mux_bridge_active;

extern boolean ds_mux_first_init_done;


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_EVENT_CB()

DESCRIPTION
  This callback function will handle the events set by the DS MUX module

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/

void qti_bridge_ds_mux_event_cb 
(
  ds_mux_ext_client_event_enum_type event_name, 	/* Event name  */
  void *                            event_payload,	/* Info assosiated to event: If Any else NULL  */
  void *                            cb_user_data	/* User data specified during registration */
)
{
  ds_iot_log_high1 ("qti_bridge_ds_mux_event_cb event name %d ",
                    event_name);

  switch (event_name)
  {
    case DS_MUX_EVENT_INIT_COMPLETE:
      qti_bridge_ds_mux_enter_mux_mode();
      break;
    case DS_MUX_EVENT_CLOSE_COMPLETE:
      qti_bridge_ds_mux_leave_mux_mode();
      break;
    default:
      ds_iot_log_high1 ("qti_bridge_ds_mux_event_cb unhandled evt %d ",
                         event_name);
    break;
  }
  return;
}

/*==============================================================================
FUNCTION QTI_BRIDGE_VALIDATE_ATFWD_CMUX_PARAMS

DESCRIPTION
  This function validates all DS MUX AT command parameters 
  received from AT-FWD and stores them  and sends validation status to
  AT-FWD 

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure
  
DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_validate_atfwd_cmux_params(void)
{
  boolean ret;
  int i = 0;

  if (at_mask == QTI_BRIDGE_AT_MASK_NA_V01)
  {
    ds_iot_log_err ("Received AT+CMUX with Name only which is not supported");
    qapi_atfwd_send_resp("+CMUX",at_resp_buf,AT_RESULT_ERROR_V01);
  }
  else if (at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 | QTI_BRIDGE_AT_MASK_EQ_V01|
                  QTI_BRIDGE_AT_MASK_AR_V01))
  {
    for (i =0; i < QTI_BRIDGES_MAX ; i++)
    {
      if ((qti_bridge_info_table[i].smd_port_id == SIO_PORT_SMD_DATA1) ||
          (qti_bridge_info_table[i].smd_port_id == SIO_PORT_SMD_DATA2) ||
          (qti_bridge_info_table[i].smd_port_id == SIO_PORT_SMD_DATA3))
      {
        if (qti_bridge_info_table[i].smd_stream_id != SIO_NO_STREAM_ID)
        {
          ds_iot_log_err1 ("qti_bridge_validate_atfwd_cmux_params: SMD port "
                             " intended for CMUX already in use %d",
                             qti_bridge_info_table[i].smd_port_id );
          qapi_atfwd_send_resp("+CMUX",at_resp_buf,AT_RESULT_ERROR_V01);
          goto end;
        }
      }
    }
    ds_iot_log_high ("Received AT+CMUX =<parameters>");
    cmux_params = (at_fwd_cmux_cmd_params_struct*)malloc(sizeof(at_fwd_cmux_cmd_params_struct));

    if (cmux_params == NULL)
    {
      ds_iot_log_err ("qti_bridge_validate_atfwd_cmux_params: Not able to"
                         " allocate memory for cmux_params");
      return;
    }
    memset (cmux_params, 0, sizeof(at_fwd_cmux_cmd_params_struct));

    qti_bridge_tokenize_cmux_input();
	
    ret = qti_bridge_validate_cmux_params();
    if (ret)
    {
      qapi_atfwd_send_resp("+CMUX",at_resp_buf,AT_RESULT_OK_V01);
      ds_mux_passive_init((dlci_cmux_param_type *)cmux_params);
    }
    else
    {
      qapi_atfwd_send_resp("+CMUX",at_resp_buf,AT_RESULT_ERROR_V01);
    }

    free (cmux_params);
    cmux_params = NULL;

  }
  else if (at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 | QTI_BRIDGE_AT_MASK_QU_V01))
  {
    ds_iot_log_high("Received AT+CMUX?");
    qti_bridge_get_current_cmux_params();
  }
  else if (at_mask == (QTI_BRIDGE_AT_MASK_NA_V01 | QTI_BRIDGE_AT_MASK_EQ_V01 |
                       QTI_BRIDGE_AT_MASK_QU_V01))
  {
    ds_iot_log_high("Received AT+CMUX=?");
    qti_bridge_get_supported_cmux_params();
  }

end:
  if (input_at_params != NULL)
  {
    free(input_at_params);
    input_at_params = NULL;
  }
}


/*==============================================================================
FUNCTION qti_bridge_tokenize_cmux_input()

DESCRIPTION
  This function tokenizes the AT+CMUX parameter input to individual cmux
  parameters
  
PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure
  
DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/

void qti_bridge_tokenize_cmux_input()
{
  uint8 idx = 0;
  uint8 buf_idx = 0;
  char buffer[8];
  int value = 0;

  ds_iot_log_high("qti_bridge_tokenize_cmux_input()");

  memset (buffer, 0, sizeof(buffer));

  // Extract the transparency (operating mode)
  cmux_params->operating_mode = (ds_mux_at_fwd_mode_enum_type)(input_at_params[idx]-'0');

  // Increament the index by two since each valid value is followed by NULL
  idx+=2;

  //Extract the subset
  if (input_at_params[idx] != NULL)
  {
    cmux_params->subset = (ds_mux_at_fwd_subset_enum_type)(input_at_params[idx]-'0');
    idx+=2;
  }
  else
  {
    idx++;
  }

  //Extract the Port speed
  if (input_at_params[idx] != NULL)
  {
    cmux_params->port_speed = (ds_mux_at_fwd_port_speed_enum_type)(input_at_params[idx]-'0');
    idx+=2;
  }
  else
  {
    idx++;
  }

  //Extract N1
  if (input_at_params[idx] != NULL)
  {
    while (input_at_params[idx] != NULL)
    {
      buffer[buf_idx] = input_at_params[idx];
      buf_idx++;
      idx++;
    }
    buffer[buf_idx] = '\0';
    value = atoi(buffer);
    cmux_params->frame_size_N1 = (uint16)value;
    value = 0;
    buf_idx = 0;
    memset (buffer, 0, sizeof(buffer));
    idx++;
  }
  else
  {
    idx++;
  }

  //Extract T1
  if (input_at_params[idx] != NULL)
  {
    while (input_at_params[idx] != NULL)
    {
      buffer[buf_idx] = input_at_params[idx];
      buf_idx++;
      idx++;
    }
    buffer[buf_idx] = '\0';
    value = atoi(buffer);
    cmux_params->response_timer_T1 = (uint16)value;
    value = 0;
    buf_idx = 0;
    memset (buffer, 0, sizeof(buffer));
    idx+=1;
  }
  else
  {
    idx++;
  }

  //Extract N2
  if (input_at_params[idx] != NULL)
  {
    while (input_at_params[idx] != NULL)
    {
      buffer[buf_idx] = input_at_params[idx];
      buf_idx++;
      idx++;
    }
    buffer[buf_idx] = '\0';
    value = atoi(buffer);
    cmux_params->re_transmissions_N2 = (uint8)value;
    value = 0;
    buf_idx = 0;
    memset (buffer, 0, sizeof(buffer));
    idx+=1;
  }
  else
  {
    idx++;
  }

  //Extract T2
  if (input_at_params[idx] != NULL)
  {
    while (input_at_params[idx] != NULL)
    {
      buffer[buf_idx] = input_at_params[idx];
      buf_idx++;
      idx++;
    }
    buffer[buf_idx] = '\0';
    value = atoi(buffer);
    cmux_params->response_timer_T2 = (uint16)value;
    value = 0;
    buf_idx = 0;
    memset (buffer, 0, sizeof(buffer));
    idx+=1;
  }
  else
  {
    idx++;
  }

  //Extract T3
  if (input_at_params[idx] != NULL)
  {
    while (input_at_params[idx] != NULL)
    {
      buffer[buf_idx] = input_at_params[idx];
      buf_idx++;
      idx++;
    }
    buffer[buf_idx] = '\0';
    value = atoi(buffer);
    cmux_params->wake_up_timer_T3 = (uint32)value;
    value = 0;
    buf_idx = 0;
    memset (buffer, 0, sizeof(buffer));
    idx+=1;
  }
  else
  {
    idx++;
  }

  //Extract K
  if (input_at_params[idx] != NULL)
  {
    while (input_at_params[idx] != NULL)
    {
      buffer[buf_idx] = input_at_params[idx];
      buf_idx++;
      idx++;
    }
    buffer[buf_idx] = '\0';
    value = atoi(buffer);
    cmux_params->window_size_k = (uint8)value;
    value = 0;
    buf_idx = 0;
    memset (buffer, 0, sizeof(buffer));
    idx+=1;
  }
  else
  {
    idx++;
  }
}


/*==============================================================================
FUNCTION qti_bridge_get_current_cmux_params()

DESCRIPTION
  This function gets the current CMUX parameters 
  
PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure
  
DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_get_current_cmux_params()
{
  dlci_cmux_param_type *curr_cmux_param_ptr;
  //dlci_cmux_param_type  cmux_param;
  //uint8 operating_mode,subset,port_speed;

  ds_iot_log_high("qti_bridge_get_current_cmux_params()");

  at_resp_buf = (char*)malloc(QTI_BRIDGE_MAX_AT_TOKENS);

  if (at_resp_buf == NULL)
  {
    ds_iot_log_err("qti_bridge_get_current_cmux_params"
                       " Not able to allocate memory for at_resp_buf");
    return;
  }

  memset (at_resp_buf,0,QTI_BRIDGE_MAX_AT_TOKENS);

  // If MUX mode is active fetch from the CMUX params global variable
  if (ds_mux_bridge_active)
  {
    if ((curr_cmux_param_ptr = ds_mux_get_cmux_params()) != NULL)
    {
      snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,
           "+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
           curr_cmux_param_ptr->operating_mode,curr_cmux_param_ptr->subset,
           curr_cmux_param_ptr->port_speed, curr_cmux_param_ptr->frame_size_N1,
           curr_cmux_param_ptr->response_timer_T1,
           curr_cmux_param_ptr->re_transmissions_N2,
           curr_cmux_param_ptr->response_timer_T2,
           curr_cmux_param_ptr->wake_up_timer_T3,
           curr_cmux_param_ptr->window_size_k);
    }
    else
    {
      snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,
           "+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
           DS_MUX_MODE_BASIC,DS_MUX_SUBSET_MIN,
           DS_MUX_PHY_PORT_SPEED_1, DS_MUX_CMUX_DEFAULT_FRAME_N1,
           DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1,DS_MUX_CMUX_DEFAULT_MAX_TX_N2,
           DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2,DS_MUX_CMUX_DEFAULT_TIMER_T3,
           DS_MUX_WINDOW_SIZE_2);
    }
  }
  else
  {
    ds_iot_log_high("MUX Mode not ON returning default params");

    snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,
           "+CMUX: %d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
           DS_MUX_MODE_BASIC,DS_MUX_SUBSET_MIN,
           DS_MUX_PHY_PORT_SPEED_1, DS_MUX_CMUX_DEFAULT_FRAME_N1,
           DS_MUX_CMUX_DEFAULT_CMD_TIMER_T1,DS_MUX_CMUX_DEFAULT_MAX_TX_N2,
           DS_MUX_CMUX_DEFAULT_DLCI0_TIMER_T2,DS_MUX_CMUX_DEFAULT_TIMER_T3,
           DS_MUX_WINDOW_SIZE_2);
  }

  qapi_atfwd_send_resp("+CMUX",at_resp_buf,AT_RESULT_OK_V01);

  free(at_resp_buf);
  at_resp_buf = NULL;
}


/*==============================================================================
FUNCTION qti_bridge_get_supported_cmux_params()

DESCRIPTION
  This function gets the supported CMUX parameters values
  
PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure
  
DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_get_supported_cmux_params()
{
  ds_iot_log_high("qti_bridge_get_supported_cmux_params()");

  at_resp_buf = (char*)malloc(QTI_BRIDGE_MAX_AT_TOKENS);

  if (at_resp_buf == NULL)
  {
    ds_iot_log_err("qti_bridge_get_supported_cmux_params"
                      " Not able to allocate memory for at_resp_buf");
    return;
  }

  memset (at_resp_buf,0,QTI_BRIDGE_MAX_AT_TOKENS);

  snprintf(at_resp_buf,QTI_BRIDGE_MAX_AT_TOKENS,
           "+CMUX:(0),(0-2),(1-6),(1-32768),(1-255),(0-100),(2-255),(1-255),(1-7)\r\n");

  qapi_atfwd_send_resp("+CMUX",at_resp_buf,AT_RESULT_OK_V01);

  free(at_resp_buf);
  at_resp_buf = NULL;
}

/*==============================================================================
FUNCTION qti_bridge_validate_cmux_params()

DESCRIPTION
  This function validates all DS MUX AT command parameters for their values
  received from AT-FWD and stores them if all are valid

PARAMETERS
  NONE

RETURN VALUE
  TRUE on sucesss
  False on failure
  
DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
boolean qti_bridge_validate_cmux_params()
{
  boolean ret = TRUE;

  ds_iot_log_high("qti_bridge_validate_cmux_params()");

    if( ds_mux_set_oprt_mode((ds_mux_mode_enum_type )cmux_params->operating_mode,
                             (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
      ret = FALSE;
      DS_MUX_LOG_MSG_ERROR("cmux_params invalid operating mode:%d", 
                            cmux_params->operating_mode,0,0);
    }

  if(cmux_params->subset)   
  {
    if( ds_mux_set_subset((ds_mux_subset_enum_type)cmux_params->subset,
                          (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
      ret = FALSE;
      DS_MUX_LOG_MSG_ERROR("cmux_params invalid subset:%d", cmux_params->subset,0,0);
    }
  }
  if(cmux_params->port_speed)
  {
    if( ds_mux_set_port_speed((ds_mux_port_speed_enum_type)cmux_params->port_speed,
                              (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
      ret = FALSE;
      DS_MUX_LOG_MSG_ERROR("cmux_params invalid port speed:%d", cmux_params->port_speed,0,0);
    }
  }

  if(cmux_params->frame_size_N1)
  {
    if( ds_mux_set_N1(cmux_params->frame_size_N1, (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       DS_MUX_LOG_MSG_ERROR("cmux_params invalid frame_size_N1:%d", cmux_params->frame_size_N1,0,0);
    }
  }

  if(cmux_params->response_timer_T1)
  {
    if( ds_mux_set_T1(cmux_params->response_timer_T1, (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       DS_MUX_LOG_MSG_ERROR("cmux_params invalid response_timer_T1:%d", cmux_params->response_timer_T1,0,0);
    }
  }

  if (cmux_params->re_transmissions_N2)
  {
    if( ds_mux_set_N2(cmux_params->re_transmissions_N2, (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       DS_MUX_LOG_MSG_ERROR("cmux_params re_transmissions_N2:%d", cmux_params->re_transmissions_N2,0,0);
    }
  }

  if(cmux_params->response_timer_T2)
  {
    if( ds_mux_set_T2(cmux_params->response_timer_T2, (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       DS_MUX_LOG_MSG_ERROR("cmux_params response_timer_T2:%d", cmux_params->response_timer_T2,0,0);
    }
  }

  if(cmux_params->wake_up_timer_T3)
  {
    if( ds_mux_set_T3(cmux_params->wake_up_timer_T3, (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
    {
       ret = FALSE;
       DS_MUX_LOG_MSG_ERROR("cmux_params wake_up_timer_T3:%d", cmux_params->wake_up_timer_T3,0,0);
    }
  }

  if(cmux_params->window_size_k)
  {
     if( ds_mux_set_K(cmux_params->window_size_k, (dlci_cmux_param_type *)cmux_params ) != DS_MUX_SUCCESS)
     {
       ret = FALSE;
       DS_MUX_LOG_MSG_ERROR("cmux_params window_size_k:%d", cmux_params->window_size_k,0,0);
     }
  }

return ret;
}

/*==============================================================================
FUNCTION QTI_BRIDGE_CHANGE_DTR()

DESCRIPTION
  Change the DTR status on the appropriate channel

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_change_dtr
(
  boolean              dtr_status,
  sio_stream_id_type   stream_id
)
{
  sio_ioctl_cmd_type               cmd;
  sio_ioctl_param_type             param;

  ds_iot_log_high2 ("qti_bridge_change_dtr status %d, stream_id %d",
                     dtr_status, stream_id);

  if(dtr_status)
  {
    cmd = SIO_IOCTL_DSR_ASSERT;
    param.data_set_ready_assert = TRUE;
  }
  else
  {
    cmd = SIO_IOCTL_DSR_DEASSERT;
    param.data_set_ready_deassert = TRUE;
  }

  sio_ioctl(stream_id, cmd, &param );
}

/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_ENTER_MUX_MODE()

DESCRIPTION
  1. Set DTR low on the DS SMD channel
  2. Open the SMD_DATA1, SMD_DATA2 and SMD_DATA3 channels
  3. Open the DATA_MUX1, DATA_MUX2 and DATA_MUX3 logical channels
  4. Set Global variable to indicate DS MUX mode of operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_enter_mux_mode(void)
{
  int idx =0;

  ds_iot_log_high ("qti_bridge_ds_mux_enter_mux_mode");

  // 1. Set DTR low on the DS SMD channel
  qti_bridge_change_dtr(FALSE, qti_bridge_info_table[0].smd_stream_id);

  if (ds_mux_first_init_done == FALSE)
  {
    // 2. Open the SMD_DATA1, SMD_DATA2 and SMD_DATA3 channels
    qti_bridge_ds_mux_smd_ch_open();

    // 3. Open the DATA_MUX1, DATA_MUX2 and DATA_MUX3 logical channels
    qti_bridge_ds_mux_logical_ch_open();
    ds_mux_first_init_done = TRUE;
  }
  else if (ds_mux_first_init_done == TRUE)
  {
    for (idx = 0; idx<QTI_BRIDGES_DS_MUX_CHANNEL_MAX; idx++)
    {
      qti_bridge_change_dtr(TRUE, qti_bridge_ds_mux_info->smd_stream_id[idx]);
    }
    qti_bridge_ds_mux_logical_ch_open();
  }
  // 4. Set Global variable to indicate DS MUX mode of operation
  ds_mux_bridge_active = TRUE;
}


/*==============================================================================
FUNCTION QTI_BRIDGE_DS_MUX_ENTER_MUX_MODE()

DESCRIPTION
  1. Set DTR Low on the SMD_DATA1, SMD_DATA2 and SMD_DATA3 channels
  2. Set DTR High on the DS SMD channel
  3. ReSet Global variable to switch to legacy mode of operation

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void qti_bridge_ds_mux_leave_mux_mode(void)
{
  int idx;
  //1. Set DTR Low on the SMD_DATA1, SMD_DATA2 and SMD_DATA3 channels
  for (idx = 0; idx<QTI_BRIDGES_DS_MUX_CHANNEL_MAX; idx++)
  {
    qti_bridge_change_dtr(FALSE, qti_bridge_ds_mux_info->smd_stream_id[idx]);
  }


  // Close the logical ports
  qti_bridge_logical_channel_close();

  // 2. Set DTR high on the DS SMD channel
  qti_bridge_change_dtr(TRUE, qti_bridge_info_table[0].smd_stream_id);

  //Temp Step until sio close cb is implemented to clean up the WM queues
  for (idx = 0; idx<QTI_BRIDGES_DS_MUX_CHANNEL_MAX; idx++)
  {
    qti_bridge_ds_mux_sio_close(idx,QTI_BRIDGE_DS_MUX_LOGICAL_SIO_CLOSE);
  }
  
  //3. ReSet Global variable to switch to legacy mode of operation
  ds_mux_bridge_active = FALSE;
}

