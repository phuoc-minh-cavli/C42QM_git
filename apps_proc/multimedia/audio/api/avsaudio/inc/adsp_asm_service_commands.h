/*======================================================================== */
/**
@file adsp_asm_service_commands.h

This file contains ASM service commands and events structures definitions.
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF. 

      The audio_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the audio_mainpage.dox file 
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_asm_service_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_ASM_SERVICE_COMMANDS_H_
#define _ADSP_ASM_SERVICE_COMMANDS_H_

#include "mmdefs.h"
#include "adsp_audio_memmap_api.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup asm_svc_cmd_get_stream_handles
    Queries for a list of currently active stream handles. This is used by the
    calibration software.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SVC_CMD_GET_STREAM_HANDLES

    @apr_msg_payload
      None.

    @prerequisites
      None.

    @return
      #ASM_SVC_CMDRSP_GET_STREAM_HANDLES
    @newpage
*/
#define ASM_SVC_CMD_GET_STREAM_HANDLES                          0x00010C0B

/** @addtogroup asm_svc_resp_get_stream_handles
@{ */
/** Returns a list of stream handles that are currently active.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SVC_CMDRSP_GET_STREAM_HANDLES \n
      Src_port -- Session ID 0, Stream ID 0

    @apr_msg_payload
      @structure{asm_svc_cmdrsp_get_stream_handles_t}
      @tablespace
      @inputtable{ASM_SVC_CMDRSP_GET_STREAM_HANDLES_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      None.
*/
#define ASM_SVC_CMDRSP_GET_STREAM_HANDLES                       0x00010C1B

/** Definition of the stream ID bitmask.
*/
#define ASM_BIT_MASK_STREAM_ID                                  (0x000000FFUL)

/** Definition of the stream ID shift value.
*/
#define ASM_SHIFT_STREAM_ID                                     0

/** Definition of the session ID bitmask.
*/
#define ASM_BIT_MASK_SESSION_ID                                 (0x0000FF00UL)

/** Definition of the session ID shift value.
*/
#define ASM_SHIFT_SESSION_ID                                    8

/** Definition of the service ID bitmask.
*/
#define ASM_BIT_MASK_SERVICE_ID                                 (0x00FF0000UL)

/** Definition of the service ID shift value.
*/
#define ASM_SHIFT_SERVICE_ID                                    16

/** Definition of the domain ID bitmask.
*/
#define ASM_BIT_MASK_DOMAIN_ID                                 (0xFF000000UL)

/** Definition of the domain ID shift value.
    @newpage
*/
#define ASM_SHIFT_DOMAIN_ID                                     24

/* Payload structure for a get stream handles ACK. */
typedef struct asm_svc_cmdrsp_get_stream_handles_t asm_svc_cmdrsp_get_stream_handles_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SVC_CMDRSP_GET_STREAM_HANDLES message, which
    returns a list of currently active stream handles.\n
    \n
    Immediately following this structure are num_handles of uint32 stream
    handles.
