/*!
  @file
  qsh.h

  @brief
  The interface to the Qualcomm Sherlock Holmes feature implementation file.

  @author
  vdalmiya

*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/utils.mpss/2.3.4/api/qsh/qsh.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
05/23/14   mm      Updated log interface and added standardized summary
03/24/14   vd      Added support for Ulog messages with 6-10 arguments.
03/12/14   vd      CR 630063: initial version                   
==============================================================================*/

#ifndef QSH_H
#define QSH_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include <comdef.h>
#include <customer.h>
#include <qsh_types.h>
#include <qsh_util.h>

/*==============================================================================

                             MACROS

==============================================================================*/

/*! @brief Logging with up to 10 arguments. Max size is 1000 bytes. 
    1st arg: Enum for client
    2nd arg: Enum for category
    3rd arg: Enum for message type
    4th arg: Summary for the logging
    >= 5th arg: Arguments for the logging (optional)
*/
#define QSH_LOG(client, category, msg_type, summary, ...) \
  QSHI_LOG(client, category, msg_type, summary, ##__VA_ARGS__)

/*! @brief Macro to handle simple "if" checks. This will maintain a running 
           count for the summary at the end of the analysis.
    1st arg: Expression (will write log if this evaluates to TRUE)
    2nd arg  Enum for client
    3rd arg: Enum for category
    4th arg: Enum for message type
    5th arg: Summary for the logging
    >= 6th arg: Arguments for the logging (optional)
*/  
#define QSH_CHECK_LOG(expr, client, category, msg_type, summary, ...) \
  QSHI_CHECK_LOG(expr, client, category, msg_type, summary, ##__VA_ARGS__)

/*! @brief to find if a particular category is enabled in category mask */
#define QSH_CATEGORY_ENABLED(category_mask, category) \
  ((category_mask & category) != 0)

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/
/*==============================================================================

  FUNCTION:  qsh_reg_client_with_cb

==============================================================================*/
/*!
  @brief
  Function for the external modules to register their call back which
  can be called when the analysis is performed by QSH.
 
  @note
  Clients are supposed to update arguments major_ver and minor_ver when changes
  are made in their callback function

*/
/*============================================================================*/
void qsh_reg_client_with_cb
(
  qsh_clt_e client, /*!< client registering for the call back*/
  qsh_analysis_cb_type analysis_cb_ptr, /*!< call back for analysis */
  uint16 major_ver, /*!< major version for the CB */
  uint16 minor_ver /*!< minor version for the CB */
);

/*==============================================================================

  FUNCTION:  qsh_invoke_cb

==============================================================================*/
/*!
  @brief
  To invoke the call backs registered with QSH. 
 
  @note The CB registered through qsh_reg_client_with_cb(...) SHOULD NOT CALL
  this function else there will be an assertion.
 
  @return
  None.
*/
/*============================================================================*/
void qsh_invoke_cb
(
  qsh_invoke_params_s *params /*!< invoke parameters */
);

#endif /* QSH_H */
