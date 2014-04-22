/*
* Copyright (c) 2015 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
@file: adsppm_configdata_8998.c
@brief: Contains all 8998-specific data definitions for Config Manager.

$Header: //components/rel/core.mpss/3.10/power/adsppm/config/9205/adsppm_configdata_9205.c#3 $
*/

#include "adsppm.h"
#include "asic.h"
#include "asic_internal.h"
#include "adsppm_utils.h"
#include "icbid.h"
#include "DDIIPCInt.h"
#include "ultaudio_hwio_9205.h"
#include "msmhwiobase.h"
#include "limits.h"

AsicHwioRegRangeType lpassRegRange = {ULTAUDIO_CORE_BASE, ULTAUDIO_CORE_BASE_PHYS, ULTAUDIO_CORE_BASE_SIZE};

/**
 * Array of 9607 Master Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType masterPorts_ADSP[]		= {AdsppmBusPort_Adsp_Master};
const AdsppmBusPortIdType masterPorts_AIF[]		= {AdsppmBusPort_Aif_Master};

/**
 * Array of 9607 Slave Bus Ports per core. Arranged by to core ID
 */
const AdsppmBusPortIdType slavePorts_LPM[]			= {AdsppmBusPort_Lpm_Slave};
const AdsppmBusPortIdType slavePorts_AIF[]			= {AdsppmBusPort_Aif_Slave};
const AdsppmBusPortIdType slavePorts_AVsync[]		= {AdsppmBusPort_AvSync_Slave};
const AdsppmBusPortIdType slavePorts_BSTC[]		= {AdsppmBusPort_Bstc_Slave};

/**
 * Array of 9607 core descriptors. Arranged according to core ID enum
 * TODO:sn-> check what needs to be populated in instance id field
 * 			& Core Clock Id field as well.
 */
AsicCoreDescType cores_array[Adsppm_Core_Id_Max] = {
        
        {   //Adsppm_Core_Id_ADSP
                Adsppm_Core_Id_ADSP, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_ADSP, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_Adsp, // Power Domain
				{0, NULL}, //Core clock instances
				{ADSPPM_ARRAY(masterPorts_ADSP)}, //Master port instances
                {0, NULL}, //Slave port instances
                Adsppm_Instance_Id_0
        },
        
        {   //Adsppm_Core_Id_LPM
                Adsppm_Core_Id_LPM, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Mem, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
				{0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances   
				{ADSPPM_ARRAY(slavePorts_LPM)}, //Slave port instances
                Adsppm_Instance_Id_0
        },
        
        {   //Adsppm_Core_Id_AIF
                Adsppm_Core_Id_AIF, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                {0, NULL}, //Core clock instances
                {ADSPPM_ARRAY(masterPorts_AIF)}, //Master port instances
                {ADSPPM_ARRAY(slavePorts_AIF)}, //Slave port instances
                Adsppm_Instance_Id_0
        },
        
        {   //Adsppm_Core_Id_AVsync
                Adsppm_Core_Id_AVsync, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
				{ADSPPM_ARRAY(slavePorts_AVsync)}, //Slave port instances
                Adsppm_Instance_Id_0
        },
        
        {   //Adsppm_Core_Id_DCodec
        		Adsppm_Core_Id_DCodec, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_None, //Power
                        AsicRsc_None, //Core Clock
                        AsicRsc_None, //Sleep Latency
                        AsicRsc_None, //MIPS
                        AsicRsc_None, //BW
                        AsicRsc_None //Thermal
                }, // HwResource IDs
                AsicPowerDomain_AON, // Power Domain
                {0, NULL}, //Core clock instances
                {0, NULL}, //Master port instances
   		{0, NULL}, //Slave port instances
                Adsppm_Instance_Id_None
        },
        
        {   //Adsppm_Core_Id_BSTC
                Adsppm_Core_Id_BSTC, // Core ID
                {
                        AsicRsc_None, //None
                        AsicRsc_Power_Core, //Power
                        AsicRsc_Core_Clk, //Core Clock
                        AsicRsc_Latency, //Sleep Latency
                        AsicRsc_MIPS_Clk | AsicRsc_MIPS_BW, //MIPS
                        AsicRsc_BW_Internal | AsicRsc_BW_External, //BW
                        AsicRsc_Thermal //Thermal
                }, // HwResource IDs
                AsicPowerDomain_LpassCore, // Power Domain
                {0, NULL}, //Core clock instances
				{0, NULL}, //Master port instances
				{ADSPPM_ARRAY(slavePorts_BSTC)}, //Slave port instances
                Adsppm_Instance_Id_0
        },
		

};

