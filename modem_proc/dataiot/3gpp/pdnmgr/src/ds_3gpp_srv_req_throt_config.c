/*===========================================================================
                      DS_3GPP_SRV_REQ_THROT_CONFIG.C
DESCRIPTION
This file defines service request throttling config APIs.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/src/ds_3gpp_srv_req_throt_config.c#2 $
  $DateTime: 2020/04/22 03:56:17 $$Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd     First checkin
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "ds_pdn_defs.h"
#include "ds_3gpp_srv_req_throt_config.h"
#include "ds_pdn_nv_manager.h"
#include "ds_3gpp_throttle_sm.h"
#include "ds_pdn_apm_efs_parser.h"
#include "ds_3gpp_data_retry.h"
#include "ds_3gpp_srv_req_throt_sm.h"
#include "ds_3gpp_pdn_throttle_sm.h"



/*-------------------------------------------------------------------------- 
  These are used to indicate what is the timer read from the service request
  config txt
  0 -- Service request throttling time
  1 -- Service request min random time
  2 -- Service request max random time
  --------------------------------------------------------------------------*/
#define DS_3GPP_SRV_REQ_THROT_TIME   0
#define DS_3GPP_SRV_REQ_MIN_RAN_TIME 1
#define DS_3GPP_SRV_REQ_MAX_RAN_TIME 2


#define DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS 10 /* max number of srv req failure timers */
#define DS_3GPP_SRV_REQ_THROTTLE_MIN_FAILURE_TIMERS  1 /* min number of failure timers */
#define DS_3GPP_SRV_REQ_THROTTLE_REF_FAILURE_TIMERS  6 /* Fill the srv req failure timers from 6-10*/
/*-----------------------------------------------------------------------------
  Structure used to store the service req throttle config values from EFS
-----------------------------------------------------------------------------*/
typedef struct
{
  uint32 failure_timer[DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS];   /* Failure Timer  */
  uint32 rand_timer_min[DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS];  /* Random min timer*/
  uint32 rand_timer_max[DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS];  /* Random max timer*/
}ds_3gpp_srv_req_throt_config_type;

/*---------------------------------------------------------------------------
  Structure to store throttle config values
---------------------------------------------------------------------------*/
ds_3gpp_srv_req_throt_config_type  *ds_3gpp_srv_req_throt_config_p = {NULL};

/*===========================================================================
FUNCTION       DS_3GPP_SRV_REQ_THROT_FILL_CONFIG_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the 
              structure with the appropriate data that corresponds to the 
              token number.   

PARAMETERS    token_id -  describes the current token which is 
                          being populated into the EFS structure
 
              srv_req_timer_index -- Indicates if the timer value read is
                                     srv_req_throttle_timer or min/max random
                                     timer values
 
              failure_timer_value
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_fill_config_with_token_contents
(
  uint8                        token_id,
  uint8                        srv_req_timer_index,
  uint32                       failure_value
)
{
  if(token_id >= DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS)
  {
    return;
  }
  
  /*-------------------------------------------------------------------------
    Init to default values
  -------------------------------------------------------------------------*/

  if (ds_3gpp_srv_req_throt_config_p == NULL)
  {
    DS_SYSTEM_HEAP_MEM_ALLOC(ds_3gpp_srv_req_throt_config_p,
                             sizeof(ds_3gpp_srv_req_throt_config_type),
                             ds_3gpp_srv_req_throt_config_type*);

    if(ds_3gpp_srv_req_throt_config_p != NULL)
    {
      memset((void*)ds_3gpp_srv_req_throt_config_p,0,
             (sizeof(ds_3gpp_srv_req_throt_config_type)));
    }
    else
    {
      return;
    }
  }

  /*--------------------------------------------------------------------------- 
    Use the timer index to know if the time read is for service request
    throttling or service request random time
  ---------------------------------------------------------------------------*/
  switch (srv_req_timer_index)
  {
    case DS_3GPP_SRV_REQ_THROT_TIME:
      ds_3gpp_srv_req_throt_config_p->failure_timer[token_id] = 
                                                   (uint32)failure_value;
      break;

    case DS_3GPP_SRV_REQ_MIN_RAN_TIME:
      ds_3gpp_srv_req_throt_config_p->rand_timer_min[token_id] =
                                                   (uint32)failure_value;
      break;

    case DS_3GPP_SRV_REQ_MAX_RAN_TIME:
      ds_3gpp_srv_req_throt_config_p->rand_timer_max[token_id] = 
                                                   (uint32)failure_value;
      break;

    default:
      break;
  }

  return;
}/*ds_3gpp_srv_req_throt_fill_config_with_token_contents*/

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_GET_FAILURE_TIMER

