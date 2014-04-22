/*===========================================================================

===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2018 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who   what, where, why
--------   ---   -----------------------------------------------------------
07/25/18   bcho  Initial Revision

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/
#include "uim_variation.h"
#include "diagcmd.h"
#include "uimdiag_mmgsdi.h"
#include "uimdiag_mmgsdi_util.h"
#include "uim_msg.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
static const void* uimdiag_req_ptr;
static uint32 uimdiag_req_len;
static uint32 uimdiag_offset;
static uimdiag_return_enum_type uimdiag_decode_status;

/*=============================================================================

                              FUNCTIONS

=============================================================================*/

/* ==========================================================================
   FUNCTION:      UIMDIAG_UTIL_CONVERT_FROM_MMGSDI_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding uimdiag_file_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     uimdiag_file_enum_type


   SIDE EFFECTS:
     None
==========================================================================*/
uimdiag_file_enum_type uimdiag_util_convert_from_mmgsdi_items_enum (
  mmgsdi_file_enum_type mmgsdi_file
)
{
  (void)mmgsdi_file;
  return UIMDIAG_NO_FILE_ENUM;
} /* uimdiag_util_convert_from_mmgsdi_items_enum */


/* ==========================================================================
   FUNCTION:      UIMDIAG_UTIL_CONVERT_TO_MMGSDI_ITEMS_ENUM

   DESCRIPTION:
     This function checks for valid range of the mmgsdi_file_enum_type and
     returns the corresponding mmgsdi_enum_type

   DEPENDENCIES:
     None

   LIMITATIONS:
     None

   RETURN VALUE:
     mmgsdi_return_enum_type

     MMGSDI_NO_FILE_ENUM     : uim item not found in the conversion table


   SIDE EFFECTS:
     None
==========================================================================*/
mmgsdi_file_enum_type uimdiag_util_convert_to_mmgsdi_items_enum (
  uimdiag_file_enum_type uimdiag_mmgsdi_file
)
{
  (void)uimdiag_mmgsdi_file;
  return MMGSDI_NO_FILE_ENUM;
} /* uimdiag_util_convert_to_mmgsdi_items_enum */


/*===========================================================================
FUNCTION uimdiag_mmgsdi_util_init_decode_params

DESCRIPTION
  This function should be called before any structure is decoded.

  This function copies diag_req_ptr,diag_req_len to static variables
  and initializes the offset and uimdiag_decode_status.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_util_init_decode_params(
    const void                                         * diag_req_ptr,
    uint32                                               diag_req_len)
{
  uimdiag_req_ptr = diag_req_ptr;
  uimdiag_req_len = diag_req_len;
  uimdiag_decode_status = UIMDIAG_SUCCESS;
  uimdiag_offset = 0;
} /* uimdiag_mmgsdi_util_decode_uint8 */


/*===========================================================================

FUNCTION uimdiag_mmgsdi_util_decode_uint32

DESCRIPTION
  This function copies uint32 from diag_req_ptr to cmd_ptr,
  from offset value.
  Before copy, it checks diag_req_len, to validate the data in diag_req_ptr.
  The offset is updated after the copy.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void uimdiag_mmgsdi_util_decode_uint32(void *cmd_ptr)
{
    if (uimdiag_decode_status != UIMDIAG_SUCCESS)
    {
      return;
    }
    if(uimdiag_offset + sizeof(uint32) > uimdiag_req_len)
    {
      UIMDIAG_MSG_ERROR_0("uimdiag_mmgsdi_util_decode_uint32- Cmd error");
      uimdiag_decode_status = UIMDIAG_ERROR;
      return;
    }
    memscpy(cmd_ptr,
          sizeof(uint32),
          (void *)((uint8*)uimdiag_req_ptr + uimdiag_offset),
          sizeof(uint32));

    uimdiag_offset  += sizeof(uint32);
}/* uimdiag_mmgsdi_util_decode_uint32 */


