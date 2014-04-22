/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                M M G S D I   O N C H I P   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the ONCHIP related functions.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2016 - 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_onchip.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
07/27/18   tq      Moving function to stub file
06/22/16   tkl     Stub out additional APIs
06/10/16   sp      Move onchip related function to onchip file
05/25/16   vdc     Remove F3 messages for memory allocation failure
05/24/16   ar      Review of macros used by MMGSDI
05/11/16   vdc     Feature guard with FEATURE_MMGSDI_3GPP2 for 3GPP2 related functions
04/26/16   dd      Initial revision

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmgsdi.h"
#include "uim_msg.h"
#include "mmgsdiutil.h"
#include "mmgsdicache.h"
#include "mmgsdi_common_rsp.h"
#include "mmgsdi_nv.h"
#include "mmgsdi_gen.h"
#include "mmgsdi_cnf.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_evt.h"
#ifdef FEATURE_SIMLOCK
#include "mmgsdi_simlock.h"
#endif /* FEATURE_SIMLOCK */

#ifdef FEATURE_LTE
#include "lte_aka_wrap.h"
#endif /* FEATURE_LTE */


/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Macros to set and check if a tag for USIM authenticate is present
   to avoid an invalid configuration in the onchip TLV */
#define MMGSDI_SET_AUTH_TAG_PRESENT(var, tag)    { (var) |= (uint32)(1 << (tag)); }
#define MMGSDI_IS_AUTH_TAG_PRESENT(var, tag)     ( (var) & (uint32)(1 << (tag)) )

/* Default value for the onchip USIM authenticate
   in case of GSM security context */
mmgsdi_len_type default_gsm_auth_rsp_len   = 14;
uint8           default_gsm_auth_rsp_ptr[] = { 0x04, 0x2B, 0x4D, 0x56,
                                               0x5D, 0x08, 0x02, 0x0B,
                                               0x01, 0x01, 0x19, 0x00,
                                               0x77, 0x67 };

/* Default value for the onchip USIM authenticate
   in case of 3G security context */
mmgsdi_len_type default_3g_auth_rsp_len    = 61;
uint8           default_3g_auth_rsp_ptr[]  = { 0xDB, 0x10, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x10, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x10,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x08, 0x00, 0x00, 0x00,
                                               0x00, 0x00, 0x00, 0x00,
                                               0x00 };

/*===========================================================================
FUNCTION MMGSDI_ONCHIP_INIT_AUTO_ACTIVATION_DATA

DESCRIPTION
  Reads the EFS item to figure out if the OnChip should be enabled on boot up
  or not. If it dictates that OnChip be auto activated on boot up, the EFS
  item contents are validated and are read into the MMGSDI global. This is done
  for all the instances of this EFS item (assuming the corresponding slot index
  is enabled via NV 70210.

DEPENDENCIES
  None

LIMITATIONS
  None

RETURN VALUE:
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_onchip_init_auto_activation_data (
  uint8 index
)
{
  int                                      efs_data_len         = 0;
  mmgsdi_app_enum_type                     mmgsdi_app_type      = MMGSDI_APP_NONE;
  boolean                                  auto_act_enabled     = FALSE;
  uint8                                    efs_slot_index       = MMGSDI_SLOT_1_INDEX;
  uim_common_efs_context_type              efs_context          = UIM_COMMON_EFS_CONTEXT_MAX;

  struct
  {
    uint8   auto_activation_enabled;
    uint8   app_type;
    uint8   slot_id;
    uint8   rfu[13];
  } onchip_auto_act_data;

  efs_context = mmgsdi_util_get_common_efs_item_context(index);
  if (efs_context == UIM_COMMON_EFS_CONTEXT_MAX)
  {
    return;
  }

  if(mmgsdi_generic_data_ptr == NULL)
  {
    return;
  }

  memset(&onchip_auto_act_data, 0x00, sizeof(onchip_auto_act_data));

  /* Lets parse the EFS data; we copy it to our global only if it is valid */
  if (uim_common_efs_read(UIM_COMMON_EFS_MMGSDI_ONCHIP_SUPPORT,
                          UIM_COMMON_EFS_ITEM_FILE_TYPE,
                          efs_context,
                          (uint8 *)&onchip_auto_act_data,
                          sizeof(onchip_auto_act_data)) !=
        UIM_COMMON_EFS_SUCCESS)
  {
    UIM_MSG_ERR_1("Invalid onchip EFS item len:0x%x", efs_data_len);
    return;
  }

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(onchip_auto_act_data.slot_id,
                                                  &efs_slot_index) ||
     index != efs_slot_index)
  {
    return;
  }

  /* Validate the input that indicates whether onchip auto activation is
     enabled or not.  */
  switch(onchip_auto_act_data.auto_activation_enabled)
  {
    case 1:
      auto_act_enabled = TRUE;
      break;
    case 0:
    default:
      /* EFS item suggested that onchip auto-activation should not be performed.
         No need to validate rest of the contents of the EFS item */
      return;
  }

  /* Validate app type from the EFS item. OnChip currently supported only
     for SIM and USIM */
  switch(onchip_auto_act_data.app_type)
  {
    case 1:
      mmgsdi_app_type = MMGSDI_APP_SIM;
      break;
    case 3:
      mmgsdi_app_type = MMGSDI_APP_USIM;
      break;
    default:
      UIM_MSG_ERR_1("Invalid app type mentioned in OnChip EFS item: 0%x",
                    onchip_auto_act_data.app_type);
      return;
  }

  mmgsdi_generic_data_ptr->onchip_act_data[index].auto_activation_enabled =
    auto_act_enabled;
  mmgsdi_generic_data_ptr->onchip_act_data[index].app_type =
    mmgsdi_app_type;
} /* mmgsdi_onchip_init_auto_activation_data */


/*===========================================================================
  FUNCTION:      MMGSDI_ONCHIP_UICC_AUTH

  DESCRIPTION:
    This function executes the onchip usim authentication

  DEPENDENCIES:
    None

  LIMITATIONS:
    This function is limited to the use of UICC ie. (technologies of type UICC only).

  RETURN VALUE:
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_onchip_uicc_auth (
  const mmgsdi_auth_req_type   *req_ptr
)
{
  uint8                              index           = 0;
  mmgsdi_return_enum_type            mmgsdi_status   = MMGSDI_SUCCESS;
  uim_rpt_type                       uim_report;
  mmgsdi_slot_data_type             *slot_data_ptr   = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);
  memset(&uim_report, 0x00, sizeof(uim_rpt_type));

  /* Check context: in onchip mode, we support only GSM and 3G contexts */
  if (req_ptr->auth_context != MMGSDI_AUTN_USIM_GSM_CONTEXT &&
      req_ptr->auth_context != MMGSDI_AUTN_USIM_3G_CONTEXT)
  {
    UIM_MSG_ERR_1("Invalid context value: 0x%x", req_ptr->auth_context);
    return MMGSDI_ERROR;
  }

  /* Populate the extra param */
  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_USIM_AUTH_REQ,
                                                           (void*)req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_ptr->request_header.slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* populate the necessary field in the uim_report_type */
  if (slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW1_OFFSET] !=
      (uint8)SW1_NO_STATUS)
  {
    /* Error response */
    uim_report.sw1 =
      (uim_sw1_type)slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW1_OFFSET];
    uim_report.sw2 =
      (uim_sw2_type)slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW2_OFFSET];
    uim_report.rpt_status = UIM_FAIL;
  }
  else if (slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len > 0 &&
           slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_ptr != NULL)
  {
    /* AUTS response */
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    if (slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len <= UIM_MAX_CHARS)
    {
      mmgsdi_memscpy(uim_report.rpt.autn.data,
        sizeof(uim_report.rpt.autn.data),
        slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_ptr,
        int32touint32(slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len));
      uim_report.rpt.autn.data_length =
        int32touint16(slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp.data_len);
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }
  }
  else if (slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len > 0 &&
           slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_ptr != NULL &&
           req_ptr->auth_context == MMGSDI_AUTN_USIM_3G_CONTEXT)
  {
    /* 3G security context */
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    if (slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len <= UIM_MAX_CHARS)
    {
      mmgsdi_memscpy(uim_report.rpt.autn.data,
        sizeof(uim_report.rpt.autn.data),
        slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_ptr,
        int32touint32(slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len));
      uim_report.rpt.autn.data_length =
        int32touint16(slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp.data_len);
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }
  }
  else if (slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len > 0 &&
           slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_ptr != NULL &&
           req_ptr->auth_context == MMGSDI_AUTN_USIM_GSM_CONTEXT)
  {
    /* GSM security context */
    uim_report.sw1        = SW1_NORMAL_END;
    uim_report.sw2        = SW2_NORMAL_END;
    uim_report.rpt_status = UIM_PASS;

    if (slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len <= UIM_MAX_CHARS)
    {
      mmgsdi_memscpy(uim_report.rpt.autn.data,
        sizeof(uim_report.rpt.autn.data),
        slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_ptr,
        int32touint32(slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len));
      uim_report.rpt.autn.data_length =
        int32touint16(slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp.data_len);
    }
    else
    {
      mmgsdi_util_free_client_request_table_index(index);
      return MMGSDI_ERROR;
    }
  }
  else
  {
    /* Dafault case */
    mmgsdi_util_free_client_request_table_index(index);
    return MMGSDI_ERROR;
  }

  /* Set the other values in uim_report */
  uim_report.rpt_type            = UIM_AUTHENTICATE_R;
  uim_report.cmd_transacted      = FALSE;
  uim_report.user_data           = index;

  /* Queue response */
  mmgsdi_uim_report(&uim_report);

  return mmgsdi_status;
} /*  mmgsdi_onchip_uicc_auth */


