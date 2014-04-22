/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <stddef.h>
#include <string>
#include "cheaders.h"
#include "cppFeatures.h"
#include "samplecppapp.h"

using namespace std;

/* This test demonstrates new and delete keyword Features. */

class app
{
  char name[40];
  int ref;
  int app_id;
};


int test_dynamicAllocation()
{
  int *p1;
  app *app1;
  int *p2;
  
  p1 = new int;
  app1 = new app;
  p2 = new int[30];
  
  //some code using above variable.
  qsee_log(QSEE_LOG_MSG_ERROR,"p1 size is : %d \n", sizeof(p1));
  qsee_log(QSEE_LOG_MSG_ERROR,"App1 size is : %d \n", sizeof(app1));
  qsee_log(QSEE_LOG_MSG_ERROR,"p2 size is : %d \n", sizeof(p2));
  
  delete p1; // this says we are deleting a thing.
  delete app1;
  delete []p2; // this says that we are deleting an array of things.
  
  qsee_log(QSEE_LOG_MSG_ERROR,"Dynamic Allocation and deletion is completed.");
  
  return SUCCESS;
}