/*===========================================================================

                            TM_CM_IFace  Module

DESCRIPTION



Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary. 
Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
Copyright (c) 2013 - 2022 Qualcomm Technologies, Inc.
All Rights Reserved.
Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_cm_iface.c#4 $

when        who     what, where, why
--------    ---     ----------------------------------------------------------
07/07/22    dy      Changes are made to ensure  TM_CM iface  update the cell info  TM_CM iface on RAT change.
05/08/20   ska     Smart compilation issues
22/05/19    kat     Changes to request fro plmn cell search
11/07/17    kpn     L+L: Handling 3rd party E911 call
10/18/17    rk      L+L: Upon E911 call orig with no service indication for RAT, used the as_id to
                    retrieve the non-DD subs RAT.
10/09/17    ak      Changes to start early RX start session on connected RAT.
08/01/17    kpn     L+L: Use AS_ID while calling tm_cm_process_ext_emergency_enter()
06/07/17    kpn     L+L: Use Srv Sys mode while handling E911 Orig call event
05/24/17    ak      E911 and apptrack concurrenct support.
04/10/17    kpn     L+L: API to get number of configured Subs
02/21/17    kpn     Support of 1x CP on Sub2
01/24/17    kpn     Support E911 CP session on non-DD sub
11/16/16    mj      E911 and apptracking concurrency support
03/11/16    mj      Changes to check as_sys_mode from cell info first for CELL DCH request to CM
11/10/15    skm     Fix MT-ES failure where autp e-slp is not generated due to missing CELL ID in WCDMA. 
11/06/15    mj      Add changes to support CM CellID event processing for MultiSIM
10/19/15    mj      Add changes to provide srv_status of selected sys_mode through phone status
07/20/15    mj      Add support for MultiMode config for MultiSim devices
06/24/15    rh      Added CM CellID event processing  
05/04/15    mj     Address code review comments to make handling of operatonal/non-operational flag thread safe
04/15/15    mj     Handle updates from CM when the stack goes from operational to non-operational, without prior event update
03/18/15    sj      Update Call END handling logic for 1x 
07/16/14    mj      During LPM event set all stacks to no service irrespective of stack operational or not
06/04/14    mj      Handle MultiSIM subscription feature SRLTE
05/01/14    mj      Changes to address KW issues wrt sub id sent from CM
03/25/14    mj      Changes to fix IMSI updates during hot swap
03/21/14    ssu     Addressing compilation issues on SMART(offfline) setup.
12/23/13    mj      Add functionality to read device config from policy manager
01/15/13    mj     Remove featurization FEATURE_DUAL_SIM to handle case where SGLTE/SVLTE code was being featured out
12/12/13    ss      Use stack type while filling is serving system in SS Info mapping
11/22/13    ss      DI 3.0 onwards, CM moved to Super build. In case of single sim mode, there will be no
                          dual sim sub event. This change is to handle that.
09/27/13    mj      TSTS Support: Changes to handle the third SUB
06/14/13    ssu     GPS changes to update subs id when a dual sim device is in single sim mode.
                    ( the single/dual mode is controlled by EFS NV )  
04/16/13    mj      Update DSDS sub id
09/07/12    mj      Added new function tm_cm_map_external_RAT and modified tm_cm_iface_get_phone_state for SGLTE
01/04/12    ssu     Obtaining th CELL id information in CELL_DCH from CM APIs 
06/23/11    rh      Added cdma_mt_end_call_immed parameter to end PD call immediately
06/16/11    rh      Added CM_CALL_EVENT_OTASP_STATUS event handling for CR 280064
04/19/11    gk      Initialize the hdr_srv_status to SYS_SRV_STATUS_NO_SRV
01/20/11    rh      Fix for CR 258513
04/06/10    ns      Fix for CR 233506: add support for CM PH INFO GET.
03/25/10    ns      Fix for CR:230743: modify MOLR CM api calls for DSDS.
03/03/10    ns      DSDS Support
11/11/08    ank     When handling stop req, end PD call only if MT session is active.
04/04/08    cl      Inform TM-core when entering GSM
06/27/07    lt      Added the passing of WCDMA in-service to UMTS submodules.
04/24/07    ank     Added support for CELL_ID_DB feature.
12/20/06    ank     Changes to read NV item.
11/30/06    ank     Initial Release
===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"
#include "err.h"
#include "event.h"

#include "sm_nv.h"
#include "tm_common.h"
#include "tm_data.h"
#include "pdapibuf.h"
#include "tm_cm_iface.h"

#ifndef FEATURE_GNSS_LEAN_BUILD
#include "policyman.h"
#endif

#ifdef FEATURE_CGPS_XTRA_T
#include "tle_api.h"
#endif /* FEATURE_CGPS_XTRA_T */

#include "mgp_api.h"
#include "gm_api.h"
#include "bit.h"

#ifdef FEATURE_LOC_GTS_SUPPORT
  #include "gts_loc_api.h"
#endif /* #ifdef FEATURE_LOC_GTS_SUPPORT */
#define TM_CM_INVALID_MCC 0xFFFFFFFF
boolean b_srvcc = FALSE;
LOCAL boolean tm_cm_is_cell_info_update_needed(void);
LOCAL void tm_cm_process_cell_info_update( const tm_cm_mm_ss_stack_info_s_type *const cp_ss_info_ptr);
extern void tm_core_handle_updated_mcc(uint32 mcc);

#ifdef FEATURE_REF_TRACKER_LOC
boolean tm_cm_psm_event_cb(void );
#endif

#ifdef FEATURE_CGPS_GTP_20_SUPPORT
#include "ref_cnt_obj.h"
#endif /* FEATURE_CGPS_GTP_20_SUPPORT */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*                        GLOBAL DATA DECLARATIONS                         */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
tm_cm_info_s_type tm_cm_info;

LOCAL void tm_cm_map_ss_info( const tm_cm_mm_ss_info_s_type * const cp_ss_info_ptr,
                              cgps_SSInfoType * p_cgps_ss_info, 
                              cgps_SSInfoDataType e_info_type);
LOCAL void tm_cm_ph_cmd_get_networks_cb (

    void                    *data_block_ptr,
        /* Pointer to client provided data block */

    cm_ph_cmd_e_type        ph_cmd,
        /* Indicate which phone command is this error status for */

    cm_ph_cmd_err_e_type    ph_cmd_err
        /* Phone command error code, indicating whether command
        ** is rejected and the reason */
);


static void tm_core_cm_notify_srv_status(void);

/*===========================================================================

FUNCTION tm_cm_get_sys_mode_for_as_id

DESCRIPTION
  This function gets the current serving system mode of the requested sub id

DEPENDENCIES

RETURN VALUE
  sys_sys_mode_e_type: Requested sub's serving system mode

SIDE EFFECTS
  None
#define tm_cm_get_sys_mode_for_as_id     SYS_SUBS_FEATURE_MODE_NORMAL
===========================================================================*/
sys_sys_mode_e_type tm_cm_get_sys_mode_for_as_id(void)
{
      return  tm_cm_info.ss_info.sys_mode;
}

