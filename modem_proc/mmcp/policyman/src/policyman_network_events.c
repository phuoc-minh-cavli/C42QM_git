/**
  @file policyman_network_events.c

  @brief  
*/

/*
    Copyright (c) 2013-2017 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_network_events.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
#include "policyman_dbg.h"
#include "policyman_device_config.h"
#include "policyman_i.h"
#include "policyman_network_events.h"
#include "policyman_plmn.h"
#include "policyman_rat_capability.h"
#include "policyman_rf.h"
#include "policyman_set.h"
#include "policyman_state.h"
#include "policyman_subs.h"
#include "policyman_task.h"
#include "policyman_timer.h"
#include "policyman_xml.h"

#include "cm_dualsim.h"
#include "sys.h"
#include "modem_mem.h"
#include <stringl/stringl.h>


/*=============================================================================
 Data Structure for Network Info
==============================================================================*/
typedef struct
{  
  POLICYMAN_NAMED_OBJECT_BASE;

  sys_band_mask_type  gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type  tdsBands;
} network_scan_band_pref_t;

typedef struct
{
  rat_capability_info_t     ratMaskPref;
  network_scan_band_pref_t  bandPref;     
} policyman_network_scan_pref_t;


struct policyman_network_info_s
{
  sys_modem_as_id_e_type             subsId;             // Subscription Id
  policyman_network_scan_e_type      ScanStatus;         // Scan Status
  policyman_network_scan_pref_t      scanPref;           // Preferred Rats and Bands for scan request
  sys_detailed_plmn_list_s_type      *available_networks; // Indicates the current searched PLMN list

  policyman_timer_t                  *pNetScanTimer;     // Timer object for retrying of Net Scan
  boolean                            is_search_allowed;  // Boolean to indicate if periodic search is allowed
};

/*=============================================================================
  Network Info callback definition.
=============================================================================*/
typedef struct 
{
  POLICYMAN_CMD_HDR;

  void                 *pInfo;
  cm_ph_cmd_e_type     cmd;
  cm_ph_cmd_err_e_type err;
} policyman_network_scan_cmd_t;

typedef struct 
{
  sys_modem_as_id_e_type asubs_id;
} policyman_network_scan_ctx_t;

typedef struct 
{
  POLICYMAN_CMD_HDR;

  sys_modem_as_id_e_type   subsId;
  boolean                  is_search_allowed;
} policyman_periodic_search_t;


cm_mode_pref_e_type cmph_map_sd_mode_pref_to_cm_mode_pref(
  sd_ss_mode_pref_e_type  mode_pref
);


/*=============================================================================
  APIs for Network Scan Events
=============================================================================*/

/*-------- policyman_network_scan_request_search --------*/
static boolean policyman_network_scan_request_search(
  sys_modem_as_id_e_type         subsId,
  policyman_network_scan_pref_t *scanPref,
  policyman_set_t               *pItemSet
  )
{
  boolean                       retval  = FALSE;
  uint32                        ratMask;
  cm_mode_pref_e_type           cm_mode_pref;
  policyman_network_scan_ctx_t *pCtx;
  policy_execute_ctx_t  rcCtx;

  if(scanPref != NULL)
  {
    cm_band_pref_e_type   scanPrefLteBands;

    rcCtx.pState   = policyman_state_get_state();
    rcCtx.pItemSet = pItemSet;
    rcCtx.asubs_id = subsId;

    ratMask      = policyman_rat_capability_evaluate(&scanPref->ratMaskPref, &rcCtx);
    cm_mode_pref = cmph_map_sd_mode_pref_to_cm_mode_pref(policyman_util_rat_mask_to_sd_mode_pref(ratMask));

    if(cm_mode_pref == CM_MODE_PREF_MAX)
    {
      POLICYMAN_MSG_ERROR_1("incorrect rat mask %d, no associated CM mode_pref - no scan issued", ratMask);
      goto Done;
    }

    pCtx = (policyman_network_scan_ctx_t *) policyman_mem_alloc(sizeof(policyman_network_scan_ctx_t));
    pCtx->asubs_id = subsId;

    // Only the lower 64 bits are used for scans
    scanPrefLteBands = sys_map_lte_band_mask_to_band_mask(scanPref->bandPref.lteBands);

    retval = cm_ph_cmd_get_networks_per_subs_extn( policyman_network_scan_cmd_cb,
                                                   (void *) pCtx,
                                                   policyman_event_get_cm_client_id(),
                                                   CM_NETWORK_LIST_PRIORITY_PLMN_SEARCH_BGND,
                                                   cm_mode_pref,
                                                   subsId,
                                                   scanPref->bandPref.gwBands,
                                                   scanPrefLteBands,
                                                   scanPref->bandPref.tdsBands );

    POLICYMAN_MSG_HIGH_2("started <network_scan> cmd status =%d with mode_pref %d and RF bands", retval, cm_mode_pref);
    policyman_rf_print_bands(scanPref->bandPref.gwBands, scanPref->bandPref.lteBands, scanPref->bandPref.tdsBands);
  }

Done:
  return retval;
}


