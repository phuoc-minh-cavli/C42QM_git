/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                U M T S

                P A C K E T   D O M A I N
                                
                A T   C O M M A N D   P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It executes UMTS & GPRS specific packet
  domain AT commands.

  
EXTERNALIZED FUNCTIONS

  dsatetsipkt_exec_cgclass_cmd
    This function takes the result from the command line parser 
    and executes it. It handles the +CGCLASS command. It supports
    only CLASSB. 

  dsatetsipkt_exec_cgpaddr_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGPADDR command.
    This command returns a list of PDP addresses for the specified
    context identifiers

  dsatetsipkt_cgpaddr_response_fmt
    This function generates the response for the +CGPADDR read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.
  
  dsatetsipkt_exec_cgdcont_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGDCONT command.
    This command sets basic PDP context parameters.

  dsatetsipkt_exec_cgdscont_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGDSCONT command.
    This command sets PDP secondary context parameters.

  dsatetsipkt_cgdcont_response_fmt
    This function generates the response for the +CGDCONT read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.

  dsatetsipkt_exec_qcpdpp_cmd
    This function takes the result from the command line parser
    and executes it. It handles the $QCDPDP command.
    This command sets PDP authentication parameters.

  dsatetsipkt_exec_cgatt_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGATT command.
    This command reports the connection status for network packet
    domain service.

  dsatetsipkt_exec_cgact_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGACT command.
    This command activates or deactivates PDP context profiles.

  dsatetsipkt_reset_pdp_context_profiles
    This function resets all persistent PDP context profiles to their
    default values.

  dsatetsipkt_exec_cgcmod_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGCMOD command.
    This command modifies the Qos, TFT and APN of active PDP contexts.

  dsatetsipkt_exec_cgauto_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGAUTO command. This command disables or 
	enables an automatic response for a MT PDP Context Activation message. 
	
  dsatetsipkt_exec_cgans_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGANS command.
	This command is used to answer a incoming MT PDP call.

  dsatetsipkt_exec_cemode_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CEMODE command.

  dsatetsipkt_exec_cnmpsd_cmd
    This function executes +CNMPSD command which releases PS signaling.
    This command indicates that no application on the MT is expected to
    exchange data.

    
INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2023 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsipkt.c_v   1.7   12 Jul 2002 10:05:36   randrew  $  
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/src/dsatetsipkt.c#10 $   $DateTime: 2023/04/06 01:33:10 $ $Author: pwbldsvc $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/23   ks      Fixing the DS task stack corruption caused by cm_ph_info_s_type
                   structure. Made the local variable dynamic.
03/02/21   ks      Made changes to properly update PSM URC values when "Name only" 
                   commands are given.
11/09/20   ks      Added support for preserving URC indices across PSM cycles.
09/29/20   sp      Fixed issue related to +CGDCONT command.
08/19/20   ks      Fixed DUN call failure issue caused by +CGDCONT test command fix.
08/17/20   ks      Fixed issue related to +CGDCONT test command.
06/08/20   sp      Fixed +CGCONTRDP crash issue.
04/03/20   sp      Fixed +CGPADDR memory leak issue.
03/27/18   skc     APN name with '-' hyphen or '.' dot is not allowed.
09/21/17   rg      Added support for Non-IP pdp type profile.
04/17/17   skc     Enabled requested AT Commands for IOE Build.
03/13/15   sc      Fixed +CGDCONT AT read command parameter display issue.
02/12/15   tk      Fixed compilation errors when feature flag
                   FEATURE_SECONDARY_PDP is undefiend.
01/26/15   sc      Fixed +CGTFT\+QCGTFT AT command issue.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   tk      Added support for Dynamic ATCoP.
05/22/14   sc      Fixed QCPDPP issue.
04/17/14   sc      Fixed ATCoP KW errors.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
01/24/14   sc      Fixed ATCoP KW errors.
01/07/14   sc      Converted high usage ATCoP macros to functions.
11/14/13   tk      Fixed issues in online command state.
09/25/13   sc      Added support for +CNMPSD command.
09/13/13   sc      Fixed KW errors.
05/22/13   sk      Fixed Issues with AT+CGCONTRDP and CGTFTRDP.
03/27/13   tk      Fixed issues in dual stack commands.
03/21/13   sk      Fixed CGPADDR dual IP separator issue.
02/01/13   sk      Fixed CGEREP URC Issue with PDP reject.
08/22/12   sk      Mainlining Nikel Changes.
07/26/12   sb      Fixed KW Errors.
07/25/12   tk      Fixed CGPADDR issue in display of static IP addresses.
07/19/12   sk      CGACT issue fixed.
05/18/12   tk      Migrated to MSG 2.0 macros
03/19/12   sk      traffic handling priority check removed for CGEQREQ.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
12/06/11   sb      Avoid QOS mismatch between GPRS and Umts profile data.
11/03/11   sb      Fixed KW Warnings.
10/05/11   ht      Fixed Klocwork Issue.
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
05/23/11   ad      Allowed to set Packet Filter ID from 1 to 16.
04/01/11   ttv     Added changes to use get/set functions.
03/01/11   nc      Modified +CEMODE to be inline with CM Design.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
02/16/11   bs      Fixed memory alignment issue.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
10/06/10   ad      Added Error msg before Assert(0).
09/16/10   nc      Fixed CGCONTRDP.
09/16/10   nc      Fixed the loop for forwarding a secondary PDP context
                   request to MH.
08/31/10   sa      Added support for default pdp type as IPv4v6.
07/09/10   bs      Fixed +CGACT for LTE.
05/31/10   ad      Fixed Klocwork errors. 
05/15/10   nc      Added support fot +CGCONTRDP,+CGSCONTRDP,
                   +CGEQOSRDP and +CGTFTRDP for LTE.
05/14/10   sa      Added support for LTE packet event reporting.
04/20/10   bs      Added support for +CGCMOD for LTE.
03/18/10   nc      Added Support for +CGEQOS and +CGPADDR for LTE.
01/15/10   ua      Using new send_response_to_te API. 
01/06/10   bs      Added +CGACT command support for LTE.
01/04/10   vk      Removing certain macros from PDP reg API header file
                   that cause problems with autogenerated RPC stub code
12/15/09   nc      Featurisation changes for LTE.
12/07/09   nc      Added support for <IPv4AddrAlloc> parameter for +CGDCONT.
10/12/09   vk      PDP profile changes for IPv4v6 support
10/09/09   sa      Adding support for +CEMODE command.
09/10/09   nc      Fixed Klocwork errors. 
09/21/09   sch     Accepting DS_UMTS_PDP_AUTH_CHAP_OR_PAP as a valid auth type
08/20/09   nc      Fixed the conversion of Uplink Max Bit rate for Rel7.
08/10/09   ars     CMI featurization related changes. Modified NUM_PROTOCOL_ID
06/29/09   vg	   Replacing MSG_XXX with MSG_SPRINTF_X.
04/20/09   ua      Fixed issues related with call commands followed by +CIND Query.
06/25/09   vk      Removing use of certain deprecated macros in pdp reg api
06/18/09   sa      Integrated the support for higher maximum bitrate and
                   guaranteed bit rate in the QoS IE for Rel7.
05/19/09   vk      Defeaturization of PDP registry for CMI
04/16/09   sa      Added DS_UMTS_QOS_RES_BER_1E5 for Conversational class.
01/30/09   ua      Added support of CGEV for DUN calls.
12/12/08   ua      Fixed Off target lint errors.
12/02/08   ss      Off target Lint fix.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/03/08   bs      Fixed Alignment issues with MDM8200.
07/31/08   nc      Verification of transfer delay is 
                   ignored for interactive and background class 
03/24/08   bs      Fixed Klocwork criticals.
11/04/07   pp      Lint low - fixes.
10/09/07   ss      Added support for $QCGANSM, $QCGARL, $QCGAPL commands.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
08/06/07   sa      Removed +CGEQREQ and +CGEQMIN support in GSM only builds.
08/02/07   sa      Feature wrap changes on header and data compression for +CGDCONT
                   and +CGDSCONT.
07/12/07   sa      Fixing side effects of Banned APIs changes. 
04/26/07   sa      Fixed lint medium errors.
05/02/07   ss      Modified dsatetsipkt_setup_cm_orig_params to fix compilation
                   error when FEATURE_SECONDARY_PDP is disabled
04/05/07   pkp     +CGATT syntax corrections.
03/30/07   sa      Fixing buffer size for APN string in +CGDCONT.
03/29/07   ua      Reverting back CGCLASS modifications which supported CLASS B.
                   Now it supports only CLASS A. 
02/15/07   ss      Fixed lint high errors
02/15/07   ph      Lint CRITICAL fixes
02/13/07   pkp     Fixed RVCT compiler warnings.
01/30/07   ss      Replace banned string API calls.
12/07/07   ua      Fixing buffer overflow and also correcting the display 
                   for cgdscont test command.
11/03/06   snb     Dual processor changes.
10/30/06   ua      Correcting +CGACT state value verification.
10/27/06   sa      Fix for cid values not getting undefined for CGPADDR.
09/14/06   snb     Comply with IPCP API change.
08/03/06   sa      Add header and data compression support for +CGDCONT and +CGDSCONT.
08/01/06   snb     Clear query_mode on failure.
06/08/06   ua      Modification of +CGCLASS command to support only CLASSB.
02/21/06   sa      Merged changs from Sirius Branch
                   -Fixed additional buffer allocation for 
                   etsipkt_initiate_packet_call.
12/08/05   snb     Add IPV6 support
11/30/05   snb     Add support for +CGCMOD command.
10/02/05   snb     On +CGATT detach command when already detached still send 
                   user preference to lower layers.
07/29/05   snb     Use correct mode type in call to dial command for +CGDATA
                   processing.
05/09/05   tkk     Modification to dsat_addr_info for CID retrieval.
05/03/05   tkk     Added execution syntax to +CGPADDR command.
04/12/05   snb     Removed mutual exclusive setting of +CGQREQ/+CGEQREQ and
                   +CGQMIN/+CGEQMIN parameters.
04/05/05   tkk     Corrected test syntax for +CGPADDR command.
04/04/05   snb     Made external a function returning number of contexts 
                   without an assigned TFT and removed verification of 
                   "1 context without TFT" rule.
03/15/05   tkk     Fixed bugs for +CGPADDR and +CGEREP commands.
01/31/05   tkk     Added +CGDATA command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
01/11/05   snb     Output +CGDCONT's PDP address as quoted string.
01/10/05   tkk     Added support for +CGPADDR command.
11/22/04    rr     Set higher order byte of profile number to DS_UMTS_PDP_
                   ATCOP_PROFILE_FAMILY to access profiles from 
                   "pdp_profile/atcop_profiles" directory
09/13/04   snb     Fix buffer overflow in $QCPDPP read command when 16 profiles
                   defined and auth parms set for each with maximum length user
                   names.
06/21/04   snb     Add support for 16 PDP profiles.
06/01/04   snb     Corrected +CGDCONT portion of &V response.
05/12/04   snb     Corrected +CGACT reponse when no contexts defined.
01/05/04   ar      Added mapping for profile QoS delivery err SDUs param.
11/19/03   ar      Replace dsatutil_format_ipaddr with dsatutil_convert_tuple
                   Added support for +CGDSCONT and +CGTFT commands.
                   Removed unused etsipkt_reset_multi_param().
09/29/03   ar      Remove default value handling on +CGACT parameters.
09/25/03   sb      Fix the order in which h_comp and d_comp parameters of 
                   +CGDCONT are copied into the profile (EFS).
08/13/03   snb     Added support for $QCPDPLT command
06/24/03   ar      Remove LOCAL on staticly declared structures.
03/20/03   ar      Generate +CGACT response only for defined contexts.
                   Gracefully handle PDP context API failure in &V response.
03/17/03   ar      Move VALID_TOKEN macro to common header file.
02/26/03   ar      Reset profile section to default before update on
                   +CGxxxx write syntax
02/11/03   ar      Use persistent context profile for +CGEQMIN. Remove order
                   dependency on context profile commands.
02/03/03   ar      Suppress action in +CGATT if demand same as current state.
01/30/03   ar      Add support for +CGEQMIN command
01/10/03   sb      Added WRITE operation for +CGATT command
01/06/03   ar      Migrated context cmds to persistent PDP profile support 
08/26/02   ar      Move special paramater validation before saving input
                   parameters. Code review action items.
07/11/02   ar      Add support for +CGQMIN command
07/10/02   ar      Add validation of +CGDCONT APN parameter
06/28/02   ar      Add support for +CGEQREQ and +CGQREQ commands
04/11/02   ar      Add support for $QCPDPP command
02/04/02   ar      Add support for +CGDCONT command
08/27/01   sb      Created module.

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
#include "ds_profile.h"
#include "ds_prof.h"

#include "ds_3gpp_rmsm_ati.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/


/* Macro to perform assigment only if user parameter specified */
#define CONDITIONAL_ASSIGN(i,dest,src)\
          if (VALID_TOKEN(i))\
          {\
            dest = src;\
          }

        /*Begin and end transactions are not needed for reset param
          After Reset to default read command return default values*/

/* Macro to reset the context base paramaters  */
#define RESET_BASE_CONTEXT_PARAMS(cid)\
         if(etsipkt_delete_profile_context (cid) != TRUE)\
        { \
           result = DSAT_ERROR;\
         }\

  /* IP(0)/IPv4(0), PPP(1), IPv6(2), IPv4v6(3) and NON-IP(4) are valid */
  #define DSAT_MAX_PDP_TYPE DS_PROFILE_PDP_NON_IP 
  
/* Defaults from dsatetsictab.c */
extern const dflm_type dsat_pdp_cid_dflm[];
extern const dflm_type dsat_cgdcont_ipv4_addr_alloc_dflm[];
extern const dflm_type dsat_emergency_dflm[];

extern const dsat_string_item_type dsat_pdptype_valstr[][8];
extern const dsat_string_item_type dsat_cgdcont_hcomp_valstr [][8];
extern const dsat_string_item_type dsat_cgdcont_hcomp_tststr[];
extern const dsat_string_item_type dsat_cgdcont_dcomp_valstr [][8];
extern const dsat_string_item_type dsat_cgdcont_dcomp_tststr[];

/* Flag for detach writes intended for local stack only:
   This change for GCF test cases allows user preference to be given to 
   local stack only and lets async event processing terminate once CMPH
   event is received as no serving system event is expected.  */
extern boolean dsatcmif_ss_evt_not_expected;

/* Invalid PDP profile id */
#define INVALID_CID (0x0)

/* PDP context state information*/
byte  dsat_pdp_cid[DSAT_MAX_ALLOWED_PROFILES] = {0};  /* Context profile id */

LOCAL ds_3gpp_atif_dynamic_params_info_type  *dynamic_param_ptr;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL dsat_result_enum_type etsipkt_validate_apn
(
  byte *s_ptr                           /*  Points to string  */
);

LOCAL dsat_result_enum_type etsipkt_process_param
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  const uint32 pindex,                  /*  Parameter index             */
  void * result_ptr ,                    /*  Pointer to result           */  
  uint16 result_buff_len           /*  Length of result buffer */
);

