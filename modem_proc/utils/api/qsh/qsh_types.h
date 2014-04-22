/*!
  @file
  qsh_types.h

  @brief
  Contains all public types.
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/utils.mpss/2.3.4/api/qsh/qsh_types.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/21/14   mm      Added TRM client
07/14/14   mm      Changed client CFM -> CFCM
05/23/14   mm      Initial check-in
==============================================================================*/

#ifndef QSH_TYPES_H
#define QSH_TYPES_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <comdef.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief Enum used to identify the client name. 
*/
typedef enum
{
  QSH_CLT_A2,    /*!<  Advanced Accelerator */
  QSH_CLT_BAM,   /*!<  Bus Access Manager */
  QSH_CLT_IPA,   /*!<  IP Accelerator */
  QSH_CLT_PS,    /*!<  Protocol Services  */
  QSH_CLT_DS,    /*!<  Data Services */
  QSH_CLT_CFCM,  /*!<  Common Flow Control Management */
  QSH_CLT_MCPM,  /*!<  Modem Clock and Power Manager */
  QSH_CLT_DSM,   /*!<  Data Services Memory */
  QSH_CLT_HEAP,  /*!<  Heap memory */
  QSH_CLT_AP,    /*!<  Application processor */  
  QSH_CLT_HOST,  /*!<  Host PC */ 
  QSH_CLT_OTHER, /*!<  Other */
  QSH_CLT_ALL,   /*!<  All Client. DO NOT USE for logging */
  QSH_CLT_LFW,   /*!<  LTE Firmware */
  QSH_CLT_LML1,  /*!<  LTE ML1 */
  QSH_CLT_LMAC,  /*!<  LTE MAC */
  QSH_CLT_LRLC,  /*!<  LTE RLC */
  QSH_CLT_LPDCP, /*!<  LTE PDCP */
  QSH_CLT_LRRC,  /*!<  LTE RRC */
  QSH_CLT_WFW,   /*!<  WCDMA Firmware */
  QSH_CLT_WML1,  /*!<  WCDMA ML1 */
  QSH_CLT_WMAC,  /*!<  WCDMA MAC */
  QSH_CLT_WRLC,  /*!<  WCDMA RLC */
  QSH_CLT_WPDCP, /*!<  WCDMA PDCP */
  QSH_CLT_WRRC,  /*!<  WCDMA RRC */
  QSH_CLT_EMBMS, /*!<  EMBMS*/
  QSH_CLT_VOLTE, /*!<  VOLTE */
  QSH_CLT_IMS,   /*!<  IMS */
  QSH_CLT_TRM,   /*!<  TRM */
  QSH_CLT_MAX,   /*!<  DO NOT USE. Add new enums before this entry*/
} qsh_clt_e;

/*! @brief Enum to represent category index (client should not use this).
*/
typedef enum
{
  QSH_CAT_TPUT_IDX,
  QSH_CAT_CFG_IDX,
  QSH_CAT_DSM_IDX,
  QSH_CAT_PERF_IDX,
  QSH_CAT_OTHER_IDX,
  QSH_CAT_MAX_IDX
} qsh_cat_idx_e;

/*! @brief Enum used to identify the category.
*/
typedef enum
{
  /*! Throughput */
  QSH_CAT_TPUT    = 1 << QSH_CAT_TPUT_IDX,
  /*! Configuration */
  QSH_CAT_CFG     = 1 << QSH_CAT_CFG_IDX,
  /*! DSM */
  QSH_CAT_DSM     = 1 << QSH_CAT_DSM_IDX,
  /*! Performance */
  QSH_CAT_PERF    = 1 << QSH_CAT_PERF_IDX,
  /*! Other */
  QSH_CAT_OTHER   = 1 << QSH_CAT_OTHER_IDX,
  /*! DO NOT USE. Add new enums before this entry and update qsh_cat_idx_e */
  QSH_CAT_MAX     = 1 << QSH_CAT_MAX_IDX,
  /*! All categories. DO NOT USE for logging */
  QSH_CAT_ALL     = 0xFFFFFFFF
} qsh_cat_e;

/*! @brief Enum used to identify the message type. 
*/
typedef enum
{
  QSH_MSG_TYPE_HIGH,    /*!< High */
  QSH_MSG_TYPE_MEDIUM,  /*!< Medium */
  QSH_MSG_TYPE_INFO,    /*!< For information */
  QSH_MSG_TYPE_MAX      /*!< DO NOT USE */
} qsh_msg_type_e;

/*! @brief Enum used to identify the action. 
*/
typedef enum
{
  QSH_ACTION_ANALYSIS,  /*!<  Analyze */
  QSH_ACTION_RESET,     /*!<  Reset statistics */
} qsh_action_e;

/*! @brief Parameters for invoking QSH. 
*/
typedef struct
{ 
  /*! client */
  qsh_clt_e client;   
  /*! whether to run analysis, reset stats ... */   
  qsh_action_e action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed; 
} qsh_invoke_params_s;

/*! @brief Parameters passed into the call back functions 
    registered with QSH using the function qsh_reg_client_with_cb(...) . 
*/
typedef struct
{
  /*! whether to run analysis, reset stats ... */
  qsh_action_e action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. 
      The client should call qsh_analysis_async_done() when async analysis
      has completed. */
  boolean async_allowed; 
} qsh_cb_params_s;

/*! @brief Analysis functions for clients registered with 
    qsh_reg_client_with_cb(...). 
 
    Argument is qsh_cb_params_s. Check the definition of the struct above. 
    Returns true for CB running in sync mode and false for async mode.
*/
typedef boolean (*qsh_analysis_cb_type) (qsh_cb_params_s *);

#endif /* QSH_TYPES_H */