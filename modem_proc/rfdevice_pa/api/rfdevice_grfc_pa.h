
#ifndef RFDEVICE_GRFC_PA_H
#define RFDEVICE_GRFC_PA_H
/*!
   @file
   rfdevice_grfc_pa.h

   @brief
   This file contains definition & prototypes for GRFC generic 3rd party pa devices

*/
/*==============================================================================

  Copyright (c) 2018 by Qualcomm Technologies, Inc.  All Rights Reserved.

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rfdevice_pa.mpss/1.30/api/rfdevice_grfc_pa.h#1 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/29/18   vp      Initial Revision

==============================================================================*/

#ifdef __cplusplus

#include "rfc_common.h"
#include "rfdevice_pa.h"
#include "rf_buffer_intf.h"
#include "rf_rffe_common.h"
#include "rfdevice_physical_device.h"
#include "rfdevice_physical_third_party.h"

/*These are shared between the two macros*/                                                             
#define RFDEVICE_GRFC_PA_VALID_FLAG_BITSHIFT       0       // Shift 0 bits : Bit 0,1
#define RFDEVICE_GRFC_PA_VALID_FLAG_BITMASK        0x3     // 2 Bits

#define RFDEVICE_GRFC_PA_GRFC_TYPE_BITSHIFT        2       // Shift 2 bits : Bit 2,3
#define RFDEVICE_GRFC_PA_GRFC_TYPE_BITMASK         0x3     // 2 Bits
                                                              
#define RFDEVICE_GRFC_PA_SIG_ID_BITSHIFT           4       // Shift 4 bits : Bits 4-13
#define RFDEVICE_GRFC_PA_SIG_ID_BITMASK            0x3FF   // 10 bits

#define RFDEVICE_GRFC_PA_MACRO_SELECT_BITSHIFT     30      // Shift 30 bits : Bit 30,31
#define RFDEVICE_GRFC_PA_MACRO_SELECT_BITMASK      0x3     // 2 bits                                                
             
/*These are only used for RFDEVICE_GRFC_PA_GAIN_CONFIG_LOGIC*/                                   
#define RFDEVICE_GRFC_PA_GAIN0_BITSHIFT            28      // Shift 28 bits : Bit 28,29
#define RFDEVICE_GRFC_PA_GAIN1_BITSHIFT            26      // Shift 26 bits : Bit 26,27
#define RFDEVICE_GRFC_PA_GAIN2_BITSHIFT            24      // Shift 24 bits : Bit 24,25
#define RFDEVICE_GRFC_PA_GAIN3_BITSHIFT            22      // Shift 22 bits : Bit 22,23
#define RFDEVICE_GRFC_PA_GAIN4_BITSHIFT            20      // Shift 20 bits : Bit 20,21
#define RFDEVICE_GRFC_PA_GAIN5_BITSHIFT            18      // Shift 20 bits : Bit 18,19
#define RFDEVICE_GRFC_PA_GAIN6_BITSHIFT            16      // Shift 20 bits : Bit 16,17
#define RFDEVICE_GRFC_PA_GAIN7_BITSHIFT            14      // Shift 20 bits : Bit 14,15
#define RFDEVICE_GRFC_PA_GAIN_BITMASK              0x3     // 2 bits
#define RFDEVICE_BITS_PER_GRFC_PA_GAIN_STATE       2       // 2 bits per gain state
     
/*These are only used for RFDEVICE_GRFC_PA_ON_CONFIG_LOGIC*/
#define RFDEVICE_GRFC_PA_ON_BITSHIFT               16      // Shift 16 bits : Bit 16,17
#define RFDEVICE_GRFC_PA_OFF_BITSHIFT              14      // Shift 16 bits : Bit 14,15
#define RFDEVICE_GRFC_PA_ON_OFF_BITMASK            0x3     // 2 bits
                                                    

