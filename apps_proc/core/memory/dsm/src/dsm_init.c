/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  D S M _ I N I T . C

  GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

  EXTERNALIZED FUNCTIONS

  dsm_init()
  Initialize the Data Services Memory pool unit.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

  Copyright (c) 2005-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/memory/dsm/src/dsm_init.c#1 $

  when        who    what, where, why
  --------    ---    ----------------------------------------------------------
11/13/09      vph    HDR needs small item_cnt as in HSDPA_PHASE2_CAPABILITY.
                     Remove the feature so it can applied to both HSDPA & HDR.
11/12/09      dng    Updated to fix the crash in 1x data call
09/02/09      umr    Merge Changes from antigua.     
10/17/06      ttl    Change DSM numbers. 
08/03/06      rr     Increased small_item_cnt feature HSDPA_PHASE2_CAPABILITY 
02/22/06      mjb    Set up values for MSM6280
10/10/94      jjw    Created file
  ===========================================================================*/


/*===========================================================================

  INCLUDE FILES FOR MODULE

  ===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"
#include "customer.h"
#include "queue.h"
#include "dsm.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "assert.h"
#include "dsm_init.h"

/*===========================================================================

  DEFINITIONS AND DECLARATIONS FOR MODULE

  ===========================================================================*/

/*---------------------------------------------------------------------------
                         TARGET SPECIFIC ITEM SIZING
---------------------------------------------------------------------------*/

#ifndef DSMI_DS_SMALL_ITEM_SIZ
#define DSMI_DS_SMALL_ITEM_SIZ           128
#endif
/*-------------------------------------------------------------------------
  Counts for various memory levels/marks for small items.

  DONT_EXCEED:    PPP starts dumping frames and the resequencing queue of
  all the tcp connections is flushed
  LINK_LAYER_DNE: RLP resets
  TRANSPORT_FEW:  TCP stops adding out of order segments to resequencing
  queue. Note that this is a count and not a memory level,
  i.e., no callback is registered for this count.
  LINK_LAYER_FEW: RLP stops processing new frames. It entertains only
  retransmissions.
  FEW:            The application is flow controlled while attempting 
  socket writes, i.e., it gets EWOULDBLOCK.
  MANY:           The application is informed that it is no longer flow
  controlled and socket writes succeed.

    In addition, there are following two counts related to HDR:

    HDR_FEW:        The forward link is flow controlled.
    HDR_MANY:       The flow control on the forward link is disabled.

  Note: The counts corresponding to different memory levels should be in
  order: DONT_EXCEED <= LINK_LAYER_DNE <= LINK_LAYER_FEW <=
  FEW <= MANY. If one needs to change this order, the order of 
  levels should also be changed for dsm_mem_level_enum_type.
  -------------------------------------------------------------------------*/

#define DSMI_DS_SMALL_ITEM_DONT_EXCEED       3
#define DSMI_DS_SMALL_ITEM_LINK_LAYER_DNE    6

#ifndef DSMI_DS_SMALL_ITEM_TRANSPORT_LAYER_FEW
#define DSMI_DS_SMALL_ITEM_TRANSPORT_LAYER_FEW 10
#endif
#ifndef DSMI_DS_SMALL_ITEM_LINK_LAYER_FEW
#define DSMI_DS_SMALL_ITEM_LINK_LAYER_FEW    10
#endif
#ifndef DSMI_DS_SMALL_ITEM_FEW_MARK
#define DSMI_DS_SMALL_ITEM_FEW_MARK            10
#endif
#ifndef DSMI_DS_SMALL_ITEM_SIO_FEW_MARK
#define DSMI_DS_SMALL_ITEM_SIO_FEW_MARK        10
#endif
#ifndef DSMI_DS_SMALL_ITEM_MANY_MARK
#define DSMI_DS_SMALL_ITEM_MANY_MARK            20
#endif
#ifndef DSMI_DS_SMALL_ITEM_SIO_MANY_MARK
#define DSMI_DS_SMALL_ITEM_SIO_MANY_MARK        20
#endif

#ifndef DSMI_DS_SMALL_ITEM_RLC_FEW_MARK
#define DSMI_DS_SMALL_ITEM_RLC_FEW_MARK       10
#endif

#ifndef DSMI_DS_SMALL_ITEM_RLC_MANY_MARK
#define DSMI_DS_SMALL_ITEM_RLC_MANY_MARK      20
#endif

#ifndef DSMI_DS_SMALL_ITEM_CNT
#define DSMI_DS_SMALL_ITEM_CNT           40
#endif

