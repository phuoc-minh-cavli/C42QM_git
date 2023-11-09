/*===========================================================================
                      DS_FAST_DORMANCY.C
DESCRIPTION
This file defines DS Fast dormancy APIs.

Copyright (c) 2018 - 2020 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:  $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

#include "datamodem_variation.h"

#ifdef FEATURE_FAST_DORMANCY

#include "ds_pdn_cp_data_hdlr.h"
#include "ds_fast_dormancy.h"
#include "ds_pdn_nv_manager.h"
#include "lte_as.h"
#include "nb1_as.h"
#include "lte_static_l2_ext_api.h"

#define FAST_DORMANCY_TIMER_INTERVAL 500

/*===========================================================================

                             DECLARATIONS

===========================================================================*/


/* ---------------------------------------------------------------------------------------------------------
   Structure to hold Fast Dormancy information 
   l2_rai_enabled:                Tells us whether L2 RAI feature support enabled or not
   interval_cnt:                  For precision, Timer been split into N intervals each of 500ms duration
                                  Tells us the number of consecutive timer intervals passed in which there 
                                  is no data activity
   max_intervals_tn:              Tells us the max number of consecutive timer intervals of no data activity
                                  to consider for triggering NAS signalling based fast dormancy.
   timer_hdl:                     Holds fast dormancy inactivity timer handle
   um_stats:                      structure to hold/cache statistics for all UMTS Instances required to
                                  check data activity for Fast Dormancy implementation
   --------------------------------------------------------------------------------------------------------*/
typedef struct 
{
  boolean                         l2_rai_enabled;
  uint8                           interval_cnt;
  uint8                           max_intervals_tn;
#ifdef FEATURE_NBIOT_NTN
  uint8                           max_intervals_ntn;
#endif  
  ps_timer_handle_type            timer_hdl;
  ds_stats_type                   um_stats[DS_FWK_MAX_INST][DS_FWK_IP_TYPE_MAX];
} ds_fast_dormancy_info_s;

static ds_fast_dormancy_info_s fast_dormancy_info = {0};


/*===========================================================================

                                      FUNCTIONS

===========================================================================*/

/*===============================================================================
FUNCTION DS_FAST_DORMANCY_INIT()

DESCRIPTION
 This function 
 1) Registers for L2 RAI UMID,
 2) Allocates PS timer,
 3) L2 RAI feature support as enabled,
 4) will read timer config from NV and calculates max number of consecutive
   timer intervals of no data activity to consider for triggering NAS signalling
   based fast dormancy.

PARAMETERS
 None

RETURN VALUE
 None

DEPENDENCIES
 None

SIDE EFFECTS
 None
===============================================================================*/
void ds_fast_dormancy_init
(
  void
)
{
  ds_pdn_fd_timer_config						  fd_config;
  /*------------------------------------------------------------*/

  /*--------------------------------------------------------------------------
      Register for L2 UMID for RAI IND
  --------------------------------------------------------------------------*/
  /* catM */
  dsmsgrrcv_reg_hdlr(LTE_MAC_RAI_INDI,
                     (dsmsgrcv_msg_hdlr_f)
                     ds_fast_dormancy_l2_rai_status_ind);

  /* NB1 */
  dsmsgrrcv_reg_hdlr(NB1_MAC_RAI_INDI,
                     (dsmsgrcv_msg_hdlr_f)
                     ds_fast_dormancy_l2_rai_status_ind);


  /*------------------------------------------------
    Define/Allocate ps timers for Fast dormancy 
    check if timers are already allocated. Allocate 
    if timer handle is not valid 
    -----------------------------------------------*/
  if(fast_dormancy_info.timer_hdl  == PS_TIMER_INVALID_HANDLE)
  {
    fast_dormancy_info.timer_hdl = ps_timer_alloc(ds_fast_dormancy_timer_expiry_cb, 
							  NULL);
  }

  /* -----------------------------------------------------------------
    L2 RAI feature supported enabled by default. L2 notifies if there
    is a change in L2 RAI feature support through UMID
  -------------------------------------------------------------------*/
  fast_dormancy_info.l2_rai_enabled = TRUE;
  
  /* ------------------------------------------------------------------
    Get the NV Fast dormancy timer configuration,
    For precision, Timer will be split into N intervals of 500ms each.
	calculate max intervals(N) and update the cache
  --------------------------------------------------------------------*/
  fd_config = ds_pdn_nv_manager_get_fd_info();

  /* TN */
  fast_dormancy_info.max_intervals_tn = (fd_config.tn_timer * 1000)/FAST_DORMANCY_TIMER_INTERVAL;
  
#ifdef FEATURE_NBIOT_NTN
  /* NTN */
  fast_dormancy_info.max_intervals_ntn = (fd_config.ntn_timer * 1000)/FAST_DORMANCY_TIMER_INTERVAL;
#endif/* FEATURE_NBIOT_NTN */
}/* ds_fast_dormancy_init */


