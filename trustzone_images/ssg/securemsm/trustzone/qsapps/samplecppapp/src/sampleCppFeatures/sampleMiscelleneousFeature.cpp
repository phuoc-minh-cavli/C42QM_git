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

/* This contains test for friend , and smart pointers.
 * Other advanced CPP features can be added in this file overtime.
 */

class container 
{
  private :
    int data;

  public:
    container()
    {
      data = 0;
    }

    void add(int n)
    {
      data += n;
    }

    int get_data()
    {
      return data;
    }

    ~container()
    {
      data = 0;
    }

    friend class smartpointer;
};

class smartpointer
{
  private :
    container *cptr;

  public:
    smartpointer()
    {
      cptr = NULL;
    }

    smartpointer(container *t)
    {
      cptr = t;
    }

    ~smartpointer()
    {
      cptr = NULL;
    }

    int operator *()
    {
      return cptr->data;
    }
};


int test_cppMiscelleneousFeature()
{
  container c1;
  int total_sum1 = 0, total_sum2 = 0;

  c1.add(10);
  c1.add(20);
  c1.add(0);
  c1.add(-40);
  c1.add(10);
  c1.add(50);

  smartpointer sptr(&c1);
  total_sum1 = c1.get_data();
  total_sum2 = *sptr;

  if(total_sum1 == total_sum2){
    qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : %s %d.***", __func__, __LINE__);
    return SUCCESS;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : %s %d.***", __func__, __LINE__);
    return FAILED;
  }
}