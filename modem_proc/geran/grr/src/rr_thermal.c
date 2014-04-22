/*!
 * \file rr_thermal.c 
 *  
 * This module contains functionality to manage Thermal Mitigation. 
 *  
 *              Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //components/rel/geran.mpss/5.2.0/grr/src/rr_thermal.c#4 $ */
/* $DateTime: 2019/07/21 13:14:20 $$Author: pwbldsvc $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"
#ifdef FEATURE_RR_THERMAL_MITIGATION
#include "rr_thermal.h"
#include "npa.h"
#include "msg.h"
#include "rr_defs.h"
#include "rr_gprs_defs.h"
#include "rr_nv.h"
#include "rr_if.h"
#include "rr_mm_send.h"
#include "rr_general.h"
#include "rr_multi_sim.h"
#include "rr_msc.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * State Machine Definition
 * -------------------------------------------------------------------------*/

/**
 * This state machine is defined as follows: 
 * There are two functions per state 
 * 1. Enter function   - This function is called when the state is entered (i.e. called once) 
 * 2. Process function - This function is called to handle any messages received while in a state (optional)
 *  
 * The state machine functions do not alter the state, the state is set to equal to state indicated by the 
 * temperature sensor. There is no switch-case implementation. See handle_thermal_mitigation_event().
 */

// Note: The state values match the temp sensor state values
typedef enum
{
  RR_THERM_NORMAL       = 0,
  RR_THERM_REDUCE_MSC   = 1,
  RR_THERM_NO_PS_ACCESS = 2,
  RR_THERM_EMERG_ONLY   = 3,
  RR_THERM_STATE_COUNT
} rr_therm_state_e_t;

typedef void (*rr_therm_state_fn_t)(void);

typedef struct
{
  rr_therm_state_fn_t enter_fn;
  rr_therm_state_fn_t process_fn;
} rr_therm_state_machine_def_t;

extern void change_event_cb_fn(void *context, unsigned int event_type, void *data, unsigned int data_size);
extern void initialise_event_cb_fn(void *context, unsigned int event_type, void *data, unsigned int data_size);

static void rr_therm_normal_action_fn(void);
static void rr_therm_reduce_msc_action_fn(void);
static void rr_therm_no_ps_access_action_fn(void);

const rr_therm_state_machine_def_t rr_therm_state_machine_def[RR_THERM_STATE_COUNT] =
{
  //  enter_fn                         process_fn
  {   rr_therm_normal_action_fn,       NULL,                            },
  {   rr_therm_reduce_msc_action_fn,   rr_therm_reduce_msc_action_fn,   },
  {   rr_therm_no_ps_access_action_fn, NULL,                            },
  {   rr_therm_no_ps_access_action_fn, NULL,                            }
};

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef boolean (*rr_msc_adjust_fn_t)(rr_msc_client_e_t);

typedef struct
{
  rr_therm_state_e_t  state;
  boolean             srv_cap_changed;
} rr_therm_data_t;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

#ifndef PERLUTF
static npa_event_handle eventClient;
#endif

static rr_therm_data_t rr_therm_data;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Returns the data-space index according to the given AS-ID.
 * 
 * \return uint8 - data-space index
 */
static uint8 as_id_to_index(void)
{
  return GERAN_DATA_SPACE_INDEX_1;
}

/*!
 * \brief Returns the state-machine state
 * 
 * \return rr_therm_state_e_t 
 */
static rr_therm_state_e_t get_state(void)
{
  return rr_therm_data.state;
}

/*!
 * \brief Generic action function, which adjusts the multi-slot class values and service capability, and if 
 *        an update to MM is required, sends RR_IMSG_THERMAL_MITIGATION_REQ into RR. 
 * 
 * \param msc_adjust_fn (in) - MSC adjustment function to be called
 * \param ps_allowed (in) - whether PS is allowed according to the current thermal state
 */
static void rr_therm_action(
  rr_msc_adjust_fn_t msc_adjust_fn,
  boolean ps_allowed
)
{
  sys_srv_capability_e_type srv_cap;

  boolean update_required = FALSE;
  rr_therm_data_t *rr_therm_data_ptr = &rr_therm_data;

  if (msc_adjust_fn != NULL)
  {
    if ((*msc_adjust_fn)(RR_MSC_CLIENT_THERMAL))
    {
      MSG_GERAN_HIGH_0_G("[MSC] msc update required");

      // Flag that RR_IMSG_THERMAL_MITIGATION_REQ should be sent
      update_required = TRUE;
    }
  }

  if (ps_allowed)
  {
    srv_cap = rr_scell_service_capability();
  }
  else
  {
    srv_cap = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
  }

  if (srv_cap != rr_mm_get_service_capability())
  {
    MSG_GERAN_HIGH_0_G("[MSC] srv_cap update required");

    // Flag that a Service Capability update is required
    rr_therm_data_ptr->srv_cap_changed = TRUE;

    // Flag that RR_IMSG_MSC_CHANGE_IND should be sent
    update_required = TRUE;
  }

  if (update_required)
  {
    // Send request to RR to request an update
    rr_msc_send_rr_imsg_msc_change_ind();
  }
}

/*!
 * \brief Action Function to match Normal condition.
 * 
 */
