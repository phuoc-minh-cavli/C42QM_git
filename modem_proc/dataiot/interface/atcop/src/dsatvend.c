/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                V E N D O R   S P E C I F I C

                A T    C O M M A N D   
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It implements execution of common 
  vendor specific AT commands.

EXTERNALIZED FUNCTIONS

  dsatvend_exec_qcclr_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcclr command.
    at$qcclr command is used for clearing mobile error log.

  dsatvend_exec_qcdmg_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmg command.
    at$qcdmg is used to transition the serial port to diagnostic
    monitor.

  dsatvend_exec_qcdmr_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmr command.
    at$qcdmr is used to set the DM baud rate.

  dsatvend_exec_qcdom_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdom command.
    at$qcdom is used to set domain name address.

  dsatvend_exec_qcdnsp_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdnsp command.
    at$qcdnsp is used to set primary DNS IP address.

  dsatvend_exec_qcdnss_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdnss command.
    at$qcdnss is used to set secondary DNS IP address.

  dsatvend_exec_brew_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmg command.
    at$brew is used to transition the serial port to diagnostic
    monitor.

  dsatvend_exec_qcpwrdn_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcpwrdn command.
    at$qcpwrdn is used to power down the terminal.
  
  dsatvend_exec_qcdgen_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdgen command.
    at$qcdgen is used to send a specified quantity of PS data as IP packet(s) 
    over a primary PDP-IP context already activated by +CGACT command.

  dsatvend_exec_qcslip_cmd
    This function brings up the SLIP Iface by posting SLIP_IFACE_UP_EV to the
    DS task. It reserves a serial port that will be used by the slip stack.

  dsatvend_exec_prefmode_cmd
    This function takes the result from the command line parser
    and executes it. It executes at^prefmode command. This command provides 
    the ability to set/get the network mode preferences.

  dsatvend_exec_sysinfo_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT^SYSINFO command. It provides
    the current SYSTEM information of <srv_status>, <srv_domain>
    <roam_status>, <sys_mode>,<sim_status>. 

  dsatvend_exec_qcpdpcfge_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcpdpcfge command. 
    at$qcpdpcfge is used to edit values in APN table. 

  dsatvend_exec_qcpdpimscfge_cmd
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpimscfge command.
  at$qcpdpimscfge is used to edit PDP profile registry . 

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2023 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE    

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvend.c_v   1.3   11 Oct 2002 10:27:58   sbandaru  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatvend.c#20 $ $DateTime: 2023/06/13 09:05:26 $ $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/23   ks      Modified frequency type from kHz to Hz and added support
                   for new frequency range in QCFSKHOPTABLE command.
04/05/23   ks      Enhancing $QCRRINFO and $QCCELLSCAN command to support NTN RAT.
10/21/22   ks      Added support for new frequency range in FSKHOPTABLE command.
11/16/21   ks      Fixed an issue related to QCRSRP command.
09/30/21   ks      Added support for $QCDLTPTENH command.
02/24/21   ks      Added support to restore JDCFG settings after PSM exit.
02/24/21   ks      Fixed an issue with FSK data abort command.
01/25/21   ks      Added support for FSK related AT commands.
11/09/20   ks      Added support for preserving URC indices across PSM cycles.
09/29/20   sp      Fixed issue related to $qcpdpcfge command.
05/22/20   ks      Added support for $QCRRINFO command.
04/23/20   ks      Fixed jammer detection modem crash.
11/13/19   sp      Added support for AT$QCRPMINFO commad
09/27/19   ks      Fixed KW issues.
09/21/17   rg      Added support for Non-IP pdp type profile.
07/24/17   skc     Fixed issue with sending display buffer for asynchrouns command
                   AT$QCRMCALL and AT+CPOL command.
07/12/17   skc     Enabled AT$QCPRFMOD for IOE Build.
04/17/17   skc     Enabled requested AT Commands for IOE Build.
01/27/17   skc     Enabled AT$QCPDPIMSCFGE for IOE build.
01/27/17   skc     Enabled AT$QCDGEN support in IOE build.
01/09/17   skc     Featurizing AT$QCRMCALL command.
10/18/16   skc     Fixed issue related to AT+CLAC
10/05/16   skc     Fixed KW issues.
09/21/16   skc     Fixed issue related to AT$QCPDPCFGE command.
08/19/16   skc     Fixed issue with AT$QCMRUC for DSDS case.
12/10/15   skc     Replacing TUPLE_TYPE_IPv4_254 with TUPLE_TYPE_IPv4 to remove
                   IP limitation for DNS
12/08/15   skc     Fixed compilation issue due to FEATURE_DUAL_SIM disabled
10/06/15   skc     Fixed KW Issues
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   tk      Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G.
04/17/14   sc      Fixed ATCoP KW errors.
03/17/14   pg      Fixed number of SW resets in Ni/Di targets.
03/18/14   pg      Fixed $QCMRUE SD LTE scan issue.
03/26/14   sc      Fixed SYSINFO display issue on SUB2.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
02/06/14   sc      Fixed $QCRSRP display issue.
01/23/14   sc      Added support for $QCPDPCFGEXT command.
01/07/14   sc      Converted high usage ATCoP macros to functions.
12/30/13   sc      Used cm_user* api instead of sys_sel* api
11/22/13   sc      Fixed Bolt MOB3.0 compiler warnings.
11/13/13   sc      Added support for $QCCSGCOPS command.
10/23/13   sc      Adding support for $QCACQDBC command to clear acq_DB
10/23/13   sc      Fixed $QCRSRP and $QCRSRQ result display issue.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
10/03/13   tk      ATCoP changes for L+G DSDS.
09/13/13   sc      Fixed KW errors.
08/26/13   pg      Fixed $QCRMCALL ip invalid issue.
08/21/13   pg      Added support for emergency calls, operator res pco, mcc and mnc fields in $QCPDPCFGE.
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/28/13   tk      Fixed $QCBANDPREF issue with WLAN France 5000 band.
05/17/13   sk      Added data retry fields to $QCPDPCFGE command.
03/27/13   tk      Fixed issues in dual stack commands.
03/18/13   tk      Fixed $QCMRUE issue with invalid input PLMN ID length.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
12/20/12   tk      Fixed $QCRMCALL command memory leak issue.
10/11/12   tk      Fixed $QCRMCALL command issue.
09/24/12   sk      Added $QCRSRP and $QCRSRQ command Support.
09/24/12   tk      Migrated to CM APIs for MRU table access.
08/22/12   sk      Mainlining Nikel Changes.
08/06/12   tk      Added support for $QCDRX command.
07/26/12   sb      Fixed KW Errors.
06/21/12   sk      Fixed $QCPDPCFGE apn bearer issue.
06/12/12   tk      Extended $QCSYSMODE command for 3GPP2 modes.
02/25/12   sb      Fixed compiler Warnings.
05/18/12   tk      Migrated to MSG 2.0 macros
05/18/12   sk      Added APN bearer field to $QCPDPCFGE.
05/14/12   sb      Fixed KW Warnings.
04/10/12   nc      Fixed Read concurrency issues for MRU commands.
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
03/15/12   nc      Initialized mru_entry before writing onto NV.
02/17/11   ua      Added support for $QCRMCALL.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
11/28/11   sb      Fixed $QCMRUE command to not show entries with NO_SRV as mode
11/03/11   nc      Replaced the deprecated API 'hw_partnum_version' with 'DalChipInfo_GetChipId'.
10/17/11   ad      Added +CECALL support.
09/27/11   mk      Added support for $QCCLAC command.
09/06/10   nc      Fixed the RSSI and RSCP display when 
                   FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY is enabled
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/07/11   nc      Added support for $QCAPNE command.
05/26/11   dvk     Global Variables Cleanup
05/13/11   ad      Added $QCPDPIMSCFGE support.
04/29/11   bs      Fixed $QCDGEN command to use COMPTASK.
03/14/11   ad      Added APN class support in $QCPDPCFGE command.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
10/06/10   ad      Added Error msg before Assert(0).
10/04/10   ad      Generates PS data on the context ID when ID is less then  SYS_MAX_PDP_CONTEXTS.  
08/31/10   sa      Added support for default pdp type as IPv4v6.
09/13/10   sa      Fixed QCDGEN handler to send more data.
08/06/10   nc      Using vbatt_read_pmic_temperature instead of 
                   adc_read(ADC_MSM_THERM_DEGC).
07/28/10   bs      Fixed $QCDGEN command for LTE builds.
07/13/10   ad      Fixed CNTI issue.
07/08/10   ad      Added support for $ECALL command.
06/03/10   sn      Added FEATURE_BT_MODEM featurization for BT specific 
                   functionality. This feature will be enabled in MODEM proc 
                   on multi-proc targets.
05/31/10   ad      Fixed lint error.
05/28/10   sa      Fixed crash during $QCPDPCFGE execution.
05/20/10   kk      Fixed on-target compilation error.
05/17/10   ad      Added Support for $QCPDPCFGE.
05/10/10   kk      Added support for $QCVOLT, $QCHWREV, $QCBOOTVER, $QCTEMP, 
                   $QCAGC, $QCALLUP.
03/18/10   nc      Added Support for $QCDGEN for LTE.
01/18/10   bs      Added support for +PACSP.
06/10/09   ua      Added support for ^PREFMODE command. 
12/15/09   nc      Featurisation changes for LTE.
11/02/09   sa      Replacing snprintf with std_snprintf.
09/22/09   vrk     Merged LTE changes.
09/09/09   ua      Added support for EONS (spec 22.101).
09/18/09   ss      DataCommon CMI SU level impacts.
09/13/09   pp      AT$QCSLIP posts CallP UP command to DCC.
07/04/09   pp      hton/ntoh* macros replaced with ps_hton/ntoh*.
06/30/09   bs      Migration to modified diag interface.
06/12/09   ua      Added support for $QCBANDPREF command.
04/29/09   ua      Fixed compiler warnings. 
12/12/08   ua      Fixed Off target lint errors.
11/07/08   sa      Modified transition of serial port to diagnostic
                   monitor using $BREW command to be non-persistent.
10/18/08   pp      Added support for SLIP interface (AT$QCSLIP)
03/24/08   bs      Fixed Klocwork criticals.
02/18/08   sa      Added modification for Passport Feature support.
02/08/08   sa      Added support for $QCSQ command.
01/11/08   sa      Added support for $QCANTE and $QCRPW commands.
12/04/07   pp      Fixed RVCT 2.2 Compiler warning.
11/24/07   ua      Fixed Lint-lows. 
07/18/07   sa      Fixed feature wrap errors for GSM 1x builds.
05/02/07   ua      Fixed lint medium errors. 
02/15/07   ss      Fixed lint high errors
01/30/07   ss      Replaced banned string API calls
11/03/06   snb     Dual processor changes.
09/26/06   sa      Inter RAT handover support for $QCDGEN.
05/22/06   rsl     Add support for at$qcsysmode to return current sysmode, 
                   possible values returned: HSDPA/HSUPA/HSDPA+HSUPA/WCDMA/GSM.
03/10/06   snb     Verify datagen_info_idx before use.
01/26/06   snb     Have $QCDGEN use DSM_DS_SMALL_ITEM_POOL,
                   correct size of and indexing into datagen_info array,
                   Lint fixes.
09/08/05   ar      Add support for $QCSDFTEST vendor command.
08/10/05   snb     Allow secondary contexts for $QCDGEN.
08/02/05   snb     Add WCDMA-mode $QCDGEN support.
08/01/05   snb     Fix for 6500-GSM builds: use legacy functions instead of
                   ps_ip4_hdr functionality.
07/18/05   snb     Use PS functions for dummy IP frame construction.
07/08/05   snb     Call the registration function to SNDCP in the same way as
                   the DS UMTS PS handler for consistent operation.
06/27/05   snb     Add support for $QCDGEN sending data simultaneously on 
                   multiple contexts.
05/16/05   ar      Merge in serialzer port support to $QCDMG 
05/10/05   snb     Add flow control to $QCDGEN GPRS data send.
04/28/05   snb     Fix 6550 GSM-CDMA build issues.
02/24/05   snb     Add $QCDGEN command support.
11/19/03   ar      Replace dsatutil_format_ipaddr with dsatutil_convert_tuple
10/21/03   ar      Added $QCPWRDN command support.
10/08/03   ar      Add wrapper FEATURE_DSAT_BREW_SUPPORT to BREW commands.
                   Lint corrections.
07/07/03   sb      Added AT$BREW command
12/06/02   ak      Removed include of nv_items.h under UNFINISHED, and
                   instead include nv.h
10/09/02   sb      ERROR check to see if it is a valid baud in $QCTER
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   functions.
12/13/01   sb      Changed the rdm_data_got_at$qcdmg() function name to 
                   rdm_data_got_atqcdmg()
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
05/15/01   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include "dstaski.h"
#include "dsati.h"
#include "dsatcmif.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatvend.h"
#include "dsatme.h"
#include "msg.h"
#include "err.h"
#include "diag.h"
#include "nv.h"

#include <stdio.h>
#include <string.h>

#ifdef FEATURE_DATA_PS_SLIP
extern ds3g_siolib_portid_e_type slip_ser_port;
#include "slip_iface_ext.h"
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#include "diagcomm.h"
#endif /* FEATURE_RUNTIME_DEVMAP */

#ifdef FEATURE_DSAT_ETSI_DATA
#define FEATURE_DSM_WM_CB
#include "dsm.h"
#include "mn_cm_exp.h"
#include "dsatetsipkt.h"
#include "dsatetsictab.h"
#include "ps_in.h"
#ifndef FEATURE_DSAT_LEGACY_GCF_SUPPORT          
#include "ps_ip4_hdr.h"
#else  /* FEATURE_DSAT_LEGACY_GCF_SUPPORT */
/* If legacy GCF feature is defined, Q304Maint branch of MMDATA_PS is being
   used and provides no ps_ip4_hdr functionality. */
#include "ps_iputil.h"
#endif /* FEATURE_DSAT_LEGACY_GCF_SUPPORT */

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_WCDMA_PS
#include "rrcdata.h"
#include "wcdmamvsif.h"
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_GSM_GPRS
#include "gsndcp.h"
#endif /* FEATURE_GSM_GPRS */

#include "dsatetsime.h"

#include "vbatt.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"

#ifdef FEATURE_HDR_AT_TEST_ONLY
#endif /* FEATURE_HDR_AT_TEST_ONLY */
#ifdef FEATURE_DS_PSTATS
#include "dsatpstatsctab.h"
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_DSAT_GOBI_MAINLINE 
#include "ftm.h"
#include "ftm_diagpkt.h"
#include "gobi_nv.h"
#include "gobi_im_common.h"
#include "ftm_rf_cmd.h"
#include "bsp.h"
/* Variable to find out what command is being processed currently */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

#include "ds_rmnet_defs.h"
#include "ds_qmi_defs.h"
#include "ds_qmux.h"
#include "ds_profile.h"
#include "sd.h"
#include "ds3gsiolib_ex.h"

#include "lte_rrc_ext_api.h"
#include "lte_cphy_msg.h"

#ifdef FEATURE_DSAT_ATCOP_VEND
#include "lte_rrc_ext_msg.h"
#if defined(FEATURE_GSM)
#include "geran_grr_msg.h"
#endif
#ifdef FEATURE_FSK_TX
#include "msgr_geran.h"
#endif /* FEATURE_FSK_TX */
#endif

#include "sys_m_reset.h"
#include "ds_3gpp_rpm.h"
#include "ds_pdn_manager.h"
#include "ds_fwki.h"
#include "ds_sys.h"
#include "ds_sys_ioctl.h"
#include "ds_pdn_psm_hdlr.h"

#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_DSAT_ATCOP_VEND

dsatcmif_lte_tx_info_s_type dsat_lte_prev_tx_info;
#endif
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DATA_QCRMCALL
dsat_qcrmcall_s_type dsat_qcrmcall_info;
#endif /* FEATURE_DATA_QCRMCALL */

#ifdef FEATURE_DSAT_ATCOP_VEND

#define MAX_PARAM_BUF                  100
#define DEF_MAX_APN_DISABLE_VAL        1
#define DEF_MAX_PDN_CONN_PER_BLK       1023
#define DEF_MAX_PDN_REQ_WAIT_INTERVAL  1023
#define DEF_MAX_PDN_CONN_TIME          300 //300 seconds 
#define DEF_PDN_REQ_WAIT_TIME          0 //0 seconds
#define DEF_MAX_INACTIVITY_TIMER_VAL   122820
#define MAX_PDN_CONN_TIME              3600
#define MAX_MNC_MCC_VAL                999
#define MIN_OPRT_RESV_PCO              65280 /* FF00H to FFFFH reserved for operator specific use spec 3GPP TS 24.008 Table 10.5.154*/
#define MAX_OPRT_RESV_PCO              65535
#define DS_PROFILE_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED  0x36
#ifdef FEATURE_DSAT_ATCOP_VEND
LOCAL const dsat_string_item_type dsat_qcjdstate_mode_valstr [] =
{ "(0,7,9)" };
LOCAL const dsat_string_item_type dsat_qcceinfo_mode_valstr [] =
{ "(0,1)" };

#define CEINFO_MODE_DISABLE 0
#define CEINFO_MODE_ENABLE 1

#endif

typedef struct
{
  ds_profile_identifier_type          identifier;
  uint16                              len;
  dsat_num_item_type                  max_value;
}dsat_qcprofile_param_details_s_type;


dsat_qcprofile_param_details_s_type profile_qcpdpcfge_param_details_list[] = 
{
  {
    DS_PROFILE_APN_DISABLE_FLAG, 
    sizeof(boolean),
    DEF_MAX_APN_DISABLE_VAL
  },
  {
    DS_PROFILE_PDN_INACTIVITY_TIMEOUT,
    sizeof(uint32),
    DEF_MAX_INACTIVITY_TIMER_VAL
  },
  {
    DS_PROFILE_APN_CLASS,
    sizeof(uint8),
    DSAT_APN_CLASS_MAX
  },
  {
    DS_PROFILE_APN_BEARER,
    sizeof(uint8),
    255 //FLAG_ALL
  },
  {
    DS_PROFILE_MAX_PDN_CONN_PER_BLOCK,
    sizeof(uint16),
    DEF_MAX_PDN_CONN_PER_BLK
  },
  {
    DS_PROFILE_MAX_PDN_CONN_TIMER,
    sizeof(uint16),
    MAX_PDN_CONN_TIME
  },
  {
    DS_PROFILE_PDN_REQ_WAIT_TIMER,
    sizeof(uint16),
    DEF_MAX_PDN_REQ_WAIT_INTERVAL
  },
  {
    DS_PROFILE_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
    sizeof(boolean),
    1
  },
  /*Special case, check for min and max in the code*/
  {
    DS_PROFILE_OPERATOR_RESERVED_PCO,
    sizeof(uint16),
    MAX_OPRT_RESV_PCO
  },
  {
    DS_PROFILE_MCC,
    sizeof(uint16),
    MAX_MNC_MCC_VAL
  },
  {
    DS_PROFILE_MNC,
    sizeof(ds_profile_mnc_type),
    MAX_MNC_MCC_VAL
  }
}; /*profile_qcpdpcfge_param_details_list*/

#define DSAT_BANDPREF_NON_PERS 0
#define DSAT_BANDPREF_PERS 1

dsat_num_item_type dsat_bandpref_pers_val;
dsat_bandpref_e_type  dsat_bandpref_val[(uint32)DSAT_MAX_BANDPREF+1];

static uint32 full_image;
#define MAX_FOOL_BOOT_INDICATION 2

#endif

#define DSAT_MAX_PROFILE_PARAM_VAL_LEN 150
#define DSAT_MAX_PROFILE_PARAM_TYPE_LEN 20

#define MAX_MNC_MCC_VAL                 999
#define RANGE_END  0
#define MIN_OPRT_RESV_PCO             65280 /* FF00H to FFFFH reserved for operator specific use spec 3GPP TS 24.008 Table 10.5.154*/
#define MAX_OPRT_RESV_PCO             65535
#define DSAT_MAX_PDP_TYPE_NUM                 4
#define DSAT_MAX_PDP_TYPE_LEN                 8

typedef struct
{
  uint32 lower;
  uint32 upper;
}dsat_profile_range_s_type;

dsat_profile_range_s_type boolean_range[] =
{
  {FALSE,TRUE},
  {RANGE_END , RANGE_END}
};

dsat_profile_range_s_type access_ctrl_range[] =
{
  {DSAT_ACCESS_CONTROL_NONE,DSAT_ACCESS_CONTROL_PERMISSION},
  {RANGE_END , RANGE_END}
};

dsat_profile_range_s_type apn_class_range[]=
{
  {DSAT_APN_CLASS_ONE, DSAT_APN_CLASS_SIX},
  {DSAT_APN_CLASS_MAX, DSAT_APN_CLASS_MAX},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uint8_range[]=
{
  {0, 255},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uint32_range[]=
{
  {0, 0xFFFFFFFF},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uint16_range[]=
{
  {0, 0xFFFF},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type mnc_mcc_range[]=
{
  {0, MAX_MNC_MCC_VAL},
  {RANGE_END, RANGE_END}
};


dsat_profile_range_s_type auth_type_range[]=
{
  {DS_PROFILE_AUTH_NONE, DS_PROFILE_AUTH_CHAP_OR_PAP},
  {RANGE_END, RANGE_END}
};


dsat_profile_range_s_type pdp_header_comp_e_type_range[]=
{
  {DS_PROFILE_HEADER_COMP_OFF, DS_PROFILE_HEADER_COMP_RFC3095},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type data_comp_e_type_range[]=
{
  {DS_PROFILE_DATA_COMP_OFF, DS_PROFILE_DATA_COMP_V44},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type pdn_inactivity_to_range[]=
{
  {0, 71582}, /* Provided by 3GPP2*/
  {RANGE_END, RANGE_END}
};

/*This is max range of length*/
dsat_profile_range_s_type pdn_type_range[]=
{
  {DSAT_MAX_PDP_TYPE_LEN, DSAT_MAX_PDP_TYPE_LEN},
  {RANGE_END, RANGE_END}
};
dsat_profile_range_s_type op_pco_range[]=
{
  {MIN_OPRT_RESV_PCO, MAX_OPRT_RESV_PCO},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type max_v4_addr_len[]=
{
  {MAX_V4_ADDR_LEN , MAX_V4_ADDR_LEN},
  {RANGE_END, RANGE_END}
};


dsat_profile_range_s_type qcpdp_3gpp_str_len[]=
{
  {DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN + 1, DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN + 1},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type max_v6_addr_len[]=
{
  {MAX_IPADDR_STR_LEN, MAX_IPADDR_STR_LEN},
  {RANGE_END, RANGE_END}
};


const dsat_string_item_type dsat_pp_pdptype [5][8] ={ "IP", "PPP", "IPV6","IPV4V6","Non-IP" };

const dsat_string_item_type dsat_p2_pdptype [4][8] =
  { "IP", "IPV6","IPV4V6","" };

const dsat_string_item_type dsat_pp_failure_timers [11][10] =
  { "PPFLRTMR1", "PPFLRTMR2","PPFLRTMR3","PPFLRTMR4",
     "PPFLRTMR5","PPFLRTMR6","PPFLRTMR7","PPFLRTMR8",
     "PPFLRTMR9","PPFLRTMR10","" };

#ifdef FEATURE_DSAT_ATCOP_VEND
#ifdef FEATURE_DSAT_LTE 
#ifdef FEATURE_NBIOT_NTN
LOCAL const dsat_string_item_type dsat_qccellscan_rat_valstr [] =
{ "(0,7,9,10)" };
#else
LOCAL const dsat_string_item_type dsat_qccellscan_rat_valstr [] =
{ "(0,7,9)" };
#endif /* FEATURE_NBIOT_NTN */
#else
LOCAL const dsat_string_item_type dsat_qccellscan_rat_valstr [] =
{ "(0)" };
#endif /* FEATURE_DSAT_LTE  */
int  qccellscan_rat =-1;

dsat_profile_range_s_type apn_str_len[]=
{
  {DS_PROFILE_APN_STRING_LEN + 1, DS_PROFILE_APN_STRING_LEN + 1},
  {RANGE_END, RANGE_END}
};


typedef struct
{
  char*                               abbrv;
  uint64                              identifier;
  uint16                              len;
  value_type_enum_type                type;
  dsat_profile_range_s_type           *range;
}dsat_profile_parameter_details_s_type;


dsat_profile_parameter_details_s_type profile_3gpp_parameter_details_list[]=
{
  {  "APNNAME"          ,    DS_PROFILE_APN_NAME,
      0,    STR_TYPE,         &apn_str_len[0]  },

   {  "PDPTYPE"        ,    DS_PROFILE_TYPE, 
      0               ,    STR_TYPE,  &pdn_type_range[0] },

  {  "PDPHCOMP"          ,    DS_PROFILE_CONTEXT_H_COMP,
        sizeof(ds_profile_header_comp_e_type),  NUM_TYPE,  &pdp_header_comp_e_type_range[0
  ]  },
 
  {  "PDPDCOMP"          ,    DS_PROFILE_CONTEXT_D_COMP,
        sizeof(ds_profile_data_comp_e_type),    NUM_TYPE,  &data_comp_e_type_range[0]  },
        
  {  "V4DNSPR"          ,   DS_PROFILE_DNS_ADDR_V4_PRIMARY,
       sizeof(uint32),    ADDR_V4_TYPE,  &max_v4_addr_len[0]   },

  {  "V4DNSSE"          ,   DS_PROFILE_DNS_ADDR_V4_SECONDARY,
      sizeof(uint32),    ADDR_V4_TYPE,  &max_v4_addr_len[0]    },
      
  {  "PUID"          ,   DS_PROFILE_AUTH_USERNAME ,
       0  ,    STR_TYPE,    &qcpdp_3gpp_str_len[0] }, 

  {  "AUPWD"          ,   DS_PROFILE_AUTH_PASSWORD ,
       0  ,    STR_TYPE,    &qcpdp_3gpp_str_len[0] },

  {  "AUPROT"          ,   DS_PROFILE_AUTH_TYPE,
       sizeof(ds_profile_auth_enum_type),    NUM_TYPE,   &auth_type_range[0]  },

  {  "V6DNSPR"          ,   DS_PROFILE_DNS_ADDR_V6_PRIMARY,
      sizeof(ds_profile_addr_type_ipv6),    ADDR_P_V6_TYPE,  &max_v6_addr_len[0]   },
  
  {  "V6DNSSE"          ,   DS_PROFILE_DNS_ADDR_V6_SECONDARY,
        sizeof(ds_profile_addr_type_ipv6),    ADDR_P_V6_TYPE,  &max_v6_addr_len[0]  },

  {  "PCSCFDHCP"          ,    DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG,
        sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "ACCESSCTRL"          ,    DS_PROFILE_ACCESS_CTRL_FLAG,
        sizeof(ds_profile_access_control_e_type),    NUM_TYPE,  &access_ctrl_range[0]  },

  {  "IMCNFLAG"          ,    DS_PROFILE_IM_CN_FLAG,
       sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "APNFLAG"          ,   DS_PROFILE_APN_DISABLE_FLAG,
       sizeof(boolean),   NUM_TYPE,   &boolean_range[0] },

  {  "INACTTO"          ,    DS_PROFILE_PDN_INACTIVITY_TIMEOUT,
      sizeof(uint32),    NUM_TYPE,  &uint32_range[0]  },

  {  "APNCLASS"          ,   DS_PROFILE_APN_CLASS ,
      sizeof(uint8),    NUM_TYPE,    &apn_class_range[0] },

  {  "APNBR"          ,    DS_PROFILE_APN_BEARER,
      sizeof(uint8),    NUM_TYPE,  &uint8_range[0]  },

  {  "OPRPCO"      ,   DS_PROFILE_OPERATOR_RESERVED_PCO,
      sizeof(uint16),    NUM_TYPE,    &op_pco_range[0]},

  {  "PMCC"          ,   DS_PROFILE_MCC,
      sizeof(uint16),    NUM_TYPE,    &mnc_mcc_range[0]    },

  {  "PMNC"          ,   DS_PROFILE_MNC,
      sizeof(ds_profile_mnc_type),   NUM_TYPE,  &mnc_mcc_range[0]  },

  {  "PDNCOBLK"          ,    DS_PROFILE_MAX_PDN_CONN_PER_BLOCK,
      sizeof(uint16),    NUM_TYPE,  &uint16_range[0]  },

  {  "PDNDTMR"          ,    DS_PROFILE_PDN_DISCON_WAIT_TIME,
      sizeof(uint8),    NUM_TYPE,  &uint8_range[0]  },

  {  "PDNREQTMR"          ,    DS_PROFILE_PDN_REQ_WAIT_TIMER,
      sizeof(uint16),    NUM_TYPE,  &uint16_range[0]  },

  {  "RODISALLW"          ,    DS_PROFILE_ROAMING_DISALLOWED,
      sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "LTEROPDP"              ,    DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE, 
      0                      ,    STR_TYPE,  &pdn_type_range[0]  },

  {  "UMTSROPDP"             ,    DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE, 
       0                      ,    STR_TYPE,  &pdn_type_range[0]  },

  {  "OVRRIDEHOPDP"          ,    DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE, 
      0                      ,    STR_TYPE,  &pdn_type_range[0]  },

  {  "ATTCWOPDN"          ,    DS_PROFILE_PARAM_ATTACH_WO_PDN, 
        0                      ,    NUM_TYPE,  &boolean_range[0]  },

  {  "PDPV4"          ,    DS_PROFILE_PDP_ADDR,
       sizeof(uint32),    ADDR_V4_TYPE,  &max_v4_addr_len[0]  },

  {  "PDPV6"          ,    DS_PROFILE_PDP_ADDR,
      sizeof(ds_profile_addr_type_ipv6),    ADDR_P_V6_TYPE,  &max_v6_addr_len[0]  },

   /*Using PP+FLRTMR to differentiate between 3GPP2 failure timers and 3GPP PDN throttling timers*/
  {  (char *)dsat_pp_failure_timers[0]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[1]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[2]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[3]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[4]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[5]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[6]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[7]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[8]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[9]          ,    DS_PROFILE_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  }

     /*P+ <PARAM_NAME> END*/

};/*profile_3gpp_parameter_details_list*/
     
dsat_qcprofile_param_details_s_type profile_qcapne_param_details_list[] = 
{
  /*Argument 1*/
  {
    DS_PROFILE_APN_CLASS,
    sizeof(uint8),
    DSAT_APN_CLASS_MAX
  },
  /*Argument 5*/
  {
    DS_PROFILE_APN_DISABLE_FLAG, 
    sizeof(boolean),
    DEF_MAX_APN_DISABLE_VAL
  },
  /*Argument 6*/
  {
    DS_PROFILE_PDN_INACTIVITY_TIMEOUT,
    sizeof(uint32),
    DEF_MAX_INACTIVITY_TIMER_VAL
  }
}; /*profile_qcapne_param_details_list*/

#endif
#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDGEN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdgen command.
  at$qcdgen is used to send a specified quantity of PS data as IP packet(s) 
  over a primary PDP-IP context already activated by +CGACT command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if async processing to start data send in progress

SIDE EFFECTS
  Phone is powered off.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcdgen_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD; /* default function return */
  dsat_num_item_type cid = 0;             /* context ID */
  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Write Command */

    /* If <cid> parameter in range 1-DSAT_UMTS_MAX_PDP_PROFILE_NUM has not been
       provided exit with diag message and ERROR result code */
    if ( !VALID_TOKEN(0) ||
         ATOI_OK != dsatutil_atoi(&cid, tok_ptr->arg[0], 10) ||
         !DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
    {
      DS_AT_MSG1_HIGH("Invalid <cid> specified: %d",cid);
      result = DSAT_ERROR;
    }
    else
    {
        dsat_num_item_type num_of_bytes = 0;
        
        if ( !VALID_TOKEN(1) ||
             ATOI_OK != dsatutil_atoi(&num_of_bytes, 
                                      tok_ptr->arg[1], 
                                      10)||
                            num_of_bytes < DSAT_DGEN_MIN_DATA_LEN )
        {
          DS_AT_MSG1_HIGH("Invalid <data_len> specified: %d",num_of_bytes);
          result = DSAT_ERROR;
        }
        else
        {
          SET_PENDING(DSAT_VENDOR_QCDGEN_IDX ,0, DSAT_PENDING_TRUE)
          
          result = ds_3gpp_rmsm_atif_qcdgen_handler ( (uint32)cid,
                                                      (uint32)num_of_bytes,NULL);
          if(result != DSAT_ASYNC_CMD)
          {
            DS_AT_MSG0_HIGH("Unsuccessful Result from RMSM");
            SET_PENDING(DSAT_VENDOR_QCDGEN_IDX ,0, DSAT_PENDING_FALSE)
            result = DSAT_ERROR;
          }
        }
     }
   } /* End write command */
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* Test Command */
    result = DSAT_OK;
  }
  else  
  {
    /* Read Command */
    result = DSAT_ERROR;
  }

  return result;
} /* dsatvend_exec_qcdgen_cmd () */
#ifdef FEATURE_DSAT_ATCOP_VEND

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPIMSCFGE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpimscfge command.
  at$qcpdpimscfge is used to edit PDP profile registry . 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpdpimscfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{


  dsat_num_item_type       profile_id       = 0;
  dsat_num_item_type       flag_i_val[3]    = {0,0,0};
  boolean                  flag_val[3]      = {FALSE,FALSE,FALSE};
  dsat_error_info_s_type   err_info;
  
  ds_profile_type_s        profile_data;    
  ds_profile_ident_info    ident_info; 
  ds_profile_status_etype  profile_status = DS_PROFILE_RESULT_SUCCESS;
  ds_profile_list_stype   *profile_list =NULL;
  int                      i =0;
  ds_profile_type_s        payload;
  uint8                    profile_number;
  
  uint8   total_profiles = 0;
  uint64                mask =0;  

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  memset(&ident_info,0,sizeof(ds_profile_ident_info));
  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments. */
    if ( (tok_ptr->args_found > parse_table->compound) || (tok_ptr->args_found < 2) )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    if( !(VALID_TOKEN(0) && (VALID_TOKEN(1) || VALID_TOKEN(2) || VALID_TOKEN(3))))
    {
      err_info.errval = DSAT_ERR_INVALID_TOKENS;
      goto send_error;
    }
    if((ATOI_OK ==dsatutil_atoi(&profile_id,tok_ptr->arg[0],10)) && 
        ( DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id)))
    {
      if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                      (profile_id,&profile_data))
      {      
         DS_AT_MSG1_HIGH("Profile not present first create %d",profile_id);
          return DSAT_ERROR;
      }
      if(profile_data.context.pdp_type == DS_PROFILE_PDP_NON_IP)
      {
        return DSAT_ERROR;
      }
     
       mask = profile_data.mask;
      if(VALID_TOKEN(2) && (ATOI_OK ==dsatutil_atoi((dsat_num_item_type*)&flag_i_val[1],tok_ptr->arg[2],10)))
      {
        if(0 == flag_i_val[1] || 1 == flag_i_val[1])
        {      
          mask = mask|DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG;
          profile_data.request_pcscf_address_flag = flag_i_val[1];
        }
        else
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;          
          err_info.arg_num = 2;
          goto send_error;
         }
      }
      if(VALID_TOKEN(3) && (ATOI_OK ==dsatutil_atoi((dsat_num_item_type*)&flag_i_val[2],tok_ptr->arg[3],10)))
      {
        if(0 == flag_i_val[2] || 1 == flag_i_val[2])
        {
           mask = mask|DS_PROFILE_IM_CN_FLAG;
           profile_data.im_cn_flag = flag_i_val[2];
        }
        else
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;          
          err_info.arg_num = 3;
          goto send_error;
        }
      }
      if(mask !=0)
      {
         profile_status = ds_profile_create_ex(ident_info,
                                                   mask,
                                                   &profile_data,
                                                   profile_id
                                                  );
         if(profile_status != DS_PROFILE_RESULT_SUCCESS)
         {
            return DSAT_ERROR;
          }
        }
    }
    else
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                               "$QCPDPIMSCFGE: (%d-%d),(0-1),(0-1),(0-1)\n",
                                                DSAT_LEGACY_PROFILE_NUM_MIN,
                                                DSAT_LEGACY_PROFILE_NUM_MAX
                                                );
    
  }
  else  if(tok_ptr->op == (NA|QU))
  {
    res_buff_ptr->used = 0;
    
    /*----------------------------------------------------*/
    DS_SYSTEM_HEAP_MEM_ALLOC(profile_list, (sizeof(ds_profile_list_stype)*
                             DS_PROFILE_MAX_PROFILE_NUM), ds_profile_list_stype*);
   
     if(profile_list ==NULL)
     {
       DS_ASSERT(0);
     }
     memset(&payload,0,   sizeof(ds_profile_type_s));
     memset(profile_list,0,(sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM));

    total_profiles = ds_profile_get_profile_list(profile_list);
    
    for(i = 0; i < total_profiles; i++)
    {
      memset(&ident_info ,0,sizeof(ds_profile_ident_info));
      ident_info.pdp_type = profile_list[i].pdp_type;   
      if(profile_list[i].apn[0] !='\0')
      {
        memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                profile_list[i].apn, sizeof(profile_list[i].apn ));
      }
      profile_status = ds_profile_get_param_ex(ident_info,&payload,&profile_number);
      if (payload.context.pdp_type == DS_PROFILE_PDP_NON_IP)
      {
        continue;
      }
      res_buff_ptr->used += 
            (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                    res_buff_ptr->size - res_buff_ptr->used,
                                   "$QCPDPIMSCFGE: %d , %d , %d , %d\n",
                                    profile_number,
                                    flag_val[0],
                                    payload.request_pcscf_address_flag,
                                    payload.im_cn_flag);

    }
    if(0 == res_buff_ptr->used )
    {
      res_buff_ptr->used = 
         (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                 res_buff_ptr->size,
                                 "$QCPDPIMSCFGE: ");
    }
    DS_SYSTEM_HEAP_MEM_FREE(profile_list);

  }
  else
  {
   err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return DSAT_OK;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_ARGUMENT )
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  else
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  return DSAT_ERROR;

} /*dsatvend_exec_qcpdpimscfge_cmd*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPCFGE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpcfge command.
  at$qcpdpcfge is used to edit APN values in APN table. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcpdpcfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type                        profile_id    = 0;
  dsat_num_item_type                        profile_params    = 0;
  dsat_error_info_s_type                    err_info;
  dsat_qcprofile_param_details_s_type *param_list = &profile_qcpdpcfge_param_details_list[0];
  boolean             valid_status = FALSE;
  uint8               token_idx = 0;
  dsat_num_item_type  buf_curr_len = 0;
  dsat_num_item_type  min_val = 0;
  char                profile_param_data[MAX_PARAM_BUF];
  uint64              mask =0;
  ds_profile_ident_info ident_info ;  
  ds_profile_type_s        profile_data;    
  ds_profile_status_etype  profile_status = DS_PROFILE_RESULT_SUCCESS;
  ds_profile_list_stype   *profile_list =NULL;
  uint8                    profile_count =0;
  int                      i =0;
  ds_profile_type_s        payload;
  uint8                    profile_number;

  memset(&ident_info ,0,sizeof(ds_profile_ident_info));
  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments.*/
    if ( tok_ptr->args_found > parse_table->compound)
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

