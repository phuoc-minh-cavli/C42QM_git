#ifndef _ADSP_AVCS_API_H_
#define _ADSP_AVCS_API_H_

/**
@file adsp_core_api.h

@brief This file contains the structure definitions for the AVCS
commands used in client-AVCS communications.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The core_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      core_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avcs/inc/adsp_core_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/03/12   AP      Created the file
11/30/11   sw      (Tech Pubs) Updated doxy markup/comments for PDF (Rev D)
08/01/11   AP       Added ADSP_CMD_SET_PERIODIC_PROFILING_EVENTS_3 and 
                    ADSP_EVENT_PERIODIC_PROFILING_3 API
06/27/11   AZ       Added ADSP_EVENT_GET_STATE and ADSP_EVENT_STATE API
04/27/11   leo     (Tech Pubs) Updated doxy comments for Interface Spec.
12/08/10   VI       Added Remote Bus BW and Power Collapse API
09/22/10   SD      (Tech Pubs) Edited Doxygen markup and comments.
05/21/10   KK      Created file.
========================================================================== */

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup core_get_version
  Queries the AVS for the build version and versions of static services.

  @apr_hdr_fields
    Opcode -- AVCS_GET_VERSIONS

  @apr_msg_payload
    None.

  @prerequisites
    None.

  @return
    #AVCS_GET_VERSIONS_RSP
  @newpage
*/
#define AVCS_GET_VERSIONS       0x00012905

/** @ingroup core_resp_version
  Response to an #AVCS_GET_VERSIONS command, which queries for versions of
  supported services.

  @apr_hdr_fields
    Opcode -- AVCS_GET_VERSIONS_RSP

  @apr_msg_payload
    @structure{avcs_service_cmd_rsp_get_api_versions_t}
    @tablespace
    @inputtable{AVCS_GET_VERSIONS_RSP_APR_msg_payload.tex}

  @parspace Service information
    @structure{avcs_svc_info}
    @tablespace
    @inputtable{AVCS_GET_VERSIONS_RSP_Service_info.tex}

  @prerequisites
    None.

  @return
    None.
  @newpage
*/
#define AVCS_GET_VERSIONS_RSP   0x00012906

/** @ingroup versions
  @xreflabel{hdr::AVCSVersionID} Version of the AVS core module. */
#define AVCS_CORE_VERSION   0x00030000

typedef struct avcs_service_cmd_rsp_get_api_versions_t avcs_service_cmd_rsp_get_api_versions_t;
#include "adsp_begin_pack.h"

/** @addtogroup core_resp_version
@{ */
/**
  @brief Payload of the #AVCS_GET_VERSIONS_RSP command response, which
  provides a listing of service IDs and their API version numbers.\n
  \n
  Immediately following this structure are num_services instances of
  acs_svc_info.
*/
struct avcs_service_cmd_rsp_get_api_versions_t
{

    uint32_t                  build_id;
    /**< This field might have some meaningful non zero value once the multi-process design is implemented.
    Until then the BuildID value should be set to zero. */

    uint32_t                  num_services;
    /**< Number of supported services in the current build.
         @newpage */

}
#include "adsp_end_pack.h"
;

typedef struct avcs_svc_info avcs_svc_info;
#include "adsp_begin_pack.h"

/**
  @brief For each instance in the #AVCS_GET_VERSIONS_RSP payload, this
  structure contains per-service information.
*/
struct avcs_svc_info
{

    uint32_t                  service_id;
    /**< Service ID.

         Supported values: 0 to 255 */

