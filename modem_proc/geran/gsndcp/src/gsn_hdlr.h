/*****************************************************************************
***
*** TITLE
***
***  SNDCP HANDLER HEADER FILE
***
***
*** DESCRIPTION
***
***  Header file for the SNDCP handler module.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/gsndcp/src/gsn_hdlr.h#5 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSN_HDLR_H
#define INC_GSN_HDLR_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "gsni.h"
#include "gsndcp.h"
#include "gllsap.h"
#include "gmutex.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

extern uint8 suspend_flag;

/* Lookup tables for SAPI address translation
*/
extern const uint8 gsn_intern_sapi[];

#ifndef FEATURE_GERAN_REDUCED_DEBUG
/* For more information about diagnostic monitoring of SNDCP
** refer to gsndcp.h.
*/
extern gsndcp_diag_actv_nsapis_rpt_fnc_t  actv_nsapis_rpt_fnc;

extern gsndcp_diag_nsapi_cntx_rpt_fnc_t   nsapi_cntx_rpt_fnc;

extern gsndcp_diag_nsapi_state_rpt_fnc_t  nsapi_state_rpt_fnc;
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_EST_REQ()
===
===  DESCRIPTION
===
===   Sends LL_ESTABLISH request to LLC layer to establish or -re-establish
===   ACK peer-to-peer operation for the SAPI in the LLC layer.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_send_ll_est_req(gsn_sapi_e_t *sapi_ptr, dsm_item_type  *xid_req, boolean psm_restore );

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_DATA_IND()
===
===  DESCRIPTION
===
===   Processes the LL_DATA indication primitive used by the LLC
===   layer to deliver the successfully received SN-PDU to the SNDCP layer.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_process_ll_data_ind(const gsn_sapi_e_t *, ll_data_ind_t  *);

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_LL_UDATA_IND()
===
===  DESCRIPTION
===
===   Processes the LL_UNITDATA indication primitive used by the LLC
===   layer to deliver the successfully received SN-PDU to the SNDCP layer.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_process_ll_udata_ind(const gsn_sapi_e_t *, ll_unitdata_ind_t  *);

/*===========================================================================
===
===  FUNCTION      GSN_RETR_NPDU_HDLR()
===
===  DESCRIPTION
===
===   Retransmits uncorfirmed N-PDUs, one at the time starting with oldest first,
===   bufferd in npdu_buff of the NSAPI.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_retr_npdu_hdlr(gsn_nsapi_t nsapi );

/*===========================================================================
===
===  FUNCTION      GSN_SEND_RETR_REQ()
===
===  DESCRIPTION
===
===   Sends retransmit request to PS or COMP task.
===
===  DEPENDENCIES
===
===   None
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_send_retr_req(gsn_nsapi_t nsapi );

/*===========================================================================
===
===  FUNCTION      GSN_SM_PRIM_HDLR()
===
===  DESCRIPTION
===
===   Processes an SM->SNDCP SNSM primitive
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gsn_sm_prim_hdlr(const gsmsn_mes_type *);


/*===========================================================================
===
===  FUNCTION      GSN_LL_CTRL_PRIM_HDLR()
===
===  DESCRIPTION
===
===   Processes an LLC->SNDCP LL control primitive
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gsn_ll_ctrl_prim_hdlr(gprs_ll_dl_sig_mes_u*, gsn_sapi_t );


/*===========================================================================
===
===  FUNCTION      GSN_LL_CTRL_PRIM_FREE_XID_PDU()
===
===  DESCRIPTION
===
===   Processes LLC->SNDCP LL control primitives and frees XID PDU.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/

void gsn_ll_ctrl_prim_free_xid_pdu( gprs_ll_dl_sig_mes_t *ll_msg_ptr );


/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SN_DATA_REQ()
===
===  DESCRIPTION
===
===   Processes SN-DATA request from PDP.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_process_sn_data_req(
                              gsn_nsapi_t, 
                              dsm_item_type **, 
                              boolean, 
                              geran_pdu_priority_t pdu_priority );


/*===========================================================================
===
===  FUNCTION      GSN_INT_CMD_HDLR()
===
===  DESCRIPTION
===
===   Processes SNDCP internal commands.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_int_cmd_hdlr(const gsn_internal_msg_t* );

/*===========================================================================
===
===  FUNCTION      GSN_PROCESS_SN_UDATA_REQ()
===
===  DESCRIPTION
===
===   Processes SN-UNITDATA request from PDP for UL UNACK data transmission. 
===   This function is also used in UNACK mode for re-transmitting N-PDUs in  
===   retransmission buffer. Input parameter retx_npdu_item_ptr is set to a 
===   N-PDU item from retransmission buffer (npdu_buff) when this function is 
===   used for N-PDU retransmissions. 
===    
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_process_sn_udata_req
( 
  gsn_nsapi_t, 
  dsm_item_type **,
  boolean,     
  gsn_npdu_item_t *,
  geran_pdu_priority_t
);

/*===========================================================================
===
===  FUNCTION      GSN_SEND_LL_XID_REQ()
===
===  DESCRIPTION
===
===   Sends LL_XID_REQ to LLC layer. LLC_XID_REQ primitive conveys an XID block
===   to be send to the peer SNDCP layer to start the XID negotiation procedure.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_send_ll_xid_req(gsn_sapi_e_t *, dsm_item_type *, boolean );


/*===========================================================================
===
===  FUNCTION      GSN_SEND_SNSM_STAT_REQ()
===
===  DESCRIPTION
===
===   Sends the SNSM-STATUS-REQ primitive to SM.
===
===  DEPENDENCIES
===
===   None.
===
===  RETURN VALUE
===
===   None.
===
===  SIDE EFFECTS
===
===   None.
===
===========================================================================*/
void gsn_send_snsm_stat_req(  gsn_sapi_t, uint32, uint8 );


#ifndef FEATURE_GERAN_REDUCED_DEBUG
/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_ACTV_NSAPIS_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes the active NSAPIs information report for diagnostic monitoring.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsn_encode_actv_nsapis_diag_rpt(void);

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_CNTX_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes context information report of the specified NSAPI
===   for diagnostic monitoring.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsn_encode_cntx_diag_rpt(   uint8 nsapi );

/*===========================================================================
===
===  FUNCTION      GSN_ENCODE_STATE_DIAG_RPT()
===
===  DESCRIPTION
===
===   Encodes state report of the specified NSAPI for diagnostic monitoring.
===
===  DEPENDENCIES
===
===    None.
===
===  RETURN VALUE
===
===    None.
===
===  SIDE EFFECTS
===
===    None.
===
===========================================================================*/
void gsn_encode_state_diag_rpt(  uint8 nsapi );
#endif /* !FEATURE_GERAN_REDUCED_DEBUG */

void gsn_set_suspend_event(gsn_nsapi_t nsapi, uint8 event);
void gsn_clr_suspend_event(gsn_nsapi_t nsapi, uint8 event);
#endif /* INC_GSN_HDLR_H */

/*** EOF: don't remove! ***/
