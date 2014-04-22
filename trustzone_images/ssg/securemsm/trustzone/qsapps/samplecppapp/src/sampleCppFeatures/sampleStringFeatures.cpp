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


/* This test demonstrates Basic CPP string specific Features. */

int comp_with_compare(string s1, string s2)
{
  int status = s1.compare(s2);
  if(status != 0)
    qsee_log(QSEE_LOG_MSG_ERROR, "%s is not equal to %s\n", s1.c_str(), s2.c_str());
  else
    qsee_log(QSEE_LOG_MSG_ERROR, "%s is equal to %s\n", s1.c_str(), s2.c_str());
  return status;
}


int test_stringFeatures()
{
  /** Basic String declartion Specific Testing ****/ 
  // Initializing 1st string
  string str = "Basic Cpp String Declared.";
  int status = 0;

  // Declaring character array
  char ch[80];

  // using copy() to copy elements into char array
  str.copy(ch,9,0);
  ch[10] = '\0';

  // Diplaying char array
  qsee_log(QSEE_LOG_MSG_ERROR, "The new copied character array is : ");
  qsee_log(QSEE_LOG_MSG_ERROR, "%s\n",ch);
  qsee_log(QSEE_LOG_MSG_ERROR, "And the string is: %s\n", str.c_str());

  /***testing String comparison with Relational operator and compare function***/

  /*** Negative Test ***/
  string str1("First String");
  string str2("Second String");
  //This Should print strings are not equal.
  status = comp_with_compare(str1, str2);
  /*It is expected that the status will be non zero value.*/
  if(status == 0){
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : cpp_string_feature_test.***");
    return 1;
  }
  /*** Positive Test ***/
  string str3("Third String");
  string str4("Third String");
  /*This should print strings are equal.*/
  status = comp_with_compare(str3, str4);
  /*It is expected that status should be a zero Value.*/
  if(status != 0){
    qsee_log(QSEE_LOG_MSG_ERROR, "****FAILED : %s %d.***", __func__, __LINE__);
    return FAILED;
  }
  else{
    qsee_log(QSEE_LOG_MSG_ERROR, "****PASSED : %s %d.***", __func__, __LINE__);
    return SUCCESS;
  }
}