#ifdef FEATURE_MMGSDI_3GPP
/*===========================================================================
  FUNCTION:      MMGSDI_ONCHIP_ICC_POPULATE_RUN_GSM_DATA

  DESCRIPTION
    Function used to populate RUN GSM Algorithm data into the buffer
    passed in, when the Onchip SIM feature is enabled and the
    Functionality is active.

  DEPENDENCIES
    None

  LIMITATIONS:
    This function is limited to the use of ICC card.

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_onchip_icc_populate_run_gsm_data (
  const uint8              *rand_ptr,
  uint8                    *data_ptr,
  mmgsdi_slot_id_enum_type  slot_id
)
{
  uint8                 *sres_ptr      = NULL;
  uint8                 *kc_ptr        = NULL;
  int                    i             = 0;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  if (rand_ptr == NULL || data_ptr == NULL)
  {
    UIM_MSG_HIGH_2("NULL rand_ptr: 0x%x or data_ptr: 0x%x", rand_ptr, data_ptr);
    return MMGSDI_ERROR;
  }

  /* Assign the rand_ptr, sres_ptr, and kc_ptr */
  sres_ptr = data_ptr;
  kc_ptr   = &data_ptr[MMGSDI_GSM_ALGO_SRES_LEN];

  UIM_MSG_HIGH_0("Onchip SIM Authentication with Rand");

  while ( i < MMGSDI_RAND_LEN)
  {
    UIM_MSG_HIGH_2("RAND[%d] = 0x%x", i, rand_ptr[i]);
    i++;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);

  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }


  /* RESET i */

  i=0;
  while ( i < MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN)
  {
    UIM_MSG_HIGH_2("ONCHIP SIM KEY[%d] = 0x%x",
                   i, slot_data_ptr->onchip_sim_data.auth_key[i]);
    i++;
  }

  /* RESET i and Perform the Auth using an XOR Algorithm */
  i = 0;
  while (i < MMGSDI_GSM_ALGO_SRES_LEN + MMGSDI_GSM_ALGO_KC_LEN)
  {
    if ( i < MMGSDI_GSM_ALGO_SRES_LEN )
    {
      /* The first 4 bytes, index 0 to 3 are populated with
      ** the SRES Value
      */
      sres_ptr[i] = rand_ptr[i] ^ slot_data_ptr->onchip_sim_data.auth_key[i];
      UIM_MSG_HIGH_2("SRES[%d] = 0x%x", i, sres_ptr[i]);
    }
    else
    {
      /* The next 8 bytes are populated with the KC Value */
      kc_ptr[i] = rand_ptr[i] ^ slot_data_ptr->onchip_sim_data.auth_key[i];
      UIM_MSG_HIGH_2("KC[%d] = 0x%x", i, kc_ptr[i]);
    }
    i++;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_onchip_icc_populate_run_gsm_data */
#endif  /* FEATURE_MMGSDI_3GPP */


/*===========================================================================
FUNCTION MMMGSDI_ONCHIP_VALIDATE_CONFIG

DESCRIPTION
  This function is used to ensure all components of the TLV DATA are
  properly set.  This is a validation function only.


DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:  MMGSDI_SUCCESS:  Properly Built TLV
                            MMGSDI_ERROR:    Improperly Built TLV

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_onchip_validate_config(
  const   mmgsdi_onchip_sim_config_data_type * onchip_config_data_ptr,
  mmgsdi_onchip_mode_enum_type                 onchip_sim_mode
)
{
  uint16     index        = 0;
  uint16     len          = 0;
  uint16     tlv_len      = 0;
  uint16     offset       = 0;
  uint8      TAG_SIZE     = 1;
  uint8      LEN_SIZE     = 1;
  uint8     *tlv_data_ptr = NULL;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr);
  if ( onchip_config_data_ptr->data_len == 0 )
  {
    /* A Zero Length is allowed for the purpose of
    ** getting to an idle screen but the call stacks
    ** will be stuck in limited service
    */
    return MMGSDI_SUCCESS;
  }
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr->data_ptr);

  if (onchip_config_data_ptr->data_len < MMGSDI_ONCHIP_TLV_PAYLOAD ||
      onchip_config_data_ptr->data_ptr[0] != MMGSDI_ACT_ONCHIP_TLV_START_TAG)
  {
    /* onchip data(TLV) must atleast have a master tag and two bytes for
       length of the value field of onchip data */
    return MMGSDI_ERROR;
  }

  /* byte 2 and 3 contain the length of the master TLV */
  tlv_len = onchip_config_data_ptr->data_ptr[1] + (onchip_config_data_ptr->data_ptr[2] * 0x100);
  tlv_data_ptr = onchip_config_data_ptr->data_ptr + MMGSDI_ONCHIP_TLV_PAYLOAD;
  index = MMGSDI_ONCHIP_TLV_PAYLOAD;

  while (index < onchip_config_data_ptr->data_len )
  {
    /* Initialize the offset to 0 */
    offset = 0;

    switch ( *tlv_data_ptr )
    {
      case MMGSDI_ACT_ONCHIP_CK_TAG:
      case MMGSDI_ACT_ONCHIP_IK_TAG:
      case MMGSDI_ACT_ONCHIP_EPSLOCI_TAG:
      case MMGSDI_ACT_ONCHIP_HPLMNWACT_TAG:
      case MMGSDI_ACT_ONCHIP_PLMNWACT_TAG:
      case MMGSDI_ACT_ONCHIP_UST_TAG:
      case MMGSDI_ACT_ONCHIP_EST_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG:
      case MMGSDI_ACT_ONCHIP_EPSNSC_TAG:
      case MMGSDI_ACT_ONCHIP_KEYSPS_TAG:
      case MMGSDI_ACT_ONCHIP_KEYS_TAG:
      case MMGSDI_ACT_ONCHIP_RPLMNACT_TAG:
        /* These tags are valid only in case of usim */
        if (onchip_sim_mode != MMGSDI_ONCHIP_MODE_USIM)
        {
          MMGSDI_DEBUG_MSG_ERROR_1("Invalid tag for onchip SIM: 0x%x",
                                   *tlv_data_ptr);
          return MMGSDI_ERROR;
        }
        /* continue without breaking because the checks below
           apply to both SIM and USIM tags */

      case MMGSDI_ACT_ONCHIP_AUTH_KEY_TAG:
      case MMGSDI_ACT_ONCHIP_IMSI_TAG:
      case MMGSDI_ACT_ONCHIP_LOCI_TAG:
      case MMGSDI_ACT_ONCHIP_KC_TAG:
      case MMGSDI_ACT_ONCHIP_PLMN_TAG:
      case MMGSDI_ACT_ONCHIP_SST_TAG:
      case MMGSDI_ACT_ONCHIP_ACC_TAG:
      case MMGSDI_ACT_ONCHIP_FPLMN_TAG:
      case MMGSDI_ACT_ONCHIP_PHASE_TAG:
      case MMGSDI_ACT_ONCHIP_AD_TAG:
      case MMGSDI_ACT_ONCHIP_PLMNSEL_TAG:
      case MMGSDI_ACT_ONCHIP_ICCID_TAG:
        /* Validate the number of bytes remainging are valid
        ** in the TLV.  -1 for the purpose of converting
        ** entire data length to respective index
        */
        if ( index >= ( onchip_config_data_ptr->data_len - LEN_SIZE ) )
        {
          MMGSDI_DEBUG_MSG_ERROR_0("Invalid TLV Format...No Length");
          return MMGSDI_ERROR;
        }

        /* move the index to the 1 Byte Length past the Tag*/
        tlv_data_ptr++;

        offset = *tlv_data_ptr + LEN_SIZE;

        /* Move the tlv_data_ptr to the next Tag */
        tlv_data_ptr += offset;

        /* Now add the Length to the Offset */
        offset += TAG_SIZE;

        /* Update the Length accordingly as well */
        len += offset;

        index += offset;
        break;

      default:
        MMGSDI_DEBUG_MSG_ERROR_1("Unhandled Tag: 0x%x",
                                 ((*tlv_data_ptr - offset) - TAG_SIZE));
        return MMGSDI_ERROR;
    }
  }

  /* --------------------------------------------------------------------------
     Check to ensure the TLV Len is equal to the SUM of all Tags and Data
     contained within this config tlv
     ------------------------------------------------------------------------*/
  if ((tlv_len != len) ||
      (tlv_len + MMGSDI_ONCHIP_TLV_PAYLOAD != onchip_config_data_ptr->data_len))
  {
    MMGSDI_DEBUG_MSG_ERROR_3("BAD TLV:  TLV LEN=0x%x, LEN=0x%x, TOTAL LEN=0x%x",
                             tlv_len, len, onchip_config_data_ptr->data_len);
    return MMGSDI_ERROR;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_onchip_validate_config */


/* ============================================================================
  FUNCTION:      MMGSDI_ONCHIP_SEND_REFRESH_EVT_FOR_ONCHIP_ACT

  DESCRIPTION:
    This function is called to send out the REFRESH RESET events to clients
    before the onchip acitvation is started.

  DEPENDENCIES:
    None

  LIMITATIONS:
    Should be used only for sending out the REFRESH RESET or RESET_AUTO events

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
============================================================================*/
static void mmgsdi_onchip_send_refresh_evt_for_onchip_act(
  mmgsdi_event_data_type   * evt_info_ptr,
  mmgsdi_slot_id_enum_type   mmgsdi_slot
)
{
  mmgsdi_session_info_type       *session_info_ptr = NULL;
  mmgsdi_slot_id_enum_type        slot             = MMGSDI_MAX_SLOT_ID_ENUM;
  uint32                          i                = 0;
  uint32                          j                = 0;
  mmgsdi_len_type                 num_sessions     = 0;
  mmgsdi_return_enum_type         mmgsdi_status    = MMGSDI_ERROR;

  MMGSDIUTIL_CHECK_NON_GSDI_TASK_CLIENT_APP_DATA_CRIT_SECT_ACCESS;

  if(evt_info_ptr == NULL)
  {
    return;
  }

  for(i=0; i<MMGSDI_MAX_CLIENT_INFO; i++)
  {
    if(mmgsdi_client_id_reg_table[i] == NULL)
    {
      continue;
    }

    num_sessions     = mmgsdi_client_id_reg_table[i]->num_session;

    /* Scan through all open sessions and send out the REFRESH event to
       valid sessions on slot 1*/
    for(j=0; (num_sessions > 0) && (j < MMGSDI_MAX_SESSION_INFO); j++)
    {
      session_info_ptr =
        mmgsdi_client_id_reg_table[i]->session_info_ptr[j];

      if(session_info_ptr == NULL)
      {
        /*Invalid Session, Continue to next*/
        continue;
      }
      num_sessions--;

      mmgsdi_status = mmgsdi_util_get_session_app_info(
                        session_info_ptr->session_id,
                        &slot,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        NULL);

      if(mmgsdi_status != MMGSDI_SUCCESS || slot != mmgsdi_slot)
      {
        continue;
      }
      /* Fetch the app type from current app */
      if((session_info_ptr->channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index] != NULL) &&
         (mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr != NULL))
      {
        evt_info_ptr->data.refresh.aid.app_type =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.app_type;
        evt_info_ptr->data.refresh.app_info.app_data.app_type =
          mmgsdi_channel_info_ptr_table[session_info_ptr->channel_info_index]->app_info_ptr->app_data.app_type;
      }
      mmgsdi_evt_notify_specific_session_id(session_info_ptr->session_id,
                                            evt_info_ptr);
    }
  }
} /* mmgsdi_onchip_send_refresh_evt_for_onchip_act */

