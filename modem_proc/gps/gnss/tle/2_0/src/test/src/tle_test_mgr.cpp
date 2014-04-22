/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  TLE teat manager implementation
GENERAL DESCRIPTION
   This file contains constants, classes, structures and function implementations
   for TDP (e.g Terrestrial Downlink Positioning) test manager class.


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

  $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/tle/2_0/src/test/src/tle_test_mgr.cpp#1 $
  $DateTime: 2020/07/21 21:23:44 $
  $Author: pwbldsvc $

when        who     what, where, why
--------    ---   ----------------------------------------------------------
8/04/2015  ahmadh Initial version

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*=============================================================================

                                INCLUDE FILES

=============================================================================*/
#ifdef FEATURE_CGPS_PLAYBACK

#include <iostream>
#include "tle_test_mgr.h"
#include "tle_cell_db.h"

/*==============================================================================

       static Variables

==============================================================================*/

/*==============================================================================

       static Functions

==============================================================================*/

/*==============================================================================
  FUNCTION      tle_TesterClass::tle_TesterClass
  
  DESCRIPTION 	
    Constructor for tle_TesterClass.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    None
  
  SIDE EFFECTS  
    None

==============================================================================*/
tle_TesterClass::tle_TesterClass()
{
  tle_TestMgrClass &z_TestMgr = tle_TestMgrClass::GetInstance();
  z_TestMgr.AddTester(this);
}

/*==============================================================================
  FUNCTION      tle_TesterClass::PrintTestResult
  
  DESCRIPTION 	
    This function prints the test result with the test name.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    strTestName - Test name
    (Input)

    b_Result - Test result, true or false.
    (Input)
  
  RETURN VALUE  
    None
  
  SIDE EFFECTS  
    None

==============================================================================*/
void tle_TesterClass::PrintTestResult(const char *strTestName, bool b_Result)
{
  std::cout << "Testing " << strTestName << "...";
  if (true == b_Result)
  {
    std::cout << "Passed!" << std::endl;
  }
  else
  {
    std::cout << "Failed!" << std::endl;
  }
}

/*==============================================================================
  FUNCTION      tle_TestMgrClass::GetInstance
  
  DESCRIPTION 	
    This function returns the reference of the TestMgr single instance.
  
  DEPENDENCIES
    None
  
  PARAMETERS
    None
  
  RETURN VALUE  
    Reference of the TestMgr instance.
  
  SIDE EFFECTS  
    None

==============================================================================*/
tle_TestMgrClass &tle_TestMgrClass::GetInstance()
{
  static tle_TestMgrClass z_Instance;

  return z_Instance;
}

#endif /* FEATURE_CGPS_PLAYBACK */