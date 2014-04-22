/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               C O M M O N   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the AT modem commands that are common across all modes of
  operation (ETSI, CDMA2000, IS-95, etc.).

EXTERNALIZED FUNCTIONS
  dsatctab_data_init
    This function initializes the AT command defaults, limits, and lists
    that depend on a run-time determination of the hardware.

    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2014-2021 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatctab_ex.c#6 $ $DateTime: 2021/10/05 04:54:10 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/30/21   ks      Added support for $QCDLTPTENH command.
01/25/21   ks      Added support for FSK related AT commands.
08/11/20   sp      Added support for $QCMSISDN command.
05/22/20   ks      Added support for $QCRRINFO command.
11/13/19   sp      Added support for AT$QCRPMINFO commad
01/08/17   skc     Fixed issue with AT+CLAC where +IPR was displaying twice.
11/17/17   skc     Fixed AT$QCSIMSTAT test command display issue.
07/12/17   skc     Enabled AT$QCPRFMOD for IOE Build.
04/17/17   skc     Enabled requested AT Commands for IOE Build.
01/27/17   skc     Enabled AT$QCPDPIMSCFGE for IOE build.
01/27/17   skc     Enabled AT$QCDGEN support in IOE build.
01/09/17   skc     Featurizing AT$QCRMCALL command.
10/18/16   skc     Fixed issue related to AT+CLAC
09/21/16   skc     Fixed issue with AT Command +IPR and $QCTER.
04/07/15   tk      Fixed issue in getting QCSIAMPP value.
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
11/24/14   pg      Fixed ws46 featurization issue.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "target.h"
#include "dsati.h"
#include "msg.h"
#include "dsatparm.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatme.h"
#include "dsatvend.h"
#include "dstaski.h"
#include "nv.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsictab.h"
#ifdef FEATURE_DSAT_DEV_CMDS
#include "dsatetsicall.h"
#endif /* FEATURE_DSAT_DEV_CMDS */
#include "dsatme.h"
#include "dsatcmif.h"
#ifdef FEATURE_ETSI_SMS
#include "dsatetsisms.h"
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_DATA_ETSI_PIN
#include "dsatetsime.h"
#endif /* FEATURE_DATA_ETSI_PIN */
#endif /* FEATURE_DSAT_ETSI_MODE */
#include "ds_rmnet_defs.h"

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  NV interface data 
---------------------------------------------------------------------------*/
//nv_item_type         ds_nv_item;                  /* actual data item     */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/

dsat_qcsimapp_table_s_type dsat_qcsimapp_table;
dsat_qcsimapp_info_s_type  dsat_qcsimapp_info[DSAT_APPS_ID_MAX];

boolean dsatme_pending_mmgsdi_command[DSAT_APPS_ID_MAX] = {FALSE,FALSE,FALSE};

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* +CPBS Command */
/* the sequence of the storage type is important, it has to be the same as
   that of dsatetsi_pb_pbm_table 
   DEFAULT_CPB_STORAGE is SM, we use macro here to synchronize it with 
   me_cpbs_default */
#ifdef FEATURE_PBM_USE_EFS_PB
const dsat_string_item_type dsat_cpbs_storage_valstr [][8] =
{ DEFAULT_CPB_STORAGE, "DC", "FD", "LD","MC",DEFAULT_NV_CPB_STORAGE, "RC", "EN","ON",""};

LOCAL const dsat_string_item_type dsat_cpbs_storage_tststr [] =
{ "(\"SM\",\"DC\",\"FD\",\"LD\",\"MC\",\"ME\",\"RC\",\"EN\",\"ON\")" };

#else
const dsat_string_item_type dsat_cpbs_storage_valstr [][8] =
{ DEFAULT_CPB_STORAGE, "DC", "FD", "LD","MC", "RC", "EN","ON",""};

LOCAL const dsat_string_item_type dsat_cpbs_storage_tststr [] =
{ "(\"SM\",\"DC\",\"FD\",\"LD\",\"MC\",\"RC\",\"EN\",\"ON\")" };

#endif/*FEATURE_PBM_USE_EFS_PB*/

const def_list_type dsat_cpbs_list =
{ 0, dsat_cpbs_storage_valstr, dsat_cpbs_storage_tststr };
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

