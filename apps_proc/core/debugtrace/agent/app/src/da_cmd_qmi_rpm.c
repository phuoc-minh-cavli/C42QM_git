/*=============================================================================
  FILE: da_cmd_qmi_rpm.c

  OVERVIEW: Debug Agent -
            Interface for sending commands for the RPM through QMI QDSSC.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_qmi_rpm.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  10/19/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include <string.h>
#include "da_cmd_qmi.h"

/*---------------------------------------------------------------------------
 * Static Variable Definitions
 * ------------------------------------------------------------------------*/
static qdssc_svc_instance_id_enum_type_v01 da_cmd_qmi_rpm_liaison =
   QDSSC_SVC_INSTANCE_ID_UNKNOWN_V01;

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/

 /*=========================================================================*
 * RPM commands
 *=========================================================================*/
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t da_cmd_qmi_set_rpm_evt(int type, uint8 group, uint64 bitmask)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_rpm_swe_req_msg_v01 rqst;
   qdssc_set_rpm_swe_resp_msg_v01 resp;

   switch (type)
   {
      case 0: rqst.log = QDSSC_RPM_SWE_LOG_RAM_V01;    break;
      case 1: rqst.log = QDSSC_RPM_SWE_LOG_QDSS_V01;   break;
      default: return TRACER_CMDRESP_FAIL;
   }
   rqst.group = group;
   rqst.bitmask = bitmask;

   if (QDSSC_SVC_INSTANCE_ID_UNKNOWN_V01 == da_cmd_qmi_rpm_liaison)
   {  // RPM's liaison is unknown so try Modem first.
      da_cmd_qmi_rpm_liaison = QDSSC_SVC_INSTANCE_ID_MODEM_V01;
      qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_rpm_liaison,
                    QMI_QDSSC_SET_RPM_SWE_REQ_V01,
                    &rqst, sizeof(rqst), &resp, sizeof(resp));
      if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
      {
         return TRACER_CMDRESP_SUCCESS;
      }
      else
      {
         // Try ADSP next.
         da_cmd_qmi_rpm_liaison = QDSSC_SVC_INSTANCE_ID_ADSP_V01;
      }
   }
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_rpm_liaison,
                 QMI_QDSSC_SET_RPM_SWE_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      return TRACER_CMDRESP_SUCCESS;
   }
   else
   {
      return TRACER_CMDRESP_FAIL;
   }
}
/*-------------------------------------------------------------------------*/
int da_cmd_qmi_set_rpm_etm(uint8 state)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_rpm_etm_req_msg_v01 rqst;
   qdssc_set_rpm_etm_resp_msg_v01 resp;

   rqst.state = (0==state) ? QDSSC_ETM_STATE_DISABLED_V01 :
                             QDSSC_ETM_STATE_ENABLED_V01;
   if (QDSSC_SVC_INSTANCE_ID_UNKNOWN_V01 == da_cmd_qmi_rpm_liaison)
   {
      // RPM's liaison is unknown so try Modem first.
      da_cmd_qmi_rpm_liaison = QDSSC_SVC_INSTANCE_ID_MODEM_V01;
      qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_rpm_liaison,
                    QMI_QDSSC_SET_RPM_ETM_REQ_V01,
                    &rqst, sizeof(rqst), &resp, sizeof(resp));
      if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
      {
         return QDSS_CONTROL_SUCCESS;
      }
      else
      {
         // Try ADSP next.
         da_cmd_qmi_rpm_liaison = QDSSC_SVC_INSTANCE_ID_ADSP_V01;
      }
   }
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_rpm_liaison,
                 QMI_QDSSC_SET_RPM_ETM_REQ_V01,
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

