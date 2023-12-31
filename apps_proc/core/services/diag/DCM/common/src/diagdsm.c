/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  D I A G D S M . C

GENERAL DESCRIPTION
  DMSS Data Services memory pool module.

EXTERNALIZED FUNCTIONS

  diagdsm_init()
  diagdsm_deinit()
  diagdsm_delete()
    Initialize the Data Services Memory pool unit.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

  dsm_init() must be called prior to any other DSM function.

Copyright (c) 2005-2014,2016,2019 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.tx/6.0/services/diag/DCM/common/src/diagdsm.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/10/19   kdey    Porting of Qsockets
07/20/16   nk      Changes to support ThreadX
03/28/14    is     Resolve 4k response issue
01/27/14    is     Command/response separation
01/07/14    xy     Added fusion DCI feature  
10/17/13    is     Add zero_init attribute to DIAG_DSM_ALIGNED_ALLOC
10/01/13    ao     Added log buffers into DIAG_LOG section
10/01/13    xy     Removed code under FEATURE_WINCE 
06/03/12    sa     Added RX/TX Control Channel Pool 
05/10/12    sa     Removed DIAG_MP_SLAVE_LAST
03/10/11    is     Move DSM_ALIGNED_ALLOC() from DSM to Diag
01/18/10    JV     Do not initilaize the 2 SMD pools for single processor targets.
10/06/09    JV     Mainlined FEATURE_DSM_DIAG_ITEMS.
09/23/09    JV     Do not initialize the SMD_RX and SMD_TX pools on the slave
                   last.
08/05/09    JV     Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/31/09    JV     Merged Q6 diag code back to mainline
07/24/09    mad    Corrected comment
06/22/09    mad    Featurized inclusion of some diag header files
04/03/09    mad    Moved DiagDSM item sizes etc to a new header file diagdsmi.h
03/26/09    mad    Re-enabled target-specific definitions of DIAG DSM item 
                   SIZ/CNT
03/24/09    mad    Correction in DSM pool macro definitions:
                   enclosed in brackets
03/16/09    mad    Changes for WM7: prevent DSM pool initialization if 
                   already initialized. Added diagdsm_deinit()
03/06/09    mad    Redefined DIAG DSM item sizes/counts.Upper-level 
                   target-specific definitions for these are now over-ridden
                   in this file.
03/05/09    vg     Corrected the DSM pools (SMD_RX_ITEM & SMD_TX_ITEM)  
01/13/09    is     Removed redundant memory allocation due to use of 
                   deprecated DSM API, dsm_init_pool()
05/01/08    as     Fixed featurization on DSM pools for modem only builds.
10/18/07    pj     Changed code to align pools on 32-byte boundary.
09/25/07    ms     Merging functions from diagdsm_wce.c into this file.
09/24/07    ms     diagdsm_init() should return false if allocation fails. 
                   Incorporating RB's changes for LG into mainline.
06/20/07    pj     changes for DSM as a DLL.
01/22/06    as     Featurized code to make DSM count & size defined by target
12/05/06    as     Increased DSM SIO and SMD TX item size.
11/21/06    as     Modified code to use diag internal features.
09/13/06    as     Created file
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/* Target-independent Include files */
#include "comdef.h"

#include "customer.h"
#include "queue.h"

#include "dsm.h"
#include "diagdsm_v.h"
#include "diagdsmi.h"
#include "diag_v.h"

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h" 
#endif

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


#if !defined (DIAG_DSM_ALIGNED_ALLOC)
#if defined __arm
#define DIAG_DSM_ALIGNED_ALLOC(name,size) \
  static uint8 __align((32)) __attribute__((section("DIAG_LOG"),zero_init)) name[size]
#elif defined __GNUC__
#define DIAG_DSM_ALIGNED_ALLOC(name, size) \
  static uint8 name[size] __attribute__ ((aligned(32)))
#else
#define DIAG_DSM_ALIGNED_ALLOC(name,size) \
  static uint32 name[(size + 31) /  sizeof(uint32)]
#endif /* __arm et. al. */
#endif /* !DIAG_DSM_ALIGNED_ALLOC */


/*--------------------------------------------------------------------------
  Declare pools sizes and control structures.
--------------------------------------------------------------------------*/