/*===========================================================================
FUNCTION DS_FAST_DORMANCY_L2_RAI_STATUS_IND
 
DESCRIPTION
   This function is used to handle L2 RAI UMID and
   update the DS3G L2 RAI local cache
 
PARAMETERS
  L2 UMID
  payload
 
DEPENDENCIES
  None.
 
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_fast_dormancy_l2_rai_status_ind
(
  msgr_umid_type               msgr_type,
  const msgr_hdr_struct_type   *dsmsg_ptr
)
{
  boolean l2_rai_enabled   = TRUE;
/*--------------------------------------------------------------------------------*/

  do
  {
    if (dsmsg_ptr == NULL ||
	    (!((msgr_type == LTE_MAC_RAI_INDI) ||
	       (msgr_type == NB1_MAC_RAI_INDI))))
    {
      return;
    }

    /* Update L2 RAI enable support */
    switch (msgr_type)
    {    
      case LTE_MAC_RAI_INDI:	  	
	    l2_rai_enabled = ((lte_mac_rai_ind_s *)dsmsg_ptr)->rai_enabled;
		break;
	  
	  case NB1_MAC_RAI_INDI:
        l2_rai_enabled = ((nb1_mac_rai_ind_s *)dsmsg_ptr)->rai_enabled;
		break;
	  
	  default:
        break;
    }

	/*-------------------------------------------------------------
     if L2 RAI enabled, stop the fast dormancy timer if running
	---------------------------------------------------------------*/
	
    if (fast_dormancy_info.l2_rai_enabled != l2_rai_enabled)
    {
      fast_dormancy_info.l2_rai_enabled = l2_rai_enabled;
	  
	  if (fast_dormancy_info.l2_rai_enabled)
	  {
	    if (ds_fast_dormancy_is_timer_running())
	    {
          ds_fast_dormancy_stop_timer();
	    }
	  }
	}
  }while(0);

  DS_MSG1(MSG_LEGACY_HIGH,"L2 RAI capability %d",
                             fast_dormancy_info.l2_rai_enabled);
}/*ds_fast_dormancy_l2_rai_status_ind*/


/*===========================================================================
FUNCTION DS_FAST_DORMANCY_GET_L2_RAI_CAPABILITY()

DESCRIPTION
 This function returns L2 RAI capability 

PARAMETERS
 None

RETURN VALUE
 L2 RAI capability

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
boolean ds_fast_dormancy_get_l2_rai_capability
(
  void
)
{
  return fast_dormancy_info.l2_rai_enabled;
}/* ds_fast_dormancy_get_l2_rai_capability */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_UPDATE_3GPP_INSTANCE_STATS()

DESCRIPTION
 This function caches 3GPP Instance UM Stats

PARAMETERS
 None

