/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

                 E T S I   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the AT modem commands that are specific to ETSI mode of
  operation.

EXTERNALIZED FUNCTIONS
  None
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2001 - 2023 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatetsictab_ex.c#7 $ $DateTime: 2023/05/16 04:05:59 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/23   ks      Enhancing +COPS, +CEREG commands to support NTN RAT.
02/24/21   ks      Changes made to remove unsupported parameter <bfr> from 
                   +CMER command.
08/19/20   ks      Fixed DUN call failure issue caused by +CGDCONT test command
                   fix by reverting the changes.
08/17/20   ks      Fixed issue related to +CGDCONT test command.
01/15/18   rk      Fixed issue related to COPS cmd.
01/08/17   skc     Fixed issue with AT+CLAC where +IPR was displaying twice.
09/21/17   rg      Added support for Non-IP pdp type profile.
06/13/17   rk      Added support for +CEDRXS & +CEDRXRDP command.
05/01/17   skc     Featurizing PBM Code. 
04/17/17   skc     Enabled requested AT Commands for IOE Build.
04/13/17   rg      Added rau and gprs ready timer for CPSMS
10/18/16   skc     Fixed issue related to AT+CLAC
09/26/16   skc     Changes made for PSM timer input to spec compliant.
09/23/16   skc     Added support for AT+CPSMS.
09/11/14   tk      Fixed compilation errors in LTE compile out flavor.
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"
#include "dsatetsictab.h"
#include "dsatctab.h"
#include "dsatact.h"
#include "dsatparm.h"
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#include "dsatsms.h"
#include "dsatetsisms.h"
#include "dsatetsime.h"
#include "dsatetsicall.h"
#include "mn_cm_exp.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/

/* GPRS Quality of Service Profile limits */
/*   see 3GPP TS 03.60 section 15.2       */
#define MAX_QOS_GPRS_REQ_PRECEDENCE_CLASS    3
#define MAX_QOS_GPRS_REQ_DELAY_CLASS         4
#define MAX_QOS_GPRS_REQ_RELIABILITY_CLASS   5
#define MAX_QOS_GPRS_REQ_PEAK_BITRATE_CLASS  9
#define MAX_QOS_GPRS_REQ_MEAN_BITRATE_CLASS  18

#define MAX_QOS_GPRS_MIN_PRECEDENCE_CLASS    3
#define MAX_QOS_GPRS_MIN_DELAY_CLASS         4
#define MAX_QOS_GPRS_MIN_RELIABILITY_CLASS   5
#define MAX_QOS_GPRS_MIN_PEAK_BITRATE_CLASS  9
#define MAX_QOS_GPRS_MIN_MEAN_BITRATE_CLASS  18

/* UMTS Quality of Service Profile limits */
/*   see 3GPP TS 23.107 section 6.5.1     */
/*       3GPP TS 24.008 section 10.5.6.5  */
#define MAX_QOS_UTMS_GTD_UL_BITRATE  QOS_MAX_UL_KBIT_RATE
#define MAX_QOS_UTMS_REQ_UL_BITRATE  QOS_MAX_UL_KBIT_RATE

#define MAX_QOS_UTMS_REQ_DL_BITRATE  QOS_MAX_DL_KBIT_RATE
#define MAX_QOS_UTMS_GTD_DL_BITRATE  QOS_MAX_DL_KBIT_RATE

#define MAX_QOS_UTMS_SDU_SIZE        1520
#define MAX_QOS_UTMS_TRANSFER_DELAY  4000
#define MIN_QOS_UTMS_TRANSFER_DELAY  100

dsat_num_item_type dsat_cbst_export_val[3] = {0};

/* +CSTA Command */
LOCAL const dflm_type dsat_csta_dflm [] =
{
  { DSAT_TA_UNKNOWN, DSAT_TA_UNKNOWN,       DSAT_TA_UNKNOWN },
  { DSAT_TA_UNKNOWN, DSAT_TA_INTERNATIONAL, DSAT_TA_INTERNATIONAL},
  { MAX_BROKEN_RANGE, 0, 0 },
};


/* +CSCS Command */
/* Values indices in array should match enum dsati_chset_type in dsati.h 
   ALPHA_HEX added to dsati_chset_type but not here so that conversion
   could be used in SMS functions but this character set wasn't requested 
   for +CSCS */
LOCAL const dsat_string_item_type dsat_cscs_valstr [][8] =
{ "IRA", "GSM", "UCS2", "" };

LOCAL const dsat_string_item_type dsat_cscs_tststr [] =
{ "(\"IRA\",\"GSM\",\"UCS2\")" };

LOCAL const def_list_type dsat_cscs_list =
{ 0, dsat_cscs_valstr, dsat_cscs_tststr };


/* +CR Command */
LOCAL const dsat_string_item_type dsat_cr_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_cr_tststr [] =
{ "(0,1)" };

const def_list_type dsat_cr_list =
{ 0, dsat_cr_valstr, dsat_cr_tststr };

/* +CRC Command */
LOCAL const dsat_string_item_type dsat_crc_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_crc_tststr [] =
{ "(0,1)" };

const def_list_type dsat_crc_list =
{ 0, dsat_crc_valstr, dsat_crc_tststr };

/* cpms values */
LOCAL const dsat_string_item_type dsat_cpms_mem1_valstr[][8] =
{ "ME", "MT", "SM", "SR", "" };

LOCAL const dsat_string_item_type dsat_cpms_mem1_tststr[] =
{ "(\"ME\",\"MT\",\"SM\",\"SR\")" };

LOCAL const dsat_string_item_type dsat_cpms_mem2_valstr[][8] =
{ "ME", "MT", "SM", "SR" };

LOCAL const dsat_string_item_type dsat_cpms_mem2_tststr[] =
{ "(\"ME\",\"MT\",\"SM\",\"SR\")" };

LOCAL const dsat_string_item_type dsat_cpms_mem3_valstr[][8] =
{ "ME", "MT", "SM", "SR", "" };

LOCAL const dsat_string_item_type dsat_cpms_mem3_tststr[] =
{ "(\"ME\",\"MT\",\"SM\",\"SR\")" };

const def_list_type dsat_cpms_list[] =
{
  { 0, dsat_cpms_mem1_valstr, dsat_cpms_mem1_tststr },
  { 0, dsat_cpms_mem2_valstr, dsat_cpms_mem2_tststr },
  { 0, dsat_cpms_mem3_valstr, dsat_cpms_mem3_tststr }
};

/*  Numeric Config items  */

#ifdef FEATURE_ETSI_SMS
/* SMS Commands */
/* +CSMS Command */
LOCAL const dflm_type dsat_csms_dflm[] =
{
  { 0  , 0  , 1  }
} ;

/* +CMGF Command */
LOCAL const dflm_type dsat_cmgf_dflm [] =
{
  { 0  , 0  , 1  },
} ;
#endif /* FEATURE_ETSI_SMS */

/* +CSDH Command */
LOCAL const dflm_type dsat_csdh_dflm [] =
{
  { 0  , 0  , 1  },
} ;

LOCAL const dsat_string_item_type dsat_csdh_tststr [] =
  { "(0, 1)"};

/* +CNMI Command */
LOCAL const dsat_string_item_type dsat_cnmi_mode_valstr [][8] =
{
  "0", "1", "2", ""
};

LOCAL const dsat_string_item_type dsat_cnmi_mode_tststr [] =
{ 
  "(0,1,2)" 
};

/* NOTE: Enable this only when WMS releases all the needed fixes  */
LOCAL const dsat_string_item_type dsat_cnmi_mt_valstr [][8] =
{ "0", "1", "2", "3", "" };

LOCAL const dsat_string_item_type dsat_cnmi_mt_tststr [] =
{ "(0,1,2,3)" };

