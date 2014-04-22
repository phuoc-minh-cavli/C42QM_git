#ifndef _HSU_AL_BAM_UTILS_H_
#define _HSU_AL_BAM_UTILS_H_

/*==============================================================================

BAM utilities for High Speed USB Internal Header File

GENERAL DESCRIPTION
Contains internal definitions needed for the BAM utilities.

EXTERNALIZED FUNCTIONS

hsu_al_bam_init
hsu_al_bam_pipe_enable
hsu_al_bam_pipe_disable
hsu_al_bam_allocate_pipes
hsu_al_bam_register_wakeup_event
hsu_al_bam_unregister_wakeup_event
hsu_al_bam_alloc
hsu_al_bam_free

Copyright (c) 2009, 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_bam_utils.h#1 $
  $DateTime: 2020/01/30 22:49:35 $


when      who       what, where, why
--------  ---       ------------------------------------------------------
03/03/09  evolfson  First revision of file  
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h"
#ifdef FEATURE_HS_USB_BAM
#include "sio.h"
#include "bam.h"
//#include "DALSys.h"
#include "DALDeviceId.h"
#include <comdef.h>
#include "jusb_core.h"
#include "cdc_fd.h"

/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/

/* please refer to excel spreadsheet for absolute address calculation */
#define HSU_AL_BAM_IPA_BAM_BASE_ADDR        0xDEADBEEF
#define HSU_AL_BAM_QDSS_BAM_BASE_ADDR       0x06064000
#define HSU_AL_BAM_QDSS_BAM_PIPE_NUM        0

#define HSU_AL_BAM_OCIMEM_BASE_ADDR         0xDEADBEEF
#define HSU_AL_BAM_QDSS_OCIMEM_BASE_ADDR    0x08627800

#define HSU_AL_BAM_UTILS_OCIMEM_BUF_SIZE          (uint32)(0 * 1024)
#define HSU_AL_BAM_UTILS_QDSS_OCIMEM_BUF_SIZE     (uint32)(2 * 1024)
#define HSU_AL_BAM_UTILS_SMEM_BUF_SIZE            (uint32)(0 * 1024) 

#define HSU_AL_BAM_EVENT_THRESHOLD                0x10
#define HSU_AL_BAM_EVENT_PROD_PIPE_THRESHOLD      0x2000
// Threshold is set to 13.5KB + 1 byte
#define HSU_AL_BAM_SUM_THRESHOLD                 13825
#define HSU_AL_BAM_IPA_PROD_PIPE_EVENT_THRESHOLD (HSU_AL_BAM_SUM_THRESHOLD)

#define HSU_AL_BAM_USB_BASE                 0x1B04000

#define HSU_AL_BAM_UL                       TRUE
#define HSU_AL_BAM_DL                       TRUE

typedef enum
{
   HSU_BAM_ENDPOINT_INVALID = 0,
   HSU_BAM_ENDPOINT_USB1,  /* HS-USB 1 */
   HSU_BAM_ENDPOINT_USB = HSU_BAM_ENDPOINT_USB1,
   HSU_BAM_ENDPOINT_USB2,  /* HS-USB 2 */

   HSU_BAM_ENDPOINT_IS_32_BIT = 0x7fffffff  /* Force enum size to 32-bit */
} hsu_bam_endpoint_type;


/* peripheral bam type is used to store all the configuration parameters specific to HSUSB/HSIC */
typedef struct
{
  bam_handle           bam_handle;
  bam_config_type      bam_config;

  bam_info_type                bam_core_dump_info;
  bam_coredump_config_type     bam_coredump_config;

  bam_callback_type    bam_top_level_isr;
} hsu_bam_peripheral_bam_type;


/* connection is defined as a pair of uni-directional pipes that forms one interface */
/* for instance, each RMNET instance is considered as one connection */
typedef struct
{
  sio_port_id_type     sio_port_id; /* populated by data service via SIO */
  uint32               if_num; /* BULK IN interface # */
  
  uint32               producer_pipe_index; /* arbitrary assigned but follow convention by BAM team */
  bam_handle           producer_pipe; /* populated by bam_pipe_init */
  bam_pipe_config_type *producer_pipe_config; /* populated by hsu_al_bam_bam_config_params wrapper from table */

  uint32               consumer_pipe_index; /* arbitrary assigned but follow convention by BAM team */
  bam_handle           consumer_pipe; /* populated by bam_pipe_init */
  bam_pipe_config_type *consumer_pipe_config; /* populated by hsu_al_bam_bam_config_params wrapper from table */

  /* upon receiving pipe wakeup event from BAM we will trigger remote wakeup */
  bam_callback_type    consumer_pipe_wakeup_event_cb; /* populated before calling bam_pipe_init */

  cdc_ctx_t            *cdc_ctx_ptr;

} hsu_bam_connection_type;

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_bam_init

DESCRIPTION
Initialize the HSUSB BAM interface.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_init(void);


void hsu_al_bam_ipa_init(void);

