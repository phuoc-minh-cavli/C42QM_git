/**
  @file policyman_phone_events.c

  @brief
*/

/*
    Copyright (c) 2013-2018 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Technologies Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this
  document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_phone_events.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "mre_efs.h"

#include "policyman_cfgitem.h"
#include "policyman_cm.h"
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_efs.h"
#include "policyman_phone_events.h"
#include "policyman_plmn.h"
#include "policyman_rat_capability.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_util.h"
#include "policyman_xml.h"
#include "policyman_thermal_state.h"

#include "cm.h"
#include "cm_v.h"
#include "sys.h"
#include "cm_dualsim.h"
#include "modem_mem.h"
#include <stringl/stringl.h>

#define PMNV_EFS_ID_TDS_BANDPREF "/nv/item_files/modem/mmode/tds_bandpref"
#define POLICYMAN_USER_PLMN_NV_PATH "/policyman/user_plmn"


/*=============================================================================
 Data Structure for CM PH Info
==============================================================================*/
struct policyman_cmph_info_s
{
  sys_modem_as_id_e_type          asubs_id;              // Subscription ID to which these preferences should apply.
  cm_mode_pref_e_type             mode_pref;             // Indicates the current mode preference.
  cm_srv_domain_pref_e_type       srv_domain_pref;       // The service domain preferred by the client. (GSM/WCDMA/TDS only).
  sys_oprt_mode_e_type            oprt_mode;             // current operating mode
  cm_network_sel_mode_pref_e_type network_sel_mode_pref; // current network selection mode preference (AUTOMATIC/MANUAL)
  sys_plmn_id_s_type              user_plmn;             // user selected PLMN if network_sel_mode_pref = MANUAL
  cm_band_pref_e_type             gw_bands;              // user preference for GW bands
  sys_lte_band_mask_e_type        lte_bands;             // user preference for LTE bands
  cm_band_pref_e_type             tds_bands;             // user preference for TDS bands
  cm_volte_enable_e_type          volte_state;           // Current VoLTE state
  cm_rtre_control_e_type          rtre_control;          // RTRE control tells 1x subscription source
};

/*=============================================================================
 Data Structure for event registration
==============================================================================*/
 struct policyman_event_s
 {
   cm_client_id_type  cm_clnt_id;
 };

 struct policyman_cmph_event_s
 {
   cm_ph_event_e_type beg;
   cm_ph_event_e_type end;
 };

 struct policyman_cmss_event_s
 {
   cm_ss_event_e_type beg;
   cm_ss_event_e_type end;
 };

 struct policyman_cmcall_event_s
 {
   cm_call_event_e_type beg;
   cm_call_event_e_type end;
 };

 struct policyman_event_s policyman_event;

/*=============================================================================
  Phone Event callback definition.
=============================================================================*/
struct policyman_cmph_evt_cb_s
{
  POLICYMAN_CMD_HDR;
  cm_ph_event_e_type evt;
  cm_ph_info_s_type info;
};

/*=============================================================================
  Function Protoypes
=============================================================================*/
static cm_mode_pref_e_type policyman_map_mode_pref_nv2cm( nv_mode_enum_type  nv_mode );

/*------------------------------------------------------------------------
  FUNCTION POLICYMAN_GET_CM_EVENT_CLIENT_ID()

  DESCRIPTION
    Function to get CM client id that registered for events
--------------------------------------------------------------------------*/
cm_client_id_type policyman_get_cm_event_client_id(void);


/*-------- policyman_ph_get_cm_ph_state --------*/
policyman_cmph_info_t * policyman_ph_get_cm_ph_state(
  sys_modem_as_id_e_type asubs_id
  )
{
  return policyman_state_get_subs_state(asubs_id)->pCmPhInfo;
}

/*=============================================================================
  APIs for CM Phone Event information
=============================================================================*/
cm_mode_pref_e_type policyman_cmph_read_nv_mode_pref(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type   nv_status;
  nv_item_type        nv_item;
  cm_mode_pref_e_type mode_pref = CM_MODE_PREF_GSM_ONLY;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  // read NV item for NV #10 to get mode_pref
  nv_status = mre_efs_get_nv_item(NV_PREF_MODE_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    mode_pref = policyman_map_mode_pref_nv2cm(nv_item.pref_mode.mode); 
  }

  POLICYMAN_MSG_HIGH_2("Got CM mode_pref %d for subs %d", mode_pref, asubs_id);

  return mode_pref;
}


