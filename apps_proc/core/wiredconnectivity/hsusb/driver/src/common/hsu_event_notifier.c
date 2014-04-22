/*==============================================================================

    High Speed USB generic event notifier
  
  GENERAL DESCRIPTION
    This module defines a generic event notifier. It allows a client outside HSUSB
    to register for events from the HSUSB.

  ==============================================================================
  
							  EDIT HISTORY FOR MODULE
	
	This section contains comments describing changes made to the module.
	Notice that changes are listed in reverse chronological order.
	
	$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/common/hsu_event_notifier.c#1 $
	$DateTime: 2020/01/30 22:49:35 $
	  
	when	  who	  what, where, why
	--------  ---	  ------------------------------------------------------------
  
	  
  ==============================================================================
    
      
  EXTERNALIZED FUNCTIONS
    hsu_register_event_notif_cb
    hsu_unregister_event_notif_cb
    hsu_event_raise

  INITALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_event_notifier.h"
#include "hsu_common_int.h"

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/* Maximal number of client callbacks that can be registered for an event.    */
#define HSU_EVENT_NOTIF_MAX_REGISTERED_CB 10

/*==============================================================================
  Variables
==============================================================================*/

/* Registered callbacks */
static hsu_event_notif_cb_info_struct 
  hsu_event_notif_registered_cb_array[HSU_EVENT_NOTIF_MAX_REGISTERED_CB] = {{NULL}};

/* Current status of each event */
static boolean hsu_event_notif_cached_event[HSU_EVENT_MAX] = {FALSE, FALSE};

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_EVENT_RAISE

DESCRIPTION
This function is called when a certain HSUSB event should be raised.
The function calls all callbacks registered to this certain event.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
All registered callbacks are invoked

==============================================================================*/
void hsu_event_notif_raise
(
  hsu_event_notif_enum event
)
{
  uint8 i;

  hsu_event_notif_cached_event[event] = TRUE;

   /* Need to notify registered callbacks of the event */
  for(i = 0 ; i < HSU_EVENT_NOTIF_MAX_REGISTERED_CB; ++i)
  {
    if(hsu_event_notif_registered_cb_array[i].event == event &&
       hsu_event_notif_registered_cb_array[i].event_cb != NULL)
    {
      hsu_event_notif_registered_cb_array[i].event_cb();
    }
  }

  HSU_MSG_MED_1("hsu_event_notif_raise(): raised event %d", event);
} /* hsu_event_notif_raise */

/*===========================================================================

FUNCTION      HSU_REGISTER_EVENT_NOTIF_CB

DESCRIPTION
  Register a callback function to be invoked when the specified event is
  raised.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE for success.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hsu_register_event_notif_cb
(
  hsu_event_notif_cb_info_struct cb_info
)
{
  uint8 i;

  for(i = 0 ; i < HSU_EVENT_NOTIF_MAX_REGISTERED_CB ; ++i)
  {
    if (hsu_event_notif_registered_cb_array[i].event_cb == NULL)
    {
      break; /* we found an available entry */
    }
  }

  if (i == HSU_EVENT_NOTIF_MAX_REGISTERED_CB)
  {
    HSU_MSG_ERROR_1("hsu_register_event_notif_cb(): can't register event %d", 
      cb_info.event);
    return FALSE;  
  }

  HSU_MSG_MED_1("hsu_register_event_notif_cb(): registered event notifier, event %d", 
    cb_info.event);

  hsu_event_notif_registered_cb_array[i].event_cb = cb_info.event_cb;
  hsu_event_notif_registered_cb_array[i].event = cb_info.event;

  /* If the event has already been raised prior to registering, raise it now. */
  if (hsu_event_notif_cached_event[cb_info.event] &&
      hsu_event_notif_registered_cb_array[i].event_cb != NULL)
  {      
    hsu_event_notif_registered_cb_array[i].event_cb();
  }

  return TRUE;
} /* hsu_register_event_notif_cb */

/*===========================================================================

FUNCTION      HSU_UNREGISTER_EVENT_NOTIF_CB

DESCRIPTION
Unregister a callback function. The callback will not be invoked anymore
when the event is raised.

DEPENDENCIES
hsu_register_event_notif_cb must have been called with the same 
callback function.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_unregister_event_notif_cb
(
  hsu_event_notif_cb_info_struct cb_info
)
{
  uint8 i;

  HSU_MSG_MED_1("hsu_unregister_event_notif_cb(): unregistered event notifier, event %d", 
    cb_info.event);

  for(i = 0 ; i < HSU_EVENT_NOTIF_MAX_REGISTERED_CB ; ++i)
  {
    if (hsu_event_notif_registered_cb_array[i].event_cb == cb_info.event_cb)
    {
      hsu_event_notif_registered_cb_array[i].event_cb = NULL;
      break; 
    }
  }
} /* hsu_unregister_event_notif_cb */
