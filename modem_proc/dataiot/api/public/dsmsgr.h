#ifndef DSMSGR_H
#define DSMSGR_H

/*===========================================================================
                      DSMSGR.H
DESCRIPTION
This file exposes 3GPP MSGR APIs to external clients outside DS.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR FILE
when           who    what, where, why
--------    ---    ----------------------------------------------------------
08/09/17    sd      First checkin
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#include "msgr.h"
#include "appmgr_msg.h"
#include "pthread.h"
#include <msgr_umid.h>  /* UMID definitions */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


#define MSGR_MODULE_MSGRRECV          0x10
#define MSGR_MODULE_UT                0x11
#define MSGR_MODULE_LTE               0x12
#define MSGR_MODULE_3GPP              0x13
#define MSGR_MODULE_MGR               0x14


#define MSGR_DS_MSGRRECV            MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_MSGRRECV )
#define MSGR_DS_UT                  MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_UT )
#define MSGR_DS_LTE                 MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_LTE )
#define MSGR_DS_3GPP                MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_3GPP )
#define MSGR_DS_MGR                 MSGR_TECH_MODULE( MSGR_TECH_DS, MSGR_MODULE_MGR )




/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*
** Receiver-side defs
*/

typedef boolean (* dsmsgrcv_msg_hdlr_f) 
(
  msgr_umid_type,
  const msgr_hdr_struct_type *
);

/* Enumeration for Traffic class information available as a part of
   UMTS QOS */
typedef enum { 
	DS_TRF_CLASS_CONVERSATIONAL = 1, 
	DS_TRF_CLASS_STREAMING = 2, 
	DS_TRF_CLASS_INTERACTIVE = 3, 
	DS_TRF_CLASS_BACKGROUND = 4, 
} ds_msgr_traffic_class_enum_type; 
/*
typedef enum
{
  DS_LOW_LATENCY_TRAFFIC_STATE_STOP,
  DS_LOW_LATENCY_TRAFFIC_STATE_START,
  DS_LOW_LATENCY_TRAFFIC_STATE_CONTINUE,
  DS_LOW_LATENCY_TRAFFIC_STATE_MAX 
} ds_msgr_low_latency_traffic_state_enum_type;
*/
/* Enumeration for supported RATs */
typedef enum {   
  DS_RAT_GSM = 0, 
  DS_RAT_LTE = 1, 
} ds_msgr_rat_enum_type; 

typedef enum
{
  DS_LTE_QCI_0                     = 0,
  DS_LTE_QCI_1                     = 1,
  DS_LTE_QCI_2                     = 2,
  DS_LTE_QCI_3                     = 3,
  DS_LTE_QCI_4                     = 4,
  DS_LTE_QCI_5                     = 5,
  DS_LTE_QCI_6                     = 6,
  DS_LTE_QCI_7                     = 7,
  DS_LTE_QCI_8                     = 8,
  DS_LTE_QCI_9                     = 9,
  DS_LTE_QCI_65                    = 65,
  DS_LTE_QCI_66                    = 66,
  DS_LTE_QCI_69                    = 69,
  DS_LTE_QCI_70                    = 70,
  DS_LTE_QCI_MAX
} ds_3gpp_lte_qci_enum_type ;


typedef struct { 
  byte eps_bearer_id; /* Id of this EPS Bearer. */ 
  ds_3gpp_lte_qci_enum_type qci_value ; /* QOS information of this EPS bearer. */ 
} ds_msgr_lte_qos_info_type; 

/* Bearer QOS Info Message */
typedef struct { 
  msgr_hdr_struct_type hdr; /* message header */ 
  
  /* Message Payload */ 
  ds_msgr_rat_enum_type current_bearer_rat; 
  ds_msgr_lte_qos_info_type lte_qos_info; //Valid when bearer is in LTE 
} ds_msgr_bearer_qos_info_msg_s ;

/* UMID definitions
*/
enum                          // need to remane it to msgr struct
{ 
  MSGR_DEFINE_UMID(DS, 3GPP, IND, BEARER_QOS_INFO, 
                   0x01, ds_3gpp_bearer_qos_info_msg_s ),
};

typedef struct
{
   /* message header */ 
   msgr_hdr_struct_type   hdr; 

   /* loopback mode H activation status */
   boolean                act_status;    
} ds_3gpp_eps_tlb_lb_mode_h_act_ind_type  ;

typedef struct
{
   /* message header */ 
   msgr_hdr_struct_type   hdr; 

   /* mo_ex data activity status */
   boolean                act_status;    
} ds_3gpp_mo_exp_activity_status_ind_type  ;

/* UMID definitions */
enum
{ 
  MSGR_DEFINE_UMID(DS, 3GPP, IND, TLB_MODE_H_ACT_DEACT, 
                   0x02, ds_3gpp_eps_tlb_lb_mode_h_act_ind_type  ),  
};

enum
{
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, LOOPBACK, MSGR_ID_LOOPBACK, 
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, THREAD_READY, APPMGR_ID_THREAD_READY, 
                   none),
  MSGR_DEFINE_UMID(DS, MSGRRECV, SPR, THREAD_KILL, APPMGR_ID_THREAD_KILL, 
                   none),                   
  MSGR_DEFINE_UMID(DS, MSGRRECV, TMRI, WDOG_RPT_TMR_EXPIRED, 0x00, 
                   none)
};

/* UMID definitions
*/
enum                          // need to remane it to msgr struct
{ 
  MSGR_DEFINE_UMID(DS, 3GPP, IND, MO_EXP_ACTIVITY_STATUS, 
                   0x03, ds_3gpp_mo_exp_activity_status_ind_type ),
};



/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


boolean ds_msgr_snd_msg_send_ext
(
  msgr_umid_type         msgtype,
  msgr_tech_module_type  tech_mod_type,
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
);

boolean ds_msgr_snd_msg_send
(
  msgr_umid_type         msgtype,  
  msgr_hdr_struct_type   *msg,
  uint32                 msglen
);

/*
** Receiver-side APIs
*/

pthread_t dsmsgrrecv_task (int priority);
void dsmsgrrcv_reg_hdlr (msgr_umid_type msgrtype, dsmsgrcv_msg_hdlr_f dshdlr);
void dsmsgrrcv_init (void);

void dsmsgr_post_ds_cmd (void);
void dsmsgr_process_ds_cmd (void * ds_cmd_p);

/*=========================================================================*/

/* need to remove late  to avoid compilation issue on nb1_rlcdl_cfg.h*/


typedef struct { 
  byte rab_id; /* Network service access point identifier for this UMTS bearer.*/           
  ds_msgr_traffic_class_enum_type traffic_class; /* QOS information of this UMTS bearer. */ 
} ds_3gpp_tds_w_qos_info_type; 



/* Bearer QOS Info Message */
typedef struct { 
  msgr_hdr_struct_type hdr; /* message header */ 
  
  /* Message Payload */ 
  ds_msgr_rat_enum_type current_bearer_rat; 
  
  union { 
     ds_3gpp_tds_w_qos_info_type tds_w_qos_info; // Valid when bearer is in WCDMA or TDSCDMA
     ds_msgr_lte_qos_info_type lte_qos_info; //Valid when bearer is in LTE
  } u;

} ds_3gpp_bearer_qos_info_msg_s;

#endif /* DSMSGR_H */