cm_band_pref_e_type policyman_cmph_read_nv_band_pref(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type   nv_status;
  nv_item_type        nv_item;
  cm_band_pref_e_type gw_bands;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  /* read NV item 441 to get first 16 bits of GW Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_BAND_PREF_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    gw_bands = (cm_band_pref_e_type)(nv_item.band_pref.band);
  }
  else
  {
    gw_bands = CM_BAND_PREF_ANY; 
  }

  /* read NV item 946 to get 16-31 bits of GW Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_BAND_PREF_16_31_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    gw_bands |= ((cm_band_pref_e_type)(nv_item.band_pref.band) << 16);
  }
  else
  {
    gw_bands = CM_BAND_PREF_ANY; 
  }

  /* read NV item 962 to get 32-63 bits of GW Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_BAND_PREF_32_63_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    gw_bands |= ((cm_band_pref_e_type)(nv_item.band_pref_32_63.band) << 32);
  }
  else
  {
    gw_bands = CM_BAND_PREF_ANY; 
  }

  return gw_bands;
}


sys_lte_band_mask_e_type policyman_cmph_read_nv_lte_bc_config(
  sys_modem_as_id_e_type asubs_id
)
{
  nv_stat_enum_type   nv_status;
  nv_item_type        nv_item;
  sys_lte_band_mask_e_type lte_bands;
  
  memset(&nv_item, 0x00, sizeof(nv_item_type));

  /* read NV item 6828 to get LTE Band pref
   */
  nv_status = mre_efs_get_nv_item(NV_LTE_BC_CONFIG_I, &nv_item, asubs_id);

  if (nv_status == NV_DONE_S)
  {
    lte_bands = sys_map_band_mask_to_lte_band_mask(nv_item.lte_bc_config.lte_bc_config);
  }
  else
  {
    SYS_LTE_BAND_MASK_SET_ALL_BANDS(&lte_bands);
  }

  return lte_bands;
}


void policyman_cmph_nv_init(
  policyman_cmph_info_t   **ppInfo,
  sys_modem_as_id_e_type    asubs_id
)
{
  (*ppInfo)->mode_pref       = policyman_cmph_read_nv_mode_pref(asubs_id);
  (*ppInfo)->srv_domain_pref = CM_SRV_DOMAIN_PREF_CS_ONLY;
  (*ppInfo)->gw_bands        = policyman_cmph_read_nv_band_pref(asubs_id);
  (*ppInfo)->lte_bands       = policyman_cmph_read_nv_lte_bc_config(asubs_id);
  (*ppInfo)->tds_bands       = CM_BAND_PREF_TDS_ANY;
}


boolean policyman_cmph_init(
  policyman_cmph_info_t   **ppInfo,
  sys_modem_as_id_e_type    asubs_id
)
{
  *ppInfo = (policyman_cmph_info_t *)policyman_mem_alloc(sizeof(policyman_cmph_info_t));

  /* initialize PM client id to 0 for CM Events
   */
  memset(&policyman_event, 0x00, sizeof(struct policyman_event_s));

  policyman_cmph_nv_init(ppInfo, asubs_id);

  /* update Ph state with asubs id and set the precondition
   */
 (*ppInfo)->asubs_id    = asubs_id;
 (*ppInfo)->oprt_mode   = SYS_OPRT_MODE_PWROFF;
 (*ppInfo)->volte_state = CM_VOLTE_NO_CHANGE;
 (*ppInfo)->rtre_control = CM_RTRE_CONTROL_NONE;

  return TRUE;
}


