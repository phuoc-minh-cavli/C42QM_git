/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             GPS Rom Almanac Module

GENERAL DESCRIPTION
  This module contains Default Rom Almanac helper function that is used 
  within the GPS sub-system.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2005-2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.
  Copyright (c) 2013-2019 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.


Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

$Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/common/src/gps_rom_almanac.c#1 $ 
  $DateTime: 2020/07/21 21:23:44 $ 
  $Author: pwbldsvc $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2019-05-09          From: xtra3grc_2019cs.txt
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "customer.h"
#include "comdef.h"
#include "gnss_common.h"
#include "err.h"
#include "msg.h"

/*
 * Constant definitions
*/

/*
 * Local function prototypes
*/


/*
 ******************************************************************************
 * gps_GetGpsRomAlmanac
 *
 * Function description:
 *
 * 	gps_GetGpsRomAlmanac is a helper function used to retreive the default GPS
 *	ROM almanac.
 *
 * Parameters: 
 *
 *  pz_GpsRomAlm - Pointer to the GPS ROM almanac structure
 *
 * Return value: 
 *
 *  void
 *
 ******************************************************************************
*/

void gps_GetGpsRomAlmanac( gps_AlmStructType **pz_RomAlm )
{

  if ( (NULL == pz_RomAlm) || (NULL == *pz_RomAlm) )
  {    
    MSG(MSG_SSID_MGPPE,MSG_LEGACY_ERROR, 
        "gps_GetGpsRomAlmanac: NULL input pointer");
    return;
  }
  pz_RomAlm[0]->u_Sv = 1 ;
  pz_RomAlm[0]->u_Health = 0 ;
  pz_RomAlm[0]->u_Toa = 78 ;
  pz_RomAlm[0]->w_E = 18461 ;
  pz_RomAlm[0]->w_DeltaI = 5546 ;
  pz_RomAlm[0]->w_OmegaDot = 64852 ;
  pz_RomAlm[0]->q_SqrtA = 10554582ul ;
  pz_RomAlm[0]->q_Omega0 = 9488818ul ;
  pz_RomAlm[0]->q_Omega = 1867678ul ;
  pz_RomAlm[0]->q_M0 = 14278798ul ;
  pz_RomAlm[0]->w_Af0 = 2030 ;
  pz_RomAlm[0]->w_Af1 = 2046 ;
  pz_RomAlm[0]->w_GpsWeek = 2053 ;

  pz_RomAlm[1]->u_Sv = 2 ;
  pz_RomAlm[1]->u_Health = 0 ;
  pz_RomAlm[1]->u_Toa = 78 ;
  pz_RomAlm[1]->w_E = 39613 ;
  pz_RomAlm[1]->w_DeltaI = 2063 ;
  pz_RomAlm[1]->w_OmegaDot = 64832 ;
  pz_RomAlm[1]->q_SqrtA = 10554684ul ;
  pz_RomAlm[1]->q_Omega0 = 9305989ul ;
  pz_RomAlm[1]->q_Omega = 12126513ul ;
  pz_RomAlm[1]->q_M0 = 15028276ul ;
  pz_RomAlm[1]->w_Af0 = 1824 ;
  pz_RomAlm[1]->w_Af1 = 2045 ;
  pz_RomAlm[1]->w_GpsWeek = 2053 ;

  pz_RomAlm[2]->u_Sv = 3 ;
  pz_RomAlm[2]->u_Health = 0 ;
  pz_RomAlm[2]->u_Toa = 78 ;
  pz_RomAlm[2]->w_E = 4525 ;
  pz_RomAlm[2]->w_DeltaI = 3330 ;
  pz_RomAlm[2]->w_OmegaDot = 64825 ;
  pz_RomAlm[2]->q_SqrtA = 10554838ul ;
  pz_RomAlm[2]->q_Omega0 = 12272043ul ;
  pz_RomAlm[2]->q_Omega = 1551509ul ;
  pz_RomAlm[2]->q_M0 = 11802652ul ;
  pz_RomAlm[2]->w_Af0 = 197 ;
  pz_RomAlm[2]->w_Af1 = 0 ;
  pz_RomAlm[2]->w_GpsWeek = 2053 ;

  /* No almanac for PRN 4 */
  pz_RomAlm[3]->u_Sv = C_GPS_SV_ALM_INVALID ;

  pz_RomAlm[4]->u_Sv = 5 ;
  pz_RomAlm[4]->u_Health = 0 ;
  pz_RomAlm[4]->u_Toa = 78 ;
  pz_RomAlm[4]->w_E = 11807 ;
  pz_RomAlm[4]->w_DeltaI = 1141 ;
  pz_RomAlm[4]->w_OmegaDot = 64811 ;
  pz_RomAlm[4]->q_SqrtA = 10554619ul ;
  pz_RomAlm[4]->q_Omega0 = 12200767ul ;
  pz_RomAlm[4]->q_Omega = 1884981ul ;
  pz_RomAlm[4]->q_M0 = 5419393ul ;
  pz_RomAlm[4]->w_Af0 = 1 ;
  pz_RomAlm[4]->w_Af1 = 0 ;
  pz_RomAlm[4]->w_GpsWeek = 2053 ;

  pz_RomAlm[5]->u_Sv = 6 ;
  pz_RomAlm[5]->u_Health = 0 ;
  pz_RomAlm[5]->u_Toa = 78 ;
  pz_RomAlm[5]->w_E = 3293 ;
  pz_RomAlm[5]->w_DeltaI = 5487 ;
  pz_RomAlm[5]->w_OmegaDot = 64848 ;
  pz_RomAlm[5]->q_SqrtA = 10554432ul ;
  pz_RomAlm[5]->q_Omega0 = 9466710ul ;
  pz_RomAlm[5]->q_Omega = 13875960ul ;
  pz_RomAlm[5]->q_M0 = 14772581ul ;
  pz_RomAlm[5]->w_Af0 = 215 ;
  pz_RomAlm[5]->w_Af1 = 2045 ;
  pz_RomAlm[5]->w_GpsWeek = 2053 ;

  pz_RomAlm[6]->u_Sv = 7 ;
  pz_RomAlm[6]->u_Health = 0 ;
  pz_RomAlm[6]->u_Toa = 78 ;
  pz_RomAlm[6]->w_E = 26569 ;
  pz_RomAlm[6]->w_DeltaI = 2247 ;
  pz_RomAlm[6]->w_OmegaDot = 64831 ;
  pz_RomAlm[6]->q_SqrtA = 10554779ul ;
  pz_RomAlm[6]->q_Omega0 = 1123916ul ;
  pz_RomAlm[6]->q_Omega = 10188523ul ;
  pz_RomAlm[6]->q_M0 = 12703242ul ;
  pz_RomAlm[6]->w_Af0 = 2022 ;
  pz_RomAlm[6]->w_Af1 = 2046 ;
  pz_RomAlm[6]->w_GpsWeek = 2053 ;

  pz_RomAlm[7]->u_Sv = 8 ;
  pz_RomAlm[7]->u_Health = 0 ;
  pz_RomAlm[7]->u_Toa = 78 ;
  pz_RomAlm[7]->w_E = 9245 ;
  pz_RomAlm[7]->w_DeltaI = 4658 ;
  pz_RomAlm[7]->w_OmegaDot = 64861 ;
  pz_RomAlm[7]->q_SqrtA = 10554773ul ;
  pz_RomAlm[7]->q_Omega0 = 6641743ul ;
  pz_RomAlm[7]->q_Omega = 16153132ul ;
  pz_RomAlm[7]->q_M0 = 3792871ul ;
  pz_RomAlm[7]->w_Af0 = 1897 ;
  pz_RomAlm[7]->w_Af1 = 0 ;
  pz_RomAlm[7]->w_GpsWeek = 2053 ;

  pz_RomAlm[8]->u_Sv = 9 ;
  pz_RomAlm[8]->u_Health = 0 ;
  pz_RomAlm[8]->u_Toa = 78 ;
  pz_RomAlm[8]->w_E = 2589 ;
  pz_RomAlm[8]->w_DeltaI = 1686 ;
  pz_RomAlm[8]->w_OmegaDot = 64852 ;
  pz_RomAlm[8]->q_SqrtA = 10554639ul ;
  pz_RomAlm[8]->q_Omega0 = 15029076ul ;
  pz_RomAlm[8]->q_Omega = 4684235ul ;
  pz_RomAlm[8]->q_M0 = 4129275ul ;
  pz_RomAlm[8]->w_Af0 = 431 ;
  pz_RomAlm[8]->w_Af1 = 2046 ;
  pz_RomAlm[8]->w_GpsWeek = 2053 ;

  pz_RomAlm[9]->u_Sv = 10 ;
  pz_RomAlm[9]->u_Health = 0 ;
  pz_RomAlm[9]->u_Toa = 78 ;
  pz_RomAlm[9]->w_E = 9505 ;
  pz_RomAlm[9]->w_DeltaI = 3332 ;
  pz_RomAlm[9]->w_OmegaDot = 64824 ;
  pz_RomAlm[9]->q_SqrtA = 10554578ul ;
  pz_RomAlm[9]->q_Omega0 = 12263396ul ;
  pz_RomAlm[9]->q_Omega = 9587897ul ;
  pz_RomAlm[9]->q_M0 = 8717086ul ;
  pz_RomAlm[9]->w_Af0 = 48 ;
  pz_RomAlm[9]->w_Af1 = 2045 ;
  pz_RomAlm[9]->w_GpsWeek = 2053 ;

  pz_RomAlm[10]->u_Sv = 11 ;
  pz_RomAlm[10]->u_Health = 0 ;
  pz_RomAlm[10]->u_Toa = 78 ;
  pz_RomAlm[10]->w_E = 34780 ;
  pz_RomAlm[10]->w_DeltaI = 60069 ;
  pz_RomAlm[10]->w_OmegaDot = 64810 ;
  pz_RomAlm[10]->q_SqrtA = 10554664ul ;
  pz_RomAlm[10]->q_Omega0 = 8287665ul ;
  pz_RomAlm[10]->q_Omega = 5033665ul ;
  pz_RomAlm[10]->q_M0 = 12364874ul ;
  pz_RomAlm[10]->w_Af0 = 1412 ;
  pz_RomAlm[10]->w_Af1 = 2 ;
  pz_RomAlm[10]->w_GpsWeek = 2053 ;

  pz_RomAlm[11]->u_Sv = 12 ;
  pz_RomAlm[11]->u_Health = 0 ;
  pz_RomAlm[11]->u_Toa = 78 ;
  pz_RomAlm[11]->w_E = 16068 ;
  pz_RomAlm[11]->w_DeltaI = 6704 ;
  pz_RomAlm[11]->w_OmegaDot = 64833 ;
  pz_RomAlm[11]->q_SqrtA = 10554562ul ;
  pz_RomAlm[11]->q_Omega0 = 4061282ul ;
  pz_RomAlm[11]->q_Omega = 2888225ul ;
  pz_RomAlm[11]->q_M0 = 10602356ul ;
  pz_RomAlm[11]->w_Af0 = 254 ;
  pz_RomAlm[11]->w_Af1 = 2047 ;
  pz_RomAlm[11]->w_GpsWeek = 2053 ;

  pz_RomAlm[12]->u_Sv = 13 ;
  pz_RomAlm[12]->u_Health = 0 ;
  pz_RomAlm[12]->u_Toa = 78 ;
  pz_RomAlm[12]->w_E = 7429 ;
  pz_RomAlm[12]->w_DeltaI = 4284 ;
  pz_RomAlm[12]->w_OmegaDot = 64870 ;
  pz_RomAlm[12]->q_SqrtA = 10554621ul ;
  pz_RomAlm[12]->q_Omega0 = 15394904ul ;
  pz_RomAlm[12]->q_Omega = 3471726ul ;
  pz_RomAlm[12]->q_M0 = 573552ul ;
  pz_RomAlm[12]->w_Af0 = 1981 ;
  pz_RomAlm[12]->w_Af1 = 0 ;
  pz_RomAlm[12]->w_GpsWeek = 2053 ;

  pz_RomAlm[13]->u_Sv = 14 ;
  pz_RomAlm[13]->u_Health = 0 ;
  pz_RomAlm[13]->u_Toa = 78 ;
  pz_RomAlm[13]->w_E = 22235 ;
  pz_RomAlm[13]->w_DeltaI = 3038 ;
  pz_RomAlm[13]->w_OmegaDot = 64863 ;
  pz_RomAlm[13]->q_SqrtA = 10554690ul ;
  pz_RomAlm[13]->q_Omega0 = 15276598ul ;
  pz_RomAlm[13]->q_Omega = 11596159ul ;
  pz_RomAlm[13]->q_M0 = 2638612ul ;
  pz_RomAlm[13]->w_Af0 = 1964 ;
  pz_RomAlm[13]->w_Af1 = 0 ;
  pz_RomAlm[13]->w_GpsWeek = 2053 ;

  pz_RomAlm[14]->u_Sv = 15 ;
  pz_RomAlm[14]->u_Health = 0 ;
  pz_RomAlm[14]->u_Toa = 78 ;
  pz_RomAlm[14]->w_E = 23728 ;
  pz_RomAlm[14]->w_DeltaI = 63124 ;
  pz_RomAlm[14]->w_OmegaDot = 64831 ;
  pz_RomAlm[14]->q_SqrtA = 10554639ul ;
  pz_RomAlm[14]->q_Omega0 = 14816259ul ;
  pz_RomAlm[14]->q_Omega = 2065544ul ;
  pz_RomAlm[14]->q_M0 = 961546ul ;
  pz_RomAlm[14]->w_Af0 = 1720 ;
  pz_RomAlm[14]->w_Af1 = 1 ;
  pz_RomAlm[14]->w_GpsWeek = 2053 ;

  pz_RomAlm[15]->u_Sv = 16 ;
  pz_RomAlm[15]->u_Health = 0 ;
  pz_RomAlm[15]->u_Toa = 78 ;
  pz_RomAlm[15]->w_E = 22678 ;
  pz_RomAlm[15]->w_DeltaI = 6761 ;
  pz_RomAlm[15]->w_OmegaDot = 64832 ;
  pz_RomAlm[15]->q_SqrtA = 10554412ul ;
  pz_RomAlm[15]->q_Omega0 = 4111802ul ;
  pz_RomAlm[15]->q_Omega = 1541904ul ;
  pz_RomAlm[15]->q_M0 = 5627496ul ;
  pz_RomAlm[15]->w_Af0 = 2006 ;
  pz_RomAlm[15]->w_Af1 = 2047 ;
  pz_RomAlm[15]->w_GpsWeek = 2053 ;

  pz_RomAlm[16]->u_Sv = 17 ;
  pz_RomAlm[16]->u_Health = 0 ;
  pz_RomAlm[16]->u_Toa = 78 ;
  pz_RomAlm[16]->w_E = 26913 ;
  pz_RomAlm[16]->w_DeltaI = 6972 ;
  pz_RomAlm[16]->w_OmegaDot = 64874 ;
  pz_RomAlm[16]->q_SqrtA = 10554786ul ;
  pz_RomAlm[16]->q_Omega0 = 6821574ul ;
  pz_RomAlm[16]->q_Omega = 12203630ul ;
  pz_RomAlm[16]->q_M0 = 3083960ul ;
  pz_RomAlm[16]->w_Af0 = 58 ;
  pz_RomAlm[16]->w_Af1 = 2 ;
  pz_RomAlm[16]->w_GpsWeek = 2053 ;

  pz_RomAlm[17]->u_Sv = 18 ;
  pz_RomAlm[17]->u_Health = 0 ;
  pz_RomAlm[17]->u_Toa = 78 ;
  pz_RomAlm[17]->w_E = 32083 ;
  pz_RomAlm[17]->w_DeltaI = 1965 ;
  pz_RomAlm[17]->w_OmegaDot = 64834 ;
  pz_RomAlm[17]->q_SqrtA = 10554673ul ;
  pz_RomAlm[17]->q_Omega0 = 9342371ul ;
  pz_RomAlm[17]->q_Omega = 3706824ul ;
  pz_RomAlm[17]->q_M0 = 13588283ul ;
  pz_RomAlm[17]->w_Af0 = 56 ;
  pz_RomAlm[17]->w_Af1 = 1 ;
  pz_RomAlm[17]->w_GpsWeek = 2053 ;

  pz_RomAlm[18]->u_Sv = 19 ;
  pz_RomAlm[18]->u_Health = 0 ;
  pz_RomAlm[18]->u_Toa = 78 ;
  pz_RomAlm[18]->w_E = 20119 ;
  pz_RomAlm[18]->w_DeltaI = 6614 ;
  pz_RomAlm[18]->w_OmegaDot = 64876 ;
  pz_RomAlm[18]->q_SqrtA = 10554568ul ;
  pz_RomAlm[18]->q_Omega0 = 6945110ul ;
  pz_RomAlm[18]->q_Omega = 3732724ul ;
  pz_RomAlm[18]->q_M0 = 10601776ul ;
  pz_RomAlm[18]->w_Af0 = 1714 ;
  pz_RomAlm[18]->w_Af1 = 1 ;
  pz_RomAlm[18]->w_GpsWeek = 2053 ;

  pz_RomAlm[19]->u_Sv = 20 ;
  pz_RomAlm[19]->u_Health = 0 ;
  pz_RomAlm[19]->u_Toa = 78 ;
  pz_RomAlm[19]->w_E = 9200 ;
  pz_RomAlm[19]->w_DeltaI = 63492 ;
  pz_RomAlm[19]->w_OmegaDot = 64792 ;
  pz_RomAlm[19]->q_SqrtA = 10554610ul ;
  pz_RomAlm[19]->q_Omega0 = 11947895ul ;
  pz_RomAlm[19]->q_Omega = 6290752ul ;
  pz_RomAlm[19]->q_M0 = 13284766ul ;
  pz_RomAlm[19]->w_Af0 = 550 ;
  pz_RomAlm[19]->w_Af1 = 0 ;
  pz_RomAlm[19]->w_GpsWeek = 2053 ;

  pz_RomAlm[20]->u_Sv = 21 ;
  pz_RomAlm[20]->u_Health = 0 ;
  pz_RomAlm[20]->u_Toa = 78 ;
  pz_RomAlm[20]->w_E = 51638 ;
  pz_RomAlm[20]->w_DeltaI = 1019 ;
  pz_RomAlm[20]->w_OmegaDot = 64832 ;
  pz_RomAlm[20]->q_SqrtA = 10554747ul ;
  pz_RomAlm[20]->q_Omega0 = 9318110ul ;
  pz_RomAlm[20]->q_Omega = 12923604ul ;
  pz_RomAlm[20]->q_M0 = 9374369ul ;
  pz_RomAlm[20]->w_Af0 = 1846 ;
  pz_RomAlm[20]->w_Af1 = 2 ;
  pz_RomAlm[20]->w_GpsWeek = 2053 ;

  pz_RomAlm[21]->u_Sv = 22 ;
  pz_RomAlm[21]->u_Health = 0 ;
  pz_RomAlm[21]->u_Toa = 78 ;
  pz_RomAlm[21]->w_E = 15213 ;
  pz_RomAlm[21]->w_DeltaI = 62942 ;
  pz_RomAlm[21]->w_OmegaDot = 64789 ;
  pz_RomAlm[21]->q_SqrtA = 10554849ul ;
  pz_RomAlm[21]->q_Omega0 = 12078022ul ;
  pz_RomAlm[21]->q_Omega = 13176636ul ;
  pz_RomAlm[21]->q_M0 = 1139258ul ;
  pz_RomAlm[21]->w_Af0 = 1313 ;
  pz_RomAlm[21]->w_Af1 = 2046 ;
  pz_RomAlm[21]->w_GpsWeek = 2053 ;

  pz_RomAlm[22]->u_Sv = 23 ;
  pz_RomAlm[22]->u_Health = 0 ;
  pz_RomAlm[22]->u_Toa = 78 ;
  pz_RomAlm[22]->w_E = 27720 ;
  pz_RomAlm[22]->w_DeltaI = 151 ;
  pz_RomAlm[22]->w_OmegaDot = 64847 ;
  pz_RomAlm[22]->q_SqrtA = 10554447ul ;
  pz_RomAlm[22]->q_Omega0 = 15015738ul ;
  pz_RomAlm[22]->q_Omega = 10740862ul ;
  pz_RomAlm[22]->q_M0 = 16341303ul ;
  pz_RomAlm[22]->w_Af0 = 1853 ;
  pz_RomAlm[22]->w_Af1 = 0 ;
  pz_RomAlm[22]->w_GpsWeek = 2053 ;

  pz_RomAlm[23]->u_Sv = 24 ;
  pz_RomAlm[23]->u_Health = 0 ;
  pz_RomAlm[23]->u_Toa = 78 ;
  pz_RomAlm[23]->w_E = 17394 ;
  pz_RomAlm[23]->w_DeltaI = 64979 ;
  pz_RomAlm[23]->w_OmegaDot = 64817 ;
  pz_RomAlm[23]->q_SqrtA = 10554701ul ;
  pz_RomAlm[23]->q_Omega0 = 948423ul ;
  pz_RomAlm[23]->q_Omega = 1615871ul ;
  pz_RomAlm[23]->q_M0 = 14991402ul ;
  pz_RomAlm[23]->w_Af0 = 1979 ;
  pz_RomAlm[23]->w_Af1 = 0 ;
  pz_RomAlm[23]->w_GpsWeek = 2053 ;

  pz_RomAlm[24]->u_Sv = 25 ;
  pz_RomAlm[24]->u_Health = 0 ;
  pz_RomAlm[24]->u_Toa = 78 ;
  pz_RomAlm[24]->w_E = 17724 ;
  pz_RomAlm[24]->w_DeltaI = 4611 ;
  pz_RomAlm[24]->w_OmegaDot = 64820 ;
  pz_RomAlm[24]->q_SqrtA = 10554694ul ;
  pz_RomAlm[24]->q_Omega0 = 3889813ul ;
  pz_RomAlm[24]->q_Omega = 2395283ul ;
  pz_RomAlm[24]->q_M0 = 9691120ul ;
  pz_RomAlm[24]->w_Af0 = 1255 ;
  pz_RomAlm[24]->w_Af1 = 2047 ;
  pz_RomAlm[24]->w_GpsWeek = 2053 ;

  pz_RomAlm[25]->u_Sv = 26 ;
  pz_RomAlm[25]->u_Health = 0 ;
  pz_RomAlm[25]->u_Toa = 78 ;
  pz_RomAlm[25]->w_E = 7822 ;
  pz_RomAlm[25]->w_DeltaI = 1492 ;
  pz_RomAlm[25]->w_OmegaDot = 64800 ;
  pz_RomAlm[25]->q_SqrtA = 10554458ul ;
  pz_RomAlm[25]->q_Omega0 = 3814046ul ;
  pz_RomAlm[25]->q_Omega = 464289ul ;
  pz_RomAlm[25]->q_M0 = 8060394ul ;
  pz_RomAlm[25]->w_Af0 = 212 ;
  pz_RomAlm[25]->w_Af1 = 3 ;
  pz_RomAlm[25]->w_GpsWeek = 2053 ;

  pz_RomAlm[26]->u_Sv = 27 ;
  pz_RomAlm[26]->u_Health = 0 ;
  pz_RomAlm[26]->u_Toa = 78 ;
  pz_RomAlm[26]->w_E = 14902 ;
  pz_RomAlm[26]->w_DeltaI = 6042 ;
  pz_RomAlm[26]->w_OmegaDot = 64870 ;
  pz_RomAlm[26]->q_SqrtA = 10554580ul ;
  pz_RomAlm[26]->q_Omega0 = 6671812ul ;
  pz_RomAlm[26]->q_Omega = 1298633ul ;
  pz_RomAlm[26]->q_M0 = 3240890ul ;
  pz_RomAlm[26]->w_Af0 = 1909 ;
  pz_RomAlm[26]->w_Af1 = 2045 ;
  pz_RomAlm[26]->w_GpsWeek = 2053 ;

  pz_RomAlm[27]->u_Sv = 28 ;
  pz_RomAlm[27]->u_Health = 0 ;
  pz_RomAlm[27]->u_Toa = 78 ;
  pz_RomAlm[27]->w_E = 39941 ;
  pz_RomAlm[27]->w_DeltaI = 6586 ;
  pz_RomAlm[27]->w_OmegaDot = 64834 ;
  pz_RomAlm[27]->q_SqrtA = 10554678ul ;
  pz_RomAlm[27]->q_Omega0 = 4122401ul ;
  pz_RomAlm[27]->q_Omega = 12859912ul ;
  pz_RomAlm[27]->q_M0 = 5483762ul ;
  pz_RomAlm[27]->w_Af0 = 805 ;
  pz_RomAlm[27]->w_Af1 = 0 ;
  pz_RomAlm[27]->w_GpsWeek = 2053 ;

  pz_RomAlm[28]->u_Sv = 29 ;
  pz_RomAlm[28]->u_Health = 0 ;
  pz_RomAlm[28]->u_Toa = 78 ;
  pz_RomAlm[28]->w_E = 2206 ;
  pz_RomAlm[28]->w_DeltaI = 7215 ;
  pz_RomAlm[28]->w_OmegaDot = 64876 ;
  pz_RomAlm[28]->q_SqrtA = 10554816ul ;
  pz_RomAlm[28]->q_Omega0 = 6851267ul ;
  pz_RomAlm[28]->q_Omega = 4448828ul ;
  pz_RomAlm[28]->q_M0 = 4883527ul ;
  pz_RomAlm[28]->w_Af0 = 211 ;
  pz_RomAlm[28]->w_Af1 = 2045 ;
  pz_RomAlm[28]->w_GpsWeek = 2053 ;

  pz_RomAlm[29]->u_Sv = 30 ;
  pz_RomAlm[29]->u_Health = 0 ;
  pz_RomAlm[29]->u_Toa = 78 ;
  pz_RomAlm[29]->w_E = 8386 ;
  pz_RomAlm[29]->w_DeltaI = 65356 ;
  pz_RomAlm[29]->w_OmegaDot = 64814 ;
  pz_RomAlm[29]->q_SqrtA = 10554441ul ;
  pz_RomAlm[29]->q_Omega0 = 1199589ul ;
  pz_RomAlm[29]->q_Omega = 8834004ul ;
  pz_RomAlm[29]->q_M0 = 12668613ul ;
  pz_RomAlm[29]->w_Af0 = 1938 ;
  pz_RomAlm[29]->w_Af1 = 2046 ;
  pz_RomAlm[29]->w_GpsWeek = 2053 ;

  pz_RomAlm[30]->u_Sv = 31 ;
  pz_RomAlm[30]->u_Health = 0 ;
  pz_RomAlm[30]->u_Toa = 78 ;
  pz_RomAlm[30]->w_E = 19309 ;
  pz_RomAlm[30]->w_DeltaI = 3006 ;
  pz_RomAlm[30]->w_OmegaDot = 64831 ;
  pz_RomAlm[30]->q_SqrtA = 10554491ul ;
  pz_RomAlm[30]->q_Omega0 = 1161306ul ;
  pz_RomAlm[30]->q_Omega = 8741ul ;
  pz_RomAlm[30]->q_M0 = 11665952ul ;
  pz_RomAlm[30]->w_Af0 = 40 ;
  pz_RomAlm[30]->w_Af1 = 2047 ;
  pz_RomAlm[30]->w_GpsWeek = 2053 ;

  pz_RomAlm[31]->u_Sv = 32 ;
  pz_RomAlm[31]->u_Health = 0 ;
  pz_RomAlm[31]->u_Toa = 78 ;
  pz_RomAlm[31]->w_E = 6367 ;
  pz_RomAlm[31]->w_DeltaI = 2398 ;
  pz_RomAlm[31]->w_OmegaDot = 64858 ;
  pz_RomAlm[31]->q_SqrtA = 10554456ul ;
  pz_RomAlm[31]->q_Omega0 = 15046816ul ;
  pz_RomAlm[31]->q_Omega = 10089002ul ;
  pz_RomAlm[31]->q_M0 = 4992630ul ;
  pz_RomAlm[31]->w_Af0 = 1957 ;
  pz_RomAlm[31]->w_Af1 = 5 ;
  pz_RomAlm[31]->w_GpsWeek = 2053 ;

}
