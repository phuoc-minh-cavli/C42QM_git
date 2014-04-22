/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              P O W E R _ S A V E _ M O D E _ S E R V I C E _ I M P L _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the psm service Data structures.

  Copyright (c) 2013-2019 Qualcomm Technologies, Inc. All rights reserved.
  Qualcomm Technologies Proprietary and Confidential.


  $Header: //components/rel/mmcp.mpss/6.1.10/mmode/qmi/src/power_save_mode_service_impl_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.7
   It was generated on: Fri Mar 13 2020 (Spin 0)
   From IDL File: power_save_mode_service_v01.idl */

#include "qmi_si.h"
#include "power_save_mode_service_v01.h"

static const uint32_t psm_indication_register_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* report_psm_status */
    QMI_SI_TLV_SET(0x11)   |  /* report_psm_cfg_change */
    QMI_SI_TLV_SET(0x12)      /* report_sensor_status */
};

static const qmi_implemented_optionals psm_indication_register_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_indication_register_req_msg_impl_array_v01
};
static const qmi_implemented_optionals psm_indication_register_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_psmication_register_reqresp_impl_tuple_v01 = 
{
  &psm_indication_register_req_msg_impl_v01,
  &psm_indication_register_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_get_cfg_params_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* psm_enabled */
    QMI_SI_TLV_SET(0x11)   |  /* duration_threshold */
    QMI_SI_TLV_SET(0x12)   |  /* duration_due_to_oos */
    QMI_SI_TLV_SET(0x13)   |  /* randomization_window */
    QMI_SI_TLV_SET(0x14)   |  /* active_timer */
    QMI_SI_TLV_SET(0x15)   |  /* periodic_update_timer */
    QMI_SI_TLV_SET(0x16)      /* early_wakeup_time */
};

static const qmi_implemented_optionals psm_get_cfg_params_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_get_cfg_params_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_get_cfg_params_reqresp_impl_tuple_v01 = 
{
  NULL,
  &psm_get_cfg_params_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_enter_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* duration_in_psm */
    QMI_SI_TLV_SET(0x11)      /* status */
};

static const qmi_implemented_optionals psm_enter_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_enter_req_msg_impl_array_v01
};
static const qmi_implemented_optionals psm_enter_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_psm_enter_reqresp_impl_tuple_v01 = 
{
  &psm_enter_req_msg_impl_v01,
  &psm_enter_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_status_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* reject_reason */
    QMI_SI_TLV_SET(0x11)      /* periodic_update_timer */
};

static const qmi_implemented_optionals psm_status_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_status_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_status_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &psm_status_ind_msg_impl_v01
};

static const uint32_t psm_set_cfg_params_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* psm_enabled */
    QMI_SI_TLV_SET(0x11)   |  /* active_timer */
    QMI_SI_TLV_SET(0x12)      /* periodic_update_timer */
};

static const qmi_implemented_optionals psm_set_cfg_params_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_set_cfg_params_req_msg_impl_array_v01
};
static const qmi_implemented_optionals psm_set_cfg_params_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_psm_set_cfg_params_reqresp_impl_tuple_v01 = 
{
  &psm_set_cfg_params_req_msg_impl_v01,
  &psm_set_cfg_params_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_cfg_params_change_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* psm_enabled */
    QMI_SI_TLV_SET(0x11)   |  /* active_timer */
    QMI_SI_TLV_SET(0x12)      /* periodic_update_timer */
};

static const qmi_implemented_optionals psm_cfg_params_change_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_cfg_params_change_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_cfg_params_change_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &psm_cfg_params_change_ind_msg_impl_v01
};

static const uint32_t psm_get_capability_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* te_psm_capability */
};

static const qmi_implemented_optionals psm_get_capability_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_get_capability_req_msg_impl_array_v01
};
static const uint32_t psm_get_capability_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* psm_capability */
};

static const qmi_implemented_optionals psm_get_capability_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_get_capability_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_get_capability_reqresp_impl_tuple_v01 = 
{
  &psm_get_capability_req_msg_impl_v01,
  &psm_get_capability_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_set_sensor_configuration_req_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* sensor_policy */
    QMI_SI_TLV_SET(0x11)   |  /* sensor_batch_cnt */
    QMI_SI_TLV_SET(0x12)   |  /* reporting_interval */
    QMI_SI_TLV_SET(0x13)      /* sensor_config_list */
};

