/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                M M G S D I   I C C / U I C C  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the command processing support for MMGSDI.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2013 - 2017 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_common.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
03/15/17   nr      Add support to cache PIN1 of the SIM card when PSM mode enabled
06/19/16   bcho    F3 frequency reduction by conditional logic
05/25/16   vdc     Remove F3 messages for memory allocation failure
05/24/16   ar      Review of macros used by MMGSDI
05/03/16   tkl     F3 log prints cleanup
04/26/16   dd      Stub out onchip functionality for IOE flavour
04/13/16   ar      Remove duplicates from mmgsdi_util_print_file_information()
03/08/16   na      Reduce duplicate code for PIN operations
03/07/16   ar      Remove ICC seek support
09/08/15   bcho    Run SIMLOCK Algo in Onchip SIM 
05/13/15   stv     Check cache status while handling MMGSDI_GET_FILE_ATTR_REQ
11/14/14   hh      Fix compilation warnings
10/27/14   hh      Fix KW error 
10/15/14   kk      Fixed onchip config for USIM EF KC
09/29/14   tkl     Added support for persistent cache
08/11/14   bcho    Optimize srv req by using cached srv table in client context
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
06/30/14   vv      Remove Perso code
06/18/14   av      Fix out of bound access in mmgsdi_util_find_app_pin_index
06/03/14   tl      Remove additional parameter variables
05/27/14   av      Don't allow access if UST/CST can't be read(pin not verified)
04/02/14   hh      Correct range check in mmgsdi_common_activate_onchip
03/27/14   yt      Ensure usage of correct slot id
03/18/14   tl      Introduce new SIM Lock feature
02/25/14   hh      Purge temporary mobile identities during UICC Reset Refresh
02/20/14   ak      Fix compile errors due to strict compiler on 9x35
12/05/13   tkl     SFI Support and decouple uim file enums
01/24/14   yt      Correctly initialize additional parameters
01/21/14   yt      Remove duplicate code
01/11/14   df      Fix off-target compilation error
01/10/14   df      Use v2.0 diag macros
12/16/13   tl      Prevent initalizing the cache with partial files
11/06/13   yt      Fixes for multi-SIM OnChip activation
10/03/13   yt      Multi-SIM support for OnChip
10/02/13   yt      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi_uim_uicc.h"
#include "mmgsdi_uim_icc.h"
#include "mmgsdi.h"
#include "mmgsdi_evt.h"
#include "uim.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_common_rsp.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_uim_common.h"
#include "mmgsdi_uicc.h"
#include "mmgsdi_icc.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_hidden_key.h"