RETURN VALUE
 None

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
static 
void ds_fast_dormancy_update_instance_stats
(
  void
)
{
  uint8					  inst;
  ds_fwk_s_type			  *fw_inst_p  = NULL;
  ds_pdn_mgr_s			  *pdn_mgr_p = NULL;  
  /*--------------------------------------------*/

  /* ----------------------------------------------------
   Loop through all valid Instances, cache the um stats
   only if the instance is in UP state
  -------------------------------------------------------*/
  
  for (inst = 0; inst < DS_FWK_MAX_INST; inst++)
  {  
	fw_inst_p = ds_fwk_get_inst_ptr_from_index(inst);
	
    if (fw_inst_p != NULL)
	{
	  pdn_mgr_p = ds_pdn_mgr_get_pdn_from_fwk_index(inst);
	  if (ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
	  {
		/* Cache stats of v4 Instance */
		if ((pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] != NULL) &&
		  	 (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4]->state == DS_INSTANCE_STATE_UP)) 	   
		{
		  fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_tx = 
		    fw_inst_p->um_stats[DS_FWK_IP_TYPE_V4].bytes_tx;

		  fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_rx =		   	
		    fw_inst_p->um_stats[DS_FWK_IP_TYPE_V4].bytes_rx;
			
     	  DS_MSG3(MSG_LEGACY_LOW, "V4 Stats Instance %d bytes_tx 0x%x bytes_rx 0x%x",
		  	       inst, 
		  	       fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_tx,
		  	       fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_rx);
		}
  
		/* Cache stats of v6 Instance */
		if ((pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6] != NULL) &&
		  	 (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6]->state == DS_INSTANCE_STATE_UP)) 	   
		  
		{
		  fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_tx = 
		    fw_inst_p->um_stats[DS_FWK_IP_TYPE_V6].bytes_tx;

		  fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_rx =		   	
			fw_inst_p->um_stats[DS_FWK_IP_TYPE_V6].bytes_rx;

			
     	  DS_MSG3(MSG_LEGACY_LOW, "V6 Stats Instance %d bytes_tx 0x%x bytes_rx 0x%x",
		  	       inst, 
		  	       fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_tx,
		  	       fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_rx);
		}
	  }
	}
  }
}/* ds_fast_dormancy_update_instance_stats */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_RESET_INVTERVAL_COUNT_AND_UPDATE_INSTANCE_STATS()

DESCRIPTION
 This function reset interval count and updates 3GPP Instance UM Stats

PARAMETERS
 None

RETURN VALUE
 None

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/

void ds_fast_dormancy_reset_interval_count_and_update_instance_stats
(
  void
)
{
  /* Reset interval count */
  fast_dormancy_info.interval_cnt = 0;

  /* Update Instance stats */
  ds_fast_dormancy_update_instance_stats();
}

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_CHECK_DATA_ACTIVITY()

DESCRIPTION
 This function checks if there is any data activity in DL/UL by
 comparing cached stats to current instance statistics.

PARAMETERS
 None

RETURN VALUE
 TRUE  - Activity is present
 FALSE - No Data activity

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
boolean ds_fast_dormancy_check_data_activity
(
  void
)
{
  uint8					  inst;
  ds_fwk_s_type			  *fw_inst_p  = NULL;
  uint64				  bytes_tx	  = 0;
  uint64				  bytes_rx	  = 0;	
  ds_pdn_mgr_s			  *pdn_mgr_p  = NULL;	
  /*------------------------------------------------*/
	
  for (inst = 0; inst < DS_FWK_MAX_INST; inst++)
  {  
	fw_inst_p = ds_fwk_get_inst_ptr_from_index(inst);
	
    if (fw_inst_p != NULL)
	{
	  pdn_mgr_p = ds_pdn_mgr_get_pdn_from_fwk_index(inst);
	  if (ds_pdn_mgr_validate_pdn_ptr(pdn_mgr_p))
	  {	   
        /* Compare v4 cache UM stats and current UM stats */
		if ((pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4] != NULL) &&
		     (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV4]->state == DS_INSTANCE_STATE_UP)) 	   
		{
		  bytes_tx = fw_inst_p->um_stats[DS_FWK_IP_TYPE_V4].bytes_tx; 		
	      bytes_rx = fw_inst_p->um_stats[DS_FWK_IP_TYPE_V4].bytes_rx;

		  DS_MSG5(MSG_LEGACY_LOW, "V4 Stats CMP Instance %d bytes_tx(cached) 0x%x "
		  	        "bytes_tx(current) 0x%x bytes_rx(cached) 0x%x bytes_rx(current) 0x%x",
		  	        inst,
		  	        fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_tx,		  	        
		  	        bytes_tx,
		  	        fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_rx,
		  	        bytes_rx);
		  
		  if (((bytes_tx > 0) &&
			    (bytes_tx != fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_tx)) ||
				((bytes_rx > 0) &&
				 (bytes_rx != fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V4].bytes_rx)))
		  {
		    return TRUE;
		  }
		}
        
        /* Compare v6 cache UM stats and current UM stats */
		if ((pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6] != NULL) &&
		    (pdn_mgr_p->inst_ptr[DS_PDN_INSTANCE_IPV6]->state == DS_INSTANCE_STATE_UP)) 	   
		{
		  bytes_tx = fw_inst_p->um_stats[DS_FWK_IP_TYPE_V6].bytes_tx; 		
		  bytes_rx = fw_inst_p->um_stats[DS_FWK_IP_TYPE_V6].bytes_rx;
		  
		  DS_MSG5(MSG_LEGACY_LOW, "V6 Stats CMP Instance %d bytes_tx(cached) 0x%x "
		  	        "bytes_tx(current) 0x%x bytes_rx(cached) 0x%x bytes_rx(current) 0x%x",
		  	        inst,
		  	        fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_tx,		  	        
		  	        bytes_tx,
		  	        fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_rx,
		  	        bytes_rx);
		  
		  if (((bytes_tx > 0) &&
			  (bytes_tx != fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_tx)) ||
			  ((bytes_rx > 0) &&
			  (bytes_rx != fast_dormancy_info.um_stats[inst][DS_FWK_IP_TYPE_V6].bytes_rx)))
		  {
			return TRUE;
		  }
		}
	  }
    }
  }
  
  return FALSE;
}/* ds_fast_dormancy_check_data_activity */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_START_TIMER()