LOCAL boolean etsipkt_build_context_processing_list
(
  const tokens_struct_type *tok_ptr,     /* Command tokens from parser   */
  dsat_call_query_mode_e_type type_list, /* For which command */
  boolean * state_changed_ptr            /* Pointer to state change flag */
);

LOCAL dsm_item_type* etsipkt_check_response_buffer
(
  dsm_item_type  *res_buff_ptr,   /* Response buffer */
  uint16          buf_limit       /* Buffer contents limit */
);

LOCAL boolean etsipkt_print_ipv4_addr
(
  dsat_ip_addr_type       *pdp_adr,
  uint32                  src_adr,
  boolean                 is_static_addr,
  dsm_item_type           *res_buff_ptr,
  char                    *buffer
);

LOCAL boolean etsipkt_print_ipv6_addr
(
  dsat_ip_addr_type                       *pdp_adr,
  ds_profile_addr_type_ipv6               *src_adr,
  boolean                                 is_static_addr,
  dsm_item_type                           *res_buff_ptr,
  char                                    *buffer
);

LOCAL boolean etsipkt_delete_profile_context
(
  const byte   primary_id  /* Primary profile identifier */
);


LOCAL dsat_result_enum_type etsipkt_cgpaddr_build_cid_list
(
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  uint8 *cid_list,                    /* Parse results list */
  uint8 *index                        /* Length of list */
);

LOCAL boolean etsipkt_cgpaddr_format_helper_response
(
  uint32                            cid,
  dsm_item_type                     *res_buff_ptr
);

LOCAL dsat_result_enum_type etsipkt_cgpaddr_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);

LOCAL boolean etsipkt_cgcontrdp_format_helper_response
(
   ds_3gpp_atif_dynamic_params_info_type  *dynamic_param_ptr,
   ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr,
   dsm_item_type                          *format_buffer
);

LOCAL dsat_result_enum_type etsipkt_cgcontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);

#ifdef FEATURE_ATCOP_TEMP_3GPP_ALT

LOCAL dsat_result_enum_type etsipkt_cgscontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);
#endif
LOCAL void etsipkt_clear_gprs_events_buffer
(
  void
);


/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION ETSIPKT_GET_PROFILE_LIST

DESCRIPTION
  This function generates the list of valid profiles either persistent
  or non-persistent per technology type. 

DEPENDENCIES
  None

RETURN VALUE 
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_get_profile_list
(
  ds_profile_list_stype    **profile_list,           /* Technology Type  */
  uint8                    *total_profiles      /* Total Number of Profiles */
)
{
  uint8                             profile_count =0;
/*----------------------------------------------------*/
  DS_SYSTEM_HEAP_MEM_ALLOC(*profile_list, (sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM),
                                          ds_profile_list_stype*);
  if(*profile_list == NULL)
  {
    DS_ASSERT(0);
    return DSAT_ERROR;
  }
  memset(*profile_list,0,(sizeof(ds_profile_list_stype)*DS_PROFILE_MAX_PROFILE_NUM));
  profile_count = ds_profile_get_profile_list(*profile_list);
  if(profile_count == 0)
  {
    DS_AT_MSG0_HIGH("No valid profile exists");
    DS_SYSTEM_HEAP_MEM_FREE(*profile_list);
    return DSAT_ERROR;
   }    
  *total_profiles = profile_count;
   return DSAT_OK;

}/*etsipkt_get_profile_list*/

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGPADDR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGPADDR command.
  This command returns a list of PDP addresses for the specified
  contect identifiers

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
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgpaddr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if( !VALID_TOKEN(0) )
    {
      /* No CID list specified */
      result = etsipkt_cgpaddr_format_response( NULL, 0,
						tok_ptr,
						res_buff_ptr);
    }
    else
    {
      /* CID list specified and must be parsed */
      uint8 cid_list[DSAT_MAX_ALLOWED_PROFILES];
      uint8 list_len = 0;

      /* Build the CID list */
      result = etsipkt_cgpaddr_build_cid_list( tok_ptr,
                                               cid_list,
                                               &list_len );

      /* Generate response */
      if( DSAT_OK == result)
      {
        result = etsipkt_cgpaddr_format_response( cid_list, list_len,
                                                  tok_ptr,
                                                  res_buff_ptr);
      }
    }
  }
  /*-----------------------------------------------------------
   Processing for a command without an argument (WRITE command)
   and TEST syntax
  ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
           (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgpaddr_format_response( NULL, 0,
                                              tok_ptr,
                                              res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsipkt_exec_cgpaddr_cmd */



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDCONT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDCONT command.
  This command sets basic PDP context parameters.

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
dsat_result_enum_type dsatetsipkt_exec_cgdcont_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;  
  dsat_ip_addr_type     ipaddr;
  dsat_num_item_type    val         = 0;
  ds_profile_status_etype   profile_status = DS_PROFILE_RESULT_SUCCESS;

  dsat_mixed_param_val_type * val_ptr = 
           (dsat_mixed_param_val_type *)dsatutil_get_val(parse_table->cmd_id,0,STR_TYPE);
  dsat_error_info_s_type    err_info;
  ds_profile_ident_info                      ident_info;  
  ds_profile_type_s                         *payload =NULL;
  uint64                                    mask =0;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  memset( (void *)&ipaddr, 0, sizeof(ipaddr) );
  memset(&ident_info, 0, sizeof(ds_profile_ident_info));

  if (tok_ptr->op == (NA|EQ|AR))
  {
     if(val_ptr ==NULL)
     {
       return DSAT_ERROR;
     }
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    dsat_num_item_type num_val = 0;

    /* Process connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    /* Test for only CID specified */
    if (DSAT_IS_PROFILE_ID_WITHIN_RANGE(val) && (1 == tok_ptr->args_found ))
    {
      /* Clear the user specified +CGDCONT context parameters */
       RESET_BASE_CONTEXT_PARAMS(val);
    }
    else
    {
      byte temp_buf[MAX_IPADDR_STR_LEN+1];
      byte apn_buff[DS_PROFILE_APN_STRING_LEN +1];

      byte                      cid = (byte)val;

      memset( temp_buf, 0, sizeof(temp_buf) );
      memset( apn_buff, 0, sizeof(apn_buff) );
      
      /* Verify IP address is correct; OK if null */
      if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 3,
                                 temp_buf,sizeof(temp_buf)))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 3;
        goto send_error;
      }

      if ( !( ('\0' == temp_buf[0]) ||
              (DSAT_OK == dsatutil_convert_ipaddr(STRING_TO_INT,
                                                  &ipaddr,
                                                  temp_buf)) ) )
      {
        /* There is a MSG_MACRO inside function. */      
        return DSAT_ERROR;
      }
      else
      {
        if(('\0' != temp_buf[0]) && (VALID_TOKEN(3)))
        {
          mask = mask|DS_PROFILE_PDP_ADDR;
        }
      }
      
      /* Verify APN is correct; OK if null */
      if( (VALID_TOKEN(2) ) && ('"' != *tok_ptr->arg[2]) )
      {
        /* APN should be entered in quotes ("") */
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 2;
        goto send_error;
      }
      if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 2, 
                                apn_buff,sizeof(apn_buff)))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 2;
        goto send_error;
      }
      if (!(('\0' == apn_buff[0]) ||
            (DSAT_OK == etsipkt_validate_apn((byte*)apn_buff))))
      {
        /* There is a MSG_MACRO inside function. */      
        return DSAT_ERROR;
      }

      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
	     if(VALID_TOKEN(1))
         {
            if( ((val_ptr[1].num_item == DS_PROFILE_PDP_IP && ipaddr.ip_vsn == DS_PROFILE_IP_V6)) ||
                 ((val_ptr[1].num_item == DS_PROFILE_PDP_IPV6 && ipaddr.ip_vsn == DS_PROFILE_IP_V4)) )
             {        
               DS_AT_MSG2_ERROR("Command dsatetsipkt_exec_cgdcont_cm ipaddr.ip_vsn%d pdp type %d "
                            ,ipaddr.ip_vsn,val_ptr[1].num_item);
                return DSAT_ERROR;
             }
             if((val_ptr[1].num_item == DS_PROFILE_PDP_PPP))
             {        
                 return DSAT_ERROR;
              }
         }
        /* Query default profile from EFS */
        DS_SYSTEM_HEAP_MEM_ALLOC(payload,
                                sizeof(ds_profile_type_s),
                                ds_profile_type_s *); 
        if (NULL == payload)
        {        
          return DSAT_ERROR;
        }
        memset(payload, 0, sizeof(ds_profile_type_s));           
          
        /* Make conditional edits to profile                                  */
        CONDITIONAL_ASSIGN(1, payload->context.pdp_type, (ds_profile_pdp_type_e)val_ptr[1].num_item);
        CONDITIONAL_ASSIGN(3, payload->pdp_addr,  ipaddr.addr); 
        payload->pdp_addr.ip_vsn= ipaddr.ip_vsn;

        if( VALID_TOKEN(4) )
        {
          if( ATOI_OK == dsatutil_atoi(&num_val,
              &dsat_cgdcont_dcomp_valstr[val_ptr[4].num_item][0] ,10))
          {
            payload->context.d_comp = (ds_profile_data_comp_e_type)num_val;
            mask = mask|DS_PROFILE_CONTEXT_D_COMP;
          }
          else
          {          
            DS_SYSTEM_HEAP_MEM_FREE(payload);
            return DSAT_ERROR;
          }
        }

        if( VALID_TOKEN(6) )
        {
           if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 6, &val,1))
           {
             DS_SYSTEM_HEAP_MEM_FREE(payload);
             return DSAT_ERROR;
           }
           else
           {
             if(val != 0)
             {
               DS_SYSTEM_HEAP_MEM_FREE(payload);
               return DSAT_ERROR;
             }
           }
         }
        
        if( VALID_TOKEN(5) )
        {
          if( ATOI_OK == dsatutil_atoi(&num_val,
              &dsat_cgdcont_hcomp_valstr[val_ptr[5].num_item][0], 10) )
          {
            payload->context.h_comp = (ds_profile_header_comp_e_type)num_val;
            mask = mask|DS_PROFILE_CONTEXT_H_COMP;
          }
          else
          {          
            DS_SYSTEM_HEAP_MEM_FREE(payload);
            return DSAT_ERROR;
          }
        }        
        if (VALID_TOKEN(2))
        {
          (void)strlcpy((char *)payload->context.apn, 
                            (const char *)val_ptr[2].string_item,
                            DS_PROFILE_APN_STRING_LEN + 1);
        }
        
       /*----------------------------------------------
        Allocate the memory for apn and payload for both create and
        modify
       ------------------------------------------------*/
      
       memset(&ident_info ,0,sizeof(ds_profile_ident_info));

       if(VALID_TOKEN(2))
       {
            mask = mask|DS_PROFILE_APN_NAME;
       }  
       if(VALID_TOKEN(1))
       {
           ident_info.pdp_type = val_ptr[1].num_item; 
           mask = mask|DS_PROFILE_TYPE;
       }
       profile_status = ds_profile_create_ex(ident_info,
                                                   mask,
                                                   payload,
                                                   cid
                                                    );
        if(profile_status != DS_PROFILE_RESULT_SUCCESS)
        {
            DS_AT_MSG1_HIGH("Profile creation failed : %d",profile_status);
            DS_SYSTEM_HEAP_MEM_FREE(payload);
            return DSAT_ERROR;
        }
        DS_SYSTEM_HEAP_MEM_FREE(payload);
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
  
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgdcont_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_TEST))
    {
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
  
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgdcont_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_READ))
    {
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
} /* dsatetsipkt_exec_cgdcont_cmd */


/*===========================================================================

FUNCTION DSATETSIPKT_CGDCONT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGDCONT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgdcont_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte ipaddr_str[MAX_IPADDR_STR_LEN+1];
  byte  cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_list_stype            *profile_list =NULL;
  uint8                             profile_count =0;
  int                               i =0;     
  ds_profile_ident_info             ident_info;     
  ds_profile_type_s                 payload;     
  uint8                              profile_number;
  ds_profile_status_etype          profile_status;
  dsat_ip_addr_type addr;

  /* Format the command name */
  (void)snprintf((char*)name_buff,
                       sizeof(name_buff),
                       "%s: ",
                       parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  ipaddr_str[0] = '\0';
  memset(&ident_info, 0, sizeof(ds_profile_ident_info));

  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        /* PPP PDP type is not supported in CT PL */
        if( i == DS_PROFILE_PDP_PPP)
        {
          continue;
        }

        size = snprintf ((char *)res_buff_ptr->data_ptr,
                        res_buff_ptr->size,
                        "%s%s%s(%d-%d),\"%s\",%s,%s,%s,%s,(%d)",//(%d-%d)",
                        res_buff_ptr->data_ptr,
                        (i > 0) ? "\n" : "",
                        name_buff,
                        DSAT_LEGACY_PROFILE_NUM_MIN,
                        DSAT_LEGACY_PROFILE_NUM_MAX,
                        dsat_pdptype_valstr[i],
                        "",
                        "",
                        dsat_cgdcont_dcomp_tststr,
                        dsat_cgdcont_hcomp_tststr,
                        ((dflm_type *)dsat_cgdcont_ipv4_addr_alloc_dflm)->lower
                       );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      /* Report each profile on seperate line */

    /*----------------------------------------------------*/
     etsipkt_get_profile_list(&profile_list,&profile_count);
     if(profile_list ==NULL)
        return result;
	 
     memset(&payload,0,   sizeof(ds_profile_type_s));
    
     memset(&ident_info ,0,sizeof(ds_profile_ident_info));
    for(i =0 ;i<profile_count ;i++)
    { 
      memset(ident_info.apn ,0,(DS_PROFILE_APN_STRING_LEN+1));
      ident_info.pdp_type = profile_list[i].pdp_type;
   
      if(profile_list[i].apn[0] !='\0')
      {
        memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                profile_list[i].apn, sizeof(profile_list[i].apn ));
      }

     profile_status = ds_profile_get_param_ex(ident_info,&payload,&profile_number);
       DS_AT_MSG2_HIGH("PDP type = %d profile numbe= %d",payload.context.pdp_type,profile_number);       
        if(payload.mask & DS_PROFILE_TYPE)
        {        
          if(payload.context.pdp_type == DS_PROFILE_PDP_IP)
          {
            addr.ip_vsn = DS_PROFILE_IP_V4;
          }
          else if(payload.context.pdp_type == DS_PROFILE_PDP_IPV6)
          {
            addr.ip_vsn = DS_PROFILE_IP_V6;
          }
          else if(payload.context.pdp_type == DS_PROFILE_PDP_IPV4V6 )
          {
            if(payload.pdp_addr.pdp_addr.pdp_addr_ipv4 != 0)
            {
              addr.ip_vsn = DS_PROFILE_IP_V4;
            }
            else
            {
              addr.ip_vsn = DS_PROFILE_IP_V6;
            }
          }
          else if(payload.context.pdp_type == DS_PROFILE_PDP_NON_IP)
          {
            addr.ip_vsn = DS_PROFILE_NONIP;
          }
          else
          {
            DS_SYSTEM_HEAP_MEM_FREE(profile_list);
            return 0;
          }

        addr.addr = payload .pdp_addr;
          /* Decode PDP IP address if set */
        if ( DSAT_OK != dsatutil_convert_ipaddr(INT_TO_STRING,
                                              &addr,
                                                ipaddr_str) )
        {
           DS_SYSTEM_HEAP_MEM_FREE(profile_list);
           return 0;
        }
      }

        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - \
                                     DS_PROFILE_APN_STRING_LEN) );
                                      
        res_buff_ptr->used = (uint16)snprintf(
                  (char *)res_buff_ptr->data_ptr, 
                  res_buff_ptr->size,
                  "%s%s%s%d,\"%s\",\"%s\",\"%s\",%d,%d,%d",
                  res_buff_ptr->data_ptr,
                  (cntr > 0) ? "\n" : "",
                  name_buff,
                  profile_number,
                  dsat_pdptype_valstr[payload.context.pdp_type],
                  payload.context.apn,
                  ipaddr_str,
                  payload.context.d_comp,
                  payload.context.h_comp,
                  0//0 for NAS pdata.ipv4_addr_alloc,
                 );

        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        res_buff_ptr->used = (uint16)snprintf(
                                         (char *)res_buff_ptr->data_ptr,
                                         res_buff_ptr->size, "%s", name_buff);
      }
      result = res_buff_ptr->used;
      DS_SYSTEM_HEAP_MEM_FREE(profile_list);
      break;
        
  default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }
  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgdcont_response_fmt */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGCONTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGCONTRDP command.
  This command returns a set of dynamic parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_exec_cgcontrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Invalid number of Arguments in CGCONTRDP");
      result = DSAT_ERROR;
    }
    else
    {
      if( !VALID_TOKEN(0) )
      {
        /* No CID specified */
        result = etsipkt_cgcontrdp_format_response( NULL, 0,
                          tok_ptr,
                          res_buff_ptr);
      }
      else
      {
        dsat_num_item_type p_cid = 0 ;
    
        if( ATOI_OK == dsatutil_atoi (&p_cid, tok_ptr->arg[0], 10) )
        {
          /* Validate range */
          if( !DSAT_IS_PROFILE_ID_WITHIN_RANGE(p_cid) )
          {
            DS_AT_MSG1_ERROR("CID out of range value=%d",p_cid);
            result = DSAT_ERROR;
          }
        }
    
        /* Generate response */
        if( DSAT_OK == result)
        {
          result = etsipkt_cgcontrdp_format_response( (uint8 *)&p_cid, 1,
                                                    tok_ptr,
                                                    res_buff_ptr);
        }
      }
    }
  }
    /*-----------------------------------------------------------
     Processing for a command without an argument (WRITE command)
     and TEST syntax
    ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
           (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgcontrdp_format_response( NULL, 0,
                                              tok_ptr,
                                              res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
  }/* dsatetsipkt_exec_cgcontrdp_cmd */


    
