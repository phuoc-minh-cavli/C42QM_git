/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   S A P   R S P   S T U B   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the UIM response processing support for MMGSDI.

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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_sap_rsp_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/25/16   bcho    Initial version


===========================================================================*/


#include "uim_variation.h"
#include "comdef.h"
#include "mmgsdi.h"
#include "mmgsdi_sap_rsp.h"


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_connect

  DESCRIPTION:
    Response to connecting the UIM to the SAP.

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_connect(
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_sap_connect */


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_disconnect

  DESCRIPTION:
    Response to disconnect the UIM to the SAP.

  PARAMETERS:
    uim_rsp_ptr      : UIM response data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_disconnect(
  const mmgsdi_uim_report_rsp_type*   uim_rsp_ptr)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_sap_disconnect */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_SAP_RSP_BUILD_CNF

   DESCRIPTION:
     This function builds the SAP Confirmation and put the
     it onto the MMGSDI task queue.

   DEPENDENCIES:


   LIMITATIONS:


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

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_build_cnf(
  const mmgsdi_generic_rsp_type        *sap_rsp_ptr
)
{
  (void)sap_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_build_cnf */


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_send_apdu

  DESCRIPTION:
    The response to sending an APDU from the SAP to the UIM.

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.
    mmgsdi_cnf_ptr: Returned MMGSDI confirmation data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_send_apdu(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr,
  mmgsdi_protocol_enum_type             protocol)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_sap_send_apdu*/


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_power_on

  DESCRIPTION:
    The response to sending a SIM power on request for the SAP to the UIM.

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.
    mmgsdi_cnf_ptr: Returned MMGSDI confirmation data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_power_on(
  const mmgsdi_uim_report_rsp_type*    uim_rsp_ptr)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_power_on */


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_power_off

  DESCRIPTION:
    The response to sending a SIM power off request for the SAP to the UIM.

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.
    mmgsdi_cnf_ptr: Returned MMGSDI confirmation data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_power_off(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_sap_rsp_pwer_off */


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_sap_reset

  DESCRIPTION:
    The response to sending a SIM RESET request for the SAP to the UIM.

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.
    mmgsdi_cnf_ptr: Returned MMGSDI confirmation data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_sap_reset(
  const mmgsdi_uim_report_rsp_type*     uim_rsp_ptr)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_reset */


/*-----------------------------------------------------------------------------
  FUNCTION: mmgsdi_sap_rsp_build_sap_rsp

  DESCRIPTION:
    build and queue a sap response

  PARAMETERS:
    uim_rsp_ptr:    UIM response data.
    mmgsdi_cnf_ptr: Returned MMGSDI confirmation data.
-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_sap_rsp_build_sap_rsp (
  mmgsdi_return_enum_type     mmgsdi_status,
  int32                       index )
{
  (void)mmgsdi_status;
  (void)index;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_sap_rsp_build_sap_rsp */