/* ============================================================================
   FUNCTION:      MMGSDI_ONCHIP_CLEAR_USIM_AUTHENTICATE

   DESCRIPTION:
     This function clears the authenticate responses for onchip

   DEPENDENCIES:
     None

   LIMITATIONS:
     This function is limited to the use of UICC

   RETURN VALUE:
     None

   SIDE EFFECTS:

============================================================================*/
static void mmgsdi_onchip_clear_usim_authenticate(
   mmgsdi_slot_id_enum_type              slot_id
)
{
  mmgsdi_data_type * auth_rsp_ptr = NULL;
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("slot_data_ptr is NULL");
    return;
  }

  /* Clear data for authenticate algorithm */
  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

    auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data;
  auth_rsp_ptr->data_len = 0;
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

  slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW1_OFFSET] =
    (uint8)SW1_NO_STATUS;
  slot_data_ptr->onchip_sim_data.usim_auth_error_rsp[GSDI_SW2_OFFSET] =
    (uint8)SW2_NORMAL_END;

  slot_data_ptr->onchip_sim_data.usim_auth_algo =
    MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED;
  slot_data_ptr->onchip_sim_data.usim_auth_algo_res_length = 0;
  slot_data_ptr->onchip_sim_data.usim_auth_milenage_seq = 0;
} /* mmgsdi_onchip_clear_usim_authenticate */