    uint32_t                  version;
    /**< Service API version.

         Supported values:
         - Bits 0 to 15 -- Minor version
         - Bits 16 to 31 -- Major version
         @newpage */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup core_resp_version */

/** @ingroup core_cmd_mem_stats
  Requests information on AVS memory consumption.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_GET_MEMORY_STATS

  @apr_msg_payload
    None.

  @prerequisites
    None.

  @return
   #AVCS_CMDRSP_GET_MEMORY_STATS
  @newpage
*/
#define AVCS_CMD_GET_MEMORY_STATS                    0x00012907

/* NOTE: Q9 = QuRT_Elite API Interface Specification */
/** @addtogroup core_resp_mem_stats
@{ */
/**
  In response to the #AVCS_CMD_GET_MEMORY_STATS command, returns statistics
  related to QuRT_Elite memory allocation use. For more information on
  QuRT_Elite, refer to @xhyperref{Q9,[Q9]}.

  This event can be used to detect how fully the aDSP heap is being used and
  to track memory allocations/free counts for memory leaks.

  @apr_hdr_fields
    Opcode -- AVCS_CMDRSP_GET_MEMORY_STATS

  @apr_msg_payload
    @structure{avcs_cmd_rsp_get_memory_stats_t}
    @tablespace
    @inputtable{AVCS_CMDRSP_GET_MEMORY_STATS_APR_msg_payload.tex}

  @prerequisites
    None.

  @return
    None.
  @newpage
*/
#define AVCS_CMDRSP_GET_MEMORY_STATS                  0x00012908

typedef struct avcs_cmd_rsp_get_memory_stats_t avcs_cmd_rsp_get_memory_stats_t;
#include "adsp_begin_pack.h"

/**
  @brief Payload of the #AVCS_CMDRSP_GET_MEMORY_STATS command response, which
  returns statistics related to QuRT_Elite memory allocation use.
*/
struct avcs_cmd_rsp_get_memory_stats_t
{

    uint32_t              num_elite_mallocs;
    /**< Number of elite memory allocations that have occurred since bootup or
         since statistics were reset. */

    uint32_t              num_elite_frees;
    /**< Number of times elite memory was freed since bootup or since statistics
         were reset. */

    uint32_t              current_elite_heap_use;
    /**< Current number of bytes allocated from the heap since statistics were
         reset for elite. */

    uint32_t              peak_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup or since
         statistics were reset for elite.*/

    uint32_t              num_non_elite_mallocs;
    /**< Number of non-elite memory allocations that have occurred since bootup or
         since statistics were reset. */

    uint32_t              num_non_elite_frees;
    /**< Number of times non-elite memory was freed since bootup or since statistics
         were reset. */

    uint32_t              current_non_elite_heap_use;
    /**< Current number of bytes allocated from the heap since statistics were
         reset for non-elite. */