/*===========================================================================

FUNCTION tm_cm_handle_ph_event

DESCRIPTION

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_handle_ph_event(
    cm_ph_event_e_type           ph_event,
        /* Indicate phone event */
    const tm_cm_ph_info_s_type      *ph_info_ptr
        /* A pointer to a phone state information struct */
    )
{

#ifndef FEATURE_GNSS_LEAN_BUILD

#ifdef FEATURE_CGPS_CELLDB
		cgps_SSInfoType ss_info = {0};
#endif /* FEATURE_CGPS_CELLDB */


    cgps_EdrxcycleInfoType edrx_info = {0};

    if(ph_info_ptr == NULL)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_cm_handle_ph_event: Null ph_info_ptr",0,0,0);
      return;
    }

    switch(ph_event)
    {
    case CM_PH_EVENT_OPRT_MODE :
      if(ph_info_ptr->lpm_on == TRUE)
      {
        /* Fire LPM event to TM-Core, Only fire phone going into LPM mode
               * Phone coming out of lpm is not important, phone getting onto a 
               * network is important, and that is a SYS SELECT event */
        tm_core_cm_notify_event(TM_PHONE_EVENT_SYS_MODE_LPM, NULL);
        
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_cm_handle_ph_event: :TM CM Iface, Operating mode is CM_PH_EVENT_OPRT_MODE, ph_state:%d, lpm_on:%d",
                (int32) ph_info_ptr->ph_state, (int32) ph_info_ptr->lpm_on, 0);

        tm_cm_info.prev_sys_mode = tm_cm_info.sys_mode;
        tm_cm_info.sys_mode      = SYS_SYS_MODE_NO_SRV;

      /* Set NO_SRV to all the stacks for all subs */
            /* Check for stack being operational or not is not required. As per CM
               operational flag is set to false when the device goes LPM */
          tm_cm_info.ss_info.prev_sys_mode = tm_cm_info.ss_info.sys_mode;
              tm_cm_info.ss_info.sys_mode   = SYS_SYS_MODE_NO_SRV;
              tm_cm_info.ss_info.srv_status = SYS_SRV_STATUS_NO_SRV;
#ifdef FEATURE_CGPS_CELLDB        
        ss_info.multisim_ssInfo[0].ss_data_type  = CGPS_SSINFO_SRV_SYS;
              ss_info.multisim_ssInfo[0].sys_mode = SYS_SYS_MODE_NO_SRV;        
#endif    
          #ifdef FEATURE_CGPS_CELLDB
          /* Phone gone into LPM mode. Inform MGP so PUNC can be propogated */
          #ifdef FEATURE_CGPS_XTRA_T
          tle_SSChange(&ss_info);
          #endif /* FEATURE_CGPS_XTRA_T */
          mgp_SSChange(&ss_info);
          #ifdef FEATURE_LOC_GTS_SUPPORT
          gts_SSChange(&ss_info);
          #endif /* #ifdef FEATURE_LOC_GTS_SUPPORT */
          #endif /* FEATURE_CGPS_CELLDB */
      }
              /* Fall through intentional */
     break;
   case CM_PH_EVENT_EDRX_CHANGE_INFO_IND:
     MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_cm_handle_ph_event: :TM CM Iface, Operating mode is CM_PH_EVENT_EDRX_CHANGE_INFO_IND,edrx state:%d length:%d",
                ph_info_ptr->edrx_enabled,ph_info_ptr->edrx_cycle_length);
     if(ph_info_ptr->edrx_enabled == TRUE)
     {
       edrx_info.edrx_enabled = TRUE;
       edrx_info.edrx_cycle_length = ph_info_ptr->edrx_cycle_length;
     }
     else
     {
       edrx_info.edrx_enabled = FALSE;
     }
     tle_EdrxCycleLengthNotify(&edrx_info);
     break;

   default:
     MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_cm_handle_ph_event: Unexpected PH event %d",ph_event,0,0);
     break;
  }
#endif /* FEATURE_GNSS_LEAN_BUILD  */
}

/*===========================================================================

FUNCTION tm_cm_process_srv_changed

DESCRIPTION
  This function processes changes for all the subs

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_process_srv_changed(const tm_cm_mm_ss_stack_info_s_type *ss_stack_info_ptr)
{

#ifndef FEATURE_GNSS_SA
//#ifndef FEATURE_GPS_LM_STANDALONE 
    if ( ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK ) ||
         ( ss_stack_info_ptr->changed_fields & CM_SS_EVT_SYS_MODE_MASK)
        )
    {
        tm_cm_info.ss_info.sys_mode = ss_stack_info_ptr->sys_mode;

      if ( (sys_srv_status_is_srv(ss_stack_info_ptr->srv_status) == FALSE) 
         )
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_cm_process_srv_changed: No Service", 0, 0, 0);
        tm_cm_info.ss_info.sys_mode =  SYS_SYS_MODE_NO_SRV;  
      }

      /*Notify TM core*/
      tm_core_cm_notify_srv_status();          
    }
//#endif
#endif /* FEATURE_GNSS_SA */

}

/*===========================================================================

FUNCTION tm_cm_handle_ss_event

DESCRIPTION
  This function handles the SS event from Call Manager

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_handle_ss_event(
  cm_ss_event_e_type       ss_event,
  /* Indicate serving system event */
  
  const tm_cm_mm_ss_info_s_type    *ss_info_ptr
  /* A pointer to a serving system information structure */
)
{
  cgps_SSInfoType ss_info;
  memset(&ss_info, 0x00, sizeof(ss_info));
  
  if(NULL == ss_info_ptr)
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_handle_ss_event: Null ss_info_ptr");
    return;
  }
  
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: Handling ss_event %u ",
          (uint8)ss_event, 0,0);
  
  switch (ss_event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
    {
      boolean bSSChanged = FALSE, bCIDChanged = FALSE, bSIDChanged = FALSE, bBSParamsChanged = FALSE;
    
      /* Just need to map it once for both stacks (CGPS_SSINFO_SRV_SYS) */
      tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_SRV_SYS);

          if ((ss_info_ptr->stack_info.changed_fields & CM_SS_EVT_SRV_STATUS_MASK) ||
              (ss_info_ptr->stack_info.changed_fields & CM_SS_EVT_SYS_MODE_MASK) ||
             (ss_info_ptr->stack_info.changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK)
       )
    {
      /* processing of srv_status_mask and mode_mask identical */
      MSG_4( MSG_SSID_GPSSM, MSG_LEGACY_MED,
        "tm_cm_handle_ss_event: SRV_STATUS_MASK / MODE_MASK set by CM for srv_status=%d sys_mode=%d, srv_domain=%d",
          0,
          ss_info_ptr->stack_info.srv_status,
          ss_info_ptr->stack_info.sys_mode,
          ss_info_ptr->stack_info.srv_domain);

#ifdef FEATURE_CGPS_CELLDB
      bSSChanged = TRUE; /* serving system has changed, will notify MGP/TLE */
#endif /*FEATURE_CGPS_CELLDB*/
        
      /*Update the Mode settings for this subs ID*/
      tm_cm_info.ss_info.prev_sys_mode = tm_cm_info.ss_info.sys_mode;
      tm_cm_info.ss_info.sys_mode    = ss_info_ptr->stack_info.sys_mode;
      tm_cm_info.ss_info.srv_status    = ss_info_ptr->stack_info.srv_status;
      tm_cm_info.ss_info.srv_domain    = ss_info_ptr->stack_info.srv_domain;
      tm_cm_info.ss_info.changed_fields = ss_info_ptr->stack_info.changed_fields;
#ifdef FEATURE_CGPS_TLE_IOT
      /*Update CIOT info*/
      tm_cm_info.ss_info.ciot_lte_mode = ss_info_ptr->stack_info.ciot_lte_mode;
#endif /* FEATURE_CGPS_TLE_IOT */
  /* Handle Cell Info update for if CM_SS_EVT_CELL_INFO_MASK is set */
          if ((ss_info_ptr->stack_info.changed_fields & CM_SS_EVT_CELL_INFO_MASK)!=0)
          {
            
			MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CELL_INFO_MASK sys_mode=%d as_sys_mode=%d",
				     (uint32)ss_info_ptr->stack_info.sys_mode, 
				     (uint32)ss_info_ptr->stack_info.cell_info.as_sys_mode,0,0);
			
			tm_cm_info.ss_info.as_sys_mode = ss_info_ptr->stack_info.cell_info.as_sys_mode;            
  
          } /* CM_SS_CELL_INFO_MASK bitmask changed */
	tm_cm_process_srv_changed(&(ss_info_ptr->stack_info));
    }  /* CM_SS_EVT_SRV_STATUS_MASK & CM_SS_EVT_SYS_MODE_MASK etc bitmask changed */

          /* Handle Cell Info update for TLE/GM/AGNSS if CM_SS_EVT_CELL_INFO_MASK is set */
          if ((ss_info_ptr->stack_info.changed_fields & CM_SS_EVT_CELL_INFO_MASK) ||
		  	   (ss_info_ptr->stack_info.changed_fields & CM_SS_CIOT_LTE_SYS_MODE_MASK))
          {
            MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_handle_ss_event: CELL_INFO_MASK sys_mode=%d", 
                      (uint32)ss_info_ptr->stack_info.sys_mode,0,0);

            /* 1) Handle Cell Info update for TLE (both DD & non-DD) */
            bCIDChanged = TRUE; /* Cell ID has changed, will notify TLE */
            /* 2) Handle Cell Info update for GM/AGNSS (currently DD-sub only) */
            if (tm_cm_is_cell_info_update_needed())
            {
              tm_cm_process_cell_info_update(&(ss_info_ptr->stack_info)); 
    }
          } /* CM_SS_EVT_CELL_INFO_MASK bitmask changed */

       
      /* make sure we only call below functions once per sub (ss_info has info for both stacks) */
      /* notify MGP about ServingSystem change, notify TLE about SS/CellID change */