/* Profile validation :- Arg 0 should present and have valid value*/
    VALID_NUM_TOKEN(TOKEN_IDX_0,0,DSAT_LEGACY_PROFILE_NUM_MAX, &profile_id)
    if (valid_status && DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id))
    {
       if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                      (profile_id,&profile_data))
      {
        DS_AT_MSG1_HIGH("Profile not present first create %d",profile_id);
        err_info.errval  = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 0;
        goto send_error;
      }
    }
    else
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }

    if(profile_data.context.pdp_type == DS_PROFILE_PDP_NON_IP)
    {
      /* configuration on NON IP pdp type is not supported */
      err_info.errval = DSAT_ERR_NO_VALID_PROFILE;
      err_info.arg_num = 0;
      goto send_error;
    }
    if(VALID_TOKEN(8))
    {
     /* configuration on NON IP pdp type is not supported */
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 0;
        DS_AT_MSG0_HIGH("Emergency flag is not supported ");
        goto send_error;
    }
    mask = profile_data.mask;
    for (i=0;i<ARR_SIZE(profile_qcpdpcfge_param_details_list);i++)
    {
      token_idx      = i+1;
      min_val        = 0;
      profile_params = 0;
        if(token_idx == TOKEN_IDX_8)
      {
       continue;
      }
	  
      /*Special case of reserved pco where min is not 0*/
      if (token_idx == TOKEN_IDX_9)
      {
        min_val = MIN_OPRT_RESV_PCO;           
      }

      VALID_NUM_TOKEN_EX(token_idx, min_val, param_list[i].max_value,&profile_params,
                          param_list[i].identifier)
     
      if (valid_status)
      {
        if(token_idx == TOKEN_IDX_11)
        {
          profile_data.mnc.mnc_includes_pcs_digit = FALSE;
          if(PLMN_STR_MNC_LEN == strlen((char*)tok_ptr->arg[10]))
          {
           profile_data.mnc.mnc_includes_pcs_digit = TRUE;
          }
          profile_data.mnc.mnc_digits = (uint16)profile_params;          
        }
        if(token_idx == TOKEN_IDX_1)
        {
          profile_data.apn_disable_flag = profile_params;          
        }
        if(token_idx == TOKEN_IDX_2)
        {
            profile_data.inactivity_timer_val = profile_params;            
        }
        if(token_idx == TOKEN_IDX_3)
        {
            profile_data.apn_class = profile_params;            
        }
        if(token_idx == TOKEN_IDX_4)
        {
            profile_data.apn_bearer = profile_params;            
        }
        if(token_idx == TOKEN_IDX_5)
        {
            profile_data.max_pdn_conn_per_blk = profile_params;            
        }
       if(token_idx == TOKEN_IDX_6)
       {
            profile_data.max_pdn_conn_time = profile_params;                                    
        }
        if(token_idx == TOKEN_IDX_7)
        {
           profile_data.pdn_req_wait_time = profile_params;           
        }
        if (token_idx == TOKEN_IDX_9)
        {
            profile_data.operator_reserved_pco = profile_params;            
        }
        if(token_idx == TOKEN_IDX_10)
        {
            profile_data.mcc = profile_params;            
        }
      }
      
     }
    if(mask !=0)
    {
        profile_status = ds_profile_create_ex(ident_info,
                                                   mask,
                                                  &profile_data,
                                                   profile_id
                                                   );
      }
     if(profile_status != DS_PROFILE_RESULT_SUCCESS)
     {
       DS_AT_MSG1_ERROR("profile_status exited with ERROR %d",profile_status);
       return DSAT_ERROR;
     }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
  
/*  
  APN bearer is a bit mask and the input can be a combination of bit masks below. 
  */
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_G   (0x1)   
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_W   (0x2)  
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_L   (0x4)   
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_T   (0x8)
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL (0xff)  

 /* Hence the allowed range is 0x1 - 0xF(G+W+L+T) and 0xFF for all.
  
*/
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCPDPCFGE: (%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d,%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)\n",
                                               DSAT_LEGACY_PROFILE_NUM_MIN ,
                                               DSAT_LEGACY_PROFILE_NUM_MAX,
                                               0,1,
                                               0,DEF_MAX_INACTIVITY_TIMER_VAL,
                                               DSAT_APN_CLASS_ONE,DSAT_APN_CLASS_MAX,
                                               DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_G,
                                               15,DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL,
                                               0,DEF_MAX_PDN_CONN_PER_BLK,
                                               0,MAX_PDN_CONN_TIME,
                                               0,DEF_MAX_PDN_REQ_WAIT_INTERVAL,
                                               0,1,
                                               MIN_OPRT_RESV_PCO,MAX_OPRT_RESV_PCO,
                                               0,MAX_MNC_MCC_VAL,
                                               0,MAX_MNC_MCC_VAL);


  }
  else if(tok_ptr->op == (NA|QU))
  {

    res_buff_ptr->used = 0;
     DS_SYSTEM_HEAP_MEM_ALLOC(profile_list, (sizeof(ds_profile_list_stype)*
                             DS_PROFILE_MAX_PROFILE_NUM), ds_profile_list_stype*);
   
     if(profile_list ==NULL)
     {
       DS_ASSERT(0);
     }
     memset(&payload,0,   sizeof(ds_profile_type_s));
     memset(profile_list,0,(sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM));

    profile_count = ds_profile_get_profile_list(profile_list);
    for ( i = 0; i < profile_count; i++)
     {    
        memset(&ident_info ,0,sizeof(ds_profile_ident_info));
      ident_info.pdp_type = profile_list[i].pdp_type;
 
      if(profile_list[i].apn[0] !='\0')
      {
        memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                profile_list[i].apn, sizeof(profile_list[i].apn ));
      }
      
      profile_status = ds_profile_get_param_ex(ident_info,&payload,&profile_number);
      if (payload.context.pdp_type == DS_PROFILE_PDP_NON_IP)
      {
        continue;
      }
      /*Reset profile parameter data and length*/
      memset(&profile_param_data[0],0x0,MAX_PARAM_BUF);
      buf_curr_len = 0;

      res_buff_ptr->used += 
             (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                              res_buff_ptr->size - res_buff_ptr->used,
                              "$QCPDPCFGE: %d, %d, %lu, %d, %d, %d, %d, %d, %d ,%d ,%d, %d\n",
                              profile_number,
                              payload.apn_disable_flag,
                             payload.inactivity_timer_val,
                             payload.apn_class,
                             payload.apn_bearer,
                             payload.max_pdn_conn_per_blk,
                             payload.max_pdn_conn_time,
                             payload.pdn_req_wait_time,
                             0,
                             payload.operator_reserved_pco,
                             payload.mcc,
                              payload.mnc.mnc_digits
                             );
      
    }
    if(0 == res_buff_ptr->used )
    {
       res_buff_ptr->used = 
             (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                              res_buff_ptr->size,
                              "$QCPDPCFGE: ");
    }
    
    DS_SYSTEM_HEAP_MEM_FREE(profile_list);
    
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return DSAT_OK;

send_error:
  DS_AT_MSG2_ERROR("Error:%d, Arg:%d",err_info.errval, err_info.arg_num);
  return DSAT_ERROR;
}/*dsatvend_exec_qcpdpcfge_cmd*/

#endif
#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPWRDN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpwrdn command.
  at$qcpwrdn is used to power down the terminal.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Phone is powered off.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcpwrdn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* function return                    */
  dsat_result_enum_type result = DSAT_OK;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
     if(TRUE == cm_rpm_check_reset_allowed())
    {
     sys_m_initiate_poweroff();
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_qcpwrdn_cmd () */

#ifdef FEATURE_DATA_QCRMCALL
/*===========================================================================
FUNCTION   DSATVEND_IFACE_EV_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on external interface.  COMING UP, DOWN and CONFIG iface events are
  registered, and affect the DUN call info message and indication behaviour.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dsatvend_rmnet_ev_cb
(
   rmnet_qcrmcall_action_e_type  action,
   rmnet_qcrmcall_result_type  * result
)
{
  ds_command_type            *cmd_buf     = NULL;
  dsat_rmnet_info_s_type *rmnet_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_AT_MSG1_HIGH("Received RmNet Action Event [%d]", action);

  switch( action )
  {
    case QCRMCALL_ACTION_START:
    case QCRMCALL_ACTION_STOP:
    case QCRMCALL_ACTION_QUERY:
    {
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_rmnet_info_s_type), FALSE);

      rmnet_info_ptr = cmd_buf->cmd_payload_ptr;
      rmnet_info_ptr->action = (uint8) action;
      dsatutil_memscpy(&rmnet_info_ptr->result,
                       sizeof(rmnet_qcrmcall_result_type),
                       result,
                       sizeof(rmnet_qcrmcall_result_type));

      /* send the message to ATCOP */
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_RMNET_EV_CMD;
        
      ds_put_cmd(cmd_buf);
      break;
    }
    case QCRMCALL_ACTION_NONE:
    default:
    {
      DS_AT_MSG1_ERROR("Unregistered event %d recvd, ignoring", action);
      break;
    }
  } /* switch( event ) */
} /* dsatvend_rmnet_ev_cb() */

/*===========================================================================
FUNCTION   DSATVEND_IFACE_EV_HANDLER()

DESCRIPTION
 This is a handler function for all the IFACE releated events. ATCoP is intrested in IFACE COMING UP, DOWN,
 CONFIGURE events. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatvend_rmnet_ev_handler
(
  ds_command_type * cmd_ptr     /* DS Command pointer */
)
{

  dsat_result_enum_type result = DSAT_OK;
  rmnet_qcrmcall_result_type  * result_rmnet = NULL;
  rmnet_qcrmcall_action_e_type  action;
  dsm_item_type *res_buff_ptr = NULL;           /*  Place to put response       */
  dsat_rmnet_info_s_type *rmnet_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
  boolean status        = FALSE;
  uint8 qcrm_inst       = 0;
  uint8 ip_type         = 0;
  dsat_num_item_type  i = 0;

  result_rmnet = (rmnet_qcrmcall_result_type  *)  &rmnet_info_ptr->result;
  action       = (rmnet_qcrmcall_action_e_type)   rmnet_info_ptr->action;

  status    = result_rmnet->sni.status;
  qcrm_inst = result_rmnet->sni.qcrmcall_inst;
  ip_type   = result_rmnet->sni.ip_family;

  DS_AT_MSG1_MED("Recvd action cmd :  %d", action);

  DS_AT_MSG3_MED("QCRMCALL status:%d , inst: %d, ip_type: %d", status,qcrm_inst, ip_type);
  
  switch (action)
  {
     case QCRMCALL_ACTION_QUERY: 

       dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE; /*Reset Pending call state*/
       
       if (result_rmnet->list.num_qcrmcall == 0)
       {
          result = DSAT_OK;
       }
       else if(result_rmnet->list.num_qcrmcall > RMNET_QCRMCALL_MAX)           
       {         
         result = DSAT_ERROR;
       }
       else
       {
         res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);

         for (i = 0; i < result_rmnet->list.num_qcrmcall; i++)
         {
           qcrm_inst = result_rmnet->list.qcrmcall[i].qcrmcall_inst;
           ip_type = result_rmnet->list.qcrmcall[i].ip_family;
           
           res_buff_ptr->used += (word)snprintf(
                              (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                              res_buff_ptr->size -  res_buff_ptr->used,
                              "$QCRMCALL: %d",(int)(qcrm_inst+1));

           if (ip_type == 4)
           {
             res_buff_ptr->used += (word)snprintf(
                                (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                                res_buff_ptr->size -  res_buff_ptr->used,
                                ", V4\n");
           }
           else if (ip_type == 6)
           {
             res_buff_ptr->used += (word)snprintf(
                                   (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                                   res_buff_ptr->size -  res_buff_ptr->used,
                                   ", V6\n");
           }
           else
           {
             DS_AT_MSG1_ERROR("Invalid Data with IP_Family: %d", ip_type);
           }
         }
         dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
       }
       break;
     case   QCRMCALL_ACTION_STOP:

       switch(ip_type)
       {
         case QMI_AF_INET:
          {
            if (status)
            {
              /*Increment instance count as we output Rmnet_inst + 1*/
              dsat_qcrmcall_info.v4_instance =  qcrm_inst+1;
            }
            else
            {
              /*Reset V4 instance in case of failure*/
              dsat_qcrmcall_info.v4_instance = 0;
            }
            /*Reset pending stop state for IPV4*/
            dsat_qcrmcall_info.state &= ~DSAT_PENDING_QCRMCALL_STOP_V4;
          }
          break;
          case QMI_AF_INET6:
            {
              if (status)
              {
                dsat_qcrmcall_info.v6_instance =  qcrm_inst+1;
              }
              else
              {
                dsat_qcrmcall_info.v6_instance = 0;
              }
              /*Reset pending stop state for IPV6*/
              dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_STOP_V6;
           }
          break;
          default:
            {
              memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
            }
            break;
       }

       /*Return error as we recieved invalid status (call failures) from RmNet*/
       if(dsat_qcrmcall_info.state ==  DSAT_PENDING_QCRMCALL_NONE)
       {
         if(dsat_qcrmcall_info.v4_instance == 0 &&
            dsat_qcrmcall_info.v6_instance == 0)
         {
           result = DSAT_ERROR;
         }
         else
         {
           memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
           result = DSAT_OK;
         }
       }
       else
       {
         result = DSAT_ASYNC_CMD;
       }
       break;

     case QCRMCALL_ACTION_START:
       switch(ip_type)
       {
         case QMI_AF_INET:
          {
            if (status)
            {
              dsat_qcrmcall_info.v4_instance =  qcrm_inst + 1;
            }
            else
            {
              dsat_qcrmcall_info.v4_instance = 0;
            }
            /*Reset pending start state for IPV4*/
            dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_START_V4;
            break;
          }
          case QMI_AF_INET6:
            {
              if (status)
              {
                dsat_qcrmcall_info.v6_instance =  qcrm_inst + 1;
              }
              else
              {
                dsat_qcrmcall_info.v6_instance = 0;
              }

              /*Reset pending start state for IPV6*/
              dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_START_V6;
              break;
            }

          default:
            memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
            DS_AT_MSG1_ERROR("Invalid Data with IP_Family: %d", ip_type);
          break;
       }
       if(dsat_qcrmcall_info.state ==  DSAT_PENDING_QCRMCALL_NONE)
       {
         res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

         if (dsat_qcrmcall_info.v4_instance != 0)
         {
           res_buff_ptr->used = snprintf((char *)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "$QCRMCALL: %d, V4", (qcrm_inst)+1);
           DS_AT_MSG0_HIGH(" QCRMCALL V4 Connected");
         }
         if (dsat_qcrmcall_info.v6_instance != 0)
         {
           if(dsat_qcrmcall_info.v4_instance != 0)
           {
             res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                        res_buff_ptr->size,
                                        "\n");
           }
           res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                        res_buff_ptr->size,
                                        "$QCRMCALL: %d, V6", (qcrm_inst)+1);             
           DS_AT_MSG0_HIGH(" QCRMCALL V6 Connected");
         }

         /*Reset No_Carrier as no call was brought up by RmNet*/
         if(dsat_qcrmcall_info.v4_instance == 0 &&
            dsat_qcrmcall_info.v6_instance == 0)
         {
           result = DSAT_NO_CARRIER;
         }
         else
         {
           memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
           dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
           result = DSAT_OK;
         }
       }
       else
       {
         result = DSAT_ASYNC_CMD;
       }
       break;

     case QCRMCALL_ACTION_NONE:
     default:
       DS_AT_MSG0_MED("Igorning other events");
       break;
  }

  return result;
}
/*===========================================================================
FUNCTION   DSAT_GET_DEFAULT_PROFILE_NUMBER

DESCRIPTION
  This function is used to retreive default profile number. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsat_get_default_profile_number
(
  uint8                        profile_type,
  uint8                        profile_family,
  uint16                      *default_profile_num
)
{
  ds_profile_status_etype           profile_status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  profile_status = DS_PROFILE_RESULT_SUCCESS;
  *default_profile_num = 0;

  /*-------------------------------------------------------------------------
                       Call the API to get the default profile number.
    -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_get_default_profile_num((ds_profile_tech_etype)profile_type, 
                                        (uint32)profile_family,
                                        default_profile_num);
  if (profile_status != DS_PROFILE_RESULT_SUCCESS && 
      profile_status != DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE)
  {
    DS_AT_MSG2_ERROR ("Get Default Profile for type %d failed. Error %d", 
               profile_type, profile_status);
  }

}

/*===========================================================================

FUNCTION QCRMCALL_VALIDATE_PARAMS

DESCRIPTION
  This functions validates the parameters received. 

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean qcrmcall_validate_input_params
(
  const tokens_struct_type *tok_ptr, 
  rmnet_meta_sm_network_info_type *sni_params
)
{
  #define QCRMCALL_ACT_3GPP2  1
  #define QCRMCALL_ACT_3GPP   2
  #define QCRMCALL_IP_V4      1
  #define QCRMCALL_IP_V6      2
  #define QCRMCALL_IP_V4V6    3
  #define QCRMCALL_NON_IP     4 

  dsat_num_item_type  temp_data = 0;
  uint16  def_profile_num = 0;
  uint32  len = 0;
  uint8   iptype = 0;
/*------------------------------------------------------------------------*/

  DS_ASSERT(NULL != sni_params );

  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    /* Set Defaults */
     dsat_get_default_profile_number(DS_PROFILE_TECH_3GPP,
                                     DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY,
                                     &def_profile_num);
    
     sni_params->umts_profile_num = def_profile_num;

     dsat_get_default_profile_number(DS_PROFILE_TECH_3GPP2,
                                     DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY,
                                     &def_profile_num);

    sni_params->cdma_profile_num = (uint8) def_profile_num;

    sni_params->tech_pref = WWAN_GROUP;
    sni_params->ip_family_pref = QMI_AF_INET;

    /* Retreive optional parameter IP type */
    if ( VALID_TOKEN(2))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[2], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      if ( (QCRMCALL_NON_IP < temp_data) || (temp_data < QCRMCALL_IP_V4) )
      {
        DS_AT_MSG1_ERROR("Invalid IP Type received [%d]",temp_data);
        return FALSE; 
      }
      switch (temp_data)
      {
        case QCRMCALL_IP_V4:         /* IPv4        */
          sni_params->ip_family_pref = QMI_AF_INET;
          break;          
        case QCRMCALL_IP_V6:        /* IPv6        */
          sni_params->ip_family_pref = QMI_AF_INET6;
          break;
        case QCRMCALL_IP_V4V6:       /* IPV4V6 */
          sni_params->ip_family_pref = QMI_AF_INET_4_6;
          break;
        case QCRMCALL_NON_IP:        /* NON IP */
          sni_params->ip_family_pref = QMI_AF_INET;
          break;
        default:
          sni_params->ip_family_pref = QMI_AF_INET;
      }
    }

    iptype = temp_data;
    /* Retreive Tech Preference */
    if ( VALID_TOKEN(3))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[3], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      if ( QCRMCALL_ACT_3GPP2 == temp_data )
      {
        sni_params->tech_pref = CDMA_SN_IFACE;
        if (iptype == QCRMCALL_NON_IP)
        {
          DS_AT_MSG1_ERROR("QCRMCALL on NON-IP pdp type is not supported on tech type %d",
                            temp_data);
        }
      }
      else if ( QCRMCALL_ACT_3GPP == temp_data )
      {
        /* If QCRMCALL is requested on NON-IP (4) family preference 
         * then we mark tech_pref as NON_IP_IFACE and for all
         * other IP family prefence UMTS_IFACE is used.
         */ 
        if (iptype == QCRMCALL_NON_IP)
        {
          sni_params->tech_pref = NON_IP_IFACE;
        }
        else
        {
          sni_params->tech_pref = UMTS_IFACE;          
        }
      }
      else
      {
        DS_AT_MSG1_ERROR("Invalid Tech Pref [%d]", temp_data);
        return FALSE;
      }
    }
    /* Retreive optional parameter UMTS Profile number */
    if ( VALID_TOKEN(4))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[4], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      if ( (temp_data == 0 ) || 
           (!DSAT_IS_PROFILE_ID_WITHIN_RANGE(temp_data)) )
      {
        DS_AT_MSG1_ERROR("Invalid UMTS Profile [%d]", temp_data);
        return FALSE;
      }
      sni_params->umts_profile_num = temp_data;
    }
    /* Retreive optional parameter CDMA Profile number */
    if ( VALID_TOKEN(5))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[5], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      sni_params->cdma_profile_num = temp_data;
    }
    /* Retreive optional parameterAPN Name */
    if ( VALID_TOKEN(6))
    {
      len = strlen((char *)tok_ptr->arg[6]);
      if ( len > ( 2 + DS_PROFILE_APN_STRING_LEN ))
      {
        DS_AT_MSG1_ERROR("APN is too long [%d]",len);
        return FALSE;
      }
      if ( '"' == *tok_ptr->arg[6]) /* Strip Quotes */
      {
        if(!dsatutil_strip_quotes_out((const byte*)tok_ptr->arg[6],
                                      (byte*)tok_ptr->arg[6],(word) len))
        {
          return FALSE;        
        }
        (void) dsatutil_memscpy((void*)sni_params->apn.name,255,
                                (void*)tok_ptr->arg[6],(len-2));
        
        sni_params->apn.length = (uint8) len - 2;
      }
      else
      {
        return FALSE;
      }

    }
    return TRUE;
    
  }

  return FALSE;
}/* qcrmcall_validate_input_params */