/* ============================================================================
  FUNCTION:      MMGSDI_ONCHIP_POPULATE_CACHE

  DESCRIPTION:
    This function is called to populate the USIM Cache with the configuration
    data provided in the ACTIVATE_ONCHIP_SIM Command.

  DEPENDENCIES:
    Can only be called after the mmgsdi_onchip_validate_config()

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

============================================================================*/
static mmgsdi_return_enum_type mmgsdi_onchip_populate_cache(
  const mmgsdi_onchip_sim_config_data_type * onchip_config_data_ptr,
  mmgsdi_slot_id_enum_type                   slot_id,
  mmgsdi_session_type_enum_type              session_type,
  mmgsdi_onchip_mode_enum_type               onchip_mode
)
{
  uint16                  index            = 0;
  uint8                   len              = 0;
  uint8                   tag              = 0;
  uint16                  tlv_len          = 0;
  uint8                   tag_size         = 1;
  uint8                   len_size         = 1;
  uint8                 * tlv_data_ptr     = NULL;
  mmgsdi_file_enum_type   file_name        = MMGSDI_NO_FILE_ENUM;
  mmgsdi_data_type      * auth_rsp_ptr     = NULL;
  uint32                  auth_tag_present = 0;
  mmgsdi_return_enum_type mmgsdi_status    = MMGSDI_SUCCESS;
  mmgsdi_slot_data_type  *slot_data_ptr    = NULL;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr);
  MMGSDIUTIL_RETURN_IF_NULL(onchip_config_data_ptr->data_ptr);

  /* --------------------------------------------------------------------------
     Assign the Starting Point for the Data
     ------------------------------------------------------------------------*/
  tlv_data_ptr = onchip_config_data_ptr->data_ptr;

  /* --------------------------------------------------------------------------
     Increment the data to get the TLV LEN which is broken into two bytes
     ------------------------------------------------------------------------*/
  tlv_data_ptr++;
  tlv_len = *tlv_data_ptr;
  tlv_data_ptr++;
  tlv_len += *tlv_data_ptr * 0x100;

  /* --------------------------------------------------------------------------
     Move the pointer to the next Tag and start populating the Cache
     ------------------------------------------------------------------------*/
  tlv_data_ptr++;
  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    UIM_MSG_ERR_0("slot_data_ptr is NULL");
    return MMGSDI_ERROR;
  }

  while ( index < tlv_len )
  {
    /* Check TLV size*/
    if ( index + tag_size + len_size > tlv_len )
    {
      MMGSDI_DEBUG_MSG_ERROR_0("Invalid TLV data");
      return MMGSDI_ERROR;
    }

    /* Copy the tag and move the pointer to the len */
    tag = *tlv_data_ptr;
    tlv_data_ptr = tlv_data_ptr + tag_size;

    /* Copy the length and move the pointer to the data (if exists)*/
    len = *tlv_data_ptr;
    tlv_data_ptr = tlv_data_ptr + len_size;

    /* Check TLV size*/
    if ( index + tag_size + len_size + len > tlv_len )
    {
      MMGSDI_DEBUG_MSG_ERROR_0("Invalid TLV data");
      return MMGSDI_ERROR;
    }

    switch ( tag )
    {
      case MMGSDI_ACT_ONCHIP_AUTH_KEY_TAG:
        if (len > 0 && len <= MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN)
        {
          mmgsdi_memscpy(slot_data_ptr->onchip_sim_data.auth_key,
            sizeof(slot_data_ptr->onchip_sim_data.auth_key),
            tlv_data_ptr,
            (uint32)len);

          /* Move the TLV Data Pointer Past the Auth Key to
          ** the next TAG
          */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else if (len > MMGSDI_ONCHIP_SIM_AUTH_KEY_LEN)
        {
          MMGSDI_DEBUG_MSG_ERROR_2("INVALID AUTH KEY LEN 0x%x > 0x%x",
                                   len, MMGSDI_ONCHIP_USIM_AUTH_KEY_LEN);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG:
      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG:
        if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_auts_rsp;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG);
        }
        else if (tag == (uint8)MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG)
        {
          auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_MILENAGE_OP_TAG);
        }

        /* Zero old value */
        auth_rsp_ptr->data_len = 0;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK( auth_rsp_ptr->data_ptr );

        if ( len > 0 )
        {
          /* Not validating the payload because invalid responses might
             be used for adverse scenario test cases */

          /* Allocate buffer */
          MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(auth_rsp_ptr->data_ptr,
                                             (uint32)len);

          if (auth_rsp_ptr->data_ptr == NULL)
          {
            mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
            break;
          }

          /* Copy response in buffer */
          auth_rsp_ptr->data_len = len;
          mmgsdi_memscpy(auth_rsp_ptr->data_ptr,
                         len,
                         tlv_data_ptr,
                         (uint32)len);

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG:
        if(len == sizeof(slot_data_ptr->onchip_sim_data.usim_auth_error_rsp))
        {
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG);

          mmgsdi_memscpy(slot_data_ptr->onchip_sim_data.usim_auth_error_rsp,
            sizeof(slot_data_ptr->onchip_sim_data.usim_auth_error_rsp),
            tlv_data_ptr,
            (uint32)len);

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID AUTH ERR LEN 0x%x", len);
          return MMGSDI_ERROR;
        }
        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG:
        if ( len == sizeof(uint8) )
        {
          uint8 algorithm = 0;
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ALGO_TAG);

          algorithm = *tlv_data_ptr;

          if (algorithm == (uint8)MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED ||
              algorithm == (uint8)MMGSDI_ONCHIP_AUTH_ALGORITHM_XOR   ||
              algorithm == (uint8)MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE)
          {
            slot_data_ptr->onchip_sim_data.usim_auth_algo =
              (mmgsdi_onchip_auth_algo_enum_type)algorithm;

            /* Move the TLV Data Pointer to the next TAG */
            tlv_data_ptr = tlv_data_ptr + len;
          }
          else
          {
            MMGSDI_DEBUG_MSG_ERROR_1("INVALID ALGORITHM 0x%x", algorithm);
            return MMGSDI_ERROR;
          }
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID ALGO LEN 0x%x", len);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG:
        if ( len == sizeof(uint8) )
        {
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_RES_LEN_TAG);

          slot_data_ptr->onchip_sim_data.usim_auth_algo_res_length = *tlv_data_ptr;

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID RES LEN 0x%x", len);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);
        continue;

      case MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG:
        if ( len == sizeof(uint64) )
        {
          MMGSDI_SET_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_MILENAGE_SEQ_TAG);

          slot_data_ptr->onchip_sim_data.usim_auth_milenage_seq = (uint64) *tlv_data_ptr;

          /* Move the TLV Data Pointer to the next TAG */
          tlv_data_ptr = tlv_data_ptr + len;
        }
        else
        {
          MMGSDI_DEBUG_MSG_ERROR_1("INVALID SEQ LEN 0x%x", len);
          return MMGSDI_ERROR;
        }

        index += (tag_size + len + len_size);

        continue;

      case MMGSDI_ACT_ONCHIP_IMSI_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_IMSI : MMGSDI_GSM_IMSI;
        if(len > 0)
        {
          /* Make sure that IMSI is one of the test IMSIs. For all other IMSIs,
             return ERROR so that we do not end up activating onchip for fake
             IMSIs (to avoid a situation where someone can for example make an
             emergency call using this fake IMSI) */
          mmgsdi_data_type        tlv_imsi_data;
          mmgsdi_imsi_data_type   imsi             = {{0}};

          memset(&tlv_imsi_data, 0, sizeof(mmgsdi_data_type));

          tlv_imsi_data.data_len = tlv_data_ptr[0];
          tlv_imsi_data.data_ptr = tlv_data_ptr;

          mmgsdi_status = mmgsdi_util_convert_to_imsi_data_type(&tlv_imsi_data,
                                                                &imsi);
          if(mmgsdi_status == MMGSDI_SUCCESS &&
             FALSE == mmgsdi_card_init_is_imsi_test_imsi(imsi))
          {
            mmgsdi_status = MMGSDI_ERROR;
          }
        }
        break;

      case MMGSDI_ACT_ONCHIP_LOCI_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_LOCI : MMGSDI_GSM_LOCI;
        break;

      case MMGSDI_ACT_ONCHIP_ACC_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_ACC : MMGSDI_GSM_ACC;
        break;

      case MMGSDI_ACT_ONCHIP_FPLMN_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_FPLMN : MMGSDI_GSM_FPLMN;
        break;

      case MMGSDI_ACT_ONCHIP_AD_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_AD : MMGSDI_GSM_AD;
        break;

      case MMGSDI_ACT_ONCHIP_EPSLOCI_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_EPSLOCI : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_HPLMNWACT_TAG:         /* HPLMN selector with access tech   */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_HPLMNWACT : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_PLMNWACT_TAG:          /* User Controlled PLMN Selector     */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_PLMNWACT : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_UST_TAG:               /* USIM SERVICE TABLE                */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_UST : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_EST_TAG:               /* USIM ENABLE SERVICE TABLE         */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_EST : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_EPSNSC_TAG:            /* EPS NAS Security Context          */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_EPSNSC : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_KEYSPS_TAG:            /* C and I keys for PS domain        */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_KEYSPS : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_KEYS_TAG:              /* C and I keys                      */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_KEYS : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_RPLMNACT_TAG:          /* RPLMN with access tech            */
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                    MMGSDI_USIM_RPLMNACT : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_KC_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_KC : MMGSDI_USIM_KC;
        break;

      case MMGSDI_ACT_ONCHIP_PLMN_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_PLMN : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_SST_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_SST : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_PHASE_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_PHASE : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_PLMNSEL_TAG:
        file_name = (onchip_mode == MMGSDI_ONCHIP_MODE_SIM) ?
                    MMGSDI_GSM_HPLMN : MMGSDI_NO_FILE_ENUM;
        break;

      case MMGSDI_ACT_ONCHIP_ICCID_TAG:             /* ICCID                             */
        file_name = MMGSDI_ICCID;
        break;

      default:
        MMGSDI_DEBUG_MSG_ERROR_1("Unhandled TAG 0x%x", *tlv_data_ptr);
        mmgsdi_status = MMGSDI_ERROR;
        break;
    }

    if ( mmgsdi_status != MMGSDI_SUCCESS )
    {
      break;
    }

    if ( len > 0 )
    {
      if (file_name == MMGSDI_NO_FILE_ENUM)
      {
        return MMGSDI_INCORRECT_PARAMS;
      }

      mmgsdi_status = mmgsdi_cache_write_item(file_name,
                                              (onchip_mode == MMGSDI_ONCHIP_MODE_USIM) ?
                                                MMGSDI_APP_USIM : MMGSDI_APP_SIM,
                                              session_type,
                                              len,
                                              0,
                                              tlv_data_ptr,
                                              MMGSDI_DATA_FROM_CARD_COMPLETE);

      if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR_0("Failed to Write Cache Data: 0x%x");
        break;
      }

      /* Move to the next Tag */
      tlv_data_ptr += len;
    }

    /* Increase the index by the size of the TAG, LEN SIZE, and DATA LEN */
    index += (tag_size + len + len_size);
  }

  if (onchip_mode == MMGSDI_ONCHIP_MODE_SIM)
  {
    /* Nothing further to be done if the onchip mode is SIM */
    return mmgsdi_status;
  }

  /* Check on the onchip usim authenticate tags
      - If error is present, no other tag should be present
      - If AUTS is present, no other tag should be present
      - It is possible to have 3G context and GSM context at same time */
  if (
      (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG) &&
       (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG) ||
        MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG) ||
        MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG)))
      ||
      (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG) &&
       (MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG) ||
        MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG)))
     )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Too many onchip authenticate tags");
    mmgsdi_status = MMGSDI_ERROR;
  }
/* Check on the onchip usim authenticate tags for milenage and xor
      - Root key must be present */
  if (
      (slot_data_ptr->onchip_sim_data.usim_auth_algo == MMGSDI_ONCHIP_AUTH_ALGORITHM_XOR ||
       slot_data_ptr->onchip_sim_data.usim_auth_algo == MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE)
      &&
      (!MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ROOT_KEY_TAG))
     )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Root key is missing");
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* Default values for USIM authenticate
     They are used only if:
      - error and AUTS are not present in the TLV
      - values for GSM context or 3G context were not present in the TLV */
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      slot_data_ptr->onchip_sim_data.usim_auth_algo == MMGSDI_ONCHIP_AUTH_ALGORITHM_FIXED &&
      !MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_ERR_RSP_TAG) &&
      !MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_AUTS_RSP_TAG))
  {
    if (!MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_GSM_RSP_TAG))
    {
      uint32 default_gsm_auth_rsp_len_uint32 = int32touint32(default_gsm_auth_rsp_len);
      auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_gsm_rsp;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(auth_rsp_ptr->data_ptr,
        default_gsm_auth_rsp_len_uint32);
      if (auth_rsp_ptr->data_ptr != NULL)
      {
        /* Copy response in buffer */
        auth_rsp_ptr->data_len = default_gsm_auth_rsp_len;
        mmgsdi_memscpy(auth_rsp_ptr->data_ptr,
          default_gsm_auth_rsp_len_uint32,
          default_gsm_auth_rsp_ptr,
          int32touint32(default_gsm_auth_rsp_len));
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
    if (!MMGSDI_IS_AUTH_TAG_PRESENT(auth_tag_present, (int)MMGSDI_ACT_ONCHIP_USIM_AUTH_3G_RSP_TAG))
    {
      uint32 default_3g_auth_rsp_len_uint32 = int32touint32(default_3g_auth_rsp_len);
      auth_rsp_ptr = &slot_data_ptr->onchip_sim_data.usim_auth_3g_rsp;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(auth_rsp_ptr->data_ptr,
                                         default_3g_auth_rsp_len_uint32);
      if (auth_rsp_ptr->data_ptr != NULL)
      {
        /* Copy response in buffer */
        auth_rsp_ptr->data_len = default_3g_auth_rsp_len;
        mmgsdi_memscpy(auth_rsp_ptr->data_ptr,
          default_3g_auth_rsp_len_uint32,
          default_3g_auth_rsp_ptr,
          int32touint32(default_3g_auth_rsp_len));
      }
      else
      {
        mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      }
    }
  }

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Clear data for authenticate algorithm */
    mmgsdi_onchip_clear_usim_authenticate(slot_id);
  }

  return mmgsdi_status;
} /* mmgsdi_onchip_populate_cache */