#ifdef FEATURE_DSAT_ATCOP_VEND
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCPDPP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the $QCDPDP command.
  This command sets PDP authentication parameters.

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
dsat_result_enum_type dsatetsipkt_exec_qcpdpp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)

{
  #define MAX_QCPDPP_ENTRY_LEN (DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN + 20)
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type *buff_ptr = res_buff_ptr;
  byte name_buff[16];
  dsat_num_item_type val = 0;
  byte cntr = 0;
  byte cid = 0;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
  ds_profile_type_s profile_data;  
  ds_profile_ident_info ident_info ;  
  uint64                mask =0;  
  ds_profile_status_etype   profile_status = DS_PROFILE_RESULT_SUCCESS;
  ds_profile_list_stype            *profile_list =NULL;
  uint8                             profile_count =0;
  int                               i =0;     
  uint8                             profile_number;

  memset(&profile_data,0,sizeof(ds_profile_type_s));  
  memset(&ident_info,0,sizeof(ds_profile_ident_info));  
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a WRITE command 
      ---------------------------------------------------------------*/
    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
     /* if(DSAT_ERROR == dsatetsipkt_read_write_param_table(auth_param_tbl,sizeof(auth_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
              val,RESET_COMMAND,NULL,0,dsat_get_current_ds_profile_subs_id()))
      {
        return DSAT_ERROR;
      }*///Need to come up with api for this
    }
    else
    {
      byte user_buf[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
      byte pass_buf[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
      cid = (byte)val;
      
      /* Validate auth type and parameters */
      if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 1, &val,1))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 1;
        goto send_error;
      }

      memset((void*)user_buf,0,sizeof(user_buf));
      memset((void*)pass_buf,0,sizeof(pass_buf));

      /* Validate authentication strings */
      /* For a valid AUTH, need username and password */
      if ( ((byte)DS_PROFILE_AUTH_NONE != val) &&
           ((byte)DS_PROFILE_AUTH_MAX != val) )
      {
        if (VALID_TOKEN(2) && (DSAT_OK !=
            etsipkt_process_param(parse_table, tok_ptr, 2,
                             pass_buf,sizeof(pass_buf))))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 2;
          goto send_error;
        }

        if (VALID_TOKEN(3) && (DSAT_OK !=
            etsipkt_process_param(parse_table, tok_ptr, 3, 
                            user_buf,sizeof(user_buf))))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 3;
          goto send_error;
        }
        if (('\0' == pass_buf[0]) || ('\0' == user_buf[0]))
        {
          return DSAT_ERROR;
        }
       
      }
      /* For NONE, no parameters allowed */
      else if ((byte)DS_PROFILE_AUTH_NONE == val)
      {
        if (2 < tok_ptr->args_found)
        {
          return DSAT_ERROR;
        }
      }
      
      /* Validate username/password string length */
      if ((DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN < strlen((const char *)pass_buf)) ||
          (DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN < strlen((const char *)user_buf)))
      {
        DS_AT_MSG1_HIGH("Username or password exceeds %d characters",
                 DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN);
        return DSAT_ERROR;
      }

      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                        (cid,&profile_data))
        {
          return DSAT_ERROR;
        }
        mask = profile_data.mask;
        if (VALID_TOKEN(2))
        {
           (void)strlcpy(profile_data.auth.password,
                          (const char *)pass_buf,
                          DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1);
            mask = mask|DS_PROFILE_AUTH_PASSWORD;
        }      
        if (VALID_TOKEN(3))
        {
          (void)strlcpy(profile_data.auth.username,
                          (const char *)user_buf,
                          DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1);
           mask = mask|DS_PROFILE_AUTH_USERNAME;        
        }
         mask = mask|DS_PROFILE_AUTH_TYPE;
         profile_data.auth.auth_type = val;
      
       /*----------------------------------------------
        Allocate the memory for apn and payload for both create and
        modify
       ------------------------------------------------*/
     
       profile_status = ds_profile_create_ex(ident_info,
                                                   mask,
                                                  &profile_data,
                                                   cid
                                                   );
        if(profile_status != DS_PROFILE_RESULT_SUCCESS)
        {
            DS_AT_MSG1_HIGH("Profile creation failed : %d",profile_status);
            return DSAT_ERROR;
        }         
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
  
    /*---------------------------------------------------------------
      Processing for a command to TEST command
      ---------------------------------------------------------------*/
    if (DSAT_OK != dsatparm_exec_param_cmd(
                                           mode,
                                           parse_table,
                                           tok_ptr,
                                           res_buff_ptr
                                          ))
    {
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
      Processing for a READ command 
      ---------------------------------------------------------------*/
    
    /* Format the command name */
    (void)snprintf((char*)name_buff,
                         sizeof(name_buff),
                         "%s: ",
                         parse_table->name
                         );
    buff_ptr->data_ptr[buff_ptr->used] = '\0';
    /*----------------------------------------------------*/
    
    etsipkt_get_profile_list(&profile_list,&profile_count);
     if(profile_list ==NULL)
         return DSAT_ERROR;
   
    if(profile_count == 0)
    {
       buff_ptr->used =
        (word)snprintf ((char *)buff_ptr->data_ptr, 
                       buff_ptr->size,
                       "%s%s",
                       buff_ptr->data_ptr,
                       name_buff
                       );
    }
    else
    {
      for ( i = 0; i < profile_count; i++) 
      {
        memset(&ident_info ,0,sizeof(ds_profile_ident_info));
        ident_info.pdp_type = profile_list[i].pdp_type;
   
        if(profile_list[i].apn[0] !='\0')
        {
          memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                  profile_list[i].apn, sizeof(profile_list[i].apn ));
        }
        profile_status = ds_profile_get_param_ex(ident_info,&profile_data,&profile_number);

        DS_AT_MSG2_HIGH("type = %d profile = %d",profile_data.context.pdp_type,profile_number);       
      
        if (NULL == (buff_ptr = etsipkt_check_response_buffer(
                                    buff_ptr,
                                    DSM_BUFFER_THRESHOLD - MAX_QCPDPP_ENTRY_LEN)))
        {         
          DS_AT_MSG0_HIGH("Mem not found for pthis buffer: ");		  
          DS_SYSTEM_HEAP_MEM_FREE(profile_list);
          return DSAT_ERROR; 
        }
  
        /* Generate response from internal storage (excluding password) */
        buff_ptr->used =
          (word)snprintf ((char *)buff_ptr->data_ptr, 
                         buff_ptr->size,
                         "%s%s%s%d,%d",
                         buff_ptr->data_ptr,
                         (cntr > 0) ? "\n" : "",
                         name_buff,
                         profile_number,
                         profile_data.auth.auth_type
                         );
        
        /* Display username for valid auth type */
        if ( (DS_PROFILE_AUTH_NONE != profile_data.auth.auth_type) &&
             (DS_PROFILE_AUTH_MAX != profile_data.auth.auth_type) )
        {
          buff_ptr->used =
            (word)snprintf ((char *)buff_ptr->data_ptr, 
                           buff_ptr->size,
                           "%s,\"%s\"",
                           buff_ptr->data_ptr,
                           profile_data.auth.username
                           );
        }
        cntr++;
      }      
        DS_SYSTEM_HEAP_MEM_FREE(profile_list);
    }
  }
  else if (tok_ptr->op == (NA))
  {
  
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
} /* dsatetsipkt_exec_qcpdpp_cmd */
#endif
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGATT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGATT command.
  This command reports the connection status for network packet
  domain service.
  
DEPENDENCIES
  This function only supports read & test commands at this time.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgatt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*---------------------------------------------------------
    Processing for a READ & TEST command
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|QU)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    if (DSAT_OK != dsatparm_exec_param_cmd(
                                           mode,
                                           parse_table,
                                           tok_ptr,
                                           res_buff_ptr
                                          ))
    {
      result = DSAT_ERROR;
    }
  }
#ifdef FEATURE_ETSI_ATTACH
  /*---------------------------------------------------------
    Processing for a WRITE command
  -----------------------------------------------------------*/
  else if ( tok_ptr->op == (NA|EQ|AR) )
  {
    dsat_num_item_type state;
	
    /* syntax check: number of arguments should be one! */
    if (tok_ptr->args_found > 1)
    {
      return DSAT_ERROR;
    }
	
    if (ATOI_OK != dsatutil_atoi(&state, tok_ptr->arg[0], 10))
    {
      DS_AT_MSG0_HIGH("+CGATT: cannot convert state parameter");
      return DSAT_ERROR;
    }
	
    /* Attach requested */
    if ( state == 1 )
    {
      /* Ensure detached currently */ 
      if ( 0 == (dsat_num_item_type)dsatutil_get_val(
                                DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,NUM_TYPE) )
      {
        return dsatetsicmif_attach_ps ();
      }
      else
      {
        DS_AT_MSG0_HIGH("Already packet domain attached");
      }
    }
    /* Detach requested */
    else if ( state == 0 )
    {
      /* If detached currently... */ 
      if ( 1 == (dsat_num_item_type)dsatutil_get_val(
                                  DSATETSI_EXT_ACT_CGATT_ETSI_IDX,0,NUM_TYPE))
      {
        return dsatetsicmif_detach_ps ();
      }
      else
      {
        /* Even when already detached call CMPH function indicating user 
           preference CM_SRV_DOMAIN_PREF_PS_DETACH to local stack but 
           terminate processing on CMPH event. */
        DS_AT_MSG0_HIGH("Already packet domain detached");
        dsatcmif_ss_evt_not_expected = TRUE;

        if ( (result = dsatetsicmif_detach_ps ()) == DSAT_ERROR )
        {
          dsatcmif_ss_evt_not_expected = FALSE;
        }

        return result;
      }
    }
    else
    {
      /* Invalid value */
      return DSAT_ERROR;
    }
  }
#endif /* FEATURE_ETSI_ATTACH */
  else 
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgatt_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_PROC_CMD

DESCRIPTION
  This function does the CGACT processing for all the requested profile id's.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : If the response is asynchronous.
    
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_cgact_proc_cmd( void )
{
  dsat_result_enum_type result = DSAT_OK;
  
  /* 
     Loop over the 'dsat_pdp_cid' to activate all the entries with VALID CID.
     Primary profile entries exist from the begin of this array to the middle while 
     secondary profiles exist from end to the middle of this array !!
  */
  for( ;(dsat_pdp_state.change_index < dsat_pdp_state.valid_profile_count); )
  {
    if(dsat_pdp_cid[dsat_pdp_state.change_index] == INVALID_CID)
    {
      dsat_pdp_state.change_index++;
      continue;
    }

    dsat_pdp_state.pending = TRUE;
    result = ds_3gpp_rmsm_atif_cgact_handler(
           dsat_pdp_cid[dsat_pdp_state.change_index],
           (ds_3gpp_atif_cgact_act_mode_e_type)dsat_pdp_state.demand_state,
            NULL);
    
    if(result == (dsat_result_enum_type)DSAT_INVALID_PROFILE)
    {
      dsat_pdp_state.change_index++;
      continue;
    }
    if(result == DSAT_OK)
    {
      dsat_pdp_state.pending = FALSE;
      dsat_pdp_state.change_index++;
    }
    else if(result == DSAT_ERROR)
    {
      DS_AT_MSG1_ERROR("PDP request failed for CID = %d",
        dsat_pdp_cid[dsat_pdp_state.change_index]);
      break;
    }
    else if(result == DSAT_ASYNC_CMD)
    {
      DS_AT_MSG1_HIGH("PDP request for CID %d sent to Mode handler:",
        dsat_pdp_cid[dsat_pdp_state.change_index]);
      return DSAT_ASYNC_CMD;
    }
    else
    {
      DS_AT_MSG1_ERROR("PDP request failed result code is %d ",result);
      DS_ASSERT(0);
    }
  }
  /*CGATT=0, No Profile present*/
  if(result == (dsat_result_enum_type)DSAT_INVALID_PROFILE)
  {
    result = DSAT_OK;
  }
  memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));
  memset((void *)&dsat_pdp_state,0,sizeof(dsat_pdp_state));
  return result;
} /* dsatetsipkt_cgact_proc_cmd */

