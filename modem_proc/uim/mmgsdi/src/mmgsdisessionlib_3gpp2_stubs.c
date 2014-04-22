/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        M M G S D I  S E S S I O N  L I B R A R Y


GENERAL DESCRIPTION

  This file contains stubs for 3GPP2 Library function

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/21/16   ar      Moving compute_ip_auth handler to 3GPP2 specific file
05/02/16   ar      Initial Version

=============================================================================*/

/*=============================================================================

                         INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"

/*=============================================================================

                                  FUNCTIONS

=============================================================================*/

/*===========================================================================
FUNCTION        MMGSDI_SESSION_RUN_CAVE_ALGO

DESCRIPTION
  Stub function

PARAMETERS
  session_id:        Session ID of the caller
  rand_type:         Type of Random Challenge
  rand_chal:         Rand Chal for CAVE Algo
  dig_len:           Length of digits provided
  digits:            Digits for CAVE calculation
  process_control:   Process Control for CAVE
  esn:               ESN for CAVE Calculation
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  None

LIMITATIONS

RETURN VALUE
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_run_cave_algo (
  mmgsdi_session_id_type             session_id,
  mmgsdi_cave_rand_type_type         rand_type,
  mmgsdi_cave_rand_chal_type         rand_chal,
  mmgsdi_cave_dig_len_type           dig_len,
  mmgsdi_cave_digits_type            digits,
  mmgsdi_cave_process_control_type   process_control,
  mmgsdi_cave_esn_type               esn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  (void)session_id;
  (void)rand_type;
  (void)rand_chal;
  (void)dig_len;
  (void)digits;
  (void)process_control;
  (void)esn;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_run_cave_algo */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_CDMA_AKA_AUTHENTICATE

DESCRIPTION
  Stub function

PARAMETERS
  session_id:        Session ID of the caller
  aka_cmd:           Specifies the AKA command
  randa:             Random Anonymity Key
  autn:              Authentication Token
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  None

LIMITATIONS