/* ============================================================================
   FUNCTION:      MMGSDI_ONCHIP_ACTIVATE_RSP

   DESCRIPTION:
     This function will build the response to ACTIVATE ONCHIP REQ

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command structure was properly generated
                              and queued onto the MMGSDI Command Queue.
     MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                              within appropriate ranges.

   SIDE EFFECTS:

============================================================================*/
static mmgsdi_return_enum_type  mmgsdi_onchip_activate_rsp(
  mmgsdi_return_enum_type                     mmgsdi_status,
  const mmgsdi_activate_onchip_sim_req_type * onchip_sim_config_ptr
)
{
  mmgsdi_sw_status_type   status_word;

  UIM_MSG_HIGH_1("MMGSDI_PROCESS_ACTIVATE_ONCHIP_RSP status 0x%x", mmgsdi_status);

  /* Status word not applicable to Onchip command */
  status_word.sw1 = 0x00;
  status_word.sw2 = 0x00;

  MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config_ptr);

  return mmgsdi_cnf_build_and_queue(mmgsdi_status,
                                    &onchip_sim_config_ptr->request_header,
                                    NULL,
                                    NULL,
                                    FALSE,/* sw not required for onchip */
                                    status_word);
} /* mmgsdi_onchip_activate_rsp */


/*===========================================================================
FUNCTION MMGSDI_UTIL_TIMED_SLEEP

DESCRIPTION
  This function is a way to do a timed sleep using mmgsdi_wait with a
  non-deferrable timer. Note that it also pets the dog in the meantime.

DEPENDENCIES
  None

RETURN VALUE
  None
===========================================================================*/
static void mmgsdi_util_timed_sleep(
  unsigned long millisec)
{
#ifndef FEATURE_UIM_TEST_FRAMEWORK
  rex_sigs_type          sigs        = 0x00;
  mmgsdi_task_enum_type  mmgsdi_task = MMGSDI_TASK_MAX;
  rex_timer_type         mmgsdi_timed_sleep_timer;
  timer_group_type       mmgsdi_always_on_timer_group;

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       return */
    return;
  }

  memset(&mmgsdi_always_on_timer_group, 0x00, sizeof(timer_group_type));

  /* Define the timer */
  timer_def(&mmgsdi_timed_sleep_timer, &mmgsdi_always_on_timer_group,
            rex_self(), MMGSDI_TIMED_SLEEP_TIMER_SIG, NULL, 0);

  (void)rex_clr_sigs(rex_self(), MMGSDI_TIMED_SLEEP_TIMER_SIG);

  /* Set Sleep Timer per requested value */
  (void)rex_set_timer(&mmgsdi_timed_sleep_timer, millisec);

  /* We now need to wait until the sleep timer sig is set,
     petting the dog during the wait */
  sigs = mmgsdi_wait(MMGSDI_TIMED_SLEEP_TIMER_SIG);
  if(sigs & MMGSDI_TIMED_SLEEP_TIMER_SIG)
  {
    (void)rex_clr_sigs(rex_self(), MMGSDI_TIMED_SLEEP_TIMER_SIG);
  }

  /* Undefine the timer before exiting */
  (void)rex_undef_timer(&mmgsdi_timed_sleep_timer);
#else
  #error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */
} /* mmgsdi_util_timed_sleep */