#ifdef FEATURE_ETSI_SMS_CB
LOCAL const dsat_string_item_type dsat_cnmi_bm_valstr [][8] =
{ "0", "1", "2", "" }; /* Currently <bm>=1 is not supported */

LOCAL const dsat_string_item_type dsat_cnmi_bm_tststr [] =
{ "(0,2)" };
#else
LOCAL const dsat_string_item_type dsat_cnmi_bm_valstr [][8] =
{ "0", "" };

LOCAL const dsat_string_item_type dsat_cnmi_bm_tststr [] =
{ "(0)" };
#endif 

LOCAL const dsat_string_item_type dsat_cnmi_ds_valstr [][8] =
{ "0", "1", "2", "" };

LOCAL const dsat_string_item_type dsat_cnmi_ds_tststr [] =
{ "(0,1,2)" };

LOCAL const dsat_string_item_type dsat_cnmi_bfr_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_cnmi_bfr_tststr [] =
{ "(0,1)" };

const def_list_type dsat_cnmi_list[] =
{
  { 0, dsat_cnmi_mode_valstr, dsat_cnmi_mode_tststr },
  { 0, dsat_cnmi_mt_valstr, dsat_cnmi_mt_tststr },
  { 0, dsat_cnmi_bm_valstr, dsat_cnmi_bm_tststr },
  { 0, dsat_cnmi_ds_valstr, dsat_cnmi_ds_tststr },
  { 0, dsat_cnmi_bfr_valstr, dsat_cnmi_bfr_tststr }
} ;
/* CSAS comand */
LOCAL const dflm_type dsat_csas_dflm [] =
{
  { 0  , 0  , 0  },  /* Currently only profile 0 is supported */
} ;

/* CRES comand */
LOCAL const dflm_type dsat_cres_dflm [] =
{
  { 0  , 0  , 0  },  /* Currently only profile 0 is supported */
} ;

/* CSCA command */
LOCAL word dsat_csca_sca_string_len = CSCA_SCA_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csca_sca_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_csca_sca_string_len
};

LOCAL word dsat_csca_tosca_string_len = CSCA_TOSCA_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csca_tosca_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_csca_tosca_string_len
};

LOCAL const mixed_def_s_type * dsat_csca_mixed_dfl[2] = 
{
  &dsat_csca_sca_def_val, &dsat_csca_tosca_def_val
};

/* CSMP Command */
LOCAL word dsat_csmp_fo_string_len = CSMP_FO_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csmp_fo_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_csmp_fo_string_len
};

LOCAL word dsat_csmp_vp_string_len = CSMP_VP_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csmp_vp_def_val =
{
  CONFIG | STRING, (mixed_def_u_type *) &dsat_csmp_vp_string_len
};

LOCAL const dflm_type dsat_csmp_pid_dflm =
{
  0, 0, 255
};

LOCAL const mixed_def_s_type dsat_csmp_pid_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_csmp_pid_dflm
};

LOCAL const dflm_type dsat_csmp_dcs_dflm =
{
  0, 0, 255
};

LOCAL const mixed_def_s_type dsat_csmp_dcs_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_csmp_dcs_dflm
};

LOCAL const mixed_def_s_type * dsat_csmp_mixed_dfl[4] = 
{
  &dsat_csmp_fo_def_val , &dsat_csmp_vp_def_val, 
  &dsat_csmp_pid_def_val, &dsat_csmp_dcs_def_val
};

#ifdef FEATURE_ETSI_SMS_CB
/* CSCB Command */
LOCAL const dflm_type dsat_cscb_mode_dflm[] =
{
  { 0, 0, 1 },
};

LOCAL const mixed_def_s_type dsat_cscb_mode_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cscb_mode_dflm
};

LOCAL word dsat_cscb_mids_string_len = CSCB_MIDS_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cscb_mids_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cscb_mids_string_len
};

LOCAL word dsat_cscb_dcss_string_len = CSCB_DCSS_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cscb_dcss_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cscb_dcss_string_len
};

LOCAL const mixed_def_s_type * dsat_cscb_mixed_dfl[3] = 
{
  &dsat_cscb_mode_def_val, 
  &dsat_cscb_mids_def_val, 
  &dsat_cscb_dcss_def_val
};
#endif /* FEATURE_ETSI_SMS_CB */

 const dflm_type dsat_cmms_dflm =
{ 0, 0, 2 };


#ifdef FEATURE_ETSI_SMS

#ifdef FEATURE_ETSI_SMS_PS
/* there is no default for +CGSMS,
   put 0 here to please compiler, it is not used anywhere.
*/
const dflm_type          dsat_cgsms_dflm = {0, 0, 3};

#endif /* FEATURE_ETSI_SMS_PS */

dsat_string_item_type dsat_csca_prev_sca_val[CSCA_SCA_STR_MAX_LEN+1];
dsat_string_item_type dsat_csca_prev_tosca_val[CSCA_TOSCA_STR_MAX_LEN+1];

dsat_mixed_param_val_type dsat_csca_prev_val[2] = {
  {dsat_csca_prev_sca_val}, 
  {dsat_csca_prev_tosca_val}
};
#endif /* FEATURE_ETSI_SMS */

dsat_num_item_type dsat_cmgw_val[3][25];

#ifdef FEATURE_CMGC
dsat_num_item_type dsat_cmgc_val[6][25];
#endif /* FEATURE_CMGC */


LOCAL const dflm_type dsat_cmod_dflm [] =
{
  { 0  , 0  , 0 },
} ;

/* +CEN Command */

LOCAL const dflm_type dsat_cen_dflm [] =
{
  { 0  , 0  , 1 },
} ;

#ifdef FEATURE_DSAT_LTE
  
LOCAL const dflm_type dsat_cemode_dflm [] =
{
  { 0  , 0  , 3 },
} ;
 
#endif /* FEATURE_DSAT_LTE */

/* +CVHU Command */
/* Default value of command is 1 which violates 27.007 recommendation but 
   was required so that ATH wouldn't end voice calls during concurrent calls */
LOCAL const dflm_type dsat_cvhu_dflm [] =
{
  { 1  , 0  , 1 },
} ;

#ifdef FEATURE_DSAT_ETSI_DATA

const dflm_type dsat_pdp_cid_dflm[] =
{
  { DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MAX},
} ;

const dflm_type dsat_emergency_dflm[] =
{
    { 0  , 0  , 1 },
} ;

/* Parameter that controls how the MT/TA requests to get the IPv4 address 
   information */
const dflm_type dsat_cgdcont_ipv4_addr_alloc_dflm[] =
{
  { 0 , 0 , 1}, /* Defualt - 0 - Address Allocation through NAS Signaling  */
} ;


#ifdef FEATURE_DATA_PS_IPV6
  const dsat_string_item_type dsat_pdptype_valstr [][8] =
  { "IP", "PPP", "IPV6", "IPV4V6", "Non-IP","" };

  LOCAL const dsat_string_item_type dsat_pdptype_tststr [] =
  { "(\"IP\",\"PPP\",\"IPV6\",\"IPV4V6\",\"Non-IP\")" };
#else  /* FEATURE_DATA_PS_IPV6 */
  const dsat_string_item_type dsat_pdptype_valstr [][8] =
  { "IP", "PPP", "IPV4V6", "Non-IP","" };
  
  LOCAL const dsat_string_item_type dsat_pdptype_tststr [] =
  { "(\"IP\",\"PPP\",\"IPV4V6\",\"Non-IP\")" };
#endif /* FEATURE_DATA_PS_IPV6 */

/* CGDCONT Command */
#ifdef FEATURE_GPRS_COMP_ENUM_TYPES
const dsat_string_item_type dsat_cgdcont_hcomp_valstr [][8] =
{ "0", "1", "2",
  "3",
  "4",
  "" };

