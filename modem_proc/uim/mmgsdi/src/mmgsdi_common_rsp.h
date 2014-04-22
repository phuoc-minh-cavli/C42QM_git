#ifndef MMGSDI_COMMON_RSP_H
#define MMGSDI_COMMON_RSP_H
/*===========================================================================


           M M G S D I   C O M M O N   R S P   H E A D E R S


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

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_common_rsp.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/15/17   nr      Add support to cache PIN1 of the SIM card when PSM mode enabled
05/03/16   tkl     F3 log prints cleanup
04/26/16   dd      Stub out onchip functionality for IOE flavour
03/07/16   ar      Remove ICC seek support
12/09/14   ar      Replace mmgsdi_u/icc_rsp_srv_available with common func
01/21/14   yt      Remove duplicate code
10/02/13   yt      Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"


/*===========================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_INCREASE

   DESCRIPTION:
     This function will build the response to INCREASE request

   DEPENDENCIES:


   LIMITATIONS:
     For UICC protocol access, mmgsdi_icc_rsp_increase should be used

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_increase (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_increase_cnf_type          ** mmgsdi_cnf_pptr,
  boolean                              synch,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_WRITE

   DESCRIPTION:
     This function will build the response to Write request

   DEPENDENCIES:


   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_write (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_write_cnf_type                ** mmgsdi_cnf_pptr,
  boolean                                 synch,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_COMPUTE_IP_AUTH

   DESCRIPTION:
     This function will build the response to a COMPUTE IP AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_compute_ip_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_RUN_CAVE

   DESCRIPTION:
     This function will build the response to a RUN CAVE command

   DEPENDENCIES:

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_run_cave (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_CDMA_AKA_AUTH

   DESCRIPTION:
     This function will build the response to a CDMA AKA AUTH command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_cdma_aka_auth (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_GENERATE_KEY_VPM

   DESCRIPTION:
     This function will build the response to a GENERATE KEY command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_generate_key_vpm (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_BLOCK

  DESCRIPTION:
    This function will build the response to OTASP Block Request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_block (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_OTAPA

  DESCRIPTION:
    This function will build the response to OTASP OTAPA Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_otapa (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_COMMIT

  DESCRIPTION:
    This function will build the response to OTASP COMMIT Request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_commit (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will build the response to OTASP SSPR Config Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_sspr_config (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_KEY_GEN

  DESCRIPTION:
    This function will build the response to OTASP Key Gen Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_key_gen (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_OTASP_MS_KEY

  DESCRIPTION:
    This function will build the response to OTASP MS Key Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_otasp_ms_key (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_BS_CHAL

  DESCRIPTION:
    This function will build the response to BS CHAL Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_bs_chal (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_SSD_CONFIRM

  DESCRIPTION:
    This function will build the response to SSD Confirm Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_ssd_confirm (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_SSD_UPDATE

  DESCRIPTION:
    This function will build the response to SSD Update Request

  DEPENDENCIES:

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS          : The command process is successful.
    MMGSDI_INCORRECT_PARAMS : The parameters supplied to the API are not
                              within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_ssd_update (
  const mmgsdi_uim_report_rsp_type   * uim_rsp_ptr,
  mmgsdi_protocol_enum_type            protocol
);

/* ============================================================================
  FUNCTION:      MMGSDI_COMMON_RSP_STORE_ESN

  DESCRIPTION:
    This function will build the response to Store ESN request

  DEPENDENCIES:
    The technology type has to be determined prior to using this function

  LIMITATIONS:

  RETURN VALUE:
    mmgsdi_return_enum_type

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_store_esn(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr,
  mmgsdi_session_store_esn_cnf_type    ** store_esn_cnf_pptr,
  boolean                                 synch,
  mmgsdi_protocol_enum_type               protocol
);

/* ============================================================================
   FUNCTION:      MMGSDI_COMMON_RSP_SRV_AVAILABLE

   DESCRIPTION:
     This function will build the response for the service available command

   DEPENDENCIES:

   LIMITATIONS:

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_rsp_srv_available(
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

/*===========================================================================
FUNCTION MMGSDI_COMMON_RSP_PIN_OP

DESCRIPTION
  This function, called in mmgsdi_process_response.
  It will determine which card technology the command requires (UICC or ICC),
  determine the command type, and call the appropriate pin operation
  functions.

DEPENDENCIES
  None

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_common_rsp_pin_op (
  const mmgsdi_uim_report_rsp_type  *uim_rsp_ptr,
  mmgsdi_protocol_enum_type          protocol,
  mmgsdi_pin_operation_cnf_type    **cnf_pptr,
  boolean                            sync_process
);

#endif /* MMGSDI_COMMON_RSP_H */