#ifdef FEATURE_CGPS_CELLDB
      if (bSSChanged == TRUE || bCIDChanged == TRUE)
      {
#ifdef FEATURE_CGPS_XTRA_T
        tle_SSChange(&ss_info);  /* Call TLE functions for both SS change and CID change */
#endif /* FEATURE_CGPS_XTRA_T */
        if (bSSChanged == TRUE)
        {
          mgp_SSChange(&ss_info);  /* call MGP function only for SS change */
          #ifdef FEATURE_LOC_GTS_SUPPORT
          gts_SSChange(&ss_info);
          #endif /* #ifdef FEATURE_LOC_GTS_SUPPORT */
        }
      }  
#endif /* FEATURE_CGPS_CELLDB */
  break;
    }
 
   default:
      break;
   }
    
#ifdef FEATURE_CGPS_XTRA_T
      if (ss_info_ptr->stack_info.changed_fields & CM_SS_EVT_MOBILITY_MGMT_MASK)
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED,"tm_cm_handle_ss_event: Updating TLE and MGP for CM_SS_EVT_MOBILITY_MGMT_MASK", 0, 0, 0);
      tm_cm_map_ss_info(ss_info_ptr, &ss_info, CGPS_SSINFO_TIMING_INFO);
      tle_SSChange(&ss_info);
      }
#endif
}

/*===========================================================================

FUNCTION tm_cm_ss_event_cb

DESCRIPTION
  This function is a callback issued by CM whenever SS event has occurred.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_ss_event_cb
(
    cm_ss_event_e_type           ss_event,
        /* Indicate serving system event */
    const cm_mm_ss_info_s_type      *ss_info_ptr
        /* A pointer to a serving system information structure */
)
{
  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;
  boolean tm_handle_ss_event = FALSE;

  if(NULL == ss_info_ptr)
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ss_event_cb: Null ss_info_ptr sent from CM ss cb");
    return;
  }
  
  switch (ss_event)
  {
    case CM_SS_EVENT_SRV_CHANGED:
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Changed fields: 0x%x",
             ss_info_ptr->changed_fields,0,0);
       

    
        if(ss_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK)
			{
          tm_handle_ss_event = TRUE;
        }

      if (
           (ss_info_ptr->changed_fields & CM_SS_EVT_SRV_STATUS_MASK ) ||
           (ss_info_ptr->changed_fields & CM_SS_EVT_SYS_MODE_MASK) ||
           (ss_info_ptr->changed_fields & CM_SS_EVT_SRV_DOMAIN_MASK ) ||
           (ss_info_ptr->changed_fields & CM_SS_EVT_MOBILITY_MGMT_MASK) ||
           (ss_info_ptr->changed_fields & CM_SS_EVT_CELL_INFO_MASK) ||
           (ss_info_ptr->changed_fields & CM_SS_CIOT_LTE_SYS_MODE_MASK)
           )
      {
        tm_handle_ss_event = TRUE;
      break;
      }
      break;

    default:
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: Ignore SS event");
      tm_handle_ss_event = FALSE;
      break;
  }

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_cm_ss_event_cb: TM handle SS event %u", tm_handle_ss_event);

  if(tm_handle_ss_event == TRUE)
  {
    ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
    /* Queue the commands in Client command Queue.
        */
    if (ipc_msg_ptr == NULL)
    {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ss_event_cb: IPC message allocation failed");
    }
    else 
    {
      event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;
      event_info_ptr->event_info.cm_event_info.ss_info.changed_fields = ss_info_ptr->changed_fields;
	  //#9x05 is operational field is removed! ask if needed
    /* If stack is non-operational, then this stack is considered OOS. */
     

    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.srv_status     = 
                                           ss_info_ptr->srv_status;
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.srv_domain    = 
                                           ss_info_ptr->srv_domain;
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.sys_mode    = 
                                           ss_info_ptr->sys_mode;
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.changed_fields    =
                                           ss_info_ptr->changed_fields;
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.mm_information =
                                            ss_info_ptr->mm_information;
    /* Roaming info */
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.roam_status = 
                                           ss_info_ptr->roam_status;
    /*Sys id info */
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.sys_id.id = 

                                           ss_info_ptr->sys_id.id;
    event_info_ptr->event_info.cm_event_info.ss_info.stack_info.sys_id.id_type= 
                                           ss_info_ptr->sys_id.id_type;

    memscpy((void*)&(event_info_ptr->event_info.cm_event_info.ss_info.stack_info.sys_id_id_plmn),
            sizeof(event_info_ptr->event_info.cm_event_info.ss_info.stack_info.sys_id_id_plmn),
            (const void*)&(ss_info_ptr->sys_id.id.plmn), sizeof(ss_info_ptr->sys_id.id.plmn));
    /*Cell info */
    /* Copy the cell info from CM to TM data structure */

        memscpy(&(event_info_ptr->event_info.cm_event_info.ss_info.stack_info.cell_info), 
             sizeof(event_info_ptr->event_info.cm_event_info.ss_info.stack_info.cell_info),
             &(ss_info_ptr->cell_info),
             sizeof(ss_info_ptr->cell_info));

#ifdef FEATURE_CGPS_TLE_IOT
      /* CIOT info */
      event_info_ptr->event_info.cm_event_info.ss_info.stack_info.ciot_lte_mode = 
                                           ss_info_ptr->ciot_lte_mode;
#endif /* FEATURE_CGPS_TLE_IOT */
     
      event_info_ptr->event_type = TM_CM_SS_EVENT;
      event_info_ptr->event_info.cm_event_type.ss_event = ss_event;
      ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
      status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
      if (status == FALSE)
      {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ss_event_cb: Error in sending CM Event msg_id=0x%x to TM thread",ipc_msg_ptr->q_MsgId);
        status = os_IpcDelete(ipc_msg_ptr);
        ipc_msg_ptr = NULL;
      } /* End status */
      else
      {
    MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_cm_ss_event_cb: Sending CM Event msg_id=0x%x to TM thread",ipc_msg_ptr->q_MsgId);
      } /* end else status */
    }

  }
} /* End tm_cm_ss_event_cb( ) */
/*===========================================================================

FUNCTION tm_cm_ph_event_cb

DESCRIPTION
  This function is a callback issued by CM whenever a phone event happens.
  The TM_CM is only interested when the mode preferences changes to 911CB.

DEPENDENCIES
  tm_cm_client_init must have been called.

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/

void tm_cm_ph_event_cb
(
    cm_ph_event_e_type           ph_event,
        /* Indicate phone event */
    const cm_ph_info_s_type      *ph_info_ptr
        /* A pointer to a phone state information struct */
)
{

  #ifndef FEATURE_GNSS_LEAN_BUILD

  os_IpcMsgType           *ipc_msg_ptr = NULL;
  tm_cm_event_info_s_type *event_info_ptr;
  boolean status;
  static boolean u_in_emergency_mode = FALSE;
  cgps_CellScanListStructType      cell_info_list;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: ph_event %d rcv'ed", ph_event);

#ifdef FEATURE_CGPS_GTP_20_SUPPORT
  /* Handle WWAN scan results */
  if (ph_event == CM_PH_EVENT_AVAILABLE_NETWORKS_CONF ||
      ph_event == CM_PH_EVENT_PREFERRED_NETWORKS_CONF)
  {
    /* Check if PLMN scan was from TLE request */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: scan list event %d requesting client ID %d (tle ID %d)",
          (int)ph_event, (int)ph_info_ptr->requesting_client_id , tle_GetOrSetCmClientId(NULL));

    /* Disable check for CM requesting client ID for now (it is returning 3 when TLE CM client ID is 22) */
    /* if (ph_info_ptr->requesting_client_id == (cm_client_id_type)tle_GetOrSetCmClientId(NULL)) */
    {
      /* Check if PLMN scan results are available */
      if (NULL != ph_info_ptr->wwan_loc_scan_result)
      {
        /* CM results buffer is available */
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: get TLE scan results buffer pointer",0,0,0);
        
        /* Use TLE heap buffer for transferring the large scan results */
        void* pz_ResultElementBuffer = tle_GetOrSetScanResultBuffer(NULL);
      
        if (NULL != pz_ResultElementBuffer)
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: copying results to TLE scan results buffer",0,0,0);
      
          /* Copy scan results into TLE heap buffer */
          memcpy(pz_ResultElementBuffer,
                 (void*)&ph_info_ptr->wwan_loc_scan_result->wwan_scan_result_per_rat,
                 sizeof(ph_info_ptr->wwan_loc_scan_result->wwan_scan_result_per_rat));  
        }
        else
        {
          MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: no TLE scan results buffer available",0,0,0);
        }

        /* Free the CM results buffer after copying using ref_cnt_obj_release() in ref_count_obj.h */
        ref_cnt_obj_release((void*)ph_info_ptr->wwan_loc_scan_result);
      }
      else
      {
        MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: CM scan results buffer is NULL",0,0,0);
      }

      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb: notify TLE of new scan results",0,0,0);
      tle_NotifyScanResultsAvail();
      return;
    }
  }
  if(ph_event ==  CM_PH_EVENT_TERMINATE_GET_NETWORKS)
  {
	if(NULL != ph_info_ptr)
	{
	  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb:WWAN Scan is Aborted(%d)",ph_info_ptr->network_list_type,0,0);
      if(CM_NETWORK_LIST_GPS_ADVANCE_SEARCH == ph_info_ptr->network_list_type)
	  {
	    tle_NotifyAbortWwanScan();
	  }
	}
	else
	{
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_ph_event_cb:WWAN Scan is Aborted",0,0,0);
	}
  }