*/
struct asm_svc_cmdrsp_get_stream_handles_t
{
    uint32_t                  num_handles;
    /**< Number of active stream handles.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_svc_resp_get_stream_handles */

/** @ingroup asm_svc_cmd_mem_map_regions
    Commands the ASM to map multiple shared memory regions.
    All the mapped regions must be from the same memory pool.

    @relevant_apr_hdr_fields
      Opcode -- ASM_CMD_SHARED_MEM_MAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmd_shared_mem_map_regions_t}
      @tablespace
      @inputtable{ASM_and_ADM_CMD_MEM_MAP_REGIONS_APR_msg_payload.tex}

    @mappayload
      @structure{avs_cmd_shared_mem_map_regions_t}
      @tablespace
      @inputtable{ASM_and_ADM_CMD_MEM_MAP_REGIONS_MapPayload.tex}

    @prerequisites
      All regions to be mapped must be from the same memory pool.

    @return
      ASM_CMDRSP_SHARED_MEM_MAP_REGIONS

    API_developer_note: A-family APIs ASM_CMD_MEMORY_MAP, ASM_CMD_MEMORY_UNMAP, ASM_CMD_MEMORY_MAP_REGIONS
    ASM_CMD_MEMORY_UNMAP_REGIONS are replaced by ASM_CMD_SHARED_MEM_MAP_REGIONS & ASM_CMD_SHARED_MEM_UNMAP_REGIONS in B family.
    Also ASM_CMDRSP_SHARED_MEM_MAP_REGIONS is introduced to return the mmhandle to the clients. 

    @newpage
*/
#define ASM_CMD_SHARED_MEM_MAP_REGIONS               0x00010D92

/* ASM memory map regions command structure is the same as the
    AVS memory map regions command structure. */
typedef struct  avs_cmd_shared_mem_map_regions_t asm_cmd_shared_mem_map_regions_t;

/**
 * This command returns the memory map handle for an ASM_CMD_SHARED_MEM_MAP_REGIONS command
 * @relevant_apr_hdr_fields
      Opcode -- ASM_CMD_SHARED_MEM_UNMAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmdrsp_shared_mem_map_regions_t}

    @prerequisites
      ASM_CMD_SHARED_MEM_MAP_REGIONS.

    @return
      none
 */
#define ASM_CMDRSP_SHARED_MEM_MAP_REGIONS     0x00010D93

/* ASM memory map regions response payload is same as AVS memory map regions payload. */
typedef struct avs_cmdrsp_shared_mem_map_regions_t asm_cmdrsp_shared_mem_map_regions_t;


/** @ingroup asm_svc_cmd_mem_unmap_regions
    Commands the ASM to unmap multiple shared memory regions that were
    previously mapped.

    @relevant_apr_hdr_fields
      Opcode -- ASM_CMD_SHARED_MEM_UNMAP_REGIONS \n
      Dst_port -- 0000 (Session ID/Stream ID = 0/0)

    @apr_msg_payload
      @structure{avs_cmd_shared_mem_unmap_regions_t}
      @tablespace
      @inputtable{ASM_and_ADM_CMD_MEM_UNMAP_REGIONS_APR_msg_payload.tex}

    @vertspace{9}
    @unmappayload
      @structure{avs_cmd_shared_mem_unmap_regions_t}
      @tablespace
      @inputtable{ASM_and_ADM_CMD_MEM_UNMAP_REGIONS_Unmap_payload.tex}

    @prerequisites
      ASM_CMD_SHARED_MEM_MAP_REGIONS.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_CMD_SHARED_MEM_UNMAP_REGIONS              0x00010D94

/* ASM memory unmap regions command structure is the same as the
    AVS memory unmap regions command structure. */
typedef struct avs_cmd_shared_mem_unmap_regions_t asm_cmd_shared_mem_unmap_regions_t;

/**
  This command adds custom topologies to the DSP for use in POPP and POPreP. These
  topologies can be created by specifying their topology IDs in the open command.
 */
#define ASM_CMD_ADD_TOPOLOGIES                                0x00010DBE

/* The payload for the ASM_CMD_ADD_TOPOLOGIES command */
typedef struct asm_cmd_add_topologies_t asm_cmd_add_topologies_t;

/** @brief Payload of the ASM_CMD_ADD_TOPOLOGIES message.
*/
#include "adsp_begin_pack.h"
struct asm_cmd_add_topologies_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< LSW of the topology buffer address. */
    uint32_t                  data_payload_addr_msw;
    /**< MSW of the topology buffer address. */

    uint32_t                  mem_map_handle;
    /**< Supported Values: Any.
     * memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  buffer_size;
    /**< Size of the valid data in the topology buffer in bytes */
}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* ADSP_ASM_SERVICE_COMMANDS_H_ */