/*===========================================================================

FUNCTION DSATVEND_QCRMCALL_ABORT_CMD_HANDLER

DESCRIPTION
  This function is an abort command handler, which stops the current originate 
  request which if it is still in progress. 

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatvend_qcrmcall_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
  rmnet_qcrmcall_request_type      action_request;
  dsat_num_item_type               temp_instance_v4;
  dsat_num_item_type               temp_instance_v6;

  memset(&action_request, 0x0 ,sizeof(action_request));

  temp_instance_v4 = dsat_qcrmcall_info.v4_instance - 1;
  temp_instance_v6 = dsat_qcrmcall_info.v6_instance - 1;

  /*Should not abort Stop command*/
  if ((dsat_qcrmcall_info.state & DSAT_PENDING_QCRMCALL_STOP_V4) ||
      (dsat_qcrmcall_info.state & DSAT_PENDING_QCRMCALL_STOP_V6))
  {
    return FALSE;
  }

  /* Check if a call is still in pending state, then tear it down*/
  else if ( dsat_qcrmcall_info.state != DSAT_PENDING_QCRMCALL_NONE )
  {
    if (dsat_qcrmcall_info.v4_instance != 0)
    {
      action_request.stop.qcrmcall_inst = temp_instance_v4;
      action_request.stop.ip_family      = QMI_AF_INET;
      dsat_qcrmcall_info.state          |= DSAT_PENDING_QCRMCALL_STOP_V4; /*Set pending stop state for IPV4*/

      if( !(rmnet_send_qcrmcall_request
          (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
      {
        dsat_qcrmcall_info.state         &= ~DSAT_PENDING_QCRMCALL_STOP_V4;
        DS_AT_MSG1_HIGH(" ABORT QCRMCall on instance %d failed on IPV4 ",temp_instance_v4);
        return FALSE;
      }
    }
    if (dsat_qcrmcall_info.v6_instance != 0 )
    {
      action_request.stop.qcrmcall_inst = temp_instance_v6;
      action_request.stop.ip_family      = QMI_AF_INET6;  /*Set pending stop state for IPV6*/
      dsat_qcrmcall_info.state          |= DSAT_PENDING_QCRMCALL_STOP_V6;

      if( !(rmnet_send_qcrmcall_request
          (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
      {
        dsat_qcrmcall_info.state         &= ~DSAT_PENDING_QCRMCALL_STOP_V6;
        DS_AT_MSG1_HIGH(" ABORT QCRMCall on instance %d failed on IPV6 ",temp_instance_v6);
        return FALSE;
      }
    }
  }

  return TRUE;
  
}/* dsatvend_qcrmcall_abort_cmd_handler */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRMCALL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCRMCALL command.
  $QCRMCALL command is used to trigger a RmNet call via AT command. 
  It just passes on the Start Network Interface parameters into RmNet to
  bring up the call. 

  $QCRMCALL =<action>, <instance>, [<IP type>, <[Act],<umts_profile>,<cdma_profile>,<APN>]

   Response:
   $QCRMCALL:<Instance>, <V4>
   $QCRMCALL:<Instance>, <V6> 
   OK

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:    syntax error.
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrmcall_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{  
#define DSAT_QCRMCALL_ACTION_START 1
#define DSAT_QCRMCALL_ACTION_STOP  0

  rmnet_meta_sm_network_info_type  *sni_params_ptr = NULL;
  dsat_num_item_type               temp_instance = 0;
  rmnet_qcrmcall_request_type      action_request;
  dsat_num_item_type               action = 0;
  dsat_error_info_s_type   err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
/* -----------------------------------------------------------------------------*/


  /*========================Few Basic Concepts ======================================
     * State Machine and Code Flow
     * We support bringing up IPv4/IPV6 calls on multi-instances of RmNet
     * We also support DUAL-IP over single RmNet (means two calls on same RmNet instance) 
     * We mantain our own state machine for bringing up RmNet call's 
     *  
     * State Machine 
     *  
     * at$qcrmcall=1,1,1 (Means Start IPV4 call on RmNet instance 0) 
     *  
     *  Example of State Transitions for QCRMCALL on IPv4-
     *  
     *  DSAT_PENDING_QCRMCALL_NONE ------------> DSAT_PENDING_QCRMCALL_START_V4
     *  Wait for Callback from RmNet
     *  DSAT_PENDING_QCRMCALL_START_V4 ---------> DSAT_PENDING_QCRMCALL_NONE
     *  
     *  IPV6 Call
     *  
     *  DSAT_PENDING_QCRMCALL_NONE ------------> DSAT_PENDING_QCRMCALL_START_V6
     *  Wait for Callback from RmNet
     *  DSAT_PENDING_QCRMCALL_START_V6 ---------> DSAT_PENDING_QCRMCALL_NONE
     *  
     *  IPV4-IPV6 - Here we bring two calls up on same RmNet instance
     *  
     *  DSAT_PENDING_QCRMCALL_NONE ------------> DSAT_PENDING_QCRMCALL_START_V4
     *  DSAT_PENDING_QCRMCALL_START_V4---------> DSAT_PENDING_QCRMCALL_START_V6
     *                               |---------->DSAT_PENDING_QCRMCALL_START_V4
     *  Wait for Callback from RmNet
     *  DSAT_PENDING_QCRMCALL_START_V4 ---------> DSAT_PENDING_QCRMCALL_NONE
     *  DSAT_PENDING_QCRMCALL_START_V6---------------------------|
     *  
    =============================================================================  */
  /* Validate Input Params for Write command */
   memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));

  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    
    /* Action Param; Mandatory */ 
    err_info.arg_num = 0;
    if ( VALID_TOKEN(0))
    {
      if ( dsatutil_atoi( &action, 
                       tok_ptr->arg[0], 10) != ATOI_OK )
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        goto send_error;
      }
  
      if( (DSAT_QCRMCALL_ACTION_START != action ) &&
          (DSAT_QCRMCALL_ACTION_STOP != action  ))
      {
        err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
        goto send_error;
      }
    }
    else
    { /* Action is mandatory */
      err_info.errval = DSAT_ERR_PARAMETER_MANDATORY;
      goto send_error;
    }

    err_info.arg_num = 1;
    if ( VALID_TOKEN(1))
    {
      if ( ( dsatutil_atoi( &temp_instance, 
                       tok_ptr->arg[1], 10) != ATOI_OK ) || ( temp_instance == 0))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        goto send_error;
      }
      sni_params_ptr = (rmnet_meta_sm_network_info_type *)dsat_alloc_memory(sizeof(rmnet_meta_sm_network_info_type),FALSE);
      if ( FALSE == qcrmcall_validate_input_params( tok_ptr, sni_params_ptr) )
      {
        DS_AT_MSG0_ERROR("Invalid Params received; cannot proceed further");
        dsatutil_free_memory(sni_params_ptr);
        return DSAT_ERROR;
      }


      /* Here we are making the RmNet instance to be 1 less as Rmnet instance start from 0*/
     temp_instance = temp_instance - 1;

    if ( DSAT_QCRMCALL_ACTION_START == action )
    {
      sni_params_ptr->call_type = RMNET_CALL_TYPE_LAPTOP; 
      action_request.start.sni_params = sni_params_ptr;
      action_request.start.qcrmcall_inst = RMNET_INSTANCE_MAX;

      DS_AT_MSG2_HIGH(" QCRMCall START inst:%d IPtype: %d ",temp_instance,sni_params_ptr->ip_family_pref);
      
      switch ( sni_params_ptr->ip_family_pref )
      {
        case QMI_AF_INET_4_6:
          {
            /*Setting Parameters recieved as Start_NWK_Interface Params for RmNet call*/

            action_request.start.qcrmcall_inst              = temp_instance; 
            action_request.start.sni_params->ip_family_pref = (uint8) QMI_AF_INET;
            dsat_qcrmcall_info.state                        = DSAT_PENDING_QCRMCALL_START_V4; /*Setting V4_STATE_START*/
            dsat_qcrmcall_info.v4_instance                  = action_request.start.qcrmcall_inst + 1;
            
            if( !(rmnet_send_qcrmcall_request
                (QCRMCALL_ACTION_START, &action_request, dsatvend_rmnet_ev_cb)))
            {
              DS_AT_MSG1_HIGH("V4 QCRMCall on instance %d failed",temp_instance);
              dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
              dsat_qcrmcall_info.v4_instance  = 0;
            }

            action_request.start.qcrmcall_inst               = temp_instance;
            action_request.start.sni_params->ip_family_pref  = (uint8) QMI_AF_INET6;
            dsat_qcrmcall_info.state                        |= DSAT_PENDING_QCRMCALL_START_V6;
            dsat_qcrmcall_info.v6_instance                   =  action_request.start.qcrmcall_inst + 1;
            
            if( !(rmnet_send_qcrmcall_request
                (QCRMCALL_ACTION_START, &action_request, dsatvend_rmnet_ev_cb)))
            {
              DS_AT_MSG1_HIGH("V6 QCRMCall on instance %d failed",dsat_qcrmcall_info.v6_instance );
              dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_START_V6;
              dsat_qcrmcall_info.v6_instance  = 0;
            }
            
            DS_AT_MSG3_HIGH("QCRMCALL V4/V6 QCRMCall on instance: %d family : %d state %d",
                                                      temp_instance, 
                                                      sni_params_ptr->ip_family_pref ,
                                                      dsat_qcrmcall_info.state);

            if(dsat_qcrmcall_info.state == DSAT_PENDING_QCRMCALL_NONE)
            {
              DS_AT_MSG0_ERROR("QCRMCALL V4/V6 Call failed");
              dsatutil_free_memory(sni_params_ptr);
              return DSAT_ERROR;
            }   
            DS_AT_MSG3_HIGH("QCRMCALL V4/V6 QCRMCall on V4: %d V6 : %d state %d",
                                                      dsat_qcrmcall_info.v4_instance, 
                                                      dsat_qcrmcall_info.v6_instance ,
                                                      dsat_qcrmcall_info.state);
            break;
          }
        case QMI_AF_INET6:
        case QMI_AF_INET:
        default:
        {
            if(sni_params_ptr->ip_family_pref == QMI_AF_INET6)
            {
              dsat_qcrmcall_info.v6_instance   =  temp_instance + 1;
              dsat_qcrmcall_info.state        |= DSAT_PENDING_QCRMCALL_START_V6;
            }else
            {
              dsat_qcrmcall_info.state         = DSAT_PENDING_QCRMCALL_START_V4;
              dsat_qcrmcall_info.v4_instance   = temp_instance + 1;
            }
            
            action_request.start.qcrmcall_inst = temp_instance;
            if( !(rmnet_send_qcrmcall_request
                (QCRMCALL_ACTION_START, &action_request, dsatvend_rmnet_ev_cb)))
            {
              DS_AT_MSG2_HIGH(" QCRMCall on instance %d failed on IPV%d ",temp_instance,sni_params_ptr->ip_family_pref);
              dsat_qcrmcall_info.state       =  DSAT_PENDING_QCRMCALL_NONE;
              dsat_qcrmcall_info.v4_instance = 0;
              dsat_qcrmcall_info.v6_instance  = 0;
              dsatutil_free_memory(sni_params_ptr);
              return DSAT_ERROR;
            }
            break;
          }   
        }
      dsatutil_free_memory(sni_params_ptr);
      return DSAT_ASYNC_CMD;
      }
    else
    {

      dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
      
        switch ( sni_params_ptr->ip_family_pref )
        {  
           case QMI_AF_INET_4_6:
           {
              action_request.stop.qcrmcall_inst              = temp_instance;
              action_request.stop.ip_family                  = QMI_AF_INET;
              dsat_qcrmcall_info.state                       = DSAT_PENDING_QCRMCALL_STOP_V4;
              dsat_qcrmcall_info.v4_instance                 = action_request.stop.qcrmcall_inst + 1;
              
              if( !(rmnet_send_qcrmcall_request
                  (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
              {
                DS_AT_MSG1_HIGH("V4 QCRMCall on instance %d failed",temp_instance);
                dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
                dsat_qcrmcall_info.v4_instance  = 0;
              }
  
              action_request.stop.qcrmcall_inst               = temp_instance;
              action_request.stop.ip_family                   = QMI_AF_INET6;
              dsat_qcrmcall_info.state                       |= DSAT_PENDING_QCRMCALL_STOP_V6;
              dsat_qcrmcall_info.v6_instance                  =  action_request.stop.qcrmcall_inst + 1;
              
              if( !(rmnet_send_qcrmcall_request
                  (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
              {
                DS_AT_MSG1_HIGH("V6 QCRMCall on instance %d failed",temp_instance+1);
                dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_STOP_V6;
                dsat_qcrmcall_info.v6_instance  = 0;
              }
              
              DS_AT_MSG3_HIGH("QCRMCALL V4/V6 on instance: %d family : %d state %d",
                                                        temp_instance, 
                                                        sni_params_ptr->ip_family_pref ,
                                                        dsat_qcrmcall_info.state);
  
              if(dsat_qcrmcall_info.state == DSAT_PENDING_QCRMCALL_NONE)
              {
                DS_AT_MSG0_ERROR("QCRMCALL V4/V6 Call failed");
                dsatutil_free_memory(sni_params_ptr);
                return DSAT_ERROR;
              }
              DS_AT_MSG3_HIGH("QCRMCALL V4/V6 QCRMCall on V4: %d V6 : %d state %d",
                                                      dsat_qcrmcall_info.v4_instance, 
                                                      dsat_qcrmcall_info.v6_instance ,
                                                      dsat_qcrmcall_info.state);
               break;
            }
              
            case QMI_AF_INET:
            case QMI_AF_INET6:
            default:
            {
              action_request.stop.qcrmcall_inst = temp_instance;

              if(sni_params_ptr->ip_family_pref == QMI_AF_INET6)
              {
                action_request.stop.ip_family    = QMI_AF_INET6;
                dsat_qcrmcall_info.v6_instance   =  temp_instance + 1;
                dsat_qcrmcall_info.state         = DSAT_PENDING_QCRMCALL_STOP_V6;
              }
              else
              {
                action_request.stop.ip_family    = QMI_AF_INET;
                dsat_qcrmcall_info.state         = DSAT_PENDING_QCRMCALL_STOP_V4;
                dsat_qcrmcall_info.v4_instance   = temp_instance + 1;
              }
              
              if( !(rmnet_send_qcrmcall_request
                  (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
              {
                DS_AT_MSG2_HIGH(" QCRMCall on instance %d failed on IPV%d ",temp_instance,sni_params_ptr->ip_family_pref);
                dsat_qcrmcall_info.state       =  DSAT_PENDING_QCRMCALL_NONE;
                dsat_qcrmcall_info.v4_instance = 0;
                dsat_qcrmcall_info.v6_instance  = 0;
                dsatutil_free_memory(sni_params_ptr);
                return DSAT_ERROR;
              }
              break;
            }   
          }
          dsatutil_free_memory(sni_params_ptr);
          return DSAT_ASYNC_CMD; /*Means No error found*/
        }
        
    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid ARG 1 received; cannot proceed further");
      return DSAT_ERROR;
    }
  }
  else if ( (NA|EQ|QU) == tok_ptr->op )
  {
    rmnet_instance_e_type rmnet_instance = RMNET_INSTANCE_MAX;

    /* Report Action Data */
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "$QCRMCALL: (0-1),(");            

   /* Report available instances  */
   for ( rmnet_instance = 0;rmnet_instance < RMNET_QCRMCALL_MAX; 
         rmnet_instance++)
   {
       res_buff_ptr->used += (word)snprintf(
                        (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                        res_buff_ptr->size -  res_buff_ptr->used,
                                      "%d,",(int)1+(rmnet_instance));
   }
   /* To Removed , after Instance */
   res_buff_ptr->used--;
   
   res_buff_ptr->used += (word)snprintf(
                        (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                        res_buff_ptr->size -  res_buff_ptr->used,
                                        ")");

   /*Report PDP Type,  AcT, UMTS, CDMA profile numbers*/

   res_buff_ptr->used += (word)snprintf(
                         (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                          res_buff_ptr->size -  res_buff_ptr->used,
                          ",(1-4),(1-2),(%d-%d,%d-%d),,",
                          DSAT_LEGACY_PROFILE_NUM_MIN,
                          DSAT_LEGACY_PROFILE_NUM_MAX,
                          DSAT_EXT_PROFILE_NUM_MIN,
                          DSAT_EXT_PROFILE_NUM_MAX); 
    return DSAT_OK;
  }
  else if ( tok_ptr->op == (NA|QU))
  {
    dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_QUERY;
    if( !(rmnet_send_qcrmcall_request
        (QCRMCALL_ACTION_QUERY, &action_request, dsatvend_rmnet_ev_cb)))
    {
      DS_AT_MSG1_HIGH("Query QCRMCall on instance %d failed",temp_instance);
      dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
      return DSAT_ERROR;
    }

    DS_AT_MSG1_HIGH("Query V4 QCRMCall on instance %d",temp_instance);
    return DSAT_ASYNC_CMD;
  }

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_TOKENS)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }

  return DSAT_ERROR;
}/*  dsatvend_exec_qcrmcall_cmd */
#endif /* FEATURE_DATA_QCRMCALL */


#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_DSAT_ATCOP_VEND

/*===========================================================================

FUNCTION DSATVEND_REMOVE_BLANK_SPACES

DESCRIPTION
This function takes char string as an argument and returns pointer to
the string without blank spaces

DEPENDENCIES
  None

RETURN VALUE
  Pointer to resultant string.

SIDE EFFECTS
  None

===========================================================================*/

void dsatvend_remove_blank_spaces
(
  char*          input
)
{
  char *output=input;
  int i;
  uint8 j;
  boolean initial_spaces = TRUE;
 
  i = strlen(input) - 2;
  
  while( i >= 0 && input[i] == ' ')
  {
    input[i--] = 0;           /*Removing spaces from the end*/
  }
  
  for ( i = 0, j = 0; i < (int) strlen(input); i++, j++)
  {
    if( initial_spaces == TRUE && input[i] == ' ')
    {
      j--;
      continue;
    }
  
    initial_spaces = FALSE;
    output[j] = input[i];
  }
  output[j] = 0;
}/*dsatvend_remove_blank_spaces*/

/*===========================================================================

FUNCTION DSATVEND_GET_PARAM_TYPE_VALUE

DESCRIPTION
This function processes the argument passed and tokenizes it to get parameter
type and value.

DEPENDENCIES
  None

RETURN VALUE
    DSAT_SUCCESS : if the tokens are successfuly created
    DSAT_FAILURE : if an invalid argument is passed

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatvend_get_param_type_value
(
  const char         *arg,                /* Argument passed in the form A:B */
  char               *param_type,         /* Parameter Type (A) */
  uint8               type_size,          /* Maximum size of Parameter Type */
  char               *param_value,        /* Parameter Value (B) */ 
  uint8               value_size          /* Maximum size of Parameter Value */
)
{
  char *type   =   param_type;
  char *value  =   param_value;
  /*Save KWs*/
  if(NULL == arg || NULL == param_type || NULL == param_value)
  {
    return DSAT_FAILURE;
  }
  while( type_size != 0  && *arg != ':' && *arg != '\0' )
  {
    *param_type++ = *arg++ ;
     type_size = type_size - 1;
  }
/*Valid terminator found ':' ; Terminate Param type*/
  if( *arg == ':' )
  {
    *param_type++ = '\0';
     arg++; /* Move to Next char in the String */
  }
  else
  {
    DS_AT_MSG1_ERROR("$QCPRFCRT: Invalid Tokens Name,Remain Size  %d", type_size);
    return DSAT_FAILURE;
  }

  while(value_size != 0 && *arg != '\0' )
  {
    *param_value++ = *arg++ ;
     value_size = value_size - 1;
  }
/* Valid  terminator found'\0' ; Terminate Param value*/
  if( *arg == '\0' )
  {
    *param_value = '\0';
  }
  else
  {
    DS_AT_MSG1_ERROR("$QCPRFCRT: Invalid Tokens value,Remain Size  %d", value_size);
    return DSAT_FAILURE;
  }

  dsatvend_remove_blank_spaces(type);
  dsatvend_remove_blank_spaces(value);
  
  /* Validating Param type and Value after removing white space*/
  if(strlen(type) == 0 || strlen(value) == 0)
  {
    DS_AT_MSG2_ERROR("$QCPRFCRT: Invalid Tokens String,Name Size %d Value Size", 
                      strlen(type),strlen(value));
    return DSAT_FAILURE;
  }
  return DSAT_SUCCESS;
}/*dsatvend_get_param_type_value*/

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCRSRP_CMD

DESCRIPTION
  This function executes $QCRSRP and $QCRSRQ commands and provides response 
  Calls CM API to receive neighbour cell info like Cell id,EARFCN,RSRP,RSRQ.
  RSRP - Reference Signal Received Power
  RSRQ - Reference Signal Received Quality
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_OK :    if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrsrp_cmd
(
    dsat_mode_enum_type mode,             /*  AT command mode:            */
    const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
    const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
    dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define CMD_RSRP 0
  #define CMD_RSRQ 1
  #define MAX_RSR_COUNT CMAPI_LTE_NGBR_IND_NUM_CELLS
  
  dsat_result_enum_type result = DSAT_ERROR;
  cmapi_err_e_type err_lte_mode = CMAPI_SUCCESS;
  cmapi_rat_meas_info_s_type *meas_info_ptr = NULL;
  boolean cmd = 0;
  int i = 0,j = 0;
  int rsr_count = 0;
  int16 value = 0;
  cmapi_signal_info_s_type *signal_info_ptr = NULL;
  boolean is_serv_meas_valid                = TRUE;

  /* Supported only NA|QU */
  if( tok_ptr->op != (NA|QU) )
  {
    return result;
  }
  
  res_buff_ptr->data_ptr[0] = '\0';
  if (0 == strncmp((const char *)parse_table->name,(const char *)"$QCRSRP",strlen((const char *)"$QCRSRP")))
  {
    res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "$QCRSRP: ");
    cmd = CMD_RSRP;
  }
  else if(0 == strncmp((const char *)parse_table->name,(const char *)"$QCRSRQ",strlen((const char *)"$QCRSRQ")))
  {
    res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "$QCRSRQ: ");
    cmd = CMD_RSRQ;
  }
  else
  {
    return result;
  }

  /* Call cmapi_get_signal_info() to check serving cell status first, then 
     fill minimum RSRP/RSRQ (like -140dBM/-20dB) if serving cell is unavailable. */
  signal_info_ptr = (cmapi_signal_info_s_type *) dsat_alloc_memory(sizeof(cmapi_signal_info_s_type),FALSE);

  err_lte_mode = cmapi_get_signal_info( CMAPI_SYS_MODE_LTE, signal_info_ptr );
  if( err_lte_mode != CMAPI_SUCCESS )
  {
    is_serv_meas_valid = FALSE;
  }
  dsatutil_free_memory((void *)signal_info_ptr);

  meas_info_ptr = (cmapi_rat_meas_info_s_type *)dsat_alloc_memory(sizeof(cmapi_rat_meas_info_s_type),FALSE);

  //get LTE Cell info
  err_lte_mode = cmapi_get_rat_meas_info( CMAPI_SYS_MODE_LTE, meas_info_ptr );
  if( err_lte_mode == CMAPI_SUCCESS )
  {
    if( meas_info_ptr->is_service_available && (meas_info_ptr->sys_mode == CMAPI_SYS_MODE_LTE) &&
         meas_info_ptr->meas_info_u.lte_nbr_info.valid )
    {
      cmapi_lte_ngbr_ind_s* nbr_info = &meas_info_ptr->meas_info_u.lte_nbr_info;
     /*lte intra start: in intra cell nbr info each cell different rsrp and rsrq values get the info for all cells */
      DS_AT_MSG1_HIGH("LTE: num intra num cells: %d", nbr_info->lte_intra.num_lte_cells);
      for( i = 0; i<nbr_info->lte_intra.num_lte_cells && i < CMAPI_LTE_NGBR_IND_NUM_CELLS; i++ )
      {
        value = (cmd == CMD_RSRP) ? nbr_info->lte_intra.cell[i].rsrp : nbr_info->lte_intra.cell[i].rsrq;

        /* If cell is invalid, assign minimum RSRP/RSRQ value. */
        if( FALSE == is_serv_meas_valid )
        {
          value = (cmd == CMD_RSRP) ? -1400 : -200;
        }
        if( cmd == CMD_RSRP )
        {
          res_buff_ptr->used += (word)snprintf(
                                (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                res_buff_ptr->size - res_buff_ptr->used,
                                "%3.3d,%lu,\"%3.3d.%d0\",",
                                nbr_info->lte_intra.cell[i].pci,
                                (uint32)nbr_info->lte_intra.earfcn,
                                value/10,
                                DSAT_ABSOLUTE_VAL(value)%10);
        }
        else
        {
          res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                    res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%2.2d.%d0\"," ,
                                    nbr_info->lte_intra.cell[i].pci,
                                    (uint32)nbr_info->lte_intra.earfcn,
                                    value/10,
                                    DSAT_ABSOLUTE_VAL(value)%10);
        }
          rsr_count++;
      }
      /*lte intra end*/

      /*lte inter start :in the inter  cell nbr info multriple ferequencies are available hence collect all cells info for each frequency*/
      DS_AT_MSG2_HIGH("LTE: num inter num freqs: %d current rsr_count = %d", nbr_info->lte_inter.num_freqs,rsr_count);
      for( i=0; ( i<nbr_info->lte_inter.num_freqs && i < CMAPI_LTE_NGBR_IND_NUM_FREQS) &&
                ( rsr_count < MAX_RSR_COUNT); i++ )
      {
        DS_AT_MSG1_HIGH("LTE: num inter num cells: %d", nbr_info->lte_inter.freqs[i].num_lte_cells);
        
        for( j=0; ( j<nbr_info->lte_inter.freqs[i].num_lte_cells && j < CMAPI_LTE_NGBR_IND_NUM_CELLS) && 
                  ( rsr_count < MAX_RSR_COUNT); j++ )
        {
            value = (cmd == CMD_RSRP) ? nbr_info->lte_inter.freqs[i].cells[j].rsrp : nbr_info->lte_inter.freqs[i].cells[j].rsrq;

          /* If cell is invalid, assign minimum RSRP/RSRQ value */
          if( FALSE == is_serv_meas_valid )
          {
            value = (cmd == CMD_RSRP) ? -1400 : -200;
          }
          if( cmd == CMD_RSRP )
          {
            res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                   res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%3.3d.%d0\"," ,
                                   nbr_info->lte_inter.freqs[i].cells[j].pci,
                                   (uint32)nbr_info->lte_inter.freqs[i].earfcn,
                                   value/10,
                                   DSAT_ABSOLUTE_VAL(value)%10);
          }
          else
          {
            res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                   res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%2.2d.%d0\",",
                                   nbr_info->lte_inter.freqs[i].cells[j].pci,
                                   (uint32)nbr_info->lte_inter.freqs[i].earfcn,
                                   value/10,
                                   DSAT_ABSOLUTE_VAL(value)%10);
          }
            rsr_count++;
        }
      }
      /*lte inter enter*/
    }
    else
    {
      DS_AT_MSG0_MED("LTE info is not valid");
    }
  }
  else
  {
    DS_AT_MSG0_ERROR("cmapi for meas info failed in CMAPI_SYS_MODE_LTE ");
  }

  /* if api returns failure for both systems then we need to disaply error message*/
  if(err_lte_mode != CMAPI_SUCCESS)
  {
    res_buff_ptr->used = 0;
    res_buff_ptr->data_ptr[0] = '\0';
     switch( err_lte_mode )
    {
      case CMAPI_NO_SERVICE:
        result = dsat_send_cme_error(DSAT_CME_NO_NETWORK_SERVICE);
        break;
      default:
        DS_AT_MSG1_MED("CMAPI Error : %d",err_lte_mode);
        result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
        break;
    }
  }
  /* if api call is successful and data is not valid display error*/
  else if( meas_info_ptr->meas_info_u.wcdma_nbr_info.is_data_valid == 0 && meas_info_ptr->meas_info_u.lte_nbr_info.valid == 0)
  {
    res_buff_ptr->used = 0;
    res_buff_ptr->data_ptr[0] = '\0';
    result = DSAT_ERROR;
  }
  /* if both or any one of the api calls are successful then return success.*/
  else
  {
    result = DSAT_OK;
  }
  dsatutil_free_memory((void *)meas_info_ptr);
  return result;
}


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRATSTATE_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCRATSTATE command, a read only command and also enables URC's
  for LTE TX ON/OFF.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcratstate_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  if( tok_ptr->op == (NA|QU))
  {
    if( dsatcmif_get_cm_ss_lte_cphy_tx_info() != DSAT_ERROR)
    {
      SET_PENDING(DSAT_VENDOR_QCRATSTATE_IDX, 0, DSAT_PENDING_TRUE);
      result = DSAT_ASYNC_CMD;
    }
  }
  else if( tok_ptr->op == (NA|EQ|QU) || ( tok_ptr->op == (NA|EQ|AR)))
  {
    result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr
                                  );
    if(DSAT_OK == result)
    {
      dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_QCRATSTATE_MODE, 
                                DSAT_VENDOR_QCRATSTATE_IDX, 
                                0);
    }
  
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCEXTCLTSTATE_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCEXTCLTSTATE command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcextcltstate_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dsat_num_item_type temp;
  boolean gnss_stat = 0;
  
  dsat_num_item_type mode_val = (dsat_num_item_type)dsatutil_get_val(
                       DSAT_VENDOR_QCEXTCLTSTATE_IDX, 0,NUM_TYPE);
  if( tok_ptr->op == (NA|EQ|QU) || tok_ptr->op == (NA|QU) )
  {
    result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr);
  }
  else if( tok_ptr->op == (NA|EQ|AR) )
  {
    if( VALID_TOKEN(0) )
    {
      if ( ATOI_OK != dsatutil_atoi (&temp, tok_ptr->arg[0], 10) )
      {
        DS_AT_MSG0_ERROR(" Cannot convert format param ");
        return DSAT_ERROR;
      } 
      if(temp == mode_val)
      {
        DS_AT_MSG0_HIGH("QCEXTCLTSTATE : Same state as previous");
        return DSAT_OK;
      }
      else
      {
        result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr);
         gnss_stat = (boolean)temp;
         result = dsatcmif_set_cm_ss_gnss_tx_activity(gnss_stat);
      }
    }
  }
  return result;
}
/*===========================================================================

FUNCTION DSAVEND_PROCESS_QCRATSTATE_CMD

DESCRIPTION
  This function process the response for the $QCRATSTATE command
  on return from asynchronous processing.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_qcratstate_cmd
(
  dsat_cm_msg_s_type *msg_ptr
)
{
  dsm_item_type *res_buff_ptr = NULL;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  
  boolean tx_status = msg_ptr->msg.ss_event.event_info.dsat_lte_tx_info.lte_tx_status;
  sys_sband_lte_e_type tx_band = msg_ptr->msg.ss_event.event_info.dsat_lte_tx_info.lte_tx_band;
  boolean tx_changed = FALSE;
  
  dsat_num_item_type mode_val = (dsat_num_item_type)dsatutil_get_val(
                       DSAT_VENDOR_QCRATSTATE_IDX, 0,NUM_TYPE);
  if( CHECK_PENDING(DSAT_VENDOR_QCRATSTATE_IDX,0,DSAT_PENDING_TRUE) )
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    SET_PENDING(DSAT_VENDOR_QCRATSTATE_IDX, 0, DSAT_PENDING_FALSE);
    if( tx_status == FALSE )
    {
      res_buff_ptr->used = snprintf( (char*)res_buff_ptr->data_ptr,
                                    res_buff_ptr->size,
                                    "%d,%d",
                                    mode_val,tx_status );
    }
    else
    {
      res_buff_ptr->used = snprintf( (char*)res_buff_ptr->data_ptr,
                                    res_buff_ptr->size,
                                    "%d,%d,%d",
                                    mode_val,tx_status,
                                    tx_band);
    }

    dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
    result = DSAT_OK;
  }
  else
  {
    if(dsat_lte_prev_tx_info.lte_tx_status != tx_status || 
       dsat_lte_prev_tx_info.lte_tx_band != tx_band )
    {
      tx_changed = TRUE;
      dsat_lte_prev_tx_info.lte_tx_status = tx_status;
      dsat_lte_prev_tx_info.lte_tx_band = tx_band;
    }
    else
    {
      tx_changed = FALSE;
      return result;
    }

    if ( mode_val == 1 && tx_changed)
    {
    DS_AT_MSG0_HIGH(" qcratstate mode is on , sending urc ");
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    if (NULL != res_buff_ptr)
    {
      if( tx_status == FALSE )
      {
        res_buff_ptr->used =
          (word) snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                            "$QCRATSTATE: %d",tx_status );
      }
      else
      {
        res_buff_ptr->used =
          (word) snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                            "$QCRATSTATE: %d,%d",tx_status,tx_band);
      }

      /* Send response to TE */
      dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
    }
  }
  }
  return result;;
}