void
policyman_cmph_deinit(
  policyman_cmph_info_t *pInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_phone_get_rtre_control --------*/
cm_rtre_control_e_type policyman_phone_get_rtre_control(
  sys_modem_as_id_e_type asubs_id
)
{
  policyman_cmph_info_t   *pCmPhInfo;

  pCmPhInfo = policyman_ph_get_cm_ph_state(asubs_id);
  return pCmPhInfo->rtre_control;
}

/*===========================================================================
  FUNCTION POLICYMAN_EXECUTE_PROCESS_CM_PHONE_EVT()

  DESCRIPTION
    Update PH state and handle changes 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_execute_process_cmph_evt
(
  policyman_cmd_t *pCmd
)
{
  struct policyman_cmph_evt_cb_s *ph_evt = (struct policyman_cmph_evt_cb_s*) pCmd;

  if(ph_evt)
  {
    POLICYMAN_MSG_HIGH_1("process cm ph event %d", ph_evt->evt);

    switch(ph_evt->evt)
    {
      case CM_PH_EVENT_OPRT_MODE:
        policyman_state_update_ph_oprt_mode(ph_evt->info.oprt_mode);
        break;

      case CM_PH_EVENT_AVAILABLE_NETWORKS_CONF:
        policyman_state_update_network_scan_result(&ph_evt->info);
        break;

      default:
        break;
    }
  }
}

/*===========================================================================
  FUNCTION POLICYMAN_CMPH_EVENT_CB()

  DESCRIPTION
    Handle CM Phone event callback

  PARAMETERS
    evt     : CM Phone EVENT type
    p_info : actual payload of CM Phone EVENT

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void policyman_cmph_event_cb
(
  cm_ph_event_e_type evt,
  const cm_ph_info_s_type *p_info
)
{
  struct policyman_cmph_evt_cb_s *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("received cm ph evt %d", evt);

  pCmd = (struct policyman_cmph_evt_cb_s *) policyman_cmd_new( sizeof(struct policyman_cmph_evt_cb_s),
                                                               policyman_execute_process_cmph_evt,
                                                               NULL,
                                                               NULL                               // No policy associated with CM Phone Event
                                                             );

  pCmd->evt    = evt;
  pCmd->info = *p_info;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}


/*-------- policyman_ph_get_mode_pref --------*/
cm_mode_pref_e_type
policyman_ph_get_mode_pref(
  sys_modem_as_id_e_type  asubs_id
  )
{
  policyman_cmph_info_t *pPhInfo = policyman_ph_get_cm_ph_state(asubs_id);

  return (pPhInfo != NULL)? pPhInfo->mode_pref : CM_MODE_PREF_NONE;
}


/*-------- policyman_ph_get_srv_domain --------*/
cm_srv_domain_pref_e_type
policyman_ph_get_srv_domain(
  sys_modem_as_id_e_type  asubs_id
  )
{
  policyman_cmph_info_t *pPhInfo = policyman_ph_get_cm_ph_state(asubs_id);

  return (pPhInfo != NULL)? pPhInfo->srv_domain_pref: CM_SRV_DOMAIN_PREF_NONE;
}


/*-------- policyman_ph_get_band_pref --------*/
void policyman_ph_get_band_pref(
  sys_modem_as_id_e_type  asubs_id,
  cm_band_pref_e_type     *gw_bands,
  sys_lte_band_mask_e_type  *lte_bands,
  cm_band_pref_e_type     *tds_bands
)
{
  policyman_cmph_info_t *pPhInfo = policyman_ph_get_cm_ph_state(asubs_id);

  if(pPhInfo)
  {
    *gw_bands = pPhInfo->gw_bands;
    *lte_bands = pPhInfo->lte_bands;
    *tds_bands = pPhInfo->tds_bands;
  }
}

sd_ss_mode_pref_e_type
cmph_map_cm_mode_pref_to_sd_mode_pref(
  cm_mode_pref_e_type mode_pref
);


/*------------------------------------------------------------------------
  FUNCTION POLICYMAN_REGISTER_FOR_CM_EVENTS()

  DESCRIPTION
    With this function, Policy Manager registers with CM events as a client
--------------------------------------------------------------------------*/
void policyman_register_for_cm_events(void)
{
  cm_client_status_e_type ret_val = CM_CLIENT_ERR_CLIENT_ID;
  static boolean          registered_for_events;
  bit_mask_256_t          event_mask_ss;
  bit_mask_256_t          event_mask_ph;
  int                     index;

  cm_ss_event_e_type  ss_events[] = { CM_SS_EVENT_SRV_CHANGED,
                                      CM_SS_EVENT_RSSI,
                                      CM_SS_EVENT_INFO,
                                      CM_SS_EVENT_REG_REJECT
                                    };

  cm_ph_event_e_type  ph_events[] = { CM_PH_EVENT_INFO,
                                      CM_PH_EVENT_OPRT_MODE,
                                      CM_PH_EVENT_AVAILABLE_NETWORKS_CONF,
                                      CM_PH_EVENT_NVRUIM_CONFIG_CHANGED
                                    };

  bit_mask_256_clear_all( &event_mask_ss );
  bit_mask_256_clear_all( &event_mask_ph );



  for ( index = 0; index < ARR_SIZE(ss_events); index++)
  {
    bit_mask_256_set_bit( &event_mask_ss, ss_events[index] );
  }

  for ( index = 0; index < ARR_SIZE(ph_events); index++)
  {
    bit_mask_256_set_bit( &event_mask_ph, ph_events[index] );
  }


  if(!registered_for_events)
  {

    ret_val = cm_client_init( CM_CLIENT_TYPE_POLICY_MANAGER, &policyman_event.cm_clnt_id);
    if ( ret_val != CM_CLIENT_OK )
    {
      goto Done;
    }

    /* register for CM Phone Events
      */
    ret_val = cm_client_ph_reg( policyman_event.cm_clnt_id,
                                policyman_cmph_event_cb,
                                CM_CLIENT_EVENT_REG,
                                event_mask_ph,
                                NULL
                              );

  /* register for CM Serving System Events
    */
  ret_val = cm_mm_client_ss_reg( policyman_event.cm_clnt_id,
                                 policyman_msim_cmss_event_cb,
                                 CM_CLIENT_EVENT_REG,
                                 event_mask_ss,
                                 NULL
                               );

  registered_for_events = TRUE;
  POLICYMAN_MSG_HIGH_0("PM registered for CM Events");
  return;

Done:
    /* release clients in case of an error
      */
    (void)cm_client_release( policyman_event.cm_clnt_id );
    POLICYMAN_MSG_ERROR_1("PM failed to register to CM Events with error cause %d", ret_val);
  }
}

/*=============================================================================
  Phone-related conditions
=============================================================================*/


/*-----------------------------------------------------------------------------
  User domain pref
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  User mode pref
-----------------------------------------------------------------------------*/


sd_ss_mode_pref_e_type
cmph_map_cm_mode_pref_to_sd_mode_pref(
  cm_mode_pref_e_type mode_pref
);


/*-----------------------------------------------------------------------------
  Operating mode pref
-----------------------------------------------------------------------------*/

typedef struct
{
  POLICYMAN_CONDITION_BASE;

  sys_oprt_mode_e_type oprt_mode;
} policyman_operating_mode_condition_t;


/*-------- policyman_str_to_operating_mode --------*/
sys_oprt_mode_e_type
policyman_str_to_operating_mode(
  char const  *pStr
  )
{
  typedef struct
  {
    const char                *pStr;
    sys_oprt_mode_e_type      oprt_mode;
  } operating_mode_map;

  sys_oprt_mode_e_type oprt_mode = SYS_OPRT_MODE_NONE;

  static operating_mode_map map[] =
  {
    {"SLEEP",   SYS_OPRT_MODE_LPM},
    {"ONLINE",  SYS_OPRT_MODE_ONLINE},
    {"OFFLINE", SYS_OPRT_MODE_OFFLINE},
    {"RESET",   SYS_OPRT_MODE_RESET},
    {"PWROFF",  SYS_OPRT_MODE_PWROFF},
  };

  size_t  i;
  char    token[32];

  policyman_get_token(&pStr, token, sizeof(token));

  for (i = 0 ; i < ARR_SIZE(map) ; ++i)
  {
    if (strcasecmp(token, map[i].pStr) == 0)
    {
      oprt_mode = map[i].oprt_mode;
      break;
    }
  }

  if (oprt_mode == SYS_OPRT_MODE_NONE)
  {
    POLICYMAN_UT_MSG("Invalid value for <phone_operating_mode>: %s.  Valid values are:", token, 0, 0);
    for (i = 0 ; i < ARR_SIZE(map) ; ++i)
    {
      POLICYMAN_UT_MSG("    %s", map[i].pStr, 0, 0);
    }
  }

  return oprt_mode;
}


/*-------- evaluate_phone_operating_mode --------*/
static boolean evaluate_phone_operating_mode(
  policyman_condition_t const *pCondition,
  void                        *pCtx,
  sys_modem_as_id_e_type       subsId
  )
{
  policyman_operating_mode_condition_t const *pCond = (policyman_operating_mode_condition_t *) pCondition;
  policyman_cmph_info_t const *pPhInfo = policyman_ph_get_cm_ph_state(subsId);
  boolean                     result;

  result = pCond->oprt_mode == pPhInfo->oprt_mode;

  POLICYMAN_MSG_HIGH_1("condition <phone_operating_mode> returns %d", result);
  return result;
}


/*-------- policyman_condition_ph_operating_mode_evaluate --------*/
static boolean policyman_condition_ph_operating_mode_evaluate(
  mre_condition_t const *pCondition,
  void                        *pCtx
  )
{
  policyman_condition_t const * pCond = (policyman_condition_t const *)pCondition;
  return policyman_evaluate_cond(pCond, pCtx, evaluate_phone_operating_mode);
}


/*-------- policyman_condition_ph_operating_mode_new --------*/
mre_status_t policyman_condition_ph_operating_mode_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_condition_t         **ppCondition
  )
{
  mre_status_t                          status = MRE_STATUS_ERR_MALFORMED_XML;
  policyman_operating_mode_condition_t *pCondition;
  char const                            *pStr;
  sys_oprt_mode_e_type                  mode;
  policyman_subs_t                      pm_subs;

  pStr = policyman_xml_get_text(pElem);
  if (pStr == NULL)
  {
    POLICYMAN_UT_MSG("<phone_operating_mode>: must specify an operating mode", 0, 0, 0);
    goto Done;
  }

  mode= policyman_str_to_operating_mode(pStr);
  if (mode == SYS_OPRT_MODE_NONE)
  {
    POLICYMAN_UT_MSG("<phone_operating_mode>: invalid operating mode", 0, 0, 0);
    goto Done;
  }

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pCondition = (policyman_operating_mode_condition_t *)
                  policyman_mem_alloc(sizeof(policyman_operating_mode_condition_t));

  ref_cnt_obj_init(pCondition, policyman_condition_dtor);
  pCondition->isTrue = policyman_condition_ph_operating_mode_evaluate;
  pCondition->oprt_mode = mode;
  pCondition->pmSubsId = pm_subs;

  *ppCondition = (mre_condition_t *) pCondition;

  status = MRE_STATUS_SUCCESS;

Done:
  return status;
}

