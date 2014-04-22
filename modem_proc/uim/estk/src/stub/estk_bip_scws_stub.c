/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                      ESTK  BEARER INDEPENDENT PROTOCOL


GENERAL DESCRIPTION : Dummy file for ENHANCED STK layer SCWS BIP support

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/estk/src/stub/estk_bip_scws_stub.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------------------
09/15/16   gs      Changes in TP based on RAT and App Type
03/22/16   gm      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_msg.h"
#include "estk_bip.h"

#ifdef FEATURE_ESTK

/*===========================================================================
FUNCTION: estk_scws_send_data

DESCRIPTION:
  Dummy fucntion for send the SCWS Send Data req to the Agent

PARAMETERS:
  index     - Index into the bip ch table
  cmd_ref_id - Cmd ref id for terminal response
DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type:
    ESTK_SUCCESS         - Success
    ESTK_ERROR           - General Error
    ESTK_BAD_INPUT_PARAM - Bad Input parameters

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data(
  int32               index,
  uint32              cmd_ref_id
)
{
  (void) index;
  (void) cmd_ref_id;
  return ESTK_UNSUPPORTED;
}/* estk_scws_send_data  */

/*===========================================================================

FUNCTION: estk_process_scws_ch_status_req

DESCRIPTION :
  Dummy fucntion for processe Ch Status request from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_ch_status_req(
  const estk_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_process_scws_ch_status_req */

/*===========================================================================

FUNCTION: estk_process_scws_data_avail_req

DESCRIPTION :
  Dummy fucntion for processe Data Avail req from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_data_avail_req(
  estk_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_process_scws_data_avail_req */

/*===========================================================================

FUNCTION: estk_process_scws_open_ch_rsp

DESCRIPTION :
  Dummy fucntion for processe the Open Ch response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_open_ch_rsp(
  const estk_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_process_scws_open_ch_rsp */

/*===========================================================================

FUNCTION: estk_process_scws_close_ch_rsp

DESCRIPTION :
  Dummy fucntion for processe the Close Ch response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_close_ch_rsp(
  const estk_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_process_scws_close_ch_rsp */

/*===========================================================================

FUNCTION: estk_process_scws_send_data_rsp

DESCRIPTION :
  Dummy fucntion for processe the Send Data Response from the Agent

PARAMETERS:
  cmd_ptr : [Input] ESTK command pointer

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_process_scws_send_data_rsp(
  const estk_cmd_type *cmd_ptr)
{
  (void) cmd_ptr;
  return ESTK_UNSUPPORTED;
} /* estk_process_scws_send_data_rsp */

/*===========================================================================

FUNCTION: estk_scws_send_data_avail_rsp

DESCRIPTION :
  Dummy fucntion for send the data available response

PARAMETERS:
  index : [Input] Index to the estk channel table

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_send_data_avail_rsp (
  uint32 index, boolean result)
{
  (void) index;
  (void) result;
  return ESTK_UNSUPPORTED;
} /* estk_scws_send_data_avail_rsp */

/*===========================================================================

FUNCTION: estk_scws_clear_send_receive_buf

DESCRIPTION :
  Dummy fucntion for clear the Tx/Rx buffers

PARAMETERS:
   ch_id : Channel id

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_send_receive_buf(
   uint8 ch_id)
{
  (void) ch_id;
} /* estk_scws_clear_send_receive_buf */

/*===========================================================================

FUNCTION: estk_scws_clear_receive_data_allowed

DESCRIPTION :
  Dummy fucntion for set receive data allowed to true

PARAMETERS:
  ch_id   [Output]: Channel ID
  slot_id [Output]: Slot ID

DEPENDENCIES:
  None

RETURN VALUE:
  None

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void estk_scws_clear_receive_data_allowed(
   uint8                  ch_id,
   gstk_slot_id_enum_type slot_id)
{
  (void) ch_id;
  (void) slot_id;
} /* estk_scws_clear_receive_data_allowed */

/*===========================================================================

FUNCTION: estk_scws_populate_open_ch_table

DESCRIPTION :
  Dummy fucntion for populate information into the Open channel Table.

PARAMETERS:
  index : [Input] Index of the channel to be populated

DEPENDENCIES:
  None

RETURN VALUE:
  estk_result_enum_type
    ESTK_SUCCESS
    ESTK_ERROR

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_scws_populate_open_ch_table(int32 index)
{
  (void) index;
  return ESTK_UNSUPPORTED;
} /* estk_scws_populate_open_ch_table */

/*===========================================================================
FUNCTION: estk_check_scws_capability

DESCRIPTION :
  Checks the SCWS capability

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  FALSE

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
estk_result_enum_type estk_check_scws_capability(void)
{
  return FALSE;
} /* estk_check_scws_capability */

#endif /* FEATURE_ESTK */

