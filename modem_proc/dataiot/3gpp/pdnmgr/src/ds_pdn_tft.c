/*===========================================================================
                      DS_PDN_TFT.C
DESCRIPTION
This file manages flow context and TFT..

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_pdn_tft.c#2 $
  $DateTime: 2023/01/19 23:06:43 $$Author: pwbldsvc $

when           who    what, where, why
--------    ---    ----------------------------------------------------------
10/25/18    sd      First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_pdn_tft.h"
#include "ds_flow.h"
#include "ds_pdn_flow_manager.h"
#include "ds_bearer_flow_manager.h"

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_TFT_SET_FILTER_PRECED_BIT

DESCRIPTION
  Set the filter precedence bit in the given precedence bit mask.

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_set_filter_preced_bit
(
  uint16      fi_preced,
  uint32     *preced_bit_mask
)

{
  uint16 index;
  uint32 *mask = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fi_preced > DS_MAX_FI_PRECED)
  {
    return;
  }
  if(preced_bit_mask == NULL)
  {
    return;
  }

  if(fi_preced == 0)
  {
    index = 0;
  }
  else
  {
    index = fi_preced/32;
  }
  
  DS_MSG1(MSG_LEGACY_HIGH, "Set 0x%x for Precedence", fi_preced);
  mask = &preced_bit_mask[index];
  if(fi_preced == 0)
  {
    DS_SET_FI_BIT(*mask, 0);
  }
  else
  {
    DS_SET_FI_BIT((*mask), (fi_preced%32));
  }
  
  return;
} /* ds_pdn_tft_set_filter_preced_bit */

/*===========================================================================
FUNCTION DS_PDN_TFT_CLEAR_FILTER_PRECED_BIT

DESCRIPTION
  Clear the filter precedence bit in the given precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_clear_filter_preced_bit
(
  uint16      fi_preced,
  uint32     *preced_bit_mask
)
{
  uint16 index;
  uint32 *mask = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (fi_preced > DS_MAX_FI_PRECED)
  {
    return;
  }

  if(preced_bit_mask == NULL)
  {
    return;
  }

  if(fi_preced == 0)
  {
    index = 0;
  }
  else
  {
    index = fi_preced/32;
  }

  DS_MSG1(MSG_LEGACY_HIGH, "Cleared 0x%x as Precedence",fi_preced);
  mask = &preced_bit_mask[index];
  if(fi_preced == 0)
  {
    DS_CLEAR_FI_BIT((*mask), 0);
  }
  else
  {
    DS_CLEAR_FI_BIT((*mask),(uint16)(fi_preced%32));
  }
  
  return;
}/* ds_pdn_tft_clear_filter_preced_bit*/

/*===========================================================================
FUNCTION ds_pdn_tft_is_filter_preced_bit_set

DESCRIPTION
  Return whether the filter precedence is in use for a give precedence bit mask

PARAMETERS
  fi_preced         - Filter precedence.
  preced_bit_mask   - Precedence bit mask to be updated.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter precedence is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_is_filter_preced_bit_set
(
  uint16     fi_preced,
  uint32    *preced_bit_mask
)
{
  uint16 index;
  uint32 *mask = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fi_preced > DS_MAX_FI_PRECED)
  {
    return FALSE;
  }

  if(preced_bit_mask == NULL)
  {
    return FALSE;
  }
  
  if(fi_preced == 0)
  {
    index = 0;
  }
  else
  {
    index = fi_preced/32;
  }
  
  mask = &preced_bit_mask[index];
  if(fi_preced == 0)
  {
    return (DS_IS_FI_BIT_SET((*mask), 0)); 
  }
  else
  {
    return (DS_IS_FI_BIT_SET((*mask),(fi_preced%32)));  
  }
}/* ds_pdn_tft_is_filter_preced_bit_set */


/*===========================================================================
FUNCTION DS_PDN_TFT_IS_FILTER_IDENTIFIER_REQUESTED

DESCRIPTION
  Return whether this filter identifier is already in use.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - the bit for this filter identifier is set
  FALSE - otherwise
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_is_filter_identifier_requested
(
  uint16     fi_identifier,
  uint32    *fi_bit_mask
)
{
  uint32 mask = 0;

  if (fi_identifier > DS_MAX_FI_ID)
  {
    DS_ERROR_LOG_1("Invalid filter identifier: =d received, ignored.",
                   fi_identifier);
    return FALSE;
  }
  
  mask = 1 << fi_identifier;  
  if (((*fi_bit_mask) & mask) == mask)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* ds_pdn_tft_is_filter_identifier_requested */

