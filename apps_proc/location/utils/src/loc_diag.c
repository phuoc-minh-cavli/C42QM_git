/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "diagcmd.h"
#include "diagpkt.h"
#include <loc_util_log.h>
#include <qurt_thread.h>
#include "timer.h"
#include "loc_diag.h"
#include <pthread_loc.h>
#include "stringl.h"


/* This is the command code for LOC Module
 * diagcmd.h
 * DIAG_SUBSYS_GPS = 13,   //Global Positioning System
 * send_data 75 13 105 0 <data-bytes>
 * data-bytes : sequence of bytes which can be used to control
 * test app or whatever else is needed.
 */
typedef enum
{

  LOC_DIAG_CMD_TEST_APP_CONTROL = 105

} loc_diag_cmd_type;

/* Size of header added to each DIAG request/response */
#define LOC_DIAG_HDR_LEN  (sizeof(diagpkt_subsys_header_type))

/* Offset in diag header where the subsystem command code is found */
#define LOC_DIAG_HDR_SSCC_OFFSET 2

/* Minimum number of data bytes that we expect
 * At least one byte for the fn_id; and one for switch ctx flag */
#define LOC_DIAG_CMD_MIN_DATA_LEN 2

/* Callback declaration */
PACKED void *loc_diag_cb
(
  PACKED void  * req_pkt,
  uint16         pkt_len
);

#define LOC_DIAG_CB_DATA_MAX_LEN 16
typedef struct loc_diag_cb_data_s{

    uint8 fn_id;
    uint8 data[LOC_DIAG_CB_DATA_MAX_LEN];
    uint8 data_len;

}loc_diag_cb_data_s;
static loc_diag_cb_data_s loc_diag_cb_data;

static const diagpkt_user_table_entry_type loc_diag_cmd_tbl[] =
{
  {
   LOC_DIAG_CMD_TEST_APP_CONTROL,
   LOC_DIAG_CMD_TEST_APP_CONTROL,
   loc_diag_cb
  }
};

/* Global list for loc_diag callback functions */
#define LOC_DIAG_CB_FN_ARR_SIZE 8
static loc_diag_client_cb_fn loc_diag_cb_fn_arr[LOC_DIAG_CB_FN_ARR_SIZE];

/* Is DIAG init done */
static uint8 loc_diag_init_done = 0;
uint8 loc_diag_cb_pending = 0;


/*===========================================================================
FUNCTION loc_diag_register_cb_fn

DESCRIPTION
  This function is meant to register callback for a specific fn_id
===========================================================================*/
void loc_diag_register_cb_fn(uint8 fn_id, loc_diag_client_cb_fn cb_fn){

    if(loc_diag_cb_pending){

        LOC_LOGE0("loc_diag_register_cb_fn(): cb pending");
        return;
    }

    if(fn_id >= LOC_DIAG_CB_FN_ARR_SIZE){

        LOC_LOGE1("loc_diag_register_cb_fn(): fn_id too large %d", fn_id);
        return;
    }

    if(loc_diag_cb_fn_arr[fn_id] != NULL){

        LOC_LOGE1("loc_diag_register_cb_fn(): "
                "fn_id %d already registered, overwriting", fn_id);
    }

    loc_diag_cb_fn_arr[fn_id] = cb_fn;

    LOC_LOGM1("loc_diag_register_cb_fn(): "
            "cb for fn_id %d registered", fn_id);
}

/*===========================================================================
FUNCTION loc_diag_init()

DESCRIPTION
  This function will initialize the LOC DIAG module.
  It registers with the DIAG module for the DIAG_SUBSYS_GPS subsystem along
  with the command codes specified in loc_diag_cmd_tbl
===========================================================================*/
void loc_diag_init()
{
/*-------------------------------------------------------------------------*/

  LOC_LOGM0("loc_diag_init()");

  if(loc_diag_init_done){

      LOC_LOGH0("loc_diag_init(): INIT already done.");
      return;
  }

  /* Register with DIAG module to receive callback once LOC DIAG command
     is entered by user in QXDM */
  DIAGPKT_DISPATCH_TABLE_REGISTER( (word)DIAG_SUBSYS_GPS,
                                   loc_diag_cmd_tbl );

  loc_diag_init_done = 1;
}

