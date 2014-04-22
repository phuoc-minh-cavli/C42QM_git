/*===========================================================================
                        P S _ L O G G I N G _ D I A G . C

GENERAL DESCRIPTION
  This file contains data path logging request handler functions which are
  diag callback functions registered during powerup.  These callback
  functions are the interface between the DM (diagnostic tool) and the PS.
  Also provides access to the lookup table maintained locally.

EXTERNAL FUNCTIONS
  dpl_init()
    Registers functions with diag.


INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2004-2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/dataiot.mpss/2.0/interface/utils/src/ps_logging_diag.c#1 $
 $Author: pwbldsvc $ $DateTime: 2020/03/17 08:03:16 $

when        who    what, where, why

--------    ---    ---------------------------------------------------------- 

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include <stringl/stringl.h>
#include "diagcmd.h"
#include "diagpkt.h"
#include "ps_byte.h"
#include "ps_logging_defs.h"
#include "ps_logging_diag.h"
#include "ps_logging.h"
#include "ps_utils.h"
#include "ds_crit_sect.h"
#include "data_msg.h"
#include "log.h"
#include "log_codes.h"
#include "rcinit_rex.h"
#include "ds_system_heap.h"
#include "ds_fwk.h"
#include "ds_pdn_apn_table.h"
#include "ds_pdn_manager.h"
#include "ds_fwki.h"



#if defined (PS_LOGGING_IOE_DPL) || defined (FEATURE_LOGGING_UNSUPPORTED_DPL)
/*===========================================================================

                                  CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of header added to each DIAG request/response
---------------------------------------------------------------------------*/
#define DIAG_HDR_LEN  (sizeof(diagpkt_subsys_header_type))

/*---------------------------------------------------------------------------
  Offset in to DIAG header where sub system command code (SSCC) is found
---------------------------------------------------------------------------*/
#define DIAG_HDR_SSCC_OFFSET  2

#define IP_HDR_LEN_MASK        0x0F
#define IP_VERSION_MASK        0xF0
#define IPV4_VERSION           0x40
#define IPV6_VERSION           0x60
#define IPV6_HEADER_LENGTH     40                

/*===========================================================================

                                VARIABLES

===========================================================================*/

dpl_state_enum_type    dpl_state = DPL_DISABLED;

dpl_net_dir_cb_type       *dpl_global_cb =NULL;



/*---------------------------------------------------------------------------
  Type used to define lookup table
---------------------------------------------------------------------------*/
#endif //  defined (PS_LOGGING_IOE_DPL) || defined (FEATURE_LOGGING_UNSUPPORTED_DPL)
/*===========================================================================

                                  EXTERNAL FUNCTIONS

===========================================================================*/
#ifdef PS_LOGGING_IOE_DPL

/*===========================================================================
FUNCTION      DPL_GET_DPL_STATE

DESCRIPTION
  This method will return current DPL state

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dpl_state_enum_type dpl_get_dpl_state
(
  void
)
{
   return dpl_state;
}/*dpl_get_dpl_state*/



/*===========================================================================
FUNCTION    DPLI_fill_apn_info

DESCRIPTION
  Reterive APN info from 3GPP and 3GPP2 profile and fill into log pointer

PARAMETERS

  log_ptr               : log pointer
  ds_fwk_ptr            : fwk ptr to be used o fetch the apn 

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
uint8 * dpl_fill_apn_info
(
  ds_fwk_s_type   *fw_inst,
  uint8           *log_ptr,
  uint8            size
)
{
  char                           apn_string[DS_PROFILE_APN_STRING_LEN+1] = {0};
  int8                           apn_len = 0;  
  ds_pdn_mgr_s       *v4_pdn_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
 if(fw_inst->v4_pdn_ptr != NULL)
    v4_pdn_ptr  = fw_inst->v4_pdn_ptr;
 else if(fw_inst->v6_pdn_ptr !=NULL)
     v4_pdn_ptr  = fw_inst->v6_pdn_ptr;
 else 
    return log_ptr;
  if(v4_pdn_ptr->pdp_profile->context.apn[0] !='\0')
  {
      strlcpy(apn_string,
              (v4_pdn_ptr->pdp_profile->context.apn),
              sizeof(v4_pdn_ptr->pdp_profile->context.apn));
    if (0 != strlen((char *)apn_string) && ( strlen((char *)apn_string) < size ))
    {
      apn_len = snprintf((char *)log_ptr, size, " %s", apn_string);
      if (apn_len > 0)
      {
          log_ptr = log_ptr + apn_len;
      }
      else
      {
        /* Default profile or Null APN Profile*/
        apn_len = 0;
      }
    }  
   }
   else
   {
     DS_MSG0(MSG_LEGACY_MED,"dpl_fill_apn_info() : Apn name is not configured on profile"); 
     apn_len = snprintf((char *)log_ptr, size, " %s", "default_apn");     
          log_ptr = log_ptr + apn_len;
   }
   return log_ptr;
}/*dpl_fill_apn_info*/


