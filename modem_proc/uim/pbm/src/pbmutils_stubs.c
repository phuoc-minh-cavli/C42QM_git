/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                P B M _ U T I LS _ S T U B S.C

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM.

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmutils_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/23/16   nr      ECC optimizations for IoE devices
02/02/16   nr      Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "pbmlib.h"
#include "pbmutils.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION pbm_is_rec_hidden

DESCRIPTION
    This is stub function for pbm_is_rec_hidden ().

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_is_rec_hidden(
  pbm_record_id_type        rec_id
)
{
  (void)rec_id;

  return FALSE;
} /* pbm_is_rec_hidden */


/*===========================================================================
FUNCTION PBM_CLIENT_CMD_HANDLE

DESCRIPTION
    This is stub function for pbm_stop_fileinfo_internal ().

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pbm_client_cmd_handle(
  void
)
{
  return;
}/* pbm_client_cmd_handle */


/*===========================================================================
FUNCTION PBM_CACHE_INIT

DESCRIPTION
 This is stub function for pbm_cache_init ().

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_cache_init(
  void
)
{
  return PBM_SUCCESS;
} /* pbm_cache_init */


/*===========================================================================
FUNCTION pbm_convert_to_complex_anr

DESCRIPTION
  Complex ANR conversion stub function.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_convert_to_complex_anr(
  const pbm_field_s_type      *fields,
  int                          num_fields,
  pbm_field_s_type           **new_fields,
  int                         *total_fields
)
{
  (void)fields;
  (void)num_fields;
  (void)new_fields;
  (void)total_fields;

  return PBM_ERROR_NOT_SUPPORTED;
} /* pbm_convert_to_complex_anr */


/*===========================================================================
FUNCTION pbm_util_get_status_of_queried_number

DESCRIPTION
 This is stub function for pbm_util_get_ss_ton_status_of_queried_number ()

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
boolean pbm_util_get_ss_ton_status_of_queried_number(
  boolean         *valid_ss_string,
  boolean         *queried_number_has_plus,
  int             *data_size,
  byte            *local_number
)
{
  (void)valid_ss_string;
  (void)queried_number_has_plus;
  (void)data_size;
  (void)local_number;

  return FALSE;
} /* pbm_util_get_ss_ton_status_of_queried_number */


/*===========================================================================
FUNCTION pbm_util_is_valid_sups_string_find_pos_for_plus

DESCRIPTION
  This is stub function for pbm_util_is_valid_sups_string ()

DEPENDENCIES

SIDE EFFECTS

===========================================================================*/
boolean pbm_util_is_valid_sups_string(
  char     *sups_str,
  uint8     sups_str_len,
  int      *pos,
  boolean   find_pos
)
{
  (void)sups_str;
  (void)sups_str_len;
  (void)pos;
  (void)find_pos;

  return FALSE;
} /* pbm_util_is_valid_sups_string */


/*===========================================================================
FUNCTION PBM_COMPLEX_NUM_SUB_FIELDS

DESCRIPTION
  This is stub function for pbm_complex_num_sub_fields ()

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
int pbm_complex_num_sub_fields(
  pbm_complex_field_s_type *complex_field
)
{
  (void)complex_field;

  return 0;
} /* pbm_complex_num_sub_fields */


/*===========================================================================
FUNCTION PBM_COMPLEX_FILL_SIZES

DESCRIPTION
  This is stub function for pbm_complex_fill_sizes ()

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
int pbm_complex_fill_sizes(
  uint32                   *sizes,
  int                       max_num_sub_fields,
  pbm_complex_field_s_type *complex_field
)
{
  (void)sizes;
  (void)max_num_sub_fields;
  (void)complex_field;

  return 0;
} /* pbm_complex_fill_sizes */


/*===========================================================================
FUNCTION PBM_COMPLEX_UNWRAP

DESCRIPTION
  This is stub function for pbm_complex_unwrap ()

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_complex_unwrap(
  pbm_complex_field_s_type  *complex_field,
  pbm_complex_result_s_type *result
)
{
  (void)complex_field;
  (void)result;

  return PBM_ERROR;
} /* pbm_complex_unwrap */


/*===========================================================================
FUNCTION pbm_get_internal_field_info
DESCRIPTION
  This is stub function for pbm_get_internal_non_ecc_field_info ()

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_internal_non_ecc_field_info(
  pbm_phonebook_type        pb_id,
  pbm_internal_field_info  *field_info,
  int                       field_group,
  pbm_session_enum_type     session_type
)
{
  (void)pb_id;
  (void)field_info;
  (void)field_group;
  (void)session_type;

  return PBM_ERROR;
}/* pbm_get_internal_non_ecc_field_info */


/*===========================================================================
FUNCTION PBM_NOTIFY

DESCRIPTION
  This is stub function for pbm_notify ()

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_notify(
  pbm_notify_data_s_type    *notify_data
)
{
  (void)notify_data;

  return;
}/* pbm_notify */


/*===========================================================================
FUNCTION pbm_add_notify

DESCRIPTION
  This is stub function for pbm_add_notify ()

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_add_notify(
  pbm_notify_event_e_type       evt,
  pbm_session_event_data_u_type evt_data
)
{
  (void)evt;
  (void)evt_data;

  return;
}/* pbm_add_notify */

