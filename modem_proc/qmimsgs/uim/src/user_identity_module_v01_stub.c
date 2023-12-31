/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        U S E R _ I D E N T I T Y _ M O D U L E _ V 0 1 _ S T U B  . C

GENERAL DESCRIPTION
  This is the file which defines the uim service Data structures.

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


  $Header: //components/rel/qmimsgs.mpss/4.3.4/uim/src/user_identity_module_v01_stub.c#1 $
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "user_identity_module_v01.h"
#include "common_v01.h"


/*Type Definitions*/

/*Message Definitions*/

/* Type Table */
static const qmi_idl_type_table_entry  uim_type_table_v01[] = {0};

/* Message Table */
static const qmi_idl_message_table_entry uim_message_table_v01[] = {0};


/* Range Table */
/* No Ranges Defined in IDL */

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object uim_qmi_idl_type_table_object_v01;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *uim_qmi_idl_type_table_object_referenced_tables_v01[] =
{&uim_qmi_idl_type_table_object_v01, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object uim_qmi_idl_type_table_object_v01 = {
  sizeof(uim_type_table_v01)/sizeof(qmi_idl_type_table_entry ),
  sizeof(uim_message_table_v01)/sizeof(qmi_idl_message_table_entry),
  1,
  uim_type_table_v01,
  uim_message_table_v01,
  uim_qmi_idl_type_table_object_referenced_tables_v01,
  NULL
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry uim_service_command_messages_v01[] = {0};

static const qmi_idl_service_message_table_entry uim_service_response_messages_v01[] = {0};

static const qmi_idl_service_message_table_entry uim_service_indication_messages_v01[] = {0};

/*Service Object*/
struct qmi_idl_service_object uim_qmi_idl_service_object_v01 = {
  0x06,
  0x01,
  0x0B,
  8204,
  { sizeof(uim_service_command_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(uim_service_response_messages_v01)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(uim_service_indication_messages_v01)/sizeof(qmi_idl_service_message_table_entry) },
  { uim_service_command_messages_v01, uim_service_response_messages_v01, uim_service_indication_messages_v01},
  &uim_qmi_idl_type_table_object_v01,
  UIM_V01_IDL_MINOR_VERS,
  NULL
};

/* Service Object Accessor */
qmi_idl_service_object_type uim_get_service_object_internal_v01
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( UIM_V01_IDL_MAJOR_VERS != idl_maj_version || UIM_V01_IDL_MINOR_VERS != idl_min_version
       || UIM_V01_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&uim_qmi_idl_service_object_v01;
}

