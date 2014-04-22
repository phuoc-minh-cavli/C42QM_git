#ifndef __CSD_STATUS_H__
#define __CSD_STATUS_H__
/**
    @file   csd_status.h
    @brief  
	Contains the return code for CSD
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CSD_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the CSD_mainpage.dox file or 
      contact Tech Pubs.
===========================================================================*/
/*-----------------------------------------------------------------------------
     Copyright (c\C) 2010-2012 Qualcomm Technologies Incorporated.
	 All rights reserved.
	 Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.
	when		who			what, where, why
	--------	-----		---------------------------------------------------
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
	10/21/10	jzhou		Doxygen comments update
	08/17/10	jzhou		Doxygen comments update
	06/01/10	jzhou		Initial Creation

=============================================================================*/

/** @addtogroup csd_status
  @{
*/

/** For internal use only. */
#define CSD_ESTATUS_BEGIN                       ( 0x00012313 )

/** Success. The operation completed, and there were no errors. */
#define CSD_EOK	                                ( 0x0        )

/** General failure. */
#define CSD_EFAILED                             ( 0x00012313 )

/** Invalid operation parameters. */
#define CSD_EBADPARAM				( 0x00012314 )

/** Unsupported routine or operation. */
#define CSD_EUNSUPPORTED			( 0x00012315 )

/** Unsupported version. */
#define CSD_EVERSION                            ( 0x00012316 )

/** Unexpected problem was encountered. */
#define CSD_EUNEXPECTED	                        ( 0x00012317 )

/** Unhandled problem occurred. */
#define CSD_EPANIC                              ( 0x00012318 )

/** Unable to allocate resources. */
#define CSD_ENORESOURCE				( 0x00012319 )

/** Invalid handle. */
#define CSD_EHANDLE                             ( 0x0001231A )

/** Operation is already processed. */
#define CSD_EALREADY                            ( 0x0001231B )

/** Operation is not ready to be processed. */
#define CSD_ENOTREADY	                        ( 0x0001231C )

/** Operation is pending completion. */
#define CSD_EPENDING                            ( 0x0001231D )

/** Operation cannot be accepted or processed. */
#define CSD_EBUSY                               ( 0x0001231E )

/** Operation aborted due to an error. */
#define CSD_EABORTED				( 0x0001231F )

/** Operation was preempted by a higher priority. */
#define CSD_EPREEMPTED                          ( 0x00012320 )

/** Operation requires intervention to complete. */
#define CSD_ECONTINUE                           ( 0x00012321 )

/** Operation requires immediate intervention to complete. */
#define CSD_EIMMEDIATE				( 0x00012322 )

/** Operation is not implemented. */
#define CSD_ENOTIMPL                            ( 0x00012323 )

/** Operation requires more data or resources. */
#define CSD_ENEEDMORE                           ( 0x00012324 )

/** Operation is a local procedure call. */
#define CSD_ELPC                                ( 0x00012325 )

/** Operation timed out. */
#define CSD_ETIMEOUT                            ( 0x00012326 )

/** CSD did not find the handle or an internal resource required to process the 
  request. */
#define CSD_ENOTFOUND                           ( 0x00012327 )

/** Operation cannot proceed due to an improper state. */
#define CSD_EBADSTATE                           ( 0x00012328 )

/** Qualcomm aDSP return error status. */ 
#define CSD_EQADSP                              ( 0x00012329 )

/** Subsystem is in reset. */
#define CSD_ESSRESET                            ( 0x0001232A )

/** For internal use only. */ 
#define CSD_ESTATUS_END                         ( 0x00012700 )

/** @} */ /* end_addtogroup csd_status */

#endif /* __CSD_STATUS_H__ */

