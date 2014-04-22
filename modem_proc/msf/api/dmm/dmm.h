

/**
  @file dmm.h

  @brief  
*/
/*
  Copyright (c) 2018 Qualcomm Technologies, Inc.
              All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

All ideas, data and information contained in or disclosed by this
document are confidential and proprietary information of
QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
By accepting this material the recipient agrees that this material
and the information contained therein are held in confidence and in
trust and will not be used, copied, reproduced in whole or in part,
nor its contents revealed in any manner to others without the express
written permission of QUALCOMM Technologies Incorporated.

$Header: //components/rel/msf.mpss/1.0/api/dmm/dmm.h#3 $
$DateTime: 2019/03/15 01:04:02 $
$Author: pwbldsvc $
*/

#pragma once
#include "idyn.h"
#include "dmm_macros.h"

/* Enum for dynamic module id */
typedef enum dm_id_e_type
{
  DM_ID_MIN = 0,
  DM_ID_NB1 = 0,
  DM_ID_M1,
  DM_ID_GERAN,
  DM_ID_GNSS,
  DM_ID_FOO,
  DM_ID_NUMS,
  DM_ID_MAX = (DM_ID_NUMS - 1),
} dm_id_e_type;

/* Enum for dynamic module state */
typedef enum dm_state_e_type {
  DM_STATE_INACTIVE = 0,
  DM_STATE_ACTIVE,
  DM_STATE_SUSPENDED,
  DM_STATE_TRANSITION,
  DM_STATE_INVALID
} dm_state_e_type;

#define DM_ID_MASK_ALL   \
    ((1<< DM_ID_NB1)  |  \
    (1<< DM_ID_M1)    |  \
    (1<< DM_ID_GERAN) |  \
    (1<< DM_ID_GNSS)  |  \
    (1<< DM_ID_FOO))

typedef uint32 dm_id_mask_type;

typedef uint32 dmif_id_type;

/* Enum for dm event type */
typedef enum dm_event_e_type
{
  DM_EVENT_CLOSE = 0,
  DM_EVENT_OPEN = 1,
  DM_EVENT_SUSPEND = 2,
  DM_EVENT_RESUME = 3,
} dm_event_e_type;


#define DM_EVENT_MASK_ALL      \
    ((1<< DM_EVENT_CLOSE)   |  \
    (1<< DM_EVENT_OPEN)     |  \
    (1<< DM_EVENT_SUSPEND)  |  \
    (1<< DM_EVENT_RESUME))

typedef uint32 dm_event_mask_type;

/* Typedef of cb function provided by client for DM event update */
typedef void (*dm_event_cb_fn)(dm_id_e_type dmid, dm_event_e_type event, dm_status_e_type status);

/*-------- dmm_open_module --------*/
/**
@brief  This is called from the shim layer by whatever API it is (like
        "tech_start") that says we're going to need to use the tech.
        The caller task will be blocked When eventCb is set to NULL, until
        dynamic module open request is completed. While with valid eventCb 
        function, the caller task will not be blocked, and eventCb function
        will be called after dynamic module open request is completed.
*/
dm_status_e_type dmm_open_module(dm_id_e_type dmId,
  dm_event_cb_fn eventCb);

/*-------- dmm_close_module --------*/
/**
@brief  Used in any layer to close the dynamic module object.
        The caller task will be blocked When eventCb is set to NULL, until
        dynamic module close request is completed. While with valid eventCb 
        function, the caller task will not be blocked, and eventCb function
        will be called after dynamic module close request is completed.
*/
dm_status_e_type dmm_close_module(dm_id_e_type dmId,
  dm_event_cb_fn eventCb);

/*-------- dmm_suspend_module --------*/
/**
@brief  Used in any layer to suspend the dynamic module object.
        The caller task will be blocked When eventCb is set to NULL, until
        dynamic module suspend request is completed. While with valid eventCb 
        function, the caller task will not be blocked, and eventCb function
        will be called after dynamic module suspend request is completed.
*/
dm_status_e_type dmm_suspend_module(dm_id_e_type dmId,
  dm_event_cb_fn eventCb);

