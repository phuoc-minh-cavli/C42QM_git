/*===========================================================================

           T M S _ E D L . C

DESCRIPTION
  Interface for handling diag command to switch device into Emergency Download Mode

Copyright (c) 2017-18 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/22/17   yog     File Created

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "diagcmd.h"
#include "tms_utils_msg.h"
#include "diagpkt.h"
#include "dog.h"
#include "tz_syscalls.h"
#include "qurt.h"
#include "timer.h"

#define EDL_DELAY_DEFAULT_MSEC  50
#define TMS_EDL_MAGIC_NUM  0x01 //0th Bit needs to be set in TZ reg to put device in EDL
#define TMS_EDL_MAGIC_SPARE  0x00 //spare field in sec_dload_mode_buffer_t

#define TMS_EDL_DIAG_PROC_ID  DIAG_APP_PROC

/*===========================================================================

                     EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
PACK(void *) tms_edl_diag (PACK(void *), uint16);
extern void dog_force_bite(void);
extern unsigned int qurt_fiq_lock_save(void);

/*===========================================================================

                     INTERNAL DATA DECLARATIONS

===========================================================================*/

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
  uint8 edl_delay;
} tms_edl_req_type;

typedef enum
{
  TMS_DIAGPKT_EDL_DEPRECATED_0,
  TMS_DIAGPKT_EDL_CMD
}TMS_EDL_CMD;

/* table for diag packet command handler registration */
static const diagpkt_user_table_entry_type tms_edl_diagpkt_tbl[] = 
{
  {TMS_DIAGPKT_EDL_CMD, TMS_DIAGPKT_EDL_CMD, tms_edl_diag}
};

static boolean cmd_pending = FALSE;

static timer_type       edlTimer;
static timer_group_type edl_timer_group = {0};

/*===========================================================================

                        Private function prototypes

===========================================================================*/

static void tms_edl_execute(timer_cb_data_type unused);

/*===========================================================================

                              Function definitions

===========================================================================*/
/*===========================================================================

FUNCTION tms_edl_send_generic_response

DESCRIPTION
  Sends the response to diag

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void tms_edl_send_generic_response (PACK(void *) req)
{
  PACK(void *) rsp = NULL;
  /* build response */
  rsp = diagpkt_subsys_alloc(
     diagpkt_subsys_get_id(req),
     diagpkt_subsys_get_cmd_code(req), 
     sizeof( diagpkt_subsys_header_type ));

  // send response 
  diagpkt_commit(rsp);
}

/*===========================================================================

FUNCTION tms_edl_init

DESCRIPTION
  Initializes the TMS EDL feature

DEPENDENCIES
  diag must be available to register command handler

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tms_edl_init (void)
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (TMS_EDL_DIAG_PROC_ID, DIAG_SUBSYS_SYSTEM_OPERATIONS, tms_edl_diagpkt_tbl);

  //initialize timer group to be safe
  memset(&edl_timer_group, 0, sizeof(edl_timer_group));
  timer_def_osal(&edlTimer,
                 &edl_timer_group,
                 TIMER_FUNC1_CB_TYPE,
                 (time_osal_notify_obj_ptr)tms_edl_execute,
                 (time_osal_notify_data)NULL);
}

/*===========================================================================

FUNCTION tms_edl_diag

DESCRIPTION
  Diag command handler to put device into EDL mode.
  Last parameter indicates delay (range is 0 to 255)  .

  Example:
  send_data 75 101 01 00 255  //255 ms delay
  send_data 75 101 01 00 00  //0 ms delay
  send_data 75 101 01 00 05  //5 ms delay

  If "send_data 75 101 01 00" will be passed(without delay parameter)
  then we have defined default 50 ms delay.

DEPENDENCIES
  err_init must be called prior

RETURN VALUE
  Pointer to the response packet of the req_ptr

SIDE EFFECTS

===========================================================================*/
PACK(void *) tms_edl_diag (PACK(void *) req_ptr, uint16 pkt_len)
{
  uint8 edl_delay = EDL_DELAY_DEFAULT_MSEC;
  tms_edl_req_type *req = NULL;

  if (cmd_pending)
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"tms_edl: cmd pending, ignoring subsequent request");
    tms_edl_send_generic_response(req_ptr);
    return(NULL);
  }

  cmd_pending = TRUE;  //set before diag response to guarantee no reentrancy issues

  // decode request
  req = (tms_edl_req_type *) req_ptr;

  if ( pkt_len == sizeof(req->hdr) )
  {
    /* No delay is mentioned in command so using default delay*/
    edl_delay = EDL_DELAY_DEFAULT_MSEC;
  }
  else if ( pkt_len == (sizeof(req->hdr) + sizeof(req->edl_delay)) )
  {
    edl_delay = req->edl_delay;
  }
  else
  {
    MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"tms_edl: length of diagpkt is not proper");
    cmd_pending = FALSE;
    tms_edl_send_generic_response(req_ptr);
    return(NULL);
  }

  tms_edl_send_generic_response(req_ptr);

  //set callback timer to trigger actual EDL since diag task need some time to report back response
  MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH,"tms_edl: timer set %d ",edl_delay);
  timer_set_64(&edlTimer, (timetick_type)edl_delay, 0, T_MSEC);

  return(NULL);  //return NULL since we already called diagpkt_commit above
}

/*===========================================================================

FUNCTION tms_edl_write_magic_numbers

DESCRIPTION
  makes tz syscall to write EDL magic number in TZ register

DEPENDENCIES
  scm_init must be called prior

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void tms_edl_write_magic_numbers(void)
{
   sec_dload_mode_buffer_t cmd_buffer = {TMS_EDL_MAGIC_NUM, TMS_EDL_MAGIC_SPARE};
   tz_config_dload_mode_syscall (&cmd_buffer);
}

/*===========================================================================

FUNCTION tms_edl_execute

DESCRIPTION
  Initiates the tms edl

DEPENDENCIES
  err_init must be called prior

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
static void tms_edl_execute(timer_cb_data_type unused)
{
   unsigned int tms_int_flag = 0;

   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH,"tms_edl: timer cb executed ");
   tms_edl_write_magic_numbers();

   /* disable interrupts */
   INTLOCK_SAVE(tms_int_flag); 

   /* Disable FIQ as well */
   /* Not saving the context since we will not be needing to re-enable it */
   (void)qurt_fiq_lock_save();

   dog_force_bite();

   /* Intentionally petting dog */
   dog_force_kick();

   /* For supressing compiler warning errors */
   tms_int_flag = tms_int_flag+1;

   /* Halt the execution here */
   while(1){};
}