DESCRIPTION
 This function starts Inactivity timer.

PARAMETERS
 None.

RETURN VALUE
 None

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
void ds_fast_dormancy_start_timer
(
  void
)
{
  /* Start the timer of 500 ms */	
  ps_timer_start(fast_dormancy_info.timer_hdl, FAST_DORMANCY_TIMER_INTERVAL);

} /* ds_fast_dormancy_start_timer */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_STOP_TIMER()

DESCRIPTION
 This function stops Inactivity timer. 

PARAMETERS
 None.

RETURN VALUE
 None

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
void ds_fast_dormancy_stop_timer
(
  void
)
{
  ps_timer_cancel(fast_dormancy_info.timer_hdl);
} /* ds_fast_dormancy_stop_timer */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_IS_TIMER_RUNNING()

DESCRIPTION
 This function gives information on whether timer is running or not currently

PARAMETERS
 None

RETURN VALUE
 TRUE -  If running
 FALSE - if Not

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
boolean ds_fast_dormancy_is_timer_running
(
  void
)
{
  return ps_timer_is_running(fast_dormancy_info.timer_hdl);
} /* ds_fast_dormancy_is_timer_running */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_CRITERIA_CHECK() 

DESCRIPTION
  This function checks fast dormancy criteria
  1. checks for data activity,
  2. pending data in any bearer,
  3. pending data in L2 buffer,
  In case of CP CIOT,
  4. check no of retries reached max,
  5. check is there any MO exception data

  CASE#1:
  ------
  For each timer interval, if none of the above condition passes, which
  means there is no data activity, will increment the interval count.
  If counter reaches max number of timer intervals, notify NAS to trigger TAU
  else start timer with interval count set to 0.

  CASE#2:
  ------
  If none of the above condition passes, if this fast dormancy criteria check
  is triggered because of RAI=1, then ignore checking interval count reached
  max intervals or not. notify NAS to trigger TAU.
  

PARAMETERS
  boolean - indicates whether interval_cnt can be ignored or not.

