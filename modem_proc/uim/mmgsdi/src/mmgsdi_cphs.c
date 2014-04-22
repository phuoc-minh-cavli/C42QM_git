/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


            G E N E R I C   S I M   D R I V E R   I N T E R F A C E

       C O M M O N   P C N   H A N D S E T   S P E C I F I C A T I O N S

                   ( C P H S )   P R O C E D U R E S


GENERAL DESCRIPTION

  This source file contains the Common PCN Handset Specifications (CPHS)
  procedures.

EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

                        COPYRIGHT INFORMATION

Copyright (c) 2001-2009, 2011-2014, 2016, 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_cphs.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/18   rps     Refragment alloc extra param code
07/27/18   tq      Moving function to stub file
09/02/16   dt      Fix Compilation error 
07/04/16   ar      EONS selective logging
06/22/16   tkl     Stub out additional APIs
05/25/16   vdc     Remove F3 messages for memory allocation failure
05/11/16   vdc     Guard with FEATURE_MMGSDI_3GPP2 for 3GPP2 related functions
05/04/16   bcho    Disable CHPS functionality for IOE LTE Only devices
08/29/14   hh      Fix compliation warnings
08/06/14   ar      Replace MMGSDIUTIL_TMC_MEM_MALLOC_AND_VALIDATE with new macro
07/28/14   vv      Remove unused data types
07/15/14   yt      Support for NV refresh
01/10/14   df      Use v2.0 diag macros
07/23/13   tl      Remove read ONS during post pin1 as a dependency on CPHSI
06/24/13   vdc     Added support for triple SIM
05/16/13   vdc     Replace memcpy with safer version memscpy
05/06/13   spo     Modified CPHS variables to array of pointers
04/16/13   tl      Clean up of radio access features from MMGSDI code
03/20/13   tl      Optional CHPS fields only read if activated and allocated
01/04/13   tl      EF-CPS/EF-ONS only read if EF-CHPSI is present
03/16/12   yt      Remove inclusion of tmc.h
01/30/12   bcho    mmgsdi_proc_cphs_post_pin1_init() modified
12/21/11   shr     Legacy GSDI removal updates
12/21/11   bcho    Added support for Get Operator Name API
02/18/11   yt      Fixed length check for CPHSI data
09/26/09   kp      ZI memory reduction changes
08/24/09   shr     Fixed compliation errors (for CDMA only)
07/08/09   shr     Fixed Lint warnings
04/29/09   js      Added prototypes for local functions
02/19/09   ssr     Removed comment from CSP data
12/15/08   ssr     Fixed Lint Error
12/08/08   ssr     Fixed parsing of CSP data
11/14/08   ssr     Fixed parsing of value added service group
04/29/08   sun     Added support for invalid 3G configuration
02/21/08   sun     Added new parameters for individual reads
05/07/07   sp      Replaced calls to mem_malloc/free with mmgsdi_malloc/free
07/07/06   sun     Removed Reference to mc.h
05/11/05   sst     Lint fixes
11/15/04   jar     Code clean up to use Debug MACROS
09/23/04   jar     Added retrieval of CPHS Information and parsing of data.
09/07/04   jar     Created File

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

/* System and Interfaces */
#include "uim_msg.h"
#include "err.h"
#include "memheap.h"
#include "customer.h"
#include "string.h"
#include "mmgsdi_cphs.h"
#include "mmgsdiutil.h"
#include "mmgsdi_card_init.h"
#include "mmgsdi_nv.h"
#include "uim.h"
#include "intconv.h"
#include "uim_selective_logging.h"
#include "mmgsdi_cnf.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* ----------------------------------------------------------------------------
     CPHS Information EF-CPHSI as organized in the File
     --------------------------------------------------------------------------*/
#define MMGSDI_CPHS_INFO_PHASE_OFFSET          0x00
#define MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2    0x01
#define MMGSDI_CPHS_INFO_SERVICES_OFFSET_B3    0x02

  /* ----------------------------------------------------------------------------
    CPHS Information EF-CPHSI Masks
    ---------------------------------------------------------------------------*/
#define MMGSDI_CPHS_INFO_CSP_MASK              0x03
#define MMGSDI_CPHS_INFO_SST_MASK              0x0C
#define MMGSDI_CPHS_INFO_MBN_MASK              0x30
#define MMGSDI_CPHS_INFO_OP_NAME_MASK          0xC0
#define MMGSDI_CPHS_INFO_INFO_NUM_MASK         0x03

  /* ----------------------------------------------------------------------------
     CPHS Service Group Codes as defined for EF-CSP
     --------------------------------------------------------------------------*/
#define MMGSDI_CPHS_CSP_SVC_GROUP_1            0x01
#define MMGSDI_CPHS_CSP_SVC_GROUP_2            0x02
#define MMGSDI_CPHS_CSP_SVC_GROUP_3            0x03
#define MMGSDI_CPHS_CSP_SVC_GROUP_4            0x04
#define MMGSDI_CPHS_CSP_SVC_GROUP_5            0x05
#define MMGSDI_CPHS_CSP_SVC_GROUP_6            0x06
#define MMGSDI_CPHS_CSP_SVC_GROUP_7            0x07
#define MMGSDI_CPHS_CSP_SVC_GROUP_8            0x08
#define MMGSDI_CPHS_CSP_SVC_GROUP_9            0x09
#define MMGSDI_CPHS_CSP_SVC_GROUP_C0           0xC0
#define MMGSDI_CPHS_CSP_SVC_GROUP_D5           0xD5

  /* ----------------------------------------------------------------------------
     CPHS Service Group Offset as defined for EF-CSP
     --------------------------------------------------------------------------*/
