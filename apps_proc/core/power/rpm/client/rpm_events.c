/*===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include <stdlib.h>
#include <stddef.h>

#include "rpm_events.h"
#include "inq.h"
#include "qurt_signal.h"
#include "CoreVerify.h"

#define QURT_DEFAULT_SIGNAL  0x1

struct rpm_event_s
{
    inq_link_t        link;
    qurt_signal_t     signal;
};

static inq_t *rpm_event_pool;


static rpm_event_t *rpm_create_dal_event(void)
{
    rpm_event_t *event;
    
    if(!rpm_event_pool)
        rpm_event_pool = inq_create(offsetof(rpm_event_t, link));

    event = (rpm_event_t *)inq_pop(rpm_event_pool);

    // If we didn't have a saved event, make one.
    if(!event)
    {
        event = (rpm_event_t *)malloc(sizeof(rpm_event_t));
        CORE_VERIFY_PTR(event);
		
        qurt_signal_init(&event->signal); 
    }

    return event;
}

static void rpm_destroy_dal_event(rpm_event_t *event)
{
    inq_append(rpm_event_pool, event);
}

static void rpm_reset_dal_event(rpm_event_t *event)
{
    qurt_signal_clear(&event->signal, QURT_DEFAULT_SIGNAL);
}

static void rpm_set_dal_event(rpm_event_t *event)
{
    qurt_signal_set(&event->signal, QURT_DEFAULT_SIGNAL);
}

static void rpm_wait_dal_event(rpm_event_t *event)
{
  if(qurt_signal_wait(&event->signal, QURT_DEFAULT_SIGNAL, QURT_SIGNAL_ATTR_WAIT_ANY))
  {
    qurt_signal_clear(&event->signal, QURT_DEFAULT_SIGNAL);
  }
}

rpm_event_t *(*rpm_event_create)(void)       = rpm_create_dal_event;
void (*rpm_event_destroy)(rpm_event_t *self) = rpm_destroy_dal_event;
void (*rpm_event_reset)(rpm_event_t *self)   = rpm_reset_dal_event;
void (*rpm_event_set)(rpm_event_t *self)     = rpm_set_dal_event;
void (*rpm_event_wait)(rpm_event_t *self)    = rpm_wait_dal_event;


