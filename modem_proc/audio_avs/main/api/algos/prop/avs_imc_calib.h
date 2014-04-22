#ifndef AVS_IMC_CALIB_H
#define AVS_IMC_CALIB_H

/*==============================================================================
  @file avs_imc_calib.h
  @brief This file contains IMC parameters
==============================================================================*/

/*=======================================================================
* Copyright (c) 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
* Confidential and Proprietary – Qualcomm Technologies, Inc.
=========================================================================*/
/*========================================================================
 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 05/17/18   sabdagir  Added h2xml comments.
 ========================================================================== */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/

/* Param id to configure inter module communication between two modules */
#define AVS_PARAM_ID_IMC 0x0001323D


/** @h2xmlp_parameter    {"PARAM_ID_AVS_IMC", AVS_PARAM_ID_IMC}
    @h2xmlp_description  { Param id to set inter moduel communication between two modules\n} 
    @h2xmlp_description  {To enable/disable RTM logging.}
    @h2xmlp_toolPolicy   {Calibration}    */


/*  Payload of the AVS_PARAM_ID_IMC parameter */
typedef struct avs_imc_payload_t
{
   uint32_t comm_dir;
    /**< @h2xmle_description {Communication direction. 0 - TRANSMIT, 1 - RECEIVE } 
         @h2xmle_default     {0} 
         @h2xmle_rangeList   {"TRANSMIT"= 0;
                              "RECEIVE"=1}
         @h2xmle_policy      {Advanced} */

   uint32_t enable;
    /**< @h2xmle_description {Enables IMC } 
         @h2xmle_default     {0} 
         @h2xmle_range       {0..1} 
         @h2xmle_policy      {Advanced} */

   uint32_t purpose;
    /**< @h2xmle_description {purpose of the communication. specific for one communication type } 
         @h2xmle_default     {0} 
         @h2xmle_range       {0..0xFFFFFFFF} 
         @h2xmle_policy      {Advanced} */

   uint32_t comm_instance;
    /**< @h2xmle_description {For a given purpose, specifies instance of the communication } 
         @h2xmle_default     {0} 
         @h2xmle_range       {0..0xFFFFFFFF} 
         @h2xmle_policy      {Advanced} */

}avs_imc_payload_t;

/* Purspose ID for FFV - SVA IMC to inform beam former freeze */
#define AVS_PURPOSE_ID_SVA_FFV_FREEZE_BEAMFORMER 0x00010965

#endif  // AVS_IMC_CALIB_H