#define MMGSDI_CPHS_CSP_SVC_GRP1_OFFSET        0x00
#define MMGSDI_CPHS_CSP_SVC_GRP2_OFFSET        0x02
#define MMGSDI_CPHS_CSP_SVC_GRP3_OFFSET        0x04
#define MMGSDI_CPHS_CSP_SVC_GRP4_OFFSET        0x06
#define MMGSDI_CPHS_CSP_SVC_GRP5_OFFSET        0x08
#define MMGSDI_CPHS_CSP_SVC_GRP6_OFFSET        0x0A
#define MMGSDI_CPHS_CSP_SVC_GRP7_OFFSET        0x0C
#define MMGSDI_CPHS_CSP_SVC_GRP8_OFFSET        0x0E
#define MMGSDI_CPHS_CSP_SVC_GRP9_OFFSET        0x10
#define MMGSDI_CPHS_CSP_SVC_GRPC0_OFFSET       0x12
#define MMGSDI_CPHS_CSP_SVC_GRPD0_OFFSET       0x14

  /* ----------------------------------------------------------------------------
     Customer Service Profile Service Masks
     --------------------------------------------------------------------------*/
  /* Byte 1 */
#define MMGSDI_CPHS_CSP_CFU_MASK               0x80
#define MMGSDI_CPHS_CSP_CFB_MASK               0x40
#define MMGSDI_CPHS_CSP_CFNRy_MASK             0x20
#define MMGSDI_CPHS_CSP_CFNRc_MASK             0x10
#define MMGSDI_CPHS_CSP_CT_MASK                0x08
  /* Byte 2 */
#define MMGSDI_CPHS_CSP_BOAC_MASK              0x80
#define MMGSDI_CPHS_CSP_BOIC_MASK              0x40
#define MMGSDI_CPHS_CSP_BOICexHC_MASK          0x20
#define MMGSDI_CPHS_CSP_BAIC_MASK              0x10
#define MMGSDI_CPHS_CSP_BIC_ROAM_MASK          0x08
  /* Byte 3 */
#define MMGSDI_CPHS_CSP_MPTY_MASK              0x80
#define MMGSDI_CPHS_CSP_CUG_MASK               0x40
#define MMGSDI_CPHS_CSP_AOC_MASK               0x20
#define MMGSDI_CPHS_CSP_PREF_CUG_MASK          0x10
#define MMGSDI_CPHS_CSP_CUG_OA_MASK            0x08
  /* Byte 4 */
#define MMGSDI_CPHS_CSP_HOLD_MASK              0x80
#define MMGSDI_CPHS_CSP_CW_MASK                0x40
#define MMGSDI_CPHS_CSP_CCBS_MASK              0x20
#define MMGSDI_CPHS_CSP_USER_SIG_MASK          0x10
  /* Byte 5 */
#define MMGSDI_CPHS_CSP_SM_MT_MASK             0x80
#define MMGSDI_CPHS_CSP_SM_MO_MASK             0x40
#define MMGSDI_CPHS_CSP_SM_CB_MASK             0x20
#define MMGSDI_CPHS_CSP_REPLY_PATH_MASK        0x10
#define MMGSDI_CPHS_CSP_DEL_CONF_MASK          0x08
#define MMGSDI_CPHS_CSP_PROTOCOL_ID_MASK       0x04
#define MMGSDI_CPHS_CSP_VALIDITY_PER_MASK      0x02
  /* Byte 6 */
#define MMGSDI_CPHS_CSP_ALS_MASK               0x80
  /* Byte 7 */
#define MMGSDI_CPHS_CSP_RSVD_SST_PHASE_1_MASK  0x80
  /* Byte 8 */
#define MMGSDI_CPHS_CSP_CLIP_MASK              0x80
#define MMGSDI_CPHS_CSP_CoLR_MASK              0x20
#define MMGSDI_CPHS_CSP_CoLP_MASK              0x10
#define MMGSDI_CPHS_CSP_MCI_MASK               0x08
#define MMGSDI_CPHS_CSP_CLI_SEND_MASK          0x02
#define MMGSDI_CPHS_CSP_CLI_BLOCK_MASK         0x01
  /* Byte 9 */
#define MMGSDI_CPHS_CSP_GPRS_MASK              0x80
#define MMGSDI_CPHS_CSP_HSCSD_MASK             0x40
#define MMGSDI_CPHS_CSP_VOICE_GRP_CALL_MASK    0x20
#define MMGSDI_CPHS_CSP_VOICE_BRDCST_SVC_MASK  0x10
#define MMGSDI_CPHS_CSP_MULTI_SUB_PROFILE_MASK 0x08
#define MMGSDI_CPHS_CSP_MULTI_BAND_MASK        0x04
  /* Byte 10 */
#define MMGSDI_CPHS_CSP_PLMN_MODE_MASK         0x80
#define MMGSDI_CPHS_CSP_VPS_MASK               0x40
#define MMGSDI_CPHS_CSP_SM_MO_PAGING_MASK      0x20
#define MMGSDI_CPHS_CSP_SM_MO_EMAIL_MASK       0x10
#define MMGSDI_CPHS_CSP_FAX_MASK               0x08
#define MMGSDI_CPHS_CSP_DATA_MASK              0x04
#define MMGSDI_CPHS_CSP_LANGUAGE_MASK          0x01
  /* Byte 11 */
