/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE Test CCAT implementation
GENERAL DESCRIPTION
   This file contains constants, classes, structures and function implementations
   for TLE CCAT Test cases.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
==============================================================================*/


/*==============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

Version Control

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/tle/2_0/src/test/src/tle_test_ccat.cpp#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
9/14/2015  hleung Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef FEATURE_CGPS_PLAYBACK

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include <iostream>
#include "tle_ccat_mgr.h"
#include "tle_test_ccat.h"

using namespace tle_CcatNamespace;

/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/

/*=============================================================================

       Structures and enumerated types

=============================================================================*/


/*==============================================================================

       Variables

==============================================================================*/

/*==============================================================================
  FUNCTION      tle_CcatTesterClass::Run
  
  DESCRIPTION 	
    This function runs all the tests for ASN.1.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    pzAsn1Decoder - Points to the decoder under test.
    (Input)

    pzAsn1Encoder - Points to the encoder under test.
    (Input)
  
  RETURN VALUE  
    None
  
  SIDE EFFECTS  
    None

==============================================================================*/
void tle_CcatTesterClass::Run()
{
  int q_NumOfTestsPassed = 0;

  std::cout << std::endl;
  std::cout << "=======================================" << std::endl;
  std::cout << "           CCAT Tests Started          " << std::endl;
  std::cout << "=======================================" << std::endl;

  PrintTestResult("TestGetCountryForMcc", TestGetCountryForMcc());
  PrintTestResult("TestGetCCATPosExpectFound", TestGetCCATPosExpectFound());
  PrintTestResult("TestGetCCATPosExpectNotFound", TestGetCCATPosExpectNotFound());

  std::cout << "=======================================" << std::endl;
  std::cout << "           CCAT Tests Finished         " << std::endl;
  std::cout << "=======================================" << std::endl;
}

/*==============================================================================
  FUNCTION      tle_Asn1TesterClass::TestGetCountryForMcc
  
  DESCRIPTION 	
    Test case for MCC
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    true  - Test passed
    false - Test failed
  
  SIDE EFFECTS  
    None

==============================================================================*/
bool tle_CcatTesterClass::TestGetCountryForMcc()
{
  const uint16 cw_UsMccMin = 310;
  const uint16 cw_UsMccMax = 316;

  // Test US MCC
  for (uint16 w_I=cw_UsMccMin; w_I<=cw_UsMccMax; w_I++)
  {
    if (tle_CcatMgrClass::COUNTRY_UNITED_STATES !=
      m_pzCcatMgr->GetCountryForMcc(w_I))
    {
      return false;
    }
  }

  if (tle_CcatMgrClass::COUNTRY_UNITED_STATES ==
    m_pzCcatMgr->GetCountryForMcc(cw_UsMccMin - 1))
  {
    return false;
  }

  if (tle_CcatMgrClass::COUNTRY_UNITED_STATES ==
    m_pzCcatMgr->GetCountryForMcc(cw_UsMccMax + 1))
  {
    return false;
  }

  // TestGetCountryForMcc passed
  return true;
}

