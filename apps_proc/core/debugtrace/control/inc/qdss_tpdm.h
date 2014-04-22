#ifndef QDSS_TPDM_H
#define QDSS_TPDM_H

/*=============================================================================

FILE:         qdss_tpdm.h

DESCRIPTION:  TPDM interface functions

================================================================================
            Copyright (c) 2016, 2019 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/
#include "DALStdDef.h"

#define WCSS_TPDM   0


/*-------------------------------------------------------------------------*/

/**
  @brief    Enable or disable a TPDM. 
            When a TPDM is enabled the trace path for it including
            the correpsonding TPDA port and funnel ports to trace sink
            are enabled.
  
  @param              tpdm_id [in ] :  TPDM id
  @param                state [in ] :  1 to enable 0 to disable

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_set(uint8 tpdm_id,uint8 state);

#define   TPDM_IMPLDEF 0x1
#define   TPDM_DSB     0x2
#define   TPDM_CMB     0x4
#define   TPDM_TC      0x8
#define   TPDM_BC     0x10
#define   TPDM_GPR    0x20
#define   TPDM_MCMB   0x40



/*-------------------------------------------------------------------------*/

/**
  @brief     Enable or disable datasets on a TPDM. 

  
  @param              tpdm_id [in ] :  TPDM id
  @param          enable_mask [in ] :  Bitmask of datasets to enable.
                                       See above for bitmask values.
                                       A combination can be specified
  @param                state [in ] :  1 to enable 0 to disable

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_dataset_set(uint8 tpdm_id,uint8 enable_mask,uint8 state);


/*-------------------------------------------------------------------------*/

/**
  @brief    Get a GPR register value
  
  @param              tpdm_id [in ] :  TPDM id
  @param              reg_num [in ] :  register number
  @param              reg_type[in ] :  register type
  @param                 pval [out ]:  Pointer to value 

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_get_gpr(uint8 tpdm_id, uint8 reg_num, uint32 reg_type,uint32 *pval);

/*-------------------------------------------------------------------------*/

/**
  @brief   Set a GPR value
  
  @param              tpdm_id [in ] :  TPDM id
  @param              reg_num [in ] :  register number
  @param              reg_type[in ] :  register type
  @param                  val [in ] :  value to be set

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_set_gpr(uint8 tpdm_id, uint8 reg_num, uint32 reg_type, uint32 val);


/*-------------------------------------------------------------------------*/

/**
  @brief  Set DSB edge detection control mask. This function can be used
          to enable any range (within 0-256)  of DSB events. 
  
  @param              tpdm_id [in ] :  TPDM id
  @param                start [in ] :  starting DSB event number
  @param                  end [in ] :  ending DSB event number
  @param                state [in ] :  enable or disable

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_dsb_set_edcmr(uint8 tpdm_id,uint8 start, uint8 end, uint8 state);


#define   TPDM_CONTROL_DSB_MODE                    1
#define   TPDM_CONTROL_DSB_TS_PATT_MATCH_ON_OFF    2
#define    TPDM_CONTROL_DSB_TS_EXT_TRIGGER_ON_OFF  3  

/*-------------------------------------------------------------------------*/

/**
  @brief   Retrieve a DSB config parameter.
  
  @param              tpdm_id [in ] :  TPDM id
  @param             param_id [in ] :  Parameter id. See above for supported
                                       options
  @param            param_val [out] :  Pointer to paramter value

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_dsb_get_param(uint8 tpdm_id, uint8 param_id, uint32 *param_val);

/*-------------------------------------------------------------------------*/

/**
  @brief  Set a DSB config parameter
  
  @param              tpdm_id [in ] :  TPDM id
  @param             param_id [in ] :  Parmeter id
  @param            param_val [in ] :  Set parameter value

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_dsb_set_param(uint8 tpdm_id, uint8 param_id, uint32 param_val);

#define   TPDM_CONTROL_CMB_MODE                   1
#define   TPDM_CONTROL_CMB_FLOWCTRL_ON_OFF        2
#define   TPDM_CONTROL_CMB_TS_PATT_MATCH_ON_OFF   3
#define   TPDM_CONTROL_CMB_TS_EXT_TRIGGER_ON_OFF  4
#define   TPDM_CONTROL_CMB_TS_PATT_VAL_LSW        5
#define   TPDM_CONTROL_CMB_TS_PATT_VAL_MSW        6  
#define   TPDM_CONTROL_CMB_TS_PATT_MASK_LSW       7
#define   TPDM_CONTROL_CMB_TS_PATT_MASK_MSW       8
#define   TPDM_CONTROL_MCMB_LANES_SELECT          9
#define   TPDM_CONTROL_MCMB_CA_MODE               10
#define   TPDM_CONTROL_CMB_TS_ALL                 11
#define   TPDM_CONTROL_MCMB_CA_XTRIG_LNSEL        12


/*-------------------------------------------------------------------------*/

/**
  @brief  Retrieve a CMB paramter
  
  @param              tpdm_id [in ] :  TPDM id
  @param             param_id [in ] :  Paramater id. See above for CMB paramter
                                       ids.
  @param            param_val [out] :  Pointer to parameter value

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_cmb_get_param(uint8 tpdm_id, uint8 param_id, uint32 *param_val);

/*-------------------------------------------------------------------------*/

/**
  @brief 
  
  @param              tpdm_id [in ] :  TPDM id
  @param             param_id [in ] :  Paramter id
  @param            param_val [in ] :  Parameter value

  @return 0 if successful, error code otherwise
 */
int qdss_tpdm_cmb_set_param(uint8 tpdm_id, uint8 param_id, uint32 param_val);



#endif //QDSS_TPDM_H
