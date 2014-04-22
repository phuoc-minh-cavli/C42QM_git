
#include "geran_multi_sim.h"
#include "geran_dual_sim_g.h"
#include "geran_multi_sim_api.h"
#include <stringl.h>

#ifndef PERLUTF
#include "ftm.h"
#include "subs_prio.h"
#endif /* !PERLUTF */
#include "geran_nv.h"
#include "cm.h"
#include "rex.h"
#include "gs.h"
/*!
 * \brief Converts a GERAN access stratum ID to a NAS ID.
 *
 * \param gas_id (in)
 *
 * \return sys_modem_as_id_e_type
 */
sys_modem_as_id_e_type geran_map_gas_id_to_nas_id()
{

  return(SYS_MODEM_AS_ID_1);
}
/*!
 * \brief Indicates if the given GAS-ID represents the multi-mode sub.
 *
 * \param 
 *
 * \return boolean - TRUE if this is the multi-mode sub, FALSE otherwise.
 */
boolean geran_is_multi_mode_gas_id(void )
{
  return GERAN_ACCESS_STRATUM_ID_1;
}

/*!
 * \brief Converts a NAS ID to a GERAN access stratum ID.
 *
 * \param as_id (in)
 *
 * \return gas_id_t
 */
gas_id_t geran_map_nas_id_to_gas_id(sys_modem_as_id_e_type as_id)
{
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)

  // Mapping is handled differently in FTM mode
  if (ftm_get_mode() == FTM_MODE)
  {
   }

  else if (as_id != SYS_MODEM_AS_ID_NONE)
  {
    if (as_id < MAX_AS_IDS)
    {
     }
  }
  return check_gas_id();
#else
  return GERAN_ACCESS_STRATUM_ID_1;
#endif
}
