#ifndef DSATETSIPKT_H
#define DSATETSIPKT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                       U M T S

                P A C K E T   D O M A I N

                A T   C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( UMTS packet domain commands ) processor.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2019 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsipkt.h_v   1.4   12 Jul 2002 10:05:54   randrew  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/atcop/inc/dsatetsipkt.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/21/17   rg      Added support for Non-IP pdp type profile.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/12/14   pg      Modified DS Profile APIs to per subs.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
09/25/13   sc      Added support for +CNMPSD command.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
07/09/10   bs      Fixed +CGACT for LTE.
05/15/10   nc      Added support fot +CGCONTRDP,+CGSCONTRDP,
                   +CGEQOSRDP and +CGTFTRDP for LTE.
04/20/10   bs      Added support for +CGCMOD for LTE.
03/18/10   nc      Added Support for +CGEQOS for LTE.
01/06/10   bs      Added +CGACT command support for LTE.
12/15/09   nc      Featurisation changes for LTE.
10/09/09   sa      Adding support for +CEMODE command.
05/19/09   vk      Defeaturization of PDP registry for CMI
01/30/09   ua      Added support of CGEV for DUN calls.
12/12/08   ua      Fixed Off target lint errors.
10/23/08   sa      Added support for $QCPDPFAM command.
11/04/07   pp      Lint low - fixes.
10/09/07   ss      Added support for $QCGANSM, $QCGARL, $QCGAPL commands.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
08/06/07   sa      Removed +CGEQREQ and +CGEQMIN support in GSM only builds.
04/23/07   pp      Lint Medium fixes.
03/29/07   ua      Reverting back CGCLASS modifications which supported CLASS B.
                   Now it supports only CLASS A. 
02/13/07   pkp     dsatetsipkt_cgcmod_test_response_fmt prototype changed.
08/02/06   snb     Dual processor changes.
06/08/06   ua      Modification of +CGCLASS command to support only CLASSB.
12/08/05   snb     Add IPV6 support
11/30/05   snb     Add support for +CGCMOD command.
07/25/05   snb     Change PDP connect info struct to support WCDMA-mode 
                   AT$QCDGEN.
06/27/05   snb     Add fields to dsat_pdp_connect_info_s_type to support 
                   $QCDGEN sending data simultaneously on multiple contexts.
05/03/05   tkk     Added prototype for context ID initialization.
04/04/05   snb     Made external a function returning number of contexts 
                   without an assigned TFT.
03/14/05   tkk     Added prototype for GPRS event (+CGEREP) queues.
03/02/05   snb     Add fields to dsat_pdp_connect_info_s_type and 
                   dsat_pdp_cm_info_s_type to support $QCDGEN command.
01/31/05   tkk     Added prototypes for exec'ing +CGDATA, packet call 
                   initiation & dsat_cgdata_info_s_type structure for 
                   +CGDATA command.
01/27/05   pdv     Merge from Sirius-ATCoP development branch.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
01/10/05   tkk     Added support for +CGPADDR command.
09/10/04   sb      Secondary PDP changes
11/19/03   ar      Added support for +CGDSCONT and +CGTFT commands.
08/13/03   snb     Added support for $QCPDPLT command
03/31/03   ar      Added command write_mode to context activation tracking.
02/14/04   ar      Add support for PDP context profile activation.
01/30/03   ar      Add support for +CGEQMIN command
01/13/03   ar      Removed obsolete dsatetsipkt_init_pdp_context() ptotype
08/26/02   ar      Code review action items
06/27/02   ar      Add support for +CGEQREQ; split query operations into 
                   separate functions for extended commands, used by &v logic
04/11/02   ar      Add support for $QCPDPP command
02/04/02   ar      Add support for +CGDCONT command
08/27/01   SB      created file

===========================================================================*/

#include "datamodem_variation.h"
#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_3gpp_rmsm_atif.h"
#include "dsatctab.h"
#include "dsatetsictab.h"
#include "dsati.h"
#include "ds_profile.h"
#include "ds_prof.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Macro to copy to uint32 fm byte array with byte reversal */
#define ADDR2INT(x,a) (x = (((a[0] << 24 ) & 0xFF000000) | \
                            ((a[1] << 16 ) & 0x00FF0000) | \
                            ((a[2] << 8  ) & 0x0000FF00) | \
                            (a[3] & 0x000000FF)))

