/****************************************************************************
***
*** TITLE
***
***  GRLC to GRR Interface Definition
***
***
*** DESCRIPTION
***
***  This module implements the RLC functions to allow MAC to send
***  messages to RLC uplink task and RLC downlink task.
***
*** 
*** EXTERNALIZED FUNCTIONS
***
***  grlc_get_cmd_buf_grr()
***  grlc_put_cmd_grr()
***
*** 
*** INITIALIZATION AND SEQUENCING REQUIREMENTS
***
***  RLC UL/DL tasks must be active and ready to receive messages from QGAN
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcgrr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
21/02/08   hv      Created header file

===========================================================================*/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */


  /* If FEATURE_GAN is undefined, a compiler warning will result because this module
  ** will contain no code - define a dummy function here 
  */
  void grlcgrr_dummy(void) { }


/*** EOF: don't remove! ***/
