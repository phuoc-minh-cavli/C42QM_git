#ifndef ERRLOG_H
#define ERRLOG_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  E R R

DESCRIPTION
  This header file contains the definitions of log structure for core dump
 
Copyright (c) 2007-2015,2018 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/err/src/errlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/25/07   tbg     Initial revision (pulled code from rexlog.h)

===========================================================================*/


/* -----------------------------------------------------------------------
** Includes 
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "errlog_armv7.h"
#include "time_svc.h"
#include "qurt.h"

#include "DDITimetick.h"
#include "err.h"



/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

typedef enum
{
  ERR_ARCH_UNKNOWN = 0,
  ERR_ARCH_ARM,
  ERR_ARCH_QDSP6,
  SIZEOF_ERR_ARCH_TYPE
} err_arch_type;

typedef enum
{
  ERR_OS_UNKNOWN = 0,
  ERR_OS_REX,
  ERR_OS_L4, 
  ERR_OS_BLAST, 
  ERR_OS_QURT, 
  SIZEOF_ERR_OS_TYPE
} err_os_type;


typedef struct
{
  err_arch_type type;
  uint32 version;
  union arch_coredump_union regs;
} arch_coredump_type;

/* update this version whenever ARM arch_coredump_type changes */
#define ERR_ARCH_COREDUMP_VER  1
#define ERR_ARCH_COREDUMP_TYPE ERR_ARCH_ARM
#define ERR_ARCH_ARM_INUSE

/************************************************************************
 *                        OS_COREDUMP_TYPES
 ************************************************************************/
/****************
 *     L4
 ****************/
#define ERR_OS_TCB_TYPE qurt_thread_t

typedef struct
{
  err_os_type type;
  uint32 version;  
  ERR_OS_TCB_TYPE thread_id; 
} os_coredump_type;

/* update this version whenever L4 os_coredump_type changes */
#define ERR_OS_COREDUMP_VER  1
#define ERR_OS_COREDUMP_TYPE ERR_OS_L4
#define ERR_OS_L4_INUSE

/************************************************************************
 *                         ERR_COREDUMP_TYPE
 ************************************************************************/

#define ERR_LOG_MAX_MSG_LEN 80
#define ERR_LOG_MAX_FILE_LEN 50 /* Q6 compiler includes relative path */
/* Needs to match number of msg parameters defined in ERR_FATAL macro */
#define ERR_LOG_NUM_PARAMS 3 
#define ERR_IMAGE_VERSION_SIZE  128
#define ERR_QURT_TASK_NAME_LEN  QURT_THREAD_ATTR_NAME_MAXLEN


#ifndef ERR_MAX_PREFLUSH_CB
#define ERR_MAX_PREFLUSH_CB 10
#endif /* ERR_MAX_PREFLUSH_CB */

typedef struct
{
  err_cb_ptr    err_cb;
  boolean       cb_start;
  DalTimetickTime64Type cb_start_tick;
} err_cb_preflush_external_type;

typedef struct
{
  char qc_image_version_string[ERR_IMAGE_VERSION_SIZE];
  char image_variant_string[ERR_IMAGE_VERSION_SIZE];
  char oem_image_version_string[ERR_IMAGE_VERSION_SIZE];
  char oem_image_uuid_string[ERR_IMAGE_VERSION_SIZE];
} image_coredump_type;

typedef struct
{
  uint32 version;
  uint32 linenum;
  uint64                        timestamp;
  uint64                        uptime;
  DalTimetickTime64Type         err_handler_start_time;
  DalTimetickTime64Type         err_handler_end_time;
  char                          filename[ERR_LOG_MAX_FILE_LEN];
  char                          message[ERR_LOG_MAX_MSG_LEN];
  uint32                        param[ERR_LOG_NUM_PARAMS];
  char                          aux_msg[ERR_LOG_MAX_MSG_LEN];
  char                          int_msg[ERR_LOG_MAX_MSG_LEN];
  err_cb_preflush_external_type err_next_to_STM;
  err_cb_preflush_external_type err_preflush_external[ERR_MAX_PREFLUSH_CB+1];
  err_cb_ptr                    err_current_cb;
  const err_const_type *        compressed_ptr;
} err_coredump_type;

/* update this version whenever err_coredump_type changes */
#define ERR_COREDUMP_VER  4


/************************************************************************
 *                           COREDUMP_TYPE
 ************************************************************************/


typedef struct
{
  uint32 version;
  arch_coredump_type arch;
  os_coredump_type os;
  err_coredump_type err;
  image_coredump_type image;
} coredump_type;

 /* update this version whenever coredump_type changes */
#define ERR_COREDUMP_VERSION  1
 
/************************************************************************
 *                           MESSAGE TYPES
 ************************************************************************/

#define ERR_LOG_MSG(str) MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH, str);
#define ERR_LOG_MSG_ERROR(str) MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR, str);
#define ERR_LOG_MSG_FATAL_1(str, x) MSG_1(MSG_SSID_TMS, MSG_LEGACY_FATAL, str, x);
 
/*===========================================================================
FUNCTION ERR_LOG_INIT

DESCRIPTION
  Initialize buffer to be used for writing log information.
  Calling the function subsequent times will reset the location and size
  of the buffer.

PARAMETERS
  char *buff     Pointer to buffer
  uint32 size    Size of buffer

RETURNS
 
============================================================================*/
void err_log_init(char *buff, uint32 size);

#endif /* ERRLOG_H */
