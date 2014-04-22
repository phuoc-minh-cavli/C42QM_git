/*===========================================================================
                      DS_3GPP_PDN_THROT_CONFIG.C
DESCRIPTION
This file defines PDN throttling config APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_pdn_throt_config.c#2 $
  $DateTime: 2022/08/16 22:48:44 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_3gpp_pdn_throt_config.h"
#include "ds_pdn_nv_manager.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_pdn_apm_efs_parser.h"
#include "ds_3gpp_pdn_throttle_sm.h"

/*-----------------------------------------------------------------------------
  Structure used to store the service req throttle config values from EFS
-----------------------------------------------------------------------------*/
typedef struct
{
  uint32 failure_timer[DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS];   /* Failure Timer  */
}ds_3gpp_pdn_throt_config_type;

/*---------------------------------------------------------------------------
  Structure to store throttle config values
---------------------------------------------------------------------------*/
ds_3gpp_pdn_throt_config_type *ds_3gpp_pdn_throt_config_p = NULL;


/*===========================================================================
FUNCTION       DS_3GPP_PDN_THROT_FILL_CONFIG_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the 
              structure with the appropriate data that corresponds to the 
              token number.   

PARAMETERS    token_id -  describes the current token which is 
                          being populated into the EFS structure
 
              Failure_value - Failure timer value being written to
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_fill_config_with_token_contents
(
  uint8                        token_id,
  uint32                       failure_value
)
{
  if(token_id >= DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS)
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Init to default values
  -------------------------------------------------------------------------*/

  if (ds_3gpp_pdn_throt_config_p == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(
               ds_3gpp_pdn_throt_config_p,sizeof(ds_3gpp_pdn_throt_config_type),
               ds_3gpp_pdn_throt_config_type *);

    if(ds_3gpp_pdn_throt_config_p != NULL)
    {
      memset((void*)ds_3gpp_pdn_throt_config_p,0,
             (sizeof(ds_3gpp_pdn_throt_config_type)));
    }
    else
    {
      return;
    }
  }

   ds_3gpp_pdn_throt_config_p->failure_timer[token_id] = 
        (uint32)failure_value;

  return;
} /*  ds_3gpp_pdn_throt_fill_config_with_token_contents */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_GET_FAILURE_TIMER

DESCRIPTION   This function provides the pdn throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index 

RETURN VALUE  Throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_pdn_throt_get_failure_timer 
(
   uint8                  counter
)
{
  uint32 return_timer_value =0;
  /*-----------------------------------------------------------------------*/

  if((counter > DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS) ||
     (counter < DS_3GPP_PDN_THROTTLE_MIN_FAILURE_TIMERS))
  {
    return return_timer_value;
  }

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;

  if(ds_3gpp_pdn_throt_config_p != NULL)
  {

    DS_MSG2(MSG_LEGACY_HIGH,"returning timer value :%d for failure_count %d",
                       ds_3gpp_pdn_throt_config_p->failure_timer[counter],
                       counter);

    return_timer_value = DS_SEC_TO_MSEC
              (ds_3gpp_pdn_throt_config_p->failure_timer[counter]);
  }
  
  return return_timer_value;
} /* ds_3gpp_pdn_throt_get_failure_timer */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_FILL_PEND_FAILURE_TIMER 

DESCRIPTION   This function is to assign pdn throttling failure timer value
              manually for 6th -10th failure timers when global throttling
              is not enabled

PARAMETERS    None

RETURN VALUE  none

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_fill_pend_failure_timer 
(
  void
)
{
  uint8 idx = 0;
 /*-----------------------------------------------------------------------*/
    /*-------------------------------------------------------------------------
   If global throttling not enabled populate 7-10 indexes with 6th failure 
   timer value. This is required, because the failure_cnt will be incremented
   till DS_PDN_THROTTLE_MAX_FAILURE_TIMERS even if global throttling is not 
   enabled.
  -------------------------------------------------------------------------*/
  if(ds_pdn_nv_manager_get_global_throttling_nv())
  {
    for(idx = DS_3GPP_PDN_THROTTLE_REF_FAILURE_TIMERS; 
        idx < DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS; idx++)
    {
      ds_3gpp_pdn_throt_config_p->failure_timer[idx] = 
                     ds_3gpp_pdn_throt_config_p->failure_timer[idx-1];
    }
  }
 
  for(idx = 0; idx < DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS; idx++)
  {
    DS_MSG2(MSG_LEGACY_HIGH,"pdn_throttle.failure_timer[%d]: %d",(idx+1),
                       ds_3gpp_pdn_throt_config_p->failure_timer[idx]);
  }

  return;
}/*ds_3gpp_pdn_throt_fill_pend_failure_timer*/

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_READ_CONFIG_FROM_EFS_PER_SUBS_ID

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the PDN throttle config structure.
              Only valid data is populated.

              EFS File Format - Param_Name:Param_Val;
              For example     - FAILURE_TIMER_1:0;
 
