#ifndef _HSU_AL_SER_OBEX_H_
#define _HSU_AL_SER_OBEX_H_

/*==============================================================================

Serial Functions over OBEX Adaptation Layer for High Speed USB
H E A D E R   F I L E

GENERAL DESCRIPTION
Contains the API for the serial functions over OBEX adapt. layer

EXTERNALIZED FUNCTIONS
hsu_al_ser_obex_populate_obex_specific_data

INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_al_ser_obex_init must be called for each function instance, 
prior to performing any other operations on the instance.

Copyright (c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_obex.h#1 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/29/06 esh  First revision of file    
==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "customer.h" /* For FEATURE_HSU_TEST to work */

#include "jtypes_int.h"
#include "hsu_al_ser_base_int.h"

#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"

#ifdef FEATURE_HS_USB_OBEX_BAM
    #include "hsu_al_bam_utils.h"         
#endif

#ifdef FEATURE_HSU_TEST

#include "sio.h"
#include "dsm.h"

#endif /* FEATURE_HSU_TEST */

/*==============================================================================
Typedefs
==============================================================================*/
#ifdef FEATURE_HS_USB_OBEX_BAM
typedef struct
{
  boolean is_bam_mode;
  boolean bam_flow_enabled;
  boolean bam_pipes_allocated;
  /* when working in BAM mode (is_bam_mode==TRUE), indicate if BAM mode was 
  enabled including setup of BAM read and write eTDs */
  hsu_bam_connection_type bam_connection;
} hsu_al_obex_bam_info_type;
#endif /* FEATURE_HS_USB_OBEX_BAM */
typedef struct
{
  boolean             dte_ready;
  /* A flag to indicated if DTE is ready or not */

  sio_vv_func_ptr_type  app_dtr_changed_cb_func;
  /* App callback to be invoked whenever DTR state is changed */

  uint8                core_idx;                  
  /* The core index of this OBEX FD */

#ifdef FEATURE_HS_USB_OBEX_BAM
  hsu_al_obex_bam_info_type bam_info;
  /* holds BAM information */
#endif /* FEATURE_HS_USB_OBEX_BAM */

} obex_specific_data_struct;
/* Data unique for the obex function */

typedef struct  
{
  hsu_al_ser_base_data_for_ser_if     base_data;
  obex_specific_data_struct           obex_if_data;
} hsu_al_ser_obex_data; 
/* Information needed from an OBEX I/F */

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*===========================================================================

FUNCTION hsu_al_ser_obex_populate_obex_specific_data

DESCRIPTION
This function, called following obex_init, populates the various fields 
needed by the OBEX FD.  

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_obex_populate_obex_specific_data
(
  hsu_al_ser_obex_data* obex_data_ptr,           /* Pointer to OBEX Data struct */
  cdc_handle_t handle,                           /* Given OBEX FD handle        */
  obex_callbacks_t* cbs,                         /* output param: OBEX callback 
                                                    struct to populate          */
  obex_init_info_t* info,                        /* output param: OBEX info struct 
                                                    to populate                 */
  cdc_appctx_t* ctx,                             /* output param: Context to be 
                                                    passed by OBEX FD in various 
                                                    callbacks                   */
  uint8 instance_number                          /* Used to distinguish the
                                                    NMEA instance of OBEX       */
);

/*===========================================================================
FUNCTION hsu_al_ser_obex_open_bam() 
 
DESCRIPTION 
Should be called with Giant mutex locked! 
 
DEPENDENCIES

RETURN VALUE
TRUE - Success FALSE - Otherwise

SIDE EFFECTS
None 
===========================================================================*/
boolean hsu_al_ser_obex_open
(
  hsu_al_ser_obex_data  *obex_data_ptr,
  const sio_open_type   *open_ptr,
  dsm_mempool_id_enum_type pool_id,
  juint8_t              rx_buff_cnt,
  uint32                rx_timeout,
  uint32                tx_max_pending_chain_cnt,
  uint32                tx_timeout,
  uint32                tx_complete_timer_val
);
/*===========================================================================

FUNCTION hsu_al_ser_obex_ioctl

DESCRIPTION
Allows passing an I/O control command to the serial I/F using CDC/OBEX.
Note that usually this would simply call the base class method. However,
in order to support Shared Memory Driver (SMD), these I/Fs need to handle
the SMD's I/O Ctls, although they have nothing to do with OBEX.

DEPENDENCIES
None

RETURN VALUE
TRUE - I/O control command handled. FALSE - Otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_obex_ioctl
(
  hsu_al_ser_obex_data* obex_data_ptr,       /* Pointer to OBEX Data struct*/
  sio_ioctl_cmd_type ioctl_cmd,              /* SIO I/O control command    */
  sio_ioctl_param_type* ioctl_param_ptr      /* SIO I/O control command's 
                                                parameter                  */
);