/*-------- dmm_resume_module --------*/
/**
@brief  This is called from the shim layer by whatever API it is (like
        "tech_start") that says we're going to need to use the tech.
        The caller task will be blocked When eventCb is set to NULL, until
        dynamic module resume request is completed. While with valid eventCb 
        function, the caller task will not be blocked, and eventCb function
        will be called after dynamic module resume request is completed.
*/
dm_status_e_type dmm_resume_module(dm_id_e_type dmId,
  dm_event_cb_fn eventCb);

/*-------- dmm_get_interface --------*/
/**
@brief  Used in the shim layers to get the pointer to the vtable so the
        static APIs can call the dynamic module. This API is not recommended
        to be called directly, MACRO DM_FUNC_CALL/DM_VOIDFUNC_CALL should be 
        used.
*/
void* dmm_get_interface(dmif_id_type dmifId);

/*-------- dmm_get_interface_p --------*/
/**
@brief  Used in the shim layers for special occasions to get the pointer to 
        the vtable so the private static APIs can call the dynamic module when 
        module is in process of init/deinit/suspend/resume. This API is not 
        recommended to be called directly, MACRO DM_FUNC_CALL_PRIV/
        DM_VOIDFUNC_CALL_PRIV should be used instead
*/
void* dmm_get_interface_p(dmif_id_type dmifId);

/*-------- dmm_release_interface --------*/
/**
@brief  Used in the shim layers to release the pointer to the vtable
*/
void dmm_release_interface(dmif_id_type dmifId);

/*-------- dmm_register_dm --------*/
/**
@brief  Used in DM_SO_REGISTER MACRO, or called by extpl manager for create 
        dynamic module in DMM. 
*/
extern dm_status_e_type dmm_register_dm(dm_id_e_type dmId,
  idyn_t *pDyn);

/*-------- dmm_register_for_event --------*/
/**
@brief  Used for external client to receive event updates of dynamic 
        module(s) through cb function specified in registration
        For shim layer, it may specify the cb function directly in 
        dmm_open_module and/or dmm_close_module requests.
*/
extern dm_status_e_type dmm_register_for_event(dm_id_mask_type dmIdMask,
  dm_event_mask_type eventMask,
  dm_event_cb_fn eventCb);

/*-------- dmm_continue --------*/
/**
@brief  Used for functions defined in DM_IMPL MACRO to notify DMM the result
        of queued request.
*/
extern dm_status_e_type dmm_continue(dm_id_e_type dmId,
  dm_ref_type refId);

/*-------- dmm_get_module_status --------*/
/**
@brief  Returns the current state of the given dynamic module
*/
dm_state_e_type dmm_get_module_state(dm_id_e_type dmId);

/*-------- DM_IMPL --------*/
/**
@brief  This macro goes at the start of the file that implements the dynamic module for a tech.
        It sets up the functions and default interface to support the dynamic module
*/
#define DM_IMPL(name, dmId)                                           \
  dm_status_e_type     name##_init(dm_ref_type);                      \
  dm_status_e_type     name##_deinit(dm_ref_type);                    \
  dm_status_e_type     name##_resume(dm_ref_type);                    \
  dm_status_e_type     name##_suspend(dm_ref_type);                   \
  idyn_iface_t idyn_iface_0_##name =                                  \
  {                                                                   \
  name##_init,                                                        \
  name##_deinit,                                                      \
  name##_resume,                                                      \
  name##_suspend                                                      \
  };

/*-------- DM_INTERFACE0 --------*/
/**
@brief  This macro is put at the start of the tech's vtable and works with
        DM_IMPL to provide pointers to implementations of default interface of dynamic module.
*/
#define DM_INTERFACE0(name)     &idyn_iface_0_##name

/*-------- DM_SO_REGISTER --------*/
/**
@brief  This macro goes at the end of the file the implements the shared object for a tech.
        It registers the function that is called from the shared object manager to create
        the shared object object for the tech.
*/
#ifdef FEATURE_EXTERNAL_PAGING
  #define DM_SO_REGISTER(...)