#endif /* FEATURE_CGPS_GTP_20_SUPPORT */

  /* Handle legacy PLMN search results */
  if (ph_event == CM_PH_EVENT_AVAILABLE_NETWORKS_CONF)
  {
    cell_info_list.status = true;
    memscpy(&cell_info_list.cell_list,sizeof(ph_info_ptr->cell_info_list),
                              &ph_info_ptr->cell_info_list,sizeof(ph_info_ptr->cell_info_list));
   tle_PlmnNotify(&cell_info_list);
   return;
  }

  if(ph_info_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"tm_cm_ph_event_cb: Null ph_info_ptr",0,0,0);
    return;
  }
  ipc_msg_ptr = os_IpcCreate(sizeof(tm_cm_event_info_s_type), IPC_ALLOCATION_DYNAMIC,(uint32)THREAD_ID_SM_TM);
  /* Queue the commands in Client command Queue.
  */
  if (ipc_msg_ptr == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_cm_ph_event_cb: IPC message allocation failed",0,0,0 );
  }
  else 
  {
    event_info_ptr = (tm_cm_event_info_s_type*)ipc_msg_ptr->p_Data;

    /* We only use the some of this large structure, so just copy the info needed 
     * to prevent overflowing the IPC memory
     */
    event_info_ptr->event_info.cm_event_info.ph_info.edrx_enabled = ph_info_ptr->edrx_enabled;
    if(ph_info_ptr->edrx_enabled)
    {
      event_info_ptr->event_info.cm_event_info.ph_info.edrx_cycle_length = ph_info_ptr->edrx_cycle_length;
    }
    event_info_ptr->event_info.cm_event_info.ph_info.ph_state = ph_info_ptr->ph_state;
    event_info_ptr->event_type = TM_CM_PH_EVENT;
    event_info_ptr->event_info.cm_event_type.ph_event = ph_event;
    ipc_msg_ptr->q_MsgId  = TM_CORE_CM_MSG_ID_EVENT_NTFY;
    status    = os_IpcSend(ipc_msg_ptr, (uint32)THREAD_ID_SM_TM);
    if (status == FALSE)
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Error in sending CM Event msg_id=%d to TM thread",ipc_msg_ptr->q_MsgId,0,0);
      status = os_IpcDelete(ipc_msg_ptr);
      ipc_msg_ptr = NULL;
    } /* End status */
    else
    {
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"sending CM Event msg_id=%d to TM thread", ipc_msg_ptr->q_MsgId,0,0);
    } /* end else status */
  }

  #endif

}

/*===========================================================================

FUNCTION  tm_cm_iface_event_proc

DESCRIPTION 
  Handles an incoming CM event

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/

void tm_cm_iface_event_proc( void* p_data )
{
  tm_cm_event_info_s_type   *info_ptr;
  
  if (p_data == NULL)
  
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_CM_IFace: Null pointer to function ", 0,0,0); 
    return;
  }
  info_ptr = (tm_cm_event_info_s_type*)p_data;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_CM_IFace: tm_cm_iface_event_proc() getting called",0,0,0);

  switch( info_ptr->event_type)
  {
    case TM_CM_PH_EVENT:
      tm_cm_handle_ph_event(info_ptr->event_info.cm_event_type.ph_event, &(info_ptr->event_info.cm_event_info.ph_info));
      break;
	
    case TM_CM_SS_EVENT:
      tm_cm_handle_ss_event(info_ptr->event_info.cm_event_type.ss_event, &(info_ptr->event_info.cm_event_info.ss_info));
      break;

    default:
      /* There is nothing we can do with this event
      */
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR," TM_CM_IFace: Event not handled ",info_ptr->event_type,0,0);
      break;
  } /* End Switch statement */
}

/*===========================================================================

FUNCTION  tm_cm_iface_cmd_proc

DESCRIPTION 
  Handles an incoming CM cmd

RETURN VALUE 
  None

DEPENDENCIES

===========================================================================*/

void tm_cm_iface_cmd_proc( void* p_data )
{
  tm_cm_cmd_info_s_type   *info_ptr;
  
  if (p_data == NULL)
  
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM_CM_IFace: Null pointer to function ", 0,0,0); 
    return;
  }
  info_ptr = (tm_cm_cmd_info_s_type*)p_data;

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM_CM_IFace: tm_cm_iface_cmd_proc() getting called",0,0,0);
}

