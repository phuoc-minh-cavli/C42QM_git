/*
  @file lte_dmm.h

  @brief
  lte dmm vtble definitions

*/
#ifndef NB1_DMM_H
#define NB1_DMM_H

#if defined (FEATURE_DYNAMIC_LOADING_LTE) || defined (FEATURE_EXTERNAL_PAGING)
#include "dmm.h"
#include "lte_static_rrc_ext_api.h"
#include "lte_static_ml1_ext_api.h"
#include "lte_static_l2_ext_api.h"

extern lte_static_rrc_ext_api_s lte_nb1_rrc_vtbl;
extern lte_static_ml1_ext_api_s lte_nb1_ml1_vtbl;
extern lte_static_l2_ext_api_s lte_nb1_l2_vtbl;

typedef enum {
    LTE_NB1_DMM_INIT_INTF,
    LTE_NB1_DMM_RRC_INTF,
    LTE_NB1_DMM_ML1_INTF,
    LTE_NB1_DMM_L2_INTF,
    LTE_NB1_DMM_PLT_INTF,
    
} LTE_NB1_DMM_INTFS;

#define LTE_NB1_DMM_INIT_INTF_ID DMIF_ID_COMBINED(DM_ID_NB1, LTE_NB1_DMM_INIT_INTF)
#define LTE_NB1_DMM_RRC_INTF_ID  DMIF_ID_COMBINED(DM_ID_NB1, LTE_NB1_DMM_RRC_INTF)
#define LTE_NB1_DMM_ML1_INTF_ID  DMIF_ID_COMBINED(DM_ID_NB1, LTE_NB1_DMM_ML1_INTF)
#define LTE_NB1_DMM_L2_INTF_ID   DMIF_ID_COMBINED(DM_ID_NB1, LTE_NB1_DMM_L2_INTF)
#define LTE_NB1_DMM_PLT_INTF_ID  DMIF_ID_COMBINED(DM_ID_NB1, LTE_NB1_DMM_PLT_INTF)


typedef struct {
    IDYN_INTERFACE0;
    lte_static_rrc_ext_api_s* rrc_intf;
    lte_static_ml1_ext_api_s* ml1_intf;
    lte_static_l2_ext_api_s* l2_intf;
} idyn_lte_nb1_t;

extern dm_status_e_type lte_nb1_init();
extern dm_status_e_type lte_nb1_deinit();
extern dm_status_e_type lte_nb1_resume();
extern dm_status_e_type lte_nb1_suspend();

#endif //(FEATURE_DYNAMIC_LOADING_LTE) || defined (FEATURE_EXTERNAL_PAGING)

#endif //NB1_DMM_H