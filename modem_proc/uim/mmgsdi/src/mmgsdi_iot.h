#ifndef MMGSDI_IOT_H
#define MMGSDI_IOT_H
/*===========================================================================


           M M G S D I   I O T   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016, 2017, 2019 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/mmgsdi/src/mmgsdi_iot.h#1 $$ $DateTime: 2020/07/22 10:53:27 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/12/19   vm      Set byte 10 in EF-TST as per ATT REQ 13340 v18.3  
11/17/17   me      Support for FEATURE_UIM_JO_3_0_3_CAPABILITIES  
08/16/16   me      Initial version 
 
=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "mmgsdi_nv.h"

/*=============================================================================

                       STRUCTURE DECLARATIONS
=============================================================================*/

/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_IOT_FEATURE_ENUM_TYPE

   DESCRIPTION:
     This enum handles the IOT features 
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_IOT_NAS_SIGNALING_PRIORITY_SUPPORT,                                                                                
  MMGSDI_IOT_MINIMUM_PERIODIC_SEARCH_TIMER_SUPPORT,                     
  MMGSDI_IOT_EXTENDED_ACCESS_BARRING_SUPPORT,                       
  MMGSDI_IOT_TIMER_T3245_SUPPORT,                                 
  MMGSDI_IOT_OVERRIDE_NAS_SIGNALING_LOW_PRIORITY_SUPPORT,              
  MMGSDI_IOT_OVERRIDE_EXTENDED_ACCESS_BARRING_SUPPORT,             
  MMGSDI_IOT_SM_RETRY_WAIT_TIME_SUPPORT,                         
  MMGSDI_IOT_DELAY_TOLERANT_ACCESS_SUPPORT, 
  MMGSDI_IOT_POWER_SAVING_MODE_PSM_SUPPORT,                      
  MMGSDI_IOT_COVERAGE_ENHANCEMENT_MODE_A_SUPPORT,                         
  MMGSDI_IOT_COVERAGE_ENHANCEMENT_MODE_B_SUPPORT,                          
  MMGSDI_IOT_IDLE_MODE_EXTENDED_DISCONTINUOUS_RECEPTION_SUPPORT,                
  MMGSDI_IOT_CONNECTED_MODE_EXTENDED_DISCONTINUOUS_RECEPTION_SUPPORT,           
  MMGSDI_IOT_DEVICE_BASED_POSITIONING_SUPPORT,                     
  MMGSDI_IOT_SINGLE_CELL_POINT_TO_MULTIPOINT_TRANSMISSION_SUPPORT
} mmgsdi_iot_feature_enum;


/* ----------------------------------------------------------------------------
   ENUM:      MMGSDI_IOT_CATEGORY_ENUM_TYPE

   DESCRIPTION:
     This enum handles the IOT CAT 
-------------------------------------------------------------------------------*/
typedef enum {
  MMGSDI_IOT_CAT_M1                                                  = 0x01,
  MMGSDI_IOT_CAT_NB1                                                 = 0x02,  
  MMGSDI_IOT_CAT_M1_NB1                                              = 0x03, 
  MMGSDI_IOT_CAT1                                                    = 0x04,
  MMGSDI_IOT_CAT2                                                    = 0x05,
  MMGSDI_IOT_CAT3                                                    = 0x06,
  MMGSDI_IOT_CAT4                                                    = 0x07,
  MMGSDI_IOT_CAT5                                                    = 0x08,
  MMGSDI_IOT_CAT6                                                    = 0x09,
  MMGSDI_IOT_CAT7                                                    = 0x0A,
  MMGSDI_IOT_CAT8                                                    = 0x0B,
  MMGSDI_IOT_CAT9                                                    = 0x0C,                                   
  MMGSDI_IOT_CAT10                                                   = 0x0D,
  MMGSDI_IOT_CAT11                                                   = 0x0E,
  MMGSDI_IOT_CAT12                                                   = 0x0F,
  MMGSDI_IOT_CAT6_CAT4                                               = 0x10,                                  
  MMGSDI_IOT_CAT6_CAT5                                               = 0x11,                                                                          
  MMGSDI_IOT_CAT7_CAT4                                               = 0x12,                                 
  MMGSDI_IOT_CAT7_CAT13                                              = 0x13,                                   
  MMGSDI_IOT_CAT9_CAT4                                               = 0x14,                                                    
  MMGSDI_IOT_CAT9_CAT5                                               = 0x15,                                   
  MMGSDI_IOT_CAT9_CAT6                                               = 0x16,                                   
  MMGSDI_IOT_CAT10_CAT4                                              = 0x17,                                  
  MMGSDI_IOT_CAT10_CAT7                                              = 0x18,                                   
  MMGSDI_IOT_CAT10_CAT13                                             = 0x19,                                                                         
  MMGSDI_IOT_CAT11_CAT4                                              = 0x1A,                                  
  MMGSDI_IOT_CAT11_CAT5                                              = 0x1B,                                   
  MMGSDI_IOT_CAT11_CAT6                                              = 0x1C,                                  
  MMGSDI_IOT_CAT11_CAT9                                              = 0x1D,                                 
  MMGSDI_IOT_CAT12_CAT4                                              = 0x1E,                                    
  MMGSDI_IOT_CAT12_CAT7                                              = 0x1F,                                   
  MMGSDI_IOT_CAT12_CAT10                                             = 0x20,                                
  MMGSDI_IOT_CAT12_CAT13                                             = 0x21,                                  
  MMGSDI_IOT_CAT13_CAT3                                              = 0x22,                                   
  MMGSDI_IOT_CAT13_CAT4                                              = 0x23,                                   
  MMGSDI_IOT_CAT13_CAT5                                              = 0x24,                                   
  MMGSDI_IOT_CAT13_CAT6                                              = 0x25,                                
  MMGSDI_IOT_CAT13_CAT7                                              = 0x26,                           
  MMGSDI_IOT_CAT13_CAT13                                             = 0x27,                              
  MMGSDI_IOT_CAT14_CAT5                                              = 0x28,                               
  MMGSDI_IOT_CAT14_CAT8                                              = 0x29,  
  /* RFU range: 0x2A - 0xFE */
  MMGSDI_IOT_UNKNOWN                                                 = 0xFF                                  
} mmgsdi_iot_category_enum;


/*===========================================================================
  FUNCTION MMGSDI_IS_IOT_CAPABILITY_SUPPORTED()

  DESCRIPTION
    Returns the status enabled or disabled

  PARAMETERS
    feature

  RETURN VALUE
    mmgsdi_feature_status_enum_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
mmgsdi_feature_status_enum_type mmgsdi_is_iot_capability_supported(
  mmgsdi_iot_feature_enum  feature 
);

/* ============================================================================
FUNCTION MMGSDI_IS_IOT_DEVICE

DESCRIPTION
  Return the if the device support MTC or not 

DEPENDENCIES
  None

LIMITATIONS
  none.

RETURN VALUE
  MTC:   TRUE OR FALSE

SIDE EFFECTS
  None

============================================================================*/
boolean mmgsdi_is_iot_device(
  void
);

/* ============================================================================
FUNCTION MMGSDI_GET_IOT_CATEGORY

DESCRIPTION
  Return the category 

DEPENDENCIES
  None

LIMITATIONS
  none.

RETURN VALUE
  CATEGORY:   category 

SIDE EFFECTS
  None

============================================================================*/
mmgsdi_iot_category_enum mmgsdi_get_iot_category(
  void
);

#endif /* MMGSDI_IOT_H */


