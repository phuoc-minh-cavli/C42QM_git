#ifndef PS_MEM_H
#define PS_MEM_H
/*===========================================================================

   P R O T O T O C O L   S E R V I C E S   D Y N A M I C   M E M O R Y

                          H E A D E R   F I L E

GENERAL DESCRIPTION
  This module implements a simple dynamic memory management mechanism for
  the protocol services.

EXTERNALIZED FUNCTIONS
  ps_mem_init()              - Initializes ps_mem module
  ps_mem_pool_init()         - Initializes a single ps_mem pool
  ps_mem_dynamic_pool_init() - Initializes a single ps_mem pool
  ps_mem_index_to_buf()      - Given an index return corresponding buffer.
  ps_mem_buf_to_index()      - Given a buffer, return its index in the pool.

  --- The following have been exported to ps_mem_ext.h. ---

  ps_mem_free()              - Free the allocated memory, like free.
  ps_mem_get_buf()           - Allocates a memory buffer for the specified poolid
  ps_mem_is_valid()          - Validates that this is memory item is of the
                               specified poolid
  ps_mem_dup()               - Allocate shared resource for buffer.
  ps_mem_pool_enum_type      - Enumeration of memory pools.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_mem_init() has to be called exactly once before using any externalized
  functions provided by this module and each client has to initialize its pool
  using ps_mem_pool_init()

Copyright (c) 2003-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/dataiot.mpss/2.0/interface/utils/inc/ps_mem.h#1 $
  $DateTime: 2007/05/18 18:56:57
===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "amssassert.h"
#include "ps_mem_ext.h"
#include "ds_crit_sect.h"


/*===========================================================================

                         PUBLIC DEFINITIONS AND DATA

===========================================================================*/
#ifndef PS_MEM_POOLS
/*---------------------------------------------------------------------------
  The following enum is used to identify the poolid associated with different
  memory buffers. Poolid must start from 1 as in ps_mem_index_to_buf, value
  in PS_MEM_BUF_HEADER is compared to match against poolid.
  PS_MEM_BUF_HEADER will have a value of 0 when a buffer is the last
  item on the free buffer list and if we use 0 as a poolid, function will
  wrongly conclude that the buffer is allocated. In order to be able to index
  ps_mem_pool_info with poolid, a dummy pool is declared with an id of 0.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_MEM_DUMMY_POOL_TYPE              = 0,         /* Must be a dummy pool */
  PS_MEM_TX_META_INFO_TYPE            = 1,
  PS_MEM_APP_TO_PS_CMD_TYPE           = 2,
  PS_MEM_DS_IPFLTER_TYPE              = 3,
  PS_MEM_PKT_META_INFO_TYPE           = 4,
  PS_MEM_DSS_SCB_TYPE                 = 6,
  PS_MEM_DSS_ACB_TYPE                 = 7,
  PS_MEM_IPV6_ADDR_TYPE               = 8,
  PS_MEM_DNSI_API_CB_TYPE             = 9,
  PS_MEM_DNSI_SESSION_MGR_CB_TYPE     = 10,
  PS_MEM_DNSI_RESOLVER_CB_TYPE        = 11,
  PS_MEM_DNSI_IO_MGR_CB_TYPE          = 12,
  PS_MEM_DNSI_GENERIC_RR_TYPE         = 13,
  PS_MEM_DNSI_HOSTENT_CB_TYPE         = 14,
  PS_MEM_RT_META_INFO_TYPE            = 15,
  PS_MEM_RX_META_INFO_TYPE            = 16,
  PS_MEM_META_INFO_TYPE_EX            = 17,

  /* PS Mem pools for DSNet buffers */
  PS_MEM_DS_NET_QOS_DEFAULT            = 18,
  PS_MEM_DS_NET_QOS_SECONDARY          = 19,
  PS_MEM_DS_NET_QOS_MANAGER            = 20,
  PS_MEM_DS_NET_QOS_FLOW_SPEC          = 21,
  
  /* PS Mem pools for DSUtils buffers */
  
  PS_MEM_DNSI_LOCAL_CACHE_ENTRY_TYPE  = 22,
  PS_MEM_DCC_CMD_DATA_BUF_TYPE        = 23,
  PS_MEM_ICMP_ERR_INFO_TYPE           = 24,
  PS_MEM_RECV_IF_INFO_TYPE            = 25,
  PS_MEM_PPP_NCP_CB_TYPE              = 26,
  PS_MEM_DNS_PROXY_SOCK_EVENT_INFO_TYPE  = 27,
  PS_MEM_DNS_PROXY_ENABLE_INFO_TYPE      = 28,
  PS_MEM_DNS_PROXY_DISABLE_INFO_TYPE     = 29,
  
  /* PS Mem pools for Network Initiated QoS */
  PS_MEM_DS_NET_QOS_NET_INITIATED_REQ        = 30,
  PS_MEM_DS_NET_QOS_NET_INITIATED            = 31,
  PS_MEM_PS_QOS_NET_INITIATED_REQ_TYPE       = 32, 
  PS_MEM_PS_QOS_NET_INITIATED_REQ_INFO_TYPE  = 33,
  PS_MEM_PS_QOS_NET_INITIATED_FLTR_TYPE      = 34,
  PS_MEM_RX_QOS_FLTR_BUF_TYPE                = 35,  
  PS_MEM_RX_QOS_FLTR_NODE_TYPE               = 36,
   
  /* PS Mem pools for DS Profile DB */
  PS_MEM_DNS_PROXY_INFO_TYPE                 = 37,
  PS_MEM_CLAT_INSTANCE_TYPE                  = 38,

  PS_MEM_DSS_IOCTL_EVT_CB_TYPE               = 39,

  PS_MEM_PPP_CMD_BUF_TYPE                    = 40,
  PS_MEM_PPP_EVENT_INFO_ITEM_TYPE            = 41,
  
  PS_MEM_HDLC_INSTANCE_TYPE                  = 42,

  PS_MEM_DNSI_CACHE_ENTRY_TYPE               = 43,
  PS_MEM_DNSI_SESSION_DNS_SERVERS_TYPE       = 44,
  PS_MEM_DNSI_MSG_RR_TYPE                    = 45,

  PS_MEM_MAX_POOLS
  
} ps_mem_pool_enum_type;
#endif /* PS_MEM_POOLS */