/*===========================================================================

FUNCTION tm_cm_map_sys_mode

DESCRIPTION
  This function maps a sys_sys_mode_e_type variable to a cgps_SrvSystemType
  variable
 
PARAMETERS
  sys_mode - The sys_sys_mode_e_type variable to be mapped

DEPENDENCIES
  None.

RETURN VALUE
  cgps_SrvSystemType

SIDE EFFECTS
  None

===========================================================================*/
cgps_SrvSystemType tm_cm_map_sys_mode(sys_sys_mode_e_type sys_mode)
{
  switch(sys_mode)
  { 
    case SYS_SYS_MODE_GW:
      return CGPS_SRV_SYS_GW;

    case SYS_SYS_MODE_GSM:
      return CGPS_SRV_SYS_GSM;
	  
    case SYS_SYS_MODE_LTE:
      return CGPS_SRV_SYS_LTE;
  
    case SYS_SYS_MODE_NO_SRV:
      return CGPS_SRV_SYS_NONE;

    default:
      return CGPS_SRV_SYS_OTHER;
  }
}


/*===========================================================================

FUNCTION tm_cm_map_external_RAT

DESCRIPTION
  This function maps a pdsm_active_RAT_e_type variable to a cgps_SrvSystemType
  variable
 
PARAMETERS
  activeRat - The pdsm_active_RAT_e_type variable to be mapped

DEPENDENCIES
  None.

RETURN VALUE
  sys_sys_mode_e_type

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_e_type tm_cm_map_external_RAT(pdsm_lite_active_RAT_e_type activeRat)
{
  switch(activeRat)
  {     
    case PDSM_LITE_ACTIVERAT_GSM:
      return SYS_SYS_MODE_GSM;
      
    case PDSM_LITE_ACTIVERAT_LTE:
      return SYS_SYS_MODE_LTE;
      
    case PDSM_LITE_ACTIVERAT_NONE:
      return SYS_SYS_MODE_NO_SRV;

    default:
      return SYS_SYS_MODE_NO_SRV;
  }
}

/*===========================================================================

FUNCTION tm_cm_iface_get_phone_state

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_iface_get_phone_state(tm_cm_phone_state_info_s_type *phone_state_info_ptr)
{ 
  if((tm_cm_info.is_init == FALSE) || (phone_state_info_ptr == NULL))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"TM CM Iface not initialized",0,0,0);
    return;
  }

  phone_state_info_ptr->srv_system = tm_cm_map_sys_mode(tm_cm_info.sys_mode);

#ifdef FEATURE_CGPS_TLE_IOT
  if(phone_state_info_ptr->srv_system == CGPS_SRV_SYS_LTE)
  {
    if (tm_cm_info.ss_info.ciot_lte_mode == SYS_SYS_MODE_LTE_NB1)
    {
      phone_state_info_ptr->srv_system = CGPS_SRV_SYS_LTE_NB1;
    }
    else
    {
      phone_state_info_ptr->srv_system = CGPS_SRV_SYS_LTE_M1;
    }
  }
#endif
  phone_state_info_ptr->srv_status = tm_cm_info.srv_status;

  MSG_4(MSG_SSID_GPSSM, MSG_LEGACY_MED,
         "Returning  sys_mode = %d, srv_system = %d, srv_status = %d",
          tm_cm_info.sys_mode, phone_state_info_ptr->srv_system, phone_state_info_ptr->srv_status,0);

}
/*===========================================================================

FUNCTION tm_cm_client_init

DESCRIPTION
  Initialializes the TM_CM, registers callbacks.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None

===========================================================================*/

boolean tm_cm_client_init(void)
{
  bit_mask_256_t           event_mask;
  boolean                  ret_val = TRUE;
  boolean                  bit_set = TRUE;//#9x05
  
    /*---------------------------------
    Initialize the CM client
   *------------------------------ */
  tm_cm_info.is_init = FALSE;
#if defined ( FEATURE_GNSS_LEAN_BUILD ) 

  tm_cm_info.is_init = TRUE;
  return FALSE;
#else /* GRIFFON_DEBUG */


  //Size of CM IPC used
  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"Size of CM IPC used by SM_TM: %d bytes\n", sizeof(tm_cm_event_info_s_type), 0, 0);

  if(cm_client_init(CM_CLIENT_TYPE_PD, &tm_cm_info.cm_client) != CM_CLIENT_OK)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"cm_client_init failed",0,0,0);
    return FALSE;
  }

  /*---------------------------------
    Register for Phone events.
   *------------------------------ */
  /*---------------------------------
     DSDS Phone Event.
  *----------------------------------*/
  
  bit_set= bit_mask_256_set_bit(&event_mask,CM_PH_EVENT_OPRT_MODE);
  bit_set= bit_mask_256_set_bit(&event_mask,CM_PH_EVENT_AVAILABLE_NETWORKS_CONF);
  bit_set= bit_mask_256_set_bit(&event_mask,CM_PH_EVENT_EDRX_CHANGE_INFO_IND);
  bit_set= bit_mask_256_set_bit(&event_mask,CM_SS_EVENT_SRV_CHANGED);

#ifdef FEATURE_CGPS_GTP_20_SUPPORT
  bit_set= bit_mask_256_set_bit(&event_mask,CM_PH_EVENT_PREFERRED_NETWORKS_CONF);
  bit_set= bit_mask_256_set_bit(&event_mask,CM_PH_EVENT_TERMINATE_GET_NETWORKS);

#endif /* FEATURE_CGPS_GTP_20_SUPPORT */

  if(cm_client_ph_reg( tm_cm_info.cm_client,
                     tm_cm_ph_event_cb,
                     CM_CLIENT_EVENT_REG,
                     event_mask, /* Per subscription info*/
                     NULL    /* not interested in other clients' cmd errors */
                    ))

  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "cm_client_ph_reg for Operating Mode failed",0,0,0);
    return FALSE;
  }

  /*---------------------------------
    Register for serving system events.
   *------------------------------ */

  if(cm_mm_client_ss_reg(tm_cm_info.cm_client,        // pointer to the client struct
                       tm_cm_ss_event_cb,         // pointer to a cb function for ss events
                       CM_CLIENT_EVENT_REG,        // event registration
                       event_mask,    // from this call event
                       NULL             // pointer to a cb function for errors
                       ))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"cm_mm_client_ss_reg failed",0,0,0);
    return FALSE;
  }

  if(cm_client_act( tm_cm_info.cm_client) != CM_CLIENT_OK)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"cm_client_act failed",0,0,0);
    return FALSE;
  }

#ifdef FEATURE_REF_TRACKER_LOC
  cm_client_psm_enter_reg(tm_cm_psm_event_cb, CM_PSM_MODULE_LOCTECH);
#endif

  tm_cm_info.is_init = TRUE;

  tm_cm_info.sys_mode = SYS_SYS_MODE_NO_SRV;
  tm_cm_info.prev_sys_mode = SYS_SYS_MODE_NO_SRV;
  tm_cm_info.srv_status = SYS_SRV_STATUS_NO_SRV;
  tm_cm_info.prev_srv_status = SYS_SRV_STATUS_NO_SRV;

    tm_cm_info.ss_info.prev_sys_mode = SYS_SYS_MODE_NO_SRV;
    tm_cm_info.ss_info.sys_mode = SYS_SYS_MODE_NO_SRV;
    tm_cm_info.ss_info.srv_status = SYS_SRV_STATUS_NO_SRV;
    tm_cm_info.ss_info.srv_domain = SYS_SRV_DOMAIN_NO_SRV;
    tm_cm_info.ss_info.changed_fields = 0;
	tm_cm_info.curr_mcc = TM_CM_INVALID_MCC;
    
  return ret_val;
#endif /*GRIFFON_DEBUG*/
}