/* ============================================================================
  FUNCTION:      MMGSDI_ONCHIP_ACTIVATE

  DESCRIPTION:
    This function will be used to activate the ONCHIP USIM or SIM.  It will
    configure the USIM/SIM Cache.

  DEPENDENCIES:


  LIMITATIONS:
    USIM Authentication in USIM mode is not supported.

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
    None.

  CRITICAL SECTIONS:
    The function uses mmgsdi_client_app_data_crit_sect_ptr to protect accesses
    to slot data ptr, channel info ptr, pin info ptr and app info ptr.
============================================================================*/
mmgsdi_return_enum_type mmgsdi_onchip_activate(
  mmgsdi_activate_onchip_sim_req_type * onchip_sim_config_ptr,
  mmgsdi_onchip_mode_enum_type          onchip_mode
)
{
  mmgsdi_return_enum_type          mmgsdi_status      = MMGSDI_ERROR;
  mmgsdi_event_data_type          *evt_info_ptr       = NULL;
  mmgsdi_int_app_info_type        *app_info_ptr       = NULL;
  mmgsdi_int_app_info_type       **app_info_pptr      = NULL;
  mmgsdi_app_pin_info_type        *pin1_ptr           = NULL;
  mmgsdi_slot_data_type           *slot_data_ptr      = NULL;
  mmgsdi_session_info_type        *session_info_ptr   = NULL;
  uint32                           i                  = 0;
  uint32                           j                  = 0;
  mmgsdi_task_enum_type            mmgsdi_task        = MMGSDI_TASK_MAX;
  mmgsdi_evt_session_notify_type   notify_type;
  uint8                            app_index          = 0;
  int32                            channel_info_index = MMGSDI_MAX_CHANNEL_INFO;
  uint8                            slot_index         = 0;
  mmgsdi_session_id_type           session_id         = 0;
  uint8                            app_sel_index      = 0;
  mmgsdi_session_type_enum_type    session_type       = MMGSDI_GW_PROV_PRI_SESSION;
  mmgsdi_channel_info_type        *channel_info_ptr   = NULL;
  mmgsdi_slot_id_enum_type         slot_id            = MMGSDI_SLOT_1;
  const uint8                      usim_aid_value[]   = { MMGSDI_3GPP_APP_ID_RID_BYTE_1,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_2,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_3,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_4,
                                                          MMGSDI_3GPP_APP_ID_RID_BYTE_5,
                                                          MMGSDI_USIM_APP_CODE_BYTE_1,
                                                          MMGSDI_USIM_APP_CODE_BYTE_2 };

  MMGSDIUTIL_GET_CURRENT_MMGSDI_TASK(&mmgsdi_task);
  if(mmgsdi_task == MMGSDI_TASK_MAX)
  {
    /* In case the current task is undetermined,
       default to main task */
    mmgsdi_task = MMGSDI_TASK_MAIN;
  }

  /* --------------------------------------------------------------------------
     Validate the input Parameters
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config_ptr);
  if ( onchip_sim_config_ptr->onchip_sim.data_len > 0 )
  {
    MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config_ptr->onchip_sim.data_ptr);
  }
  MMGSDIUTIL_RETURN_IF_EXCEEDS(onchip_sim_config_ptr->onchip_sim.data_len,
                           MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE);

  slot_id = onchip_sim_config_ptr->request_header.slot_id;
  if (mmgsdi_util_get_slot_index(slot_id, &slot_index) != MMGSDI_SUCCESS)
  {
    return mmgsdi_onchip_activate_rsp(MMGSDI_ERROR,onchip_sim_config_ptr);
  }

  /* Set app select index and session id depending on slot. For simplicity,
     primary session is mapped to slot 1, secondary to slot 2 and so on. */
  if (slot_id == MMGSDI_SLOT_1)
  {
    session_id = mmgsdi_generic_data_ptr->pri_gw_session_id;
    app_sel_index = MMGSDI_PRI_GW_PROV_APP_SEL_INDEX;
    session_type = MMGSDI_GW_PROV_PRI_SESSION;
  }
  else if (slot_id == MMGSDI_SLOT_2)
  {
    session_id = mmgsdi_generic_data_ptr->sec_gw_session_id;
    app_sel_index = MMGSDI_SEC_GW_PROV_APP_SEL_INDEX;
    session_type = MMGSDI_GW_PROV_SEC_SESSION;
  }
  else
  {
    session_id = mmgsdi_generic_data_ptr->ter_gw_session_id;
    app_sel_index = MMGSDI_TER_GW_PROV_APP_SEL_INDEX;
    session_type = MMGSDI_GW_PROV_TER_SESSION;
  }

  /* Proceed only if the session id is inactive or active on the same slot on
     which OnChip is being activated. */
  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    NULL,
                    &channel_info_ptr);
  if (mmgsdi_status == MMGSDI_SUCCESS &&
      channel_info_ptr != NULL &&
      channel_info_ptr->slot_id != slot_id)
  {
    UIM_MSG_ERR_2("Session type 0x%x active on slot 0x%x; cannot proceed with OnChip",
                  session_type, channel_info_ptr->slot_id);
    return mmgsdi_onchip_activate_rsp(MMGSDI_ERROR, onchip_sim_config_ptr);
  }

  memset(&notify_type, 0x00, sizeof(mmgsdi_evt_session_notify_type));

  /* --------------------------------------------------------------------------
     Validate the TLV Data
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_onchip_validate_config(
                       &onchip_sim_config_ptr->onchip_sim,
                       onchip_mode);

  if ( mmgsdi_status != MMGSDI_SUCCESS )
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Validate ONCHIP SIM TLV Failure");
    /* No external client...directly generate a response */
    return mmgsdi_onchip_activate_rsp(mmgsdi_status,onchip_sim_config_ptr);
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(evt_info_ptr,
                                     sizeof(mmgsdi_event_data_type));
  if (evt_info_ptr == NULL)
  {
    /* Directly generate a response */
    return mmgsdi_onchip_activate_rsp(mmgsdi_status,onchip_sim_config_ptr);
  }

  /* --------------------------------------------------------------------------
   Use the REFRESH SIM RESET Command to trigger the UI to get into
   the correct state so that a GSDI_SIM_INSERTED can be handled
   ------------------------------------------------------------------------*/
  UIM_MSG_HIGH_0("Sending out MMGSDI_REFRESH_STAGE_IN_PROGRESS");
  evt_info_ptr->evt                    = MMGSDI_REFRESH_EVT;
  evt_info_ptr->data.refresh.slot      = slot_id;
  evt_info_ptr->data.refresh.stage     = MMGSDI_REFRESH_STAGE_START;
  evt_info_ptr->data.refresh.mode      = MMGSDI_REFRESH_RESET;
  evt_info_ptr->data.refresh.orig_mode = MMGSDI_REFRESH_RESET;

  /* Send out the REFRESH start event */
  mmgsdi_onchip_send_refresh_evt_for_onchip_act(evt_info_ptr, slot_id);

  /* Allocating memory  for Slot data*/
  if(mmgsdi_data_slot_ptr[slot_index] == NULL)
  {
    MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
      mmgsdi_data_slot_ptr[slot_index],
      sizeof(mmgsdi_slot_data_type));

    if(mmgsdi_data_slot_ptr[slot_index] == NULL)
    {
      return MMGSDI_ERROR;
    }
  }

  /* Reset session and application data */
  mmgsdi_util_preinit_mmgsdi(0,
                             MMGSDI_APP_NONE,
                             MMGSDI_REFRESH_RESET,
                             slot_id,
                             FALSE);

  mmgsdi_state_update_card_error_state(slot_id);

  mmgsdi_util_timed_sleep(200);

  UIM_MSG_HIGH_0("Sending out MMGSDI_REFRESH_STAGE_END_SUCCESS");
  evt_info_ptr->data.refresh.stage = MMGSDI_REFRESH_STAGE_END_SUCCESS;

  /* Send out the REFRESH END_SUCCESS event */
  mmgsdi_onchip_send_refresh_evt_for_onchip_act(evt_info_ptr, slot_id);

  /* Allocating memory  for application*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_pin_table_ptr[slot_index][app_index],
    sizeof(mmgsdi_int_app_info_type));
  if(mmgsdi_app_pin_table_ptr[slot_index][app_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* Allocating memory  for PIN1 info*/
  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_app_pin1_table_ptr[slot_index][app_index],
    sizeof(mmgsdi_app_info_type));
  if(mmgsdi_app_pin1_table_ptr[slot_index][app_index] == NULL)
  {
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_data_slot_ptr[slot_index];
  app_info_ptr = mmgsdi_app_pin_table_ptr[slot_index][app_index];
  app_info_pptr = mmgsdi_app_pin_table_ptr[slot_index];
  pin1_ptr = mmgsdi_app_pin1_table_ptr[slot_index][app_index];

  slot_data_ptr->onchip_sim_data.state = MMGSDI_ONCHIP_SIM_NOT_VALID;
  slot_data_ptr->mmgsdi_uim_sanity_timer_value = MMGSDI_UIM_SANITY_ONCHIP_TIMER_VALUE;
  if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM)
  {
    slot_data_ptr->protocol = MMGSDI_UICC;
  }
  else
  {
    slot_data_ptr->protocol = MMGSDI_ICC;
  }

  /* --------------------------------------------------------------------------
     Now Set the MMGSDI Global Data Variables
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;

  /* Set select index to the first location */
  slot_data_ptr->prov_app_indices[app_sel_index] =
    MMGSDI_PROV_INDEX_VALIDITY_MASK | 0;

  /* Assign the first index location for the app info global table */
  app_info_ptr->app_selected = TRUE;

  /* Assign the first pin1 from the pin1 table to the app */
  app_info_ptr->pin1_ptr = pin1_ptr;

  /* populate pin1 info */
  pin1_ptr->key_ref             = MMGSDI_CHV1_KEY;
  pin1_ptr->pin_id              = MMGSDI_PIN1;
  pin1_ptr->status              = MMGSDI_PIN_DISABLED;
  pin1_ptr->pin_replacement     = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
  pin1_ptr->num_retries         = 3;
  pin1_ptr->num_unblock_retries = 10;
  pin1_ptr->cached              = TRUE;

  /* populate pin2 info */
  app_info_ptr->pin2.key_ref             = MMGSDI_CHV2_KEY;
  app_info_ptr->pin2.pin_id              = MMGSDI_PIN2;
  app_info_ptr->pin2.status              = MMGSDI_PIN_DISABLED;
  app_info_ptr->pin2.pin_replacement     = MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL;
  app_info_ptr->pin2.num_retries         = 3;
  app_info_ptr->pin2.num_unblock_retries = 10;
  app_info_ptr->pin2.cached = TRUE;

  if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM)
  {
    app_info_ptr->app_data.app_type = MMGSDI_APP_USIM;

    /* Set length to max len 16, it should not be 0 for USIM app */
    app_info_ptr->app_data.aid.data_len = MMGSDI_MAX_APP_ID_LEN;

    /* Set first seven byte of AID data to 3GPP USIM App identifier */
    memscpy(app_info_ptr->app_data.aid.data_ptr,
            sizeof(app_info_ptr->app_data.aid.data_ptr),
            usim_aid_value,
            sizeof(usim_aid_value));
  }
  else
  {
    app_info_ptr->app_data.app_type = MMGSDI_APP_SIM;
    app_info_ptr->app_data.aid.data_len = 0;
  }
  app_info_ptr->app_state         = MMGSDI_APP_STATE_DETECTED;

  /* assign the global data to the slot1 global data */
  slot_data_ptr->app_info_pptr    = app_info_pptr;
  slot_data_ptr->mmgsdi_tp_state = MMGSDI_TP_STATE_DONE;

  mmgsdi_generic_data_ptr->me_capabilities.me_supports_fdn = TRUE;
  mmgsdi_generic_data_ptr->me_capabilities.me_supports_bdn = TRUE;
  mmgsdi_generic_data_ptr->me_capabilities.me_supports_acl = TRUE;

  /* Get a free index from channel info table to avoid using a channel that is
     being used. */
  mmgsdi_status = mmgsdi_util_get_free_channel_index(&channel_info_index);
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_status      != MMGSDI_SUCCESS ||
      channel_info_index >= MMGSDI_MAX_CHANNEL_INFO)
  {
    UIM_MSG_ERR_0("Could not find free channel index");
    /* free evt_info_ptr */
    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
    return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_and_channel_info(
                    session_id,
                    &session_info_ptr,
                    NULL);

  if((mmgsdi_status != MMGSDI_SUCCESS) ||  (session_info_ptr == NULL))
  {
    UIM_MSG_ERR_0("Could not get GW_PRI Session info");

    /* free evt_info_ptr */
    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
    return MMGSDI_ERROR;
  }

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  do
  {
    if (mmgsdi_channel_info_ptr_table[channel_info_index] == NULL)
    {
      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        mmgsdi_channel_info_ptr_table[channel_info_index],
        sizeof(mmgsdi_channel_info_type));
    }

    if (mmgsdi_channel_info_ptr_table[channel_info_index] == NULL)
    {
      mmgsdi_status = MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
      break;
    }

    memset(mmgsdi_channel_info_ptr_table[channel_info_index], 0x00,
           sizeof(mmgsdi_channel_info_type));

    session_info_ptr->channel_info_index = (uint8)channel_info_index;

    mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections++;
    mmgsdi_channel_info_ptr_table[channel_info_index]->channel_id = MMGSDI_DEFAULT_CHANNEL;
    mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id = slot_id;
    mmgsdi_channel_info_ptr_table[channel_info_index]->app_info_ptr = app_info_ptr;

    for(i = 0; i < MMGSDI_MAX_CLIENT_INFO; i++)
    {
      if (mmgsdi_client_id_reg_table[i] == NULL)
      {
        continue;
      }
      for(j = 0; j < MMGSDI_MAX_SESSION_INFO; j++)
      {
        uint8 curr_channel_info_index = 0;

        if (mmgsdi_client_id_reg_table[i]->session_info_ptr[j] == NULL)
        {
          continue;
        }

        curr_channel_info_index = mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index;
        if(curr_channel_info_index != channel_info_index)
        {
          if(/* Update channel info for sessions of provisioning type */
             (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
              session_info_ptr->session_type)
                                               ||
             /* Update channel info for sessions of card slot type */
             (
              (curr_channel_info_index < MMGSDI_MAX_CHANNEL_INFO)    &&
              mmgsdi_channel_info_ptr_table[curr_channel_info_index] &&
              (mmgsdi_channel_info_ptr_table[curr_channel_info_index]->channel_id ==
                MMGSDI_DEFAULT_CHANNEL) &&
              (
               /* Update channel info for sessions of card slot1 type */
               ((mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id == MMGSDI_SLOT_1) &&
                (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 MMGSDI_CARD_SESSION_SLOT_1))
                                               ||
               /* Update channel info for sessions of card slot2 type */
               ((mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id == MMGSDI_SLOT_2) &&
                (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 MMGSDI_CARD_SESSION_SLOT_2))
                                               ||
               /* Update channel info for sessions of card slot3 type */
               ((mmgsdi_channel_info_ptr_table[channel_info_index]->slot_id == MMGSDI_SLOT_3) &&
                (mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_type ==
                 MMGSDI_CARD_SESSION_SLOT_3))
               )
              )
             )
          {
            UIM_MSG_HIGH_1("Channel mapping updated for session_id=0x%x",
                           mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->session_id);
            if((curr_channel_info_index < MMGSDI_MAX_CHANNEL_INFO) &&
               (mmgsdi_channel_info_ptr_table[curr_channel_info_index] != NULL) &&
               (mmgsdi_channel_info_ptr_table[curr_channel_info_index]->num_connections > 0))
            {
              mmgsdi_channel_info_ptr_table[curr_channel_info_index]->num_connections--;
            }
            mmgsdi_channel_info_ptr_table[channel_info_index]->num_connections++;
            mmgsdi_client_id_reg_table[i]->session_info_ptr[j]->channel_info_index =
              (uint8)channel_info_index;
          }
        }
      } /* for j<MAX_SESSION_INFO */
    } /* for i<MAX_CLIENT_INFO */
  }
  while(0);

  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* free evt_info_ptr */
    mmgsdi_evt_free_data(evt_info_ptr);
    MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);
    return MMGSDI_ERROR;
  }

  if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM)
  {
    /* Clear data for authenticate algorithm */
    mmgsdi_onchip_clear_usim_authenticate(slot_id);
  }

  /* Set the appropriate flag to indicate an activate ONCHIP SIM */
  slot_data_ptr->onchip_sim_data.state = MMGSDI_ONCHIP_SIM_INIT;

  /* --------------------------------------------------------------------------
     Now Notify the tasks that we have a SIM Inserted.
     ------------------------------------------------------------------------*/
  UIM_MSG_HIGH_0("MMGSDI_CARD_INSERTED_EVT");

  mmgsdi_evt_build_and_send_card_inserted(TRUE, 0, slot_id);

  /* Sleep for another 100 ms while the SIM INSERTED propogates */
  mmgsdi_util_timed_sleep(100);

  UIM_MSG_HIGH_0("MMGSDI_SESSION_CHANGED_EVT");

  /* Assign session notify type info */
  notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
  notify_type.slot_id     = slot_id;

  mmgsdi_evt_build_and_send_session_changed(
    notify_type, TRUE, (const mmgsdi_int_app_info_type*)app_info_ptr);

  /* Sleep for another 100 ms while the SESSION CHANGED propogates */
  mmgsdi_util_timed_sleep(100);

  /* --------------------------------------------------------------------------
     Now populate the USIM or SIM cache
     ------------------------------------------------------------------------*/
  if ( onchip_sim_config_ptr->onchip_sim.data_len > 0 )
  {
    mmgsdi_status = mmgsdi_onchip_populate_cache(&onchip_sim_config_ptr->onchip_sim,
                                                        slot_id,
                                                        session_type,
                                                        onchip_mode);
  }
  /* --------------------------------------------------------------------------
     Now Notify the tasks that we have a SIM Inserted.
     With an immediate notification that the SIM Init has Completed.
     ------------------------------------------------------------------------*/
  if ( mmgsdi_status == MMGSDI_SUCCESS )
  {
#ifdef FEATURE_LTE
    /* Initialize AKA algorithm if required */
    if (onchip_mode == MMGSDI_ONCHIP_MODE_USIM &&
        slot_data_ptr->onchip_sim_data.usim_auth_algo ==
          MMGSDI_ONCHIP_AUTH_ALGORITHM_MILENAGE)
    {
      lte_init_milenage_aka(
        slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key.data_ptr,
        int32touint8(slot_data_ptr->onchip_sim_data.usim_auth_algo_root_key.data_len),
        slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data.data_ptr,
        int32touint8(slot_data_ptr->onchip_sim_data.usim_auth_milenage_op_data.data_len),
        slot_data_ptr->onchip_sim_data.usim_auth_milenage_seq);
    }
#endif /* FEATURE_LTE */

    UIM_MSG_HIGH_0("MMGSDI_PIN1_DISABLED_EVT");
    (void)mmgsdi_evt_build_pin(session_id,
            MMGSDI_PIN1, MMGSDI_PIN_DISABLED, evt_info_ptr);

    mmgsdi_evt_notify_session(evt_info_ptr,
                              slot_id,
                              (const mmgsdi_int_app_info_type*)app_info_ptr,
                              TRUE);

    /* Sleep for another 500 ms while the PIN1 Disabled propogates */
    mmgsdi_util_timed_sleep(100);

    UIM_MSG_HIGH_0("MMGSDI_PIN2_DISABLED_EVT");
    (void)mmgsdi_evt_build_pin(session_id,
            MMGSDI_PIN2, MMGSDI_PIN_DISABLED, evt_info_ptr);

    mmgsdi_evt_notify_session(evt_info_ptr,
                              slot_id,
                              (const mmgsdi_int_app_info_type*)app_info_ptr,
                              TRUE);

    /* Sleep for another 100 ms while the PIN2 Disabled propogates */
    mmgsdi_util_timed_sleep(100);

    app_info_ptr->perso_state = MMGSDI_SIMLOCK_VERIFIED;
    app_info_ptr->pre_perso_files_cached = TRUE;

#ifdef FEATURE_SIMLOCK
    mmgsdi_simlock_build_and_run_algo(TRUE);

    /* Sleep for another 100 ms while SIMLOCK module updates valid IMSI list */
    mmgsdi_util_timed_sleep(100);

    UIM_MSG_HIGH_0("MMGSDI_PERSO_EVT");

    /* Assign session notify type info */
    notify_type.notify_type = MMGSDI_EVT_NOTIFY_ALL_SESSIONS;
    notify_type.slot_id     = slot_id;

    mmgsdi_evt_build_and_send_perso(
      notify_type,
      (const mmgsdi_int_app_info_type*)app_info_ptr,
      MMGSDI_PERSO_STATUS_DONE,
      MMGSDI_MAX_PERSO_FEATURE_ENUM,
      0,
      0);
#endif /* FEATURE_SIMLOCK */

    UIM_MSG_HIGH_0("MMGSDI_SUBSCRIPTION_READY_EVT");

    mmgsdi_evt_build_and_send_subscription_ready(
      notify_type, (const mmgsdi_int_app_info_type*)app_info_ptr);

    app_info_ptr->app_state = MMGSDI_APP_STATE_READY;

    /* Sleep for another 100 ms while the SUBS_READY propogates */
    mmgsdi_util_timed_sleep(100);
  }
  else
  {
    MMGSDI_DEBUG_MSG_ERROR_0("Failed to populate USIM CACHE");
    slot_data_ptr->onchip_sim_data.state = MMGSDI_ONCHIP_SIM_NOT_VALID;
    mmgsdi_status = MMGSDI_ERROR;
  }

  /* free evt_info_ptr */
  mmgsdi_evt_free_data(evt_info_ptr);
  MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(evt_info_ptr);

  /* Directly generate a response */
  return mmgsdi_onchip_activate_rsp(mmgsdi_status,
                                           onchip_sim_config_ptr);
} /* mmgsdi_onchip_activate */


