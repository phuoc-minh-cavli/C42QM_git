#ifndef DS_CRIT_SECT_H
#define DS_CRIT_SECT_H
/*===========================================================================

                           D S _ C R I T _ S E C T . H

GENERAL DESCRIPTION
  Collection of Critical Section Macros used by various modules in DS.
  
Copyright (c) 2018, 2020 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_utils.h_v   1.0   08 Aug 2002 11:19:58   akhare  $
  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ds_crit_sect.h#2 $ $DateTime: 2020/09/28 02:10:53 $ $Author: pwbldsvc $

when      who     what, where, why
-------   ---     ----------------------------------------------------------
10/08/18      mks      Created the api for aquiring the critical sections in DS
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                            PUBLIC DATA DECLARATIONS

===========================================================================*/
/*!
  @brief 
  Definition of the DS critical section type.
  
  @details
  This definition is provided for allowing different platform specific
  critical section implementation. The handle variable inside the struct
  refers to the platform specific implementation of the critical section.
*/
typedef struct ds_crit_sect_s
{
  void* handle;
} ds_crit_sect_type;

/*!
  Declaration of global DS critical section.
*/
extern ds_crit_sect_type global_ps_crit_section;

/*!
  Declaration of critical section, this is used for synchronizing a global
  variable between ps and ds tasks. Do not use this for any other purpose.
*/
extern ds_crit_sect_type ps_ds_crit_section;

/*!
  Declaration of global DS_UTILS critical section, this macro is used only
  for ref-count update serialization in DSNET/DSSOCK/UTILS layers. Do not
  use this for any other purpose.
*/
extern ds_crit_sect_type global_ds_utils_crit_section;

/*===========================================================================

                          PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*!
  @brief 
  Initialize a critical section

  @params[in] crit_sect_ptr
  Critical section pointer to be initialized, client just need to pass a 
  structure of type ds_crit_sect_type, memory for any implementation 
  specific data is allocated by platforms. 

  @return
  NONE.

  @notes
  This function should have been called before any calls to
  DS_ENTER_CRIT_SECT() or DS_LEAVE_CRIT_SECT().
  After the critical section usage is complete, client should call
  DS_DESTROY_CRIT_SECTION to release the critical section and its 
  associated memory.
*/
#define DS_INIT_CRIT_SECTION ds_init_crit_section
void ds_init_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
);


/*!
  @brief 
  Enter a critical section

  @params[in] crit_sect_ptr
  Critical section to be entered. 

  @return
  NONE.

  @notes
  Critical section should be initialized prior to calling this function. 
*/
#define DS_ENTER_CRIT_SECTION ds_enter_crit_section
void ds_enter_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
);

/*!
  @brief 
  Leave a critical section

  @params[in] crit_sect_ptr
  Critical section to be left.

  @return
  NONE.

  @notes
  Critical section should be initialized prior to calling this function. 
  If not already in critical section, the behavior is undefined.
*/
#define DS_LEAVE_CRIT_SECTION ds_leave_crit_section
void ds_leave_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
);

/*!
  @brief 
  Destroy a critical section

  @params[in] crit_sect_ptr
  Critical section to be destroyed.

  @return
  NONE.

  @notes
  Should be only called on initialized critical sections. Otherwise
  behavior is undefined.
*/
#define DS_DESTROY_CRIT_SECTION ds_destroy_crit_section
void ds_destroy_crit_section
(
  ds_crit_sect_type*  crit_sect_ptr
);


/*---------------------------------------------------------------------------
  Legacy critical section variations. 
  These are being deprecated and should not be used.
---------------------------------------------------------------------------*/
#define DS_BRANCH_ENTER_CRIT_SECTION(cs_p)      DS_ENTER_CRIT_SECTION(cs_p)
#define DS_BRANCH_LEAVE_CRIT_SECTION(cs_p)      DS_LEAVE_CRIT_SECTION(cs_p)
#define DS_ENTER_CRIT_SECTION_NO_TASKLOCK(cs_p) DS_ENTER_CRIT_SECTION(cs_p)
#define DS_LEAVE_CRIT_SECTION_NO_TASKLOCK(cs_p) DS_LEAVE_CRIT_SECTION(cs_p)


#ifdef __cplusplus
}
#endif

#endif /* DS_CRIT_SECT_H */

