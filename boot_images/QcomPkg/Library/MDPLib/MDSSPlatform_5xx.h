#ifndef __MDSSPlatform_5xx_H__
#define __MDSSPlatform_5xx_H__
/*=============================================================================
 
  File: MDSSPlatform_5xx.h
 
  Internal header file for MDP library
  
 
  Copyright (c) 2011-2016, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "MDPLib_i.h"
#include "MDSSPlatform_516.h"

/*===========================================================================
                                Defines 
===========================================================================*/

#define RES_MAX_FAMILY_MAJOR                      6
#define RES_MAX_FAMILY_MINOR                     18

#define XO_DEFAULT_FREQ_IN_HZ                    19200000
 /*=========================================================================
       Public Functions
 ==========================================================================*/

/* MDP resource list for MDP1.x
*/
const MDP_HwMinorFamilyResourceList asHarwareFamilyMinor1x[RES_MAX_FAMILY_MINOR] = {
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.0 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.1 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.2 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.3 */   
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.4 */   
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.5 */   
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.6 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.7 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.8 */    
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.9 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.10 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.11 */    
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.12 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.13 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.14 */
    {MDSS_DEVICE_VERSION_MAX, NULL},                                              /* MDP Family 1.15 */
	{MDSS_DEVICE_VERSION_05_16, (DisplayResourceList*)&sDisplaySDE516Resources},  /* MDP Family 1.16 , MSM8953 */
	{MDSS_DEVICE_VERSION_05_17, (DisplayResourceList*)&sDisplaySDE516Resources},  /* MDP Family 1.17 , QCS405 */
};


/* MDP resource list  based on MDP major version.
*/
const MDP_HwMajorFamilyResourceList asHarwareFamilyMajor[RES_MAX_FAMILY_MAJOR] = {
    {NULL},                                                    /* MDP Family 0.x   */
    {(MDP_HwMinorFamilyResourceList*)&asHarwareFamilyMinor1x[0]}, /* MDP Fmaily 1.x   */
    {NULL},                                                    /* MDP Family 2.x   */ 
    {NULL},                                                    /* MDP Fmaily 3.x   */
    {NULL},                                                    /* MDP Family 4.x   */
    {NULL}                                                     /* MDP Family 5.x   */
}; 
       



#endif // __MDSSPlatform_5xx_H__

