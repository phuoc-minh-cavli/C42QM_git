#ifndef L1_EDRX_PEO_H
#define L1_EDRX_PEO_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  L 1   EDRX PEO   H E A D E R   F I L E

DESCRIPTION
   Layer 1 PEO and EDRX function declarations.


Copyright (c) 2001-2015 Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gl1/inc/l1_edrx_peo.h#5 $
$DateTime: 2019/01/01 22:24:35 $ $Author: pwbldsvc $

when       who      what, where, why
--------   -------- --------------------------------------------- 
31/05/18    hp      CR2201368  FR 49924: eDRx/Idle power optimizations
11/05/18    km       CR2240546 Error recovery handling of PEO activities
15/11/17    hd       CR2142995 SBD for CCCH wakeups before own paging block or uplink TBF when eDRx > 30 sec
18/07/17    nm       CR2040158 Optimization to reschedule BCCH if not enough time for timing sync
11/07/17    nm       CR2038718 skip some PEO activities based on last successful Sync FN
17/04/17    nm       CR2031283 serv_meas_ind not getting sent in when SMR recvd during MON act
08/02/17    nm       CR2003353 serv_meas_ind not getting sent in PSM+PEO combination
11/01/17    nm       CR1087215 FR 36703: Rel.13 GERAN Power Efficient Operation (PEO)
27/01/17    nm       CR1116718 correct featurization for SP build

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "l1_utils.h"
#include "l1_edrx_peo_defines.h"

#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)
/*===========================================================================

                     FUNCTION DECLARATIONS                                                                                                              

===========================================================================*/


/*===========================================================================

FUNCTION l1_peo_enable

DESCRIPTION
  This function enables the power efficient operation within GL1

INPUTS
  eDRX_enabled - Whether EDRX is enabled or not 
  BS_ePA_MFRMS -Value of DRX cycle if eDRX is enabled

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void  l1_peo_enable(boolean eDRX_enabled, word BS_ePA_MFRMS);


/*===========================================================================

FUNCTION l1_peo_disable

DESCRIPTION
  This function disables the power efficient operation of GL1

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void  l1_peo_disable(void);


/*===========================================================================

FUNCTION l1_peo_init

DESCRIPTION
  Initializes the PEO state machine to start the new set of PEO activites either before every Page or before RACH/UL TX
  which is indicated by "serv_meas_req_rcd" to find the first PEO activity type and FN to run. 

INPUTS
  serv_meas_req_rcd - if TRUE respresents that this PEO measurements are to be done for a MPH_SERVING_MEAS_REQ
  serv_meas_req_rcd - if FALSE, these are for Page reception in Idle mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  l1_peo_init(boolean serv_meas_req_rcd);


/*===========================================================================

FUNCTION l1_peo_tick

DESCRIPTION
  Tick for the PEO state machine to to be given in Idle mode

INPUTS
  leaving_idle_mode - if TRUE, state machine will abort the PEO activities

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void  l1_peo_tick(boolean leaving_idle_mode);

/*===========================================================================

FUNCTION l1_peo_post_tick

DESCRIPTION
  Post Tick after l1_peo_tick for the PEO state machine to be given in Idle mode. 

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void l1_peo_post_tick(void);

/*===========================================================================

FUNCTION l1_peo_ccch_callback_rcvd

DESCRIPTION
  Make PEO Activity status to callback_received after CCCH_metrics_callback is received.

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/


void l1_peo_ccch_callback_rcvd(void);

/*===========================================================================

FUNCTION l1_peo_reset

DESCRIPTION
  Reset the PEO state machine to to start afresh next time

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void  l1_peo_reset(void);


/*===========================================================================

FUNCTION l1_peo_abort

DESCRIPTION
  Abort the PEO activites due to either leaving idle mode or deactivate gsm stack

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void  l1_peo_abort(void);

/*===========================================================================

FUNCTION l1_peo_get_enabled

DESCRIPTION
  Whether PEO is enabled or not at GL1

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE is PEO enabled else returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean l1_peo_get_enabled(void);

/*===========================================================================

FUNCTION l1_is_peo_act_in_progress

DESCRIPTION
  Whether any PEO activity is in progress(ongoing) 

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE is PEO activity is ongoing else returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean l1_is_peo_act_in_progress(void);


/*===========================================================================

FUNCTION l1_peo_is_it_time_for_serv_meas_ind

DESCRIPTION
  Based on current EDRX value and or MPH_SERV_MEAS_REQ reception, this function returns 
  whether it is time to send the serving_meas_ind to GRR

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is time to send serv_meas_ind else returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean l1_peo_is_it_time_for_serv_meas_ind(void);

/*===========================================================================

FUNCTION l1_peo_serv_meas_ind_sent

DESCRIPTION
  To inform peo engine that serv meas ind is sent

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None.

===========================================================================*/
void l1_peo_serv_meas_ind_sent(void);