    uint32_t              peak_non_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup or since
         statistics were reset for non-elite.
         @newpage */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup core_resp_mem_stats */

/** @ingroup core_cmd_peak_heap_use
  Requests that the AVS reset the peak heap usage to 0, as reported in
  profiling events and ACKs.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_RESET_PEAK_HEAP_USE

  @apr_msg_payload
    None.

  @prerequisites
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
  @newpage
*/
#define AVCS_CMD_RESET_PEAK_HEAP_USE                  0x00012909


/** @ingroup core_cmd_profiling
  Requests that periodic profiling be provided at regular intervals. This
  command provides additional information: AHB BW, L1 and L2 cache events, and
  AXI BW vote.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS

  @apr_msg_payload
    @structure{avcs_cmd_get_periodic_profiling_events}
    @tablespace
    @inputtable{AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS_APR_msg_payload.tex}

  @description
    At requested intervals, the AVS writes profiling information to the
    provided memory location and sends the AVCS_EVENT_PERIODIC_PROFILING
    events.
      @negparspace
    The physical_address field is automatically mapped in the aDSP before
    profiling starts, and is unmapped after profiling is complete. There is no
    need to send memory map commands before this command.
      @negparspace
    The size field indicates how much space is reserved. If the entire
    profiling data does not fit in this field, the profiler sends partial
    profiling information covering as many software threads as will fit. For
    example, 1 KB can fit up approximately 80 profiled software threads.
      @negparspace
    Currently, the maximum number of software threads that can be profiled is
    64 (#ADSP_MAX_PROFILED_SW_THREADS).

  @prerequisites
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
  @newpage
*/
#define AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS   0x0001290A

/** @addtogroup core_cmd_profiling_2
@{ */
/** @xreflabel{hdr:ProfilingOff}
  Disables profiling.
*/
#define AVS_PROFILING_OFF                0

/** @xreflabel{hdr:ProfilingSummary}
  Enables profiling to collect summary information of MIPS/memory.
*/
#define AVS_PROFILING_SUMMARY            1

/** @xreflabel{hdr:ProfilingPerThread}
  Enables profiling to collect summary information of MIPS/memory,
  per-software-thread MIPS, and stack consumption.

  For the #ADSP_CMD_SET_PERIODIC_PROFILING_EVENTS_2 and
  #ADSP_CMD_SET_PERIODIC_PROFILING_EVENTS_3 commands, AXI BW, AHB BW, MPPS,
  and cache miss ratios (L1 I$, L1D$, L2$) are also collected.
*/
#define AVS_PROFILING_PER_THREAD         2

/**
  Maximum number of software threads to profile. This value limits the size
  of the profiling event message.
  @newpage
*/
#define AVS_MAX_PROFILED_SW_THREADS 64

#define SHARED_MEM_PROPERTY_VIRTUAL   1

#define SHARED_MEM_PROPERTY_PHYSICAL  0

typedef struct avcs_cmd_get_periodic_profiling_events avcs_cmd_get_periodic_profiling_events;
#include "adsp_begin_pack.h"

/**
  @brief Payload of the #AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS command,
  which requests periodic events with profiling information.
*/
struct avcs_cmd_get_periodic_profiling_events
{

    uint32_t              profiling_level;
    /**< Level of profiling to be collected.

         Supported values:
           - #AVS_PROFILING_OFF
           - #AVS_PROFILING_SUMMARY
           - #AVS_PROFILING_PER_THREAD */

    uint32_t              physical_address_lsw;
    /**< Physical address lsw of the buffer into which the AVS can write the
         profiling data for each event. It must be 4&nbsp;KB aligned. */

    uint32_t              physical_address_msw;
    /**< Physical address msw of the buffer into which the AVS can write the
         profiling data for each event. */

    uint32_t              size;
    /**< Number of bytes available in the shared buffer. It must be in
         multiples of 4 KB pages. */

    uint32_t              sampling_period;
    /**< Number of microseconds between the profiling measurements.

         Supported values: @ge 100000 */

