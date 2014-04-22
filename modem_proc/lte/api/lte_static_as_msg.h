/*!
  @file lte_static_as_msg.h

  @brief  Defines the CPHY and L2 messages interafce common to CATM1 and NB1


*/

/*===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/lte.mpss/4.7.2/api/lte_static_as_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/24/18   kss    Initial Revision

===========================================================================*/

#ifndef LTE_STATIC_AS_MSG_H
#define LTE_STATIC_AS_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <stm2.h>
#include <msgr_lte.h>
#include <msgr.h>
#include <IxErrno.h>
#include "../cust/inc/custlte.h"
#include <lte_cphy_msg_ids.h>

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
#define LTE_STATIC_INVALID_DMM_INTF_ID 0xFFFFFFFF

typedef enum
{
  LTE_STATIC_AS_ACTIVE_SYS_TYPE_NONE,
  LTE_STATIC_AS_ACTIVE_SYS_TYPE_M1,
  LTE_STATIC_AS_ACTIVE_SYS_TYPE_NB1
} lte_static_as_active_sys_type_e;

typedef enum
{
  LTE_STATIC_AS_REQ_NONE,
  LTE_STATIC_AS_INIT_REQ,
  LTE_STATIC_AS_DEINIT_REQ
} lte_static_as_init_req_type_e;


#define LTE_NO_TASK_PENDING      0x00
#define LTE_ML1_TASK          0x01
#define LTE_MAC_DL_TASK       0x02
#define LTE_MAC_UL_TASK       0x04
#define LTE_MAC_CTRL_TASK     0x08
#define LTE_RLC_UL_TASK       0x10
#define LTE_PDCP_COMP_TASK    0x20
#if defined (FEATURE_LTE_UDC_ENABLED) || defined (FEATURE_NB1_UDC_ENABLED)
#define LTE_ALL_TASK          0x3F
#else
#define LTE_ALL_TASK          0x1F
#endif

/*! @brief Task mask bit type (Max 11 tasks for now) */
typedef uint16 lte_as_task_mask_t;

#define LTE_ML1_NO_TASK       0x00
#define LTE_ML1_MGR_TASK      0x01
#define LTE_ML1_GM_TASK       0x02
#define LTE_ML1_OFFLOAD_TASK  0x04
#define LTE_ML1_ALL_TASK      0x07

/*! @brief Task mask bit type (Max 11 tasks for now) */
typedef uint16 lte_ml1_task_mask_t;


/*! @brief
  This message is used to signal ML1 to start handling commands in M1 or NB1 mode of
  operation.
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*request type to indicate for init/deinit */
  lte_static_as_init_req_type_e req_type;

  /*! Device system type */
  lte_static_as_active_sys_type_e  active_sys_type;

} lte_cphy_init_req_s;


/*! @brief
  This message confirms the start request
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr; 

  /*! Whether or not the start was successful */
  errno_enum_type      status;

  //cnf from task (bit mask)
  lte_as_task_mask_t cnf_task_mask;
  
} lte_cphy_init_cnf_s;

#ifdef FEATURE_FMD_SPEED_INFO
/*! @brief Message for FMD (Flight Mode Decision) Speed Info Request (CATM1:RRC to ML1)
*/
typedef struct
{
  /*!< Message router header */
  msgr_hdr_s msg_hdr; 
}lte_cphy_fmd_speed_info_req_s;

/*! @brief Message for FMD (Flight Mode Decision) Speed Info Cnf (CATM1:ML1 to RRC)
*/
typedef struct {
  /*!< Message router header */
  msgr_hdr_s msg_hdr; 
  
  /*!< Provides the absolute speed in kmph as estimated by the modem firmware.
       Agreement is, ML1 will send response only with valid speed info */
  uint16 speed;
}lte_cphy_fmd_speed_info_cnf_s;
#endif /* FEATURE_FMD_SPEED_INFO */

/*! @brief LOAD/UNLOAD cnf msg to L1M from ML1 modules
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type    msgr_hdr;
  /*! ML1 module bitmask */
  lte_ml1_task_mask_t cnf_task_mask;
  /*! Status */
  errno_enum_type         status;
} lte_cphy_load_unload_cnf_s;

/* list of msgs sent from static RRC code to ML1 and L2 */
enum
{
  /* LTE_CPHY_INIT_REQ/LTE_CPHY_INIT_CNF to/from ML1/L2 tasks */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, INIT, LTE_CPHY_INIT_REQ_ID, lte_cphy_init_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, INIT, LTE_CPHY_INIT_REQ_ID, lte_cphy_init_cnf_s),

  /* DMM framework related ML1 internal UMIDs. Loads/Unloads GM and offload tasks */
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, LOAD, LTE_CPHY_LOAD_REQ_ID, msgr_hdr_struct_type ),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, LOAD, LTE_CPHY_LOAD_REQ_ID, lte_cphy_load_unload_cnf_s ),
  
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, UNLOAD, LTE_CPHY_UNLOAD_REQ_ID, msgr_hdr_struct_type ),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, UNLOAD, LTE_CPHY_UNLOAD_REQ_ID, lte_cphy_load_unload_cnf_s ), 

  /* FMD (Flight Mode Decision) Speed Info Req/Cnf 
     (LTE_CPHY_FMD_SPEED_INFO_REQ/LTE_CPHY_FMD_SPEED_INFO_CNF)to/from ML1 */
  #ifdef FEATURE_FMD_SPEED_INFO
  MSGR_DEFINE_UMID(LTE, CPHY, REQ, FMD_SPEED_INFO, LTE_CPHY_FMD_SPEED_INFO_ID, lte_cphy_fmd_speed_info_req_s),
  MSGR_DEFINE_UMID(LTE, CPHY, CNF, FMD_SPEED_INFO, LTE_CPHY_FMD_SPEED_INFO_ID, lte_cphy_fmd_speed_info_cnf_s),
  #endif /* FEATURE_FMD_SPEED_INFO */  
};

extern lte_static_as_active_sys_type_e lte_static_rrc_dispatcher_get_active_sys_type
(
  void
);
#endif /* LTE_STATIC_AS_MSG_H */
