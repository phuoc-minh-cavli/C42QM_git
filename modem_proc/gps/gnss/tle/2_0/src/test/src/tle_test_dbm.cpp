/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE Test DBM implementation
GENERAL DESCRIPTION
   This file contains constants, classes, structures and function implementations
   for TLE DBM Test cases.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

  Copyright (c) 2017 Qualcomm Technologies, Inc. 
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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/tle/2_0/src/test/src/tle_test_dbm.cpp#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
10/16/2017  hleung Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef FEATURE_CGPS_PLAYBACK

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#include <iostream>
#include <vector>
#include <algorithm>
#include "tle_dbm_mgr.h"
#include "tle_test_dbm.h"
#include "tle_fdcl_store.h"

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
  FUNCTION      tle_DbmTesterClass::Run
  
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
void tle_DbmTesterClass::Run()
{
  int q_NumOfTestsPassed = 0;

  std::cout << std::endl;
  std::cout << "=======================================" << std::endl;
  std::cout << "           DBM Tests Started           " << std::endl;
  std::cout << "=======================================" << std::endl;

  PrintTestResult("TestFdcl", TestFdcl());
  PrintTestResult("TestFdclNvItems", TestFdclNvItems());
  PrintTestResult("TestGnssListSort(0)", TestGnssListSort(0));  
  PrintTestResult("TestGnssListSort(1)", TestGnssListSort(1));
  PrintTestResult("TestGnssListSort(2)", TestGnssListSort(2));
  PrintTestResult("TestGnssListSort(3)", TestGnssListSort(3));
  PrintTestResult("TestGnssListSort(100)", TestGnssListSort(100));
  PrintTestResult("TestGnssListSort(101)", TestGnssListSort(101));
  PrintTestResult("TestGnssListSort(99999)", TestGnssListSort(99999));
  PrintTestResult("TestGnssListSort2", TestGnssListSort2());

  std::cout << "=======================================" << std::endl;
  std::cout << "           DBM Tests Finished          " << std::endl;
  std::cout << "=======================================" << std::endl;
}

/*==============================================================================
FUNCTION      GetDummyFdclInjectionData

DESCRIPTION
  This function returns a pointer to a dummy FDCL injection data.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  Pointer to the dummy FDCL injection data.

SIDE EFFECTS
  None

==============================================================================*/
static const tle_FdclInjectDataReqType *GetDummyFdclInjectionData()
{
  static const tle_FdclInjectDataReqType z_DummyData =
  {
    //z_Msg
    {
      //uint32_t cellPosList_len
      2,
      //qmiLocFdclCellPosStructT_v02 cellPosList[QMI_LOC_FDCL_CELL_POS_LIST_LENGTH_V02]
      {
        //cellPosList[0]
        {
          //qmiLocFdclCellPosValidMaskT_v02 validMask
          QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02 | QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_UNC_V02 | QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_CONFIDENCE_V02 | QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02,          
          //qmiLocFdclCellIdStructT_v02 cellId
          {
            //qmiLocFdclAirInterfaceTypeEnumT_v02 airInterfaceType
            eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_LTE_V02,
            //uint32_t id1
            123,
            //uint32_t id2
            456,
            //uint32_t id3
            789,
            //uint32_t id4
            234,
          },
          //double latitude
          37.3725,
          //double longitude
          -121.9871,
          //float horCoverageRadius
          1000,
          //uint8_t horConfidence
          99,
          //qmiLocReliabilityEnumT_v02 horReliability
          eQMI_LOC_RELIABILITY_HIGH_V02,
          //float altitude
          10,
          //float altUnc
          10,
          //uint8_t altConfidence
          99,
          //qmiLocReliabilityEnumT_v02 altReliability
          eQMI_LOC_RELIABILITY_HIGH_V02,
        },
        //cellPosList[1]
        {
          //qmiLocFdclCellPosValidMaskT_v02 validMask
          QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALTITUDE_V02 | QMI_LOC_FDCL_CELL_POS_MASK_VALID_ALT_RELIABILITY_V02,
          //qmiLocFdclCellIdStructT_v02 cellId
          {
            //qmiLocFdclAirInterfaceTypeEnumT_v02 airInterfaceType
            eQMI_LOC_FDCL_AIR_INTERFACE_TYPE_WCDMA_V02,
            //uint32_t id1
            111,
            //uint32_t id2
            222,
            //uint32_t id3
            333,
            //uint32_t id4
            444,
          },
          //double latitude
          27.3725,
          //double longitude
          -131.9871,
          //float horCoverageRadius
          500,
          //uint8_t horConfidence
          63,
          //qmiLocReliabilityEnumT_v02 horReliability
          eQMI_LOC_RELIABILITY_HIGH_V02,
          //float altitude
          5,
          //float altUnc
          50,
          //uint8_t altConfidence
          63,
          //qmiLocReliabilityEnumT_v02 altReliability
          eQMI_LOC_RELIABILITY_HIGH_V02,
        },
      },

      //uint32_t daysValid;
      10,

      //uint8_t timestampUtc_valid;
      0,

      //uint64_t timestampUtc;
      0,
    },

    //l_Client_handle
    0,

    //q_TransactionId
    0
  };

  return &z_DummyData;
}

