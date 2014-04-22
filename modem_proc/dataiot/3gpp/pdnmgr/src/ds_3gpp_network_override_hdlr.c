/*===========================================================================
                      DS_PDN_NETWORK_OVERRIDE_HDLR.C
DESCRIPTION
This file defines network override handler APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_network_override_hdlr.c#1 $
  $DateTime: 2020/03/17 08:03:16 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ds_pdn_defs.h"
#include "ds_3gpp_network_override_hdlr.h"
#include "ds_3gpp_pdn_throttle_sm.h"
#include "ds_pdn_nv_manager.h"
#include "dssysmgr.h"


/*===========================================================================

                  FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION    DS_3GPP_NETWORK_OVERRIDE_CLEAR_THROT

DESCRIPTION
  This function checks is used deallocate memory for the network override
  table and also unblock LTE PLMN if previously blocked
 
PARAMETERS 
  plmn_id                     - pointer to the Current PLMN ID
  per_rat_ntw_override_info_p - Pointer to the network override based throt SM
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE  - If PLMN present in the ePLMN list
  FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static void ds_3gpp_network_override_clear_throt
(
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr,
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s       *per_rat_ntw_override_info_p,
  ds_3gpp_throt_rat_e_type                 rat_type
)
{
  sys_plmn_id_s_type                       unblock_plmn;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((NULL == per_apn_throt_sm_ptr) || (NULL == per_rat_ntw_override_info_p))
  {
    return;
  }

  if (rat_type == DS_3GPP_THROT_RAT_LTE)
  {
    memscpy(&unblock_plmn,
            sizeof(sys_plmn_id_s_type),
            &(per_rat_ntw_override_info_p->plmn_id),
            sizeof(sys_plmn_id_s_type));

    /*---------------------------------------------------------------------
      Setting it to PDP_UNDEFINIED here will make sure that the overridden
      IP type will return PDP_UNDEFINED when trying to unblock PLMN later 
    ---------------------------------------------------------------------*/
    per_rat_ntw_override_info_p->nw_overridden_ip_bmask = 
              DS_3GPP_PDP_NON_IP_BMASK;

    ds_3gpp_pdn_throt_enable_lte_plmn_if_attach_prof_available(unblock_plmn);
  }

  if (per_rat_ntw_override_info_p->eplmn_list_ptr)
  {
    DS_SYSTEM_HEAP_MEM_FREE(per_rat_ntw_override_info_p->eplmn_list_ptr);
  }

  DS_SYSTEM_HEAP_MEM_FREE(per_rat_ntw_override_info_p);

  per_rat_ntw_override_info_p = NULL;
  per_apn_throt_sm_ptr->network_override_ip_info_p[rat_type] = NULL;

}/* ds_3gpp_network_override_clear_throt */

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET

DESCRIPTION
  This function initializes the data structure to NULL
 
  Exception:- If rel12_plus check is set to TRUE
  The data structures are to be initialized to NULL only if the network
  override handler was set by PDN CONN REJ received

PARAMETERS 
   rel2 version 

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset
(
  boolean                rel12_plus_check
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  uint8                                   apn_loop_index = 0;
  uint8                                   ntw_override_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s       *per_rat_ntw_override_info_p = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_cause      override_cause 
                                            = DS_3GPP_PDN_THROT_NTW_O_RIDE_CAUSE_MAX;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl();

    if(apn_based_tbl_ptr == NULL)
    {
      break;
    }

    for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
    {
      per_apn_throt_sm_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];
  
      if((per_apn_throt_sm_ptr == NULL) || (!per_apn_throt_sm_ptr->valid))
      {
        continue;
      }
  
      for (ntw_override_index = 0;ntw_override_index < DS_3GPP_THROT_RAT_MAX;
           ntw_override_index++)
      {
        per_rat_ntw_override_info_p = 
          per_apn_throt_sm_ptr->network_override_ip_info_p[ntw_override_index];

        if (per_rat_ntw_override_info_p == NULL)
        {
          continue;
        }

        override_cause = per_rat_ntw_override_info_p->override_cause;
  
      /*--------------------------------------------------------------------- 
          If REL 12 plus check is enabled reset only if override_cause is
          not BEARER_ACT_ACCEPT
          ---------------------------------------------------------------------*/
        if((!rel12_plus_check) || 
           (rel12_plus_check && 
            override_cause != DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT))
        {
          ds_3gpp_network_override_clear_throt(
             per_apn_throt_sm_ptr, 
             per_rat_ntw_override_info_p,
             (ds_3gpp_throt_rat_e_type)ntw_override_index);
        }
      }
  
      ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed(per_apn_throt_sm_ptr,
                                                          TRUE);
    }
  }while(0); 
} /*ds_3gpp_network_override_hdlr_reset*/