/*===========================================================================
  FUNCTION DSATVEND_DISPLAY_QCPRFMOD_TEST_RESULT
 
  DESCRIPTION
  It displays $QCPRFMOD test command results
 
  PARAMETERS

  RETURN VALUE
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsat_result_enum_type dsatvend_display_qcprfmod_test_result
(
  dsm_item_type *res_buff_ptr
)
{
  uint8       idx;
  dsat_profile_parameter_details_s_type *param_list;
  uint8                                  param_list_size;
  uint8                                  i;

  param_list = &profile_3gpp_parameter_details_list[0];
  param_list_size = ARR_SIZE(profile_3gpp_parameter_details_list);

  for(idx = 0; idx < param_list_size; idx++)
  {
      CHECK_APPEND_RESPONSE_BUFFER(res_buff_ptr->size - 30)
      switch(param_list[idx].type)
      {
        case NUM_TYPE:
        case LNGR_TMR_TYPE:
        {
          res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                   res_buff_ptr->size - res_buff_ptr->used,
                                               "\n%s:(",param_list[idx].abbrv);
          
          for(i = 0;!( param_list[idx].range[i].lower == RANGE_END &&
                       param_list[idx].range[i].upper == RANGE_END ); i++)
          {
            if ( param_list[idx].range[i].lower == param_list[idx].range[i].upper)
            {
              res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                    res_buff_ptr->size - res_buff_ptr->used,
                                    "%lu,",param_list[idx].range[i].lower);
            }
            else
            {
              res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                     res_buff_ptr->size - res_buff_ptr->used,
                                                     "%lu-%lu,",
                                                     param_list[idx].range[i].lower,
                                                     param_list[idx].range[i].upper );
            }
          }
          res_buff_ptr->used--;
          res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 ")");
          break;
        }
        case STR_TYPE:
        case ADDR_V4_TYPE:
        case ADDR_P_V6_TYPE:
        {
          /* Both 3GPP APN name and 3GPP2 APN name will  point to same value.(General Param)
                    Skipping P2 APN name*/
          if( param_list[idx].identifier == DS_PROFILE_APN_NAME )
          {
            res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                   res_buff_ptr->size - res_buff_ptr->used,
                                                  "\nAPNNAME:\"\"");
          }
          else
          {
             res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                   "\n%s:\"\"",param_list[idx].abbrv);
          }
          break;
        }
        default:
         break;
      }
  }
  return 0;
}/* dsatvend_display_qcprfmod_test_result */

/*===========================================================================
  FUNCTION DSATVEND_PROFILE_UPDATE_PARAM_DETAILS()
 
  DESCRIPTION
    Gets the param_identifier, param_len, param_value_validation using
    param_type and tech_type.
 
  PARAMETERS
    param_id :         Parameter identifier.
    param_type:        parameter type (string)
 
  RETURN VALUE
    if param_id, param_len, param_val is successfully found : DSAT_SUCCESS.
    else  :DSAT_FAILURE
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsat_result_enum_type dsatvend_profile_update_param_details
(
  dsat_num_item_type                profile_id,/*  Profile ID       */
  dsm_item_type                    *res_buff_ptr /*  Place to put response       */
)
{
  #define BUFFER_OFFSET_VAL 30
  dsat_profile_parameter_details_s_type   *param_list = NULL;
  uint8                                    param_list_size;
  dsat_result_enum_type                    result = DSAT_OK;
  uint16                                   index = 0;
  byte                                     ipaddr_str[MAX_IPADDR_STR_LEN+1];
  uint8                                    array_size_pp = 0;
  dsat_num_item_type                       dsat_failure_timers[DS_3GPP_MAX_FAILURE_TIMERS];
  uint8                                    failure_index = 0;
  ds_profile_type_s                        profile;
  char                                     buffer[100];
  dsat_ip_addr_type                        addr;
  uint16                                   current_index = 0;
  memset(buffer,0,100);
  memset(&profile,0,sizeof(ds_profile_type_s));

  if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                    (profile_id,&profile))
  {    
    DS_AT_MSG1_HIGH("Profile not present first create %d",profile_id);
    return DSAT_ERROR;
  }
  ipaddr_str[0] = '\0';
  array_size_pp   = ARR_SIZE(profile_3gpp_parameter_details_list);

  param_list_size   = array_size_pp;

  /* Check to see if a new DSM item is required */
  CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

  res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                              res_buff_ptr->size - res_buff_ptr->used,
                                              "$QCPRFMOD: PID:%d TECH:%d\n",
                                               profile_id,
                                               0);
  param_list =  &profile_3gpp_parameter_details_list[0];
  
  while (index < param_list_size)

  {
    switch(param_list[index].type)
    {
       case NUM_TYPE:
       {
         /*Special case for 3GPP failure timers since they are all tagged to one identifier 
          First:   Get all the 10 timers and 
          Second : print all the ten timers and increment the index by max-1 */

        if( param_list[index].identifier == DS_PROFILE_FAILURE_TIMERS)
        {        
          memset(&dsat_failure_timers[0], 0, sizeof(dsat_failure_timers));
          /*getting the value for all the timers set (its an array) */
		    current_index = 30;
            for(failure_index=0; 
                failure_index < DS_3GPP_MAX_FAILURE_TIMERS; 
                failure_index++) 
             {
               /*print all the failure timer values 1-10*/
               res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr
->used],
                                                          res_buff_ptr->size - res_buff_ptr->used,
                                                          "%s:%ld\n",
                                                          param_list[current_index+failure_index].
abbrv,
                                                          profile.failure_timers[failure_index]);
             }
             /*increment the index for each iteration of failure timer (max -1)*/
             index += DS_3GPP_MAX_FAILURE_TIMERS - 1;
             break;
        }
        else if(param_list[index].identifier == DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG)
        {        
         /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[index].abbrv,
                                                      profile.request_pcscf_address_flag);
           break;
         }
         else if(param_list[index].identifier == DS_PROFILE_CONTEXT_D_COMP)
         {         
          /* Check to see if a new DSM item is required */
            CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
            res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[index].abbrv,
                                                      profile.context.d_comp);
           break;
         }
         else if(param_list[index].identifier == DS_PROFILE_CONTEXT_H_COMP)
         {         
           /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[index].abbrv,
                                                      profile.context.h_comp);
           break;
         }
         else if(param_list[index].identifier == DS_PROFILE_AUTH_TYPE)
         {         
         /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[index].abbrv,
                                                      profile.auth.auth_type);
           break;
         }
         else if(param_list[index].identifier == DS_PROFILE_ACCESS_CTRL_FLAG)
         {  
                  /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
    
           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
         used],
                                                               res_buff_ptr->size - res_buff_ptr->used,
                                                               "%s:%u\n",
                                                               param_list[index].abbrv,
                                                               profile.context.access_ctrl_flag);

                  break;
         }
         else if(param_list[index].identifier == DS_PROFILE_IM_CN_FLAG)
         {         
         /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[index].abbrv,
                                                      profile.im_cn_flag);
           break;
         }
        else if(param_list[index].identifier == DS_PROFILE_APN_DISABLE_FLAG)
        {      
         /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[index].abbrv,
                                                      profile.apn_disable_flag);         
           break;
         }
         else if(param_list[index].identifier == DS_PROFILE_PDN_INACTIVITY_TIMEOUT)
         {         
                 /* Check to see if a new DSM item is required */
                   CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
        
                   res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                              "%s:%ld\n",
                                                              param_list[index].abbrv,
                                                              profile.inactivity_timer_val);
           break;
          }
          else if(param_list[index].identifier == DS_PROFILE_MAX_PDN_CONN_PER_BLOCK)
          {          
             /* Check to see if a new DSM item is required */
               CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
               res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                             "%s:%u\n",
                                                              param_list[index].abbrv,
                                                              profile.max_pdn_conn_per_blk);
           break;
          }
          else if(param_list[index].identifier == DS_PROFILE_PDN_DISCON_WAIT_TIME)
          {          
             /* Check to see if a new DSM item is required */
              CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                              "%s:%u\n",
                                                              param_list[index].abbrv,
                                                              profile.pdn_discon_wait_time);
           break;
          }
          else if(param_list[index].identifier == DS_PROFILE_PDN_REQ_WAIT_TIMER)
          {          
            /* Check to see if a new DSM item is required */
             CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                              "%s:%u\n",
                                                              param_list[index].abbrv,
                                                              profile.pdn_req_wait_time);
           break;
          }
          else if(param_list[index].identifier == DS_PROFILE_APN_CLASS)
          {          
            /* Check to see if a new DSM item is required */
              CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                              "%s:%u\n",
                                                              param_list[index].abbrv,
                                                              profile.apn_class);
           break;
          }
          else if(param_list[index].identifier == DS_PROFILE_APN_BEARER)
          {         
            /* Check to see if a new DSM item is required */
              CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                              "%s:%u\n",
                                                              param_list[index].abbrv,
                                                              profile.apn_bearer);
            break;
          }
          else if(param_list[index].identifier == DS_PROFILE_OPERATOR_RESERVED_PCO)
          {          
            /* Check to see if a new DSM item is required */
              CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
        used],
                                                              res_buff_ptr->size - res_buff_ptr->used,
                                                              "%s:%u\n",
                                                              param_list[index].abbrv,
                                                              profile.operator_reserved_pco);
            break;
          }
          else if(param_list[index].identifier == DS_PROFILE_MCC)
          {          
             /* Check to see if a new DSM item is required */
              CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
                used],
                                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                                      "%s:%u\n",
                                                                      param_list[index].abbrv,
                                                                      profile.mcc);
              break;
          }
          else if(param_list[index].identifier == DS_PROFILE_MNC)
          {          
             /* Check to see if a new DSM item is required */
                CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
                res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
                used],
                                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                                      "%s:%u\n",
                                                                      param_list[index].abbrv,
                                                                      profile.mnc.mnc_digits);
             break;
          }
          else if(param_list[index].identifier == DS_PROFILE_ROAMING_DISALLOWED)
          {
             /* Check to see if a new DSM item is required */
               CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
               res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
                used],
                                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                                      "%s:%u\n",
                                                                      param_list[index].abbrv,
                                                                      profile.roaming_disallowed);             
            break;
          }
          else if(param_list[index].identifier == DS_PROFILE_PARAM_ATTACH_WO_PDN)
          {          
              /* Check to see if a new DSM item is required */
              CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
                      used],
                                                                            res_buff_ptr->size - res_buff_ptr->used,
                                                                            "%s:%u\n",
                                                                            param_list[index].abbrv,
                                                                            profile.attach_wo_pdn);              
            break;
         }
         else
          break;
       }
       case STR_TYPE:
       {
        /*Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - (DS_PROFILE_APN_STRING_LEN 
+ BUFFER_OFFSET_VAL))
        
         if(param_list[index].identifier == DS_PROFILE_APN_NAME)
         {
             DS_AT_MSG_SPRINTF_2_ERROR("Failed to get param_list[index].identifier %s %d",param_list[index].abbrv,param_list[index].identifier);
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "APNNAME:\"%s\"\n",
                                                    (char*)profile.context.apn);
           }
           else if(param_list[index].identifier == DS_PROFILE_AUTH_PASSWORD )
           {           
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                     res_buff_ptr->size - res_buff_ptr->used,
                                                     "%s:\"\"\n",
                                                      param_list[index].abbrv);
           }
           else if(param_list[index].identifier == DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE)
           {           
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "%s:\"%s\"\n",
                                                    param_list[index].abbrv,
                                                    (profile.context.lte_roaming_pdp_type != DS_PROFILE_ROAMING_PDP_MAX)?
                                                    (char*)dsat_p2_pdptype[profile.context.lte_roaming_pdp_type]:"");
           }
           else if(param_list[index].identifier == DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE)
           {           
              res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "%s:\"%s\"\n",
                                                    param_list[index].abbrv,
                                                    (profile.context.umts_roaming_pdp_type != DS_PROFILE_ROAMING_PDP_MAX)?
                                                    (char*)dsat_p2_pdptype[profile.context.umts_roaming_pdp_type]:"");
           }           
           else if(param_list[index].identifier == DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
           {           
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
           used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                        "%s:\"%s\"\n",
                                                    param_list[index].abbrv,
                                                    (profile.context.override_home_pdp_type != DS_PROFILE_ROAMING_PDP_MAX)?
                                                    (char*)dsat_p2_pdptype[profile.context.override_home_pdp_type]:"");
           }
           else if(param_list[index].identifier == DS_PROFILE_TYPE)
           {           
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->
                   used],
                                            res_buff_ptr->size - res_buff_ptr->used,
                                              "%s:\"%s\"\n",
                                            param_list[index].abbrv,
                                           (char*)dsat_pp_pdptype[profile.context.pdp_type]);
           }
           break;
      }
      case ADDR_V4_TYPE:
      {
        addr.ip_vsn = DS_PROFILE_IP_V4;
		
        if(param_list[index].identifier == DS_PROFILE_DNS_ADDR_V4_PRIMARY)
        {        
          addr.addr = profile.dns_addr.primary_dns_addr;
        }
        else if(param_list[index].identifier == DS_PROFILE_DNS_ADDR_V4_SECONDARY)
        { 
          addr.addr = profile.dns_addr.secondary_dns_addr;
        }
        else if(param_list[index].identifier == DS_PROFILE_PDP_ADDR)
        {
          addr.addr = profile.pdp_addr;
        }
        if ( DSAT_OK != dsatutil_convert_ipaddr(INT_TO_STRING,
                                                &addr,
                                                ipaddr_str) )
          {
            DS_AT_MSG1_ERROR("IPV4 address conversion failed for param_id %d",param_list[index].identifier);
            return DSAT_ERROR;
          }
          /* Check to see if a new DSM item is required */
          CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - (MAX_V4_ADDR_LEN + BUFFER_OFFSET_VAL))

          res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used
],
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 "%s:\"%s\"\n",
                                                 param_list[index].abbrv,
                                                 (char*)ipaddr_str);
      }
      break;
      case ADDR_P_V6_TYPE:
      {
      
        addr.ip_vsn = DS_PROFILE_IP_V6;
        if(param_list[index].identifier == DS_PROFILE_DNS_ADDR_V6_PRIMARY)
        {        
           addr.addr = profile.dns_addr.primary_dns_addr;
        }
        else if(param_list[index].identifier == DS_PROFILE_DNS_ADDR_V6_SECONDARY)
        { 
           addr.addr = profile.dns_addr.secondary_dns_addr;
        }
        else if(param_list[index].identifier == DS_PROFILE_PDP_ADDR)
        {
          addr.addr = profile.pdp_addr;
        }
        if ( DSAT_OK != dsatutil_convert_ipaddr(INT_TO_STRING,
                                                 &addr,
                                                ipaddr_str) )
        {
            DS_AT_MSG1_ERROR("IPV6 address conversion failed for param_id %d",param_list[index].identifier);
            return DSAT_ERROR;
          }
          CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - (MAX_IPADDR_STR_LEN + 
BUFFER_OFFSET_VAL))
          
          res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used
],
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 "%s:\"%s\"\n",
                                                 param_list[index].abbrv,
                                                 (char*)ipaddr_str);
        }
      break;
     default:
       break;
    }
    index++; 
  }
  
  return result;
}/*dsatvend_profile_set_param_details*/
/*===========================================================================
  FUNCTION DSATVEND_PROFILE_SET_PARAM_DETAILS
 
  DESCRIPTION
    Gets the param_identifier, param_len, param_value_validation using
    param_type and tech_type.
 
  PARAMETERS
    param_id :         Parameter identifier.
    param_type:        parameter type (string)
    mask           To fill the mask for profile to update the value
 
  RETURN VALUE
    if param_id, param_len, param_val is successfully found : DSAT_SUCCESS.
    else  :DSAT_FAILURE
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
LOCAL boolean dsatvend_profile_set_param_details
(
  char                                  *param_type,
  char                                  *param_value,
  uint8                                  profile_id,
  uint64                                *mask,
  ds_profile_type_s                     *profile
)
{
  uint8                                    idx;
  uint8                                    i;
  dsat_num_item_type                       value = 0;
  dsat_profile_parameter_details_s_type    *param_list;
  uint8                                    param_list_size;
  ds_profile_info_type                     param_info;
//  ds_profile_status_etype                  profile_status;
  uint64                                   param_id;
  dsat_v4_addr_type                        v4_addr;
  uint32                                   addr[4];
  boolean                                  valid_value = FALSE;
  boolean                                  param_found = FALSE;
  uint32                                   dsat_failure_timers[DS_3GPP_MAX_FAILURE_TIMERS];
  uint8                                    failure_index = 0;
  ds_profile_type_s                        profile_save;
  memset(&profile_save,0,sizeof(ds_profile_type_s));
  
  DS_AT_MSG_SPRINTF_2_HIGH(" param_type:%s, param_value=%s",param_type,param_value);
/*
  * APNNAME mapped to 3GPP2 APN name or 3GPP APN name based on profile tech type
  * APN NAME is general Param and tied to profile.
 */
 /* Search in 3GPP list*/
    param_list = &profile_3gpp_parameter_details_list[0];
    param_list_size = ARR_SIZE(profile_3gpp_parameter_details_list);

    for(idx = 0; idx < param_list_size; idx++)
    {
      if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
      {
        param_found = TRUE;
        break;
      }
    }

  if( param_found )
  {
    param_id       =  param_list[idx].identifier;
    param_info.len =  param_list[idx].len;

    switch( param_list[idx].type )
    {
      case STR_TYPE:
      {
        if( FALSE == dsatutil_strip_quotes_out( (byte*) param_value,
                                                (byte*) param_value,
                                                (word)  param_list[idx].range[0].lower ))
         {
           return DSAT_FAILURE;
         }

        /*Special case:: PDPTYPE- 3GPP and 3GPP2/Common have different values for legacy. 
          New PDPTYPE getting added will have this value ex - ROAMING_PDP_TYPE below*/

        if( param_id == DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE ||
            param_id == DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE ||
            param_id == DS_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
        {
        
          for (i = 0; i < DSAT_MAX_PDP_TYPE_NUM - 1; i++)
          {
            if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_value, (byte*)dsat_p2_pdptype[i] ))
            {
            
              if(param_id == DS_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE)
                profile->context.lte_roaming_pdp_type = (ds_profile_roaming_pdp_type_e)i;
             else if(param_id == DS_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE)
                profile->context.umts_roaming_pdp_type = (ds_profile_roaming_pdp_type_e)i;
             else
                profile->context.override_home_pdp_type = (ds_profile_roaming_pdp_type_e)i;
              break;
            }
          }
          if((DSAT_MAX_PDP_TYPE_NUM - 1) == i)
          {
            DS_AT_MSG1_ERROR("Invalid PDP TYPE =%d",param_id );
            return DSAT_FAILURE;
          }
        }
        else if( param_id == DS_PROFILE_TYPE)
        {
        
          if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_value, (byte*)dsat_pp_pdptype[4] ))
          {
            DS_AT_MSG1_ERROR("Invalid PDP TYPE =%d",param_id );
            return DSAT_FAILURE;
          }
          for (i = 0; i < (DSAT_MAX_PDP_TYPE_NUM +1); i++)
          {
          
            if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_value, (byte*)dsat_pp_pdptype[i] ))
            {
              profile->context.pdp_type = (ds_profile_pdp_type_e)i;
              break;
            }
          
          }
        }
        else
        { 
          if(param_id == DS_PROFILE_APN_NAME)
          {
            memscpy(profile->context.apn, DS_PROFILE_APN_STRING_LEN+1, 
                    param_value, DS_PROFILE_APN_STRING_LEN+1);
          }
          else if(param_id == DS_PROFILE_AUTH_USERNAME)
          {          
            memscpy(profile->auth.username, DS_PROFILE_MAX_AUTH_STRING_LEN+1,
                    param_value, DS_PROFILE_MAX_AUTH_STRING_LEN+1);
          }
          else if(param_id == DS_PROFILE_AUTH_PASSWORD)
          {          
            memscpy(profile->auth.password, DS_PROFILE_MAX_AUTH_STRING_LEN+1,
                    param_value, DS_PROFILE_MAX_AUTH_STRING_LEN+1);
          }          
        }
        break;
      }
      case NUM_TYPE:
      {
        if( ATOI_OK != dsatutil_atoi( &value, (byte*) param_value, 10 ))
        {
          DS_AT_MSG1_ERROR("ATOI failed for param_id =%d",param_id );
          return DSAT_FAILURE;
        }

        /* Range Validation */
        for( i = 0; !(param_list[idx].range[i].lower == RANGE_END &&
                       param_list[idx].range[i].upper == RANGE_END ); i++ )
        {
          if( value >= param_list[idx].range[i].lower &&
              value <= param_list[idx].range[i].upper	)
          {
            valid_value = TRUE;
            break;
          }
        }
        if(!valid_value)
        {
          DS_AT_MSG2_ERROR("Range Validation failed for param_id:%d, value:%d",param_id,value);
          return DSAT_FAILURE;
        }

        /*Special case for 3GPP failure timers since they are all tagged to one identifier 
          First:   Get all the 10 timers and 
          Second : Then set back the modified timer(s)*/

        if( param_id == DS_PROFILE_FAILURE_TIMERS)
        {
           memset(&dsat_failure_timers[0], 0, sizeof(dsat_failure_timers));
           param_info.buf = (void*)&dsat_failure_timers[0];
           param_info.len =  sizeof(dsat_failure_timers);

           /*Find which failure timer needs to be modified*/
           for (failure_index = 0;
                failure_index < DS_3GPP_MAX_FAILURE_TIMERS; 
                failure_index++)
           {
             if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_list[idx].abbrv, 
                                                 (byte*)dsat_pp_failure_timers[failure_index] ))
             {
               break;
             }
           }

           /*getting the value for all the timers set (its an array) */
           if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                       (profile_id,&profile_save))
           {    
              DS_AT_MSG1_HIGH("Profile not present first create %d",profile_id);
              return DSAT_ERROR;
           }
           memscpy(&dsat_failure_timers,
                   (sizeof(uint32   )*DS_3GPP_MAX_FAILURE_TIMERS),&profile_save.failure_timers,
                     (sizeof(uint32)*DS_3GPP_MAX_FAILURE_TIMERS));
           if (failure_index < DS_3GPP_MAX_FAILURE_TIMERS) 
           {
             profile->failure_timers[failure_index] = value;
           }
           else
           {
             DS_AT_MSG1_ERROR("ERROR: failure_timer invalid param index: %d" , i);
           }
           break;
        }
        if(param_id == DS_PROFILE_CONTEXT_H_COMP)
        {
          profile->context.h_comp = (ds_profile_header_comp_e_type)value;
          break;
        }
        else if(param_id == DS_PROFILE_CONTEXT_D_COMP)
        {
          profile->context.d_comp = (ds_profile_data_comp_e_type)value;
          break;
        }
        else if(param_id == DS_PROFILE_AUTH_TYPE)
        {
          profile->auth.auth_type = (ds_profile_auth_enum_type)value;
          break;
        }
        else if(param_id == DS_PROFILE_PCFCH_ADDRESS_USING_DHCP_FLAG)
        {
          profile->request_pcscf_address_flag = (value)?TRUE:FALSE;
          break;
        }
        else if(param_id == DS_PROFILE_ACCESS_CTRL_FLAG)
        {
          profile->context.access_ctrl_flag = (ds_profile_access_control_e_type)value;
          break;
        }
        else if(param_id == DS_PROFILE_IM_CN_FLAG)
        {
          profile->im_cn_flag = (value)?TRUE:FALSE;		  
          break;
        }
        else if(param_id == DS_PROFILE_APN_DISABLE_FLAG)
        {
          profile->apn_disable_flag = (value)?TRUE:FALSE;
          break;
        }
        else if(param_id == DS_PROFILE_PDN_INACTIVITY_TIMEOUT)
        {
          profile->inactivity_timer_val = (uint32)value;
          break;
        }
        else if(param_id == DS_PROFILE_APN_CLASS)
        {
          profile->apn_class = (uint8)value;
          break;
        }
        else if(param_id == DS_PROFILE_APN_BEARER)
        {
          profile->apn_bearer = (uint8)value;
          break;
        }
        else if(param_id == DS_PROFILE_OPERATOR_RESERVED_PCO)
        {
          profile->operator_reserved_pco = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_MCC)
        {
          profile->mcc = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_MNC)
        {
          profile->mnc.mnc_includes_pcs_digit = FALSE;
          if(PLMN_STR_MNC_LEN == strlen((char*)param_value))
          {
           profile->mnc.mnc_includes_pcs_digit = TRUE;
          }

          profile->mnc.mnc_digits = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_MAX_PDN_CONN_PER_BLOCK)
        {
          profile->max_pdn_conn_per_blk = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_MAX_PDN_CONN_TIMER)
        {
          profile->max_pdn_conn_time = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_PDN_DISCON_WAIT_TIME)
        {
          profile->pdn_discon_wait_time = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_PDN_REQ_WAIT_TIMER)            
        {
          profile->pdn_req_wait_time = (uint16)value;
          break;
        }
        else if(param_id == DS_PROFILE_ROAMING_DISALLOWED)
        {
          profile->roaming_disallowed = (value)?TRUE:FALSE;
          break;
        }
        else if(param_id == DS_PROFILE_PARAM_ATTACH_WO_PDN)
        {
          profile->attach_wo_pdn = (value)?TRUE:FALSE;
          break;
        }
        else
          break;
      }
      case ADDR_V4_TYPE:
      {
  /*-----------------------------------------------------------------------
    Handling IPV4 address. Converting them to Big Endian format before
    calling DS Profile API's
  -----------------------------------------------------------------------*/

        if( FALSE == dsatutil_strip_quotes_out( (byte*) param_value,
                                                (byte*) param_value,
                                                (word)  param_list[idx].range[0].lower ))
        {
          DS_AT_MSG1_ERROR("Strip quote failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        if(DSAT_OK != dsatutil_convert_tuple( STRING_TO_INT,
                                              TUPLE_TYPE_IPv4,
                                              &v4_addr,
                                              (byte*)param_value,
                                              MAX_V4_ADDR_LEN ,
                                              DSAT_CONVERT_IP) )
        {
          DS_AT_MSG1_ERROR("V4 addr conversion failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        if(param_id == DS_PROFILE_DNS_ADDR_V4_PRIMARY)
        {
          profile->dns_addr.ip_vsn = DS_PROFILE_IP_V4;
          profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv4 = v4_addr;
          profile->dns_addr.primary_dns_addr.ip_vsn = DS_PROFILE_IP_V4;
        }
        else if(param_id == DS_PROFILE_DNS_ADDR_V4_SECONDARY)
        {
          profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv4 = v4_addr;
          profile->dns_addr.secondary_dns_addr.ip_vsn = DS_PROFILE_IP_V4;
          profile->dns_addr.ip_vsn = DS_PROFILE_IP_V4;
        }
        else if(param_id == DS_PROFILE_PDP_ADDR)
        {
          profile->pdp_addr.ip_vsn = DS_PROFILE_IP_V4;
          profile->pdp_addr.pdp_addr.pdp_addr_ipv4 = v4_addr;
        }
        break;
      }
      case ADDR_P_V6_TYPE:
      {
        /* Conditions to verify V6 address */
        if( FALSE == dsatutil_strip_quotes_out( (byte*) param_value,
                                                (byte*) param_value,
                                                (word)  param_list[idx].range[0].lower ))
        {
          DS_AT_MSG1_ERROR("Strip quote failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        if(DSAT_OK != dsatutil_convert_tuple( STRING_TO_INT,
                                              TUPLE_TYPE_IPv6_OCTETS,
                                              &addr[0],
                                              (byte*)param_value,
                                              MAX_IPADDR_STR_LEN,
                                              DSAT_CONVERT_IP) )
        {
          DS_AT_MSG1_ERROR("V6 addr conversion failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        if(param_id == DS_PROFILE_DNS_ADDR_V6_PRIMARY)
        {
          profile->dns_addr.ip_vsn = DS_PROFILE_IP_V6;
          profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[0] = addr[0];
          profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[1] = addr[1];
          profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[2] = addr[2];
          profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[3] = addr[3];
          profile->dns_addr.primary_dns_addr.pdp_addr.pdp_addr_ipv6.\
                                          type = DS_PROFILE_IPV6_32;
        }
        else if(param_id == DS_PROFILE_DNS_ADDR_V6_SECONDARY)
        {
          profile->dns_addr.ip_vsn = DS_PROFILE_IP_V6;
          profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[0] = addr[0];
          profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[1] = addr[1];
          profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[2] = addr[2];
          profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.in6_u.\
                                          u6_addr32[3] = addr[3];
          profile->dns_addr.secondary_dns_addr.pdp_addr.pdp_addr_ipv6.\
                                          type = DS_PROFILE_IPV6_32;		  
        }
        else if(param_id == DS_PROFILE_PDP_ADDR)
        {
          profile->pdp_addr.ip_vsn = DS_PROFILE_IP_V6;
          profile->pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32[0] = addr[0];          
          profile->pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32[1] = addr[1];
          profile->pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32[2] = addr[2];          
          profile->pdp_addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr32[3] = addr[3];
          profile->pdp_addr.pdp_addr.pdp_addr_ipv6.type = DS_PROFILE_IPV6_32;
        }
        break;
      }
      default:
        return DSAT_FAILURE;
    }
    *mask = (*mask)|param_id;
    /*-----------------------------------------------------------------------
      Set the Profile with the value of profile identifier.
    -----------------------------------------------------------------------*/
      DS_AT_MSG2_ERROR("Set Profile. Profile_identifier %d, %d",
                       param_id , mask);
    return DSAT_SUCCESS;
  }

  DS_AT_MSG0_ERROR("ARG not found!");
  return DSAT_FAILURE;
}/*dsatvend_profile_set_param_details*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPRFMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to configure parameters 
  for 3GPP profile.
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcprfmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type             result = DSAT_OK;
  char                              param_type[DSAT_MAX_PROFILE_PARAM_TYPE_LEN];
  char                              param_value[DSAT_MAX_PROFILE_PARAM_VAL_LEN];
  dsat_num_item_type                profile_id;
  ds_profile_status_etype           profile_status;
  uint8                             index;
  ds_profile_type_s                 profile;
  ds_profile_ident_info             ident_info ;
  uint8                             profile_count =0;
  profile_status = DS_PROFILE_RESULT_SUCCESS;  
  ds_profile_list_stype            *profile_list =NULL;
  uint64                  mask =0;

  memset(&profile,0,sizeof(ds_profile_type_s));  
  memset(&ident_info,0,sizeof(ds_profile_ident_info));  

  if( tok_ptr->op == (NA|EQ|AR) )
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    
    /*---------------------------------------------------------------
      Checking no. of arguments.
      Min Args = 2 (Profile ID and minimum one parameter to configure)
      Max Args = 16 (Profile ID and max 15 paramters to configure)
    ---------------------------------------------------------------*/
    if( tok_ptr->args_found < 2 || tok_ptr->args_found > 16 )
    {
      DS_AT_MSG1_ERROR("Invalid Number of arguements  %d", tok_ptr->args_found);
      return(DSAT_ERROR);
    }
    /*---------------------------------------------------------------
      Function call to get Profile ID.
    ---------------------------------------------------------------*/
    if(!VALID_TOKEN(0) || DSAT_FAILURE == dsatvend_get_param_type_value( (char*)tok_ptr->arg[0],
                                            (char*) param_type,
                                            DSAT_MAX_PROFILE_PARAM_TYPE_LEN,
                                            (char*)param_value,
                                             DSAT_MAX_PROFILE_PARAM_VAL_LEN ) )
    {
      DS_AT_MSG0_ERROR("Failed to get Profile ID");
      return DSAT_ERROR;
    }

    if( !( ( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*) "PID")) &&
          ( ATOI_OK == dsatutil_atoi(&profile_id,(byte*)param_value, 10 ))  ) )
    {
      DS_AT_MSG0_ERROR("Invalid first argument. Should be valid profile ID");
      return(DSAT_ERROR);
    }
    /* Test for only CID specified */         
    if (!DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id))
    {
      /* Clear the user specified +CGDCONT context parameters */        
      DS_AT_MSG1_ERROR("Profile ID not in within ranges %d",profile_id);
      return DSAT_ERROR;
    }
    
