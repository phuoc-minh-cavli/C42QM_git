/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   C N F   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains stub confirmation functions

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

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
05/02/16   ar      Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmgsdi.h"
#include "mmgsdi_cnf.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_COMPUTE_IP_AUTH_DATA

   DESCRIPTION:
     Stub function

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_compute_ip_auth_data(
  mmgsdi_compute_ip_auth_cnf_type   *compute_ip_auth_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *compute_ip_auth_rsp_data_ptr
)
{
  (void)compute_ip_auth_cnf_ptr;
  (void)mmgsdi_status;
  (void)compute_ip_auth_rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_compute_ip_auth_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_CAVE_DATA

   DESCRIPTION:
     Stub function

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_run_cave_data(
  mmgsdi_run_cave_cnf_type          *run_cave_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *run_cave_rsp_data_ptr
)
{
  (void)run_cave_cnf_ptr;
  (void)mmgsdi_status;
  (void)run_cave_rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_run_cave_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_CDMA_AKA_DATA

   DESCRIPTION:
     This function builds the CDMA AKA Authentication confirmation data

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_cdma_aka_data(
  mmgsdi_cdma_aka_cnf_type          *aka_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *aka_rsp_data_ptr)
{
  (void)aka_cnf_ptr;
  (void)mmgsdi_status;
  (void)aka_rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_cdma_aka_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GENERATE_KEY_VPM_DATA

   DESCRIPTION:
     This function builds the Generate Key confirmation data:  An 8-byte Key and
     the requested set of VPM (Voice Privacy Mask) Octets

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_generate_key_vpm_data(
  mmgsdi_generate_key_vpm_cnf_type  *generate_key_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *generate_key_rsp_data_ptr
)
{
  (void)generate_key_cnf_ptr;
  (void)mmgsdi_status;
  (void)generate_key_rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_generate_key_vpm_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_READ_PRL_DATA

   DESCRIPTION:
     This function builds the Cnf data for session read_prl command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_session_read_prl_data(
  mmgsdi_session_read_prl_cnf_type              *session_read_prl_cnf_ptr,
  mmgsdi_return_enum_type                       *mmgsdi_status_ptr,
  mmgsdi_session_id_type                         session_id
)
{
  (void)session_read_prl_cnf_ptr;
  (void)mmgsdi_status_ptr;
  (void)session_id;
  return MMGSDI_NOT_SUPPORTED;
}/*mmgsdi_cnf_build_session_read_prl_data*/

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_BS_CHAL_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for BS Chal request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_bs_chal_req_data(
  mmgsdi_session_bs_chal_cnf_type                * bs_chal_cnf_ptr,
  mmgsdi_return_enum_type                          mmgsdi_status,
  const mmgsdi_data_type                         * rsp_data_ptr
)
{
  (void)bs_chal_cnf_ptr;
  (void)mmgsdi_status;
  (void)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_bs_chal_req_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_MS_KEY_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP MS KEY request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_ms_key_req_data(
  mmgsdi_session_otasp_ms_key_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                               mmgsdi_status,
  const mmgsdi_data_type                              * rsp_data_ptr
)
{
  (void)otasp_cnf_ptr;
  (void)mmgsdi_status;
  (void)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_otasp_ms_key_req_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_KEY_GEN_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP KEY GEN request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_key_gen_req_data(
  mmgsdi_session_otasp_key_gen_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                                mmgsdi_status,
  const mmgsdi_data_type                               * rsp_data_ptr
)
{
  (void)otasp_cnf_ptr;
  (void)mmgsdi_status;
  (void)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_otasp_key_gen_req_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_SSPR_CONFIG_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP SSPR Config request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_sspr_config_req_data(
  mmgsdi_session_otasp_sspr_config_cnf_type            * otasp_cnf_ptr,
  mmgsdi_return_enum_type                                mmgsdi_status,
  const mmgsdi_data_type                               * rsp_data_ptr
)
{
  (void)otasp_cnf_ptr;
  (void)mmgsdi_status;
  (void)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_otasp_sspr_config_req_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_OTAPA_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTAPA request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_otapa_req_data(
  mmgsdi_session_otasp_otapa_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                              mmgsdi_status,
  const mmgsdi_data_type                             * rsp_data_ptr
)
{
  (void)otasp_cnf_ptr;
  (void)mmgsdi_status;
  (void)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_otasp_otapa_req_data */

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_OTASP_BLOCK_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for OTASP block request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     MMGSDI_NOT_SUPPORTED

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_block_req_data(
  mmgsdi_session_otasp_block_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                              mmgsdi_status,
  const mmgsdi_client_req_extra_info_type            * extra_param_ptr,
  const mmgsdi_data_type                             * rsp_data_ptr
)
{
  (void)otasp_cnf_ptr;
  (void)mmgsdi_status;
  (void)extra_param_ptr;
  (void)rsp_data_ptr;
  return MMGSDI_NOT_SUPPORTED;
} /* mmgsdi_cnf_build_otasp_block_req_data */