/*===========================================================================
FUNCTION DSATETSIPKT_AT_CMD_HANDLER

DESCRIPTION
  This function is handler function for reporting +CGACT,+CGCMOD command response.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_at_cmd_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  
  dsat_result_enum_type result   = DSAT_ERROR;
  ds_at_pdp_cmd_type *pdp_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  if(!dsatcmdp_is_etsi_cmd())
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG2_HIGH("Response for %d = %d",pdp_info_ptr->dsat_rmsm_cmd,
                  pdp_info_ptr->response);
  
  if(pdp_info_ptr->response == DSAT_OK)
  {
    result = pdp_info_ptr->response;
  }
  else
  {
    DS_AT_MSG2_HIGH("Reason_type %d,net_down %d",pdp_info_ptr->reason_type,
                       pdp_info_ptr->inst_net_down);

    if(pdp_info_ptr->reason_type == DSAT_RMSM_IFACE_NET_DOWN_REASON)
    {
      
      switch(pdp_info_ptr->inst_net_down)
      {
        case DS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED:
          result =  dsat_send_cme_error(DSAT_CME_SERV_OP_NOT_SUPPORTED);
          break;
        case DS_NET_DOWN_REASON_OPTION_TEMP_OOO:
          result = dsat_send_cme_error(DSAT_CME_SERV_OP_TEMP_OUT_OF_ORDER);
          break;
        case DS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED:
          result = dsat_send_cme_error(DSAT_CME_REQ_SERV_OP_NOT_SUBSCRIBED);
          break;
        case DS_NET_DOWN_REASON_AUTH_FAILED:
          result = dsat_send_cme_error(DSAT_CME_PDP_AUTH_FAILURE);
          break;
        case DS_NET_DOWN_REASON_NO_SRV:
        case DS_NET_DOWN_REASON_NO_GW_SRV:
           result = dsat_send_cme_error(DSAT_CME_NO_NETWORK_SERVICE);
           break;
        default:
          result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
        
      }
    }
    else
    {
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }
  }

  return result;
}/* dsatetsipkt_at_cmd_handler */

/*===========================================================================
FUNCTION DSATETSIPKT_CGACT_DONE_HANDLER

DESCRIPTION
  This function is called when +CGACT processing for a profile 
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatetsipkt_cgact_done_handler
(
  dsat_rmsm_info_s_type  dsat_rmsm_info,
  void* user_info_ptr
)
{
  ds_command_type           *cmd_buf;
  ds_at_pdp_cmd_type    *pdp_info_ptr = NULL;
  dsat_result_enum_type  response  = dsat_rmsm_info.result;

  DS_AT_MSG1_HIGH("CGACT done handler, result = %d",response);
  if(dsat_pdp_state.pending == TRUE)
  {
    dsat_pdp_state.pending = FALSE;
    if(response == DSAT_ERROR)
    {
      memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));
      memset((void*)&dsat_pdp_state,0,sizeof(dsat_pdp_state));
    }
    else if(response == DSAT_OK)
    {
      dsat_pdp_state.change_index++;
      response = dsatetsipkt_cgact_proc_cmd();
      if(response == DSAT_ASYNC_CMD)
      {
        return;
      }
      dsat_reg_atz_cb (dsatetsicmif_context_activation_atz_cb);
    }
    else
    {
      DS_AT_MSG0_ERROR("No pending command, ignore");
      return;
    }

    cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_pdp_cmd_type), FALSE);
    pdp_info_ptr = cmd_buf->cmd_payload_ptr;

    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PDP_CMD;

    pdp_info_ptr->response         = response;
    pdp_info_ptr->reason_type      = dsat_rmsm_info.reason_type;
    pdp_info_ptr->inst_net_down   = dsat_rmsm_info.reason.iface_net_down;
    pdp_info_ptr->dsat_rmsm_cmd    = DSAT_RMSM_CGACT_CMD;
    ds_put_cmd(cmd_buf);
  }
  else
  {
    DS_AT_MSG0_ERROR("No pending +CGACT command");
  }
} /* dsatetsipkt_cgact_done_handler */
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGACT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGACT command.
  This command activates or deactivates PDP context profiles.
  
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
dsat_result_enum_type dsatetsipkt_exec_cgact_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  uint8 loop;
  
  /*---------------------------------------------------------
    Processing for a READ command
  -----------------------------------------------------------*/
  if ( tok_ptr->op == (NA|QU) )
  {
      //@SANJAYKC: TODO
      ds_3gpp_atif_profile_status_info_type profile_status;
      uint16 cntr = 0;
      uint32 length = 0;

      memset(&profile_status,0,sizeof(profile_status));

      if(DSAT_OK != ds_3gpp_rmsm_atif_cgact_query(&profile_status))
      {
        DS_AT_MSG0_ERROR("CGACT query failed");
        return DSAT_ERROR;
      }
      else if (profile_status.profile_count > DSAT_MAX_ALLOWED_PROFILES)
      {
        DS_AT_MSG0_ERROR("Invalid Profiles provided by 3GPP ATIF");
        return DSAT_ERROR;
      }

      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
      DS_AT_MSG1_HIGH("CGACT query Total_Profile: %d ",profile_status.profile_count);
  
      /*Here we support max 24 profiles but there can be only max 8 PDN up at a time*/
      for (loop=0; loop < profile_status.profile_count; loop++) 
        {
        DS_AT_MSG1_HIGH("CGACT query executing, profile_num: %d ",profile_status.list[loop].profile_num);
        
        /* Generate response from data structure */
        length = snprintf ( (char *)res_buff_ptr->data_ptr,
           res_buff_ptr->size,
           "%s%s%s: %lu,%d",
           res_buff_ptr->data_ptr,
           (cntr++ > 0) ? "\n" : "",
           "+CGACT",
           profile_status.list[loop].profile_num,
           (profile_status.list[loop].act_status == DS_3GPP_ATIF_PDP_ACTIVATED)?
           DS_3GPP_ATIF_ACTIVATION : DS_3GPP_ATIF_DEACTIVATION );
      }
           /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        length = snprintf((char *)res_buff_ptr->data_ptr, 
                                res_buff_ptr->size,"%s: ", "+CGACT");
      }
      res_buff_ptr->used = (uint16)length;
      return DSAT_OK;
    }
  /*---------------------------------------------------------
    Processing for a WRITE command
  -----------------------------------------------------------*/
  else if ( tok_ptr->op == (NA|EQ|AR) )
  {
    boolean state_changed = FALSE;
    /* Build the processing order for CID list */
    if ( !etsipkt_build_context_processing_list(tok_ptr, 
                                                DSAT_CQM_CGACT_WRITE, 
                                                &state_changed) )
    {
      DS_AT_MSG0_ERROR("Error building CGACT processing order list");
      return DSAT_ERROR;
    }
    
    if(dsat_pdp_state.valid_profile_count == 0)
    {
      memset ((void*)&dsat_pdp_state, 0x0,sizeof(dsat_pdp_state));
      DS_AT_MSG0_ERROR("Empty CGACT processing order list");
      return DSAT_OK;
    }
    
    return dsatetsipkt_cgact_proc_cmd();
  }
  /*---------------------------------------------------------
    Processing for a TEST command
  -----------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    if (DSAT_OK != dsatparm_exec_param_cmd(
                                           mode,
                                           parse_table,
                                           tok_ptr,
                                           res_buff_ptr
                                          ))
    {
      result = DSAT_ERROR;
    }
  }
  else 
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgact_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_RESPONSE_FMT

DESCRIPTION
  This function generates +CGACT response for AMPV command  
  commands.  Data is queried from ModeHandlers and formatted according
  to parameter flag.

DEPENDENCIES
  Query to Mode Handlers.

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_num_item_type dsatetsipkt_cgact_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16] = "+CGACT";
  int size = 0;
  //@SANJAYKC: todo
  ds_3gpp_atif_profile_status_info_type profile_status;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  uint8 loop;
  byte cntr = 0;

  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  if( format != MULTI_INDEX_AMPV )
  {
    DS_AT_MSG0_HIGH("UnSupported format");
    return DSAT_ERROR;
  }

  size = res_buff_ptr->used;
  result = (word)snprintf((char *)res_buff_ptr->data_ptr,
                             res_buff_ptr->size,
                             "%s%s",
                             res_buff_ptr->data_ptr,
                             name_buff);
  memset(&profile_status,0,sizeof(profile_status));
		
  if(DSAT_OK != ds_3gpp_rmsm_atif_cgact_query(&profile_status))
  {
    DS_AT_MSG0_ERROR("CGACT query failed for AMPV Command");
    return DSAT_ERROR;
  }
  DS_AT_MSG1_HIGH("CGACT query Total_Profile: %d ",profile_status.profile_count);

  /* Got profile_status and add CID of each 3GPP profile to response list... */
  for (loop = 0; loop < profile_status.profile_count; loop++) 
    {
      /* Check to see if a new DSM item is required */
      CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
      result = (word)snprintf ((char *)res_buff_ptr->data_ptr, 
                     res_buff_ptr->size,
                   "%s%s(%lu,%d)",
                     res_buff_ptr->data_ptr,
                     (cntr++ > 0) ? "," : "",
                   profile_status.list[loop].profile_num,
                   (profile_status.list[loop].act_status == DS_3GPP_ATIF_PDP_ACTIVATED)?
                    DS_3GPP_ATIF_ACTIVATION : DS_3GPP_ATIF_DEACTIVATION );
  }
  /* caller increments res_buff_ptr->used */
  result -= size;
  DS_AT_MSG0_HIGH("CGACT - AMPV Query response");
  return (dsat_num_item_type)result;
} /* dsatetsipkt_cgact_response_fmt */

/*===========================================================================

FUNCTION DSATETSIPKT_RESET_PDP_CONTEXT_PROFILES

DESCRIPTION
  This function resets all persistent PDP context profiles to their
  default values.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in execution
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_reset_pdp_context_profiles ( void )
{
#ifdef FEATURE_ATCOP_TEMP_PROFILE_TODO
  byte i;
  dsat_result_enum_type result = DSAT_OK;
  ds_profile_num_type num;
  uint8     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8     total_profiles = 0;

  if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                             sizeof(profile_list), &total_profiles,
                                             dsat_get_current_ds_profile_subs_id()) )
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
  }
  for (i = 0; i < total_profiles; i++) 
  {
    /* Perform reset for specific profile */
    RESET_BASE_CONTEXT_PARAMS(profile_list[i]);
    if(result == DSAT_ERROR)
    {
      return DSAT_ERROR;
    }
  }
  if(DS_PROFILE_RESULT_SUCCESS != ds_profile_get_default_profile_num(DS_PROFILE_TECH_3GPP,
                 DS_PROFILE_EMBEDDED_PROFILE_FAMILY,&num))
  {
    DS_AT_MSG0_HIGH("Error getting default embedded profile: %d");
    return DSAT_ERROR;
  }
  /* RESET default socket profile to default values */
  if(DS_PROFILE_RESULT_SUCCESS == ds_profile_create(DS_PROFILE_TECH_3GPP,&num))
  {
    if(ds_profile_reset_profile_to_default_per_sub(DS_PROFILE_TECH_3GPP,num,dsat_get_current_ds_profile_subs_id()) != DS_PROFILE_RESULT_SUCCESS)
    {
      DS_AT_MSG1_HIGH("Error defaulting profile: %d",DS_UMTS_DEF_SOCKET_PROFILE);
      return DSAT_ERROR;
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Error defaulting profile: %d",DS_UMTS_DEF_SOCKET_PROFILE);
    return DSAT_ERROR;
  }
  if(DS_PROFILE_RESULT_SUCCESS != ds_profile_set_default_profile_num(DS_PROFILE_TECH_3GPP,
               DS_PROFILE_EMBEDDED_PROFILE_FAMILY,num))
  {
    DS_AT_MSG0_HIGH("Error setting default embedded profile: %d");
    return DSAT_ERROR;
  }
  return DSAT_OK;
#else
  return DSAT_ERROR;
#endif
} /* dsatetsipkt_reset_pdp_context_profiles */

/*===========================================================================

FUNCTION ETSIPKT_VALIDATE_APN

DESCRIPTION
  This function validates the format of an access point name (APN)
  per specification TS23.003 section 9.1

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
LOCAL dsat_result_enum_type etsipkt_validate_apn
(
  byte *s_ptr                             /*  Points to string  */
)
{
  byte temp_buf[DS_PROFILE_APN_STRING_LEN+1];
  byte c;
  byte * w_ptr = s_ptr;
  int i, cnt = 0;

  /* Do nothing with empty string */
  if ((0 == s_ptr) || ('\0' == *s_ptr))
    return DSAT_OK;

  /* Strip quotes if required */
  if ('"' == *s_ptr)
  {
    (void)dsatutil_strip_quotes_out(s_ptr, temp_buf,
                                    (word)strlen((const char*)s_ptr));
    w_ptr = temp_buf;
  }
 
  /* Process until end of string */
  while ('\0' != *(w_ptr+cnt))
  {
    c = UPCASE(*(w_ptr+cnt)); 

    /* Check for valid character */
    if ( ! ((('A' <= c ) && ('Z' >= c)) ||
            (('0' <= c ) && ('9' >= c)) ||
            ('-' == c) ||
            ('.' == c)) )
    {
      DS_AT_MSG1_HIGH("Invalid character in APN: %c", c);
      return DSAT_ERROR;
    }
    cnt++;
  }

  if ( cnt == 1 )
  {
    /* Following TS23003 and RFC 1123, the label shall begin and end with either
       an alphabetic character or a digit. APN Name as '-' or '.' is not allowed */
    if('-' == c || '.' == c)
    {
      return DSAT_ERROR;
    } 
    /* We received a single character for APN */
    /* No need for further error checks       */
    return DSAT_OK;
  }

  /* Check first and last characters */
  for (i = 0; i < cnt; i += cnt-1)
  {
    c = UPCASE(*(w_ptr+i));
    if ( ! ((('A' <= c ) && ('Z' >= c)) ||
            (('0' <= c ) && ('9' >= c))) )
    {
      DS_AT_MSG1_HIGH("Invalid begin/end character in APN: %c", c);
      return DSAT_ERROR;
    }
  }
  
  /* Check APN length < maximum (100) */
  if (DS_PROFILE_APN_STRING_LEN < cnt)
  {
      DS_AT_MSG1_HIGH("APN exceeds maximum length: %d", cnt);
      return DSAT_ERROR;
  }
  
  return DSAT_OK;
} /* etsipkt_validate_apn */