/* Both IPHC and ROHC are ON */
const dsat_string_item_type dsat_cgdcont_hcomp_tststr[]
  = { "(0-4)" };

const dsat_string_item_type dsat_cgdcont_dcomp_valstr [][8] =
{ "0", "1","2", "" };

const dsat_string_item_type dsat_cgdcont_dcomp_tststr [] =
{ "(0-2)" };

#else /* FEATURE_GPRS_COMP_ENUM_TYPES */

const dsat_string_item_type dsat_cgdcont_hcomp_valstr [][8] =
{ "0", "1", "" };
const dsat_string_item_type dsat_cgdcont_dcomp_valstr [][8] =
{ "0", "1","" };
const dsat_string_item_type dsat_cgdcont_hcomp_tststr [] =
{ "(0-1)" };
const dsat_string_item_type dsat_cgdcont_dcomp_tststr [] =
{ "(0-1)" };
#endif /* FEATURE_GPRS_COMP_ENUM_TYPES */

LOCAL const def_list_type dsat_cgdcont_pdptype_list =
{ 0, dsat_pdptype_valstr, dsat_pdptype_tststr };

LOCAL const def_list_type dsat_cgdcont_hcomp_list =
{ 0, dsat_cgdcont_hcomp_valstr, dsat_cgdcont_hcomp_tststr };

LOCAL const def_list_type dsat_cgdcont_dcomp_list =
{ 0, dsat_cgdcont_dcomp_valstr, dsat_cgdcont_dcomp_tststr };

/* Allow quotes on following strings */
LOCAL const word dsat_cgdcont_apn_string_len = DS_PROFILE_APN_STRING_LEN+2;
LOCAL const word dsat_cgdcont_pdpaddr_string_len = MAX_IPADDR_STR_LEN+2;


LOCAL const mixed_def_s_type dsat_pdp_cid_def_val =
{
  CONFIG | LOCAL_TEST , (mixed_def_u_type *) dsat_pdp_cid_dflm
};

LOCAL const mixed_def_s_type dsat_cgdcont_ipv4_addr_alloc_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgdcont_ipv4_addr_alloc_dflm
};

LOCAL const mixed_def_s_type dsat_cgdcont_pdptype_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgdcont_pdptype_list
};

LOCAL const mixed_def_s_type dsat_cgdcont_apn_def_val =
{
  CONFIG | STRING | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgdcont_apn_string_len
};

LOCAL const mixed_def_s_type dsat_cgdcont_pdpaddr_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_cgdcont_pdpaddr_string_len
};

LOCAL const mixed_def_s_type dsat_cgdcont_hcomp_def_val =
{
  CONFIG |  LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cgdcont_hcomp_list
};

LOCAL const mixed_def_s_type dsat_cgdcont_dcomp_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cgdcont_dcomp_list
};

LOCAL const mixed_def_s_type dsat_cgdcont_emergency_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_emergency_dflm
};

LOCAL const mixed_def_s_type * dsat_cgdcont_mixed_dfl[7] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgdcont_pdptype_def_val,
  &dsat_cgdcont_apn_def_val,
  &dsat_cgdcont_pdpaddr_def_val,
  &dsat_cgdcont_dcomp_def_val,
  &dsat_cgdcont_hcomp_def_val,
  &dsat_cgdcont_ipv4_addr_alloc_val
//  &dsat_cgdcont_emergency_def_val
};

//Does CGEQOS support is there?

#ifdef FEATURE_DSAT_LTE
#define  QOS_LTE_MAX_DL_KBIT_RATE                  150000
#define  QOS_LTE_MAX_UL_KBIT_RATE                  50000

#define QOS_3GPP_MAX_DL_KBIT_RATE  QOS_LTE_MAX_DL_KBIT_RATE
#define QOS_3GPP_MAX_UL_KBIT_RATE  QOS_LTE_MAX_UL_KBIT_RATE

#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DSAT_ETSI_DATA
/* +CGEREP command */
LOCAL const dflm_type dsat_cgerep_dflm [] =
{
  { 0  , 0  , 2  },   /* mode */
  { 0  , 0  , 1  },   /* bfr  */
} ;

/* +CGPIAF command */

LOCAL const dflm_type dsat_cgpiaf_dflm[] =
{
  { 0  , 0  , 1  },   /* Address Format */
  { 0  , 0  , 1  },   /* Subnet Notation  */
  { 0  , 0  , 1  },   /* Leading Zeros  */
  { 0  , 0  , 1  },   /* Compress Zeros  */
} ;
#endif /* FEATURE_DSAT_ETSI_DATA */


/* +CGATT command */
#ifdef FEATURE_ETSI_ATTACH
LOCAL const dsat_string_item_type dsat_cgatt_valstr [][8] =
{ "0", "1", "" } ;

LOCAL const dsat_string_item_type dsat_cgatt_tststr [] =
{ "(0,1)" };
#else
const dsat_string_item_type dsat_cgatt_valstr [][8] =
{ "" } ;

LOCAL const dsat_string_item_type dsat_cgatt_tststr [] =
{ "" };
#endif /* FEATURE_ETSI_ATTACH */

LOCAL def_list_type dsat_cgatt_list =
{
  0,
  dsat_cgatt_valstr,
  dsat_cgatt_tststr
};

/* +CGACT command */
LOCAL const dsat_string_item_type dsat_cgact_state_valstr [][8] =
{ "0", "1", "" } ;

LOCAL const dsat_string_item_type dsat_cgact_state_tststr [] =
{ "(0,1)" };

LOCAL def_list_type dsat_cgact_list =
{
  0,
  dsat_cgact_state_valstr,
  dsat_cgact_state_tststr
};

#if defined(FEATURE_GSM)

/* +CGCLASS command */
#if defined( FEATURE_WCDMA )
LOCAL const dsat_string_item_type dsat_cgclass_valstr [][8] =
{ "A", "" };

LOCAL const dsat_string_item_type dsat_cgclass_tststr [] =
{ "(\"A\")" };

#elif defined (FEATURE_GSM_GPRS)
const dsat_string_item_type dsat_cgclass_valstr [][8] =
{ "B", "" };

LOCAL const dsat_string_item_type dsat_cgclass_tststr [] =
{ "(\"B\")" };
#endif /* defined( FEATURE_WCDMA )   */

LOCAL def_list_type dsat_cgclass_list =
{
  0,
  dsat_cgclass_valstr,
  dsat_cgclass_tststr
};
#endif /* defined(FEATURE_GSM) */

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_ETSI_PIN 
/* CPIN Command */
LOCAL const word dsat_cpin_pin_string_len = MAX_PIN_STRING_LEN;
LOCAL const word dsat_cpin_newpin_string_len = MAX_PIN_STRING_LEN;

LOCAL const mixed_def_s_type dsat_cpin_pin_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_cpin_pin_string_len
};

LOCAL const mixed_def_s_type dsat_cpin_newpin_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_cpin_newpin_string_len
};

LOCAL const mixed_def_s_type * dsat_cpin_mixed_dfl[2] = 
{
  &dsat_cpin_pin_def_val,
  &dsat_cpin_newpin_def_val
};
#endif /* FEATURE_DATA_ETSI_PIN */


/* +CREG Command */
LOCAL const dflm_type dsat_creg_dfl[1] =
{ 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  { 0, 0, 2 },
  /* 0 - disable, 1 - enable, 2 - enable with location and cell id */
#else
  { 0, 0, 1 },     /* 0 - disable, 1 - enable */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
};

