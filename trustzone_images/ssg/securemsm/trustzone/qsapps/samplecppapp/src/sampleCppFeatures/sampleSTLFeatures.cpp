/**
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
#include <string>
#include "cheaders.h"
#include "cppFeatures.h"
#include "samplecppapp.h"
#include <vector>
#include <iterator>
#include <algorithm>
#include <list>
#include <deque>
#include <set>

using namespace std;

extern int __dso_handle;

/**
  A generic conatiner addition function. 
  Should 't depend on container type, 
  Should just take iterators as argument and return the 
  Sum in the given range. 
**/
template <typename Container>

int sumContainer(Container begin, Container end) {
  //Generic container summation.
  int total_sum = 0;
  for (Container it = begin; it != end; it++) {
      total_sum += *it;
  }
  return total_sum;
}


int test_STLFeature(){

  //Data elements.
  int numbersarray[] = { 1, 2 , 3 , 7 , 8 , 9 , 10 , 6, 5, 4 };
  
  //Create containers.
  vector<int> myvector(numbersarray, numbersarray+10);
  list<int> mylist(numbersarray, numbersarray+10);
  set<int> myset(numbersarray, numbersarray+10);
  deque<int> mydq(numbersarray, numbersarray+10);

  int vector_sum0 =0, vector_sum =0, vector_sum1 =0;
  int list_sum =0, list_sum1 =0;
  int set_sum =0, set_sum1= 0;
  int dq_sum0 =0, dq_sum =0, dq_sum1 =0;

  for(int i=0; i< myvector.size(); i++)
    vector_sum0 += myvector[i];

  for(vector<int>:: iterator it = myvector.begin(); 
      it != myvector.end(); ++it)
      vector_sum += *it;

  for(list<int>:: iterator it = mylist.begin(); 
      it != mylist.end(); ++it)
      list_sum += *it;

  for(set<int>:: iterator it = myset.begin(); 
      it != myset.end(); ++it)
      set_sum += *it;

  for(int i=0; i< mydq.size(); i++)
    dq_sum0 += mydq[i];

  for(deque<int>:: iterator it = mydq.begin(); 
      it != mydq.end(); ++it)
      dq_sum += *it;

  //A generic addition inetrface for all the containers.
  vector_sum1 = sumContainer(myvector.begin(), myvector.end());
  list_sum1 = sumContainer(mylist.begin(), mylist.end());
  set_sum1 = sumContainer(myset.begin(), myset.end());
  dq_sum1 = sumContainer(mydq.begin(), mydq.end());


  /*Sum of all the container elements should be same as the same array have been used for initializing them.*/
  qsee_log(QSEE_LOG_MSG_ERROR,"vector sum0 is : %d and vecotr sum is : %d \n and vecotr sum1 is : %d",
            vector_sum0, vector_sum, vector_sum1 );
  /*qsee_log(QSEE_LOG_MSG_ERROR,"list sum1 is : %d \n", list_sum1);
  qsee_log(QSEE_LOG_MSG_ERROR,"Set sum1 is : %d \n", set_sum1);
  qsee_log(QSEE_LOG_MSG_ERROR,"dq sum is : %d and dq sum1 is : %d \n", dq_sum, dq_sum1);*/

  if((vector_sum != vector_sum0) || (vector_sum != vector_sum1) || (vector_sum0 != dq_sum0) || 
     (vector_sum0 != dq_sum) || (vector_sum0 != dq_sum1) || (vector_sum != list_sum) ||
     (vector_sum != list_sum1) ||(vector_sum != set_sum) || (vector_sum != set_sum1)){
      qsee_log(QSEE_LOG_MSG_ERROR,"Summing over some container failed.\n");
      return FAILED;
  }
  else{
      qsee_log(QSEE_LOG_MSG_ERROR,"Summing over containers completed Successfully.\n");
      return SUCCESS;
  }
}