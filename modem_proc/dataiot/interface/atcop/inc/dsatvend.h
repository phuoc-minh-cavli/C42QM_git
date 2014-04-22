#ifndef DSATVEND_H
#define DSATVEND_H
/*===========================================================================

                D A T A   S E R V I C E S

                A T  C O M M A N D   
                
                ( V E N D O R  S P E C I F I C )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services vendor specific AT command processor.

Copyright (c) 2001-2023 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvend.h_v   1.0   08 Aug 2002 11:19:44   akhare  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/inc/dsatvend.h#8 $ $DateTime: 2023/04/06 01:33:10 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/23   ks      Enhancement of $QCRRINFO command.
09/30/21   ks      Added support for $QCDLTPTENH command.
01/25/21   ks      Added support for FSK related AT commands.
08/11/20   sp      Added support for command $QCMSISDN.
05/22/20   ks      Added support for $QCRRINFO command.
04/23/20   ks      Fixed jammer detect modem crash.
11/13/19   sp      Added support for AT$QCRPMINFO commad
01/09/17   skc     Featurizing AT$QCRMCALL command.
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
01/23/14   sc      Added support for $QCPDPCFGEXT command.
01/07/14   sc      Converted high usage ATCoP macros to functions.
10/23/13   sc      Adding support for $QCACQDBC command to clear acq_DB
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/28/13   tk      Fixed $QCBANDPREF issue with WLAN France 5000 band.
09/24/12   sk      Added $QCRSRP and $QCRSRQ command Support.
09/24/12   tk      Migrated to CM APIs for MRU table access.
08/06/12   tk      Added support for $QCDRX command.
06/21/12   sk      Fixed $QCPDPCFGE apn bearer issue.
06/12/12   tk      Extended $QCSYSMODE command for 3GPP2 modes.
05/18/12   sk      Added APN bearer field to $QCPDPCFGE.
02/17/11   ua      Added support for $QCRMCALL. 
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
10/17/11   ad      Added +CECALL support.
09/27/11   mk      Added support for $QCCLAC command.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/07/11   nc      Added support for $QCAPNE command.
05/13/11   ad      Added $QCPDPIMSCFGE support.
03/14/11   ad      Added APN class support in $QCPDPCFGE command.
01/11/11   ad      Removed pending variable .
07/08/10   ad      Added support for $ECALL command.
05/17/10   ad      Added Support for $QCPDPCFGE.
05/10/10   kk      Mainlining pending GOBI changes.
01/18/10   bs      Added support for +PACSP.
06/10/09   ua      Added support for ^PREFMODE command. 
07/16/09   ua      Added support for $QCBANDPREF command.
12/15/09   nc      Featurisation changes for LTE.
08/04/09   nc      Added support for *CNTI.
07/15/09   sa      Added support for $CSQ command.
04/29/09   ua      Fixed compiler warnings. 
03/04/09   sa      AU level CMI modifications.
10/18/08   pp      Added support for SLIP interface [AT$QCSLIP].
02/20/08   sa      Added support for $QCSQ command.
01/11/08   sa      Added support for $QCANTE and $QCRPW commands.
03/07/07   sa      Fixed feature wrap issues
02/06/07   ar      Migrate from SIO to SMD interface.
11/03/06   snb     Dual processor changes.
09/26/06   sa      Inter RAT handover support for $QCDGEN.
05/22/06   rsl     Add support for at$qcsysmode to return current sysmode, 
				   possible values returned: HSDPA/HSUPA/HSDPA+HSUPA/WCDMA/GSM.
01/26/06   snb     Correct size of datagen_info array.
09/08/05   ar      Add support for $QCSDFTEST vendor command.
07/25/05   snb     Add constants,type,array and external functions supporting 
                   WCDMA-mode data generation command $QCDGEN.
02/24/05   snb     Added $QCDGEN command.
10/21/03   ar      Added $QCPWRDN command support.
10/08/03   ar      Add wrapper FEATURE_DSAT_BREW_SUPPORT to BREW commands.
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   functions.
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      created file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "sys.h"
#ifdef FEATURE_DSAT_LTE
#include "cmapi.h"
#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
#include "ims_settings_atcop.h"
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
#endif /* FEATURE_DSAT_LTE */
#include "dsat_v.h"
#include "dsati.h"
#include "dsatctab.h"
#include "dsatcmif.h"
#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_MODE */
#include "ds_rmnet_meta_sm.h"

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/



