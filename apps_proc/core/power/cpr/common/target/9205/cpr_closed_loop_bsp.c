/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  9x45 Closed Loop BSP
  ===========================================================================*/

#include "cpr_closed_loop.h"
#include "CoreVerify.h"
#include <string.h>
#include "cpr_image.h"
#include "cpr_enablement.h"
#ifdef WINSIM
#define MSS_RBCPR_WRAPPER_REG_BASE 0
#else
#include "msmhwiobase.h"
#endif
#include "cpr_qfprom.h"
#include "HAL_cpr_hwio.h"


static const cpr_closed_loop_rail_config_t mss_9x45_closed_loop_config = 
{
    .rail_id =                  CPR_RAIL_APC0,
    .ref_clk_resource =         "clk_rbcpr_ref",
    .ahb_clk_resource =         "clk_bus_rbcpr",
    .hal_handle =               {APCS_RBCPR_WRAPPER_REG_BASE},
    .hal_clk_handle =           {APCS_GLB_REG_BASE},
    .aging_sensor_ids =         (const uint8[]){9},
    .aging_sensor_count =       1,   // 0 = Don't use aging
    .aging_ro_kv_x100 =         289,
    .aging_margin_limit =       13,  //12.5 
    .aging_voltage_mode =       CPR_VOLTAGE_MODE_TURBO,	
    .sensors_to_mask =          0,
    .sensors_to_mask_count =    0,
    .step_quot =                22, /* For ROSC0, step Quot is 22. */
    .interrupt_id =             52, /* Interrupt # mentioined in IPCAT is spi[20]. So, the
                                       actual interrupt # would be 52 (ie. 32 + 20) */
    .up_threshold =             2,
    .dn_threshold =             3,
    .consecutive_up =           0,
    .consecutive_dn =           2,
    .thermal_margin =         -13, //rounded 12.5 to 13
};


const cpr_closed_loop_config_t cpr_bsp_closed_loop_config =
{
    .closed_loop_config = (const cpr_closed_loop_rail_config_t*[])
    {
        &mss_9x45_closed_loop_config,
    },
    .closed_loop_config_count = 1,
};