//Check if the profile exist or not
    if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                     (profile_id,&profile))
    {    
      DS_AT_MSG1_HIGH("Profile not present first create %d",profile_id);
      return DSAT_ERROR;
    }
    ident_info.pdp_type = profile.context.pdp_type;
    if(profile.context.apn[0] != '\0')
      strlcpy(ident_info.apn ,profile.context.apn,DS_PROFILE_APN_STRING_LEN);
    memset(&profile,0,sizeof(ds_profile_type_s));
  /*-------------------------------------------------------------------------
    Begin transaction
  -------------------------------------------------------------------------*/
    for(index = 1; index < tok_ptr->args_found; index++)
    {
      memset(param_type, 0, sizeof(param_type));
      memset(param_value, 0, sizeof(param_value));

     /*---------------------------------------------------------------
       Function call to get each param_type and param_value.
      ---------------------------------------------------------------*/
      if( !VALID_TOKEN(index) || DSAT_FAILURE == dsatvend_get_param_type_value( (char*)tok_ptr->arg[index],
                                                   (char*)param_type,
                                                    DSAT_MAX_PROFILE_PARAM_TYPE_LEN,
                                                   (char*)param_value,
                                                    DSAT_MAX_PROFILE_PARAM_VAL_LEN ))
      {
        DS_AT_MSG1_ERROR("Failed to tokenize argument no. %d", index);
        return DSAT_ERROR;
      } 													
      if( DSAT_FAILURE == dsatvend_profile_set_param_details(
                                                              param_type,
                                                              param_value ,
                                                              profile_id,
                                                              &mask,
                                                              &profile) )
      {
        DS_AT_MSG1_ERROR("Failed to set value for param for agr no. %d", index);
        return DSAT_ERROR;
      }
    } /* end FOR */
    profile_status = ds_profile_create_ex(ident_info,
                                                mask,
                                                &profile,
                                                profile_id
                                                );

    if(profile_status != DS_PROFILE_RESULT_SUCCESS)
    {
       DS_AT_MSG1_HIGH("Profile creation failed : %d",profile_status);
       return DSAT_ERROR;
    }
    
  }

  else if( tok_ptr->op == (NA|EQ|QU) )
  {
     res_buff_ptr->used = (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                                   res_buff_ptr->size ,
                                               "$QCPRFMOD: ");
     (void) dsatvend_display_qcprfmod_test_result( res_buff_ptr );
  }
  else if( tok_ptr->op == (NA|QU) )
  {
    /*---------------------------------------------------------------
      Processing for read command
    ---------------------------------------------------------------*/
      etsipkt_get_profile_list(&profile_list,&profile_count);
      for(index =0; index< profile_count;index++)
      {
        result = dsatvend_profile_update_param_details( index+1,
                                                       res_buff_ptr);
         if(result == DSAT_ERROR)
         {
            DS_SYSTEM_HEAP_MEM_FREE(profile_list);
            return result;
         }
      }
      DS_SYSTEM_HEAP_MEM_FREE(profile_list);
      if( res_buff_ptr->used == 0 )
      {
         res_buff_ptr->used = (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                  res_buff_ptr->size,
                             "$QCPRFMOD: ");
      }
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
 return result;
}/*dsatvend_exec_qcprfmod_cmd*/


/*===========================================================================
FUNCTION PROCESS_QCBANDPREF_PARAMS

DESCRIPTION
  This function process the input band values received in a byte array
  and updates the global variable dsat_bandpref_val list after parsing the data.
 
DEPENDENCIES
  None

RETURN VALUE
  returns a boolean that describes the result of parsing of data. 
  possible values:
  FALSE :    if there was any problem in parsing the data
  TRUE :     if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean process_qcbandpref_params
(
  byte *band_pref  
)
{
  #define MAX_DIG_NUM 6
  
  byte noquotes_band[DSAT_MAX_BANDPREF_STRING_LENGTH] = {0};
  uint32 index = 0;
  uint32 loop = 0;
  byte temp[MAX_DIG_NUM]={0};  /* Maxnumber of digits of a max integer */
  dsat_num_item_type value = 0;
  dsat_bandpref_e_type index_band = DSAT_BAND_PREF_NONE;

  /* Extract data without quoutes */
  if ( FALSE == dsatutil_strip_quotes_out( band_pref, 
                                           noquotes_band, 
                                           DSAT_MAX_BANDPREF_STRING_LENGTH ) )
  {
    DS_AT_MSG0_ERROR("Error in stripping quotes");
    return FALSE;
  }

  /* loop through the data till null */
  while( loop < DSAT_MAX_BANDPREF_STRING_LENGTH && noquotes_band[loop] != '\0')
  {
    if ( index_band >= DSAT_MAX_BANDPREF )
    {
      DS_AT_MSG1_ERROR(" invalid band received %d",index_band);
      return FALSE;
    }
    index = 0;
    /* parse the byte data till the delimiter of , or end of string */
    while (loop < DSAT_MAX_BANDPREF_STRING_LENGTH && noquotes_band[loop] !=',' && noquotes_band [
loop]!= '\0'   )
    {
      /* ERROR if more number of digits than 6 */
      if ( index >= (MAX_DIG_NUM -1))
      {
        return FALSE;
      }
      temp[index++] = noquotes_band[loop++];
    }
    temp[index] = '\0';

    /* convert the parsed ascii format of bytes of data into integer number */
    if ( ATOI_OK != dsatutil_atoi( &value, temp, 10 ) )
    {
      return FALSE;
    }

    dsat_bandpref_val[index_band++]=(dsat_bandpref_e_type)value;	
	DS_AT_MSG1_ERROR(" band received %d",value);
    loop++;
  }
  dsat_bandpref_val[index_band] = DSAT_BAND_PREF_NONE;

  return TRUE;

}/* process_qcbandpref_params */



/*===========================================================================

FUNCTION VEND_PROCESS_INBAND_VALUES

DESCRIPTION
  This function process the inband values and returns the values in CM
  data type. This function is used for ^BANDPREF write command to convert
  the receievd input value to CM band type. 
 
DEPENDENCIES
  None

RETURN VALUE

  CM_BAND_PREF_E_TYPE value. 

SIDE EFFECTS
  None

===========================================================================*/
cm_band_pref_e_type vend_process_inband_values
(
  dsat_bandpref_e_type in_band
)
{
  cm_band_pref_e_type out_band;
  switch( in_band )
  {
    case DSAT_BAND_PREF_BC0_A :
      out_band = CM_BAND_PREF_BC0_A;
      break;
    case DSAT_BAND_PREF_BC0_B :
      out_band = CM_BAND_PREF_BC0_B;
      break;
    case DSAT_BAND_PREF_BC0 :
      out_band = CM_BAND_PREF_BC0;
      break;
    case DSAT_BAND_PREF_BC1 :
      out_band = CM_BAND_PREF_BC1;
      break;
    case DSAT_BAND_PREF_BC3 :
      out_band = CM_BAND_PREF_BC3;
      break;
    case DSAT_BAND_PREF_BC4 :
      out_band = CM_BAND_PREF_BC4;
      break;
    case DSAT_BAND_PREF_BC5 :
      out_band = CM_BAND_PREF_BC5;
      break;
    case DSAT_BAND_PREF_GSM_DCS_1800 :
      out_band = CM_BAND_PREF_GSM_DCS_1800;
      break;
    case DSAT_BAND_PREF_GSM_EGSM_900 :
      out_band = CM_BAND_PREF_GSM_EGSM_900;
      break;
    case DSAT_BAND_PREF_GSM_PGSM_900 :
      out_band = CM_BAND_PREF_GSM_PGSM_900;
      break;
    case DSAT_BAND_PREF_BC6 :
      out_band = CM_BAND_PREF_BC6;
      break;
    case DSAT_BAND_PREF_BC7 :
      out_band = CM_BAND_PREF_BC7;
      break;
    case DSAT_BAND_PREF_BC8 :
      out_band =  CM_BAND_PREF_BC8;
      break;
    case DSAT_BAND_PREF_BC9 :
      out_band = CM_BAND_PREF_BC9;
      break;
    case DSAT_BAND_PREF_BC10 :
      out_band = CM_BAND_PREF_BC10;
      break;
    case DSAT_BAND_PREF_BC11 :
      out_band = CM_BAND_PREF_BC11;
      break;
    case DSAT_BAND_PREF_BC12 :
      out_band = CM_BAND_PREF_BC12;
      break;
    case DSAT_BAND_PREF_BC14 :
      out_band = CM_BAND_PREF_BC14;
      break;
    case DSAT_BAND_PREF_BC15 :
      out_band = CM_BAND_PREF_BC15;
      break;
    case DSAT_BAND_PREF_BC16 :
      out_band = CM_BAND_PREF_BC16;
      break;
    case DSAT_BAND_PREF_GSM_450 :
      out_band = CM_BAND_PREF_GSM_450;
      break;
    case DSAT_BAND_PREF_GSM_480 :
      out_band = CM_BAND_PREF_GSM_480;
      break;
    case DSAT_BAND_PREF_GSM_750 :
      out_band = CM_BAND_PREF_GSM_750;
      break;
    case DSAT_BAND_PREF_GSM_850 :
      out_band = CM_BAND_PREF_GSM_850;
      break;
    case DSAT_BAND_PREF_GSM_RGSM_900 :
      out_band = CM_BAND_PREF_GSM_RGSM_900;
      break;
    case DSAT_BAND_PREF_GSM_PCS_1900 :
      out_band = CM_BAND_PREF_GSM_PCS_1900;
      break;
	/*
    case DSAT_BAND_PREF_WCDMA_I_IMT_2000 :
      out_band = CM_BAND_PREF_WCDMA_I_IMT_2000;
      break;
    case DSAT_BAND_PREF_WCDMA_II_PCS_1900 :
      out_band = CM_BAND_PREF_WCDMA_II_PCS_1900;
      break;
    case DSAT_BAND_PREF_WCDMA_III_1700 :
      out_band = CM_BAND_PREF_WCDMA_III_1700;
      break;
    case DSAT_BAND_PREF_WCDMA_IV_1700 :
      out_band = CM_BAND_PREF_WCDMA_IV_1700;
      break;
    case DSAT_BAND_PREF_WCDMA_V_850 :
      out_band = CM_BAND_PREF_WCDMA_V_850;
      break;
    case DSAT_BAND_PREF_WCDMA_VI_800 :
      out_band = CM_BAND_PREF_WCDMA_VI_800;
      break;
    case DSAT_BAND_PREF_WCDMA_VII_2600 :
      out_band = CM_BAND_PREF_WCDMA_VII_2600;
      break;
    case DSAT_BAND_PREF_WCDMA_VIII_900 :
      out_band = CM_BAND_PREF_WCDMA_VIII_900;
      break;
    case DSAT_BAND_PREF_WCDMA_IX_1700 :
      out_band = CM_BAND_PREF_WCDMA_IX_1700;
      break;
    case DSAT_BAND_PREF_WLAN_2400_US :
      out_band = CM_BAND_PREF_WLAN_2400_US;
      break;
    case DSAT_BAND_PREF_WLAN_2400_JP :
      out_band = CM_BAND_PREF_WLAN_2400_JP;
      break;
    case DSAT_BAND_PREF_WLAN_2400_ETSI :
      out_band = CM_BAND_PREF_WLAN_2400_ETSI;
      break;
    case DSAT_BAND_PREF_WLAN_2400_SP :
      out_band = CM_BAND_PREF_WLAN_2400_SP;
      break;
    case DSAT_BAND_PREF_WLAN_2400_FR :
      out_band = CM_BAND_PREF_WLAN_2400_FR;
      break;
    case DSAT_BAND_PREF_WLAN_5000_US :
      out_band = CM_BAND_PREF_WLAN_5000_US;
      break;
    case DSAT_BAND_PREF_WLAN_5000_JP :
      out_band = CM_BAND_PREF_WLAN_5000_JP;
      break;
    case DSAT_BAND_PREF_WLAN_5000_ETSI :
      out_band = CM_BAND_PREF_WLAN_5000_ETSI;
      break;
    case DSAT_BAND_PREF_WLAN_5000_SP :
      out_band = CM_BAND_PREF_WLAN_5000_SP;
      break;
    case DSAT_BAND_PREF_WLAN_5000_FR :
      out_band = CM_BAND_PREF_WLAN_5000_FR;
      break;*/
    case DSAT_BAND_PREF_ANY :
      out_band = CM_BAND_PREF_ANY;
      break;
    default:
      out_band = CM_BAND_PREF_NONE;
  }

  return out_band;
}

/*===========================================================================

FUNCTION VEND_PROCESS_OUTBAND_VALUES

DESCRIPTION
  This function process is used for ^BANDPREF read command to convert
  the CM band pref type to DSAT band pref type and prepares a list to 
  display the data in strings. 
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE: Success in conversion.
  FAIL: Failed in conversion. 

SIDE EFFECTS
  None

===========================================================================*/
boolean vend_process_outband_values
(
  cm_band_pref_e_type  out_band,
  dsat_bandpref_e_type result_list[],
  uint32               result_list_size
)
{
  uint32 out_index = 0;
  dsat_bandpref_e_type local_dsat_band = DSAT_BAND_PREF_NONE;
  cm_band_pref_e_type  local_cm_band   = CM_BAND_PREF_NONE;

  /* Reset the values to 0 so that the last data after valid band would be 
     DSAT_BAND_PREF_NONE */
  memset((void *)result_list, 0X00, result_list_size );
  while ( DSAT_BAND_PREF_MAX > local_dsat_band )
  {
    if ( result_list_size < out_index )
    {
      DS_AT_MSG2_ERROR("Insufficient space for BAND PREF list size %d, out_index %d",
                                                        result_list_size,out_index);
      return FALSE;
    }
    /* Point to the next DSAT_BAND_PREF */
    ++local_dsat_band;
    /* get the corresponding CM_BAND_PREF */
    local_cm_band = vend_process_inband_values ( local_dsat_band );
    /* Verify if this is set in CM */
    if ( local_cm_band == (out_band & local_cm_band ) )
    {
      result_list[out_index++] = local_dsat_band;
    }
  }
  DS_AT_MSG0_MED("Processing of out bands done; List is updated");
  return TRUE;
}/* vend_process_outband_values  */

/*===========================================================================

FUNCTION DSAVEND_PROCESS_PREFMODE_CMD

DESCRIPTION
  This function process the response for the ^PREFMODE command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_CMD_ERR_RESP:  If mode value is not valid. 
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatvend_process_prefmode_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  cm_cmd_user_pref_update_type         user_pref;
  DS_ASSERT( pref_mode != NULL );
  DS_AT_MSG2_HIGH("Processing PREFMODE cmd %d, mode pref %d", cmd_type,
                                           pref_mode->network_rat_mode_pref);
  switch ( cmd_type )
  {
    case DSAT_PENDING_PREFMODE_SYSMODE_READ:
    {
      dsm_item_type *res_buff_ptr = NULL;
      switch ( pref_mode->network_rat_mode_pref )
      {
        case CM_MODE_PREF_CDMA_ONLY:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,DSAT_PREFMODE_SYSMODE_CDMA,NUM_TYPE)
          break;
        case CM_MODE_PREF_HDR_ONLY:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,DSAT_PREFMODE_SYSMODE_HDR,NUM_TYPE)
          break;
        case CM_MODE_PREF_CDMA_HDR_ONLY:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,DSAT_PREFMODE_SYSMODE_HYBRID_CDMA_HDR,
NUM_TYPE)
          break;
        case CM_MODE_PREF_NO_CHANGE:
          /* Nothing changed report the old prefmode value */
          break;
        case CM_MODE_PREF_AUTOMATIC:

        default:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,DSAT_PREFMODE_SYSMODE_AUTOMATIC,NUM_TYPE)
          DS_AT_MSG1_ERROR("Invalid pref mode %d received, seeting it to Automatic",
                                          pref_mode->network_rat_mode_pref);
      }
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                             "^PREFMODE:%d", 
                                             (dsat_num_item_type)dsatutil_get_val(
                                              DSAT_VENDOR_PREFMODE_IDX,0,NUM_TYPE));
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      result = DSAT_OK;
    }
      break;
    case DSAT_PENDING_PREFMODE_SYSMODE_WRITE:
    {
      /* Nothing to do  */
      result = DSAT_OK;
    }
      break;
    case DSAT_PENDING_PREFMODE_BANDPREF_READ:
    {

      dsat_bandpref_e_type  result_list[DSAT_MAX_BANDPREF];
      dsm_item_type *res_buff_ptr = NULL;
      dsat_string_item_type  *buffer = NULL;
      dsat_num_item_type      size   = 0;

      if ( FALSE == vend_process_outband_values( pref_mode->network_band_pref,
                                   &result_list[0],
                                   (uint32)DSAT_MAX_BANDPREF ) )
      {
        DS_AT_MSG0_ERROR("Band preferene query failed");
        result = DSAT_ERROR;
        break;
      }
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf ( (char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "$QCBANDPREF: \n");
      if((size = dsat_read_efs_data("bandpref", DSAT_FILE_STR_TYPE, (void **)&buffer)))
      {
        result = DSAT_OK;
        if(!dsat_process_efs_data(buffer,size,res_buff_ptr,DSAT_EFS_PREFMODE_QUERY,&result_list[0]
))
        {
          result = DSAT_ERROR;
        }
      }
        /*free the buffer here*/
      dsatutil_free_memory_ext((void **) &buffer);
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
    }
      break;
    case DSAT_PENDING_PREFMODE_BANDPREF_VERIFY:
      {
        uint32 index = 0;
        cm_band_pref_e_type temp_band_pref = CM_BAND_PREF_NONE;
        cm_band_pref_e_type req_band_pref  = CM_BAND_PREF_NONE;
        cm_pref_term_e_type pref_term = CM_PREF_TERM_PWR_CYCLE;

        /* Prepare the required CM band pref type from the received input
           which is stored as DSAT band pref type. */
        while ( dsat_bandpref_val[index] != DSAT_BAND_PREF_NONE )
        {
          if ( CM_BAND_PREF_NONE == 
                ( temp_band_pref = vend_process_inband_values(          
                                               dsat_bandpref_val[index])))
          {
            DS_AT_MSG1_ERROR("Invalid band preference %d",dsat_bandpref_val[index ]);
            memset((void *)dsat_bandpref_val, 0x0, sizeof(dsat_bandpref_val));
            SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
            return DSAT_ERROR;
          }
          req_band_pref |= temp_band_pref;
          ++index;
        }
        /* Verify if the requested bandpref is valid in current mode */
        DS_AT_MSG2_HIGH("network_rat_mode_pref = %d, req_band_pref = %d",
                       pref_mode->network_rat_mode_pref, req_band_pref);
        if ( FALSE == cm_is_valid_mode_band_pref( 
                                     pref_mode->network_rat_mode_pref, 
                                     req_band_pref ))     
        {
          DS_AT_MSG0_ERROR("Invalid Mode+Band Preference");
          SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
          (void)dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
          return DSAT_CMD_ERR_RSP;
        }

        if ( DSAT_BANDPREF_PERS == dsat_bandpref_pers_val )
        {
          pref_term = CM_PREF_TERM_PERMANENT;
        }
        cm_user_pref_init(&user_pref);
        user_pref.client_id = dsatcm_client_id;
        user_pref.pref_term = pref_term;
        user_pref.band_pref = req_band_pref;
        user_pref.network_sel_mode_pref = CM_NETWORK_SEL_MODE_PREF_AUTOMATIC;
        if(TRUE == cm_user_pref_update_req(&user_pref,
                               dsatcmif_ph_cmd_cb_func,
                               NULL
                                ))
        {
          DS_AT_MSG0_HIGH(" Band pref phase 1 succesful, posted commadn for phase 2");
          return DSAT_ASYNC_CMD;
        }
       else
       return DSAT_ERROR;

      }
    case DSAT_PENDING_PREFMODE_BANDPREF_WRITE:
    {
      /* Setting of band pref is done, Nothing to do */
      return DSAT_OK;
    }
    default:
      DS_AT_MSG1_ERROR("Preference mode mismatch %d",pref_mode->network_rat_mode_pref);
      DS_ASSERT(0);
  }

  return result;
}/* dsatvend_process_prefmode_cmd */

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCBANDPREF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcbandpref command.
  This command provides the ability to set/get the band preferences

  
<band_pref>: Is a string containing indexes separated by comma. 
for eg: "1,2,3,4,5,6" which means set all the bandprefs which has indexes from
1,2,3,4,5,6 in the test command.

<persitence>:0/1