/* +CGREG Command */
LOCAL const dflm_type dsat_cgreg_dfl[] =
{ 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
	{ 0, 0, 2 },    /* 0 - disable, 1 - enable, 2 - enable with location and cell id */
	{ 0, 4, 4 },    /* 0 - disable, 4 - display gprs time and periodic RAU  */
	{ MAX_BROKEN_RANGE, 0, 0 }
#else
	{ 0, 0, 1 },     /* 0 - disable, 1 - enable */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
};
/* +CEREG Command */
#ifdef FEATURE_DSAT_LTE

/* +CEREG Command */
LOCAL const dflm_type dsat_cereg_dfl[] =
{
    { 0, 0, 2 },    /* 0 - disable, 1 - enable, 2 - enable with location and cell id */
    { 0, 4, 4 },    /* 0 - disable, 4 - display active time and periodic TAU  */
    { MAX_BROKEN_RANGE, 0, 0 }
};
#endif /* FEATURE_DSAT_LTE */

/* +CTFR Command */

#define CTFR_NUMBER_STR_MAX_LEN (CM_MAX_NUMBER_CHARS+2)
#define CTFR_SUBADDR_STR_MAX_LEN (CM_MAX_CALLED_INFO_CHARS+2)


word dsat_ctfr_number_string_len = CTFR_NUMBER_STR_MAX_LEN;
LOCAL dsat_string_item_type ctfr_number_val[CTFR_NUMBER_STR_MAX_LEN+1];

const mixed_def_s_type dsat_ctfr_number_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_ctfr_number_string_len
};

word dsat_ctfr_subaddr_string_len = CTFR_SUBADDR_STR_MAX_LEN;
dsat_string_item_type ctfr_subaddr_val[CTFR_SUBADDR_STR_MAX_LEN+1];


LOCAL const mixed_def_s_type dsat_ctfr_subaddr_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_ctfr_subaddr_string_len
};

LOCAL const dflm_type dsat_ctfr_type_dflm =
{
  129, 0, 255
};

LOCAL const mixed_def_s_type dsat_ctfr_type_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ctfr_type_dflm
};

LOCAL const dflm_type dsat_ctfr_satype_dflm =
{
  128, 0, 255
};

LOCAL const mixed_def_s_type dsat_ctfr_satype_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ctfr_satype_dflm
};

LOCAL const mixed_def_s_type * dsat_ctfr_mixed_dfl[4] = 
{
  &dsat_ctfr_number_def_val,
  &dsat_ctfr_type_def_val,
  &dsat_ctfr_subaddr_def_val,
  &dsat_ctfr_satype_def_val,
};



/* +CCUG Command */
LOCAL const dflm_type dsat_ccug_dfl[3] =
{ 
  { 0, 0, 1 },    /* enable/disable */
  { 0, 0, 10 },   /* index */
  { 0, 0, 3 },    /* info */
};

/* +COPS Command */
LOCAL const dsat_string_item_type dsat_cops_mode_valstr [][8] =
{
  "0", "1", "2", "3", "4", ""
};

LOCAL const dsat_string_item_type dsat_cops_mode_tststr [] =
{ "(0,1,2,3,4)" };

LOCAL const def_list_type dsat_cops_mode_list =
{
  0, dsat_cops_mode_valstr, dsat_cops_mode_tststr
};

LOCAL const mixed_def_s_type dsat_cops_mode_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cops_mode_list
};

LOCAL const dsat_string_item_type dsat_cops_format_valstr [][8] =
{
  "0", "1", "2", "" 
};

LOCAL const dsat_string_item_type dsat_cops_format_tststr [] =
{ "(0,1,2)" };

LOCAL const def_list_type dsat_cops_format_list =
{ 0, dsat_cops_format_valstr, dsat_cops_format_tststr };

LOCAL const mixed_def_s_type dsat_cops_format_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cops_format_list
};

LOCAL word dsat_cops_oper_string_len = COPS_OPER_STR_MAX_LEN;
LOCAL const mixed_def_s_type dsat_cops_oper_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cops_oper_string_len
};

#ifdef FEATURE_DSAT_LTE
#ifdef FEATURE_NBIOT_NTN
LOCAL const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", "2", "7", "9", "14", ""  /* 0-GSM   2-UTRAN   7-E-UTRAN   9-EUTRAN NB1 14-Satellite EUTRAN NB1(NTN)*/
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0,2,7,9,14)" };
#else
LOCAL const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", "2", "7", "9", ""  /* 0-GSM   2-UTRAN   7-E-UTRAN   9-EUTRAN NB1 */
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0,2,7,9)" };
#endif /* FEATURE_NBIOT_NTN */
#elif defined (FEATURE_WCDMA)
LOCAL const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", "2", ""
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0,2)" };
#else
const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", ""
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0)" };
#endif /* FEATURE_DSAT_LTE  */

LOCAL const def_list_type dsat_cops_act_list =
{
  0, dsat_cops_act_valstr, dsat_cops_act_tststr
};


LOCAL const mixed_def_s_type dsat_cops_act_def_val =
{
  CONFIG | LIST | LOCAL_TEST | NO_DISPLAY,
  (mixed_def_u_type *) &dsat_cops_act_list
};


const mixed_def_s_type * dsat_cops_mixed_dfl[6] = 
{
  &dsat_cops_mode_def_val,
  &dsat_cops_format_def_val,
  &dsat_cops_oper_def_val,
  &dsat_cops_act_def_val
};
/* +CLCK Command */
#define CLCK_FAC_STR_MAX_LEN 4
#define CLCK_MODE_STR_MAX_LEN 1
/* Allow for quotes on next string */
#define CLCK_PASSWD_STR_MAX_LEN (MAX_PWD_CHAR+2)

word dsat_clck_fac_string_len = CLCK_FAC_STR_MAX_LEN;
LOCAL dsat_string_item_type clck_number_val[CLCK_FAC_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_clck_fac_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_clck_fac_string_len
};

/* The order of facility entries must be reflected in ss_fac_e_type */
/* For all Personalization facilities the order is in cb_cmd_e_type */
#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI_PERSONALIZATION
const dsat_string_item_type dsat_clck_fac_valstr[][8] =
{ 
#ifdef FEATURE_ATCOP_CM_SUPP
"\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", 
#endif
"\"SC\"", "\"FD\"",
  "\"PN\"", "\"PU\"", "\"PP\"", "\"PC\"", "\"PF\"", "" };

#ifdef FEATURE_ATCOP_CM_SUPP 
const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"AB\",\"AC\",\"AG\",\"AI\",\"AO\",\"IR\",\"OI\",\"OX\",\"SC\",\"FD\",\"PN\",\"PU\",\"PP\",\"PC\",\"PF\")" };
#else
const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"SC\",\"FD\",\"PN\",\"PU\",\"PP\",\"PC\",\"PF\")" };
#endif


#else
const dsat_string_item_type dsat_clck_fac_valstr[][8] =
{ 
#ifdef FEATURE_ATCOP_CM_SUPP
"\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"",
#endif
"\"SC\"", "\"FD\"","" };

#ifdef FEATURE_ATCOP_CM_SUPP 
const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"AB\",\"AC\",\"AG\",\"AI\",\"AO\",\"IR\",\"OI\",\"OX\",\"SC\",\"FD\")" };
#else
const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"SC\",\"FD\")" };
#endif

#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#else
const dsat_string_item_type dsat_clck_fac_valstr[][8] =
{ 
#ifdef FEATURE_ATCOP_CM_SUPP
"\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", 
#endif
"" };

#ifdef FEATURE_ATCOP_CM_SUPP
const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"AB\",\"AC\",\"AG\",\"AI\",\"AO\",\"IR\",\"OI\",\"OX\")" };
#else
const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "()" };
#endif
#endif /* FEATURE_DATA_ETSI_PIN */


word dsat_clck_mode_string_len = CLCK_MODE_STR_MAX_LEN;
LOCAL dsat_string_item_type clck_mode_val[CLCK_MODE_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_clck_mode_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_clck_mode_string_len
};

