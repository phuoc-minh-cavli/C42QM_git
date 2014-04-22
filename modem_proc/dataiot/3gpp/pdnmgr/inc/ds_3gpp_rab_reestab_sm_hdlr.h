 /*!
  @file
  ds_3gpp_rab_reestab_sm_hdlr.h

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009-2017 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/dataiot.mpss/2.0/3gpp/pdnmgr/inc/ds_3gpp_rab_reestab_sm_hdlr.h#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
3/20/17    vb        RAB restablishment handling

===========================================================================*/
#include "ds_bearer_manager.h"
#include "lte_rrc_ext_msg.h"


#if defined TEST_FRAMEWORK 
#error code not present
#else
#define DS_3GPP_RAB_GUARD_TIMER_INTERVAL 5000
#endif 
/*-------------------------------------------------------------------------- 
  RAB RESTABLISHMENT handling state
 --------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_RAB_REQ_NOT_PENDING_STATE   = 0,
  DS_3GPP_RAB_REQ_PENDING_STATE       = 1,
  DS_3GPP_RAB_REJ_RAND_TMR_STATE      = 2,
  DS_3GPP_RAB_REJ_RRC_BARRING_STATE   = 3,
  DS_3GPP_RAB_REJ_THROTTLED_STATE     = 4,
  DS_3GPP_RAB_MAX_STATE
}ds_3gpp_rab_state_e;

/*-------------------------------------------------------------------------- 
  Current Rab procedure that is on going
 --------------------------------------------------------------------------*/
typedef enum
{
  DS_3GPP_PROC_RAB_REESTAB_REQ                   = 0,
  DS_3GPP_PROC_RAB_REESTAB_RAND_RETRY            = 1,
  DS_3GPP_PROC_RAB_REESTAB_REJ_DEPRIOR           = 2,
  DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_START_IND = 3,
  DS_3GPP_PROC_RAB_REESTAB_RRC_BARRING_STOP_IND  = 4,
  DS_3GPP_PROC_RAB_REESTAB_RAND_TIMER_EXP        = 5,
  DS_3GPP_PROC_RAB_REESTAB_IND                   = 6,
  DS_3GPP_PROC_RAB_REESTAB_THROTTLED             = 7,
  DS_3GPP_PROC_RAB_MAX         
}ds_3gpp_rab_proc_e;


/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_GET_STATE

DESCRIPTION
   Get the current rab reestablish state
 
PARAMETERS
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
ds_3gpp_rab_state_e ds_3gpp_rab_reestab_get_state();

/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_TRANSITION_STATE

DESCRIPTION
   Perform rab reestab state transition 
 
PARAMETERS 
   Rab procedure that is undergoing
   user data - Timer value
   subs_id
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_rab_reestab_transition_state_ev
(
  ds_3gpp_rab_proc_e     rab_proc,
  void                  *user_data
);


/*===========================================================================
  FUNCTION DS_3GPP_PERFORM_RRC_BARRING

  DESCRIPTION
  This function processes rrc barring start stop commands
  
  PARAMETERS
  Deprior status - RRC barring start/stop
  subs_id
  
  DEPENDENCIES 
  None.
  
  RETURN VALUE 
  None.
   
  SIDE EFFECTS 
  None.
  
===========================================================================*/
void ds_3gpp_perform_rrc_barring
(
  lte_rrc_cell_deprior_sts_e    deprior_status
);
/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_SM_HDLR_INIT

DESCRIPTION
 Init rab reestab sm hdlr

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void ds_3gpp_rab_reestab_sm_hdlr_init
(
   void 
);

/*===========================================================================
FUNCTION DS_3GPP_RAB_REESTAB_RESET_STATE

DESCRIPTION
   resets RAB state, This function is to be called when
   Srv req throttling is reset
 
PARAMETERS 
    
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds_3gpp_rab_reestab_reset_state();
