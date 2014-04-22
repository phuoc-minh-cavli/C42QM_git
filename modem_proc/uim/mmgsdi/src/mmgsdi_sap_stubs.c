/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   S A P   S T U B   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UICC protocol processing support for MMGSDI.

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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_sap_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/22/16   tkl     Stub out additional APIs
04/25/16   bcho    Initial version

===========================================================================*/


#include "uim_variation.h"
#include "comdef.h"
#include "mmgsdi.h"
#include "mmgsdi_sap.h"


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_connect

  DESCRIPTION:
    Connects the UIM to the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_connect (
  const mmgsdi_sap_connect_req_type* req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_sap_connect */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_disconnect

  DESCRIPTION:
    Disconnects the UIM from the SAP.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_disconnect (
  const mmgsdi_sap_disconnect_req_type* req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_sap_disconnect */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_send_apdu

  DESCRIPTION:
    Sends an APDU from the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_send_apdu (
  mmgsdi_sap_send_apdu_req_type* req_ptr,
  mmgsdi_protocol_enum_type      protocol
)
{
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_sap_send_apdu */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_on

  DESCRIPTION:
    Sends a SIM power on request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_on (
  const mmgsdi_sap_power_on_req_type* req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_sap_power_on */


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_off

  DESCRIPTION:
    Sends a SIM power off request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_off (
  const mmgsdi_sap_power_off_req_type* req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_sap_power_off */

/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_reset

  DESCRIPTION:
    Sends a SIM RESET request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_reset (
  const mmgsdi_sap_reset_req_type* req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_sap_reset */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_SEND_APDU_DATA

   DESCRIPTION:
     This function builds the SAP Send APDU for Get Response confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_send_apdu_data(
  mmgsdi_sap_send_apdu_cnf_type           * sap_send_apdu_cnf_ptr,
  mmgsdi_return_enum_type                   mmgsdi_status,
  const mmgsdi_client_req_extra_info_type * extra_param_ptr,
  const mmgsdi_data_type                  * apdu_data_ptr)
{
  (void)sap_send_apdu_cnf_ptr;
  (void)mmgsdi_status;
  (void)extra_param_ptr;
  (void)apdu_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_sap_send_apdu_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_GET_ATR_DATA

   DESCRIPTION:
     This function builds the get ATR confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_get_atr_data(
  mmgsdi_sap_get_atr_cnf_type       * get_atr_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_slot_id_enum_type            slot_id)
{
  (void)get_atr_cnf_ptr;
  (void)mmgsdi_status;
  (void)slot_id;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_sap_get_atr_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SAP_DISCONNECT_DATA

   DESCRIPTION:
     This function builds the SAP Disconnection confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_sap_disconnect_data(
  mmgsdi_sap_disconnect_cnf_type    * disconnect_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  mmgsdi_disconnect_mode_enum_type    disconnect_mode)
{
  (void)disconnect_cnf_ptr;
  (void)mmgsdi_status;
  (void)disconnect_mode;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_sap_disconnect_data */
