/*==============================================================================
  FILE:         rsc_config.c

  OVERVIEW:     This file contains target specific data/configuration for rsc
                instances.

  DEPENDENCIES: None
                
                Copyright (c) 2016-2018 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "rsc.h"
#include "rsc_config.h"
#include "rsc_common.h"
#include "rsc_hwio.h"

/*==========================================================================
 *                        QDSP6 RSC CONFIGURATIONS
 *=========================================================================*/
uint32 g_rscChildSequence[Q6SS_RSCC_SSC_LPASS_NUMSEQWORDS] = 
{
//BUS_ISO_SEQ:
  0xe3bce8b6 ,  //0x00: 0xb6    SET1      rsc_intf_haltreq              
                //0x01: 0xe8    WAIT1     rsc_intf_haltack_event        
				//0x02: 0xbc    SET1      rsc_intf_clk_halt             
                //0x03: 0xe3    WAIT1     rsc_core_clk_off_event        
  0x969cb8b7 ,  //0x04: 0xb7    SET1      rsc_intf_clamp_l_set          
                //0x05: 0xb8    SET1      rsc_intf_clamp_e_set          
				//0x06: 0x9c    SET0      rsc_intf_clk_halt             
                //0x07: 0x96    SET0      rsc_intf_haltreq              
  0xbae3bc20 ,  //0x08: 0x20    RETURN    
//BUS_DEISO_SEQ:
                //0x09: 0xbc    SET1      rsc_intf_clk_halt             
				//0x0a: 0xe3    WAIT1     rsc_core_clk_off_event        
                //0x0b: 0xba    SET1      rsc_intf_clamp_e_clear        
  0x9cb99bbb ,  //0x0c: 0xbb    SET1      rsc_intf_fifo_areset          
                //0x0d: 0x9b    SET0      rsc_intf_fifo_areset          
				//0x0e: 0xb9    SET1      rsc_intf_clamp_l_clear        
                //0x0f: 0x9c    SET0      rsc_intf_clk_halt             
  0xa601a420 ,  //0x10: 0x1000  RETURN    
//SHORT_APCR_SEQ_LPASS:
                //0x11: 0xa4    SET1 rsc_clk_halt                
				//0x12: 0x1     DELI 52ns                              
                //0x13: 0xa6    SET1 rsc_save                
  0xabe9a986 ,  //0x14: 0x86    SET0 rsc_save                
                //0x15: 0xa9    SET1 rsc_mem_periph_slp      
				//0x16: 0xe9   	WAIT1 rsc_mem_periph_pd_event     
                //0x17: 0xab   	SET1 rsc_clamp_qmc_mem
  0xa8c6aca5 ,  //0x18: 0xa5   	SET1 rsc_clamp_io
                //0x19: 0xac    SET1 rsc_bhs_off
				//0x1a: 0xc6    WAIT0 rsc_bhs_event      
                //0x1b: 0xa8    SET1 rsc_ext_shutdown_req 
  0x81e1a188 ,  //0x1c: 0x88    SET0 rsc_ext_shutdown_req 
                //0x1d: 0xa1    SET1 rsc_child_parent_shutdown_req 
				//0x1e: 0xe1    WAIT1 rsc_parent_child_shutdown_ack 
                //0x1f: 0x81    SET0 rsc_child_parent_shutdown_req 
  0xe2a2e0a0 ,  //0x20: 0xa0    SET1 rsc_seq_wait_wakeup 
                //0x21: 0xe0    WAIT1 rsc_core_rsc_bringup_req      
				//0x22: 0xa2    SET1 rsc_child_parent_bringup_req      
                //0x23: 0xe2    WAIT1 rsc_parent_child_bringup_acknt    
  0xe68cbfa3 ,  //0x24: 0xa3    SET1 rsc_ares      
                //0x25: 0xbf    SET1 rsc_mem_stagger_ares            
				//0x26: 0x8c    SET0 rsc_bhs_off            
                //0x27: 0xe6    WAIT1 rsc_bhs_event            
  0x9f8587a7 ,  //0x28: 0xa7    SET1 rsc_restore            
                //0x29: 0x87    SET0 rsc_restore            
				//0x2a: 0x85    SET0 rsc_clamp_io            
                //0x2b: 0x9f    SET0 rsc_mem_stagger_ares            
  0x8b9eedbe ,  //0x2c: 0xbe    SET1 rsc_mem_stagger_restore
                //0x2d: 0xed    WAIT1 rsc_mem_stagger_restore_event   
				//0x2e: 0x9e    SET0 rsc_mem_stagger_restore   
                //0x2f: 0x8b    SET0 rsc_clamp_qmc_mem   
  0xb583e489 ,  //0x30: 0x89    SET0 rsc_mem_periph_slp   
                //0x31: 0xe4    WAIT1 rsc_mem_periph_ready_event   
				//0x32: 0x83    SET0 rsc_ares   
                //0x33: 0xb5    SET1 rsc_pm_irq   
  0xa4208495 ,  //0x34: 0x95    SET0 rsc_pm_irq   
                //0x35: 0x84    SET0 rsc_clk_halt   
				//0x36: 0x20    RETURN  
  //LONG_APCR_SEQ_LPASS:
                //0x37: 0xa4    SET1 rsc_clk_halt
  0x86a601e3 ,  //0x38: 0xe3    WAIT1 rsc_core_clk_off_event
                //0x39: 0x1     DELI 52ns
				//0x3a: 0xa6    SET1 rsc_save
                //0x3b: 0x86    SET0 rsc_save
  0xeaaae9a9 ,  //0x3c: 0xa9    SET1 rsc_mem_periph_slp
                //0x3d: 0xe9    WAIT1 rsc_mem_periph_pd_event
				//0x3e: 0xaa    SET1 rsc_mem_core_slp
                //0x3f: 0xea    WAIT1 rsc_mem_core_pd_event
  0xacada5ab ,  //0x40: 0xab    SET1 rsc_clamp_qmc_mem
                //0x41: 0xa5    SET1 rsc_clamp_io
				//0x42: 0xad    SET1 rsc_ldo_bypass_open
                //0x43: 0xac    SET1 rsc_bhs_off
  0xb0c6afae  , //0x44: 0xae    SET1 rsc_ldo_pd
                //0x45: 0xaf    SET1 rsc_ldo_bg_pd
				//0x46: 0xc6    WAIT0 rsc_pwr_on_event
                //0x47: 0xb0    SET1 rsc_pll_outdis
  0xb4b2b3b1 ,  //0x48: 0xb1    SET1 rsc_pll_reset
                //0x49: 0xb3    SET1 rsc_pll_bypass
				//0x4a: 0xb2    SET1 rsc_pll_freeze
                //0x4b: 0xb4    SET1 rsc_clamp_mss2cx
  0xe1a188a8 ,  //0x4c: 0xa8    SET1 rsc_ext_shutdown_req
                //0x4d: 0x88    SET0 rsc_ext_shutdown_req
				//0x4e: 0xa1    SET1 rsc_child_parent_shutdown_req
                //0x4f: 0xe1    WAIT1 rsc_parent_child_shutdown_ack
  0xa2e0a081 ,  //0x50: 0x81    SET0 rsc_child_parent_shutdown_req
                //0x51: 0xa0    SET1 rsc_seq_wait_wakeup
				//0x52: 0xe0    WAIT1 rsc_core_rsc_bringup_req
                //0x53: 0xa2    SET1 rsc_child_parent_bringup_req
  0x94bfa3e2 ,  //0x54: 0xe2    WAIT1 rsc_parent_child_bringup_ack
                //0x55: 0xa3    SET1 rsc_ares
				//0x56: 0xbf    SET1 rsc_mem_stagger_ares
                //0x57: 0x94    SET0 rsc_clamp_mss2cx
  0x91069392 ,  //0x58: 0x92    SET0 rsc_pll_freeze
                //0x59: 0x93    SET0 rsc_pll_bypass
				//0x5a: 0x6     DELI 5044ns
                //0x5b: 0x91    SET0 rsc_pll_reset
  0x9dbd068f ,  //0x5c: 0x8f    SET0 rsc_ldo_bg_pd
                //0x5d: 0x6     DELI 5044ns
				//0x5e: 0xbd    SET1 rsc_ldo_update
                //0x5f: 0x9d    SET0 rsc_ldo_update
  0x8de68c8e ,  //0x60: 0x8e    SET0 rsc_ldo_pd
                //0x61: 0x8c    SET0 rsc_bhs_off
				//0x62: 0xe6    WAIT1 rsc_pwr_on_event 
                //0x63: 0x8d    SET0 rsc_ldo_bypass_open
  0x8587a7eb ,  //0x64: 0xeb    WAIT1 rsc_mem_refresh_event
                //0x65: 0xa7    SET1 rsc_restore
				//0x66: 0x87    SET0 rsc_restore
                //0x67: 0x85    SET0 rsc_clamp_io
  0xbe90e79f ,  //0x68: 0x9f    SET0 rsc_mem_stagger_ares
                //0x69: 0xe7    WAIT1 rsc_pll_ready_event
				//0x6a: 0x90    SET0 rsc_pll_outdis
                //0x6b: 0xbe    SET1 rsc_mem_stagger_restore
  0x8a8b9eed ,  //0x6c: 0xed    WAIT1 rsc_mem_stagger_restore_event
                //0x6d: 0x9e    SET0 rsc_mem_stagger_restore
				//0x6e: 0x8b    SET0 rsc_clamp_qmc_mem
                //0x6f: 0x8a    SET0 rsc_mem_core_slp
  0x83e489e5 ,  //0x70: 0xe5    WAIT1 rsc_mem_core_ready_event
                //0x71: 0x89    SET0 rsc_mem_periph_slp
				//0x72: 0xe4    WAIT1 rsc_mem_periph_ready_event
                //0x73: 0x83    SET0 rsc_ares
  0x208495b5 ,  //0x74: 0xb5    SET1 rsc_pm_irq
                //0x75: 0x95    SET0 rsc_pm_irq
				//0x76: 0x84    SET0 rsc_clk_halt
                //0x77: 0x20    RETURN  
};                      
                                             