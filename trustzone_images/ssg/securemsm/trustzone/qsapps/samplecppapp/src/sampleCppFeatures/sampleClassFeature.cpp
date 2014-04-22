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

/* This test demostrates a simple class with implicit constructor and another class with explicit constructor and uses of "this" pointer. */

/* An example of Class with Implicite Constructor. */
class first
{
  private:
    int data1;
    int data2;
    string data3;

  public:
    /*Example of a method declaration whose body is inside the class itself.*/
    void inputIntegerData(int d1, int d2);

    /*Example of a method declaration whose body is inside the class itself.*/
    string inputStringData (){
      data3 =  "I am String Data.";
      return data3;
    }
    /*At this point, we are not sure whether we should write a float data or not?*/

};


/* The function definition can also be inside the class. */
void first :: inputIntegerData(int d1, int d2)
{
  data1 = d1; // One way to assign the value to member variable of the class.
  this->data2 = d2; // Another way to assignn the value to member variable , i.e. using this pointer.
  qsee_log(QSEE_LOG_MSG_ERROR, "Integer data value assigned to the data1 is : %d, data2 is : %d\n", data1, data2);
}

int test_classFeatures()
{
  first o1;
  o1.inputIntegerData(7, 9);
  string s = o1.inputStringData();
  qsee_log(QSEE_LOG_MSG_ERROR, "String data value assigned to the data2 is : %s\n", s.c_str());
  return SUCCESS;
}
