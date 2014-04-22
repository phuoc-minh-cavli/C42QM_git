/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER LIBRARY

GENERAL DESCRIPTION
  This file contains the APIs exported by PBM service which is executed as a
  library within the PBM client's task.

  Copyright (c) 2016 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbmlib_stubs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/16    sn     Fix compiler errors 
05/09/16    sp     Fix compiler warnings
03/24/16    lm     Fix compilation errors
31/01/16    nr     Intial version
===========================================================================*/
#include "customer.h"
#include "uim_msg.h"
#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmcache.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmefs.h"
#include "pbmtask_i.h"
#include "stdlib.h"
#include "pbm_conversion.h"
#include "stringl/stringl.h"


/*===========================================================================
                         DEFINITIONS
===========================================================================*/
#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
 FUNCTION PBM_SESSION_CALCULATE_UNDECODED_FIELDS_SIZE_FROM_ID

 DESCRIPTION
   This is stub function for pbm_session_calculate_undecoded_fields_size_from_id ().

 DEPENDENCIES

 SIDE EFFECTS
===========================================================================*/
int pbm_session_calculate_undecoded_fields_size_from_id(
  pbm_record_id_type   rec_id
)
{
  (void)rec_id;

  return 0;
} /* pbm_session_calculate_undecoded_fields_size_from_id */


/*===========================================================================
FUNCTION PBM_SESSION_UNDECODED_RECORD_READ

DESCRIPTION
  This is stub function for pbm_session_record_read_undecoded ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_read_undecoded(
  pbm_record_id_type            rec_id,
  uint16                       *category_ptr,
  int                          *num_fields_ptr,
  uint8                        *data_buf,
  uint32                        data_buf_size
)
{
  (void)rec_id;
  (void)category_ptr;
  (void)num_fields_ptr;
  (void)data_buf;
  (void)data_buf_size;

  return PBM_ERROR;
} /* pbm_session_record_read_undecoded */


/*===========================================================================
FUNCTION PBM_SESSION_RECORD_WRITE

DESCRIPTION
  This is stub function for pbm_session_record_write ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_write(
  pbm_phonebook_type      pb_id,
  pbm_record_id_type     *rec_id_ptr,
  uint16                  cat,
  int                     num_fields,
  const uint8            *data_buf,
  int                     data_buf_size,
  PBM_WRITE_CB_FUNC       proc_func,
  void                   *user_data)
{
  (void)pb_id;
  (void)rec_id_ptr;
  (void)cat;
  (void)num_fields;
  (void)data_buf;
  (void)data_buf_size;
  (void)proc_func;
  (void)user_data;

  return PBM_ERROR;
}/* pbm_session_record_write */


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_REC_INIT

DESCRIPTION
  This is stub function for pbm_session_enum_rec_init ().

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_rec_init(
  pbm_phonebook_type    pb_id,
  uint16                category,
  pbm_field_id_e_type   field_id,
  const uint8          *data_ptr,
  uint16                data_size,
  uint32                flags)
{
  (void)pb_id;
  (void)category;
  (void)field_id;
  (void)data_ptr;
  (void)data_size;
  (void)flags;

  return PBM_ERROR;
} /* pbm_session_enum_rec_init */


/*===========================================================================
FUNCTION PBM_SESSION_ENUM_NEXT_REC_ID

DESCRIPTION
  This is stub function for pbm_session_enum_next_rec_id ().

DEPENDENCIES
pbm_enum_rec_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_enum_next_rec_id(
  pbm_record_id_type     *rec_id_ptr
)
{
  (void)rec_id_ptr;

  return PBM_ERROR;
}/* pbm_session_enum_next_rec_id */


/*===========================================================================
FUNCTION PBM_SESSION_CLEAR_PHONEBOOK

DESCRIPTION
  This is stub function for pbm_session_clear_phonebook ().

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_clear_phonebook(
  pbm_phonebook_type   pb_id
)
{
  (void)pb_id;

  return PBM_ERROR;
} /* pbm_session_clear_phonebook */



/*===========================================================================*
 *              PHONE BOOK MANAGER LEGACY BASED API                          *
 *                                                                           *
 * These functions supply the API for the legacy based requests.             *
 *===========================================================================*/