/*==============================================================================
  FUNCTION      tle_Asn1TesterClass::TestFdcl
  
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
bool tle_DbmTesterClass::TestFdcl()
{
  tle_FdclStoreClass z_FdclStore;

  const tle_FdclInjectDataReqType *pz_InjectedData = GetDummyFdclInjectionData();
  
  m_pzDbmMgr->InjectFdclData(*pz_InjectedData);

  z_FdclStore.SaveToStore(m_pzDbmMgr->m_FDCellDbTable);

  z_FdclStore.InjectToDbm(*m_pzDbmMgr);
  
  // TestFdcl passed
  return true;
}

/*==============================================================================
  FUNCTION      tle_DbmTesterClass::TestFdclNvItems
  
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
bool tle_DbmTesterClass::TestFdclNvItems()
{
  tle_FdclStoreClass z_FdclStore;

  tle_FdclStoreClass::FdclNvItems z_NvItems =
  {
    //q_Version
    123,
    //t_CumDeviceUpTimeInSecs
    456,
  };
  
  if (!z_FdclStore.SaveNvItems(z_NvItems))
  {
    return false;
  }

  memset(&z_NvItems, 0, sizeof(z_NvItems));

  if (!z_FdclStore.RetrieveNvItems(z_NvItems))
  {
    return false;
  }

  if (z_NvItems.q_Version != 1 ||
    z_NvItems.t_CumDeviceUpTimeInSecs != 456)
  {
    return false;
  }  
  
  // TestFdcl passed
  return true;
}

/*==============================================================================
  FUNCTION      tle_DbmTesterClass::TestGnssListSort
  
  DESCRIPTION 	
    Test case for Gnss List Sorting algorithm
  
  DEPENDENCIES
    None
  
  PARAMETERS
    q_ListLength - Number of elements to test
    (Input)
  
  RETURN VALUE  
    true  - Test passed
    false - Test failed
  
  SIDE EFFECTS  
    None

==============================================================================*/
bool tle_DbmTesterClass::TestGnssListSort(uint32 q_ListLength)
{
  gnss_StlNameSpace::gnss_ListClass<int> GnssList;
  std::vector<int> NumList(q_ListLength);
  uint32 i;

  class IntCompFn : public gnss_StlNameSpace::MergeSortCompFunctionBase<int>
  {
  public:
    bool isSortCompFunImplemented() { return true; }

    bool sortComp(int &first, int &second)
    {
      return first < second;
    }
  };

  for (i = 0; i < q_ListLength; i++)
  {
    NumList[i] = i;
  }
  
  std::random_shuffle(NumList.begin(), NumList.end());

  for (i = 0; i < q_ListLength; i++)
  {
    GnssList.push_back(NumList[i]);
  }

  GnssList.msort(&IntCompFn());

  if (NumList.size() != GnssList.getNumOfElements())
  {
    /* sanity check */
    return false;
  }

  i = 0;
  for (gnss_StlNameSpace::gnss_ListClass<int>::iterator it = GnssList.begin(); it != GnssList.end(); ++it)
  {
    if (**it != i)
    {
      return false;
    }

    if (it != GnssList.begin())
    {
      /* check pPrev is set correctly */
      if (*it.m_Content.m_pPrev->m_pData != i - 1)
      {
        return false;
      }
    }

    i++;
  }

  if (i != q_ListLength)
  {
    return false;
  }

  return true;
}

/*==============================================================================
  FUNCTION      tle_DbmTesterClass::TestGnssListSort2
  
  DESCRIPTION 	
    Test case for Gnss List Sorting algorithm
  
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
bool tle_DbmTesterClass::TestGnssListSort2()
{
  gnss_StlNameSpace::gnss_ListClass<int> GnssList;
  const int val = 10;
  std::vector<int> NumList(10, val);
  uint32 i;

  class IntCompFn : public gnss_StlNameSpace::MergeSortCompFunctionBase<int>
  {
  public:
    bool isSortCompFunImplemented() { return true; }

    bool sortComp(int &first, int &second)
    {
      return first < second;
    }
  };

  for (i = 0; i < NumList.size(); i++)
  {
    GnssList.push_back(NumList[i]);
  }

  GnssList.msort(&IntCompFn());

  if (NumList.size() != GnssList.getNumOfElements())
  {
    /* sanity check */
    return false;
  }

  for (gnss_StlNameSpace::gnss_ListClass<int>::iterator it = GnssList.begin(); it != GnssList.end(); ++it)
  {
    if (**it != val)
    {
      return false;
    }

    if (it != GnssList.begin())
    {
      /* check pPrev is set correctly */
      if (*it.m_Content.m_pPrev->m_pData != val)
      {
        return false;
      }
    }
  }

  return true;
}

#endif /* FEATURE_CGPS_PLAYBACK */