/*===========================================================================
FUNCTION    DS_3GPP_CHECK_NTW_THROT_SM_IF_PLMN_PRESENT_IN_EPLMN_LIST

DESCRIPTION
   This function checks if a particular PLMN ID is present in the ePLMN
   list of the network override throttle SM table. 
 
PARAMETERS 
  plmn_id                     - pointer to the Current PLMN ID
  per_rat_ntw_override_info_p - Pointer to the network override based throt SM
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE  - If PLMN present in the ePLMN list
  FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_check_ntw_throt_sm_if_plmn_present_in_eplmn_list
(
  sys_plmn_id_s_type                  *plmn_id_ptr, 
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s   *per_rat_ntw_override_info_p
)
{
  uint8 eplmn_index;
  sys_plmn_list_s_type *eplmn_list_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do 
  {
    if((NULL == plmn_id_ptr ) || (NULL == per_rat_ntw_override_info_p))
    {
      break;
    }

    if(memcmp((void*)(plmn_id_ptr),
              (void*)(&per_rat_ntw_override_info_p->plmn_id),
              sizeof(sys_plmn_id_s_type)) == 0)
    {      
      return TRUE;
    }

    eplmn_list_ptr = per_rat_ntw_override_info_p->eplmn_list_ptr;

    if (NULL == eplmn_list_ptr)
    {     
      break;
    }

    /*------------------------------------------------------------------------
      If ePLMN list present, then loop through it to find if the current PLMN
      that is being throttled is present
    ------------------------------------------------------------------------*/    
    for (eplmn_index = 0; eplmn_index < eplmn_list_ptr->length; eplmn_index++)
    {
      if (memcmp((void*)(plmn_id_ptr),
                 (void*)(&eplmn_list_ptr->plmn[eplmn_index]),
                 sizeof(sys_plmn_id_s_type)) == 0)
      {
        /*------------------------------------------------------------------------
          ePLMN match. Break from the loop. We have found the entry in the table
          for the current PLMN
        ------------------------------------------------------------------------*/
        DS_MSG0(MSG_LEGACY_HIGH,"REL_13: Passed PLMN is an ePLMN");
        return TRUE;
      }
    } /* EPLMN List loop*/
  }
  while (0); 

  return FALSE;
}

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET_IF_PLMN_CHANGED

DESCRIPTION
  This function initializes the data structure to NULL