LOCAL const dflm_type dsat_amp_c_dflm [] =
{
  { 2  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_amp_d_dflm [] =
{
  { 2  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_amp_f_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat_amp_s_dflm [] =
{
  { 0 , 0 , 1 },
} ;

LOCAL const dflm_type dsat_icf_dflm [] =
{
  { 3  , 3  , 3    },
  { 3  , 0  , 3    },
} ;
LOCAL const dflm_type dsat_ifc_dflm [] =
{
  { 2  , 0  , 3    },
  { 2  , 0  , 2    },
} ;

/*  The bit rates in the "valstr" table must match exactly the  */
/*  value and order of the "sio_baudrate_type" enum in sio.h    */
/*  Unused values must have space strings as placeholders       */
/*  The "valstr" table must be terminated with a null string    */

/*  Bit rates for MSM 2.3 and later                             */
/* Applies to ETSI & IS707 modes */  
 const dsat_string_item_type dsat_ipr_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", "300", "600", "1200","2400",
    "4800", "9600", " ", "19200", "38400", "57600", " ", "115200",
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400", " ", "921600", "1000000", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ", " ",
    "2000000", "2900000", "3000000", "3200000", "3686400", "4000000",
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
    "" };

LOCAL const dsat_string_item_type dsat_ipr_tststr [] =
  { "(),"
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,921600,1000000,2000000,2900000,3000000,3200000,3686400,4000000)"
#else
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200)"
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
  };

/* Initialized at powerup by dsatctab_data_init().This initialization to 
MSM 2.2 is "just in case". */
LOCAL def_list_type dsat_ipr_list =
{
  17,  /*  115200 */
  dsat_ipr_valstr,  
  dsat_ipr_tststr
} ;

LOCAL const dflm_type dsat_e_dflm [] =
{
  { 1  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_l_dflm [] =
{
  { 0  , 0  , 3    },
} ;

LOCAL const dflm_type dsat_m_dflm [] =
{
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_q_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_s0_dflm [] =
{
  { 0  , 0  , 255  },
} ;

/* V.25 defines a range of 1 - 154            */
/* IS707 has a range of 1 - 255, IS-707 Wins! */
LOCAL const dflm_type dsat_s10_dflm [] =
{
  { 14 , 1  , 255  },
} ;

LOCAL const dflm_type dsat_s11_dflm [] =
{
  { 95 , 50 , 255  },
} ;

LOCAL const dflm_type dsat_s2_dflm [] =
{
  { 43 , 0 , 127 },
} ;

LOCAL const dflm_type dsat_o_dflm[]=
{
  { 0, 0, 2 },
};

LOCAL const dflm_type dsat_s3_dflm [] =
{
  { 13 , 0  , 127  },
} ;
LOCAL const dflm_type dsat_s4_dflm [] =
{
  { 10 , 0  , 127  },
} ;

LOCAL const dflm_type dsat_s5_dflm [] =
{
  { 8  , 0  , 127  },
} ;

LOCAL const dflm_type dsat_x_dflm [] =
{
  { 1  , 0  , 4    },
} ;

LOCAL const dflm_type dsat_s6_dflm [] =
{
  { 2  , 2  , 10   },
} ;

LOCAL const dflm_type dsat_s7_dflm [] =
{
  { 0 , 0 , 255  },/* 0 - Default - Will not Start the timer  */
} ;

LOCAL const dflm_type dsat_s8_dflm [] =
{
  { 2  , 0  , 255  },
} ;

LOCAL const dflm_type dsat_v_dflm [] =
{
  { 1  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_z_dflm [] =
{
  { 0  , 0  , 0    },
} ;

/* +CMEE Command */
LOCAL const dsat_string_item_type dsat_cmee_valstr [][8] =
{ "0", "1", "2", ""};

LOCAL const dsat_string_item_type dsat_cmee_tststr [] =
{ "(0,1,2)" };

const def_list_type dsat_cmee_list =
{ 2, dsat_cmee_valstr, dsat_cmee_tststr };

#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/* $CREG Command */
LOCAL const dflm_type dsat_qccreg_dfl[] =
{ 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  { 0, 0, 2 },    /* 0 - disable, 1 - enable, 
     2 - enable with location, cell id and PSC if present */
#else
  { 0, 0, 1 },     /* 0 - disable, 1 - enable */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
};
#endif /* ( FEATURE_DSAT_ETSI_MODE) */
#endif /* FEATURE_DSAT_DEV_CMDS */
#ifdef FEATURE_TTY
LOCAL const dflm_type dsat_qcctm_dflm [] =
{
  { 0  ,  0  , 1    }
};
#endif/* FEATURE_TTY */

#ifdef FEATURE_DSAT_ETSI_MODE
/* +WS46 Command */
#ifdef FEATURE_DSAT_LTE
LOCAL const dsat_string_item_type dsat_ws46_valstr [][8] =
  { "12", "28", "30", "" };
LOCAL const dsat_string_item_type dsat_ws46_tststr [] =
  { "(12,28,30)" };
#else
LOCAL const dsat_string_item_type dsat_ws46_valstr [][8] =
  { "12", "" };
LOCAL const dsat_string_item_type dsat_ws46_tststr [] =
  { "(12)" };
#endif /* defined ( FEATURE_DSAT_LTE ) || defined( FEATURE_WCDMA ) */
const def_list_type dsat_ws46_list[] =
{
  { 0, dsat_ws46_valstr, dsat_ws46_tststr },
};
#endif /* FEATURE_DSAT_ETSI_MODE */
/* $QCSIMAPP */
LOCAL const dflm_type dsat_qcsimapp_dflm [] =
{
  { 0, 0, 2 }
} ;

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_ATCOP_VEND

/* $QCSIMISTAT Command */
LOCAL const dflm_type dsat_qcsimstat_dflm [] =
{
  { 0, 0, 1 }
};
#endif /* FEATURE_DSAT_ETSI_MODE*/
#endif

#ifdef FEATURE_DSAT_ETSI_DATA
#ifdef FEATURE_DSAT_ATCOP_VEND

/* $QCPDPP Command */
LOCAL const dflm_type dsat_qcpdpp_cid_dflm [] =
{
  { DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MAX }
} ;

LOCAL const dflm_type dsat_qcpdpp_auth_dflm [] =
{
  { 0  , 0  , 3  },
} ;


/*  Use over-long strings to validate max permitted length */
/* Accomodate quotes in addition to DS_MAX_QCPDP_STRING_LEN characters */
LOCAL const mixed_def_u_type dsat_qcpdpp_uname_string_len = 
                                            {{DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+5}};
LOCAL const mixed_def_u_type dsat_qcpdpp_passw_string_len = 
                                            {{DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+5}};



LOCAL const mixed_def_s_type dsat_qcpdpp_cid_def_val =
{
  CONFIG | LOCAL_TEST , (mixed_def_u_type *) dsat_qcpdpp_cid_dflm
};

LOCAL const mixed_def_s_type dsat_qcpdpp_auth_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_qcpdpp_auth_dflm
};

LOCAL const mixed_def_s_type dsat_qcpdpp_passw_def_val =
{
  CONFIG | STRING | NO_DISPLAY | NO_QUOTE,
  (mixed_def_u_type *) &(dsat_qcpdpp_passw_string_len)
};

LOCAL const mixed_def_s_type dsat_qcpdpp_uname_def_val =
{
  CONFIG | STRING | YES_QUOTE,
  (mixed_def_u_type *) &(dsat_qcpdpp_uname_string_len)
};

/*lint -save -e708 */
LOCAL const mixed_def_s_type * dsat_qcpdpp_mixed_dfl[4] = 
{
  &dsat_qcpdpp_cid_def_val,
  &dsat_qcpdpp_auth_def_val,
  &dsat_qcpdpp_passw_def_val,
  &dsat_qcpdpp_uname_def_val
};
#endif
/*lint -restore */
/*lint -restore */


LOCAL const dflm_type dsat_qcpdplt_dflm [] =
{
  { QCPDPLT_DEF_VAL, 0, 1 },
} ;

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/* *CNTI command*/
LOCAL const dflm_type dsat_qccnti_dflm [] =
{
  { 0 , 0 , 2 } /* 0 -- default indicates Current Technology */
};
dsati_cnti_tech_enum_type dsat_qccnti_tech;
#endif /* FEATURE_DSAT_ETSI_MODE*/
#endif /* FEATURE_DSAT_DEV_CMDS */
/* ^PREFMODE variables  */
LOCAL const dflm_type dsat_prefmode_dfl[] =
{
  {0,0,8}  /* Defaulting ^PREFMODE to Automatic 
              But valid values are 0 - Automatic, 2-CDMA, 4-HDR, 8-HYBRID */
};

LOCAL const dflm_type dsat_dsci_dflm [] =
{
  { 0  , 0  , 1  },
} ;

/* +CFUN Command */
/* Functionality */
LOCAL const dflm_type dsat_cfun_fun_dfl[] =
{ 
  { 1, 0, 1 },
#if defined(FEATURE_DSAT_EXTENDED_CMD)
  { 1, 4, 6 },
#else
  { 1, 4, 7 },
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
  { MAX_BROKEN_RANGE, 0, 0 }
};

LOCAL const mixed_def_s_type dsat_cfun_fun_def_val =
{
  CONFIG | NO_RESET | NO_DISPLAY | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_cfun_fun_dfl
};

/* Note: reset parameter value of 1 is only supported for full
   functionality */
LOCAL const dflm_type dsat_cfun_rst_dfl[] =
{
  { 0, 0, 1 }
};

LOCAL const mixed_def_s_type dsat_cfun_rst_def_val =
{
  CONFIG | NO_RESET | NO_DISPLAY | LOCAL_TEST,
  (mixed_def_u_type *) dsat_cfun_rst_dfl
};

LOCAL const mixed_def_s_type * dsat_cfun_mixed_dfl[2] = 
{
  &dsat_cfun_fun_def_val,
  &dsat_cfun_rst_def_val
};

/*^MODE Command*/
LOCAL const dflm_type dsat_mode_dflm[] =
{
  { 0, 0, 1},
};

#ifdef FEATURE_DSAT_LTE
/*$QCRATSTATE Command*/
#ifdef FEATURE_DSAT_ATCOP_VEND
LOCAL const dflm_type dsat_qcratstate_dflm[] =
{
  { 0, 0, 1},
};

/*$QCEXTCLTSTATE Command*/
LOCAL const dflm_type dsat_qcextcltstate_dflm[] =
{
  { 0, 0, 1},
};
#endif
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DSAT_ATCOP_VEND

/*$QCJDCFG Command*/
LOCAL const dflm_type dsat_qcjdcfg_dflm[] =
{
  { 0, 0, 1},
};

/*$QCJDSTATE Command*/
LOCAL const dflm_type dsat_qcjdstate_dflm[] =
{
  { 0, 7, 9},
};

#endif

#ifdef FEATURE_DSAT_ATCOP_VEND
/*$QCCEINFO Command*/
LOCAL const dflm_type dsat_qcceinfo_dflm[] =
{
  { 0, 0, 1},
};

#ifdef FEATURE_FSK_TX
/* $QCFSKDBG Command */
LOCAL const dflm_type dsat_qcfskdbg_freq_dfl[] =
{ 
  { 0, 902000, 928000 },
  { 0, 966037, 967957 },
  { MAX_BROKEN_RANGE, 0, 0 }
};

LOCAL const mixed_def_s_type dsat_qcfskdbg_freq_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_qcfskdbg_freq_dfl
};

LOCAL const dsat_string_item_type dsat_qcfskdbg_delta_valstr [][8] =
{
  "1", "2", "3", "5", ""
};

LOCAL const dsat_string_item_type dsat_qcfskdbg_delta_tststr [] =
{ "(1,2,3,5)" };

LOCAL const def_list_type dsat_qcfskdbg_delta_list =
{
  2, dsat_qcfskdbg_delta_valstr, dsat_qcfskdbg_delta_tststr
};

LOCAL const mixed_def_s_type dsat_qcfskdbg_delta_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_qcfskdbg_delta_list
};

LOCAL const dflm_type dsat_qcfskdbg_baudrate_dfl[] =
{
  { 1, 0, 2 }
};

LOCAL const mixed_def_s_type dsat_qcfskdbg_baudrate_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) dsat_qcfskdbg_baudrate_dfl
};

LOCAL const mixed_def_s_type * dsat_qcfskdbg_dflm[3] = 
{
  &dsat_qcfskdbg_freq_def_val,
  &dsat_qcfskdbg_delta_def_val,
  &dsat_qcfskdbg_baudrate_def_val
};

/*$QCFSKDATA Command*/
LOCAL const dflm_type dsat_qcfskdata_dflm[] =
{
  { 0, 1, 200}          /* bytestosend */
};

/* $QCFSKSTART Command */
LOCAL const dflm_type dsat_qcfskstart_dflm[] =
{
  { 1, 0, 2 },                 /* baudrate */
  { 0, 0, 1024 },   /* numberofbitsgaptime */
  { 0, 0, 1 }               /* restartmode */
};

/* $QCFSKSTOP Command */
LOCAL const dflm_type dsat_qcfskstop_dflm[] =
{
  { 0, 0, 2 },                 /* mode */
};
#endif /* FEATURE_FSK_TX */
#endif
/* $QCCOPS Command*/
extern const mixed_def_s_type * dsat_cops_mixed_dfl[];

/*--------------------------------------------------------------------------
                          Common Command Tables 
--------------------------------------------------------------------------*/

/* Basic common AT command table. */
const dsati_cmd_type dsat_basic_table [] =
{
  { "&C",       CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_BASIC_N_C_IDX,	&dsat_amp_c_dflm[0],dsatparm_exec_param_cmd},

  { "&D",       CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE,1,DSAT_BASIC_N_D_IDX,	&dsat_amp_d_dflm[0],dsatparm_exec_param_cmd},

  { "&F",       CONFIG | SIMPLE_DEC | RESTRICTED | COMMON_CMD,
    SPECIAL_AMP_F, 1,DSAT_BASIC_N_F_IDX,   &dsat_amp_f_dflm[0],dsatact_exec_ampf_cmd},

  { "&V",       ATTRIB_NONE | COMMON_CMD,
    SPECIAL_AMP_V, 0 ,DSAT_BASIC_N_V_IDX,   NULL,dsatparm_exec_ampv_cmd},

  { "Q",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED | COMMON_CMD|NOT_PROVISIONED,
    SPECIAL_NONE,   1, DSAT_BASIC_Q_IDX ,       &dsat_q_dflm[0],dsatparm_exec_param_cmd},

  { "V",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE, 1, DSAT_BASIC_V_IDX,       &dsat_v_dflm[0], dsatparm_exec_param_cmd},

  { "X",        CONFIG | SIMPLE_DEC | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE, 1, DSAT_BASIC_X_IDX,     &dsat_x_dflm[0], dsatparm_exec_param_cmd},

  { "Z",        CONFIG | SIMPLE_DEC | RESTRICTED | COMMON_CMD,
    SPECIAL_Z,   1, DSAT_BASIC_Z_IDX,       &dsat_z_dflm[0], dsatact_exec_atz_cmd},

  { "&S",       CONFIG | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_BASIC_N_S_IDX,   &dsat_amp_s_dflm[0], dsatact_exec_amps_cmd},

#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  { "E",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE,1,DSAT_BASIC_E_IDX,&dsat_e_dflm[0], dsatact_exec_ate_cmd},
#else
  { "E",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE,  1 , DSAT_BASIC_E_IDX ,   &dsat_e_dflm[0], dsatact_exec_ate_cmd},
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

  { "I",        READ_ONLY | RESTRICTED|NOT_PROVISIONED | COMMON_CMD ,
    SPECIAL_NONE,    0,   DSAT_BASIC_I_IDX,  NULL, dsatact_exec_ati_cmd},

{ "L",        CONFIG | SIMPLE_DEC | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
  SPECIAL_NONE,    1,DSAT_BASIC_L_IDX,    &dsat_l_dflm[0], dsatparm_exec_param_cmd},

{ "M",        CONFIG | SIMPLE_DEC | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
  SPECIAL_NONE,    1,DSAT_BASIC_M_IDX,    &dsat_m_dflm[0], dsatparm_exec_param_cmd},
} ;

/* Basic common action command table. */
const dsati_cmd_type dsat_basic_action_table [] =
{
  { "A",       ABORT_CMD | ATTRIB_NONE | COMMON_CMD , SPECIAL_NONE,0,
       DSAT_BASIC_ACT_A_IDX,NULL , dsatact_exec_ata_cmd},

  { "D",       ABORT_CMD | RESTRICTED | NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_ATD,    0,DSAT_BASIC_ACT_D_IDX,  NULL, dsatact_exec_atd_cmd },

  { "H",       ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,    0,DSAT_BASIC_ACT_H_IDX,NULL, dsatact_exec_ath_cmd },

  { "O",       CONFIG | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_BASIC_ACT_O_IDX,   &dsat_o_dflm[0], dsatact_exec_ato_cmd }
};

/* Common S-register command table. */
const dsati_cmd_type dsat_sreg_table [] =
{

  { "S0",       CONFIG | COMMON_CMD | RESTRICTED|NOT_PROVISIONED,
  SPECIAL_NONE,    1,DSAT_SREG_S0_IDX,  &dsat_s0_dflm[0], dsatparm_exec_szero_cmd},

  { "S4",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S4_IDX,  &dsat_s4_dflm[0], dsatparm_exec_param_cmd},

  { "S5",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S5_IDX,  &dsat_s5_dflm[0], dsatparm_exec_param_cmd},

  { "S6",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S6_IDX, &dsat_s6_dflm[0], dsatparm_exec_param_cmd},

  { "S7",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S7_IDX,  &dsat_s7_dflm[0], dsatparm_exec_param_cmd},

  { "S8",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S8_IDX, &dsat_s8_dflm[0],  dsatparm_exec_param_cmd},

  { "S10",      CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S10_IDX, &dsat_s10_dflm[0], dsatparm_exec_param_cmd},

  { "S3",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S3_IDX,  &dsat_s3_dflm[0], dsatparm_exec_param_cmd},

  { "S2",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S2_IDX, &dsat_s2_dflm[0],  dsatparm_exec_param_cmd},
} ;

/* Extended common AT command table */
const dsati_cmd_type dsat_ext_table [] =
{
  { "+ICF",     EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 2,DSAT_EXT_ICF_IDX,  &dsat_icf_dflm[0],  dsatparm_exec_param_cmd},

  { "+IFC",     EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 2,DSAT_EXT_IFC_IDX,   &dsat_ifc_dflm[0],  dsatparm_exec_param_cmd},

  { "+IPR",     EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 1, DSAT_EXT_IPR_IDX,      &dsat_ipr_list,  dsatparm_exec_ipr_cmd},

  { "+GMI",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE, 0,DSAT_EXT_GMI_IDX,   NULL,  dsatact_exec_gmi_cmd},

  { "+GMM",     READ_ONLY | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_EXT_GMM_IDX,   NULL,  dsatact_exec_gmm_cmd},

  { "+GMR",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_EXT_GMR_IDX,   NULL,  dsatact_exec_gmr_cmd},

  { "+GSN",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0,  DSAT_EXT_GSN_IDX,    NULL,  dsatact_exec_gsn_cmd},
    
  { "+GCAP",    READ_ONLY | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_EXT_GCAP_IDX,    NULL,  dsatact_exec_gcap_cmd},

  { "+CMEE",     EXTENDED | CONFIG | LIST | LOCAL_TEST | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   1,DSAT_EXT_CMEE_IDX,     &dsat_cmee_list,  dsatparm_exec_param_cmd},

#if defined(FEATURE_DSAT_ETSI_MODE)
/* Exclude CDMA (for now, may be required later) */
  { "+WS46",      EXTENDED | CONFIG |  LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1,DSAT_EXT_WS46_IDX,      &dsat_ws46_list[0],  dsatact_exec_ws46_cmd},
#endif /* FEATURE_DSAT_ETSI_MODE */

  { "+CFUN",    EXTENDED | CONFIG | LOCAL_TEST | LIST | MIXED_PARAM | 
                NO_RESET | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE, 2,DSAT_EXT_CFUN_IDX,   dsat_cfun_mixed_dfl,  dsatme_exec_cfun_cmd},
} ;

/* Vendor specific common AT command table. */
const dsati_cmd_type dsat_vendor_table [] =
{
  { "$QCPWRDN",  EXTENDED | COMMON_CMD| RESTRICTED,
    SPECIAL_NONE,    0, DSAT_VENDOR_QCPWRDN_IDX,   NULL,  dsatvend_exec_qcpwrdn_cmd}
#ifdef FEATURE_DATA_QCRMCALL
  ,{ "$QCRMCALL",  ABORT_CMD | EXTENDED | COMMON_CMD, 
       SPECIAL_NONE, 6, DSAT_VENDOR_QCRMCALL_IDX,  NULL,  dsatvend_exec_qcrmcall_cmd}
#endif /* FEATURE_DATA_QCRMCALL */

#ifdef FEATURE_DSAT_ETSI_DATA
#ifdef FEATURE_DSAT_ATCOP_VEND
  ,{ "$QCPDPP",    EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,4, DSAT_VENDOR_QCPDPP_IDX, &dsat_qcpdpp_mixed_dfl[0],  dsatetsipkt_exec_qcpdpp_cmd}
#endif  
  ,{ "$QCDGEN",  EXTENDED | UMTS_CMD,
	  SPECIAL_NONE,    2,DSAT_VENDOR_QCDGEN_IDX,   NULL,  dsatvend_exec_qcdgen_cmd}
  
#ifdef FEATURE_DSAT_ATCOP_VEND
  ,{ "$QCPDPIMSCFGE",  EXTENDED | UMTS_CMD,
	 SPECIAL_NONE,	  4,DSAT_VENDOR_QCPDPIMSCFGE_IDX,  NULL,  dsatvend_exec_qcpdpimscfge_cmd }
  ,{ "$QCPDPCFGE",  EXTENDED | COMMON_CMD,
     SPECIAL_NONE,   12,DSAT_VENDOR_QCPDPCFGE_IDX,   NULL,  dsatvend_exec_qcpdpcfge_cmd }
#endif
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_ATCOP_VEND
  ,{ "$QCSIMSTAT",    EXTENDED | CONFIG | RESTRICTED | UMTS_CMD,
    SPECIAL_NONE,1,DSAT_VENDOR_QCSIMSTAT_IDX, &dsat_qcsimstat_dflm[0],  dsatme_exec_qcsimstat_cmd}
#endif
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_LTE  
#ifdef FEATURE_DSAT_ATCOP_VEND

  ,{ "$QCPRFMOD",  EXTENDED  | LOCAL_TEST | COMMON_CMD,
	  SPECIAL_NONE,  15,  DSAT_VENDOR_QCPRFMOD_IDX,  NULL,  dsatvend_exec_qcprfmod_cmd }

  ,{ "$QCBANDPREF",    EXTENDED | LOCAL_TEST | COMMON_CMD,
        SPECIAL_NONE,    1,DSAT_VENDOR_QCBANDPREF_IDX,NULL, dsatvend_exec_qcbandpref_cmd }

  ,{ "$QCRSRP", READ_ONLY| EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 0 , DSAT_VENDOR_QCRSRP_IDX,  NULL,  dsatvend_exec_qcrsrp_cmd  }
  
 ,{ "$QCRSRQ", READ_ONLY| EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 0 , DSAT_VENDOR_QCRSRQ_IDX,   NULL,   dsatvend_exec_qcrsrp_cmd }
 
 ,{ "$QCRATSTATE",  EXTENDED  | CONFIG | LOCAL_TEST | UMTS_CMD,
     SPECIAL_NONE,  1,  DSAT_VENDOR_QCRATSTATE_IDX,  dsat_qcratstate_dflm,  dsatvend_exec_qcratstate_cmd}

 ,{ "$QCEXTCLTSTATE",  EXTENDED  | CONFIG | LOCAL_TEST | UMTS_CMD | RESTRICTED,
     SPECIAL_NONE,  1,   DSAT_VENDOR_QCEXTCLTSTATE_IDX,  dsat_qcextcltstate_dflm,  dsatvend_exec_qcextcltstate_cmd}
 
 ,{ "$QCCELLSCAN",  EXTENDED  |  UMTS_CMD ,
     SPECIAL_NONE,  5,   DSAT_VENDOR_QCCELLSCAN_IDX,  NULL,  dsatvend_exec_qccellscan_cmd}
 ,{ "$QCAPNE",  EXTENDED | UMTS_CMD,
     SPECIAL_NONE,    7,DSAT_VENDOR_QCAPNE_IDX,  NULL , dsatvend_exec_qcapne_cmd}
 ,{ "$QCCEINFO",  EXTENDED |  CONFIG | LOCAL_TEST| UMTS_CMD,
     SPECIAL_NONE,    1,DSAT_VENDOR_QCCEINFO_MODE,  dsat_qcceinfo_dflm , dsatvend_exec_qcceinfo_cmd}
 ,{ "$QCSFN",  EXTENDED |  UMTS_CMD | LOCAL_TEST| UMTS_CMD,
     SPECIAL_NONE,    1,DSAT_VENDOR_QCSFNGET_IDX,  NULL , dsatvend_exec_qcsfn_cmd}
#endif
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DSAT_ATCOP_VEND

 ,{ "$QCJDCFG",  EXTENDED  |  CONFIG | LOCAL_TEST | UMTS_CMD ,
     SPECIAL_NONE,  1,   DSAT_VENDOR_QCJDCFG,  dsat_qcjdcfg_dflm,  dsatvend_exec_qcjdcfg_cmd}

 ,{ "$QCJDSTATE",  EXTENDED  |  CONFIG | LOCAL_TEST | UMTS_CMD ,
     SPECIAL_NONE,  1,   DSAT_VENDOR_QCJDSTATE,  dsat_qcjdstate_dflm,  dsatvend_exec_qcjdstate_cmd}
 ,{ "$QCRPMINFO",  EXTENDED  |  UMTS_CMD ,
    SPECIAL_NONE,  0,   DSAT_VENDOR_QCRPMINFO_IDX,  NULL,  dsatvend_exec_qcrpminfo_cmd}
 ,{ "$QCRRINFO",  EXTENDED |  UMTS_CMD | LOCAL_TEST| UMTS_CMD,
     SPECIAL_NONE,    0, DSAT_VENDOR_QCRRINFO_IDX,  NULL , dsatvend_exec_qcrrinfo_cmd}
 ,{ "$QCMSISDN",  EXTENDED | LOCAL_TEST| COMMON_CMD,
     SPECIAL_NONE,    1, DSAT_VENDOR_QCMSISDN_IDX,  NULL , dsatvend_exec_qcmsisdn_cmd}
 ,{ "$QCCELLSELECT",  EXTENDED | LOCAL_TEST| COMMON_CMD,
     SPECIAL_NONE,    1, DSAT_VENDOR_QCCELLSELECT_IDX,  NULL , dsatvend_exec_qccellselect_cmd}
 #ifdef FEATURE_FSK_TX
 ,{ "$QCFSKDBG",  EXTENDED | CONFIG | LIST | MIXED_PARAM | UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    3, DSAT_VENDOR_QCFSKDBG_IDX, dsat_qcfskdbg_dflm , dsatvend_exec_qcfskdbg_cmd}
 ,{ "$QCFSKDATA",  EXTENDED |  CONFIG | UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    1, DSAT_VENDOR_QCFSKDATA_IDX,  dsat_qcfskdata_dflm , dsatvend_exec_qcfskdata_cmd}
 ,{ "$QCFSKHOPTABLE",  EXTENDED | UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    52, DSAT_VENDOR_QCFSKHOPTABLE_IDX,  NULL , dsatvend_exec_qcfskhoptable_cmd}
 ,{ "$QCFSKSTART",  EXTENDED |  CONFIG | UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    3, DSAT_VENDOR_QCFSKSTART_IDX,  dsat_qcfskstart_dflm , dsatvend_exec_qcfskstart_cmd}
 ,{ "$QCFSKSTOP",  EXTENDED |  CONFIG | UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    1, DSAT_VENDOR_QCFSKSTOP_IDX,  dsat_qcfskstop_dflm , dsatvend_exec_qcfskstop_cmd}
 ,{ "$QCPCLFIX",  EXTENDED | UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    3, DSAT_VENDOR_QCPCLFIX_IDX,  NULL , dsatvend_exec_qcpclfix_cmd}
#endif /* FEATURE_FSK_TX */
 ,{ "$QCDLTPTENH",  EXTENDED |  UMTS_CMD | LOCAL_TEST,
     SPECIAL_NONE,    1, DSAT_VENDOR_QCDLTPTENH_IDX,  NULL , dsatvend_exec_qcdltptenh_cmd}
#endif 
};

/* Command table sizes. */
const unsigned int dsat_basic_table_size = ARR_SIZE( dsat_basic_table );
const unsigned int dsat_basic_action_table_size =
                        ARR_SIZE( dsat_basic_action_table );
const unsigned int dsat_sreg_table_size = ARR_SIZE( dsat_sreg_table );
const unsigned int dsat_ext_table_size = ARR_SIZE( dsat_ext_table );
const unsigned int dsat_vendor_table_size = ARR_SIZE( dsat_vendor_table );

/*===========================================================================

FUNCTION DSATCTAB_DATA_INIT

DESCRIPTION
  This function initializes the AT command defaults, limits, and lists
  that depend on a run-time determination of the hardware.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatctab_data_init
(
  void
)
{
  /*---------------------------------------------------------------------------
  status from NV call. This variable will be used to obtain the status of the 
  read/write operations to the NV.
  ---------------------------------------------------------------------------*/
  nv_stat_enum_type    nv_status;          

  /*-----------------------------------------------------------------------
  Read the stored Baud rate from the NV. If NV is not written, then use 
  the default values.
  -----------------------------------------------------------------------*/
  nv_status = dsatutil_get_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
  
  /*-----------------------------------------------------------------------
  Check if the NV has valid data inside and load the data from NV to 
  local item.
  
  If NV is ACTIVE, confirm that the value inside NV is the valid value 
  by calling sio_is_baud_valid()       
  -----------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    if( sio_is_baud_valid((sio_bitrate_type)ds_nv_item.ds_default_baudrate) 
      == TRUE)
    {
      dsat_ipr_list.default_v   = ds_nv_item.ds_default_baudrate;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
    If NV was never written then copy the default values into the ipr_val. 
    -----------------------------------------------------------------------*/
    dsat_ipr_list.default_v    = (dsat_num_item_type)SIO_BITRATE_115200;
  }
}  /* dsatctab_data_init( ) */

