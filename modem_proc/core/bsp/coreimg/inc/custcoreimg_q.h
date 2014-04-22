/*===============================================================================
#
# CORE Image overide custfile
#
# GENERAL DESCRIPTION
#    build script
#
# Copyright (c) 2009-2009 by Qualcomm Technologies, Incorporated.
# All Rights Reserved.
# QUALCOMM Proprietary/GTDR
#
#-------------------------------------------------------------------------------
#
#  $Header: //components/rel/core.mpss/3.10/bsp/coreimg/inc/custcoreimg_q.h#2 $
#  $DateTime: 2018/12/18 02:39:12 $
#  $Author: pwbldsvc $
#  $Change: 17879814 $
#                      EDIT HISTORY FOR FILE
#
#  This section contains comments describing changes made to the module.
#  Notice that changes are listed in reverse chronological order.
#
# when       who     what, where, why
# --------   ---     ---------------------------------------------------------
#
#===============================================================================*/
#ifndef CUSTCOREIMG_Q_H
#define CUSTCOREIMG_Q_H

/* Include the target cust file for this build flavor */
//#include AMSS_CUST_H
#include AMSS_CUST_H
#undef FEATURE_NV_RUIM
#undef FEATURE_UIM_RUN_TIME_ENABLE
#undef FEATURE_DIAG_WCDMA_CFN_SFN_TIMESTAMP
#undef FEATURE_IS2000
#undef FEATURE_A2
//#undef FEATURE_DSM_MEM_CHK
#undef FEATURE_NV_MEID_ENABLE





#endif /* CUSTCOREIMG_Q_H */
