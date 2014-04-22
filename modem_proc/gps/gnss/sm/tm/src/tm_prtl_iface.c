/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TM-Core / Protocol sub-module Interface

GENERAL DESCRIPTION
  This file implements TM-Core / protocol sub-module interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2013 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/sm/tm/src/tm_prtl_iface.c#1 $  
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/15   mj      Added support for UMTS OTA Reset
01/05/16   mc      Dynamically allocate variables for huge structures to reduce stack usage
10/01/15   rk      Define new internal API for protocol to know if A_GNSS position 
                   methods are supported. 
03/14/15   rh      Added support for MultiSIM NV  
08/02/14   gk      do not send another session begin if already sent for early rcvr session when NI is started. 
06/23/14   gk      OVerwrite the early rcvr session with a new CP session if the session starts after 911 end
11/13/13   rk      Missing initialization of pd_event_type causes sending garbage data to LBS.
12/13/11   gk      LPP related fixes
07/27/11   rh      Configurably accept 1X MT CP even if MO session is active
12/2/09    gk      intermediate position fixes
07/22/09   atien   Add tm_post_ext_refpos_rep_event
07/14/09   atien   tm_sess_req() - save client id when starting MT session
04/25/08   gk      On-Demand changes
08/10/07   gk      check if the E911 call is in orig state to accept MT-LR
05/27/07   rw      1x CP can end 1x UP during KDDI protocol
12/07/06   cl      Added checks for start session request from protocl modules
06/15/06   CL      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gps_variation.h"
#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "msg.h"
#include "tm_data.h"
#include "tm_common.h"
#include "tm_pdapi_client.h"
#include "tm_prtl_iface.h"
#include "tm_cm_iface.h"
#include "time_svc.h"

/*===========================================================================

FUNCTION TM_PRTL_REG

DESCRIPTION
  This function is called by protocol sub-moudles to inform TM-Core sub-module 
  that it is ready to serve.

  It should provide a cb table with 5 function callbacks to TM-Core, 
  so TM-Core knows how to communicate with it.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module allows registration
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_prtl_reg(
                     tm_prtl_type             prtl_type,
                     const tm_prtl_cb_s_type  *cb_tbl
                   )
{
  int prtlFuncTblIndex;

  if(cb_tbl == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"cb_tbl is NULL", 0, 0, 0);
    return FALSE;  
  }
  
  /* Find the correct array index for the prtl_type */
  prtlFuncTblIndex = tm_core_get_prtl_idx_by_type(prtl_type);

  if((prtlFuncTblIndex < 0) || (prtlFuncTblIndex >= TM_PRTL_NUM))
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"Can not find a index for protocl type %d", prtl_type, 0, 0);
    return FALSE;
  } 
  
  /* Setup protocol callback table */  
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].start_sess_req_fp   = cb_tbl->start_sess_req_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].stop_sess_req_fp    = cb_tbl->stop_sess_req_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].sess_req_data_fp    = cb_tbl->sess_req_data_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].sess_info_fp        = cb_tbl->sess_info_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].timer_cb_fp         = cb_tbl->timer_cb_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].event_cb_fp         = cb_tbl->event_cb_fp;
  tm_core_info.prtl_func_cb_table[prtlFuncTblIndex].gm_event_handle_fp  = cb_tbl->gm_event_handle_fp;
  return TRUE; /*lint !e506 */
}

/*===========================================================================

FUNCTION tm_sess_req

DESCRIPTION
  This function is called by protocol sub-moudles to start, stop a session.
  It is also called by protocol sub-modules to request aiding data and get 
  the status (validity) of aiding data.
  
DEPENDENCIES

RETURN VALUE
  TURE: If TM-Core sub-module accepts the request
  FALSE: Otherwise

SIDE EFFECTS

===========================================================================*/
boolean tm_sess_req(
                     tm_prtl_type                    prtl_type,
                     tm_sess_handle_type             sess_handle,
                     tm_sess_req_type                req_type, 
                     const tm_sess_req_param_u_type  *req_param
                   )
{
  boolean retVal                 = FALSE;
  boolean mt_allowed             = TRUE;  
  tm_cm_phone_state_info_s_type  phone_state_info;
 


  /* Init */
  memset(&phone_state_info, 0, sizeof(phone_state_info));



  if(req_param == NULL)
  {
    MSG_3(MSG_SSID_GPSSM, MSG_LEGACY_ERROR,"NULL req_param", 0, 0, 0);
    return retVal;
  }
  
  switch(req_type)
  {
    case TM_SESS_REQ_START:
      
      tm_cm_iface_get_phone_state(&phone_state_info);    

        retVal = FALSE;
      
    break;

    case TM_SESS_REQ_CONTINUE:
      retVal = tm_core_sess_req_continue(prtl_type, sess_handle, req_param->continue_param);
    break;

    case TM_SESS_REQ_STOP:
      retVal = tm_core_sess_req_stop(prtl_type, sess_handle, req_param->stop_param);
    break;

    default:
      retVal = FALSE;
    break;
  }

  return retVal;
}

/*===========================================================================

FUNCTION tm_post_data   not used by standalone, only protocols

*/
