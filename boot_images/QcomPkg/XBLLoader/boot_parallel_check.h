#ifndef BOOT_PARALLEL_CHECK_H
#define BOOT_PARALLEL_CHECK_H

/*=======================================================================
 * FILE:        boot_parallel_check.h
 *
 * SERVICES:    Header file for parallel read utilities
 *
 * DESCRIPTION: Header file shared across boot and flash 
 *
 * Copyright (c) 2017, 2019 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/XBLLoader/boot_parallel_check.h#2 $ *
 *
 * when         who         what, where, why
 * ----------   ------      ----------------------------------------------
 * 08/14/19     aus        Removed unused functions
 * 08/29/17     rohik      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/


/*-----------------------------------------------------------------------
 *           Definitions and Constants
 *----------------------------------------------------------------------*/

/***********************************************************************
 *           Generic Defines
 **********************************************************************/

 typedef enum
{
  READ_SEQUENTIAL = 0,
  READ_SEQUENTIAL_WITH_CHECK,
  READ_PARALLEL_WITH_NO_CHECK,
  READ_PARALLEL_WITH_CHECK
} read_state;

extern read_state read_parallel;

#define PARALLEL_LOAD_SIZE SIZE_12KB

#endif  /* BOOT_PARALLEL_CHECK_H */