#define MMGSDI_CPHS_CSP_INFO_NUMBERS_MASK      0xFF


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
  mmgsdi_cphs_int_info_type           **cphs_int_info_pptr)
{
  MMGSDIUTIL_RETURN_IF_NULL(cphs_int_info_pptr);

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
      *cphs_int_info_pptr = mmgsdi_cphs_info_ptr[MMGSDI_PRI_CPHS_INFO_INDEX];
      break;
    case MMGSDI_GW_PROV_SEC_SESSION:
      *cphs_int_info_pptr = mmgsdi_cphs_info_ptr[MMGSDI_SEC_CPHS_INFO_INDEX];
      break;
    case MMGSDI_GW_PROV_TER_SESSION:
      *cphs_int_info_pptr = mmgsdi_cphs_info_ptr[MMGSDI_TER_CPHS_INFO_INDEX];
      break;
    default:
      UIM_MSG_ERR_1("Invalid session_type 0x%x in retrieving CPHS info ptr",
                    session_type);
      return MMGSDI_ERROR;
  }
  return MMGSDI_SUCCESS;
} /* mmgsdi_cphs_get_cphs_info_ptr */


/*===========================================================================
FUNCTION MMGSDI_CPHS_PARSE_CPHSI_DATA

DESCRIPTION
  This function is a local function used to parse the CPHSI Data and store
  the results in gsdi_data (The global Data Structure).

  The information stored in the Global Data Structure will be utilized
  by the CPHS GET INFO API.

INPUT:
  mmgsdi_data_type *  data:  The pointer to the buffer used to store the
                             read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cphs_parse_cphsi_data(
  mmgsdi_data_type              data,
  mmgsdi_session_type_enum_type session_type
)
{
  mmgsdi_cphs_int_info_type *cphs_int_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(data.data_ptr);
  if(data.data_len <= MMGSDI_CPHS_INFO_SERVICES_OFFSET_B3)
  {
    UIM_MSG_ERR_1("CPHSI return data len 0x%x too short", data.data_len);
    return MMGSDI_ERROR;
  }

  if(mmgsdi_cphs_get_cphs_info_ptr(session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(cphs_int_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* --------------------------------------------------------------------------
     CPHS Information was read which indicates that CPHS is supported to some
     degree.  It is up to the (EF-CSP) Customer Service Profile to indicate
     exactly which services are supported.
     Logically, as far as GSDI is concerned, reading CPHS Information is an
     indication that the card is CPHS Capable.
     ------------------------------------------------------------------------*/
  cphs_int_info_ptr->cphs_info.cphs_capable = TRUE;

  /* --------------------------------------------------------------------------
     Store the CPHS Phase Information
     ------------------------------------------------------------------------*/
  cphs_int_info_ptr->cphs_info.cphs_service.phase = data.data_ptr[MMGSDI_CPHS_INFO_PHASE_OFFSET];

  /* --------------------------------------------------------------------------
     Now parse the CPHS Information Data to determine which "Services" are
     allocated and activated
     ------------------------------------------------------------------------*/
  cphs_int_info_ptr->cphs_info.cphs_service.csp =
    (mmgsdi_current_service_enum_type)(data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                     & MMGSDI_CPHS_INFO_CSP_MASK);
  cphs_int_info_ptr->cphs_info.cphs_service.sst =
    (mmgsdi_current_service_enum_type)((data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                      & MMGSDI_CPHS_INFO_SST_MASK) >> 2);
  cphs_int_info_ptr->cphs_info.cphs_service.mbn =
    (mmgsdi_current_service_enum_type)((data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                      & MMGSDI_CPHS_INFO_MBN_MASK) >> 4);
  cphs_int_info_ptr->cphs_info.cphs_service.ops =
    (mmgsdi_current_service_enum_type)((data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B2]
                                      & MMGSDI_CPHS_INFO_OP_NAME_MASK) >> 6);
  cphs_int_info_ptr->cphs_info.cphs_service.ifn =
    (mmgsdi_current_service_enum_type)(data.data_ptr[MMGSDI_CPHS_INFO_SERVICES_OFFSET_B3]
                                     & MMGSDI_CPHS_INFO_INFO_NUM_MASK);
  return MMGSDI_SUCCESS;
} /* mmgsdi_cphs_parse_cphsi_data */


