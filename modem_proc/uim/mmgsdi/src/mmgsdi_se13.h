#ifndef MMGSDI_SE13_H
#define MMGSDI_SE13_H
/*===========================================================================


                    M M G S D I   S E 1 3   H E A D E R


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013-2016, 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_se13.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
07/09/18   tq      UIM interface optimization
06/22/16   tkl     Stub out additional APIs
07/14/14   tl      Remove always ON feature FEATURE_MODEM_RCINIT
04/03/14   am      Fix compile error on SBM due to assert_on_crit_sect_owner
10/28/13   kb      Add offtarget support to check mutex is owned or not
10/04/13   vv      Added API to return se13 table release date and source
08/30/13   av      Support for Chinese language PLMN names
08/09/13   kk      Deprecating uimcallctrl and moving task init functions
07/15/13   vs      EONS updates to support single baseband SGLTE
06/28/13   bcho    APIs added to return plmn info from SE13 table.
04/17/13   at      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"
#include "mmgsdi_eons.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION MMGSDI_SE13_INIT

DESCRIPTION
  Function called to initialize critical section for cached SE13 data.

DEPENDENCIES
  None

RETURN VALUE
  Void

SIDE EFFECTS
  None.  Function should only be called once during task initialization.
===========================================================================*/
void mmgsdi_se13_init
(
  void
);

/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_NUM_ENTRIES

DESCRIPTION:
  Fetches the number of entries in the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, and the read & sanity check of the file is successful, number of
    entries is determined from this file.
  If not, the number entries from the statically defined table is returned.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
uint16 mmgsdi_se13_table_get_num_entries
(
  void
);


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_MCC

DESCRIPTION:
  Fetches the MCC for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, MCC (of that index) is fetched from this file.
  If not, MCC (of that index) is fetched from the statically defined table.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
uint16 mmgsdi_se13_table_get_mcc
(
  uint16           index
);


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_MNC

DESCRIPTION:
  Fetches the MNC for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, MCC (of that index) is fetched from this file.
  If not, MCC (of that index) is fetched from the statically defined table.

RETURN VALUE
  UINT16

SIDE EFFECTS
  None
==========================================================================*/
uint16 mmgsdi_se13_table_get_mnc
(
  uint16           index
);


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_NETWORK_MASK

DESCRIPTION:
  Fetches the network type mask for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, network type mask (of that index) is fetched from this file.
  If not, network type mask (of that index) is fetched from the statically
    defined table.

RETURN VALUE
  UINT32

SIDE EFFECTS
  None
==========================================================================*/
uint32 mmgsdi_se13_table_get_network_mask
(
  uint16           index
);


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_IS_GSMA_ENTRY

DESCRIPTION:
  Fetches the flag that indicates if entry for the specified index is a
  GSMA entry.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, this function always returns TRUE since EFS entries are per GSMA.
  If not, GSMA entry flag (of that index) is fetched from the statically
    defined table.

RETURN VALUE
  BOOLEAN

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_se13_table_is_gsma_entry
(
  uint16           index
);


/*==========================================================================
FUNCTION: MMGSDI_SE13_TABLE_GET_ENTRY

DESCRIPTION:
  Fetches the entire PLMN entry for the specified index from the SE13 Table.
  First, the dynamic SE13 table file is checked to be present in te EFS:
  If yes, PLMN entry (of that index) is fetched from this file.
  If not, PLMN entry (of that index) is fetched from the statically
    defined table.

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_se13_table_get_entry
(
  uint16                                  index,
  mmgsdi_eons_network_info_data_type    * info_ptr
);

/*===========================================================================
FUNCTION MMGSDI_SE13_PROCESS_GET_SE13_PLMN_INFO_BY_NAME

DESCRIPTION
  This function queues a response for handling get se13 plmn info by name in the cnf

PARAMETERS:
  mmgsdi_get_se13_plmn_info_by_name_req_type req_ptr: request pointer

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS:
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_se13_process_get_se13_plmn_info_by_name(
  mmgsdi_get_se13_plmn_info_by_name_req_type * req_ptr
);

/*==========================================================================
FUNCTION MMGSDI_EONS_GET_SE13_TABLE_ONS

DESCRIPTION
  Function to get operator name string from the SE13 table

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_se13_table_ons(
  mmgsdi_plmn_info_type * plmn_info_ptr,
  mmgsdi_plmn_id_type     plmn_id,
  sys_sys_mode_e_type     sys_mode
);

/*==========================================================================
FUNCTION MMGSDI_EONS_GET_SE13_TABLE_DETAILS_INFO

DESCRIPTION
  Function to get the SE13 tabel release date and source

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_get_se13_table_details_info(
  mmgsdi_se13_table_details_info * se13_table_info_ptr
);

/*==========================================================================
FUNCTION MMGSDI_EONS_SYS_MODE_NETWORK_TYPE_MATCH

DESCRIPTION
  Function to compare sys mode with network types

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_eons_sys_mode_network_type_match(
  sys_sys_mode_e_type              sys_mode,
  uint32                           network_mask);

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */

/*===========================================================================
  FUNCTION:  MMGSDI_PROC_GET_SE13_PLMN_NAMES

  This function is used to get plmn name by directly accessing SE13 
  table for given PLMN Id

  @param[in]   plmn_id:             PLMN id
  @param[out]  plmn_name_info_ptr:  Contains PLMN name information.
                                    This info is valid when
                                    function return MMGSDI_SUCCESS
  @dependencies
  None 
  
  @limitations
  None

  @return
  MGSDI_SUCCESS:            Request is processed successfully
  MMGSDI_INCORRECT_PARAMS:  The parameters supplied to the API are not
                            within appropriate ranges.
  MGSDI_ERROR:              Return Error

  @side effects
  None
=========================================================================== */
mmgsdi_return_enum_type   mmgsdi_proc_get_se13_plmn_name
(
  mmgsdi_plmn_id_type                plmn_id,
  mmgsdi_plmn_info_static_type      *plmn_name_info_ptr
);

/* ============================================================================
FUNCTION       MMGSDI_EONS_GET_SE13_PLMN_INFO_BY_NAME_SYNC

DESCRIPTION
  This function returns plmn info from SE13 table for given long/short name

PARAMETERS:
  mmgsdi_data_type[in]                    plmn_long_name,
  mmgsdi_data_type[in]                    plmn_short_name,
  mmgsdi_rat_enum_type[in]                rat_type,
  mmgsdi_se13_plmn_static_info_type[out] *se13_plmn_info_ptr

DEPENDENCIES:
  None

RETURN VALUE:
  uint16        number of PLMN Ids 

SIDE EFFECTS:
  None
===============================================================================*/
uint16  mmgsdi_eons_get_se13_plmn_info_by_name_sync(  
  mmgsdi_data_type                   plmn_long_name,
  mmgsdi_data_type                   plmn_short_name,
  mmgsdi_rat_enum_type               rat_type,
  mmgsdi_se13_plmn_static_info_type *se13_plmn_info_ptr
);

#endif /* MMGSDI_SE13_H */