/*===========================================================================
FUNCTION PBM_GET_PHONEBOOK_USIM_FIELDS_CAPIBILITIES

DESCRIPTION
  This is stub function for pbm_get_phonebook_usim_fields_capibilities ().

DEPENDENCIES
  ADN cache should be ready.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_phonebook_usim_fields_capibilities(
  pbm_phonebook_type                      pb_id,
  pbm_all_usim_field_capabilities_s_type *info
)
{
  (void)pb_id;
  (void)info;

  return PBM_ERROR;
} /* pbm_get_phonebook_usim_fields_capibilities */


/*===========================================================================
FUNCTION PBM_GET_ADN_PB_SET_CAPIBILITIES

DESCRIPTION
  This is stub function for pbm_get_adn_pbset_capibilities ().

DEPENDENCIES
  ADN cache should be ready.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_adn_pbset_capibilities(
  pbm_phonebook_type                pb_id,
  pbm_adn_capabilites_pbset_s_type *info
)
{
  (void)pb_id;
  (void)info;
  
  return PBM_ERROR;
} /* pbm_get_adn_pbset_capibilities */


/*===========================================================================
FUNCTION PBM_SESSION_EXTENDED_FILE_INFO

DESCRIPTION
  This is stub function for pbm_session_extended_file_info ().

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_extended_file_info(
  pbm_phonebook_type             pb_id,
  pbm_extended_fileinfo_s_type  *info
)
{
  (void)pb_id;
  (void)info;

  return PBM_ERROR;
} /* pbm_session_extended_file_info */


/*===========================================================================
FUNCTION PBM_SESSION_EXTENDED_FILE_INFO_ASYNC

DESCRIPTION
  This is stub function for pbm_session_extended_file_info_async ().

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_extended_file_info_async(\
  pbm_phonebook_type                      pb_id,
  PBM_SESSION_EXTENDED_FILE_INFO_CB_FUNC  proc_func
)
{
  (void)pb_id;
  (void)proc_func;

  return PBM_ERROR;
} /* pbm_session_extended_file_info_async */


/*===========================================================================
FUNCTION PBM_SESSION_GET_NUM_RECS

DESCRIPTION
  This is stub function for pbm_session_get_num_recs ().

DEPENDENCIES
pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
uint16 pbm_session_get_num_recs(
  pbm_phonebook_type    pb_id
)
{
  (void)pb_id;

  return 0;
} /* pbm_session_get_num_recs */


/*===========================================================================
FUNCTION PBM_REG_EVENT_CB

DESCRIPTION
  This is stub function for pbm_reg_event_cb ().

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_reg_event_cb(
  PBM_EVENT_FUNC    user_fn
)
{
  (void)user_fn;
  
  return PBM_ERROR;
} /* pbm_reg_event_cb */


/*===========================================================================
FUNCTION PBM_SESSION_FIND_LOCATION

DESCRIPTION
  This is stub function for pbm_session_find_location ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_find_location(
  pbm_phonebook_type    pb_id,
  int                   indx,
  pbm_record_s_type   * data
)
{
  (void)pb_id;
  (void)indx;
  (void)data;

  return PBM_ERROR;
} /* pbm_session_find_location */


/*===========================================================================
FUNCTION PBM_SESSION_FIND_NUMBER

DESCRIPTION
  This is stub function for pbm_session_find_number ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_find_number(
  pbm_phonebook_type             pb_id,
  const byte                    *number,
  int                            loc_number_len,
  pbm_record_s_type            * record,
  PBM_FIND_CB_FUNC               proc_func
)
{
  (void)pb_id;
  (void)number;
  (void)loc_number_len;
  (void)record;
  (void)proc_func;

  return PBM_ERROR;
} /* pbm_session_find_number */


/*===========================================================================
FUNCTION PBM_SESSION_GET_FIELD_INFO_COUNT

DESCRIPTION
  This is stub function for pbm_session_get_field_info_count ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_get_field_info_count(
  pbm_phonebook_type       pb_id,
  pbm_cat_e_type           cat,
  int                     *num
)
{
  (void)pb_id;
  (void)cat;
  (void)num;

  return PBM_ERROR;
} /* pbm_session_get_field_info_count */


