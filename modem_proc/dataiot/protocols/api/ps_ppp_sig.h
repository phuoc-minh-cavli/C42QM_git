#ifndef PS_PPP_SIG_H
#define PS_PPP_SIG_H
/*===========================================================================
                          P S _ P P P _ S I G . H

DESCRIPTION
  Header file containing the PPP signal specific information.  This is intended
  to isolate all of the signal specific information to a single module.

Copyright (c) 2013-2018 QUALCOMM Technology Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_task.h_v   1.4   31 Jan 2003 18:45:14   ubabbar  $
  $Header: //components/rel/dataiot.mpss/2.0/protocols/api/ps_ppp_sig.h#1 $ $DateTime: 2020/03/17 08:03:16 $ $Author: pwbldsvc $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/28/18    ds     Code changes for chiltepin(9x05).
11/18/14    skc    Removing Usage of PS_RX_UW_FMC_SIGNAL
11/03/13    cx     Added support for second rm rx device
05/07/09    pp     Initial release. Created as part of CMI Phase-4: SU Level 
                   API Effort.
===========================================================================*/
#include "ps_svc.h"

/*---------------------------------------------------------------------------
  define all of the signal related stuff.
---------------------------------------------------------------------------*/
typedef ps_sig_enum_type ppp_sig_type;
typedef ps_sig_handler_type ppp_sig_handler_type;

#define PPP_INVALID_SIGNAL    PS_MAX_SIGNALS

#define PPP_UM_1_RX_SIGNAL    PS_PPP_UM_SN_RX_SIGNAL
#define PPP_RM_RX_SIGNAL      PS_RM_RX_SIGNAL
#define PPP_UM_2_RX_SIGNAL    PS_PPP_UM_AN_RX_SIGNAL
#define PPP_RM_2_RX_SIGNAL    PS_RM_2_RX_SIGNAL

#endif /* PS_PPP_SIG_H */
