 /*! \file  pm_config_target_sbl_sequence.c 
 *   
 *  \brief  File Contains the PMIC Set Mode Driver Implementation 
 *  \details Set Mode Driver implementation is responsible for setting and getting 
 *  all mode settings such as Register values, memory values, etc. 
 *   
 *    PMIC code generation Version: 1.0.0.0 
 *    Date: 6/19/2019 
 *    PMIC PSI Version: MDM9205-SBL-06192019-b0x30_v0x31 - Approved 
 *    PBS RAM Version: PME9205_3p0_RAM_b0x30_v0x25 
 *    This file contains code for Target specific settings and modes. 
 *   
 *  &copy; Copyright 2018 Qualcomm Technologies Incorporated, All Rights Reserved 
 */ 
 
/*=========================================================================== 
 
                        EDIT HISTORY FOR MODULE 
 
  This document is created by a code generator, therefore this section will 
  not contain comments describing changes made to the module. 
 
$Header: //components/rel/boot.xf/0.2/QcomPkg/Library/PmicLib/target/mdm9205_pme9205/psi/pm_config_target_sbl_sequence.c#13 $  
$DateTime: 2019/07/02 20:46:00 $  $Author: pwbldsvc $ 
 
when       who     what, where, why 
--------   ---     ----------------------------------------------------------  
 
===========================================================================*/ 
 
/*=========================================================================== 
 
                     INCLUDE FILES  
 
===========================================================================*/ 
 
#include "pm_target_information.h" 
#include "pm_config_sbl.h" 
 
/*========================== PSI Sequence LUT =============================*/ 
 
//NOTES ON CREATING PSI SEQUENCE: 
 
//1. When creating PSI sequence(Table A), if configuration do not need conditional check, set cond start/end index to 0 
//2. If Reg configuration require conditional check, indicate the range of conditional check using cond start/end index 
//3. For Reg operation PM_SBL_DELAY case, address field contains delay amount in us 
//4. For Reg Operation PM_SBL_PBS_RAM case, data field contains index to access in PBS RAM Table 
//5. For Reg Operation PM_SBL_PBS_RAM case, address field contains size of PBS RAM 
 
//Table B (Conditional configuration check Table): 
//1. DO NOT use the first entry. Index 0 is used when no conditional configuration is needed (This is just to avoid confusion) 
//2. Single or multiple configuration can be added in Table B 
//3. If multiple configuration is needed, it should be entered in a consecutive order, so that it can be indexed in one range(from Table A) 
 
pm_sbl_config_info_type
pm_sbl_config_info = 
{
 	PSI_SIGNATURE,	// PSI Signature
 	0x01,	// PSI Major Version
 	0x03,	// PSI Minor Version
 	0x01,	// Number of target configurations
 	0x3	// Total number of conditional entries on pm_sbl_cond_seq_type table
 };
pm_sbl_cond_seq_type 
pm_sbl_cond_seq [ ] = 
{	
	 // sid 	data 	mask 	register 	operator	 
	 { 0,	0x30,	0xFF,	0x0052,	EQUAL}, 	//1
	 { 0,	0x20,	0xFF,	0x0052,	LESS_OR_EQUAL}, 	//2
	 { 0,	0x01,	0x01,	0xC308,	EQUAL}, 	//3
 };
