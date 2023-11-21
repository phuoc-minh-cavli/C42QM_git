#ifndef GL1_HW_GSAC_H
#define GL1_HW_GSAC_H
/*===========================================================================
                      GSM GPRS GSAC NPL1 FRAME LAYER HEADER FILE
DESCRIPTION
  Definitions for accessing GSAC block in MSM.  This block provides hardware
  acceleration for the GSM/GPRS ciphering algorithms.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2003-2013 Qualcomm Technologies, Inc.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/gdrivers/inc/gl1_hw_gsac.h#2 $ 
$DateTime: 2018/10/10 02:31:27 $ $Author: pwbldsvc $

when        who     what, where, why
--------    ---     --------------------------------------------------------
29/05/13    pjr     CR492594 dual dataspaced gl1_hw_gsac_init
05/18/06    gfr     Support for clock regime resource manager.
10/17/05    gfr     Moved GSAC code to gl1_hw_gsac.c
06/10/05    gfr     Rename GSM APIs to GSAC
02/12/03    gw      Added definitions for gsm clock voting functions for 
                    when FEATURE_GSM_SLEEP_CONTROLLER is not defined.
01/15/03    gw      Initial revision.
===========================================================================*/

#include "geran_variation.h"

#define GL1_HW_GSAC_CLOCK_GLLC_MASK    0x02
#define GL1_HW_GSAC_CLOCK_MDSP_MASK    0x01

void gl1_hw_gsac_clock_is_required (uint32 mask);
void gl1_hw_gsac_clock_not_required (uint32 mask);


void gl1_hw_gsac_init(void);
boolean gl1_hw_gsac_inact (boolean force);

#endif /* GL1_HW_GSAC_H */