/*===========================================================================
FUNCTION:      MMGSDI_ONCHIP_CREATE_ONCHIP_REQUEST

DESCRIPTION
  This function will build a MMGSDI_ACTIVATE_ONCHIP_SIM_REQ and queue it to
  the MMGSDI Task for further processing.

PARAMS:
  client_id:         Client Id of the caller
  card_slot:         Physical slot for multiple slot targets. Only slot-1
                     is supported
  onchip_sim_config: TLV Data Len and TLV Data.
  onchip_mode:       ICC/UICC mode to be used for onchip behavior
  response_cb_ptr:   Response callback.
  client_ref:        User Data returned upon completion of this cmd.

DEPENDENCIES:
  The client must have a valid client ID.  If the client ID is not part
  of the client_id_table, and failure will be returned in the response.
  Achived by calling mmgsdi_client_id_reg().

  Note: If client fails to provide a client id, meaning the client_id
        passed is 0, then an internal client id will be used.

LIMITATIONS:
  None.

RETURN VALUE:
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS:
  Access to a real card will not be possible without a power cycle
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_onchip_create_onchip_request (
  mmgsdi_client_id_type              client_id,
  mmgsdi_slot_id_enum_type           card_slot,
  mmgsdi_onchip_sim_config_data_type onchip_sim_config,
  mmgsdi_onchip_mode_enum_type       onchip_mode,
  mmgsdi_callback_type               response_cb_ptr,
  mmgsdi_client_data_type            client_ref
)
{
  mmgsdi_task_cmd_type                 * task_cmd_ptr  = NULL;
  mmgsdi_activate_onchip_sim_req_type  * msg_ptr       = NULL;
  mmgsdi_return_enum_type                mmgsdi_status = MMGSDI_ERROR;
  int32                                  total_mem_len = 0;
  int32                                  temp_mem      = 0;

  /* --------------------------------------------------------------------------
     Validate the input parameters
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_client_id_valid_in_cmd(client_id);

  MMGSDIUTIL_ENTER_CLIENT_APP_DATA_CRIT_SECT;
  if (mmgsdi_status != MMGSDI_SUCCESS && mmgsdi_generic_data_ptr != NULL)
  {
    client_id = mmgsdi_generic_data_ptr->client_id;
  }
  MMGSDIUTIL_LEAVE_CLIENT_APP_DATA_CRIT_SECT;

  if (mmgsdi_util_is_slot_valid(card_slot) == FALSE)
  {
    return MMGSDI_INCORRECT_PARAMS;
  }

  if ( onchip_sim_config.data_len > MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE ||
       onchip_sim_config.data_len < 0  )
  {
    MMGSDI_DEBUG_MSG_ERROR_1("CONFIG DATA LEN INVALID 0x%x",
                             onchip_sim_config.data_len);
    return MMGSDI_ERROR;
  }
  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  if ( onchip_sim_config.data_len > 0 )
  {
    /* Check is only valid if the Len != 0 */
    MMGSDIUTIL_RETURN_IF_NULL(onchip_sim_config.data_ptr);
  }
  /* --------------------------------------------------------------------------
     Allocate 1 large block of memory for the task_cmd_ptr and for the
     onchip config data.
     NOTE:  Side effect of the following total len is that it may not
            match exactly as a result of mmgsdi_task_cmd_data_type being
            a union. Will be addressed by using the correct type size
            rather than sizeof mmgsdi_task_cmd_data_type
     ------------------------------------------------------------------------*/
  total_mem_len = sizeof(mmgsdi_task_cmd_type);
  mmgsdi_util_cal_align_size(total_mem_len, &temp_mem);
  MMGSDIUTIL_RETURN_IF_ALIGNED_VAR_OUT_OF_BOUNDS(total_mem_len, temp_mem);

  total_mem_len = temp_mem + onchip_sim_config.data_len;

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, int32touint32(total_mem_len));
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.act_onchip_sim_req, MMGSDI_ACTIVATE_ONCHIP_SIM_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.act_onchip_sim_req;

  /* ----------------------------------------------------------------------------
     Populate the command Request Header with the information provided by
     the client.
     ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_ACTIVATE_ONCHIP_SIM_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_ACTIVATE_ONCHIP_SIM_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_ACTIVATE_ONCHIP_SIM_REQ;
  msg_ptr->request_header.client_id         = client_id;
  msg_ptr->request_header.slot_id           = card_slot;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  msg_ptr->onchip_sim.data_len = onchip_sim_config.data_len;
  msg_ptr->onchip_sim_mode     = onchip_mode;

  if ( onchip_sim_config.data_len > 0 )
  {
    /* --------------------------------------------------------------------------
       Set the Onchip configuration Data to point to the end of the task
       command pointer block allocated.  Then copy the configuration Data
       ------------------------------------------------------------------------*/
    mmgsdi_memscpy(msg_ptr->onchip_sim.data_ptr,
                   int32touint32(onchip_sim_config.data_len),
                   onchip_sim_config.data_ptr,
                   int32touint32(onchip_sim_config.data_len));
  }

  /* --------------------------------------------------------------------------
     Queue the command, free the command, and return the status
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr
    ** has not been put onto the command queue
    */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);
  }

  UIM_MSG_HIGH_2("Queue of MMGSDI command: MMGSDI_ACTIVATE_ONCHIP_SIM_REQ status=0x%x, onchip mode=0x%x",
                 mmgsdi_status, onchip_mode);

  return mmgsdi_status;
} /* mmgsdi_onchip_create_onchip_request */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_ONCHIP_SIM_DATA

   DESCRIPTION:
     This function builds cnf for the ONCHIP SIM ACTIVATION REQUEST

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
mmgsdi_return_enum_type mmgsdi_cnf_build_onchip_sim_data(
  mmgsdi_act_onchip_sim_cnf_type         * act_onchip_sim_cnf,
  mmgsdi_return_enum_type                  mmgsdi_status,
  mmgsdi_slot_id_enum_type                 slot_id)
{
  mmgsdi_slot_data_type *slot_data_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(act_onchip_sim_cnf);

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(slot_id);
  if(slot_data_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  act_onchip_sim_cnf->onchip_sim_state.state =
    slot_data_ptr->onchip_sim_data.state;

  return MMGSDI_SUCCESS;
} /* mmgsdi_cnf_build_onchip_sim_data */


/*===========================================================================
  FUNCTION MMGSDI_ONCHIP_AUTO_ACTIVATE

  DESCRIPTION
    This function auto activates the OnChip USIM/SIM feature for the given
    slot. This feature is controlled by an EFS item. If the EFS item indicates
    that OnChip feature is to be auto-activated, the function queues the
    command to MMGSDI for activating it for SIM or USIM (which is also
    indicated in the EFS item). The configuration required for activating
    OnChip is read from an EFS item. If the OnChip auto-activation has been
    successfully kicked off, the function returns SUCCESS.

  DEPENDENCIES
    None

  RETURN VALUE
    MMGSDI_SUCCESS: If OnChip activation req has been successfully queued
    MMGSDI_ERROR:   Otherwise

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_onchip_auto_activate(
  mmgsdi_slot_id_enum_type req_slot_id
)
{
  mmgsdi_return_enum_type             mmgsdi_status               = MMGSDI_ERROR;
  mmgsdi_slot_data_type              *slot_data_ptr               = NULL;
  uint32                              efs_onchip_config_file_size = 0;
  mmgsdi_onchip_sim_config_data_type  mmgsdi_onchip_data;
  uint8                               req_slot_index              = MMGSDI_SLOT_1_INDEX;
  char                               *file_path_ptr               = NULL;

  if(MMGSDI_SUCCESS != mmgsdi_util_get_slot_index(req_slot_id, &req_slot_index))
  {
    return MMGSDI_ERROR;
  }

  /* Check the auto-onchip-activation global to see if Onchip should be auto
     activated or not */
  if(mmgsdi_generic_data_ptr == NULL ||
     mmgsdi_generic_data_ptr->onchip_act_data[req_slot_index].auto_activation_enabled == FALSE)
  {
    UIM_MSG_ERR_1("Auto onchip activation disabled for slot 0x%x", req_slot_id);
    return MMGSDI_ERROR;
  }

  slot_data_ptr = mmgsdi_util_get_slot_data_ptr(req_slot_id);

  /* If Onchip is already activated, do not re-activate it and return SUCCESS */
  if(slot_data_ptr != NULL &&
     slot_data_ptr->onchip_sim_data.state == MMGSDI_ONCHIP_SIM_INIT)
  {
    UIM_MSG_HIGH_1("Onchip already activated for slot 0x%x", req_slot_id);
    return MMGSDI_SUCCESS;
  }

  /* Read the onchip config file and continue with activating the OnChip
     support with the data in that file */

  /* Get the OnChip config file data for slot specific simulation */
  switch (req_slot_id)
  {
    case MMGSDI_SLOT_1:
      file_path_ptr = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT1;
      break;
    case MMGSDI_SLOT_2:
      file_path_ptr = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT2;
      break;
    case MMGSDI_SLOT_3:
      file_path_ptr = UIM_COMMON_EFS_PATH_MMGSDI_ONCHIP_CONFIG_EFS_FILE_SLOT3;
      break;
    default:
     return MMGSDI_ERROR;
  }

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
    mmgsdi_onchip_data.data_ptr,
    MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE);

  if(mmgsdi_onchip_data.data_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }

  efs_onchip_config_file_size = MMGSDI_ACTIVATE_ONCHIP_SIM_CONFIG_MAX_SIZE;

  /* Read the OnChip config file for onchip activation */
  if(MMGSDI_SUCCESS != gsdi_efs_read_file(file_path_ptr, 
                                          &efs_onchip_config_file_size,
                                          mmgsdi_onchip_data.data_ptr,
                                          UIM_COMMON_EFS_DEVICE))
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_onchip_data.data_ptr);
    return MMGSDI_ERROR;
  }

  if(efs_onchip_config_file_size < MMGSDI_ONCHIP_TLV_PAYLOAD)
  {
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_onchip_data.data_ptr);
    return MMGSDI_ERROR;
  }

  /* Lets construct the length of onchip data to be fed into the MMGSDI API.
     The Onchip data present in the global(read from EFS item) starts with a
     MASTER_TAG (1 byte), followed by 2 bytes for data_length followed by the
     actual data. So, lets calculate the total data length:
     1. Get the number of bytes stored in second and third byte of the onchip
        data buf (reserved for length of onchip data)
     2. Increment it by the OVERHEAD length (1 byte for MASTER_TAG and two
        bytes for length of onchip data */
  mmgsdi_onchip_data.data_len = mmgsdi_onchip_data.data_ptr[1];
  mmgsdi_onchip_data.data_len += mmgsdi_onchip_data.data_ptr[2] * 0x100;
  mmgsdi_onchip_data.data_len += MMGSDI_ONCHIP_TLV_PAYLOAD;

  /* In order to avoid buffer overflow, make sure the data length does not
     exceed the data buffer size */
  if(mmgsdi_onchip_data.data_len > efs_onchip_config_file_size)
  {
    UIM_MSG_ERR_0("Invalid Onchip activation data length");
    MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_onchip_data.data_ptr);
    return MMGSDI_ERROR;
  }

  switch(mmgsdi_generic_data_ptr->onchip_act_data[req_slot_index].app_type)
  {
    case MMGSDI_APP_SIM:
      /* start OnChipSIM command processing for SIM */
      mmgsdi_status = mmgsdi_activate_onchip_sim(
               mmgsdi_generic_data_ptr->client_id, /* MMGSDI client ID */
               req_slot_id,
               mmgsdi_onchip_data,
               mmgsdi_util_internal_cb,
               0x00);
      break;
    case MMGSDI_APP_USIM:
      /* start OnChipSIM command processing for USIM */
      mmgsdi_status = mmgsdi_activate_onchip_usim(
               mmgsdi_generic_data_ptr->client_id, /* MMGSDI client ID */
               req_slot_id,
               mmgsdi_onchip_data,
               mmgsdi_util_internal_cb,
               0x00);
      break;
    default:
      mmgsdi_status = MMGSDI_ERROR;
      break;
  }
  MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_onchip_data.data_ptr);

  return mmgsdi_status;
} /* mmgsdi_onchip_auto_activate */