/*===========================================================================

FUNCTION tm_cm_map_ss_info

DESCRIPTION 
  Maps a  tm_cm_mm_ss_info_s_type data type to the coresponding cgps_SSInfoType
  data

DEPENDENCIES
  

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_map_ss_info( const tm_cm_mm_ss_info_s_type * const cp_ss_info_ptr,
            cgps_SSInfoType * p_cgps_ss_info, cgps_SSInfoDataType e_info_type)
{  
  if((NULL == p_cgps_ss_info) ||
   (NULL == cp_ss_info_ptr))
  {
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "p_cgps_ss_info or cp_ss_info_ptr is NULL");
  return;
  }

  memset(p_cgps_ss_info, 0x00, sizeof(*p_cgps_ss_info));
  p_cgps_ss_info->subs_id = 0;//#9x05: Now the number of stacks would be 0 only
  p_cgps_ss_info->number_of_stacks = 1; //#9x05: Now the number of stacks would be 1 only
      
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mapping changed_fields:%u, sys_mode:%u, sys_id_id_type:%u",
          cp_ss_info_ptr->stack_info.changed_fields, 
          cp_ss_info_ptr->stack_info.sys_mode, 
          cp_ss_info_ptr->stack_info.sys_id.id_type);
      
      if(cp_ss_info_ptr->stack_info.changed_fields & CM_SS_MOBILITY_MGMT_MASK)
      {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mobility management mask is set");
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Date mm/dd/yy  %u/%u/%u",
           cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.month, 
           cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.day,
           cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.year);
      
      
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Time hh:mm:ss %u:%u:%u",
           cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.hour,
           cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.minute,
           cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.second);
      }
      else
      {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "Mobility management mask is not detected");
      }
    //#9x05 check gps_common.h 
      p_cgps_ss_info->multisim_ssInfo[0].ss_data_type = e_info_type;//#9x05 gps_common.h we can define variabke instead of an array
      p_cgps_ss_info->multisim_ssInfo[0].sys_mode = cp_ss_info_ptr->stack_info.sys_mode;
    
      p_cgps_ss_info->multisim_ssInfo[0].srv_status = cp_ss_info_ptr->stack_info.srv_status;
      
      if (cp_ss_info_ptr->stack_info.sys_id.id_type == SYS_SYS_ID_TYPE_IS95)
      {
        p_cgps_ss_info->multisim_ssInfo[0].sid = cp_ss_info_ptr->stack_info.sys_id.id.is95.sid;
        p_cgps_ss_info->multisim_ssInfo[0].nid = cp_ss_info_ptr->stack_info.sys_id.id.is95.nid;
      }
      else
      {
        p_cgps_ss_info->multisim_ssInfo[0].sid = CM_SS_SID_NONE;//#9x05 needed?
        p_cgps_ss_info->multisim_ssInfo[0].nid = CM_SS_NID_NONE;
      }
      //#9x05 multisim array?
      p_cgps_ss_info->multisim_ssInfo[0].changed_fields = cp_ss_info_ptr->stack_info.changed_fields;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.daylight_saving_adj_avail = 
              cp_ss_info_ptr->stack_info.mm_information.daylight_saving_adj_avail;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.time_zone = 
              cp_ss_info_ptr->stack_info.mm_information.time_zone;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.time_zone_avail = 
              cp_ss_info_ptr->stack_info.mm_information.time_zone_avail;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.day = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.day;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.hour = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.hour;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.minute = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.minute;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.month = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.month;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.second = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.second;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.time_zone  = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.time_zone;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone.year  = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone.year;
      p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.univ_time_and_time_zone_avail  = 
              cp_ss_info_ptr->stack_info.mm_information.univ_time_and_time_zone_avail;
      
      if(cp_ss_info_ptr->stack_info.mm_information.daylight_saving_adj_avail)
      {
        switch(cp_ss_info_ptr->stack_info.mm_information.daylight_saving_adj)
        {
        case SYS_DAY_SAV_ADJ_NO_ADJUSTMENT:
          p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_NO_ADJUSTMENT;
          break;
              
        case SYS_DAY_SAV_ADJ_PLUS_ONE_HOUR:
          p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_PLUS_ONE_HOUR;
          break;
          
        case SYS_DAY_SAV_ADJ_PLUS_TWO_HOUR:
          p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_PLUS_TWO_HOUR;
          break;
          
        default:
          p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_NONE;
        }
      }
      else
      {
        p_cgps_ss_info->multisim_ssInfo[0].z_TimeZoneInfo.daylight_saving_adj = CGPS_DAY_SAV_ADJ_NONE;
      }
      
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM CM Iface, SID:%u, NID:%u, base_id:%u",
          p_cgps_ss_info->multisim_ssInfo[0].sid, p_cgps_ss_info->multisim_ssInfo[0].nid, 
          p_cgps_ss_info->multisim_ssInfo[0].base_id);
      
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM CM Iface, sys_mode:%u, srch_win_n:%u, ss_data_type:%u",
          (int32) p_cgps_ss_info->multisim_ssInfo[0].sys_mode, p_cgps_ss_info->multisim_ssInfo[0].srch_win_n, p_cgps_ss_info->multisim_ssInfo[0].ss_data_type); 
#ifdef FEATURE_CGPS_TLE_IOT
      /* CIOT info */
      p_cgps_ss_info->multisim_ssInfo[0].ciot_lte_mode = cp_ss_info_ptr->stack_info.ciot_lte_mode;
      MSG_2(MSG_SSID_GPSSM, MSG_LEGACY_MED, "TM CM Iface, sys_mode:%u, ciot_lte_mode:%u",
        (int32) p_cgps_ss_info->multisim_ssInfo[0].sys_mode,
        (int32) p_cgps_ss_info->multisim_ssInfo[0].ciot_lte_mode);
#endif /* FEATURE_CGPS_TLE_IOT */
    
}

/*===========================================================================

FUNCTION tm_core_cm_notify_srv_status

DESCRIPTION 
  Sends serving status change notification to TM core if needed.
  Also sends serving status change notification to PDCOMM if needed.

DEPENDENCIES
  

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
static void tm_core_cm_notify_srv_status(void)
{
  boolean notify_tm = FALSE;  
  boolean notify_tle_mgp = FALSE;
    
  tm_phone_event_payload_u_type *cm_evt_payload_ptr=NULL;

  
#ifdef FEATURE_CGPS_CELLDB
  cgps_SSInfoType ss_info;
#endif
  sys_sys_mode_e_type  cdma_stack_sys_mode       =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  lte_stack_sys_mode        =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  external_stack_sys_mode   =   SYS_SYS_MODE_NO_SRV;
  sys_sys_mode_e_type  sys_mode                  =   SYS_SYS_MODE_NO_SRV;
  sys_srv_status_e_type srv_status               =   SYS_SRV_STATUS_NO_SRV;

  if (tm_cm_info.srv_status != srv_status)
  { /* Update if serving status has changed. */
    tm_cm_info.prev_srv_status = tm_cm_info.srv_status;
    tm_cm_info.srv_status = srv_status;
  } 
  if((tm_cm_info.sys_mode != sys_mode))
  {
    tm_cm_info.prev_sys_mode = tm_cm_info.sys_mode;
    tm_cm_info.sys_mode = sys_mode;		
  
    notify_tm = TRUE;
    notify_tle_mgp = TRUE;
  
  }
  if(notify_tm)
  {
    if ( tm_cm_info.sys_mode == SYS_SYS_MODE_CDMA )
    {
           cm_evt_payload_ptr = 
                (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
    }
    else if (( tm_cm_info.sys_mode == SYS_SYS_MODE_GW ) ||   //#9x05 gw needed?
              ( tm_cm_info.sys_mode == SYS_SYS_MODE_GSM ) ||
              ( tm_cm_info.sys_mode == SYS_SYS_MODE_LTE ) 
            )
    {
         cm_evt_payload_ptr = 
                (tm_phone_event_payload_u_type *)pdsm_getbuf(sizeof(tm_phone_event_payload_u_type));
           if(cm_evt_payload_ptr != NULL)
           {
             /* processing of srv_status_mask and mode_mask identical, use SS_SRV_STATUS for all */
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_MED, "tm_core_cm_notify_srv_status: Sending Current and Previous Serving stack information");

             (void)pdsm_freebuf((char *)cm_evt_payload_ptr);
      }
    else
      {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_ERROR, "tm_core_cm_notify_srv_status: No buffer available");
           }
    }
    else if( tm_cm_info.sys_mode == SYS_SYS_MODE_NO_SRV )
    {
      tm_core_cm_notify_event(TM_PHONE_EVENT_OOS, NULL);
    }        
  }

  if (notify_tle_mgp)
  {
#ifdef FEATURE_CGPS_CELLDB
      memset(&ss_info, 0x00, sizeof(ss_info));
      ss_info.number_of_stacks = 1;

#ifdef FEATURE_CGPS_TLE_IOT
      ss_info.multisim_ssInfo[0].ciot_lte_mode = tm_cm_info.ss_info.ciot_lte_mode;
      MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_core_cm_notify_srv_status: Serving sys_mode %u, ciot_lte_mode %u and srv_status %u passed to TLE and MGP",
        ss_info.multisim_ssInfo[0].sys_mode, ss_info.multisim_ssInfo[0].ciot_lte_mode, ss_info.multisim_ssInfo[0].srv_status);
