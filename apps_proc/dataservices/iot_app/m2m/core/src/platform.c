/******************************************************************************

  @file    platform.c

  ---------------------------------------------------------------------------
  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------

******************************************************************************/

/*******************************************************************************
 *
 * Copyright (c) 2013, 2014, 2015 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *******************************************************************************/

#include <liblwm2m.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "time_svc.h"

#ifndef LWM2M_MEMORY_TRACE

/**
 * @fn void * lwm2m_malloc()
 * @brief This function is used allocate memory for given size
 *
 * @param s size to be allocated
 *
 * @return    void pointer to allocated memory
 *        
 */
void * lwm2m_malloc(size_t s)
{
  return malloc(s);
}

/**
 * @fn void * lwm2m_calloc()
 * @brief This function is used allocate array of memory of c members with each
 * member of given size. The memory is initialized to zero.
 *
 * @param s size to be allocated
 * @param c count of chunks of size s
 *
 * @return    void pointer to allocated memory
 *        
 */
void * lwm2m_calloc(size_t c, size_t s)
{
  return calloc(c, s);
}

/**
 * @fn void lwm2m_free()
 * @brief This function is used free the given pointer
 *
 * @param p pointer to be free
 *
 * @return  void 
 *        
 */
void lwm2m_free(void * p)
{
  if(p) 
  {
    free(p);
    p = NULL;
  }
}

/**
 * @fn char * lwm2m_strdup()
 * @brief This function is used to allocate memory and copy the given string
 *
 * @param str pointer to the string to be copied
 *
 * @return  pointer to newly allocated memory
 *        
 */
char * lwm2m_strdup(const char * str)
{
  char * ret = NULL;
  int str_len = 0;

  if(NULL == str) return ret;
  str_len = strlen(str);
  if(str_len <= 0) return ret;

  ret = lwm2m_malloc(str_len + 1);
  if (ret != NULL)
  {
    memset(ret, 0 , str_len + 1);
    LWM2M_STRCPY(ret, str, str_len + 1);
  }
  return ret;
}

#endif

/**
 * @fn int lwm2m_strncmp()
 * @brief This function is used to compare two strings
 *
 * @param s1 pointer to the first string
 * @param s2 pointer to the second string
 *
 * @return  0 if both are equal,
 *          -1 if s1 is less than s2
 *          1 if s1 is greater tan s2
 *        
 */
int lwm2m_strncmp(const char * s1,
    const char * s2,
    size_t n)
{
  if(s1 == NULL || s2 == NULL) 
  {
    return -1;
  }
  return strncmp(s1, s2, n);
}

/**
 * @fn time_t lwm2m_gettime()
 * @brief This function is used to get the time
 *
 * @param void
 *
 * @return  time_t 
 *        
 */
time_t lwm2m_gettime(void)
{
  return time_get_secs();
}

#define MAX_DIAG_LOG_MSG_SIZE       512
/**
 * @fn void lwm2m_printf()
 * @brief This function is used to print the given string
 *
 * @param format    pointer to the string to be printed
 *
 * @return  void
 *        
 */
void lwm2m_printf(const char * format, ...)
{
  char buf_ptr[MAX_DIAG_LOG_MSG_SIZE];
  va_list ap;
  
  va_start(ap, format);
  vsnprintf( buf_ptr, (size_t)MAX_DIAG_LOG_MSG_SIZE, format, ap );
  va_end( ap );

  QAPI_MSG_SPRINTF( MSG_SSID_LINUX_DATA, MSG_LEGACY_HIGH, "%s", buf_ptr);

}

/**
 * @fn bool lwm2m_isprint()
 * @brief This function is used to check if the given character is printable
 *        or not
 *
 * @param c     character to be varified
 *
 * @return  true if given char is printable
 *          false if given char is not printable
 *        
 */
bool lwm2m_isprint(char c)
{
  return ' ' <= c && c < '\177';
}


/**
 * @fn bool lwm2m_isspace()
 * @brief This function is used to check if the given character is space or not
 *
 * @param c     character to be varified
 *
 * @return  true if given char is space
 *          false if given char is not space
 *        
 */
bool lwm2m_isspace(char c)
{
  return c == ' ' || ('\011' <= c && c <= '\015');
}