PARAMETERS
   PLMN id
   rat type
   REL 12 check

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset_if_plmn_changed
(
  sys_plmn_id_s_type          plmn_id,
  ds_3gpp_throt_rat_e_type    rat_type,
  boolean                     rel_ver_check
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  uint8                                    apn_loop_index = 0;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s       *per_rat_ntw_override_info_p = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_cause       override_cause = 
                                           DS_3GPP_PDN_THROT_NTW_O_RIDE_CAUSE_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl();

    if(apn_based_tbl_ptr == NULL)
    {
      break;
    }

    if((rat_type != DS_3GPP_THROT_RAT_LTE) &&
       (rat_type != DS_3GPP_THROT_RAT_UMTS))
    {
        break;
    }

    for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
    {
      per_apn_throt_sm_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];

      if((per_apn_throt_sm_ptr == NULL) || (!per_apn_throt_sm_ptr->valid))
      {
        continue;
      }

      per_rat_ntw_override_info_p = 
        per_apn_throt_sm_ptr->network_override_ip_info_p[rat_type];

      if (per_rat_ntw_override_info_p == NULL)
      {
        continue;
      }

      if (memcmp(&per_rat_ntw_override_info_p->plmn_id, 
           &plmn_id, sizeof(sys_plmn_id_s_type)) == 0)
      {
        continue;
      }
      /*--------------------------------------------------------------------- 
        Clear throttling info only if -
         1. Current PLMN ID is different from the one that is already throttled and 
         2. It's not in the list of ePLMNs in the throttle SM or 
         3. If it's not in the ePLMN list returned by NAS 
      ---------------------------------------------------------------------*/
      if ((per_rat_ntw_override_info_p->eplmn_list_ptr != NULL) && 
          (ds_3gpp_check_ntw_throt_sm_if_plmn_present_in_eplmn_list(
             &plmn_id, per_rat_ntw_override_info_p) || 
           ds_3gppi_throt_is_plmn_eplmn(
             plmn_id, per_rat_ntw_override_info_p->plmn_id))
         )
      {
        DS_MSG0(MSG_LEGACY_HIGH,"REL_13: Not clearing throttle information ");
        continue;
      }

      override_cause = per_rat_ntw_override_info_p->override_cause;

 /*--------------------------------------------------------------------- 
        If REL 12 plus check is enabled reset only if override_cause is
        not BEARER_ACT_ACCEPT
 ---------------------------------------------------------------------*/
      if((!rel_ver_check) || 
         (rel_ver_check && override_cause != DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT))
      {
        ds_3gpp_network_override_clear_throt( per_apn_throt_sm_ptr, 
                                              per_rat_ntw_override_info_p,
                                              rat_type);
      }

      ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed(per_apn_throt_sm_ptr,TRUE);
    }
  }while(0); 
}/*ds_3gpp_network_override_hdlr_reset_if_plmn_changed*/

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_SET_OVERRIDDEN_IP

DESCRIPTION
  This function sets the passed IP type based on the apn entry passed.
  If the apn entry does not exist, it would create a new entry

PARAMETERS
   byte                              *apn
   ds_3gpp_network_overrridden_ip_type_e   ip_type
   override cause
   rat

DEPENDENCIES
  None.

RETURN VALUE
  boolean 

SIDE EFFECTS
  None