static void rr_therm_normal_action_fn(void)
{
  rr_therm_action(
    rr_msc_revert_to_orig,   // msc_adjust_fn
    TRUE                     // ps_allowed
  );
}

/*!
 * \brief Action Function to reduce MSC.
 * 
 */
static void rr_therm_reduce_msc_action_fn(void)
{
  rr_therm_action(
    rr_msc_reduce_tx_slots,   // msc_adjust_fn
    TRUE                      // ps_allowed
  );
}

/*!
 * \brief Action Function to match No PS Access condition.
 * 
 */
static void rr_therm_no_ps_access_action_fn(void)
{
  rr_therm_action(
    rr_msc_revert_to_orig,   // msc_adjust_fn
    FALSE                    // ps_allowed
  );
}

/*!
 * \brief Handle event changes triggered by the temperature sensor.
 * 
 * \param state (in)
 */
static void handle_thermal_mitigation_event(uint8 state)
{
  if (state < RR_THERM_STATE_COUNT)
  {
    uint8 i;
    rr_therm_state_e_t current_state;

    rr_therm_state_e_t new_state = (rr_therm_state_e_t)state;
    uint8 num_of_instances = 1;

    // Take the thermal mitigation action for each instance
    for (i = GERAN_DATA_SPACE_INDEX_1; i < num_of_instances; i++)
    {
      current_state = rr_therm_data.state;
      rr_therm_data.state = new_state;

      // Check for a change of state
      if (current_state != new_state)
      {
        // Call the Enter Function for the new state
        if (rr_therm_state_machine_def[new_state].enter_fn != NULL)
        {
          (*rr_therm_state_machine_def[new_state].enter_fn)();
        }
      }
      else
      {
        // Call the Process Function for the current state
        if (rr_therm_state_machine_def[new_state].process_fn != NULL)
        {
          (*rr_therm_state_machine_def[new_state].process_fn)();
        }
      }
    }
  }
}

#ifndef PERLUTF
/*!
 * \brief This is the call-back function registered with npa_create_change_event_cb.
 *  
 * This function is called upon any status change.
 * 
 * \param context 
 * \param event_type 
 * \param data 
 * \param data_size 
 */
void change_event_cb_fn_dyn(void         *context,
                               unsigned int  event_type,
                               void         *data,
                               unsigned int  data_size)
{
  npa_event_data *event_data_ptr = (npa_event_data*)data;

  if (event_data_ptr != NULL)
  {
    handle_thermal_mitigation_event(event_data_ptr->state);
  }
}
#endif // #ifndef PERLUTF

#ifndef PERLUTF
/*!
 * \brief This is the call-back function registered with npa_resource_available_cb.
 *
 * This function is called when the NPA resources are available. A further call-back function can then be
 * registered to be called upon any status change
 *
 * \param context 
 * \param event_type 
 * \param data 
 * \param data_size 
 */
void initialise_event_cb_fn_dyn(
  void         *context,
  unsigned int  event_type,
  void         *data,
  unsigned int  data_size
)
{
  npa_query_type  query_result;

  eventClient = npa_create_change_event_cb(
    "/therm/mitigate/pa",
    "Thermal Mitigation (GSM-RR)",
    change_event_cb_fn,
    NULL
  );

  npa_query_by_event(
    eventClient,
    NPA_QUERY_CURRENT_STATE,
    &query_result
  );

  /* Free the memory associated with NPA event */
  npa_destroy_event_handle(eventClient);
  eventClient = NULL;

  if (query_result.data.state != 0)
  {
    handle_thermal_mitigation_event(query_result.data.state);
  }
}
#endif // #ifndef PERLUTF

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Updates MM with new paramters as required.
 * 
 * \param can_send_service_ind (in) - Set to TRUE if a RR_SERVICE_IND can be sent to MM
 */
void rr_thermal_update(boolean can_send_service_ind )
{
 
  rr_therm_data_t *rr_therm_data_ptr = &rr_therm_data;

  // Check if a service capability update is required
  if (rr_therm_data_ptr->srv_cap_changed)
  {
    // Only update MM if the caller has indicated it is appropriate
    if (can_send_service_ind)
    {
      grr_update_mm_with_rr_service_ind();
    }

    rr_therm_data_ptr->srv_cap_changed = FALSE;
  }
}

/*!
 * \brief Indicates if thermal mitigation is blocking PS access or not.
 * 
 * 
 * \return boolean - TRUE is PS is allowed, FALSE otherwise
 */
boolean rr_thermal_ps_allowed( )
{
  boolean ps_allowed = TRUE;

  switch (get_state())
  {
    case RR_THERM_NO_PS_ACCESS:
    case RR_THERM_EMERG_ONLY:
    {
      ps_allowed = FALSE;
    }

    default: ;
  }

  return ps_allowed;
}

/*!
 * \brief Initialisation function to be called at RR task start.
 */
void rr_thermal_task_start_init(void)
{
  // Zero data structure across all subs
  memset(&rr_therm_data, 0, sizeof(rr_therm_data));

#ifndef PERLUTF
  // Register the call-back to indicate when the temperature sensor is available
  npa_resource_available_cb(
    "/therm/mitigate/pa",
    initialise_event_cb_fn,
    NULL
  );
#endif
}

#ifdef PERLUTF
#error code not present
#endif
#endif /* FEATURE_RR_THERMAL_MITIGATION */

/* EOF */