/*===========================================================================
FUNCTION UIMDIAG_MMGSDI_UTIL_POPULATE_SESSION_NAA_REFRESH_CMD

DESCRIPTION
  This command is used to build a request to trigger for refresh

DEPENDENCIES
  None.

RETURN VALUE
  uimdiag_return_enum_type

SIDE EFFECTS
  This function assigns memory. The calling function should properly manage
  this memory
===========================================================================*/
static uimdiag_return_enum_type
  uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd(
    uimdiag_mmgsdi_naa_refresh_cmd_type   * cmd_ptr,
    const void                            * diag_req_ptr,
    uint32                                  diag_req_len)
{

  UIMDIAG_UTIL_RETURN_IF_NULL(cmd_ptr);
  UIMDIAG_UTIL_RETURN_IF_NULL(diag_req_ptr);


  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. Client ID Id Lower/Higher 32 bits
  2. Slot ID.
  3. App ID Length
  4. App ID
  5. Refresh Mode
  6. Path Length
  7. Path

  ----------------------------------------------------- */
  uimdiag_mmgsdi_util_init_decode_params(diag_req_ptr,diag_req_len);

  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_high);
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->client_id_low);

   /* offset updated to access slot ID  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->slot_id);


  /* offset updated to access App Id Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->sim_app_id_len);

  /* offset updated to access aid*/
  /* Note sim_app_id is uint32*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->sim_app_id_len > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->sim_app_id,
      (cmd_ptr->sim_app_id_len * sizeof(uint8)));
    if(cmd_ptr->sim_app_id == NULL)
    {
      uimdiag_decode_status = UIMDIAG_ERROR;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->sim_app_id_len) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->sim_app_id,
      (cmd_ptr->sim_app_id_len * sizeof(uint8)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->sim_app_id_len* sizeof(uint8)));
  }

  if(cmd_ptr->sim_app_id_len == 0)
  {
    cmd_ptr->sim_app_id = NULL;
  }

  uimdiag_offset  += (cmd_ptr->sim_app_id_len*sizeof(uint8));

  /* offset being updated to point to refresh mode */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->refresh_mode);

  /* offset being updated to point to path Length  */
  uimdiag_mmgsdi_util_decode_uint32((void*)&cmd_ptr->path_len);

  /* offset updated to path */
  /* Note path length is uint8*        */
  UIMDIAG_ASSERT((uimdiag_offset <= diag_req_len), UIMDIAG_ERROR);

  if(cmd_ptr->path_len > 0)
  {
    UIMDIAG_UTIL_TMC_MEM_MALLOC_AND_VALIDATE(
      cmd_ptr->path,
      (cmd_ptr->path_len * sizeof(uint8)));

    if(cmd_ptr->path == NULL)
    {
      uimdiag_decode_status = UIMDIAG_ERROR;
      return uimdiag_decode_status;
    }

    UIMDIAG_ASSERT(((uimdiag_offset + cmd_ptr->path_len) <= diag_req_len), UIMDIAG_ERROR);
    memscpy((void *)cmd_ptr->path,
      (cmd_ptr->path_len * sizeof(uint8)),
      (void *)((uint8*)diag_req_ptr + uimdiag_offset),
      (cmd_ptr->path_len* sizeof(uint8)));
  }

  if(cmd_ptr->path_len == 0)
  {
    cmd_ptr->path = NULL;
  }

  uimdiag_offset  += (cmd_ptr->path_len*sizeof(uint8));

  /* offset updated to the end of the data structure */
  return uimdiag_decode_status;

}/* uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd */


/*===========================================================================
FUNCTION: UIMDIAG_PARSE_CMD_DATA

DESCRIPTION:
  Populates the request structure with data from the Diag packet.

INPUTS:
  *req_ptr:       Generic_command struct request pointer
                  which will be populated with data from the diag_req_ptr
  *diag_req_ptr:  Pointer to raw Diag request packet
  diag_req_len:   Length of the diag request packet

DEPENDENCIES:
  None

RETURN VALUE:
  uimdiag_return_enum_type

OUTPUT:
  None
===========================================================================*/
uimdiag_return_enum_type uimdiag_parse_cmd_data (
  uimdiag_generic_req_type  *req_ptr,
  PACKED void               *diag_req_ptr,
  uint32                     diag_req_len
)
{
  uimdiag_subsystem_cmd_enum_type uimdiag_subsys_cmd = UIMDIAG_MAX_CMD;

  UIMDIAG_MSG_HIGH_0("uimdiag_parse_cmd_data");

  if ((req_ptr == NULL) ||
      (diag_req_ptr == NULL) ||
      (diag_req_len < sizeof(uimdiag_cmd_header_type)))
  {
    UIMDIAG_MSG_ERROR_0("req_ptr or diag_req_ptr is NULL or diag_req_len to small:");
    return UIMDIAG_ERROR;
  }

  /* -----------------------------------------------------
  Parse following fields from diag buffer

  1. COMMAND CODE
  2. SUBSYSTEM ID
  3. SUBSYSTEM CMD CODE
  4. COMMAND Specific params
  ----------------------------------------------------- */

  /* command code */
  (void)memscpy( ((uint8*)&(req_ptr->cmd_header.command_code)),
                 sizeof(req_ptr->cmd_header.command_code),
                 (uint8*)diag_req_ptr, sizeof(uint8));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(uint8);
  diag_req_len -=  sizeof(uint8);
  
  /* subsystem id */
  (void)memscpy(((uint8*)&(req_ptr->cmd_header.subsys_id)),
                sizeof(req_ptr->cmd_header.subsys_id),
                (uint8*)diag_req_ptr, sizeof(diagpkt_subsys_id_type));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(diagpkt_subsys_id_type);
  diag_req_len -=  sizeof(diagpkt_subsys_id_type);
  
  /* subsystem command code */
  (void) memscpy( ((void*)&(req_ptr->cmd_header.subsys_cmd_code)),
                  sizeof(req_ptr->cmd_header.subsys_cmd_code),
                  (void*)diag_req_ptr,sizeof(diagpkt_subsys_cmd_code_type));

  diag_req_ptr  = (char*)diag_req_ptr + sizeof(diagpkt_subsys_cmd_code_type);
  diag_req_len -=  sizeof(diagpkt_subsys_cmd_code_type);

  uimdiag_subsys_cmd = (uimdiag_subsystem_cmd_enum_type)req_ptr->cmd_header.subsys_cmd_code;
  switch(uimdiag_subsys_cmd)
  {
      case UIMDIAG_MMGSDI_NAA_REFRESH_CMD:
        return uimdiag_mmgsdi_util_populate_session_naa_refresh_cmd(
                 &req_ptr->request.session_trigger_refresh,
                 (void*)diag_req_ptr, diag_req_len);

      default:
        UIMDIAG_MSG_ERROR_1("Invalid subcmd 0x%x", uimdiag_subsys_cmd);
        return UIMDIAG_ERROR;
  }
} /* uimdiag_parse_cmd_data */
