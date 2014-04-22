/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC UPLINK MODULE INTERFACE DEFINITION
***
***
*** DESCRIPTION
***
***  Interface definition of RLC uplink to the L2 task control module.
***
***
*** Copyright (c) 2001-2017 Qualcomm Technologies, Inc.
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
*** $Header: //components/rel/geran.mpss/5.2.0/grlc/src/grlcul.h#3 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/29/01    hv     Created
*** 09/19/12    hv     Added dual data space functionality
***
*****************************************************************************/

#ifndef INC_GRLCUL_H
#define INC_GRLCUL_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "gmutex.h"
#include "geran_multi_sim.h"
#include "grlcmaccmn.h"
#include "grlci.h"
#include "queue.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*-----------------------------------------------------------------
** Critical sections to protect GRLC UL message and variable access
**-----------------------------------------------------------------
*/

extern rex_crit_sect_type grlc_ul_msg_crit_sec;
extern rex_crit_sect_type grlc_ul_no_data_crit_sec;
extern rex_crit_sect_type grlc_ul_tx_vars_crit_sec;
extern rex_crit_sect_type grlc_ul_procedure_crit_sec;


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/
#ifdef  DEBUG_GSM_GPRS_DS_UNIT_TEST
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST */

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/




/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      RLC_UPLINK_INIT
===
===  DESCRIPTION
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
===    return value written to module var cv.
===
===========================================================================*/
void grlc_ul_init_vars(void);

/*===========================================================================
===
===  FUNCTION      GRLC_UL_HANDLE_NO_PS_ACCESS()
===
===  DESCRIPTION
===
===    Set sub-state to no access and delete PDU as per indicated by the flag.
===    If RLC is in user suspended state then delete all PDUs. Otherwise, delete
===    the current PDU if DEL_ONE is indicated, or delete no PDU if DEL_NONE is
===    indicated
===
===  DEPENDENCIES
===
===  PARAM
===    delete_pdu_flag: DEL_NONE or DEL_ONE
===
===  RETURN VALUE
===    none
===
===  SIDE EFFECTS
===
===========================================================================*/
void grlc_ul_handle_no_ps_access (delete_pdu_cause_t flag);

/*===========================================================================
===
===  FUNCTION      INDICATE_UL_RELEASE()
===
===  DESCRIPTION
===
===    Send an uplink release indication to MAC with a release cause.
===
===  DEPENDENCIES
===
===    None
===
===  PARAMS
===
===    release cause of type rm_tbf_release_t
===
===  RETURN VALUE
===
===    None
===
===  SIDE EFFECTS
===
===    None
===
============================================================================*/
void indicate_ul_release (rm_tbf_release_t rel_cause );

#ifdef FEATURE_GERAN_POWER_EFFICIENT_OPERATION
void grlc_ul_ext_tbf_start_ind (void);
#endif /* FEATURE_GERAN_POWER_EFFICIENT_OPERATION */

extern void grlc_ul_free_tx_array(void);

#endif /* INC_GRLCUL_H */

/*** EOF: don't remove! ***/