/*===========================================================================
FUNCTION PBM_SESSION_GET_FIELD_INFO

DESCRIPTION
  This is stub function for pbm_session_get_field_info ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_get_field_info(
  pbm_phonebook_type       pb_id,
  pbm_cat_e_type           cat,
  pbm_field_info_s_type   *pf,
  int                      size
)
{
  (void)pb_id;
  (void)cat;
  (void)pf;
  (void)size;

  return PBM_ERROR;
} /* pbm_session_get_field_info */


/*===========================================================================
FUNCTION PBM_SESSION_RECORD_VALIDATE

DESCRIPTION
  This is stub function for pbm_session_record_validate ().

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_session_record_validate(
  pbm_phonebook_type       pb_id,
  pbm_record_id_type       rec_id,
  pbm_cat_e_type           cat,
  uint8                   *data_buf,
  int                      data_buf_size,
  int                      num_fields
)
{
  (void)pb_id;
  (void)rec_id;
  (void)cat;
  (void)data_buf;
  (void)data_buf_size;
  (void)num_fields;

  return PBM_ERROR;
} /* pbm_session_record_validate */


/*===========================================================================
FUNCTION PBM_NOTIFY_REGISTER

DESCRIPTION
  This is stub function for pbm_notify_register ().

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_unregister
===========================================================================*/
pbm_return_type pbm_notify_register(
  PBM_NOTIFY_FUNC    func,
  void              *user_data
)
{
  (void)func;
  (void)user_data;

  return PBM_ERROR;
} /* pbm_notify_register */


/*===========================================================================
FUNCTION PBM_NOTIFY_UNREGISTER

DESCRIPTION
  This is stub function for pbm_notify_unregister ().

DEPENDENCIES
  None.

SIDE EFFECTS
  None

SEE ALSO
  pbm_notify_register
===========================================================================*/
pbm_return_type pbm_notify_unregister(
  PBM_NOTIFY_FUNC     func,
  void               *user_data
)
{
  (void)func;
  (void)user_data;
	
  return PBM_ERROR;
} /* pbm_notify_unregister */


/*===========================================================================
FUNCTION PBM_IS_INIT_COMPLETED

DESCRIPTION
  This is stub function for pbm_is_init_completed ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_is_init_completed(
)
{
  return FALSE;
} /* pbm_is_init_completed */


/*===========================================================================
FUNCTION PBM_ENUM_REC_INIT

DESCRIPTION
  This is stub function for pbm_enum_next_rec_id ().

DEPENDENCIES
  pbm_init() needs to be called first

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_enum_rec_init(
  pbm_device_type           device_type,
  uint16                    category,
  pbm_field_id_e_type       field_id,
  const uint8              *data_ptr,
  uint16                    data_size,
  uint32                    flags
)
{
  (void)device_type;
  (void)category;
  (void)field_id;
  (void)data_ptr;
  (void)data_size;
  (void)flags;

  return PBM_ERROR;
} /* pbm_enum_rec_init */


/*===========================================================================
FUNCTION pbm_enum_next_rec_id

DESCRIPTION
  This is stub function for pbm_enum_next_rec_id ().
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

pbm_return_type pbm_enum_next_rec_id(
  uint16      *rec_id_ptr
)
{
  (void)rec_id_ptr;

  return PBM_ERROR;
} /* pbm_enum_next_rec_id */


/*===========================================================================
FUNCTION pbm_find_location

DESCRIPTION
  This is stub function for pbm_find_location ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_find_location(
  pbm_device_type       device_type,
  int                   indx,
  pbm_record_s_type   * data,
  PBM_FIND_CB_FUNC      proc_func
)
{
  (void)device_type;
  (void)indx;
  (void)data;
  (void)proc_func;

  return PBM_ERROR;
} /* pbm_find_location */


