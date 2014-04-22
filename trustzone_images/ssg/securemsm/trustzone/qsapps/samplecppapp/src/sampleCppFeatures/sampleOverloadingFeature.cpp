/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string>
#include "cheaders.h"
#include "cppFeatures.h"
#include "samplecppapp.h"

using namespace std;

/* This calls the sample function and operator overloading specifc Features testing. */

int test_overloadingFeatures()
{
  int status = 0;
  if(0 != (status = test_operatorOverloading()))
     goto Exit;
  if(0 != (status = test_functionOverloading()))
     goto Exit;
 
  Exit:
    if(!status){
      qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : %s %d.***", __func__, __LINE__);
      return SUCCESS;
    }
    else{
      qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : %s %d.***", __func__, __LINE__);
      return FAILED;
    }
}