/*===========================================================================
FUNCTION DS_PDN_TFT_SET_FILTER_IDENTIFIER_BIT

DESCRIPTION
  Set the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_set_filter_identifier_bit
(
  uint16     fi_identifier,
  uint32    *fi_bit_mask
)

{
  uint32    mask = 0;
  mask = 1 << fi_identifier;
  *fi_bit_mask |= mask;
  return;
} /* ds_pdn_tft_set_filter_identifier_bit */

/*===========================================================================
FUNCTION DS_PDN_CLEAR_FILTER_IDENTIFIER_BIT

DESCRIPTION
  unset the bit for this filter identifier.

PARAMETERS
  fi_identifier       - Filter identifier.
  fi_bit_mask         - Filter bitmask to check filter identifier 

DEPENDENCIES
  None.

RETURN VALUE
 None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_clear_filter_identifier_bit
(
  uint16 fi_identifier,
  uint32    *fi_bit_mask
)
{
  uint32    mask = 0;
  mask = 1 << fi_identifier;
  *fi_bit_mask &= (~mask);
  return;
} /* ds_pdn_tft_clear_filter_identifier_bit */


/*===========================================================================
FUNCTION DS_PDN_TFT_CONFIGURE_NW_BK_FLOW_SPEC

DESCRIPTION
  Configure the flow spec and filters in the Network book keeping 
  data strutcure.

PARAMETERS
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_configure_nw_bk_flow_spec
(
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk
)
{
  if(nw_bk ==NULL)
  {
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Clear the memory.
  -------------------------------------------------------------------------*/
  memset(&(nw_bk->curr_qos),0, sizeof(qos_spec_type));                      
  memset(nw_bk->curr_rx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);   
  memset(nw_bk->curr_tx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  memset(&(nw_bk->mod_qos),0, sizeof(qos_spec_type));                      
  memset(nw_bk->mod_rx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);   
  memset(nw_bk->mod_tx_fltr,0,sizeof(ip_filter_type)*MAX_PACKET_FILTERS);

  /*------------------------------------------------------------------------
     Assign Filter Space
  -------------------------------------------------------------------------*/
  nw_bk->curr_qos.rx.fltr_template.list_ptr = nw_bk->curr_rx_fltr;
  nw_bk->curr_qos.tx.fltr_template.list_ptr = nw_bk->curr_tx_fltr;

  nw_bk->mod_qos.rx.fltr_template.list_ptr = nw_bk->mod_rx_fltr;
  nw_bk->mod_qos.tx.fltr_template.list_ptr = nw_bk->mod_tx_fltr;

  return TRUE;
}/* ds_pdn_tft_configure_nw_bk_flow_spec */

/*===========================================================================
FUNCTION DS_PDN_TFT_CHECK_MOD_FLTR_SEMANTIC_ERROR

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_check_mod_fltr_semantic_error
(
  tft_type_T                            *nw_tft  
)
{  
  int filter_idx_outer = 0, filter_idx_inner = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_tft == NULL)
  {
    return TRUE;
  }

  if(nw_tft->e_bit != DS_FLOW_E_BIT_DISABLE)
  {
    DS_ERROR_LOG_0("E Bit set");
    return TRUE;
  }

  if(nw_tft->num_filters <= 0)
  {
    DS_ERROR_LOG_0("Input filters is zero");
    return TRUE;
  }
  
  /* Fail if there are two or more filters with identical packet filter identifiers */
  for ( filter_idx_outer = 0; filter_idx_outer < nw_tft->num_filters; filter_idx_outer++ )
  {
    for ( filter_idx_inner = filter_idx_outer + 1; 
          filter_idx_inner < nw_tft->num_filters; 
          filter_idx_inner++ )
    {
      if( (nw_tft->filter_list[filter_idx_outer].identifier & 0x0F) == 
                     (nw_tft->filter_list[filter_idx_inner].identifier & 0x0F ))
      {
        DS_ERROR_LOG_1("Input filters have two TFT with identical identifier =d",
                       nw_tft->filter_list[filter_idx_outer].identifier);
        return TRUE;
      }
    }
  }
  
  return FALSE;
}/* ds_pdn_tft_check_mod_fltr_semantic_error */