/*===========================================================================

FUNCTION hsu_al_bam_allocate_pipes

DESCRIPTION
Allocate IN and OUT BAM pipes.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_allocate_pipes
(
  hsu_bam_connection_type *connection
);

/*===========================================================================

FUNCTION hsu_al_bam_register_wakeup_event

DESCRIPTION
Enable pipe wakeup event for the given BAM pipe and register the given event 
object for wakeup event.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful registration. FALSE otherwise.

SIDE EFFECTS
Enable pipe wakeup interrupt in BAM HW

===========================================================================*/
boolean hsu_al_bam_register_wakeup_event
(
  hsu_bam_connection_type* connection
);

/*===========================================================================

FUNCTION hsu_al_bam_unregister_wakeup_event

DESCRIPTION
Disable pipe wakeup event for the given BAM pipe. This will unregister any 
event object registered for wakeup event.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful unregistration. FALSE otherwise.

SIDE EFFECTS
Disable pipe wakeup interrupt in BAM HW

===========================================================================*/
boolean hsu_al_bam_unregister_wakeup_event
(
  hsu_bam_connection_type* connection
);

/*===========================================================================

FUNCTION hsu_al_bam_alloc

DESCRIPTION
Allocate a buffer of pipe memory.

DEPENDENCIES
None.

RETURN VALUE
TRUE for successful allocation. FALSE otherwise.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_alloc
(
  uint32 uBytes, 
  uint32 align,
  void** virt_address, 
  void** phys_address, 
  void** handle
);

/*===========================================================================

FUNCTION hsu_al_bam_free

DESCRIPTION
Free an allocated buffer of pipe memory.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_free
(
  void* handle
);

/*===========================================================================

FUNCTION hsu_al_bam_reset

DESCRIPTION
reset BAM pipe

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_reset(void);

/*===========================================================================

FUNCTION hsu_al_bam_pipe_disable

DESCRIPTION
Disables the BAM pipe (this is a lightweight version that does not require full pipe deinit)

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_pipe_disable
(
  hsu_bam_connection_type* connection, boolean ul, boolean dl
);

/*===========================================================================

FUNCTION hsu_al_bam_utils_disable_all_pipe

DESCRIPTION
Lightweight version that disables all BAM pipes opened

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_disable_all_pipe(boolean ul, boolean dl);

#ifdef FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
/*===========================================================================
FUNCTION hsu_al_bam_utils_allow_subsequent_reset_bam

DESCRIPTION
Allow subsequent CI core bam reset to be exercised

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_al_bam_utils_allow_subsequent_reset_bam(void);

/*===========================================================================
FUNCTION hsu_al_bam_utils_reset_bam

DESCRIPTION
API to reset the CI core bam early before get descriptor

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
==========================================================================*/
void hsu_al_bam_utils_reset_bam(void);
#endif // FEATURE_HS_USB_CI_BAM_RESET_BEFORE_TRANSFER
#ifndef CI_MERGE
/*===========================================================================

FUNCTION hsu_al_bam_utils_reset_bam_and_enable_all_pipe

DESCRIPTION
Reset all BAM pipes and then enable them (lightweight without pipe de-init/init)

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_reset_bam_and_enable_all_pipe(boolean ul, boolean dl);
#endif /*CI_MERGE*/
/*===========================================================================

FUNCTION hsu_al_bam_utils_enable_all_pipes

DESCRIPTION
Reset all BAM pipes and then enable them (lightweight without pipe de-init/init)

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_enable_all_pipes(boolean ul, boolean dl);

/*===========================================================================

FUNCTION hsu_al_bam_utils_flush_all_pipe

DESCRIPTION
flush out data on all of the bam pipes opened

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_flush_all_pipe(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_get_pipes_status

DESCRIPTION
return FALSE when hsu_al_bam_utils_disable_all_pipe has been called, but
hsu_al_bam_utils_reset_all_pipe has not been called. 

DEPENDENCIES
None.

RETURN VALUE
boolean.

SIDE EFFECTS
None.

===========================================================================*/
boolean  hsu_al_bam_utils_get_dl_pipes_status(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_are_DL_pipes_empty

DESCRIPTION
Check if all BAM DL pipes are empty

DEPENDENCIES
None.

RETURN VALUE
TRUE if all BAM DL pipes are empty, and FALSE if any BAM DL pipe is found to
be not empty.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_utils_are_DL_pipes_empty(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_are_UL_pipes_empty

DESCRIPTION
Check if all BAM UL pipes are empty

DEPENDENCIES
None.

RETURN VALUE
TRUE if all BAM UL pipes are empty, and FALSE if any BAM UL pipe is found to
be not empty.

SIDE EFFECTS
None.

===========================================================================*/
boolean hsu_al_bam_utils_are_UL_pipes_empty(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_print_all_pipe

DESCRIPTION
Print BAM pipe related info to F3

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_print_all_pipe(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_core_dump

DESCRIPTION
External function to collect BAM related data in coredump.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_bam_utils_core_dump(void);

/*===========================================================================

FUNCTION hsu_al_bam_ipa_pipe_connect

DESCRIPTION
API to notify API that it can perform bam_pipe_enable( )
===========================================================================*/
void hsu_al_bam_ipa_pipe_connect(hsu_bam_connection_type* connection);

/*===========================================================================

FUNCTION hsu_al_bam_is_ipa_rdy_for_pipe_connect

DESCRIPTION
Query whether IPA is ready to receive a hsu_al_bam_ipa_pipe_connect( ) API call
===========================================================================*/
boolean hsu_al_bam_is_ipa_rdy_for_pipe_connect(void);


/*===========================================================================

FUNCTION hsu_al_bam_utils_peer_all_pipe_disable

DESCRIPTION
Indication to peer BAMs  that they should cease data flow.
===========================================================================*/
void hsu_al_bam_utils_peer_all_pipe_disable(boolean ul, boolean dl);

/*===========================================================================

FUNCTION hsu_al_bam_utils_peer_all_pipe_enable

DESCRIPTION
Indication to peer BAMs  that they should start data flow.
===========================================================================*/
void hsu_al_bam_utils_peer_all_pipe_enable(boolean ul, boolean dl);

/*===========================================================================

FUNCTION hsu_al_bam_utils_ipa_state_chg_suspend

DESCRIPTION
Notify IPA of a state change to suspend.
===========================================================================*/
boolean hsu_al_bam_utils_ipa_state_chg_suspend(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_ipa_state_chg_resume

DESCRIPTION
Notify IPA of a state change to resume.
===========================================================================*/
void hsu_al_bam_utils_ipa_state_chg_resume(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_ipa_pm_release_clock 

DESCRIPTION
Notify IPA that it may perform IPA clock gating.
===========================================================================*/
void hsu_al_bam_utils_ipa_pm_release_clock(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_ipa_pm_request_clk

DESCRIPTION
Notify IPA that USB requires CLK resource (undo IPA clock gating).
===========================================================================*/
void hsu_al_bam_utils_ipa_pm_request_clk(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_dbm_init

DESCRIPTION
Initialize the DBM.
===========================================================================*/
void hsu_al_bam_utils_dbm_init(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_all_dbm_ep_init

DESCRIPTION
Initialize all DBM EPs.
===========================================================================*/
void hsu_al_bam_utils_all_dbm_ep_init(boolean ul, boolean dl);

/*===========================================================================

FUNCTION hsu_al_bam_utils_all_dbm_ep_reset

DESCRIPTION
Reset all DBM EPs.
===========================================================================*/
void hsu_al_bam_utils_all_dbm_ep_reset(boolean ul, boolean dl);

/*===========================================================================

FUNCTION hsu_al_bam_utils_init_all_pipes_for_ipa_and_bam

DESCRIPTION
Initialize all bam pipes (bam_pipe_init) for both IPA and USB.
===========================================================================*/
void hsu_al_bam_utils_init_all_pipes_for_ipa_and_bam(void);

/*===========================================================================

FUNCTION hsu_al_bam_utils_all_ipa_pipe_connect

DESCRIPTION
Notifies IPA that it can perform BAM pipe enable (used once at bootup)
===========================================================================*/
void hsu_al_bam_utils_all_ipa_pipe_connect(void);

/*===========================================================================

FUNCTION hsu_al_bam_init_pipes_for_ipa_and_bam

DESCRIPTION
BAM pipe init for both IPA and USB pipes
===========================================================================*/
void hsu_al_bam_init_pipes_for_ipa_and_bam(hsu_bam_connection_type* connection);

/*===========================================================================

FUNCTION hsu_al_bam_init_loopback_pipes

DESCRIPTION
Setup the pipes for USB self loopback
===========================================================================*/
void hsu_al_bam_init_loopback_pipes(hsu_bam_connection_type* connection);

/*===========================================================================
FUNCTION hsu_al_bam_enable_read_write_infinite_qdss_bam_ep

DESCRIPTION
Enables BAM flow on all h/w accelerated endpoints.
===========================================================================*/
void hsu_al_bam_enable_read_write_infinite_qdss_bam_ep(void);

/*===========================================================================
hsu_al_bam_utils_ipa_per_drop
===========================================================================*/
void hsu_al_bam_utils_ipa_per_drop(void);

/*===========================================================================
hsu_al_bam_utils_ipa_per_enable_clear_data_path
===========================================================================*/
void hsu_al_bam_utils_ipa_per_enable_clear_data_path(void);

/*===========================================================================
hsu_al_bam_utils_ipa_per_disable_clear_data_path
===========================================================================*/
void hsu_al_bam_utils_ipa_per_disable_clear_data_path(void);

/*===========================================================================
hsu_al_bam_utils_ipa_per_undrop
===========================================================================*/
void hsu_al_bam_utils_ipa_per_undrop(void);

/*===========================================================================
hsu_al_bam_utils_ipa_notify_core_speed
===========================================================================*/
void hsu_al_bam_utils_ipa_notify_core_speed(void);
#endif /* FEATURE_HS_USB_BAM */
#endif /* _HSU_AL_BAM_UTILS_H_ */
