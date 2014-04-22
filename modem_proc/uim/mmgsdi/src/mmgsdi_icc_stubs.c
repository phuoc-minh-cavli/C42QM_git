/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016, 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_icc_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
07/27/18   tq      Moving function to stub
02/17/16   bcho    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_icc.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_READ

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will provide read access to any file using the ICC protocol.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
     MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                              but the service table indicates the card does not
                              have the support

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_read (
  const mmgsdi_read_req_type      * req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_read */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_STATUS

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will send a status command to card.  It will stay compliant to
    GSM 11.11 or IS820C.

    TThis function returns information concerning the current directory.
    A current EF is not affected by the STATUS function. It is also used
    to give an opportunity for a pro active SIM to indicate that the
    SIM wants to issue a SIM Application Toolkit command to the ME.

    The information is provided asynchronously in the response cnf.
    Which will contain information containing:  file ID, total memory space
    available, CHV enabled/disabled indicator, CHV status and other
    GSM/CDMA specific data (identical to SELECT).

  DEPENDENCIES:


  LIMITATIONS:
    For ICC protocol access, mmgsdi_icc_status should be used.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_status (
  mmgsdi_status_req_type      * req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_status */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_ENABLE_SERVICE

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will enable a service by doing a rehabiliate or invalidate to UIM.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_enable_service (
  const mmgsdi_session_enable_service_req_type *req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_process_enable_service */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_PROCESS_DISABLE_SERVICE

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will disable a service by doing a rehabiliate or
    invalidate to UIM.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using
    this function

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_process_disable_service (
  const mmgsdi_session_disable_service_req_type *req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_process_disable_service */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_SET_FILE_STATUS

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will rehabilitate or invalidate the EF.


  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function


  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type GSM
    and CDMA only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_set_file_status (
  const mmgsdi_set_file_status_req_type   *req_ptr,
  mmgsdi_cmd_enum_type                     req_type
)
{
  (void)req_ptr;
  (void)req_type;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_set_file_status */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_IS_DISABLE_CHV1_ALLOWED

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will check if disable CHV1 is allowed or not in the Service Table.

  DEPENDENCIES:
    The technology type (GSM/CDMA/UICC) has to be determined prior to using this
    function

  LIMITATIONS:
    This function is limited to the use of ICC

  RETURN VALUE:
    TRUE/FALSE

  SIDE EFFECTS:
     None

-----------------------------------------------------------------------------*/
boolean  mmgsdi_icc_is_disable_chv1_allowed (
  mmgsdi_session_id_type  session_id
)
{
  (void)session_id;
  return FALSE;
}/* mmgsdi_icc_is_disable_chv1_allowed */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_PIN_STATUS

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will get the Pin Status from the cache or send the command to the
    card if the PIN Status in not available.

  DEPENDENCIES:


  LIMITATIONS:
    For UICC protocol access, mmgsdi_uicc_pin_status should be used.

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_NOT_SUPPORTED:    When the file has an entry in the Service Table
                             but the service table indicates the card does not
                             have the support

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_pin_status (
  const mmgsdi_get_all_pin_status_req_type      * req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_pin_status */


/* ============================================================================
  FUNCTION:      MMGSDI_ICC_CHK_SRV_AVAILABLE

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which checks the SRV AVAILABLE BIT in GSM/RUIM.

  DEPENDENCIES:


  LIMITATIONS:
    There is no corresponding EST bit in SIM/RUIM

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_chk_srv_available(
  mmgsdi_session_id_type    session_id,
  mmgsdi_chk_srv_enum_type  check_type,
  mmgsdi_service_enum_type  srv_type,
  boolean                  *srv_available_ptr)
{
  (void)session_id;
  (void)check_type;
  (void)srv_type;
  (void)srv_available_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/*mmgsdi_icc_chk_srv_available*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_INIT_REHAB_FILE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which checks if passed in file is invalidated. If so, it will rehabilitate
     the file.

   DEPENDENCIES:
     Should only be called if FDN is enabled and should only be called for
     initialization processing.

   LIMITATIONS:


   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     Input file is re-habilitated conditionally

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_init_rehab_file(
  boolean                    skip_get_file_attr,
  mmgsdi_session_id_type     session_id,
  mmgsdi_app_enum_type       app_type,
  mmgsdi_file_enum_type      sim_filename,
  mmgsdi_slot_id_enum_type   slot
)
{
  (void)skip_get_file_attr;
  (void)session_id;
  (void)app_type;
  (void)sim_filename;
  (void)slot;  
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_init_rehab_file */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GET_SIM_CAPABILITIES

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will provide capabilities on fdn, bdn, acl and imsi rehabilitation
    procedure for SIM.

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When FEATURE_MMGSDI_GSM is not defined

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_get_sim_capabilities (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot
)
{
  (void)session_id;
  (void)slot;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_icc_get_sim_capabilities */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_GET_RUIM_CAPABILITIES

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will provide capabilities on fdn, bdn, acl and imsi rehabilitation
    procedure for RUIM.

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS          : The command processing was successful.
    MMGSDI_ERROR            : The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.
    MMGSDI_NOT_SUPPORTED    : When FEATURE_MMGSDI_3GPP2 is not defined

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_get_ruim_capabilities (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot
)
{
  (void)session_id;
  (void)slot;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_icc_get_ruim_capabilities */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_RUN_GSM_ALGO

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will allow RUN GSM algorithm command to be sent to UIM. In case the
    onchip is activated, the onchip algorithm is executed. 

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC ie. (technologies of type ICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type   *req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_icc_run_gsm_algo */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_IS_DCS1800_DF_PRESENT

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which checks if the DCS 1800 DF is present on the card. The DCS 1800 NV
    must be checked before calling this funciton.

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of ICC protocol

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_is_dcs1800_df_present (
  mmgsdi_session_id_type   session_id,
  mmgsdi_slot_id_enum_type slot_id
)
{
  (void)session_id;
  (void)slot_id;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_is_dcs1800_df_present */


/*===========================================================================
  FUNCTION:      MMGSDI_ICC_UPDATE_FDN_STATUS

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
    which will update capabilities on fdn procedure for ICC based on ADN
    activation status.

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_icc_update_fdn_status (
  mmgsdi_session_id_type      session_id,
  mmgsdi_slot_id_enum_type    slot
)
{
  (void)session_id;
  (void)slot;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_icc_update_fdn_status */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_GSM_ALGO_DATA

   DESCRIPTION:
     This function builds the RUN GSM algorithm confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_NOT_SUPPORTED:    This command not supported

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_run_gsm_algo_data(
  mmgsdi_session_run_gsm_algo_cnf_type *run_gsm_algo_cnf_ptr,
  mmgsdi_return_enum_type               mmgsdi_status,
  const mmgsdi_data_type               *rsp_data_ptr)
{
  (void*)run_gsm_algo_cnf_ptr;
  (void)mmgsdi_status;
  (void*)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_cnf_build_run_gsm_algo_data */
