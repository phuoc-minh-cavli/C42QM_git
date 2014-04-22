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

/*This calls the template and generic programming specific tests.*/

int test_templateFeature()
{
  int status = 0;
  if(SUCCESS != (status = test_templateFunction()))
     goto Exit;
  
  if(SUCCESS != (status = test_STLFeature()))
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