#define RFDEVICE_GRFC_PA_VALID_FLAG_GET(x)        ((x>>RFDEVICE_GRFC_PA_VALID_FLAG_BITSHIFT)    & RFDEVICE_GRFC_PA_VALID_FLAG_BITMASK)
#define RFDEVICE_GRFC_PA_GRFC_TYPE_GET(x)         ((x>>RFDEVICE_GRFC_PA_GRFC_TYPE_BITSHIFT)     & RFDEVICE_GRFC_PA_GRFC_TYPE_BITMASK)
#define RFDEVICE_GRFC_PA_GRFC_NUMBER_GET(x)       ((x>>RFDEVICE_GRFC_PA_SIG_ID_BITSHIFT)   & RFDEVICE_GRFC_PA_SIG_ID_BITMASK)
#define RFDEVICE_GRFC_PA_MACRO_SELECT_GET(x)      ((x>>RFDEVICE_GRFC_PA_MACRO_SELECT_BITSHIFT)  & RFDEVICE_GRFC_PA_MACRO_SELECT_BITMASK)
#define RFDEVICE_GRFC_PA_ON_GET(x)      ((x>>RFDEVICE_GRFC_PA_ON_BITSHIFT)  & RFDEVICE_GRFC_PA_ON_OFF_BITMASK)
#define RFDEVICE_GRFC_PA_OFF_GET(x)      ((x>>RFDEVICE_GRFC_PA_OFF_BITSHIFT)  & RFDEVICE_GRFC_PA_ON_OFF_BITMASK)
#define RFDEVICE_GRFC_PA_GAIN_STATE_GET(x, y)   ((x>>y) & RFDEVICE_GRFC_PA_GAIN_BITMASK)

#define RFDEVICE_GRFC_PA_ON_CONFIG_LOGIC(grfc_type, sig_name, pa_on, pa_off)                                  \
         (( (TRUE & RFDEVICE_GRFC_PA_VALID_FLAG_BITMASK)      << RFDEVICE_GRFC_PA_VALID_FLAG_BITSHIFT )     | \
         ( (grfc_type & RFDEVICE_GRFC_PA_GRFC_TYPE_BITMASK)  << RFDEVICE_GRFC_PA_GRFC_TYPE_BITSHIFT )       | \
         ( (sig_name & RFDEVICE_GRFC_PA_SIG_ID_BITMASK) << RFDEVICE_GRFC_PA_SIG_ID_BITSHIFT )               | \
         ( (pa_on & RFDEVICE_GRFC_PA_ON_OFF_BITMASK) << RFDEVICE_GRFC_PA_ON_BITSHIFT )                      | \
         ( (pa_off & RFDEVICE_GRFC_PA_ON_OFF_BITMASK) << RFDEVICE_GRFC_PA_OFF_BITSHIFT )                    | \
         ( (TRUE & RFDEVICE_GRFC_PA_MACRO_SELECT_BITMASK) << RFDEVICE_GRFC_PA_MACRO_SELECT_BITSHIFT ) )