    uint32_t              property_flag;
   /** indicates whether the shared memory provided is virtual or physical

       Supported values: 0,1
       @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup core_cmd_profiling */

/** @ingroup core_event_profiling
  In response to the #AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS command, provides
  statistics for the last measurement event. For detailed information on how to
  process this data, refer to @xhyperref{Q8,[Q8]}.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_PERIODIC_PROFILING

  @apr_msg_payload
    @structure{avcs_event_periodic_profiling}
    @tablespace
    @inputtable{AVCS_EVENT_PERIODIC_PROFILING_APR_msg_payload.tex}

  @parspace Profiling payload
    @structure{avcs_event_periodic_profiling_t}
    @tablespace
    @inputtable{AVCS_EVENT_PERIODIC_PROFILING_Profiling_payload.tex}

  @parspace Software thread information
    @structure{avcs_sw_thread_info}
    @tablespace
    @inputtable{AVCS_EVENT_PERIODIC_PROFILING_SW_thread_info}

  @parspace PMU mode information
    A PMU is a hardware block that supports measurements of only four events.
    To calculate different metrics (like AXI BW, AHB BW, MPPS, and Cache miss
    ratios), PMU must be configured to count different events in successive
    profiling events. The pmu_mode field provides the information of which
    events are measured by PMU in a given profiling period.
    Table @xref{tbl:PmuModeMapping} lists the mapping of pmu_mode to measured
    PMU events.
    @tablespace
    @inputtable{AVS_EVENT_PERIODIC_PROFILING_PMU_mode_info.tex}

  @newpage
  @par Performance metric formulas
    @xreflabel{hdr:PerfMetricFormulas}
    The parameters in this event allow clients to calculate AXI BW, AHB BW,
    MPPS, and cache miss ratios of the aDSP during any application. The
    following formulas can be used to calculate the different parameters.\n
      @negparspace
    - AXI Read BW (in KB)
      @code
((axi_read_cached*32) + ((axi_read_total - axi_read_cached) * 4) + /
(1024 * profiling _duration)
      @endcode
      @negparspace
    - AXI Write BW (in KB)
      @code
((axi_write_cached*32) + ((axi_write_total - axi_write_cached) * 4) /
(1024 * profiling _duration)
      @endcode
      @negparspace
    - AHB Read BW (in KB)
      @code
(AHB read request * 4) / (1024 * profiling_duration)
      @endcode
      @negparspace
    - AHB Write BW (in KB)
      @code
(AHB write request * 4) / (1024 * profiling_duration)
      @endcode
      @negparspace
    - L1 I$ Miss Per Instruction Packet
      @code
(L1 I$ miss event) / (number of instructions executed)
      @endcode
      @negparspace
    - L1 D$ Miss Per Instruction Packet
      @code
(L1 D$ miss event) / (number of instructions executed)
      @endcode
      @negparspace
    - L2 Miss Ratio
      @code
(instruction misses + data misses ) / (instruction accesses + data accesses)
      @endcode
      @negparspace
    - MPPS
      @code
(number of packets executed) / (profiling_duration * 1000000)
      @endcode

  @prerequisites
    None.

  @return
    None.
  @newpage
    */
#define AVCS_EVENT_PERIODIC_PROFILING   0x0001290B

/** @ingroup core_event_get_state
  Used by AVS clients to query the AVCS for the readiness of AVS static
  services. If the AVCS itself is not ready (it is not registered with APR),
  the event is discarded in the APR layer.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_GET_STATE

  @apr_msg_payload
    None.

  @prerequisites
    None.

  @return
    None.
  @newpage
*/
#define AVCS_EVENT_GET_STATE   0x0001290C

/** @ingroup core_event_state
  Used by the AVCS to respond to the #AVCS_EVENT_GET_STATE event. When all
  static services are ready, the payload is set to 0x1. If a static service is
  not ready, the AVCS either does not respond or it responds with a payload
  other than 0x1.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_STATE

  @apr_msg_payload
    @structure{acs_event_state_t}
    @tablespace
    @inputtable{AVCS_EVENT_STATE_APR_msg_payload.tex}

  @description
    AVS clients can periodically send #AVCS_EVENT_GET_STATE to the AVCS at
    start up. If the AVS services are all up and ready, the AVCS must respond
    with AVCS_EVENT_STATE with a payload of 0x1 to indicate the state.
      @negparspace
    If AVS services are not all up and ready, the AVCS may not respond or it
    may respond with AVCS_EVENT_STATE with a payload other than 0x1. The
    clients must be able to handle this behavior appropriately.
      @negparspace
    If the AVCS service (static service) is not ready (not registered with the
    APR), the APR discards the AVCS_EVENT_GET_STATE message.
      @negparspace
    After receiving the AVCS_EVENT_STATE from the AVCS, with the indication that
    all AVS static services are up and ready, the clients stop sending
    AVCS_EVENT_GET_STATE and ignore any further responses (which could be the
    latent response for previous requests). The clients proceed to excercise
    AVS static services.
      @negparspace
    Currently, the interface does not support query of individual service
    readiness, which is reserved for future requirements. It conveys only the
    overall readiness of all AVS static services.

  @newpage
  @prerequisites
    #AVCS_EVENT_GET_STATE event must have been sent by the client.

  @return
    None.
  @newpage
*/
#define AVCS_EVENT_STATE   0x0001290D

typedef struct avcs_event_periodic_profiling avcs_event_periodic_profiling;
#include "adsp_begin_pack.h"

/** @ingroup core_cmd_profiling
  @brief Payload of the #AVS_EVENT_PERIODIC_PROFILING and
   which respond to the #AVS_CMD_SET_PERIODIC_PROFILING_EVENTS .
*/
struct avcs_event_periodic_profiling
{

