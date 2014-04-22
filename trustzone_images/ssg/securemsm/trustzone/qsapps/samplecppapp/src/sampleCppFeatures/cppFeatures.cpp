/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#include <string> // for string class
#include "cheaders.h"
#include "cppFeatures.h"
#include "samplecppapp.h"
using namespace std;

int cppFeatures()
{
  int status = FAILED;
  if(SUCCESS != (status = test_classFeatures()))
    goto Exit;
  if(SUCCESS != (status = test_stringFeatures()))
    goto Exit;
  if(SUCCESS != (status = test_overloadingFeatures()))
    goto Exit;
  if(SUCCESS != (status = test_encapsulationFeature()))
    goto Exit;
  if(SUCCESS != (status = test_inheritanceFeature()))
    goto Exit;
  if(SUCCESS != (status = test_templateFeature()))
    goto Exit;
  if(SUCCESS != (status = test_dynamicAllocation()))
    goto Exit;
  if(SUCCESS != (status = test_cppMiscelleneousFeature()))
    goto Exit;

  Exit:
    if(!status)
      qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED :  %s %d.***", __func__, __LINE__);
    else
      qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED :  %s %d.***", __func__, __LINE__);
  return status;
}