typedef enum
{
  DSAT_BAND_PREF_NONE = 0,
  DSAT_BAND_PREF_BC0_A,
  DSAT_BAND_PREF_BC0_B,
  DSAT_BAND_PREF_BC0,
  DSAT_BAND_PREF_BC1,
  DSAT_BAND_PREF_BC3,
  DSAT_BAND_PREF_BC4,
  DSAT_BAND_PREF_BC5,
  DSAT_BAND_PREF_GSM_DCS_1800,
  DSAT_BAND_PREF_GSM_EGSM_900,
  DSAT_BAND_PREF_GSM_PGSM_900,
  DSAT_BAND_PREF_BC6,
  DSAT_BAND_PREF_BC7,
  DSAT_BAND_PREF_BC8,
  DSAT_BAND_PREF_BC9,
  DSAT_BAND_PREF_BC10,
  DSAT_BAND_PREF_BC11,
  DSAT_BAND_PREF_BC12,
  DSAT_BAND_PREF_BC14,
  DSAT_BAND_PREF_BC15,
  DSAT_BAND_PREF_BC16,
  DSAT_BAND_PREF_GSM_450,
  DSAT_BAND_PREF_GSM_480,
  DSAT_BAND_PREF_GSM_750,
  DSAT_BAND_PREF_GSM_850,
  DSAT_BAND_PREF_GSM_RGSM_900,
  DSAT_BAND_PREF_GSM_PCS_1900,
  /*
  DSAT_BAND_PREF_WCDMA_I_IMT_2000,
  DSAT_BAND_PREF_WCDMA_II_PCS_1900,
  DSAT_BAND_PREF_WCDMA_III_1700,
  DSAT_BAND_PREF_WCDMA_IV_1700,
  DSAT_BAND_PREF_WCDMA_V_850,
  DSAT_BAND_PREF_WCDMA_VI_800,
  DSAT_BAND_PREF_WCDMA_VII_2600,
  DSAT_BAND_PREF_WCDMA_VIII_900,
  DSAT_BAND_PREF_WCDMA_IX_1700,
  DSAT_BAND_PREF_WLAN_2400_US,
  DSAT_BAND_PREF_WLAN_2400_JP,
  DSAT_BAND_PREF_WLAN_2400_ETSI,
  DSAT_BAND_PREF_WLAN_2400_SP,
  DSAT_BAND_PREF_WLAN_2400_FR,
  DSAT_BAND_PREF_WLAN_5000_US,
  DSAT_BAND_PREF_WLAN_5000_JP,
  DSAT_BAND_PREF_WLAN_5000_ETSI,
  DSAT_BAND_PREF_WLAN_5000_SP,
  DSAT_BAND_PREF_WLAN_5000_FR,*/
  DSAT_BAND_PREF_ANY,
  DSAT_BAND_PREF_MAX
}dsat_bandpref_e_type;

#define DSAT_BM_32BIT( val ) ( 1<< ((uint32)(val)) )

#define CHECK_TRUE( val )  ( (val) ? TRUE : FALSE )
  
#define DSAT_BAND_CHECK64( band_pref1, band_pref2 ) \
          ( CHECK_TRUE( (uint64)band_pref1 & (uint64)band_pref2 ) )
  
#define DSAT_BAND_CHECK32( band_pref1, band_pref2 )  \
          ( CHECK_TRUE( (uint32)band_pref1 & (uint32)band_pref2 ) )

#define DSAT_BAND_ADD32( band_pref1, band_pref2 )  \
        ( (uint32)( (uint32)band_pref1 | (uint32)band_pref2 ) )

  /* equivalent to CMSS_ECIO_VALUE_NO_SIGNAL_UMTS */
#define DSAT_ECIO_NO_SIGNAL        63 
#define DSAT_BER_NO_SIGNAL         0xFF
  /* equivalent to CMSS_PATHLOSS_VALUE_NO_SIGNAL_UMTS */
#define DSAT_PATHLOSS_NO_SIGNAL     0xFF
  /* equivalent to CMSS_SIR_VALUE_NO_SIGNAL_UMTS */
#define DSAT_SIR_NO_SIGNAL         0xFF

#ifdef FEATURE_DSAT_ETSI_DATA
#define DSAT_DGEN_MIN_DATA_LEN    21
#define DSAT_DGEN_DONT_EXCEED     13500
#define DSAT_DGEN_MIN_DONT_EXCEED 1
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_QCRMCALL
/* ====================================================================================
 *                     QCRMCALL Definitions 
 * ==================================================================================*/