/* Macro for copying between PDP addresses per 24.008 section 10.5.6.4 and 
   DSUMTS IP addresses: performs byte reversal during copy, assumes d & s 
   are byte pointers */
#define BYTE_REV_CPY(d,s,l) {uint8 idx; for (idx=0; idx<l; idx++) *(d + idx) = *(s + ((l - idx) - 1));} 

typedef enum
{
  READ_COMMAND,
  WRITE_COMMAND,
  RESET_COMMAND
}dsat_pdp_command_type;

/* +CSODCP */
typedef enum
{
  CSODCP_DATA_TYPE_REGULAR = 0,
  CSODCP_DATA_TYPE_MO_EXCEPTION = 1,
  CSODCP_DATA_TYPE_MAX 
}dsat_csodcp_type_of_user_data_e_type;

typedef enum
{
  CSODCP_RAI_NO_INFORMATION_AVAILABLE = 0, /* No information available*/
  CSODCP_RAI_COMPLETED_WITH_TX = 1, /* The MT expects that exchange of datawill be completed with 
                                       the transmission of the ESM DATA TRANSPORT message */
  CSODCP_RAI_COMPLETED_WITH_RX = 2, /* The MT expects that exchange of data will be completed with
                                       the receipt of an ESM DATA TRANSPORT message. */
  CSODCP_RAI_MAX
}dsat_csodcp_rai_e_type;

/* +CRTDCP */
typedef enum
{
  CRTDCP_REPORT_MT_CONTROL_DATA_DISABLE = 0,
  CRTDCP_REPORT_MT_CONTROL_DATA_ENABLE = 1,
  CRTDCP_REPORT_MT_CONTROL_DATA_MAX
}dsat_crtdcp_report_mt_control_data_e_type;

/*---------------------------------------------------------------------------
    Typedefs for PDP context profile activation support
---------------------------------------------------------------------------*/
/* Parameter table strucure  used in get and set param API's */
typedef struct atcop_profile_3gpp_param_list
{
//@sanjaykc: check functionality
  ds_profile_param_enum_type        param_type;
  uint16                            param_len;  
}atcop_profile_3gpp_param_list_type;

//@sanjaykc: Look for alternative
#ifdef FEATURE_ATCOP_TEMP_PROFILE_ALT
/* Param length for Strings is 0*/
static const atcop_profile_3gpp_param_list_type context_param_tbl[] = 
{
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
    sizeof (ds_profile_3gpp_pdp_context_number_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
    sizeof (ds_profile_3gpp_pdp_type_enum_type) },
  
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
    sizeof (ds_profile_3gpp_pdp_header_comp_e_type) },
  
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
    sizeof(ds_profile_3gpp_pdp_data_comp_e_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv4) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv6) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
    DS_PROFILE_APN_STRING_LEN+1 },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
    sizeof(ds_profile_3gpp_pdp_context_secondary_flag_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
    sizeof(ds_profile_3gpp_pdp_context_primary_id_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
    sizeof(ds_profile_3gpp_pdp_access_control_e_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC,
    sizeof(ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type) }
};

static const atcop_profile_3gpp_param_list_type dsat_cgdcont_ext_table[] = 
{
  { DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
    sizeof(ds_profile_3gpp_emergency_calls_are_supported) }
};

static const atcop_profile_3gpp_param_list_type auth_param_tbl[] = 
{

  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
    sizeof (ds_profile_3gpp_auth_pref_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
    DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1 },
  
  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME,
    DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1 }
};
#endif

typedef enum pdp_connect_state
{
  DSAT_PDP_CONNECT_DEACTIVATED = 0,
  DSAT_PDP_CONNECT_ACTIVATED   = 1,
  DSAT_PDP_CONNECT_MAX            /* Internal use only ! */
} dsat_pdp_connect_state_e_type;

typedef enum call_query_mode
{
  DSAT_CQM_NONE,                  /* Internal use only ! */
  DSAT_CQM_CPAS_READ,             /* +CPAS read command */
  DSAT_CQM_CGACT_WRITE,           /* +CGACT write command */
  DSAT_CQM_CGCMOD_WRITE,          /* +CGCMOD write command */
  DSAT_CQM_CGCMOD_TEST,           /* +CGCMOD test command */
  DSAT_CQM_MAX                    /* Internal use only ! */
} dsat_call_query_mode_e_type;

