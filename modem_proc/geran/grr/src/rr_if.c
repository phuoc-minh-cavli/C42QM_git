/*! \file rr_if.c
 
  This module contains the functionality to send RR-IMSGs into RR, as defined by
  the API in rr_if.h.
 
                Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_if.c#5 $ */
/* $DateTime: 2019/12/02 06:22:48 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "comdef.h"
#include "rr_seg_load.h"
#include "rr_if.h"
#include "rr_rr.h"
#include "rr_gprs_debug.h"
#include "geran_multi_sim.h"
#include "stringl/stringl.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

const char * rr_imsg_name(const rr_imsg_e imsg)
{
  switch (imsg)
  {
    case RR_IMSG_PS_ACCESS_IND:                     return "RR_IMSG_PS_ACCESS_IND";
#ifdef FEATURE_LTE
    case RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND: return "RR_IMSG_INDIVIDUAL_PRIORITIES_UPDATED_IND";
    case RR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND:     return "RR_IMSG_G2L_RESEL_ALLOWED_UPDATED_IND";
    case RR_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND: return "RR_RESEL_IMSG_IRAT_SYSTEM_INFORMATION_UPDATED_IND";
#endif /* FEATURE_GSM_TO_LTE */
    case RR_IMSG_RR_EVENT_IND:                      return "RR_IMSG_RR_EVENT_IND";
    case RR_IMSG_SAVED_MSG_IND:                     return "RR_IMSG_SAVED_MSG_IND";
    case RR_IMSG_PS_ACCESS_REQ:                     return "RR_IMSG_PS_ACCESS_REQ";
    case RR_IMSG_NO_PS_ACCESS_REQ:                  return "RR_IMSG_NO_PS_ACCESS_REQ";
    case RR_IMSG_GPRS_IA_IND:                       return "RR_IMSG_GPRS_IA_IND";
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_RR_THERMAL_MITIGATION)
    case RR_IMSG_MSC_CHANGE_IND:                    return "RR_IMSG_MSC_CHANGE_IND";
#endif
    case RR_IMSG_X2G_CCO_COMPLETE_IND:              return "RR_IMSG_X2G_CCO_COMPLETE_IND";
    case RR_IMSG_PAGE_IND:                          return "RR_IMSG_PAGE_IND";
    case RR_IMSG_PAGE_MODE_IND:                     return "RR_IMSG_PAGE_MODE_IND";
    case RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED:      return "RR_IMSG_L1_IDLE_MODE_CHANGE_REQUIRED";
#ifdef FEATURE_SGLTE
    case RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ: return "RR_IMSG_SGLTE_SWITCH_TO_CS_ONLY_GSM_ONLY_REQ";
#endif /* FEATURE_SGLTE */
#ifdef FEATURE_MODEM_CONFIG_REFRESH
    case RR_IMSG_GERAN_NV_REFRESH_IND:              return "RR_IMSG_GERAN_NV_REFRESH_IND";
#endif /* FEATURE_MODEM_CONFIG_REFRESH */
    case RR_IMSG_CSI_MULTIPLE_INSTANCES_SIS_RETRIEVED_IND: return "RR_IMSG_CSI_MULTIPLE_INSTANCES_SIS_RETRIEVED_IND";
    case RR_IMSG_DMM_RESUME_IND:                    return "RR_IMSG_DMM_RESUME_IND";
    default:                                        return "?";
  }
}


void rr_send_imsg(
  rr_imsg_e imsg
   
)
{
  rr_message_header_t header;

  memset(&header, 0, sizeof(header));

  rr_rr_send(
    &header,
    (uint8) imsg,
    sizeof(header),
    RR_IMSG
  );
}

void rr_send_priority_imsg_rr_event_ind(
  rr_event_T rr_event
   
)
{
  rr_imsg_rr_event_ind_t message;

  MSG_SPRINTF_1(MSG_SSID_DFLT,
                MSG_LEGACY_HIGH,
                "Sending RR_IMSG_RR_EVENT_IND with %s",
                rr_event_name(rr_event));

  message.rr_event = rr_event;
  message.priority_over_other_imsg = TRUE;

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_RR_EVENT_IND,
    sizeof(message),
    RR_IMSG
  );
}