/*===========================================================================

FUNCTION ETSIPKT_PROCESS_PARAM

DESCRIPTION
  This function performs multitype parameter set processing/conversion.
  The target parameter to process is passed as an index.  The result is
  passed back to the caller by reference.
  If no string value was specified, the previous value is returned.

DEPENDENCIES
  The caller must allocate sufficient storage for string result buffer.

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_ERROR : if there was any problem
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_process_param
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  const uint32 pindex,                  /*  Parameter index             */
  void * result_ptr,                     /*  Pointer to result           */  
  uint16 result_buff_len           /*  Length of result buffer */
)
{
  word radix;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,STR_TYPE);
  mixed_def_s_type ** def_lim_pptr = 
    ( mixed_def_s_type **) parse_table->def_lim_ptr;
  boolean is_range_valid = FALSE;
  if(val_ptr ==NULL)
  	return DSAT_ERROR;
  /* Check for user specified value */
  if ( VALID_TOKEN(pindex) &&
      ((pindex+1) <= tok_ptr->args_found))
  {
    /* Check for list attribute */
    if (0 != (def_lim_pptr[pindex]->attrib & LIST))
    {
      /* Lookup position in list */
      const def_list_type * def_list_ptr = &def_lim_pptr[pindex]->def->def_list;
	  param_srch_enum_type srchrc = 
      dsatutil_mixed_param_set_num_arg(result_ptr, def_list_ptr,
                                         tok_ptr->arg[pindex],
                                         tok_ptr->arg_length[pindex],
                                         def_lim_pptr[pindex]->attrib);
      
      /* Continue if parameter found */
      if (PARAM_NULL != srchrc)
      {
        if(PARAM_MISS == srchrc)
        {
          DS_AT_MSG1_HIGH("Cannot find in list parameter %d",pindex);
          return DSAT_ERROR;
        }
      }
    }
    /* Check for STRING attribute */
    else if (0 != (def_lim_pptr[pindex]->attrib & STRING))
    {
      word len = (word) strlen((char*)tok_ptr->arg[pindex]);
      if( len > def_lim_pptr[pindex]->def->string_len )
      {
        DS_AT_MSG0_HIGH("The size of the parameter is greater ");
        return DSAT_ERROR;
      }
      /* if the argument is quoted, strip out the quotes */
      if ((((def_lim_pptr[pindex]->attrib & YES_QUOTE)!= 0) ||
           ((def_lim_pptr[pindex]->attrib & NO_QUOTE) != 0)) &&
          ('"' == *tok_ptr->arg[pindex]))
      {
        if(!dsatutil_strip_quotes_out((const byte*)tok_ptr->arg[pindex],
                                      (byte*)result_ptr, len))
        {
          return DSAT_ERROR;        
        }
      }
      else
      {
        (void)strlcpy ((char*)result_ptr, (const char*)tok_ptr->arg[pindex],
                           result_buff_len);
       }
    }
    /* Assume numeric otherwise */
    else 
    {
      /* Convert to number */
      radix = ( (def_lim_pptr[pindex]->attrib & HEX) ? 16 : 10);
      if (ATOI_OK != dsatutil_atoi(result_ptr, tok_ptr->arg[pindex], radix))
      {
        DS_AT_MSG1_HIGH("Cannot convert parameter %d",pindex);
        return DSAT_ERROR;
      }

      /* Check against limits */
      if ( def_lim_pptr[pindex]->attrib & BROKEN_RANGE )
      {
        const mixed_def_u_type *def = def_lim_pptr[pindex]->def;

        while( def->dflm.default_v != MAX_BROKEN_RANGE &&
               def->dflm.lower != def->dflm.upper )
        {
          if ( (def->dflm.lower <= *(uint32 *)result_ptr) &&
               (def->dflm.upper >= *(uint32 *)result_ptr) )
          {
            is_range_valid = TRUE;
            break;
          }
          def++;
        }
      }
      else
      {
        if ( (def_lim_pptr[pindex]->def->dflm.lower <= *(uint32 *)result_ptr) &&
             (def_lim_pptr[pindex]->def->dflm.upper >= *(uint32 *)result_ptr) )
        {
          is_range_valid = TRUE;
        }
      }
      if( !is_range_valid )
      {
        DS_AT_MSG1_HIGH("Value outside range: %d",*(int32 *)(result_ptr));
        return DSAT_ERROR;
      }
    }
  }
  else
  {
    /* Load previous value */
    if (0 != (def_lim_pptr[pindex]->attrib & STRING))
    {
      /* String values (quotes already stripped) */
      (void)strlcpy ((char*)result_ptr, (const char*)val_ptr[pindex].string_item,
                         result_buff_len);
     }
    else
    {
      /* Numeric & LIST values */
      *(dsat_num_item_type *)(result_ptr) = val_ptr[pindex].num_item;
    }
  }
  return DSAT_OK; 
} /* etsipkt_process_param */


/*===========================================================================

FUNCTION ETSIPKT_DELTE_PROFILE_CONTEXT

DESCRIPTION
  This function finds profile delete it.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/


LOCAL boolean etsipkt_delete_profile_context
(
  const byte   primary_id  /* Primary profile identifier */
)
{
  if(ds_profile_delete_ex (primary_id) != DS_PROFILE_RESULT_SUCCESS)
  { 
     DS_AT_MSG1_HIGH("Error on profile context reset: %d",primary_id);
     return FALSE;
  }
  return TRUE;

}/*etsipkt_delete_profile_context*/

/*===========================================================================

FUNCTION  ETSIPKT_CHECK_RESPONSE_BUFFER

DESCRIPTION
  This function checks the lenth of the response buffer to see if
  another must be chained to it.  A buffer limit value of zero means
  use normal DSM buffer default.

DEPENDENCIES
  None

RETURN VALUE
  returns pointer to current DSM item buffer on success; NULL otherwise

SIDE EFFECTS
  DSM item may be chained to passed in buffer, moving input pointer
  to new item.
  
===========================================================================*/
LOCAL dsm_item_type* etsipkt_check_response_buffer
(
  dsm_item_type  *res_buff_ptr,   /* Response buffer */
  uint16          buf_limit       /* Buffer contents limit */
)
{
  dsm_item_type  *cur_buff_ptr = res_buff_ptr;

  DS_ASSERT (NULL != res_buff_ptr);

  /* Use default DSM packet limit if necessary */
  if (0 == buf_limit)
  {
    buf_limit = DSM_BUFFER_THRESHOLD;
  }
  
  /* Append new buffer if required. */
  cur_buff_ptr = dsatutil_append_dsm_item( cur_buff_ptr,
                                           buf_limit );
  /* If buffer appended, ensure null terminated for string functions. */
  if ( cur_buff_ptr != res_buff_ptr)
  {
    cur_buff_ptr->data_ptr[cur_buff_ptr->used]   = '\0';
  }

  return cur_buff_ptr;
} /* etsipkt_check_response_buffer */


/*===========================================================================

FUNCTION  ETSIPKT_BUILD_CONTEXT_PROCESSING_LIST

DESCRIPTION
  This function builds the processing order for +CGACT and +CGCMOD list of 
  context profiles.  The order of activation is all primary contexts before
  any secondary.  This avoids unnecessary failures should a dependent
  secondary context attempt activation before the primary yet both are
  in user specified list. 
  *state_changed_ptr only set when type_list is DSAT_CQM_CGACT_WRITE.
   
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful 
    FALSE : otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_build_context_processing_list
(
  const tokens_struct_type *tok_ptr,     /* Command tokens from parser   */
  dsat_call_query_mode_e_type type_list, /* For which command */
  boolean * state_changed_ptr            /* Pointer to state change flag */
)
{
  byte                    prim_ctxlist[DSAT_MAX_ALLOWED_PROFILES];
  boolean no_cid_list;
  byte cid_limit = 0;
  dsat_num_item_type demand_state = (dsat_num_item_type)DSAT_PDP_CONNECT_MAX;
  dsat_num_item_type prim_profile_count = 0;
  uint8                   profile_cid[DSAT_MAX_ALLOWED_PROFILES] = {0};
  ds_profile_type_s      profile ;
  ds_profile_list_stype            *profile_list =NULL;
  uint8                             profile_count =0;
  int                               i =0;
  ds_profile_ident_info             ident_info ;     
  ds_profile_type_s                 payload;     
  uint8                             profile_number;
  uint8                             total_profiles=0;

  DS_ASSERT (NULL != tok_ptr);
  memset(&ident_info,0, sizeof(ds_profile_ident_info));

  switch ( type_list )
  {
    case DSAT_CQM_CGACT_WRITE:
      *state_changed_ptr = FALSE;
      /* For +CGACT write command the first parm is state */
      no_cid_list = (1 == tok_ptr->args_found);
    break;

    case DSAT_CQM_CGCMOD_WRITE:
    /* For +CGCMOD write command there is only CID list */
      no_cid_list = (0 == tok_ptr->args_found);
    break;

    default:
      return FALSE;
  }
  
  memset((void*)prim_ctxlist, 0x0, sizeof(prim_ctxlist));
  
  if(DSAT_ERROR == etsipkt_get_profile_list(&profile_list,&profile_count))
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    return FALSE;
  }

   if (profile_count > DSAT_MAX_ALLOWED_PROFILES)
   {
     DS_AT_MSG1_ERROR("Profile count (%d) exceeded max value!", profile_count);	 
     DS_SYSTEM_HEAP_MEM_FREE(profile_list);
     return FALSE;
   }
   total_profiles = profile_count;

   memset(&ident_info ,0,sizeof(ds_profile_ident_info));
   
   for( i = 0; i < total_profiles ; i++)
   {
      memset(&ident_info, 0, sizeof(ds_profile_ident_info));
      
      profile_number =0;
      ident_info.pdp_type = profile_list[i].pdp_type;
   
      if(profile_list[i].apn[0] !='\0')
      {
        memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                profile_list[i].apn, sizeof(profile_list[i].apn ));

      }
      ds_profile_get_param_ex(ident_info,&payload,&profile_number);
      profile_cid[i] = profile_number;
   } 

  if ( type_list == DSAT_CQM_CGACT_WRITE )
  {
    cid_limit = (byte)((no_cid_list)? profile_count: (tok_ptr->args_found-1));

    /* Convert state parameter string to integer */
    if ( ATOI_OK != dsatutil_atoi(&demand_state, tok_ptr->arg[0], 10))
    {
      DS_SYSTEM_HEAP_MEM_FREE(profile_list);          
      DS_AT_MSG1_ERROR("Cannot convert to integer parameter: %d",0);
      return FALSE;
    }

    if ((dsat_num_item_type)DSAT_PDP_CONNECT_MAX <= demand_state)
    {
      DS_AT_MSG1_ERROR("Demand state invalid: %d",demand_state);
      DS_SYSTEM_HEAP_MEM_FREE(profile_list);          
      return FALSE;
    }
  }
  else if ( type_list == DSAT_CQM_CGCMOD_WRITE )
  {
    cid_limit = (byte)( no_cid_list ? profile_count:tok_ptr->args_found);
  }
    
  if ( no_cid_list == FALSE )
  {
    memset( &profile_cid[0], 0x00, sizeof(profile_cid) );
    total_profiles = 0;
    for ( i = 1; i <= cid_limit; i++)
    {
      dsat_num_item_type cid = 0;
      
      /* First parameter is <state> for +CGACT, 
         start of <cid> list for +CGCMOD */
      byte *parm_str = (type_list == DSAT_CQM_CGACT_WRITE) ? 
      tok_ptr->arg[i] : tok_ptr->arg[i-1];
      if (ATOI_OK != dsatutil_atoi(&cid, parm_str, 10))
      {
        DS_AT_MSG1_ERROR("Cannot convert to integer parameter: %d",i);
        DS_SYSTEM_HEAP_MEM_FREE(profile_list);          
        return FALSE;
      }
      /* Test for CID within valid range */
      if (!DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
      {
        DS_AT_MSG1_ERROR("Connection ID invalid: %d",cid);
        DS_SYSTEM_HEAP_MEM_FREE(profile_list);          
        return FALSE;
      }                                                 

      if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id(cid,&profile))
        {
          DS_AT_MSG1_ERROR("Profile id %d is not a 3gpp profile",cid);         
          DS_SYSTEM_HEAP_MEM_FREE(profile_list);          
          return FALSE;
        }
      profile_cid[i-1] = (uint8) cid;
      total_profiles++;
    }
      }

     memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));
     memset ((void*)&dsat_pdp_state, 0x0,sizeof(dsat_pdp_state));
  /* Loop over each cid to build list of profiles to work on */
  /* Limit loop to maximum number of supported context profiles */

  for ( i = 0; i < total_profiles ; i++)
  {
    dsat_pdp_cid[i] = profile_cid[i];
  } /* for */
  
  dsat_pdp_state.change_index = 0;
    dsat_pdp_state.valid_profile_count                = total_profiles;

    /* Copy to structure */
    DS_AT_MSG2_HIGH("Valid PDP profile count:%d, prim_count:%d ", 
                total_profiles, prim_profile_count);
  
  if ( type_list == DSAT_CQM_CGACT_WRITE )
  {
    dsat_pdp_state.demand_state = (dsat_pdp_connect_state_e_type)demand_state;
  }
  DS_SYSTEM_HEAP_MEM_FREE(profile_list); 
  return TRUE;
} /* etsipkt_build_context_processing_list */

/*===========================================================================

FUNCTION  ETSIPKT_PRINT_IPV4_ADDR

DESCRIPTION
  This function prints IPV4 address in the required format.
   
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_SUCCESS : if processing successful 
    DSAT_FAILURE : otherwise

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean etsipkt_print_ipv4_addr
(
  dsat_ip_addr_type      *pdp_adr_ptr,
  uint32                  src_adr,
  boolean                 is_static_addr,
  dsm_item_type           *res_buff_ptr,
  char                    *buffer
)
{
  pdp_adr_ptr->ip_vsn = DS_PROFILE_IP_V4; 
  if( FALSE == is_static_addr ) 
  {
    pdp_adr_ptr->addr.pdp_addr.pdp_addr_ipv4 = ps_ntohl(src_adr);
  }
  else 
  { 
    pdp_adr_ptr->addr.pdp_addr.pdp_addr_ipv4 = src_adr;
  }
  if( dsatutil_convert_ipaddr(INT_TO_STRING, pdp_adr_ptr,(byte *)buffer) != DSAT_OK )
  {
    return DSAT_FAILURE;
  } 
  else
  {
    res_buff_ptr->used += (word) snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "%s",buffer);
  }
  return DSAT_SUCCESS;
} /* etsipkt_print_ipv4_addr */

