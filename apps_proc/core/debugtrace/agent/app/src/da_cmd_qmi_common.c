/*=============================================================================
  FILE: da_cmd_qmi_common.c

  OVERVIEW: Debug Agent -
            Common functions for sending commands through QMI QDSSC.

  DEPENDENCIES: None.

=============================================================================*/
/*=============================================================================
  Copyright (c) 2013 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_qmi_common.c#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  10/19/13   lht    Initial revision.

=============================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "qualcomm_debug_subsystem_control_v01.h"
#include "da_parser.h"

/*---------------------------------------------------------------------------
 * Function Definitions
 * ------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
// Convert parser's subsystem ID to QDSSC's service ID
qdssc_svc_instance_id_enum_type_v01
da_cmd_qmi_ssid2svcid(da_parser_ssid_t ssid)
{
   switch (ssid)
   {
      case DA_PARSER_SSID_RSV:   return QDSSC_SVC_INSTANCE_ID_UNKNOWN_V01;
      case DA_PARSER_SSID_APPS:  return QDSSC_SVC_INSTANCE_ID_APPS_V01;
      case DA_PARSER_SSID_MODEM: return QDSSC_SVC_INSTANCE_ID_MODEM_V01;
      case DA_PARSER_SSID_WCNSS: return QDSSC_SVC_INSTANCE_ID_WCN_V01;
      case DA_PARSER_SSID_RPM:   return QDSSC_SVC_INSTANCE_ID_RPM_V01;
      case DA_PARSER_SSID_ADSP:  return QDSSC_SVC_INSTANCE_ID_ADSP_V01;
      case DA_PARSER_SSID_VENUS: return QDSSC_SVC_INSTANCE_ID_VENUS_V01;
      case DA_PARSER_SSID_GNSS:  return QDSSC_SVC_INSTANCE_ID_GNSS_V01;
      case DA_PARSER_SSID_SENSOR:return QDSSC_SVC_INSTANCE_ID_SENSOR_V01;
      case DA_PARSER_SSID_AUDIO: return QDSSC_SVC_INSTANCE_ID_AUDIO_V01;
      case DA_PARSER_SSID_VPU:   return QDSSC_SVC_INSTANCE_ID_VPU_V01;
      default:                   return QDSSC_SVC_INSTANCE_ID_UNKNOWN_V01;
   }
}