/*==============================================================================
  FUNCTION      tle_Asn1TesterClass::TestGetCCATPosExpectFound
  
  DESCRIPTION 	
    Test case for GetCCATPos expected to be found.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    true  - Test passed
    false - Test failed
  
  SIDE EFFECTS  
    None

==============================================================================*/
bool tle_CcatTesterClass::TestGetCCATPosExpectFound()
{
  tle_CellType z_Cell;
  mgp_CellDBPosInfoStruct z_Pos;
  const CdmaCCATEntryType * cpz_CdmaCCATEntry = NULL;
  const UmtsCCATEntryType * cpz_UmtsCCATEntry = NULL;

  // Test CDMA
  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos));  
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_CDMA;
  cpz_CdmaCCATEntry = m_pzCcatMgr->m_cpzCdmaCCAT;
  for (uint32 q_I = 0; q_I<m_pzCcatMgr->m_cqNumOfCdmaCCAT; q_I++)
  {
    cpz_CdmaCCATEntry = &m_pzCcatMgr->m_cpzCdmaCCAT[q_I];

    // Test SID Start
    z_Cell.z_Cell.z_Cdma.z_Id.w_Sid = cpz_CdmaCCATEntry->w_SIDStart;
    if (CM_SS_SID_NONE == z_Cell.z_Cell.z_Cdma.z_Id.w_Sid)
    {
      continue;
    }
    if ( (true != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos)) ||
         (z_Pos.d_PosLLa[0] != ((DBL)cpz_CdmaCCATEntry->x_Latitude * m_pzCcatMgr->cfCDMALatLonScale)) ||
         (z_Pos.d_PosLLa[1] != ((DBL)cpz_CdmaCCATEntry->x_Longitude * m_pzCcatMgr->cfCDMALatLonScale)) ||
         (z_Pos.f_PosUnc != ((FLT)cpz_CdmaCCATEntry->w_Radius * m_pzCcatMgr->cfCDMACellRadiusScale)) )
    {
      return false;
    }

    // Test mid-point
    z_Cell.z_Cell.z_Cdma.z_Id.w_Sid =
      (cpz_CdmaCCATEntry->w_SIDStart + cpz_CdmaCCATEntry->w_SIDEnd) / 2;
    if (CM_SS_SID_NONE == z_Cell.z_Cell.z_Cdma.z_Id.w_Sid)
    {
      continue;
    }    
    if ( (true != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos)) ||
      (z_Pos.d_PosLLa[0] != (cpz_CdmaCCATEntry->x_Latitude * m_pzCcatMgr->cfCDMALatLonScale)) ||
      (z_Pos.d_PosLLa[1] != (cpz_CdmaCCATEntry->x_Longitude * m_pzCcatMgr->cfCDMALatLonScale)) ||
         (z_Pos.f_PosUnc != (cpz_CdmaCCATEntry->w_Radius * m_pzCcatMgr->cfCDMACellRadiusScale)) )
    {
      return false;
    }

    // Test SID End    
    z_Cell.z_Cell.z_Cdma.z_Id.w_Sid = cpz_CdmaCCATEntry->w_SIDEnd;
    if (CM_SS_SID_NONE == z_Cell.z_Cell.z_Cdma.z_Id.w_Sid)
    {
      continue;
    }
    if ( (true != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos)) ||
         (z_Pos.d_PosLLa[0] != (cpz_CdmaCCATEntry->x_Latitude * m_pzCcatMgr->cfCDMALatLonScale)) ||
         (z_Pos.d_PosLLa[1] != (cpz_CdmaCCATEntry->x_Longitude * m_pzCcatMgr->cfCDMALatLonScale)) ||
         (z_Pos.f_PosUnc != (cpz_CdmaCCATEntry->w_Radius * m_pzCcatMgr->cfCDMACellRadiusScale)) )
    {
      return false;
    }
  }

  // Test WCDMA/GSM/LTE
  const cgps_ActiveRATEnumType RatTests[] =
  {
    CELLDB_ACTIVERAT_WCDMA,
    CELLDB_ACTIVERAT_GSM,
    CELLDB_ACTIVERAT_LTE,
    CELLDB_ACTIVERAT_NONE
  };

  for (const cgps_ActiveRATEnumType *pe_Rat = &RatTests[0];
       *pe_Rat != CELLDB_ACTIVERAT_NONE;
       pe_Rat++)
  {
    memset(&z_Cell, 0, sizeof(z_Cell));
    memset(&z_Pos, 0, sizeof(z_Pos)); 
    z_Cell.e_ActiveRat = *pe_Rat;

    for (uint32 q_I = 0; q_I<m_pzCcatMgr->m_cqNumOfUmtsCCAT; q_I++)
    {
      cpz_UmtsCCATEntry = &m_pzCcatMgr->m_cpzUmtsCCAT[q_I];
     
      z_Cell.z_Cell.z_Gsm.z_Id.w_Mcc = cpz_UmtsCCATEntry->w_MCC;
      if ( (true != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos)) ||
        (z_Pos.d_PosLLa[0] != (cpz_UmtsCCATEntry->f_Lat * DEG2RAD)) ||
        (z_Pos.d_PosLLa[1] != ((DBL)cpz_UmtsCCATEntry->f_Lon * DEG2RAD)) ||
        (z_Pos.f_PosUnc != ((FLT)cpz_UmtsCCATEntry->q_MAR)) )
      {
        return false;
      }
    }
  }

  return true;
}

/*==============================================================================
  FUNCTION      tle_Asn1TesterClass::TestGetCCATPosExpectNotFound
  
  DESCRIPTION 	
    Test case for GetCCATPos expected to be not found.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    true  - Test passed
    false - Test failed
  
  SIDE EFFECTS  
    None

==============================================================================*/
bool tle_CcatTesterClass::TestGetCCATPosExpectNotFound()
{
  tle_CellType z_Cell;
  mgp_CellDBPosInfoStruct z_Pos;

  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos)); 
 
  // CDMA: Invalid SID
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_CDMA;
  z_Cell.z_Cell.z_Cdma.z_Id.w_Sid = 0xFFFF;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }  
  z_Cell.z_Cell.z_Cdma.z_Id.w_Sid = CM_SS_SID_NONE;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }

  // GSM: Invalid MCC
  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos)); 
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_GSM;
  z_Cell.z_Cell.z_Gsm.z_Id.w_Mcc = 0xFFFF;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }  

  // WCDMA: Invalid MCC
  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos)); 
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_WCDMA;
  z_Cell.z_Cell.z_Wcdma.z_Id.w_Mcc = 0xFFFF;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }  

  // LTE: Invalid MCC
  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos)); 
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_LTE;
  z_Cell.z_Cell.z_Lte.z_Id.w_Mcc = 0xFFFF;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }

  // Invalid RAT
  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos)); 
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_TDSCDMA;
  z_Cell.z_Cell.z_Lte.z_Id.w_Mcc = m_pzCcatMgr->m_cpzUmtsCCAT[0].w_MCC;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }
  
  // Invalid RAT
  memset(&z_Cell, 0, sizeof(z_Cell));
  memset(&z_Pos, 0, sizeof(z_Pos)); 
  z_Cell.e_ActiveRat = CELLDB_ACTIVERAT_NONE;
  z_Cell.z_Cell.z_Lte.z_Id.w_Mcc = m_pzCcatMgr->m_cpzUmtsCCAT[0].w_MCC;
  if (false != m_pzCcatMgr->GetCCATPos(z_Cell, &z_Pos))
  {
    return false;
  }

  return true;
}

#endif /* FEATURE_CGPS_PLAYBACK */