RETURN VALUE
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_cdma_aka_authenticate(
  mmgsdi_session_id_type             session_id,
  mmgsdi_cdma_aka_cmd_enum_type      aka_cmd,
  mmgsdi_cdma_aka_randa_type         randa,
  mmgsdi_data_type                   autn,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  (void)session_id;
  (void)aka_cmd;
  (void)randa;
  (void)autn;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_cdma_aka_authenticate */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_GENERATE_KEY_VPM

DESCRIPTION
  Stub function

PARAMETERS
  session_id:        Session ID of the caller
  first_octet:       First Octet of VPM to be output
  last_octet:        Last Octet of VPM to be output
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES
  None

LIMITATIONS

RETURN VALUE
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_generate_key_vpm (
  mmgsdi_session_id_type             session_id,
  mmgsdi_single_vpm_octet_type       first_octet,
  mmgsdi_single_vpm_octet_type       last_octet,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  (void)session_id;
  (void)first_octet;
  (void)last_octet;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_generate_key_vpm */


/*================================================================
FUNCTION:      MMGSDI_SESSION_SSD_UPDATE

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  randssd        : 56 bits from BS for SSD Generation
  process_control: process control bits
  esn:           : esn
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_ssd_update(
  mmgsdi_session_id_type               session_id,
  qword                                randssd,
  byte                                 process_control,
  dword                                esn,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  (void)session_id;
  (void)randssd;
  (void)process_control;
  (void)esn;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_ssd_update */


/*================================================================
FUNCTION:      MMGSDI_SESSION_CONFIRM_SSD

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  authbs         : Auth Signature from Base Station
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_confirm_ssd(
  mmgsdi_session_id_type               session_id,
  dword                                authbs,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  (void)session_id;
  (void)authbs;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_confirm_ssd */


/*================================================================
FUNCTION:      MMGSDI_SESSION_BS_CHAL

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  randseed       : Random number generator seed
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_bs_chal(
  mmgsdi_session_id_type               session_id,
  dword                                randseed,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  (void)session_id;
  (void)randseed;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_bs_chal */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_MS_KEY_REQ

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  randseed       : Seed used to generate TRUE random no
  a_key_p_rev    : A-KEY-P_REV supported by BS
  param_p        : Param P
  param_g        : Param G
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_ms_key_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_data_type                      randseed,
  byte                                  a_key_p_rev,
  mmgsdi_data_type                      param_p,
  mmgsdi_data_type                      param_g,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  (void)session_id;
  (void)randseed;
  (void)a_key_p_rev;
  (void)param_p;
  (void)param_g;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_otasp_ms_key_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_KEY_GEN_REQ

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  bs_result      : BS Result
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_key_gen_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_data_type                      bs_result,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  (void)session_id;
  (void)bs_result;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_otasp_key_gen_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_SSPR_CONFIG_REQ

DESCRIPTION:
  TStub function

PARAMETERS:
  session_id     : Session Id of the caller
  block_id       : Block ID of the block
  req_offset     : Requested offset in the PRL
  req_max_size   : Requested Max size in the response
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_sspr_config_req(
  mmgsdi_session_id_type                session_id,
  byte                                  block_id,
  word                                  req_offset,
  byte                                  req_max_size,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  (void)session_id;
  (void)block_id;
  (void)req_offset;
  (void)req_max_size;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_otasp_sspr_config_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_BLOCK_REQ

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id           : Session Id of the caller
  otasp_block_op_type  : Config, Download, SSPR download, or Validate
  block_id             : Block ID for request
  block_data           : Block DATA (only for download, sspr download, or validate)
  response_cb_ptr      : Call back function when execution is done.
  client_ref           : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_block_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_otasp_block_op_enum_type       otasp_block_op_type,
  byte                                  block_id,
  mmgsdi_data_type                      block_data,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  (void)session_id;
  (void)otasp_block_op_type;
  (void)block_id;
  (void)block_data;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_otasp_block_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_COMMIT_REQ

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_commit_req(
  mmgsdi_session_id_type                session_id,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  (void)session_id;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_otasp_commit_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_OTASP_OTAPA_REQ

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller
  start_stop     : OTAPA Start Stop
  randseed       : Random seed
  esn            : ESN or Pseudo ESN
  response_cb_ptr: Call back function when the command execution is done.
  client_ref     : User Data returned upon completion of this cmd.

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  mmgsdi_return_enum_type:
    MMGSDI_SUCCESS:          The command structure was properly
                             Generated and queued onto the MMGSDI
                             Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API
                             are not within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the
                             MMGSDI Task because the max number of
                             commands are already queued.

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_otasp_otapa_req(
  mmgsdi_session_id_type                session_id,
  byte                                  start_stop,
  dword                                 randseed,
  dword                                 esn,
  mmgsdi_callback_type                  response_cb_ptr,
  mmgsdi_client_data_type               client_ref
)
{
  (void)session_id;
  (void)start_stop;
  (void)randseed;
  (void)esn;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_otasp_otapa_req */


/*================================================================
FUNCTION:      MMGSDI_SESSION_READ_PRL

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id     : Session Id of the caller

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_read_prl (
  mmgsdi_session_id_type               session_id,
  mmgsdi_callback_type                 response_cb_ptr,
  mmgsdi_client_data_type              client_ref
)
{
  (void)session_id;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_session_read_prl*/


/*================================================================
FUNCTION:      MMGSDI_SESSION_GET_NVRUIM_DATA_SYNC

DESCRIPTION:
  Stub function

PARAMETERS:
  session_id      : Session Id of the caller
  nvruim_data_ptr:  returned data

DEPENDENCIES:
  None

LIMITATIONS:
  None

RETURN VALUE:
  MMGSDI_NOT_SUPPORTED

SIDE EFFECTS:
  None
================================================================*/
mmgsdi_return_enum_type mmgsdi_session_get_nvruim_data_sync(
  mmgsdi_session_id_type               session_id,
  mmgsdi_nvruim_data_type             *nvruim_data_ptr
)
{
  (void)session_id;
  (void)nvruim_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_session_get_nvruim_data_sync */


/*===========================================================================
FUNCTION        MMGSDI_SESSION_COMPUTE_IP_AUTH

DESCRIPTION
  Stub function

PARAMETERS
  session_id:        Session ID of the caller
  compute_ip_data:   Data required for Compute IP Command
  response_cb_ptr:   Response callback.
  client_ref:        User data returned upon completion of this cmd.

DEPENDENCIES

LIMITATIONS

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_session_compute_ip_auth (
  mmgsdi_session_id_type             session_id,
  mmgsdi_compute_ip_data_type        compute_ip_data,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  (void)session_id;
  (void)compute_ip_data;
  (void)response_cb_ptr;
  (void)client_ref;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_session_compute_ip_auth */