/*===========================================================================

FUNCTION DSAT_INIT_QCSIMAPP_TABLE

DESCRIPTION
  This function initializes the QCSIMAPP table.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_init_qcsimapp_table(void)
{
  dsat_apps_id_e_type  apps_id;

  for (apps_id = DSAT_APPS_ID_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
  {
    dsat_qcsimapp_table.apps_id_mapped[apps_id] = FALSE;
  }

  dsat_qcsimapp_table.apps_id_mapped[DSAT_APPS_ID_PRIMARY]   = TRUE;
  dsat_qcsimapp_table.apps_id_mapped[DSAT_APPS_ID_SECONDARY] = TRUE;
  dsat_qcsimapp_table.apps_id_mapped[DSAT_APPS_ID_TERTIARY]  = TRUE;
  return;
} /* dsat_init_qcsimapp_table */

/*===========================================================================

FUNCTION  DSAT_GET_QCSIMAPP_VAL

DESCRIPTION
  This function returns value of dsat_qcsimapp_val 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_qcsimapp_val

SIDE EFFECTS
  None

===========================================================================*/

dsat_num_item_type dsat_get_qcsimapp_val(void)
{
  dsat_num_item_type       qcsimapp_val = dsatcmdp_dds_qcsimapp_val;
  ds3g_siolib_port_e_type  port_id      = ds3g_siolib_get_active_port();
  dsat_sio_info_s_type    *sio_info_ptr = NULL;

  if ((port_id >= DS3G_SIOLIB_DATA_PORT) && (port_id < DS3G_SIOLIB_PORT_MAX))
  {
    if (TRUE == dsatcmdp_is_sio_info_inited(port_id))
    {
      sio_info_ptr = dsatcmdp_get_sio_info_ptr(port_id);
      qcsimapp_val = sio_info_ptr->qcsimapp_val;
    }
  }

  return qcsimapp_val;
}/*dsat_get_qcsimapp_val*/

