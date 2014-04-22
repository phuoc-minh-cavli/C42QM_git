#ifndef _DETECTION_ENGINE_H_
#define _DETECTION_ENGINE_H_

/*==============================================================================
  @file detection_engine_type.h
  @brief This file contains Public APIs for Modules using Detection Engine type
  of features

  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  Export of this technology or software is regulated by the U.S. Government,
  Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
  Edit History

  $Header: //components/rel/avs.adsp/2.8.6.c2/api/listen/inc/detection_engine.h#1 $

  when       who        what, where, why
  --------   ---        ------------------------------------------------------
  07/06/18   saxena     SVA 3.0 support addition
  04/20/17   sw         (Tech Pubs) Edited descriptions
  03/20/17   unni       Created File
==============================================================================*/

/** @h2xml_title1     {Detection Engine Interface}
    @h2xml_title_date {April 7, 2017} */

#include "mmdefs.h"

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Parameter id */
#define LSM_PARAM_ID_REGISTER_SOUND_MODEL     0x00012C14
/* Remapping */
#define PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER \
        LSM_PARAM_ID_REGISTER_SOUND_MODEL

/*==============================================================================
   Type definitions
==============================================================================*/

/** @h2xmlp_emptyParameter {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER",
                            PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER}
    @h2xmlp_description    {Registers a sound model with a module. \n
           \n
           The sound model is a blob of data interpreted by the module that
           supports this parameter. The model contains unique sound
           characteristics and signatures that are used by the algorithm hosted
           in the module. \n
           \n
           This parameter must be loaded into aDSP memory using
           GRAPHITE_CMD_LOAD_DATA. Subsequent graphite commands use this
           parameter via GRAPHITE_PARAM_ID_INDIRECT_CALIB_DATA. When data is no
           longer required, it is freed using GRAPHITE_CMD_UNLOAD_DATA.
           While the module is active, this data can be freed only after
           PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER is invoked. \n
           \n
           All calibration parameters start from a 4-byte aligned address.
           This parameter for this module has an additional 8-byte alignment
           requirement because it performs vector processing directly on the
           data. \n
           \n
           GRAPHITE_CMD_LOAD_DATA guarantees 16-byte alignment for the payload
           (immediately after the CMI header).}
    @h2xmlp_toolPolicy {NO_SUPPORT} */

/*==============================================================================
   Constants
==============================================================================*/

/* Unique Parameter id */
#define LSM_PARAM_ID_DEREGISTER_SOUND_MODEL     0x00012C15
/* Remapping */
#define PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER \
        LSM_PARAM_ID_DEREGISTER_SOUND_MODEL

/*==============================================================================
   Type definitions
==============================================================================*/

/** @h2xmlp_emptyParameter {"PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER",
                            PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_DEREGISTER}
    @h2xmlp_description    {Deregisters a sound model from a module. \n
                            \n
                            After this parameter is invoked, the memory
                            allocated for the indirect calibration
                            (PARAM_ID_DETECTION_ENGINE_SOUND_MODEL_REGISTER)
                            can be freed.}
    @h2xmlp_toolPolicy     {NO_SUPPORT} */

/* Unique Parameter id */
#define LSM_PARAM_ID_DETECTION_ENGINE_EVENT_TYPE             0x00012c2c

/* Minor version of this PARAM */
#define API_VERSION_DETECTION_ENGINE_EVENT_TYPE          0x1

/*==============================================================================
   Type definitions
==============================================================================*/

/* Structure definition for Parameter */
typedef struct lsm_param_id_detection_engine_event_type lsm_param_id_detection_engine_event_type_t;

/** @h2xmlp_parameter   {"LSM_PARAM_ID_DETECTION_ENGINE_EVENT_TYPE", LSM_PARAM_ID_DETECTION_ENGINE_EVENT_TYPE}
    @h2xmlp_description {The type of Detection event raised by the Detection Engine module}
    @h2xmlp_version     {API_VERSION_DETECTION_ENGINE_EVENT_TYPE} 
  @h2xmlp_toolPolicy  {NO_SUPPORT}*/
#include "adsp_begin_pack.h"
struct lsm_param_id_detection_engine_event_type
{
  uint32_t minor_version;
  /**< @h2xmle_description {Tracks the current version of this parameter.}
       @h2xmle_range       {1..API_VERSION_DETECTION_ENGINE_EVENT_TYPE}
       @h2xmle_default     {API_VERSION_DETECTION_ENGINE_EVENT_TYPE}
       @h2xmle_readonly    {true}
       @h2xmle_isVersion   {true} */

  uint32_t event_type;
  /**< @h2xmle_description {The type of Detection event raised by the Detection Engine module}
                            0 = LSM_SESSION_EVENT_DETECTION_STATUS_V2 to be sent;\n 
                            1 = LSM_SESSION_DETECTION_ENGINE_GENERIC_EVENT to be sent.\n
                            ACD module always sends LSM_SESSION_DETECTION_ENGINE_GENERIC_EVENT,so
                            only accepts value one \n
                           }
     @h2xmle_range         {0..1}
       @h2xmle_default     {0} */

  uint32_t mode;
  /**< @h2xmle_description {Bit field to indicate which type of information need to 
                           embed in event.Set this field only if event_type variable is 1}

       @h2xmle_bitField    {0x00000001}
       @h2xmle_bitName     {confidence_level_information}
       @h2xmle_description {to include confidence level information in detection event.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_bitFieldEnd

       @h2xmle_bitField    {0x00000002}
       @h2xmle_bitName     {keyword_indices_information}
       @h2xmle_description {to include keyword indices information in detection event from SVA \n
                          to include time stamp information in detection event from ACD.
                         }
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_bitFieldEnd     

       @h2xmle_bitField    {0x00000003}
       @h2xmle_bitName     {time_stamp_information}
       @h2xmle_description {to include time stamp information in detection event from SVA.}
       @h2xmle_rangeList   {Disabled=0;Enabled=1}
       @h2xmle_bitFieldEnd       

     @h2xmle_default      {1}*/

}
#include "adsp_end_pack.h"
;
#endif /* _DETECTION_ENGINE_H_ */
