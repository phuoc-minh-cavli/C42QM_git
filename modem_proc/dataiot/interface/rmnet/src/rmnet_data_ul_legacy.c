/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    R M N E T _ D A T A _ U L _ L E G A C Y . C

GENERAL DESCRIPTION
  This is the implementation of  legacy path Data Path Manager module

Copyright (c) 2019 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/rmnet/src/rmnet_data_ul_legacy.c#1 $
  $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/04/15    gk     Removed un-necessary F3s from data path
09/10/13    bvd    created file.
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "ps_meta_info.h"
#include "ps_svc.h"
#include "ds_rmnet_smi.h"
#include "ps_metai_info.h"
#include "data_msg.h"
#include "rmnet_data_ul_legacy.h"

dsm_watermark_type     rmnet_data_ul_legacy_wm;

q_type                 rmnet_data_ul_legacy_wm_q;


static ps_meta_info_type_ex    * meta_info_ex_ptr;
static ps_tx_meta_info_type    * tx_meta_info_ptr;

static boolean rmnet_legacy_wm_flow_enabled = TRUE;

#define   RMNET_DATA_UL_LEGACY_WM_LOW   15000
#define   RMNET_DATA_UL_LEGACY_WM_HIGH  45000
#define   RMNET_DATA_UL_LEGACY_WM_DNE   800000


/*===========================================================================

                             INTERNAL FUNCTIONS

===========================================================================*/
static void rmnet_data_ul_legacyi_non_empty_data_cback
(
 dsm_watermark_type  * wm,
 void                * cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set Rx Signal
  -------------------------------------------------------------------------*/
  ps_set_signal(PS_RX_RMNET1_SIGNAL);

} /* rmnet_data_ul_legacyi_non_empty_data_cback() */


static void rmnet_data_ul_legacyi_wm_low_cb_func
(
  struct dsm_watermark_type_s * legacy_wm_ptr,
  void                        * user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  rmnet_legacy_wm_flow_enabled = TRUE;
  DS_MSG1(MSG_LEGACY_ERROR,"In ps_dpm_ul_legacyi_wm_low_cb_func %d",rmnet_legacy_wm_flow_enabled);


  /*-------------------------------------------------------------------------
    Once legacy wm goes from flow disabled to flow enabled state, it has to 
    flow enable all the wm on DPM RM. In ps_dpm_rm_enable_all_flows, loop is 
    ran through all DPM RMs to enable flow on each of them by setting the
    DPM UL signal
  -------------------------------------------------------------------------*/

  //ps_dpm_rm_enable_all_flows();

}/*rmnet_data_ul_legacyi_wm_low_cb_func()*/

static void rmnet_data_ul_legacyi_wm_high_cb_func
(
  struct dsm_watermark_type_s * legacy_wm_ptr,
  void                        * user_data_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_MSG0(MSG_LEGACY_ERROR,"In ps_dpm_ul_legacyi_wm_high_cb_func");

  rmnet_legacy_wm_flow_enabled = FALSE;

}/*rmnet_data_ul_legacyi_wm_high_cb_func()*/