#define DSMI_DIAG_SIO_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_RX_ITEM_CNT * \
   (DSMI_DIAG_SIO_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_rx_item_pool;

#define DSMI_DIAG_SIO_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_TX_ITEM_CNT * \
   (DSMI_DIAG_SIO_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_tx_item_pool;

#if defined(DIAG_MP_MASTER)
#define DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_rx_item_pool;

#define DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_tx_item_pool;

#define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_rx_item_pool;

#define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_tx_item_pool;
#define DSMI_DIAG_CMD_SMD_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_rx_item_pool;

#define DSMI_DIAG_CMD_SMD_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_tx_item_pool;

#define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_dci_rx_item_pool;

#define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_smd_dci_tx_item_pool;

#else
#define DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_rx_item_pool;

#define DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_tx_item_pool;

#define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_rx_item_pool;

#define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT * \
   (DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_cmd_sio_dci_tx_item_pool;
#endif /* DIAG_MP_MASTER */

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)

#define DSMI_DIAG_SMD_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SMD_TX_ITEM_CNT * \
   (DSMI_DIAG_SMD_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_smd_tx_item_pool;

//FUSION DCI sio channel array size
#define DSMI_DIAG_SIO_FUSION_RX_ITEM_ARRAY_SIZ (DSMI_DIAG_SIO_FUSION_RX_ITEM_CNT * \
   (DSMI_DIAG_SIO_FUSION_RX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
dsm_pool_mgmt_table_type dsm_diag_sio_fusion_rx_item_pool;

#ifdef DIAG_CONSUMER_API
  #define DSMI_DIAG_SMD_DCI_TX_ITEM_ARRAY_SIZ (DSMI_DIAG_SMD_DCI_TX_ITEM_CNT * \
     (DSMI_DIAG_SMD_DCI_TX_ITEM_SIZ + DSM_ITEM_HEADER_SIZE))
  dsm_pool_mgmt_table_type dsm_diag_smd_dci_tx_item_pool;
#endif /* #ifdef DIAG_CONSUMER_API */

#endif


DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_rx_item_array,DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_tx_item_array,DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ);

DIAG_DSM_ALIGNED_ALLOC(dsm_diag_sio_rx_item_array,DSMI_DIAG_SIO_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_sio_tx_item_array,DSMI_DIAG_SIO_TX_ITEM_ARRAY_SIZ);

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_smd_tx_item_array,DSMI_DIAG_SMD_TX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_sio_fusion_rx_item_array,DSMI_DIAG_SIO_FUSION_RX_ITEM_ARRAY_SIZ);
#ifdef DIAG_CONSUMER_API
  DIAG_DSM_ALIGNED_ALLOC(dsm_diag_smd_dci_tx_item_array,DSMI_DIAG_SMD_DCI_TX_ITEM_ARRAY_SIZ);
#endif /* #ifdef DIAG_CONSUMER_API */
#endif /* DIAG_MP && DIAG_MP_MASTER */

DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_rx_item_array,DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_tx_item_array,DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ);

#if !defined(DIAG_MP_MASTER)
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_dci_rx_item_array,DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_sio_dci_tx_item_array,DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ);
#else 
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_rx_item_array,DSMI_DIAG_CMD_SMD_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_tx_item_array,DSMI_DIAG_CMD_SMD_TX_ITEM_ARRAY_SIZ);

DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_dci_rx_item_array,DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_ARRAY_SIZ);
DIAG_DSM_ALIGNED_ALLOC(dsm_diag_cmd_smd_dci_tx_item_array,DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_ARRAY_SIZ);
#endif


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           EXTERNALIZED FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/


/*===========================================================================
FUNCTION DIAGDSM_INIT()

DESCRIPTION
  This function will initialize the Data Service Memory Pool. It should be
  called once upon system startup. All the memory items are initialized and
  put onto their respective free queues.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if successful, or if memory already allocated. FALSE if allocation is unsuccessful.

SIDE EFFECTS
  None
  This function should be called only from Windiag, on WM.
===========================================================================*/

boolean diagdsm_init(void)
{


  /* In WM, dsm_init_pool() is a deprecated DSM API which is replaced by dsm_create_pool().  Since dsm_create_pool() internally calls
     dsm_init_pool(), we will continue to use this deprecated API to enable WM/AMSS code to be consistent. */
  dsm_init_pool(DSM_DIAG_SIO_RX_ITEM_POOL, 
    (uint8*)dsm_diag_sio_rx_item_array,
    DSMI_DIAG_SIO_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SIO_RX_ITEM_SIZ);
  
  dsm_init_pool(DSM_DIAG_SIO_TX_ITEM_POOL, 
    (uint8*)dsm_diag_sio_tx_item_array,
    DSMI_DIAG_SIO_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SIO_TX_ITEM_SIZ);

#if defined (DIAG_MP) && defined (DIAG_MP_MASTER)

  dsm_init_pool(DSM_DIAG_SMD_TX_ITEM_POOL, 
    (uint8*)dsm_diag_smd_tx_item_array,
    DSMI_DIAG_SMD_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SMD_TX_ITEM_SIZ);


//Initializing the dci SMD pool
#ifdef DIAG_CONSUMER_API
  dsm_init_pool(DSM_DIAG_SMD_DCI_TX_ITEM_POOL, 
    (uint8*)dsm_diag_smd_dci_tx_item_array,
    DSMI_DIAG_SMD_DCI_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SMD_DCI_TX_ITEM_SIZ);	
#endif /* #ifdef DIAG_CONSUMER_API */

#endif /* DIAG_MP && DIAG_MP_MASTER */

//Initializing the FUSION DCI TX Control Pool
  dsm_init_pool(DSM_DIAG_SIO_FUSION_RX_ITEM_POOL, 
    (uint8*)dsm_diag_sio_fusion_rx_item_array,
    DSMI_DIAG_SIO_FUSION_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_SIO_FUSION_RX_ITEM_SIZ);

dsm_init_pool(DSM_DIAG_CMD_SIO_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_rx_item_array,
    DSMI_DIAG_CMD_SIO_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ);

dsm_init_pool(DSM_DIAG_CMD_SIO_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_tx_item_array,
    DSMI_DIAG_CMD_SIO_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ);
    
#ifndef DIAG_MP_MASTER
  dsm_init_pool(DSM_DIAG_CMD_SIO_DCI_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_dci_rx_item_array,
    DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SIO_DCI_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_sio_dci_tx_item_array,
    DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ);
#else
  dsm_init_pool(DSM_DIAG_CMD_SMD_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_rx_item_array,
    DSMI_DIAG_CMD_SMD_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SMD_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_tx_item_array,
    DSMI_DIAG_CMD_SMD_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ);

  dsm_init_pool(DSM_DIAG_CMD_SMD_DCI_RX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_dci_rx_item_array,
    DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ);
    
  dsm_init_pool(DSM_DIAG_CMD_SMD_DCI_TX_ITEM_POOL, 
    (uint8*)dsm_diag_cmd_smd_dci_tx_item_array,
    DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_ARRAY_SIZ,
    DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ);
#endif
  
return TRUE;
} /* diagdsm_init() */