/*===========================================================================
  FUNCTION POLICYMAN_PH_GET_VOLTE_IS_ENABLED()

  DESCRIPTION
    Get per subs volte_is_enabled from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    TRUE if volte_is_enabled, FALSE otherwise
===========================================================================*/
boolean policyman_ph_get_volte_is_enabled(
  sys_modem_as_id_e_type    subsId
)
{
  return (policyman_ph_get_cm_ph_state(subsId)->volte_state == CM_VOLTE_ENABLED);
}

/*===========================================================================
  FUNCTION policyman_ph_set_volte_enabled_state()

  DESCRIPTION
    Set volte_state into PM Phone state

  PARAMETERS
    sys_modem_as_id_e_type : Subscription to update
    cm_volte_enable_e_type : Value of the volte_enabled state to be set

  RETURN VALUE
    Boolean: TRUE if the internal volte state was changed
             FALSE if the requested volte state was already in the PM state
===========================================================================*/
boolean policyman_ph_set_volte_enabled_state(
  sys_modem_as_id_e_type  asubs_id,
  cm_volte_enable_e_type  volte_state
)
{
  policyman_cmph_info_t  *pPhInfo;

  pPhInfo = policyman_ph_get_cm_ph_state(asubs_id);

  if (pPhInfo->volte_state != volte_state)
  {
    POLICYMAN_MSG_HIGH_3( "Volte_enabled(subs %d) changed: %d -> %d",
                          asubs_id,
                          pPhInfo->volte_state,
                          volte_state );

    pPhInfo->volte_state = volte_state;
    return TRUE;
  }

  return FALSE;
} /* policyman_ph_set_volte_enabled_state() */

/*===========================================================================
  FUNCTION policyman_ph_get_volte_enabled_state()

  DESCRIPTION
    Get volte_enabled state from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    Value of 'volte_state' in the subscription state
===========================================================================*/
cm_volte_enable_e_type policyman_ph_get_volte_enabled_state(
  sys_modem_as_id_e_type  asubs_id
)
{
  return policyman_ph_get_cm_ph_state(asubs_id)->volte_state;
} /* policyman_ph_get_volte_enabled_state() */


