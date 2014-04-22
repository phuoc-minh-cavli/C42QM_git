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

/*This test demosntrates Different Number, Different Types and Default Function overloading*/
class displayData
{
public:
  int display(int a = 0)
  {
    return a;
  }

  int display(int a, int b)
  {
    return a+b;
  }

  string display (string s)
  {
    return s;
  }
};


int test_functionOverloading(void )
{
  int a = 10;
  int b = 20;
  string s = "string comparison.";
  displayData display;
  bool status = true;
  
  if(true != (status = (0 == display.display())))
    goto Exit;
  if(true != (status = (a == display.display(a))))
    goto Exit;
  if(true != (status = ((a + b) == display.display(a, b))))
    goto Exit;
  if(true != (status = (0 == s.compare(display.display(s)))))
    goto Exit;
Exit:
  if(status){
    qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED :  %s %d.***", __func__, __LINE__);
     return SUCCESS;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED :  %s %d.***", __func__, __LINE__);
    return FAILED;
  }
}