void rr_send_imsg_rr_event_ind(
  rr_event_T rr_event
   
)
{
  rr_imsg_rr_event_ind_t message;

  MSG_SPRINTF_1(MSG_SSID_DFLT,
                MSG_LEGACY_HIGH,
                "Sending RR_IMSG_RR_EVENT_IND with %s",
                rr_event_name(rr_event));

  message.rr_event = rr_event;
  message.priority_over_other_imsg = FALSE;

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_RR_EVENT_IND,
    sizeof(message),
    RR_IMSG
  );
}

void rr_send_imsg_saved_msg_ind(
  mm_rr_msgs_u *msg
   
)
{
  rr_imsg_saved_msg_ind_t message;

  MSG_SPRINTF_1(MSG_SSID_DFLT,
                MSG_LEGACY_HIGH,
                "Sending RR_IMSG_SAVED_MSG_IND with %s",
                rr_gprs_message_name(
                  (rr_cmd_bdy_type *)msg,
                  msg->header.message_set,
                  msg->header.message_id,
                  NULL
                )
               );

  memscpy(&message.msg,sizeof(message.msg), msg, sizeof(message.msg));

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_SAVED_MSG_IND,
    sizeof(message),
    RR_IMSG
  );
}

void rr_send_imsg_ps_access_req(
  boolean purge_gmm_signalling,
  rr_cb_fn_t confirm_cb_fn
   
)
{
  rr_imsg_ps_access_req_t message;

  MSG_GERAN_HIGH_1_G("RR_IMSG_PS_ACCESS_REQ: purge_gmm=%d", purge_gmm_signalling);

  message.purge_gmm_signalling = purge_gmm_signalling;
  message.confirm_cb_fn = confirm_cb_fn;

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_PS_ACCESS_REQ,
    sizeof(message),
    RR_IMSG
  );
}

void rr_send_imsg_no_ps_access_req(
  boolean del_all_pdus,
  rr_cb_fn_t confirm_cb_fn
   
)
{
  rr_imsg_no_ps_access_req_t message;

  message.del_all_pdus = del_all_pdus;
  message.confirm_cb_fn = confirm_cb_fn;

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_NO_PS_ACCESS_REQ,
    sizeof(message),
    RR_IMSG
  );
}

void rr_send_imsg_gprs_ia_ind(
  packet_channel_description_t *packet_ch_desc,
  ia_rest_octets_t *ia_rest_octets,
  frequency_list_T *frequency_list,
  byte timing_advance
   
)
{
  rr_imsg_gprs_ia_ind_t msg;

  msg.packet_ch_desc = (*packet_ch_desc);
  msg.ia_rest_octets = (*ia_rest_octets);
  msg.frequency_list = (*frequency_list);
  msg.timing_advance = timing_advance;

  rr_rr_send(
    &msg.header,
    (uint8) RR_IMSG_GPRS_IA_IND,
    sizeof(msg),
    RR_IMSG
  );
}

void rr_send_imsg_page_ind(
  rr_page_info_t *page_info_ptr
   
)
{
  rr_imsg_page_ind_t msg;

  //RR_NULL_CHECK_RETURN_VOID(page_info_ptr);

  msg.page_info = (*page_info_ptr);

  rr_rr_send(
    &msg.header,
    (uint8) RR_IMSG_PAGE_IND,
    sizeof(msg),
    RR_IMSG
  );
}

void rr_send_imsg_page_mode_ind(
  paging_mode_T old_page_mode,
  paging_mode_T new_page_mode
  
)
{
  rr_imsg_page_mode_ind_t msg;

  msg.old_page_mode = old_page_mode;
  msg.new_page_mode = new_page_mode;

  rr_rr_send(
    &msg.header,
    (uint8) RR_IMSG_PAGE_MODE_IND,
    sizeof(msg),
    RR_IMSG
  );
}

#ifdef FEATURE_MODEM_CONFIG_REFRESH
void rr_send_imsg_geran_nv_refresh_ind(
   
)
{
  rr_imsg_geran_nv_refresh_ind_t message;

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_GERAN_NV_REFRESH_IND,
    sizeof(message),
    RR_IMSG
  );
}
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

void rr_send_imsg_dmm_resume_ind()
{
  rr_imsg_dmm_resume_ind_t message;

  rr_rr_send(
    &message.header,
    (uint8) RR_IMSG_DMM_RESUME_IND,
    sizeof(message),
    RR_IMSG
  );
}

/* EOF */