dpl_net_dir_cb_type *dpl_get_net_dir_cb
(
  void
)
{
  return dpl_global_cb;
}/*dpl_get_dpl_state*/

/*=======================================================
FUNCTION    dpli_update_ifaces_dpl_cb

DESCRIPTION
  This method run over all ifaces and enables the logging in valid iface, 
  sets protocol and packet
  log length
 
PARAMETERS
  ps_iface_ptr[in]   Pointer to the interface 
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
 static void dpli_update_global_dpl_cb
( 
  dpl_state_enum_type  dpl_state
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(dpl_state)
  {
    case DPL_ENABLED_PARTIAL:
    {
      dpl_global_cb->snaplen[DPL_IID_NETPROT_IP]  = DPL_PACKET_PARTIAL_LENGTH;
      dpl_global_cb->is_logging_enabled  = TRUE;
      break;
    }
    case DPL_ENABLED_FULL:
    case DPL_ENABLED_IP_HEADER:
    {
       dpl_global_cb->snaplen[DPL_IID_NETPROT_IP]  = SNAPLEN_ALL;
       dpl_global_cb->is_logging_enabled = TRUE;
       break;
    }
    case DPL_DISABLED:
    {
       dpl_global_cb->snaplen[DPL_IID_NETPROT_IP]  = 0;
       dpl_global_cb->is_logging_enabled = FALSE;
       break;
    }
    default :
      break;
  }
  dpl_global_cb->mask        |= (0x01 << (DPL_IID_NETPROT_IP - 1));
  
  DS_MSG2(MSG_LEGACY_ERROR,"dpli_update_global_dpl_cb) : DPL state %d update len %d",dpl_state,
          dpl_global_cb->is_logging_enabled);
  return;
}/*dpli_update_global_dpl_cb*/

/*===========================================================================
FUNCTION       DPL_CHECK_LOG_CODE()

DESCRIPTION
  This function will check partial and full log codes and will disable/enable DPL state.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void dpl_check_log_code
(
  void
)
{
  uint8       is_dpl_state_change =  FALSE;
  uint32      pkt_len             = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* 
  * Full byte logging (if both are enabled) gets higher precedence over  Partial logging .
  * Update packet len to DPM only when logging state change
  */
  if (TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_TX_FULL_C) ||
          TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_RX_FULL_C) ||
          TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_TX_FULL_C) ||
          TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_RX_FULL_C))
  {
    if (DPL_ENABLED_FULL != dpl_state)
    {
      dpl_state           = DPL_ENABLED_FULL;
      pkt_len             = SNAPLEN_ALL;
      is_dpl_state_change = TRUE;
    }
  }
  else if (TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_TX_80_BYTES_C) ||
    TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_RX_80_BYTES_C) ||
    TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_TX_80_BYTES_C) ||
    TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_RX_80_BYTES_C))
  {
    if (DPL_ENABLED_PARTIAL != dpl_state)
    {
      dpl_state           = DPL_ENABLED_PARTIAL;
      pkt_len             = DPL_PACKET_PARTIAL_LENGTH;
      is_dpl_state_change = TRUE;
    }
  } 
  else if(TRUE == log_status( LOG_NETWORK_IP_HEADER_RM_TX_C) ||
            TRUE == log_status( LOG_NETWORK_IP_HEADER_RM_RX_C) ||
            TRUE == log_status( LOG_NETWORK_IP_HEADER_UM_TX_C) ||
            TRUE == log_status( LOG_NETWORK_IP_HEADER_UM_RX_C))
  {
      /*-----------------------------------------------------------------------
        Currently passing the packet lenght as SNAPLEN_ALL as we don't know the 
        actual length of the IP header of the packet at this point
      -----------------------------------------------------------------------*/
      if (DPL_ENABLED_IP_HEADER != dpl_state)
      {
        dpl_state           = DPL_ENABLED_IP_HEADER;
        pkt_len             = SNAPLEN_ALL;
        is_dpl_state_change = TRUE;
      }
  }
  else
  {
    if (DPL_DISABLED != dpl_state)
    {
      dpl_state           = DPL_DISABLED;
      pkt_len             = 0;
      is_dpl_state_change = TRUE;
    }
  }

  if (TRUE == is_dpl_state_change)
  {
    DS_MSG2(MSG_LEGACY_ERROR,"dpl_diag_sig_handler() : DPL state %d update len %d", 
    dpl_state,pkt_len);

    /* Update global            cb with DPL state*/    
    dpli_update_global_dpl_cb(dpl_state);

    /* Now Update pakcet len to DPM */
  }
  return ;
} /* dpl_check_log_code */