/* Add one to string length to test overflow */
word dsat_clck_passwd_string_len = CLCK_PASSWD_STR_MAX_LEN+1;
LOCAL dsat_string_item_type clck_passwd_val[CLCK_PASSWD_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_clck_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_clck_passwd_string_len
};

LOCAL const dflm_type dsat_clck_class_dflm =
{
  7, 1, 255
};

LOCAL const mixed_def_s_type dsat_clck_class_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_clck_class_dflm
};

LOCAL const mixed_def_s_type * dsat_clck_mixed_dfl[4] = 
{
  &dsat_clck_fac_def_val,
  &dsat_clck_mode_def_val,
  &dsat_clck_passwd_def_val,
  &dsat_clck_class_def_val
};

/* +CPOL Command */
LOCAL const dflm_type dsat_cpol_index_dflm = 
{ 0, 0, SYS_USER_PPLMN_LIST_MAX_LENGTH};

LOCAL const mixed_def_s_type dsat_cpol_index_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_index_dflm
};

LOCAL const dflm_type dsat_cpol_format_dflm = 
{ 2, 0, 2};

LOCAL const mixed_def_s_type dsat_cpol_format_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_format_dflm
};

#ifdef FEATURE_WCDMA 
LOCAL const dflm_type dsat_cpol_act3_dflm = 
{ 0, 0, 1};
#else
const dflm_type dsat_cpol_act3_dflm = 
{ 0, 0, 0};
#endif /*FEATURE_WCDMA */
LOCAL const dflm_type dsat_cpol_act_dflm = 
{ 0, 0, 1};
LOCAL const mixed_def_s_type dsat_cpol_act_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_act_dflm
};
LOCAL const mixed_def_s_type dsat_cpol_act3_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_act3_dflm
};
LOCAL word dsat_cpol_oper_string_len = CPOL_OPER_STR_MAX_LEN;
LOCAL const mixed_def_s_type dsat_cpol_oper_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpol_oper_string_len
};

#ifdef FEATURE_DSAT_LTE
/* E-UTRAN access technology */
const dflm_type dsat_cpol_act4_dflm = 
{ 0, 0, 1 };   /* 0 - tech not selected; 1 - tech selected; */

LOCAL const mixed_def_s_type dsat_cpol_act4_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_act4_dflm
};

LOCAL const mixed_def_s_type * dsat_cpol_mixed_dfl[7] = 
{
  &dsat_cpol_index_def_val,
  &dsat_cpol_format_def_val,
  &dsat_cpol_oper_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act3_def_val,
  &dsat_cpol_act4_def_val
};
#define MAX_CPOL_ARGS 7
#else

LOCAL const mixed_def_s_type * dsat_cpol_mixed_dfl[6] = 
{
  &dsat_cpol_index_def_val,
  &dsat_cpol_format_def_val,
  &dsat_cpol_oper_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act3_def_val
};
#define MAX_CPOL_ARGS 6
#endif /* FEATURE_DSAT_LTE */

/* +CPWD Command */
#define CPWD_FAC_STR_MAX_LEN 4
/* Allow for quotes on password strings */
/* String length is longest of either GSDI or MN password */ 
#ifdef FEATURE_DSAT_ETSI_MODE
#define CPWD_PASSWD_STR_MAX_LEN ((MAX(MMGSDI_PIN_MAX_LEN, MAX_PWD_CHAR))+2)
#else
#define CPWD_PASSWD_STR_MAX_LEN ((MAX_PWD_CHAR)+2)
#endif /* FEATURE_DSAT_ETSI_MODE */

 /* Required as part of lint error fixing 
    GSDI_PIN_IS_MAX would be true when GSDI_MAX_PIN_LEN
    is greater than MAX_PWD_CHAR; Based on GSDI_PIN_IS_MAX
    use either GSDI_MAX_PIN_LEN or MAX_PWD_CHAR accordingly.
NOTE: GSDI_PIN_IS_MAX doesnt have the MAX value  */
#ifdef FEATURE_DSAT_ETSI_MODE
#define GSDI_PIN_IS_MAX (MMGSDI_PIN_MAX_LEN == MAX(MMGSDI_PIN_MAX_LEN, MAX_PWD_CHAR))
#endif /* FEATURE_DSAT_ETSI_MODE */

LOCAL word dsat_cpwd_fac_string_len = CPWD_FAC_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cpwd_fac_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpwd_fac_string_len
};

/* The order of facility entries must be reflected in ss_fac_e_type */
#ifdef FEATURE_DATA_ETSI_PIN
const dsat_string_item_type dsat_cpwd_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "\"SC\"", "\"P2\"","" };

#else
const dsat_string_item_type dsat_cpwd_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "" };
#endif /* FEATURE_DATA_ETSI_PIN */


/* Add one to string length to test overflow */
LOCAL word dsat_cpwd_passwd_string_len = CPWD_PASSWD_STR_MAX_LEN+1;
#ifdef GSDI_PIN_IS_MAX
LOCAL dsat_string_item_type cpwd_old_passwd_val[MMGSDI_PIN_MAX_LEN + 2];
LOCAL dsat_string_item_type cpwd_new_passwd_val[MMGSDI_PIN_MAX_LEN + 2];
#else
LOCAL dsat_string_item_type cpwd_old_passwd_val[MAX_PWD_CHAR + 2];
LOCAL dsat_string_item_type cpwd_new_passwd_val[MAX_PWD_CHAR + 2];
#endif /* GSDI_PIN_IS_MAX */

LOCAL const mixed_def_s_type dsat_cpwd_old_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpwd_passwd_string_len
};

LOCAL const mixed_def_s_type dsat_cpwd_new_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpwd_passwd_string_len
};

LOCAL const mixed_def_s_type * dsat_cpwd_mixed_dfl[3] = 
{
  &dsat_cpwd_fac_def_val,
  &dsat_cpwd_old_passwd_def_val,
  &dsat_cpwd_new_passwd_def_val,
};

/* +CUSD Command */
LOCAL const dflm_type dsat_cusd_n_dflm =
{
  0, 0, 2
};

LOCAL const mixed_def_s_type dsat_cusd_n_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_cusd_n_dflm
};


/* Add one to string length to test overflow */
LOCAL word dsat_cusd_str_string_len = (CUSD_STR_MAX_LEN);

LOCAL const mixed_def_s_type dsat_cusd_str_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cusd_str_string_len
};

LOCAL word dsat_cusd_dcs_string_len = CUSD_DCS_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cusd_dcs_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_cusd_dcs_string_len
};

LOCAL const mixed_def_s_type * dsat_cusd_mixed_dfl[3] = 
{
  &dsat_cusd_n_def_val,
  &dsat_cusd_str_def_val,
  &dsat_cusd_dcs_def_val,
};

/* +CMER */
const dflm_type dsat_cmer_dflm [] =
{
  { 0, 0, 3 },                 /* mode */
  { 0, 0, 0 },                 /* keyp */
  { 0, 0, 0 },                 /* disp */
  { 0, 0, 1 }                  /* ind  */
};

/* +CMEC */
LOCAL const dflm_type dsat_cmec_dflm [] =
{
  { 0, 0, 2 },                 /* keyp */
  { 0, 0, 0 },                 /* disp */
  { 0, 0, 0 },                 /* ind  */
  { 0, 0, 2 }                  /* touch */  
};


/* +CIND */
/* This value list is shared for multiple indicator parameters as
 * write syntax is not supported */
LOCAL const dsat_string_item_type dsat_cind_01_valstr [][8] =
{ "0", "1", "" };

LOCAL const dsat_string_item_type dsat_cind_03_valstr [][8] =
{ "0", "1", "2", "3", "" };

