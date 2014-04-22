#ifndef MMGSDI_CPHS_H
#define MMGSDI_CPHS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

       C O M M O N   P C N   H A N D S E T   S P E C I F I C A T I O N S

                   ( C P H S )   P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the Common PCN Handset Specifications (CPHS)
  Function Prototypes.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

                        COPYRIGHT INFORMATION

Copyright (c) 2001 - 2005, 2007 - 2009, 2011 - 2012, 2016, 2018 QUALCOMM Technologies,
Inc (QTI) and its licensors. All Rights Reserved. QUALCOMM Technologies Proprietary.
Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_cphs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/27/18   tq      Moving function to stub file
06/22/16   tkl     Stub out additional APIs
05/11/16   vdc     Guard with FEATURE_MMGSDI_3GPP2 for 3GPP2 related functions
05/04/16   bcho    Disable CHPS functionality for IOE LTE Only devices
01/30/12   bcho    mmgsdi_proc_cphs_post_pin1_init() modified
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Added support for Get Operator Name API
02/20/09   ssr     Removed CSP file offset macro
11/14/08   ssr     Fixed parsing of value added service group
02/21/08   sun     Added new parameters for CPHS Read
01/08/07   sp      Updated GSDI_CPHS_CSP_NUM_IDENTIFY_OFFSET value from
                   0xFF to 0x0F.
05/11/05   sst     Lint fixes
12/27/04   jk      Fixed Compiler Warnings
09/22/04   jar     Added CPHS Post PIN1 Initialization procedures and
                   #defines for use in CPHS Procedures.
09/07/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
  STRUCTURE  MMGSDI_CPHS_READ_CPHS_EFS_INFO_TYPE

  DESCRIPTION:
    This structure contains flags to read cphs related uim files
------------------------------------------------------------------------------*/

typedef struct{
  boolean         read_cphsi;
  boolean         read_csp;
  boolean         read_ons;
  boolean         read_ons_short;
} mmgsdi_cphs_read_cphs_efs_info_type ;


/*--------------------------------------------------------------------------
                             Type Defs
--------------------------------------------------------------------------*/


/*===========================================================================
FUNCTION MMGSDI_CPHS_POST_PIN1_INIT

DESCRIPTION
  This function will run the CPHS Intialization procedures.  The purpose
  of this function is to cache the relevant CPHS Information for use
  by other Tas

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type:

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cphs_post_pin1_init(
  mmgsdi_session_id_type               session_id,
  mmgsdi_slot_id_enum_type             slot,
  mmgsdi_app_enum_type                 app_type,
  mmgsdi_cphs_read_cphs_efs_info_type  read_cphs_efs
);

/*==========================================================================
FUNCTION MMGSDI_CPHS_GET_ONS

DESCRIPTION
  Function to get operator name string from cphs file caches

DEPENDENCIES
  None

RETURN VALUE
  BOOLEAN: TRUE OR FALSE

SIDE EFFECTS
  None
==========================================================================*/
boolean mmgsdi_cphs_get_ons(
  mmgsdi_session_type_enum_type   session_type,
  mmgsdi_plmn_info_type         * plmn_info_ptr
);

/*===========================================================================
FUNCTION: MMGSDI_CPHS_GET_CPHS_INFO_PTR

DESCRIPTION
  Based on the session type, this function will return the cphs info ptr

DEPENDENCIES
  none

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_cphs_get_cphs_info_ptr(
  mmgsdi_session_type_enum_type         session_type,
  mmgsdi_cphs_int_info_type           **cphs_int_info_pptr
);

/*===========================================================================
  FUNCTION:      MMGSDI_GEN_GET_CPHS_INFO

  DESCRIPTION:
    This function will handle get cphs information request

  DEPENDENCIES:
    NONE

  LIMITATIONS:
    NONE

  RETURN VALUE:
    MMGSDI_SUCCESS:        The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
  SIDE EFFECTS:
    NONE

===========================================================================*/
mmgsdi_return_enum_type mmgsdi_gen_get_cphs_info(
  const mmgsdi_session_get_cphs_info_req_type *req_ptr
);

/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_SESSION_GET_CPHS_INFO

   DESCRIPTION:
     This function builds the Cnf data for session get cphs info command

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_SUCCESS:          The command processing was successful.
     MMGSDI_ERROR:            The command processing was not successful.

   SIDE EFFECTS:
     None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_cnf_build_session_get_cphs_info(
  mmgsdi_session_id_type                  session_id,
  mmgsdi_session_get_cphs_info_cnf_type  *session_get_cphs_info_cnf_ptr,
  mmgsdi_return_enum_type                 mmgsdi_status);

/*===========================================================================
FUNCTION        MMGSDI_SESSION_QUEUE_GET_CPHS_INFO_REQ

DESCRIPTION
  This function is used to extract the CPHS related information which is
  already populated in MMGSDI. This is only valid and applicable when an USIM
  or GSM Application is available . This function will perform a check for
  the USIM or GSM Application before sending the command.

PARAMETERS
  mmgsdi_session_id_type  session_id:      Session ID of the caller
  mmgsdi_callback_type    response_cb_ptr: Call back function when the
                                           command execution is done.
  mmgsdi_client_data_type client_ref:      User Data returned upon
                                           completion of this cmd.

DEPENDENCIES
  The client must also have a valid session open if the USIM Application
  is not the default application
  Achieved by calling mmgsdi_session_open().

LIMITATIONS
  None.

RETURN VALUE
  mmgsdi_return_enum_type:

    MMGSDI_SUCCESS:          The command structure was properly generated
                             and queued onto the MMGSDI Command Queue.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.
    MMGSDI_CMD_QUEUE_FULL:   The command was not Queued to the MMGSDI Task
                             because the max number of commands are already
                             queued.

SIDE EFFECTS
  None.
===========================================================================*/
mmgsdi_return_enum_type  mmgsdi_session_queue_get_cphs_info_req (
  mmgsdi_session_id_type              session_id,
  mmgsdi_callback_type                response_cb_ptr,
  mmgsdi_client_data_type             client_ref
);

/* ============================================================================
FUNCTION:      MMGSDI_GEN_RSP_GET_CPHS_INFO

DESCRIPTION:
  This function will build cnf for get cphs info command

DEPENDENCIES:
  NONE

LIMITATIONS:
  NONE

RETURN VALUE:
  mmgsdi_return_enum_type
    MMGSDI_SUCCESS:       The cnf was successfully queued to cnf Queue
    MMGSDI_ERROR:         Failed in queuing cnf to cnf Queue

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_get_cphs_info (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
);

#endif /* MMGSDI_CPHS_H */