    uint32_t              profiling_payload_address_lsw;
      /**< Pointer to the physical address lsw of the profiling payload
           (avcs_event_periodic_profiling_t ).*/

    uint32_t              profiling_payload_address_msw;
      /**< Pointer to the physical address lsw of the profiling payload
           (avcs_event_periodic_profiling_t ).
           @newpage */

}
#include "adsp_end_pack.h"
;

typedef struct avcs_event_periodic_profiling_t avcs_event_periodic_profiling_t;
#include "adsp_begin_pack.h"

/** @ingroup core_event_profiling
  @brief Profiling payload of the #AVCS_EVENT_PERIODIC_PROFILING event,
  which provides a periodic update of profiling statistics from the AVS. The
  payload contains one instance of this structure, followed by
  profiled_sw_threads instances of acs_sw_thread_info.
  */

#define AVCS_API_VERSION_PERIODIC_PROFILING     0x1

struct avcs_event_periodic_profiling_t
{
    uint16_t              avcs_profiling_minor_version;
   /**< Minor version used for tracking the version of the profiling
         interface.*/

    uint16_t              swthreadinfo_offset;
    /**<Offset in number of bytes from this field to SwThreadInfos of profiled
    software threads. This field can also help in future API expansion.*/

    uint32_t              core_clock_freq;
    /**< Configured frequency of the aDSP core clock, in kHz. */

    uint32_t              bus_clock_freq;
    /**< Configured frequency of the aDSP AHB bus clock in kHz. */

    uint32_t              wall_clock_ticks;
    /**< Number of ticks from the previous event. This is used to estimate the
         duration since the previous measurement event. Each tick is a cycle of
         the 32768 Hz sleep timer. */

    uint32_t             wall_clock_freq;
    /**<Represented in Hz.
    Used to estimate the duration since the previous measurement  event*/


    uint32_t              proc_cycles;
    /**< Number of processor cycles that occurred since the previous
         measurement event. */

    uint32_t              current_elite_heap_use;
    /**< Current number of bytes allocated from the heap. */

    uint32_t              peak_elite_heap_use;
    /**< Peak heap consumption in bytes since bootup or the last
         #ADSP_CMD_RESET_PEAK_HEAP_USE. */

    uint32_t              current_non_elite_heap_use;
    /**< Current number of bytes allocated from the heap for non elite mallocs*/

    uint32_t              peak_non_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup for non elite
         mallocs */

    uint32_t              tot_num_hw_threads;
    /**< Number of hardware threads on the existing platform.

         Supported values: 1 to 4 */

    uint32_t              hw_idle_cycles[4];
    /**< Per hardware thread, the number of idle cycles since the last
         measurement event. The array size of 4 accommodates all anticipated
         platforms to be supported */

    uint32_t              profiled_sw_threads;
    /**< Indicates the number of profiled software threads.

         Supported values:
         - 0 -- Software thread profiling is not present
         - > 0 -- Number of active software threads in the aDSP

         The payload for this event comprises one instance of
           acs_event_periodic_profiling_3_t, followed immediately by
         profiled_sw_threads number of instances of acs_sw_thread_info. */

    uint32_t              pmu_event_counters[4];
    /**< Counts the occurrence of configured PMU events.
         @newpage */