#define LOC_DIAG_THREAD_STACK_SIZE (8*2048)
static qurt_thread_attr_t loc_diag_thread_attr;
static qurt_thread_t loc_diag_tcb;
/*===========================================================================
FUNCTION loc_diag_thread_entry()

DESCRIPTION
  This is the entry function for loc diag thread.
===========================================================================*/
void loc_diag_thread_entry(void* unused){

    int i = 0;

    LOC_LOGH0("loc_diag_thread_entry(): invoking client cb");

    loc_diag_cb_fn_arr[loc_diag_cb_data.fn_id](
            loc_diag_cb_data.fn_id,
            loc_diag_cb_data.data, loc_diag_cb_data.data_len);
    loc_diag_cb_pending = 0;

    LOC_LOGH0("loc_diag_thread_entry(): "
              "client cb complete, terminating thread.");

    // Keep thread alive for some time;
    for(i=0; i<40; i++){

        timer_sleep(50, T_MSEC, FALSE);
        //LOC_LOGL1("LocNetIface_test(): alive for %d sec", i);
    }
}
/*===========================================================================
FUNCTION loc_diag_invoke_client_cb()

DESCRIPTION
  This function will invoke client callback.
  If switch_ctx is 0, callback would be invoked in current DIAG context.
  Else, a new thread would be spawned and callback would be invoked in
  that.
===========================================================================*/
void loc_diag_invoke_client_cb(
        uint8 fn_id, uint8 switch_ctx,
        uint8* data_ptr, uint8 data_len){

    // check if pending
    if(loc_diag_cb_pending){

        LOC_LOGE0("loc_diag_invoke_client_cb(): have an ongoing cb !");
        return;
    }

    // set pending cb to true
    loc_diag_cb_pending = 1;

    // If no need to switch context, invoke cb immediately
    if(switch_ctx == 0){

        LOC_LOGM0("loc_diag_invoke_client_cb(): switch_ctx 0");
        loc_diag_cb_fn_arr[fn_id](fn_id, data_ptr, data_len);
        loc_diag_cb_pending = 0;
        return;
    }

    // Save fn_id and data
    memset(&loc_diag_cb_data, 0, sizeof(loc_diag_cb_data));
    loc_diag_cb_data.fn_id = fn_id;
    memscpy(loc_diag_cb_data.data, sizeof(loc_diag_cb_data.data),
            data_ptr, data_len);
    loc_diag_cb_data.data_len = data_len;

    /* this cb is in DIAG thread; and switch_ctx flag is 1
     * we should switch thread before invoking callback */
    LOC_LOGM0("loc_diag_invoke_client_cb(): "
              "Spawning new thread to invoke client callback.");
    qurt_thread_attr_init(&loc_diag_thread_attr);
    qurt_thread_attr_set_name(&loc_diag_thread_attr,
        (char*)"loc_diag_thread");
    qurt_thread_attr_set_priority(&loc_diag_thread_attr, pthread_get_loc_thread_prio() + 1);
    qurt_thread_attr_set_stack_size(
            &loc_diag_thread_attr, LOC_DIAG_THREAD_STACK_SIZE);
    qurt_thread_create(
            &loc_diag_tcb,
            &loc_diag_thread_attr,
            loc_diag_thread_entry,
            NULL);
}

/*===========================================================================
FUNCTION loc_diag_cb()

DESCRIPTION
  This function is the registered subsystem command handler for loc diag
  handling.

  This is invoked in the DIAG context.
  We should switch context to our LOC Thread before any processing.

  Sample usage from QXDM to invoke this callback:
  send_data 75 13 101 0 <data-bytes>

PARAMETERS
  *req_pkt: A pointer to the subsystem command packet.
  pkt_len:  Length of the subsystem command packet.

RETURN VALUE
  None

DEPENDENCIES
  This function must have been registered in the diag table in order to be
  invoked when the appropriate subsystem command is recognized.
===========================================================================*/
PACKED void *loc_diag_cb
(
  PACKED void  * req_pkt,
  uint16         pkt_len
)
{
  uint8*   request_ptr = NULL;
  uint8    fn_id = 0;
  uint8    switch_ctx = 0;
  uint16   data_len = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  LOC_LOGM2("loc_diag_cb(): req_pkt 0x%p, pkt_len %d",
            req_pkt, pkt_len);

  /*-------------------------------------------------------------------------
    Validate the request
  -------------------------------------------------------------------------*/
  if( req_pkt == NULL )
  {
    LOC_LOGM0("loc_diag_cb(): NULL req_pkt" );
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }
  if( pkt_len < LOC_DIAG_HDR_LEN + LOC_DIAG_CMD_MIN_DATA_LEN )
  {
    LOC_LOGM1("loc_diag_cb(): pkt len %d", pkt_len );
    return (diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Print the bytes received as req_pkt
    Printing only first 6 bytes as that is the minimum possible length
  -------------------------------------------------------------------------*/
  request_ptr = (uint8*)req_pkt;
  LOC_LOGM6("loc_diag_cb(): req_pkt initial 6 bytes: %d %d %d %d %d %d",
            *(request_ptr), *(request_ptr + 1),
            *(request_ptr + 2), *(request_ptr + 3),
            *(request_ptr + 4), *(request_ptr + 5) );

  /*-------------------------------------------------------------------------
    Skip Diag header of request pkt
  -------------------------------------------------------------------------*/
  request_ptr = (uint8 *)req_pkt + LOC_DIAG_HDR_LEN;

  /*-------------------------------------------------------------------------
    request_ptr now points to the first data-byte, = fn_id
  -------------------------------------------------------------------------*/
  fn_id = *((uint8*)request_ptr);
  if(fn_id >= LOC_DIAG_CB_FN_ARR_SIZE){

      LOC_LOGE2("fn_id too large %d, limit %d",
                fn_id, LOC_DIAG_CB_FN_ARR_SIZE);
      return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }
  if(loc_diag_cb_fn_arr[fn_id] == NULL){

      LOC_LOGE1("No cb fn for fn_id %d", fn_id);
      return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  /*-------------------------------------------------------------------------
    Invoke the cb fn in temp DIAG thread
  -------------------------------------------------------------------------*/
  request_ptr++;
  switch_ctx = *((uint8*)request_ptr);
  request_ptr++;
  data_len = pkt_len - LOC_DIAG_HDR_LEN - 2;

  // data should not be more than 16 bytes
  if(data_len > 16){

      LOC_LOGE1("loc_diag_cb(): Too many data bytes %d", data_len);
      return (diagpkt_err_rsp(DIAG_BAD_CMD_F, req_pkt, pkt_len));
  }

  LOC_LOGM1("loc_diag_cb(): Client Data Len %d", data_len);

  // -2 : 1 byte fn_id, 1 byte switch context needed or not
  loc_diag_invoke_client_cb(
          fn_id, switch_ctx, request_ptr, (uint8)data_len);

  /*-------------------------------------------------------------------------
    Return success at the end of processing.
  -------------------------------------------------------------------------*/
  return diagpkt_subsys_alloc((diagpkt_subsys_id_type)DIAG_SUBSYS_GPS,
                                    LOC_DIAG_CMD_TEST_APP_CONTROL,
                                    LOC_DIAG_HDR_LEN);
} /* loc_diag_cb */
