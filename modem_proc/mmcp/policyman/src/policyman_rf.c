/**
  @file policyman_rf.c

  @brief  RAT and RF band information
*/

/*
    Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
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

  $Header: //components/rel/mmcp.mpss/6.1.10/policyman/src/policyman_rf.c#1 $
  $DateTime: 2023/04/25 14:39:18 $
  $Author: pwbldsvc $
*/

#include "policyman_cfgitem.h"
#include "policyman_device_config.h"
#include "policyman_diag.h"
#include "policyman_plmn.h"
#include "policyman_rat_capability.h"
#include "policyman_rf.h"
#include "policyman_subs.h"
#include "policyman_tech.h"
#include "policyman_util.h"
#include "policyman_xml.h"

#include "trm.h"
#include "rfm.h"
#include "rcevt.h"
#include "mcs_wrappers.h"  // MCS wrappers for 'critical' rcevt_* functions



#define HAS_CAPABILITY(mode)  \
  (mcs_modem_has_capability(MCS_MODEM_CAPABILITY_FEATURE_##mode) != MCS_MODEM_CAP_UNAVAILABLE)


struct policyman_rf_info_t
{
  policyman_rf_device_info_t  overall;    /* this is the composite of all devices */
  size_t                      nDevices;
};

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
struct rf_bands_item_t
{
  POLICYMAN_ITEM;

  sys_band_mask_type  gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type  tdsBands;
};


struct rf_bands_t
{
  POLICYMAN_NAMED_OBJECT_BASE;

  sys_band_mask_type  gwBands;
  sys_lte_band_mask_e_type  lteBands;
  sys_band_mask_type  tdsBands;
};


typedef struct
{
  POLICYMAN_ACTION_BASE;

  char const       *pListName;   /* Used to indicate which named band list to use */
  rf_bands_t       *pBands;
} rf_bands_action_t;
#else
static policyman_rf_info_t device_rfinfo;
#endif



/*-------- policyman_rf_bands_item_new --------*/
policyman_item_t * policyman_rf_bands_item_new(
  sys_band_mask_type  gwBands,
  sys_lte_band_mask_e_type  lteBands,
  sys_band_mask_type  tdsBands
)
{
  rf_bands_item_t *prfb;
  policyman_rf_device_info_t *pOverall;

  prfb = (rf_bands_item_t *) policyman_cfgitem_new_item( POLICYMAN_ITEM_RF_BANDS,
                                                         sizeof(rf_bands_item_t),
                                                         policyman_simple_dtor );

  // mask with HW RF bands
  pOverall = policyman_rf_get_overall_info();

  prfb->gwBands  = gwBands & pOverall->gwBands;
  prfb->lteBands = SYS_LTE_BAND_MASK_COMMON(&lteBands, &pOverall->lteBands);
  prfb->tdsBands = tdsBands & pOverall->tdsBands;

  return (policyman_item_t *) prfb;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*=============================================================================
  Actions related to RF bands
=============================================================================*/

/*-----------------------------------------------------------------------------
  Helper functions for RF bands.
-----------------------------------------------------------------------------*/


/*-------- policyman_rf_bands_dtor --------*/
static void
policyman_rf_bands_dtor(
  void  *pObj
  )
{
  rf_bands_t  *pBands = (rf_bands_t *) pObj;

  POLICYMAN_MEM_FREE_IF(pBands->pName);
  policyman_mem_free(pBands);
}


/*-------- policyman_rf_band_action_dtor --------*/
static void
policyman_rf_band_action_dtor(
  void  *pObj
  )
{
  rf_bands_action_t  *pAction = (rf_bands_action_t *) pObj;

  POLICYMAN_MEM_FREE_IF(pAction->pListName);
  REF_CNT_OBJ_RELEASE_IF(pAction->pBands);

  policyman_action_dtor(pAction);
}


/*-------- policyman_rf_bands_parse --------*/
static boolean
policyman_rf_bands_parse(
  char const  *pStr,
  sys_band_mask_type  *pMask
  )
{
  char    token[32];
  int     bandNum;
  sys_band_mask_type  mask = 0;

  while (policyman_get_token(&pStr, token, sizeof(token)))
  {
    bandNum = atoi(token);

    if (bandNum < 0 || bandNum > 63)
    {
      POLICYMAN_UT_MSG("Invalid band in RF bands specification: %s", token, 0, 0);
      return FALSE;
    }

    mask |= (1LL << bandNum);
  }

  *pMask = mask;

  return TRUE;
}

/*-------- policyman_rf_lte_bands_parse --------*/
static boolean policyman_rf_lte_bands_parse(
  char const               *pStr,
  sys_lte_band_mask_e_type *pLteBandMask
)
{
  char                      token[32];
  int                       bandNum;
  sys_lte_band_mask_e_type  tempLteBandMask;

  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&tempLteBandMask);

  while (policyman_get_token(&pStr, token, sizeof(token)))
  {
    bandNum = atoi(token);

    if ( (bandNum < 0) || (bandNum > 255) )
    {
      POLICYMAN_MSG_HIGH_1("Invalid band in LTE RF bands specification: %d", bandNum);
      POLICYMAN_UT_MSG("Invalid band in LTE RF bands specification: %s", token, 0, 0);
      return FALSE;
    }

    SYS_LTE_BAND_MASK_ADD_BAND(&tempLteBandMask, bandNum);
  }

  *pLteBandMask = tempLteBandMask;

  return TRUE;
}

/*-------- policyman_rf_band_mask_read --------*/
static boolean
policyman_rf_band_mask_read(
  policyman_xml_element_t const *pElem,
  void                          *pInputMask,
  boolean                        is_complex_mask
  )
{
  boolean                       succeeded = FALSE;
  policyman_base_t              base;
  size_t                        nChildren;
  policyman_xml_element_t const *pChild;
  size_t                        i;
  char const                    *pStr;
  sys_band_mask_type            mask;
  sys_lte_band_mask_e_type       maskLte;
  sys_band_mask_type            *pMask     = (sys_band_mask_type *)pInputMask;
  sys_lte_band_mask_e_type      *pLteMask  = (sys_lte_band_mask_e_type *)pInputMask;

  /*  Get the base from which to work.  We don't support "config".
   */
  succeeded = policyman_util_get_base(pElem, &base);

  if (!succeeded)
  {
    goto Done;
  }

  if (base == POLICYMAN_BASE_CONFIG)
  {
    goto Done;
  }

  /*  The mask passed in has been initialized to the hardware band mask.
   (  If the base is "none", initialize the mask to 0.
   */
  if (base == POLICYMAN_BASE_NONE)
  {
    if (is_complex_mask)
    {
      SYS_LTE_BAND_MASK_CLR_ALL_BANDS(pLteMask);
    }
    else
    {
      *pMask = 0;
    }
  }

  /*  Get the number of child elements.  If there are none, just return.
   */
  nChildren = policyman_xml_num_children(pElem);

  if (nChildren == 0)                                       { goto Done; }

  for (i = 0 ; i < nChildren ; ++i)
  {
    pChild = policyman_xml_get_child(pElem, i);
    if (pChild == NULL)
    {
      POLICYMAN_UT_MSG("Internal error - no element #%d", i, 0, 0);
      goto Done;
    }

    pStr = policyman_xml_get_text(pChild);
    if      (policyman_xml_tag_is(pChild, "include"))
    {
      if (is_complex_mask)
      {
        succeeded = policyman_rf_lte_bands_parse(pStr, &maskLte);
        if (!succeeded)                                       
        {
          goto Done;
        }
        *pLteMask = maskLte;
      }
      else
      {
        succeeded = policyman_rf_bands_parse(pStr, &mask);
        if (!succeeded)                                       
        {
          goto Done;
        }
        *pMask |= mask;
      }
    }
    else if (policyman_xml_tag_is(pChild, "exclude"))
    {
      if (is_complex_mask)
      {
        succeeded = policyman_rf_lte_bands_parse(pStr, &maskLte);
        if (!succeeded)                                       
        {
          goto Done;
        }
        *pLteMask = maskLte;
      }
      else
      {
        succeeded = policyman_rf_bands_parse(pStr, &mask);
        if (!succeeded)                                       
        {
          goto Done;
        }
        *pMask &= ~mask;
      }
    }
    else
    {
      POLICYMAN_UT_MSG("Invalid tag in RF bands specification: %s", pStr, 0, 0);
      goto Done;
    }
  }

  succeeded = TRUE;

Done:
  return succeeded;
}



/*-------- policyman_rf_band_list_read --------*/
boolean
policyman_rf_band_list_read(
  policyman_xml_element_t const *pElem,
  rf_bands_t                    *pBands
  )
{
  policyman_status_t          succeeded = FALSE;
  size_t                      nChildren;
  size_t                      i;
  policyman_rf_device_info_t  rfInfo;

  /*  Initialize band information to hardware settings, so that the XML
   *  only needs to specify band masks that differ from the hardware.
   */
  if (policyman_state_get_rf_info(policyman_state_get_state(), 0, &rfInfo))
  {
    pBands->gwBands = rfInfo.gwBands;
    pBands->lteBands = rfInfo.lteBands;
    pBands->tdsBands = rfInfo.tdsBands;
  }

  nChildren = policyman_xml_num_children(pElem);
  if (nChildren == 0)
  {
    succeeded = TRUE;
    goto Done;
  }

  for (i = 0 ; i < nChildren ; ++i)
  {
    policyman_xml_element_t const *pChild;

    pChild = policyman_xml_get_child(pElem, i);
    if (pChild == NULL)
    {
      POLICYMAN_UT_MSG("Internal error - no element #%d", i, 0, 0);
      goto Done;
    }
    if(policyman_xml_tag_is(pChild, "gw_bands"))
    {
      if (!policyman_rf_band_mask_read(pChild, (void *)&pBands->gwBands, FALSE))    { goto Done; }
    }
    else if (policyman_xml_tag_is(pChild, "lte_bands"))
    {
      if (!policyman_rf_band_mask_read(pChild, (void *)&pBands->lteBands, TRUE))   { goto Done; }
    }
    else if (policyman_xml_tag_is(pChild, "tds_bands"))
    {
      if (!policyman_rf_band_mask_read(pChild, (void *)&pBands->tdsBands, FALSE))   { goto Done; }
    }
    else
    {
      goto Done;
    }
  }

  succeeded = TRUE;

Done:
  return succeeded;
}



/*-----------------------------------------------------------------------------
  Named RF band list actions
-----------------------------------------------------------------------------*/


/*-------- policyman_rf_band_list_action_execute --------*/
static boolean
policyman_rf_band_list_action_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
  )
{
  rf_bands_action_t  *pBandAction = (rf_bands_action_t *) pAction;
  policyman_policy_t *pPolicy = (policyman_policy_t *) pAction->pPolicy;

  if (pPolicy == NULL)
  {
    POLICYMAN_MSG_ERROR_0("Internal error - executing RF band list action with no policy");
    return FALSE;
  }

  return mre_named_object_add(
                  (policyman_named_object_t *) pBandAction->pBands,
                  MRE_NAMESPACE_SUBS,
                  (mre_policy_t *) pPolicy
                  );
}


/*-------- policyman_rf_band_list_action_new --------*/
mre_status_t
policyman_rf_band_list_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  mre_status_t      status = MRE_STATUS_ERR_MALFORMED_XML;
  rf_bands_action_t *pAction = NULL;
  char const        *pName;

  /*  Get the name to be associated with this list of bands.
   */
  pName = policyman_xml_get_attribute(pElem, "name");
  if (pName == NULL)
  {
    POLICYMAN_UT_MSG("<rf_band_list>: must have a 'name' attribute", 0, 0, 0);
    goto Done;
  }

  /*  Allocate and initialize the action itself.
   */
  pAction = (rf_bands_action_t *)
                policyman_mem_alloc(sizeof(rf_bands_action_t));
  ref_cnt_obj_init(pAction, policyman_rf_band_action_dtor);
  pAction->execute = policyman_rf_band_list_action_execute;

  /*  Allocate and initialize the bands object, giving it the name.
   */
  pAction->pBands = (rf_bands_t *) policyman_mem_alloc(sizeof(rf_bands_t));
  ref_cnt_obj_init(pAction->pBands, policyman_rf_bands_dtor);
  policyman_named_object_init(
      (policyman_named_object_t *) pAction->pBands,
      pName,
      POLICYMAN_NAMED_RF_BANDS
      );

  /*  Read the bands
   */
  if (!policyman_rf_band_list_read(pElem, pAction->pBands))
  {
    POLICYMAN_UT_MSG("<rf_band_list>: invalid band specification", 0, 0, 0);
    REF_CNT_OBJ_RELEASE_IF(pAction);
    goto Done;
  }

  *ppAction = (mre_action_t *) pAction;
  status = MRE_STATUS_SUCCESS;

Done:
  POLICYMAN_MSG_HIGH_1("<rf_band_list> returned status %d", status);
  return status;
}



