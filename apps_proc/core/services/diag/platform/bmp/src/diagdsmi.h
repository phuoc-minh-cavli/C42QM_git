#ifndef DIAGDSMI_H
#define DIAGDSMI_H
/*===========================================================================

                                  D I A G D S M I . H

  GENERAL DESCRIPTION
  Declares sizes for diag dsm items.

Copyright (c) 2009-2016, 2018-2020 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                            EDIT HISTORY FOR FILE
                                      
  $Header: //components/rel/core.tx/6.0/services/diag/platform/bmp/src/diagdsmi.h#2 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
01/10/19    kdey   Porting of Qsockets
09/04/18    gn     Memory optimizations
07/20/16    nk     Changes to support ThreadX
04/21/15    rh     Increased the size of DSM items on the apps to 8kB 
03/28/14    is     Resolve 4k response issue
02/26/14    sa     Added support for compression in buffering mode.
02/21/14    is     Command/response separation
01/07/14    xy     Added fusion DCI feature  
06/20/13    rh     Added control channel flow bounds
06/20/12    rh     Reduced default outgoing DSM pool watermarks. 
                   Added new DSM pool watermarks for buffering mode.  
01/31/12    is     Add flow control on diagcomm_sio_tx_wmq
01/24/12    is     Support for buffered circular mode
01/05/12    is     Revert DSM increases for SIO_TX pool
11/04/11    is     Support for buffered threshold mode
04/23/10    JV     For the SIO_TX pool, disable flow if we have only 3 items free
                   and enable it back on if we have 10 items free.
10/06/09    JV     Mainlined FEATURE_DSM_DIAG_ITEMS.
04/15/09    mad    Reduced SIO_TX count on slave processors
04/03/09    mad    Created from diagdsm.c
===========================================================================*/

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SIO RX
  (Receive diag pkts from USB on master. 
  Receive diag pkts forwarded via SMD on slave.)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SIO_RX_ITEM_SIZ
#define DSMI_DIAG_SIO_RX_ITEM_SIZ                 2048
#endif

#if !defined DSMI_DIAG_SIO_RX_ITEM_CNT
#define DSMI_DIAG_SIO_RX_ITEM_CNT                 13
#endif

#if !defined DSMI_DIAG_SIO_FUSION_RX_ITEM_SIZ
#define DSMI_DIAG_SIO_FUSION_RX_ITEM_SIZ                 0
#endif

#if !defined DSMI_DIAG_SIO_FUSION_RX_ITEM_CNT
#define DSMI_DIAG_SIO_FUSION_RX_ITEM_CNT                 25
#endif


  #define DSMI_DIAG_SIO_RX_ITEM_DONT_EXCEED         3 /* we have 3 re-tries for a packet */
  #define DSMI_DIAG_SIO_RX_ITEM_FEW_MARK            ((DSMI_DIAG_SIO_RX_ITEM_CNT)/10)
  #define DSMI_DIAG_SIO_RX_ITEM_MANY_MARK           ((DSMI_DIAG_SIO_RX_ITEM_CNT) - 10)

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SIO TX
  (Transmit to USB on master
  Transmit via SMD on slave)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SIO_TX_ITEM_SIZ
#define DSMI_DIAG_SIO_TX_ITEM_SIZ                 8192
#endif

#if !defined DSMI_DIAG_SIO_TX_ITEM_CNT
#if defined DIAG_MP_MASTER
#define DSMI_DIAG_SIO_TX_ITEM_CNT                 13
#else
#define DSMI_DIAG_SIO_TX_ITEM_CNT                 13 /* Making the count less on slave processor, since master should have enough DSM items to handle sum of all slave traffic */
#endif /* defined DIAG_MP_MASTER */
#endif
  
  #define DSMI_DIAG_SIO_TX_ITEM_DONT_EXCEED         2
  #define DSMI_DIAG_SIO_TX_ITEM_FEW_MARK            3
  #define DSMI_DIAG_SIO_TX_ITEM_MANY_MARK           5 

/*---------------------------------------------------------------------------
  Size, Count, Few, many and do not exceed counts for DIAG items SMD TX
  (Receive diag pkts from slave and Transmit to USB, in master
  Not used, in slave)
---------------------------------------------------------------------------*/
#if !defined DSMI_DIAG_SMD_TX_ITEM_SIZ
#define DSMI_DIAG_SMD_TX_ITEM_SIZ                   (DSMI_DIAG_SIO_TX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_SMD_TX_ITEM_CNT
#define DSMI_DIAG_SMD_TX_ITEM_CNT                   (13)
#endif

#ifdef DIAG_CONSUMER_API
  #if !defined DSMI_DIAG_SMD_DCI_TX_ITEM_SIZ
    #define DSMI_DIAG_SMD_DCI_TX_ITEM_SIZ                   0
  #endif
  #if !defined DSMI_DIAG_SMD_DCI_TX_ITEM_CNT
    #define DSMI_DIAG_SMD_DCI_TX_ITEM_CNT                   (25)
  #endif