/*===========================================================================
FUNCTION DS_PDN_TFT_CHECK_FOR_REPEATED_PRECEDENCE

DESCRIPTION
  Verify if the input TFT has any semantic errors.

PARAMETERS
  nw_tft                -  TFT information  

DEPENDENCIES
  None.

RETURN VALUE
  boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_check_for_repeated_precedence
(
  tft_type_T                            *nw_tft  
)
{  
  int filter_idx_outer = 0, filter_idx_inner = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_tft == NULL)
  {
    return TRUE;
  }

  if(nw_tft->e_bit != DS_FLOW_E_BIT_DISABLE)
  {
    DS_ERROR_LOG_0("E Bit set");
    return TRUE;
  }

  if(nw_tft->num_filters <= 0)
  {
    DS_ERROR_LOG_0("Input filters is zero");
    return TRUE;
  }
  
  /* Fail if there are two or more filters with identical packet filter eval precedence */
  for ( filter_idx_outer = 0; 
        filter_idx_outer < nw_tft->num_filters; 
        filter_idx_outer++ )
  {
    for ( filter_idx_inner = filter_idx_outer + 1; 
          filter_idx_inner < nw_tft->num_filters; 
          filter_idx_inner++ )
    {
      if( (nw_tft->filter_list[filter_idx_outer].eval_precedence) == 
                     (nw_tft->filter_list[filter_idx_inner].eval_precedence))
      {
        DS_ERROR_LOG_1("Input filters have two TFT with identical precedences =d",
                       nw_tft->filter_list[filter_idx_outer].eval_precedence);
        return TRUE;
      }
    }
  }
        
  return FALSE;
}/* ds_pdn_tft_check_mod_fltr_semantic_error */

/*===========================================================================
FUNCTION DS_PDN_TFT_CLEAR_PREC_CONFLICT_IF_INTERCHANGED_WITHIN_TFT

DESCRIPTION
   Clear the precedence conflict if the old filter (in any direction )corresponce to give
   precedence also getting changed 
eg:(<filter id>, <precedence>)
1st create: (1,1),(2,2)
now replace: (1,2)(2,1)

PARAMETERS
  preced                      -  precedence for which it has conflict
  filter_id                   -  filter id
  nw_tft_p                    -  TFT information
  nw_bk                       -  Boook keeping info.  
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
void ds_pdn_tft_clear_prec_conflict_if_interchanged_within_tft
(
  uint16                          preced, 
  uint16                          filter_id, 
  tft_type_T                     *nw_tft,
  ds_pdn_tft_nw_modify_qos_bk_s  *nw_bk

)
{
  int                      rep_cnt=0,cnt_temp = 0;
  packet_filter_type_T	   *nw_filter_temp = NULL;
  uint16                   rep_filter_id=0, filter_id_temp =0;
  uint16                   rep_preced =0;
  ip_filter_type           *fltr = NULL;
  /*---------------------------------------------------------------------------
   Clear the precedence conflict if the old filter corresponce to this precedence 
   also getting changed
  ----------------------------------------------------------------------*/
  for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.tx.fltr_template.num_filters; rep_cnt++)
  {
    fltr = &(nw_bk->curr_qos.tx.fltr_template.list_ptr[rep_cnt]);		 
    /*---------------------------------------------------------------
    extract the filter id  and precedence.
    ---------------------------------------------------------------*/
    rep_preced = fltr->ipfltr_aux_info.fi_precedence;
    rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
    
    if(preced == rep_preced && nw_tft->operation_code == 
                                                     DS_PDN_TFT_OPCODE_REPLACE)
    {
      //check if rep_filter_id is present in new filters
      for(cnt_temp=0; cnt_temp< nw_tft->num_filters; cnt_temp++)
      {
        nw_filter_temp = &(nw_tft->filter_list[cnt_temp]);
        filter_id_temp = nw_filter_temp->identifier & 0xF;
        if((filter_id_temp == rep_filter_id) &&
			        (ds_pdn_tft_is_filter_preced_bit_set(preced, nw_bk->preced_conflict)))
        {
          DS_MSG3(MSG_LEGACY_HIGH, "changing precedence of both the filters "
                  "%d, %d dont set precedence conflict: %d", 
                  filter_id, rep_filter_id, nw_bk->preced_conflict_cnt);
          ds_pdn_tft_clear_filter_preced_bit(preced, 
                                       						nw_bk->preced_conflict);
          nw_bk->preced_conflict_cnt--;
        }
      }    
    }
  } /* for tx */ 
  
  for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.rx.fltr_template.num_filters;rep_cnt++)
  {
    fltr = &(nw_bk->curr_qos.rx.fltr_template.list_ptr[rep_cnt]);		 
    /*-----------------------------------------------------------------
      extract the filter id  and precedence.
    -----------------------------------------------------------------*/
    rep_preced = fltr->ipfltr_aux_info.fi_precedence;
    rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
  
    if(preced == rep_preced && nw_tft->operation_code == 
                                                      DS_PDN_TFT_OPCODE_REPLACE)
    {    
      //check if rep_filter_id is present in new filters
      for(cnt_temp=0; cnt_temp< nw_tft->num_filters; cnt_temp++)
      {
        nw_filter_temp = &(nw_tft->filter_list[cnt_temp]);
        filter_id_temp = nw_filter_temp->identifier & 0xF;		
        if((filter_id_temp == rep_filter_id)&&
           (ds_pdn_tft_is_filter_preced_bit_set(preced, nw_bk->preced_conflict)))
        {
          DS_MSG3(MSG_LEGACY_HIGH, "changing precedence of both the filters "
                  "%d, %d dont set precedence conflict: %d", 
                  filter_id, rep_filter_id, nw_bk->preced_conflict_cnt);
          ds_pdn_tft_clear_filter_preced_bit(preced, 
          		                                 nw_bk->preced_conflict);
          nw_bk->preced_conflict_cnt--;
        }
      }			  
    }
  }/*for rx*/
}/*ds_pdn_tft_clear_prec_conflict_if_interchanged_within_tft*/