/*-------- policyman_network_scan_stop_search --------*/
static boolean policyman_network_scan_stop_search(
  void
  )
{
  boolean retval = cm_ph_cmd_terminate_get_networks( NULL,
                                                     NULL,
                                                     policyman_event_get_cm_client_id()
                                                   );  

  POLICYMAN_MSG_HIGH_1("stopping <network_scan> with cmd status =%d", retval);
  return retval;
}

/*-------- policyman_network_scan_timer_expired --------*/
static void policyman_network_scan_timer_expired(
  policyman_timer_t *pTimer
)
{
  sys_modem_as_id_e_type    subsId       = policyman_timer_get_subs(pTimer);
  policyman_network_info_t *pNetworkInfo = policyman_state_get_network_info(subsId);

  /* reissue the network scan if operation is pending and increase timer duration
   */
  if(   pNetworkInfo != NULL 
     && policyman_network_scan_request_search(subsId, &pNetworkInfo->scanPref, NULL)
    )
  {    
    /* Increase new duration by 5 secs
      */
    policyman_timer_modify_interval(pTimer, 5);
  } 
}

/*-------- policyman_network_scan_timer_init --------*/
static void policyman_network_scan_timer_init(
  policyman_network_info_t *pNetworkInfo,
  sys_modem_as_id_e_type    subsId
)
{
  policyman_timer_t      *pNetScanTimer = NULL;
  
  if(pNetworkInfo)
  {
    /*---------------------------------------------------------------------------
      Find and initialize the Net Scan timer.
     ---------------------------------------------------------------------------*/
    POLICYMAN_MSG_HIGH_0("Network Scan timer created with fixed interval = 30 sec");

    pNetScanTimer = policyman_timer_create_fixed_timer( "netscan",
                                                        subsId, 
                                                        TIMER_NET_SCAN,
                                                        30,              // fixed duration 30 sec
                                                        policyman_network_scan_timer_expired
                                                      );

    pNetworkInfo->pNetScanTimer = pNetScanTimer;
  }
}

/*-------- policyman_network_scan_timer_deinit --------*/
void policyman_network_scan_timer_deinit(
  policyman_network_info_t *pNetworkInfo
  )
{
  /*  Release the timer object
   */
  REF_CNT_OBJ_RELEASE_IF(pNetworkInfo->pNetScanTimer);
}

/*-------- policyman_network_init --------*/
boolean policyman_network_init
(
  policyman_network_info_t **ppNetworkInfo,
  sys_modem_as_id_e_type     subsId
)
{
   policyman_network_info_t *pInfo;

   pInfo = (policyman_network_info_t *) policyman_mem_alloc(sizeof(policyman_network_info_t));

   pInfo->subsId = subsId;
   pInfo->is_search_allowed = TRUE;
   
  *ppNetworkInfo = pInfo;  

  return (*ppNetworkInfo != NULL);
}