/*-----------------------------------------------------------------------------
  RF bands action
-----------------------------------------------------------------------------*/


/*-------- execute_rf_bands --------*/
static boolean execute_rf_bands(
  policyman_action_t const *pAction,
  void                     *pCtx,
  sys_modem_as_id_e_type    subsId 
  )
{
  rf_bands_action_t *prfbAction = (rf_bands_action_t *) pAction;
  policyman_set_t   *pItemSet = ((policy_execute_ctx_t *) pCtx)->pItemSet;
  boolean            succeeded = FALSE;
  rf_bands_item_t   *prfb;

  /*  If we don't have a bands object, but we do have a name of one, look it up.
   */
  if (prfbAction->pListName != NULL && prfbAction->pBands == NULL)
  {
    /*  Look up the list.  Add a reference to the resulting object for this action.
     */
    prfbAction->pBands = (rf_bands_t *) mre_named_object_find(
                                                prfbAction->pListName,
                                                POLICYMAN_NAMED_RF_BANDS,
                                                prfbAction->pPolicy
                                                );
    if (prfbAction->pBands == NULL)   { goto Done; }
    ref_cnt_obj_add_ref(prfbAction->pBands);

    /*  Having looked up the list, we don't need to keep its name.
     */
    policyman_mem_free((void *) prfbAction->pListName);
    prfbAction->pListName = NULL;
  }

  prfb = (rf_bands_item_t *) policyman_rf_bands_item_new( prfbAction->pBands->gwBands,
                                                          prfbAction->pBands->lteBands,
                                                          prfbAction->pBands->tdsBands );

  prfb->asubs_id = subsId;

  policyman_set_replace(pItemSet, &prfb);
  ref_cnt_obj_release(prfb);

  POLICYMAN_MSG_HIGH_1("action <rf_bands> set for subs %d:", prfb->asubs_id);
  policyman_rf_print_bands(prfb->gwBands, prfb->lteBands, prfb->tdsBands);

  succeeded = TRUE;

Done:
  return succeeded;
}


