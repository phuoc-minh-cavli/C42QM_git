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

/* This test demonstrates the encapsulation Features. */
class account
{
  public :
    //constructor
    account()
    {
      total_amount =0;
    }
  
    account (int amount =0)
    {
      total_amount = amount;
    }
  
    // interface to outside world
    void depositeAmount(int amount)
    {
      total_amount += amount;
    }
  
    //interface to outside World
    int get_totalAmount()
    {
      return total_amount;
    }
  
    // Interface to Outside World
    void withdrawAmount(int amount)
    {
      if(total_amount < amount)
        qsee_log(QSEE_LOG_MSG_ERROR, "total amount is less than reuqested Amount. So withdrwal is possible.");
      else
        total_amount -= amount;
    }
  
  private:
    int total_amount;
};

int test_encapsulationFeature()
{
  account A (200);
  qsee_log(QSEE_LOG_MSG_ERROR, "total amount is : %d\n", A.get_totalAmount());
  int a = A.get_totalAmount();
  qsee_log(QSEE_LOG_MSG_ERROR, "a is : %d\n", a);
  A.depositeAmount(200);
  qsee_log(QSEE_LOG_MSG_ERROR, "total amount is : %d\n", A.get_totalAmount());
  A.withdrawAmount(200);
  qsee_log(QSEE_LOG_MSG_ERROR, "total amount is : %d\n", A.get_totalAmount());
  if(a != A.get_totalAmount()){
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : %s %d.***", __func__, __LINE__);
    return FAILED;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : %s %d.***", __func__, __LINE__);
    return SUCCESS;
  }
}
