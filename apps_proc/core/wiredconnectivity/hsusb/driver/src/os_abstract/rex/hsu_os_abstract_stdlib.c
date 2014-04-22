/*==============================================================================

                 H S U _ O S _ A B S T R A C T _ S T D L I B . C


GENERAL DESCRIPTION
  High Speed USB stack REX OS abstraction layer standard library 
  API implementation.

EXTERNALIZED FUNCTIONS
  jos_memcmp
    Rex porting of the memcmp function.

  jos_memcpy
    Rex porting of the memcpy function.

  jos_memset
    Rex porting of the memset function.

  jos_strlen
    Rex porting of the strlen function.

  jos_strlcpy
    String copy.

  jos_strcmp
    Rex porting of the strcmp function.

  jos_snprintf
    Rex porting of the snprintf function.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/os_abstract/rex/hsu_os_abstract_stdlib.c#1 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/10/06  ds      Initial version

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES

==============================================================================*/

/* C runtime library include files. */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stringl/stringl.h>

/* AMSS include files. */
#include <comdef.h>

/* OS abstraction layer include files. */
#include <port_generic_int.h>

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================
  Externalized Variables
==============================================================================*/

/*==============================================================================
  File Static Variables
==============================================================================*/

/*==============================================================================

                    FUNCTION DEFINITIONS

==============================================================================*/

/* ==========================================================================
FUNCTION JOS_MEMCMP

DESCRIPTION
  Rex porting of the memcmp function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library memcmp function.

SIDE EFFECTS
  None.
============================================================================= */
// +++ HSU ADDITION
// Remove uncovered functions
//jint_t jos_memcmp(const void *s1, const void *s2, juint32_t n)
//{
//  return memcmp(s1, s2, n);
//} /* jos_memcmp */
// --- HSU ADDITION


/* ==========================================================================
FUNCTION jos_memcpy

DESCRIPTION
  Rex porting of the memcpy function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library memcpy function.

SIDE EFFECTS
  None.
============================================================================= */
void* jos_memcpy(void *out, const void *in, juint32_t n)
{
#if defined(FEATURE_HSU_EBI) || defined(BUILD_HOSTDL) || defined(BUILD_EHOSTDL)
  /* workaround for EBI issue, HOSTDL/EHOSTDL 4 byte alignment issue */
  juint32_t i;
  
  for (i=0; i<n; ++i)
  {
    *((char*)out+i) = *((char*)in+i);
  }

  return out;
#else
  return (void*)memscpy(out, n, in, n);
#endif /* FEATURE_HSU_EBI || BUILD_HOSTDL || BUILD_EHOSTDL */
} /* jos_memcpy */


/* ==========================================================================
FUNCTION jos_memset

DESCRIPTION
  Rex porting of the memset function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library memset function.

SIDE EFFECTS
  None.
============================================================================= */
void* jos_memset(void *b, jint_t c, juint32_t len)
{
  return (void*)memset(b, c, len);
} /* jos_memset */


/* ==========================================================================
FUNCTION jos_strlen

DESCRIPTION
  Rex porting of the strlen function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library strlen function.

SIDE EFFECTS
  None.
============================================================================= */
juint32_t jos_strlen(const char *str)
{
  return (juint32_t)strlen(str);
} /* jos_strlen */


/* ==========================================================================
FUNCTION jos_strcmp

DESCRIPTION
   Rex porting of the strcmp function..

DEPENDENCIES
   None.

RETURN VALUE
   Same as the standard library strcmp function.

SIDE EFFECTS
   None.
============================================================================= */
// +++ HSU ADDITION
// Remove uncovered functions
//jint_t jos_strcmp(const char *s1, const char *s2)
//{
//  return strcmp(s1, s2);
//} /* jos_strcmp */
// --- HSU ADDITION


/* ==========================================================================
FUNCTION jos_strncmp

DESCRIPTION
  Rex porting of the strncmp function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library strncmp function.

SIDE EFFECTS
  None.
============================================================================= */
// +++ HSU ADDITION
// Remove uncovered functions
//jint_t jos_strncmp(const char *s1, const char *s2, const juint32_t n)
//{
//  return strncmp(s1, s2, n);
//} /* jos_strncmp */
// --- HSU ADDITION


/* ==========================================================================
FUNCTION JOS_STRLCPY

DESCRIPTION
  A variant of the strncpy function which guarantees
  NULL termination of the destination string.

  Copies a specified number of characters from a string.

  Guarantees null-termination of the destination string when its
  given size is not zero.

  When the size of the destination string is larger than the source string,
  the function does not zero-fill the remaining characters of the destination
  string (apart from the null-terminator character).


DEPENDENCIES
  None.

RETURN VALUE
  Returns the length of the source string.

SIDE EFFECTS
  None.
============================================================================= */
// +++ HSU ADDITION
// Remove uncovered functions
//juint32_t jos_strlcpy(char *dst, const char *src, juint32_t n)
//{
//  /*lint -e{732} 
//  ** jos_strlcpy gets n as an unsigned int, while strlcpy 
//  ** gets it as a signed int. Lint yells about this. 
//  */
//  return strlcpy(dst, src, (int)n);
//
//} /* jos_strlcpy */
// --- HSU ADDITION


/* ==========================================================================
FUNCTION JOS_SNPRINTF

DESCRIPTION
  Rex porting of the snprintf function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library snprintf function.

SIDE EFFECTS
  None.
============================================================================= */
jint32_t jos_snprintf(char *buf, juint32_t size, const char *fmt, ...)
{
  int32       string_length;
  jint32_t  return_value;
  va_list   arg_list;

  #ifdef T_REXNT
  buf[size - 1] = '\0';
  #endif

  /*lint -e{530}
  ** The ARM implementation of va_start raises a Lint error.
  */
  va_start(arg_list, fmt);

  string_length = vsnprintf(buf, size, fmt, arg_list);
  va_end(arg_list);

  #ifndef T_REXNT

  /* AMSS target. */

  if (string_length < 0)
  {
    /* Output error. */
    return_value = -1;
  }
  else if (string_length >= (int32)size)
  {
    /* The number of bytes required to store the string
    ** (including the NULL terminator) exceeds 'size' bytes.
    */
    return_value = -1;
  }
  else
  {
    return_value = string_length;
  }

  #else  /* #ifndef T_REXNT */

  /* Windows target. */

  if (string_length < 0)
  {
    /* The result string is too long, or an error 
    ** has occurred.
    */

    if (buf[size - 1] != '\0')
    {
      /* The string size is greater than 'size' bytes.
      ** Replace the last stored character with a NULL terminator, since
      ** Windows '_vsnprintf' function does not store a NULL terminator 
      ** when the resulting string is too long.
      */
      buf[size - 1] = '\0';
    }
  }

  return_value = string_length;

  #endif /* #ifndef T_REXNT */

  return return_value;
} /* jos_snprintf */
