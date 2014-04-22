/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


 P H O N E B O O K  C A C H I N G   S T U B   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the PB caching supporting functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/4.0/mmgsdi/src/mmgsdi_pb_cache_stub.c

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/18   dd      Fixing MOB compilation error
06/20/16   nr      PB cache stub functions for IoE

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdiutil.h"
#include "mmgsdi_pb_cache.h"

/* MMGSDI Globals to store the PB cache data */
mmgsdi_pb_cache_type *mmgsdi_pb_cache_tbl_ptr[MMGSDI_MAX_NUM_SLOTS];

/*===========================================================================
FUNCTION MMGSDI_PB_FREE_CACHE_DATA

DESCRIPTION
  This function contains stub for mmgsdi_pb_free_cache_data

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_cache_data(
  mmgsdi_slot_id_enum_type      slot_id,
  mmgsdi_session_id_type        session_id,
  boolean                       delete_all_sessions
)
{
  (void)slot_id;
  (void)session_id;
  (void)delete_all_sessions;
} /* mmgsdi_pb_free_cache_data */


/*==========================================================================
FUNCTION MMGSDI_PB_CACHE_RECORDS

DESCRIPTION
  This function contains stub for mmgsdi_pb_cache_records

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_pb_cache_records(
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_slot_id_enum_type   slot_id,
  mmgsdi_rec_num_type        rec_num,
  mmgsdi_session_id_type     session_id
)
{
  (void)file_access_ptr;
  (void)slot_id;
  (void)rec_num;
  (void)session_id;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_pb_cache_records */


/*===========================================================================
FUNCTION MMGSDI_PB_CACHE_GET_RECORD

DESCRIPTION
  This function contains stub for mmgsdi_pb_cache_get_record

DEPENDENCIES
  None

RETURNS
  mmgsdi_return_enum_type

SIDE EFFECTS
  None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_pb_cache_get_record(
  mmgsdi_slot_id_enum_type   slot,
  mmgsdi_rec_num_type        rec_num,
  const mmgsdi_access_type  *file_access_ptr,
  mmgsdi_data_type          *record_data_holder,
  mmgsdi_session_id_type     session_id
)
{
  (void)slot;
  (void)rec_num;
  (void)file_access_ptr;
  (void)record_data_holder;
  (void)session_id;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_pb_cache_get_record */


/*===========================================================================
FUNCTION MMGSDI_PB_INIT_CACHE_DATA

DESCRIPTION
  This function contains stub for mmgsdi_pb_init_cache_data

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_init_cache_data(
  void
)
{
} /* mmgsdi_pb_init_cache_data */


/*===========================================================================
FUNCTION MMGSDI_PB_FREE_FCN_CACHE

DESCRIPTION
  This function contains stub for mmgsdi_pb_free_fcn_cache

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_fcn_cache(
  uint8                                          slot_index,
  const mmgsdi_internal_refresh_file_list_type  *refresh_files_ptr,
  mmgsdi_session_id_type                         session_id
)
{
  (void)slot_index;
  (void)refresh_files_ptr;
  (void)session_id;
}/* mmgsdi_pb_free_fcn_cache */


/*===========================================================================
FUNCTION MMGSDI_PB_FREE_EF_CACHE

DESCRIPTION
  This is stub function for mmgsdi_pb_free_ef_cache()

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void mmgsdi_pb_free_ef_cache(
  mmgsdi_path_type                  path_type,
  mmgsdi_session_id_type            session_id,
  mmgsdi_slot_id_enum_type          slot
)
{
  (void)path_type;
  (void)session_id;
  (void)slot;

  return;
}/* mmgsdi_pb_free_ef_cache */

