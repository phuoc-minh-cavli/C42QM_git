/*=============================================================================
  FILE: da_cmd_qmi_misc.c

  OVERVIEW: Debug Agent -
            Interface for sending non-tracer commands through QMI QDSSC.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_qmi_misc.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  10/19/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "stringl/stringl.h"
#include "da_cmd_qmi.h"

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

/*=========================================================================*
 * ETM commands
 *=========================================================================*/
int da_cmd_qmi_get_etm(da_parser_ssid_t ssid, uint8 *state)
{
   qmi_client_error_type qmi_rval;

   qdssc_get_etm_req_msg_v01 rqst;
   qdssc_get_etm_resp_msg_v01 resp;

   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_ETM_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      *state = resp.state;
      return QDSS_CONTROL_SUCCESS;
   }
   else
   {
      return QDSS_CONTROL_ERROR;
   }
}

/*-------------------------------------------------------------------------*/
int da_cmd_qmi_set_etm(da_parser_ssid_t ssid, uint8 state)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_etm_req_msg_v01 rqst;
   qdssc_set_etm_resp_msg_v01 resp;

   rqst.state = (0==state) ? QDSSC_ETM_STATE_DISABLED_V01 :
                             QDSSC_ETM_STATE_ENABLED_V01;

   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_ETM_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      return QDSS_CONTROL_SUCCESS;
   }
   else
   {
      return QDSS_CONTROL_ERROR;
   }
}

/*-------------------------------------------------------------------------*/
int da_cmd_qmi_get_etm_config(da_parser_ssid_t ssid, char *get_str,
                              char *setting_str, size_t setting_size)
{
   qmi_client_error_type qmi_rval;
   qdssc_get_etm_config_req_msg_v01 rqst;
   qdssc_get_etm_config_resp_msg_v01 resp;

   memset (setting_str, '\0', setting_size);
   strlcpy(rqst.query_str, get_str, sizeof(rqst.query_str));

   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_ETM_CONFIG_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      strlcpy(setting_str, resp.settings_str, setting_size);
      return QDSS_CONTROL_SUCCESS;
   }
   else
   {
      return QDSS_CONTROL_ERROR;
   }
}

/*-------------------------------------------------------------------------*/
int da_cmd_qmi_set_etm_config(da_parser_ssid_t ssid, char *set_str)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_etm_config_req_msg_v01 rqst;
   qdssc_set_etm_config_resp_msg_v01 resp;

   strlcpy(rqst.command_str, set_str, sizeof(rqst.command_str));

   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_ETM_CONFIG_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      return QDSS_CONTROL_SUCCESS;
   }
   else
   {
      return QDSS_CONTROL_ERROR;
   }
}

/*=========================================================================*
 * HWE commands
 *=========================================================================*/
int da_cmd_qmi_set_hwe_reg(da_parser_ssid_t ssid,
                           uint32 regaddr, uint32 value)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_hwe_reg_req_msg_v01 rqst;
   qdssc_set_hwe_reg_resp_msg_v01 resp;

   rqst.regaddr = regaddr;
   rqst.value = value;

   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_HWE_REG_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      return QDSS_CONTROL_SUCCESS;
   }
   else
   {
      return QDSS_CONTROL_ERROR;
   }
}

