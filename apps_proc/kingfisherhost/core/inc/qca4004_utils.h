/*****************************************************************************
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/

#ifndef __QCA4004_UTILS_H__
#define __QCA4004_UTILS_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/
#include "qurt.h"
#include "qapi_types.h"


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function create and start a timer.

   @param[in] timeout is Timer duration (ms).
   @param[in] cbfunc  Pointer to the timer callback function.
   @param[in] cbfunc  Pointer to the timer callback context.

   @return
    - 0 if timer create or start fail.
    - else the timer which created.
*/
qurt_timer_t start_qca4004_timer(uint32_t timeout, qurt_timer_callback_func_t cbfunc, void *cbdata);

/**
   @brief This function stop and delete a timer.

   @param[in] timer is Pointer to the timer which will be stop and delete.

   @return
*/
void stop_qca4004_timer(qurt_timer_t *timer);

/**
   @brief This function convert type uint32_t to type char s[].

   @param[in] 		n is the value of type uint32_t which to be convert.
   @param[inout] 	s is the value of type char * which to be convert to.
   @return
*/
void qca4004_uxtoa(uint32_t n, char s[]);

/**
   @brief This function convert type char s[] to type uint32_t.

   @param[in] 	s is the value of type char * which to be convert.
   @return	the value of type uint32_t which to be convert to.
*/
uint32_t qca4004_atoux(char s[]);

/**
   @brief This function compare two string with length, ignore case.

   @param[in] 	str1 is one of string which to be compare.
   @param[in] 	str2 is one of string which to be compare.
   @param[in] 	length is the length which to be compare.
   @return	
    - 0 if same
    - else different.   
*/
int qca4004_strncmpi(char *str1, char *str2, int length);

/**
   @brief This function for the general byte order swapping

   @param[in]   x is the value of type uint32_t which to be swap.
   @return      the value of type uint32_t which to be swap to.
*/
uint32_t qca4004_swap32(uint32_t x);
#endif

