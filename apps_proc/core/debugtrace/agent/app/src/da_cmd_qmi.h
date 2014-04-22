#ifndef DA_CMD_QMI_H
#define DA_CMD_QMI_H
/**
  @file da_cmd_qmi.h

Debug Agent -  Interface for sending commands through QMI QDSSC.

*/
/*=============================================================================
  Copyright (c) 2013-2015 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
/*=============================================================================
  $Header: //components/rel/core.tx/6.0/debugtrace/agent/app/src/da_cmd_qmi.h#1 $

  when       who     what, where, why
  --------   ---    -----------------------------------------------------------
  10/19/13   lht    Initial release

=============================================================================*/


/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/

#include "com_dtypes.h"
#include "qdss_qmi_ctrl_client.h"
#include "da_parser.h"
#include "tracer_config.h"
#include "qdss_control.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/
qdssc_svc_instance_id_enum_type_v01
da_cmd_qmi_ssid2svcid(da_parser_ssid_t ssid);

/*-------------------------------------------------------------------------*/
/* SWT commands */
tracer_cmdresp_t da_cmd_qmi_get_swt(da_parser_ssid_t ssid, uint32 *state);
tracer_cmdresp_t da_cmd_qmi_set_swt(da_parser_ssid_t ssid, uint32 state);

tracer_cmdresp_t da_cmd_qmi_get_entity(da_parser_ssid_t ssid, uint8 entity,
                                       uint32 *mask, int mask_len);
tracer_cmdresp_t da_cmd_qmi_set_entity(da_parser_ssid_t ssid, uint8 entity,
                                       uint32 state);

tracer_cmdresp_t da_cmd_qmi_get_swevt_tag(da_parser_ssid_t ssid,
                                          char *tag, int tag_len);

tracer_cmdresp_t da_cmd_qmi_get_swevt(da_parser_ssid_t ssid, uint32 evtid,
                                      uint32 *state);
tracer_cmdresp_t da_cmd_qmi_set_swevt(da_parser_ssid_t ssid, uint32 evtid,
                                      uint32 state);


/*-------------------------------------------------------------------------*/
/* RPM commands */
tracer_cmdresp_t da_cmd_qmi_set_rpm_evt(int type, uint8 group, uint64 bitmask);
int da_cmd_qmi_set_rpm_etm(uint8 state);

/*-------------------------------------------------------------------------*/
/* ETM commands */
// Return values defined in qdss_control.h

int da_cmd_qmi_set_etm(da_parser_ssid_t ssid, uint8 state);
int da_cmd_qmi_get_etm(da_parser_ssid_t ssid, uint8 *state);

int da_cmd_qmi_set_etm_config(da_parser_ssid_t ssid, char *set_str);
int da_cmd_qmi_get_etm_config(da_parser_ssid_t ssid, char *get_str,
                              char *setting_str, size_t setting_size);

/*-------------------------------------------------------------------------*/
/* HWE commands */
// Return values defined in qdss_control.h

int da_cmd_qmi_set_hwe_reg(da_parser_ssid_t ssid,
                           uint32 regaddr, uint32 value);


#endif /* #ifndef DA_CMD_QMI_H */

