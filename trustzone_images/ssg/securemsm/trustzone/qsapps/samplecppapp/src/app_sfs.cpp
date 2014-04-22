/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/**
  We want to compile the app_sfs.c file via clang++ to ensure the intearction of 
  generated executable and commonlib interfaces are working as expected. 
  At the same time, we want to make sure that code reuse will happen as much as
  as possible and code maintainability will take as less effort as possible. 
  Keeping that in mind, we are including the app_sfs.c file here dirrectly.
 **/
#include "cheaders.h"
#include <string>
using namespace std;

#include "app_sfs.c"