/*===========================================================================
   FUNCTION:      MMGSDI_COMMON_GET_FILE_ATTR

   DESCRIPTION:
     This function process the GET_FILE_ATTR command for ICC or UICC.

     Behavior for the function for the following file types:

     TRANSPARENT FILE:  Will get information on the characteristics of
                        Transparent file
     CYCLIC FILE:       Will get information on the characteristics of
                        Cyclic file.
     LINEAR FIXED FILE: Will get information on the characteristics of
                        Linear Fixed file.
     MASTER FILE:       Will get information on the Master File
     DEDICATED FILE:    Will get information on the Dedicated File

   DEPENDENCIES:
     The technology type (GSM/CDMA/UICC) has to be dtermined prior to using this
     function


   LIMITATIONS:
     None


   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The funtion was successful in getting the file
                              attributes of a particular file.
     MMGSDI_ERROR:
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the function are not
                              within appropriate ranges.

   SIDE EFFECTS:
     None

=============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_get_file_attr(
  mmgsdi_get_file_attr_req_type * req_ptr,
  mmgsdi_protocol_enum_type       protocol
)
{
  mmgsdi_return_enum_type             mmgsdi_status        = MMGSDI_ERROR;
  mmgsdi_int_app_info_type          * prov_app_info_ptr    = NULL;
  mmgsdi_len_type                     cache_len            = 0;
  mmgsdi_cache_init_enum_type         cache_state          = MMGSDI_CACHE_MAX_ENUM;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  switch (req_ptr->access.access_method)
  {
    case MMGSDI_EF_ENUM_ACCESS:
      UIM_MSG_HIGH_1("Sending SELECT to UIM for MMGSDI_EF_ENUM_ACCESS 0x%x",
                     req_ptr->access.file.file_enum);
      break;

    case MMGSDI_DF_ENUM_ACCESS:
      UIM_MSG_HIGH_1("Sending SELECT to UIM for MMGSDI_DF_ENUM_ACCESS 0x%x",
                     req_ptr->access.file.df_enum);
      break;

    case MMGSDI_BY_PATH_ACCESS:
      UIM_MSG_HIGH_0("Sending SELECT to UIM for MMGSDI_BY_PATH_ACCESS");
      break;

    default:
      MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                   req_ptr->access.access_method);
      return MMGSDI_INCORRECT_PARAMS;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                           req_ptr->request_header.session_id,
                                           req_ptr->access,
                                           protocol))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       NULL,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    if (protocol == MMGSDI_UICC)
    {
      mmgsdi_util_check_usim_spec_version(&req_ptr->access,
                                          prov_app_info_ptr->usim_spec_version);
    }

    /* Check status of EF in cached SST/CST/UST */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
      req_ptr->request_header.client_id,
      req_ptr->request_header.session_id,
      prov_app_info_ptr->app_data.app_type,
      &req_ptr->access );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      if(req_ptr->access.file.file_enum == MMGSDI_USIM_7F66_PROP1_ACT_HPLMN)
      {
        mmgsdi_status = MMGSDI_ACCESS_DENIED;
      }
      mmgsdi_util_print_file_information(req_ptr->access);
      return mmgsdi_status;
    }
  }

  /* Check if MMGSDI has tried to cache this file and status is NOT FOUND. If
     yes, there is no reason to try to access the same file again and we can
     avoid sending the SELECT command */
  mmgsdi_status = mmgsdi_cache_read_len(req_ptr->request_header.session_id,
                                        &req_ptr->access,
                                        &cache_len,
                                        0,
                                        &cache_state);

  if((mmgsdi_status == MMGSDI_NOT_FOUND) &&
     (cache_state == MMGSDI_CACHE_NOT_FOUND))
  {
    return mmgsdi_status;
  }

  UIM_MSG_HIGH_0("Sending down SELECT command to MMGSDI UIM");

  /* Send select request to the UIM server */
  if (protocol == MMGSDI_ICC)
  {
    mmgsdi_status = mmgsdi_uim_icc_select(req_ptr, FALSE, NULL);
  }
  else if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_select(req_ptr, FALSE, NULL, TRUE);
  }

  return mmgsdi_status;
} /* mmgsdi_common_get_file_attr */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_WRITE

  DESCRIPTION:
    This function will process the write command to any file

  DEPENDENCIES:


  LIMITATIONS:
    None

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

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_write (
  mmgsdi_write_req_type     * req_ptr,
  mmgsdi_protocol_enum_type   protocol
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type          slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type         *prov_app_info_ptr    = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  /* Check for protocol */
  if ((protocol != MMGSDI_UICC) && (protocol != MMGSDI_ICC))
  {
    return MMGSDI_ERROR;
  }

  /* Check for access method only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_BY_PATH_ACCESS:
  case MMGSDI_EF_ENUM_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                 req_ptr->access.access_method);
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                            req_ptr->request_header.session_id,
                                            req_ptr->access,
                                            protocol))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       &slot,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    /* Check if requested file is being purged */
    if ((MMGSDI_EF_ENUM_ACCESS   == req_ptr->access.access_method)      &&
        ((MMGSDI_GSM_LOCI        == req_ptr->access.file.file_enum) ||
         (MMGSDI_USIM_LOCI       == req_ptr->access.file.file_enum) ||
         (MMGSDI_USIM_PSLOCI     == req_ptr->access.file.file_enum) ||
         (MMGSDI_USIM_EPSLOCI    == req_ptr->access.file.file_enum))    &&
        (mmgsdi_util_is_purge_temp_identities_in_progress(slot))
        )
    {
      UIM_MSG_ERR_1("Request not allowed, file_enum=0x%x is being purged",
                    req_ptr->access.file.file_enum);
      return MMGSDI_ERROR;
    } /* XXLOCI file being purged */

    /*Check to see if the security attributes allow this file to be updated*/
    mmgsdi_status = mmgsdi_util_check_file_attr(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      slot,
                      prov_app_info_ptr,
                      &req_ptr->access,
                      MMGSDI_WRITE_REQ,
                      FALSE);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("File Update not allowed");
      return mmgsdi_status;
    }

    if (protocol == MMGSDI_UICC)
    {
      mmgsdi_util_check_usim_spec_version(&req_ptr->access,
                                          prov_app_info_ptr->usim_spec_version);
    }

    /* Check status of EF in cached UST/SST/CST */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      prov_app_info_ptr->app_data.app_type,
                      &req_ptr->access );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_print_file_information(req_ptr->access);
      UIM_MSG_ERR_1("SERVICE not allowed: error 0x%x", mmgsdi_status);
      return mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_1("Sending down WRITE command to MMGSDI UIM 0x%x",
                 req_ptr->access.file.file_enum);
  switch (req_ptr->file_type)
  {
    case MMGSDI_LINEAR_FIXED_FILE:
    case MMGSDI_CYCLIC_FILE:
      if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_uim_icc_write_record(req_ptr);
      }
      else
      {
        mmgsdi_status = mmgsdi_uim_uicc_write_record(req_ptr);
      }
      break;

    case MMGSDI_TRANSPARENT_FILE:
      if (protocol == MMGSDI_ICC)
      {
        mmgsdi_status = mmgsdi_uim_icc_write_transparent(req_ptr, FALSE, NULL);
      }
      else
      {
        /* note: with SFI changes, no need to use access lookup table */
        mmgsdi_status = mmgsdi_uim_uicc_write_transparent(req_ptr, FALSE, NULL);
      }
      break;

    default:
      mmgsdi_status = MMGSDI_INCORRECT_PARAMS;
      break;
  }

  return mmgsdi_status;
} /* mmgsdi_common_write */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_INCREASE

  DESCRIPTION:
    This function will provide increase access to any file

  DEPENDENCIES:


  LIMITATIONS:
    None

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

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_increase (
  mmgsdi_increase_req_type     * req_ptr,
  mmgsdi_protocol_enum_type      protocol
)
{
  mmgsdi_return_enum_type           mmgsdi_status        = MMGSDI_SUCCESS;
  mmgsdi_slot_id_enum_type          slot                 = MMGSDI_MAX_SLOT_ID_ENUM;
  mmgsdi_int_app_info_type         *prov_app_info_ptr    = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* Check for access method only */
  switch (req_ptr->access.access_method)
  {
  case MMGSDI_BY_PATH_ACCESS:
  case MMGSDI_EF_ENUM_ACCESS:
    break;

  default:
    MMGSDIUTIL_DEBUG_MSG_ERROR_1("Invalid Access Type 0x%x",
                                 req_ptr->access.access_method);
    return MMGSDI_ERROR;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_check_file_access_data(
                                            req_ptr->request_header.session_id,
                                            req_ptr->access,
                                            protocol))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_util_get_prov_session_info(req_ptr->request_header.session_id,
                                       NULL,
                                       &slot,
                                       &prov_app_info_ptr) != MMGSDI_SUCCESS)
  {
    prov_app_info_ptr = NULL;
  }

  if (prov_app_info_ptr != NULL)
  {
    /*Check to see if the security attributes allow this file to be updated*/
    mmgsdi_status = mmgsdi_util_check_file_attr(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      slot,
                      prov_app_info_ptr,
                      &req_ptr->access,
                      MMGSDI_INCREASE_REQ,
                      FALSE);
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      UIM_MSG_ERR_0("File Update not allowed");
      return mmgsdi_status;
    }

    /* Check status of EF in cached UST */
    mmgsdi_status = mmgsdi_util_is_file_ok_in_svc_table(
                      req_ptr->request_header.client_id,
                      req_ptr->request_header.session_id,
                      prov_app_info_ptr->app_data.app_type,
                      &req_ptr->access );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      mmgsdi_util_print_file_information(req_ptr->access);
      UIM_MSG_ERR_1("SERVICE not allowed: error 0x%x", mmgsdi_status);
      return mmgsdi_status;
    }
  }

  UIM_MSG_HIGH_1("Sending down INCREASE command to MMGSDI UIM 0x%x",
                 req_ptr->access.file.file_enum);
  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uim_uicc_increase(req_ptr);
  }
  else if (protocol == MMGSDI_ICC)
  {
    mmgsdi_status = mmgsdi_uim_icc_increase(req_ptr);
  }

  return mmgsdi_status;
} /* mmgsdi_common_increase */


