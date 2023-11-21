#ifndef QSEE_COMSTR_H
#define QSEE_COMSTR_H
/**
@file qsee_comstr.h
@brief Provide string manipulation API wrappers
*/

/*===========================================================================
  Copyright (c) 2012-2015 QUALCOMM Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.1.3/api/securemsm/trustzone/qsee/qsee_comstr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/23/15    cz     Fixed compiler warnings in TZ.4.0
--------   ---     --------------------------------------------------------
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifndef WCHAR
#if defined (_MSC_VER)
typedef unsigned __int16        WCHAR;
#elif defined (__GN23UC__)
typedef unsigned int16_t        WCHAR;
#else
typedef unsigned short          WCHAR;
#endif
#endif

#ifndef NULL
  #define NULL  0
#endif

/**
  @addtogroup qtee_comstr
  @{
*/

/**
 * @brief  Compare two substrings without case sensitivity
 *
 * @param[in] s1     The first input string
 * @param[in] s2     The second input string
 * @param[in] n      The length to compare
 *
 * @return Returns 0 (zero) if s1 is identical to s2.  Returns a number > 0 if s1 > s2.  Returns a number < 0 if s1 < s2.
 */
int qsee_strnicmp (const char* s1, const char* s2, unsigned int n);

/**
 * @brief  Copies a string into a destination buffer and guarantees that it is NULL-terminated.
 *
 * @param[out] pcDst       Destination string to copy into.
 * @param[in] pszSrc        Source string to copy from.
 * @param[in] nDestSize     Size of the destination buffer in bytes.
 *
 * @return Length of the full string copied, if successful.
 */
unsigned int qsee_strlcpy(char* pcDst, const char* pszSrc, unsigned int nDestSize);

/**
 * @brief  Concatenates a string into a destination buffer and guarantees that it is NULL-terminated.
 *
 * @param[out] pcDst        Destination string to copy into.
 * @param[in] pszSrc        Source string to copy from.
 * @param[in] nDestSize     Size of destination buffer (in bytes).
 *
 * @return If successful, the copied, full string length.
 */
unsigned int qsee_strlcat(char* pcDst, const char* pszSrc, unsigned int nDestSize);

/**
 * @brief  Returns length of the specified wide string.
 *
 * @param[in] s         Input wide string.
 *
 * @return Length of input string without trailing nulls.
 */
int qsee_wstrlen( const WCHAR *s);

/**
 * @brief  Copies a wide string into a destination buffer and guarantees that it is NULL-terminated.
 *
 * @param[out] pwszDst      Destination string to copy into.
 * @param[in] cpwszSrc      Source string to copy from.
 * @param[in] nDestSize     Size of the destination buffer in bytes.
 *
 * @return If successful, the copied, full string length.
 */
int qsee_wstrlcpy(WCHAR* pwszDst, const WCHAR* cpwszSrc, long unsigned int nDestSize);

/**
 * @brief  Concatenates a string into a destination buffer and guarantees that it is NULL-terminated.
 *
 * @param[out] pcDst        Destination string to copy into.
 * @param[in] pszSrc        Source string to copy from.
 * @param[in] nDestSize     Size of the destination buffer in bytes.
 *
 * @return If successful, the copied, full string length.
 */
int qsee_wstrlcat(WCHAR* pcDst, const WCHAR* pszSrc, long unsigned int nDestSize);

/**
 * @brief  Converts a string of single-byte characters into a wide string.
 *
 * @param[in] pszIn      Pointer to a NULL-terminated string comprised of single-byte characters.
 * @param[out] pDest     Pointer to a destination buffer to receive wide string.
 * @param[in] nSize      Size (in bytes) of pDest buffer. \n
                         If nSize = 0, there is no conversion and pDest is unchanged. \n
                         Negative nSize values are invalid.
 *
 * @return Returns destination string of wide characters.
 */
WCHAR *qsee_strtowstr(const char *pszIn, WCHAR *pDest, int nSize);

/**
 * @brief  Converts a string of wide characters into a single-byte string.
 *
 * @param[in] cpwszIn     Pointer to a NULL-terminated wide string that must be converted to a single-byte character string.
 * @param[out] pDest      Pointer to a destination buffer to receive the single-byte string.
 * @param[in] nSize       Size (in bytes) of pDest buffer. \n
                          If this is 0, there is no conversion and pDest is unchanged. \n
                          Negative nSize values are invalid.
 *
 * @return Returns destination string of single-byte characters.
 */
char * qsee_wstrtostr(const WCHAR *cpwszIn, char *pDest, int nSize);

/**
 * @brief  Finds a specified wide character in a wide string.
 *
 * @param[in] pText       NULL-terminated wide string to search.
 * @param[in] ch          Character to locate.
 *
 * @return Returns a pointer to the first occurrence of c in s1, or NULL if c is not found.
 */
WCHAR *qsee_wstrchr(const WCHAR *pText, WCHAR ch);

/**
 * @brief  Compares two wide strings, s1 and s2. \n
 *         Returns an integer value that shows comparison result.
 *
 * @param[in] s1         Pointer to first NULL-terminated string.
 * @param[in] s2         Pointer to second NULL-terminated string.
 *
 * @return Returns 0 if s1 is identical to s2. \n
 *         Returns a number > 0 if s1 > s2. \n
 *         Returns a number < 0 if s1 < s2.
 */
int qsee_wstrcmp(const WCHAR *s1,  const WCHAR *s2);

/** @} */

#endif /*QSEE_COMSTR_H*/