/*===========================================================================

FUNCTION l1_peo_is_Lth_page

DESCRIPTION
  checks if this is the Lth Page where L = 30/EDRX ifor EDRX<30sec and 1 for EDRX > 30sec

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it is Lth page else returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean l1_peo_is_Lth_page(void);

/*===========================================================================

FUNCTION l1_peo_increment_page_count

DESCRIPTION
  increment the page count for curren tEDRX required to calculate the  
  time for sending serving meas ind

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void l1_peo_increment_page_count(void);

/*===========================================================================

FUNCTION l1_peo_reset_page_count

DESCRIPTION
  increment the page count for current EDRX (after sending serving meas ind)

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void l1_peo_reset_page_count(void);

/*===========================================================================

FUNCTION l1_is_peo_serv_meas_req_in_progress

DESCRIPTION
  Whether SERV_MEAS_REQ was received from GRR and serv meas ind for the same is pending 

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE if serv_meas_req_in_progress else returns FALSE

SIDE EFFECTS
  None.

===========================================================================*/

boolean l1_is_peo_serv_meas_req_in_progress(void);

/*===========================================================================

FUNCTION l1_peo_act_in_progress

DESCRIPTION
  Whether any PEO act is ongoing 

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  TRUE if activity ongoing
  Activity type

SIDE EFFECTS
  None.

===========================================================================*/
boolean l1_peo_act_in_progress(l1_peo_activity_T* act);
/*===========================================================================

FUNCTION l1_peo_set_last_sync_FN

DESCRIPTION
  set the FN at which sync was successful last(successfull decode of SCH/FCCH/CCCH) 

INPUTS
  FN

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void l1_peo_set_last_sync_FN(uint32 fn);
/*===========================================================================

FUNCTION l1_peo_get_last_sync_FN

DESCRIPTION
  Get the FN where Sync was sucessful last time 

INPUTS
  gas id

DEPENDENCIES
  None

RETURN VALUE
  FN where Sync was sucessful last time

SIDE EFFECTS
  None.

===========================================================================*/
uint32 l1_peo_get_last_sync_FN(void);

/*===========================================================================

FUNCTION l1_peo_check_all_paging_block

DESCRIPTION
  Check whether to read a CCCH block in PEO mode before own paging block read or uplink packet transfer

INPUTS
  gas id

DEPENDENCIES
  None

RETURN VALUE
  TRUE      a CCCH block in PEO mode before own paging block read or uplink packet transfer
  FALSE    otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean l1_peo_check_all_paging_block(void);
void l1_peo_update_scheduled_activity(void);
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
/*===========================================================================

FUNCTION l1_peo_scheduler

DESCRIPTION
  Calculates the next PEO activity FN. If valid, schedule it

INPUTS
  None

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void l1_peo_scheduler(void);



extern peo_data_T l1_peo_data;
#endif
#endif /*#if defined (FEATURE_GERAN_EXTENDED_DRX) || defined(FEATURE_GERAN_POWER_EFFICIENT_OPERATION)*/

#endif /* L1_EDRX_PEO_H */

/*EOF*/