/*===========================================================================

FUNCTION  ETSIPKT_PRINT_IPV6_ADDR

DESCRIPTION
  This function prints IPV6 address in the required format.
   
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_SUCCESS : if processing successful 
    DSAT_FAILURE : otherwise

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean etsipkt_print_ipv6_addr
(
  dsat_ip_addr_type                       *pdp_adr_ptr,
  ds_profile_addr_type_ipv6               *src_adr_ptr,
  boolean                                 is_static_addr,
  dsm_item_type                           *res_buff_ptr,
  char                                    *buffer
)
{
  pdp_adr_ptr->ip_vsn = DS_PROFILE_IP_V6;
  if( FALSE == is_static_addr )
  {
    pdp_adr_ptr->addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0] = ps_ntohll(src_adr_ptr->in6_u.u6_addr64[1]);
    pdp_adr_ptr->addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1] = ps_ntohll(src_adr_ptr->in6_u.u6_addr64[0]);
  }
  else
  {
    pdp_adr_ptr->addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0] = src_adr_ptr->in6_u.u6_addr64[0];
    pdp_adr_ptr->addr.pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1] = src_adr_ptr->in6_u.u6_addr64[1];
  }
  if( dsatutil_convert_ipaddr(INT_TO_STRING,pdp_adr_ptr,(byte *)buffer) != DSAT_OK )
  {
    return DSAT_FAILURE;
  }
  else
  {
    res_buff_ptr->used += (word) snprintf( (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                           res_buff_ptr->size - res_buff_ptr->used,
                                           "%s",buffer);
  }
  return DSAT_SUCCESS;
} /* etsipkt_print_ipv6_addr */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEREP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEREP command.
  This command reports (buffers) certain GPRS events when occured viz.,
  detach, PDP deactivate, class change.

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
dsat_result_enum_type dsatetsipkt_exec_cgerep_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  dsat_num_item_type cgerep_val ;
  dsatetsipkt_gprs_msg_info * gprs_dd_val = NULL;

  gprs_dd_val = dsat_get_base_addr(DSAT_MD_VALS, FALSE);
 
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if((DSAT_OK == result) && (tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA)))
  {
    dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_CGEREP_MODE, 
                                 DSATETSI_EXT_CGEREP_IDX, 
                                 0);
    dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_CGEREP_BFR, 
                                 DSATETSI_EXT_CGEREP_IDX, 
                                 1);
    if(VALID_TOKEN(1))
    {
      cgerep_val =(dsat_num_item_type)dsatutil_get_val(
                                         DSATETSI_EXT_CGEREP_IDX,0,NUM_TYPE);
      if(cgerep_val == DSAT_CGEREP_BUFFER_DISCARD_FWD_TE ||
         cgerep_val == DSAT_CGEREP_BUFFER_FWD_TE )
      {
        cgerep_val =(dsat_num_item_type)dsatutil_get_val(
                                         DSATETSI_EXT_CGEREP_IDX,1,NUM_TYPE);
        if(cgerep_val == DSAT_CGEREP_EMPTY_BUFFER)
        {
          /* Clearing the event buffer */
          etsipkt_clear_gprs_events_buffer();
        }
        else
        {
          gprs_dd_val->flush_gprs_buffer = TRUE;
        }
      }
    }
  }

  return result;
} /* dsatetsipkt_exec_cgerep_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CSODCP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CSODCP command.
  This command is used to send originating data over control plane
  to the network.

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

dsat_result_enum_type dsatetsipkt_exec_csodcp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
/* Maxmimum allowed command line buffer is 2100; We have put 100 as placeholder for 
   command name and other parameters ; 2000 is placeholder for cp_data.
   So maximum number of bytes is 2000/2 = 1000 */
#define DSAT_MAX_CP_DATA_LENGTH 1000
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type cid = 0 ;
  dsat_num_item_type cp_data_len = 0;
  dsat_num_item_type rai = (dsat_num_item_type)CSODCP_RAI_NO_INFORMATION_AVAILABLE;
  dsat_num_item_type type_of_user_data = (dsat_num_item_type)CSODCP_DATA_TYPE_REGULAR;
  dsm_item_type *tx_data = NULL;
  dsm_item_type **tx_data_dptr = NULL;
  uint8 *cp_data_buf = NULL;
  uint8 local_error = 0;

  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if (tok_ptr->args_found < 3 || tok_ptr->args_found > 5 )
    {
      /* Invalid number of arguments */
      result = DSAT_ERROR;
      local_error = 1;
      goto bail;
    }

    if ( !VALID_TOKEN(0) || !VALID_TOKEN(1) || !VALID_TOKEN(2))
    {
      /* No CID specified , data length should not be empty*/
      result = DSAT_ERROR;
      local_error = 2;
      goto bail;
    }
    else
    {
      if( ATOI_OK == dsatutil_atoi (&cid, tok_ptr->arg[0], 10) )
      {
        /* Validate range */
        if( !DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid) )
        {
          /* profile number out of range */
          result = DSAT_ERROR;
          local_error = 3;
          goto bail;
        }
      }
      else
      {
        /* invalid context ID */
        result = DSAT_ERROR;
        local_error = 4;
        goto bail;
      }

      if( ATOI_OK != dsatutil_atoi (&cp_data_len, tok_ptr->arg[1], 10) || 
          cp_data_len > DSAT_MAX_CP_DATA_LENGTH )
      {
         /* invalid cp data length */
         result = DSAT_ERROR;
         local_error = 5;
         goto bail;
      }

      if(((tok_ptr->arg_length[2]) % 2) == 1 || 
          /* Even number of characters should be there for hex string.
             Null string would have already validated at VALID_TOKEN(2).
             Empty hex(only with quotations) string will be taken care later.  */
          cp_data_len != ((tok_ptr->arg_length[2] - 2)/2)) 
          /* cp_data_len and length of hex string (without quotation) must match */
      {
        /* Incomplete octet string */
        /* Mismatch cp data length between given and actual data length */
        result = DSAT_ERROR;
        local_error = 6;
        goto bail;
      }

      /* Strip quotes of cp data */
      if ( !dsatutil_strip_quotes_out( tok_ptr->arg[2], tok_ptr->arg[2], tok_ptr->arg_length[2] ) )
      {
        /* without quotes */
        result = DSAT_ERROR;
        local_error = 7;
        goto bail;
      }

      if(tok_ptr->arg_length[2] == 2)
      {
        /* Empty data */
        goto validate_other;
      }

      /* Allocate buffer for data to be sent; we will put hexstring as bytes in buffer */
      DS_SYSTEM_HEAP_MEM_ALLOC(cp_data_buf,
                                cp_data_len,
                                uint8 *);
      if(!cp_data_buf ||
         !dsatutil_hex_string_to_bytes((char*)tok_ptr->arg[2], cp_data_len, cp_data_buf))
      {
        DS_AT_MSG1_ERROR("dsm allocation failed 0x%x or invalid hex string", cp_data_buf);
        result = DSAT_ERROR;
        local_error = 8;
        goto bail;
      }

      /* Allocate DSM item for data payload */
      if ( NULL == ( tx_data = dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL)))
      {
        /* dsm allocation failed for tx data */
        result = DSAT_ERROR;
        local_error = 9;
        goto bail;
      }

      tx_data_dptr = &tx_data;

      if ( cp_data_len != dsm_pushdown_packed( tx_data_dptr,
              (void*)cp_data_buf, cp_data_len, DSM_DS_LARGE_ITEM_POOL))
      {
        /* copy failed for tx data */
        result = DSAT_ERROR;
        local_error = 10;
        goto bail;
      }

validate_other:
      if ( VALID_TOKEN(3))
      {
        if( ATOI_OK != dsatutil_atoi (&rai, tok_ptr->arg[3], 10) ||
            rai >= CSODCP_RAI_MAX)
        {
           /* invalid RAI */
           result = DSAT_ERROR;
           local_error = 11;
           goto bail;
        }
      }
      
      if ( VALID_TOKEN(4))
      {
        if( ATOI_OK != dsatutil_atoi (&type_of_user_data, tok_ptr->arg[4], 10) ||
            type_of_user_data >= CSODCP_DATA_TYPE_MAX )
        {
           result = DSAT_ERROR;
           local_error = 12;
           goto bail;
        }
      }

      /* call MH to send data over control plane */
      result = ds_3gpp_rmsm_atif_csodcp_hdlr(cid, cp_data_len, tx_data_dptr, rai, type_of_user_data);

      if ( DSAT_ASYNC_CMD == result )
      {
        SET_PENDING(DSATETSI_EXT_CSODCP_IDX, 0, DSAT_PENDING_TRUE );
      }
bail: 
      DS_AT_MSG4_HIGH("result %d with local_error %d cp_data_buf 0x%x tx_data 0x%x",
                       result, local_error, cp_data_buf, tx_data );


      /* During ASYNC, buffer will be needed by RMSM-->Dont free dsm item.
         RMSM or FWK should take care of freeing it.
         Other Pass case or fail case, free the dsm buffer */
      if(tx_data && DSAT_ASYNC_CMD != result)
      {
        dsm_free_packet(&tx_data);
      }
      if(cp_data_buf)
      {
        DS_SYSTEM_HEAP_MEM_FREE(cp_data_buf);
      }

      return result;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*
      +CSODCP: (range of supported <cid>s),(maximum number of octets of user data indicated 
                by <cpdata_length>), (list of supported <RAI>s),(list of supported 
                <type_of_user_data>s)
    */
    res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';	
    res_buff_ptr->used = snprintf ((char *)res_buff_ptr->data_ptr,
				  res_buff_ptr->size,
				  "+CSODCP: (%d-%d),%d,(0-2),(0-1)",
				  DSAT_LEGACY_PROFILE_NUM_MIN,
				  DSAT_LEGACY_PROFILE_NUM_MAX,
				  DSAT_MAX_CP_DATA_LENGTH);

    if(0 == res_buff_ptr->used)
    {
      result = DSAT_ERROR;
    }
    else
    {
      result = DSAT_OK;
    }
  }
  else 
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;

}

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CRTDCP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CRTDCP command.
  This command is used to enable and disable reporting of data
  from the network that is transmitted over the control plane
  in downlink direction. 

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

dsat_result_enum_type dsatetsipkt_exec_crtdcp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type reporting = (dsat_num_item_type)CRTDCP_REPORT_MT_CONTROL_DATA_DISABLE;

  if ( tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA|EQ))
  {
    if ( tok_ptr->op == (NA|EQ|AR))
    {
      if( tok_ptr->args_found > 1 )
      {
        /* processing crtdcp_cmd found more argument */
        return DSAT_ERROR;
      }
      else
      {
        if ( !VALID_TOKEN(0) )
        {
          /* processing crtdcp_cmd invalid argument */
          return DSAT_ERROR;
        }
 
        if ( ATOI_OK != dsatutil_atoi (&reporting, tok_ptr->arg[0], 10 ) ||
             reporting >= CRTDCP_REPORT_MT_CONTROL_DATA_MAX )
        {
          /* cannot convert to int */
          /* only 0 and 1 allowed */
          return DSAT_ERROR;
        }

        result = ds_3gpp_rmsm_atif_crtdcp_hdlr((reporting?TRUE:FALSE),
                                                dsatetsipkt_rmsm_crtdcp_done_handler);

        if (result == DSAT_OK )
        {
          dsatparm_exec_param_cmd ( mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr
                                  );
          dsat_psm_urc_update_bit_mask(DSAT_PSM_URC_CRTDCP, 
                                       DSATETSI_EXT_CRTDCP_IDX, 
                                       0);
        }
        return result;
      }
    }
    return DSAT_OK; /* for NA|EQ */
  }
  else if (tok_ptr->op == (NA|QU) ||
           tok_ptr->op == (NA|EQ|QU))
  {
    result = dsatparm_exec_param_cmd ( mode,
                              parse_table,
                              tok_ptr,
                              res_buff_ptr);
  }
  else 
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;

}

/*===========================================================================
FUNCTION DSATETSIPKT_CRTDCP_DONE_HANDLER

DESCRIPTION
  This function is handler function for reporting +CRTDCP: <cid>,<cpdata_length>,<cpdata>
  URC.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type dsatetsipkt_crtdcp_done_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
)
{
  /* Hexstring will take memory twice the number of bytes; +2 for quotation and +1 for NULL terminated string
  +50 as placeholder for +CRTDCP:%d,%d, 
  URC will be chained with max size of TEMP_BUFF_SIZE */
#define TEMP_BUFF_SIZE 1500
#define HEX_STRING_LENGTH 2

  dsm_item_type           *res_buff_ptr = NULL;
  uint32                   cp_data_len = 0;
  uint8                   *cp_data = NULL;
  uint32                   idx = 0;
  char                    *temp_buff = NULL;
  uint32                   temp_buff_used = 0;
  ds_at_crtdcp_cmd_type   *crtdcp_info_ptr =  dsat_get_cmd_payload_ptr(cmd_ptr);
  dsat_num_item_type       report_crtdcp = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CRTDCP_IDX,0,MIX_NUM_TYPE);

  if ( !crtdcp_info_ptr->data_frame )
  {
    return DSAT_ASYNC_EVENT;
  }
  
  if (!report_crtdcp)
  {
    dsm_free_packet(&(crtdcp_info_ptr->data_frame));
    return DSAT_ASYNC_EVENT;
  }

  cp_data_len = dsm_length_packet(crtdcp_info_ptr->data_frame);

  DS_AT_MSG1_HIGH("dsatetsipkt_crtdcp_done_handler dsm_item length %d",
                  (int)cp_data_len );

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  res_buff_ptr->data_ptr[0] = '\0';

  cp_data = (uint8 *) dsat_alloc_memory(cp_data_len,FALSE);
  if(!cp_data)
  {
    DS_AT_MSG0_ERROR("Unable to allocate memory for CP DATA");
    dsm_free_buffer(res_buff_ptr);
    return DSAT_ASYNC_EVENT;
  }
  
  memset(cp_data, 0, cp_data_len);

  if ( cp_data_len != dsm_pullup_tail ( &(crtdcp_info_ptr->data_frame), cp_data, cp_data_len) ||
       !(temp_buff = (char*) dsat_alloc_memory( TEMP_BUFF_SIZE, FALSE)) )
  {
    DS_AT_MSG0_ERROR("Unable to pull dsm buffer or unable to allocate memory");
    dsm_free_buffer(res_buff_ptr);
    dsm_free_packet(&(crtdcp_info_ptr->data_frame));
    return DSAT_ASYNC_EVENT;
  }

  temp_buff = (char*) dsat_alloc_memory( TEMP_BUFF_SIZE, FALSE);
  if(!temp_buff)
  {
    DS_AT_MSG0_ERROR("Unable to allocate memory");
    dsm_free_buffer(res_buff_ptr);
    dsm_free_packet(&(crtdcp_info_ptr->data_frame));
    return DSAT_ASYNC_EVENT;
  }
  memset(temp_buff, 0, TEMP_BUFF_SIZE);

  /* Prepare URC: +CRTDCP: <cid>,<cpdata_length>,<cpdata> */
  /* Put start of URC +CRTDCP: <cid>,<cpdata_length>, */
  temp_buff_used = (word)snprintf(
                        temp_buff,
                        TEMP_BUFF_SIZE, "+CRTDCP: %d,%d,\"",
                        (int)crtdcp_info_ptr->profile_number,
                        (int)cp_data_len);

  /* Put hex string */
  while(idx < cp_data_len )
  {
    temp_buff_used += (word)snprintf(temp_buff + temp_buff_used,
                   (TEMP_BUFF_SIZE - temp_buff_used), "%02X",
                   /* 0 will be added at front if the number is single digit number */
                   cp_data[idx]);
    if((temp_buff_used + HEX_STRING_LENGTH) >= TEMP_BUFF_SIZE)
    {
      /* pushing dsm item; possilbe that display may exceed one large dsm item */
      dsat_dsm_pushdown_tail(&res_buff_ptr,
                           temp_buff, temp_buff_used,
                           DSM_ITEM_POOL(res_buff_ptr), FALSE);
      temp_buff_used = 0;
    }
    idx++;
  }

  /* Put end of quotation */
  temp_buff_used += (word)snprintf(
                        temp_buff + temp_buff_used,
                        (TEMP_BUFF_SIZE - temp_buff_used), "\"");

  dsat_dsm_pushdown_tail(&res_buff_ptr,
                         temp_buff, temp_buff_used,
                         DSM_ITEM_POOL(res_buff_ptr), FALSE);

  dsatutil_free_memory_ext((void **) &cp_data);
  dsatutil_free_memory_ext((void **) &temp_buff);
  dsm_free_packet(&(crtdcp_info_ptr->data_frame));
  /* Set the Tx port and Type. */
  ds3g_siolib_set_tx_type(DS3G_SIOLIB_UNSOLICTED_CM, DS3G_SIOLIB_DATA_PORT);
  dsatcmdp_send_urc(res_buff_ptr, DSAT_COMPLETE_RSP);
  ds3g_siolib_set_tx_type(DS3G_SIOLIB_UNSOLICTED_NONE, DS3G_SIOLIB_PORT_NONE);
  return DSAT_ASYNC_EVENT;
}