DESCRIPTION   This function provides the srv req throttling failure timer value

PARAMETERS    counter - Throttling counter used as an index 

RETURN VALUE  Throttle Failure Timer value in msec

SIDE EFFECTS  None
===========================================================================*/
uint32 ds_3gpp_srv_req_throt_get_failure_timer 
(
   uint8                  counter
)
{
  uint32 return_timer_value =0;
  /*-----------------------------------------------------------------------*/

  if((counter > DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS) ||
     (counter < DS_3GPP_SRV_REQ_THROTTLE_MIN_FAILURE_TIMERS))
  {
    return return_timer_value;
  }

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;

  if(ds_3gpp_srv_req_throt_config_p != NULL)
  {
    return_timer_value = DS_SEC_TO_MSEC
                    (ds_3gpp_srv_req_throt_config_p->failure_timer[counter]);

    DS_MSG2(MSG_LEGACY_HIGH, "returning timer value :%d for failure_count %d",
            ds_3gpp_srv_req_throt_config_p->failure_timer[counter], counter);			  
  }
  
  return return_timer_value;
} /* ds_3gpp_srv_req_throt_get_failure_timer */

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_GET_RANDOM_TIMERS 

DESCRIPTION   This function provides the srv req throttle random timers

PARAMETERS 
              counter - Throttling counter used as an index
              uint32    *min_random_timer - min Random timer value will
                                            be populated   
              uint32    *max_random_timer - max Random timer value will
                                            be populated

RETURN VALUE  void

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_get_random_timers
(
   uint8                  counter,
   uint32                *min_random_timer_p,    
   uint32                *max_random_timer_p
)
{

  if((counter > DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS) ||
     (counter < DS_3GPP_SRV_REQ_THROTTLE_MIN_FAILURE_TIMERS) ||
     (min_random_timer_p == NULL) ||
     (max_random_timer_p == NULL))
  {
    return;
  }

  *min_random_timer_p = 0;
  *max_random_timer_p = 0;

  /*-------------------------------------------------------------------------
    Decrement the counter by 1 so that it can be used as an index into timer
    array also check for its validity
  -------------------------------------------------------------------------*/
  counter -= 1;

  if(ds_3gpp_srv_req_throt_config_p != NULL)
  {
    *min_random_timer_p = ds_3gpp_srv_req_throt_config_p->
                                      rand_timer_min[counter];
    *max_random_timer_p = ds_3gpp_srv_req_throt_config_p->
                                      rand_timer_max[counter];
  }
  
  return;
} /* ds_3gpp_srv_req_throt_get_random_timers() */

/*===========================================================================
FUNCTION DS_3GPP_SRV_REQ_THROT_GET_TIMER_CB

DESCRIPTION
  This function is used to get Service request throttling timer value

PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  Throttle Timer value

SIDE EFFECTS
  None.
===========================================================================*/
uint32 ds_3gpp_srv_req_throt_get_throt_timer_cb
(
  void* throt_sm_void_ptr,
  void* cb_data
)
{
  ds_3gppi_throttle_sm_s *throt_sm_ptr = 
                            (ds_3gppi_throttle_sm_s*)throt_sm_void_ptr;
  uint32                  throt_tmr_val = 0;
  /*--------------------------------------------------------------------------*/

  if (throt_sm_ptr == NULL)
  {
    return throt_tmr_val;
  }

  /*---------------------------------------------------------------------------- 
   For data retry, if max lower layer count has been reached it must be lower
   layer throttling(1 min + rand)
   Also reset the lower layer counter
    ----------------------------------------------------------------------------*/
   if((ds_3gpp_data_retry_is_enabled()) &&
      (throt_sm_ptr->lwr_lyr_failure_cnt >= DS_3GPP_THROT_MAX_LOWER_LAYER_CNT))
   {
  
     throt_tmr_val = ds_3gpp_srv_req_throt_get_failure_timer(
                                                DS_3GPP_PDN_THROT_MIN_FAILURE_CNT);
     throt_sm_ptr->lwr_lyr_failure_cnt = 0;
   }
   else
   {

  throt_tmr_val = ds_3gpp_srv_req_throt_get_failure_timer(
                                             throt_sm_ptr->failure_count);
   }
  
  /*-------------------------------------------------------------------------- 
   Get the random throttling timers.
   Also if min_ramdom_timer is non zero and max_random_timer is zero then
   Set the max_random_timer to min_random_timer
  --------------------------------------------------------------------------*/

  ds_3gpp_srv_req_throt_get_random_timers(throt_sm_ptr->failure_count,
                                          &throt_sm_ptr->min_random_timer_val,
                                          &throt_sm_ptr->max_random_timer_val);
  
  if(((throt_sm_ptr->max_random_timer_val == 0) && 
     (throt_sm_ptr->min_random_timer_val != 0)) || 
     (throt_sm_ptr->max_random_timer_val < throt_sm_ptr->min_random_timer_val))
  {
     throt_sm_ptr->max_random_timer_val = throt_sm_ptr->min_random_timer_val;
  }
  
  DS_MSG3(MSG_LEGACY_HIGH, 
          "Srv req throttle timer values in secs are: throt_timer_val: %d, "
          "rand_min: %d, rand_max: %d",
          throt_tmr_val, throt_sm_ptr->min_random_timer_val,
          throt_sm_ptr->max_random_timer_val);
  return throt_tmr_val;
} /* ds_3gpp_srv_req_throt_get_throt_timer_cb */

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_THROT_FILL_PEND_FAILURE_TIMER 