currently, the band preference change is not persistent. 

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if success.
SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcbandpref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  
  dsat_error_info_s_type  err_info;
  dsat_num_item_type      size = 0;
  dsat_string_item_type   *buffer = NULL;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    if ( ( tok_ptr->args_found > 2 ) || ( !VALID_TOKEN(1) ))
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

  /* Prepare a list of band pref values out of the given string and 
       update it in dsat_bandpref_val*/
    if ( FALSE == process_qcbandpref_params(tok_ptr->arg[1]))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 1;
      goto send_error;
    }

    /* Extract persistence details; default is NOT PERSISTANT  */
    err_info.arg_num = 0;
    if ( VALID_TOKEN(0) )
    {
       if ( ( ATOI_OK != dsatutil_atoi( &dsat_bandpref_pers_val,
                                     tok_ptr->arg[0], 
                                     10 ) ) )
       {
         err_info.errval =  DSAT_ERR_INVALID_ARGUMENT;
         goto send_error;
       }
       if ( ( dsat_bandpref_pers_val != DSAT_BANDPREF_NON_PERS ) &&
           ( dsat_bandpref_pers_val != DSAT_BANDPREF_PERS ) )
       {
         err_info.errval =  DSAT_ERR_PARAMETER_OUT_OF_RANGE;
         goto send_error;
       }
    }
    else
    {
       /* Set default persistance value to 0 when second parameter is not given */
       dsat_bandpref_pers_val = DSAT_BANDPREF_NON_PERS;
    }
    /* Setting bandpref would happen in two stages. 
       1. Get the mode pref to validate the band pref values 
       2. Set the band pref */
    SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_BANDPREF_VERIFY)
    /* The return value is ignored as both success and failure cases
      * are handled by the call back function */
    (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                 NULL,
                                 dsatcm_client_id
                                 );
    result = DSAT_ASYNC_CMD;
  }
  if ( (NA|QU) == tok_ptr->op )
  {
    SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_BANDPREF_READ)
    /* The return value is ignored as both success and failure cases
     * are handled by the call back function */

    (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                 NULL,
                                 dsatcm_client_id
                                 );
    
    result = DSAT_ASYNC_CMD;

  }
  if ( (NA|EQ|QU) == tok_ptr->op )
  {

    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
                                 "$QCBANDPREF: (0-1),\n");

    if((size = dsat_read_efs_data("bandpref", DSAT_FILE_STR_TYPE, (void **)&buffer)))
    {
      result = DSAT_OK;
      if(!dsat_process_efs_data(buffer,size,res_buff_ptr,DSAT_EFS_PREFMODE_TEST,NULL))
      {
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /*free the buffer here*/
  dsatutil_free_memory_ext((void **) &buffer);
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
}/* dsatvend_exec_qcbandpref_cmd */

#endif
#endif

#ifdef FEATURE_DSAT_ATCOP_VEND
/*===========================================================================

FUNCTION DSAVEND_GET_RAT_STATE

DESCRIPTION
  This function gives the rat for which the QCCELLSCAN was issued

DEPENDENCIES
  None

RETURN VALUE
  Returns an rat on which the QCCELLSCAN was issued.

SIDE EFFECTS
  None

===========================================================================*/
int* dsatvend_get_rat_state()
{
  return &qccellscan_rat;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCELLSCAN_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. 
  It executes $QCCELLSCAN command. 

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qccellscan_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type               rattype;
  dsati_mode_e_type                current_mode;
  uint64                           band_pref[4];  
  dsat_result_enum_type            result = DSAT_OK;
  cm_band_pref_e_type              band_pref_lmt = CM_BAND_PREF_NO_CHANGE;
  cm_band_pref_e_type              lte_band_pref_lmt = CM_BAND_PREF_NO_CHANGE;
  cm_band_pref_e_type              tds_band_pref_lmt = CM_BAND_PREF_NO_CHANGE;
  cm_ciot_user_lte_pref_s_type     ciot_lte_pref;

  dsat_num_item_type mode_val = (dsat_num_item_type)dsatutil_get_val(
                      DSAT_VENDOR_QCCELLSCAN_IDX, 0,NUM_TYPE);

  sys_radio_access_tech_e_type sys_mode = SYS_RAT_MAX;
  sys_mode = emm_get_active_rat(); 
  
  DS_AT_MSG2_HIGH("args %d sys_mode %d",tok_ptr->args_found,sys_mode);
  if( tok_ptr->op == (NA|EQ|QU) || tok_ptr->op == (NA|QU) )
  {
     res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                           res_buff_ptr->size,
                                           "$QCCELLSCAN: (%s),%s\n",
                                            dsat_qccellscan_rat_valstr,
                                            "(band 1-64),(band 65-128),"
                                            "(band128-192),(band193-256)"
                                            );
  }
  else if( tok_ptr->op == (NA|EQ|AR) )
  {
    memset(&ciot_lte_pref, 0, sizeof(cm_ciot_user_lte_pref_s_type));
    memset(&band_pref,0,sizeof(uint64)*4);
    current_mode = dsatcmdp_get_current_mode();
    if(current_mode != DSAT_MODE_LTE && current_mode != DSAT_MODE_GSM )
    {
      DS_AT_MSG1_HIGH(": invalid current_mode %d", current_mode);
      return DSAT_ERROR;
    }
    if(mode_val == 0)
    {
      if(VALID_TOKEN(0) )
      {
        if ( ATOI_OK != dsatutil_atoi (&rattype, tok_ptr->arg[0], 10) )
        {         
          DS_AT_MSG0_ERROR(" Cannot convert the rat param ");
          return DSAT_ERROR;
        } 
        if((rattype != DSAT_QCCELLSCAN_RAT_GSM) 
        && (rattype != DSAT_QCCELLSCAN_RAT_LTE_M1) 
        && (rattype != DSAT_QCCELLSCAN_RAT_LTE_NB1) 
#ifdef FEATURE_NBIOT_NTN
        && (rattype != DSAT_QCCELLSCAN_RAT_LTE_NB1_NTN)
#endif /* FEATURE_NBIOT_NTN */
        )
        {
          DS_AT_MSG0_ERROR("QCCELLSCAN wrong rat type passed should be 0,7,9,10");
          return DSAT_ERROR;
        }
        if((sys_mode == SYS_RAT_GSM_RADIO_ACCESS)&&
           (rattype != DSAT_QCCELLSCAN_RAT_GSM))
        {
          DS_AT_MSG0_ERROR("QCCELLSCAN wrong rat type passed should be 0");
          return DSAT_ERROR;
        }
        else if((sys_mode == SYS_RAT_LTE_M1_RADIO_ACCESS)&&
                (rattype != DSAT_QCCELLSCAN_RAT_LTE_M1))
        {
          DS_AT_MSG0_ERROR("QCCELLSCAN wrong rat type passed should be 7");
          return DSAT_ERROR;
        }
        else if((sys_mode == SYS_RAT_LTE_NB1_RADIO_ACCESS)&&
                 (rattype != DSAT_QCCELLSCAN_RAT_LTE_NB1))
        {
          DS_AT_MSG0_ERROR("QCCELLSCAN wrong rat type passed should be 9");
          return DSAT_ERROR;
        }
#ifdef FEATURE_NBIOT_NTN
        else if((sys_mode == SYS_RAT_LTE_NB1_NTN_RADIO_ACCESS)&&
                 (rattype != DSAT_QCCELLSCAN_RAT_LTE_NB1_NTN))
        {
          DS_AT_MSG0_ERROR("QCCELLSCAN wrong rat type passed should be 10");
          return DSAT_ERROR;
        }
#endif /* FEATURE_NBIOT_NTN */
      }
      else
      {
         DS_AT_MSG0_HIGH("QCCELLSCAN rat type not passed should be given input");
         return DSAT_ERROR;
      }

      if(VALID_TOKEN(1))
      {
        if ( ATOI_OK != dsatutil64_atoi (&band_pref[0], tok_ptr->arg[1]) )
        {
          DS_AT_MSG0_ERROR(" Cannot convert the band[0] param ");
          return DSAT_ERROR;
        } 
        DS_AT_MSG2_HIGH("band_pref[0] %llu",band_pref[0],(band_pref[0]>>32));
      }

      if((tok_ptr->args_found >2) &&(rattype != DSAT_QCCELLSCAN_RAT_GSM))
      {
        if(VALID_TOKEN(2))
        {
           if ( ATOI_OK != dsatutil64_atoi (&band_pref[1], tok_ptr->arg[2]) )
           {
             DS_AT_MSG0_ERROR(" Cannot convert the band[1] param ");
             return DSAT_ERROR;
           } 
           DS_AT_MSG2_HIGH("band_pref[1] %llu",band_pref[1],(band_pref[1]>>32));
        }
        else 
        {
          band_pref[1] = 0;
        }

        if(VALID_TOKEN(3))
        {
           if ( ATOI_OK != dsatutil64_atoi (&band_pref[2], tok_ptr->arg[3]) )
           {
             DS_AT_MSG0_ERROR(" Cannot convert the band[2] param ");
             return DSAT_ERROR;
           } 
           DS_AT_MSG2_HIGH("band_pref[2] %llu",band_pref[2],(band_pref[2]>>32));
        }
        else 
        {
         band_pref[2] = 0;
        }

        if(VALID_TOKEN(4))
        {

          if ( ATOI_OK != dsatutil64_atoi (&band_pref[3], tok_ptr->arg[4]))
          {
             DS_AT_MSG0_ERROR(" Cannot convert the band[3] param ");
             return DSAT_ERROR;
           } 
           DS_AT_MSG2_HIGH("band_pref[3] %llu",band_pref[3], (band_pref[3]>>32));
        }
        else 
        {
         band_pref[3] = 0;
        }
      }

      if((band_pref[0] ==0)&&(band_pref[1] == 0)&&(band_pref[2]==0)&&(band_pref[3] ==0))
      { 
        if(rattype == DSAT_QCCELLSCAN_RAT_LTE_NB1)
        {
         band_pref[0] =  CM_BAND_PREF_LTE_NB1_ANY;
        }
        else if(rattype == DSAT_QCCELLSCAN_RAT_LTE_M1)
        {
          band_pref[0] = CM_BAND_PREF_LTE_M1_ANY;
        }
        else if (rattype == DSAT_QCCELLSCAN_RAT_GSM)
        {
          band_pref[0] = CM_BAND_PREF_ANY;
        }
#ifdef FEATURE_NBIOT_NTN
        else if(rattype == DSAT_QCCELLSCAN_RAT_LTE_NB1_NTN)
        {
         band_pref[0] = CM_BAND_PREF_LTE_NB1_ANY;
        }
#endif /* FEATURE_NBIOT_NTN */

        DS_AT_MSG2_HIGH(" band_pref[0],[1],[2],[3] are 0 band_pref[0] %llu",
                          band_pref[0],(band_pref[0]>>32));
      }

      if(rattype == DSAT_QCCELLSCAN_RAT_LTE_NB1)
      {
        memset(&ciot_lte_pref.lte_nb1_band_pref, 0, sizeof(sys_lte_band_mask_e_type));
        memscpy(&ciot_lte_pref.lte_nb1_band_pref, sizeof(sys_lte_band_mask_e_type), &band_pref, sizeof(sys_lte_band_mask_e_type));
        ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NB1;
      }
      else if(rattype == DSAT_QCCELLSCAN_RAT_LTE_M1)
      {
        memset(&ciot_lte_pref.lte_m1_band_pref, 0, sizeof(sys_lte_band_mask_e_type));        
        memscpy(&ciot_lte_pref.lte_m1_band_pref, sizeof(sys_lte_band_mask_e_type), &band_pref, sizeof(sys_lte_band_mask_e_type));
        ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_M1;
      }
      else if (rattype == DSAT_QCCELLSCAN_RAT_GSM)
      {
        band_pref_lmt = band_pref[0];        
      }
#ifdef FEATURE_NBIOT_NTN
      else if(rattype == DSAT_QCCELLSCAN_RAT_LTE_NB1_NTN)
      {
        memset(&ciot_lte_pref.lte_nb1_ntn_band_pref, 0, sizeof(sys_lte_band_mask_e_type));
        memscpy(&ciot_lte_pref.lte_nb1_ntn_band_pref, sizeof(sys_lte_band_mask_e_type), &band_pref, sizeof(sys_lte_band_mask_e_type));
        ciot_lte_pref.lte_op_mode_pref = CM_LTE_IOT_OP_MODE_PREF_NB1_NTN;
      }
#endif /* FEATURE_NBIOT_NTN */

      DS_AT_MSG1_HIGH("rat type  %d",rattype);
            
      DS_AT_MSG8_HIGH("lte_m1_band_pref.bits_1_64 %llu,"
                      "bits_65_128 %llu,"
                      "bits_129_192 %llu,"
                      "bits_193_256 %llu,",
                      (uint32)ciot_lte_pref.lte_m1_band_pref.bits_1_64,
                      (uint32)(ciot_lte_pref.lte_m1_band_pref.bits_1_64 >>32),
                      (uint32)ciot_lte_pref.lte_m1_band_pref.bits_65_128,
                      (uint32)(ciot_lte_pref.lte_m1_band_pref.bits_65_128 >>32),
                      (uint32)ciot_lte_pref.lte_m1_band_pref.bits_129_192,
                      (uint32)(ciot_lte_pref.lte_m1_band_pref.bits_129_192 >>32),
                      (uint32)ciot_lte_pref.lte_m1_band_pref.bits_193_256,         
                      (uint32)(ciot_lte_pref.lte_m1_band_pref.bits_193_256   >>32));
      
      DS_AT_MSG8_HIGH("lte_nb1_band_pref.bits_1_64 %llu"
                      "bits_65_128 %llu,"
                      "bits_129_192 %llu,"
                      "bits_193_256 %llu,",
                      (uint32)ciot_lte_pref.lte_nb1_band_pref.bits_1_64,
                      (uint32)(ciot_lte_pref.lte_nb1_band_pref.bits_1_64 >>32),
                      (uint32)ciot_lte_pref.lte_nb1_band_pref.bits_65_128 ,
                      (uint32)(ciot_lte_pref.lte_nb1_band_pref.bits_65_128 >>32),
                      (uint32)ciot_lte_pref.lte_nb1_band_pref.bits_129_192   ,
                      (uint32)(ciot_lte_pref.lte_nb1_band_pref.bits_129_192    >>32),
                      (uint32)ciot_lte_pref.lte_nb1_band_pref.bits_193_256,
                      (uint32)(ciot_lte_pref.lte_nb1_band_pref.bits_193_256 >>32));

#ifdef FEATURE_NBIOT_NTN
      DS_AT_MSG8_HIGH("lte_nb1_ntn_band_pref.bits_1_64 %llu,"
                      "bits_65_128 %llu,"
                      "bits_129_192 %llu,"
                      "bits_193_256 %llu,",
                      (uint32)ciot_lte_pref.lte_nb1_ntn_band_pref.bits_1_64,
                      (uint32)(ciot_lte_pref.lte_nb1_ntn_band_pref.bits_1_64 >>32),
                      (uint32)ciot_lte_pref.lte_nb1_ntn_band_pref.bits_65_128,
                      (uint32)(ciot_lte_pref.lte_nb1_ntn_band_pref.bits_65_128 >>32),
                      (uint32)ciot_lte_pref.lte_nb1_ntn_band_pref.bits_129_192,
                      (uint32)(ciot_lte_pref.lte_nb1_ntn_band_pref.bits_129_192 >>32),
                      (uint32)ciot_lte_pref.lte_nb1_ntn_band_pref.bits_193_256,         
                      (uint32)(ciot_lte_pref.lte_nb1_ntn_band_pref.bits_193_256   >>32));
#endif /* FEATURE_NBIOT_NTN */

      DS_AT_MSG2_HIGH("band_pref_lmt %llu ",band_pref_lmt,(band_pref_lmt>>32));
      DSATUTIL_SET_VAL(DSAT_VENDOR_QCCELLSCAN_IDX,0,0,1,NUM_TYPE)
        
      qccellscan_rat = rattype;            
      if (FALSE == cm_ph_cmd_get_networks_per_subs_ciot(        
                                       dsatcmif_ph_cmd_cb_func,
                                       NULL,
                                       dsatcm_client_id,
                                       CM_NETWORK_LIST_CELL_SEARCH,
                                       CM_MODE_PREF_GWL,
                                       0,
                                       band_pref_lmt,
                                       lte_band_pref_lmt,
                                       tds_band_pref_lmt,
                                       ciot_lte_pref
                                      ))
      {      
        DSATUTIL_SET_VAL(DSAT_VENDOR_QCCELLSCAN_IDX,0,0,(dsat_num_item_type)0,NUM_TYPE)
        result = DSAT_ERROR;
      }

    }
    else 
    {
      DS_AT_MSG0_HIGH("qccellscan is in progress ");
      return DSAT_ERROR;
    }
  }
  return result;
}


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCAPNE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcapne command.
  at$qcapne is used to edit APN values in APN table. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcapne_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define MAX_APNE_OUTPUT 150
  uint8                      token_idx         = 0;
  uint16                     i                 = 0;
  uint16                     index             = 0;
  boolean                    valid_status      = FALSE;
  dsat_num_item_type         profile_params    = 0;
  dsat_num_item_type         min_val           = 0;
  dsat_num_item_type         profile_id    = 0;
  dsat_string_item_type      apnb_type[10];
  dsat_string_item_type      pdptype_val[8];
  byte                       temp_buff[8];
  byte                       apn_buff[DS_PROFILE_APN_STRING_LEN+1];
  ds_profile_type_s          profile;  
  uint8                      profile_count =0;
  ds_profile_ident_info      ident_info ;
  uint64                     mask =0;
  ds_profile_status_etype    profile_status;
  ds_profile_list_stype     *profile_list =NULL;

  dsat_qcprofile_param_details_s_type     *param_list = &profile_qcapne_param_details_list[0];
  dsat_error_info_s_type     err_info;

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments. */
    if ( tok_ptr->args_found != parse_table->compound )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

    /* Profile ID  */

    VALID_NUM_TOKEN(TOKEN_IDX_0,0,DSAT_EXT_PROFILE_NUM_MAX, &profile_id)
    if (valid_status && DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id))
    {
    //Check if the profile exist or not
      if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                       (profile_id,&profile))
      {    
        DS_AT_MSG1_HIGH("Profile not present first create ",profile_id);
        return DSAT_ERROR;
      }
      ident_info.pdp_type = profile.context.pdp_type;
      if(profile.context.apn[0] != '\0')
        strlcpy(ident_info.apn ,profile.context.apn,DS_PROFILE_APN_STRING_LEN);
      memset(&profile,0,sizeof(ds_profile_type_s));
    }
    else
    {
    
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      goto send_error;
    }
    /* APN CLASS */

    for (i=0;i<ARR_SIZE(profile_qcapne_param_details_list);i++)
    {
      min_val        = 0;
      profile_params = 0;

      if (i == 0)
      {
        token_idx = TOKEN_IDX_1;
        VALID_NUM_TOKEN(token_idx, min_val, param_list[i].max_value,&profile_params)
        profile.apn_class = profile_params;
        mask = mask|DS_PROFILE_APN_CLASS;		

      }
      else if (i == 1)
      {
        token_idx = TOKEN_IDX_5;
        VALID_NUM_TOKEN(token_idx, min_val, param_list[i].max_value,&profile_params)
        profile.apn_disable_flag = profile_params;
        mask = mask|DS_PROFILE_APN_DISABLE_FLAG;		
	    DS_AT_MSG1_HIGH("Profile not present first create %d",profile_params);
      }
      else if (i == 2)
      {
        token_idx = TOKEN_IDX_6;
        VALID_NUM_TOKEN(token_idx, min_val, param_list[i].max_value,&profile_params)
        profile.inactivity_timer_val = profile_params;
        mask = mask|DS_PROFILE_PDN_INACTIVITY_TIMEOUT;		
	    DS_AT_MSG1_HIGH("Profile not present first create %d",profile_params);
      }
      if (!valid_status)
      {      
        DS_AT_MSG1_HIGH("Profile not present first create ",profile_id);
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        goto send_error;
      }
    }
    /*         APN Name          */
    VALID_STRING_TOKEN(TOKEN_IDX_2, tok_ptr->arg[2], apn_buff,sizeof(apn_buff))
    if (valid_status)
    {
      (void)strlcpy((char *)profile.context.apn,(const char *)apn_buff,DS_PROFILE_APN_STRING_LEN + 1);
      mask = mask|DS_PROFILE_APN_NAME;
    }

    /*         PDP Type          */
    VALID_STRING_TOKEN(TOKEN_IDX_3, tok_ptr->arg[3], pdptype_val,sizeof(pdptype_val))
    if (valid_status)
    {
      if( !dsatutil_strcmp_ig_sp_case((const byte *)pdptype_val, (const byte *)"IPV6") )
      {
        profile.context.pdp_type = DS_PROFILE_PDP_IPV6;
      }
      else if( !dsatutil_strcmp_ig_sp_case((const byte *)pdptype_val, (const byte *)"IPV4V6") )
      {
        profile.context.pdp_type = DS_PROFILE_PDP_IPV4V6;
      }
      else
      {

        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 3;
        goto send_error;
      }      
      mask = mask|DS_PROFILE_TYPE;
    }

    /*         APN Bearer type          */

    VALID_STRING_TOKEN(TOKEN_IDX_4, tok_ptr->arg[4], apnb_type,sizeof(apnb_type))
    if (valid_status)
    {
      if( dsatutil_strcmp_ig_sp_case((const byte *)apnb_type, (const byte *)"LTE") )
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        goto send_error;
      }
      profile.apn_bearer = 4;//LTE
      mask = mask|DS_PROFILE_APN_BEARER;
    }
    profile_status = ds_profile_create_ex(ident_info,
                                                 mask,
                                                 &profile,
                                                 profile_id
                                                );
    if(profile_status != DS_PROFILE_RESULT_SUCCESS)
    {
       DS_AT_MSG1_HIGH("Profile creation failed : %d",profile_status);
       return DSAT_ERROR;
    }
  }
  
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
      res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                     res_buff_ptr->size,
                             "$QCAPNE: (%d-%d),(%d-%d),,(%s and %s),\n%s,(%d-%d ),(%d-%d)\n",
                                      DSAT_LEGACY_PROFILE_NUM_MIN,
                                      DSAT_LEGACY_PROFILE_NUM_MAX,
                                      DSAT_APN_CLASS_ONE,DSAT_APN_CLASS_MAX,
                               "IPV6","IPV4V6","LTE",0,1,
                                      0,DEF_MAX_INACTIVITY_TIMER_VAL);
  }
  else if(tok_ptr->op == (NA|QU))
  {    
    if(DSAT_ERROR == etsipkt_get_profile_list(&profile_list,
                                               &profile_count)
                                               )
    {    
	  DS_SYSTEM_HEAP_MEM_FREE(profile_list);
      return DSAT_ERROR;
    }
    for(index = 0; index < profile_count ;index++)
    {
      memset((void*)&profile, 0, sizeof(ds_profile_type_s));
      if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                     (index+1,&profile))
      {    
        DS_AT_MSG1_HIGH("Profile not present first create id %d",index+1);
        DS_SYSTEM_HEAP_MEM_FREE(profile_list);
        return DSAT_ERROR;
      }
      else
      {
        memset((void *)temp_buff,0,sizeof(temp_buff));
        switch(profile.context.pdp_type)
        {
          case DS_PROFILE_PDP_IPV6:
            strlcpy((char*)temp_buff,"IPV6",sizeof(temp_buff));
            break;
          case DS_PROFILE_PDP_IPV4V6:
            strlcpy((char*)temp_buff,"IPV4V6",sizeof(temp_buff));
            break;
          default:
            continue;
        }

        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - MAX_APNE_OUTPUT) );

        res_buff_ptr->used += 
          (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                  res_buff_ptr->size - res_buff_ptr->used,
                                  "$QCAPNE: %d , %d ,\"%s\",\"%s\",%d, %d, %ld\n",
                                   index+1,
                                   profile.apn_class,
                                   profile.context.apn[0]?   profile.context.apn :"",
                                   temp_buff,
                                   profile.apn_bearer,
                                   profile.apn_disable_flag,
                                   profile.inactivity_timer_val);
      }
    }
    DS_SYSTEM_HEAP_MEM_FREE(profile_list);
    if(res_buff_ptr->used == 0)
    {
      res_buff_ptr->used = 
            (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                             res_buff_ptr->size,
                             "$QCAPNE: ");
    }
  }
  else
  {      
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return DSAT_OK;

send_error:
  DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  return DSAT_ERROR;
}/*dsatvend_exec_qcapne_cmd*/
#endif
#ifdef FEATURE_DSAT_ATCOP_VEND

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCJDCFG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCJDCFG command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcjdcfg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type    mode_val;
  dsat_num_item_type    temp;
  
  mode_val = (dsat_num_item_type)dsatutil_get_val(
                       DSAT_VENDOR_QCJDCFG, 0,NUM_TYPE);

  if( tok_ptr->op == (NA|EQ|AR) )
  {
    if( VALID_TOKEN(0) )
    {
      if ( ATOI_OK != dsatutil_atoi (&temp, tok_ptr->arg[0], 10) )
      {
        return DSAT_ERROR;
      } 
      if(temp == mode_val)
      {
        return DSAT_OK;
      }
      else
      {
        result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr);
        if((DSAT_OK == result) && (tok_ptr->op == (NA|EQ|AR)))
        {
          dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_JDCFG, 
                                       DSAT_VENDOR_QCJDCFG, 
                                       0);
        }
      }
    }
    else
    {
      return DSAT_ERROR;
    }
  }
  else if(( tok_ptr->op == (NA|EQ|QU)) || ( tok_ptr->op == (NA|QU)))
  {
    result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr
                                  );
  
  }
  else
  {
    return DSAT_ERROR;
  } 
  return result;
}
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCJDSTATE_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCJDSTATE command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcjdstate_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type         rat_type;
  dsat_result_enum_type      result = DSAT_OK;
  
  dsat_num_item_type         mode_val;    
  dsatcmif_servs_state_info *ph_val = NULL;
  dsat_jdstate_act_e_type    rat = DSAT_JD_ACT_MAX;
  mode_val = (dsat_num_item_type)dsatutil_get_val(
                       DSAT_VENDOR_QCJDSTATE, 0,NUM_TYPE);  
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if(ph_val == NULL)
  { 
     DS_AT_MSG0_ERROR("Ph val is NULL return error");
     return DSAT_ERROR;
  }
  
  if( tok_ptr->op == (NA|EQ|AR) )
  {       
    if(VALID_TOKEN(0) )
    {
       if ( ATOI_OK != dsatutil_atoi (&rat_type, tok_ptr->arg[0], 10) )
       {               
          return DSAT_ERROR;
       }
       if((rat_type != DSAT_COPS_ACT_GSM)&&
          (rat_type != DSAT_COPS_ACT_EUTRAN) &&
          (rat_type != DSAT_COPS_ACT_EUTRAN_NB_S1))
       {
         DS_AT_MSG0_ERROR("QCJDSTATE wrong rat type passed should be 0,7,9");
         return DSAT_ERROR;
       }  
       if(rat_type == DSAT_COPS_ACT_GSM)
         rat = DSAT_JD_ACT_GSM;
       else if(rat_type == DSAT_COPS_ACT_EUTRAN)
         rat = DSAT_JD_ACT_EUTRAN;
       else if(rat_type == DSAT_COPS_ACT_EUTRAN_NB_S1)
         rat = DSAT_JD_ACT_EUTRAN_NB_S1;

       res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                            "$QCJDSTATE:%d,%d\n",
                                             ph_val->jdstate[rat].jamming_status,
                                             ph_val->jdstate[rat].soft_jamming_status
                                         ); 
    }

    else    
    {
      return DSAT_ERROR;
    }
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
     res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                           "$QCJDSTATE:%s\n",
                                            dsat_qcjdstate_mode_valstr
                                     );
  }
  else
  {
    return DSAT_ERROR;
  }
  return result;
}
/*===========================================================================

FUNCTION DSAT_JAMMER_DETECT_CB

DESCRIPTION
  This function is the callback function from NB,GSM teams

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
void dsat_jammer_detect_cb
(
  sys_radio_access_tech_e_type         rat_type,
  uint32                               inc_jamming_status,
  boolean                              inc_soft_jamming_status
)
{
  ds_command_type                     *cmd_buf  = NULL;
  dsat_jammer_detect_info_s_type      *jd_info = NULL;
  
  cmd_buf = dsat_get_cmd_buf(sizeof(dsat_jammer_detect_info_s_type), FALSE);
  if (cmd_buf == NULL)
  {
    return;
  }
  jd_info = cmd_buf->cmd_payload_ptr;

  jd_info->rat_type                = rat_type;
  jd_info->inc_jamming_status      = inc_jamming_status;
  jd_info->inc_soft_jamming_status = inc_soft_jamming_status;
  cmd_buf->hdr.cmd_id              = DS_CMD_ATCOP_JAMMER_DETECT_CMD;	
  ds_put_cmd(cmd_buf);            
}

/*===========================================================================
FUNCTION DSAT_JAMMER_DETECT_CMD_HANDLER

DESCRIPTION
  Handler function for jammer detection state change indication from LTE/GSM

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ASYNC_EVENT : asynchronous event being processed is not associated
                       with the processing of an AT command.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsat_jammer_detect_cmd_handler
(
  ds_command_type        *cmd_ptr      /* DS Command pointer		 */
)
{
  dsat_jammer_detect_info_s_type     *jd_info = NULL;
  dsat_cops_act_e_type                rat = DSAT_COPS_ACT_MAX;  
  dsat_jdstate_act_e_type             jdrat = DSAT_JD_ACT_MAX;
  dsatcmif_servs_state_info          *ph_val = NULL;
  dsat_num_item_type                  current_mode;
  boolean                             flag = FALSE;
  #define MODE_ENABLE 1
  current_mode = (dsat_num_item_type)dsatutil_get_val(
                       DSAT_VENDOR_QCJDCFG, 0,NUM_TYPE);

  jd_info = cmd_ptr->cmd_payload_ptr;
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if((ph_val == NULL) || (jd_info == NULL))
  { 
     DS_AT_MSG0_ERROR("Ph val or JD val is NULL return");
     return DSAT_ASYNC_EVENT;
  }
  if(jd_info->rat_type == SYS_RAT_GSM_RADIO_ACCESS)
  {
    rat = DSAT_COPS_ACT_GSM;
    jdrat = DSAT_JD_ACT_GSM;
  }
  else if(jd_info->rat_type == SYS_RAT_LTE_M1_RADIO_ACCESS)
  {
    rat = DSAT_COPS_ACT_EUTRAN;  
    jdrat = DSAT_JD_ACT_EUTRAN;
  }
  else if(jd_info->rat_type == SYS_RAT_LTE_NB1_RADIO_ACCESS)
  {
   rat      = DSAT_COPS_ACT_EUTRAN_NB_S1;   
   jdrat      = DSAT_JD_ACT_EUTRAN_NB_S1;
  }
  else
  {
    return DSAT_ASYNC_EVENT;
  }
  
  DS_AT_MSG5_HIGH("Ph val cached JDSTATE main %d soft %d recived main %d soft %d current  mode %d",
                   ph_val->jdstate[jdrat].jamming_status,
                   ph_val->jdstate[jdrat].soft_jamming_status,
                   jd_info->inc_jamming_status,
                   jd_info->inc_soft_jamming_status,
                   current_mode);
  
  if((ph_val->jdstate[jdrat].jamming_status != jd_info->inc_jamming_status)||
     (ph_val->jdstate[jdrat].soft_jamming_status != jd_info->inc_soft_jamming_status))
  {
    ph_val->jdstate[jdrat].jamming_status     = jd_info->inc_jamming_status;
    ph_val->jdstate[jdrat].soft_jamming_status = jd_info->inc_soft_jamming_status; 
    flag = TRUE;
  }
  
  if((current_mode == MODE_ENABLE && flag == TRUE))
  {
    dsat_jamming_status_notify(rat,jd_info->inc_jamming_status,
                               jd_info->inc_soft_jamming_status);
  }
  return DSAT_ASYNC_EVENT;
}

/*===========================================================================

FUNCTION DSAT_JAMMING_STATUS_NOTIFY

DESCRIPTION
  This function takes rat_type,jamming status as input and used to notify when 
  there is a jamming status change.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None         

===========================================================================*/
void dsat_jamming_status_notify
(
  dsat_cops_act_e_type  rat_type,
  uint32                inc_jamming_status,
  boolean               inc_soft_jamming_status
)          
{         
  dsm_item_type *res_buff_ptr=NULL;
  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);    
  if (NULL != res_buff_ptr)
  { 
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "$QCJDSTATE:%d,%ld %d\n",
                                    rat_type,
                                    inc_jamming_status,
                                    inc_soft_jamming_status
                                   );
    dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP); 
  }
  else    
    DS_AT_MSG0_HIGH("DSM items are not there");    
}

/*===========================================================================

FUNCTION DSATVEND_GET_IPR_BAUD_RATE

DESCRIPTION
  This function takes incoming baud rate and converts it into SIO baud rate

DEPENDENCIES
  None

RETURN VALUE
  Returns the sio equivalent of the incoming baud rate

SIDE EFFECTS
  None

===========================================================================*/

sio_bitrate_type dsatvend_get_ipr_baud_rate
(
  byte *value
)
{
  if(0 == dsatutil_strcmp_ig_sp_case( (const byte *)value, 
                                           (const byte *)"115200") )
  {
    return SIO_BITRATE_115200;
  }
  else if(0 == dsatutil_strcmp_ig_sp_case( (const byte *)value, 
                                           (const byte *)"1000000"))
  {
    return SIO_BITRATE_1000000;
  }
										   
  return SIO_BITRATE_115200;
}