LOCAL const dsat_string_item_type dsat_cind_05_valstr [][8] =
{ "0", "1", "2", "3", "4", "5", "" };

LOCAL const dsat_string_item_type dsat_cind_battchg_tststr [] =
{ "(\"battchg\",(0-5))"  };

LOCAL const dsat_string_item_type dsat_cind_signal_tststr [] =
{ "(\"signal\",(0-5))"  };

LOCAL const dsat_string_item_type dsat_cind_service_tststr [] =
{ "(\"service\",(0-1))" };

LOCAL const dsat_string_item_type dsat_cind_call_tststr [] =
{ "(\"call\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_roam_tststr [] =
{ "(\"roam\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_smsfull_tststr [] =
{ "(\"smsfull\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_packet_tststr [] =
{ "(\"GPRS coverage\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_callsetup_tststr [] =
{ "(\"callsetup\",(0-3))"  };

LOCAL const def_list_type dsat_cind_list[MAX_CIND_INDICATORS] =
{
  { 0, dsat_cind_05_valstr, dsat_cind_battchg_tststr },
  { 0, dsat_cind_05_valstr, dsat_cind_signal_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_service_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_call_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_roam_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_smsfull_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_packet_tststr },
  { 0, dsat_cind_03_valstr, dsat_cind_callsetup_tststr },
};

/* Timezone Change reporting settings */
LOCAL const dflm_type dsat_ctzr_dflm [] =
{
  {0, 0, 1}, /* defaulting timezone change reporting to "disable" */
};

LOCAL const dsat_string_item_type dsat_ctzr_tststr [] =
{
  "(0,1)"
};


LOCAL const dflm_type dsat_ctzu_dflm [] =
{
  {0, 0, 1}, /* defaulting timezone update to "disable" */
};

LOCAL const dsat_string_item_type dsat_ctzu_tststr [] =
{
  "(0,1)"
};

LOCAL const dflm_type dsat_clip_dflm [] =
{
  {0, 0, 1}, /* defaulting CLIP to "disable" */ 
}; 
LOCAL const dsat_string_item_type dsat_clip_tststr [] =
{
  "(0,1)"
};


LOCAL const dflm_type dsat_cdip_dflm [] =
{
  {0, 0, 1}, /* defaulting CDIP to "disable" */ 
}; 


LOCAL const dflm_type dsat_colp_dflm [] =
{
  {0, 0, 1}, /* defaulting COLP to "disable" */ 
}; 
LOCAL const dsat_string_item_type dsat_colp_tststr [] =
{
  "(0,1)"
};

LOCAL const dsat_string_item_type dsat_crsl_tststr [] =
{
  "(0,1,2,3,4,5)"
};

dsat_num_item_type dsat_crsl_val;

/* Ringer alert mode settings */
LOCAL const dflm_type dsat_calm_dflm [] =
{
  {0, 0, 2},
};

LOCAL const dsat_string_item_type dsat_calm_tststr [] =
{
  "(0,1,2)"
};

dsat_num_item_type dsat_calm_val;

#ifdef FEATURE_DSAT_ETSI_DATA
/* +CGDATA command */
LOCAL const dsat_string_item_type dsat_cgdata_valstr [][8] =
{ "PPP", "" };

LOCAL const dsat_string_item_type dsat_cgdata_tststr [] =
{ "(\"PPP\")" };

LOCAL def_list_type dsat_cgdata_list =
{
  0,
  dsat_cgdata_valstr,
  dsat_cgdata_tststr
};

#endif /* FEATURE_DSAT_ETSI_DATA */
LOCAL const dflm_type dsat_clir_dflm [] =
{
  {0, 0, 2}, /* defaulting CLIR to "presentation indicator" */ 
}; 

LOCAL const dsat_string_item_type dsat_clir_tststr [] =
{
  "(0,1,2)"
};

#ifdef FEATURE_MMGSDI
/* +CPLS command */


LOCAL const dflm_type dsat_cpls_dflm [] =
{
   {0, 0, 2}, /* defaulting CPLS to "PLMN Selector" */ 
};
/* +CPLS command End */
#endif /* FEATURE_MMGSDI */
/*lint +e826 */
/*lint +e708 */

dsat_num_item_type dsat_cvib_val;

/* +CPSMS Command */
const dflm_type dsat_cpsms_mode_dflm [] =
{
  { 0  , 0  , 1  },
} ;

LOCAL const mixed_def_s_type dsat_cpsms_mode_def_val =
{
  CONFIG, (mixed_def_u_type *) &dsat_cpsms_mode_dflm
};

LOCAL word dsat_cpsms_timer_string_len = CPSMS_TIMER_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cpsms_periodic_rau_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpsms_timer_string_len
};

LOCAL const mixed_def_s_type dsat_cpsms_gprs_ready_timer_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpsms_timer_string_len
};

LOCAL const mixed_def_s_type dsat_cpsms_periodic_tau_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpsms_timer_string_len
};

LOCAL const mixed_def_s_type dsat_cpsms_active_timer_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpsms_timer_string_len
};

LOCAL const mixed_def_s_type * dsat_cpsms_mixed_dflm[5] =
{
  &dsat_cpsms_mode_def_val,             /* PSM mode */
  &dsat_cpsms_periodic_rau_def_val,     /* Requested_Periodic RAU, GPRS Timer 3 */
  &dsat_cpsms_gprs_ready_timer_def_val, /* Requested GPRSTimerRead, GPRS Timer 2 */
  &dsat_cpsms_periodic_tau_def_val,     /* Requested Periodic TAU  GPRS Timer 3 */
  &dsat_cpsms_active_timer_def_val  	/* Requested Active-Time, GPRS Timer 2(coded same as GPRS Timer)  */
};

#ifdef FEATURE_DSAT_EDRX
/* +CEDRXS Command */
const dflm_type dsat_cedrxs_mode_dflm [] =
{
  { 0  , 0  , 3  },
};

LOCAL const mixed_def_s_type dsat_cedrxs_mode_def_val =
{
  CONFIG, (mixed_def_u_type *) &dsat_cedrxs_mode_dflm
};

const dflm_type dsat_cedrxs_act_dflm [] =
{
  { 1  , 1  , 5  },
};

LOCAL const mixed_def_s_type dsat_cedrxs_act_def_val =
{
  CONFIG, (mixed_def_u_type *) &dsat_cedrxs_act_dflm
};

LOCAL word dsat_cedrxs_cycle_string_len = 5;

LOCAL const mixed_def_s_type dsat_cedrxs_cycle_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cedrxs_cycle_string_len
};

LOCAL const mixed_def_s_type * dsat_cedrxs_mixed_dflm[3] =
{
  &dsat_cedrxs_mode_def_val,             /* EDRX mode */
  &dsat_cedrxs_act_def_val,              /* Requested Act type */
  &dsat_cedrxs_cycle_def_val             /* Requested EDRX value*/
};
#endif /*FEATURE_DSAT_EDRX*/

/* +CRTDCP */
LOCAL const dflm_type dsat_crtdcp_dfl[1] =
{ 
  { 0, 0, 1 },
  /* 0 - disable, 1 - enable */
};

