/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                P B M _ E C C _ L I B.C

GENERAL DESCRIPTION
  This file contains the APIs exported by PBM service which is executed as a
  library within the PBM client's task.

  Copyright (c) 2016 - 2017 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/pbm/src/pbm_ecc_lib.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/17   tq      Fix KW error
06/23/16   nr      ECC optimizations for IoE devices
06/15/16   nr      Deprecate legacy APIs
05/26/16   nr      Logging improvements
03/10/16   nr      Logging improvements
31/01/16   nr      Intial Version
===========================================================================*/
#include "customer.h"
#include "uim_msg.h"
#include "pbm.h"
#include "pbmlib.h"
#include "pbmutils.h"
#include "pbmcache.h"
#include "pbmuim.h"
#include "pbmgsdi.h"
#include "pbmtask_i.h"
#include "stdlib.h"
#include "stringl/stringl.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION PBM_SESSION_EMERGENCY_NUMBER_CAT_EXT

DESCRIPTION
  This function searches the ECC phonebook for the number just passed in
  to determine if the current number is an Emergency number.

  If the found is found in gw only  then found_in_gw is marked TRUE and relavent category
  if associated with the number will be filled in ecc_category_ptr_gw

  If the found is found in 1x only  then found_in_1x is marked TRUE and relavent category
  if associated with the number will be filled in ecc_category_ptr_1x

  If the found is found in both 1x and gw  then found_in_gw and found_in_1x are marked TRUE
  and relavent category if associated with the number will be filled in ecc_category_ptr_gw
  and ecc_category_ptr_1x

  Length does not include the trailing NULL.

DEPENDENCIES


RETURN VALUE
  pbm_return_type - PBM_SUCCESS

SIDE EFFECTS
  none