typedef enum
{
  DSAT_PENDING_QCRMCALL_NONE        = 0X0, /* None  */
  DSAT_PENDING_QCRMCALL_START_V4    = 0X1,  /* Start V4 action */
  DSAT_PENDING_QCRMCALL_START_V6    = 0X2,  /* Start V6 action */
  DSAT_PENDING_QCRMCALL_STOP_V4     = 0X4,  /* Stop V4 action */
  DSAT_PENDING_QCRMCALL_STOP_V6     = 0X8,  /* Stop V6 action */
  DSAT_PENDING_QCRMCALL_QUERY       = 0X10  /* Query */
}dsat_qcrmcall_state_e_type;

typedef struct
{
  dsat_qcrmcall_state_e_type state;
  uint32                     v4_instance;
  uint32                     v6_instance;
}dsat_qcrmcall_s_type;

typedef struct
{
  uint8                      action;
  rmnet_qcrmcall_result_type result;
} dsat_rmnet_info_s_type;

#endif /* FEATURE_DATA_QCRMCALL */


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
dsat_result_enum_type dsatvend_exec_qcpwrdn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_ETSI_DATA
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDGEN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdgen command.
  at$qcdgen is used to send a specified quantity of PS data as an IP packet 
  over a PDP-IP context already activated by +CGACT command.

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
dsat_result_enum_type dsatvend_exec_qcdgen_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

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
dsat_result_enum_type dsatvend_exec_qcpdpcfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPCFGEXT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpcfgext command.
  at$qcpdpcfgext is used for getting and setting MBIM context type. 

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
dsat_result_enum_type dsatvend_exec_qcpdpcfgext_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


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
);

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
dsat_result_enum_type dsatvend_exec_qcapne_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCMRUE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$QCMRUE command.
  AT$QCMRUE is used to edit MRU database. 

  For Manual MRU :
  at$qcmrue=3,3,1,"00101"
  OK
  
  For AUTO MRU (NULL PLMN-ID) : 
  at$qcmrue=3,3,1,""
  OK

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : If the command has been successfully executed
    DSAT_ERROR : If there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcmrue_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


#ifdef FEATURE_DSAT_GOBI_MAINLINE

/*===========================================================================

FUNCTION DSATVEND_FTM_DIAGPKY_RSP_HANDLER

DESCRIPTION
   Called in DS context for DS_AT_FTM_DIAGPKT_RSP_CMD event.
   This event is used to execute callbacks to process FTM diag packets.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_ftm_diagpkt_rsp_handler
(
  ds_command_type         *cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION dsatvend_exec_qcsku_cmd

DESCRIPTION
  This function returns the SKU info

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsku_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

/*===========================================================================
FUNCTION   DSATVEND_IFACE_EV_HANDLER

DESCRIPTION
 This is a handler function for all the Rmnet releated events. 
 ATCoP is intrested in Rmnet START, STOP and QUERY events. 

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: Call is connected or teard down succesfully. 
  DSAT_ASYNC_EVENT: Waiting for more events before command processing is complete. 
  DSAT_CMD_ERR_RSP: If connection failed. 

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatvend_rmnet_ev_handler
(
  ds_command_type * cmd_ptr     /* DS Command pointer */
);

#ifdef FEATURE_DATA_QCRMCALL
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
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRMCALL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCRMCALL command.
  $QCRMCALL command is used to trigger a RmNet call via AT command. 
  It just passes on the Start Network Interface parameters into RmNet to
  bring up the call. 

  $QCRMCALL =<action>, <instance> [,<IP type> [,<Act> [,<umts_profile> [,<cdma_profile> [,<APN>]]]]]

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
);

#endif /* FEATURE_DATA_QCRMCALL */

#ifdef FEATURE_DSAT_LTE

#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
/*===========================================================================
FUNCTION   DSAT_CONVERT_AMRNUM_TO_AMRSTR

DESCRIPTION
  Function will convert AMR number value int AMR string. It will check the SET bit in given number
  and generate corresponding string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsat_convert_amrnum_to_amrstr
(
  char                   *rsp_buf,
  dsat_num_item_type      buf_len,
  dsat_num_item_type      amr_num
);

/*===========================================================================
FUNCTION DSATVEND_VOIP_CONFIG_CB

DESCRIPTION
  This function handles set/get response from IMS.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsatvend_voipm_config_cb
(
  ims_settings_at_cmd_rsp_info at_cmd_rsp_info
);
/*===========================================================================
FUNCTION DSATVEND_VOIPM_CONFIG_HANDLER

DESCRIPTION
  Handler function for IMS  response.

DEPENDENCIES
  None
  
RETURN VALUE
  
DSAT_ERROR : if there was any problem in executing the command.
DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_voipm_config_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCVOIPM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVOIPM command.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_ASYNC_CMD : if success.
    DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcvoipm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
#ifdef FEATURE_DSAT_ATCOP_VEND

/*===========================================================================
FUNCTION DSATVEND_PROCESS_QCRSRP_CMD

DESCRIPTION
  This function executes $QCRSRP and $QCRSRQ commands and provides response 
  Calls CM API to receive neighbour cell info like Cell id,EARFCN,RSRP,RSRQ.
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrsrp_cmd
(
    dsat_mode_enum_type mode,             /*  AT command mode:            */
    const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
    const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
    dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

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
);

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
);


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
dsat_result_enum_type dsatvend_exec_qcapne_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCBANDPREF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcbandpref command.
  This command provides the ability to set/get the band preferences

  
