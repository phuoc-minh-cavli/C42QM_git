/*===========================================================================
                      EDIT HISTORY FOR MODULE

This section contains comments describing changes made to this module.
Notice that changes are listed in reverse chronological order.


when        who        what, where, why
--------    ---        ------------------------------------------------------
04/20/2010 asheeshk    initial draft

============================================================================*/


/*===========================================================================
 Include Files
============================================================================*/
#include "csd_diag_service_msg.h"
#include <stdlib.h>

/* =======================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* =======================================================================
**                        Class & Function Definations
** ======================================================================= */

#define CSD_MAX_DEBUG_BUFFER  512
#define CSD_MAX_FORMAT_BUFFER 256

/**
 * Initializes the Diag Interface
 *
 * @return 0 value on success else failure
 */
int csd_debug_initialize()
{
  int nResult = 1;
  return nResult;
}

/**
 * De-Initializes the Diag Interface
 *
 * @return 0 value on success else failure
 */
int csd_debug_deinitialize()
{
  int nResult = 1;
  return nResult;
}

/* // commented asheeshk: this should be only enabled in win32
void CSD_OutputDebugMessage
(
  const char *file,
  int line,
  const char* format,
  ...
)
{
  const char *_file = 0;
  char buf[CSD_MAX_DEBUG_BUFFER];
  va_list arglist;

  _file = strrchr( file, '\\' );
  if(_file == NULL)
  {
    _file = strrchr( file, '/' );
  } // in case of linux, use forward slash
  if(_file == NULL)
  { // if still null, print error msg, page fault about to ensue
    _file = file;
  }
  else
  {
    _file++;
  } // Just past the last '\'
  sprintf_s( buf, CSD_MAX_DEBUG_BUFFER, "%s [%Id] ", _file, line );

  va_start(arglist, format);
  vsprintf_s(buf + strlen(buf), CSD_MAX_DEBUG_BUFFER - strlen(buf), format, arglist);
  strncat_s( buf, CSD_MAX_DEBUG_BUFFER, "\n", (CSD_MAX_DEBUG_BUFFER - strlen(buf)) - 1);
  OutputDebugString(buf);
  //Also redirecting to cmd window
  fprintf(stderr, buf);
 }

 */ // commented asheeshk



