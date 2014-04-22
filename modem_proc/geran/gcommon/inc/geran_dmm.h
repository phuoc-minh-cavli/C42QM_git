/*****************************************************************************
***
*** TITLE
***
*** 
***
***
*** DESCRIPTION
***
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
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 17/01/23   sastryk     Do not de-register with MARB when the previous request is in pending accept state
*** 10/12/17   nsathya     Added dmm life cycle functions for GERAN dynamic module.
***
*****************************************************************************/

#ifndef GERAN_DMM_H
#define GERAN_DMM_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "bit.h"
#include "comdef.h"
#include "dmm.h"
#include "marb.h"
#include "rr_main.h"
#include "l2_intf.h"
#include "geran_grr_static_api.h"
#include "rr_static_main.h"
#include "l1_static_api.h"
#include "gdrv_static_api.h"
#include "gmdsp_static_api.h"
#include "gl1_msgrif_static_api.h"
#include "gmac_static_task.h"
#include "gsndcp_static_main.h"
#include "grlc_static_main.h"
#include "gllci.h"
#include "gllc_static_main.h"
#include "geran_dmm_api.h"

#define HI_32_OF_64_BITS(x) (uint32) ((x >> 32) & 0xFFFFFFFF)
#define LO_32_OF_64_BITS(x) (uint32) (x & 0xFFFFFFFF)

typedef void (*arb_cb_t) (marb_async_callback_input_t *);

typedef enum
{
  GERAN_INACTIVE,
  GERAN_UNLOADED,
  GERAN_LOADED,
  GERAN_UNLOADED_SLEEP,
  GERAN_LOADED_SLEEP,
  GERAN_UNLOADED_STOP
  ,GERAN_FSK_LOADED
} geran_dyn_state_e;

typedef enum
{
  ARB_GERAN_UNLOADED,
  ARB_GERAN_LOADED
} geran_arb_ind_e;

typedef enum
{
  ARB_RESULT_FAIL,
  ARB_RESULT_SUCCESS,
  ARB_RESULT_PENDING
} geran_arb_result_e;

typedef struct
{  
  IDYN_INTERFACE0;
#ifdef FEATURE_EXTERNAL_PAGING_GERAN
  geran_static_gl1_ext_api_s*  gl1_intf; 
  geran_static_gdrv_ext_api_s*        gdrv_intf;
  geran_static_gmdsp_ext_api_s*       gmdsp_intf;
  geran_static_gl1_msgrif_ext_api_s*  gmsgrif_intf;
  geran_gl2_intf_s*            gl2_intf; 
  geran_static_gmac_ext_api_s* gmac_intf; 
  geran_static_grlc_ext_api_s* grlc_intf;
  geran_static_gllc_ext_api_s* gllc_intf;
  geran_static_gsn_ext_api_s*  gsn_intf;
#endif
  geran_static_grr_ext_api_s*  grr_intf; 
} idyn_geran_t; 


extern dm_status_e_type geran_init();
extern dm_status_e_type geran_deinit();
extern dm_status_e_type geran_resume();
extern dm_status_e_type geran_suspend();

extern void geran_set_dyn_state(geran_dyn_state_e state);
extern geran_dyn_state_e geran_get_dyn_state(void);
extern void geran_set_curr_proc(proc_type_e proc);
extern proc_type_e geran_get_curr_proc(void);
extern void geran_set_mm_high_priority(boolean priority);
extern boolean geran_get_mm_high_priority(void);
extern boolean is_geran_loaded(void);

extern void geran_dal_init(void);
extern uint32 geran_timeticks_to_ms(uint64 tt);
extern uint64 geran_ms_to_timeticks(uint32 ms);
extern uint64 geran_get_time_ms(void);

extern void geran_arb_register(arb_cb_t cb);
extern void geran_arb_deregister(void);
extern geran_arb_result_e geran_arb_request(proc_type_e proc, uint32 duration, uint32 cleanup_time);
extern void geran_arb_sleep(proc_type_e next_proc, uint32 start_time, uint32 cleanup_time);
extern void geran_arb_indication(geran_arb_ind_e indication);
extern boolean geran_arb_status_pending_accept( void );
extern void geran_arb_reset_pending_accept(void );


extern void rr_send_mph_proc_req(proc_type_e proc);
extern void L1_send_MPH_PROC_CNF(proc_type_e proc, proc_status_e status);
extern void L1_send_MPH_PROC_IND(proc_type_e proc);
#endif

#endif /*GERAN_DMM_H*/

