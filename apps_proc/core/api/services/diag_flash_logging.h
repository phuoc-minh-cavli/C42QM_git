#ifndef DIAG_FLASH_LOGGING_H
#define DIAG_FLASH_LOGGING_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostics Flash Logging Header File 

General Description
  Global data declarations for diagnostics flash logging.

Copyright (c) 2022 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=====================================================================================================================

                         Edit History

$Header: //components/dev/core.tx/6.0/kdey.core.tx.6.0.CR3191674_Flash_Logging_FR/api/services/diag_flash_logging.h#1 $
========================================================================================================================*/

#include "comdef.h"
#include "com_dtypes.h"
#include "diag.h"


/* -------------------------------------------------------------------------
** Macros
** ------------------------------------------------------------------------- */



/* -------------------------------------------------------------------------
** Type definitions
** ------------------------------------------------------------------------- */
typedef PACK(struct){

    /*! @brief 
        Signifies whether flash logging needs to be enabled or disabled.
        Value should be set to TRUE for enabling flash logging.
    */
    boolean flash_logging_enable;

    /*! @brief 
        For current implementation version should be 1.
    */
    uint8 version;

    /*! @brief  
        Kept for future enhancements. Can be ignored for current 
        implementation. 
    */
    uint16 reserved; 

}diag_flash_config;


/*! @brief  
    These enum values are returned by diag_enable_flash_logging API.
*/
typedef enum{
   DIAG_FLASH_LOGGING_SUCCESS = 0,
   DIAG_FLASH_LOGGING_FAILURE = 1
}diag_flash_logging_enum;


/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */


/*===================================================================================
FUNCTION DIAG_ENABLE_FLASH_LOGGING

DESCRIPTION
  This routine is called from the external world to enable/disable
  writing of diag packets to flash/EFS. 

  This routine will execute the following steps
  * Clear masks from mask tables and sets signal to diag task to send mask update
    control packets
  * Sets signal to diag task for enabling or disabling flash logging

PARAMETERS
  @param config
  Pointer to a diag_flash_config type structure object which 
  indicates the mode of logging
  
  config->flash_logging_enable = TRUE to start flash logging
  config->version = 1, version 1. Currently this field is ignored
  config->reserved, this field is ignored currently
  
DEPENDENCIES
  None.

RETURN VALUE
  Returns enum values from diag_flash_logging_enum in case
  of success/failure.

  Returning success means signal is set to diag task succesfully
  for enabling/disabling flash logging
============================================================================================*/

diag_flash_logging_enum diag_enable_flash_logging(diag_flash_config* config);

#endif /* DIAG_FLASH_LOGGING_H */