===========================================================================*/
boolean ds_3gpp_network_override_hdlr_set_overridden_ip
(
   byte                                      *apn,
   ds_3gpp_pdn_throt_ntw_o_ridden_ip_type_e   overridden_ip,
   ds_3gpp_pdn_throt_ntw_o_ride_cause         override_cause,
   uint32                                     rat_mask,
   reattempt_ind_T                            reattempt_indicator
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s     *apn_index_ptr = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s           *rat_based_pdn_throt_p = NULL;
  boolean                                      ret_val = TRUE;
  uint32                                       temp_rat_mask = 0;
  ds_3gpp_throt_rat_e_type                     rat_type = 0;
  boolean                                      block_eplmns = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    if (apn == NULL)
    {
      break;
    }

    if(rat_mask == 0)
    {
      break;
    }
	
    if (overridden_ip != DS_3GPP_PDN_THROT_PDP_UNDEFINED)
    {
      apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_index_ptr
                           (apn,TRUE);
    
      if ((apn_index_ptr == NULL) || (!apn_index_ptr->valid))
      {
        break;
      }

      temp_rat_mask = rat_mask;

      /* Block all ePLMNs only if reattempt indicator is valid and EPLMN bit is set */
      if (reattempt_indicator.valid)
      {
        block_eplmns = (reattempt_indicator.reattempt_ind_content &
                          DS_3GPP_THROT_REATTEMPT_EPLMN_BMASK);
      }

      while ( temp_rat_mask != DS_3GPP_THROT_RAT_MASK_NONE)
      {
        rat_type = ds_3gpp_pdn_throt_get_next_rat_type(&temp_rat_mask);
        
        rat_based_pdn_throt_p = 
                           apn_index_ptr->network_override_ip_info_p[rat_type];

        if (rat_based_pdn_throt_p == NULL)
        {
          DS_SYSTEM_HEAP_MEM_ALLOC(rat_based_pdn_throt_p,
                                   sizeof(ds_3gpp_pdn_throt_ntw_o_ride_ip_s),
                                   ds_3gpp_pdn_throt_ntw_o_ride_ip_s *);

          apn_index_ptr->network_override_ip_info_p[rat_type] = 
                                                      rat_based_pdn_throt_p;

          if (rat_based_pdn_throt_p == NULL)
          {
            break;
          }
        }

        memset(rat_based_pdn_throt_p, 0, 
               sizeof(ds_3gpp_pdn_throt_ntw_o_ride_ip_s));

        rat_based_pdn_throt_p->nw_overridden_ip_bmask = 
            DS_3GPP_GET_NW_OVERRIDE_BMASK(overridden_ip);

        /*-------------------------------------------------------------------------
          If connection was accepted, then make sure overridden IP bmask is set
         to allow NON-IP type as well. 
        --------------------------------------------------------------------------*/
        if (DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT == override_cause)
        {
          rat_based_pdn_throt_p->nw_overridden_ip_bmask |= 
              DS_3GPP_PDP_NON_IP_BMASK;
        }

        rat_based_pdn_throt_p->override_cause = override_cause;

        ds_pdn_hdlr_get_current_plmn(&rat_based_pdn_throt_p->plmn_id);

        /* Create or update ePLMN list to make sure all ePLMNs for this
           PLMN are throttled */
        if (block_eplmns)
        {
          ds_3gpp_throt_create_new_eplmn_entry(rat_based_pdn_throt_p->eplmn_list_ptr,
                                               rat_based_pdn_throt_p,
                                               &rat_based_pdn_throt_p->plmn_id, 
                                               TRUE, 
                                               DS_3GPP_PDN_THROT_NTW_O_RIDE_THROT_TBL);
        }

        ds_3gpp_pdn_throt_advertise_blocked_apns_current_plmn();

        if (rat_type == DS_3GPP_THROT_RAT_LTE)
        {
          ds_3gpp_pdn_cntxt_disable_lte_if_attach_profile_unavailable();
        }
      }
    }
    else
    {
      apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_index_ptr
                                      (apn,FALSE);

      if ((apn_index_ptr == NULL) || (!apn_index_ptr->valid))
      {
        break;
      }

      temp_rat_mask = rat_mask;

      while ( temp_rat_mask != DS_3GPP_THROT_RAT_MASK_NONE)
      {
        rat_type = ds_3gpp_pdn_throt_get_next_rat_type(&temp_rat_mask);

        rat_based_pdn_throt_p = 
                            apn_index_ptr->network_override_ip_info_p[rat_type];

        if (rat_based_pdn_throt_p == NULL)
        {
			       continue;
        }

        ds_3gpp_network_override_clear_throt(apn_index_ptr, 
                                             rat_based_pdn_throt_p,
                                             rat_type);        
      }

      ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed(apn_index_ptr, TRUE);       
    }
  }while(0); 

  return ret_val;
}/* ds_3gpp_network_override_hdlr_set_overridden_ip*/