/*===========================================================================
FUNCTION  ETSIPKT_CGPADDR_BUILD_CID_LIST

DESCRIPTION
  This function builds the CID list from the passed tokens.
   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgpaddr_build_cid_list
(
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  uint8 *cid_list,                    /* Parse results list */
  uint8 *index                        /* Length of list */
)
{
  dsat_result_enum_type  result = DSAT_OK;

  DS_ASSERT( ( NULL != cid_list ) && ( NULL != index ) );

  if(tok_ptr->args_found > MAX_ARG )
  {
    DS_AT_MSG2_ERROR("Args found %d > %d max args supported",tok_ptr->args_found,
    	                                                        MAX_ARG);
    return DSAT_ERROR;
  }
    
  while( ( *index < tok_ptr->args_found) &&
         ( *index < DSAT_MAX_ALLOWED_PROFILES ) )
  {
    dsat_num_item_type cid;

    /* Convert token to numeric */
    if( ATOI_OK == dsatutil_atoi (&cid, tok_ptr->arg[*index], 10) )
    {
      /* Validate range */
      if( DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
      {
        /* Store in list to report on */
        cid_list[(*index)++] = (uint8)cid;
      }
      else
      {
        DS_AT_MSG2_ERROR("CID out of range: index=%d value=%d",*index,cid);
        result = DSAT_ERROR;
        break;
      }
    }
    else
    {
      DS_AT_MSG1_ERROR("Cannot convert CID value: index=%d",*index);
      result = DSAT_ERROR;
      break;
    }
  } /* while */

  return result;
} /* etsipkt_cgpaddr_build_cid_list */


/*===========================================================================
FUNCTION  ETSIPKT_CGPADDR_FORMAT_HELPER_RESPONSE

DESCRIPTION
  This function builds the CID, PDP address formatting before sending to TE.
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If Successful, reutrn TRUE
  FALSE: Otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_cgpaddr_format_helper_response
(
  uint32                            cid,
  dsm_item_type                     *res_buff_ptr
)
{

  boolean                          is_static_addr = FALSE;
  boolean                          is_context_valid = FALSE;
  dsat_ip_addr_type                ip_addr={0};
  dsat_ip_addr_type                temp_pdp_addr={0};
  char                             buf[MAX_IPADDR_STR_LEN+1];  
  ds_profile_type_s                 profile_data;

  if (res_buff_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Buffer NULL");
    return FALSE;
  }
  
  if(DS_PROFILE_RESULT_SUCCESS == ds_profile_setting_from_profile_id
                                  (cid,&profile_data))
  {
      is_context_valid = TRUE;
      is_static_addr = FALSE;
      if(profile_data.context.pdp_type == DS_PROFILE_PDP_NON_IP)
      {
          ip_addr.ip_vsn = DS_PROFILE_NONIP;
      }
      else if(DSAT_OK != ds_3gpp_rmsm_ati_get_pdp_addr(cid, &(ip_addr.addr)))
      {
        is_static_addr = TRUE;
        DS_AT_MSG1_MED("PDP context - %d has a static address.",cid);

        if(ip_addr.addr.ip_vsn == DS_PROFILE_IP_V6)
        {
          ip_addr.ip_vsn = DS_PROFILE_IP_V6;
        }
        else if(ip_addr.addr.ip_vsn == DS_PROFILE_IP_V4V6)
        {
          ip_addr.ip_vsn = DS_PROFILE_IP_V4V6;
        }
        else
        {
          ip_addr.ip_vsn = DS_PROFILE_IP_V4;
        }
      }
      else
      {
      
        DS_AT_MSG2_MED("PDP context - %d has a dynamic address: version %d",
                       cid,ip_addr.addr.ip_vsn);
     }
    }
    else
    {
      is_context_valid = FALSE;
    }

   if( TRUE == is_context_valid )
   {
    buf[0] = '\0';
    res_buff_ptr->used += (word) snprintf(
                          (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                          res_buff_ptr->size - res_buff_ptr->used,
                          "%s %lu,", "+CGPADDR:",cid);
            
    if( ip_addr.addr.ip_vsn == DS_PROFILE_IP_V4)
    {    
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr , 
                                   ip_addr.addr.pdp_addr.pdp_addr_ipv4,
                                   is_static_addr, 
                                   res_buff_ptr, 
                                   buf ))
      {
        return FALSE;
      }
    }
    else if( ip_addr.addr.ip_vsn == DS_PROFILE_IP_V6 )
    {
    
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr , 
                                   &ip_addr.addr.pdp_addr.pdp_addr_ipv6,
                                   is_static_addr, 
                                   res_buff_ptr, 
                                   buf ))
      {
         return FALSE;
      }
    }
    else if( ip_addr.addr.ip_vsn == DS_PROFILE_IP_V4V6 )
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr,
                                   ip_addr.addr.pdp_addr.pdp_addr_ipv4,
                                   is_static_addr,
                                   res_buff_ptr,
                                   buf ))
      {
         return FALSE;
      }
      buf[0] = '\0';
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   &ip_addr.addr.pdp_addr.pdp_addr_ipv6,
                                   is_static_addr, 
                                   res_buff_ptr, 
                                   buf ))
      {
        return FALSE;
      }
    }

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
  }
  return TRUE;
}/*etsipkt_cgpaddr_format_helper_response*/

/*===========================================================================
FUNCTION  ETSIPKT_CGPADDR_FORMAT_RESPONSE

DESCRIPTION
  This function builds the CID, PDP address formatting before sending to TE.
   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgpaddr_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *res_buff_ptr
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint16                 used;
  ds_profile_ident_info ident_info;  
  ds_profile_status_etype   profile_status = DS_PROFILE_RESULT_SUCCESS;
  ds_profile_list_stype            *profile_list =NULL;
  uint8                             profile_count =0;
  ds_profile_type_s                 payload;
  uint8                             profile_number;
  uint8                             i=0;
  memset(&ident_info,0,   sizeof(ds_profile_ident_info));

  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */
    
    /* Loop through the list to check if atleast one CID is defined 
       Else just return OK */

    res_buff_ptr->used = (word)snprintf(
             (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
             res_buff_ptr->size - res_buff_ptr->used,
             "%s", "+CGPADDR: (");
    used = res_buff_ptr->used;

    etsipkt_get_profile_list(&profile_list,&profile_count);
    if(profile_list ==NULL)
    {
      return DSAT_ERROR;
    }
   memset(&payload,0,   sizeof(ds_profile_type_s));

    for(i = 0; i < profile_count; ++i)
    {
      memset(&ident_info ,0,sizeof(ds_profile_ident_info));
      ident_info.pdp_type = profile_list[i].pdp_type;
      if(profile_list[i].apn[0] !='\0')
      {
        memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                profile_list[i].apn, sizeof(profile_list[i].apn ));
      }
      profile_status = ds_profile_get_param_ex(ident_info,&payload,&profile_number);

      res_buff_ptr->used += (word)snprintf(
          (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
          res_buff_ptr->size - res_buff_ptr->used,
                         "%d,", profile_number);
    }
    if( used == res_buff_ptr->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      res_buff_ptr->used = 0;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    else
    {
      res_buff_ptr->used--; 
      res_buff_ptr->used += (word)snprintf(
               (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
               res_buff_ptr->size - res_buff_ptr->used,
               ")");
    }
   DS_SYSTEM_HEAP_MEM_FREE(profile_list);
  }
  else
  {
    /* WRITE syntax */
    res_buff_ptr->used = 0;
    
    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {    
      etsipkt_get_profile_list(&profile_list,&profile_count);
      if(profile_list ==NULL)
      {
        return DSAT_ERROR;
      }
    for(i = 0; i < profile_count; ++i)
    {
      memset(&ident_info ,0,sizeof(ds_profile_ident_info));
      ident_info.pdp_type = profile_list[i].pdp_type;
      if(profile_list[i].apn[0] !='\0')
      {
        memscpy(ident_info.apn, DS_PROFILE_APN_STRING_LEN+1, 
                profile_list[i].apn, sizeof(profile_list[i].apn ));
      }
      profile_status = ds_profile_get_param_ex(ident_info,&payload,&profile_number);
    
      if(!etsipkt_cgpaddr_format_helper_response(profile_number,
                                                  res_buff_ptr))
         {
            DS_SYSTEM_HEAP_MEM_FREE(profile_list);
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
         }
      }
      DS_SYSTEM_HEAP_MEM_FREE(profile_list);
    }
    else
    {
      /* Process specific contexts */
      for(i = 0; (i < cid_list_len) && (i < DSAT_MAX_ALLOWED_PROFILES); ++i)
      {
        byte index = cid_list_ptr[i];
        if(DS_PROFILE_RESULT_SUCCESS != ds_profile_setting_from_profile_id
                                        (index,&payload))
        {
          DS_AT_MSG1_HIGH("Profile No t present with the profile id %d",
                           index);
          return DSAT_ERROR;
        }
        if(!etsipkt_cgpaddr_format_helper_response(cid_list_ptr[i],
                                                   res_buff_ptr))
        {
           return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
        }
      }
    }
    if(res_buff_ptr->used != 0)
    {
      res_buff_ptr->used--;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
  }
  return result;
} /* etsipkt_cgpaddr_format_response */

/*===========================================================================
FUNCTION  ETSIPKT_CGCONTRDP_FORMAT_HELPER_RESPONSE

DESCRIPTION
This function builds the P-CID, and related Dynamic parameters info
into a formatted form as mentioned in the 27.007.
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If Successful, reutrn TRUE
  FALSE: Otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_cgcontrdp_format_helper_response
(
  ds_3gpp_atif_dynamic_params_info_type  *dynamic_param_ptr,
  ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr,
  dsm_item_type                          *res_buff_ptr
)
{
  dsat_ip_addr_type      temp_pdp_addr;
  char                   buf[MAX_IPADDR_STR_LEN+1];
  ds_profile_addr_type_ipv6 temp_src_adr;
  
  if (prim_dynamic_ptr == NULL ||
      prim_dynamic_ptr == NULL ||
      res_buff_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Buffer NULL");
    return FALSE;
  }

  if( DS_3GPP_ATIF_PRIM_INFO == dynamic_param_ptr->info_type)
  {
    if( prim_dynamic_ptr->ip_addr.ip_vsn ==(ds_profile_ip_version_enum_type)DS_PROFILE_IP_V4V6)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr , 
                                   prim_dynamic_ptr->ip_addr.pdp_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

      buf[0] = '\0';

      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                     (ds_profile_addr_type_ipv6*)&prim_dynamic_ptr->ip_addr.pdp_addr.pdp_addr_ipv6,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->ip_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V4)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->ip_addr.pdp_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->ip_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V6)
    {
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_addr_type_ipv6 
                                   *)&prim_dynamic_ptr->ip_addr.pdp_addr.pdp_addr_ipv6,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

    /* Print the <gw_addr> - First the IPv4 followed by IPv6 */
    if(prim_dynamic_ptr->v4_ipcp.ipcp4_options.gateway_addr != 0)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->v4_ipcp.ipcp4_options.gateway_addr,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

    }
    buf[0] = '\0';

    if(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.gateway_addr.ps_s6_addr64[0] != 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ' ';
      dsatutil_memscpy(&(temp_src_adr.in6_u),sizeof(temp_src_adr.in6_u),
                         &(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.gateway_addr.in6_u),sizeof(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.gateway_addr.in6_u));
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   &temp_src_adr,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

    /* Print the <DNS_prim_addr> - First the IPv4 followed by IPv6 */
    if(prim_dynamic_ptr->v4_ipcp.ipcp4_options.primary_dns!= 0)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->v4_ipcp.ipcp4_options.primary_dns,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    if(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.primary_dns.ps_s6_addr64[0] != 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ' ';
      dsatutil_memscpy(&(temp_src_adr.in6_u),sizeof(temp_src_adr.in6_u),
                         &(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.primary_dns.in6_u),sizeof(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.primary_dns.in6_u));
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   &temp_src_adr,
                                    FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    /* Print the <DNS_sec_addr> - First the IPv4 followed by IPv6 */
    if(prim_dynamic_ptr->v4_ipcp.ipcp4_options.secondary_dns!= 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->v4_ipcp.ipcp4_options.secondary_dns,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    if(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.secondary_dns.ps_s6_addr64[0] != 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ' ';
      dsatutil_memscpy(&(temp_src_adr.in6_u),sizeof(temp_src_adr.in6_u),
                         &(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.secondary_dns.in6_u),sizeof(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.secondary_dns.in6_u));
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   &temp_src_adr,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    /* Print the <P_CSCF_prim_addr>  */
    if( prim_dynamic_ptr->prim_pcscf_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V4V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

      buf[0] = '\0';

      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_addr_type_ipv6*)&prim_dynamic_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv6,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->prim_pcscf_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V4)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->prim_pcscf_addr.ip_vsn == (ds_profile_ip_version_enum_type)  DS_PROFILE_IP_V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_addr_type_ipv6 *) 
                                   &prim_dynamic_ptr->prim_pcscf_addr.pdp_addr.pdp_addr_ipv6,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    /* Print the <P_CSCF_sec_addr>  */
    if( prim_dynamic_ptr->sec_pcscf_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V4V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

      buf[0] = '\0';

      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_addr_type_ipv6 *) 
                                   &prim_dynamic_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv6,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->sec_pcscf_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V4)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->sec_pcscf_addr.ip_vsn == (ds_profile_ip_version_enum_type)DS_PROFILE_IP_V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_addr_type_ipv6 *) 
                                   &prim_dynamic_ptr->sec_pcscf_addr.pdp_addr.pdp_addr_ipv6,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
  }
  else
  {
    DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_PRIM_INFO %d ",dynamic_param_ptr->info_type);
    DS_ASSERT(0);
    return FALSE;
  }

  return TRUE;
} /*etsipkt_cgcontrdp_format_helper_response*/