/*============================================================================
   FUNCTION:      MMGSDI_COMMON_COMPUTE_IP_AUTH

   DESCRIPTION
     This function is called for a Compute IP Authentication request.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_compute_ip_auth(
  const mmgsdi_compute_ip_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if ((protocol != MMGSDI_ICC) && (protocol != MMGSDI_UICC))
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_compute_ip_auth(req_ptr, protocol);
}/* mmgsdi_common_compute_ip_auth */


/*============================================================================
   FUNCTION:      MMGSDI_COMMON_RUN_CAVE

   DESCRIPTION
     This function is called for a RUN CAVE ALGORITHM request.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_run_cave(
  const mmgsdi_run_cave_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
)
{
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr          - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_run_cave(req_ptr, protocol);
} /* mmgsdi_common_run_cave */


/*============================================================================
   FUNCTION:      MMGSDI_COMMON_CDMA_AKA_AUTH

   DESCRIPTION
     This function is called for a CDMA AKA Authenticate request.

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
mmgsdi_return_enum_type mmgsdi_common_cdma_aka_auth(
  const mmgsdi_cdma_aka_req_type        *  req_ptr,
  mmgsdi_protocol_enum_type                protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  boolean                        cdma_aka_avail = FALSE;
  mmgsdi_return_enum_type        mmgsdi_status  = MMGSDI_ERROR;
  mmgsdi_session_type_enum_type  session_type   = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  mmgsdi_status = mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                                               &session_type, NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS ||
     (session_type != MMGSDI_1X_PROV_PRI_SESSION &&
      session_type != MMGSDI_1X_PROV_SEC_SESSION &&
      session_type != MMGSDI_1X_PROV_TER_SESSION ))
  {
    UIM_MSG_ERR_1("Invalid session type: %d",session_type);
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC)
  {
    mmgsdi_status = mmgsdi_uicc_chk_srv_available(req_ptr->request_header.client_id,
                                                  req_ptr->request_header.session_id,
                                                  MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                  MMGSDI_CSIM_SRV_AKA,
                                                  &cdma_aka_avail);
  }
  else
  {
    mmgsdi_status = mmgsdi_icc_chk_srv_available(req_ptr->request_header.session_id,
                                                 MMGSDI_CHK_SRV_FROM_CARD_OR_CACHE,
                                                 MMGSDI_CDMA_SRV_AKA,
                                                 &cdma_aka_avail);
  }

  if (mmgsdi_status != MMGSDI_SUCCESS || cdma_aka_avail == FALSE)
  {
    UIM_MSG_ERR_2("CDMA AKA AUTH service check failed. status: %d, srv avail: %d",
                  mmgsdi_status,cdma_aka_avail);
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_cdma_aka_auth(req_ptr, protocol);
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_cdma_aka_auth */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_GENERATE_KEY_VPM

  DESCRIPTION:
    This function will allow GENERATE KEY / VPM command to be sent to UIM

  DEPENDENCIES:


  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_common_generate_key_vpm (
  const mmgsdi_generate_key_req_type   *req_ptr,
  mmgsdi_protocol_enum_type             protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
    2. req_ptr->request_header.response_cb      - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_generate_key_vpm(req_ptr, protocol);
#else
  (void)req_ptr;
  (void)protocol;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_generate_key_vpm */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_BLOCK

  DESCRIPTION:
    This function will allow OTASP Block Request command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application


  LIMITATIONS:
    None

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_block (
  const mmgsdi_session_otasp_block_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  if (protocol != MMGSDI_ICC && protocol != MMGSDI_UICC)
  {
    return MMGSDI_NOT_SUPPORTED;
  }

  return mmgsdi_uim_common_otasp_block(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_block */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_OTAPA

  DESCRIPTION:
    This function will allow OTAPA Request command to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_otapa (
  const mmgsdi_session_otasp_otapa_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* No need to check callback,
     already checked when preparing mmgsdi command*/
  /* No need to check randseed,
     already checked when preparing mmgsdi command */
  if (protocol == MMGSDI_UICC && 0 == req_ptr->esn)
  {
    UIM_MSG_ERR_0("ESN value should not be 0x00 for UICC OTAPA Request");
    return MMGSDI_ERROR;
  }
  
  return mmgsdi_uim_common_otasp_otapa(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_otapa */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_COMMIT

  DESCRIPTION:
    This function will allow OTASP Commit Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_commit (
  const mmgsdi_session_otasp_commit_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                       protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_commit(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_commit */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_SSPR_CONFIG

  DESCRIPTION:
    This function will allow OTASP SSPR Config Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_sspr_config (
  const mmgsdi_session_otasp_sspr_config_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                            protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_sspr_config(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_sspr_config */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_KEY_GEN

  DESCRIPTION:
    This function will allow OTASP KEY GEN Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_key_gen (
  const mmgsdi_session_otasp_key_gen_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                        protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_key_gen(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_key_gen */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_OTASP_MS_KEY

  DESCRIPTION:
    This function will allow OTASP MS Key Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_otasp_ms_key (
  const mmgsdi_session_otasp_ms_key_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                       protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_otasp_ms_key(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_otasp_ms_key */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_BS_CHAL

  DESCRIPTION:
    This function will allow BS CHAL Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_bs_chal (
  const mmgsdi_session_bs_chal_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                  protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_bs_chal(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_bs_chal */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SSD_CONFIRM

  DESCRIPTION:
    This function will allow SSD Confirm Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_ssd_confirm (
  const mmgsdi_session_ssd_confirm_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                      protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_ssd_confirm(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_ssd_confirm */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_SSD_UPDATE

  DESCRIPTION:
    This function will allow SSD Update Request to be sent to UIM

  DEPENDENCIES:
    If the application is any application other than default, then the session
    should have been opened with that application

  LIMITATIONS:

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_ssd_update (
  const mmgsdi_session_ssd_update_req_type    * req_ptr,
  mmgsdi_protocol_enum_type                     protocol
)
{
#ifdef FEATURE_MMGSDI_3GPP2
  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_ptr                                  - Cannot be NULL
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  if(protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  return mmgsdi_uim_common_ssd_update(req_ptr, protocol);
#else
  (void)req_ptr;
  return MMGSDI_NOT_SUPPORTED;
#endif /* FEATURE_MMGSDI_3GPP2 */
} /* mmgsdi_common_ssd_update */


/*===========================================================================
  FUNCTION:      MMGSDI_COMMON_PIN_OPERATION

  DESCRIPTION:
    This function will perform pin operations

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

===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_pin_operation (
  const mmgsdi_pin_operation_req_type     * req_ptr,
  mmgsdi_protocol_enum_type                 protocol
)
{
  mmgsdi_return_enum_type    mmgsdi_status     = MMGSDI_ERROR;
  mmgsdi_int_app_info_type  *sel_app_ptr       = NULL;
  uint32                     pin_index         = 0;
  mmgsdi_app_pin_info_type **app_pin1_tbl_pptr = NULL;
  uint8                      app_pin1_tbl_size = 0;
  mmgsdi_session_info_type  *session_info_ptr  = NULL;

  /*---------------------------------------------------------------------------
    FUNCTION PARAMETER CHECKS
    1. req_p                                  - Cannot be NULL
    2. req_p->request_header.response_cb      - Cannot be NULL
    3. req_p->access.access_method            - MMGSDI_EF_ENUM_ACCESS
  ---------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL_2(req_ptr, req_ptr->request_header.response_cb);

  if (protocol == MMGSDI_NO_PROTOCOL)
  {
    return MMGSDI_ERROR;
  }

  /* If request is for a Hiddenkey Operation */
  if(req_ptr->pin_id == MMGSDI_HIDDENKEY)
  {
    /* Hiddenkey is supported only on UICC */
    if(protocol == MMGSDI_UICC)
    {
      mmgsdi_status = mmgsdi_hidden_key_operation(req_ptr);
    } /* protocol is MMGSDI_UICC */
    else
    {
      UIM_MSG_ERR_0("Hiddenkey operation not supported for non-UICC protocols");
      mmgsdi_status = MMGSDI_NOT_SUPPORTED;
    } /* protocol is MMGSDI_ICC */
    return mmgsdi_status;
  }

  if (protocol == MMGSDI_ICC &&
      (req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_8 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_8))
  {
    UIM_MSG_ERR_1("Key ref based PIN input 0x%x not supported for ICC mode",
                  req_ptr->orig_pin_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(req_ptr->request_header.session_id,
                                                   NULL,
                                                   NULL,
                                                   &sel_app_ptr,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);
  if (mmgsdi_status != MMGSDI_SUCCESS || sel_app_ptr == NULL ||
      session_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Block PIN operations for non-provisioning apps in ICC mode because
     MMGSDI has valid PIN info only for prov sessions. Streaming APDUs
     should be used to perform such PIN operations. */
  if(protocol == MMGSDI_ICC &&
     mmgsdi_util_is_non_prov_session(session_info_ptr->session_type))
  {
    UIM_MSG_ERR_1("PIN OP not supported for session type 0x%x in non-UICC mode",
                  session_info_ptr->session_type);
    return MMGSDI_NOT_SUPPORTED;
  }

  if(protocol == MMGSDI_UICC &&
     req_ptr->pin_id == MMGSDI_UNIVERSAL_PIN &&
     sel_app_ptr->universal_pin_ptr != NULL &&
     sel_app_ptr->universal_pin_ptr->status == MMGSDI_PIN_DISABLED &&
     req_ptr->pin_op != MMGSDI_PIN_OP_ENABLE)
  {
    UIM_MSG_ERR_0("UPIN is not in use or disabled - PIN operation skipped");
    return MMGSDI_ERROR;
  }

  if (protocol == MMGSDI_UICC &&
      sel_app_ptr->fcp_pin_data_absent &&
      (req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_PIN_APPL_8))
  {
    mmgsdi_key_ref_values_enum_type   input_key_ref = MMGSDI_PIN_APPL_1;

    switch (req_ptr->orig_pin_id)
    {
      case MMGSDI_KEY_REF_PIN_APPL_1:
        input_key_ref = MMGSDI_PIN_APPL_1;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_2:
        input_key_ref = MMGSDI_PIN_APPL_2;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_3:
        input_key_ref = MMGSDI_PIN_APPL_3;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_4:
        input_key_ref = MMGSDI_PIN_APPL_4;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_5:
        input_key_ref = MMGSDI_PIN_APPL_5;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_6:
        input_key_ref = MMGSDI_PIN_APPL_6;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_7:
        input_key_ref = MMGSDI_PIN_APPL_7;
        break;
      case MMGSDI_KEY_REF_PIN_APPL_8:
        input_key_ref = MMGSDI_PIN_APPL_8;
        break;
      default:
        break;
    }
    if (sel_app_ptr->pin1_ptr->key_ref != input_key_ref)
    {
      UIM_MSG_HIGH_0("PIN1 keyref switch as req keyref mismatch with curr keyref");

      app_pin1_tbl_pptr = mmgsdi_util_get_app_pin1_info_tbl_ptr(&app_pin1_tbl_size,
                                                                req_ptr->request_header.slot_id);
      if(app_pin1_tbl_pptr == NULL ||
         app_pin1_tbl_size == 0)
      {
        UIM_MSG_ERR_0("Could not get PIN1 table");
        return MMGSDI_ERROR;
      }

      /* Ignoring return value because app pin table is fixed and pin index for
         a particular key ref will always be found */
      (void)mmgsdi_util_find_app_pin_index((const mmgsdi_app_pin_info_type **)app_pin1_tbl_pptr,
                                           app_pin1_tbl_size,
                                           input_key_ref,
                                           &pin_index);
      sel_app_ptr->pin1_ptr = app_pin1_tbl_pptr[pin_index];
    }
  }

  if (protocol == MMGSDI_UICC &&
      sel_app_ptr->fcp_pin_data_absent &&
      (req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_1 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_2 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_3 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_4 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_5 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_6 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_7 ||
       req_ptr->orig_pin_id == MMGSDI_KEY_REF_SEC_PIN_APPL_8))
  {
    mmgsdi_key_ref_values_enum_type   input_key_ref = MMGSDI_SEC_PIN_APPL_1;
    switch (req_ptr->orig_pin_id)
    {
      case MMGSDI_KEY_REF_SEC_PIN_APPL_1:
        input_key_ref = MMGSDI_SEC_PIN_APPL_1;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_2:
        input_key_ref = MMGSDI_SEC_PIN_APPL_2;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_3:
        input_key_ref = MMGSDI_SEC_PIN_APPL_3;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_4:
        input_key_ref = MMGSDI_SEC_PIN_APPL_4;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_5:
        input_key_ref = MMGSDI_SEC_PIN_APPL_5;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_6:
        input_key_ref = MMGSDI_SEC_PIN_APPL_6;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_7:
        input_key_ref = MMGSDI_SEC_PIN_APPL_7;
        break;
      case MMGSDI_KEY_REF_SEC_PIN_APPL_8:
        input_key_ref = MMGSDI_SEC_PIN_APPL_8;
        break;
      default:
        break;
    }

    if (sel_app_ptr->pin2.key_ref != input_key_ref)
    {
      UIM_MSG_HIGH_0("PIN2 keyref switch as req keyref mismatch with curr keyref");
      sel_app_ptr->pin2.key_ref = input_key_ref;
    }
  }

  if(protocol == MMGSDI_UICC && req_ptr->pin_id == MMGSDI_PIN2)
  {
    if(MMGSDI_SUCCESS == mmgsdi_util_select_adf_before_pin2_op(
          req_ptr->request_header.session_id))
    {
      UIM_MSG_HIGH_0("Selected ADF successfully prior to PIN2 operation");
    }
  }

  /* Check for access method only */
  switch(req_ptr->pin_op)
  {
    case MMGSDI_PIN_OP_VERIFY:
    case MMGSDI_PIN_OP_UNBLOCK:
      mmgsdi_status = mmgsdi_uim_common_unblock_verify_pin(req_ptr,
                                                   MMGSDI_RETRY_NONE,
                                                   protocol,
                                                   FALSE);
      break;
    case MMGSDI_PIN_OP_ENABLE:
    case MMGSDI_PIN_OP_DISABLE_AND_REPLACE:
    case MMGSDI_PIN_OP_DISABLE:
      if (protocol == MMGSDI_ICC && req_ptr->pin_op != MMGSDI_PIN_OP_ENABLE)
      {
        if (req_ptr->pin_op == MMGSDI_PIN_OP_DISABLE_AND_REPLACE)
        {
          return MMGSDI_INCORRECT_PARAMS;
        }

        if (mmgsdi_icc_is_disable_chv1_allowed(
              req_ptr->request_header.session_id) == FALSE)
        {
          return MMGSDI_NOT_SUPPORTED;
        }
      }
      mmgsdi_status = mmgsdi_uim_common_enable_disable_pin(req_ptr, protocol);
      break;
    case MMGSDI_PIN_OP_CHANGE:
      mmgsdi_status = mmgsdi_uim_common_change_pin(req_ptr, protocol);
      break;
    default:
      return MMGSDI_INCORRECT_PARAMS;
  }
  return mmgsdi_status;
} /* mmgsdi_common_pin_operation */


/*===========================================================================
FUNCTION MMGSDI_COMMON_IS_SERVICE_AVAILABLE

DESCRIPTION
  This function is a common function to check if service is available.

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type

  SIDE EFFECTS:
    None
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_common_is_service_available (
  const mmgsdi_srv_available_req_type      * req_ptr
)
{
  mmgsdi_return_enum_type     mmgsdi_status        = MMGSDI_ERROR;
  uint8                       index                = 0;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SRV_AVAILABLE_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(index,
                                                          MMGSDI_SRV_AVAILABLE_REQ,
                                                          MMGSDI_SUCCESS);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
}/* mmgsdi_common_is_service_available */

