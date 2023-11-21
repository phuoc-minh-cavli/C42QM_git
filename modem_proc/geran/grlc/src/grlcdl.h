/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC DOWNLINK FUNCTIONS
***
***
*** DESCRIPTION
***
***  This module implements the RLC downlink functions
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcdl.h#3 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCDL_H
#define INC_GRLCDL_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "grlcdli.h"
#include "grlcrx.h"
#include "grlcasm.h"
#include "grlctestmode.h"
#include "gmutex.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


extern rex_crit_sect_type grlc_dl_msg_crit_sec;


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

/* Turn this on to show PDAN. Default on
*/
#define GRLD_SHOW_PDAN

/* Turn this on to show the BSN of the block received
*/
#undef  GRLD_SHOW_BSN

/* Turn this on to show content of data blocks received. Default off
*/
#undef  GRLD_SHOW_BLOCK

/* Turn this on to show DL data blocks being assembled. Default off
*/
#undef  GRLD_SHOW_BLOCK_ASSEMBLED

/* Turn this on to show PDU being sent to LLC. Default off
*/
#define  GRLD_SHOW_PDU_SENT

/* Turn this on to show downlink assignment received. Default off
*/
#define GRLD_SHOW_DL_ASS

/* Turn this on to show downlink release received. Default off
*/
#undef  GRLD_SHOW_DL_REL

/* Turn this on to show general timer expiry indication received. Default off
*/
#undef  GRLD_SHOW_TIMER_IND

/* Turn this on to show T3190 expiry. Default off
*/
#define  GRLD_SHOW_T3190

/* Turn this on to show T3192 expiry. Default off
*/
#undef  GRLD_SHOW_T3192

/* Turn this on to show pertinent downlink variables
*/
#define  ERLD_SHOW_DL_VARS
#define  ERLD_SHOW_EPDAN
#undef  ERLD_SHOW_BLOCK

/* Turn on to show individual codewords being formed. Default off
*/
#undef  ERLD_SHOW_DL_CODEWORDS


/* Turn this on to show PH_DATA_IND received from PL1. Default off
*/
#undef  GRLD_SHOW_PH_DATA_IND

/* Turn this on to show when an element of the RX array is cleared whilst the
** DSM payload is not NULL. This could happen when the array is cleared after
** an abonormal release
*/
#undef  GRLD_SHOW_NON_NULL_PAYLOAD


#ifdef  GRLD_SHOW_BLOCK
#define SHOW_BLOCK(b)   show_block(b)
#else
#define SHOW_BLOCK(b)
#endif

/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

extern grlc_dl_t dl;

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      RLC_DL_INIT()
===
===  DESCRIPTION
===
===    Power up init of rlc downlink operation.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
extern void grlc_dl_init_vars(void);

/*===========================================================================
===
===  FUNCTION       GRLC_DL_RELEASE()
===
===  DESCRIPTION
===
===    Perform release of the downlink TBF. This applies to both abnormal and
===    normal as the actions are the same.
===
===    This function and related code was put in to anticipate MAC sending
===    an explicit release signal (MR_DL_RELEASE_IND) during Cell Reselection.
===    MAC is to determine if this signal is required. In not this handler
===    and associated code will be removed.
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===========================================================================*/
extern void grlc_dl_release (void);

#ifdef FEATURE_GPRS_PS_HANDOVER
/*===========================================================================
===
===  FUNCTION      grlc_dl_clr_psho_vars
===
===  DESCRIPTION
===
===
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
extern void grlc_dl_clr_psho_vars (void);

#endif /* FEATURE_GPRS_PS_HANDOVER */

extern void grlc_dl_free_rx_array(void);

#endif /* INC_GRLCDL_H */

/*** EOF: don't remove! ***/