/*===========================================================================
FUNCTION  ETSIPKT_CGCONTRDP_FORMAT_RESPONSE

DESCRIPTION
This function builds the P-CID, and related Dynamic parameters info
into a formatted form as mentioned in the 27.007.
   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgcontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *res_buff_ptr
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint16                 used;
  uint32                 i;

  ds_prim_profile_list_type prim_profile_list;

  memset((void*)&prim_profile_list, 0x0, sizeof(prim_profile_list));
  res_buff_ptr->used = 0;

  /* Get the Active Primary Contexts list */

  ds_3gpp_rmsm_atif_get_all_active_prim_profiles(&prim_profile_list);

  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */

    res_buff_ptr->used = (word)snprintf(
             (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
             res_buff_ptr->size - res_buff_ptr->used,
             "%s ", "+CGCONTRDP: (");
    used = res_buff_ptr->used;

    for(i = 0;(i< DS_3GPP_MAX_PDN_CONTEXT)&&(i< prim_profile_list.data_len); ++i)
    {
      res_buff_ptr->used += (word)snprintf(
           (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
           res_buff_ptr->size - res_buff_ptr->used,
           "%lu,", prim_profile_list.data_ptr[i]);
    }
    if( used == res_buff_ptr->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      
      res_buff_ptr->used = 0;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    else
    {
      res_buff_ptr->used--; 
      res_buff_ptr->used += (word)snprintf(
               (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
               res_buff_ptr->size - res_buff_ptr->used,
               " )");
    }
  }
  else
  {
    /* WRITE syntax */
    dynamic_param_ptr = 
      (ds_3gpp_atif_dynamic_params_info_type *) dsat_alloc_memory(
                               sizeof(ds_3gpp_atif_dynamic_params_info_type), FALSE);

    memset((void*)dynamic_param_ptr,0x0,sizeof(ds_3gpp_atif_dynamic_params_info_type));

    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
      /* Process specified context */
      for(i = 0;(i< DS_3GPP_MAX_PDN_CONTEXT)&&(i< prim_profile_list.data_len); ++i)
      {
        memset((void*)dynamic_param_ptr,0x0,sizeof(ds_3gpp_atif_dynamic_params_info_type));

        if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(prim_profile_list.data_ptr[i]
                                                         ,DS_3GPP_ATIF_PRIM_INFO,dynamic_param_ptr))
        {
          ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr = 
                                            &dynamic_param_ptr->u.prim_dynamic_info;
          if(prim_dynamic_ptr->apn != NULL)
          {
            uint16 check_size = 20+9*MAX_IPADDR_STR_LEN+ \
                            strlen((const char *)prim_dynamic_ptr->apn);
            /*One loop below would need 40 max for IPV6, 20 for other params 
            +length of APN */

            CHECK_APPEND_RESPONSE_BUFFER_NEW (((uint16)DSM_BUFFER_THRESHOLD - check_size));

            res_buff_ptr->used += (word)snprintf(
                    (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                    res_buff_ptr->size - res_buff_ptr->used,
                    "%s %lu,%u,\"%s\",", "+CGCONTRDP:",
                    prim_profile_list.data_ptr[i],
                    prim_dynamic_ptr->bearer_id,
                    prim_dynamic_ptr->apn);
          }
          else
          {
            uint16 check_size = 20+9*MAX_IPADDR_STR_LEN+2;
            /*One loop below would need 40 max for IPV6, 20 for other params 
            +length of APN */

            CHECK_APPEND_RESPONSE_BUFFER_NEW (((uint16)DSM_BUFFER_THRESHOLD - check_size));

            res_buff_ptr->used += (word)snprintf(
                    (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                    res_buff_ptr->size - res_buff_ptr->used,
                    "%s %lu,%u,\"\",", "+CGCONTRDP:",
                    prim_profile_list.data_ptr[i],
                    prim_dynamic_ptr->bearer_id
                    );
          }
          if (!etsipkt_cgcontrdp_format_helper_response(dynamic_param_ptr,
                                                        prim_dynamic_ptr,
                                                        res_buff_ptr))
          {
            dsatutil_free_memory((void*)dynamic_param_ptr);
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
          }
        }
      }
    }
    else if(cid_list_len == 1)
    {
      /* Process specified context */

      if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(cid_list_ptr[0],
                                                       DS_3GPP_ATIF_PRIM_INFO,
                                                       dynamic_param_ptr))
      {
        if( DS_3GPP_ATIF_PRIM_INFO == dynamic_param_ptr->info_type)
        {
          ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr = 
                                            &dynamic_param_ptr->u.prim_dynamic_info;
          if(prim_dynamic_ptr->apn != NULL)
          {
            res_buff_ptr->used = (word)snprintf(
                    (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                    res_buff_ptr->size - res_buff_ptr->used,
                    "%s %d,%u,\"%s\",", "+CGCONTRDP:",
                    cid_list_ptr[0],
                    prim_dynamic_ptr->bearer_id,
                    prim_dynamic_ptr->apn);
           }
           else
           {
             res_buff_ptr->used = (word)snprintf(
                   (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                   res_buff_ptr->size - res_buff_ptr->used,
                   "%s %d,%u,\"\",", "+CGCONTRDP:",
                   cid_list_ptr[0],
                   prim_dynamic_ptr->bearer_id
                   );
             }

          DS_AT_MSG2_HIGH(" IP addr version for the cid %d is %d ",cid_list_ptr[0],prim_dynamic_ptr->ip_addr.ip_vsn);

          if (!etsipkt_cgcontrdp_format_helper_response(dynamic_param_ptr,
                                                        prim_dynamic_ptr,
                                                        res_buff_ptr))
          {
            dsatutil_free_memory((void*)dynamic_param_ptr);
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
          }
        }
      }
    }

    if(res_buff_ptr->used != 0)
    {
      res_buff_ptr->used--;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    dsatutil_free_memory((void*)dynamic_param_ptr);
  }

  return result;
} /* etsipkt_cgcontrdp_format_response */

#ifdef FEATURE_ATCOP_TEMP_3GPP_ALT

/*===========================================================================
FUNCTION  ETSIPKT_CGSCONTRDP_FORMAT_RESPONSE

DESCRIPTION
This function builds the CID, and related Dynamic parameters of all 
Non Primary Contexts info into a formatted form as mentioned in the 27.007.

   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgscontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint32                 i;
  uint16                 used;

  ds_3gpp_atif_flow_cid_list_type flow_profile_list;

  memset((void*)&flow_profile_list, 0x0, sizeof(flow_profile_list));
  format_buffer->used = 0;

  /* Get the Active Non-Primary Contexts list */
   ds_3gpp_rmsm_atif_get_all_active_flow_profiles(0,&flow_profile_list);


  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */
    
    format_buffer->used = (word)snprintf(
             (char *)&format_buffer->data_ptr[format_buffer->used], 
             format_buffer->size - format_buffer->used,
             "%s ", "+CGSCONTRDP: (");
    used = format_buffer->used;

    for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
    {
      format_buffer->used += (word)snprintf(
           (char *)&format_buffer->data_ptr[format_buffer->used], 
           format_buffer->size - format_buffer->used,
           "%ld,", (long int) flow_profile_list.data_ptr[i]);
    }
    if( used == format_buffer->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      format_buffer->used = 0;
      format_buffer->data_ptr[format_buffer->used] = '\0';
    }
    else
    {
      format_buffer->used--; 
      format_buffer->used += (word)snprintf(
               (char *)&format_buffer->data_ptr[format_buffer->used],
               format_buffer->size - format_buffer->used,
               " )");
    }
  }
  else
  {
    /* WRITE syntax */
    dynamic_param_ptr = 
      (ds_3gpp_atif_dynamic_params_info_type *) dsat_alloc_memory(
                               sizeof(ds_3gpp_atif_dynamic_params_info_type), FALSE);

    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
      /* Process All Non - Primary contexts */

      for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
      {

        if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(flow_profile_list.data_ptr[i],
                                  0,DS_3GPP_ATIF_SEC_INFO,dynamic_param_ptr))
        {
          if( DS_3GPP_ATIF_SEC_INFO == dynamic_param_ptr->info_type)
          {
            ds_3gpp_atif_secondary_dynamic_info_type *sec_dynamic_ptr = 
                                              &dynamic_param_ptr->u.sec_dynamic_info;
  
  
            format_buffer->used += (word)snprintf(
                     (char *)&format_buffer->data_ptr[format_buffer->used], 
                     format_buffer->size - format_buffer->used,
                    "%s %d,%d,%d \n", "+CGSCONTRDP:",
                      (int)sec_dynamic_ptr->cid,
                      (int)sec_dynamic_ptr->p_cid,
                      (int)sec_dynamic_ptr->bearer_id);
          }
          else
          {
            DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_SEC_INFO %d ",dynamic_param_ptr->info_type);
            DS_ASSERT(0);
          }
        }
      }/* for Loop */
    }
    else if(cid_list_len == 1)
    {
      /* Process specified context */

      if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(cid_list_ptr[0],
                                0,DS_3GPP_ATIF_SEC_INFO,dynamic_param_ptr))
      {
        if( DS_3GPP_ATIF_SEC_INFO == dynamic_param_ptr->info_type)
        {
          ds_3gpp_atif_secondary_dynamic_info_type *sec_dynamic_ptr = 
                                            &dynamic_param_ptr->u.sec_dynamic_info;
      
      
          format_buffer->used = (word)snprintf(
                   (char *)&format_buffer->data_ptr[format_buffer->used], 
                   format_buffer->size - format_buffer->used,
                  "%s %d,%d,%d ", "+CGSCONTRDP:",
                   (int)sec_dynamic_ptr->cid,
                   (int)sec_dynamic_ptr->p_cid,
                   (int)sec_dynamic_ptr->bearer_id);
      
        }
        else
        {
          DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_SEC_INFO %d ",dynamic_param_ptr->info_type);
          DS_ASSERT(0);
        }
      }
    }

    if(format_buffer->used != 0)
    {
      format_buffer->used--;
      format_buffer->data_ptr[format_buffer->used] = '\0';
    }
    dsatutil_free_memory((void*)dynamic_param_ptr);
  }

  return result;
}/* etsipkt_cgscontrdp_format_response */
#endif

/*===========================================================================
FUNCTION  ETSIPKT_CLEAR_GPRS_EVENTS_BUFFER

DESCRIPTION
  Clears the GPRS event buffer queue

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void etsipkt_clear_gprs_events_buffer
(
  void
)
{
  etsipkt_gprs_msg_type  *msg_ptr;
  dsatetsipkt_gprs_msg_info     *gprs_dd_ms_val = NULL;

  gprs_dd_ms_val = dsat_get_base_addr(DSAT_MD_VALS, FALSE);

  /* Get messages from the queue until the queue is empty */
  while( (msg_ptr = (etsipkt_gprs_msg_type *)
                    q_get( &gprs_dd_ms_val->etsipkt_gprs_msg_q )) != NULL )
  {
    /* Free the message buffer */
    dsatutil_free_memory(msg_ptr);
  }
} /* etsipkt_clear_gprs_events_buffer */

/*===========================================================================
FUNCTION  DSATETSIPKT_FLUSH_GPRS_EVENT_INDICATIONS

DESCRIPTION
  Flushes the buffered GPRS event reports to TE

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_flush_gprs_event_indications
(
  void
)
{
  dsm_item_type          *res_buff_ptr;
  etsipkt_gprs_msg_type  *msg_ptr;
  dsatetsipkt_gprs_msg_info     *gprs_dd_ms_val = NULL;

  gprs_dd_ms_val = dsat_get_base_addr(DSAT_MD_VALS, FALSE);

  /* Get messages from the queue until the queue is empty */
  while( (msg_ptr = (etsipkt_gprs_msg_type *)
                    q_get( &gprs_dd_ms_val->etsipkt_gprs_msg_q )) != NULL )
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
    if (NULL != res_buff_ptr)
    {
      (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
              res_buff_ptr->size,(void*) msg_ptr->event_data,
              MIN(msg_ptr->event_data_len,GPRS_EVENT_DATA_SIZE));
    
      res_buff_ptr->used =  MIN(msg_ptr->event_data_len,GPRS_EVENT_DATA_SIZE);
      (void)dsatcmdp_send_respose_to_te (res_buff_ptr, DSAT_COMPLETE_RSP);
    }

    /* Free the message buffer */
    dsatutil_free_memory(msg_ptr);
  }
} /* dsatetsipkt_flush_gprs_event_indications */
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_LTE
/*===========================================================================
FUNCTION DSATETSIPKT_EXEC_CEMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CEMODE command.
  The set command is used to set the MT to operate according to the specified 
  mode of operation for EPS. The read command returns the mode of operation set
  by the TE, independent of the current serving cell capability and independent
  of the current serving cell Access Technology. The test command is used for 
  requesting information on the supported MT mode of operation.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CME_OP_NOT_SUPPORTED : in case of failure
    DSAT_OK : if the command succeeds.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cemode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dsat_eps_mode_e_type  eps_mode = DSAT_EPS_MODE_MAX;
  cm_ph_info_s_type     *ph_info_ptr = NULL;

  ph_info_ptr = (cm_ph_info_s_type *)dsat_alloc_memory(sizeof(cm_ph_info_s_type), FALSE);
  memset(ph_info_ptr, 0, sizeof(cm_ph_info_s_type));

  if(tok_ptr->op == (NA|QU) || (tok_ptr->op == (NA|EQ|QU))) 
  {
     if(tok_ptr->op == (NA|QU))
     {
       cm_client_get_ph_info(ph_info_ptr);

       switch(ph_info_ptr->lte_ue_mode_pref)
       {
         case CM_LTE_UE_MODE_PREF_PS_MODE1:
           eps_mode = DSAT_EPS_PS_MODE_1;
           break;
         case CM_LTE_UE_MODE_PREF_PS_MODE2:  
           eps_mode = DSAT_EPS_PS_MODE_2;
           break;
         case CM_LTE_UE_MODE_PREF_CS_PS_MODE1:
           eps_mode = DSAT_EPS_CS_PS_MODE_1;
           break;
         case CM_LTE_UE_MODE_PREF_CS_PS_MODE2:
           eps_mode = DSAT_EPS_CS_PS_MODE_2;
           break;
         default:
           break;
       }
	   if(((dsat_num_item_type)
              dsatutil_get_val( DSATETSI_EXT_CEMODE_IDX,0,NUM_TYPE) != eps_mode)&& 
              (eps_mode != DSAT_EPS_MODE_MAX))
       {
         DSATUTIL_SET_VAL(DSATETSI_EXT_CEMODE_IDX,0,0,eps_mode,NUM_TYPE);
         DS_AT_MSG1_HIGH(" UPDATED CEMODE as %d ",eps_mode);
       }
     }
    result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
  }
  if (DSAT_OK != result)
  {
    /* In case of error return operation not supported */
    result = dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
  }
  dsatutil_free_memory(ph_info_ptr);
  return result;
}/* dsatetsipkt_exec_cemode_cmd */
#endif /* FEATURE_DSAT_LTE */

