#ifndef ERR_H
#define ERR_H

/*===========================================================================

                    Error Handling Common Header File

Description
  The error handling services (ERR) provide user error handling services
  through an assert-like ERR_FATAL macro.  Calling this macro indicates an
  unrecoverable error. Other services are provided as defined by the APIs
  in this header file.

Copyright (c) 1992-2017, 2020 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           Edit History

$Header: //components/rel/core.tx/6.0/api/debugtools/err.h#2 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/01/15   din     Removed dead code and updated file.
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "comdef.h"


/* DEPRECATION FLAG */
#ifndef ERR_DEPRECATED_ATTRIBUTE  
  #define ERR_DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#endif
/*
#if (defined(__ARMCC_VERSION) && (__ARMCC_VERSION > 200000)) || (defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)))
#define ERR_DEPRECATED __attribute__ ((deprecated))
#else
#define ERR_DEPRECATED
#endif
 */


#include "msg.h"


#define FEATURE_GENERIC_ERR_FATAL_MSG

#define ERR_HAS_NV_LOG_PACKET /* flag to indicate that ERR code will register 
                                 the NV log read and clear packets. Definitions
								 for these packets are given below. */

/* -------------------------------------------------------------------------
   Definitions and Declarations
   ------------------------------------------------------------------------- */

/* Type used for post-crash callbacks from error handler */
typedef void (*err_cb_ptr)(void);

boolean err_crash_cb_dereg(err_cb_ptr cb);
boolean err_crash_cb_register(err_cb_ptr cb);

#define ERR_FILE_NAME_LEN 8

typedef enum
{
  ERR_RESET = 0,
  ERR_DLOAD,
  ERR_NO_ACTION,
  SIZEOF_ERR_ACTION
} err_action_type;

typedef struct
{
  byte address;         /* Storage address 0 to ERR_MAX_LOG-1 */
  byte err_count;       /* Number of occurances (0=empty,FF=full) */
  byte file_name[ERR_FILE_NAME_LEN];

  word line_num;
  boolean fatal;
}
err_log_type;

/* Status returned from err_get_log and err_clr_log. */

typedef enum
{
  ERR_DONE_S,           /* Request completed OK */
  ERR_NONE_S            /* No such log number */
}
err_stat_type;

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
extern const char err_generic_msg[];
#endif


/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */




/*===========================================================================

FUNCTION ERR_INIT

DESCRIPTION
  This function checks if NV has been built.  If so then it loads up the
  error log cache from NV, it initializes error counts and local variables,
  and it sets error services to online.  If NV has not been built then
  error log is not loaded and error services remain offline and error
  logging does not take place.  Any access to NV is performed here directly,
  without relying on the NV task.  Thus error service can be initialized
  and used early in the life of the DMSS and right after boot.  Note
  that if NV has not been built, error services provide no error logging
  capabilities.  However NV is built once, after initial factory startup,
  and rarely thereafter.  Thus except for the first ever powerup of the
  unit after NV build or re-build, error logging will take place as usual.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Error log is loaded from NVM into RAM resident cache.

===========================================================================*/
  void err_init (void);

/*===========================================================================

FUNCTION ERR_GET_LOG

DESCRIPTION
  Copy to the caller's pointer a specific error record in the error
  log cache.  The error record is identified by its number, from 0 to
  NV_MAX_ERR_LOG.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error log existed
  ERR_NONE_S    - No such log number (out of range).

SIDE EFFECTS
  None.

===========================================================================*/
  
/* A dependency in uismenu.c requires this workaround. */
#define err_get_log(a, b) err_get_log_ex (a, (err_log_type *) b)
err_stat_type err_get_log_ex  (byte log_num, err_log_type * log_ptr);


/*===========================================================================

FUNCTION ERR_CLR_LOG

DESCRIPTION
  This function clears a specific error record both in cache and in NV.
  An error record is cleared simply by clearing the error count field
  in it.
  
  If 'log_num' == 0xFF, all logs will be cleared.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place and ERR_NONE_S completion status is returned.

RETURN VALUE
  ERR_DONE_S    - Requested error record existed and cleared.
  ERR_NONE_S    - Error record out of range or clear failed.

SIDE EFFECTS
  The specified error record is cleared in NV and in cache and is now
  available to accept a new unique error.

===========================================================================*/
  err_stat_type err_clr_log (byte log_num);

/*===========================================================================

FUNCTION ERR_PUT_LOG

DESCRIPTION
  This function is invoked from the ERR(...) macro.  It then calls
  other local functions to store the error.

DEPENDENCIES
  err_init must be called before this call, otherwise no operation
  takes place.  This function must not be called directly; it may
  be called only from the ERR macro.

RETURN VALUE
  None.

SIDE EFFECTS
  Attempt will be made to write an error record to NV and cache.

===========================================================================*/
  void err_put_log (word line, const char *file_ptr);


