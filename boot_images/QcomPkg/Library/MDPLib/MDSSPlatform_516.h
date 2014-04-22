#ifndef __MDSSPlatform_516_H__
#define __MDSSPlatform_516_H__
/*=============================================================================
 
  File: MDSSPlatform_516.h
 
  Internal header file for MDP library
  
 
  Copyright (c) 2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "MDPLib_i.h"

/*===========================================================================
                                Defines 
===========================================================================*/
 
 
 /*=========================================================================
       Public Functions
 ==========================================================================*/
 
 /* SDE Clock list for SDE516 (8953)
  * 
  */

 const MDPExternalClockEntry            sSDE516DSI0ExtClocks[] = 
 {
   {"gcc_mdss_esc0_clk",     0, 2, 0, 0, 0, 0},  // Index 0 : primary source :XO , Secondary source : dsi pll
   {"gcc_mdss_pclk0_clk",    1, 0, 0, 0, 0, 0},  // Index 1 : Source DSI0_PLL
   {"gcc_mdss_byte0_clk",    1, 0, 0, 0, 0, 0},  // Index 2 : Source DSI0_PLL
   {"\0",                    0, 0, 0, 0, 0, 0},  
 };     

 const MDPExternalClockEntry			 sSDE516DSI1ExtClocks[] = 
 {
   {"gcc_mdss_esc1_clk",	 0, 2, 0, 0, 0, 0},  // Index 0 : primary source :XO , Secondary source : dsi pll
   {"gcc_mdss_pclk1_clk",	 1, 0, 0, 0, 0, 0},  // Index 1 : Source DSI1_PLL
   {"gcc_mdss_byte1_clk",	 1, 0, 0, 0, 0, 0},  // Index 2 : Source DSI1_PLL
   {"\0",				     0, 0, 0, 0, 0, 0},  
 };	 

 /*For Dual DSI Split display, DSI0/DSI1 share same PLL-- DSI0_PLL */
 const MDPExternalClockEntry            sSDE516SharedSourceExtClocks[] = 
 {
   {"gcc_mdss_esc1_clk",     0, 2, 0, 0, 0, 0},    // Index 0 : primary source :XO , Secondary source : dsi pll
   {"gcc_mdss_pclk1_clk",    1, 0, 0, 0, 0, 0},    // Index 1 : Source DSI0_PLL
   {"gcc_mdss_byte1_clk",    1, 0, 0, 0, 0, 0},    // Index 2 : Source DSI0_PLL
   {"\0",                    0, 0, 0, 0, 0, 0},  
 }; 

 const MDPClockEntry SDE516Clocks[] =
 {
     {"gcc_mdss_ahb_clk",                 0, NULL},
     {"gcc_mdss_vsync_clk",               0, NULL},
     {"gcc_mdss_mdp_clk",         320000000, NULL},
     {"gcc_mdss_axi_clk",                 0, NULL},
     {"gcc_bimc_mdss_clk",                0, NULL},
     {"\0", 0, NULL}
 };
 
 
 /* DSI0 Clock list for 8953
  */
 const MDPClockEntry sSDE516DSI0Clocks[] =
 {
     {"gcc_mdss_esc0_clk",   0,         NULL},
     {"gcc_mdss_byte0_clk",  0,         NULL},
     {"gcc_mdss_pclk0_clk",  0,         NULL},
     {"\0", 0, NULL}
 };

  /* DSI1 Clock list for 8953
   */
 const MDPClockEntry sSDE516DSI1Clocks[] =
 {
     {"gcc_mdss_esc1_clk",   0,         NULL},
     {"gcc_mdss_byte1_clk",  0,         NULL},
     {"gcc_mdss_pclk1_clk",  0,         NULL},
     {"\0", 0, NULL}
 };
 
  /* MDSS power domain list
   */
  const MDPPowerDomainEntry MDSS3xxPowerDomain[] = 
  {
      {"VDD_BIMC_SMMU", MDP_CLOCKFLAG_SHARED},
      {"VDD_MDSS",                       0x0},
      {"\0",                             0x0}
  };
 /* SDE resource list for SDE516 (8953)
 */
 const MDP_ResourceList sSDE516Resources =
 {
     (MDPPowerDomainEntry*)  &MDSS3xxPowerDomain,   /* Power domain    */
     (MDPClockEntry*)    &SDE516Clocks,       /* MDP clocks      */
     (MDPClockEntry*)    &sSDE516DSI0Clocks,  /* DSI0 clocks     */
     (MDPClockEntry*)    &sSDE516DSI1Clocks,  /* DSI1 clocks     */
     NULL,                                    /* EDP clocks      */
     NULL                                     /* HDMI clocks     */
 };

 
 /* SDE External resource list for SDE516
 */
 const MDP_ExtClockResourceList sSDE516ExtClockResources =
 {
     (MDPExternalClockEntry*)    &sSDE516DSI0ExtClocks,          /* DSI0 Ext clocks      */
     (MDPExternalClockEntry*)    &sSDE516DSI1ExtClocks,          /* DSI1 Ext clocks      */
     (MDPExternalClockEntry*)    &sSDE516SharedSourceExtClocks,  /* DSI shared clocks for dual DSI     */
 };
 
 /* Display resource list 
 */
 const DisplayResourceList sDisplaySDE516Resources =
 {
    (MDP_ResourceList*)            &sSDE516Resources,               /* SDE Resources          */
    (MDP_ExtClockResourceList*)    &sSDE516ExtClockResources,       /* SDE Ext Resources      */
 };

#endif // __MDSSPlatform_516_H__

