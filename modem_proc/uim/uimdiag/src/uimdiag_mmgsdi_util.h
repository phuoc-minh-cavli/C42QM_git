#ifndef UIMDIAG_MMGSDI_UTIL_H
#define UIMDIAG_MMGSDI_UTIL_H

/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2009 - 2015, 2018, 2023 by QUALCOMM Technologies, Inc (QTI).
All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/uimdiag/src/uimdiag_mmgsdi_util.h#2 $$ $DateTime: 2023/03/08 09:28:28 $ $Author: pwbldsvc $

when       who   what, where, why
--------   ---   -----------------------------------------------------------
03/07/23   pvb   Added macro for max PLMN's in UIM diag cmd
07/25/18   bcho  Support UIMDIAG interface to only trigger NAA Refresh
05/13/15   stv   Add SPDI in uimdiag ef mapping table
09/14/14   kk    Remove unused uimdiag ef mapping entries
06/10/14   tl    Introduce EF 3GPDUPPExt
12/02/13   tkl   Support for SFI
06/27/13   spo   Added support for files under DF TELECOM
11/09/12   yt    Remove parsing of length field for PERSO_GET_DATA_CMD
09/25/12   tl    Adding nvruim_data_req_enum_type to sessionid_type
07/13/12   vv    Renamed CSGI to CSGL
12/23/11   bcho  uimdiag_file_to_mmgsdi_table modified
10/24/11   adp   Removing support for UIMDIAG_MMGSDI_SAP_PIN_VERIFY_CMD,
                 UIMDIAG_MMGSDI_SAP_INITIALIZE_CMD and
                 UIMDIAG_MMGSDI_SAP_DEREGISTER_CMD
10/10/11   adp   Adding uimdiag enum mapping
10/18/11   bcho  Adding support for mmgsdi_session_open_ext API
10/14/11   bcho  Added support for Get Operator Name API
10/10/11   nb    Fixed Incorrect featurization due to merge error
09/23/11   nb    Support for SAP CONNECT/SAP DISCONNECT
09/15/11   adp   Adding support to trigger refresh in uimdiag
09/15/11   nb    Support for new refresh stage
08/22/11   av    Added support for pdown,pup,activate onchip,send apdu commands
12/29/10   ms    Adding support for BT SAP APIs
12/03/10   adp   Adding support for a number of commands
09/07/10   adp   Adding Support for dck_num_retries,lock_down_enable_ltd_access,
                 seek and search commands
08/09/10   adp   Adding support for service enable, disable, get perso key,
                 is service available, perso unblock.
06/09/10   adp   Fixing Compiler Warnings
03/25/10   adp   Adding support for mmgsdi_session_deactivate_provisioning,
                 mmgsdi_session_get_app_capabilities,get_cphs_info
                 isim_authenticate, mmgsdi_send_card_status,
                 create_pkcs15_lookup_table
03/19/10   rk    Added support for JCDMA get info
02/22/10   adp   Adding support for Perso Commands
12/27/09   ap    Adding support for Refresh commands
12/03/09   rk    Adding support for get file attributes, read and write commands
09/02/09   adp   Initial Revision -Added initial mmgsdi session util functions


=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim.h"
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "diagpkt.h"
#include "uimdiag.h"
#include "uimdiag_util.h"


/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/
/* Maximum number of plmn in uimdiag command */
#define UIMDIAG_MAX_PLMN_IN_LIST  256

/*=============================================================================

                       FUNCTIONS DECLARATIONS

=============================================================================*/

/*===========================================================================

FUNCTION UIMDIAG_UTIL_CONVERT_FROM_MMGSDI_ITEMS_ENUMCMD

DESCRIPTION
  This function converts mmgsdi enums to uimdiag enums

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_file_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
uimdiag_file_enum_type uimdiag_util_convert_from_mmgsdi_items_enum (
  mmgsdi_file_enum_type mmgsdi_file
);

/*===========================================================================

FUNCTION UIMDIAG_UTIL_CONVERT_TO_MMGSDI_ITEMS_ENUMCMD

DESCRIPTION
  This function converts uimdiag enums to mmgsdi enums

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_file_enum_type

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_file_enum_type uimdiag_util_convert_to_mmgsdi_items_enum (
  uimdiag_file_enum_type uimdiag_mmgsdi_file
);

/*===========================================================================
FUNCTION: uimdiag_parse_cmd_data

DESCRIPTION:
  Populates the request structure with data from the Diag packet.

INPUTS:
  *req_ptr:      Generic_command struct request pointer
                 which will be populated with data from the diag_req_ptr
  *diag_req_ptr: pointer to raw Diag request packet
   diag_req_len: len of the diag request packet

DEPENDENCIES:
  None

RETURN VALUE:
   UIMDIAG_SUCCESS
   UIMDIAG_ERROR

OUTPUT:
  None
===========================================================================*/
uimdiag_return_enum_type uimdiag_parse_cmd_data (
  uimdiag_generic_req_type  *req_ptr,
  PACKED void               *diag_req_ptr,
  uint32                     diag_req_len
);

#endif /* UIMDIAG_MMGSDI_UTIL_H */
