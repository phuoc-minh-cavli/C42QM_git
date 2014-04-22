#ifndef LWM2M_ID_MAPPING_H
#define LWM2M_ID_MAPPING_H
#ifdef LWM2M_AT_ENABLE
/*===========================================================================
                         LWM2M_ID_MAPPING_H.H

DESCRIPTION
  This header file contains definitions used internally by the lwm2m id mapping  
  Module.

  Copyright (c)2018, 2019 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/
#include "qapi_lwm2m.h"
#include "at_handler_utility.h"
/*------------------------------------------------------------------------------
  MACRO declaration
------------------------------------------------------------------------------*/
#define LWM2M_IDMAP_MAX_LINE 128            /*Max length of file line*/
#define LWM2M_IDMAP_MAP_FILE "/datatx/lwm2m_id_map.txt"
#define LWM2M_IDMAP_ORG_TO_MAP TRUE
#define LWM2M_IDMAP_MAP_TO_ORG FALSE
#define NIL    0
/*----------------------------------------------------------------------------
  Structure define
----------------------------------------------------------------------------*/
//lwm2m id mapping endpoint
typedef struct lwm2m_idmap_ep_s
{
  struct lwm2m_idmap_ep_s *    next;
  uint16_t                     org_id;      /*origianl id*/
  uint16_t                     map_id;      /*mapped id*/
  qapi_Net_LWM2M_ID_t          id_type;     /*ID type*/
  struct lwm2m_idmap_ep_s *    sub_hdr;     /*resource id endpoint header*/
} lwm2m_idmap_ep_t;

/*------------------------------------------------------------------------------
  Function declaration
------------------------------------------------------------------------------*/
atfwd_result_code_t lwm2m_idmap_ObjId_map(uint16_t * in_id, boolean direct);
atfwd_result_code_t lwm2m_idmap_ObjInfo_map(qapi_Net_LWM2M_Obj_Info_t * in_ptr, boolean direct);
atfwd_result_code_t lwm2m_idmap_FlatData_map(qapi_Net_LWM2M_Flat_Data_t * in_ptr, size_t in_len, uint16 obj_id, boolean direct);
atfwd_result_code_t lwm2m_idmap_efs_read(void);
atfwd_result_code_t lwm2m_idmap_efs_writeline(uint8 * buffer, size_t len, boolean is_end);
int lwm2m_data_getline(char **lineptr, size_t *n, int stream);
#endif /*LWM2M_AT_ENABLE*/
#endif