/*===========================================================================
FUNCTION pbm_find_number

DESCRIPTION
  This is stub function for pbm_find_number ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_find_number(
  pbm_device_type           device_type,
  const byte               *number,
  int                       loc_number_len,
  pbm_record_s_type       * record,
  PBM_FIND_CB_FUNC          proc_func
)
{
  (void)device_type;
  (void)number;
  (void)loc_number_len;
  (void)record;
  (void)proc_func;

  return PBM_ERROR;
} /* pbm_find_number */


/*===========================================================================
FUNCTION pbm_get_num_recs

DESCRIPTION
  This is stub function for pbm_get_num_recs ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint16 pbm_get_num_recs(
  pbm_device_type     device_type
)
{
  (void)device_type;

  return 0;
} /* pbm_get_num_recs */


/*===========================================================================
FUNCTION pbm_calculate_fields_size_from_id

DESCRIPTION
  This is stub function for pbm_calculate_fields_size_from_id ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int pbm_calculate_fields_size_from_id(
  uint16       rec_id
)
{
  (void)rec_id;

  return 0;
} /* pbm_calculate_fields_size_from_id */


/*===========================================================================
FUNCTION pbm_record_read

DESCRIPTION
  This is stub function for pbm_record_read ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_record_read(
  uint16            rec_id,
  uint16           *category_ptr,
  int              *num_fields_ptr,
  uint8            *data_buf,
  uint32            data_buf_size
)
{
  (void)rec_id;
  (void)category_ptr;
  (void)num_fields_ptr;
  (void)data_buf;
  (void)data_buf_size;

  return PBM_ERROR;
} /* pbm_record_read */


/*===========================================================================
FUNCTION PBM_EXTENDED_FILE_INFO

DESCRIPTION
  This is stub function for pbm_extended_file_info ().

DEPENDENCIES
  GSDI_EXP.H.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_extended_file_info(
  pbm_device_type                  device_type,
  PBM_EXTENDED_FILE_INFO_CB_FUNC   proc_func
)
{
  (void)device_type;
  (void)proc_func;

  return PBM_ERROR;
} /* pbm_extended_file_info */


/*===========================================================================
FUNCTION pbm_record_write

DESCRIPTION
  This is stub function for pbm_record_write ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_record_write(
  pbm_device_type         device_type,
  uint16                 *rec_id_ptr,
  uint16                  cat,
  int                     num_fields,
  const uint8            *data_buf,
  int                     data_buf_size,
  PBM_WRITE_CB_FUNC       proc_func,
  void                   *user_data
)
{
  (void)device_type;
  (void)rec_id_ptr;
  (void)cat;
  (void)num_fields;
  (void)data_buf;
  (void)data_buf_size;
  (void)proc_func;
  (void)user_data;

  return PBM_ERROR;
} /* pbm_record_write */


/*===========================================================================
FUNCTION pbm_record_validate

DESCRIPTION
  This is stub function for pbm_record_validate ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_record_validate(
  pbm_device_type          device_type,
  uint16                   rec_id,
  pbm_cat_e_type           cat,
  uint8                   *data_buf,
  int                      data_buf_size,
  int                      num_fields
)
{
  (void)device_type;
  (void)rec_id;
  (void)cat;
  (void)data_buf;
  (void)data_buf_size;
  (void)num_fields;

  return PBM_ERROR;
} /* pbm_record_validate */


/*===========================================================================
FUNCTION pbm_record_validate

DESCRIPTION
  This is stub function for pbm_clear_phonebook ().

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_clear_phonebook(
  pbm_device_type  device_type
)
{
  (void)device_type;

  return PBM_ERROR;
} /* pbm_clear_phonebook */


/*===========================================================================
FUNCTION PBM_SESSION_EMERGENCY_NUMBER_CAT

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.  If it is, the
  category is returned in ecc_category_ptr.  If the number has no category, it is
  set to 0.

  Length does not include the trailing NULL.

DEPENDENCIES

RETURN VALUE
  Boolean

SIDE EFFECTS
  none
===========================================================================*/
boolean pbm_session_emergency_number_cat(
  pbm_session_enum_type  sess_type,
  const byte            *num, 
  byte                   len, 
  uint8                 *ecc_category_ptr
)
{
  return TRUE;
}


#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */