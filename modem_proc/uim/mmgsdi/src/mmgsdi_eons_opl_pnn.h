#ifndef MMGSDI_EONS_OPL_PNN_H
#define MMGSDI_EONS_OPL_PNN_H
/*===========================================================================


                  M M G S D I   E O N S   O P L   P N N   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2014, 2016 - 2017 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_eons_opl_pnn.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/26/17   bcho    Store EF-OPL and EF-PNN records to PSM cache
03/07/16   ar      Remove ICC seek support
12/03/14   bcho    featurize support to use stored rplmn name across operators
03/11/14   bcho    Don't run EONS algo if OPL-PNN not cached
01/16/14   bcho    Update PNN DATA CI MASK to check fourth byte in octet
07/15/13   vs      EONS updates to support single baseband SGLTE

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "cm.h"
#include "mmgsdi.h"
#include "mmgsdi_eons.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#define MMGSDI_EONS_PNN_FULL_NAME_IEI_POS       0
#define MMGSDI_EONS_PNN_FULL_NAME_IEI_VAL    0x43
#define MMGSDI_EONS_PNN_SHORT_NAME_IEI_VAL   0x45
#define MMGSDI_EONS_PNN_NAME_LEN_POS            1
#define MMGSDI_EONS_PNN_NAME_DATA_POS           2
#define MMGSDI_EONS_PNN_ADD_INFO_TAG_VAL     0x80
#define MMGSDI_EONS_PNN_ADD_INFO_CODING_POS     0
#define MMGSDI_EONS_PNN_ADD_INFO_LEN_POS        1
#define MMGSDI_EONS_PNN_ADD_INFO_DATA_POS       2
#define MMGSDI_EONS_PNN_DATA_ENCODING_MASK   0x70
#define MMGSDI_EONS_PNN_DATA_CI_MASK         0x08
#define MMGSDI_EONS_PNN_DATA_SPARE_BITS_MASK 0x07
#define MMGSDI_EONS_PNN_NAME_INFO_BYTE_POS      0
#define MMGSDI_EONS_PNN_NAME_BUF_POS            1
#define MMGSDI_EONS_PNN_ENC_GSM_DEF          0x00
#define MMGSDI_EONS_PNN_ENC_UCS2             0x10

#define MMGSDI_EONS_OPL_RECORD_PLMN_ID_POS      0
#define MMGSDI_EONS_LAC_START_UPPER_BYTE_POS    3
#define MMGSDI_EONS_LAC_START_LOWER_BYTE_POS    4
#define MMGSDI_EONS_LAC_END_UPPER_BYTE_POS      5
#define MMGSDI_EONS_LAC_END_LOWER_BYTE_POS      6
#define MMGSDI_EONS_PNN_RECORD_NUM_POS          7

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION MMGSDI_EONS_SEND_CACHE_READY_EVENT

DESCRIPTION
  Function to send EONS cache ready event

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_send_cache_ready_event(
  mmgsdi_session_id_type        session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_DELETE_FILE_CACHE

DESCRIPTION
  Function to delete OPL/PNN cache associated with GW PRI or GW SEC session

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_delete_file_cache(
  mmgsdi_session_id_type  session_id,
  mmgsdi_file_enum_type   file_enum
);

/*==========================================================================
FUNCTION MMGSDI_EONS_GET_OPL_PNN_ONS

DESCRIPTION
  Function to get operator name string for opl pnn lookup

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_opl_pnn_ons(
  mmgsdi_session_id_type             session_id,
  mmgsdi_session_type_enum_type      session_type,
  const mmgsdi_int_app_info_type   * app_info_ptr,
  mmgsdi_eons_call_stack_enum_type   stack_type,
  boolean                            is_plmn_rplmn,
  mmgsdi_plmn_id_type                hplmn_id,
  mmgsdi_plmn_info_type            * plmn_info_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_UPDATE_FILE_ATTR

DESCRIPTION
  Function to update file attribute in OPL-PNN cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_update_file_attr(
  mmgsdi_session_id_type        session_id,
  mmgsdi_file_enum_type         file_enum,
  mmgsdi_rec_num_type           num_of_records,
  uint32                        len_of_record,
  mmgsdi_return_enum_type       status
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_UPDATE_VALID_RECORD_INFO

DESCRIPTION
  Function to update valid record info in OPL cache

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_update_valid_record_info(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_data_type         invalid_record_data,
  mmgsdi_return_enum_type  status
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_FIRST_UNCACHED_VALID_RECORD_NUMBER

DESCRIPTION
  Function to get first uncached valid record number and record length while caching OPL-PNN.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_first_uncached_valid_record_number(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_rec_num_type     *next_valid_rec_num_ptr,
  mmgsdi_len_type         *next_valid_rec_len_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_NEXT_UNCACHED_VALID_RECORD_NUMBER

DESCRIPTION
  Function to get next valid record number and record length while caching OPL-PNN

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_next_uncached_valid_record_number(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  mmgsdi_rec_num_type      current_rec_number,
  mmgsdi_rec_num_type     *next_valid_rec_num_ptr,
  mmgsdi_len_type         *next_valid_rec_len_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_INCREASE_AND_GET_REF_COUNT

DESCRIPTION
  Function to increase cache ref count. cache ref count is incremented when caching starts.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_increase_and_get_ref_cnt(
  mmgsdi_session_id_type   session_id,
  mmgsdi_file_enum_type    file_enum,
  uint32                  *ref_cnt_ptr
);

/*==========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_CACHE_READY

DESCRIPTION
  Function to check when OPL PNN cache is ready or not. It returns TRUE if
  caching is completed or OPL-PNN is not found in card

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_opl_pnn_cache_ready (
  mmgsdi_session_type_enum_type      session_type
);

/*==========================================================================
FUNCTION: MMGSDI_EONS_COMPARE_WILDCARD_PLMN

DESCRIPTION
  Function to compare two PLMN IDs with considering wildcard
  (wildcard is specific to OPL record's PLMN)

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_compare_wildcard_plmn(
  mmgsdi_plmn_id_type opl_plmn_id,
  mmgsdi_plmn_id_type plmn_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_CACHE_OPL

DESCRIPTION
  Function intiates the caching of EF-OPL files. The valid records of the
  EF-OPL will be read and cached first and then the EF-PNN records will be
  read and cached later in the callback, after all the EF-OPL records have
  been cached, based on the PNN records referenced by valid OPL records.

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_cache_opl(
  mmgsdi_session_id_type      session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_SEARCH_UICC_OPL_INVALID_PATTERN

DESCRIPTION
  Function to search invalid pattern in EF-OPL to identify valid records

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_search_uicc_opl_invalid_pattern(
  mmgsdi_session_id_type session_id,
  mmgsdi_len_type        len_of_rec
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_GET_ATTR

DESCRIPTION
  Function to get file attr for an EF

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_opl_pnn_get_attr(
  mmgsdi_session_id_type session_id,
  mmgsdi_file_enum_type  file_enum,
  mmgsdi_rec_num_type  * rec_num_ptr,
  mmgsdi_len_type      * rec_len_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_PNN_UPDATE_VALID_RECORD_INFO

DESCRIPTION
  Function to update valid record info in PNN cache. Valid records will be
  determined from records identified by the cached EF-OPL.

DEPENDENCIES
  Valid EF-OPL files must be cached before calling this function.

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_pnn_update_valid_record_info(
  mmgsdi_session_id_type   session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_OPL_PNN_IS_VALID_RECORD_LIST_UPDATED

DESCRIPTION
  Function to return whether valid record list is updated or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None
===========================================================================*/
boolean mmgsdi_eons_opl_pnn_is_valid_record_list_updated(
  mmgsdi_session_id_type              session_id,
  mmgsdi_file_enum_type               file,
  uint8                              *valid_record_count_ptr,
  uint8                              *vaid_record_list_ptr
);

/*===========================================================================
FUNCTION MMGSDI_EONS_TRIGGER_OPL_PNN_CACHE

DESCRIPTION
  This function triggers caching of EF-OPL/EF-PNN records if not cached already
  from PSM EFS data.

PARAMETERS
  mmgsdi_session_id_type

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_trigger_opl_pnn_cache(
  mmgsdi_session_id_type session_id
);
#endif /* MMGSDI_EONS_OPL_PNN_H */
