/*===========================================================================

              S Y S T E M   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions that are shared between Call Manager,
  Call Manager clients and the protocol stacks.


  Organization of the file:

    The file is divided into multiple sections. Each section contains
    validation routines for a particular entity.  The following entities
    are defined in this file:

        Service Indicators
        System Identifiers
        Network Lists
        Mobility Management Information

EXTERNALIZED FUNCTIONS
  All functions are externalized except for sys_plmn_id_digit_is_valid.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None


Copyright (c) 2002 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.CT.1.0/Main/modem_proc/stubs/src/sys.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/24/11   rj      MNC value incorrectly calculated in sys_plmn_get_mcc_mnc2()
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
10/17/10   am      Removing header files added for MOB_CUTE
10/16/10   am      Fixing cute compilation
08/30/10   aj      update to MMSS utility APIs
03/28/10   aj      Add sys_plmn_get_mcc_mnc2 
03/22/10   aj      KW fixes
03/21/10   aj      MMSS support
12/03/09   rm      Correct the order of input parameters in 
                   sys_plmn_list_found_plmn
10/29/09   sg      Add non-demand paging macro to function that are not to be 
                   demand-paged.
08/07/09   rm      CMI phase-2: Removing featurisation from /api files
02/11/09   ks      Validate spare_bits field in the mm_information to indicate 
                   no. of spare bits in end octet of n/w names(full/short)
08/07/07   ic      Lint clean up
11/15/06   pk      Added support for wild card MNC for GW systems
03/10/06   pk      Modified sys_sys_id_match to check for MCC and IMSI_11_12
                   for IS 95 systems
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/18/05   jqi     Replaced cmdbg.h with cmidbg.h file to reduce header file 
                   dependency.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/26/05   up      Updated sys_plmn_match to compare NA PLMNs of 2 digit MNCs 
                   when 3rd digit MNC is transmitted as '0'/'F'.
05/12/05   ic      Removed bcmcs_srv_status_is_srv()
04/04/05   ic      Removed unnecessary Lint comments 
03/22/05   up      Function sys_mcc_same_country is added to extend MCC
                   matching for NA MCC range 310-316.
03/09/05   kwa     Replaced code within sys_plmn_match with the code in
                   sys_hplmn_match.  Fixed a bug in sys_plmn_match so it
                   properly compares two PLMN IDs with 2 digit MNCs within
                   the North American country codes.
08/10/04   kwa     Added sys_hplmn_match.  Moved the functionality added to
                   sys_plmn_match in the previous update to sys_hplmn_match.
                   Removed the North American HPLMN comparison requirements
                   from sys_plmn_match.
06/24/04   kwa     Updated the function sys_plmn_match to support the PLMN
                   matching requirements for North American GSM networks as
                   defined in Annex A of 3GPP TS 23.122.
03/15/04   ws      Fixed bug with system id comparison.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/06/04   ws      Initial jaguar/mainline merge.
11/06/03   sj      Updated sys_srv_status_is_srv() for limited_regional srv.
10/24/03   ic      Changes to support common sys.c across targets
05/05/03   kwa     Updated the logic in sys_mm_information_is_valid for
                   determining if an sys_mm_information_s_type is valid to
                   account for optional elements within the structure.
                   Modified several functions to pass pointers rather than
                   structures to minimize stack utilization.
05/05/03   vt      Removed #if 0 stuff.
04/28/03   AT      Lint cleanup.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Modified sys_user_pref_plmn_list_info_is_valid() to only
                   validate plmn.
11/22/02   MKS     Added sys_network_name_coding_scheme_is_valid() to check
                   if a valid coding scheme is used to encode short name or
                   full name of the network.
04/01/02   PRK     Initial version
===========================================================================*/

#include "sys.h"

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_srv_status_is_srv

DESCRIPTION

  Function that verifies that the input value is indicates any service.

PARAMETERS

  sys_srv_status_e_type  value            Value that is to be checked
                                          for validity.

RETURN VALUE

  boolean  Flag indicating whether the value indicates service( TRUE)
           or no service (FALSE).

==============================================================================*/
boolean sys_srv_status_is_srv
(
  sys_srv_status_e_type  value
)
{

  if( value == SYS_SRV_STATUS_NO_SRV  ||
      value == SYS_SRV_STATUS_PWR_SAVE )
  {
    return FALSE;
  }


  return TRUE;

} /* sys_srv_status_is_srv */