/*-------- policyman_network_deinit --------*/
void policyman_network_deinit(
  policyman_network_info_t *pInfo
  )
{
  policyman_network_scan_timer_deinit(pInfo);

  /* Don't free it here, call <network_scan action="clear"> 
       after scan results are tested to clear the list
   */  
  //POLICYMAN_MEM_FREE_IF(pInfo->available_networks);
  POLICYMAN_MEM_FREE_IF(pInfo);
}

/*-------- policyman_execute_periodic_plmn_search_allowed --------*/
static void policyman_execute_periodic_plmn_search_allowed(
  policyman_cmd_t *pCmd
)
{
  policyman_periodic_search_t *pSearchCmd   = (policyman_periodic_search_t *) pCmd;
  policyman_network_info_t    *pNetworkInfo = NULL;

  if(pSearchCmd)
  {
    POLICYMAN_MSG_HIGH_2("process periodic_plmn_search cmd subs %d with allowed status = %d", pSearchCmd->subsId, pSearchCmd->is_search_allowed);
    pNetworkInfo = policyman_state_get_network_info(pSearchCmd->subsId);
    pNetworkInfo->is_search_allowed = pSearchCmd->is_search_allowed;
    policyman_state_handle_update(pSearchCmd->subsId);
  }
}

/*-------- policyman_set_periodic_plmn_search_allowed--------*/
void policyman_set_periodic_plmn_search_allowed(
   sys_modem_as_id_e_type  subsId,
   boolean                 isAllowed
)
{
  policyman_periodic_search_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("received set_periodic_plmn_search_allowed isAllowed = %d", isAllowed);

  pCmd = (policyman_periodic_search_t *) policyman_cmd_new( sizeof(policyman_periodic_search_t), 
                                                            policyman_execute_periodic_plmn_search_allowed, 
                                                            NULL,
                                                            NULL                               // No policy associated with Network Scan Event
                                                          );

  pCmd->is_search_allowed = isAllowed;
  pCmd->subsId = subsId;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}


/*-------- policyman_network_scan_handle_error --------*/
static void policyman_network_scan_handle_error(
  cm_ph_cmd_err_e_type   err,
  sys_modem_as_id_e_type subsId
  )
{
  policyman_network_info_t *pNetworkInfo = policyman_state_get_network_info(subsId);

  /* Intialize a timer for handling Net Scan errors. Initializing here guarantees 
         that we don't allocate memory for time object when network scan is not 
         being done. Another advantage is we'll initialize timer only for mmode SUBS 
         so if mmode SUBS changes, timer for new one will automatically be created
   */
  if (pNetworkInfo->pNetScanTimer==NULL)
  {
    policyman_network_scan_timer_init(pNetworkInfo, subsId);
  }

  /*
     If no error reported, then wait for CM event(AVAILABLE_NETWROKS_CONF) 
     that shall have a list of available networks
  */
  if (err == CM_PH_CMD_ERR_NOERR)
  {
    POLICYMAN_MSG_HIGH_0("waiting for cm ph event AVAILABLE_NETWROKS_CONF");
  }
  else
  {
    /* Net Scan cmd itself is rejected, try giving scan after some time
      */
    pNetworkInfo->ScanStatus = POLICYMAN_NET_SCAN_ABORTED; 
    policyman_timer_start(pNetworkInfo->pNetScanTimer);
    POLICYMAN_MSG_HIGH_2(" Network Search cmd rejected with error %d, retry after timer expiry duration %d", 
                          err, policyman_timer_get_interval(pNetworkInfo->pNetScanTimer)
                         );
  }
}

/*-------- policyman_network_scan_get_scan_status --------*/
policyman_network_scan_e_type policyman_network_scan_get_scan_status(
  sys_modem_as_id_e_type subsId
  )
{
  policyman_network_info_t *pNetworkInfo = policyman_state_get_network_info(subsId);

  return (pNetworkInfo != NULL) ? pNetworkInfo->ScanStatus : POLICYMAN_NET_SCAN_NOT_STARTED;
}