/*===========================================================================
FUNCTION MMGSDI_CPHS_PARSE_CSP_DATA

DESCRIPTION
  This function is a local function used to parse the CSP Data and store
  the results in gsdi_data (The global Data Structure).

  The information stored in the Global Data Structure will be utilized
  by the CPHS GET INFO API.

INPUT:
  mmgsdi_data_type  data:  The pointer to the buffer used to store the
                           read from the SIM / USIM.
DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type.

SIDE EFFECTS
  None
===========================================================================*/
static mmgsdi_return_enum_type mmgsdi_cphs_parse_csp_data(
  mmgsdi_data_type              data,
  mmgsdi_session_type_enum_type session_type
)
{
  uint32                      iteration     = 0x0000;
  uint32                      svcs_index    = 0x0000;
  mmgsdi_cphs_int_info_type *cphs_int_info_ptr = NULL;

  MMGSDIUTIL_RETURN_IF_NULL(data.data_ptr);

  if(mmgsdi_cphs_get_cphs_info_ptr(session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(cphs_int_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  /* -------------------------------------------------------------------------
   In case if Mandatory Data Objects are not present in CSP File then
   we will pick the default value.
   For example default value of PLMN mode is ON, So in case where
   the PLMN bit is not present in file then we will allow to show manual option
  ----------------------------------------------------------------------------*/

  /* Default value is ON for Value Added Service group */
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_PLMN_MODE    = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_VPS          = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_PAGING = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_EMAIL  = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_FAX          = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.value_added_DATA         = MMGSDI_CSP_IND_ON;
  cphs_int_info_ptr->cphs_info.cphs_csp.valued_added_Language    = MMGSDI_CSP_IND_ON;

  /* --------------------------------------------------------------------------
     Iterate through the Service Group offsets, and set the Global
     data Structure Information appropriately
     ------------------------------------------------------------------------*/
  while ( (iteration + 1) < int32touint32(data.data_len))
  {
    /* Service Group offset and its data should be there.*/
    svcs_index = iteration + 1;
    switch ( data.data_ptr[iteration] )
    {
    case MMGSDI_CPHS_CSP_SVC_GROUP_1:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFU_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFU = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFU = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFB_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFB = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFB = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFNRy_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRy = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRy = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CFNRc_MASK)
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRc = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CFNRc = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CT_MASK)
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CT = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_offering_CT = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_2:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BOAC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOAC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOAC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BOIC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BOICexHC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC_exHC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BOIC_exHC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BAIC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BAIC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BAIC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_BIC_ROAM_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BIC_Roam = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_restrict_BIC_Roam = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_3:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MPTY_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_MPTY = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_MPTY = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CUG_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_AOC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_AoC = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_AoC = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_PREF_CUG_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_Pref_CUG = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_Pref_CUG = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CUG_OA_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG_OA = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.other_supps_CUG_OA = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_4:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_HOLD_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_HOLD = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_HOLD = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CW_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CW = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CW = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CCBS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CCBS = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_CCBS = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_USER_SIG_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_User_Signalling = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.call_completion_User_Signalling = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_5:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MT_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MT = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MT = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MO_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MO = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_MO = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_CB_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_CB = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_SM_CB = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_REPLY_PATH_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Reply_Path = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Reply_Path = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_DEL_CONF_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Del_Conf = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Del_Conf = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_PROTOCOL_ID_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Protocol_ID = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Protocol_ID = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VALIDITY_PER_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Validity_Period = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.teleservices_Validity_Period = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_6:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_ALS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_teleservices_als = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_teleservices_als = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_7:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_RSVD_SST_PHASE_1_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_features_SST = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.cphs_features_SST = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_8:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CLIP_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLIP = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLIP = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CoLR_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLR = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLR = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CoLP_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLP = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CoLP = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MCI_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_MCI = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_MCI = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CLI_SEND_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_send = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_send = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_CLI_BLOCK_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_block = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.number_identify_CLI_block = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_9:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_GPRS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_GPRS = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_GPRS = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_HSCSD_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_HSCSD = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_HSCSD = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VOICE_GRP_CALL_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Group_call = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Group_call = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VOICE_BRDCST_SVC_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Broadcast_Svc = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Voice_Broadcast_Svc = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MULTI_SUB_PROFILE_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Muli_Sub_Profile = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Muli_Sub_Profile = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_MULTI_BAND_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Multiband = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.p2plus_services_Multiband = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_C0:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_PLMN_MODE_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_PLMN_MODE = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_PLMN_MODE = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_VPS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_VPS = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_VPS = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MO_PAGING_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_PAGING = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_PAGING = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_SM_MO_EMAIL_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_EMAIL = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_SM_MO_EMAIL = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_FAX_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_FAX = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_FAX = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_DATA_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_DATA = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.value_added_DATA = MMGSDI_CSP_IND_OFF;

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_LANGUAGE_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.valued_added_Language = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.valued_added_Language = MMGSDI_CSP_IND_OFF;

      break;

    case MMGSDI_CPHS_CSP_SVC_GROUP_D5:

      if ( data.data_ptr[svcs_index] & MMGSDI_CPHS_CSP_INFO_NUMBERS_MASK )
        cphs_int_info_ptr->cphs_info.cphs_csp.information_numbers = MMGSDI_CSP_IND_ON;
      else
        cphs_int_info_ptr->cphs_info.cphs_csp.information_numbers = MMGSDI_CSP_IND_OFF;

      break;

    default:
      MMGSDI_DEBUG_MSG_ERROR_2("Bad Service Group: 0x%x Iteration: 0x%x",
                               data.data_ptr[iteration], iteration);
    }
    iteration += 2;
  }

  return MMGSDI_SUCCESS;
} /* mmgsdi_cphs_parse_csp_data */


/*===========================================================================
FUNCTION MMGSDI_CPHS_POST_PIN1_INIT

DESCRIPTION
  This function will run the CPHS Intialization procedures.  The purpose
  of this function is to cache the relevant CPHS Information for use
  by other Tasks for later usage.

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
)
{
  mmgsdi_file_enum_type       chpsi_filename          = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type       csp_filename            = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type       chps_ons_filename       = MMGSDI_NO_FILE_ENUM;
  mmgsdi_file_enum_type       cphs_ons_short_filename = MMGSDI_NO_FILE_ENUM;
  mmgsdi_return_enum_type     read_cphsi_status       = MMGSDI_ERROR;
  mmgsdi_return_enum_type     mmgsdi_status           = MMGSDI_ERROR;
  mmgsdi_session_info_type   *session_info_ptr        = NULL;
  mmgsdi_data_type            mmgsdi_data_buf;
  mmgsdi_cphs_int_info_type  *cphs_int_info_ptr       = NULL;

  memset(&mmgsdi_data_buf, 0x00, sizeof(mmgsdi_data_type));

  /* --------------------------------------------------------------------------
     Perform Paramater Checks and allocate memory for the cphs Requests
     ------------------------------------------------------------------------*/
  if((slot != MMGSDI_SLOT_1) &&
     (slot != MMGSDI_SLOT_2) &&
     (slot != MMGSDI_SLOT_3))
  {
    MMGSDI_DEBUG_MSG_ERROR_1("CPHS: Bad MMGSDI Slot ID: 0x%x", slot);
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch(app_type)
  {
    case MMGSDI_APP_USIM:
      chpsi_filename            = MMGSDI_USIM_CPHSI;
      csp_filename              = MMGSDI_USIM_CSP;
      chps_ons_filename         = MMGSDI_USIM_ONS;
      cphs_ons_short_filename   = MMGSDI_USIM_ONS_SHORT;
      break;
    case MMGSDI_APP_SIM:
      chpsi_filename            = MMGSDI_GSM_CPHSI;
      csp_filename              = MMGSDI_GSM_CSP;
      chps_ons_filename         = MMGSDI_GSM_ONS;
      cphs_ons_short_filename   = MMGSDI_GSM_ONS_SHORT;
      break;
    default:
      UIM_MSG_HIGH_1("CPHS not valid for non SIM/USIM app type 0x%x", app_type );
      return MMGSDI_ERROR;
  }

  mmgsdi_status = mmgsdi_util_get_session_app_info(session_id,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   &session_info_ptr,
                                                   NULL);
  if((mmgsdi_status != MMGSDI_SUCCESS) || (session_info_ptr == NULL))
  {
    return MMGSDI_ERROR;
  }

  if(mmgsdi_cphs_get_cphs_info_ptr(session_info_ptr->session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return MMGSDI_ERROR;
  }

  if(cphs_int_info_ptr == NULL)
  {
    return MMGSDI_ERROR;
  }

  if(read_cphs_efs.read_cphsi)
  {
    /* --------------------------------------------------------------------------
       First Determine if CPHS is supported on this SIM/USIM by trying to
       read EF CPHS Information Files to access the CPHS Service Table
       ------------------------------------------------------------------------*/
    read_cphsi_status = mmgsdi_card_init_cache_binary( session_id,
                                                       app_type,
                                                       slot,
                                                       chpsi_filename,
                                                       &mmgsdi_data_buf);
    if (read_cphsi_status == MMGSDI_SUCCESS )
    {
      mmgsdi_status = mmgsdi_cphs_parse_cphsi_data(mmgsdi_data_buf,
                                                   session_info_ptr->session_type);
      if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR_1("Failure parsing CPHSI: 0x%x, CPHSI Not Valid",
                                 mmgsdi_status);
        read_cphs_efs.read_csp       = FALSE;
        read_cphs_efs.read_ons_short = FALSE;
      }
      else
      {
        /* Optional CHPS data-fields should only be read or updated
           if indicated as being both allocated and activated */
        if (cphs_int_info_ptr->cphs_info.cphs_service.csp != MMGSDI_ACTIVATED_AND_ALLOCATED)
        {
          read_cphs_efs.read_csp       = FALSE;
        }
        if (cphs_int_info_ptr->cphs_info.cphs_service.ops != MMGSDI_ACTIVATED_AND_ALLOCATED)
        {
          read_cphs_efs.read_ons_short = FALSE;
        }
      }
      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Could not read CPHSI: 0x%x", mmgsdi_status);
      /*If Mode is USIM, then the File may not be present*/
      if(app_type == MMGSDI_APP_SIM)
      {
        /* EF-CPS and EF-ONS short are dependent on CPHS Service Table */
        read_cphs_efs.read_csp       = FALSE;
        read_cphs_efs.read_ons_short = FALSE;
      }
      else if(app_type == MMGSDI_APP_USIM)
      {
        if (!mmgsdi_nv_is_ens_enabled(slot))
        {
          /* EF-CPS and EF-ONS are dependent on CPHS Service Table
             and 3G cards with ENS flag enabled do not use CPHS
             Service Table so continue with CSP and ONS short read */
          read_cphs_efs.read_csp       = FALSE;
          read_cphs_efs.read_ons_short = FALSE;
        }
      }
    }
  }

  if(read_cphs_efs.read_csp)
  {
    /* --------------------------------------------------------------------------
       Next Read and Cache the Customer Service Profile File:
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_card_init_cache_binary(session_id,
                                                  app_type,
                                                  slot,
                                                  csp_filename,
                                                  &mmgsdi_data_buf);
    if (mmgsdi_status == MMGSDI_SUCCESS )
    {
      /* --------------------------------------------------------------------------
         At this point, we will Parse the CSP File in order to save the information
         to the gsdi_data type for future use as needed.
         ------------------------------------------------------------------------*/
      mmgsdi_status = mmgsdi_cphs_parse_csp_data(mmgsdi_data_buf,
                                                 session_info_ptr->session_type);
      if ( mmgsdi_status != MMGSDI_SUCCESS )
      {
        MMGSDI_DEBUG_MSG_ERROR_1("Failure parsing CSP: 0x%x, CSP Not Valid",
                                 mmgsdi_status);
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);

      /*-------------------------------------------------------------------------
        2G CARD Configuration:
        DF GSM -> EF CPHSI, EF CSP

        3g Card Configuration
        ADF-> EF CSP
        DF_GSM -> EF CPHSI

        With 3G cards ,the CPHSI file is only present under DF GSM and not under
        ADF, hence the read of the USIM CPHSI will result in an error. Since CSP
        is still present in the ADF , it implies that the card supports CPHS.

        cphs_capable suggest if the card supports CPHS or not.

        Set the CPHS Available to TRUE so that the applications can read the
        other information from cphs_info globals
        -----------------------------------------------------------------------*/
      if(read_cphsi_status != MMGSDI_SUCCESS &&
         app_type == MMGSDI_APP_USIM)
      {
        cphs_int_info_ptr->cphs_info.cphs_capable = TRUE;
        cphs_int_info_ptr->cphs_info.cphs_service.csp =
          MMGSDI_ACTIVATED_AND_ALLOCATED;
        cphs_int_info_ptr->cphs_info.cphs_service.ops =
          MMGSDI_ACTIVATED_AND_ALLOCATED;
      }
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Failure reading CSP: 0x%x, CSP Not Supported",
                               mmgsdi_status);
    }
  }

  if(read_cphs_efs.read_ons)
  {
    /* --------------------------------------------------------------------------
       Next Read and Cache the CPHS ONS File:
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_card_init_cache_binary( session_id,
                                                   app_type,
                                                   slot,
                                                   chps_ons_filename,
                                                   &mmgsdi_data_buf);
    if (mmgsdi_status == MMGSDI_SUCCESS )
    {
      if(mmgsdi_data_buf.data_len > 0 && mmgsdi_data_buf.data_ptr != NULL)
      {
        cphs_int_info_ptr->cphs_ons_len = mmgsdi_data_buf.data_len;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cphs_int_info_ptr->cphs_ons_ptr);
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(cphs_int_info_ptr->cphs_ons_ptr,
          cphs_int_info_ptr->cphs_ons_len);

        if (cphs_int_info_ptr->cphs_ons_ptr != NULL)
        {
          mmgsdi_memscpy((void *)(cphs_int_info_ptr->cphs_ons_ptr),
                         cphs_int_info_ptr->cphs_ons_len,
                         (void *)mmgsdi_data_buf.data_ptr,
                         cphs_int_info_ptr->cphs_ons_len);
        }
        else
        {
          cphs_int_info_ptr->cphs_ons_len = 0;
        }
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Failure reading ONS: 0x%x, ONS Not Supported",
                               mmgsdi_status);
    }
  }

  if(read_cphs_efs.read_ons_short)
  {
    /* --------------------------------------------------------------------------
       Next Read and Cache the CPHS ONS SHORT File:
       ------------------------------------------------------------------------*/
    mmgsdi_status = mmgsdi_card_init_cache_binary( session_id,
                                                   app_type,
                                                   slot,
                                                   cphs_ons_short_filename,
                                                   &mmgsdi_data_buf);
    if (mmgsdi_status == MMGSDI_SUCCESS )
    {
      if(mmgsdi_data_buf.data_len > 0 && mmgsdi_data_buf.data_ptr != NULL)
      {
        cphs_int_info_ptr->cphs_ons_short_len = mmgsdi_data_buf.data_len;
        MMGSDIUTIL_TMC_MEM_FREE_NULL_OK(cphs_int_info_ptr->cphs_ons_short_ptr);
        MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
          cphs_int_info_ptr->cphs_ons_short_ptr,
          cphs_int_info_ptr->cphs_ons_short_len);

        if (cphs_int_info_ptr->cphs_ons_short_ptr != NULL)
        {
          mmgsdi_memscpy((void *)(cphs_int_info_ptr->cphs_ons_short_ptr),
                         cphs_int_info_ptr->cphs_ons_short_len,
                         (void *)mmgsdi_data_buf.data_ptr,
                         cphs_int_info_ptr->cphs_ons_short_len);
        }
        else
        {
          cphs_int_info_ptr->cphs_ons_short_len = 0;
        }
      }

      MMGSDIUTIL_TMC_MEM_FREE(mmgsdi_data_buf.data_ptr);
    }
    else
    {
      MMGSDI_DEBUG_MSG_ERROR_1("Failure reading ONS Short: 0x%x, ONS Short Not Supported",
                               mmgsdi_status);
    }
  }

  /* --------------------------------------------------------------------------
     Cleanup procedures to free up any memory allocated for this function
     ------------------------------------------------------------------------*/
  return MMGSDI_SUCCESS;
} /* mmgsdi_cphs_post_pin1_init */


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
  mmgsdi_plmn_info_type         * plmn_info_ptr)
{
  boolean                     is_ons_retrieved  = FALSE;
  mmgsdi_cphs_int_info_type * cphs_int_info_ptr = NULL;

  UIM_SELECTIVE_MSG_HIGH_0(UIM_LOG_EONS, "Get cphs ons");

  if(plmn_info_ptr == NULL)
  {
    return is_ons_retrieved;
  }

  if(mmgsdi_cphs_get_cphs_info_ptr(session_type,
                                   &cphs_int_info_ptr) != MMGSDI_SUCCESS)
  {
    return is_ons_retrieved;
  }
  if(cphs_int_info_ptr == NULL)
  {
    return is_ons_retrieved;
  }

  /*read CPHS ONS cache*/
  if(cphs_int_info_ptr->cphs_ons_ptr != NULL &&
     cphs_int_info_ptr->cphs_ons_len > 0)
  {
    uint8 len = 0;

    /*get first occurrence of 0xFF in CPHS ONS Buffer*/
    unsigned char *cphs_ons_buff_ptr =
        (unsigned char *)memchr (cphs_int_info_ptr->cphs_ons_ptr,
                                0xFF, cphs_int_info_ptr->cphs_ons_len);

    /*if pointer is not NULL, an occurrence of 0xFF is found*/
    if(cphs_ons_buff_ptr)
    {
      /*Calculate length of CPHS ONS buffer before ocurrence of 0xFF*/
      len = (uint8)(cphs_ons_buff_ptr - (cphs_int_info_ptr->cphs_ons_ptr));
    }
    else
    {
      /*CPHS ONS Buffer does not have 0xFF*/
      len = (uint8)cphs_int_info_ptr->cphs_ons_len;
    }

    if(len)
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS, "cphs ons valid buffer len: 0x%x",
                               len);

      /*copy long name length*/
      plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = len;

      /*copy long name encoding*/
      plmn_info_ptr->plmn_long_name.plmn_name.eons_encoding =
        MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
        len);

      if(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr != NULL)
      {
        /*copy long name data buffer*/
        mmgsdi_memscpy(plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_ptr,
                       len,
                       cphs_int_info_ptr->cphs_ons_ptr,
                       len);
        is_ons_retrieved = TRUE;
      }
      else
      {
        plmn_info_ptr->plmn_long_name.plmn_name.eons_data.data_len = 0;

        UIM_MSG_ERR_0("Memory allocation failed for plmn_long_name eons_data.data_ptr");
      }
    }
  }

  /*read CPHS ONS SHORT cache*/
  if(cphs_int_info_ptr->cphs_ons_short_ptr != NULL &&
     cphs_int_info_ptr->cphs_ons_short_len > 0)
  {
    uint8 len = 0;

    /*get first occurrence of 0xFF in CHPS ONS SHORT Buffer*/
    unsigned char *cphs_ons_short_ptr =
        (unsigned char *)memchr (cphs_int_info_ptr->cphs_ons_short_ptr,
                                0xFF, cphs_int_info_ptr->cphs_ons_short_len);

    /*if pointer is not NULL, an occurrence of 0xFF is found*/
    if(cphs_ons_short_ptr)
    {
      /*Calculate length of CHPS ONS SHORT buffer before ocurrence of 0xFF*/
      len = (uint8)(cphs_ons_short_ptr - (cphs_int_info_ptr->cphs_ons_short_ptr));
    }
    else
    {
      /*CPHS ONS SHORT Buffer does not have 0xFF*/
      len = (uint8)cphs_int_info_ptr->cphs_ons_short_len;
    }

    if(len)
    {
      UIM_SELECTIVE_MSG_HIGH_1(UIM_LOG_EONS,
                               "cphs ons short valid buffer len: 0x%x", len);

      /*copy short name length*/
      plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = len;

      /*copy short name encoding*/
      plmn_info_ptr->plmn_short_name.plmn_name.eons_encoding =
        MMGSDI_EONS_ENC_GSM_7BIT_DEF_ALPHA_UNPACKED;

      MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(
        plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
        len);

      if(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr != NULL)
      {
        /*copy short name data buffer*/
        mmgsdi_memscpy(plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_ptr,
                       len,
                       cphs_int_info_ptr->cphs_ons_short_ptr,
                       len);
        is_ons_retrieved = TRUE;
      }
      else
      {
        plmn_info_ptr->plmn_short_name.plmn_name.eons_data.data_len = 0;

        UIM_SELECTIVE_MSG_ERR_0(UIM_LOG_EONS,
                                "Memory allocation failed for plmn_short_name eons_data.data_ptr");
      }
    }
  }
  UIM_SELECTIVE_MSG_HIGH_2(UIM_LOG_EONS,
                           "cphs ons data long len: 0x%x, short len: 0x%x",
                 cphs_int_info_ptr->cphs_ons_len,
                 cphs_int_info_ptr->cphs_ons_short_len);

  return is_ons_retrieved;
}/* mmgsdi_cphs_get_ons */


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
)
{
  mmgsdi_return_enum_type                 mmgsdi_status = MMGSDI_SUCCESS;
  uint8                                   index         = 0;
  mmgsdi_session_type_enum_type           session_type  = MMGSDI_MAX_SESSION_TYPE_ENUM;

  /* --------------------------------------------------------------------------
     Validate the Input Parameters checks.
     ----------------------------------------------------------------------- */
  MMGSDIUTIL_RETURN_IF_NULL(req_ptr);

  /* --------------------------------------------------------------------------
     Validate input Session type: has to be a GW provisioning type
     ----------------------------------------------------------------------- */
  mmgsdi_status = mmgsdi_util_get_session_type(req_ptr->request_header.session_id,
                                               &session_type,
                                               NULL);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    UIM_MSG_ERR_1("Could not retrieve Session Type for Session ID: 0x%x",
                  req_ptr->request_header.session_id);
    return MMGSDI_INCORRECT_PARAMS;
  }

  switch (session_type)
  {
    case MMGSDI_GW_PROV_PRI_SESSION:
    case MMGSDI_GW_PROV_SEC_SESSION:
    case MMGSDI_GW_PROV_TER_SESSION:
      break;

    default:
      return MMGSDI_INCORRECT_PARAMS;
  }

  /* --------------------------------------------------------------------------
     Get an index from the table used to store client information.
     ------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_fill_and_alloc_request_table(MMGSDI_SESSION_GET_CPHS_INFO_REQ,
                                                           req_ptr,
                                                           NULL,
                                                           &index);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  /* send out the response */
  mmgsdi_status = mmgsdi_util_queue_mmgsdi_uim_report_rsp(
                    index, req_ptr->request_header.request_type,
                    MMGSDI_SUCCESS);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    mmgsdi_util_free_client_request_table_index(index);
  }

  return mmgsdi_status;
}/* mmgsdi_gen_get_cphs_info */


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
  mmgsdi_return_enum_type                 mmgsdi_status)
{
  mmgsdi_cphs_int_info_type    *cphs_int_info_ptr = NULL;
  mmgsdi_session_type_enum_type session_type      = MMGSDI_MAX_SESSION_TYPE_ENUM;

  MMGSDIUTIL_RETURN_IF_NULL(session_get_cphs_info_cnf_ptr);

  if(mmgsdi_status == MMGSDI_SUCCESS)
  {
    if(mmgsdi_util_get_session_type(session_id,
                                    &session_type,
                                    NULL) != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    if(mmgsdi_cphs_get_cphs_info_ptr(session_type,
                                     &cphs_int_info_ptr) != MMGSDI_SUCCESS)
    {
      return MMGSDI_ERROR;
    }

    if(cphs_int_info_ptr == NULL)
    {
      UIM_MSG_ERR_0("Null cphs_info_ptr");
      return MMGSDI_ERROR;
    }

    session_get_cphs_info_cnf_ptr->cphs_info.cphs_capable =
               cphs_int_info_ptr->cphs_info.cphs_capable;

    mmgsdi_memscpy(&session_get_cphs_info_cnf_ptr->cphs_info.cphs_service,
      sizeof(session_get_cphs_info_cnf_ptr->cphs_info.cphs_service),
      &cphs_int_info_ptr->cphs_info.cphs_service,
      sizeof(mmgsdi_cphs_service_table_type));

    mmgsdi_memscpy(&session_get_cphs_info_cnf_ptr->cphs_info.cphs_csp,
      sizeof(session_get_cphs_info_cnf_ptr->cphs_info.cphs_csp),
      &cphs_int_info_ptr->cphs_info.cphs_csp,
      sizeof(mmgsdi_cphs_cust_service_profile_type));
  }

  return MMGSDI_SUCCESS;
}/*mmgsdi_cnf_build_session_get_cphs_info*/


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
)
{
  mmgsdi_task_cmd_type                  *task_cmd_ptr     = NULL;
  mmgsdi_session_get_cphs_info_req_type *msg_ptr          = NULL;
  mmgsdi_return_enum_type                mmgsdi_status    = MMGSDI_ERROR;
  uint32                                 task_cmd_len     = 0;

  /*---------------------------------------------------------------------------
    Input Parameters Check
    1) Session ID
    2) Response CB function Check
    3) Memory allocation for msg_ptr and check for allocation
  ---------------------------------------------------------------------------*/
  mmgsdi_status = mmgsdi_util_is_session_id_valid_in_cmd(
                    session_id,
                    MMGSDI_SESSION_GET_CPHS_INFO_REQ);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  MMGSDIUTIL_RETURN_IF_NULL(response_cb_ptr);

  task_cmd_len = sizeof(mmgsdi_task_cmd_type);

  MMGSDIUTIL_MEM_MALLOC_AND_VALIDATE(task_cmd_ptr, task_cmd_len);
  if (task_cmd_ptr == NULL)
  {
    return MMGSDI_MEMORY_ERROR_HEAP_EXHAUSTED;
  }
  /*---------------------------------------------------------------------------
    - Align pointers of the various structures members
  ---------------------------------------------------------------------------*/
  mmgsdi_util_align_mem( &task_cmd_ptr->cmd.cmd.session_get_cphs_info_req,
                         MMGSDI_SESSION_GET_CPHS_INFO_REQ);

  msg_ptr = &task_cmd_ptr->cmd.cmd.session_get_cphs_info_req;

  /*---------------------------------------------------------------------------
    Populating mmgsdi_session_get_cphs_info_req_type header
      1) Session ID
      2) Request TYPE
      3) Client Data Pointer
      4) Response CallBack
      5) Payload len and request len will be populated after the content
         has been populated
  ---------------------------------------------------------------------------*/
  task_cmd_ptr->cmd.cmd_enum                = MMGSDI_SESSION_GET_CPHS_INFO_REQ;
  msg_ptr->request_header.request_type      = MMGSDI_SESSION_GET_CPHS_INFO_REQ;
  msg_ptr->request_header.orig_request_type = MMGSDI_SESSION_GET_CPHS_INFO_REQ;
  msg_ptr->request_header.session_id        = session_id;
  msg_ptr->request_header.client_data       = client_ref;
  msg_ptr->request_header.response_cb       = response_cb_ptr;

  /* Put on mmgsdi command queue */
  mmgsdi_status = mmgsdi_cmd(task_cmd_ptr);
  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    /* Free the pointer since the task_cmd_ptr has not been put
     onto the command queue */
    MMGSDIUTIL_TMC_MEM_FREE(task_cmd_ptr);

  }

  UIM_MSG_HIGH_1("Queue of MMGSDI command: MMGSDI_SESSION_GET_CPHS_INFO_REQ status 0x%x",
                 mmgsdi_status);

  return mmgsdi_status;
}


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
    MMGSDI_NOT_SUPPORTED  This request not supported