RETURN VALUE
  None
  
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds_fast_dormancy_criteria_check
(
  boolean ignore_interval_count
)
{
  boolean 		   trigger_fd	   = FALSE;  
  uint8			   index		   = 0;
  boolean          is_fd_triggered = FALSE;
  /* This varies per Network type */
  uint8            max_intervals   = 0;
  /*-----------------------------------------*/

  if ((FALSE == ds_fast_dormancy_check_data_activity()) &&
  	  (FALSE == ds_bearer_mgr_is_data_pending()) && 
      (FALSE == lte_nb1_l2_get_ul_buffer_status()))
  {
    trigger_fd = TRUE;
  }

  /* This is applicable, only when CP data is enabled */
  if (ds_pdn_cp_data_is_enabled())
  {
    if (ds_pdn_cp_data_is_retry_count_reached_max())
    {			
	  for (index = 0; index < DS_3GPP_MAX_BEARER_CONTEXT; index++)
	  {
        if (ds_pdn_cp_data_pending_mo_ex_data_avail(index))
        {
          trigger_fd = FALSE;
	      break;
        }
	  }
    }
	else
	{
	  trigger_fd = FALSE;
	}
  }

  /* --------------------------------------------------------------------
    All the fast dormancy criteria conditions passed,
    check for interval count reached max intervals or not. Based on this,
    trigger fast dormancy 
   --------------------------------------------------------------------*/
  if (trigger_fd)
  {
    /* default */
    max_intervals = fast_dormancy_info.max_intervals_tn - 1;

    /*---------------------------------------------------------------
  	  check network type is non-terrestrial, overwrite the existing
  	  value
  	 --------------------------------------------------------------*/
#ifdef FEATURE_NBIOT_NTN
	if (dssysmgr_get_nb1_ntn_rat_status())
	{
  	  max_intervals = fast_dormancy_info.max_intervals_ntn - 1;
	}
#endif /* FEATURE_NBIOT_NTN */

    /* Ignore interval count only applicable in a scenario where we receive RAI as 1 */
    if ((TRUE == ignore_interval_count) ||
	   (fast_dormancy_info.interval_cnt == max_intervals))
    {
      DS_MSG0(MSG_LEGACY_LOW, "Invoking NAS API to trigger TAU");

      /* Notify NAS to trigger TAU */
 	  emm_chk_fast_dormancy_start_possible(SYS_MODEM_AS_ID_1);

	  /* -----------------------------------------------------------------
	     Since DS notified NAS, set triggered to TRUE.
	     This flag helps to decide whether timer can be started or not 
	  -----------------------------------------------------------------*/
      is_fd_triggered = TRUE;
		  
      /* Since fast dormancy is triggered, reset counter to 0 */
	  fast_dormancy_info.interval_cnt = 0;	
    }
	else if (fast_dormancy_info.interval_cnt < max_intervals)
	{
	  /* If fast dormancy criteria met, increment the counter if its less than max */
	  fast_dormancy_info.interval_cnt ++;
	}
  }
  else
  {
    /* --------------------------------------------------
      Since there is some data activity , fast dormancy
      cannot be triggered, reset the counter 
    ---------------------------------------------------*/
    fast_dormancy_info.interval_cnt = 0;
  }

  /* ------------------------------------------------------
    If fast dormancy not triggered, update the stats 
    and start the timer of 500 ms
  -------------------------------------------------------*/
  if (FALSE == is_fd_triggered)
  {  
    ds_fast_dormancy_update_instance_stats();
    ds_fast_dormancy_start_timer();
  }

  DS_MSG5(MSG_LEGACY_HIGH, "criteria satisifed %d is_triggered %d current interval %d "
            "max intervals %d ignore_interval %d",
            trigger_fd,
            is_fd_triggered,
            fast_dormancy_info.interval_cnt,
            max_intervals,
            ignore_interval_count);
   
}/* ds_fast_dormancy_criteria_check */

/*===========================================================================
FUNCTION DS_FAST_DORMANCY_TIMER_EXPIRY_CB() 

DESCRIPTION
 This function gets invoked on Fast dormancy timer expiry. This inturn invokes
 fast dormancy criteria evaluation API.

PARAMETERS
 CB data

RETURN VALUE
 None

DEPENDENCIES
 None

SIDE EFFECTS
 None
===========================================================================*/
void ds_fast_dormancy_timer_expiry_cb
(
  void *cb_data
)
{
  ds_fast_dormancy_criteria_check(FALSE);	
} /* ds_fast_dormancy_timer_expiry_cb */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */
#endif /* FEATURE_FAST_DORMANCY */

