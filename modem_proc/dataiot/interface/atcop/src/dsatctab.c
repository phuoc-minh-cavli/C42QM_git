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

   Copyright (c) 2001 - 2017 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatctab.c_v   1.10   11 Nov 2002 14:02:12   sbandaru  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatctab.c#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/12/17   skc     Enabled AT$QCPRFMOD for IOE Build.
04/17/17   skc     Enabled requested AT Commands for IOE Build.
01/27/17   skc     Enabled AT$QCPDPIMSCFGE for IOE build.
01/27/17   skc     Enabled AT$QCDGEN support in IOE build.
01/09/17   skc     Featurizing AT$QCRMCALL command.
10/18/16   skc     Fixed issue related to AT+CLAC
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
11/24/14   pg      Fixed ws46 featurization issue.
06/27/14   sc      Added support for Dynamic ATCoP.
03/26/14   pg      Fixed invalid subsinfo issue in case 1xLTE.
03/14/14   tk      Optimized debug macros usage in ATCoP.
01/23/14   sc      Added support for $QCPDPCFGEXT command.
12/25/13   tk      Enhanced $QCSIMAPP command for better user experience.
11/14/13   tk      Extended +IPR and $QCTER commands for bit-rates up to 4Mbps.
11/13/13   sc      Added support for $QCCSGCOPS command.
10/23/13   sc      Adding support for $QCACQDBC command to clear acq_DB
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
08/21/13   pg      Added support for emergency calls, operator res pco, mcc and mnc fields in $QCPDPCFGE.
07/25/13   pg      Modified $QCPDPCFGE to COMMON_CMD
07/16/13   pg      Fixed no ^MODE URC when UE goes out of service and comes back
07/16/13   pg      Fixed ^SYSINFO o/p for "SIM is not inserted"
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/17/13   sk      Added data retry fields to $QCPDPCFGE command.
05/16/13   sk      $QCCOPS command for Extension of network search.
03/27/13   tk      Fixed issues in dual stack commands.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
09/24/12   sk      Added $QCRSRP and $QCRSRQ command Support.
08/06/12   tk      Added support for $QCDRX command.
06/30/12   sk      Added UMTS_CMD check.
05/18/12   tk      Migrated to MSG 2.0 macros
05/18/12   sk      Added APN bearer field to $QCPDPCFGE.
03/07/12   sk      Made CLCC common for all modes.
02/17/11   ua      Added support for $QCRMCALL.
02/06/12   nc      Added support for ^SPN and URC Queue.
01/19/12   sk      Feature cleanup.
09/27/11   mk      Added support for $QCCLAC command.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/07/11   nc      Added support for $QCAPNE command.
05/13/11   ad      Added $QCPDPIMSCFGE support.
04/01/11   ttv     Added changes to use get/set functions.
03/14/11   ad      Added APN class support in $QCPDPCFGE command.
02/28/11   ad      Allowed L and M commands to execute in restricted mode . 
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
09/28/10   ad      Added LTE support in +WS46.
08/08/10   ua      Allowing ATL, ATM, ATS0 commands in RESTRICTED mode for 
                   Fusion solution. 
07/08/10   ad      Added support for $ECALL command.
05/31/10   ad      Added support for S7 command.
05/17/10   ad      Added Support for $QCPDPCFGE.
05/10/10   kk      Mainlining pending GOBI changes.
01/18/10   bs      Added support for +PACSP.
01/20/10   kk      Extended +CFUN to CDMA/MM targets.
01/15/10   ua      PBM AT Commands support for DSDS and CSIM. 
11/16/09   ca      Added support for MMGSDI Auth CME errors.
06/10/09   ua      Added support for ^PREFMODE command. 
12/15/09   nc      Featurisation changes for LTE.
08/04/09   nc      Added support for *CNTI.
07/15/09   sa      Added support for $CSQ command.
07/14/09   bs      Added support for $CREG & $CCLK commands.
06/12/09   ua      Added support for $QCBANDPREF command.
04/29/09   ua      Fixed compiler warnings. 
01/19/09   bs      Added support for $qcctm.
12/12/08   ua      Fixed Off target lint errors.
11/28/08   cs      OffTarget Lint and Compiler warnings.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/30/08   ua      Changing the default values for &E and X to be 0 and 1 
                   respectively to display the connect rate as radio rate 
                   by default.
10/23/08   sa      Added support for $QCPDPFAM command.
10/18/08   pp      Added support for SLIP interface (AT$QCSLIP).
02/20/08   sa      Added support for $QCSQ command.
02/18/08   sa      Added modification for Passport Feature support.
01/31/08   bs      Fixing $QCSIMSTAT featurization issue.
01/21/08   bs      Added new command $QCSIMSTAT for sim status.
01/11/08   sa      Added support for $QCANTE and $QCRPW commands.
10/26/07   ua      Added support for AT&S command. 
10/09/07   ss      Added support for $QCGANSM, $QCGARL, $QCGAPL commands.
10/31/07   sa      Allow Q,V,E,+FCLASS,+GMM,+GMR,+GMI,+GSN in restricted mode.
09/13/07   sa      Feature wrapped S2 register in FEATURE_DATA_S2_REG.
08/17/07   ua      Added support for $QCCNMI command which compensates CNMI 
                   GCF test cases.