    uint32_t              pmu_mode;
    /**< Specifies the events that are measured in PMU mode in a given
         profiling period.

         Supported values:
         - 0 -- AXI BW
         - 1 -- AHB BW
         - 2 -- L1 cache events, MPPS
         - 3 -- L2 cache events
         - 4 -- events related to thread running cycles
         - 5 -- events related to thread loading in DMT mode*/

    uint32_t              axi_bw_vote_ab;
    /**< AXI Average bandwidth vote in bytes per second (refer to @xhyperref{Q6,[Q6]}).
          */
    uint32_t              axi_bw_vote_ib;
    /**< AXI instantaneous bandwidth vote in bytes per second (refer to @xhyperref{Q6,[Q6]}).
         @newpage */
    /* NOTE: Q6 = Hexagon Multimedia: Elite Firmware Architecture Description */

}
#include "adsp_end_pack.h"
;

typedef struct avcs_sw_thread_info avcs_sw_thread_info;
#include "adsp_begin_pack.h"

/** @ingroup core_event_profiling_2
  @brief Per-software-thread profiling information that is part of the
  avcs_event_periodic_profiling_t and avcs_event_periodic_profiling_t
  payload structures.
*/
struct avcs_sw_thread_info
{

    uint32_t              thread_id;
      /**< Unique identifier of a software thread. The ID comprises the first
           four ASCII characters of the thread name. It can be formatted as a
           string and displayed after appending the EOS character. */

    uint32_t              run_cycles;
      /**< Number of cycles this thread has been actively running on a hardware
           thread since the last measurement event. */

    uint16_t              stack_size;
      /**< Allocation size in bytes of this thread stack. */

    uint16_t              stack_free;
      /**< Estimated stack amount that has not been touched since thread
           inception.
           @newpage */

}
#include "adsp_end_pack.h"
;

typedef struct avcs_event_state_t avcs_event_state_t;
#include "adsp_begin_pack.h"

/** @ingroup core_event_state
  @brief Payload of the #AVCS_EVENT_STATE event, which responds to an
  #AVCS_EVENT_GET_STATE event sent by clients.
*/
struct avcs_event_state_t
{

    uint32_t              state;
    /**< Indicates the aDSP state.

         Supported values:
         - 0x1 -- aDSP services are up and ready
         - Other values -- Reserved for future use; current clients must ignore
           these other values
         @newpage */

}
#include "adsp_end_pack.h"
;

/** @addtogroup core_cmd_dolby_license
@{ */
/**
  Sets the license information for Dolby<sup>&reg;</sup> modules.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_SET_DOLBY_LICENSE

  @apr_msg_payload
    @structure{avcs_service_cmd_set_dolby_license_t}
    @tablespace
    @inputtable{AVCS_CMD_SET_DOLBY_LICENSE_APR_msg_payload.tex}

  @parspace Payload format

    @inputtable{AVCS_CMD_SET_DOLBY_LICENSE_Payload_format.tex}

  @description
    To enable the modules provided by Dolby (like the Dolby Digital Plus
    decoder and the Dolby Audio Kernel), valid license information must be
    sent to the aDSP. The license information consists of two parts: ID
    information (such as the manufacturer ID) and a license file.
      @newpage
      @negparspace
    This command allows the client to send this license information to the
    aDSP before opening any sessions that use the Dolby modules. All the
    license information is sent at one time. A success result from the aDSP
    indicates that the license information was successfully stored internally.
    It does not indicate whether the aDPS found the license to be valid.
      @negparspace
    This API can be called only once.

  @prerequisites
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
  @newpage
*/
#define AVCS_CMD_SET_DOLBY_LICENSE               0x0001290E

typedef struct avcs_service_cmd_set_dolby_license_t avcs_service_cmd_set_dolby_license_t;
#include "adsp_begin_pack.h"

/**
  @brief Payload of the #AVCS_CMD_SET_DOLBY_LICENSE command, which sends
  Dolby license information to the AVS.
*/
struct avcs_service_cmd_set_dolby_license_t
{
    uint32_t                  payload_addr_lsw;
      /**< Physical address lsw  of the payload containing the license information.
         The address must be 4 KB aligned. */