/*===========================================================================

FUNCTION policyman_map_mode_pref_nv2cm

DESCRIPTION
  Translates NV enum to CM enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  CM enum

SIDE EFFECTS
  none

===========================================================================*/
static cm_mode_pref_e_type
policyman_map_mode_pref_nv2cm(
  nv_mode_enum_type  nv_mode
  )
{

  switch (nv_mode)
  {
    case NV_MODE_DIGITAL_ONLY:
      return CM_MODE_PREF_DIGITAL_ONLY;

    case NV_MODE_ANALOG_ONLY:
      return CM_MODE_PREF_AMPS_ONLY;

    case NV_MODE_AUTOMATIC:
      return CM_MODE_PREF_AUTOMATIC;

    case NV_MODE_E911:
      return CM_MODE_PREF_EMERGENCY;

    case NV_MODE_CDMA_ONLY:
      return CM_MODE_PREF_CDMA_ONLY;

    case NV_MODE_HDR_ONLY:
      return CM_MODE_PREF_HDR_ONLY;

    case NV_MODE_CDMA_AMPS_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_ONLY;

    case NV_MODE_GPS_ONLY:
      return CM_MODE_PREF_GPS_ONLY;

    case NV_MODE_GSM_ONLY:
      return CM_MODE_PREF_GSM_ONLY;

    case NV_MODE_WCDMA_ONLY:
      return CM_MODE_PREF_WCDMA_ONLY;

    case NV_MODE_ANY_BUT_HDR:
      return CM_MODE_PREF_ANY_BUT_HDR;

    case NV_MODE_GSM_WCDMA_ONLY:
      return CM_MODE_PREF_GSM_WCDMA_ONLY;

    case NV_MODE_DIGITAL_LESS_HDR_ONLY:
      return CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY;

    case NV_MODE_CDMA_HDR_ONLY:
      return CM_MODE_PREF_CDMA_HDR_ONLY;

    case NV_MODE_CDMA_AMPS_HDR_ONLY:
      return CM_MODE_PREF_CDMA_AMPS_HDR_ONLY;

    case NV_MODE_LTE_ONLY:
      return CM_MODE_PREF_LTE_ONLY;

    case NV_MODE_GWL:
      return CM_MODE_PREF_GWL;

    case NV_MODE_HDR_LTE_ONLY:
      return CM_MODE_PREF_HDR_LTE_ONLY;

    case NV_MODE_CDMA_HDR_LTE_ONLY:
      return CM_MODE_PREF_CDMA_HDR_LTE_ONLY;

    case NV_MODE_CDMA_HDR_GSM_WCDMA:
      return CM_MODE_PREF_CDMA_HDR_GW;

    case NV_MODE_CDMA_GSM_WCDMA:
      return CM_MODE_PREF_CDMA_GW;

    case NV_MODE_CDMA_LTE_ONLY:
      return CM_MODE_PREF_CDMA_LTE_ONLY;

    case NV_MODE_CDMA_HDR_GSM:
      return CM_MODE_PREF_CDMA_HDR_GSM;

    case NV_MODE_CDMA_GSM:
      return CM_MODE_PREF_CDMA_GSM;

    case NV_MODE_GSM_LTE_ONLY:
      return CM_MODE_PREF_GSM_LTE;

    case NV_MODE_CDMA_GSM_LTE_ONLY:
      return CM_MODE_PREF_CDMA_GSM_LTE;

    case NV_MODE_HDR_GSM_LTE_ONLY:
      return CM_MODE_PREF_HDR_GSM_LTE;

    case NV_MODE_WCDMA_LTE_ONLY:
      return CM_MODE_PREF_WCDMA_LTE;

    case NV_MODE_CDMA_WCDMA_LTE_ONLY:
      return CM_MODE_PREF_CDMA_WCDMA_LTE;

    case NV_MODE_HDR_WCDMA_LTE_ONLY:
      return CM_MODE_PREF_HDR_WCDMA_LTE;

    case NV_MODE_TDS_ONLY:
      return CM_MODE_PREF_TDS_ONLY;

    case NV_MODE_TDS_GSM:
      return CM_MODE_PREF_TDS_GSM;

    case NV_MODE_TDS_GSM_LTE:
      return CM_MODE_PREF_TDS_GSM_LTE;

    case NV_MODE_TDS_LTE:
      return CM_MODE_PREF_TDS_LTE;

    case NV_MODE_TDS_GSM_WCDMA_LTE:
      return CM_MODE_PREF_TDS_GSM_WCDMA_LTE;

    case NV_MODE_TDS_GSM_WCDMA:
      return CM_MODE_PREF_TDS_GSM_WCDMA;

    case NV_MODE_CDMA_HDR_GSM_WCDMA_LTE:
       return CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE;

    case NV_MODE_CDMA_GSM_WCDMA_LTE:
       return CM_MODE_PREF_CDMA_GSM_WCDMA_LTE;

    case NV_MODE_TDS_WCDMA:
       return CM_MODE_PREF_TDS_WCDMA;

    case NV_MODE_TDS_WCDMA_LTE:
        return CM_MODE_PREF_TDS_WCDMA_LTE;

    case NV_MODE_CDMA_HDR_GSM_TDS_LTE:
       return CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE;

    default:
      POLICYMAN_MSG_ERROR_1("Invalid NV MODE %d, return AUTO",nv_mode);
      return CM_MODE_PREF_AUTOMATIC;
  }
} /* policyman_map_mode_pref_nv2cm() */


