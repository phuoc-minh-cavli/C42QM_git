#ifndef CUSTUIM_HW_OFFTARGET_H
#define CUSTUIM_HW_OFFTARGET_H
/*===========================================================================

            " C U S T -  U I M - H W - OFFTARGET "   H E A D E R   F I L E

DESCRIPTION
  Configuration for UIM hardware specifics for MSM9x15.  Please note that
  the 9x15 Configuration is controlled by the T_MSM9x15 Marco definition

  Copyright (c) 2012-2014, 2016 by QUALCOMM Technologies, Inc(QTI). 
  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/uim.mpss/3.3.1/cust/inc/custuim_hw_offtarget.h#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/19/16   lm      Added flag for IOE
10/10/14   vr      Fix compilation issue
09/16/13   yk      Removed the code for old targets and for old features
08/27/13   js      Fixed compilation error for MOB hotswap support
12/21/12   js      Renamed FEATURE_UIM_DSDA_MOB to FEATURE_UIM_TEST_FRAMEWORK_DSDA
12/05/12   js      UIM Parallel Processing changes
12/03/12   js      Feature for new DSDA MOB
09/05/12   js      Feature for new UIM DRV UT framework
08/28/12   js      Initial changes for UT framework
05/23/12   js      Initial Revision

===========================================================================*/

  #define FEATURE_UIM_TEST_FRAMEWORK

    /* FEATURE flag to support hotswap in MOB */
    #define FEATURE_TEST_FRAMEWORK_SUPPORT_HOTSWAP

    /* Feature flag for UIM DSDA Parallel Processing Mob */
#ifdef FEATURE_DUAL_SIM
    #define FEATURE_UIM_TEST_FRAMEWORK_DSDA
#endif /* FEATURE_DUAL_SIM */

    /* Feature flag for UIMDRV UT Framework */
    #define FEATURE_UIMDRV_UT_FRAMEWORK

    #ifdef FEATURE_UIM_IOE_DISABLE_CSIM_RUIM
      #define FEATURE_UIM_DISABLE_NON_IOE_TC  // Flag to disable all the NON IOE(LWG, GENSS, LTE ONLY testcase(Hotswap...)
    #endif /* FEATURE_UIM_IOE_DISABLE_CSIM_RUIM */
#endif /* CUSTUIM_HW_OFFTARGET_H */