/*-------- policyman_execute_network_scan_cmd --------*/
void policyman_execute_network_scan_cmd(
  policyman_cmd_t *pCmd
)
{
  policyman_network_scan_cmd_t *pScanCmd = (policyman_network_scan_cmd_t*) pCmd;
  policyman_network_scan_ctx_t *pCtx = NULL;
  sys_modem_as_id_e_type        subsId = SYS_MODEM_AS_ID_1;

  if (NULL != pScanCmd)
  {
    POLICYMAN_MSG_HIGH_1("process network scan cmd %d", pScanCmd->cmd);

    pCtx = (policyman_network_scan_ctx_t *)pScanCmd->pInfo;

    if (NULL != pCtx)
    {
      subsId = (pCtx->asubs_id);
      policyman_mem_free(pCtx);
    }
    
    switch (pScanCmd->cmd)
    {
      case CM_PH_CMD_GET_NETWORKS:
       policyman_network_scan_handle_error(pScanCmd->err, subsId);
       break;

      default:
        break;
    }
  }
}

/*-------- policyman_network_scan_cmd_cb --------*/
void policyman_network_scan_cmd_cb( 
  void                 *pInfo, 
  cm_ph_cmd_e_type     cmd, 
  cm_ph_cmd_err_e_type err
  )
{
  policyman_network_scan_cmd_t *pCmd = NULL;

  POLICYMAN_MSG_HIGH_1("received cm ph cmd %d", cmd);

  pCmd = (policyman_network_scan_cmd_t *) policyman_cmd_new( sizeof(policyman_network_scan_cmd_t), 
                                                             policyman_execute_network_scan_cmd, 
                                                             NULL,
                                                             NULL                               // No policy associated with Network Scan Event
                                                           );

  pCmd->cmd   = cmd;
  pCmd->err   = err;
  pCmd->pInfo = pInfo;

  policyman_queue_put_cmd((policyman_cmd_t *) pCmd);
  ref_cnt_obj_release(pCmd);
}

 /*-------- policyman_network_scan_clear_results --------*/
static void policyman_network_scan_clear_results(
  policyman_network_info_t  *pNetworkInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pNetworkInfo->available_networks);
  POLICYMAN_MSG_HIGH_0("cleared <network_scan> results in state");
}

/*-------- policyman_network_scan_update_list --------*/
static boolean policyman_network_scan_update_list(
  policyman_network_info_t      *pInfo,
  sys_modem_as_id_e_type         subsId,
  sys_detailed_plmn_list_s_type *pPlmnList
  )
{
  size_t    i;
  boolean                            changed       = FALSE;
  policyman_network_info_t           *pNetworkInfo = policyman_state_get_network_info(subsId);
  sys_detailed_plmn_list_info_s_type *pDetailedPlmnList;

  pNetworkInfo->ScanStatus = POLICYMAN_NET_SCAN_COMPLETED;

  if(pPlmnList != NULL && pPlmnList->length > 0)
  {
    /* clear any existing list
      */
    if(pNetworkInfo->available_networks != NULL)
    {
      policyman_network_scan_clear_results(pNetworkInfo);
    }

    /* Update the information in state
      */
    pNetworkInfo->subsId     = subsId;
    POLICYMAN_MSG_HIGH_2("updated %d PLMNs for network scan result in state for subs %d", pPlmnList->length, subsId);
 
    /* Allocate memory for new list, copy the returned detailed PLMNs/RAT info to state
      */
    pNetworkInfo->available_networks = (sys_detailed_plmn_list_s_type *) policyman_mem_alloc(sizeof(sys_detailed_plmn_list_s_type));
    pNetworkInfo->available_networks->length = pPlmnList->length;
    
    for(i=0; i < pPlmnList->length; ++i)
    {
      pDetailedPlmnList = &pNetworkInfo->available_networks->info[i];
      memscpy(pDetailedPlmnList, sizeof(sys_detailed_plmn_list_info_s_type), &pPlmnList->info[i], sizeof(sys_detailed_plmn_list_info_s_type));      
      policyman_plmn_print(&pDetailedPlmnList->plmn);      
    }

    changed = TRUE;
  }

  return changed;
}