    uint32_t                  payload_addr_msw;
      /**< Physical address msw  of the payload containing the license information.*/

    uint32_t                  num_id_params;
    /**< Number of ID-related parameters in the payload. The payload format
         is an array of ID-related parameters for the license. */

    uint32_t                  license_data_size;
    /**< Size in bytes of the license data in the payload. The payload format
         is an array of of the contents of the license file. The size is given
         in the APR message.*/


    uint32_t                 property_flag;
   /** indicates whether the shared memory provided is virtual or physical

          Supported values: 0,1
       @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */  /* end_addtogroup core_cmd_dolby_license */


/** @addtogroup core_cmd_low_power_segments
@{ */
/**
  Gets low power memory segments information.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO

  @description
    This command is used to query for the physical addresses of sections
    that would be advantageous to move to OCMEM during low-power single-stream
    audio use case and also the required OCMEM read/write bandwidth to support
    the use case.
 
    A low power audio use case is one in which a single stream playback or a
    gapless playback is done on a single Slimbus device or a single I2S device.
    The content should be 16-bit mono/stereo in PCM/MP3/AAC/AAC+/eAAC+ formats only.


  @prerequisites
    None.

  @return
    AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO
  @newpage
*/
#define AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO              0x00012903

/** @addtogroup core_cmd_rsp_low_power_segments
@{ */
/**
  Returns low power memory segments information.

  @apr_hdr_fields
   Opcode -- AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO
   
  @apr_msg_payload
    @structure{avcs_cmd_rsp_get_low_power_segments_info_t}
    @tablespace
    @inputtable{AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO_APR_msg_payload.tex}

  @parspace Payload format

    @inputtable{AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO_Payload_format.tex}
   
  @description
    This response provides the list of memory sections requested
    to be mapped to OCMEM in the case of low-power audio
    playback.

  @prerequisites
    AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO should have been sent
    by the client.

  @return
    None.
  @newpage
*/
#define AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO           0x00012904

typedef struct avcs_cmd_rsp_get_low_power_segments_info_t avcs_cmd_rsp_get_low_power_segments_info_t;
#include "adsp_begin_pack.h"
/** @brief AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO payload
    structure. Payload for this event comprises one instance of
    avcs_cmd_rsp_get_low_power_segments_info_t, followed
    immediately by num_segments number of instances of the
    avcs_mem_segment_t structure. */
struct avcs_cmd_rsp_get_low_power_segments_info_t
{
    uint32_t              num_segments; 
      /**< Number of segments in this response. 
           0: there are no known sections that should be mapped
           from DDR to OCMEM.
           >0: the number of memory segments in the following list.
        */

    uint32_t              bandwidth;
      /**< Required OCMEM read/write bandwidth (in bytes per second) 
        if OCMEM is granted.
        0 if num_segments = 0
        >0 if num_segments > 0. */
      
}
#include "adsp_end_pack.h"
;

/**
  Types of Low Power Memory Segments.
*/
#define READ_ONLY_SEGMENT      1
    /**< Read Only memory segment. */
#define READ_WRITE_SEGMENT     2
    /**< Read Write memory segment. */
    
/**
  Category indicates whether audio/os/sensor segments.
*/
#define AUDIO_SEGMENT          1
    /**< Audio memory segment. */
#define OS_SEGMENT             2
    /**< QDSP6's OS memory segment. */

typedef struct avcs_mem_segment_t avcs_mem_segment_t;
#include "adsp_begin_pack.h"
/** @brief Payload structure for AVS low power memory segment
 *         structure. */
struct avcs_mem_segment_t
{
    uint16_t              type;
      /**< Indicates which type of memory this segment is. 
           Allowed values: READ_ONLY_SEGMENT or READ_WRITE_SEGMENT only. */

