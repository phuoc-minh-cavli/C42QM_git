/* Copyright (c) 2016 The Linux Foundation. All rights reserved.
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

#ifndef LOC_LOG_H
#define LOC_LOG_H

#include <comdef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================
FUNCTION POINTER loc_diag_client_cb_fn

DESCRIPTION
  This callback would be invoked when corresponding DIAG command is
  entered in QXDM.
  The callback is registered by client along with a uint8 fn_id;
  When below command is invoked,
  send_data 75 13 101 0 <fn_id> <cmd_data_0> <cmd_data_1> <cmd_data_2> ...
  e.g.
  send_data 75 13 101 0 4 1 0 1
  Client would get this callback invoked with:
  fn_id = 4
  cmd_data = [1,0,1]
  cmd_data_len = 3

This callback would be invoked in DIAG thread, switch context before
performing long running operations.
===========================================================================*/
typedef void (*loc_diag_client_cb_fn)(
        uint8 fn_id,
        uint8* cmd_data_ptr,
        uint8 cmd_data_len);

/*===========================================================================
FUNCTION loc_diag_register_cb_fn

DESCRIPTION
  This function is meant to register callback for a specific fn_id
===========================================================================*/
void loc_diag_register_cb_fn(uint8 fn_id, loc_diag_client_cb_fn cb_fn);

/*===========================================================================
FUNCTION loc_diag_init()

DESCRIPTION
  This function will initialize the LOC DIAG module.
  It registers with the DIAG module for the DIAG_SUBSYS_GPS subsystem along
  with the command codes specified in loc_diag_cmd_tbl
===========================================================================*/
void loc_diag_init(void);


#ifdef __cplusplus
}
#endif

#endif /* LOC_LOG_H */