#endif /* #ifdef DIAG_CONSUMER_API */

  
  /* Adjust to tune DMS flow control to prevent running out of DSM items */
  #define DSMI_DIAG_SMD_TX_ITEM_DONT_EXCEED         (2)
  /* Number of items we want consumed before triggering flow control */
  #define DSMI_DIAG_SMD_TX_ITEM_MANY_MARK           5
  /* Number of items we want outstanding before restoring flow */
  #define DSMI_DIAG_SMD_TX_ITEM_FEW_MARK            3

    /* Number of items we want consumed before triggering flow control */
  #ifdef DIAG_CONSUMER_API
    #define DSMI_DIAG_SMD_DCI_TX_ITEM_MANY_MARK           10
    /* Number of items we want outstanding before restoring flow */
    #define DSMI_DIAG_SMD_DCI_TX_ITEM_FEW_MARK            3
  #endif /* #ifdef DIAG_CONSUMER_API */
  
  /* Adjust to tune WMQ flow control to prevent silent DSM drops when DNE is exceeded */
  #define DSMI_DIAG_SMD_TX_LOW_WM_MARK              (10)
  #define DSMI_DIAG_SMD_TX_HIGH_WM_MARK             (DSMI_DIAG_SMD_TX_ITEM_CNT / 2)
  
  /* Number of items we want consumed before triggering flow control */
  #define DSMI_DIAG_SMD_TX_ITEM_BUFFERED_MANY_MARK           ((DSMI_DIAG_SMD_TX_ITEM_CNT) - 8)
  /* Number of items we want outstanding before restoring flow */
  #define DSMI_DIAG_SMD_TX_ITEM_BUFFERED_FEW_MARK            ((DSMI_DIAG_SMD_TX_ITEM_BUFFERED_MANY_MARK) - 7)
  
#if !defined DSMI_DIAG_SMD_TX_ITEM_BUFFERED_DRAIN_MANY_MARK
  #define DSMI_DIAG_SMD_TX_ITEM_BUFFERED_MANY_DRAIN_MARK  ((DSMI_DIAG_SMD_TX_ITEM_CNT) - 15)
#endif

#if !defined DSMI_DIAG_SMD_TX_ITEM_BUFFERED_DRAIN_FEW_MARK
  #define DSMI_DIAG_SMD_TX_ITEM_BUFFERED_FEW_DRAIN_MARK   (5)
#endif

/* Command/Response Path */
#if !defined DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_SIZ                  (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SMD_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_CNT                  (DSMI_DIAG_SIO_RX_ITEM_CNT)
#endif

#define DSMI_DIAG_CMD_SMD_RX_ITEM_DONT_EXCEED            (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_FEW_MARK             (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SMD_RX_ITEM_MANY_MARK            (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_SIZ                   2048 
#endif

#if !defined DSMI_DIAG_CMD_SMD_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_CNT                   13
#endif

  #define DSMI_DIAG_CMD_SMD_TX_ITEM_DONT_EXCEED           5
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_FEW_MARK              ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SMD_TX_ITEM_MANY_MARK             ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT) - 10)

#if !defined DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_SIZ               0
#endif

#if !defined DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_CNT               (DSMI_DIAG_SIO_RX_ITEM_CNT)
#endif

  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_DONT_EXCEED       (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_FEW_MARK          (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SMD_DCI_RX_ITEM_MANY_MARK         (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_SIZ                0 
#endif

#if !defined DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT                25
#endif

  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_DONT_EXCEED        5
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_FEW_MARK           ((DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_MANY_MARK          ((DSMI_DIAG_CMD_SMD_DCI_TX_ITEM_CNT) - 10)

#if !defined DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_SIZ                    (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SIO_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_CNT                    5
#endif

  #define DSMI_DIAG_CMD_SIO_RX_ITEM_DONT_EXCEED            (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_FEW_MARK               (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SIO_RX_ITEM_MANY_MARK              (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_SIZ                    2048 
#endif

#if !defined DSMI_DIAG_CMD_SIO_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_CNT                    5
#endif

  #define DSMI_DIAG_CMD_SIO_TX_ITEM_DONT_EXCEED            5
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_FEW_MARK               ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SIO_TX_ITEM_MANY_MARK              ((DSMI_DIAG_SIO_CMD_TX_ITEM_CNT) - 10)

#if !defined DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_SIZ                (DSMI_DIAG_SIO_RX_ITEM_SIZ)
#endif

#if !defined DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_CNT                10
#endif

  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_DONT_EXCEED        (DSMI_DIAG_SIO_RX_ITEM_CNT)
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_FEW_MARK           (DSMI_DIAG_SIO_RX_ITEM_FEW_MARK)
  #define DSMI_DIAG_CMD_SIO_DCI_RX_ITEM_MANY_MARK          (DSMI_DIAG_SIO_RX_ITEM_MANY_MARK)
  
#if !defined DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_SIZ                2048 
#endif

#if !defined DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT                10
#endif

  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_DONT_EXCEED        5
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_FEW_MARK           ((DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT)/10)
  #define DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_MANY_MARK          ((DSMI_DIAG_CMD_SIO_DCI_TX_ITEM_CNT) - 10)

#endif /* DIAGDSMI_H */