/*===========================================================================
FUNCTION DS_PDN_TFT_CHECK_NW_FLTR_ID_PRECED

DESCRIPTION
  Verify the filter ID and precedence for each filter passed by the network.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information
  bearer_context_p            -  Bearer information
  flow_context_p              -  Flow context
  nw_tft_p                    -  TFT information
  nw_bk                       -  Boook keeping info.
  del_filter                  -  Indicate delete filter operation
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_check_nw_fltr_id_preced
(
  ds_pdn_mgr_s                       *pdn_context_p,
  ds_bearer_mgr_s                    *bearer_context_p,
  ds_flow_type                       *flow_context_p,
  tft_type_T                         *nw_tft,
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk,
  boolean                             del_filter
)
{
  int                       cnt = 0, rep_cnt=0;
  uint32                    temp_filter_id = 0;//filter-id repeats
  packet_filter_type_T      *nw_filter=NULL;
  uint16                    filter_id = 0, rep_filter_id=0;
  ds_bearer_flow_manager_s  *bflow_manager_p = NULL;
  ds_pdn_flow_manager_s     *flow_manager_p = NULL;
  uint16                    preced = 0, rep_preced =0;
  uint32                    temp_preced[DS_FI_PRECED_ARR_SIZE];//precd repeat
  ip_filter_type            *fltr = NULL;
  boolean                   rep_fltr_found = FALSE, replacing_same_filter = FALSE;
  byte                      direction = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
     Retrieve the bearer flow manager and flow manager.
  -----------------------------------------------------------------------*/
  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    return FALSE;
  }

  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if(flow_manager_p == NULL)
  {
    return FALSE;
  }

  if( (nw_bk->is_add == FALSE) &&(!del_filter))
  {
    /*---------------------------------------------------------------------
      For replace retrieve current filter spec. 
    ---------------------------------------------------------------------*/
    /*--------------------------------------------------------------------
      Configure the qos flow spec.
    -----------------------------------------------------------------------*/
    if(ds_pdn_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
    {
      return FALSE;
    }

    memscpy(&nw_bk->curr_qos, sizeof(qos_spec_type),
            flow_context_p->qos_info_ptr, sizeof(qos_spec_type));
  }/* replace only */

  /*-----------------------------------------------------------------------
    For each filter verify if the filter-id is present in the bearer 
    identifier and also if the filter-id is present in the flow context.
  -----------------------------------------------------------------------*/
  memset(temp_preced, 0, (sizeof(uint32) *DS_FI_PRECED_ARR_SIZE));
  for(cnt=0; cnt< nw_tft->num_filters; cnt++)
  {
    nw_filter = &(nw_tft->filter_list[cnt]);
    filter_id = nw_filter->identifier & 0xF;
    preced    = nw_filter->eval_precedence;
    direction = ((nw_filter->identifier & 0x30)>>4);
    if(!del_filter)    
    {
      /*---------------------------------------------------------------------
        Verify precedence is repeated 
      ---------------------------------------------------------------------*/
      if(ds_pdn_tft_is_filter_preced_bit_set(preced,temp_preced) == FALSE) 
      {
        ds_pdn_tft_set_filter_preced_bit(preced, temp_preced);
      }
      else
      {
        return FALSE;
      }
      
      /*---------------------------------------------------------------------
        Check for precedence conflict
      ---------------------------------------------------------------------*/
      if(ds_pdn_tft_is_filter_preced_bit_set(preced,
                                             flow_manager_p->fi_preced) == TRUE) 
      {
        if((nw_bk->is_add == TRUE) || 
           (nw_tft->operation_code == DS_PDN_TFT_OPCODE_CREATE))
        {
          DS_MSG1(MSG_LEGACY_HIGH, "Precedence conflict for 0x%x", preced);
          ds_pdn_tft_set_filter_preced_bit(preced, nw_bk->preced_conflict);
          nw_bk->preced_conflict_cnt++;
        }/* add */
        else
        {
          rep_fltr_found = FALSE;
          replacing_same_filter = FALSE;

        /*check existing uplink filters*/
          for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.tx.fltr_template.num_filters;
              rep_cnt++)
          {
            fltr = &(nw_bk->curr_qos.tx.fltr_template.list_ptr[rep_cnt]);        
        /*---------------------------------------------------------------
              extract the filter id  and precedence.
            ---------------------------------------------------------------*/
            rep_preced = fltr->ipfltr_aux_info.fi_precedence;
            rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
            if(filter_id == rep_filter_id)
            {
              if(preced != rep_preced)
              {
         /*--------------------------------------------------------
                 Update only if the ID matches but preced is different. 
                ----------------------------------------------------------*/
                DS_MSG3(MSG_LEGACY_HIGH,
                        "Precedence conflict on TX for prec 0x%x, "
                        "prec cnt: %d filter_id: %d",
                        preced, nw_bk->preced_conflict_cnt, filter_id);
                ds_pdn_tft_set_filter_preced_bit(preced,
                                                 nw_bk->preced_conflict);
                nw_bk->preced_conflict_cnt++;
                rep_fltr_found = TRUE;
              }
              else
              {
         /*-----------------------------------------------------------------
                Replacing a filter with same fid, precedence and direction
                don't set precedence conflict
               -----------------------------------------------------------------*/                 
               DS_MSG0(MSG_LEGACY_HIGH,
                       "Replacing a filter with same fid, "
                       "precedence and tx direction");
                replacing_same_filter = TRUE;
              }
            }/* filter_id == rep_filter_id*/
         }
          /*In uplink filter check existing downlink filters*/
          for(rep_cnt=0;rep_cnt<nw_bk->curr_qos.rx.fltr_template.num_filters;
              rep_cnt++)
          {
            fltr = &(nw_bk->curr_qos.rx.fltr_template.list_ptr[rep_cnt]);        
          /*---------------------------------------------------------------
              extract the filter id  and precedence.
          --------------------------------------------------------------*/
            rep_preced = fltr->ipfltr_aux_info.fi_precedence;
            rep_filter_id = fltr->ipfltr_aux_info.fi_id & (0xF);
            if(filter_id == rep_filter_id)
            {
               if(preced != rep_preced)
               {
               /*only if repeated filter not found*/
                 if (!rep_fltr_found)
                 {
                 /*--------------------------------------------------------
                  Update only if the ID matches but preced is different. 
                 ----------------------------------------------------------*/
                   DS_MSG3(MSG_LEGACY_HIGH,
                           "Precedence conflict on RX for prec 0x%x, "
                           "prec cnt: %d filter_id:%d",
                           preced, nw_bk->preced_conflict_cnt, filter_id);
                   ds_pdn_tft_set_filter_preced_bit(preced,
                                                    nw_bk->preced_conflict);
                   nw_bk->preced_conflict_cnt++;
                   rep_fltr_found = TRUE;
                 }
              }
              else
              {
        /*-----------------------------------------------------------------
              Replacing a filter with same fid, precedence and different direction
              don't set precedence conflict
                -----------------------------------------------------------------*/
                DS_MSG0(MSG_LEGACY_HIGH,
                        "Replacing a filter with same fid, "
                        "precedence with RX direction");
                replacing_same_filter = TRUE;
              }
            }/* filter_id == rep_filter_id*/
          }
				  
          if((rep_fltr_found == TRUE)&& 
             (nw_tft->operation_code == DS_PDN_TFT_OPCODE_REPLACE))
          {
            ds_pdn_tft_clear_prec_conflict_if_interchanged_within_tft(preced, 
                                                  filter_id, nw_tft, nw_bk );
          }  
          
	         if((rep_fltr_found == FALSE) &&
             (replacing_same_filter == FALSE) &&
             (ds_pdn_tft_is_filter_preced_bit_set(preced, 
                                              nw_bk->preced_conflict)== FALSE ))
          {
       /*-----------------------------------------------------------------
            precedence conflict on other bearers
            -----------------------------------------------------------------*/		 
            DS_MSG3(MSG_LEGACY_HIGH,
                    "Precedence conflict on other bearers for	0x%x prec cnt:%d,"
                    "filter_id:%d ", 
                    preced, nw_bk->preced_conflict_cnt, filter_id);
            ds_pdn_tft_set_filter_preced_bit(preced,
                                             nw_bk->preced_conflict);
            nw_bk->preced_conflict_cnt++;
          }/* rep_fltr_found == TRUE */
        }/* replace */
      }/* if preced conflict */
    } /* del_filter == FALSE*/
    
    /*---------------------------------------------------------------------
      Verify if the filter ID is repeated in the request.
    ---------------------------------------------------------------------*/
    if(ds_pdn_tft_is_filter_identifier_requested(
                                           filter_id, &temp_filter_id) == FALSE)
    {
      ds_pdn_tft_set_filter_identifier_bit(filter_id,&temp_filter_id);
    }
    else
    {
      DS_ERROR_LOG_0("Invalid NW input.FI-ID repeated");
      return FALSE;
    }
    
    /*---------------------------------------------------------------------
      Track filter id conflict per bearer and flow.
      Increment bearer only if conflict is not on the flow.
    ---------------------------------------------------------------------*/
    if(ds_pdn_tft_is_filter_identifier_requested(
                          filter_id,&(bflow_manager_p->fi_identifier)) == TRUE)
    {
      if(ds_pdn_tft_is_filter_identifier_requested(
                          filter_id, &(flow_context_p->fi_identifier)) == TRUE)
      {
        ds_pdn_tft_set_filter_identifier_bit(filter_id,
                                             &(nw_bk->flow_cntx_fltr_id));
      }
      else
      {
        ds_pdn_tft_set_filter_identifier_bit(filter_id,
                                             &(nw_bk->bearer_fltr_id));
      }
    }/* filter id part of the bearer */
    else
    {
      DS_MSG1(MSG_LEGACY_HIGH, 
              "Filter id: %d is not present in existing flow",
              filter_id);
      ds_pdn_tft_set_filter_identifier_bit(filter_id,
                                           &(nw_bk->non_match_fltr_id));
    }  
  }/* for */
  
  return TRUE;
} /* ds_pdn_tft_check_nw_fltr_id_preced */

