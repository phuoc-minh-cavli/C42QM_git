/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

      M U L T I M O D E   T A R G E T   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the top level command tables for the multimode
  target.  It also contains any target specific command tables required
  to support target specific support of AT modem commands.

EXTERNALIZED FUNCTIONS
  None
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2005-2021 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath$
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatmmtgt.c#7 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/25/21   ks      Added support for FSK related AT commands.
11/09/20   ks      Added support for preserving URC indices across PSM cycles.
04/23/20   ks      Fixed jammer detect modem crash.
04/17/17   skc     Enabled requested AT Commands for IOE Build.
04/13/17   rg      Added rau and gprs ready timer for CPSMS
1/19/17   skc     Added enhancement on CHLD and CLCC for VoLTE MPTY Call.
06/13/16   skc     Added support for AT+CPSMS.
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
06/27/14   tk/sc   Added support for Dynamic ATCoP.
12/30/13   sc      Fixed static code bugs.
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
08/22/12   sk      Mainlining Nikel Changes.
07/31/12   sk      C-EONS Feature Support.
02/17/11   ua      Added support for $QCRMCALL.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
04/20/11   ad      Fixed NIKEL featurization issue. 
04/20/11   bs      NIKEL Phase I support.
03/16/11   bs      Restructured command tables to be mode independant.
07/08/10   ad      Added support for $ECALL command.
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      Added support for $QCVOLT, $QCHWREV, $QCBOOTVER, $QCTEMP, 
                   $QCAGC, $QCALLUP.
01/06/10   bs      Added +CGACT command support for LTE.
11/16/09   ca      Added support for MMGSDI Auth commands.
12/15/09   nc      Featurisation changes for LTE.
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
09/09/09   ua      Added support for EONS (spec 22.101).
12/29/08   nc      Added support for +VTS Command
11/28/08   cs      Off-Target build Compiler and Lint warning fixes.
10/23/08   bs      Added support for 1X AT Phonebook commands.
07/21/08   bs      Added support for CDMA NAARefresh.
07/16/08   ua      Added support for external client support.
06/30/08   bs      Fixing Featurization issues.
06/13/08   ua      Correcting reading of data from RUIM/NV items
                   for OMH 2.
02/18/08   sa      Added modification for Passport Feature support.
09/07/07   sa      Corrected feature wrapping for dsat707_hdr_table
                   and updated vendor tables.
08/30/07   sa      Updated for CDMA and GSM(WCDMA) enabled targets.
04/28/05   snb     Initial version, after dsatmsm6300tgt.c

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"
#include "dsatclienti.h"
#include "dsatctab.h"
#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsictab.h"
#include "dsatetsicmif.h"
#include "dsatetsime.h"
#include "dsatetsicall.h"
#endif /* FEATURE_DSAT_ETSI_MODE */
#ifdef FEATURE_ETSI_SMS
#include "dsatsms.h"
#endif /* FEATURE_ETSI_SMS */