#endif /* FEATURE_CGPS_TLE_IOT */	

#ifdef FEATURE_CGPS_XTRA_T
      tle_SSChange(&ss_info);
#endif /* FEATURE_CGPS_XTRA_T */
      mgp_SSChange(&ss_info);
      /* call GTS function only for SS change */
      #ifdef FEATURE_LOC_GTS_SUPPORT
      gts_SSChange(&ss_info);
      #endif /* #ifdef FEATURE_LOC_GTS_SUPPORT */
#endif /* FEATURE_CGPS_CELLDB*/ 
  }
}

#ifdef FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH

/*===========================================================================

FUNCTION tm_cm_request_wcdma_dch_cell_info

#9x05 dummy remove

===========================================================================*/
boolean tm_cm_request_wcdma_dch_cell_info(void)
{
return TRUE;
}

#endif //FEATURE_WCDMA_CM_CID_UPDATE_IN_DCH


/*===========================================================================

FUNCTION tm_cm_process_external_srv_changed

DESCRIPTION
  This function processes srv changes from external modem

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_process_external_srv_changed(
    const pdsm_lite_cell_info_s_type *external_cell_db_info_ptr
)
{
  sys_modem_as_id_e_type sglte_sub = SYS_MODEM_AS_ID_NONE;
  
  if (tm_cm_info.external_stack_ss_info.prev_sys_mode !=
      tm_cm_info.external_stack_ss_info.sys_mode)
  {
     tm_cm_info.external_stack_ss_info.prev_sys_mode =  
    tm_cm_info.external_stack_ss_info.sys_mode;
  }  

  /* Checking two cases to verify if external modem is OOS
        1. When the serviceStatus says OOS or UNK
        2. When LAC and CID is -1. This happens when we receive an inject external cell update
            that indicate that we are camped on a non-gsm RAT. And for SGLTE that means its camped on 
            the main modem. If RAT is non-gsm then LAC and CID will be -1
    */
  if (((external_cell_db_info_ptr->serviceStatus == STATUS_OUT_OF_SERVICE) ||
       (external_cell_db_info_ptr->serviceStatus == STATUS_UNKNOWN)) ||
      ((external_cell_db_info_ptr->cellInfo.gsmCell.LAC == HEX_NEGATIVE_ONE) &&
       (external_cell_db_info_ptr->cellInfo.gsmCell.CID == HEX_NEGATIVE_ONE)))
  {
    tm_cm_info.external_stack_ss_info.sys_mode = SYS_SYS_MODE_NO_SRV; 
  } 
  else
  {
    tm_cm_info.external_stack_ss_info.sys_mode = 
    tm_cm_map_external_RAT(external_cell_db_info_ptr->activeRAT);
    
  }


  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH,"tm_cm_process_external_srv_changed: Prev_sys_mode = %d and current sys_mode = %d and activeExternalRat = %d ",tm_cm_info.external_stack_ss_info.prev_sys_mode
            ,tm_cm_info.external_stack_ss_info.sys_mode, external_cell_db_info_ptr->activeRAT);

}
/*===========================================================================

FUNCTION tm_cm_is_cell_info_update_needed
# 9x05 svlte and srlte removed
returns TRUE always considers SYS_SUBS_FEATURE_MODE_NORMAL , check if it
can be replaced by #define
Do we want a default failure case?
===========================================================================*/
LOCAL boolean tm_cm_is_cell_info_update_needed()

{
  return TRUE;
}
/*===========================================================================

FUNCTION tm_cm_convert_plmn_id_format

DESCRIPTION
  This function converts from the NAS PLMN ID format (2 BCD digits per byte)
  to the RRC PLMN_id_T format (1 BCD digit per byte).

DEPENDENCIES
  None

RETURN VALUE
  PLMN ID converted to rrc_plmn_id_type format

SIDE EFFECTS
  None

===========================================================================*/

static void tm_cm_convert_plmn_id_format( sys_plmn_id_s_type plmn_id, uint8 mcc[3], uint8 mnc[3], uint8 *num_mnc_digits)
{
   /* ----------------------------------------
   ** PLMN octet 1 = MCC digit 2 | MCC digit 1
   ** ---------------------------------------- */
   mcc[0] = plmn_id.identity[0] & 0x0F;
   mcc[1] = (plmn_id.identity[0] & 0xF0) >> 4;

   /* ----------------------------------------
   ** PLMN octet 2 = MNC digit 3 | MCC digit 3
   ** ---------------------------------------- */
   mcc[2] = plmn_id.identity[1] & 0x0F;

   if ((plmn_id.identity[1] & 0xF0) == 0xF0)
   {
      *num_mnc_digits = 2;
      mnc[2] = 0x0F; /* Set the 3rd digit to 0xF as per the spec */
   }
   else
   {
      *num_mnc_digits = 3;
      mnc[2] = (plmn_id.identity[1] & 0xF0) >> 4;
   }

   /* ----------------------------------------
   ** PLMN octet 3 = MNC digit 2 | MNC digit 1
   ** ---------------------------------------- */
   mnc[0] = plmn_id.identity[2] & 0x0F;
   mnc[1] = (plmn_id.identity[2] & 0xF0) >> 4;

} /* end tm_cm_convert_plmn_id_format() */


