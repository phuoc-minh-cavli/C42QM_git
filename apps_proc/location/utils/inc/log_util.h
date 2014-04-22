#ifndef LOC_UTIL_H
#define LOC_UTIL_H

#include "loc_util_log.h"

/*=============================================================================
 *
 *                         LOC LOGGER TYPE DECLARATION
 *
 *============================================================================*/
/* LOC LOGGER */
typedef struct loc_logger_s
{
  unsigned long  DEBUG_LEVEL;
  unsigned long  TIMESTAMP;
} loc_logger_s_type;

extern void loc_logger_init(unsigned long debug, unsigned long timestamp);

/*=============================================================================
 *
 *                               EXTERNAL DATA
 *
 *============================================================================*/
extern loc_logger_s_type loc_logger;

#endif //LOC_UTIL_H