/*===========================================================================
FUNCTION DSATVEND_PSM_BOOT_IMAGE_HANDLER

DESCRIPTION
  Handler function for PSM boot image indication.

DEPENDENCIES
  None
  
RETURN VALUE
  
DSAT_ERROR : if there was any problem in executing the command.
DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_psm_boot_image_handler
(
  ds_command_type 		*cmd_ptr			   /* DS Command pointer		 */
)
{
  dsat_psm_image_boot_e_type  *boot_image;
  boot_image = cmd_ptr->cmd_payload_ptr;
  if(((*boot_image) == DSAT_PSM_IMAGE_BOOT_NONE)||
    ((*boot_image) == DSAT_PSM_IMAGE_BOOT_MAX))
  {
    DS_AT_MSG1_ERROR("dsatvend_psm_boot_image_handler ERROR %d",(*boot_image));
    return DSAT_ASYNC_EVENT;
  }

  dsat_vend_psm_image_indication(*boot_image);
  return DSAT_ASYNC_EVENT;
}
/*===========================================================================
FUNCTION DSAT_VEND_PSM_IMAGE_INDICATION

DESCRIPTION
  This function notifies the TE about the Image type 
  which could be Full Boot(1),Page Only Boot(2) and SNS_BOOT(sensor 3).

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
void dsat_vend_psm_image_indication
(
  dsat_psm_image_boot_e_type boot_image
)
{
  dsm_item_type *res_buff_ptr=NULL;
  if((boot_image != DSAT_PSM_IMAGE_BOOT_NONE )&&
  	 (boot_image != DSAT_PSM_IMAGE_BOOT_MAX))
  {
    if(((boot_image == DSAT_PSM_IMAGE_BOOT_FULL)&&(full_image != MAX_FOOL_BOOT_INDICATION))
      ||(boot_image == DSAT_PSM_IMAGE_BOOT_PAGE) ||(boot_image == DSAT_PSM_IMAGE_BOOT_SNS))
    { 
      full_image++;

      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);    
      if (NULL != res_buff_ptr)
      { 
        res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                     res_buff_ptr->size,
                                      "QCIMGBOOTTYPE : %d\n",
                                     boot_image
                                    );
        dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP); 
     }
     else    
       DS_AT_MSG0_HIGH("DSM items are not there");    
   }
  }
  else
    DS_AT_MSG1_HIGH("Image Type is not valid %d",boot_image);    
}
/*===========================================================================
FUNCTION DSATVEND_EXEC_QCRPMINFO_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCRPMINFO command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrpminfo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type      result = DSAT_OK;
  if( tok_ptr->op == (NA|QU) )
  {
    ds_3gpp_atcop_rpm_params_type rpm_params;
    rpm_params = ds_3gpp_rpm_get_rpm_parameters();
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCRPMINFO:%d %d %d %d "
                                          "%d %d %d %d %d %d %d %d\n",
                                          rpm_params.ds_rpm_param.is_rpm_enabled,
                                          rpm_params.ds_rpm_param.f_cnt[0],
                                          rpm_params.ds_rpm_param.f_cnt[1],
                                          rpm_params.ds_rpm_param.f_cnt[2],
                                          rpm_params.ds_rpm_param.f_cnt[3],
                                          rpm_params.ds_rpm_param.cpdp_cnt[0],
                                          rpm_params.ds_rpm_param.cpdp_cnt[1],
                                          rpm_params.ds_rpm_param.cpdp_cnt[2],
                                          rpm_params.ds_rpm_param.cpdp_cnt[3],
                                          rpm_params.ds_rpm_param.leak_rate3,
                                          rpm_params.N1,
                                          rpm_params.T1);
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCRPMINFO:(0,1),(0-255),(0-255),"
                                          "(0-255),(0-255),(0-255),(0-255),(0-255),"
                                          "(0-255),(0-255), (0-5),(0-120)\n"
                                        );
  }
  else
  {
    return DSAT_ERROR;
  }
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_PROCESS_CEINFO_CMD

DESCRIPTION
  This function posts the CEINFO information from LTE to DS task and calls 
  the CEINFO handler API in the DS task.
   

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_ceinfo_cmd
(
  ds_command_type 		*cmd_ptr	  
)
{
  dsatcmif_servs_state_info	    *ph_val = NULL;
  dsat_ceinfo_s_type            *ceinfo = NULL;  
  dsat_result_enum_type          result = DSAT_ASYNC_EVENT;
  
  ceinfo = cmd_ptr->cmd_payload_ptr;
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if((ph_val == NULL)||(ceinfo == NULL))
  { 
    DS_AT_MSG0_ERROR("Ph val or ceinfo is NULL return error");
    return DSAT_ASYNC_EVENT;
  }
  else
  {  
    result = dsatvend_process_ceinfo_data(ceinfo,NULL,TRUE);
  }  
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_PROCESS_CEINFO_DATA

DESCRIPTION
 This function is the handler function of the data recieved from LTE
 and runs in DS task After processing the CEINFO data result will be returned
 to the host/MCU.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
   Possible values:
   DSAT_ERROR : if there was any problem in executing the command
   DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_ceinfo_data
(
  dsat_ceinfo_s_type  *ceinfo,
  dsm_item_type       *res_buff,
  boolean              async_command
)
{
  dsatcmif_servs_state_info	   *ph_val = NULL;    
  uint8                         ce_enabled =0;
  dsm_item_type                *res_buff_ptr = NULL;
  dsat_result_enum_type         result = DSAT_ASYNC_EVENT;

  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  if((ph_val == NULL)||(ceinfo == NULL))
  { 
    DS_AT_MSG0_ERROR("Ph val or ceinfo is NULL return error");
    SET_PENDING(DSAT_VENDOR_QCCEINFO_IDX, 0, DSAT_PENDING_FALSE);
    return DSAT_ASYNC_EVENT;
  }
    
  if(ceinfo->active_rat == SYS_SYS_MODE_LTE_M1)
  {
    if(ceinfo->enh_cov_mode_enabled == TRUE)
    {
      ce_enabled = 1;
    }
    else
    {    
      DS_AT_MSG0_ERROR("enh_cov_mode_enabled is not enabled return error");
      return DSAT_ASYNC_EVENT;
    }
  }
  if(ceinfo->state == DSAT_CE_INFO_UE_STATE_MAX)
  {  
    DS_AT_MSG0_ERROR("ceinfo->state is maximum return error");  
    return DSAT_ASYNC_EVENT;
  }

   //copy the data in to the ph_val
  ph_val->ceinfo.active_rat           = ceinfo->active_rat;
  ph_val->ceinfo.dl_num_rep           = ceinfo->dl_num_rep;
  ph_val->ceinfo.rsrp                 = ceinfo->rsrp;
  ph_val->ceinfo.enh_cov_mode         = ceinfo->enh_cov_mode;
  ph_val->ceinfo.enh_cov_mode_enabled = ceinfo->enh_cov_mode_enabled;
  ph_val->ceinfo.state                = ceinfo->state;
  ph_val->ceinfo.cinr                 = ceinfo->cinr;
  ph_val->ceinfo.ul_num_rep           = ceinfo->ul_num_rep;
  ph_val->ceinfo.ceinfo_valid         = TRUE;

  DS_AT_MSG7_HIGH("ceinfo info ceinfo.dl_num_rep %d,"
                    "ceinfo.rsrp %d ,ceinfo.enh_cov_mode %d,"
                    "ceinfo.enh_cov_mode_enabled %d,"
                    "ceinfo.state %d,"
                    "ceinfo.cinr %d,"
                    "ceinfo.ul_num_rep %d",
                    ceinfo->dl_num_rep,
                    ceinfo->rsrp,
                    ceinfo->enh_cov_mode,
                    ceinfo->enh_cov_mode_enabled,
                    ceinfo->state,
                    ceinfo->cinr,
                    ceinfo->ul_num_rep
                    );  
  if(res_buff == NULL)
  { 
     res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE); 
  }
  else
  {
    res_buff_ptr = res_buff;
  }

  if((res_buff_ptr != NULL))
  {
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                               "QCCEINFO:%d,%d,%d,%d,%d,%ld,%d", 
                                               (dsat_num_item_type)dsatutil_get_val(
                                                DSAT_VENDOR_QCCEINFO_MODE,0,NUM_TYPE),
                                                ce_enabled,
                                                ceinfo->state,
                                                ceinfo->dl_num_rep,
                                                ceinfo->ul_num_rep,
                                                ceinfo->rsrp,
                                                ceinfo->cinr
                                               );
    if(async_command == TRUE)
    {
      dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);	 
    }
    else
    {
      result = DSAT_OK;
    }
  }
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCEINFO_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCCEINFO command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcceinfo_cmd
(
  dsat_mode_enum_type        mode,             /*  AT command mode:            */
  const dsati_cmd_type      *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type  *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type             *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type                   mode_value;
  dsat_result_enum_type                result = DSAT_OK;
  dsat_num_item_type                   mode_val;    
  dsatcmif_servs_state_info           *ph_val = NULL;
  lte_cphy_ce_info_rpt_req_s           ceinfo_req;  
  sys_radio_access_tech_e_type         active_rat = SYS_RAT_NONE;
  dsat_err_number_e_type               err_info = DSAT_ERR_ERROR_MAX;

  active_rat = ds_pdn_mgr_get_active_rat();
  
  mode_val = (dsat_num_item_type)dsatutil_get_val(
                       DSAT_VENDOR_QCCEINFO_MODE, 0,NUM_TYPE); 
  memset(&ceinfo_req,0,sizeof(lte_cphy_ce_info_rpt_req_s));
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);  
  if(ph_val == NULL)
  { 
     err_info = DSAT_ERR_ERROR_1;
     goto bail;
  }
  if((active_rat != SYS_RAT_LTE_NB1_RADIO_ACCESS)&&
  	  (active_rat != SYS_RAT_LTE_M1_RADIO_ACCESS))
  {
    err_info = DSAT_ERR_ERROR_2;
    goto bail;
  }
  
  if( tok_ptr->op == (NA|EQ|AR) )
  { 
    if(VALID_TOKEN(0) )
    {    
       if ( ATOI_OK != dsatutil_atoi (&mode_value, tok_ptr->arg[0], 10) )
       {              
          err_info = DSAT_ERR_ERROR_3;
          goto bail;
       }
       if((mode_value != CEINFO_MODE_ENABLE)&&
          (mode_value != CEINFO_MODE_DISABLE) )
       {
         err_info = DSAT_ERR_ERROR_4;
         goto bail;
       }  
    }
    else    
    {
      err_info = DSAT_ERR_ERROR_5;
      goto bail;
    }
    if(mode_value == mode_val)
    {	
       DS_AT_MSG0_ERROR("Ph val is NULL return error");
       return result;
    }
    DSATUTIL_SET_VAL(DSAT_VENDOR_QCCEINFO_MODE,0,0,(dsat_num_item_type)mode_value,NUM_TYPE);
    //Call the ML1 UIMD req
    ceinfo_req.one_time_query = FALSE;
    ceinfo_req.enable_reporting   = mode_value;
    if (!ds_msgr_snd_msg_send(
         LTE_CPHY_CE_INFO_REPORT_REQ,
         (msgr_hdr_struct_type *)&ceinfo_req,
         sizeof(lte_cphy_ce_info_rpt_req_s)))
    {
      err_info = DSAT_ERR_ERROR_6;
      SET_PENDING(DSAT_VENDOR_QCCEINFO_IDX, 0, DSAT_PENDING_FALSE); 
      goto bail;	  
    }
    else
    {      
       SET_PENDING(DSAT_VENDOR_QCCEINFO_IDX, 0, DSAT_PENDING_TRUE);
    }  
    if(mode_value == CEINFO_MODE_DISABLE)
    {	  
      result = DSAT_OK;	
      SET_PENDING(DSAT_VENDOR_QCCEINFO_IDX, 0, DSAT_PENDING_FALSE);
      //Clear the cache too here
       ph_val->ceinfo.ceinfo_valid = FALSE;		
       memset(&(ph_val->ceinfo),0,sizeof(dsat_ceinfo_s_type));
     }
     return result;
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
     res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                           "$QCCEINFO:%s\n",
                                            dsat_qcceinfo_mode_valstr
                                           );	 	 
     return result;
  }
  else if(tok_ptr->op == (NA|QU))
  {
    if(CHECK_PENDING(DSAT_VENDOR_QCCEINFO_IDX,0,DSAT_PENDING_TRUE))
    {    
      err_info = DSAT_ERR_ERROR_7;
      goto bail;	
    }
    //First time query without the registartion of the URC 
    if(ph_val->ceinfo.ceinfo_valid == FALSE)
    {    
       ceinfo_req.one_time_query     = TRUE;
       ceinfo_req.enable_reporting   = FALSE;
       if (!ds_msgr_snd_msg_send(
            LTE_CPHY_CE_INFO_REPORT_REQ,
           (msgr_hdr_struct_type *)&ceinfo_req,
           sizeof(lte_cphy_ce_info_rpt_req_s)))
       {      
          err_info = DSAT_ERR_ERROR_7;
          SET_PENDING(DSAT_VENDOR_QCCEINFO_IDX, 0, DSAT_PENDING_FALSE);	
          goto bail;  
      }
      else
      {      
         SET_PENDING(DSAT_VENDOR_QCCEINFO_IDX, 0, DSAT_PENDING_TRUE);
      }      
      return result;
    }
    //Query when URC was enabled we give the cached data
    if(ph_val->ceinfo.ceinfo_valid == TRUE)
    {	
      //print the values from cache data      
      result = dsatvend_process_ceinfo_data(&ph_val->ceinfo,res_buff_ptr,FALSE);
    }
    return result;
  }

  bail:
  DS_AT_MSG1_ERROR("return error %d",err_info);
  return DSAT_ERROR;
}

/*===========================================================================
FUNCTION DSATVEND_JD_SERVICE_STATUS_IND_HANDLER

DESCRIPTION
  Handler function for DS service indication from LTE

DEPENDENCIES
  None
  
RETURN VALUE
  
DSAT_ERROR : if there was any problem in executing the command.
DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None
  
============================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_jd_service_status_ind_handler
(
  ds_command_type        *cmd_ptr      /* DS Command pointer		 */
)
{
  dsat_jd_service_status_ind_s_type     *ds_service_ind = NULL;
  dsatcmif_servs_state_info             *ph_val = NULL;

  ds_service_ind = cmd_ptr->cmd_payload_ptr;  
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
 
  if((ph_val == NULL)||
     (ds_service_ind == NULL))
  {
    return DSAT_ASYNC_EVENT;
  }
  
  if(ds_service_ind->svc_status == DS_JAM_DET_SVC_STATUS_MAX)
  {
     DS_AT_MSG1_ERROR("dsatvend_ds_service_ind_handler "
	 	              "ds_service_ind->svc_status %d",
                       ds_service_ind->svc_status);
     return DSAT_ASYNC_EVENT;
  }
  if(ds_service_ind->svc_status == DS_JAM_DET_SVC_STATUS_NOT_AVAILABLE)
  {
    //Clear all the jamming state for all the rat
    memset(&ph_val->jdstate[0],0,sizeof(dsat_jd_state_s_type)*DSAT_JD_ACT_MAX);
  }
  dsat_vend_ds_service_indication(ds_service_ind);
  return DSAT_ASYNC_EVENT;
}

/*===========================================================================
FUNCTION DSAT_VEND_DS_SERVICE_INDICATION

DESCRIPTION
  This function notifies the Host about the rat and service status 
  which could be not available (0) and available(1).

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None
===========================================================================*/
void dsat_vend_ds_service_indication
(
  dsat_jd_service_status_ind_s_type *ds_service_ind
)
{
  dsm_item_type *res_buff_ptr = NULL;
  uint8          rattype = 0;

  if(ds_service_ind->svc_status != DS_JAM_DET_SVC_STATUS_MAX )
  {
    if(ds_service_ind->act_rat_type == DS_SOFT_JAM_RAT_TYPE_M1)
    {
      rattype = DSAT_COPS_ACT_EUTRAN;
    }
    else if(ds_service_ind->act_rat_type == DS_SOFT_JAM_RAT_TYPE_NB1)
    {
      rattype = DSAT_COPS_ACT_EUTRAN_NB_S1;
    }

    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE); 
    if (NULL != res_buff_ptr)
    { 
      res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "QCSRVCINFO : %d ,%d\n",
                                   rattype,
                                   ds_service_ind->svc_status
                                  );
      dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP); 
   }
   else    
     DS_AT_MSG0_ERROR("DSM items are not there");    
  }
  else
    DS_AT_MSG1_ERROR("service status is not valid %d",ds_service_ind->svc_status);
}

/*===========================================================================

FUNCTION DSATVEND_PROCESS_SFN_DATA

DESCRIPTION
 This function is the handler function of the data received from LTE
 and runs in DS task After processing the SFN data result will be returned
 to the host/MCU.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
   Possible values:
   DSAT_ERROR : if there was any problem in executing the command
   DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_sfn_data
(
  dsat_sfn_s_type         *sfn,
  dsm_item_type           *res_buff,
  boolean                 async_command,
  ds_command_enum_type    cmd_id
)
{
  dsm_item_type                *res_buff_ptr = NULL;
  dsat_result_enum_type         result = DSAT_OK;

  if(sfn == NULL)
  { 
    DS_AT_MSG0_ERROR("Ph val or sfn is NULL return error");
    return DSAT_ERROR;
  }

  if(res_buff == NULL)
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE); 
  }

  if(res_buff_ptr != NULL)
  {
    if (cmd_id == DS_CMD_ATCOP_GET_SFN_CMD)
    {
      if((CHECK_PENDING(DSAT_VENDOR_QCSFNGET_IDX, 0, DSAT_PENDING_TRUE)))
      {
        res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "QCGETSFN:%lu", 
                                              sfn->hsfn_sfn_nbr );
        SET_PENDING(DSAT_VENDOR_QCSFNGET_IDX, 0, DSAT_PENDING_FALSE);
      }
      else if((CHECK_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX, 0, DSAT_PENDING_TRUE)))
      {
         res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "QCSFNRANGE:%lu - %lu", 
                                              sfn->min_hsfn_sfn_nbr,
                                              sfn->max_hsfn_sfn_nbr);
        SET_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX, 0, DSAT_PENDING_FALSE);
      }
    }
    else if (cmd_id == DS_CMD_ATCOP_SET_SFN_RESP_CMD)
    {
       res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                           res_buff_ptr->size,
                                           "QCSETSFN:%d", 
                                            sfn->req_accepted);
    }
    else if(cmd_id == DS_CMD_ATCOP_SFN_REACHED_IND_CMD)
    {
      ULOG_RT_PRINTF_1(ds_fwk_get_ulog_handle(), "SFN Match %d", sfn->hsfn_sfn_nbr);
      res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                           res_buff_ptr->size,
                                           "QCSFNMATCH:%lu", 
                                            sfn->hsfn_sfn_nbr);
    }

    if(async_command == TRUE)
    {
      dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);	 
    }
  }
  return result;
}


/*===========================================================================

FUNCTION DSATVEND_PROCESS_SFN_CMD

DESCRIPTION
  This function posts the SFN information from LTE to DS task and calls 
  the SFN handler API in the DS task.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_sfn_cmd
(
  ds_command_type          *cmd_ptr
)
{
  dsat_sfn_s_type               *sfn_info = NULL;  
  dsat_result_enum_type          result = DSAT_OK;
  ds_command_enum_type           cmd_id;

  sfn_info = cmd_ptr->cmd_payload_ptr;
  cmd_id = cmd_ptr->hdr.cmd_id;

  if(sfn_info == NULL)
  { 
    DS_AT_MSG0_ERROR("sfn_info is NULL return error");
    if (cmd_id == DS_CMD_ATCOP_GET_SFN_CMD)
    {
      if((CHECK_PENDING(DSAT_VENDOR_QCSFNGET_IDX, 0, DSAT_PENDING_TRUE)))
      {
        SET_PENDING(DSAT_VENDOR_QCSFNGET_IDX, 0, DSAT_PENDING_FALSE);
      }
      else if ((CHECK_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX, 0, DSAT_PENDING_TRUE)))
      {
        SET_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX, 0, DSAT_PENDING_FALSE);
      }
    }
    return DSAT_ERROR;
  }
  else
  {  
    result = dsatvend_process_sfn_data(sfn_info,NULL,TRUE, cmd_id);
  }  
  return result;
}


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSFN_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCSFN command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsfn_cmd
(
  dsat_mode_enum_type        mode,             /*  AT command mode:            */
  const dsati_cmd_type      *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type  *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type             *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type                   sfn_value;
  lte_cphy_get_next_sfn_req_s          get_sfn_req;
  lte_cphy_set_sfn_config_req_s        set_sfn_req;
  sys_radio_access_tech_e_type         active_rat = SYS_RAT_NONE;
  dsat_err_number_e_type               err_info = DSAT_ERR_ERROR_MAX;
  ds_command_type                      *cmd_buf = NULL;
  ds_command_type                      *cmd_buf_ptr = NULL;
  dsat_sfn_s_type                      *sfn = NULL;
  dsat_sfn_s_type                      *sfn_ptr = NULL;

  if (dsatcmdp_get_current_mode() == DSAT_MODE_GSM)
  {
    err_info = DSAT_ERR_ERROR_1;
    goto bail;
  }

  active_rat = ds_pdn_mgr_get_active_rat();

  if((active_rat != SYS_RAT_LTE_NB1_RADIO_ACCESS)&&
     (active_rat != SYS_RAT_LTE_M1_RADIO_ACCESS))
  {
    cmd_buf = dsat_get_cmd_buf(sizeof(dsat_sfn_s_type), FALSE);
    if (cmd_buf == NULL)
    {
      err_info = DSAT_ERR_ERROR_2;
      goto bail;
    }

    sfn = cmd_buf->cmd_payload_ptr;
    if( tok_ptr->op == (NA|EQ|AR) )
    {
      //First post the SET_SFN success cmd and then post the SFN reach command
      cmd_buf_ptr = dsat_get_cmd_buf(sizeof(dsat_sfn_s_type), FALSE);
      if (cmd_buf_ptr == NULL)
      {
        err_info = DSAT_ERR_ERROR_3;
        goto bail;
      }

      sfn_ptr = cmd_buf_ptr->cmd_payload_ptr;
      sfn_ptr->req_accepted = TRUE;
      cmd_buf_ptr->hdr.cmd_id = DS_CMD_ATCOP_SET_SFN_RESP_CMD;
      ds_put_cmd(cmd_buf_ptr);


      sfn->hsfn_sfn_nbr = 0xFFFFFFFF;
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_SFN_REACHED_IND_CMD;	
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      sfn->min_hsfn_sfn_nbr = 0xFFFFFFFF;
      sfn->max_hsfn_sfn_nbr = 0xFFFFFFFF;
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_GET_SFN_CMD;
      SET_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX, 0, DSAT_PENDING_TRUE);
    }
    else if (tok_ptr->op == (NA|QU))
    {
      sfn->hsfn_sfn_nbr = 0xFFFFFFFF;
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_GET_SFN_CMD;
      SET_PENDING(DSAT_VENDOR_QCSFNGET_IDX, 0, DSAT_PENDING_TRUE);
    }
    ds_put_cmd(cmd_buf);
    return DSAT_ASYNC_CMD;
  }


  if( tok_ptr->op == (NA|EQ|AR) )
  {
    if(VALID_TOKEN(0) )
    {    
       if ( ATOI_OK != dsatutil_atoi (&sfn_value, tok_ptr->arg[0], 10) )
       {              
          err_info = DSAT_ERR_ERROR_4;
          goto bail;
       }
    }
    else    
    {
      err_info = DSAT_ERR_ERROR_5;
      goto bail;
    }

    set_sfn_req.hsfn_sfn_nbr = sfn_value;

    if (!ds_msgr_snd_msg_send(
         LTE_CPHY_SET_SFN_CONFIG_REQ,
         (msgr_hdr_struct_type *)&set_sfn_req,
         sizeof(lte_cphy_set_sfn_config_req_s)))
    {
      err_info = DSAT_ERR_ERROR_6;
      goto bail;
    }

    return DSAT_ASYNC_CMD;
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    if(CHECK_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX,0,DSAT_PENDING_TRUE))
    {    
      err_info = DSAT_ERR_ERROR_7;
      goto bail;	
    }
    if (!ds_msgr_snd_msg_send(
         LTE_CPHY_GET_NEXT_SFN_REQ,
        (msgr_hdr_struct_type *)&get_sfn_req,
         sizeof(lte_cphy_get_next_sfn_req_s)))
    {
      err_info = DSAT_ERR_ERROR_8;
      goto bail;  
    }
    else
    {
      SET_PENDING(DSAT_VENDOR_QCSFNRANGE_IDX, 0, DSAT_PENDING_TRUE);
    }	 
    return DSAT_ASYNC_CMD;
  }
  else if(tok_ptr->op == (NA|QU))
  {
    if(CHECK_PENDING(DSAT_VENDOR_QCSFNGET_IDX,0,DSAT_PENDING_TRUE))
    {    
      err_info = DSAT_ERR_ERROR_9;
      goto bail;	
    }
    if (!ds_msgr_snd_msg_send(
         LTE_CPHY_GET_NEXT_SFN_REQ,
        (msgr_hdr_struct_type *)&get_sfn_req,
         sizeof(lte_cphy_get_next_sfn_req_s)))
    {
      err_info = DSAT_ERR_ERROR_10;
      goto bail;  
    }
    else
    {
      SET_PENDING(DSAT_VENDOR_QCSFNGET_IDX, 0, DSAT_PENDING_TRUE);
    }
    return DSAT_ASYNC_CMD;
  }

  bail:
    DS_AT_MSG1_ERROR("return error %d",err_info);
    return DSAT_ERROR;
}

/*===========================================================================
FUNCTION DSATVEND_PROCESS_RRINFO_DATA

DESCRIPTION
  This function is the helper function to process rat info. It maps SYS
  enum type to DS enum type.

DEPENDENCIES
  None

RETURN VALUE
  Returns out_rat which holds values of DS RR enum type to further process.

SIDE EFFECTS
  None

===========================================================================*/
dsat_rr_cur_rat_e_type dsatvend_process_rrinfo_data
(
  sys_sys_mode_e_type in_rat
)
{
  dsat_rr_cur_rat_e_type out_rat = DSAT_RR_ACT_NONE;
  switch (in_rat)
  {
    case SYS_SYS_MODE_LTE_NB1:
      out_rat = DSAT_RR_ACT_LTE_NB1;
      break;

    case SYS_SYS_MODE_LTE_M1:
      out_rat = DSAT_RR_ACT_LTE_M1;
      break;

    case SYS_SYS_MODE_LTE:
      out_rat = DSAT_RR_ACT_LTE;
      break;

    case SYS_SYS_MODE_GSM:
      out_rat = DSAT_RR_ACT_GSM;
      break;

#ifdef FEATURE_NBIOT_NTN
    case SYS_SYS_MODE_LTE_NB1_NTN:
      out_rat = DSAT_RR_ACT_LTE_NB1_NTN;
      break;
#endif /* FEATURE_NBIOT_NTN */

    default:
      out_rat = DSAT_RR_ACT_NONE;
      break;
  }
  return out_rat;
}
/*===========================================================================
FUNCTION DSATVEND_EXEC_QCRRINFO_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCRRINFO command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrrinfo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_rr_cur_rat_e_type         current_rat = DSAT_RR_ACT_NONE;
  dsat_rr_cur_rat_e_type         scanning_rat = DSAT_RR_ACT_NONE;
  dsat_rr_roam_status_e_type     roaming_state = DSAT_RR_ROAM_STATUS_NONE;
  sys_scan_params_s_type         scan_params;
  
  if( tok_ptr->op == (NA|QU) || tok_ptr->op == NA)
  {
    nas_command_get_scan_params(&scan_params);
    DS_AT_MSG3_HIGH("rr info current_rat %d, roaming state %d, scanning_rat %d",
                     scan_params.current_camped_rat,
                     scan_params.roam_state,
                     scan_params.scanning_rat
                     );

    current_rat = dsatvend_process_rrinfo_data(scan_params.current_camped_rat);
    scanning_rat = dsatvend_process_rrinfo_data(scan_params.scanning_rat);

    switch (scan_params.roam_state)
    {
      case SYS_ROAM_STATUS_OFF:
        roaming_state = DSAT_RR_ROAM_STATUS_OFF;
        break;

      case SYS_ROAM_STATUS_ON:
        roaming_state = DSAT_RR_ROAM_STATUS_ON;
        break;

      default:
        roaming_state = DSAT_RR_ROAM_STATUS_NONE;
        break;
    }

    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCRRINFO: %d,%d,%d",
                                          current_rat,
                                          roaming_state,
                                          scanning_rat
                                          );
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCRRINFO: (0-15),(%d-%d),(%d-%d)",
                                          DSAT_RR_ROAM_STATUS_NONE,
                                          DSAT_RR_ROAM_STATUS_MAX - 1,
                                          DSAT_RR_ACT_NONE,
                                          DSAT_RR_ACT_MAX - 1
                                          );
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCMSISDN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to get APN MSISDN info

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcmsisdn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define NUM_ARGS_SUPPORTED 1
  dsat_result_enum_type                 result = DSAT_OK;
  int16                                 dss_errno = DS_ENOERR;
  int16                                 return_val = DSS_SUCCESS;
  ds_sys_ioctl_msisdn_info_type         apn_msisdn_info;
  boolean                               valid_status = FALSE;
  byte                                  apn_buff[DS_SYS_MAX_APN_LEN+1];
  dsat_error_info_s_type                err_info;
  byte                                  msisdn_value[255];
  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
  
  if(tok_ptr->op == (NA|QU) || tok_ptr->op == (NA) || tok_ptr->op == (NA|EQ|QU))
  {
    return result;
  }
  else if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found == NUM_ARGS_SUPPORTED)
    {
      VALID_STRING_TOKEN(TOKEN_IDX_0, tok_ptr->arg[0], apn_buff,sizeof(apn_buff))
      if (!valid_status)
      {
        err_info.errval = DSAT_ERR_INVALID_TOKENS;
        goto send_error;
      }
      memset(&apn_msisdn_info, 0, sizeof(ds_sys_ioctl_msisdn_info_type));
      (void)strlcpy((char *)apn_msisdn_info.apn_name,(const char *)apn_buff,DS_SYS_MAX_APN_LEN + 1);
      /*-------------------------------------------------------------------------
      Retrieve pco information by calling into DS Sys Layer
      -------------------------------------------------------------------------*/ 
      return_val = ds_sys_ioctl(DS_SYS_TECH_3GPP, DS_SYS_IOCTL_3GPP_GET_APN_MSISDN_INFO, &apn_msisdn_info, &dss_errno); 
      if (return_val != DSS_SUCCESS)
      {
        DS_AT_MSG2_ERROR("$qcmsisdn ds_sys_ioctl ret [%d] errno [%d]", return_val, dss_errno);
        return DSAT_ERROR;
      }
      if( DSAT_OK == dsatvend_decode_msisdn(&apn_msisdn_info.msisdn_info,msisdn_value))
      {
        res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                   res_buff_ptr->size,
                                                   "$QCMSISDN: %s",
                                                   (char *)msisdn_value);
      }
      else
      {
        err_info.errval = DSAT_ERR_INVALID_FORMAT;
        goto send_error;
      }
    }
    else
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }   
  }	
  else
  {
    err_info.errval = DSAT_ERR_INVALID_OPERATION;
    goto send_error;
  }
  return result;

send_error:
  DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  return DSAT_ERROR;
}

/*===========================================================================

FUNCTION DSATVEND_DECODE_MSISDN

DESCRIPTION
  This function taken array of MSISDN Info and convert it into string format.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the function.
  possible values:
    DSAT_ERROR : if there was any problem in converting to string.
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/

dsat_result_enum_type dsatvend_decode_msisdn
(
  ds_sys_msisdn_info_type * msisdn_ptr,    /* Structure of msisdn data */
  byte * msisdn_buf                        /* Output string buffer */
)
{
  uint8 length = 0;
  byte * s_ptr = msisdn_buf;
  if(0 == msisdn_ptr->msisdn_length)
  {
    DS_AT_MSG0_ERROR("msisdn length is 0");
    return DSAT_ERROR;
  }
  for(length = 0; length< msisdn_ptr->msisdn_length; length++)
  {
    *s_ptr++ = msisdn_ptr->msisdn[length] + '0' ;
  }
  *s_ptr ='\0';
  return DSAT_OK;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCELLSELECT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to get parameters for 
  RSRP values for Cell Selection.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/

dsat_result_enum_type dsatvend_exec_qccellselect_cmd
(
  dsat_mode_enum_type          mode,           /*  AT command mode:            */
  const dsati_cmd_type        *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type    *tok_ptr,        /*  Command tokens from parser  */
  dsm_item_type               *res_buff_ptr    /*  Place to put response       */
)
{
  dsat_result_enum_type               result = DSAT_OK;

  lte_rrc_cell_select_info_req_s      lte_rrc_cell_select_req;
  memset( &lte_rrc_cell_select_req, 0x00, sizeof(lte_rrc_cell_select_info_req_s) );
  
  #if defined(FEATURE_GSM)
  geran_grr_cell_select_info_req_t    grr_cell_select_req;
  memset( &grr_cell_select_req, 0x00, sizeof(geran_grr_cell_select_info_req_t) );
  #endif
  
  if (tok_ptr->op == (NA) || tok_ptr->op == (NA|QU) )
  {
    
    if (dsatcmdp_get_current_mode() == DSAT_MODE_LTE)
    { 
      if (!ds_msgr_snd_msg_send(
               LTE_RRC_CELL_SELECT_INFO_REQ,
              (msgr_hdr_struct_type *)&lte_rrc_cell_select_req,
               sizeof(lte_rrc_cell_select_info_req_s)))
      {
        result = DSAT_ERROR; 
      }
      else
      {
        SET_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_TRUE);
        result = DSAT_OK;
      }
    }
    #if defined(FEATURE_GSM)
    else if (dsatcmdp_get_current_mode() == DSAT_MODE_GSM)
    { 
      if (!ds_msgr_snd_msg_send(
               GERAN_GRR_CELL_SELECT_INFO_REQ,
              (msgr_hdr_struct_type *)&grr_cell_select_req,
              sizeof(geran_grr_cell_select_info_req_t)))
      {
        result = DSAT_ERROR; 
      }
      else
      {
        SET_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_TRUE);
        result = DSAT_OK;
      }
    }
    #endif
    else
    {
      DS_AT_MSG0_ERROR("Device Not Camped");
      return DSAT_ERROR;
    }
  }
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_PROCESS_CELL_SELECT_CMD