AsicCoreDescriptorArrayType cores = {ADSPPM_ARRAY(cores_array)};

AsicMemDescriptorType memories_array[Adsppm_Mem_Max] = {
        {   //Adsppm_Mem_None
                Adsppm_Mem_None, // Mem ID
                AsicPowerDomain_AON, // Power Domain
        },
        {   //Adsppm_Mem_Lpass_LPM
                Adsppm_Mem_Lpass_LPM, // Core ID
                AsicPowerDomain_Lpm, // Power Domain
        }
};

AsicMemDescriptorArrayType memories = {ADSPPM_ARRAY(memories_array)};

/**
 * Array of 9607 core clock descriptors. Arranged by core clock ID
 */
AsicClkDescriptorType clocks_array[] = {
        {
                AdsppmClk_None, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlNone, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Adsp_Core, //Clk ID
                AsicClk_TypeNpa, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "/clk/cpu",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Ahb_Root, //Clk ID
                AsicClk_TypeDalFreqSet, //Clk Type
                AsicClk_CntlSW_SHARED, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_ahbfabric_ixfabric_clk",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
		{
				AdsppmClk_AhbI_Hclk, //Clk ID
				AsicClk_TypeNone, //Clk Type
				AsicClk_CntlNone, // Cntl Type
				AsicClk_MemCntlNone, // MemCtrl Type
				.clkInfo.clkName = "",//Name
				AdsppmClk_None, //Source
				Adsppm_Mem_None //Memory ID
		},
		{
				AdsppmClk_AhbX_Hclk, //Clk ID
				AsicClk_TypeInternalCGC, //Clk Type
				AsicClk_CntlHW, // Cntl Type. TODO: Fall back to AsicClk_CntlAlwaysON if HW control doesn't work
				AsicClk_MemCntlNone, // MemCtrl Type
				.clkInfo.hwioInfo = {
						HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_OFFS, //CGCR Offset
						HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_CLK_ENABLE_BMSK, //Enable mask
						HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_MST_CGCR_HW_CTL_BMSK, //HW control mask
						0	//Status mask
				},
				AdsppmClk_None, //Source
				Adsppm_Mem_None //Memory ID
		},
        
        {
                AdsppmClk_Aif_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_Aif_Csr_Hclk, //Clk ID
                AsicClk_TypeNone, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_LPAIF_CTL_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
        
        {
                AdsppmClk_AvSync_Hclk, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AVSYNC_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_Bstc_Hclk, //Source. Relying on recursion to enable both AVSync and AudSync Hclks
 				Adsppm_Mem_None //Memory ID
        },
        
        {
                AdsppmClk_Lpm_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_ahbfabric_ixfabric_lpm_clk",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_Lpass_LPM //Memory ID
        },
        
		{
			AdsppmClk_Csr_Hclk, //Clk ID Using this for VFR clock in Bear
			AsicClk_TypeInternalCGC, //Clk Type
			AsicClk_CntlAlwaysON, // Cntl Type
			AsicClk_MemCntlNone, // MemCtrl Type
			.clkInfo.hwioInfo = {
						HWIO_LPASS_AUDIO_CORE_VFR_CGCR_OFFS, //CGCR Offset
						HWIO_LPASS_AUDIO_CORE_VFR_CGCR_CLK_ENABLE_BMSK, //Enable mask
						HWIO_LPASS_AUDIO_CORE_VFR_CGCR_HW_CTL_BMSK, //HW control mask
						0	//Status mask
						},
			AdsppmClk_None, //Source
			Adsppm_Mem_None //Memory ID
		},
	
		{
				AdsppmClk_Bstc_Hclk, //Clk ID
				AsicClk_TypeInternalCGC, //Clk Type
				AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
				AsicClk_MemCntlNone, // MemCtrl Type
				.clkInfo.hwioInfo = {
						HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_OFFS, //CGCR Offset
						HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_CLK_ENABLE_BMSK, //Enable mask
						HWIO_LPASS_AUDIO_CORE_AUDSYNC_CGCR_HW_CTL_BMSK, //HW control mask
						0	//Status mask
				},
				AdsppmClk_None, //Source
				Adsppm_Mem_None //Memory ID
		},
        
		{
				AdsppmClk_Smmu_Lpass_Hclk, //Clk ID
				AsicClk_TypeInternalCGC, //Clk Type
				AsicClk_CntlAlwaysON, //AsicClk_CntlHW, // Cntl Type. TODO: Fall back to AsicClk_CntlAlwaysON if HW control doesn't work
				AsicClk_MemCntlNone, // MemCtrl Type
				.clkInfo.hwioInfo = {
						HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_OFFS, //CGCR Offset
						HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_CLK_ENABLE_BMSK, //Enable mask
						HWIO_LPASS_AUDIO_CORE_AHB2AHB_PCNOC_SLV_CGCR_HW_CTL_BMSK, //HW control mask
						0	//Status mask
				},
				AdsppmClk_None, //Source
				Adsppm_Mem_None //Memory ID
		},
       
        {
                AdsppmClk_Adsp_Hclk, //Clk ID - using for pcnoc sway clk
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW_SHARED,   // Cntl Type; 
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_pcnoc_sway_clk",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
		{
                AdsppmClk_Smmu_Sysnoc_Hclk, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_pcnoc_mport_clk",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
        
        {
                AdsppmClk_Sram_Hclk, //Clk ID. Using this for LPM CGC
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW,  // Cntl Type. TODO: Fall back to AsicClk_CntlAlwaysON if HW control doesn't work
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_LPM_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_LPM_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_LPM_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_None, //Source. None is used to prevent LPM CBC to be ON all the time.
 				Adsppm_Mem_Lpass_LPM //Memory ID
        },
        
        {
                AdsppmClk_AvSync_Xo, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_avsync_xo_clk",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Bt, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_BT_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_AvSync_Xo, //Source
 				Adsppm_Mem_None //Memory ID
        },
        {
                AdsppmClk_AvSync_Fm, //Clk ID
                AsicClk_TypeInternalCGC, //Clk Type
                AsicClk_CntlHW, //AsicClk_CntlHW, // Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.hwioInfo = {
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_OFFS, //CGCR Offset
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_CLK_ENABLE_BMSK, //Enable mask
                		HWIO_LPASS_AUDIO_CORE_AVTIMER_XO_FM_CGCR_HW_CTL_BMSK, //HW control mask
                		0	//Status mask
                },
                AdsppmClk_AvSync_Xo, //Source
 				Adsppm_Mem_None //Memory ID
        },
        
		{
                AdsppmClk_Avtimer_core, //Clk ID
                AsicClk_TypeDalEnable, //Clk Type
                AsicClk_CntlSW,// Cntl Type
                AsicClk_MemCntlNone, // MemCtrl Type
                .clkInfo.clkName = "gcc_ultaudio_stc_xo_clk",//Name
                AdsppmClk_None, //Source
 				Adsppm_Mem_None //Memory ID
        },

};

AsicClockDescriptorArrayType clocks = {ADSPPM_ARRAY(clocks_array)};

/**
 * Arrays of 9607 register programming clocks
 */
const AdsppmClkIdType regProgClocks_ADSP_Master[]			= {AdsppmClk_AhbX_Hclk};
const AdsppmClkIdType regProgClocks_AIF_Master[]			= {AdsppmClk_Aif_Hclk};
const AdsppmClkIdType regProgClocks_ExtAHB_Master[]		= {AdsppmClk_AhbX_Hclk};
const AdsppmClkIdType regProgClocks_AIF_Slave[]			= {AdsppmClk_Aif_Hclk};
const AdsppmClkIdType regProgClocks_AvSync_Slave[]			= {AdsppmClk_AvSync_Xo};
const AdsppmClkIdType regProgClocks_LPM_Slave[]			= {AdsppmClk_Lpm_Hclk};
const AdsppmClkIdType regProgClocks_ExtAHB_Slave[]			= {AdsppmClk_Smmu_Lpass_Hclk};
const AdsppmClkIdType regProgClocks_DDR_Slave[]			= {AdsppmClk_Smmu_Sysnoc_Hclk};

/**
 * Array of 9607 Bus port descriptors arranged by Bus port ID
 */
AsicBusPortDescriptorType busPorts_array[AdsppmBusPort_EnumMax] = {
        {
                AdsppmBusPort_None, //ID
                0, //Connection
                AdsppmClk_None, //Bus clock
				{0, NULL}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_APPSS_PROC}, // icbarb ID
                AdsppmBusPort_None // Access port
        },
        {
                AdsppmBusPort_Adsp_Master, //ID
                AsicBusPort_Ext_M , //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock                
                {ADSPPM_ARRAY(regProgClocks_ADSP_Master)}, //Array of reg prog clocks                
                {.icbarbMaster = ICBID_MASTER_MSS_PROC}, // icbarb ID
                AdsppmBusPort_Adsp_Master // Access port
        },
      
        {
                AdsppmBusPort_Aif_Master, //ID
                AsicBusPort_AhbX_M, //Connection
                AdsppmClk_Aif_Hclk, //Bus clock
				{ADSPPM_ARRAY(regProgClocks_AIF_Master)}, //Array of reg prog clocks				  
                {.icbarbMaster = ICBID_MASTER_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
       
        {
                AdsppmBusPort_Ext_Ahb_Master, //ID
                AsicBusPort_Ext_M, //Connection
                AdsppmClk_AhbX_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_ExtAHB_Master)}, //Array of reg prog clocks
                {.icbarbMaster = ICBID_MASTER_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
        
        {
                AdsppmBusPort_Aif_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_Aif_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_AIF_Slave)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        
        {
                AdsppmBusPort_AvSync_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_AvSync_Hclk, //Bus clock
                {ADSPPM_ARRAY(regProgClocks_AvSync_Slave)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Lpm_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_Lpm_Hclk, //Bus clock
				{ADSPPM_ARRAY(regProgClocks_LPM_Slave)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        
        {
                AdsppmBusPort_Ext_Ahb_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_Smmu_Lpass_Hclk, //Bus clock
				{ADSPPM_ARRAY(regProgClocks_ExtAHB_Slave)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        {
                AdsppmBusPort_Ddr_Slave, //ID
                AsicBusPort_Ext_S, //Connection
                AdsppmClk_Smmu_Sysnoc_Hclk, //Bus clock  
                {ADSPPM_ARRAY(regProgClocks_DDR_Slave)}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_EBI1}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Master // Access port
        },
		
        {
                AdsppmBusPort_Bstc_Slave, //ID
                AsicBusPort_AhbX_S, //Connection
                AdsppmClk_Bstc_Hclk, //Bus clock
                {0, NULL}, //Array of reg prog clocks
                {.icbarbSlave = ICBID_SLAVE_AUDIO}, // icbarb ID
                AdsppmBusPort_Ext_Ahb_Slave // Access port
        },
        
};

AsicBusPortDescriptorArrayType busPorts = {ADSPPM_ARRAY(busPorts_array)};

/**
 * List of supported external bus routes
 */
const AdsppmBusRouteType extBusRoutes_array[] =
{
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        },
        {
                AdsppmBusPort_Adsp_Master, //master
                AdsppmBusPort_Ext_Ahb_Slave //slave
        },
        {
                AdsppmBusPort_Ext_Ahb_Master, //master
                AdsppmBusPort_Ddr_Slave //slave
        }
};

AsicBusRouteArrayType extBusRoutes = {ADSPPM_ARRAY(extBusRoutes_array)};

const AdsppmBusRouteType mipsBwRoutes_array[] =
{
        {
                AdsppmBusPort_Adsp_Master,
                AdsppmBusPort_Ddr_Slave
        }
};

AsicBusRouteArrayType mipsBwRoutes = {ADSPPM_ARRAY(mipsBwRoutes_array)};

/**
* Array of register programming speeds in byte/sec
* TODO:sn-> Update the contents of table below
*/
const uint64 regProgSpeeds_array[] =
{
        0,          //Adsppm_RegProg_None
        19200000*4, //Adsppm_RegProg_Norm
        61410000*4  //Adsppm_RegProg_Fast
};

AsicRegProgSpeedArrayType regProgSpeeds = {ADSPPM_ARRAY(regProgSpeeds_array)};

/**
 * Array of power domain descriptors
 */
AsicPowerDomainDescriptorType pwrDomains_array[AsicPowerDomain_EnumMax] = {
        {
                AsicPowerDomain_AON, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_Adsp, //Id
                "/core/cpu/vdd", //Name
                AsicRsc_Power_ADSP, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
		{
                AsicPowerDomain_LpassCore, //Id Should we change it to AON or make new ULT AUDIO Macro??
                "", //Name
                AsicRsc_Power_Core, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
		{
                AsicPowerDomain_Lpm, //Id
                "", //Name
                AsicRsc_Power_Mem, //Type
                AdsppmClk_Lpm_Hclk, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_SRam, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, // Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
		{
                AsicPowerDomain_Ocmem, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        },
        {
                AsicPowerDomain_Hvx, //Id
                "", //Name
                AsicRsc_None, //Type
                AdsppmClk_None, //Clock ID
                DALIPCINT_NUM_INTS, //TZ notify
                0, //TZ Done
                {0, NULL} //array of Security clocks
        }
};
   
AsicPwrDescriptorArrayType pwrDomains = {ADSPPM_ARRAY(pwrDomains_array)};

/************** BEGIN (Table Updates)*****************************************/
/* TODO:sn-> Update the contents of the tables below */

/**
 * Table of DDR compensation factors to be applied at different BW ranges
 * This is used to compensate for bus latencies/cache misses
 * Latency/Snoc floor vote also updated
 */
const AsicCompensatedDdrBwTableEntryType compensatedDdrBw_array[] = {
        //        bwThreshold,            bwVote,   latencyVote,  snocFloorVote,    honestFlag
        {            10000000,         153000000,            0,               0,           0 },
        {            60000000,         399000000,            0,               0,           0 },
        {            96000000,                -1,          600,               0,           1 },
        {           120000000,                -1,          450,               0,           1 },
        {           155000000,                -1,          450,               0,           1 },
        {           230000000,                -1,          300,               0,           1 },
        {           250000000,                -1,          300,               0,           1 },
        {          ULLONG_MAX,                -1,          300,               0,           1 }
};

AsicCompensatedDdrBwTableType compensatedDdrBwTable = {ADSPPM_ARRAY(compensatedDdrBw_array)};

const AsicCompensatedAhbBwTableEntryType compensatedAhbBw_array[] = {
    // bwThreshold,            bwVote
    {      9784305,          61440000 }, // target SVS2  19.2 Mhz
    {     19568611,         122880000 }, // target SVS2  38.4 Mhz
    {     39137222,         245760000 }, // target SVS   76.8 Mhz
    {    490240000,         490240000 }, // target NOM  153.2 Mhz
    {   ULLONG_MAX,                -1 },
};

AsicCompensatedAhbBwTableType compensatedAhbBwTable = {ADSPPM_ARRAY(compensatedAhbBw_array)};

const AsicThreadLoadingInfoType threadLoadingData_array[] =
{
        { 90,   10,      0,     0 },    // oneDominantThreadLoading
        { 65,   25,     10,     0 },    // twoDominantThreadLoading
        { 50,   30,     20,     0 }     // threeDominantThreadLoading
};

AsicThreadLoadingInfoTableType threadLoadingData = {ADSPPM_ARRAY(threadLoadingData_array)};

const uint32 audioVoiceCppFactors_array[] =
{
        2000,    // one thread active CPP   * 1000
        1600,    // two thread active CPP   * 1000
        1500,    // three thread active CPP * 1000
           0     // four thread active CPP  * 1000
};

AsicAudioVoiceCppTrendFactorsType audioVoiceCppFactors = {ADSPPM_ARRAY(audioVoiceCppFactors_array)};

// Table for cache partition configuration.
// Audio and voice classes are grouped together for the purposes
// of determining class concurrency.
// Client class combinations not present in the table will default
// to a shared cache (no partitioning).
const AsicCachePartitionConfigType cachePartitionConfig_array[] = {

    // aggregateClass,                mainPartSize

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

    { MMPM_AUDIO_CLIENT_CLASS |
      MMPM_VOICE_CLIENT_CLASS |
      MMPM_COMPUTE_CLIENT_CLASS,      HALF_SIZE }, // 2 way AV/comp

};

AsicCachePartitionConfigTableType cachePartitionConfigTable = {ADSPPM_ARRAY(cachePartitionConfig_array)};

const AsicBwConcurrencySettingsType bwConcurrencySettings =
{

    3,          /* adspDdrConcurrencyVotersThreshold*/
    15000000,   /* adspDdrCompensatedBWThreshold*/
    {
        256,    /* adspDdrConcurrencyFactor = 1*/
        384,    /* adspDdrConcurrencyFactor = 1.5*/
        512,    /* adspDdrConcurrencyFactor = 2*/
    }
};

const AsicAdspToAhbeFreqTableEntry adspToAhbeFreq_array[] = {
    // adspFreqHz,      abheFreqHz
    {   307200000,       38400000 }, // SVS2 f-max
    {   499200000,       76800000 }, // SVS f-max
    {   672000000,      153600000 }, // nominal f-max
    {   844800000,      153600000 }, // turbo ADSP f-max (nom. AHBE, no turbo)
};

AsicAdspToAhbeFreqTableType adspToAhbeFreqTable = {ADSPPM_ARRAY(adspToAhbeFreq_array)};

//Table to lookup MPPS threshold values for different client classes
//Class Name, Threshold value
const AsicAdspCacheSizeMPPSThresholdTableEntry adspcachesizeMPPSThreshold_array[] = {
    {   MMPM_AUDIO_CLIENT_CLASS,  100  },

};

AsicAdspCacheSizeMPPSThresholdTableType  adspCacheSizeMPPSThreshold = {ADSPPM_ARRAY(adspcachesizeMPPSThreshold_array)};


const AsicAdspCacheSizeBWScalingTableEntry adspcachesizebwscaling_array[] = {
    {   FULL_SIZE,             67  },
    {   THREE_QUARTER_SIZE,   100  },
    {   HALF_SIZE,            100  },    
};

AsicAdspCacheSizeBWScalingTableType  adspCacheSizeBWScaling = {ADSPPM_ARRAY(adspcachesizebwscaling_array)};

/***** END (Table updates) *****/

AsicFeatureDescType features [AsicFeatureId_enum_max] =
{
        //AsicFeatureId_Adsp_Clock_Scaling
        {
                AsicFeatureState_Enabled,
                80, //min MIPS
                492, //max MIPS
        },
        //AsicFeatureId_Adsp_LowTemp_Voltage_Restriction
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Adsp_PC
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_Scaling
        {
                AsicFeatureState_Enabled,
                32768, //min bytes per second
                561736000, //max bytes per second
        },
        //AsicFeatureId_Ahb_Sw_CG
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Ahb_DCG
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LpassCore_PC_TZ_Handshake
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_Bus_Scaling
        {
                AsicFeatureState_Enabled,
                32768, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_CoreClk_Scaling
        {
                AsicFeatureState_Enabled,
                0, //min Hz
                140434000, //max Hz
        },
        //AsicFeatureId_Min_Adsp_BW_Vote
        {
                AsicFeatureState_Disabled,
                10000000, //min bytes per second
                0x7FFFFFFF, //max bytes per second
        },
        //AsicFeatureId_InitialState
        {
                AsicFeatureState_Enabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_TimelineOptimisationMips
        {
                AsicFeatureState_Disabled,
                240, //min MIPS for periodic clients
                600  //max MIPS value
        },
        //AsicFeatureId_TimelineOptimisationBw
        {
                AsicFeatureState_Disabled,
                30000000, //min ext BW in bytes per second
                1000000000, //max ext BW in bytes per second
        },
        //AsicFeatureId_TimelineOptimisationAhb
        {
                AsicFeatureState_Enabled,
                19000000, //min AHB clock in Hz
                141000000, //max AHB clock in Hz
        },
        //AsicFeatureId_LpassClkSleepOptimization
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_LPMRetention
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        //AsicFeatureId_DomainCoreClocks
        {
                AsicFeatureState_Disabled,
                0, //min
                0, //max
        },
        // AsicFeatureId_CachePartitionControl
        {
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_DcvsControl
        {
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_AhbeScaling
        {
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_CacheSizeMPPSThreshold
        {
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },
        // AsicFeatureId_CacheSizeBWScaling
        {
                AsicFeatureState_Disabled,
                0, // min (not used)
                0, // max (not used)
        },	
};