    uint16_t              category;
      /**< Indicates whether audio or OS segments. 
           Allowed values: AUDIO_SEGMENT or OS_SEGMENT only. */

    uint32_t              size;
      /**< Size (in bytes) of this segment. 
           Will be a non-zero value.*/
    
    uint32_t              start_address_lsw;
      /**< Lower 32 bits of the 64-bit physical start address 
           of this segment. */
      
    uint32_t              start_address_msw;
      /**< Upper 32 bits of the 64-bit physical start address 
           of this segment. */
}
#include "adsp_end_pack.h"
;
/** @} */  /* end_addtogroup core_cmd_low_power_segments */

/** @addtogroup core_cmd_remote_avtimer_vote_request
@{ */
/**
  API to AVCS for handling avtimer vote requests from clients.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST

  @description
    This command can be used by clients to request avtimer enablement.
    Remote clients can access avtimer only after vote request.
    ADSP returns handle to avtimer as response to this command. 
    Client need to make sure that it votes avtimer only once. 
    After use case finishes client has to request AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST 
    to release avtimer.
    It is assumed that client will not send multiple vote requests. If client sends multiple
    vote commands, same handle of avtimer will be sent as response.

  @prerequisites
    None.

  @return
    AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST.
  @newpage
*/
#define AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST              0x00012914

typedef struct avcs_cmd_remote_avtimer_vote_request avcs_cmd_remote_avtimer_vote_request_t;
#include "adsp_begin_pack.h"
/** @brief AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST payload
    structure. Payload for this command. */
struct avcs_cmd_remote_avtimer_vote_request
{
   char client_name[8];
      /**< Client name, max is 8 chars. Avtimer driver takes first 6 characters */     
}
#include "adsp_end_pack.h"
;

/** @} */  /* end_addtogroup core_cmd_remote_avtimer_vote_request */

/** @addtogroup core_cmd_rsp_remote_avtimer_vote_request
@{ */
/**
  Response event from ADSP for AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST

  @description
    This is response event to the command AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST. 
    Command response contain handle to the avtimer. 	
    After use case finishes client has to request AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST 
    to release avtimer.

  @prerequisites
    AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST should have been sent
    by the client.

  @return
    None.
  @newpage
*/
#define AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST              0x00012915

typedef struct avcs_cmd_rsp_remote_avtimer_vote_request avcs_cmd_rsp_remote_avtimer_vote_request_t;
#include "adsp_begin_pack.h"
/** @brief AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST payload
    structure. Payload for this command. */
struct avcs_cmd_rsp_remote_avtimer_vote_request
{
   uint32_t avtimer_handle;
      /**< avtimer handle */     
}
#include "adsp_end_pack.h"
;

/** @} */  /* end_addtogroup core_cmd_rsp_remote_avtimer_vote_request */

/** @addtogroup core_cmd_remote_avtimer_release_request
@{ */
/**
  API to AVCS for handling avtimer disable requests from clients

  @apr_hdr_fields
   Opcode -- AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST

  @description
    This command can be used by clients to request avtimer disablement.
    Clietns should send valid handle for disabling avtimer.

  @prerequisites
    AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST should have been sent
    by the client.

  @return
    None.
  @newpage
*/
#define AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST              0x00012916

typedef struct avcs_cmd_remote_avtimer_release_request avcs_cmd_remote_avtimer_release_request_t;
#include "adsp_begin_pack.h"
/** @brief AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST payload
    structure. Payload for this command. */
struct avcs_cmd_remote_avtimer_release_request
{
   uint32_t avtimer_handle;
      /**< avtimer handle */     
}
#include "adsp_end_pack.h"
;

/** @} */  /* end_addtogroup core_cmd_remote_avtimer_release_request */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ACS_API_H_ */