/*===========================================================================
FUNCTION   DPL_DIAG_SIG_HANDLER()

DESCRIPTION
  This function process signal registered with diag.

DEPENDENCIES
  None.

PARAMETERS
  sig: the Signal that is handled
  user_data_ptr: NOT USED

RETURN VALUE
  TRUE: no more processing to be done
  FALSE: Needs to be called again.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dpl_diag_sig_handler
(
  ps_sig_enum_type       sig,
  void                 * user_data_ptr
)
{
  dpl_check_log_code ();
  return TRUE;
} /* dpl_diag_sig_handler */


uint32 dpli_get_ip_header_length
(
  dsm_item_type  * item_ptr
)
{
  uint8                              ip_hdr_first_byte;
  uint8                              header_length = 0;
  uint8                              version;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Peek in to the first byte to determine lenght of the IP header
  -----------------------------------------------------------------------*/
  (void) dsm_extract(item_ptr, 0, &ip_hdr_first_byte, 1);

  version = ip_hdr_first_byte & IP_VERSION_MASK;
  if (IPV4_VERSION == version)
  {
    header_length = ip_hdr_first_byte & IP_HDR_LEN_MASK;
    /*-----------------------------------------------------------------------
      multiply by 4 as the lenght is stored in 32-bit words
    -----------------------------------------------------------------------*/
    return (header_length * 4); 
  }
  /*-----------------------------------------------------------------------
    For IPv6 packet, IP header length is constant (40 bytes)
  -----------------------------------------------------------------------*/
  else if (IPV6_VERSION == version)
  {
    return IPV6_HEADER_LENGTH;
  }
  else
  {
    return header_length;
  }
}