/*---------------------------------------------------------------------------
  Buffer header, used to store the pool id or the pointer to the next buffer
  item
---------------------------------------------------------------------------*/
typedef struct
{
  int32  poolid;
  int16  ref_cnt;
  int16 index;
} ps_mem_buf_hdr_type;

#define PS_MEM_BUF_HDR_SIZE sizeof(ps_mem_buf_hdr_type)

/*---------------------------------------------------------------------------
TYPEDEF PS_MEM_WM_CB_F_PTR_TYPE

DESCRIPTION
  Definition for the watermark callback function for ps_mem.

PARAMS
  Value of Hi/Low watermark which trggerred the callback.

RETURN VALUE
  None
---------------------------------------------------------------------------*/
typedef void (*ps_mem_wm_cb_f_ptr_type)
(
  int16 wm_value
);

//---------------------------------------------------------------------------
//  PS_MEM_GET_TOT_SIZE vs PS_MEM_GET_TOT_SIZE_OPT
//    Usage for PS_MEM_GET_TOT_SIZE:
//    Allocate the complete buffer and not just the pointer to the buffer
//    i.e. static allocation.
//    Should be used in data path (dynamic allocation takes more time)
//    Should be used in singletons
//    Usage for PS_MEM_GET_TOT_SIZE_OPT:
//    Allocate the pointer to the buffer, i.e. Dynamic Allocation
//      Should be used on all other cases
//---------------------------------------------------------------------------

/*===========================================================================
MACRO PS_MEM_GET_TOT_SIZE

DESCRIPTION
  This macro returns the amount of memory in words, that a client needs to
  allocate given number of buffers and size of each buffer
===========================================================================*/
#define PS_MEM_GET_TOT_SIZE(buf_num, buf_size)                           \
  ( buf_num * (buf_size + PS_MEM_BUF_HDR_SIZE) / sizeof(int) )