/*-------- policyman_rf_bands_action_execute --------*/
static boolean policyman_rf_bands_action_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
  )
{
  policyman_action_t const *pAct = (policyman_action_t const *)pAction;

  return policyman_execute_action(pAct, pCtx, execute_rf_bands);
}


/*-------- policyman_rf_bands_if_action_execute --------*/
static boolean
policyman_rf_bands_if_action_execute(
  mre_action_t const  *pAction,
  void                      *pCtx
  )
{
  rf_bands_action_t  *pRfBandsAction  = (rf_bands_action_t *)pAction;
  boolean             succeeded       = TRUE;
  boolean             item_is_present;

  item_is_present = policyman_cfgitem_already_present( POLICYMAN_ITEM_RF_BANDS,
                                                       ACTION_SUBS(pRfBandsAction),
                                                       pRfBandsAction->pmSubsId );
  if (item_is_present)
  {
    POLICYMAN_UT_MSG_0("<rf_bands_if> skipping update - item present");
  }
  else
  {
    succeeded = policyman_rf_bands_action_execute(pAction, pCtx);
  }

  return succeeded;
}


/*-------- policyman_rf_bands_create_new_action --------*/
mre_status_t
policyman_rf_bands_create_new_action(
  policyman_xml_element_t const *pElem,
  policyman_policy_t            *pPolicy,
  mre_action_t            **ppAction,
  pfn_execute_t                 executefn
  )
{
  mre_status_t            status = MRE_STATUS_ERR_MALFORMED_XML;
  rf_bands_action_t       *pAction = NULL;
  char const              *pStr;
  policyman_subs_t        pm_subs;

  /*  Allocate and initialize the action itself.
   */
  pAction = (rf_bands_action_t *)
                policyman_mem_alloc(sizeof(rf_bands_action_t));
  ref_cnt_obj_init(pAction, policyman_rf_band_action_dtor);

  pAction->execute = executefn;

  /* Does this action indicate the subscription
   *  Default is subscription from policy
   */
  status = policyman_util_get_subs( pElem, POLICY_SUBS(pPolicy), &pm_subs);
  if (MRE_FAILED(status))
  {
    goto Done;
  }
  pAction->pmSubsId = pm_subs;
  
  /*  If this action uses a named bands list, save the name and we're done.
   */
  pStr = policyman_xml_get_attribute(pElem, "list");
  if (pStr != NULL)
  {
    pAction->pListName = policyman_str_dup(pStr);
    status = MRE_STATUS_SUCCESS;
    goto Done;
  }

  /*  Otherwise, read the inline band list.
   */
  pAction->pBands = (rf_bands_t *) policyman_mem_alloc(sizeof(rf_bands_t));
  ref_cnt_obj_init(pAction->pBands, policyman_rf_bands_dtor);
  if (!policyman_rf_band_list_read(pElem, pAction->pBands))
  {
    POLICYMAN_UT_MSG("<rf_bands>: invalid specification", 0, 0, 0);
    goto Done;
  }

  status = MRE_STATUS_SUCCESS;

Done:
  if (MRE_SUCCEEDED(status))
  {
    *ppAction = (mre_action_t *) pAction;
  }
  else
  {
    REF_CNT_OBJ_RELEASE_IF(pAction);
  }

  return status;
}