#ifndef DSMI_DS_SMALL_ITEM_HDR_FEW_MARK
    #define DSMI_DS_SMALL_ITEM_HDR_FEW_MARK   DSMI_DS_SMALL_ITEM_LINK_LAYER_FEW
#endif /* DSMI_DS_SMALL_ITEM_HDR_FEW_MARK */

#ifndef DSMI_DS_SMALL_ITEM_HDR_MANY_MARK
    #define DSMI_DS_SMALL_ITEM_HDR_MANY_MARK  DSMI_DS_SMALL_ITEM_SIO_FEW_MARK
#endif /* DSMI_DS_SMALL_ITEM_HDR_MANY_MARK */


  #ifdef FEATURE_DSM_LARGE_ITEMS
    /*---------------------------------------------------------------------------
     Size, Count and count for different memory marks/levels for LARGE items. 
      The significants of the counts DONT_EXCEED, TRANSPORT_FEW, FEW and MANY
      is same as described above for small items.

     Sizing for Large Memory Pool items. Header size defines enough space for
     worst case TCP/IP/PPP header.  This should be:

     Default Max MSS + TCP Header Size + MAX TCP Options Size + IP Header Size
     + MAX IP Options Size + MAX PPP Header Size + PPP Checksum Size.

     The large item count needs to be increased if maximum-size SSL records
     are processed.
    ---------------------------------------------------------------------------*/
  #ifndef DSMI_DS_LARGE_ITEM_SIZ
    #define DSMI_DS_LARGE_ITEM_SIZ                 1536
  #endif

  #ifndef DSMI_DS_LARGE_ITEM_CNT
    #define DSMI_DS_LARGE_ITEM_CNT                 1500
  #endif

      #define DSMI_DS_LARGE_ITEM_DONT_EXCEED       1  
      #define DSMI_DS_LARGE_ITEM_TRANSPORT_FEW     3
      #define DSMI_DS_LARGE_ITEM_FEW_MARK          7
      #define DSMI_DS_LARGE_ITEM_LINK_LAYER_FEW    7
      #define DSMI_DS_LARGE_ITEM_SIO_FEW_MARK      7
      #define DSMI_DS_LARGE_ITEM_MANY_MARK         11
      #define DSMI_DS_LARGE_ITEM_SIO_MANY_MARK     11
  #endif /* FEATURE_DSM_LARGE_ITEMS */

  #ifdef FEATURE_DSM_DUP_ITEMS
    /*---------------------------------------------------------------------------
     Size, Count, Few, many and do not exceed counts for DUP items 
     ---------------------------------------------------------------------------*/
  #ifndef DSMI_DUP_ITEM_CNT
  #define DSMI_DUP_ITEM_CNT                     2400
  #endif

      #define DSMI_DUP_ITEM_DONT_EXCEED           5
  #define DSMI_DUP_ITEM_FEW_MARK                25
  #define DSMI_DUP_ITEM_MANY_MARK               50
  #endif /* FEATURE_DSMI_DUP_ITEMS */


/*--------------------------------------------------------------------------
  Defining the static array that stores the small items.
  --------------------------------------------------------------------------*/
#define DSMI_DS_SMALL_ITEM_ARRAY_SIZ ((DSMI_DS_SMALL_ITEM_CNT * \
         (DSMI_DS_SMALL_ITEM_SIZ + DSM_ITEM_HEADER_SIZE)) +  31)
static uint32  dsm_ds_small_item_array[DSMI_DS_SMALL_ITEM_ARRAY_SIZ/4];
dsm_pool_mgmt_table_type dsm_ds_small_item_pool;

/*--------------------------------------------------------------------------
  Defining the static array that stores the large items.
  --------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_LARGE_ITEMS

#define DSMI_DS_LARGE_ITEM_ARRAY_SIZ ((DSMI_DS_LARGE_ITEM_CNT * \
         (DSMI_DS_LARGE_ITEM_SIZ + DSM_ITEM_HEADER_SIZE)) +  31)
static uint32  dsm_ds_large_item_array[DSMI_DS_LARGE_ITEM_ARRAY_SIZ/4];
dsm_pool_mgmt_table_type dsm_ds_large_item_pool;

#endif /* FEATURE_DSM_LARGE_ITEMS */

/*--------------------------------------------------------------------------
  Defining the static array that stores the DUP items.
  --------------------------------------------------------------------------*/
#ifdef FEATURE_DSM_DUP_ITEMS