boolean rmnet_data_ul_legacyi_sig_hdlr  
(
  ps_sig_enum_type     sig,
  void               * user_data
)
{
   ps_meta_info_type_ex		    *dup_meta_info_ex_ptr = NULL;
   dsm_item_type 			    *legacy_rx_pkt;
#ifdef FEATURE_DATA_QMI_QOS
   uint16						 bytes_read;
   ps_rmnet_qos_hdr_type			 qos_hdr;
   ps_rmnet_qos2_hdr_type			 qos2_hdr;
   uint8 						  data_format;
   rmnet_qos_format_e_type 		 qos_hdr_format;
#endif /* FEATURE_DATA_QMI_QOS */
   ip_addr_enum_type			 ip_ver;
   rmnet_sm_link_prot_e_type 	 link_prot;
  rmnet_smi_info_type            *info;
  ds_fwk_s_type                  *fwk_inst;

  
  legacy_rx_pkt = dsm_dequeue(&rmnet_data_ul_legacy_wm);
  if (NULL == legacy_rx_pkt)
  {
    return TRUE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Validate Rm and get it's IP family
  -------------------------------------------------------------------------*/
   info = (rmnet_smi_info_type *)(legacy_rx_pkt->app_ptr);
  
    if(RMNET_META_SM_NULL_STATE == info->meta_sm.state)
	{
	  DS_MSG0(MSG_LEGACY_ERROR,"RMNET state is in RMNET_META_SM_NULL_STATE" );
	  return TRUE;
	}

   fwk_inst = (ds_fwk_s_type *)legacy_rx_pkt->app_field;
   if(fwk_inst ==NULL)
   {
      DS_MSG0(MSG_LEGACY_ERROR,"FWK instance is NULL" );
	  return TRUE;
	}

   ip_ver = rmnet_get_addr_family(info);
   if(ip_ver == NON_IP_ADDR)
   {
     DS_MSG0(MSG_LEGACY_ERROR,"Non -ip data recieved in the legacy watermark");
	 return TRUE;
   }

  /*-------------------------------------------------------------------------
    Fetch DPM RM using app_field of DSM item and cache the needed info in
    local variables
  -------------------------------------------------------------------------*/
      
   link_prot        = RMNET_LINK_PROT(info);
#ifdef FEATURE_DATA_QMI_QOS
  data_format      = (uint8)RMNET_QOS_ENABLED(info);
  qos_hdr_format   = (uint8)RMNET_QOS_HEADER_TYPE(info);
#endif /* FEATURE_DATA_QMI_QOS */

#ifdef FEATURE_DATA_QMI_QOS

/*---------------------------------------------------------------------------
    Extract QoS header from DSM item if QoS is negotiated as data format
  -------------------------------------------------------------------------*/
  memset((void *) &qos_hdr, 0, sizeof(ps_rmnet_qos_hdr_type));  
  if (PS_RMNET_QOS_ENABLED == data_format)
  {
	  if (RMNET_QOS_FORMAT_8_BYTE == qos_hdr_format)
	  {
		bytes_read = dsm_pullup(&legacy_rx_pkt, (byte *) &qos2_hdr, sizeof(qos2_hdr));
		if (sizeof(qos2_hdr) != bytes_read)
		{
		  dsm_free_packet(&legacy_rx_pkt);
		  return FALSE;
		}
	  }
	  else
	  {
		bytes_read = dsm_pullup(&legacy_rx_pkt, (byte *) &qos_hdr, sizeof(qos_hdr));
		if (sizeof(qos_hdr) != bytes_read)
		{
		  dsm_free_packet(&legacy_rx_pkt);
		  return FALSE;
		}
	  }
 }
#endif
 /*---------------------------------------------------------

 check valid rmnet state when data can actually be processed.
 ------------------------------------------------------------*/

  if(info->meta_sm.state >= RMNET_META_SM_RM_CONFIGURING_STATE ||
   info->meta_sm.state <= RMNET_META_SM_RM_RECONFIGURING_STATE)
  {
    PS_TX_META_RESET_FILTER_RESULT(tx_meta_info_ptr, FLTR_CLIENT_QOS_OUTPUT);
    meta_info_ex_ptr->tx_meta_info_ptr = tx_meta_info_ptr;
    
    tx_meta_info_ptr->rt_meta_info_ptr->is_rmnet_packet = TRUE;
    PS_META_INFO_DUP(meta_info_ex_ptr, &dup_meta_info_ex_ptr);
#ifdef FEATURE_DATA_QMI_QOS
    if (PS_RMNET_QOS_ENABLED == data_format)
	{
      if (RMNET_QOS_FORMAT_8_BYTE == qos_hdr_format)
      {
        rmnet_process_qos_hdr(legacy_rx_pkt,
                                NULL,
                               ip_ver,
                               FALSE,
                               dup_meta_info_ex_ptr->tx_meta_info_ptr,
                               (void *)&qos2_hdr,
                               qos_hdr_format,
                               NULL
                               );
      }
      else
      {
        rmnet_process_qos_hdr(legacy_rx_pkt,
                               NULL,
                               ip_ver,
                               FALSE,
                               dup_meta_info_ex_ptr->tx_meta_info_ptr,
                               (void *)&qos_hdr,
                               RMNET_QOS_FORMAT_6_BYTE,
                               NULL
                               );

  }
    }
  #endif
  }
/*-------------------------------------------------------------------
   Execute powersave filters on incoming packet in legacy path. 
   Check if we have to do the low latency  
-------------------------------------------------------------------*/   
  if(RMNET_IP_MODE == link_prot)
  {
    ds_fwk_input(fwk_inst,&legacy_rx_pkt ,dup_meta_info_ex_ptr);
  }
  
  tx_meta_info_ptr->rt_meta_info_ptr->is_rmnet_packet = FALSE;
  return FALSE;
} /* rmnet_data_ul_legacyi_sig_hdlr() */

/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
void rmnet_data_ul_legacy_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initiatlize the WM
  -------------------------------------------------------------------------*/
  dsm_queue_init(&rmnet_data_ul_legacy_wm,
                 RMNET_DATA_UL_LEGACY_WM_DNE,
                 &rmnet_data_ul_legacy_wm_q);

  dsm_set_low_wm(&rmnet_data_ul_legacy_wm, RMNET_DATA_UL_LEGACY_WM_LOW);
  dsm_set_hi_wm(&rmnet_data_ul_legacy_wm, RMNET_DATA_UL_LEGACY_WM_HIGH);
  dsm_set_dne(&rmnet_data_ul_legacy_wm, RMNET_DATA_UL_LEGACY_WM_DNE);

  rmnet_data_ul_legacy_wm.non_empty_func_data = NULL;
  rmnet_data_ul_legacy_wm.non_empty_func_ptr  =
    rmnet_data_ul_legacyi_non_empty_data_cback;

  rmnet_data_ul_legacy_wm.lowater_func_data = NULL;
  rmnet_data_ul_legacy_wm.lowater_func_ptr  =  
    rmnet_data_ul_legacyi_wm_low_cb_func;
    
  rmnet_data_ul_legacy_wm.hiwater_func_data = NULL;
  rmnet_data_ul_legacy_wm.hiwater_func_ptr  =  
    rmnet_data_ul_legacyi_wm_high_cb_func;
    
  /*-------------------------------------------------------------------------
    Initialize signal used for legacy path
  -------------------------------------------------------------------------*/
  (void) ps_set_sig_handler(PS_RX_RMNET1_SIGNAL,
                            rmnet_data_ul_legacyi_sig_hdlr,
                            NULL);
  ps_enable_sig(PS_RX_RMNET1_SIGNAL);

  /*-------------------------------------------------------------------------
    Pre-allocate meta info at powerup so that it is not allocated every time
    in data path. This meta info is DUPed and sent along with each legacy pkt
  -------------------------------------------------------------------------*/
  PS_META_INFO_GET_EX(meta_info_ex_ptr);
  PS_TX_META_INFO_GET_ALL(tx_meta_info_ptr);
  if (NULL == meta_info_ex_ptr || NULL == tx_meta_info_ptr)
  {
    DS_ERROR_FATAL("ps_dpm_ul_legacy_init(): Couldn't alloc meta info");
    return;
  }

  meta_info_ex_ptr->tx_meta_info_ptr = tx_meta_info_ptr;
  return;

} /* rmnet_data_ul_legacy_init() */