/*===========================================================================
  FUNCTION POLICYMAN_PH_SET_OPRT_MODE()

  DESCRIPTION
    Set operating mode into PM Phone state

  PARAMETERS
    pPhInfo     : Pointer to PM Phone State info
    oprt_mode : operating mode to be set

  RETURN VALUE
    TRUE if mode_pref changed, FALSE otherwise
===========================================================================*/
boolean
policyman_ph_set_oprt_mode(
  policyman_cmph_info_t *pPhInfo,
  sys_oprt_mode_e_type oprt_mode
  )
{
  if(   pPhInfo != NULL
     && pPhInfo->oprt_mode != oprt_mode
    )
  {
    pPhInfo->oprt_mode = oprt_mode;
    POLICYMAN_MSG_HIGH_1("oprt_mode %d updated in PM state", oprt_mode);

    /* we don't want to run rules in FTM mode
      */
    return oprt_mode != SYS_OPRT_MODE_FTM;
  }

  return FALSE;
}

/*===========================================================================
  FUNCTION POLICYMAN_PH_GET_OPRT_MODE()

  DESCRIPTION
    Get operating mode from PM Phone state

  PARAMETERS
    None

  RETURN VALUE
    TRUE if mode_pref changed, FALSE otherwise
===========================================================================*/
sys_oprt_mode_e_type policyman_ph_get_oprt_mode(
  sys_modem_as_id_e_type subsId
  )
{
  return policyman_state_get_state()->pSubsState[subsId]->pCmPhInfo->oprt_mode;
}

/*-------- policyman_ue_is_online --------*/
boolean policyman_ue_is_online( void )
{
  return policyman_ph_get_oprt_mode(SYS_MODEM_AS_ID_1) == SYS_OPRT_MODE_ONLINE;
}


/*=============================================================================
===============================================================================

  Preference handling

===============================================================================
=============================================================================*/

typedef struct
{
  POLICYMAN_CMD_HDR;

  sys_modem_as_id_e_type               asubs_id;
  cm_sys_sel_pref_params_s_type       *pPref;
} policyman_cmd_report_pref_chg_t;

/*-------- policyman_pref_update_needs_policy_run --------*/
STATIC boolean policyman_pref_update_needs_policy_run(
   policyman_cmph_info_t               *pPhState,
   cm_sys_sel_pref_params_s_type const *pPref
)
{
  boolean                             doUpdate = FALSE;
  sys_mcc_type                        userMcc;

  if (pPhState->mode_pref != pPref->mode_pref)
  {
    POLICYMAN_MSG_HIGH_2( "mode_pref updated from %d to %d",
                          pPhState->mode_pref,
                          pPref->mode_pref );

    pPhState->mode_pref = pPref->mode_pref;
    doUpdate = TRUE;
  }

  if (pPhState->srv_domain_pref != pPref->srv_domain_pref)
  {
    POLICYMAN_MSG_HIGH_2( "srv_domain_pref updated from %d to %d",
                          pPhState->srv_domain_pref,
                          pPref->srv_domain_pref );

    pPhState->srv_domain_pref = pPref->srv_domain_pref;
    doUpdate = TRUE;
  }

  /* Check for MANUAL mode if PLMN is valid and then only run rules
       otherwise reset to AUTOMATIC
    */
  if (pPref->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    userMcc = policyman_plmn_get_mcc((sys_plmn_id_s_type  *)pPref->plmn_ptr);

    if (userMcc > 0 && userMcc < 1000)
    {
      pPhState->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_MANUAL;
      pPhState->user_plmn = *pPref->plmn_ptr;
      POLICYMAN_MSG_HIGH_0("Network selection = MANUAL with new PLMN:");
      doUpdate = TRUE;
    }
    else
    {
      pPhState->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
      policyman_plmn_clear(&pPhState->user_plmn);
      POLICYMAN_MSG_ERROR_0("Network selection = MANUAL with invalid PLMN");
    }
    policyman_plmn_print((sys_plmn_id_s_type *)pPref->plmn_ptr);
  }
  else
  {
    pPhState->network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
    policyman_plmn_clear(&pPhState->user_plmn);
  }

  /* Handle volte enable/disable pref
   */
   if (  pPref->volte_enable_state != CM_VOLTE_NO_CHANGE
       && pPhState->volte_state != pPref->volte_enable_state )
   {
     POLICYMAN_MSG_HIGH_2( "volte_enabled state updated from %d to %d",
                           pPhState->volte_state,
                           pPref->volte_enable_state );

     pPhState->volte_state = pPref->volte_enable_state;
     doUpdate = TRUE;
   }

  return doUpdate;
}


/*-------- policyman_report_preference_execute --------*/
STATIC void policyman_report_preference_execute(
  policyman_cmd_t *pCmd
)
{
  cm_sys_sel_pref_params_s_type const *pPref;
  policyman_cmd_report_pref_chg_t     *pPrefCmd;
  policyman_cmph_info_t               *pState;

  pPrefCmd = (policyman_cmd_report_pref_chg_t *) pCmd;
  pPref = pPrefCmd->pPref;

  pState = policyman_ph_get_cm_ph_state(pPrefCmd->asubs_id);

  POLICYMAN_MSG_HIGH_4( "executing pref change command with subs = %d, mode_pref = %d, srv_domain_pref = %d, volte_enabled = %d",
                        pPrefCmd->asubs_id,
                        pPref->mode_pref,
                        pPref->srv_domain_pref,
                        pPref->volte_enable_state );

  if (policyman_pref_update_needs_policy_run(pState, pPref))
  {
    policyman_subs_update_multimode_subs(pPrefCmd->asubs_id, pCmd);
  }

  policyman_mem_free(pPrefCmd->pPref) ;
}


