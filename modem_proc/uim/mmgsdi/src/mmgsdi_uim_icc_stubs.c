/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   U I M    I C C   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ICC protocol processing support for MMGSDI
  before the command is being sent to UIM Module.

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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_uim_icc_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/16   bcho    Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uim_icc.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_SELECT

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will format the get file attribute request in the form required by
     the UIM.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.

     NOTE that this function must make sure to free req_ptr->int_client_data
     for every error return path.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only). This function is also limited to the use of EFs.


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_select (
  const mmgsdi_get_file_attr_req_type   *req_ptr,
  boolean                                sync_process,
  mmgsdi_get_file_attr_cnf_type        **cnf_pptr
)
{
  (void)req_ptr;
  (void)sync_process;
  (void)cnf_pptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_select */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_TRANSPARENT

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which packages the Write Transparent command and send it to UIM for
     further processing

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_transparent (
  const mmgsdi_write_req_type *  req_ptr,
  boolean                        sync_process,
  mmgsdi_write_cnf_type       ** cnf_pptr
)
{
  (void)req_ptr;
  (void)sync_process;
  (void)cnf_pptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_write_transparent */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_WRITE_RECORD

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which packages the Write Record command and send it to UIM for further
     processing.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_write_record should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_write_record (
  const mmgsdi_write_req_type *  req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_write_record */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_READ_TRANSPARENT

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which packages the Read Transparent command and send it to UIM for
     further processing.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_transparent (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_pptr
)
{
  (void)req_ptr;
  (void)sync_process;
  (void)cnf_pptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_read_transparent */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_READ_RECORD

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which packages the Read Record command and send it to UIM for further
     processing.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uicc_read_transparent should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_read_record (
  const mmgsdi_read_req_type *  req_ptr,
  boolean                       sync_process,
  mmgsdi_read_cnf_type       ** cnf_pptr
)
{
  (void)req_ptr;
  (void)sync_process;
  (void)cnf_pptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_read_record */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_INCREASE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which packages the Increase command and send it to UIM for further
     processing.

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_uim_uicc_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

-------------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_increase (
  const mmgsdi_increase_req_type *  req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_increase */


/*===========================================================================
   FUNCTION:  MMGSDI_UIM_ICC_SEND_APDU

   DESCRIPTION
     This is a stub function equivalent of the actual function by the same name
     which packages the SEND APDU command and send it to UIM for further
     processing.
   
   DEPENDENCIES
     None

   RETURN VALUE
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_send_apdu(
  mmgsdi_send_apdu_req_type* req_ptr)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_send_apdu */


/* =============================================================================
   FUNCTION:      MMGSDI_UIM_ICC_STATUS

   DESCRIPTION
     This is a stub function equivalent of the actual function by the same name
     which is called to queue the command to the UIM for a Status command.

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_uim_icc_status(
  const mmgsdi_status_req_type        *  req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_status */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_REHABILITATE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will rehabilitate a file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function

   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only).

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the rehabilitate
                              or invalidate the BDN file
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_rehabilitate (
  const mmgsdi_set_file_status_req_type  *req_ptr,
  boolean                                sync_process,
  mmgsdi_rehabilitate_cnf_type           **cnf_pptr
)
{
  (void)req_ptr;
  (void)sync_process;
  (void)cnf_pptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_rehabilitate */


/* ----------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_INVALIDATE

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which will invalidate a file.

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function

   LIMITATIONS:
     This function is limited to the use of ICC ie. (technologies of type GSM
     and CDMA only).

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The function was successful queue the invalidate
                              command
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
  None

-----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_invalidate (
  const mmgsdi_set_file_status_req_type *  req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_invalidate */


/* --------------------------------------------------------------------------
   FUNCTION:      MMGSDI_UIM_ICC_RUN_GSM_ALGO

   DESCRIPTION:
     This is a stub function equivalent of the actual function by the same name
     which packages the Run GSM algorithm command and send it to UIM for
     further processing.

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function can be used only in case of ICC Card

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS          : The command processing was successful.
     MMGSDI_ERROR            : The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                               within appropriate ranges.

   SIDE EFFECTS:
     None
----------------------------------------------------------------------------*/
mmgsdi_return_enum_type mmgsdi_uim_icc_run_gsm_algo (
  const mmgsdi_session_run_gsm_algo_req_type       * req_ptr
)
{
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
}/* mmgsdi_uim_icc_run_gsm_algo */