boolean rmnet_data_ul_legacy_input
(
  rmnet_smi_info_type     *info,
  dsm_item_type           *rx_pkt,
  ds_fwk_s_type           *fwk_inst
)
{
  rmnet_smi_ip_info_type  *ip_info;

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(info == NULL)
  {
     DS_MSG0(MSG_LEGACY_ERROR,"rmnet_data_ul_legacy_input(): RMNET Call not UP");
	 return TRUE;
  }
  
  ip_info = rmnet_get_ip_info_from_inst(info->constants.ip_inst);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ip_info == NULL)
  {
     DS_MSG0(MSG_LEGACY_ERROR,"rmnet_data_ul_legacy_input(): Call not UP");
	 return TRUE;
  }
  /*-------------------------------------------------------------------------
    If legacy watermark is not flow enabled, then return true
  -------------------------------------------------------------------------*/
  if (FALSE == rmnet_legacy_wm_flow_enabled)
  {
    ip_info->rx_pkt = rx_pkt;
    return TRUE;
  }
  
  /*-------------------------------------------------------------------------
    Enqueue packet in to legacy WM. These packets will handled by the
    legacy sig hdlr in PS task
  -------------------------------------------------------------------------*/
  rx_pkt->app_field = (uint32)fwk_inst;
  rx_pkt->app_ptr   = info;
  dsm_enqueue(&rmnet_data_ul_legacy_wm, &rx_pkt);

  return FALSE;
} /* rmnet_data_ul_legacy_input() */
