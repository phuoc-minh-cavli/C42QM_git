/*===========================================================================
                      DS_PDN_RATE_CTRL_COMMON.H
  DESCRIPTION
  This file defines common rate control APIs.

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE

when           who    what, where, why
--------    ---    ----------------------------------------------------------
10/01/19       aks    First checkin
===========================================================================*/


#ifndef DS_PDN_RATE_CTRL_COMMON_H
#define DS_PDN_RATE_CTRL_COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define PDN_RATE_CTRL_VALID_COOKIE       0x123a123a

/*---------------------------------------------------------------------------
  This structure is used to read the NV item(NV:rate control interval factor).
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{ 
  uint8   version;             /* version of the NV item*/
  uint8   apn_rc_intrvl_factor; /* NV item value */
  uint8   splmn_rc_intrvl_factor; /* NV item value */
  uint8   reserve_bytes[2];    /* reserve bytes for future use*/
}ds_pdn_rate_ctrl_interval_factor_s;

#define DS_PDN_RATE_CTRL_DEFAULT_INTERVAL 4
/*-----------------------------------------------------------------------
  This enumeration will indicate the kind of rate control being applied
  (APN Rate Control or Serving PLMN Rate Control)
------------------------------------------------------------------------*/
typedef enum
{
  DS_PDN_RATE_CTRL_TYPE_MIN = 0,
  DS_PDN_RATE_CTRL_TYPE_SPLMN = DS_PDN_RATE_CTRL_TYPE_MIN,
  DS_PDN_RATE_CTRL_TYPE_APN,
  DS_PDN_RATE_CTRL_TYPE_ADDITONAL_APN,
  DS_PDN_RATE_CTRL_TYPE_MAX
} ds_pdn_rate_ctrl_type;

/*-----------------------------------------------------------------------
  This struct holds all common information needed for handling of rate
  control algorithms.
------------------------------------------------------------------------*/
typedef struct
{
  uint32                           cookie;
  /* Cookie to validate memory */
  ds_pdn_rate_ctrl_type            rate_ctrl_type;
  /* Rate Control Type i.e. APN, SPLMN */
  int                               index;
  /* Depending on the rate control type, index will
     be the relative index. i.e. apn index for apn rate control or
     bearer index for serving plmn rate control */
  ps_timer_handle_type              interval_timer;
  /* Sliding Window rate control interval for more accurate
     data packet tracking */
  int                               timer_val;
  /* timer value for each rate control algorithm */
  int                               interval_timer_val;
  /* timer value for each interval */
  int                               threshold;
  /* max data packet threshold value */
  boolean                           threshold_exceeded;
  /* Determine whether rate control threshold has been exceeded or not */
  int                               current_interval_index;
  /* Keeping track of the current data pkt counter index (interval index) */
  int                               data_pkt_counter_q_max_size;
  /* Pre-determined number of intervals from NV item configuration */
  uint32                            *data_pkt_counter_q_ptr;
  /* Queue to keep track of data packets enqueued in a given time period
     made up of rate_ctrl_intervals.  This field MUST remain the last
     field in this struct for the dynamic size to work since q_max_size
     is not determined at compile time but read from NV. */
} ds_pdn_rate_ctrl_info_s;

/*------------------------------------------------------------------------------
  Function pointer type for rate control timer start/stop callbacks 
------------------------------------------------------------------------------*/
typedef void (*ds_pdn_rate_ctrl_timer_ev_cb_f) (ds_pdn_rate_ctrl_info_s* rate_ctrl_ptr);

/*---------------------------------------------------------------------------------
  This structure maintains information specific to each Rate Control Type.
  tmr_exp_f:                 Function to be invoked when rate control timer expires
-----------------------------------------------------------------------------------*/
typedef struct
{
  ds_pdn_rate_ctrl_timer_ev_cb_f     tmr_exp_f;                                   
} ds_pdn_rate_ctrl_cb_tbl_s;

/*------------------------------------------------------------------------------
  This Macro validates the rate control type value
------------------------------------------------------------------------------*/
#define DS_PDN_RATE_CTRL_VALIDATE_RATE_CTRL_TYPE(rate_ctrl_type) \
          (rate_ctrl_type >= DS_PDN_RATE_CTRL_TYPE_MAX || \
		       rate_ctrl_type < DS_PDN_RATE_CTRL_TYPE_MIN)? FALSE: TRUE