/*-------- policyman_rf_bands_action_new --------*/
mre_status_t
policyman_rf_bands_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  return policyman_rf_bands_create_new_action(
            pElem, (policyman_policy_t *) pPolicy, ppAction,
            policyman_rf_bands_action_execute);
}

/*-------- policyman_rf_bands_if_action_new --------*/
mre_status_t
policyman_rf_bands_if_action_new(
  policyman_xml_element_t const *pElem,
  mre_policy_t                  *pPolicy,
  mre_action_t            **ppAction
  )
{
  return policyman_rf_bands_create_new_action(
            pElem, (policyman_policy_t *) pPolicy, ppAction,
            policyman_rf_bands_if_action_execute);
}


/*=============================================================================
  Methods for the RF band configuration item
=============================================================================*/

/*-------- policyman_rf_bands_get_default_from_efs --------*/
policyman_item_t * policyman_rf_bands_get_default_from_efs(
  sys_modem_as_id_e_type subs_id
)
{
  rf_bands_item_t             *prfb = NULL;
  uint32                       ignore;
  sys_band_mask_type           gwBands;
  sys_lte_band_mask_e_type     lteBands;
  sys_band_mask_type           tdsBands;

  /* read EFS for given subscription
   */
  if (!policyman_retrieve_rats_bands(subs_id, &ignore, &gwBands, &lteBands, &tdsBands))
  {
    POLICYMAN_MSG_ERROR_1("subs %d: can't populate RF item from EFS", subs_id);
    goto Done;
  }

  prfb = (rf_bands_item_t *)policyman_rf_bands_item_new( gwBands,
                                                         lteBands,
                                                         tdsBands );
  prfb->asubs_id = subs_id;

Done:

  return (policyman_item_t *)prfb;
}
#endif
/*-------- policyman_rf_bands_get_default --------*/
policyman_item_t *
policyman_rf_bands_get_default(
  sys_modem_as_id_e_type subsId
  )
{
  policyman_rf_device_info_t  *pOverall = policyman_rf_get_overall_info();
  rf_bands_item_t             *prfb;

  if (pOverall == NULL)   
  { 
    return NULL; 
  }

  /* Get RF band Item with all bands
   */
  prfb = (rf_bands_item_t *) policyman_rf_bands_item_new( pOverall->gwBands,
                                                          pOverall->lteBands,
                                                          pOverall->tdsBands );

  prfb->asubs_id = subsId;

  POLICYMAN_MSG_HIGH_1("Default RF bands for subs %d", subsId);
  policyman_rf_print_bands(prfb->gwBands, prfb->lteBands, prfb->tdsBands);

  return (policyman_item_t *) prfb;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rf_bands_compare --------*/
boolean
policyman_rf_bands_compare(
  policyman_item_t  *pi1,
  policyman_item_t  *pi2
  )
{
  rf_bands_item_t *prf1 = (rf_bands_item_t *) pi1;
  rf_bands_item_t *prf2 = (rf_bands_item_t *) pi2;

  return      prf1 != NULL
          &&  prf2 != NULL
          &&  policyman_cfgitem_id(pi1) == POLICYMAN_ITEM_RF_BANDS
          &&  policyman_cfgitem_id(pi2) == POLICYMAN_ITEM_RF_BANDS
          &&  prf1->gwBands == prf2->gwBands
          &&  SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&prf1->lteBands, &prf2->lteBands)
          &&  prf1->tdsBands == prf2->tdsBands
          ;

}