pm_sbl_seq_type
pm_sbl_seq [ ] = 
{
	 //Mode - PBS_CONFIG_PME9205 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xC0,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0,	     0x0100,	 0xFF,	 PM_SBL_PBS_RAM, 0,	 0}, 	  
	 { 0,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7042,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7043,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7050,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7051,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x04,	 0x7054,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x08,	 0x7055,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x70DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7313,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xFF,	 0x7312,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x0C,	 0x7315,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7413,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xFF,	 0x7412,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7415,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x03,	 0x7240,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7241,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x7246,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x7040,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - INTERRUPT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x80,	 0x0546,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - PON_INIT 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x80,	 0x087F,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xE4,	 0x0880,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0882,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x2446,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x5A46,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - INIT_GENERAL 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x4A,	 0x3940,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x63,	 0x3744,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x3746,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x3747,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x40,	 0x3C40,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - RESET_CONFIG_PME9205 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x00,	 0x085B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0863,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x085A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x085B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0843,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0840,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x0842,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x0841,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x0843,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x99,	 0x012C,	 0xFF,	 PM_SBL_DELAY,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x09DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - BUS 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x01,	 0x04DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x04DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x04DA,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - GPIO_CONFIG_PME9205 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x01,	 0xC241,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0xC141,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0xC041,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - CLOCKS 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x00,	 0x5059,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5058,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5048,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x75,	 0x5940,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x92,	 0x505E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x5063,	 0xFF,	 PM_SBL_WRITE,	 2,	 2}, 	  
	 { 0,	 0x00,	 0x5063,	 0xFF,	 PM_SBL_WRITE,	 1,	 1}, 	  
	 { 0,	 0x20,	 0x5044,	 0xFF,	 PM_SBL_WRITE,	 1,	 1}, 	  
	 { 0,	 0x11,	 0x5058,	 0xFF,	 PM_SBL_WRITE,	 1,	 1}, 	  
	 { 0,	 0x0F,	 0x505D,	 0xFF,	 PM_SBL_WRITE,	 1,	 1}, 	  
	 { 0,	 0x1F,	 0x505C,	 0xFF,	 PM_SBL_WRITE,	 1,	 1}, 	  
	 { 0,	 0x04,	 0x5F50,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0xDC,	 0x5F5E,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x05,	 0x5F5D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x5F68,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5941,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x5F55,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5F69,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x5F67,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x514C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x02,	 0x544C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x5448,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x504F,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x01,	 0x6147,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x6146,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x80,	 0x6046,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x5944,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x11,	 0x6156,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x5F6B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - MISC 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x00,	 0x0961,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0960,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x55,	 0x095B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x4086,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x00,	 0x4386,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x095A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x095C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0A66,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x00,	 0x0A48,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x07,	 0x0A4C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - YODA_BUCK_S2_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0xD4,	 0x1760,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x9C,	 0x1761,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0C,	 0x174D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x37,	 0x184A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x22,	 0x185A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x02,	 0x1863,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x80,	 0x1768,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x75,	 0x1769,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x38,	 0x176A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x11,	 0x1780,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x1A,	 0x1764,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x1745,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x09,	 0x1750,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x68,	 0x1755,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0F,	 0x1756,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x2B,	 0x1757,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x174C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x01,	 0x175A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x89,	 0x184B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1745,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - BUCK_S1_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0x66,	 0x154B,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x84,	 0x1460,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x9C,	 0x1461,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x0C,	 0x144D,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x2B,	 0x154A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x54,	 0x155A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x1563,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x80,	 0x1468,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x33,	 0x1469,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x09,	 0x144C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x20,	 0x146A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x11,	 0x1480,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x5A,	 0x1464,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x43,	 0x1469,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x24,	 0x1455,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x1456,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x2D,	 0x1457,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x144C,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x06,	 0x1445,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - VREG_ULS_CONFIG 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0x78,	 0x1439,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x143A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x30,	 0x4039,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x403A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x30,	 0x4339,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x433A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD0,	 0x4139,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x04,	 0x413A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x10,	 0x4239,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x05,	 0x423A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x60,	 0x1739,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x09,	 0x173A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4439,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x443A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4539,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x453A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x08,	 0x4639,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x07,	 0x463A,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - VREG_OCP 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 1,	 0x6C,	 0x1488,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0x6C,	 0x1788,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4088,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4188,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4288,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4488,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 1,	 0xD4,	 0x4688,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 //Mode - generateSblBranchAndVersion 
	 //sid 	data 	register 	Mask 	reg op 	cond start 	cond end	 
	 { 0,	 0x31,	 0x0150,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
	 { 0,	 0x30,	 0x0151,	 0xFF,	 PM_SBL_WRITE,	 0,	 0}, 	  
 	 // This line of data is created by PSI Compiler per request from Embedded SW Driver.It is not part of original code.
 	 {0, 0x00, 0x0000, 0xFF, PM_SBL_OPERATION_INVALID, 0, 0} 
 };