/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION DS_PDN_RATE_CTRL_REGISTER_CB_INFO_TBL

DESCRIPTION
  This function is used by various rate control modules to register
  their respective info tables, which specify attributes such as the
  timer expiry callbacks, etc. 

PARAMETERS   :
  cb_info_tbl_p     - Pointer to the callback info table
  rate_ctrl_type    - Type of rate control algorithm

DEPENDENCIES
  None.

RETURN VALUE
  uint32.

SIDE EFFECTS
  None.
===========================================================================*/
void ds_pdn_rate_ctrl_register_cb_info_tbl
(
  ds_pdn_rate_ctrl_cb_tbl_s    *cb_info_tbl_p,
  ds_pdn_rate_ctrl_type         rate_ctrl_type
);

/*=========================================================================
  FUNCTION DS_PDN_RATE_CTRL_ALLOCATE_RATE_CTRL_CONTEXT

  DESCRIPTION
    This function initializes the rate control init information properly.
  
  PARAMETERS
                     
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    Boolean value for success or failure
  
  SIDE EFFECTS 
    None.
  
===========================================================================*/
boolean ds_pdn_rate_ctrl_allocate_rate_ctrl_context
(
  ds_pdn_rate_ctrl_info_s   *rate_ctrl_ptr,
  ds_pdn_rate_ctrl_type      current_rate_ctrl_type,
  int                        index,
  uint32                     timer_val,
  int                        threshold
);

/*===========================================================================
FUNCTION DS_PDN_RATE_CTRL_INCREMENT_COUNTER

DESCRIPTION
  This function increments the rate control 
  
PARAMETERS 
  
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_rate_ctrl_increment_counter
(
   ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*===========================================================================
FUNCTION DS_PDN_RATE_CTRL_CLEAR_INTERVAL_TIMER

DESCRIPTION
  This function clears the interval timer for a given interval
  for a given rate control instance.
  
PARAMETERS  
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_rate_ctrl_clear_interval_timer
(
  ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*===========================================================================
FUNCTION DS_PDN_RATE_CTRL_EVALUATE_THRESHOLD_EXCEEDED

DESCRIPTION
  This function evaluates whether the threshold is exceeded for a given
  Rate Control Instance by looking at the last N interval blocks.
  
PARAMETERS 
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean ds_pdn_rate_ctrl_evaluate_threshold_exceeded
(
  ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*===========================================================================
FUNCTION DS_PDN_RATE_CTRL_GET_CURRENT_DATA_PKT_COUNT

DESCRIPTION
  This function evaluates whether the threshold is exceeded for a given
  Rate Control Instance by looking at the last N interval blocks.
  
PARAMETERS 
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
uint32 ds_pdn_rate_ctrl_get_current_data_pkt_count
(
  ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*===========================================================================
FUNCTION ds_pdn_rate_ctrl_free_rate_ctrl_context

DESCRIPTION
  This function frees dyn memory allocated for rate control info.
 
PARAMETERS
  Rate control pointer - rate control context that needs to be freed
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Non null
  FALSE- Otherwise

SIDE EFFECTS
  None

=============================================================================*/
boolean ds_pdn_rate_ctrl_free_rate_ctrl_context
(
  ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*===========================================================================
FUNCTION ds_pdn_apn_rate_ctrl_reset_timer_fields

DESCRIPTION
  This function resets all parameters related to the APN Rate control
  timer - used during initialization and timer expiry.
  
PARAMETERS  
    APN Rate Control info pointer
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_rate_ctrl_reset_timer_fields
(
   ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*=============================================================================
  FUNCTION DS_PDN_RATE_CTRL_IS_INTERVAL_TIMER_RUNNING

  DESCRIPTION
    This function checks to see if the instance of rate control interval
    timer is running
  
  PARAMETERS   
    None.
 
  DEPENDENCIES
    None.
  
  RETURN VALUE 
    None.
  
  SIDE EFFECTS 
    None.
  
=============================================================================*/
boolean ds_pdn_rate_ctrl_is_interval_timer_running
(
  ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

/*===========================================================================
FUNCTION DS_PDN_RATE_CTRL_START_INTERVAL_TIMER

DESCRIPTION
  This function starts the interval timer for a given rate control algorithm
  
PARAMETERS  
 
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_pdn_rate_ctrl_start_interval_timer
(
  ds_pdn_rate_ctrl_info_s *rate_ctrl_ptr
);

#endif /* DS_PDN_RATE_CTRL_COMMON_H */
