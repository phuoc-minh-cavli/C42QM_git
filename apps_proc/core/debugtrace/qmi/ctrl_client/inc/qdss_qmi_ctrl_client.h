#ifndef QDSS_QMI_CTRL_CLIENT_H
#define QDSS_QMI_CTRL_CLIENT_H
/**
  File: qdss_qmi_ctrl_client.h

  QMI Client for QDSS Subystem Control

*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/qmi/ctrl_client/inc/qdss_qmi_ctrl_client.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  09/24/13   lht    Initial release

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "qmi_client.h"
#include "qualcomm_debug_subsystem_control_v01.h"

/*---------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
 * Type Declarations
 * ------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/**
  @brief Parse one line of ASCII text.

  @param svc_inst_id [in]: Service instance id.
  @param pRqst [in]: Pointer to request message.
  @param rqst_len [in]: Length of request message.
  @param pResp [in]: Pointer to structure for response message.
  @param resp_len [in]: Length of response message.

  @return  QMI_NO_ERROR : Completed without error.
           otherwise : Error occurred.
 */
qmi_client_error_type qdss_qmi_ctrl_client_send(
   qdssc_svc_instance_id_enum_type_v01 svc_inst_id,
   unsigned int msg_id,
   void *pRqst,
   unsigned int rqst_len,
   void *pResp,
   unsigned int resp_len
);

#endif /* #ifndef QDSS_QMI_CTRL_CLIENT_H */