#endif
/*-------- policyman_rf_print_bands --------*/
void
policyman_rf_print_bands(
  sys_band_mask_type  gwBands,
  sys_lte_band_mask_e_type  lteBands,
  sys_band_mask_type  tdsBands
  )
{
  POLICYMAN_MSG_HIGH_2( "   GW-64  0x%08x.%08x",
                        (int)(gwBands >> 32),
                        (int)(gwBands) );
#if defined(LTE_BAND_NUM) && (LTE_BAND_NUM == 256)
  POLICYMAN_MSG_HIGH_4( "  LTE-256-MSB 0x%08x%08x.%08x%08x",
                        (int)(lteBands.bits_193_256 >> 32),
                        (int)(lteBands.bits_193_256),
                        (int)(lteBands.bits_129_192 >> 32),
                        (int)(lteBands.bits_129_192) );
  POLICYMAN_MSG_HIGH_4( "  LTE-256-LSB 0x%08x%08x.%08x%08x",
                        (int)(lteBands.bits_65_128 >> 32),
                        (int)(lteBands.bits_65_128),
                        (int)(lteBands.bits_1_64 >> 32),
                        (int)(lteBands.bits_1_64) );
#else
  POLICYMAN_MSG_HIGH_2( "  LTE-64  0x%08x.%08x",
                        (int)(lteBands >> 32),
                        (int)(lteBands ) );
#endif
  POLICYMAN_MSG_HIGH_2( "  TDS-64  0x%08x.%08x",
                        (int)(tdsBands >> 32),
                        (int)(tdsBands) );
}


/*-------- policyman_rf_bands_display --------*/
void
policyman_rf_bands_display(
  policyman_item_t  *pItem
  )
{
  rf_bands_item_t *prfb = (rf_bands_item_t *) pItem;

  POLICYMAN_MSG_HIGH_1("RF bands for subs %d:", pItem->asubs_id);
  policyman_rf_print_bands(prfb->gwBands, prfb->lteBands, prfb->tdsBands);
}



/*-------- policyman_rf_get_rats --------*/
static uint32
policyman_rf_get_rats(
  sys_band_mask_type       gwBands,
  sys_lte_band_mask_e_type lteBands,
  sys_band_mask_type       tdsBands
  )
{
  uint32  ratMask = 0;

  if ((gwBands & ALL_GSM_BANDS) && HAS_CAPABILITY(GSM))
  {
    ratMask |= SYS_SYS_MODE_MASK_GSM;
  }

  if ((gwBands & ALL_WCDMA_BANDS) && HAS_CAPABILITY(WCDMA) && policyman_tech_is_loaded(CLSID_WCDMA))
  {
    ratMask |= SYS_SYS_MODE_MASK_WCDMA;
  }

  if ((gwBands & ALL_CDMA_BANDS) && HAS_CAPABILITY(1X))
  {
    ratMask |= SYS_SYS_MODE_MASK_CDMA;
  }

  if ((gwBands & ALL_HDR_BANDS) && HAS_CAPABILITY(DO))
  {
    ratMask |= SYS_SYS_MODE_MASK_HDR;
  }

  if ( (!SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&lteBands)) && ( HAS_CAPABILITY(LTE) || HAS_CAPABILITY(NB_IOT) ))
  {
    ratMask |= SYS_SYS_MODE_MASK_LTE;
  }

  if ((tdsBands & ALL_TDS_BANDS) && HAS_CAPABILITY(TDSCDMA) && policyman_tech_is_loaded(CLSID_TDSCDMA))
  {
    ratMask |= SYS_SYS_MODE_MASK_TDS;
  }

  return ratMask;
}


/*-------- policyman_rf_get_num_devices --------*/
static size_t
policyman_rf_get_num_devices(
  void
  )
{
  // TODO: Use rfm_get_num_devices() once it becomes available.  Use '4' for now...
  return 4;
}

