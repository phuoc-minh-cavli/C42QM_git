#include "geran_dmm.h"

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "dmm.h"
#include "geran_tasks.h"
#include "gsmdiag.h"
#include "mm_rr.h"

DM_IMPL(geran, DM_ID_GERAN)

dm_status_e_type geran_init()   
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  geran_gl1_init();  
  geran_gl2_init();  
  geran_gmac_init();
  geran_grlc_init();  
  geran_gsn_init();
  geran_gllc_init();
#endif
  geran_grr_init();

  return DM_STATUS_DONE;
}

dm_status_e_type geran_deinit()
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  gsmdiag_deinit();
  umtsdiag_deinit();
  geran_gl1_deinit();
  geran_gl2_deinit();
  geran_gmac_deinit();
  geran_grlc_deinit();
  geran_gllc_deinit();
  geran_gsn_deinit();
#endif
  geran_grr_deinit();

  return DM_STATUS_DONE;
}

dm_status_e_type geran_resume()
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  //gsmdiag_init();
  //umtsdiag_init();
  geran_gl1_resume();
  geran_gl2_resume();
  geran_gmac_resume();
  geran_grlc_resume();
  geran_gllc_resume();
  geran_gsn_resume();
#endif
  geran_grr_resume();

  /* WWAN tech status whether tech is loaded or unloaded */
  mm_wwan_set_tech_status(TRUE);

  return DM_STATUS_DONE;
}

dm_status_e_type geran_suspend()
{
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  gsmdiag_deinit();
  umtsdiag_deinit();
  geran_gl1_suspend();
  geran_gl2_suspend();
  geran_gmac_suspend();
  geran_grlc_suspend();
  geran_gllc_suspend();
  geran_gsn_suspend();
#endif
  geran_grr_suspend();

  return DM_STATUS_DONE;
}

 static idyn_geran_t idyn_geran = 
{
  DM_INTERFACE0(geran),
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  &gl1_vtbl,
  &gdrv_vtbl,
  &gmdsp_vtbl,
  &gmsgrif_vtbl,
  &l2_vtbl,
  &mac_vtbl,
  &grlc_vtbl,
  &gllc_vtbl,
  &gsndcp_vtbl,
#endif
  &rr_vtbl
}; 


DM_EXTPL_EXPORT(DM_ID_GERAN, idyn_geran);

DM_SO_REGISTER(geran, DM_ID_GERAN)

#endif

