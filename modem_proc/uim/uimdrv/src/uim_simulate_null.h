#ifndef UIM_SIMULATE_NULL_H
#define UIM_SIMULATE_NULL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  UIM_SIMULATE_NULL HEADER

DESCRIPTION
  This contains all the declarations of the simulate null  related APIs and globals.

Copyright (c) 2016 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //components/rel/uim.mpss/3.3.1/uimdrv/src/uim_simulate_null.h#1 $
$DateTime: 2020/07/22 10:53:27 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/07/16   gm       Initial Draft

===========================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE:      UIM_FEATURE_BUSY_RESPONSE_SIMULATE_NULL_TYPE
-------------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST {
  uint8    version;
  /* version 0: NEW FEATURE ITEMS = 2; TOTAL CURRENT FEATURE ITEMS = 2. */
  boolean uim_disable_simulate_null;
  uint8 uim_simulate_null_ins;
  /* version 0: RESERVED_FOR_FUTURE_USE size is 5 - 3 = 2 */
  uint8    reserved_for_future_use[2];
} uim_feature_busy_response_simulate_null_type;


/*===========================================================================

FUNCTION UIM_SIMULATE_NULL_TIMER_STATE_HANDLER

DESCRIPTION
  This function handles actions and change of state of uim_simulate_null_timer,
  when some event happens. This function is only for SIM BUSY Feature testing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_timer_state_handler
(
  uim_instance_global_type *uim_ptr,
  uim_busy_response_timer_event_type simulate_null_timer_event
);


/*===========================================================================

FUNCTION uim_simulate_read_from_efs

DESCRIPTION
  This function read data from efs for simulate NULL

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_read_from_efs
(
  uim_instance_global_type *uim_ptr
);


/*===========================================================================

FUNCTION UIM_SIMULATE_NULL_ADD_STATE

DESCRIPTION
  This function add extra state to uim command,

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_simulate_null_add_state
(
  uim_instance_global_type *uim_ptr
);
#endif /*UIM_SLOT_MAPPING_H*/