/*-------- policyman_get_hardware_rats_bands --------*/
void
policyman_get_hardware_bands_all_techs(
  sys_band_mask_type       *pGwBands,
  sys_lte_band_mask_e_type *pLteBands,
  sys_band_mask_type       *pTdsBands
  )
{
  size_t  i;
  size_t  nDevices = policyman_rf_get_num_devices();

  /*  Get the GW band information for all devices.
   */
  if( pGwBands != NULL )
  {
    *pGwBands = 0;
    for (i = 0 ; i < nDevices ; ++i)
    {
      *pGwBands |= rfm_get_band_mask(i);
    }
  }

  /*  LTE bands only if enabled.
   */
  if( pLteBands != NULL )
  {
    SYS_LTE_BAND_MASK_CLR_ALL_BANDS(pLteBands);
#ifdef  FEATURE_LTE
    *pLteBands = rfm_get_lte_band_mask();
#endif
  }

  /*  TDS bands only on device 0, if enabled, and if loaded.
   */
  if( pTdsBands != NULL )
  {
    *pTdsBands = 0;
#ifdef  FEATURE_TDSCDMA
    *pTdsBands = rfm_get_tds_band_mask(RFCOM_TRANSCEIVER_0);
#endif
  }
}

/*-------- policyman_get_hardware_rats_bands --------*/
void
policyman_get_hardware_rats_bands(
  uint32                  *pRatMask,
  sys_band_mask_type       *pGwBands,
  sys_lte_band_mask_e_type *pLteBands,
  sys_band_mask_type       *pTdsBands
  )
{
  sys_band_mask_type       gwBands;
  sys_band_mask_type       tdsBands;
  sys_lte_band_mask_e_type lteBands;
  static boolean     isRfmInitDone = FALSE;

  if(!isRfmInitDone)
  {
    mcs_rcevt_wait_name("RFM_INIT_COMPLETE");
    isRfmInitDone = TRUE;
  }

  policyman_get_hardware_bands_all_techs( &gwBands, &lteBands, &tdsBands );

  if (!policyman_tech_is_loaded(CLSID_WCDMA))
  {
    gwBands &= ~ALL_WCDMA_BANDS;
  }

  if (!policyman_tech_is_loaded(CLSID_TDSCDMA))
  {
    tdsBands = 0;
  }

  if( pGwBands != NULL )
  {
    *pGwBands = gwBands;
  }
  if( pLteBands != NULL )
  {
    *pLteBands = lteBands;
  }
  if( pTdsBands != NULL )
  {
    *pTdsBands = tdsBands;
  }

  /*  Combine the information from all devices into the overall device info.
   */
  if( pRatMask != NULL )
  {
    *pRatMask = policyman_rf_get_rats(gwBands, lteBands, tdsBands);
  }
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rf_get_specialization --------*/
sys_specialization_t policyman_rf_get_specialization(
  void
)
{
  sys_specialization_t         specialization = SYS_SPECIAL_NONE;
  size_t                       numItems;
  policyman_item_id_t const    ids[]          = {POLICYMAN_ITEM_DEVICE_CONFIGURATION};
  policyman_item_t const      *pItem;

  /* Fetch specialization.
   * Failure to fetch leaves specialization untouched and as initialized above.
   */

  numItems = policyman_cfgitem_get_items( ids,
                                          ARR_SIZE(ids),
                                          &pItem );
  if (numItems == 1)
  {
    (void)policyman_device_config_get_specialization( pItem, &specialization );

    policyman_item_release(pItem);
  }

  return specialization;
}

/*-------- policyman_rf_determine_volte_restricted_e911_rats --------*/
/**
@brief  Return the RATs allowed for e911 calls when volte is enabled.
        This is a synchronous call that can be made at any time.

        Should only be called when volte is enabled and appropriate
        specialization is active.  Assumes these checks have been
        done by caller.

@return
  No return value. The supplied rat mask is modified as necessary.
*/
void policyman_rf_determine_volte_restricted_e911_rats(
  sys_modem_as_id_e_type   subsId,
  uint32                  *ratMask
)
{
  sys_mcc_type       mcc;
  boolean            mcc_is_in_japan;
  boolean            csim_is_enabled;

  mcc              = policyman_state_get_subs_mcc(subsId);
  mcc_is_in_japan  = policyman_plmn_mcc_is_volte_only(mcc, subsId);
  csim_is_enabled  = (MMGSDI_APP_CSIM == policyman_state_get_cdma_sim_type(subsId));

  if (mcc_is_in_japan && csim_is_enabled)
  {
    // Hardware minus C H T
    *ratMask &= ~(SYS_SYS_MODE_MASK_CDMA | SYS_SYS_MODE_MASK_HDR | SYS_SYS_MODE_MASK_TDS);
  }
  else if (csim_is_enabled)
  {
    // Hardware minus T
    *ratMask &= ~(SYS_SYS_MODE_MASK_TDS);
  }
  else
  {
    // Hardware minus C H
    *ratMask &= ~(SYS_SYS_MODE_MASK_CDMA | SYS_SYS_MODE_MASK_HDR);
  }

  return;
}

/*-------- policyman_get_e911_rat_mask --------*/
/**
@brief  Return the RATs allowed for e911 calls.
        This is a synchronous call that can be made at any time.

@return
  ratMask  The allowed RAT mask.
*/
uint32 policyman_get_e911_rat_mask(
  sys_modem_as_id_e_type  subsId
)
{
  sys_specialization_t         specialization;
  boolean                      volte_is_enabled;
  policyman_rf_device_info_t   hwRatsAndBands;
  uint32                       ratMask;

  specialization   = policyman_rf_get_specialization();
  volte_is_enabled = policyman_ph_get_volte_is_enabled(subsId);


  // Fetch hardware rats and bands
  policyman_get_hardware_rats_bands( &hwRatsAndBands.ratMask,
                                     &hwRatsAndBands.gwBands,
                                     &hwRatsAndBands.lteBands,
                                     &hwRatsAndBands.tdsBands );

  // Start with hardware RATs
  ratMask = hwRatsAndBands.ratMask;

  if ( (SYS_SPECIAL_VOLTE_E911_CALLS == specialization) &&
       volte_is_enabled )
  {
    policyman_rf_determine_volte_restricted_e911_rats(subsId, &ratMask);
  }


  POLICYMAN_MSG_HIGH( "e911_rat_mask: hw 0x%04x, ret 0x%04x, spec %d",
                      hwRatsAndBands.ratMask,
                      ratMask,
                      specialization );

  return ratMask;
}

/*-------- policyman_rf_init --------*/
boolean
policyman_rf_init(
  policyman_rf_info_t **ppRfInfo
  )
{
  policyman_rf_info_t         *pRfInfo;
  policyman_rf_device_info_t  *pOverall;
  size_t                      nDevices = policyman_rf_get_num_devices();

  pRfInfo = (policyman_rf_info_t *) policyman_mem_alloc(
                                      sizeof(policyman_rf_info_t) +
                                      nDevices * sizeof(policyman_rf_device_info_t)
                                      );

  pRfInfo->nDevices = nDevices;

  pOverall = &pRfInfo->overall;
  policyman_get_hardware_rats_bands(
      &pOverall->ratMask,
      &pOverall->gwBands,
      &pOverall->lteBands,
      &pOverall->tdsBands
      );

  POLICYMAN_MSG_HIGH_1("Default RAT mask: 0x%04x", pOverall->ratMask);
  POLICYMAN_MSG_HIGH_0("Default RF bands:");
  policyman_rf_print_bands(pOverall->gwBands, pOverall->lteBands, pOverall->tdsBands);

  POLICYMAN_MEM_FREE_IF(*ppRfInfo);

  *ppRfInfo = pRfInfo;

  return TRUE;
}


/*-------- policyman_rf_deinit --------*/
void
policyman_rf_deinit(
  policyman_rf_info_t *pRfInfo
  )
{
  POLICYMAN_MEM_FREE_IF(pRfInfo);
}
#else

/*-------- policyman_state_rf_init --------*/
void
policyman_state_rf_init(
  void
  )
{
  policyman_rf_device_info_t  *pOverall;

  device_rfinfo.nDevices = policyman_rf_get_num_devices();

  pOverall = &device_rfinfo.overall;
  policyman_get_hardware_rats_bands(
      &pOverall->ratMask,
      &pOverall->gwBands,
      &pOverall->lteBands,
      &pOverall->tdsBands
      );

  POLICYMAN_MSG_HIGH_1("Default RAT mask: 0x%04x", pOverall->ratMask);
  POLICYMAN_MSG_HIGH_0("Default RF bands:");
  policyman_rf_print_bands(pOverall->gwBands, pOverall->lteBands, pOverall->tdsBands);

}


/*-------- policyman_rf_deinit --------*/
void
policyman_rf_deinit(
  void
  )
{
  return;
}

#endif

/*-------- policyman_rf_get_device_info --------*/
boolean
policyman_rf_get_device_info(
  policyman_rf_info_t const   *pRfInfo,
  size_t                      device,
  policyman_rf_device_info_t  *pDeviceInfo
  )
{
  if (pRfInfo == NULL || device >= pRfInfo->nDevices)
  {
    return FALSE;
  }
  #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  *pDeviceInfo = pRfInfo->overall;
  #else
  *pDeviceInfo = device_rfinfo.overall;
  #endif
  return TRUE;
}


/*-------- policyman_rf_get_overall_info --------*/
policyman_rf_device_info_t *policyman_rf_get_overall_info(
  void
)
{
  #ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  policyman_state_t           *pState;
  policyman_rf_device_info_t  *pOverall;

  pState   = policyman_state_get_state();
  pOverall = &pState->pDeviceInfo->pRfInfo->overall;
  #else
  policyman_rf_device_info_t  *pOverall;

  pOverall = &device_rfinfo.overall;
  #endif
  return pOverall;
}


/*-------- policyman_rf_get_overall_rat_mask --------*/
uint32
policyman_rf_get_overall_rat_mask(
  void
  )
{
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
  uint32 ratMask = SYS_SYS_MODE_MASK_NONE;
  policyman_rf_device_info_t * pOverall = policyman_rf_get_overall_info();

  if( pOverall != NULL )
  {
    ratMask = pOverall->ratMask;
  }
  return ratMask;

  #else
  policyman_rf_device_info_t *pOverall;

  pOverall = policyman_rf_get_overall_info();

  return pOverall->ratMask;

  #endif
}


/*-------- policyman_validate_rf_bands_item --------*/
static policyman_status_t
policyman_validate_rf_bands_item(
  rf_bands_item_t *prf
  )
{
  if (prf == NULL)
  {
    POLICYMAN_MSG_ERROR_0("get_rf_bands returned err POLICYMAN_STATUS_ERR_INVALID_ARGS");
    return POLICYMAN_STATUS_ERR_INVALID_ARGS;
  }

  if (policyman_cfgitem_id((policyman_item_t *) prf) != POLICYMAN_ITEM_RF_BANDS)
  {
    POLICYMAN_MSG_ERROR_0("get_rf_bands returned err POLICYMAN_STATUS_ERR_INVALID_ITEM_ID");
    return POLICYMAN_STATUS_ERR_INVALID_ITEM_ID;
  }

  return POLICYMAN_STATUS_SUCCESS;
}


/*-------- policyman_get_rf_bands --------*/
policyman_status_t
policyman_get_rf_bands(
  policyman_item_t const    *pItem,
  sys_band_mask_type        *pGWbands,
  sys_lte_band_mask_e_type  *pLTEbands,
  sys_band_mask_type        *pTDSbands
  )
{
  policyman_status_t  status = POLICYMAN_STATUS_SUCCESS;
  rf_bands_item_t     *prf = (rf_bands_item_t *) pItem;

  status = policyman_validate_rf_bands_item(prf);
  if (POLICYMAN_FAILED(status))   { return status; }

  if (pGWbands != NULL)
  {
    *pGWbands = prf->gwBands;
  }
  if (pLTEbands != NULL)
  {
    *pLTEbands = prf->lteBands;
  }
  if (pTDSbands != NULL)
  {
    *pTDSbands = prf->tdsBands;
  }

  POLICYMAN_MSG_HIGH_0("state RF bands:");
  policyman_rf_print_bands(prf->gwBands, prf->lteBands, prf->tdsBands);

  return status;
}


/*-------- policyman_get_rf_bands_force --------*/
policyman_status_t policyman_get_rf_bands_force(
  policyman_item_t const  *pItem,
  boolean                 *pForce
)
{
  policyman_status_t   status = POLICYMAN_STATUS_SUCCESS;
  rf_bands_item_t     *prf    = (rf_bands_item_t *) pItem;

  status = policyman_validate_rf_bands_item(prf);

  if (POLICYMAN_STATUS_SUCCESS == status)
  {
    *pForce = FALSE;
  }

  return status;
}

#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_get_bands_item_for_named_bands --------*/
rf_bands_item_t * policyman_get_bands_item_for_named_bands(
     rf_bands_t *pBands
)
{
  rf_bands_item_t *prfb = NULL;

  if (NULL != pBands)
  {
    prfb = (rf_bands_item_t*)policyman_rf_bands_item_new( pBands->gwBands,
                                                          pBands->lteBands,
                                                          pBands->tdsBands );
  }

  return prfb;
}

/*-------- policyman_get_bands_for_named_object --------*/
void
policyman_get_bands_for_named_object(
  rf_bands_t         *pBands,
  rf_bands_t         *pGetBands
)
{
  pGetBands->gwBands  = pBands->gwBands;
  pGetBands->lteBands = pBands->lteBands;
  pGetBands->tdsBands = pBands->tdsBands;
}
#endif

/*-------- policyman_rf_bands_filter_based_on_rat --------*/
void
policyman_rf_bands_filter_based_on_rat(
  rf_bands_item_t  *prfb,
  uint32           ratMask
)
{
  if( (ratMask & (SYS_SYS_MODE_MASK_WCDMA|SYS_SYS_MODE_MASK_TDS)) == (SYS_SYS_MODE_MASK_WCDMA|SYS_SYS_MODE_MASK_TDS))
  {
    POLICYMAN_MSG_MED_0("RAT mask has WCDMA and TDS : bands not filtered");
  }
  else if( (ratMask & SYS_SYS_MODE_MASK_WCDMA) == SYS_SYS_MODE_MASK_WCDMA)
  {
    POLICYMAN_MSG_MED_0("RAT mask has WCDMA: removing TDS bands");
    prfb->tdsBands = 0;
  }
}
#ifndef FEATURE_DISABLE_DYNAMIC_POLICIES
/*-------- policyman_rf_bands_fill_subs_diag_log --------*/
void policyman_rf_bands_fill_subs_diag_log(
  policyman_item_t const  *pItem,
  LOG_PM_SUBS_INFO_type   *log_ptr
)
{
  rf_bands_item_t     *pRfBands = (rf_bands_item_t *)pItem;

  log_ptr->gw_bands  = pRfBands->gwBands;
  log_ptr->lte_band_count = sizeof(sys_lte_band_mask_e_type) / sizeof(uint64);
#if defined(LTE_BAND_NUM) && (LTE_BAND_NUM == 256)
  log_ptr->lte_bands_0    = pRfBands->lteBands.bits_1_64;
  log_ptr->lte_bands_1    = pRfBands->lteBands.bits_65_128;
  log_ptr->lte_bands_2    = pRfBands->lteBands.bits_129_192;
  log_ptr->lte_bands_3    = pRfBands->lteBands.bits_193_256;
#else
  log_ptr->lte_bands_0    = pRfBands->lteBands;
  log_ptr->lte_bands_1    = 0;
  log_ptr->lte_bands_2    = 0;
  log_ptr->lte_bands_3    = 0;
#endif
  log_ptr->tds_bands = pRfBands->tdsBands;

}
#endif
