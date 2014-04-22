

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

$Header: //components/rel/msf.mpss/1.0/api/dmm/dmm_debug.h#1 $
$DateTime: 2018/06/27 11:21:46 $
$Author: pwbldsvc $
*/



#pragma once

#define DMM_DEBUG

#ifdef DMM_DEBUG

#define DMM_FILE_LEN 20
typedef struct
{
  char file[DMM_FILE_LEN+1];
  uint32 line; 
  uint16 index; 
} dmm_intf_record_t;

dmm_intf_record_t * dmm_intf_record_add(
  dmif_id_type dmifId, const char * file, uint32 line);

void dmm_intf_record_rmv(
  dmif_id_type dmifId, dmm_intf_record_t * pItem);

#undef DM_FUNC_CALL
#undef DM_VOIDFUNC_CALL
#undef DM_FUNC_CALL_PRIV
#undef DM_VOIDFUNC_CALL_PRIV

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
  dmm_intf_record_t *pRecord = dmm_intf_record_add(dmifId, __FILE__, __LINE__);\
  ret_var = _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_intf_record_rmv(dmifId, pRecord);                               \
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
  dmm_intf_record_t *pRecord = dmm_intf_record_add(dmifId, __FILE__, __LINE__);\
  _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_intf_record_rmv(dmifId, pRecord);                               \
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
  dmm_intf_record_t *pRecord = dmm_intf_record_add(dmifId, __FILE__, __LINE__);\
  ret_var = _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_intf_record_rmv(dmifId, pRecord);                               \
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
  dmm_intf_record_t *pRecord = dmm_intf_record_add(dmifId, __FILE__, __LINE__);\
  _DMMPASTE(_DMMFUNC,_DMMCNT(iface->pf,##__VA_ARGS__))(iface->pf,##__VA_ARGS__); \
  dmm_intf_record_rmv(dmifId, pRecord);                               \
  dmm_release_interface(dmifId);                                      \
  }

#endif 