/*-------- policyman_compute_policy_execute --------*/
STATIC void policyman_compute_policy_execute(
  policyman_cmd_t *pCmd
)
{
  cm_sys_sel_pref_params_s_type const *pPref;
  policyman_cmd_report_pref_chg_t     *pPrefCmd;
  policyman_cmph_info_t               *pPhInfo;
  policyman_cmph_info_t               *pPhInfoOriginal;
  policyman_set_t                     *pItemSet         = policyman_itemset_new();
  policyman_state_t                   *pState           = policyman_state_get_state();

  pPrefCmd   = (policyman_cmd_report_pref_chg_t *) pCmd;
  pPref      = pPrefCmd->pPref;
  pCmd->pMsg = NULL;

  pPhInfo = (policyman_cmph_info_t *) policyman_mem_alloc(sizeof(policyman_cmph_info_t));

  *pPhInfo        = *policyman_ph_get_cm_ph_state(pPrefCmd->asubs_id);
  pPhInfoOriginal = policyman_ph_get_cm_ph_state(pPrefCmd->asubs_id);
  pState->pSubsState[pPrefCmd->asubs_id]->pCmPhInfo = pPhInfo;

  POLICYMAN_MSG_HIGH_4( "executing compute policy command with subs = %d, mode_pref = %d, srv_domain_pref = %d, volte_enabled = %d",
                        pPrefCmd->asubs_id,
                        pPref->mode_pref,
                        pPref->srv_domain_pref,
                        pPref->volte_enable_state );

  /* Run a policy check if needed. We should return a non-NULL message
        in case preference is accepted and no policy change is done. In such
        scenarios MSG num_attach should be set to 0.
   */
  if (policyman_pref_update_needs_policy_run(pPhInfo, pPref))
  {
    policyman_policy_run_policy_check(pState, pPrefCmd->asubs_id, pItemSet);
  }

  pCmd->pMsg = policyman_cfgitem_get_change_msg_hdr(pItemSet);
  pState->pSubsState[pPrefCmd->asubs_id]->pCmPhInfo = pPhInfoOriginal;

  policyman_mem_free(pPrefCmd->pPref);
  policyman_mem_free(pPhInfo);
  REF_CNT_OBJ_RELEASE_IF(pItemSet);

  /*  Unblock the thread that issued the command.
   */
  policyman_client_block_signal_set(pCmd->signal);
}

/*-------- policyman_report_prefs_have_changed --------*/
STATIC boolean policyman_report_prefs_have_changed(
  sys_modem_as_id_e_type               asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
)
{
  policyman_cmph_info_t *pInfo = policyman_ph_get_cm_ph_state(asubs_id);

  return (   (pInfo->mode_pref != pPref->mode_pref)
          || (pInfo->srv_domain_pref != pPref->srv_domain_pref)
          || (pInfo->network_sel_mode_pref != pPref->network_sel_mode_pref)
          || (    (pPref->network_sel_mode_pref == CM_NETWORK_SEL_MODE_PREF_MANUAL)
               && !policyman_plmns_are_equal( (sys_plmn_id_s_type *)pPref->plmn_ptr, &pInfo->user_plmn) )
          || (pInfo->volte_state != CM_VOLTE_NO_CHANGE) );
}


/*-------- policyman_block_for_pref_change --------*/
static boolean
policyman_block_for_pref_change(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  )
{

  /*  Block the caller of policyman_report_preference_change() if:
   *    - We're not initializing
   *    - and there are changes in preferences that we care about
   */
  return    !policyman_is_initializing()
         && policyman_report_prefs_have_changed(asubs_id, pPref);
}


/*-------- policyman_report_preference_change --------*/
msgr_hdr_s *
policyman_report_preference_change(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  )
{
  policyman_cmd_report_pref_chg_t *pCmd;
  msgr_hdr_s                      *pMsg = NULL;

  /*  If we aren't going to block to process this request, just return.
   */
  if (!policyman_block_for_pref_change(asubs_id, pPref))
  {
    POLICYMAN_MSG_ERROR_0("not blocking caller of policyman_report_preference_change()");
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = (policyman_cmd_report_pref_chg_t *) policyman_cmd_new(
                                                sizeof(policyman_cmd_report_pref_chg_t),
                                                policyman_report_preference_execute,
                                                NULL,
                                                NULL
                                                );
  pCmd->asubs_id = asubs_id;

  pCmd->pPref = (cm_sys_sel_pref_params_s_type *)
                policyman_mem_alloc(sizeof(cm_sys_sel_pref_params_s_type));
  *pCmd->pPref = *pPref;

  pCmd->pMsg = NULL;

  POLICYMAN_MSG_HIGH_0("blocking caller of policyman_report_preference_change()");
  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);

  /* Block the caller only when policy is not suspended
  */
  if(!policyman_policy_is_suspended())
  {
    policyman_client_block_signal_wait(pCmd->signal);
  }

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

Done:
  POLICYMAN_MSG_HIGH_0("returning to caller of policyman_report_preference_change()");
  return pMsg;
}

