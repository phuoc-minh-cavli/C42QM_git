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

/*This test demonstrates simple template function implementation and it's uses.*/

/*Here are two Different polymorphic Function for swapping.*/
void polymorphic_myswap(int &a, int &b)
{
  int temp_int = a;
  a = b;
  b = temp_int;
}

void polymorphic_myswap(string &s1, string &s2)
{
  string temp_string = s1;
  s1= s2;
  s2 = temp_string;
}

/* 
 * This is a single template Function for swapping.
 * We are going to use typename in this example.
 */
template <typename T>
void template_myswap(T &x, T &y)
{
  T z = x;
  x = y;
  y = z;
}

/* 
 * This is a single template Function for swapping.
 * We are going to use class in this example.
 * which also allowed in cpp.
 */

template <class T1>
void template_myswap2 (T1 &x, T1 &y)
{
  T1 z = x;
  x = y;
  y = z;
}


int test_templateFunction()
{
  int a = 10, b= 20, c = 30, d = 40;
  string s1 = "First String.";
  string s2 = "Second String.";

  /*First Use Polymorphic myswap and then Use the template Myswap*/
  qsee_log(QSEE_LOG_MSG_ERROR,"a & b is : %d %d", a ,b);
  polymorphic_myswap(a, b);
  qsee_log(QSEE_LOG_MSG_ERROR,"a & b is : %d %d", a ,b);
  template_myswap(a, b);
  qsee_log(QSEE_LOG_MSG_ERROR,"a & b is : %d %d", a ,b);
  template_myswap2(a, b);
  qsee_log(QSEE_LOG_MSG_ERROR,"a & b is : %d %d", a ,b);
  template_myswap2<>(a, b);
  qsee_log(QSEE_LOG_MSG_ERROR,"a & b is : %d %d", a ,b);
  if ((a!= 10 ) || (b!= 20)){
    qsee_log(QSEE_LOG_MSG_ERROR,"One of the call to Swap method failed");
    return 1;
  }
  else
    qsee_log(QSEE_LOG_MSG_ERROR," Call to Swap method for integers  is successful.");

  /*Again First Use Polymorphic myswap and then Use the template Myswap*/
  qsee_log(QSEE_LOG_MSG_ERROR,"s1 is : %s & s2 is : %s", s1.c_str() ,s2.c_str());
  polymorphic_myswap(s1, s2);
  qsee_log(QSEE_LOG_MSG_ERROR,"s1 is : %s & s2 is : %s", s1.c_str() ,s2.c_str());
  /*Example of template Call.*/
  template_myswap<>(s1, s2);
  qsee_log(QSEE_LOG_MSG_ERROR,"s1 is : %s & s2 is : %s", s1.c_str() ,s2.c_str());
  /*Example of implicite type paramaterizing*/
  template_myswap(s1, s2);
  qsee_log(QSEE_LOG_MSG_ERROR,"s1 is : %s & s2 is : %s", s1.c_str() ,s2.c_str());
  /*Example of Explicite type paramaterizing*/
  template_myswap<string>(s1, s2);
  qsee_log(QSEE_LOG_MSG_ERROR,"s1 is : %s & s2 is : %s", s1.c_str() ,s2.c_str());
  if ((0 != s1.compare("First String.") ) || (0 != (s2.compare("Second String.")))){
    qsee_log(QSEE_LOG_MSG_ERROR,"FAILED: %s %d.***", __func__, __LINE__);
    return FAILED;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR,"PASSED: %s %d.***", __func__, __LINE__);
    return SUCCESS;
  }
}