/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                    M M G S D I   I C C   R S P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC response processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 - 2017 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_icc_rsp_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/15/17   nr      Add support to cache PIN1 of the SIM card when PSM mode enabled
02/17/16   bcho    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "comdef.h"
#include "mmgsdi_icc_rsp.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_PARSE_UIM_RESPONSE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will parse write access to any file using the ICC protocol.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_rsp_parse_uim_response(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_parse_uim_response */


/* ----------------------------------------------------------------------------
  FUNCTION:      MMGSDI_ICC_RSP_EXTRACT_FILE_ATTR

  DESCRIPTION:
    This is a stub function equivalent of the actual function by the same name
     which is used to decode and map the Header information from the SIM
     format as define in GSM 11.11 as follows:

    byte 01:    rfu
    byte 02:    rfu
    byte 03:    file size MSB
    byte 04:    file size LSB
    byte 05:    file id MSB
    byte 06:    file id LSB
    byte 07:    type of file
              00 --> RFU
              01 --> MF
              02 --> DF
              04 --> EF
    byte 08:    RFU for Linear and Transparent Files
              bit7 = 1 Indicates Cyclic Increase allowed
    byte 09:    B8-B5 Read/Seek Permissions
              B4-B1 Update Permissions
    byte 10:    B8-B5 Increase Permissions
              B4-B1 RFU
    byte 11:    B8-B5 RehabilitatePermissions
              B4-B1 Invalidate Permissions
    byte 12:    File Status
              B8-B4 RFU
              B3 = 0 Not Readable/Writeable when invalidate
              B3 = 1 Readable/Writeable when invalidated
              B2 RFU
              B1 = 0 (Invalidated)
              B1 = 1 (Not Invalideated - Valid)
    byte 13:    Length of the following Information (Byte 14 to end]
    byte 14:    Structure of EF
              00 --> TRANSPARENT
              01 --> LINEAR FIXED
              03 --> CYCLIC
    byte 15:    Length of Record.  For cyclic and linear fixed, this denotes
              the length of the record.  For a transparent file it is 0x00.
    byte 16+:   (optional when defined)

    and it is populated into mmgsdi_file_attributes_type

  DEPENDENCIES:


  LIMITATIONS:


  RETURN VALUE:
    None

  SIDE EFFECTS:

-------------------------------------------------------------------------------*/
void mmgsdi_icc_rsp_extract_file_attr (
  mmgsdi_slot_id_enum_type       slot,
  uint8 *                        sim_header_attr,
  mmgsdi_len_type                sim_header_data_len,
  mmgsdi_file_attributes_type  * attr_ptr
)
{
  (void)slot;
  (void)sim_header_attr;
  (void)sim_header_data_len;
  (void)attr_ptr;
}/* mmgsdi_icc_rsp_extract_file_attr *//*lint !e715 */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_READ

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to Read request.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_read (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_read_cnf_type                 ** mmgsdi_cnf_pptr,
  boolean                                 synch
)
{
  (void)uim_rsp_ptr;
  (void)mmgsdi_cnf_pptr;
  (void)synch;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_read */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_GET_FILE_ATTR

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to Get File Attribute request.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function

   LIMITATIONS:
     This function is limited to the use of ICC

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_get_file_attr(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_get_file_attr_cnf_type        ** attr_cnf_pptr,
  boolean                                 synch
)
{
  (void)uim_rsp_ptr;
  (void)attr_cnf_pptr;
  (void)synch;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_get_file_attr */


/*----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_SEND_APDU

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to SEND_APDU request.

   LIMITATIONS:
     For ICC protocol access, mmgsdi_icc_rsp_send_apdu should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.
------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_icc_rsp_send_apdu(
  const mmgsdi_uim_report_rsp_type* uim_rsp_ptr)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_send_apdu*/ /*lint !e715 */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_STATUS

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to the Status Command.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_REHABILITATE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to Rehabilitate request.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_rehabilitate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_rehabitate (
  const mmgsdi_uim_report_rsp_type      *  uim_rsp_ptr,
  mmgsdi_rehabilitate_cnf_type          ** rehab_cnf_pptr,
  boolean                                  synch
)
{
  (void)uim_rsp_ptr;
  (void)rehab_cnf_pptr;
  (void)synch;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_rehabitate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_INVALIDATE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to Invalidate request.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_rsp_invalidate should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_invalidate (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_invalidate */


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_OPERATION

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to the PIN Operation Command.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_rsp_pin_operation should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_operation (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_pin_operation_cnf_type         **pin_operation_cnf_pptr,
  boolean                                 sync_process
)
{
  (void)uim_rsp_ptr;
  (void)pin_operation_cnf_pptr;
  (void)sync_process;

  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_pin_operation*/


/* ============================================================================
   FUNCTION:      MMGSDI_ICC_RSP_PIN_STATUS

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to the PIN Status Command.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol status, mmgsdi_uicc_pin_status should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_pin_status (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_icc_rsp_pin_status*/


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_ICC_RSP_RUN_GSM_ALGO

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will build the response to RUN GSM algorithm.

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function is limited to the use of CDMA ICC cards

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command structure was properly generated
                               and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type  mmgsdi_icc_rsp_run_gsm_algo (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr
)
{
  (void)uim_rsp_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_icc_rsp_run_gsm_algo */