/*===========================================================================

FUNCTION hsu_al_ser_obex_init

DESCRIPTION
Initializes OBEX AL specific fields.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_obex_init
(
 hsu_al_ser_obex_data* obex_data_ptr          /* Pointer to OBEX Data struct */
);

/*===========================================================================

FUNCTION hsu_al_ser_obex_uninit

DESCRIPTION
Serves as a callback whenever the CDC FD wishes to notify it is being 
un-initialized, or as an initializer for the given context when the very first 
hsu_al_ser_base_init is called.

DEPENDENCIES
See cdc_uninit_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
The data structure corresponding to the given context is invalidated.

===========================================================================*/
jresult_t hsu_al_ser_obex_uninit
(
 cdc_appctx_t ctx                           /* Context passed in callback */
);

/*===========================================================================

FUNCTION hsu_al_ser_obex_enabled

DESCRIPTION
Enabled callback whenever the serial CDC FD wishes to notify that the interface
is enabled.

DEPENDENCIES
See cdc_enabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_obex_enabled
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 );

/*===========================================================================

FUNCTION hsu_al_ser_obex_disabled

DESCRIPTION
Disabled callback whenever the serial CDC FD wishes to notify that the interface
is disabled.

DEPENDENCIES
See cdc_disabled_cb for more information

RETURN VALUE
HSU_JSUCCESS - Always

SIDE EFFECTS
None

===========================================================================*/
jresult_t hsu_al_ser_obex_disabled
(
 cdc_appctx_t ctx                           /* Context passed in callback */
 );

/*===========================================================================

FUNCTION hsu_al_ser_obex_close

DESCRIPTION
This function closes an open stream of a obex serial interface.
It allows an optional callback to notify caller when all the bookkeeping 
needed for closing the stream (flushing Tx, aborting Rx, etc.) 
has been completed.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_ser_obex_close
(
 hsu_al_ser_obex_data* obex_data_ptr,    
 /* Pointer to serial obex data struct                      */
 hsu_al_ser_base_op_complete_cb close_cb,  /* Callback to be invoked 
                                           when close has been completed */
                                           boolean abort_io_operations_immediately   /* Whether or not to abort all 
                                                                                     read/write operations 
                                                                                     immediately or wait until all 
                                                                                     Tx data is flushed, followed 
                                                                                     by an abort read              */
                                                                                     );

/*===========================================================================
FUNCTION hsu_al_ser_obex_transmit

DESCRIPTION
This function transmit a DSM chain over CDC/OBEX FD.

DEPENDENCIES
Should be called with the Giant mutex locked

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_ser_obex_transmit
( 
  hsu_al_ser_obex_data* obex_data_ptr,
  dsm_item_type *tx_ptr
);
/*===========================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE UNIT TESTS (ut)
===========================================================================*/
#ifdef FEATURE_HSU_TEST
/*===========================================================================

FUNCTION hsu_al_ser_obex_ut_check_open_params

DESCRIPTION
This function will check, following an sio_open command, if all the parameters
have been updated/set correctly. This is part of the open unit test.
If an error was encountered, returns FALSE and a description string.

DEPENDENCIES
None

RETURN VALUE
TRUE - Passed. FALSE (and returned_error_str as output param).

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_ser_obex_ut_check_open_params
(
  const hsu_al_ser_obex_data* obex_data_ptr,  /* Pointer to OBEX Data 
                                                   struct                  */
  const sio_open_type *open_ptr,                /* Configurations of stream*/
  dsm_mempool_id_enum_type pool_id,             /* Pool ID from where to take 
                                                   DSM items               */
  char* returned_error_str,                     /* output param: Error 
                                                   description string (if 
                                                   applicable)             */
  uint8 error_str_size                          /* Size of given string    */
);
#endif /* FEATURE_HSU_TEST */

#endif /* _HSU_AL_SER_OBEX_H_ */
