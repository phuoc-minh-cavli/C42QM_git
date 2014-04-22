/*! \file geran_dual_sim.c

  Provides functions for use by GERAN entities supporting Dual SIM.

                Copyright (c) 2009-2014 QUALCOMM Technologies, Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*/
/* $Header: //components/rel/geran.mpss/5.2.0/gcommon/static/src/geran_dual_sim.c#2 $ */
/* $DateTime: 2018/10/10 02:31:27 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 *--------------------------------------------------------------------------*/

#include "geran_variation.h"
#include "customer.h"
#include "geran_dual_sim_g.h"
#include "rex.h"
#include <stringl/stringl.h>
#include "geran_msgs.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 *--------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 *--------------------------------------------------------------------------*/
 static char check_gas_id_task_name[REX_TASK_NAME_LEN+1];

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 *--------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Externalised Function Definitions
 *---------------------------------------------------------------------------*/

/*!
 * \brief Range check gas_id to ensure no buffer overflow when it is used.
 *
 * \param gas_id_t
 *
 * \return gas_id_t
 */
gas_id_t check_gas_id()
{
  return GERAN_ACCESS_STRATUM_ID_1;
}

/* EOF */