static const qmi_implemented_optionals psm_set_sensor_configuration_req_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_set_sensor_configuration_req_msg_impl_array_v01
};
static const qmi_implemented_optionals psm_set_sensor_configuration_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_psm_set_sensor_configuration_reqresp_impl_tuple_v01 = 
{
  &psm_set_sensor_configuration_req_msg_impl_v01,
  &psm_set_sensor_configuration_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals psm_get_sensor_data_req_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const uint32_t psm_get_sensor_data_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)      /* sensor_info */
};

static const qmi_implemented_optionals psm_get_sensor_data_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_get_sensor_data_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_get_sensor_data_reqresp_impl_tuple_v01 = 
{
  &psm_get_sensor_data_req_msg_impl_v01,
  &psm_get_sensor_data_resp_msg_impl_v01,
  NULL
};

static const qmi_implemented_optionals psm_reset_sensor_config_data_resp_msg_impl_v01 =
{
  0, /* No Optional TLVs in message */
  0
};
static const qmi_implemented_optionals_tuple qmi_psm_reset_sensor_config_data_reqresp_impl_tuple_v01 = 
{
  NULL,
  &psm_reset_sensor_config_data_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_get_last_policy_met_info_resp_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* policy_met_sensor_mask */
    QMI_SI_TLV_SET(0x11)      /* batch_full_sensor_mask */
};

static const qmi_implemented_optionals psm_get_last_policy_met_info_resp_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_get_last_policy_met_info_resp_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_get_last_sensor_policy_met_info_reqresp_impl_tuple_v01 = 
{
  NULL,
  &psm_get_last_policy_met_info_resp_msg_impl_v01,
  NULL
};

static const uint32_t psm_sensor_policy_met_ind_msg_impl_array_v01[] =
{
    QMI_SI_TLV_SET(0x10)   |  /* policy_met_sensor_mask */
    QMI_SI_TLV_SET(0x11)      /* batch_full_sensor_mask */
};

static const qmi_implemented_optionals psm_sensor_policy_met_ind_msg_impl_v01 =
{
  1, /* number of elements in array */
  psm_sensor_policy_met_ind_msg_impl_array_v01
};
static const qmi_implemented_optionals_tuple qmi_psm_sensor_policy_met_impl_tuple_v01 = 
{
  NULL,
  NULL,
  &psm_sensor_policy_met_ind_msg_impl_v01
};

static const qmi_implemented_optionals_tuple * const psm_service_implemented_array_v01[] =
{
    NULL,                                                /* 0x0000 */
    &qmi_psmication_register_reqresp_impl_tuple_v01,     /* 0x0001 */
    &qmi_psm_get_cfg_params_reqresp_impl_tuple_v01,      /* 0x0002 */
    &qmi_psm_enter_reqresp_impl_tuple_v01,               /* 0x0003 */
    &qmi_psm_status_impl_tuple_v01,                      /* 0x0004 */
    &qmi_psm_set_cfg_params_reqresp_impl_tuple_v01,      /* 0x0005 */
    &qmi_psm_cfg_params_change_impl_tuple_v01,           /* 0x0006 */
    &qmi_psm_get_capability_reqresp_impl_tuple_v01,      /* 0x0007 */
    &qmi_psm_set_sensor_configuration_reqresp_impl_tuple_v01, /* 0x0008 */
    &qmi_psm_get_sensor_data_reqresp_impl_tuple_v01,     /* 0x0009 */
    &qmi_psm_reset_sensor_config_data_reqresp_impl_tuple_v01, /* 0x000A */
    &qmi_psm_get_last_sensor_policy_met_info_reqresp_impl_tuple_v01, /* 0x000B */
    &qmi_psm_sensor_policy_met_impl_tuple_v01            /* 0x000C */
};

static const qmi_implemented_messages psm_service_implemented_v01 =
{
  0x000C,  /* maximum message ID in this service */
  psm_service_implemented_array_v01
};

const qmi_implemented_messages *psm_get_service_impl_v01 (void)
{
  return &psm_service_implemented_v01;
}
