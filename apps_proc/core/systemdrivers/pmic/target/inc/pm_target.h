#ifndef __PM_TARGET_H__
#define __PM_TARGET_H__

/*! \file pm_target.h 
*  \n
*  \brief
*  \n  
*  \n &copy; Copyright 2020 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/core.tx/6.0/systemdrivers/pmic/target/inc/pm_target.h#1 $ 

===========================================================================*/
#include "pm_err_flags.h"

/*===========================================================================

                     FUNCTION DECLARATION 

===========================================================================*/

/*! \brief This is place to do target specific configs if any .
 *  \param[in] None
 *
 *  \return err_flag
 */
pm_err_flag_type pm_shutdown_target(void);

#endif //__PM_TARGET_H__