/*===========================================================================

FUNCTION  DSAT_SET_QCSIMAPP_VAL_USER_FLAG

DESCRIPTION
  This function sets dsat_qcsimapp_val_user_flag

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_set_qcsimapp_val_user_flag(void)
{
  dsat_sio_info_s_type *sio_info_ptr = NULL;
  
  sio_info_ptr = dsatcmdp_get_sio_info_ptr(ds3g_siolib_get_active_port());

  sio_info_ptr->qcsimapp_val_user_flag = TRUE;
} /* dsat_set_qcsimapp_val_user_flag */

#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
/*===========================================================================

FUNCTION DSATCTAB_GET_MODEM_PORT_ID

DESCRIPTION
  This function returns the corresponding modem port id for AT terminal

DEPENDENCIES
  None
  
RETURN VALUE
  -1: if the input port is not modem port

SIDE EFFECTS
  None

===========================================================================*/
int dsatctab_get_modem_port_id(ds3g_siolib_port_e_type port)
{

  int modem_port_id = DSAT_MODEM_PORT_NONE;

  if ( port == DS3G_SIOLIB_USB_CDC_PORT )
  {
    modem_port_id = DSAT_MODEM_PORT_1;
  }
  else if ( port == DS3G_SIOLIB_USB_SER3_PORT ) 
  {
    modem_port_id = DSAT_MODEM_PORT_2;
  }
  else
  {
  }
  return modem_port_id;
} /*dsatctab_get_modem_port_id*/

#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/
