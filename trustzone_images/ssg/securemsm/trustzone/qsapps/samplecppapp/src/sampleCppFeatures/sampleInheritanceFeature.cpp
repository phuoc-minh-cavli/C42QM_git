/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string>
#include "cheaders.h"
#include "cppFeatures.h"
#include "samplecppapp.h"

/* This test demonstrates inheritance specific Features. */

// base class
class Index
{
  protected:
    int count;

  public:
    //zero argument constructor
    Index()
    {
      count=0;
    }

    // one argument constructor
    Index(int c)
    {
      count=c;
    }

    int display()
    {
      return count;
    }

    void operator ++ ()
    {
      count++;
    }
};

//derived class
class Index1:public Index 
{
  public :
    Index1()
    {
      count=0;
    }

    Index1(int c)
    {
      count=c;
    }

    void operator --()
    {
      count--;
    }
};

int test_inheritanceFeature()
{
  Index1 obj;   // this will not give error
  Index1 obj1(8); // this will give error because constructor and destructor Never get inherited. so we need to define it in derived class.
  int a = obj1.display();
  ++obj1;
  ++obj1;
  int b = obj1.display();
  Index1 obj2(b);
  --obj2;
  --obj2;
  int c = obj2.display();
  if(a == c){
    qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : %s %d.***", __func__, __LINE__);
    return SUCCESS;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : %s %d.***", __func__, __LINE__);
    return FAILED;
  }
}// end of main