DESCRIPTION
  This function posts the CELL Selection parameters information from LTE/GERAN
  to DS task and calls the Cell Select handler API in the DS task.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_cell_select_cmd
(
  ds_command_type          *cmd_ptr
)
{
  dsat_cell_select_info_s  *cell_select = NULL;
  dsat_result_enum_type     result = DSAT_OK;
  ds_command_enum_type      cmd_id;
  dsm_item_type            *res_buff_ptr = NULL;
       

  cell_select = cmd_ptr->cmd_payload_ptr;
  cmd_id = cmd_ptr->hdr.cmd_id;

  if(cell_select == NULL)
  { 
    if (cmd_id == DS_CMD_ATCOP_CELL_SELECT_INFO_CMD) 
    {
      if((CHECK_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_TRUE)))
      {
        SET_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_FALSE);
      }
    }
    return DSAT_ERROR;
  }
  else
  {
   res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

   if(res_buff_ptr == NULL)
   {
      ULOG_RT_PRINTF_0(ds_fwk_get_ulog_handle(),"Small DSM items exhausted.");
      return DSAT_ERROR;
   }
   if(cell_select->is_ue_camped == FALSE)
   {
     if((CHECK_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_TRUE)))
      {
        res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "QCCELLSELECT:%d", 
                                              cell_select->is_ue_camped);
        SET_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_FALSE);
      }
   }
   else if (cell_select->rat == DSAT_COPS_ACT_GSM)
   {
      if((CHECK_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_TRUE)))
      {
        res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "QCCELLSELECT:%d,%d,,,,%d", 
                                              cell_select->is_ue_camped,
                                              cell_select->rat,
                                              cell_select->rxlev_access_min);
        SET_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_FALSE);
      }
   }
   else if (cell_select->rat == DSAT_COPS_ACT_EUTRAN || 
   	        cell_select->rat == DSAT_COPS_ACT_EUTRAN_NB_S1)
   {
      if((CHECK_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_TRUE)))
      {
        res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "QCCELLSELECT:%d,%d,%ld,%ld,%ld,", 
                                              cell_select->is_ue_camped,
                                              cell_select->rat,
                                              cell_select->qrxlevmin,
                                              cell_select->qrxlevmin_ce,
                                              cell_select->qrxlevmin_ce1);
        SET_PENDING(DSAT_VENDOR_QCCELLSELECT_IDX, 0, DSAT_PENDING_FALSE);
      }
   }
     dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
  }
  return result;
}

#ifdef FEATURE_FSK_TX
/*===========================================================================
FUNCTION DSATVEND_EXEC_QCFSKDBG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCFSKDBG command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcfskdbg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  geran_fsk_debug_query_req_type            fsk_dbg_query_req;
  dsat_result_enum_type                     result = DSAT_OK;
  dsatcmif_servs_state_info  *ph_val = NULL;
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
   Processing for TEST command 
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
    if ( DSAT_OK == result && 
        (tok_ptr->op == (NA|EQ|AR)))
    {
      /* Invoke Call Manager API requesting functionality change */
      result = dsatcmif_change_operating_mode (SYS_OPRT_MODE_LPM);
      SET_PENDING(DSAT_VENDOR_QCFSKDBG_IDX, 0, DSAT_PENDING_TRUE);
      if(result == DSAT_ASYNC_CMD)
      {
        //Do nothing
      }
      else if (DSAT_OK == result && ph_val->is_gw_subscription_available)
      {
        DS_AT_MSG0_HIGH(" FSK debug - sim_not_avaialble command invoked");     
        (void) cm_ph_cmd_subscription_not_available3 ( dsatcmif_ph_cmd_cb_func,
                                            NULL,
                                            dsatcm_client_id,
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                            CM_SUBSCRIPTION_STATUS_CHANGE,
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                            CM_SUBSCRIPTION_STATUS_NO_CHANGE);
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = dsatvend_send_fsk_start();
      }
    }
  }
  else if( tok_ptr->op == (NA|QU))
  {
    memset(&fsk_dbg_query_req,0,sizeof(geran_fsk_debug_query_req_type));
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_DEBUG_QUERY_REQ,
         (msgr_hdr_struct_type *)&fsk_dbg_query_req,
         sizeof(geran_fsk_debug_query_req_type)))
    {
      result = DSAT_ERROR;
    }
    else
    {
      SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
      result = DSAT_ASYNC_CMD;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCFSKDATA_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCFSKDATA command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcfskdata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type                     result = DSAT_OK;
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  
  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
   Processing for TEST command 
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
    if ( DSAT_OK == result && 
        (tok_ptr->op == (NA|EQ|AR)))
    {
      /* Send mesasge to SIO data preprocessor to change state to */
      /* handle SMS message entry */
      dsatprep_set_preprocessor_mode ( DSATI_FSK_DATA_ENTRY_MODE, port );
      result = DSAT_ASYNC_CMD;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCFSKHOPTABLE_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCFSKHOPTABLE command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcfskhoptable_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* <freq> - Frequency to use in the transmission, expressed in Hz 
        Range: 902000000-928000000, 966037000-967957000, 862000000-870000000 */
  #define FSK_FREQ_MIN1         902000000
  #define FSK_FREQ_MAX1         928000000
  #define FSK_FREQ_MIN2         966037000
  #define FSK_FREQ_MAX2         967957000
  #define FSK_FREQ_MIN3         862000000
  #define FSK_FREQ_MAX3         870000000

  geran_fsk_hop_table_req_type            fsk_hop_table_req;
  geran_fsk_hop_table_query_req_type      fsk_hop_table_query_req;
  dsat_num_item_type          index; /*possible values 0,1,2*/
  dsat_num_item_type          delta = 2; /*possible values: 1,2,3,5 default: 2 */
  uint8          num_freq = 0; /*number of valid frequencies */
  dsat_num_item_type         freq[50] = {0};   /*Range: 902000000-928000000, 
                                                        966037000-967957000,
                                                        862000000-870000000 */
  dsat_error_info_s_type   err_info;
  boolean              valid_status           = FALSE;

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
  
  if(tok_ptr->op == (NA|EQ|AR))
  {
    memset(&fsk_hop_table_req,0,sizeof(geran_fsk_hop_table_req_type));
    /* Check the number of arguments. */
    if ( tok_ptr->args_found > parse_table->compound )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
  
    VALID_NUM_TOKEN(TOKEN_IDX_0,0,2, &index)
    if (!valid_status)
    {
      err_info.arg_num = TOKEN_IDX_0;
      goto send_error;
    }

    VALID_NUM_TOKEN(TOKEN_IDX_1,1,5, &delta)
    /* Range validation will be taken care by above API. 
    Have a check for value 4 since only values 1,2,3 and 5 are supported*/
    if (!(valid_status && delta != 4))
    {
      err_info.arg_num = TOKEN_IDX_1;
      goto send_error;
    }

    /* Two tokens are already validated above, that's why i+2 */
    for(uint8 i = 0; i < 50; i++)
    {
      if( VALID_TOKEN(i+2) )
      {
        VALID_NUM_TOKEN(i+2,0,MAX_VAL_NUM_ITEM, &freq[i])
        if (!(valid_status && 
            ((freq[i] >= FSK_FREQ_MIN1 && freq[i] <= FSK_FREQ_MAX1) || 
             (freq[i] >= FSK_FREQ_MIN2 && freq[i] <= FSK_FREQ_MAX2) || 
             (freq[i] >= FSK_FREQ_MIN3 && freq[i] <= FSK_FREQ_MAX3))))
        {
          err_info.arg_num = i+2;
          goto send_error;
        }
        fsk_hop_table_req.hop_table.freq[i] = (uint32)freq[i];
        num_freq++;
      }
      else
      {
        break;
      }
    }

    fsk_hop_table_req.hop_table.index     = (uint8)index;
    fsk_hop_table_req.hop_table.delta     = (uint8)delta;
    fsk_hop_table_req.hop_table.num_freq  = num_freq;
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_HOP_TABLE_REQ ,
         (msgr_hdr_struct_type *)&fsk_hop_table_req,
         sizeof(geran_fsk_hop_table_req_type)))
    {
      err_info.errval = DSAT_ERR_INVALID_OPERATION;
      goto send_error;
    }
    SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
    return DSAT_ASYNC_CMD;
  }
  else if( tok_ptr->op == (NA|QU))
  {
    memset(&fsk_hop_table_query_req,0,sizeof(geran_fsk_hop_table_query_req_type));
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_HOP_TABLE_QUERY_REQ ,
         (msgr_hdr_struct_type *)&fsk_hop_table_query_req,
         sizeof(geran_fsk_hop_table_query_req_type)))
    {
      err_info.errval = DSAT_ERR_INVALID_OPERATION;
      goto send_error;
    }
    SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
    return DSAT_ASYNC_CMD;
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCFSKHOPTABLE: (0-2),(1-3,5),(%d-%d,%d-%d,%d-%d)",
                                          FSK_FREQ_MIN1,
                                          FSK_FREQ_MAX1,
                                          FSK_FREQ_MIN2,
                                          FSK_FREQ_MAX2,
                                          FSK_FREQ_MIN3,
                                          FSK_FREQ_MAX3
                                          );
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;

  send_error:
  DS_AT_MSG2_ERROR("Error:%d, Arg:%d",err_info.errval, err_info.arg_num);
  return DSAT_ERROR;

}

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCFSKSTART_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCFSKSTART command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcfskstart_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define TDMA_FRAME_MULTIPLE               16
  dsat_result_enum_type                     result = DSAT_OK;
  dsat_num_item_type baudrate; /*<0-2> 0->3466bps(16 bits/frame) 1->1733bps 2->866bps*/

  /* Value allowed multiple of bit per TDMA Frame
  Baudrate: 3466: value allowed 0,16,32,48,64 until 1024
  Baudrate: 1733, value allowed 0,8,16,24,32...until 1024
  Baudrate: 866, value allowed 0,4,8,12,16,20...until 1024
  Gap time depends on value of baudrate */
  dsat_num_item_type gaptime;
  uint8    multiple = TDMA_FRAME_MULTIPLE;
  dsat_num_item_type    bytestosend = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKDATA_IDX,0,NUM_TYPE);
  dsatcmif_servs_state_info  *ph_val = NULL;
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);
  
  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
   Processing for TEST command 
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
    if ( DSAT_OK == result && 
        (tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA)))
    {
      baudrate = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKSTART_IDX,0,NUM_TYPE);
      gaptime = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKSTART_IDX,1,NUM_TYPE);
      multiple = TDMA_FRAME_MULTIPLE >> baudrate;
      if(gaptime % multiple != 0 || 
         !(bytestosend != 0) ||
           gaptime/multiple == 0)
      {
        DS_AT_MSG2_ERROR("invalid operation - gaptime %d bytestosend %d",
                          gaptime,
                          bytestosend);
        result = DSAT_ERROR;
      }
      else
      {
        /* Invoke Call Manager API requesting functionality change */
        result = dsatcmif_change_operating_mode (SYS_OPRT_MODE_LPM);
        SET_PENDING(DSAT_VENDOR_QCFSKSTART_IDX, 0, DSAT_PENDING_TRUE);
        if(result == DSAT_ASYNC_CMD)
        {
          //Do nothing
        }
        else if (DSAT_OK == result && ph_val->is_gw_subscription_available)
        {
          DS_AT_MSG0_HIGH(" FSK start - sim_not_avaialble command invoked");     
          (void) cm_ph_cmd_subscription_not_available3 ( dsatcmif_ph_cmd_cb_func,
                                              NULL,
                                              dsatcm_client_id,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                              CM_SUBSCRIPTION_STATUS_NO_CHANGE);
          result = DSAT_ASYNC_CMD;
        }
        else
        {
          result = dsatvend_send_fsk_start();
        }
      }
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCFSKSTOP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCFSKSTOP command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcfskstop_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type                     result = DSAT_OK;
  geran_fsk_stop_req_type             fsk_stop_req;

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
   Processing for TEST command 
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    result = dsatparm_exec_param_cmd(mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
    if ( DSAT_OK == result && 
        (tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA)))
    {
      memset(&fsk_stop_req,0,sizeof(geran_fsk_stop_req_type));
      if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_STOP_REQ,
         (msgr_hdr_struct_type *)&fsk_stop_req,
         sizeof(geran_fsk_stop_req_type)))
      {
        result = DSAT_ERROR;
      }
      else
      {
        SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
        result = DSAT_ASYNC_CMD;
      }
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCPCLFIX_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCPCLFIX command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpclfix_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  geran_fsk_pcl_req_type            fsk_pcl_req;
  geran_fsk_pcl_query_req_type      fsk_pcl_query_req;
  dsat_num_item_type       pcl_gsm = 99; /*Range:  5-19, default: 99 */
  dsat_num_item_type       pcl_dcs = 99; /*Range: 0-15, default: 99 */
  dsat_num_item_type       pcl_pcs = 99; /*Range: 0-15, default: 99 */
  dsat_error_info_s_type     err_info;
  boolean                   valid_status       = FALSE;

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if(tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA))
  {
    memset(&fsk_pcl_req,0,sizeof(geran_fsk_pcl_req_type));
    /* Check the number of arguments. */
    if ( tok_ptr->args_found > parse_table->compound )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

    if( VALID_TOKEN(0) )
    {
      VALID_NUM_TOKEN(TOKEN_IDX_0,5,99, &pcl_gsm)
      /* Range validation will be taken care by above API. */
      if (!valid_status && ((pcl_gsm >= 5 && pcl_gsm <= 19) || pcl_gsm == 99))
      {
        err_info.arg_num = TOKEN_IDX_0;
        goto send_error;
      }
    }

    if( VALID_TOKEN(1) )
    {
      VALID_NUM_TOKEN(TOKEN_IDX_1,0,99, &pcl_dcs)
      /* Range validation will be taken care by above API. */
      if (!valid_status && ((pcl_dcs >= 0 && pcl_dcs <= 15) || pcl_dcs == 99))
      {
        err_info.arg_num = TOKEN_IDX_1;
        goto send_error;
      }
    }
    
    if( VALID_TOKEN(2) )
    {
      VALID_NUM_TOKEN(TOKEN_IDX_2,0,99, &pcl_pcs)
	  	/* Range validation will be taken care by above API. */
      if (!valid_status && ((pcl_pcs >= 0 && pcl_pcs <= 15) || pcl_pcs == 99))
      {
        err_info.arg_num = TOKEN_IDX_2;
        goto send_error;
      }
    }
    
    fsk_pcl_req.pcl_gsm    = (uint8)pcl_gsm;
    fsk_pcl_req.pcl_dcs    = (uint8)pcl_dcs;
    fsk_pcl_req.pcl_pcs    = (uint8)pcl_pcs;
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_PCL_REQ ,
         (msgr_hdr_struct_type *)&fsk_pcl_req,
         sizeof(geran_fsk_pcl_req_type)))
    {
      err_info.errval = DSAT_ERR_INVALID_OPERATION;
      goto send_error;
    }
    SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
    return DSAT_ASYNC_CMD;
  }
  else if( tok_ptr->op == (NA|QU))
  {
    memset(&fsk_pcl_query_req,0,sizeof(geran_fsk_pcl_query_req_type));
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_PCL_QUERY_REQ ,
         (msgr_hdr_struct_type *)&fsk_pcl_query_req,
         sizeof(geran_fsk_pcl_query_req_type)))
    {
      err_info.errval = DSAT_ERR_INVALID_OPERATION;
      goto send_error;
    }
    SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
    return DSAT_ASYNC_CMD;
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "$QCPCLFIX: (5-19,99),(0-15,99),(0-15,99)"
                                );
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;

  send_error:
  DS_AT_MSG2_ERROR("Error:%d, Arg:%d",err_info.errval, err_info.arg_num);
  return DSAT_ERROR;

}

/*===========================================================================

FUNCTION DSATVEND_PROCESS_FSK_CMD

DESCRIPTION
  This function posts the FSK information from GERAN to DS task and calls 
  the FSK handler API in the DS task.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_fsk_cmd
(
  ds_command_type          *cmd_ptr
)
{
  dsat_fsk_s_type                *fsk_info = NULL;  
  dsat_result_enum_type          result = DSAT_OK;
  dsm_item_type                  *res_buff_ptr = NULL;
  ds_command_enum_type           cmd_id;
  uint32                         *freq = NULL;
  uint8   mode = (uint8)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKSTOP_IDX,0,NUM_TYPE);
  
  dsatcmif_servs_state_info  *ph_val = NULL;
  ph_val = dsat_get_base_addr(DSAT_PH_VALS, TRUE);

  fsk_info = cmd_ptr->cmd_payload_ptr;
  cmd_id = cmd_ptr->hdr.cmd_id;
  DS_AT_MSG1_HIGH("fsk_info cmd %d ", cmd_id);

  if((CHECK_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE)))
  {
    SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_FALSE);
  }
  else if(cmd_id != DS_CMD_ATCOP_FSK_START_RSP_CMD)
  {
    if(cmd_id == DS_CMD_ATCOP_FSK_HOP_TABLE_QUERY_RSP_CMD && fsk_info->status)
    {
      dsatutil_free_memory(fsk_info->hop_table);
    }
    return DSAT_ASYNC_EVENT;
  }
  
  if(fsk_info == NULL)
  { 
    DS_AT_MSG0_ERROR("fsk_info is NULL return error");
    return DSAT_ERROR;
  }
  else
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    if(res_buff_ptr == NULL)
    {
      DS_AT_MSG0_ERROR("Large DSM items exhausted.");
      return DSAT_ERROR;
    }
    switch (cmd_id) 
    {
      case DS_CMD_ATCOP_FSK_DEBUG_RSP_CMD :
      case DS_CMD_ATCOP_FSK_DATA_RSP_CMD :
      case DS_CMD_ATCOP_FSK_DATA_ABT_CMD :
      case DS_CMD_ATCOP_FSK_HOP_TABLE_RSP_CMD :
      case DS_CMD_ATCOP_FSK_PCL_RSP_CMD :
        if(res_buff_ptr != NULL)
        { 
          (void)dsm_free_buffer(res_buff_ptr);
        }
        if(!fsk_info->status)
        {
          result = DSAT_ERROR;
        }
        break;
      case DS_CMD_ATCOP_FSK_STOP_RSP_CMD :
        if(res_buff_ptr != NULL)
        {
          (void)dsm_free_buffer(res_buff_ptr);
        }
        if(!fsk_info->status)
        {
          result = DSAT_ERROR;
        }
        else
        {
          if(mode == 0 || mode == 2)
          {
            SET_PENDING(DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_FALSE);
            result = DSAT_OK;
          }
          else if(mode == 1)
          {
            /* Invoke Call Manager API requesting functionality change */
            result = dsatcmif_change_operating_mode (SYS_OPRT_MODE_ONLINE);
            SET_PENDING(DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_TRUE);
            if(result == DSAT_ASYNC_CMD)
            {
              //Do nothing
            }
            else if (DSAT_OK == result && !ph_val->is_gw_subscription_available)
            {
              DS_AT_MSG0_HIGH(" FSK stop - subs_available command invoked");
              (void) cm_ph_cmd_subscription_available ( dsatcmif_ph_cmd_cb_func,
                                          NULL,
                                          dsatcm_client_id,
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE,
                                          CM_SUBSCRIPTION_STATUS_CHANGE,
                                          CM_SUBSCRIPTION_STATUS_NO_CHANGE);
              result = DSAT_ASYNC_CMD;
            }
            else
            {
              SET_PENDING(DSAT_VENDOR_QCFSKSTOP_IDX, 0, DSAT_PENDING_FALSE);
              result = DSAT_OK;
            }
          }
        }
        break;
      case DS_CMD_ATCOP_FSK_START_RSP_CMD :
        res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "$QCFSKSTART: %d", 
                                             fsk_info->status);
        dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
        result = DSAT_ASYNC_EVENT;
        break;
      case DS_CMD_ATCOP_FSK_DEBUG_QUERY_RSP_CMD :
        if(fsk_info->status)
        {
          res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCFSKDBG: %lu,%d,%d",
                                               fsk_info->freq,
                                               fsk_info->delta,
                                               fsk_info->baudrate);
          DSAT_SEND_RESPONSE( res_buff_ptr, DSAT_COMPLETE_RSP );
        }
        break;
      case DS_CMD_ATCOP_FSK_PCL_QUERY_RSP_CMD :
        if(fsk_info->status)
        {
          res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCPCLFIX: %d,99,99",
                                               fsk_info->pcl_gsm);
          DSAT_SEND_RESPONSE( res_buff_ptr, DSAT_COMPLETE_RSP );
        }
        break;
      case DS_CMD_ATCOP_FSK_HOP_TABLE_QUERY_RSP_CMD :
        if(fsk_info->status)
        {
          for (uint8 i = 0; i < 3; i++)
          {
            res_buff_ptr->used += (word) snprintf(
                                  (char*)(res_buff_ptr->data_ptr + res_buff_ptr->used),
                                  (res_buff_ptr->size - res_buff_ptr->used),
                                   "$QCFSKHOPTABLE: %d,%d",
                                   fsk_info->hop_table[i].index,
                                   fsk_info->hop_table[i].delta);
            freq = fsk_info->hop_table[i].freq;
            for (uint8 j = 0; j < fsk_info->hop_table[i].num_freq; j++)
            {
              if (freq[j])
              {
                res_buff_ptr->used += (word) snprintf(
                            (char*)(res_buff_ptr->data_ptr + res_buff_ptr->used),
                            (res_buff_ptr->size - res_buff_ptr->used),
                             ",%lu",
                             freq[j]);
              }
              else
              {
                res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
                break;
              }
            }
            res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
          }
          res_buff_ptr->data_ptr[res_buff_ptr->used--] = '\0';
          DSAT_SEND_RESPONSE( res_buff_ptr, DSAT_COMPLETE_RSP );
          dsatutil_free_memory(fsk_info->hop_table);
        }
        break;
      default:
        break;
    }
  }  
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_FSK_DATA_MSG

DESCRIPTION
  Central place/function call for sending FSK data.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that data in HEX format has to be typed. Then the 
  message is transmited.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatvend_send_fsk_data
(
 byte  * msg, /* Ptr to the FSK data that has to be sent */
 int     len  /* Len of the FSK data that has to be sent */
)
{
  byte                           *msg_dup = msg;
  byte                            hex_str[3];
  dsat_num_item_type              out_val = 0;
  uint8                           data[200]={0};
  geran_fsk_data_req_type         fsk_data_req;
  dsat_num_item_type    bytestosend = (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKDATA_IDX,0,NUM_TYPE);

  DS_ASSERT( NULL != msg_dup );

  DS_AT_MSG3_HIGH("dsatme_send_fsk_data, len: %d, msg: %c%c...",
                len, msg_dup[0], msg_dup[1]);

  /*Hex data in ASCII so double the size */
  if ( len != bytestosend * 2 ) 
  {
    dsatvend_fsk_data_abort_cmd(FALSE);
    return;
  }

  for (uint8 i = 0; i < bytestosend; i++)
  {
    hex_str[0] = *msg_dup++;
    hex_str[1] = *msg_dup++;
    hex_str[2] = '\0';
    if(ATOI_OK != dsatutil_atoi (&out_val, hex_str, 16))
    {
      dsatvend_fsk_data_abort_cmd(FALSE);
      return;
    }
    data[i] = (uint8) out_val;
  }

  memset(&fsk_data_req,0,sizeof(geran_fsk_data_req_type));

  fsk_data_req.bytestosend = (uint8) bytestosend;
  (void) dsatutil_memscpy((void*)fsk_data_req.data,
                           bytestosend,
                           (void*)data,
                           bytestosend);

  if (!ds_msgr_snd_msg_send(
       GERAN_GL1_FSK_DATA_REQ,
       (msgr_hdr_struct_type *)&fsk_data_req,
       sizeof(geran_fsk_data_req_type)))
  {
    dsatvend_fsk_data_abort_cmd(FALSE);
    return;
  }
  else
  {
    SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
  }
}/* dsatvend_send_fsk_data */

/*===========================================================================

FUNCTION DSATVEND_FSK_DATA_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an FSK Data command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send a DS CMD to the AT Command processor indicating that
  the command has been aborted.

  If status is TRUE, the function is called when user enters ESC key.
  If status is FALSE, the function is called when we run out of buffer
                  or other cmd error.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  DS CMD is generated.

===========================================================================*/
void dsatvend_fsk_data_abort_cmd
(
  boolean status
)
{
  ds_command_type       *cmd_ptr;
  dsat_fsk_s_type       *fsk_info = NULL;

  /* tell atcop that there is a command abort */
  cmd_ptr = dsat_get_cmd_buf(sizeof(dsat_fsk_s_type), FALSE);
  fsk_info = cmd_ptr->cmd_payload_ptr;

  DS_AT_MSG0_HIGH("Aborting FSK Data command");

  SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_FSK_DATA_ABT_CMD;
  fsk_info->status = status;
  ds_put_cmd(cmd_ptr);
}/* dsatvend_fsk_data_abort_cmd */

/*===========================================================================

FUNCTION DSATVEND_SEND_FSK_START

DESCRIPTION
  This function posts the FSK information from GERAN to DS task and calls 
  the FSK handler API in the DS task.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_send_fsk_start()
{
  dsat_result_enum_type          result = DSAT_OK;

  geran_fsk_start_req_type            fsk_start_req;
  geran_fsk_debug_req_type            fsk_dbg_req;
  uint16  gaptime;
  uint8   restart_mode;
  uint32          freq; /*Range: 902000-928000, 966037-967957*/
  uint8          delta = 2; /*possible values: 1,2,3,5 default: 2 */
  uint8       baudrate = 1; /*possible values: 0,1,2   default: 1 */

  if ( CHECK_PENDING( DSAT_VENDOR_QCFSKSTART_IDX, 0, DSAT_PENDING_TRUE ))
  {
    baudrate = (uint8)dsatutil_get_val(
                                         DSAT_VENDOR_QCFSKSTART_IDX,0,NUM_TYPE);
    gaptime = (uint16)dsatutil_get_val(
                                         DSAT_VENDOR_QCFSKSTART_IDX,1,NUM_TYPE);
    restart_mode = (uint8)dsatutil_get_val(
                                         DSAT_VENDOR_QCFSKSTART_IDX,2,NUM_TYPE);

    memset(&fsk_start_req,0,sizeof(geran_fsk_start_req_type));
    fsk_start_req.baudrate     = baudrate;
    fsk_start_req.gaptime      = gaptime;
    fsk_start_req.restart_mode = restart_mode;
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_START_REQ,
         (msgr_hdr_struct_type *)&fsk_start_req,
         sizeof(geran_fsk_start_req_type)))
    {
      result = DSAT_ERROR;
    }
    else
    {
      SET_PENDING(DSAT_VENDOR_QCFSKSTART_IDX, 0, DSAT_PENDING_FALSE);
      result = DSAT_OK;
    }
  }
  else if( CHECK_PENDING( DSAT_VENDOR_QCFSKDBG_IDX, 0, DSAT_PENDING_TRUE ))
  {
    freq = (uint32)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKDBG_IDX,0,NUM_TYPE);
    delta = (uint8)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKDBG_IDX,1,NUM_TYPE);
    baudrate = (uint8)dsatutil_get_val(
                                       DSAT_VENDOR_QCFSKDBG_IDX,2,NUM_TYPE);

    memset(&fsk_dbg_req,0,sizeof(geran_fsk_debug_req_type));
    fsk_dbg_req.freq     = freq;
    fsk_dbg_req.delta    = delta;
    fsk_dbg_req.baudrate = baudrate;
    if (!ds_msgr_snd_msg_send(
         GERAN_GL1_FSK_DEBUG_REQ,
         (msgr_hdr_struct_type *)&fsk_dbg_req,
         sizeof(geran_fsk_debug_req_type)))
    {
      result = DSAT_ERROR;
    }
    else
    {
      SET_PENDING(DSAT_VENDOR_QCFSKDBG_IDX, 0, DSAT_PENDING_FALSE);
      SET_PENDING(DSAT_VENDOR_QCFSK_IDX, 0, DSAT_PENDING_TRUE);
      result = DSAT_ASYNC_CMD;
    }
  }
  return result;
}

#endif /* FEATURE_FSK_TX */

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCDLTPTENH_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCDLTPTENH command.
  Parameter values:
    0 - Enable DL throughput enhancement.
    1 - Disable DL throughput enhancement.

  By default, DL throughput enhancement is disabled.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdltptenh_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  int16                           dss_errno = DS_ENOERR;
  int16                           return_val = DSS_SUCCESS;
  ds_sys_ioctl_l2_rai_info        ds_sys_ioctl_dl_tput_enh_info;
  dsat_num_item_type              dsat_dl_tput_enh_val; /*possible values 0,1*/
  dsat_error_info_s_type          err_info;
  boolean                         valid_status           = FALSE;

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
  
  if(tok_ptr->op == (NA|EQ|AR))
  {
    /* Check the number of arguments. */
    if ( tok_ptr->args_found != parse_table->compound )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
  
    VALID_NUM_TOKEN(TOKEN_IDX_0,0,1, &dsat_dl_tput_enh_val)
    if (!valid_status)
    {
      err_info.arg_num = TOKEN_IDX_0;
      goto send_error;
    }

    /*-------------------------------------------------------------------------
      Set DL throughput info by calling into DS Sys Layer
    -------------------------------------------------------------------------*/ 
    if(ds_fwk_get_dl_throughput_status() != dsat_dl_tput_enh_val)
    {
      memset(&ds_sys_ioctl_dl_tput_enh_info, 0, sizeof(ds_sys_ioctl_l2_rai_info));
      ds_sys_ioctl_dl_tput_enh_info.rai_val = dsat_dl_tput_enh_val;
      return_val = ds_sys_ioctl(DS_SYS_TECH_3GPP, 
                                DS_SYS_IOCTL_3GPP_SET_L2_ENABLE_RAI, 
                                &ds_sys_ioctl_dl_tput_enh_info, 
                                &dss_errno);

      if (return_val == DSS_SUCCESS)
      {
        ds_fwk_set_dl_throughput_status(dsat_dl_tput_enh_val);
      }
      else
      {
        DS_AT_MSG2_ERROR("$qcdltptenh ds_sys_ioctl ret [%d] errno [%d]", return_val, dss_errno);
        return DSAT_ERROR;
      }
    }
  }
  else if( tok_ptr->op == (NA|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCDLOPT: %d",
                                          ds_fwk_get_dl_throughput_status()
                                          );
  }
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          "$QCDLOPT: (0-1)"
                                          );
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;

  send_error:
  DS_AT_MSG2_ERROR("Error:%d, Arg:%d",err_info.errval, err_info.arg_num);
  return DSAT_ERROR;
}

#endif

