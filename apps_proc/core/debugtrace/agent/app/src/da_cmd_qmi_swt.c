/*=============================================================================
  FILE: da_cmd_qmi_swt.c

  OVERVIEW: Debug Agent -
            Interface for sending swt commands through QMI QDSSC.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_qmi_swt.c#1 $

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

/*=========================================================================*/
//  Tracer output commands
tracer_cmdresp_t da_cmd_qmi_get_swt(da_parser_ssid_t ssid, uint32 *state)
{
   qmi_client_error_type qmi_rval;

   qdssc_get_swt_req_msg_v01 rqst;
   qdssc_get_swt_resp_msg_v01 resp;

   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_SWT_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      *state = (QDSSC_SWT_OUTPUT_STATE_ENABLED_V01 == resp.state) ? 1 : 0;
      return TRACER_CMDRESP_SUCCESS;
   }
   else
   {
      return TRACER_CMDRESP_FAIL;
   }
}
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t da_cmd_qmi_set_swt(da_parser_ssid_t ssid, uint32 state)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_swt_req_msg_v01 rqst;
   qdssc_set_swt_resp_msg_v01 resp;

   rqst.state = (0 == state) ? QDSSC_SWT_OUTPUT_STATE_DISABLED_V01 :
                               QDSSC_SWT_OUTPUT_STATE_ENABLED_V01;
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_SWT_REQ_V01,
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
/*=========================================================================*/
//  Tracer entity commands
tracer_cmdresp_t _get_entity(da_parser_ssid_t ssid, uint8 entity, uint32 *mask)
{
   qmi_client_error_type qmi_rval;

   qdssc_get_entity_req_msg_v01 rqst;
   qdssc_get_entity_resp_msg_v01 resp;

   rqst.entity_id = (qdssc_entity_enum_type_v01)entity;
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_ENTITY_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      *mask = resp.state;
      return TRACER_CMDRESP_SUCCESS;
   }
   else
   {
      return TRACER_CMDRESP_FAIL;
   }
}
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t _get_entity_all(da_parser_ssid_t ssid, uint32 *mask,
                                 int mask_bytes)
{
   qmi_client_error_type qmi_rval;

   qdssc_get_entity_all_req_msg_v01 rqst;
   qdssc_get_entity_all_resp_msg_v01 resp;

   memset(resp.mask, 0, sizeof(resp.mask));
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_ENTITY_ALL_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      memscpy(mask, mask_bytes, resp.mask, sizeof(resp.mask));
      return TRACER_CMDRESP_SUCCESS;
   }
   else
   {
      memset(mask, 0, mask_bytes);
      return TRACER_CMDRESP_FAIL;
   }
}
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t da_cmd_qmi_get_entity(da_parser_ssid_t ssid, uint8 entity,
                                       uint32 *mask, int mask_bytes)
{
   if (0xFF == entity)
   {
      return _get_entity_all(ssid, mask, mask_bytes);
   }
   else
   {
      return _get_entity(ssid, entity, mask);
   }

}
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t _set_entity(da_parser_ssid_t ssid, uint8 entity, uint32 state)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_entity_req_msg_v01 rqst;
   qdssc_set_entity_resp_msg_v01 resp;

   rqst.entity_id = (qdssc_entity_enum_type_v01)entity;
   rqst.state = (0 == state) ? QDSSC_ENTITY_STATE_DISABLED_V01 :
                               QDSSC_ENTITY_STATE_ENABLED_V01;
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_ENTITY_REQ_V01,
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
tracer_cmdresp_t _set_entity_all(da_parser_ssid_t ssid, uint32 state)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_entity_all_req_msg_v01 rqst;
   qdssc_set_entity_all_resp_msg_v01 resp;

   rqst.state = (0 == state) ? QDSSC_ENTITY_STATE_DISABLED_V01 :
                               QDSSC_ENTITY_STATE_ENABLED_V01;
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_ENTITY_ALL_REQ_V01,
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
tracer_cmdresp_t da_cmd_qmi_set_entity(da_parser_ssid_t ssid,
                                       uint8 entity, uint32 state)
{
   if (0xFF == entity)
   {
      return _set_entity_all(ssid,  state);
   }
   else
   {
      return _set_entity(ssid, entity, state);
   }
}
/*=========================================================================*/
//  Tracer event commands
tracer_cmdresp_t da_cmd_qmi_get_swevt_tag(da_parser_ssid_t ssid,
                                          char *tag, int tag_len)
{
   qmi_client_error_type qmi_rval;

   qdssc_get_swevt_tag_req_msg_v01 rqst;
   qdssc_get_swevt_tag_resp_msg_v01 resp;

   memset (tag, 0, tag_len);
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_SWEVT_TAG_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      memscpy(tag, tag_len, resp.tag_str, sizeof(resp.tag_str) - 1);
      return TRACER_CMDRESP_SUCCESS;
   }
   else
   {
      return TRACER_CMDRESP_FAIL;
   }
}
/*-------------------------------------------------------------------------*/
tracer_cmdresp_t da_cmd_qmi_get_swevt(da_parser_ssid_t ssid, uint32 evtid,
                                      uint32 *state)
{
   qmi_client_error_type qmi_rval;

   qdssc_get_swevt_req_msg_v01 rqst;
   qdssc_get_swevt_resp_msg_v01 resp;

   rqst.event_id = evtid;
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_GET_SWEVT_REQ_V01,
                 &rqst, sizeof(rqst), &resp, sizeof(resp));
   if (QMI_NO_ERR == qmi_rval && QMI_RESULT_SUCCESS_V01 == resp.resp.result)
   {
      *state = (QDSSC_EVENT_STATE_ENABLED_V01 == resp.state) ? 1 : 0;
      return TRACER_CMDRESP_SUCCESS;
   }
   else
   {
      return TRACER_CMDRESP_FAIL;
   }
}

/*-------------------------------------------------------------------------*/
tracer_cmdresp_t da_cmd_qmi_set_swevt(da_parser_ssid_t ssid, uint32 evtid,
                                      uint32 state)
{
   qmi_client_error_type qmi_rval;

   qdssc_set_swevt_req_msg_v01 rqst;
   qdssc_set_swevt_resp_msg_v01 resp;

   rqst.event_id = evtid;
   rqst.state = (0 == state) ? QDSSC_EVENT_STATE_DISABLED_V01 :
                               QDSSC_EVENT_STATE_ENABLED_V01;
   qmi_rval = qdss_qmi_ctrl_client_send(da_cmd_qmi_ssid2svcid(ssid),
                 QMI_QDSSC_SET_SWEVT_REQ_V01,
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