/*=========================================================================

FUNCTION err_crash_cb_reg_next_to_STM

DESCRIPTION
  Registers a function (ptr type err_cb_ptr) to be called immediately after
  STM API call 
  Function should NOT rely on any messaging, task switching (or system calls
  that may invoke task switching), interrupts, etc.
 
  !!!These functions MUST NOT call ERR_FATAL under ANY circumstances!!!

DEPENDENCIES
  None
 
RETURN VALUE
  TRUE if function added to table successfully
  FALSE if function not added.

SIDE EFFECTS
  Only one registration of such API is supported so if its used by more than one
  clients than it will overwrite the old registered callback.
  
===========================================================================*/
boolean err_crash_cb_reg_next_to_STM(err_cb_ptr cb);

/*=========================================================================

FUNCTION err_crash_cb_dereg_next_to_STM

DESCRIPTION
 Deregisters a function from the error callback table.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if removed
  FALSE if function is not found in table

SIDE EFFECTS
  None

===========================================================================*/
boolean err_crash_cb_dereg_next_to_STM(err_cb_ptr cb);

/*===========================================================================

FUNCTION ERR_ENABLE_FS_LOG
DESCRIPTION
  Enables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_enable_fs_log (void);


/*===========================================================================

FUNCTION ERR_DISABLE_FS_LOG
DESCRIPTION
  Disables logging of errors to the filesystem. Logging of errors is enabled
  by default.

============================================================================*/
void err_disable_fs_log (void);


/*===========================================================================

FUNCTION ERR_FATAL_JETTISON_CORE
DESCRIPTION
  Used by ERR_FATAL to populates coredump structure. Do not call directly.
  If not using ERR_FATAL, call err_fatal_core_dump instead.

============================================================================*/
void err_fatal_jettison_core (unsigned int line, const char *file_name,
  const char *format, uint32 param1, uint32 param2, uint32 param3);

/*===========================================================================

FUNCTION ERR_FATAL_LOCK
DESCRIPTION
  Obtains err_fatal mutex

============================================================================*/  
void err_fatal_lock( void );

/*===========================================================================

MACRO ERR

DESCRIPTION
  Log an error to error log and update cache and return to caller.
  Error is sent to the MSG utility to be routed to DM or PC screen,
  depending on conditional compile switches defined in MSG.  If host
  is a PC then NV logging does not take place and only MSG output
  is used.

DEPENDENCIES
  If MSG_FILE is defined, ERR uses it as the filename.

RETURN VALUE
  None.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR must only be called because of
  an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effect are OK.

===========================================================================*/

#define ERR_FILE_NAME   msg_file

/* Map to MSG_ERR_LOG for memory savings.
   Equivalent to calling:
     MSG_ERROR(format, (code1), (code2), (code3));
     err_put_log ((word) __LINE__, ERR_FILE_NAME);
 */
#define  ERR(format, code1, code2, code3)          \
  MSG_ERR_LOG (3, format, code1, code2, code3,msg_file,__LINE__)



/*===========================================================================

MACRO ERR_FATAL

DESCRIPTION
  Log an error to error log, update the cache (for debug purpose only)
  and RESET THE SYSTEM.  Control DOES NOT RETURN to caller.  Error is
  sent to the MSG utility to be routed to DM or PC screen, depending
  on conditional compile switches defined in MSG.  If host is a PC
  then NV logging does not take place.  Instead a MSG output is made
  and the program exits (simulating hardware reset of sorts).

DEPENDENCIES
  If MSG_FILE is defined, ERR_FATAL uses it as the filename.

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  When the host is target hardware a call is generated to err_put_log
  to queue log request to NV task.  As a result unexpected execution
  times are likely to take place.  ERR_FATAL must only be called because
  of an error condition (never as an information announcement -- use MSG
  for that) therefore these timing side effects are OK.

===========================================================================*/


/* Prototypes register core dump function */
extern void jettison_core( void );

#define ERR_FATAL_BYPASS_COREDUMP(line, file, msg)                         \
    do {                                                                   \
      err_fatal_lock();                                                    \
      err_fatal_jettison_core ( line, file, msg, 0, 0, 0);                 \
    } while (0);

typedef struct __attribute__((packed))
{
  uint16     line;
  const char *fmt;      /*!< Printf style format string */
  const char *fname;    /*!< Pointer to source file name */
}
err_const_type;

#if defined (FEATURE_FATAL_SECTION) 

/********************
 NOTE:
  Define FEATURE_FATAL_SECTION *globally* to enable this feature. Necesary
  scatter loader/linker script sections, etc. must be defined as needed and
  are *not* the responsibility of error services.  This FEATURE is required
  for error compression to work (external dependencies belong to
  modem performance team)
 
 *********************/


#pragma GCC diagnostic ignored "-Wstatic-in-inline"
static const char __THIS_FILENAME__[] __attribute__ ((aligned(1),section (".fatal.file.rodata." ))) = __FILENAME__;


/* NOTE: fatal.fmt.rodata. and fatal.struct.rodata are zlib compressed post compilation
 fatal.file.rodata. is NOT compressed to prevent possible conflicts with the optimizer
 all of these *MUST* remain outside of q6zip compression sections or they maybe
 paged out during err_fatal, which is unrecoverable */