/*-------- policyman_network_scan_delete_list --------*/
static boolean policyman_network_scan_delete_list(
  policyman_network_info_t      *pInfo,
  sys_modem_as_id_e_type         subsId,
  sys_detailed_plmn_list_s_type *pPlmnList
  )
{
  boolean                  changed       = FALSE;
  policyman_network_info_t *pNetworkInfo = policyman_state_get_network_info(subsId);


  if(pNetworkInfo != NULL)
  {
    /* clear any existing list
      */
    policyman_network_scan_clear_results(pNetworkInfo);

    /* Update the information in state
      */
    pNetworkInfo->subsId = subsId;
    pNetworkInfo->ScanStatus = POLICYMAN_NET_SCAN_NOT_STARTED;
    changed = TRUE;
  }

  return changed;
}

/*-------- policyman_network_scan_update_result --------*/
boolean policyman_network_scan_update_result(
  policyman_network_info_t      *pInfo,
  sys_modem_as_id_e_type         subsId,
  sys_plmn_list_status_e_type    listStatus,
  sys_detailed_plmn_list_s_type *pPlmnList
  )
{
  boolean  changed = FALSE;

  /* First check if we requested for network scan on this subs
    */
  if(pInfo->ScanStatus != POLICYMAN_NET_SCAN_PENDING)
  {
    POLICYMAN_MSG_HIGH_1("not updating scan result in PM state, current scan status is %d", pInfo->ScanStatus);
    return changed;
  }

  switch(listStatus)
  {
    case SYS_PLMN_LIST_SUCCESS:
      changed = policyman_network_scan_update_list(pInfo, subsId, pPlmnList);
      break;

    case SYS_PLMN_LIST_AS_ABORT:
      changed = policyman_network_scan_delete_list(pInfo, subsId, pPlmnList);
      break;

    default:
      break;
  }

  return changed;
}

/*-------- policyman_net_scan_update_state_rf_info --------*/
void policyman_net_scan_update_state_rf_info(
  policyman_network_scan_pref_t *pScanPref
  )
{
  policyman_network_info_t      *pNetworkInfo   =NULL;
  policyman_network_scan_pref_t *pStateScanPref =NULL;
  size_t  subsId;
  
  /* Update for all SUBS - updating only for mmode subs is not correct 
       since mmode SUBS might change during run time and we'll attempt 
       to issue scan with no RATs/BANDs in state for that SUBS.
   */
  if(pScanPref)
  {
    for(subsId=0; subsId < POLICYMAN_NUM_SUBS; subsId++)
    {  
      pNetworkInfo = policyman_state_get_network_info(subsId);
      pStateScanPref = &pNetworkInfo->scanPref;

      memscpy(pStateScanPref, sizeof(policyman_network_scan_pref_t), pScanPref, sizeof(policyman_network_scan_pref_t));
    }
  }
}


/*-------- policyman_net_scan_rf_bands_read_info --------*/
policyman_status_t policyman_net_scan_rf_bands_read_info(
  policyman_xml_element_t const   *pElem,
  rf_bands_t                      *pBands,
  policyman_policy_t              *pPolicy
  )
{
  char const         *pListName;

  rf_bands_t         *rfBandList;
  policyman_status_t status = POLICYMAN_STATUS_ERR_MALFORMED_XML;
    
  /*  If this action uses a named bands list, look up for named list
        and get bands from it.
   */
  pListName = policyman_xml_get_attribute(pElem, "list");
  if (pListName != NULL)
  {
    rfBandList = (rf_bands_t *) mre_named_object_find(
                                                pListName,
                                                POLICYMAN_NAMED_RF_BANDS,
                                                (mre_policy_t *) pPolicy
                                                );
    if (rfBandList != NULL)   
    { 
      policyman_get_bands_for_named_object(rfBandList, pBands);
      status = POLICYMAN_STATUS_SUCCESS;
      goto Done;
    }
  }

  /* Otherwise, read the inline band list.
   */
  if (!policyman_rf_band_list_read(pElem, pBands))
  {
    POLICYMAN_MSG_ERROR_0("<rf_bands>: invalid specification");
    goto Done;
  }

  status = POLICYMAN_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_1("policyman_net_scan_rf_bands_read_info returned status %d", status);
  return status;
}