DESCRIPTION   This function is to assign pdn throttling failure timer value
              manually for 6th -10th failure timers when global throttling
              is not enabled

PARAMETERS    subs_id

RETURN VALUE  none

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_throt_fill_pend_failure_timer 
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
  if( (!ds_pdn_nv_manager_get_global_throttling_nv()) )
  {
    for(idx = DS_3GPP_SRV_REQ_THROTTLE_REF_FAILURE_TIMERS; 
        idx < DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS; idx++)
    {
      ds_3gpp_srv_req_throt_config_p->failure_timer[idx] = 
                     ds_3gpp_srv_req_throt_config_p->failure_timer[idx-1];
    }
  }
 
  for(idx = 0; idx < DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS; idx++)
  {
    DS_MSG2(MSG_LEGACY_HIGH, "srv_req_throttle.failure_timer[%d]: %d",
            (idx+1), ds_3gpp_srv_req_throt_config_p->failure_timer[idx]);
  }

  return;

}/*ds_3gpp_srv_req_throt_fill_pend_failure_timer*/

/*===========================================================================
FUNCTION      DS_3GPP_SRV_REQ_READ_THROTTLE_CONFIG_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary 
              data & fill the srv req throttle config structure. Only valid data is populated.

              EFS File Format - Param_Name:Failure_Value:Min_Random_Timer:MAx_Random_Timer;
              For example     - FAILURE_TIMER_1:0:2:3;
 
PARAMATERS   None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_srv_req_read_throttle_config_from_efs
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
  uint8  token_id = 0;
  uint8  srv_req_timer_index = 0; /*
                                   If 0 -- Srv req throttle value is read
                                      1 -- Min random timer value is read
                                      2 -- Max random timer value
                                  */

  char   file_name[] 
                = "/data/3gpp/throttling/srv_req_throt_config.txt";

  int    atoi_result = 0;
/*-------------------------------------------------------------------------*/

  if(ds_pdn_nv_manager_efs_file_init(file_name,&efs_db) == -1)
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
          DS_ERROR_LOG_0("Incorrect Param Name" );
        }
        else
        {
          DS_MSG1( MSG_LEGACY_HIGH, "Token Id: %d", token_id );
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a 
          param value
          -----------------------------------------------------------------*/
          param_name = FALSE;
          /* set the seperator as ; */
          efs_db.seperator = '*';
        }
      }
      /*---------------------------------------------------------------------
      This means that the token is a param value
      ---------------------------------------------------------------------*/
      else 
      {
        /*-------------------------------------------------------------------
         pass in the identifier and param value to fill the throttle 
         config structure
         ------------------------------------------------------------------*/
         if(DS_3GPP_SRV_REQ_THROTTLE_MAX_FAILURE_TIMERS >= token_id)
         {
           atoi_result = ds_pdn_apm_atoi(from,to);

           ds_3gpp_srv_req_throt_fill_config_with_token_contents(token_id,
                                                                 srv_req_timer_index,
                                                                 (uint32)atoi_result);
         }
        /*-------------------------------------------------------------------
         set param_name as TRUE This means that next token is a param name
        -------------------------------------------------------------------*/
        if(efs_db.seperator == ';')
        {
          param_name = TRUE;
          /* Set the seperator as : */
          efs_db.seperator = ':';
          srv_req_timer_index =  0;
        }
        else
        {
          efs_db.seperator = '*';
          srv_req_timer_index++;
        }
      }
    }
  } /* end of while loop */

 /*----------------------------------------------------------------------- 
    Fill failure timers 6-10 if necessary
 -----------------------------------------------------------------------*/
  ds_3gpp_srv_req_throt_fill_pend_failure_timer();

/*lint -restore (644,token_id)*/
  ds_pdn_nv_manager_efs_file_close(&efs_db);
  return;
} /* ds_3gpp_srv_req_read_throttle_config_from_efs */