08/06/07   pp      Moved AT\Q command to mainline.
07/06/07   ar      Added export of dsat_ds_dflm variable.
02/07/07   pp      Added full support for AT&W, AT\S commands.
05/25/07   sa      ATI and ATZ commands are allowed in restricted mode.
05/15/07   sa      &F should be allowed in restricted mode as per spec.
04/23/07   pp      Lint Medium fixes.
04/11/07   ua      Modifications as per KDDI requirements. 
04/03/07   pkp     New commands: &E, \V, &S, \S, &W, S2, S30, S103, S104, %V.
05/22/06   rsl     Add support for at$qcsysmode to return current sysmode, 
                   possible values returned: HSDPA/HSUPA/HSDPA+HSUPA/WCDMA/GSM.
11/03/05   ar      Make +WS46 responses consistent with spec CR.
09/08/05   ar      Add support for $QCSDFTEST vendor command.
04/14/05   sb      Changes for incoming CS call when serializer is enabled
03/30/05   tkk     Featurized ATB command.
03/03/05   snb     Add $QCDGEN command for data generation in +CGACT context
                   activation.
03/03/05   tkk     Added NO_DISPLAY attribute to ATB command to force AT&V not
                   to display ATB's parameter value.
03/02/05   tkk     Added support for ATB command.
01/24/05   hap     Adding support for AT\Q command
01/24/05   hap     Adding support for AT\V command
01/11/05   ar      Add support for +WS46 command.
09/17/04   snb     Use dsatact_exec_ate_cmd handler.
11/14/03   snb     Change ATX default to 0 in support of CSD call data rate 
                   reporting on CONNECT.
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
10/08/03   ar      Add wrapper FEATURE_DSAT_BREW_SUPPORT to BREW commands.
08/15/03   ar      Added support for $QCSLOT command.
08/13/03   sb      Call dsatparm_exec_fclass_cmd() from +FCLASS command table
08/13/03   rsl     Moved at+fclass to dsatctab.c
08/13/03   snb     Added support for $QCPDPLT command
07/11/03   snb     Correct feature definitions.
07/07/03   sb      Added AT$BREW command
05/28/03   sb      Fix for WCDMA CS Calls
02/20/02   sb      changed the range of S10 register from 1-255
02/19/03   ar      Fix DSPE build error.
02/11/03   ar      Adjust +DS string paramater range
01/27/03   ar      Added 230400 baud to common +IPR and $QCTER value lists.
01/16/03   ar      Migrated context cmds to persistent PDP profile support
01/07/03   rsl     Removed dsat_qcpma_val, no longer used.
11/11/02   sb      NV intialization in dsatctab_data_init()
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/15/02   sb      Changed the power on default of &C in JCDMA mode
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   variables.
04/11/02   ar      Add support for $QCPDPP command
09/12/01   ar      Added commands to extended AT command table:
                     +GMI,+GMM,+GMR,+GSN,+GCAP,I
08/27/01   sjd     Initial release of new VU structure to MSM archives.

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
#include "dsatetsicmif.h"
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

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_v.h"
#endif /* FEATURE_DS_MOBILE_IP */

#if defined(FEATURE_DSAT_SP_CMDS)
#include "hw.h"
#include "prl_v.h"
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#if defined(FEATURE_DSAT_GPS_CMDS)
#include "pdapi.h"
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

dsat_global_state dsat_pending_state[MAX_PORT];

/*--------------------------------------------------------------------------
                          Common Command Tables 
--------------------------------------------------------------------------*

/*Order of commands in this table doesn't matter*/
const dsati_cmd_abort_type dsat_abort_table [] = 
{
  { DSAT_BASIC_ACT_D_IDX,            dsatact_call_abort_handler }

 ,{ DSAT_BASIC_ACT_A_IDX,            dsatact_call_abort_handler }
 
#ifdef FEATURE_DATA_QCRMCALL
  ,{ DSAT_VENDOR_QCRMCALL_IDX,        dsatvend_qcrmcall_abort_cmd_handler}
#endif /* FEATURE_DATA_QCRMCALL */
  ,{ DSATETSI_EXT_ACT_COPS_ETSI_IDX,  dsatetsicall_cops_abort_cmd_handler }
#ifdef FEATURE_ATCOP_CM_SUPP

  ,{ DSATETSI_EXT_ACT_CLCK_ETSI_IDX,  dsatetsicmif_sups_abort_handler }

#endif
  ,{ DSATETSI_EXT_ACT_CLIR_ETSI_IDX,  dsatetsicall_exec_ss_abort_handler }

  ,{ DSATETSI_EXT_CGDATA_IDX,         dsatact_call_abort_handler }

  /*Featurized aborts go here*/
#ifndef FEATURE_DSAT_CUST
  ,{ DSATETSI_EXT_ACT_COPN_ETSI_IDX,  dsatetsicall_plmn_abort_cmd_handler }
#endif /* FEATURE_DSAT_CUST */

#ifdef FEATURE_MMGSDI
  ,{ DSATETSI_EXT_ACT_CPOL_ETSI_IDX,  dsatetsicall_plmn_abort_cmd_handler }
#endif /* FEATURE_MMGSDI */
};

const unsigned int dsat_abort_table_size = ARR_SIZE( dsat_abort_table );
