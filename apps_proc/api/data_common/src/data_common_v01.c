/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A _ C O M M O N _ V 0 1  . C

GENERAL DESCRIPTION
  This is the file which defines the data_common service Data structures.

  Copyright (c) 2006-2019 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.tx/1.0/data_common/src/data_common_v01.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 6.14.9 
   It was generated on: Fri Aug 14 2020 (Spin 0)
   From IDL File: data_common_v01.idl */

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "data_common_v01.h"


/*Type Definitions*/
static const uint8_t data_ep_id_type_data_v01[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ep_id_type_v01, ep_type),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(data_ep_id_type_v01, iface_id),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_coalescing_info_type_data_v01[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_coalescing_info_type_v01, tcp_coalescing),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_coalescing_info_type_v01, udp_coalescing),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t data_mac_addr_type_data_v01[] = {
  QMI_IDL_FLAGS_IS_ARRAY |QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(data_mac_addr_type_v01, mac_addr),
  QMI_DATA_MAC_ADDR_LEN_V01,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
/* Type Table */
static const qmi_idl_type_table_entry  data_common_type_table_v01[] = {
  {sizeof(data_ep_id_type_v01), data_ep_id_type_data_v01},
  {sizeof(data_coalescing_info_type_v01), data_coalescing_info_type_data_v01},
  {sizeof(data_mac_addr_type_v01), data_mac_addr_type_data_v01}
};

/* Message Table */
/* No Messages Defined in IDL */

/* Range Table */
/* Predefine the Type Table Object */
const qmi_idl_type_table_object data_common_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *data_common_qmi_idl_type_table_object_referenced_tables_v01[] =
{&data_common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
const qmi_idl_type_table_object data_common_qmi_idl_type_table_object_v01 = {
  sizeof(data_common_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  0,
  1,
  data_common_type_table_v01,
  NULL,
  data_common_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