<band_pref>: Is a string containing indexes separated by comma. 
for eg: "1,2,3,4,5,6" which means set all the bandprefs which has indexes from
1,2,3,4,5,6 in the test command.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if success.
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcbandpref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSAVEND_PROCESS_QCRATSTAT_CMD

DESCRIPTION
  This function process the response for the $QCRATSTAT command
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
);


/*===========================================================================
FUNCTION DSATVEND_PROCESS_PREFMODE_CMD

DESCRIPTION
  This function process the response for the ^PREFMODE command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_prefmode_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,        /* cmd type */
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
);

#endif
#endif /*FEATURE_DSAT_LTE */

#ifdef FEATURE_DSAT_ATCOP_VEND
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCELLSCAN_CMD

DESCRIPTION
  This function takes the result from the command line parser and executesit. 
  It executes $QCCELLSCAN command which performs teh cell scan based on the input 
  bands and rat and gives the cell info in an asynchronous response.

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
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPRFMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to configure parameters 
  for 3GPP   profile.
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
);

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
);

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
);
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
);

/*===========================================================================
FUNCTION DSAT_JAMMER_DETECT_CMD_HANDLER

DESCRIPTION
  Handler function for jammer_detect_cb

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
/*ARGSUSED*/
dsat_result_enum_type dsat_jammer_detect_cmd_handler
(
  ds_command_type        *cmd_ptr      /* DS Command pointer		 */
);

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
  dsat_cops_act_e_type     rat_type,
  uint32                   inc_jamming_status,
  boolean                  inc_soft_jamming_status
);

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
);

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
  byte 		*value			   /* baud value		 */
);

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
);
/*===========================================================================
FUNCTION DSATVEND_EXEC_QCCEINFO_CMD

DESCRIPTION
  This function takes the input given with QCCEINFO and executes.

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
  dsat_mode_enum_type        mode,        /*  AT command mode:            */
  const dsati_cmd_type      *parse_table, /*  Ptr to cmd in parse table   */
  const tokens_struct_type  *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type             *res_buff_ptr /*  Place to put response       */
);

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
);

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
);

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
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_jd_service_status_ind_handler
(
  ds_command_type        *cmd_ptr      /* DS Command pointer		 */
);


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
);

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
  dsat_sfn_s_type       *sfn,
  dsm_item_type         *res_buff,
  boolean               async_command,
  ds_command_enum_type  cmd_id
);


/*===========================================================================

FUNCTION DSATVEND_PROCESS_SFN_CMD

DESCRIPTION
  This function posts the SFN information from LTE to DS task and calls 
  the SFN handler API in the DS task.
   

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
dsat_result_enum_type dsatvend_process_sfn_cmd
(
  ds_command_type          *cmd_ptr
);


/*===========================================================================
FUNCTION DSATVEND_EXEC_QCSFN_CMD

DESCRIPTION
  This function takes the input given with QCSFN and executes.

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
  dsat_mode_enum_type        mode,        /*  AT command mode:            */
  const dsati_cmd_type      *parse_table, /*  Ptr to cmd in parse table   */
  const tokens_struct_type  *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type             *res_buff_ptr /*  Place to put response       */
);

/*===========================================================================
FUNCTION DSATVEND_PROCESS_RRINFO_DATA

DESCRIPTION
  This function is the helper function to process rat info. It maps SYS
  enums to DS enums.

DEPENDENCIES
  None

RETURN VALUE
  Returns out_rat which holds values of DS types to further process.

SIDE EFFECTS
  None

===========================================================================*/

dsat_rr_cur_rat_e_type dsatvend_process_rrinfo_data
(
  sys_sys_mode_e_type in_rat
);

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
);

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
);

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
  byte * msisdn_buf               /* Output string buffer */
);

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
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
dsat_result_enum_type dsatvend_send_fsk_start();
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
);

#endif
#endif /* DSATVEND_H */