#include "dsatvoice.h"
#include "dsatcmif.h"
#include "dsatme.h"
#include "dsatvend.h"
#include "dsatact.h"
/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                      Asynchronous Event Handler Table
---------------------------------------------------------------------------*/
const dsati_async_event_table_entry_type async_event_table_entries[] =
{
  { DS_CMD_ATCOP_STATUS_CMD,       dsatme_cmd_status_handler_ex},
  { DS_CMD_ATCOP_CM_CALL_CMD,        dsatcmif_cm_call_cmd_handler_ex },
  { DS_CMD_ATCOP_CM_CALL_INFO_CMD,   dsatcmif_cm_call_event_handler_ex },
  
#ifdef FEATURE_DSAT_ATCOP_VEND
  { DS_CMD_ATCOP_CM_SS_CMD,          dsatcmif_cm_ss_cmd_handler },

#endif
  { DS_CMD_ATCOP_CM_PH_CMD,          dsatcmif_cm_ph_cmd_handler_ex },
  { DS_CMD_ATCOP_CM_PH_INFO_CMD,     dsatcmif_cm_ph_event_handler_ex },
  { DS_CMD_ATCOP_TIMER_EXPIRED_CMD,  dsatutil_dispatch_timer_handler },  
#ifdef FEATURE_DSAT_ETSI_MODE
  { DS_CMD_ATCOP_CM_INBAND_CMD,       dsatcmif_cm_inband_cmd_handler },
  { DS_CMD_ATCOP_CM_INBAND_INFO_CMD,  dsatcmif_cm_inband_event_handler },
#endif /* FEATURE_DSAT_ETSI_MODE */
  #ifdef FEATURE_MMGSDI
  { DS_CMD_ATCOP_MMGSDI_INFO_CMD,    dsatme_mmgsdi_event_handler },
  { DS_CMD_ATCOP_MMGSDI_SIM_INFO,    dsatme_mmgsdi_sim_event_handler_ex },
  //{ DS_CMD_ATCOP_COPS_AT_CMD,        dsatetsicall_cops_mmgsdi_handler},
  //{ DS_CMD_ATCOP_CPOL_AT_CMD,        dsatetsime_cpol_mmgsdi_handler},
#ifdef FEATURE_DSAT_MMGSDI_AUTH
  { DS_CMD_ATCOP_MMGSDI_AUTH_CMD,    dsat707_mmgsdi_auth_event_handler },
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_MMGSDI_ATCSIM
  { DS_CMD_ATCOP_MMGSDI_APDU_RESP,     dsatetsime_mmgsdi_send_apdu_handler },
  { DS_CMD_ATCOP_MMGSDI_CRSM_RESP,     dsatetsime_mmgsdi_crsm_handler      },
#endif /* FEATURE_MMGSDI_ATCSIM */

#endif /* FEATURE_DSAT_ETSI_MODE */

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  { DS_CMD_ATCOP_PBM_CB_CMD,         dsatme_pbm_cb_cmd_handler },
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

  { DS_CMD_ATCOP_TO_SIO_LOW_CMD,     dsati_to_sio_low_cmd_handler },

  { DS_CMD_ATCOP_SIO_RTS_CHANGED_CMD,   dsati_sio_rts_changed_cmd_handler },

#ifdef FEATURE_DSAT_ETSI_MODE


  { DS_CMD_ATCOP_CM_SUPS_CMD,        dsatetsicmif_cm_sups_cmd_handler_ex },
  { DS_CMD_ATCOP_CM_SUPS_INFO_CMD,   dsatetsicmif_cm_sups_event_handler_ex },

#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_ETSI_SMS
  { DS_CMD_ATCOP_SMS_ERR_CMD,   dsatsms_err_handler },
  { DS_CMD_ATCOP_SMS_ABT_CMD,   dsatsms_abt_handler },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DSAT_MMGSDI_AUTH 
{ DS_CMD_ATCOP_MD5_ABT_CMD,    dsatme_md5_abt_handler },
#endif /* FEATURE_DSAT_MMGSDI_AUTH  */

#ifdef FEATURE_DSAT_GOBI_MAINLINE
  { DS_CMD_ATCOP_FTM_DIAGPKT_RSP_CMD, dsatvend_ftm_diagpkt_rsp_handler },
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

  { DS_CMD_ATCOP_FWD_CLIENT_DEREG,  dsatclient_deregister_fwd_client_handler },
  { DS_CMD_ATCOP_FWD_AT_CMD_REG,    dsatclient_register_fwd_at_cmd_handler },
  { DS_CMD_ATCOP_FWD_AT_CMD_DEREG,  dsatclient_deregister_fwd_at_cmd_handler },
  { DS_CMD_ATCOP_EXT_CMD_RESP,      dsatclient_ext_at_resp_handler },
  { DS_CMD_ATCOP_RESET_REQ_CMD,     dsatclient_reset_cmd_request_handler},
  { DS_CMD_ATCOP_SHUTDOWN_REQ_CMD,  dsatclient_shutdown_cmd_request_handler},
  { DS_CMD_ATCOP_EXT_CMD_URC,       dsatclient_ext_at_urc_handler},

  { DS_CMD_ATCOP_PDP_CMD,       dsatetsipkt_at_cmd_handler },
  { DS_CMD_ATCOP_CRTDCP_CMD,    dsatetsipkt_crtdcp_done_handler},

#ifdef FEATURE_DSAT_GPS_CMDS

  { DS_CMD_ATCOP_PD_INFO_CMD      ,  dsat_pd_event_handler },
                                 
#endif /* FEATURE_DSAT_GPS_CMDS */


#ifdef FEATURE_DATA_QCRMCALL
    { DS_CMD_ATCOP_RMNET_EV_CMD, dsatvend_rmnet_ev_handler },
#endif /* FEATURE_DATA_QCRMCALL */

#ifdef FEATURE_DATA_MUX
    { DS_CMD_ATCOP_MUX_NOTIFY_CMD ,dsatact_cmux_notify_handler },
    { DS_CMD_ATCOP_RDM_NOTIFY_CMD, dsatact_rdm_notify_handler},
#endif /* FEATURE_DATA_MUX */

#ifdef FEATURE_DSAT_LTE 
#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
  { DS_CMD_ATCOP_VOIPM_AT_CMD,         dsatvend_voipm_config_handler },
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
#endif /* FEATURE_DSAT_LTE */

  { DS_CMD_ATCOP_PSM_GET_CONFIG_CMD, dsatetsime_psm_get_config_handler},
  { DS_CMD_ATCOP_PSM_CFG_CHANGE_IND_CMD, dsatetsime_psm_get_config_handler},
  { DS_CMD_ATCOP_RESTORE_URC_INFO_IN_PSM_CMD, dsatetsime_psm_urc_restore_handler},
  
  { DS_CMD_ATCOP_EDRX_CONFIG_GET_CNF_CMD, dsatetme_process_cedrx_ex_cmd},

  { DS_CMD_ATCOP_EDRX_CONFIG_SET_CNF_CMD, dsatetme_process_cedrx_ex_cmd},
 #ifdef FEATURE_DSAT_ATCOP_VEND
  { DS_CMD_ATCOP_PSM_BOOT_IMAGE_TYPE_IND_CMD, dsatvend_psm_boot_image_handler},
  { DS_CMD_ATCOP_CEINFO_CMD, dsatvend_process_ceinfo_cmd},
  { DS_CMD_ATCOP_JD_SERVICE_STATUS_CMD, dsatvend_jd_service_status_ind_handler},
  { DS_CMD_ATCOP_GET_SFN_CMD, dsatvend_process_sfn_cmd},
  { DS_CMD_ATCOP_SET_SFN_RESP_CMD, dsatvend_process_sfn_cmd},
  { DS_CMD_ATCOP_SFN_REACHED_IND_CMD, dsatvend_process_sfn_cmd},
  { DS_CMD_ATCOP_JAMMER_DETECT_CMD, dsat_jammer_detect_cmd_handler},
  { DS_CMD_ATCOP_CELL_SELECT_INFO_CMD, dsatvend_process_cell_select_cmd},
#ifdef FEATURE_FSK_TX
  { DS_CMD_ATCOP_FSK_DEBUG_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_DEBUG_QUERY_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_DATA_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_DATA_ABT_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_HOP_TABLE_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_HOP_TABLE_QUERY_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_START_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_STOP_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_PCL_RSP_CMD, dsatvend_process_fsk_cmd},
  { DS_CMD_ATCOP_FSK_PCL_QUERY_RSP_CMD, dsatvend_process_fsk_cmd},
#endif /* FEATURE_FSK_TX */
#endif
  /* need this here to appease the compiler when none of the
     above features is defined,
     DS_AT_ASYNC_EVENT_END should never be used for a real 
     event, ERR will be thrown if it is used */
  { DS_CMD_ATCOP_ASYNC_EVENT_END,    NULL }
};

const dsati_async_event_table_type async_event_table =
{
  async_event_table_entries, ARR_SIZE(async_event_table_entries)
};
