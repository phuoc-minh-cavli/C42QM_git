#ifndef SIMLOCK_CATEGORY_H
#define SIMLOCK_CATEGORY_H
/*===========================================================================


            S I M L O C K   C A T E G O R Y   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2014 - 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/simlock/src/simlock_category.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/01/15   bcho    Support for whitelist and blacklist codes in same category
05/04/15   vv      Support for more than 1 bytes of GID1/GID2
04/14/14   vv      Added support for identifying conflicting codes
02/14/14   vv      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef FEATURE_SIMLOCK

#include "simlock.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define SIMLOCK_CATEGORY_FILE_HEADER_SIZE      64

/* ----------------------------------------------------------------------------
   ENUM:      SIMLOCK_CATEGORY_FILE_VERSION_ENUM_TYPE

   DESCRIPTION:
     This enum specifies the version of the category file supported
-------------------------------------------------------------------------------*/
typedef enum {
  SIMLOCK_CATEGORY_FILE_VERSION_0 = 0,
  SIMLOCK_CATEGORY_FILE_VERSION_1 = 1,
  SIMLOCK_CATEGORY_FILE_VERSION_2 = 2
} simlock_category_file_version_enum_type;

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_FREE_CODE_DATA

DESCRIPTION
  This function frees the code data in the category file data

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  void

SIDE EFFECTS
  NONE
===========================================================================*/
void simlock_category_free_code_data
(
  simlock_category_file_data_type  * category_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_READ_LOCK_DATA

DESCRIPTION
  This function reads the lock data for a category and provides the data in
  a packed structure. It allocates memory based on the number of lock codes.
  The caller of this function is responsible to free that memory.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_read_lock_data
(
  simlock_category_enum_type         category,
  simlock_slot_enum_type             slot,
  simlock_category_file_data_type *  out_category_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_WRITE_LOCK_DATA

DESCRIPTION
  This function writes the lock data for a category. It takes the lock data
  in a packed structure format and constructs a data buffer.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_write_lock_data
(
  simlock_slot_enum_type             slot,
  simlock_category_file_data_type  * category_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_BUILD_LOCK_DATA

DESCRIPTION
  This function builds the lock data for a category from the request.It 
  allocates memory based on the number of lock codes. The caller of this
  function is responsible to free that memory.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_build_lock_data
(
  const simlock_set_lock_ck_msg_req_type   *  msg_type_ptr,
  const simlock_salt_type                     salt,
  const simlock_hck_type                      hck,
  simlock_category_file_data_type          *  category_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_IS_WHITELIST

DESCRIPTION
  This function checks if the lock codes for a category are considered as
  whitelist

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
boolean simlock_category_is_whitelist
(
  simlock_category_header_data_type  * header_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_UPDATE_WRITE_LOCK_DATA

DESCRIPTION
  If the auto_lock feature is enabled for a category during the lock 
  activation, the codes are updated later. This function updates the lock data
  for such categories.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_update_and_write_lock_data
(
  simlock_category_header_data_type  * header_data_ptr,
  simlock_category_code_data_type    * code_data_ptr,
  simlock_slot_enum_type               slot
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_GET_CODE_DATA

DESCRIPTION
  This function gets the lock data for a category

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_get_code_data
(
  simlock_slot_enum_type         slot,
  simlock_category_enum_type     category_type,
  simlock_get_category_data_msg_resp_type   * resp_data_ptr
);

/*===========================================================================
FUNCTION SIMLOCK_CATEGORY_APPEND_LOCK_DATA

DESCRIPTION
  This function appends the lock data for a category from the request.It
  allocates memory based on the total number of lock codes. The caller of
  this function is responsible to free that memory.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  simlock_result_enum_type

SIDE EFFECTS
  NONE
===========================================================================*/
simlock_result_enum_type simlock_category_append_lock_data
(
  const simlock_category_data_type      *  new_code_data_ptr,
  const boolean                            blacklist,
  simlock_category_file_data_type       *  category_data_ptr
);

#endif /* FEATURE_SIMLOCK */

#endif /* SIMLOCK_CATEGORY_H */