// #9x05 can replace by a#define needs rework on process_cell after confirmation
/*===========================================================================

FUNCTION tm_cm_process_cell_info_update

DESCRIPTION 
  Maps a CM Cell Info sys_cell_info_s_type data type to the coresponding
  cgps_CellInfoCachedStruct data

DEPENDENCIES
  

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void tm_cm_process_cell_info_update( const tm_cm_mm_ss_stack_info_s_type *const cp_ss_info_ptr)

{
  cgps_CellInfoCachedStruct  cgps_cell_info;
  uint32 mcc, mnc;
  sys_cell_info_s_type  *cp_cell_info_ptr = NULL;

  if (cp_ss_info_ptr == NULL)
  {
    return;
  }

  cp_cell_info_ptr = (sys_cell_info_s_type *)&cp_ss_info_ptr->cell_info;
  memset(&cgps_cell_info, 0x00, sizeof(cgps_cell_info));

  mcc = ( ((uint32)cp_cell_info_ptr->plmn_id.identity[0] & 0x0F) << 8 ) |
          ((uint32)cp_cell_info_ptr->plmn_id.identity[0] & 0xF0) |
          ((uint32)cp_cell_info_ptr->plmn_id.identity[1] & 0x0F);
  mnc = ( ((uint32)cp_cell_info_ptr->plmn_id.identity[2] & 0x0F) << 8 ) |
          ((uint32)cp_cell_info_ptr->plmn_id.identity[2] & 0xF0) |
        ( ((uint32)cp_cell_info_ptr->plmn_id.identity[1] & 0xF0) >> 4 );
  if(tm_cm_info.curr_mcc != mcc)
  {
    tm_cm_info.curr_mcc = mcc;
    tm_core_handle_updated_mcc(mcc);
  }
  if ((mnc & 0x0F) == 0x0F)
  {
    mnc >>= 4;  // if last one is 0x0F, then it's two-digit only, so right-shift 4 bits 
  }

  MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "CM_SS_CELL_INFO_MASK set by CM, CID=%d, PLMN(MCC=%x MNC=%x)",
           (uint32)cp_cell_info_ptr->cell_id, mcc, mnc);

  cgps_cell_info.u_CachedCellIDValid = TRUE;
  cgps_cell_info.q_NumCellChangesCachedCellID = 1;

  switch (cp_cell_info_ptr->as_sys_mode) // serving system type
  {

#if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2)
  case SYS_SYS_MODE_GSM: /* 3, GSM mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_GSM;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.camped_on_flag = TRUE;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.cell_identity = cp_cell_info_ptr->cell_id;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.plmn_id = cp_cell_info_ptr->plmn_id;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.location_area_identification.location_area_code = cp_cell_info_ptr->lac_id;
    /**< Absolute Radio Frequency Channel Number (ARFCN) of the serving system
       (applicable only in GSM mode). Range is 0 to 1023. */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn.num = cp_cell_info_ptr->arfcn;
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.arfcn.band = 0; // no band info from CM
    /**< Base Station Identity Code of the serving system (applicable only in GSM mode). 
      Range is 0 to 63 (see 3GPP TS 24.008, section 10.5.1.3). */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoGSMGlobal.bsic = cp_cell_info_ptr->bsic;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "GSM CID Update: LAC=%d ARCFN=%d BSIC=%d", cp_cell_info_ptr->lac_id,
             cp_cell_info_ptr->arfcn, cp_cell_info_ptr->bsic);
    break;

    /* Obtain the plmn_id information of the system. */
    /* Refer to the sys_plmn_id_s_type to find the format of the plmn_id */
    /** Public Land Mobile Network (PLMN) ID.

    A PLMN ID is defined within a GSM network to be the combination of a Mobile
    Country Code (MCC) and Mobile Network Code (MNC). A PLMN ID is stored in
    three octets, as specified in 3G TS 24.008, Section 10.5.1.3 and shown below:
                                        Bit
                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
                 |===============================================|
         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
                 |-----------------------------------------------|
         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
                 |-----------------------------------------------|
         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
                 |===============================================|
    [ Digit range is 0..9 ]
    */

#endif /* if defined(FEATURE_CGPS_UMTS_CELLDB) || defined (FEATURE_FSM_V2) */

#ifdef FEATURE_CGPS_LTE_CELLDB
  case SYS_SYS_MODE_LTE: /* 9, LTE mode */
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_LTE;
    tm_cm_convert_plmn_id_format(cp_cell_info_ptr->plmn_id,
         cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mcc,
         cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.mnc,
         &cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.selected_plmn.num_mnc_digits);
    /**< Downlink the Evolved-UTRA ARFCN (EARFCN) of the serving system. 
      Applicable for LTE systems. (DL Center frequency of cell) */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.freq = cp_cell_info_ptr->earfcn_dl;
    /* Physical cell id of cell  CM is saving LTE physical cell ID in psc after CR 622382 */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_id = cp_cell_info_ptr->psc; 
    /* Global cell id (from SIB1) of cell */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.cell_identity = cp_cell_info_ptr->cell_id;
    /**< TAC(Tracking Area Code) information for LTE. */
    cgps_cell_info.z_CellInfoGlobalCached.z_CellInfoLTEGlobal.tac = cp_cell_info_ptr->tac;
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_MED, "LTE CID Update: TAC=%d Freq=%d PSC=%d", cp_cell_info_ptr->tac,
             cp_cell_info_ptr->earfcn_dl, cp_cell_info_ptr->psc);
    break;
#endif /* FEATURE_CGPS_LTE_CELLDB */

  case SYS_SYS_MODE_GW: /* 7, GSM and WCDMA mode */  // FIXME: should we leave GW here?
  case SYS_SYS_MODE_GWL: /* 10, GSM/WCDMA/LTE mode */ 
  case SYS_SYS_MODE_NO_SRV: /* 0, NO Service */
  default:  /* All modes not covered above */
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Other CID Update: SS=%d CID=%d", 
              (uint32)cp_ss_info_ptr->sys_mode, cp_cell_info_ptr->cell_id, 0);
    cgps_cell_info.e_ActiveRAT = CELLDB_ACTIVERAT_NONE;
    cp_cell_info_ptr->cell_id = 0xFFFFFFFF; // mark it as invalid
    break;
  }

  /* The cell ID returned is invalid */
  if(cp_cell_info_ptr->cell_id == 0xFFFFFFFF)
  {
    cgps_cell_info.u_CachedCellIDValid = FALSE;
    cgps_cell_info.q_NumCellChangesCachedCellID = 0xFFFFFFFF; /* Set to a maximum */
  }
  gm_cell_db_update(cgps_cell_info.e_ActiveRAT, (const cgps_CellInfoCachedStruct *)&cgps_cell_info);
  /* Send Cell ID update IPC TM_CORE_MSG_ID_MC_CELLDB_UPDATE to TM task
     TM task's tm_core_cell_db_proc() function will then send to SUPL, LPP CP, GM, etc */
}
/*===========================================================================
FUNCTION tm_cm_ph_cmd_get_networks_cb

DESCRIPTION

RETURN VALUE

DEPENDENCIES
  None.
===========================================================================*/
void tm_cm_ph_cmd_get_networks_cb
(
  void                         *data_ptr,         /* Data block pointer    */
  cm_ph_cmd_e_type              cmd,              /* Command ID            */
  cm_ph_cmd_err_e_type          cmd_err           /* Command error code    */
)
{
  cgps_CellScanListStructType cell_scan_list = {0};
  (void)data_ptr;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "tm_cm_ph_cmd_get_networks_cb  cmd:%d",cmd);
  if((cmd != CM_PH_CMD_GET_NETWORKS) && (cmd_err != CM_PH_CMD_ERR_NOERR))
  {
    cell_scan_list.status = false;
    tle_PlmnNotify(&cell_scan_list);
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Request to get cell scan requests failed ");
    return;
  }
  else
  {
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Request to get cell scan requests success");
  }

} /* tm_cm_ph_cmd_cb_func */

/*===========================================================================

FUNCTION tm_cm_request_plmn_search

DESCRIPTION
  This function is used to request plmn cell scan from cm 

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
void tm_cm_request_plmn_search(void *p_data)
{
  tle_tm_plmn_search *info_ptr;
  cgps_CellScanListStructType cell_scan_list = {0};
  info_ptr = (tle_tm_plmn_search *)p_data;

  if (FALSE == cm_ph_cmd_get_networks_per_subs_ciot_num_cells(
                                       tm_cm_ph_cmd_get_networks_cb,
                                       NULL,
                                       tm_cm_info.cm_client,
                                       info_ptr->list_type,
                                       info_ptr->network_type,
                                       info_ptr->asubs_id,
                                       info_ptr->band_pref_lmt,
                                       info_ptr->lte_band_pref_lmt,
                                       info_ptr->tds_band_pref_lmt,
                                       info_ptr->ciot_lte_pref,
                                       info_ptr->cell_search_num_cells
                                       ))
  {
    cell_scan_list.status = FALSE;
    tle_PlmnNotify(&cell_scan_list);
    MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "FAILED to request cell scan from CM");
    return ;
  }
  MSG(MSG_SSID_GPSSM, MSG_LEGACY_HIGH, "Cell scan request to CM successfull");
}


#ifdef FEATURE_REF_TRACKER_LOC

/*===========================================================================

FUNCTION tm_cm_psm_event_cb

DESCRIPTION
  This function is a callback for psm module 

DEPENDENCIES


RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
boolean tm_cm_psm_event_cb(void )
{
  locMW_SendIpcMsg(LM_MIDDLEWARE_MSG_ID_PSM_ENTRY_IND, 0, NULL);	
}

#endif