SIDE EFFECTS:
  NONE
=============================================================================*/
mmgsdi_return_enum_type  mmgsdi_gen_rsp_get_cphs_info (
  const mmgsdi_uim_report_rsp_type      * uim_rsp_ptr
)
{
  mmgsdi_request_header_type           req_header;
  uint32                               index           = 0;
  mmgsdi_return_enum_type              mmgsdi_status   = MMGSDI_SUCCESS;
  mmgsdi_client_req_extra_info_type  * extra_param_ptr = NULL;
  mmgsdi_sw_status_type                status_word;

  memset(&status_word, 0x00, sizeof(mmgsdi_sw_status_type));
  memset(&req_header, 0x00, sizeof(mmgsdi_request_header_type));

  /* --------------------------------------------------------------------------
     Validate the Input pointers.  Must not be NULL
     ------------------------------------------------------------------------*/
  MMGSDIUTIL_RETURN_IF_NULL(uim_rsp_ptr);

  /*---------------------------------------------------------------------------
    Obtain index
    Index should be [0, MMGSDI_MAX_CLIENT_REQ_INFO-1],
    Index is set when
    a) a success queuing of command to UIM
    b) a failure in queuing command to UIM but client_request pointer should
      be set in a valid index location in mmgsdi process command
  ---------------------------------------------------------------------------*/
  index = uim_rsp_ptr->rsp_data_index;

  MMGSDIUTIL_RETURN_IF_OUT_OF_RANGE(index, 0, MMGSDI_MAX_CLIENT_INFO-1);

  mmgsdi_status = mmgsdi_util_get_client_request_table_info( index,
                                                            &req_header,
                                                            &extra_param_ptr);

  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    return mmgsdi_status;
  }

  mmgsdi_status = mmgsdi_cnf_build_and_queue(uim_rsp_ptr->mmgsdi_error_status,
                                             &req_header,
                                             extra_param_ptr,
                                             NULL,
                                             FALSE,
                                             status_word);

  return mmgsdi_status;
} /* mmgsdi_gen_rsp_get_cphs_info */
