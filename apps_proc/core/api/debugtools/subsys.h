#ifndef __SUBSYS_H__
#define __SUBSYS_H__

/**
  @file subsys.h
  @brief This file contains subsys module data definitions and functions.
*/
/*=============================================================================

                     S U B S Y S T E M     M O D U L E

GENERAL DESCRIPTION
  Implements Subsys framework for booting/shutting down various subsystems.


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  subsys_start(subsys name)
    Starts requested subsystem.

INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/api/debugtools/subsys.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation

=============================================================================*/

/** @addtogroup subsys_module
@{ */

#ifdef __cplusplus
/* Avoid name mangling for any C++ customers */
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"

/*=============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/** @brief Macro to be used for providing "modem" subsys as input to subsys_* apis. 
*/
#define SUBSYS_MODEM_NAME      "modem"


//Todo: can remove some errors
/** @brief Various Subsys Error types. 
*/
typedef enum
{
  SUBSYS_SUCCESS,
  SUBSYS_FAILURE,
  SUBSYS_INVALID_SUBSYS,
  SUBSYS_INVALID_PARAMETERS,
  SUBSYS_NOT_ALLOWED
}subsys_error_type;


/** @brief Return status type when subsys_start is called. 
*/
typedef enum
{
  SUBSYS_START_SUCCESS,
  SUBSYS_START_INVALID_SUBSYS,
  SUBSYS_START_INVALID_ELF_ERROR,
  SUBSYS_START_FLASH_READ_ERROR,
  SUBSYS_START_MMU_ERROR,
  SUBSYS_START_TIMEOUT_ERROR,
  SUBSYS_START_OTHER_FAILURE,
}subsys_start_status_type;

/* Options input for Start function */
typedef struct
{
  boolean hibernate_boot;         /* set TRUE for Boot from hibernation, otherwise FALSE for normal boot */
}subsys_start_options_type;


/** @brief callback function prototype for subsys_start() api input
*/
typedef void (*subsys_start_cb_fn)(void * cb_data, subsys_start_status_type start_status);

/*=============================================================================

                            FUNCTION DECLARATIONS
                            
=============================================================================*/
#define subsys_start_options_init(options) \
{ \
    options.hibernate_boot = FALSE; \
}

/** 
Api for bringing a subsystem up by loading the related image from storage and
bringing the processor up. This also keeps the count of votes for subsys_start.


@note1hang The function returns immediately indicating success or failure about the start
request. But the actual loading will happen from internal thread and status of
bringing up the subsystem is indicated in the provided callback. 

@param[in] subsys_str   Subsys name. Use the macro indicated above. ex: SUBSYS_MODEM_NAME
@param[in] options      Options to indicate whether to boot from hibernate or not etc. 
                        Init using subsys_start_options_init() macro before updating internal variables.
@param[in] start_cb     callback to which client wants notification once bringup is success/failure.
@param[in] cb_data      cb_data to be returned with callback

@sideeffects 
None

@dependencies  
None
*/
subsys_error_type subsys_start(char *subsys_str, subsys_start_options_type *options, 
                               subsys_start_cb_fn start_cb, void *cb_data);

#ifdef __cplusplus
} /* extern "C" { */
#endif

/** @} */ /* end_addtogroup subsys_module */

#endif /* __SUBSYS_H__ */


