#ifndef MMGSDI_SAP_H
#define MMGSDI_SAP_H
/*===========================================================================


           M M G S D I   U I M   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2005, 2008 - 2010, 2013, 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  .
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_sap.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/22/16   tkl     Stub out additional APIs
05/03/16   tkl     F3 log prints cleanup
04/25/16   bcho    Disable SAP server mode for IOE
04/15/13   tl      Remove SAP init, dereg and pin verify functionality
04/07/10   nb      SAP Enhancements
06/23/08   tml     Added featurization for BT in WM
09/10/07   tml     Used generic get ATR and get Card Reader functions
07/26/06   sun     Lint Fixes and Cleanup
07/07/06   sun     Lint Fixes
12/05/05   sun     Added support for MMGSDI_SAP_CARD_READER_STATUS_REQ and
                   MMGSDI_SAP_RESET_REQ
11/03/05   tml     Fixed header
10/20/05   sun     Added support for BT Deregisteration
08/26/05   sun     Added support for BT SAP
07/28/05   sst     Fixed compile errors when SAP feature is not defined
06/06/05   sst     Initial version

=============================================================================*/

#include "uim_variation.h"
#include "mmgsdi.h"

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
mmgsdi_return_enum_type mmgsdi_sap_sap_connect(
  const mmgsdi_sap_connect_req_type* req_ptr
);


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
mmgsdi_return_enum_type mmgsdi_sap_sap_disconnect(
  const mmgsdi_sap_disconnect_req_type* req_ptr
);


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
mmgsdi_return_enum_type mmgsdi_sap_sap_send_apdu(
  mmgsdi_sap_send_apdu_req_type* req_ptr,
  mmgsdi_protocol_enum_type      protocol
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_power_on

  DESCRIPTION:
    Sends a SIM power on request for the SAP to the UIM.

  PARAMETERS:
    req_ptr: The request data,
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_power_on(
  const mmgsdi_sap_power_on_req_type* req_ptr
);


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
mmgsdi_return_enum_type mmgsdi_sap_sap_power_off(
  const mmgsdi_sap_power_off_req_type* req_ptr
);


/*=============================================================================
  FUNCTION: mmgsdi_sap_sap_reset

  DESCRIPTION:
    Sends a SIM reset request for the SAP to the UIM.

  DEPENDENCIES
    None

  LIMITATIONS
    NONE

  RETURN VALUE
    mmgsdi_return_enum_type

  SIDE EFFECTS
    NONE
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_sap_sap_reset(
  const mmgsdi_sap_reset_req_type* req_ptr
);

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
  const mmgsdi_data_type                  * apdu_data_ptr);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_ATR_DATA

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
  mmgsdi_slot_id_enum_type            slot_id);

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
  mmgsdi_disconnect_mode_enum_type    disconnect_mode);

#endif /* MMGSDI_SAP_H */
