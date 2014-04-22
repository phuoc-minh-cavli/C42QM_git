#ifndef PM_DAL_PROP_IDS_H
#define PM_DAL_PROP_IDS_H
/*! \file
 *  
 *  \brief  pm_dal_prop_id.h ----This file contains all the available PMIC DAL device config property values.
 *  \details This file contains all the available PMIC DAL device config property values.
 *  
 *  &copy; Copyright 2013 Qualcomm Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/framework/inc/pm_dal_prop_ids.h#2 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/25/13   hs      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/

//PMIC HW peripheral specific dal properties -- starting from 1
#define PM_PROP_CLK_REG                        1
#define PM_PROP_SMPS_REG                       2
#define PM_PROP_LDO_REG                        3
#define PM_PROP_VS_REG                         4
#define PM_PROP_BOOST_REG                      5
#define PM_PROP_FTS_VOLT                       6
#define PM_PROP_HFS_VOLT                       7
#define PM_PROP_BOOST_VOLT                     8
#define PM_PROP_NMOS_VOLT                      9
#define PM_PROP_N600_VOLT                      10
#define PM_PROP_N1200_VOLT                     11
#define PM_PROP_PMOS_VOLT                      12
#define PM_PROP_LN_VOLT                        13
#define PM_PROP_CLK_SLEEP_REG                  14
#define PM_PROP_CLK_XO_REG                     15
#define PM_PROP_CLK_COMMON                     16
#define PM_PROP_PWRON_REG                      17
#define PM_PROP_GPIO_REG                       18
#define PM_PROP_MPP_REG                        19

#define PM_PROP_TALM_REG                       21
#define PM_PROP_SMPS_ILIMIT_LUT                22
#define PM_PROP_CBLPWRN_DEBOUNCE_DURATION      23
#define PM_PROP_FTS2p5_VOLT                    24
#define PM_PROP_PBS_CLIENT_REG                 25
#define PM_PROP_VBUS_REG                       26
#define PM_PROP_PBS_SUPPORTED_CLIENTS          27
#define PM_PROP_ULT_BUCK_VOLT_1                28
#define PM_PROP_ULT_BUCK_VOLT_2                29


//Target/power grid specific dal properties -- starting from 101
#define PM_PROP_PMIC_NUM                      101
#define PM_PROP_SMPS_NUM                      102
#define PM_PROP_CLK_NUM                       103
#define PM_PROP_LDO_NUM                       104
#define PM_PROP_VS_NUM                        105
#define PM_PROP_BOOST_NUM                     106
#define PM_PROP_GPIO_NUM                      107
#define PM_PROP_MPP_NUM                       108
#define PM_PROP_PWRON_NUM                     109
#define PM_PROP_RTC_NUM                       110
#define PM_PROP_TALM_NUM                      111
#define PM_PROP_MEGAXO_NUM                    112
#define PM_PROP_PAM_NODE_RSRCS                113
#define PM_PROP_PAM_NODE_NUM                  114
#define PM_PROP_REMOTE_LDO                    115
#define PM_PROP_REMOTE_CLK                    116
#define PM_PROP_REMOTE_SMPS                   117
#define PM_PROP_REMOTE_VS                     118
#define PM_PROP_XO_SPECIFIC                   119
#define PM_PROP_UICC_SPECIFIC                 120
#define PM_PROP_MPP_SPECIFIC                  121
#define PM_PROP_MX                            122
#define PM_PROP_CX                            123
#define PM_PROP_APPS                          124
#define PM_PROP_PBS_CLIENT_NUM                125
#define PM_PROP_VBUS_NUM                      126

#define PM_PROP_APPS_CFG_INFO                  127//"PM_PROP_APSS_CFG_INFO"



//dummy dal properties to get rid of compiler warnings/errors -- staring from 201
#define PM_PROP_PMIC_DUMMY_1                 201
#define PM_PROP_PMIC_DUMMY_2                 202


#endif // PM_DAL_PROP_IDS_H