#define RFDEVICE_GRFC_PA_GAIN_CONFIG_LOGIC(grfc_type, sig_name, G0,G1,G2,G3,G4,G5,G6,G7)                      \
         (( (TRUE & RFDEVICE_GRFC_PA_VALID_FLAG_BITMASK)      << RFDEVICE_GRFC_PA_VALID_FLAG_BITSHIFT )     | \
         ( (grfc_type & RFDEVICE_GRFC_PA_GRFC_TYPE_BITMASK)  << RFDEVICE_GRFC_PA_GRFC_TYPE_BITSHIFT )       | \
         ( (sig_name & RFDEVICE_GRFC_PA_SIG_ID_BITMASK) << RFDEVICE_GRFC_PA_SIG_ID_BITSHIFT )               | \
         ( (G0 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN0_BITSHIFT )                       | \
         ( (G1 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN1_BITSHIFT )                       | \
         ( (G2 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN2_BITSHIFT )                       | \
         ( (G3 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN3_BITSHIFT )                       | \
         ( (G4 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN4_BITSHIFT )                       | \
         ( (G5 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN5_BITSHIFT )                       | \
         ( (G6 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN6_BITSHIFT )                       | \
         ( (G7 & RFDEVICE_GRFC_PA_GAIN_BITMASK ) << RFDEVICE_GRFC_PA_GAIN7_BITSHIFT )                       | \
         ( (FALSE & RFDEVICE_GRFC_PA_MACRO_SELECT_BITMASK) << RFDEVICE_GRFC_PA_MACRO_SELECT_BITSHIFT ) )

#define RFDEVICE_GRFC_PA_MAX_NUM_GRFCS 4 //1 for grfc on_off, 3 for gain control 
#define RFDEVICE_MAX_PA_RX_GAIN_STATES 8

/*To capture the PA gain settings per GRFC, supports both macros*/
typedef struct
{
  uint8 grfc_num;
  uint8 gain_states[RFDEVICE_MAX_PA_RX_GAIN_STATES];
  uint8 valid_flag : 1;
  uint8 grfc_type : 2;
  uint8 pa_on_logic : 2;
  uint8 pa_off_logic : 2;
  uint8 macro_select : 1;
}rfdevice_grfc_pa_rfc_info_type;

/*To capture the GRFC settings per PA*/
typedef struct
{
  uint8 num_grfc;
  rfdevice_grfc_pa_rfc_info_type rfc_grfc_pa_data[RFDEVICE_GRFC_PA_MAX_NUM_GRFCS];
}rfdevice_grfc_pa_rfc_tbl_type;

/*To capture the PA GRFC info for all techs/band/sig_path from RFC*/
typedef struct
{
  rfdevice_grfc_pa_rfc_tbl_type gsm[RFCOM_NUM_GSM_BANDS];
  rfdevice_grfc_pa_rfc_tbl_type tdscdma[RFCOM_NUM_TDSCDMA_BANDS];
  rfdevice_grfc_pa_rfc_tbl_type cdma[RFM_CDMA_MAX_BAND];
  rfdevice_grfc_pa_rfc_tbl_type wcdma[RFCOM_NUM_WCDMA_BANDS];
  rfdevice_grfc_pa_rfc_tbl_type lte[RFCOM_NUM_LTE_BANDS];
} rfdevice_grfc_pa_rfc_tbl_band_map_type;

class rfdevice_grfc_pa:public rfdevice_pa
{
public:
  /*! Constructor overloaded for physical device support */
  rfdevice_grfc_pa(rfdevice_physical_device *phy_dev_obj_ptr,
                rfc_phy_device_info_type *phy_device_info,
                rfc_logical_device_info_type *logical_device_info);

  virtual ~rfdevice_grfc_pa();

  boolean config(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf *buff_obj_ptr,
                rf_device_execution_type dev_action,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean set_pa_txagc (rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf *script_buffer,
                rf_device_execution_type execution_type,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean set_pa_on_off(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf* script_buffer,
                rf_device_execution_type  execution_type,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean set_pa_trigger(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf *buff_obj_ptr,
                rf_device_execution_type dev_action,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean sleep(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf* script_buffer,
                rf_device_execution_type  execution_type,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  boolean wakeup(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf* script_buffer,
                rf_device_execution_type  execution_type,
                int16 script_timing = RFCOM_INVALID_TIMING_OFFSET);

  uint16 get_script_size(void);
 
  boolean set_band_map(rfcom_mode_enum_type mode,
                rfcom_band_type_u band,
                int32* data_ptr,
                uint8 size);

  boolean init_status;
  
protected:
  rfc_device_cfg_info_type grfc_pa_device_info;
  rfdevice_grfc_pa_rfc_tbl_band_map_type band_map_grfc_tbl;
  boolean set_pa_on_flag; 
  boolean set_pa_gain_flag;
  uint16 pa_on_off; 
  uint16 pa_gain;

private:
  boolean get_band_map_tbl(rfcom_mode_enum_type mode,
                rfcom_band_type_u band,
                rfdevice_grfc_pa_rfc_tbl_type **rfc_grfc_tbl_ptr);
  
  boolean set_grfc(rfdevice_pa_cfg* pa_params_p,
                rf_buffer_intf* script_buffer,
                rf_device_execution_type execution_type,
                int16 script_timing,
                rfdevice_grfc_pa_rfc_tbl_type* rfc_grfc_tbl_ptr);

};

#endif  /* #ifdef __cplusplus */

#endif /*RFDEVICE_GRFC_PA_H*/