/*===========================================================================
FUNCTION DS_PDN_TFT_FIND_BEARER_WITH_FLTR_PRECED_CONFLICT

DESCRIPTION
  Find the bearers on which the precedence conflict occurs.
  This checks if the filter ID or precedence is repeated.
  Filter ID conflict is detected. Precedence conflict is detected.
  Returns the filters with conflict, without conflict.
  Returns the precedence values with conflict.
  This function is called for NW initiated Qos Modification check.

PARAMETERS
  pdn_context_p               -  PDN information.
  bearer_context_p            -  Bearer information.
  nw_bk                       -  Boook keeping info.
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_find_bearer_with_preced_conflict
(
  ds_pdn_mgr_s                       *pdn_context_p,
  ds_bearer_mgr_s                    *bearer_context_p,
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk
)
{
  ds_bearer_flow_manager_s *bflow_manager_p = NULL;
  ds_pdn_flow_manager_s    *flow_manager_p = NULL;
  ds_flow_type             *flow_cntx = NULL;
  uint16                    preced = 0;
  uint32                    temp_preced[DS_FI_PRECED_ARR_SIZE];
  int                       cnt = 0;
  ip_filter_type           *fltr = NULL;
  int                       tpreced_conflict_cnt = 0;
  int                       loop = 0;
  ds_local_err_val_e_type   local_err = DS_LOCAL_ERR_VAL_NONE;
  ds_bearer_flow_manager_list_type  *bflow_list_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_pdn_mgr_validate_pdn_ptr(pdn_context_p) ||
     !ds_bearer_mgr_validate_bearer_ptr(bearer_context_p) ||
     !nw_bk)
  {
    local_err = DS_LOCAL_ERR_VAL_0;
    goto func_exit;
  }

  bflow_manager_p = ds_bearer_flow_manager_find_entry(bearer_context_p);
  if(bflow_manager_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_1;
    goto func_exit;
  }
  
  /*-----------------------------------------------------------------------
    Retrieve the flow manager.
  -----------------------------------------------------------------------*/
  flow_manager_p = ds_pdn_flow_mgr_get_entry(pdn_context_p->index);
  if (flow_manager_p == NULL)
  {
    local_err = DS_LOCAL_ERR_VAL_2;
    goto func_exit;
  }

  /*-----------------------------------------------------------------------
    Copy the precedence conflict count and precedence
  -----------------------------------------------------------------------*/
  tpreced_conflict_cnt = nw_bk->preced_conflict_cnt;
  memscpy(temp_preced,(sizeof(uint32)*DS_FI_PRECED_ARR_SIZE),
  	       (nw_bk->preced_conflict), (sizeof(uint32)*DS_FI_PRECED_ARR_SIZE));

  /*---------------------------------------------------------------------
    Loop thru all the bearer flow managers on PDN. 
    Also need to search the pending flow contexts!
  -----------------------------------------------------------------------*/
  bflow_manager_p = list_peek_front(&(flow_manager_p->bearer_flow_mgr));
  while(bflow_manager_p != NULL)
  {
    /*-------------------------------------------------------------------
       Loop thru all the flow contexts on the bearer.
    --------------------------------------------------------------------*/
    bflow_list_p = list_peek_front(&(bflow_manager_p->flow_context));
    while(bflow_list_p != NULL && bflow_list_p->flow_context_p != NULL)
    {  
      flow_cntx = bflow_list_p->flow_context_p;
      /*-----------------------------------------------------------------------
               Configure the qos flow spec.
           -----------------------------------------------------------------------*/
      if(ds_pdn_tft_configure_nw_bk_flow_spec(nw_bk) == FALSE)
      {
        local_err = DS_LOCAL_ERR_VAL_3;
        goto func_exit;
      }

      /*-----------------------------------------------------------------
               Retrieve the Filters
          -----------------------------------------------------------------*/
      memscpy(&nw_bk->curr_qos, sizeof(qos_spec_type),
              flow_cntx->qos_info_ptr, sizeof(qos_spec_type));
      
      /*----------------------------------------------------------------
               Check every filter precedence.
           ----------------------------------------------------------------*/
      for(cnt =0; cnt<nw_bk->curr_qos.rx.fltr_template.num_filters; cnt++)
      {
        fltr = &(nw_bk->curr_qos.rx.fltr_template.list_ptr[cnt]);        
        preced = fltr->ipfltr_aux_info.fi_precedence;
        if(ds_pdn_tft_is_filter_preced_bit_set(preced,temp_preced) == TRUE)
        {
          /*------------------------------------------------------------
                     Bearer with precedence conflict found
                   ------------------------------------------------------------*/
          DS_MSG2(MSG_LEGACY_HIGH,
                  "Conflict preced: 0x%x on bearer %d",
                  preced, bflow_manager_p->bearer_ptr->index);

          /*------------------------------------------------------------
                     Check if the bearer is already in conflict bearer list. In that
                     case do not add again.
                 -------------------------------------------------------------*/      
          loop = nw_bk->conflict_bearer_cnt;
          while(loop > 0)
          {
            if(nw_bk->conflict_bearer[loop-1] == 
                                (void *)(bflow_manager_p->bearer_ptr))
            {
              break;
            }
            loop--;
          }
          
          if(loop == 0)
          {
          /*------------------------------------------------------------
                      Add the bearer in conflict bearer list. 
                  ------------------------------------------------------------*/ 
            nw_bk->conflict_bearer[nw_bk->conflict_bearer_cnt] 
                                       = (void *)bflow_manager_p->bearer_ptr;
            nw_bk->conflict_bearer_cnt++;
          }
          else
          {
            DS_MSG1(MSG_LEGACY_HIGH,
                    "Bearer %d already on conflict list",
                    bflow_manager_p->bearer_ptr->index);
          }
          
          tpreced_conflict_cnt--;
          ds_pdn_tft_clear_filter_preced_bit(preced, temp_preced);
          if(tpreced_conflict_cnt == 0)
          {
            return TRUE;
          }
        }/* conflict preced found in bearer */
      }/* for RX*/ 

      for(cnt =0;cnt<nw_bk->curr_qos.tx.fltr_template.num_filters; cnt++)
      {
        fltr = &(nw_bk->curr_qos.tx.fltr_template.list_ptr[cnt]);        
        /*--------------------------------------------------------------
                  NW Assigned filter id is in the last 4 bits.
              ---------------------------------------------------------------*/
        preced = fltr->ipfltr_aux_info.fi_precedence;
        if(ds_pdn_tft_is_filter_preced_bit_set(preced,temp_preced) == TRUE)
        {
          DS_MSG2(MSG_LEGACY_HIGH,
                  "Conflict preced: 0x%x on bearer %d",
                  preced, bflow_manager_p->bearer_ptr->index);

          loop = nw_bk->conflict_bearer_cnt;
          while(loop > 0)
          {
            if(nw_bk->conflict_bearer[loop-1] == 
                                     (void *)(bflow_manager_p->bearer_ptr))
            {
              break;
            }
            loop--;
          }
          
          if(loop == 0)
          {
            if(nw_bk->conflict_bearer_cnt >= 7)
            {
              local_err = DS_LOCAL_ERR_VAL_4;
              goto func_exit;
            }
            
            nw_bk->conflict_bearer[nw_bk->conflict_bearer_cnt] 
                                         = (void *)bflow_manager_p->bearer_ptr;
            nw_bk->conflict_bearer_cnt++;
          }
          else
          {
            DS_MSG1(MSG_LEGACY_HIGH,
                    "Bearer %d already on conflict list",
                    bflow_manager_p->bearer_ptr->index);
          }
          
          tpreced_conflict_cnt--;
          ds_pdn_tft_clear_filter_preced_bit(preced, temp_preced);
          if(tpreced_conflict_cnt == 0)
          {
            return TRUE;
          }
        }
      }/* for TX*/ 
      bflow_list_p = list_peek_next(&(bflow_manager_p->flow_context),
                                    &(bflow_list_p->bflow_link));
    }/* flow_cntx != NULL */
    bflow_manager_p = list_peek_next(&(flow_manager_p->bearer_flow_mgr),
                                     &(bflow_manager_p->link));
  }/* bflow_manager_p != NULL*/

  DS_ERROR_LOG_1("Did not find all the bearers for preced cnt:=d",
                 nw_bk->preced_conflict_cnt);