#define UNUSED 255

typedef struct dsat_pdp_connect_tbl
{
  dsat_pdp_connect_state_e_type  demand_state;    /* Connection state */
  uint8                          change_index;    /* Connection changing */
  boolean                        pending;         /* Operation pending? */
  dsat_num_item_type             valid_profile_count;/*Total valid profile  */
} dsat_pdp_connect_s_type;

/* For +CGDATA */
typedef struct dsat_cgdata_info {
  boolean   valid;
  byte      cid; /* Context ID */
} dsat_cgdata_info_s_type;

#define DSAT_CGEREP_BUFFER_AT_MT           (dsat_num_item_type)0
#define DSAT_CGEREP_BUFFER_DISCARD_FWD_TE  (dsat_num_item_type)1
#define DSAT_CGEREP_BUFFER_FWD_TE          (dsat_num_item_type)2

#define DSAT_CGEREP_EMPTY_BUFFER           (dsat_num_item_type)0
#define DSAT_CGEREP_FLUSH_BUFFER           (dsat_num_item_type)1

/* PDP context profile activation state information */
extern dsat_pdp_connect_s_type dsat_pdp_state;
extern byte  dsat_pdp_cid[];  /* Context profile id */

/*---------------------------------------------------------------------------
                        Command defaults and limits
---------------------------------------------------------------------------*/
#define QCPDPLT_DEF_VAL 1

#define DSATPKT_CID_ALL (dsat_num_item_type)0x55

/* Added for +CGDATA support */
#define DSATETSIPKT_GPRS_SERVICE_CODE       99 
#define DSATETSIPKT_CGDATA_CMD              0x66
#define DSATETSIPKT_CID_PRIMARY             (boolean)0
#define DSATETSIPKT_CID_SECONDARY           (boolean)1

#define DSAT_GPRS_EVENT_MSG_COUNT   15
/* Worst case: +CGEV: NW REACT <PDP_type>,<PDP_addr>,<cid> */
#define GPRS_EVENT_DATA_SIZE        (30 + MAX_IPADDR_STR_LEN)

typedef struct {
  q_link_type    link;           /* Queue link type */
  char           event_data[GPRS_EVENT_DATA_SIZE]; /* GPRS event data */
  uint16         event_data_len;
} etsipkt_gprs_msg_type;

typedef struct  dsat_gprs_msg_info
{
  boolean flush_gprs_buffer;
  q_type  etsipkt_gprs_msg_q;
}dsatetsipkt_gprs_msg_info;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

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
 dsm_item_type* etsipkt_check_response_buffer
(
  dsm_item_type  *res_buff_ptr,   /* Response buffer */
  uint16          buf_limit       /* Buffer contents limit */
);

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

);
//#endif
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
dsat_result_enum_type dsatetsipkt_exec_cgpaddr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_LTE

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
dsat_result_enum_type dsatetsipkt_exec_cgcontrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* FEATURE_DSAT_LTE */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDCONT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDCONT command.
  This command sets PDP context parameters.

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
);

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
  const dsati_cmd_type  *parse_table,     /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,   /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
);

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
);

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
dsat_result_enum_type dsatetsipkt_exec_cgatt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

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
);

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGACT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgact_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);

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

dsat_result_enum_type dsatetsipkt_cgact_proc_cmd( void );

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
);

#ifdef FEATURE_DSAT_LTE
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
  dsat_mode_enum_type       mode,         /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr  /*  Place to put response       */
);

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
  dsat_mode_enum_type       mode,           /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,        /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr    /*  Place to put response       */
);

#endif /* FEATURE_DSAT_LTE */

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
);


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
);


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
void dsatetsipkt_init_service_preference_from_nv();

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
  dsat_rmsm_info_s_type         dsat_rmsm_info,
  void* user_info_ptr
);

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
dsat_result_enum_type dsatetsipkt_exec_cemode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_LTE */
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
dsat_result_enum_type dsatetsipkt_at_cmd_handler
(
  ds_command_type         * cmd_ptr              /* DS Command pointer         */
);

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
);


#endif /* DSATETSIPKT_H */
