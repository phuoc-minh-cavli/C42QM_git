

#include "ddr_common.h"
#include "HALhwio.h"
#include "DDR_PHY.h"
#include "ddr_target.h"
#include "ddr_hwio.h"
#include "ddr_config.h"
#include "HAL_DDR.h"
#include "DDR_PHY.h"
#include "AHB2PHY.h"
#include "BIMC.h"
void HAL_SDRAM_BIMC_CA_Training_Pattern_Toggle(uint32 reg_base, SDRAM_INTERFACE interface);
void HAL_SDRAM_BIMC_CA_Training_Exit(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);
void HAL_SDRAM_BIMC_CA_Training_Pattern_Set(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select, uint8 mapping, uint32
    pre_rising_pattern, uint32 pre_falling_pattern, uint32 rising_pattern, uint32 falling_pattern, uint32 post_rising_pattern,
    uint32 post_falling_pattern);
void BIMC_Enable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);
void BIMC_Disable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);
