/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 MMGSDI - SILENT PIN RELATED STUB FUNCTIONS


GENERAL DESCRIPTION

  This source file contains stub functions for managing and utilizing silent
  pin data.

                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Techologies, Inc (QTI) and its licensors.  All Rights
Reserved.  QUALCOMM Techologies Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_silent_pin_stubs.c#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/16   bcho    Initial version
===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "mmgsdi_silent_pin.h"


/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/


/*=============================================================================

                          FUNCTIONS

=============================================================================*/

/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_INIT()

  DESCRIPTION
    Initalizes the mmgsdi_silent_pin_info_table_ptr.

    This should only be called once when MMGSDI is initalizing
    all global variables.

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    This should only be called when MMGSDI is being initalized

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_silent_pin_init(
  void
)
{
  return;
} /* mmgsdi_silent_pin_init */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_PURGE_INFO()

  DESCRIPTION
    Purge the PIN information saved for silent PIN verification

  PARAMETERS
    slot_id : slot on which to purge pin info

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_silent_pin_purge_info(
  mmgsdi_slot_id_enum_type    slot_id
)
{
  (void)slot_id;
} /* mmgsdi_silent_pin_purge_info */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_CACHE_DATA()

  DESCRIPTION
    Save the PIN information to be used for silent PIN verification

  PARAMETERS
    slot_id : slot on which to save pin info

  RETURN VALUE
    mmgsdi_return_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_silent_pin_cache_data(
  mmgsdi_slot_id_enum_type    slot_id
)
{
  (void)slot_id;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_silent_pin_cache_data */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_MATCH_ICCID()

  DESCRIPTION
    Checks that the input ICCID data matches the data stored in the
    silent pin info table which is cached when an extended recovery
    results in a card reset.

  PARAMETERS
    iccid_data_ptr : pointer to iccid data
    slot_id : identifies the slot corresponding to the pin info cache

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_silent_pin_match_iccid(
  const mmgsdi_iccid_info_type     * iccid_data_ptr,
  mmgsdi_slot_id_enum_type           slot_id
)
{
  (void)iccid_data_ptr;
  (void)slot_id;
  return FALSE;
} /* mmgsdi_silent_pin_match_iccid */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_IS_CACHED()

  DESCRIPTION
    Returns whether the silent pin information is cached on the slot

  PARAMETERS
    slot_id : identifies the slot corresponding to the pin info cache

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_silent_pin_is_cached(
  mmgsdi_slot_id_enum_type           slot_id
)
{
  (void)slot_id;
  return FALSE;
} /* mmgsdi_silent_pin_is_cached */


/*===========================================================================
  FUNCTION MMGSDI_SILENT_PIN_VERIFY_PIN()

  DESCRIPTION
    This function will match the application AID with the AID stored
    in the silent PIN cache and if a match is found, send the PIN
    verification request with the cached PIN data.

  PARAMETERS
    session_id : id of the current session
    app_info_ptr : application information to match with the cached data
    slot_id : identifies the slot corresponding to the pin info cache

  RETURN VALUE
    boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean mmgsdi_silent_pin_verify_pin(
  mmgsdi_session_id_type             session_id,
  const mmgsdi_int_app_info_type    *app_info_ptr,
  mmgsdi_slot_id_enum_type           slot_id
)
{
  (void)session_id;
  (void)app_info_ptr;
  (void)slot_id;
  return FALSE;
} /* mmgsdi_silent_pin_verify_pin */


/*===========================================================================
  FUNCTION MMGSDI_CLEAR_SILENT_PIN_DATA()

  DESCRIPTION
    This function will clear the silent PIN info stored on the heap

  PARAMETERS
    None

  RETURN VALUE
    void

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void mmgsdi_clear_silent_pin_data(
  void
)
{
  return;
} /* mmgsdi_clear_silent_pin_data */