/*-------- policyman_net_scan_timer_running --------*/
boolean policyman_net_scan_timer_running(
  sys_modem_as_id_e_type subsId
  )
{
  policyman_network_info_t *pNetworkInfo = policyman_state_get_network_info(subsId);
  return policyman_timer_is_running(pNetworkInfo->pNetScanTimer);

}

#if defined(TEST_FRAMEWORK)
#error code not present
#endif /* +TEST_FRAMEWORK. */

/*-----------------------------------------------------------------------------
  Network Scan Request
-----------------------------------------------------------------------------*/
typedef enum
{
  POLICYMAN_NET_SCAN_ACTION_NONE  =0,
  POLICYMAN_NET_SCAN_ACTION_START =1,
  POLICYMAN_NET_SCAN_ACTION_STOP  =2,
  POLICYMAN_NET_SCAN_ACTION_ABORT =3,
  POLICYMAN_NET_SCAN_ACTION_CLEAR =4
} policyman_scan_action_e;

typedef struct
{
  POLICYMAN_ACTION_BASE;

  policyman_scan_action_e       action;
  policyman_network_scan_pref_t scan_pref;
} network_scan_action_t;

/*-------- policyman_network_scan_read_info --------*/
static policyman_status_t policyman_network_scan_read_info(
  policyman_xml_element_t const   *pElem,
  network_scan_action_t           *pAction,
  policyman_policy_t              *pPolicy
  )
{
  policyman_status_t            status = POLICYMAN_STATUS_ERR_INVALID_ACTION;
  policyman_network_scan_pref_t *pInfo = &pAction->scan_pref;

  char const        *pActionStr;
  size_t            nChildren;
  size_t            i;

  memset(pInfo, 0x00, sizeof(policyman_network_scan_pref_t));

  pActionStr = policyman_xml_get_attribute(pElem, "action");
  if (pActionStr == NULL)        { goto Done; }

  /* Start network scan
    */
  if(strcmp(pActionStr, "start")==0)
  {    
    nChildren = policyman_xml_num_children(pElem);
    if (nChildren == 0)
    {
      status = POLICYMAN_STATUS_SUCCESS;
      goto Done;
    }

    for (i = 0 ; i < nChildren ; ++i)
    {
      policyman_xml_element_t const *pChild;
  
      pChild = policyman_xml_get_child(pElem, i);
      if (pChild == NULL)
      {
        POLICYMAN_MSG_ERROR_1("Internal error - no element #%d", i);
        goto Done;
      }
      
      if      (policyman_xml_tag_is(pChild, "rats"))
      {
        status = policyman_rat_capability_read_info(pChild, &pInfo->ratMaskPref);
      }
      else if (policyman_xml_tag_is(pChild, "bands"))
      {
        status = policyman_net_scan_rf_bands_read_info(
                    pChild, (rf_bands_t *)&pInfo->bandPref, pPolicy
                    );
      }
      else
      {
        goto Done;
      }
    }

    POLICYMAN_MSG_HIGH_0("setting network scan action to START");
    pAction->action = POLICYMAN_NET_SCAN_ACTION_START;
   }
   /* Stop network scan
     */
   else if(strcmp(pActionStr, "stop")==0)
   {
     pAction->action = POLICYMAN_NET_SCAN_ACTION_STOP;
     POLICYMAN_MSG_HIGH_0("setting network scan action to STOP");
   }
   /* Abort network scan
     */
   else if(strcmp(pActionStr, "abort")==0)
   {
     pAction->action = POLICYMAN_NET_SCAN_ACTION_ABORT;
     POLICYMAN_MSG_HIGH_0("setting network scan action to ABORT");
   }
   /* Clear the network scan state
     */
   else if(strcmp(pActionStr, "clear")==0)
   {
     pAction->action = POLICYMAN_NET_SCAN_ACTION_CLEAR;
     POLICYMAN_MSG_HIGH_0("setting network scan action to CLEAR");
   }

  status = POLICYMAN_STATUS_SUCCESS;

Done:
  return status;
}


