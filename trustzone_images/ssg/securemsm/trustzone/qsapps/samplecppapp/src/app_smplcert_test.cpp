/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */


/**
  This test is to demonstrate the MINK IPC from sampleCPP TA
  to samplecert TA. Indirect inclusion of correspoding .c file 
  have been done to have code reuse and avoid code maintainibility.
 **/

#include "cheaders.h"
#include <string>
using namespace std;

#include "app_smplcert_test.c"