/*-------- policyman_report_preference_change --------*/
msgr_hdr_s *
policyman_compute_policy_with_newpref(
  sys_modem_as_id_e_type              asubs_id,
  cm_sys_sel_pref_params_s_type const *pPref
  )
{
  policyman_cmd_report_pref_chg_t *pCmd;
  msgr_hdr_s                      *pMsg = NULL;

  /*  If we aren't going to block to process this request, just return.
   */
  if (policyman_is_initializing())
  {
    POLICYMAN_MSG_HIGH_0("Policyman is initializing, can't accept the preferences");
    goto Done;
  }

  /*  If we should block the calling thread to do this update, get a signal on
   *  which to block for command completion, and put it in the command.
   *  Queue the command and wait for completion.
   */
  pCmd = (policyman_cmd_report_pref_chg_t *) policyman_cmd_new(
                                                sizeof(policyman_cmd_report_pref_chg_t),
                                                policyman_compute_policy_execute,
                                                NULL,
                                                NULL
                                                );
  pCmd->asubs_id = asubs_id;
  pCmd->pPref    = (cm_sys_sel_pref_params_s_type *)policyman_mem_alloc(sizeof(cm_sys_sel_pref_params_s_type));
  *pCmd->pPref   = *pPref;

  pCmd->pMsg = NULL;
  POLICYMAN_MSG_HIGH_0("blocking caller of policyman_compute_policy_with_newpref");

  pCmd->signal = policyman_client_block_sig_get();
  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);

  /* Block the caller only when policy is not suspended
  */
  if(!policyman_policy_is_suspended())
  {
    policyman_client_block_signal_wait(pCmd->signal);
  }

  /*  Get any message to be returned and then release the command.
   */
  pMsg = pCmd->pMsg;
  ref_cnt_obj_release(pCmd);

Done:
  POLICYMAN_MSG_HIGH_0("returning to caller of policyman_report_preference_change()");
  return pMsg;
}


/*-------- policyman_report_preference_msg_free --------*/
void
policyman_report_preference_msg_free(
  msgr_hdr_s  *pMsg
  )
{
  policyman_report_msg_free(pMsg);
}

/*-------- policyman_event_get_cm_client_id --------*/
cm_client_id_type
policyman_event_get_cm_client_id(
  void
  )
{
  return policyman_event.cm_clnt_id;
}

/* ------ policyman_get_persisted_user_plmn ------*/
boolean policyman_get_persisted_user_plmn(
  sys_modem_as_id_e_type  subs,
  boolean                 clear,
  sys_plmn_id_s_type     *pPlmn
)
{
  size_t                   plmnSize;
  sys_plmn_id_s_type      *pUserPlmn = NULL;
  mre_efs_status_t         efsStatus = MRE_EFS_STATUS_SUCCESS;

  efsStatus = mre_efs_get_file( POLICYMAN_USER_PLMN_NV_PATH,
                                (void **)&pUserPlmn,
                                &plmnSize,
                                subs );

  if (efsStatus != MRE_EFS_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_2( "user_plmn EFS file not read for subs %d, status %d",
                           subs,
                           efsStatus );
    goto Done;
  }

  POLICYMAN_MSG_HIGH_4( "Read User selected PLMN from EFS for subs %d with status %d, PLMN size %d and clear %d",
                        subs,
                        efsStatus,
                        plmnSize,
                        clear );

  *pPlmn  = *pUserPlmn;
  policyman_plmn_print(pUserPlmn);

  if (clear)
  {
    POLICYMAN_MSG_HIGH_0("Clearing User_plmn EFS");
    (void)mre_efs_file_delete( POLICYMAN_USER_PLMN_NV_PATH,
                               subs );
  }

Done:
  POLICYMAN_MEM_FREE_IF(pUserPlmn);

  return (efsStatus == MRE_EFS_STATUS_SUCCESS);
}

void policyman_update_persisted_user_plmn(
  sys_modem_as_id_e_type  subsId
)
{
  cm_network_sel_mode_pref_e_type     net_sel_mode;
  sys_plmn_id_s_type                  user_plmn;
  policyman_cmph_info_t const        *pPhInfo;
  mre_efs_status_t                    efsStatus    = MRE_EFS_STATUS_SUCCESS;

  pPhInfo      = policyman_ph_get_cm_ph_state(subsId);
  net_sel_mode = pPhInfo->network_sel_mode_pref;


  POLICYMAN_MSG_HIGH_1("Network selection mode is: %d", net_sel_mode);
  if (net_sel_mode != CM_NETWORK_SEL_MODE_PREF_MANUAL)
  {
    goto Done;
  }

  user_plmn    = pPhInfo->user_plmn;

  efsStatus = mre_efs_put_file( POLICYMAN_USER_PLMN_NV_PATH,
                                (void *)&user_plmn,
                                sizeof(user_plmn),
                                FALSE,
                                subsId );

  if (efsStatus != MRE_EFS_STATUS_SUCCESS)
  {
    POLICYMAN_MSG_ERROR_2( "Failed to write user plmn EFS file for subs %d with status: %d",
                           subsId,
                           efsStatus );
    goto Done;
  }

  POLICYMAN_MSG_HIGH_0("User PLMN persisted");
  policyman_plmn_print(&user_plmn);

Done:
  return;
}
#endif