#define DSMI_DUP_ITEM_ARRAY_SIZ ((DSMI_DUP_ITEM_CNT * \
         (0 + DSM_ITEM_HEADER_SIZE)) +  31)
static uint32 dsm_dup_item_array[DSMI_DUP_ITEM_ARRAY_SIZ/4];
dsm_pool_mgmt_table_type dsm_dup_item_pool;

#endif /* FEATURE_DSM_DUP_ITEMS */





/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

  =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION DSM_INIT()

DESCRIPTION
   This function will initialize the Data Service Memory Pool. It should be
   called once upon system startup. All the memory items are initialized and
   put onto their respective free queues.

DEPENDENCIES
   None

PARAMETERS
   None

RETURN VALUE
   None

SIDE EFFECTS
   None
===========================================================================*/
void dsm_init(void)
{
   dsm_init_pool(DSM_DS_SMALL_ITEM_POOL, 
         (uint8*)dsm_ds_small_item_array,
         DSMI_DS_SMALL_ITEM_ARRAY_SIZ,
         DSMI_DS_SMALL_ITEM_SIZ);


   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_DNE,
         DSMI_DS_SMALL_ITEM_DONT_EXCEED);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_LINK_LAYER_DNE,
         DSMI_DS_SMALL_ITEM_LINK_LAYER_DNE);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_TRANSPORT_LAYER_FEW,
         DSMI_DS_SMALL_ITEM_TRANSPORT_LAYER_FEW);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_LINK_LAYER_FEW,
         DSMI_DS_SMALL_ITEM_LINK_LAYER_FEW);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_HDR_FEW,
         DSMI_DS_SMALL_ITEM_HDR_FEW_MARK);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
                          DSM_MEM_LEVEL_RLC_FEW,
                          DSMI_DS_SMALL_ITEM_RLC_FEW_MARK);
  dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_FEW,
         DSMI_DS_SMALL_ITEM_FEW_MARK);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_SIO_FEW,
         DSMI_DS_SMALL_ITEM_SIO_FEW_MARK);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_HDR_MANY,
         DSMI_DS_SMALL_ITEM_HDR_MANY_MARK);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_RLC_MANY,
         DSMI_DS_SMALL_ITEM_RLC_MANY_MARK);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_MANY,
         DSMI_DS_SMALL_ITEM_MANY_MARK);
   dsm_reg_mem_event_level(DSM_DS_SMALL_ITEM_POOL,
         DSM_MEM_LEVEL_SIO_MANY,
         DSMI_DS_SMALL_ITEM_SIO_MANY_MARK);


#ifdef FEATURE_DSM_LARGE_ITEMS
   dsm_init_pool(DSM_DS_LARGE_ITEM_POOL, 
         (uint8*)dsm_ds_large_item_array,
         DSMI_DS_LARGE_ITEM_ARRAY_SIZ,
         DSMI_DS_LARGE_ITEM_SIZ);


   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_DNE,
         DSMI_DS_LARGE_ITEM_DONT_EXCEED);
   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_TRANSPORT_LAYER_FEW,
         DSMI_DS_LARGE_ITEM_TRANSPORT_FEW);
   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_LINK_LAYER_FEW,
         DSMI_DS_LARGE_ITEM_LINK_LAYER_FEW);
   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_FEW,
         DSMI_DS_LARGE_ITEM_FEW_MARK);
   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_SIO_FEW,
         DSMI_DS_LARGE_ITEM_SIO_FEW_MARK);
   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_MANY,
         DSMI_DS_LARGE_ITEM_MANY_MARK);
   dsm_reg_mem_event_level(DSM_DS_LARGE_ITEM_POOL,
         DSM_MEM_LEVEL_SIO_MANY,
         DSMI_DS_LARGE_ITEM_SIO_MANY_MARK);

#endif /* FEATURE_DSM_LARGE_ITEMS */
#ifdef FEATURE_DSM_DUP_ITEMS
   dsm_init_pool(DSM_DUP_ITEM_POOL,
         (uint8*)dsm_dup_item_array,
         DSMI_DUP_ITEM_ARRAY_SIZ,
         0);

   dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
         DSM_MEM_LEVEL_FEW,
         DSMI_DUP_ITEM_FEW_MARK);


   dsm_reg_mem_event_level(DSM_DUP_ITEM_POOL,
         DSM_MEM_LEVEL_MANY,
         DSMI_DUP_ITEM_MANY_MARK);


#endif /* FEATURE_DSM_DUP_ITEMS */

} /* dsm_init() */
#endif /* !MSM5000_IRAM_FWD */
