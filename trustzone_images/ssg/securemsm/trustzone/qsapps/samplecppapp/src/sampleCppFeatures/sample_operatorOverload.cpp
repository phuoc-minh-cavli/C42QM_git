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

/* This test demostrates sample function and operator overloading specifc Features. */

class complex
{
  int real, imag;

  public:
    complex (){}
    
    complex(int x, int y)
    {
      real = x; imag =y;
    }
    
    complex operator + (complex c)
    {
      this -> real += c.real;
      this -> imag += c.imag;
      return *this;
    }
    
    bool is_equal (complex c2)
    {
      return ((this->real == c2.real) && (this->imag == c2.imag));
    }

};


int test_operatorOverloading()
{
  //First we will test the operator overloading functionality.
  //create three complex number Such that third will have the sum of first two.
  complex c1, c2, c3;
  c1 = complex(3, 4);
  c2 = complex (5 ,8);
  c3 = complex(8, 12);
  // this check should pass as the third number is the sum of the first two Numbers.
  bool status = c3.is_equal(c1 + c2);
  if(status){
    qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED :  %s %d.***", __func__, __LINE__);
    return SUCCESS;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED :  %s %d.***", __func__, __LINE__);
    return FAILED;
  }
}