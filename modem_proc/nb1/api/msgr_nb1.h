/*!
  @file msgr_nb1.h

  @brief
   Describes the modules using the message router

  Details...

*/

/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/nb1.mpss/2.2/api/msgr_nb1.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef __MSGR_NB1_H__
#define __MSGR_NB1_H__

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/


#include <msgr_umid.h>  /* UMID definitions */

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#define MSGR_NB1_ML1           MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x01 )
#define MSGR_NB1_ML1_SM        MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x02 )
#define MSGR_NB1_ML1_DLM       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x03 )
#define MSGR_NB1_ML1_ULM       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x04 )
#define MSGR_NB1_ML1_MGR       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x05 )
#define MSGR_NB1_PLT           MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x06 )

#define MSGR_LTE_LL1_SRCH      MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x07 )
#define MSGR_LTE_LL1_DL        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x08 )
#define MSGR_LTE_LL1_UL        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x09 )
#define MSGR_LTE_LL1_SYS       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0a )
#define MSGR_LTE_LL1_ASYNC     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0b )

#define MSGR_NB1_CPHY          MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x0c )
#define MSGR_NB1_RRC           MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x0d )
#define MSGR_LTE_A2_SIM        MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x0e )
#define MSGR_NB1_MAC           MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x0f )
#define MSGR_NB1_MAC_EXT       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x10 )  // TEMPORARY: Used until messages from A2, ML1 modules are defined
#define MSGR_NB1_RLCDL         MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x11 )
#define MSGR_NB1_RLCUL         MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x12 )
#define MSGR_NB1_PDCPDL        MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x13 )
#define MSGR_NB1_PDCPUL        MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x14 )
#define MSGR_LTE_DDEHW         MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x16 ) // Used for MSIM simulation.
#define MSGR_LTE_A2_DL_PHY     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x17 )
#define MSGR_LTE_STARTUP       MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x18 )
#define MSGR_LTE_EMM           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x19 )
#define MSGR_NB1_CPHY_TEST     MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x1a )
#define MSGR_LTE_ESM           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1c )
#define MSGR_LTE_CM            MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1d )

#define MSGR_LTE_APP_DSPSW     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1e )
#define MSGR_LTE_APP_DSPFW     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x1f )

#define MSGR_NB1_ML1_GM        MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x1b )
#define MSGR_NB1_ML1_SCHDLR    MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x20 )
#define MSGR_NB1_ML1_FWD       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x22 )
#define MSGR_LTE_A2T           MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x23 )
#define MSGR_NB1_ML1_IMP       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x24 )
#define MSGR_NB1_ML1_TEST      MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x25 )

#define MSGR_LTE_Q6_MSG_GW     MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x26 )
#define MSGR_NB1_TLB           MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x27 )

#define MSGR_NB1_ML1_RFMGR     MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x29 )
#define MSGR_NB1_ML1_BPLMN     MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x2a )
#define MSGR_NB1_ML1_SLEEPMGR  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x2b )
#define MSGR_NB1_ML1_MD_GSM    MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x2c )
#define MSGR_NB1_ML1_GAPMGR    MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x2d )
#define MSGR_NB1_ML1_MD_WCDMA  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x2e )
#define MSGR_NB1_CPHY_IRAT_MEAS_W2L  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x2f)
#define MSGR_NB1_CPHY_IRAT_MEAS_G2L  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x30)
#define MSGR_NB1_ML1_HRPDMEAS  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x31 )
#define MSGR_NB1_ML1_POS       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x32 )
#define MSGR_LTE_FC            MSGR_TECH_MODULE( MSGR_TECH_LTE, 0x33 )
#define MSGR_NB1_ML1_AFC       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x34 )
#define MSGR_NB1_CPHY_IRAT_MEAS_D2L  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x35)
#define MSGR_NB1_CPHY_IRAT_MEAS_C2L  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x36)
#define MSGR_NB1_CPHY_IRAT_MEAS_F2L  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x37)
#define MSGR_NB1_ML1_COEX      MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x38)
#define MSGR_NB1_ML1_CXM       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x39)
#define MSGR_NB1_ML1_COMMON    MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x3a)
#define MSGR_NB1_PDCPOFFLOAD   MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x3b)
#define MSGR_NB1_CPHY_IRAT_MEAS_T2L  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x3c)
#define MSGR_NB1_ML1_MCLK      MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x3d)
#define MSGR_NB1_ML1_OFFLOAD   MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x3e)
#define MSGR_NB1_ML1_COEX_DSDA MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x3f)
#define MSGR_NB1_ML1_NLIC      MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x40)
#define MSGR_NB1_ML1_MSMGR     MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x41 )
#define MSGR_NB1_PDCPCOMP      MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x42)
#define MSGR_NB1_L2            MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x44)

/* FW interface */
#define MSGR_NB_FW_APP    MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x45)
#define MSGR_NB1_FW_SYS   MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x46)
#define MSGR_NB1_FW_SRCH  MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x47)
#define MSGR_NB1_FW_DL    MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x48)
#define MSGR_NB1_FW_UL    MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x49)
#define MSGR_NB1_FW_RX    MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x4a)
#define MSGR_NB1_FW_DBACK MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x4b)
#define MSGR_NB1_FW_PLT   MSGR_TECH_MODULE(MSGR_TECH_NB1, 0x4c)

/* NB1 NS interface */
#define MSGR_NB1_NS       MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x4d )
#define MSGR_NB1_ML1_POI  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x4e )
#define MSGR_NB1_ML1_POI_SLEEPMGR  MSGR_TECH_MODULE( MSGR_TECH_NB1, 0x4f)


#endif /* __MSGR_NB1_H__ */