===========================================================================*/
pbm_return_type pbm_session_emergency_number_cat_ext(
  pbm_session_enum_type        p_session_type,
  const byte                  *num,
  byte                         len,
  boolean                     *found_in_gw,
  boolean                     *found_in_1x,
  uint8                       *ecc_category_ptr_gw,
  uint8                       *ecc_category_ptr_1x
)
{
  pbm_session_enum_type                 l_session_type   = PBM_SESSION_MAX;
  pbm_record_id_type                    rec_id           = PBM_INVALID_REC_ID;
  pbm_phonebook_type                    pb_id            = {PBM_GPB};
  pbm_return_type                       ret_val          = PBM_ERROR;
  boolean                               is_emergency_num = FALSE;
  /* field found in the current record */
  pbm_field_id_e_type                   found_field      = PBM_FIELDID_NONE;
  uint32                                best_match_index_gw;
  /* index into priority array of best category so far */
  uint32                                best_match_index_1x;
  uint32                                i                = 0;
  /* category found in the current record */
  uint8                                 found_category   = 0;
  uint8                                 handle           = 0;

  /* Rel 11 specs say to prefer categories in network over sim, and SIM over others
   * NV aren't spec'd, so they go to the bottom */
  pbm_field_id_e_type ecc_order_preference[] =
  {
    PBM_FIELD_NETWORK_ECC,
    PBM_FIELD_SIM_ECC,
    PBM_FIELD_HARDCODED_ECC,
    PBM_FIELD_NV_ECC
  };

  /* specs say to prefer categories in SIM over network, and network over others
   * NV aren't spec'd, so they go to the bottom */
  pbm_field_id_e_type ecc_order_preference_pre_rel11[] =
  {
    PBM_FIELD_SIM_ECC,
    PBM_FIELD_NETWORK_ECC,
    PBM_FIELD_HARDCODED_ECC,
    PBM_FIELD_NV_ECC
  };
  byte my_ph_num[PBM_MAX_NUM_LENGTH+1]; //plus null char

  PBM_CHECK_PTR5_RET(num, ecc_category_ptr_gw, ecc_category_ptr_1x, found_in_gw, found_in_1x, PBM_ERROR);

  UIM_MSG_HIGH_8("Checking num as emergency. Len:0x%x. Num:0x%x 0x%x 0x%x 0x%x 0x%x pbm_3gpp_rel_ver 0x%x p_session_type 0x%x",
                  len, (len>0?num[0]:-1), (len>1?num[1]:-1), (len>2?num[2]:-1), (len>3?num[3]:-1), (len>4?num[4]:-1),
                  pbm_3gpp_rel_ver, p_session_type);
  *ecc_category_ptr_gw = 0;
  *ecc_category_ptr_1x = 0;
  *found_in_gw = FALSE;
  *found_in_1x = FALSE;
  if ( pbm_3gpp_rel_ver < LTE_3GPP_REL11  )
  {
    memscpy( ecc_order_preference , ARR_SIZE(ecc_order_preference) ,
             ecc_order_preference_pre_rel11 , ARR_SIZE(ecc_order_preference_pre_rel11)  );
  }
  best_match_index_gw = ARR_SIZE(ecc_order_preference);
  best_match_index_1x = ARR_SIZE(ecc_order_preference);

  if (len == 0 || len > PBM_MAX_NUM_LENGTH)
  {
    return PBM_SUCCESS;
  }
  if (p_session_type < PBM_SESSION_GPB_1 ||
      p_session_type >= PBM_SESSION_MAX)
  {
    return PBM_ERROR;
  }

  memset(my_ph_num,0,sizeof(my_ph_num));
  memscpy(my_ph_num, sizeof(my_ph_num), num, len);

  for(l_session_type = PBM_SESSION_GPB_1;l_session_type < PBM_SESSION_MAX;l_session_type++)
  {
    if(PBM_LPB != pbm_session_type_to_pb_category(l_session_type))
    {
      continue;
    }

    /* only check in RATs which belong to the requested slot.*/
    if(pbm_session_info[l_session_type].slot_id != pbm_session_info[p_session_type].slot_id )
    {
      continue;
    }

    pb_id = pbm_uim_device_type_to_phonebook_type((pbm_uim_app_enum_type)l_session_type,PBM_ECC);

    /* If no valid slot_id associated with provisioning sessions means 
       either card is in detected state or in card error state */
    if(!(pbm_session_info[p_session_type].slot_id >= PBM_SLOT_1 &&
        pbm_session_info[p_session_type].slot_id < PBM_NUM_SLOTS))
    {
      pbm_slot_type            slot           = PBM_SLOT_1;
      pbm_session_enum_type    card_session   = PBM_SESSION_DEFAULT;

      for(slot = PBM_SLOT_1; slot < PBM_NUM_SLOTS; slot++)
      {
        if(pbm_is_card_present_in_slot(slot))
        {
          break;
        }
      }

      /*
       * 1. Check first NV per slot cache content to see if the dialing string is emergency or not
       * 2. check Hard code cache if the number is not found in NV per sub
       */
      if(slot != PBM_NUM_SLOTS)
      {
        /* get session_index for GPB for same (slot) first */
        card_session = pbm_pb_type_and_slot_to_session_type(slot, PBM_GPB);
        pb_id        = pbm_uim_device_type_to_phonebook_type((pbm_uim_app_enum_type)card_session,
                                                              PBM_ECC);
      }
    }

    /* Try the numbers in ECC phonebook. */
    ret_val = pbm_session_enum_rec_init_ext(pb_id, (uint16)PBM_CAT_ECC, PBM_FIELD_NONE,
                                             my_ph_num, (uint16)strlen((char*)my_ph_num)+1,
                                             PBM_SEARCHTYPE_EXACT | PBM_DATATYPE_STR |
                                             PBM_FIELDID_PHONE | PBM_STRTYPE_PBM8, &handle);
    if (PBM_SUCCESS == ret_val)
    {
      while (PBM_SUCCESS == pbm_session_enum_next_rec_id_ext(&rec_id, handle))
      {
        pbm_ecc_mode_type emergency_mode = NOT_EMERGENCY;

        /* we have a match */
        is_emergency_num = TRUE;

        /* see if we can find a category */
        if (pbm_find_ecc_cat_in_rec(rec_id, &found_category,&found_field,&emergency_mode))
        {
          /* we found a category, Now, see if it's better than any category we may have. */
          for (i=0; i<ARR_SIZE(ecc_order_preference); i++)
          {
            if (ecc_order_preference[i] == found_field)
            {
              if (i < best_match_index_gw &&
                  (emergency_mode == EMERGENCY_GW || emergency_mode == EMERGENCY_GW_1X))
              {
                best_match_index_gw = i;
                *ecc_category_ptr_gw = found_category;
                UIM_MSG_HIGH_2("Picking GW emergency category 0x%x based on field 0x%x",
                               *ecc_category_ptr_gw, found_field);
              }
              else if (i < best_match_index_1x &&
                       (emergency_mode == EMERGENCY_1X ||
                        emergency_mode == EMERGENCY_GW_1X))
              {
                best_match_index_1x = i;
                *ecc_category_ptr_1x = found_category;
                UIM_MSG_HIGH_2("Picking 1X emergency category 0x%x based on field 0x%x",
                                *ecc_category_ptr_1x, found_field);
              }
            }
          }
        }

        UIM_MSG_HIGH_4("best_match_index_gw 0x%x best_match_index_1x 0x%x i 0x%x rec_id 0x%x",
                         best_match_index_gw, best_match_index_1x, i, rec_id);
        if (emergency_mode == EMERGENCY_1X ||
            emergency_mode == EMERGENCY_GW_1X)
        {
          *found_in_1x = TRUE;
        }
        if (emergency_mode == EMERGENCY_GW ||
            emergency_mode == EMERGENCY_GW_1X)
        {
          *found_in_gw = TRUE;
        }
      }
      pbm_session_enum_rec_init_ext_free_handle(handle); /* free the handle */
    }
    else
    {
      UIM_MSG_ERR_1("pbm_session_enum_rec_init failed %d", ret_val);
    }
  }
  UIM_MSG_HIGH_6("Emergency number status 0x%x gw cat 0x%x 1X cat 0x%x found_in_gw 0x%x found_in_1x 0x%x session 0x%x",
  	          is_emergency_num, *ecc_category_ptr_gw, *ecc_category_ptr_1x,
                  *found_in_gw,
  	          *found_in_1x, p_session_type);
  return PBM_SUCCESS;
} /* pbm_session_emergency_number_cat_ext */