#define _XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
        static const char _xx_fmt [] __attribute__ ((section ("fatal.fmt.rodata." ))) = {xx_fmt}; \
        static const err_const_type xx_msg_const __attribute__ ((section ("fatal.struct.rodata." ))) = { \
              __LINE__, (_xx_fmt), __THIS_FILENAME__}

#else /* FEATURE_FATAL_SECTION */

#define _XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
        static const err_const_type xx_msg_const = { \
              __LINE__, (xx_fmt), __FILENAME__}

#endif /* FEATURE_FATAL_SECTION */

void err_Fatal_internal3 (const err_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);
void err_Fatal_internal2 (const err_const_type* const_blk, uint32 code1, uint32 code2);
void err_Fatal_internal1 (const err_const_type* const_blk, uint32 code1);
void err_Fatal_internal0 (const err_const_type* const_blk);

 
  #define ERR_FATAL(format, code1, code2, code3)  \
   ERR_FATAL_VAR(format, 3, code1, code2, code3)


  #define ERR_SAVE_FATAL_PAR(Msg, NbPar, Par1, Par2, Par3)  \
  switch (NbPar)                                         \
  {                                                      \
   case 2:                                               \
   err_Fatal_internal2 (Msg, Par1, Par2);                     \
   break;                                                \
   case 1:                                               \
   err_Fatal_internal1 (Msg, Par1);                           \
   break;                                                \
   case 0:                                               \
   err_Fatal_internal0 (Msg);                                 \
   break;                                                \
   case 3:                                               \
   default:                                              \
   err_Fatal_internal3 (Msg, Par1, Par2, Par3);               \
   break;                                                \
  }


#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  #define ERR_FATAL_VAR(format, NbPar, code1, code2, code3)                     \
    do                                                                       \
    {                                                                        \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, err_generic_msg );    \
        ERR_SAVE_FATAL_PAR (&xx_msg_const, NbPar, (uint32)(code1), (uint32)(code2), (uint32)(code3))  \
     /*lint -save -e{717} */                                                 \
     } while (0)                                                             \
     /* lint -restore */

#else
  #define ERR_FATAL_VAR(format, NbPar, code1, code2, code3)                     \
    do                                                                       \
    {                                                                        \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, format );             \
        ERR_SAVE_FATAL_PAR (&xx_msg_const, NbPar, (uint32)(code1), (uint32)(code2), (uint32)(code3))  \
     /*lint -save -e{717} */                                                 \
     } while (0)                                                             \
     /* lint -restore */

#endif /* FEATURE_GENERIC_ERR_FATAL_MSG */


/*===========================================================================

MACRO ERR_FATAL_AUX_MSG

DESCRIPTION
  Accepts a backward compatible, zero-parameter ERR_FATAL string, as well
  as an additional string to fill a dynamic message buffer
  which is stored in the coredump.err.aux_msg.

NOTE
  The SFR created by this message is parsed by some HLOS's/customers.
  As such format maybe important.  Please consult with owners before
  using.

DEPENDENCIES
  None

RETURN VALUE
  None.  Control does NOT return to caller.

SIDE EFFECTS
  Terminates processing, dumps core

===========================================================================*/
void err_Fatal_aux_msg 
(
  const err_const_type* const_blk, 
  const char * dynamic_msg,
  uint16 size_dynamic_msg
);

#ifdef FEATURE_GENERIC_ERR_FATAL_MSG
  #define ERR_FATAL_AUX_MSG(legacy_msg, dynamic_msg, size_dynamic_msg)             \
    do                                                                             \
    {                                                                              \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, err_generic_msg_dynamic ); \
        err_Fatal_aux_msg (&xx_msg_const, dynamic_msg, size_dynamic_msg);          \
    /*lint -save -e{717} */                                                        \
    } while (0)                                                                    \
    /* lint -restore */

#else /* !FEATURE_GENERIC_ERR_FATAL_MSG */
  #define ERR_FATAL_AUX_MSG(legacy_msg, dynamic_msg, size_dynamic_msg)             \
    do                                                                             \
    {                                                                              \
        _XX_MSG_CONST( MSG_SSID_DFLT, MSG_LEGACY_FATAL, legacy_msg );              \
        err_Fatal_aux_msg (&xx_msg_const, dynamic_msg, size_dynamic_msg );         \
    /*lint -save -e{717} */                                                        \
    } while (0)                                                                    \
    /* lint -restore */
#endif /* FEATURE_GENERIC_ERR_FATAL_MSG */

/*===========================================================================

FUNCTION ERR_STORE_INFO_TO_FLASH
DESCRIPTION
  Stores information associated with a fatal error or an exception to
  flash.
============================================================================*/
void err_store_info_to_flash (void);

/* Temp function for external reference */
void err_SaveFatal0 (const msg_const_type* const_blk);


#ifdef __cplusplus
}
#endif

#endif /* ERR_H */