/*-------- execute_network_scan --------*/
static boolean execute_network_scan(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId 
  )
{
  network_scan_action_t    *pnsAction = (network_scan_action_t *)pAction;
  policy_execute_ctx_t     *pnsCtx = (policy_execute_ctx_t *)pCtx;
  policyman_network_info_t *pNetworkInfo = policyman_state_get_network_info(subsId);
  boolean                   retval = TRUE;

  POLICYMAN_MSG_HIGH_2("<network_scan> state: is_allowed =%d, scan_status =%d", 
                        pNetworkInfo->is_search_allowed, 
                        pNetworkInfo->ScanStatus
                      );


  /* Clear the pending scan list. This case will hit when back to back 
        NET SCAN is triggered when previous list didn't had the desired PLMN.
   */
   if(pNetworkInfo->available_networks != NULL)
   {
     pNetworkInfo->ScanStatus = POLICYMAN_NET_SCAN_NOT_STARTED;
     policyman_network_scan_clear_results(pNetworkInfo);
   }

  /* Start Net Scan if it is allowed
    */
  if(   pnsAction->action == POLICYMAN_NET_SCAN_ACTION_START
     && pNetworkInfo->is_search_allowed == TRUE
     && policyman_network_scan_request_search(subsId, &pNetworkInfo->scanPref, pnsCtx->pItemSet)
    )
  {
    pNetworkInfo->ScanStatus = POLICYMAN_NET_SCAN_PENDING;
  }
  /* Stop/Abort any ongoing Net Scan
    */
  else if(  (pnsAction->action == POLICYMAN_NET_SCAN_ACTION_STOP
          || pnsAction->action == POLICYMAN_NET_SCAN_ACTION_ABORT)
          && pNetworkInfo->ScanStatus == POLICYMAN_NET_SCAN_PENDING
          && policyman_network_scan_stop_search()
         )
  {
    pNetworkInfo->ScanStatus = POLICYMAN_NET_SCAN_ABORTED;
  }
  /* Error case
   */
  else
  {
    retval = FALSE;
  }

  return retval;
}


/*-------- policyman_network_scan_execute --------*/
static boolean policyman_network_scan_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
  )
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;
  return policyman_execute_action(pAct, pCtx, execute_network_scan);
}

/*-------- policyman_network_scan_action_new --------*/
mre_status_t policyman_network_scan_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  mre_status_t            status = MRE_STATUS_ERR_INVALID_ACTION;
  network_scan_action_t  *pAction = NULL;
  policyman_subs_t        pm_subs;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }

  pAction = (network_scan_action_t *)
                policyman_mem_alloc(sizeof(network_scan_action_t));
  
  ref_cnt_obj_init(pAction, policyman_action_dtor);
  pAction->execute = policyman_network_scan_execute;
  pAction->pmSubsId = pm_subs;

  status = policyman_network_scan_read_info(pElem, pAction, (policyman_policy_t *) pPolicy);

  if (MRE_FAILED(status))
  {
    POLICYMAN_UT_MSG("<network_scan>: invalid XML", 0, 0, 0);
    REF_CNT_OBJ_RELEASE_IF(pAction);
    goto Done;
  }

  *ppAction = (mre_action_t *) pAction;

  /* Update the state with specified RATs/BANDs for all SUBS when action is START
        For ABORT/STOP/CLEAR no need to update the database for scan preference as
        it overwrites the previous preferences
   */
  if(pAction->action == POLICYMAN_NET_SCAN_ACTION_START)
  {
    policyman_net_scan_update_state_rf_info(&pAction->scan_pref);
  }

Done:
  POLICYMAN_MSG_HIGH_1("action <network_scan> returned status %d", status);
  return status;
}

#endif