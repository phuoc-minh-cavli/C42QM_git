/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                U M T S

                P A C K E T   D O M A I N
                                
                A T   C O M M A N D   P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It executes UMTS & GPRS specific packet
  domain AT commands.

  
EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatetsipkt_ex.c#1 $   $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
08/12/14   pg      Modified DS Profile APIs to per subs.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DSAT_ETSI_DATA
#include "stdio.h"

#include <stringl/stringl.h>
#include "msg.h"
#include "nv.h"
#include "dstaski.h"

#include "dsati.h"
#include "dsatparm.h"
#include "dsatctab.h"
#include "dsatdl.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsipkt.h"
#include "dsatetsicmif.h"
#include "dstaski.h"
#include "nv.h"
#include "sm.h"
#include "sm_common.h"
#include "cm.h"
#include "dsatact.h"
#include "ps_ppp_defs.h"
#include "ps_utils.h"
#include "ps_in.h"
#include "ds_3gpp_rmsm_ati.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
  These macros used to be in dsumtspdpreg.h but had to removed from there 
  due to compilation issues seen with pdp reg rpc stub code.
-------------------------------------------------------------------------*/
#define pdp_addr_ipv4 pdp_addr.pdp_addr_ipv4
#define pdp_addr_ipv6 pdp_addr.pdp_addr_ipv6

LOCAL char *etsipkt_cgerep_event_buffer[] = {"+CGEV: REJECT ",
                                             "+CGEV: NW REACT ",
                                             "+CGEV: NW DEACT ",
                                             "+CGEV: ME DEACT ",
                                             "+CGEV: NW DETACH",
                                             "+CGEV: ME DETACH",
                                             "+CGEV: NW CLASS CG",
                                             "+CGEV: NW CLASS CC",
                                             "+CGEV: ME CLASS B",
                                             "+CGEV: ME CLASS CG",
                                             "+CGEV: ME CLASS CC",
                                             "+CGEV: PDN ACT",
                                             "+CGEV: PDN DEACT",
                                             "+CGEV: DED NW DED ACT",
                                             "+CGEV: ME ACT",
                                             "+CGEV: DED NW DEACT",
                                             "+CGEV: ME DEACT",
                                             "+CGEV: NW MODIFY",
                                             "+CGEV: ME MODIFY"
};

dsat_pdp_connect_s_type dsat_pdp_state;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL void etsipkt_add_gprs_msg_to_queue
(
  char                  *buffer,
  uint16                 buf_len
);

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================
FUNCTION  ETSIPKT_ADD_GPRS_MSG_TO_QUEUE

DESCRIPTION
  Buffers the GPRS event report in MT.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void etsipkt_add_gprs_msg_to_queue
(
  char                    *buffer,
  uint16                   buf_len
)
{
  etsipkt_gprs_msg_type *msg_ptr;
  dsatetsipkt_gprs_msg_info     *gprs_dd_ms_val = NULL;

  gprs_dd_ms_val = dsat_get_base_addr(DSAT_MD_VALS, TRUE);
  if (NULL == gprs_dd_ms_val)
  {
    return;
  }
  
  if (q_cnt(&gprs_dd_ms_val->etsipkt_gprs_msg_q) < DSAT_GPRS_EVENT_MSG_COUNT)
  {
    msg_ptr = dsat_alloc_memory(sizeof(etsipkt_gprs_msg_type), TRUE);
  }
  else
  {
    /* Get the first element in the actual queue 
       Strategy -circular buffer, discard the oldest ones */
    msg_ptr = q_get(&gprs_dd_ms_val->etsipkt_gprs_msg_q);
  }

  if (NULL == msg_ptr)
  {
    DS_AT_MSG0_ERROR("Failed to buffer the GPRS event report in MT");
  }
  else
  {
    /* Add event data to the message */
    (void) dsatutil_memscpy((void*)msg_ptr->event_data,
            GPRS_EVENT_DATA_SIZE,(void*)buffer,MIN(buf_len,GPRS_EVENT_DATA_SIZE));
    msg_ptr->event_data_len = MIN(buf_len,GPRS_EVENT_DATA_SIZE);

    /* Put the message on the message queue */
    q_put( &gprs_dd_ms_val->etsipkt_gprs_msg_q, &msg_ptr->link );
  }

  return;
} /* etsipkt_add_gprs_msg_to_queue */