/*===========================================================================
FUNCTION    DS_3GPP_NW_OVERRIDE_GET_THROT_PER_PLMN

DESCRIPTION
   Function used to determine if network override throt needs to be
   retrieved per PLMN (for rel 13 and above) or per rat across all
   RATs
 
PARAMETERS 
  rat_based_ntw_throt_p       - Pointer to the network override based throt SM
  plmn_id                     - pointer to the Current PLMN ID
 
DEPENDENCIES
  None.

RETURN VALUE 
  TRUE  - If per PLMN throttling needs to be retrieved
  FALSE - Otherwise
 
SIDE EFFECTS
  None.

===========================================================================*/
static boolean ds_3gpp_nw_override_get_throt_per_plmn
(
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s    *rat_based_ntw_throt_p,
  sys_plmn_id_s_type                   *plmn_id_p
)
{

  if ((NULL == rat_based_ntw_throt_p) || (NULL == plmn_id_p))
  {
    return FALSE;
  }

  /*--------------------------------------------------------------------- 
    For release 13, return override IP type per PLMN. Break and return
    DS_3GPP_PDN_THROT_PDP_UNDEFINED if
     
    1. Release 13 is enabled and 
    2. If the passed PLMN is different from the throt SM PLMN, and if 
       a. It's not present in the throttle SM ePLMN list and 
       b. Is not a part of the ePLMN list returned by NAS 
  ---------------------------------------------------------------------*/
  if ((ds_pdn_nv_manager_is_rel_13_or_above_supported()) &&
       (rat_based_ntw_throt_p->override_cause != 
        DS_3GPP_PDN_THROT_BEARER_ACT_ACCEPT) &&
        ((memcmp(plmn_id_p, &rat_based_ntw_throt_p->plmn_id,
                 sizeof(sys_plmn_id_s_type))!= 0) && 
        ((!ds_3gpp_check_ntw_throt_sm_if_plmn_present_in_eplmn_list(
               plmn_id_p, rat_based_ntw_throt_p)) && 
         (!ds_3gppi_throt_is_plmn_eplmn(
               *plmn_id_p, rat_based_ntw_throt_p->plmn_id))))
     )
  {
    return TRUE;
  }

  return FALSE;
}

/*=========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_GET_OVERRIDDEN_IP

DESCRIPTION
  This function returns the overridden IP on the specified rat

PARAMETERS
   byte                              *apn
   rat_type

DEPENDENCIES
  None.

RETURN VALUE
  Overriden IP Bmask - IPv4/IPv6/PDP_UNDEFINIED 

SIDE EFFECTS
  None

===========================================================================*/

uint8 ds_3gpp_network_override_hdlr_get_overridden_ip
(
   byte                              *apn,
   ds_3gpp_throt_rat_e_type           rat_type,
   sys_plmn_id_s_type                *plmn_id_p
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s     *apn_index_ptr = NULL;
  uint8                                        override_ip_bmask = 
                                                DS_3GPP_PDP_UNDEFINED_BMASK;
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s           *rat_based_ntw_throt_p = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  do
  {
    if(apn == NULL)
    {
      break;
    }

    if((rat_type != DS_3GPP_THROT_RAT_LTE) && 
       (rat_type != DS_3GPP_THROT_RAT_UMTS))
    {
      break;
    }

    apn_index_ptr = ds_3gpp_pdn_throt_sm_get_apn_index_ptr
                         (apn,FALSE);

    if ((apn_index_ptr == NULL) || (!apn_index_ptr->valid))
    {
      break;
    }

    rat_based_ntw_throt_p = apn_index_ptr->network_override_ip_info_p[rat_type];

    if (rat_based_ntw_throt_p == NULL)
    {
      break;
    }

    if (ds_3gpp_nw_override_get_throt_per_plmn(rat_based_ntw_throt_p, 
                                               plmn_id_p))
    {
      break;
    }

    override_ip_bmask = rat_based_ntw_throt_p->nw_overridden_ip_bmask;

  }while(0); 
   
  return override_ip_bmask;
}/*ds_3gpp_network_override_hdlr_get_overridden_ip*/