/*===========================================================================
MACRO PS_MEM_GET_TOT_SIZE_OPT

DESCRIPTION
  This macro returns the amount of memory in words, that a client needs to
  allocate given number of buffers and size of each buffer. Will
  return just the size for the pointer array if FEATURE_DATA_PS_SYSTEM_HEAP
  is defined. This is useful for ps_mem clients that need to featurize use
  of the system heap.
===========================================================================*/
#ifdef FEATURE_DATA_PS_SYSTEM_HEAP
#define PS_MEM_GET_TOT_SIZE_OPT(buf_num, buf_size)  ( buf_num )
#else
#define PS_MEM_GET_TOT_SIZE_OPT(buf_num, buf_size)                 \
  ( buf_num * (buf_size + PS_MEM_BUF_HDR_SIZE) / sizeof(int) )

#endif /* FEATURE_DATA_PS_SYSTEM_HEAP  */

/*===========================================================================
MACRO PS_MEM_POOL_INIT_OPT

DESCRIPTION
  This macro will call the dynamic version of the pool_init, if
  FEATURE_DATA_PS_SYSTEM_HEAP is defined..
===========================================================================*/

#ifdef FEATURE_DATA_PS_SYSTEM_HEAP
#define PS_MEM_POOL_INIT_OPT ps_mem_dynamic_pool_init
#else
#define PS_MEM_POOL_INIT_OPT ps_mem_pool_init
#endif /* FEATURE_DATA_PS_SYSTEM_HEAP */


/*===========================================================================

                         PUBLIC FUNCTIONS AND MACROS

===========================================================================*/
/*===========================================================================
FUNCTION      PS_MEM_INIT()

DESCRIPTION   This function is used to initialize ps_mem module

RETURN VALUE  None

DEPENDENCIES  Called only once from PS_TASK
===========================================================================*/
void ps_mem_init
(
  void
);

/*===========================================================================
FUNCTION      PS_MEM_DEINIT()

DESCRIPTION   This function is used to cleanup ps_mem module

RETURN VALUE  None

DEPENDENCIES  None
===========================================================================*/
void ps_mem_deinit
(
  void
);

/*===========================================================================
FUNCTION      PS_MEM_POOL_INIT()

DESCRIPTION   This function is used to initialize the PS buffer pools.
              It creates the memory pool based on the different pool id
              (size) and number of buffers for this pool id, as defined in
              ps_mem_pool_info. As such, these numbers should be tuned to
              achieve minimum yet sufficient memory usage.

PARAMETERS
  pool_id                       - Pool ID Number 1..PS_MEM_MAX_POOLS-1
  buf_memory                    - Integer-aligned memory buffer location
                                  that is (buf_size+hdr_size) * num_buf in size.
  buf_size                      - Size of each buffer or structure
  num_buf                       - Number of buffers
  high_wm                       - Callback trigger for high watermark
  low_wm                        - Callback trigger for low watermark
  mem_avail_cb_f_ptr            - Callback that is triggered by watermarks
  buf_hdr_arr                   - Array of pointers to buffer headers
  buf_ptr_arr                   - Array of pointers to buffer payloads
                                  Suggest to make these of the type in use

RETURN VALUE  0 Success, -1 Failure

DEPENDENCIES  Called only once by each client
===========================================================================*/
int32 ps_mem_pool_init
(
  ps_mem_pool_enum_type      pool_id,
  int                      * buf_memory,
  uint16                     buf_size,
  int16                      num_buf,
  int16                      high_wm,
  int16                      low_wm,
  ps_mem_wm_cb_f_ptr_type    mem_avail_cb_f_ptr,
  int                        buf_hdr_arr[],
  int                        buf_ptr_arr[]
);