/*===========================================================================
FUNCTION  DSATETSIPKT_REPORT_GPRS_EVENT_TO_TE

DESCRIPTION
  Reports the GPRS event report buffer in MT.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_report_gprs_event_to_te
(
  dsat_cgerep_e_type     event_type, 
  void                  *event_data
)
{
  char                           buffer[GPRS_EVENT_DATA_SIZE];
  char                           ip_address[MAX_IPADDR_STR_LEN+1];
  uint16                         buf_len = 0;
  dsat_num_item_type             cgerep_val ;
  dsat_cgerep_evt_info_s_type    *cgerep_evt_info = NULL;
  ds_profile_type_s               profile ;
  dsat_ip_addr_type                temp_pdp_addr;
  dsat_ip_addr_type                ip_addr ={0};

  memset ( ip_address, 0, sizeof(ip_address));
  memset(&temp_pdp_addr,0x00,sizeof(temp_pdp_addr));
  
  if (event_type >= ARR_SIZE(etsipkt_cgerep_event_buffer))
  {
    DS_AT_MSG1_ERROR("Invalid event type %d",(int)event_type);
    return;
  }

  buf_len = (uint16)strlen(etsipkt_cgerep_event_buffer[event_type]);
  buf_len = MIN(buf_len,sizeof(buffer) );
  
  DS_AT_MSG1_HIGH("In dsatetsipkt_report_gprs_event_to_te event = %d",event_type);
  
  (void) dsatutil_memscpy((void*)buffer,
          GPRS_EVENT_DATA_SIZE,(void*)etsipkt_cgerep_event_buffer[event_type],buf_len);
  
  switch (event_type)
  {
    case DSAT_MS_INITIATED_PDP_DEACTIVATE:
    case DSAT_NW_INITIATED_PDP_DEACTIVATE:
    case DSAT_NW_INITIATED_PDP_REACTIVATE:
      {
      
      if(DS_PROFILE_RESULT_SUCCESS == ds_profile_setting_from_profile_id(((dsat_cgerep_evt_info_s_type*)event_data)->cid,&profile))
        {
            if(DSAT_OK == ds_3gpp_rmsm_ati_get_pdp_addr((uint32)(int)((dsat_cgerep_evt_info_s_type*)event_data)->cid,
                                                          &(ip_addr.addr)))
            {
              DS_AT_MSG2_MED("PDP context - %d has a dynamic address: version %d.",
                   (((dsat_cgerep_evt_info_s_type*)event_data)->cid),ip_addr.addr.ip_vsn);
              
              ip_addr.ip_vsn = (ds_profile_ip_version_enum_type)ip_addr.ip_vsn;              
              if( ip_addr.ip_vsn == DS_PROFILE_IP_V4)
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_IP_V4; 
                temp_pdp_addr.addr.pdp_addr_ipv4 = ps_ntohl(ip_addr.addr.pdp_addr_ipv4);
                dsatutil_convert_ipaddr(INT_TO_STRING,&temp_pdp_addr,(byte *)ip_address);
              }
              else if( ip_addr.ip_vsn == DS_PROFILE_IP_V6 || ip_addr.ip_vsn == DS_PROFILE_IP_V4V6 )
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_IP_V6;
                temp_pdp_addr.addr.pdp_addr_ipv6.in6_u.u6_addr64[0] = ps_ntohll(ip_addr.addr.pdp_addr_ipv6.in6_u.u6_addr64[1]);
                temp_pdp_addr.addr.pdp_addr_ipv6.in6_u.u6_addr64[1] = ps_ntohll(ip_addr.addr.pdp_addr_ipv6.in6_u.u6_addr64[0]);
                dsatutil_convert_ipaddr(INT_TO_STRING,&temp_pdp_addr,(byte *)ip_address);
              }
              else
              {
                return;
              }
              buf_len += (uint16)
              snprintf((char *)&buffer[buf_len],
              sizeof(buffer)-buf_len,
              "IP, %s, %d", 
              ip_address, 
              (int)((dsat_cgerep_evt_info_s_type*)event_data)->cid);
            }
            else
            {
              if( ip_addr.addr.ip_vsn == DS_PROFILE_IP_V6 ||
                               ip_addr.addr.ip_vsn == DS_PROFILE_IP_V4V6 )
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_IP_V6;
              }
              else
              {
                temp_pdp_addr.ip_vsn = DS_PROFILE_IP_V4;
              }
              dsatutil_convert_ipaddr(INT_TO_STRING,&temp_pdp_addr,(byte *)ip_address);
              buf_len += (uint16)
              snprintf((char *)&buffer[buf_len],
              sizeof(buffer)-buf_len,
              "IP, %s, %d", 
              ip_address, 
              (int)((dsat_cgerep_evt_info_s_type*)event_data)->cid);
            }
          }
        }
      break;
    case DSAT_PDN_ACTIVATE:
    case DSAT_PDN_DEACTIVATE:
      buf_len += (uint16)snprintf((char *)&buffer[buf_len],
                                          sizeof(buffer)-buf_len,
                                          "%d", 
                    (int)((dsat_cgerep_evt_info_s_type*)event_data)->cid);
      break;
    case DSAT_DED_NW_DED_ACTIVATE:
    case DSAT_DED_NW_DEACTIVATE:
    case DSAT_MS_ACTIVATE:
    case DSAT_MS_DEACTIVATE:
      cgerep_evt_info = (dsat_cgerep_evt_info_s_type*)event_data;

      buf_len += (uint16)snprintf((char *)&buffer[buf_len],
                                          sizeof(buffer)-buf_len,
                                          "%d %d %d", 
                                          (int)cgerep_evt_info->cid,
                                          (int)cgerep_evt_info->cid,
                                          cgerep_evt_info->evt_type);
      break;
    case DSAT_NW_MODIFY:
    case DSAT_MS_MODIFY:
      cgerep_evt_info = (dsat_cgerep_evt_info_s_type*)event_data;

      buf_len += (uint16)snprintf((char *)&buffer[buf_len],
                                          sizeof(buffer)-buf_len,
                                          "%d %d %d", 
                                          (int)cgerep_evt_info->cid,
                                          cgerep_evt_info->change_reason,
                                          cgerep_evt_info->evt_type);
      break;
    default:;
  }/* Switch */
  cgerep_val =(dsat_num_item_type)dsatutil_get_val(
                                          DSATETSI_EXT_CGEREP_IDX,0,NUM_TYPE);
  if ( !dsatcmdp_block_indications_ex() )
  {
   /* TA-TE link is not reserved, forward the results directly to TE 
    except when mode is DSAT_CGEREP_BUFFER_AT_MT */
    if ( ( cgerep_val == DSAT_CGEREP_BUFFER_DISCARD_FWD_TE ) ||
         ( cgerep_val == DSAT_CGEREP_BUFFER_FWD_TE ))
    {
      /* Fwd to TE as the MT is not in online-data mode */
      dsm_item_type *res_buff_ptr;
  
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
      if (NULL != res_buff_ptr)
      {
        (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
                res_buff_ptr->size,(void*)buffer,MIN(buf_len,sizeof(buffer)));
        
        res_buff_ptr->used = MIN(buf_len,sizeof(buffer));
        (void)dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
      }
    }
    /* TA-TE link is not reserved but mode is  
       DSAT_CGEREP_BUFFER_AT_MT so buffer it*/
    else if (  cgerep_val == DSAT_CGEREP_BUFFER_AT_MT )
    {
      etsipkt_add_gprs_msg_to_queue(buffer, buf_len);
    }
  }
  else
  {
    /* TA-TE link is reserved. Buffer the result only when 
      mode is DSAT_CGEREP_BUFFER_FWD_TE or DSAT_CGEREP_BUFFER_AT_MT */
    /* Buffer the unsolicited result codes in MT. No forwarding 
     to TE needed */
    if ( ( cgerep_val == DSAT_CGEREP_BUFFER_FWD_TE )||
         ( cgerep_val == DSAT_CGEREP_BUFFER_AT_MT ))
      etsipkt_add_gprs_msg_to_queue(buffer, buf_len);
  }
} /* dsatetsipkt_report_gprs_event_to_te */

/*===========================================================================
FUNCTION  DSATETSIPKT_INIT_SERVICE_PREFERENCE_FROM_NV

DESCRIPTION
  Retrieves MS's service preference option from NV.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_init_service_preference_from_nv()
{
  nv_item_type      nv_item;   /* NV interface data */  
  dsatcmif_servs_state_info  *ph_val = NULL;

  //TODO: sanjaykc : subs_id
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if (NULL == ph_val)
  {
    return;
  }

  if(dsatutil_get_nv_item( NV_SERVICE_DOMAIN_PREF_I, &nv_item) == NV_DONE_S)
  {
    ph_val->dsat_ms_service_preference = (cm_srv_domain_pref_e_type)nv_item.service_domain_pref.srv_domain;
  }
} /* dsatetsipkt_init_service_preference_from_nv */

#endif /* FEATURE_DSAT_ETSI_DATA */



