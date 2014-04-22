/*===========================================================================

GENERAL DESCRIPTION
  This file contains functions and other defines used for
  image version reporting feature.

Copyright 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/debugtools/version/src/version_diagpkt.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

=============================================================================*/


/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "smem.h"
#include "string.h"

#include "osal.h"
#include "diagpkt.h"
#include "diagcmd.h"

#include "version.h"
#include "image_version.h"

#define VERSION_DIAGPKT_PROCID            0x80              // VERSION_PROCID 128
#define VERSION_DIAGPKT_SUBSYS            0x63              // VERSION_SUBSYS 99
#define VERSION_DIAGPKT_PREFIX            0x00              // VERSION_PREFIX 0

#define VERSION_SMEM_SIZE                 (IMAGE_INDEX_RESERVED15+1)*SMEM_IMAGE_VERSION_ENTRY_SIZE

typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;        // diag transport header required first

   uint8    cmd;                          // (opt) command
   uint16   len;                          // (opt) data length
   uint8    dat[VERSION_SMEM_SIZE];       // (opt) data

} version_cmd_req_t, *version_cmd_req_p;

typedef PACK(struct)
{
   diagpkt_subsys_header_type hdr;        // diag transport header required first

   uint16   len;                          // data length
   uint8    dat[VERSION_SMEM_SIZE];       // data

} version_cmd_rsp_t, *version_cmd_rsp_p;

PACK(void*) diagpkt_handler(PACK(void*) req_ptr, uint16 pkt_len); // forward reference

static const diagpkt_user_table_entry_type diagpkt_tbl[] =
{
   { VERSION_DIAGPKT_PREFIX, VERSION_DIAGPKT_PREFIX, diagpkt_handler }
};

static uint32 version_table_s = 0;
static uint8* version_table_p = NULL;

// DIAGPKT Handler

static PACK(void*) diagpkt_handler(PACK(void*) req_ptr, uint16 pkt_len)
{
   version_cmd_req_p req;
   version_cmd_rsp_p rsp = NULL;

   req = (version_cmd_req_t*)req_ptr;

   if (NULL != req)
   {
      if (pkt_len <= sizeof(req->hdr)) // no command, no payload
      {
         // NULL
      }

      else if (pkt_len <= sizeof(req->hdr) + sizeof(req->cmd)) // command, no payload
      {
         if (0x00 == req->cmd) // cmd 0
         {
            rsp = (version_cmd_rsp_p)diagpkt_subsys_alloc(VERSION_DIAGPKT_SUBSYS, VERSION_DIAGPKT_PREFIX, sizeof(version_cmd_rsp_t));

            if (NULL != rsp)
            {
               if (NULL != version_table_p)
               {
                  memcpy(rsp->dat, version_table_p, sizeof(rsp->dat));
               }
               else
               {
                  memset(rsp->dat, NULL, sizeof(rsp->dat));
               }

               rsp->len = sizeof(rsp->dat);

               diagpkt_commit(rsp);
            }
         }
      }

      else if (pkt_len <= sizeof(req->hdr) + sizeof(req->cmd) + sizeof(req->len)) // command, payload
      {
         // NULL
      }
   }

   return (rsp);
}

void version_diagpkt_init(void)
{
   version_table_p = (uint8*) smem_get_addr(SMEM_IMAGE_VERSION_TABLE, &version_table_s);

   if (NULL != version_table_p)
   {
      DIAGPKT_DISPATCH_TABLE_REGISTER_PROC(VERSION_DIAGPKT_PROCID, VERSION_DIAGPKT_SUBSYS, diagpkt_tbl);
   }
}