PARAMATERS   None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_pdn_throt_read_config_from_efs
(
  void
)
{
  ds_pdn_nv_efs_token_type  efs_db; /* structure used for tokenizing the file*/
  char *from, *to; /* ptrs to start and end of the token */
  ds_pdn_nv_efs_token_parse_status_type  ret_val 
                                              = DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS;
  boolean param_name = TRUE; /* flag to keep track of the parser state 
                                TRUE - parser expects Token Name in the EFS
                                FALSE - parser expects Token Val in the EFS*/
  uint8 token_id = 0;
  char file_name[] 
                = "/nv/item_files/modem/data/epc/pdn_throttling_config.txt";
  int atoi_result = 0;
/*---------------------------------------------------------------------------*/

  if( ds_pdn_nv_manager_efs_file_init(file_name, &efs_db) == -1)
  {
    return;
  }

  /* Set the seperator as : */
  efs_db.seperator = ':';
/*lint -save -esym(644,token_id) param_name boolean flag takes care of this */
  while (DS_PDN_NV_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ds_pdn_nv_manager_efs_tokenizer(&efs_db, &from, &to )))
  {
    if (DS_PDN_NV_EFS_TOKEN_PARSE_FAILURE == ret_val)
    {
      break;
    }
    /*------------------------------------------------------------------------
      Token being read. 'from' points to the beginning of the token and 
      'to' point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS_PDN_NV_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(DS_PDN_NV_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*--------------------------------------------------------------------- 
      check if we are looking for param name or param value 
      ---------------------------------------------------------------------*/
      if (param_name == TRUE)
      {
        /*------------------------------------------------------------------
        get the token identifier for this param name 
        ------------------------------------------------------------------*/
        if (ds_util_get_throttle_token_id(from,to,&token_id) < 0)
        {
          /* This is an error scenario, Skip till the end of the line? */
          DS_MSG0( MSG_LEGACY_ERROR, "Incorrect Param Name" );
        }
        else
        {
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a 
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db.seperator = ';';
        }
        
      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else 
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the PDN throttle 
         config structure
         ------------------------------------------------------------------*/
        if(DS_3GPP_PDN_THROTTLE_MAX_FAILURE_TIMERS >= token_id)
        {
          atoi_result = ds_pdn_apm_atoi(from,to);
          ds_3gpp_pdn_throt_fill_config_with_token_contents(token_id,
                                                            (uint32)atoi_result);
        }
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        param_name = TRUE;
        /* Set the seperator as : */
        efs_db.seperator = ':';
      }
    }
  } /* end of while loop */

  /*----------------------------------------------------------------------- 
    Fill failure timers 6-10 if necessary
    -----------------------------------------------------------------------*/
  ds_3gpp_pdn_throt_fill_pend_failure_timer();

/*lint -restore (644,token_id)*/
  ds_pdn_nv_manager_efs_file_close(&efs_db);
  return;
} /* ds_read_pdn_throttle_config_from_efs() */

/*===========================================================================
FUNCTION      DS_3GPP_PDN_THROT_GET_REMAINING_TIMER_FOR_ATTACH_PDN

DESCRIPTION   This function will return the remaining throttling timer for
              attach PDN
 
PARAMATERS    None

DEPENDENCIES  None

RETURN VALUE  timer value

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_pdn_throt_get_remaining_timer_for_attach_pdn()
{
  ds_profile_type_s                     profile = {{0}};
  ds_pdn_apm_lte_attach_pdn_list_type   attch_pdn_list = {0};
  uint32                                timer = 0;
  uint8                                 i = 0;
/*---------------------------------------------------------------------------*/
  if (ds_pdn_apm_get_attach_pdn_list(&attch_pdn_list))
  {
    for (i = 0; i < attch_pdn_list.attach_pdn_profile_list_num; i++)
    {
      if(ds_profile_number_is_valid(attch_pdn_list.attach_pdn_profile_list[i]))
      {
        memset(&profile,0,sizeof(ds_profile_type_s));
        if(ds_profile_setting_from_profile_id(attch_pdn_list.attach_pdn_profile_list[i], &profile) == DS_PROFILE_RESULT_SUCCESS)
        {
          timer  = MAX( timer , ds_3gpp_pdn_throt_sm_get_remaining_throttle_timer_val((byte*)&profile.context.apn, DS_3GPP_THROT_RAT_GLOBAL));
        }
      }
    }
  }
  DS_MSG1( MSG_LEGACY_HIGH, "MAX throttling timer for attach profile %d", timer );

  return timer;
}