/*--------------------------------------------------------------------------
                         ETSI Command Tables 
--------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_ETSI_MODE
/* ETSI extended AT command table */
const dsati_cmd_type dsatetsi_ext_action_table [] =
{
  { "+CSQ",    READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CSQ_ETSI_IDX,    NULL, dsatetsime_exec_csq_cmd},

#ifdef FEATURE_DATA_ETSI_PIN 
  { "+CPIN",    EXTENDED | CONFIG | MIXED_PARAM | LIST | 
               LOCAL_TEST | RESTRICTED | UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,   2,DSATETSI_EXT_ACT_CPIN_ETSI_IDX, &dsat_cpin_mixed_dfl[0], dsatetsime_exec_cpin_cmd},
#endif /* FEATURE_DATA_ETSI_PIN */

  { "+CMER",    EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   4, DSATETSI_EXT_ACT_CMER_ETSI_IDX,   &dsat_cmer_dflm[0],  dsatetsime_exec_cmer_cmd},


/* SMS Commands */
#ifdef FEATURE_ETSI_SMS
  { "+CPMS",    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST| UMTS_CMD,
    SPECIAL_NONE,    3, DSATETSI_EXT_ACT_CPMS_ETSI_IDX, &dsat_cpms_list[0], dsatetsisms_exec_cpms_cmd},
  
  { "+CNMI",    EXTENDED | CONFIG | LIST | LOCAL_TEST| UMTS_CMD,
    SPECIAL_NONE,    5,DSATETSI_EXT_ACT_CNMI_ETSI_IDX,   &dsat_cnmi_list[0],  dsatetsisms_exec_cnmi_cmd},
  
  { "+CMGL",    EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CMGL_ETSI_IDX, NULL,  dsatetsisms_exec_cmgl_cmd},
  
  { "+CMGR",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CMGR_ETSI_IDX, NULL,  dsatetsisms_exec_cmgr_cmd},
  
  { "+CMGS",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CMGS_ETSI_IDX, NULL,  dsatsms_exec_sms_cmd},
  
  { "+CMSS",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,	0,DSATETSI_EXT_ACT_CMSS_ETSI_IDX, NULL,  dsatetsisms_exec_cmss_cmd},
  
  { "+CMGW",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_ACT_CMGW_ETSI_IDX, NULL,  dsatsms_exec_sms_cmd},
  
  { "+CMGD",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CMGD_ETSI_IDX,  NULL,  dsatetsisms_exec_cmgd_cmd},
  
  { "+CMGC",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_ACT_CMGC_ETSI_IDX,  NULL,  dsatetsisms_exec_cmgc_cmd},
  
  { "+CNMA",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    2,DSATETSI_EXT_ACT_CNMA_ETSI_IDX,  NULL,  dsatetsisms_exec_cnma_cmd},
  
  { "+CMMS",    EXTENDED | CONFIG| UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CMMS_ETSI_IDX,  &dsat_cmms_dflm,  dsatetsisms_exec_cmms_cmd},
#endif /* FEATURE_ETSI_SMS */

  { "+COPS",    ABORT_CMD | EXTENDED | LOCAL_TEST | CONFIG | LIST | MIXED_PARAM | UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,    4,DSATETSI_EXT_ACT_COPS_ETSI_IDX, &dsat_cops_mixed_dfl[0],  dsatetsicall_exec_cops_cmd},

  { "+CIMI",     READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CIMI_ETSI_IDX, NULL,  dsatetsime_exec_cimi_cmd},

  { "+CGMI",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGMI_ETSI_IDX,   NULL,  dsatact_exec_gmi_cmd},

  { "+CGMM",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGMM_ETSI_IDX,   NULL,  dsatact_exec_gmm_cmd},

  { "+CGMR",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGMR_ETSI_IDX,    NULL,  dsatact_exec_gmr_cmd},

  { "+CGSN",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGSN_ETSI_IDX,    NULL,  dsatact_exec_gsn_cmd},

  { "+CLAC",      READ_ONLY | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CLAC_ETSI_IDX,     NULL,  dsatetsime_exec_clac_cmd},

#ifdef FEATURE_DSAT_LTE
  { "+CVMOD",    ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CVMOD_ETSI_IDX,  NULL,  dsatetsicall_exec_cvmod_cmd},
#endif /*FEATURE_DSAT_LTE*/

#ifdef FEATURE_DSAT_ETSI_DATA
  { "+CGATT",
    EXTENDED | CONFIG | LIST | LOCAL_TEST | NO_RESET| UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_CGATT_ETSI_IDX,    &dsat_cgatt_list,  dsatetsipkt_exec_cgatt_cmd},

  { "+CGACT",
    EXTENDED | CONFIG | LIST | LOCAL_TEST | NO_RESET | MULTI_INDEX | UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_CGACT_ETSI_IDX,   &dsat_cgact_list, dsatetsipkt_exec_cgact_cmd},
#endif /* FEATURE_DSAT_ETSI_DATA */

  { "+CPAS",    READ_ONLY | RESTRICTED | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CPAS_ETSI_IDX ,  NULL,dsatetsime_exec_cpas_cmd},

#ifdef FEATURE_ETSI_PBM
  { "+CPBS",    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CPBS_ETSI_IDX, &dsat_cpbs_list, dsatme_exec_cpbs_cmd},

  { "+CPBW",	  ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,    10,	DSATETSI_EXT_ACT_CPBW_ETSI_IDX,NULL,  dsatme_exec_cpbw_cmd },
#endif // FEATURE_ETSI_PBM

  { "+CLCK",    ABORT_CMD | EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,    4,DSATETSI_EXT_ACT_CLCK_ETSI_IDX, &dsat_clck_mixed_dfl[0],  dsatetsicall_exec_clck_cmd },

  { "+CPWD",    EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_ACT_CPWD_ETSI_IDX, &dsat_cpwd_mixed_dfl[0],  dsatetsicall_exec_cpwd_cmd},

#ifdef FEATURE_ETSI_PBM
  { "+CNUM",     READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CNUM_ETSI_IDX,   NULL,  dsatetsime_exec_cnum_cmd },
#endif /* FEATURE_ETSI_PBM */

#ifdef FEATURE_MMGSDI_ATCSIM
  { "+CSIM",     EXTENDED | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   2,DSATETSI_EXT_ACT_CSIM_ETSI_IDX,   NULL,  dsatetsime_exec_csim_cmd },

  { "+CRSM",     EXTENDED | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   6,DSATETSI_EXT_ACT_CRSM_ETSI_IDX,    NULL,  dsatetsime_exec_crsm_cmd },
#endif /* FEATURE_MMGSDI_ATCSIM */

  { "+CCLK",    EXTENDED | COMMON_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CCLK_ETSI_IDX,  NULL,  dsatetsime_exec_cclk_cmd},
    
#ifndef FEATURE_DSAT_CUST
  { "+COPN",   ABORT_CMD | ATTRIB_NONE | UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_COPN_ETSI_IDX,  NULL,  dsatetsicall_exec_copn_cmd},
#endif /* FEATURE_DSAT_CUST */

#ifdef FEATURE_MMGSDI
  { "+CPOL",    ABORT_CMD | EXTENDED | LOCAL_TEST | CONFIG | LIST | MIXED_PARAM| UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE, MAX_CPOL_ARGS,DSATETSI_EXT_ACT_CPOL_ETSI_IDX,  &dsat_cpol_mixed_dfl[0],  dsatetsime_exec_cpol_cmd },

  { "+CPLS",     EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_CPLS_ETSI_IDX,  &dsat_cpls_dflm[0],  dsatetsime_exec_cpls_cmd},
#endif /* FEATURE_MMGSDI */

  { "+CTZR",     EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CTZR_ETSI_IDX,  &dsat_ctzr_dflm[0],  dsatparm_exec_param_cmd},
    
  { "+CTZU",     EXTENDED | CONFIG | LOCAL_TEST | NO_RESET | COMMON_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_CTZU_ETSI_IDX, &dsat_ctzu_dflm[0],  dsatetsime_exec_ctzu_cmd},

  { "+CUAD",	   EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CUAD_ETSI_IDX,  NULL,  dsatetsime_exec_cuad_cmd},

#ifdef FEATURE_DATA_MUX
  { "+CMUX",    EXTENDED | LOCAL_TEST,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CMUX_ETSI_IDX,   NULL,  dsatact_exec_cmux_cmd},
#endif /* FEATURE_DATA_MUX */
  
  { "+CSDF",	  ATTRIB_NONE,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CSDF_ETSI_IDX,	 NULL,  dsatetsime_exec_csdf_cmd},

  { "+CSTF",	  ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CSTF_ETSI_IDX,	 NULL,  dsatetsime_exec_cstf_cmd},

  { "+CUSD",
    EXTENDED | CONFIG | LOCAL_TEST | LIST | MIXED_PARAM | MULTI_INDEX | UMTS_CMD,
    SPECIAL_NONE,    3, DSATETSI_EXT_ACT_CUSD_ETSI_IDX,  &dsat_cusd_mixed_dfl[0],  dsatetsicall_exec_cusd_cmd},

  { "+VTS", 	EXTENDED | LOCAL_TEST | UMTS_CMD,
	 SPECIAL_NONE,	  1,DSATETSI_EXT_ACT_VTS_ETSI_IDX,	NULL, dsatetsicall_exec_vts_cmd},

#ifdef FEATURE_MMGSDI
  { "+ICCID",     READ_ONLY | RESTRICTED | UMTS_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_ICCID_ETSI_IDX, NULL,  dsatetsime_exec_iccid_cmd },
#endif /* FEATURE_MMGSDI */

  { "+CPSMS",	   EXTENDED | CONFIG | MIXED_PARAM | LIST | UMTS_CMD,
	  SPECIAL_NONE, 5, DSATETSI_EXT_ACT_CPSMS_ETSI_IDX, &dsat_cpsms_mixed_dflm[0],dsatetsime_exec_cpsms_cmd}

#ifdef FEATURE_DSAT_EDRX
  , { "+CEDRXS",   EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST,
	  SPECIAL_NONE, 3, DSATETSI_EXT_ACT_CEDRXS_ETSI_IDX, &dsat_cedrxs_mixed_dflm[0],dsatetsime_exec_cedrxs_cmd}

  , { "+CEDRXRDP",   EXTENDED | READ_ONLY | YES_QUOTE,
	  SPECIAL_NONE, 0, DSATETSI_EXT_ACT_CEDRXRDP_ETSI_IDX, NULL,  dsatetsime_exec_cedrxrdp_cmd},
#endif /* FEATURE_DSAT_EDRX */
  { "+CHUP",	   ATTRIB_NONE | RESTRICTED |  UMTS_CMD | NOT_FORWARDABLE,
	  SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CHUP_ETSI_IDX,	NULL ,dsatetsicall_exec_chup_cmd},

  };