/*===========================================================================
FUNCTION       DS_3GPP_NTW_OVERRIDE_GET_EPLMN_LIST_PER_PLMN

DESCRIPTION
  API used to get the list of throttled ePLMNs on a particular RAT for a
  PLMN ID that is passed to this function. 

PARAMETERS  
  eplmn_list_ptr - Pointer to the ePLMN list 
  rat_type       - Enum that indicates the RAT on which ePLMN list info
                   is needed.
                   
DEPENDENCIES 
  None.
  
RETURN VALUE 
  Returns a pointer to the ePLMN list in a particular RAT
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
sys_plmn_list_s_type* ds_3gpp_ntw_override_get_eplmn_list_per_plmn
(
  sys_plmn_id_s_type        plmn_id,
  ds_3gpp_throt_rat_e_type  rat_type
)
{
  uint8                                     apn_index;                                            
  ds_3gpp_pdn_throttle_sm_apn_based_s      *apn_based_tbl_ptr = NULL;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s  *apn_index_ptr = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s        *ntw_override_tbl_per_rat = NULL;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl();

  if (apn_based_tbl_ptr == NULL)
  {
    return NULL;
  }

  for (apn_index = 0; apn_index < DS_MAX_APN_NUM; apn_index++)
  {
    apn_index_ptr = apn_based_tbl_ptr->apn_based_dyn_tbl[apn_index];    

    if (apn_index_ptr != NULL)
    {
      ntw_override_tbl_per_rat = 
            apn_index_ptr->network_override_ip_info_p[rat_type];

      if (ntw_override_tbl_per_rat != NULL)
      {
        /* Check if the PLMN passed is present in the 
           1. Parent PLMN network override table or
           2. ePLMN list */
        if (ds_3gpp_check_ntw_throt_sm_if_plmn_present_in_eplmn_list(
             &plmn_id, ntw_override_tbl_per_rat) || 
            ds_3gppi_throt_is_plmn_eplmn(
               plmn_id, ntw_override_tbl_per_rat->plmn_id))           
        {
          return (ntw_override_tbl_per_rat->eplmn_list_ptr);
        }
      } /* ntw_override_tbl_per_rat */
    } 
  }/* apn_index LOOP */

  return NULL;
} /* ds_3gpp_ntw_override_get_eplmn_list_per_plmn */

/*===========================================================================
FUNCTION DS_3GPP_NETWORK_OVERRIDE_HDLR_RESET_THROT_PER_RAT

DESCRIPTION
  This function is used to clear network override throttling information
  for a given rat.

PARAMETERS 
  rat_type  - RAT on which throttling needs to be cleared

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_network_override_hdlr_reset_throt_per_rat
(
  ds_3gpp_throt_rat_e_type         rat_type
)
{
  ds_3gpp_pdn_throttle_sm_apn_based_s     *apn_based_tbl_ptr = NULL;
  uint8                                    apn_loop_index = 0;  
  ds_local_err_val_e_type                  local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_3gpp_pdn_throttle_sm_apn_based_dyn_s *per_apn_throt_sm_ptr = NULL;
  ds_3gpp_pdn_throt_ntw_o_ride_ip_s       *per_rat_ntw_override_info_p = NULL;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  do
  {
    apn_based_tbl_ptr = ds_3gpp_pdn_throt_get_apn_based_tbl();

    if(apn_based_tbl_ptr == NULL)
    {
      local_err = DS_LOCAL_ERR_VAL_1;
      break;
    }

    if((rat_type <= DS_3GPP_THROT_RAT_MIN) ||
       (rat_type >= DS_3GPP_THROT_RAT_ILLEGAL))
    {
      local_err = DS_LOCAL_ERR_VAL_2;
      break;
    }

    for(apn_loop_index = 0; apn_loop_index < DS_MAX_APN_NUM; apn_loop_index++)
    {
      per_apn_throt_sm_ptr = 
                    apn_based_tbl_ptr->apn_based_dyn_tbl[apn_loop_index];

      if((per_apn_throt_sm_ptr == NULL) || (!per_apn_throt_sm_ptr->valid))
      {
        continue;
      }
      
      per_rat_ntw_override_info_p = 
          per_apn_throt_sm_ptr->network_override_ip_info_p[rat_type];

      if (per_rat_ntw_override_info_p == NULL)
      {
        continue;
      }
            
      ds_3gpp_network_override_clear_throt( per_apn_throt_sm_ptr, 
                                            per_rat_ntw_override_info_p,
                                            rat_type);
              
      ds_3gpp_pdn_throt_cleanup_apn_index_entry_if_needed(per_apn_throt_sm_ptr,
                                                          TRUE);
    }
  }while(0); 

  DS_MSG2(MSG_LEGACY_HIGH,
          "ds_3gpp_network_override_hdlr_reset_throt_per_rat "
          "rat_type: %d, err_val:%d", rat_type, local_err);
}/* ds_3gpp_network_override_hdlr_reset_throt_per_rat */