#else
  #define DM_SO_REGISTER(name, dmId)                                    \
    boolean dm_register_so(void)                                        \
    {return dmm_register_dm(dmId, (idyn_t *)&idyn_##name);}
#endif

/*-------- DM_EXPORT_EXTPL --------*/
/**
@brief  This macro goes at the end of the file the implements the shared object for a tech.
        It creates the function that is called from the shared object manager to create
        the shared object object for the tech.
*/
#ifdef FEATURE_EXTERNAL_PAGING
  #define DM_EXTPL_EXPORT(dmId, dyn)                                    \
    idyn_t * __attribute__ (( section (".rodata_dmm_extpl_intf") ))     \
    dmm_extpl_intf_##dmId = (idyn_t *)&dyn;
#else
  #define DM_EXTPL_EXPORT(...)
#endif

/*-------- DM_FUNC_CALL --------*/
/**
@brief  This macro is provided for public API to access its void private function in 
        one of the vtables/interfaces provided by dynamic module.
        ret_type nb1_l2_public_func(arg1, arg2, ...){
          ret_type ret_var = ERROR_IF_MODULE_NOT_PRESENT; 
          DM_FUNC_CALL(DMIFID_NB1_L2 NB1_L2_IFACE_T, ret_var, nb1_l2_private_func,
                       arg1, arg2, ...);
          return ret_var;
        }
*/
#define DM_FUNC_CALL(dmifId, iface_t, ret_var, pf, ...)               \
  iface_t *iface = (iface_t *)dmm_get_interface(dmifId);              \
  if (iface != NULL){                                                 \
  ret_var = _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_release_interface(dmifId);                                      \
  }

/*-------- DM_VOIDFUNC_CALL --------*/
/**
@brief  This macro is provided for public API to access its void private function in 
        one of the vtables/interfaces provided by dynamic module.
        void nb1_l2_public_void_func(arg1, arg2, ...){
          DM_VOIDFUNC_CALL(DMIFID_NB1_L2, NB1_L2_IFACE_T, nb1_l2_private_void_func,
                           arg1, arg2, ...);
        }
*/
#define DM_VOIDFUNC_CALL(dmifId, iface_t, pf, ...)                    \
  iface_t *iface = (iface_t *)dmm_get_interface(dmifId);              \
  if (iface != NULL){                                                 \
  _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_release_interface(dmifId);                                      \
  }

/*-------- DM_FUNC_CALL_PRIV --------*/
/**
@brief  This macro is provided for public API to access its void private function in 
        one of the vtables/interfaces provided by dynamic module.
        ret_type nb1_l2_public_func(arg1, arg2, ...){
          ret_type ret_var = ERROR_IF_MODULE_NOT_PRESENT; 
          DM_FUNC_CALL_PRIV(DMIFID_NB1_L2 NB1_L2_IFACE_T, ret_var, nb1_l2_private_func,
                       arg1, arg2, ...);
          return ret_var;
        }
*/
#define DM_FUNC_CALL_PRIV(dmifId, iface_t, ret_var, pf, ...)          \
  iface_t *iface = (iface_t *)dmm_get_interface_p(dmifId);            \
  if (iface != NULL){                                                 \
  ret_var = _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_release_interface(dmifId);                                      \
  }

/*-------- DM_VOIDFUNC_CALL_PRIV --------*/
/**
@brief  This macro is provided for public API to access its void private function in 
        one of the vtables/interfaces provided by dynamic module.
        void nb1_l2_public_void_func(arg1, arg2, ...){
          DM_VOIDFUNC_CALL_PRIV(DMIFID_NB1_L2, NB1_L2_IFACE_T, nb1_l2_private_void_func,
                           arg1, arg2, ...);
        }
*/
#define DM_VOIDFUNC_CALL_PRIV(dmifId, iface_t, pf, ...)               \
  iface_t *iface = (iface_t *)dmm_get_interface_p(dmifId);            \
  if (iface != NULL){                                                 \
  _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_release_interface(dmifId);                                      \
  }

/*-------- DMIF_ID_COMBINED --------*/
/**
@brief  This macro combines dynamic module ID and interface ID to be unqiue DMIF ID.
*/
#define DMIF_ID_COMBINED(dmId, ifId) (((dmId&0xFFFF) << 16) | (ifId & 0xFFFF))

//#include "dmm_debug.h"