/*===========================================================================
FUNCTION      DPLI_LOG_IID_DESC_AND_STATUS

DESCRIPTION
  Logs a packet with iface status(UP/DOWN) and iid description

PARAMETERS

  this_iface_ptr      : logged this iface description
  bridged_iface_ptr : bridged iface ptr(will be used in case of RM ifaces) 

RETURN VALUE
  None

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dpli_log_iid_desc_and_status
(
  ds_fwk_s_type                *fw_inst,
  ip_addr_enum_type             family,  
  ds_dpl_client_type            client_id,
  char                         *apn
)
{
  uint8          * log_ptr;      
  uint8          * return_ptr;
  uint16           space_left     = 0;
  dpl_net_cb       *dpl_cb = NULL;
  uint8            dpl_index=0;
  char             desc[DPL_IFNAME_DESC_S_LEN];  
  int8                           apn_len = 0;  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (fw_inst == NULL)
  {
  
     DS_MSG0(MSG_LEGACY_ERROR,"dpli_log_iface_desc_and_status: "
                     "Null Fwk pointer" );
     return;
  }

  if(family == IPV4_ADDR)
    dpl_index  =0;
  else if(family == IPV6_ADDR)
    dpl_index  =1;

  if(client_id == DS_DPL_CLIENT_UM)
  {
    dpl_cb = ds_fwk_get_dpl_ptr(fw_inst,(ip_version_enum_type)family,client_id);
  }
  else if(client_id == DS_DPL_CLIENT_RM)
  {
    dpl_cb = ds_fwk_get_dpl_ptr(fw_inst,(ip_version_enum_type)family,client_id);
  }
  else
    return;

  if (dpl_cb == NULL)
  {
    return;
  }

  DS_MSG2(MSG_LEGACY_ERROR,"dpli_log_iface_desc_and_status: %d"
          " dpl_index %d",dpl_cb->dpl_id.ifname,dpl_index);

  snprintf(desc ,DPL_IFNAME_DESC_S_LEN -1 ,"%s:%d",(dpl_index ==0)?"_v4":"_v6",dpl_cb->dpl_id.ifname);
  /*-----------------------------------------------------------------------
   DPL Iface description is present in physical iface DPL control block.
   Ignore Logical ifaces
    -----------------------------------------------------------------------*/

  log_ptr = (uint8 *) log_alloc(LOG_DPL_IFACE_DESCRIPTION_AND_STATUS_C,
                                  MAX_DPL_DESC_LEN + LOG_HDR_SIZE);
  

  if (NULL == log_ptr)
  {
    
    DS_MSG0(MSG_LEGACY_MED,"dpli_log_iface_desc_and_status: "
                      "log_alloc failed ");
    return;
  }
  
  return_ptr =  log_ptr;
  
  log_ptr   += LOG_HDR_SIZE;
    
  log_ptr = put32(log_ptr, *(uint32 *)((void *) &( dpl_cb->dpl_id)));
  space_left = MAX_DPL_DESC_LEN - (log_ptr- return_ptr);
  
 
  DS_MSG_SPRINTF_1(MSG_LEGACY_ERROR,"Found dplid in rmnet pool  %d",space_left);
  if(apn == NULL)
  log_ptr = dpl_fill_apn_info(fw_inst, log_ptr, space_left);
  else
  {
    apn_len = snprintf((char *)log_ptr, space_left, " %s", apn);         
    log_ptr = log_ptr + apn_len;
   }

  space_left = MAX_DPL_DESC_LEN - (log_ptr- return_ptr);
  
  log_ptr += snprintf((char *)log_ptr, 
                        space_left,
                        "%s", desc);
  
  log_shorten(return_ptr, (uint32) (log_ptr - return_ptr));

  log_commit(return_ptr);
  
  return;

}/*dpli_log_iface_desc_and_status*/

/*===========================================================================
FUNCTION    DPL_INIT

DESCRIPTION
  Registers functions with DIAG.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dpl_init
(
  void
)
{
  RCEVT_SIGEX_SIGREX rcevt_diag_sig;  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register log mask change for diag.
  -------------------------------------------------------------------------*/
  rcevt_diag_sig.signal = rex_self();
  rcevt_diag_sig.mask = 1 << PS_DPL_DIAG_SIGNAL;  

  if (RCEVT_NULL != rcevt_register_sigex_name(DIAG_LOG_MASK_CHANGE_NOTIFY, 
                                              RCEVT_SIGEX_TYPE_SIGREX, 
                                              &rcevt_diag_sig))
  {
    (void) ps_set_sig_handler(PS_DPL_DIAG_SIGNAL, dpl_diag_sig_handler, NULL);

    ps_enable_sig( PS_DPL_DIAG_SIGNAL );

    DS_MSG0(MSG_LEGACY_ERROR,"ps_mm_task(): Registration Successful for diag event");
  }

  /*-------------------------------------------------------------------------
    Initilize DPL state
  -------------------------------------------------------------------------*/
  dpl_state = DPL_DISABLED;
  DS_SYSTEM_HEAP_MEM_ALLOC(dpl_global_cb, sizeof(dpl_net_dir_cb_type), dpl_net_dir_cb_type *);

  /*-------------------------------------------------------------------------
  Check Log Codes
  -------------------------------------------------------------------------*/
  dpl_check_log_code ();

} /* dpl_init() */
#endif //PS_LOGGING_IOE_DPL