/*===========================================================================
FUNCTION      PS_MEM_DYNAMIC_POOL_INIT()

DESCRIPTION   This function is used to initialize the PS buffer pools.
              This version of the init causes the memory for buffers to be
              allocated from the system heap instead of statically.
              It creates the memory pool based on the different pool id
              (size) and number of buffers for this pool id, as defined in
              ps_mem_pool_info. As such, these numbers should be tuned to
              achieve minimum yet sufficient memory usage.

PARAMETERS
  pool_id                       - Pool ID Number 1..PS_MEM_MAX_POOLS-1
  ps_mem_meta_data              - Allocation for integer pointer array
                                  that is sizeof(int*) *  num_buf in size.
  buf_size                      - Size of each buffer or structure
  num_buf                       - Number of buffers
  high_wm                       - Callback trigger for high watermark
  low_wm                        - Callback trigger for low watermark
  mem_avail_cb_f_ptr            - Callback that is triggered by watermarks
  buf_hdr_arr                   - Array of pointers to buffer headers
  buf_ptr_arr                   - Array of pointers to buffer payloads
                                  Suggest to make these of the type in use

RETURN VALUE  None

DEPENDENCIES  Called only once by each client
===========================================================================*/
int32 ps_mem_dynamic_pool_init
(
  ps_mem_pool_enum_type      pool_id,
  int                      * ps_mem_meta_data, /* num_buf * sizeof(int*) */
  uint16                     buf_size,
  int16                      num_buf,
  int16                      high_wm,
  int16                      low_wm,
  ps_mem_wm_cb_f_ptr_type    mem_avail_cb_f_ptr,
  int                        buf_hdr_arr[],
  int                        buf_ptr_arr[]
);


/*===========================================================================
FUNCTION     PS_MEM_REG_MEM_AVAIL_CB()

DESCRIPTION  This function is used to register for the Memory available
             callback. The low watermark for each poolid is a compile time
             constant. When the number of free items equals the low watermark
             value on the way up, the registered callback is called.

RETURN VALUE None

DEPENDENCIES None
===========================================================================*/
void ps_mem_reg_mem_avail_cb
(
  ps_mem_pool_enum_type   poolid,
  ps_mem_wm_cb_f_ptr_type f_ptr
);

/*===========================================================================
FUNCTION     PS_MEM_LESS_THAN_FEW_FREE_ITEMS()

DESCRIPTION  This function is used to query whether the number of free items
is below the low watermark or not.

RETURN VALUE
  TRUE if the number of free items is less than low watermark
  else FALSE.

DEPENDENCIES None
===========================================================================*/
boolean ps_mem_less_than_few_free_items
(
  ps_mem_pool_enum_type poolid
);

/*==========================================================================
FUNCTION     PS_MEM_INDEX_TO_BUF()

DESCRIPTION  This function returns a pointer to a buffer given an index.

RETURN VALUE If successful, a pointer to the corresponding memory block is
             returned. Otherwise, a NULL pointer is returned.

DEPENDENCIES None
===========================================================================*/
void *ps_mem_index_to_buf
(
  int16                index,
  ps_mem_pool_enum_type poolid
);

/*==========================================================================
FUNCTION     PS_MEM_BUF_TO_INDEX()

DESCRIPTION  This function returns an index given a pointer to a buffer.

RETURN VALUE If successful, an index is returned. Otherwise, -1 is returned.

DEPENDENCIES None
===========================================================================*/
int16 ps_mem_buf_to_index
(
  void *buf
);

/*===========================================================================
FUNCTION PS_MEM_POOL_FREE

DESCRIPTION
  This function is INTERNAL ONLY and is used for testing purposes. Frees up
  the pool id used by STA ps_mem client

PARAMETERS
  poolid - the dummy pool

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mem_pool_free
(
  ps_mem_pool_enum_type poolid
);

#ifndef PS_MEM_POOLS
/*===========================================================================
FUNCTION     PS_MEM_GET_BUF()

DESCRIPTION  This function is used for dynamic buffer allocation of buffers
             for a specified poolid.

RETURN VALUE If successful, a pointer to the allocated memory block is
             returned. Otherwise, a NULL pointer is returned.

DEPENDENCIES None
===========================================================================*/
void *ps_mem_get_buf
(
  ps_mem_pool_enum_type poolid
);

/*===========================================================================
FUNCTION     PS_MEM_IS_VALID()

DESCRIPTION  This function validates that this is memory item is of the
             specied poolid type.

PARAMETERS   mi_ptr: pointer to the memory item being checked

RETURN VALUE TRUE if the item is valid
             FALSE otherwise

DEPENDENCIES None

SIDE EFFECTS None
===========================================================================*/
boolean ps_mem_is_valid
(
  void                 *mi_ptr,
  ps_mem_pool_enum_type poolid
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* PS_MEM_POOLS*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PS_MEM_H */
