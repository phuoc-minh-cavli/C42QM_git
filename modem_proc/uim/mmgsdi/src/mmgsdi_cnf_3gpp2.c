/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 M M G S D I   C N F   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains functions to build confirmation message for
  3GPP2 related functions

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

#include "uim_variation.h"
#include "intconv.h"
#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdi_cnf.h"
#include "bit.h"
#include "mmgsdi_card_init.h"


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
     This function builds the Comppute IP authentication confirmation data

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
mmgsdi_return_enum_type mmgsdi_cnf_build_compute_ip_auth_data(
  mmgsdi_compute_ip_auth_cnf_type   *compute_ip_auth_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *compute_ip_auth_rsp_data_ptr
)
{
  uint32                   compute_ip_len            = 0;

  UIM_MSG_HIGH_1("MMGSDI_CNF_BUILD_COMPUTE_IP_AUTH_DATA, status = 0x%x",
                  mmgsdi_status);

  MMGSDIUTIL_RETURN_IF_NULL_2(compute_ip_auth_cnf_ptr,
                              compute_ip_auth_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("Compute IP Rsp Success");

    /*-------------------------------------------------------------------------
      Compute IP Auth response is successful
      - Mem malloc
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   compute_ip_len = int32touint32(compute_ip_auth_rsp_data_ptr->data_len);

    if (compute_ip_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_ptr,
        compute_ip_len);
      if (compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy(compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_ptr,
                     compute_ip_len,
                     compute_ip_auth_rsp_data_ptr->data_ptr,
                     compute_ip_len);
    }
    compute_ip_auth_cnf_ptr->cmpt_ip_response_data.data_len =
      compute_ip_auth_rsp_data_ptr->data_len;
  }
  else
  {
    UIM_MSG_HIGH_1("COMPUTE IP Auth Fail, status %x", mmgsdi_status);
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_compute_ip_auth_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_RUN_CAVE_DATA

   DESCRIPTION:
     This function builds the Run Cave confirmation data

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
mmgsdi_return_enum_type mmgsdi_cnf_build_run_cave_data(
  mmgsdi_run_cave_cnf_type          *run_cave_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *run_cave_rsp_data_ptr
)
{
  mmgsdi_return_enum_type  build_cnf_status    = MMGSDI_SUCCESS;

  UIM_MSG_HIGH_1 ("MMGSDI_CNF_BUILD_RUN_CAVE_DATA, status = 0x%x", mmgsdi_status);

  MMGSDIUTIL_RETURN_IF_NULL_2(run_cave_cnf_ptr, run_cave_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0 ("RUN CAVE Rsp Success");

    /*-------------------------------------------------------------------------
      RUN CAVE response is successful
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   run_cave_cnf_ptr->run_cave_response_data =
       b_unpackd (run_cave_rsp_data_ptr->data_ptr, 0,
                  int32touint16(run_cave_rsp_data_ptr->data_len*8));
  }
  else
  {
    UIM_MSG_ERR_1("RUN CAVE Fail, status %x", mmgsdi_status);
  }
  return build_cnf_status;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_cdma_aka_data(
  mmgsdi_cdma_aka_cnf_type          *aka_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *aka_rsp_data_ptr)
{
  uint32                   auth_len            = 0;

  UIM_MSG_HIGH_0 ("MMGSDI_CNF_BUILD_CDMA_AKA_DATA");

  MMGSDIUTIL_RETURN_IF_NULL_2(aka_cnf_ptr, aka_rsp_data_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("CDMA AKA Auth Fail, status %x", mmgsdi_status);
    return MMGSDI_SUCCESS;
  }

  if (aka_rsp_data_ptr->data_ptr == NULL ||
      aka_rsp_data_ptr->data_len < 1)
  {
    UIM_MSG_ERR_0("Invalid response data");
    return MMGSDI_ERROR;
  }

  aka_cnf_ptr->sync_fail_tag = aka_rsp_data_ptr->data_ptr[0];

  if (aka_cnf_ptr->sync_fail_tag == MMGSDI_CDMA_AKA_VALID_SEQ_NUM)
  {
    /*-------------------------------------------------------------------------
      AKA Auth response is successful
      - Mem malloc corresponding data pointers for RES
      - Populate the data pointers RES, CK and IK
    -------------------------------------------------------------------------*/
    if (aka_rsp_data_ptr->data_len <
          (MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
           MMGSDI_CDMA_AKA_CK_LEN +
           MMGSDI_CDMA_AKA_IK_LEN +
           MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE))
    {
      UIM_MSG_ERR_1("Invalid CDMA aka data length: 0x%x",
                    aka_rsp_data_ptr->data_len);
      return MMGSDI_ERROR;
    }

    memscpy(aka_cnf_ptr->aka_data.aka_keys.ck,
            sizeof(aka_cnf_ptr->aka_data.aka_keys.ck),
            (byte*)(aka_rsp_data_ptr->data_ptr +
                 MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN),
            MMGSDI_CDMA_AKA_CK_LEN);

    memscpy(aka_cnf_ptr->aka_data.aka_keys.ik,
            sizeof(aka_cnf_ptr->aka_data.aka_keys.ik),
            (byte*)(aka_rsp_data_ptr->data_ptr+
                 MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
                 MMGSDI_CDMA_AKA_CK_LEN),
            MMGSDI_CDMA_AKA_IK_LEN);

    auth_len = (uint32)aka_rsp_data_ptr->data_ptr[
                 MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
                 MMGSDI_CDMA_AKA_CK_LEN +
                 MMGSDI_CDMA_AKA_IK_LEN];
    if (auth_len > (aka_rsp_data_ptr->data_len -
                    (MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
                     MMGSDI_CDMA_AKA_CK_LEN +
                     MMGSDI_CDMA_AKA_IK_LEN +
                     MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE)))
    {
      UIM_MSG_ERR_1("Invalid RES data length: 0x%x",
                    auth_len);
      return MMGSDI_ERROR;
    }

    if (auth_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        aka_cnf_ptr->aka_data.aka_keys.res.data_ptr,
        auth_len);
      if (aka_cnf_ptr->aka_data.aka_keys.res.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      aka_cnf_ptr->aka_data.aka_keys.res.data_len = auth_len;
      memscpy(aka_cnf_ptr->aka_data.aka_keys.res.data_ptr,
              auth_len,
              (byte*)(aka_rsp_data_ptr->data_ptr +
                 MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN +
                 MMGSDI_CDMA_AKA_CK_LEN +
                 MMGSDI_CDMA_AKA_IK_LEN +
                 MMGSDI_CDMA_AKA_AUTH_RES_LEN_SIZE),
              auth_len);
    }
  }
  else if (aka_cnf_ptr->sync_fail_tag == MMGSDI_CDMA_AKA_INVALID_SEQ_NUM)
  {
    UIM_MSG_HIGH_0 ("Sequence number resynchronization required");

    /*-------------------------------------------------------------------------
      AKA authenticate synchronization failure
      - Populate the data pointer
    -------------------------------------------------------------------------*/
    if (aka_rsp_data_ptr->data_len < (MMGSDI_CDMA_AKA_AUTS_LEN +
                                      MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN))
    {
      UIM_MSG_ERR_1("Invalid CDMA aka data length: 0x%x",
                    aka_rsp_data_ptr->data_len);
      return MMGSDI_ERROR;
    }

    memscpy(aka_cnf_ptr->aka_data.auts,
            sizeof(aka_cnf_ptr->aka_data.auts),
            (byte*)(aka_rsp_data_ptr->data_ptr +
                 MMGSDI_CDMA_AKA_AUTH_SYNC_TAG_LEN),
            MMGSDI_CDMA_AKA_AUTS_LEN);
  }
  else
  {
    UIM_MSG_ERR_1("Invalid CDMA AKA sync tag %x", aka_cnf_ptr->sync_fail_tag);
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_generate_key_vpm_data(
  mmgsdi_generate_key_vpm_cnf_type  *generate_key_cnf_ptr,
  mmgsdi_return_enum_type            mmgsdi_status,
  const mmgsdi_data_type            *generate_key_rsp_data_ptr
)
{
  uint32                   generate_key_vpm_len        = 0;

  UIM_MSG_HIGH_1("MMGSDI_CNF_BUILD_GENERATE_KEY_VPM_DATA, status = 0x%x",
                 mmgsdi_status);

  MMGSDIUTIL_RETURN_IF_NULL_2(generate_key_cnf_ptr, generate_key_rsp_data_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("GENERATE KEY VPM Rsp Success");

    /*-------------------------------------------------------------------------
      GENERATE KEY response is successful
      - Populate the data pointers
      - Error and free if memory allocation failed
    -------------------------------------------------------------------------*/
   if(generate_key_rsp_data_ptr->data_len < MMGSDI_KEY_SIZE)
   {
     /* As per spec, the response should begin with an 8 byte key */
     UIM_MSG_ERR_1("Bad response length for generate key! Length: 0x%x",
                   generate_key_rsp_data_ptr->data_len);
     return MMGSDI_ERROR;
   } /* end if generate_key_vpm_len < MMGSDI_KEY_SIZE */
   generate_key_vpm_len = int32touint32(generate_key_rsp_data_ptr->data_len);

   mmgsdi_memscpy(generate_key_cnf_ptr->key_data,
                  sizeof(generate_key_cnf_ptr->key_data),
                  generate_key_rsp_data_ptr->data_ptr,
                  MMGSDI_KEY_SIZE);
   if(generate_key_vpm_len > MMGSDI_KEY_SIZE)
   {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          generate_key_cnf_ptr->octet_data.data_ptr,
        generate_key_vpm_len - MMGSDI_KEY_SIZE);
      if (generate_key_cnf_ptr->octet_data.data_ptr == NULL)
      {
        return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
      mmgsdi_memscpy(generate_key_cnf_ptr->octet_data.data_ptr,
                     generate_key_vpm_len - MMGSDI_KEY_SIZE,
                     &(generate_key_rsp_data_ptr->data_ptr[MMGSDI_KEY_SIZE]),
                     generate_key_vpm_len - MMGSDI_KEY_SIZE);
      generate_key_cnf_ptr->octet_data.data_len = generate_key_vpm_len - MMGSDI_KEY_SIZE;
   }
   else
   {
     generate_key_cnf_ptr->octet_data.data_len = 0;
   }
  }
  else
  {
    UIM_MSG_HIGH_1("GENERATE KEY VPM Fail, status 0x%x", mmgsdi_status);
  }
  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_session_read_prl_data(
  mmgsdi_session_read_prl_cnf_type              *session_read_prl_cnf_ptr,
  mmgsdi_return_enum_type                       *mmgsdi_status_ptr,
  mmgsdi_session_id_type                         session_id
)
{
  mmgsdi_session_info_type               *session_info_ptr  = NULL;
  mmgsdi_int_app_info_type               *int_app_info_ptr  = NULL;
  uint8                                   prl_index         = 0;
  mmgsdi_technology_enum_type             dummy_tech        = MMGSDI_TECH_UNKNOWN;
  mmgsdi_attribute_enum_type              attribute         = MMGSDI_ATTR_UNKNOWN;

  MMGSDIUTIL_RETURN_IF_NULL_2(session_read_prl_cnf_ptr,
                              mmgsdi_status_ptr);

  UIM_MSG_HIGH_1("MMGSDI_CNF_BUILD_SESSION_READ_PRL, status = 0x%x",
                 *mmgsdi_status_ptr);

  *mmgsdi_status_ptr = mmgsdi_util_get_session_app_info(session_id,
                                                        NULL,
                                                        NULL,
                                                        &int_app_info_ptr,
                                                        NULL,
                                                        &session_info_ptr,
                                                        NULL);

  if(*mmgsdi_status_ptr != MMGSDI_SUCCESS ||
     session_info_ptr == NULL ||
     int_app_info_ptr == NULL)
  {
    UIM_MSG_ERR_1("Invalid session ID = 0x%x",
                  session_id);
    return MMGSDI_SUCCESS;
  }

  if(mmgsdi_util_is_1x_prov_session(session_info_ptr->session_type) == FALSE ||
     (int_app_info_ptr->app_state != MMGSDI_APP_STATE_READY &&
      int_app_info_ptr->app_state != MMGSDI_APP_STATE_ILLEGAL))
  {
    UIM_MSG_ERR_3("app type: 0x%x app state: 0x%x session_type: 0x%x",
                  int_app_info_ptr->app_data.app_type,
                  int_app_info_ptr->app_state,
                  session_info_ptr->session_type);

    *mmgsdi_status_ptr = MMGSDI_INCORRECT_PARAMS;
    return MMGSDI_SUCCESS;
  }

  (void)mmgsdi_util_get_tech_and_attr_from_session_type(
                                         session_info_ptr->session_type,
                                         &dummy_tech,
                                         &attribute);
  prl_index = (uint8)attribute;
  if (prl_index > MMGSDI_TER_PRL_INDEX)
  {
    *mmgsdi_status_ptr = MMGSDI_ERROR;
    return MMGSDI_SUCCESS;
  }

  /* If during initialization time (post pin and SUB_READY) PRL was not
     validated because SD was not init then do the PRL validation now.
     In case it is not success, no need to return ERROR from here. We
     will copy all 0s and send to CM in cnf handling as legacy behavior */
  if (mmgsdi_1x_prl[prl_index].prl_validated == FALSE)
  {
    (void)mmgsdi_card_init_1x_read_prl_init(session_info_ptr->session_id);
  }

  if (*mmgsdi_status_ptr == MMGSDI_SUCCESS)
  {
    UIM_MSG_HIGH_0("READ PRL Info - Success");
    if(mmgsdi_1x_prl[prl_index].prl_data_ptr != NULL)
    {
      /* copy the read PRL to the cnf ptr */
      session_read_prl_cnf_ptr->prl_version = mmgsdi_1x_prl[prl_index].prl_data_ptr->prl_version;
      session_read_prl_cnf_ptr->size = mmgsdi_1x_prl[prl_index].prl_data_ptr->size;
      session_read_prl_cnf_ptr->valid = mmgsdi_1x_prl[prl_index].prl_data_ptr->valid;
      session_read_prl_cnf_ptr->roaming_list_ptr = mmgsdi_1x_prl[prl_index].prl_data_ptr->roaming_list_ptr;
      session_read_prl_cnf_ptr->sspr_p_rev = mmgsdi_1x_prl[prl_index].prl_data_ptr->sspr_p_rev;
    }/* session_read_prl_data_ptr->prl_ptr != NULL */
    else
    {
      /* There is no Valid PRL cached for this session */
      UIM_MSG_HIGH_0("No valid PRL cached for this 1X Prov Session");
      session_read_prl_cnf_ptr->prl_version = 0;
      session_read_prl_cnf_ptr->size = 0;
      session_read_prl_cnf_ptr->valid = 0;
      session_read_prl_cnf_ptr->roaming_list_ptr = NULL;
      session_read_prl_cnf_ptr->sspr_p_rev = 0;
    }
  }

  return MMGSDI_SUCCESS;
}/* mmgsdi_cnf_build_session_read_prl_data */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_BS_CHAL_REQ_DATA

   DESCRIPTION:
     This function builds the CNF data for BS Chal request command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_bs_chal_req_data(
  mmgsdi_session_bs_chal_cnf_type                * bs_chal_cnf_ptr,
  mmgsdi_return_enum_type                          mmgsdi_status,
  const mmgsdi_data_type                         * rsp_data_ptr
)
{
  UIM_MSG_HIGH_1("mmgsdi_cnf_build_bs_chal_req_data status = 0x%x", mmgsdi_status);
  MMGSDIUTIL_RETURN_IF_NULL(bs_chal_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* Response data contains
       RANDBS            4 bytes */
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr);
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr->data_ptr);
    bs_chal_cnf_ptr->randbs =
             b_unpackd((byte*)(rsp_data_ptr->data_ptr),0,sizeof(dword)*8);
  }
  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_ms_key_req_data(
  mmgsdi_session_otasp_ms_key_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                               mmgsdi_status,
  const mmgsdi_data_type                              * rsp_data_ptr
)
{
  UIM_MSG_HIGH_1("mmgsdi_cnf_build_otasp_ms_key_req_data status = 0x%x",
                 mmgsdi_status);
  MMGSDIUTIL_RETURN_IF_NULL(otasp_cnf_ptr);

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr);
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr->data_ptr);
    /* Response data contains
       Result            1 byte */
    otasp_cnf_ptr->result = rsp_data_ptr->data_ptr[0];
  }

  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_key_gen_req_data(
  mmgsdi_session_otasp_key_gen_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                                mmgsdi_status,
  const mmgsdi_data_type                               * rsp_data_ptr
)
{
  UIM_MSG_HIGH_1("mmgsdi_cnf_build_otasp_key_gen_req_data status = 0x%x",
                 mmgsdi_status);

  MMGSDIUTIL_RETURN_IF_NULL(otasp_cnf_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr);
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr->data_ptr);
    /* Response data contains
       Result Code           1 byte
       MS Result Length      1 byte
       MS Result data        X byte */
    otasp_cnf_ptr->result                   = rsp_data_ptr->data_ptr[0];
    otasp_cnf_ptr->ms_result.data_len       = rsp_data_ptr->data_ptr[1];
    if (otasp_cnf_ptr->ms_result.data_len  > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(otasp_cnf_ptr->ms_result.data_ptr,
                                         otasp_cnf_ptr->ms_result.data_len);

      if(otasp_cnf_ptr->ms_result.data_ptr != NULL)
      {
        mmgsdi_memscpy(otasp_cnf_ptr->ms_result.data_ptr,
                       int32touint32(otasp_cnf_ptr->ms_result.data_len),
                       (byte *)(rsp_data_ptr->data_ptr +
                            MMGSDI_OTASP_KEY_GEN_RSP_FIX_LEN),
                       int32touint32(otasp_cnf_ptr->ms_result.data_len));
      }
      else
      {
         otasp_cnf_ptr->ms_result.data_len    = 0;
         return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
  }
  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:    The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_sspr_config_req_data(
  mmgsdi_session_otasp_sspr_config_cnf_type            * otasp_cnf_ptr,
  mmgsdi_return_enum_type                                mmgsdi_status,
  const mmgsdi_data_type                               * rsp_data_ptr
)
{
  UIM_MSG_HIGH_1("mmgsdi_cnf_build_otasp_sspr_config_req_data status = 0x%x",
                 mmgsdi_status);

  MMGSDIUTIL_RETURN_IF_NULL(otasp_cnf_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr);
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr->data_ptr);
    /* Response data contains
       Block ID              1 byte
       Result Code           1 byte
       Block Length          1 byte
       Param Data            X bytes  */

    otasp_cnf_ptr->block_id                 = rsp_data_ptr->data_ptr[0];
    otasp_cnf_ptr->result                   = rsp_data_ptr->data_ptr[1];
    otasp_cnf_ptr->block_data.data_len      = rsp_data_ptr->data_ptr[2];
    if (otasp_cnf_ptr->block_data.data_len > 0)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(otasp_cnf_ptr->block_data.data_ptr,
                                         otasp_cnf_ptr->block_data.data_len);

      if(otasp_cnf_ptr->block_data.data_ptr != NULL)
      {
        mmgsdi_memscpy(otasp_cnf_ptr->block_data.data_ptr,
                       int32touint32(otasp_cnf_ptr->block_data.data_len),
                       (byte *)(rsp_data_ptr->data_ptr +
                           MMGSDI_OTASP_SSPR_CONFIG_RSP_FIX_LEN),
                       int32touint32(otasp_cnf_ptr->block_data.data_len));
      }
      else
      {
         otasp_cnf_ptr->block_data.data_len    = 0;
         return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
  }
  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR  :  The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_otasp_otapa_req_data(
  mmgsdi_session_otasp_otapa_cnf_type                * otasp_cnf_ptr,
  mmgsdi_return_enum_type                              mmgsdi_status,
  const mmgsdi_data_type                             * rsp_data_ptr
)
{
  UIM_MSG_HIGH_1("MMGSDI_CNF_BUILD_OTASP_OTPA_REQ status = 0x%x", mmgsdi_status);
  MMGSDIUTIL_RETURN_IF_NULL(otasp_cnf_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* OTASP OTAPA Response data
       Result Code        1 byte
       NAM_LOCK Indicator 1 byte
       RAND OTAPA         4 bytes */
    MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr);
    otasp_cnf_ptr->result       = rsp_data_ptr->data_ptr[0];
    otasp_cnf_ptr->nam_lock_ind = rsp_data_ptr->data_ptr[1];
    otasp_cnf_ptr->rand_otapa   = b_unpackd(
                                     (byte*)(rsp_data_ptr->data_ptr + 2),
                                     0,
                                     sizeof(dword)*8);
  }
  return MMGSDI_SUCCESS;
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
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:  The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

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
  UIM_MSG_HIGH_1("mmgsdi_cnf_build_otasp_block_req_data status = 0x%x",
                 mmgsdi_status);

  MMGSDIUTIL_RETURN_IF_NULL_3(otasp_cnf_ptr, extra_param_ptr, rsp_data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(rsp_data_ptr->data_ptr);

  otasp_cnf_ptr->block_data.data_len        = 0;
  otasp_cnf_ptr->block_op_type              =
             extra_param_ptr->session_otasp_block_data.otasp_block_op_type;
  otasp_cnf_ptr->segment_size               = 0;

  if (mmgsdi_status == MMGSDI_SUCCESS)
  {
    /* No need to check the response data length, we always have */
    switch(otasp_cnf_ptr->block_op_type)
    {
      case MMGSDI_OTASP_CONFIG:
        /* OTASP Config Response data structure
          Block ID         1 byte
          Block Length     1 byte
          Result Code      1 byte
          Block parm data  depend on the block length.  */
        otasp_cnf_ptr->block_id             = rsp_data_ptr->data_ptr[0];
        otasp_cnf_ptr->block_data.data_len  = rsp_data_ptr->data_ptr[1];
        otasp_cnf_ptr->result               = rsp_data_ptr->data_ptr[2];
        if (otasp_cnf_ptr->block_data.data_len > 0)
        {
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(otasp_cnf_ptr->block_data.data_ptr,
                                             otasp_cnf_ptr->block_data.data_len);

          if(otasp_cnf_ptr->block_data.data_ptr != NULL)
          {
            mmgsdi_memscpy(otasp_cnf_ptr->block_data.data_ptr,
                           int32touint32(otasp_cnf_ptr->block_data.data_len),
                           (byte *)(rsp_data_ptr->data_ptr +
                             MMGSDI_OTASP_CONFIG_RSP_FIX_LEN),
                           int32touint32(otasp_cnf_ptr->block_data.data_len));
          }
          else
          {
            otasp_cnf_ptr->block_data.data_len= 0;
            return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
          }
        }
        break;

      case MMGSDI_OTASP_DOWNLOAD:
      case MMGSDI_OTASP_VALIDATE:
        /* OTASP Download response data structure
           Block ID       1 byte
           Result Code    1 byte.       */
        otasp_cnf_ptr->block_id             = rsp_data_ptr->data_ptr[0];
        otasp_cnf_ptr->result               = rsp_data_ptr->data_ptr[1];
        break;

      case MMGSDI_OTASP_SSPR_DOWNLOAD:
        /* OTASP SSPR Download response data structure
           Block ID              1 byte
           Result Code           1 byte
           Segment Offset        2 bytes
           Segment Size          1 byte    */
        otasp_cnf_ptr->block_id             = rsp_data_ptr->data_ptr[0];
        otasp_cnf_ptr->result               = rsp_data_ptr->data_ptr[1];
        otasp_cnf_ptr->segment_offset       =
                           b_unpackw(
                               (byte *)(rsp_data_ptr->data_ptr + 2),
                               0,
                               sizeof(word)*8);
        otasp_cnf_ptr->segment_size         = rsp_data_ptr->data_ptr[4];
        break;

      default:
        UIM_MSG_ERR_1("Invalid OTASP Block request 0x%x",
                      otasp_cnf_ptr->block_op_type);
        return MMGSDI_ERROR;
    }
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_otasp_block_req_data */

