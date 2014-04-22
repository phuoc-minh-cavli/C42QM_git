/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


S T U B S   F O R   M M G S D I   E  U I C C   R E L A T E D  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains stubs for routines used for eUICCs.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2017 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_euicc_m2m_stubs.c#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/19   vgd     GET EID for M2M cards where LPA is not supported
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "mmgsdiutil.h"

#ifdef FEATURE_UIM_TEST_FRAMEWORK
#error code not present
#endif /* FEATURE_UIM_TEST_FRAMEWORK */



/*===========================================================================
   FUNCTION:      MMGSDI_PROCESS_GET_M2M_EID

   DESCRIPTION:
     This function processes the GET_EID by performing the following process.
     1) Open channel
     2) Select ECASD
     3) Send GET_DATA APDU to get EID
     4) Close channel

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

   SIDE EFFECTS:
     None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_get_m2m_eid(
  mmgsdi_task_cmd_type        *task_cmd_ptr
)
{
  (void) task_cmd_ptr;
  return MMGSDI_ERROR;
} /* mmgsdi_process_get_m2m_eid */


/* =============================================================================
   FUNCTION:      MMGSDI_CNF_BUILD_GET_M2M_EID_DATA

   DESCRIPTION:
     This function build the get_eid confirmation specific data

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
mmgsdi_return_enum_type mmgsdi_cnf_build_get_m2m_eid_data(
  mmgsdi_get_m2m_eid_cnf_type       * eid_cnf_ptr,
  mmgsdi_return_enum_type             mmgsdi_status,
  const mmgsdi_send_apdu_data_type  * data_read_ptr
)
{
  (void) data_read_ptr;
  (void) mmgsdi_status;
  (void) eid_cnf_ptr;
  
  return MMGSDI_ERROR;
} /* mmgsdi_cnf_build_get_m2m_eid_data */
