/*===========================================================================
                      DS_FAST_DORMANCY.H
DESCRIPTION
This file exposes DS Fast dormancy through APIs and data structures.

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

#ifdef FEATURE_FAST_DORMANCY

#ifndef DS_FAST_DORMANCY_H
#define DS_FAST_DORMANCY_H

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           LOCAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#define DS_FAST_DORMANCY_DEFAULT_TIMER_VALUE_TN     2

#define DS_FAST_DORMANCY_DEFAULT_TIMER_VALUE_NTN    60

/* --------------------------------------------------------------------
   Structure to store fast dormancy Inactivity timer value in seconds 
   tn_timer:       timer value for TN
   ntn_timer:      timer value for NTN 
 ----------------------------------------------------------------------*/
 
typedef struct
{
  uint32 tn_timer;
  uint32 ntn_timer;
} ds_pdn_fd_timer_config; 


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
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

===========================================================================*/
void ds_fast_dormancy_init
(
  void
);


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
  msgr_umid_type,
  const msgr_hdr_struct_type *
);

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
boolean ds_fast_dormancy_get_l2_rai_capability(void);

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
);

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
);

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
);

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
void ds_fast_dormancy_stop_timer(void);

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
);


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
  boolean
);

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
  void *
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* DS_FAST_DORMANCY_H */
#endif /* FEATURE_FAST_DORMANCY */