#endif /* FEATURE_DSAT_ETSI_MODE */


const dsati_cmd_type dsatetsi_ext_table [] =
{
#ifdef FEATURE_DSAT_LTE

  { "+CEREG",    EXTENDED | CONFIG | BROKEN_RANGE | LOCAL_TEST| UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,    1,DSATETSI_EXT_CEREG_IDX,   dsat_cereg_dfl, dsatetsicall_exec_creg_cmd  },

  { "+CEMODE",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CEMODE_IDX,        &dsat_cemode_dflm[0], dsatetsipkt_exec_cemode_cmd},
#endif /* FEATURE_DSAT_LTE */

  { "+CSCS",    EXTENDED | CONFIG | LIST | LOCAL_TEST | YES_QUOTE | COMMON_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,   1,DSATETSI_EXT_CSCS_IDX,   &dsat_cscs_list,dsatparm_exec_param_cmd},

  { "+CRC",    EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,   1,DSATETSI_EXT_CRC_IDX,    &dsat_crc_list,dsatparm_exec_param_cmd},

#ifdef FEATURE_DSAT_ETSI_DATA
  { "+CGDCONT",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   7,DSATETSI_EXT_CGDCONT_IDX,   &dsat_cgdcont_mixed_dfl[0],dsatetsipkt_exec_cgdcont_cmd},
  { "+CGEREP",
    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   2, DSATETSI_EXT_CGEREP_IDX,	 &dsat_cgerep_dflm[0],dsatetsipkt_exec_cgerep_cmd},

#ifdef FEATURE_DSAT_LTE
  { "+CGCONTRDP",   ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_CGCONTRDP_IDX, NULL,dsatetsipkt_exec_cgcontrdp_cmd},
#endif /* FEATURE_DSAT_LTE */

  { "+CGPADDR",	ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_CGPADDR_IDX,  NULL,dsatetsipkt_exec_cgpaddr_cmd},
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_ETSI_SMS
#ifdef FEATURE_ETSI_SMS_PS
	{ "+CGSMS",   EXTENDED | CONFIG | LOCAL_TEST | NO_RESET| UMTS_CMD,
	  SPECIAL_NONE,    1, DSATETSI_EXT_CGSMS_IDX,		 &dsat_cgsms_dflm, dsatetsisms_exec_cgsms_cmd},
#endif /* FEATURE_ETSI_SMS_PS */

  { "+CSMS",    EXTENDED | CONFIG | LOCAL_TEST| UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CSMS_IDX,        &dsat_csms_dflm[0],dsatetsisms_exec_csms_cmd },

  { "+CMGF",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CMGF_IDX,   &dsat_cmgf_dflm[0],  dsatparm_exec_param_cmd},

  { "+CSAS",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CSAS_IDX,        &dsat_csas_dflm[0], dsatetsisms_exec_csas_cmd},

  { "+CRES",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CRES_IDX,      &dsat_cres_dflm[0], dsatetsisms_exec_cres_cmd},

  { "+CSCA",    EXTENDED | CONFIG | MIXED_PARAM | LIST | NO_RESET| UMTS_CMD,
    SPECIAL_NONE,    2, DSATETSI_EXT_CSCA_IDX,       &dsat_csca_mixed_dfl[0], dsatsms_exec_sms_cmd},

  { "+CSMP",    EXTENDED | CONFIG | MIXED_PARAM | LIST | UMTS_CMD,
    SPECIAL_NONE,    4,DSATETSI_EXT_CSMP_IDX ,  &dsat_csmp_mixed_dfl[0], dsatsms_exec_sms_cmd},

  { "+CVHU",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CVHU_IDX, &dsat_cvhu_dflm[0], dsatparm_exec_param_cmd },

#endif /* FEATURE_ETSI_SMS */
 
  { "+CREG",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,    1,DSATETSI_EXT_CREG_IDX,    dsat_creg_dfl,dsatetsicall_exec_creg_cmd},

  { "+CGREG",    EXTENDED | CONFIG | BROKEN_RANGE | LOCAL_TEST| UMTS_CMD | NOT_FORWARDABLE,
    SPECIAL_NONE,    1, DSATETSI_EXT_CGREG_IDX,    dsat_cgreg_dfl, dsatetsicall_exec_creg_cmd},

#ifdef FEATURE_DSAT_ETSI_DATA
  { "+CGPIAF",EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   4, DSATETSI_EXT_CGPIAF_IDX, &dsat_cgpiaf_dflm[0], dsatparm_exec_param_cmd },
#endif //FEATURE_DSAT_ETSI_DATA

#ifdef FEATURE_ETSI_SMS
  { "+CSDH",	  EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CSDH_IDX,    &dsat_csdh_dflm[0], dsatparm_exec_param_cmd},
#endif //FEATURE_ETSI_SMS

  { "+CSODCP",    EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_CSODCP_IDX,   NULL, dsatetsipkt_exec_csodcp_cmd },

  { "+CRTDCP",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CRTDCP_IDX,    dsat_crtdcp_dfl, dsatetsipkt_exec_crtdcp_cmd }

} ;


/* Command table sizes. */
const unsigned int dsatetsi_ext_table_size = ARR_SIZE( dsatetsi_ext_table );
const unsigned int dsatetsi_ext_action_table_size = ARR_SIZE( dsatetsi_ext_action_table );

