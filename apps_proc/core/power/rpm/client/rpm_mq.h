/*===========================================================================

  rpm_mq.h - management for unsent and in-flight rpm synchronization

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_MQ_H
#define RPM_MQ_H

#include "rpm.h"
#include "rpm_resource.h"

typedef struct rpm_mq_s rpm_mq_t;

rpm_mq_t *rpm_mq_create(void);

void rpm_mq_destroy(rpm_mq_t *self);

unsigned rpm_mq_put(rpm_mq_t *self, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *resource_data);

void rpm_mq_wait(rpm_mq_t *self, unsigned msg_id);

void rpm_mq_completed(rpm_mq_t *self, unsigned msg_id, bool send_pending_msg);

bool rpm_mq_check_empty(rpm_mq_t *self);

void rpm_mq_update_resource_pointers(rpm_mq_t *self);

// Specific to sleep / next active set behavior.
unsigned rpm_mq_get_oldest_in_flight_msg_id(rpm_mq_t *self);
unsigned rpm_mq_is_in_flight(rpm_mq_t *self, unsigned msg_id);
void rpm_mq_force_churn_cb(rpm_mq_t *self, unsigned size);
void rpm_mq_flush(rpm_mq_t *self);
unsigned rpm_mq_check_num_in_flight(rpm_mq_t *self);

// This needs to be removed as part of cleanup
// retaining it as it is required by power/sleep2.0/src/uSleep/ddr/uSleep_util.c
void rpm_mq_retire_acks(rpm_mq_t *self);

bool is_standalone(void);

#endif // RPM_MQ_H