func_exit:
  if(local_err != DS_LOCAL_ERR_VAL_NONE)
  {
    DS_ERROR_LOG_1("ds_pdn_tft_find_bearer_with_preced_conflict "
                   "failed with err =d", local_err);
  }
  
  return FALSE;
}/* ds_pdn_tft_find_bearer_with_preced_conflict */

/*===========================================================================
FUNCTION DS_PDN_TFT_PRECED_CONFLICT_ON_BEARER

DESCRIPTION
  Verifies if the input bearer is in the conflict bearer array.

PARAMETERS
  nw_bk                       -  Boook keeping info.
  input                       -  bearer_ptr
                      
DEPENDENCIES
  None.

RETURN VALUE
boolean - return TRUE or FALSE
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_pdn_tft_preced_conflict_on_bearer
(
  ds_pdn_tft_nw_modify_qos_bk_s      *nw_bk,
  void                               *input
)
{
  uint32 cnt = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(nw_bk ==NULL || input ==NULL)
  {
    return FALSE;
  }
  
  for(cnt=0; cnt<nw_bk->conflict_bearer_cnt; cnt++)
  {
    if(nw_bk->conflict_bearer[cnt] == input)
    {
      DS_MSG1(MSG_LEGACY_HIGH, "Conflict array index: %d has input", cnt);
      return TRUE;
    }
  }
  
  return FALSE;
}/* ds_pdn_tft_preced_conflict_on_bearer */

