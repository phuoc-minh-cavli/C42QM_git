#ifndef _HSU_AL_SER_I_H_
#define _HSU_AL_SER_I_H_

/*==============================================================================

Serial Functions (Modem/DIAG/NMEA) Adaptation Layer for High Speed USB
                            Internal Header File

GENERAL DESCRIPTION
Contains definitions for the constants that is shared internally among the 
HS-USB seral adaptation layer.

EXTERNALIZED FUNCTIONS
None

Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_ser_i.h#1 $
$DateTime: 2020/01/30 22:49:35 $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
06/20/06 esh  First revision of file  
==============================================================================*/
/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "jusb_int.h"  /* For USB_2_MAX_BULK_PACKET */
#include "sio.h"

/*==============================================================================

Constants and Macros

==============================================================================*/

#if !defined(FEATURE_USB_DIAG)
#error("FEATURE_USB_DIAG is NOT defined!")
#endif /* !FEATURE_USB_DIAG */

#define AL_SER_PORT_MDM (SIO_PORT_USB_MDM)
#define AL_SER_PORT_DIAG (SIO_PORT_USB_DIAG)
  
#ifdef FEATURE_USB_DIAG_NMEA
#define AL_SER_PORT_NMEA (SIO_PORT_USB_NMEA)
#else /* !FEATURE_USB_DIAG_NMEA */
#define AL_SER_PORT_NMEA (SIO_PORT_NULL)
#endif /* FEATURE_USB_DIAG_NMEA */
  
#ifdef FEATURE_HS_USB_SER3_PORT
#define AL_SER_PORT_USB_SER3 (SIO_PORT_USB_SER3)
#endif /* FEATURE_HS_USB_SER3_PORT */
  
#ifdef FEATURE_HS_USB_SER5_PORT
#define AL_SER_PORT_USB_SER5 (SIO_PORT_USB_SER5)
#endif /* FEATURE_HS_USB_SER5_PORT */ 

#ifdef FEATURE_HS_USB_SER6_PORT
#define AL_SER_PORT_USB_SER6 (SIO_PORT_USB_SER6)
#endif /* FEATURE_HS_USB_SER6_PORT */

#ifdef FEATURE_HS_USB_DCI_PORT
#define AL_SER_PORT_USB_DCI (SIO_PORT_USB_DCI)
#endif /* FEATURE_HS_USB_DCI_PORT */

#ifdef FEATURE_HS_USB_SER4_PORT
#define AL_SER_PORT_USB_SER4 (SIO_PORT_USB_SER4)
#endif /* FEATURE_HS_USB_SER4_PORT */
  
#ifdef FEATURE_HS_USB_QDSS_PORT
#define AL_SER_PORT_USB_QDSS (SIO_PORT_USB_QDSS)
#endif /* FEATURE_HS_USB_QDSS_PORT */

#ifdef FEATURE_HS_USB_CSVT_PORT
#define MAX_NUM_OF_ACM_USERS 2
#define AL_SER_PORT_CSVT (SIO_PORT_USB_CSVT)
#define AL_SER_PORT_MDM2 (SIO_PORT_USB_MDM2)

#endif /* FEATURE_HS_USB_CSVT_PORT */


/*==============================================================================
 * Number of users for each serial port
 *============================================================================*/
/* Diag port */
#define NUM_OF_DIAG_OBEX_USERS              (1)

/* NMEA port */
#ifdef FEATURE_HS_USB_NMEA 
#define NUM_OF_NMEA_OBEX_USERS              (1)
#else
#define NUM_OF_NMEA_OBEX_USERS              (0)
#endif

/* AT commands port */
#ifdef FEATURE_HS_USB_SER3_PORT 
#define NUM_OF_SER3_PORT_OBEX_USERS         (1)
#else
#define NUM_OF_SER3_PORT_OBEX_USERS         (0)
#endif

#ifdef FEATURE_HS_USB_SER5_PORT 
#define NUM_OF_SER5_PORT_OBEX_USERS         (1)
#else
#define NUM_OF_SER5_PORT_OBEX_USERS         (0)
#endif

#ifdef FEATURE_HS_USB_SER6_PORT 
#define NUM_OF_SER6_PORT_OBEX_USERS         (1)
#else
#define NUM_OF_SER6_PORT_OBEX_USERS         (0)
#endif

/* DCI port */
#ifdef FEATURE_HS_USB_DCI_PORT 
#define NUM_OF_DCI_PORT_OBEX_USERS         (1)
#else
#define NUM_OF_DCI_PORT_OBEX_USERS         (0)
#endif

#ifdef FEATURE_HS_USB_SER4_PORT 
#define NUM_OF_SER4_PORT_OBEX_USERS         (1)
#else
#define NUM_OF_SER4_PORT_OBEX_USERS         (0)
#endif

#ifdef FEATURE_HS_USB_QDSS_PORT 
#define NUM_OF_QDSS_PORT_OBEX_USERS         (1)
#else
#define NUM_OF_QDSS_PORT_OBEX_USERS         (0)
#endif

#define MAX_NUM_OF_OBEX_USERS               ((NUM_OF_NMEA_OBEX_USERS)       + \
                                             (NUM_OF_SER3_PORT_OBEX_USERS)  + \
                                             (NUM_OF_SER4_PORT_OBEX_USERS)  + \
                                             (NUM_OF_DIAG_OBEX_USERS)       + \
                                             (NUM_OF_SER5_PORT_OBEX_USERS)  + \
                                             (NUM_OF_SER6_PORT_OBEX_USERS)  + \
                                             (NUM_OF_DCI_PORT_OBEX_USERS)   + \
                                             (NUM_OF_QDSS_PORT_OBEX_USERS))


/*==============================================================================
 * Tx max pending H/W queue buffers
 *============================================================================*/

#define HS_USB_DIAG_TX_MAX_PENDING_BUFFERS    (5)
#define HS_USB_ACM_TX_MAX_PENDING_BUFFERS     (5)
#define HS_USB_NMEA_TX_MAX_PENDING_BUFFERS    (5)
#define HS_USB_SER3_TX_MAX_PENDING_BUFFERS    (5)
#define HS_USB_SER5_TX_MAX_PENDING_BUFFERS    (5)
#define HS_USB_SER6_TX_MAX_PENDING_BUFFERS    (5)
#define HS_USB_DCI_TX_MAX_PENDING_BUFFERS     (5)
#define HS_USB_SER4_TX_MAX_PENDING_BUFFERS    (1)
#define HS_USB_QDSS_TX_MAX_PENDING_BUFFERS    (1)

//==============================================================================
// Rx buffer count for each serial port
//==============================================================================

// Please make sure MAX_RX_ARRAY_CNT is max of
// DIAG, ACM, etc. RX_BUFF_CNT otherwise device will HSU_ERR_FATAL
#define HS_USB_SER_MAX_RX_ARRAY_CNT  (1)

#define HS_USB_DIAG_RX_BUFF_CNT      (1)
#define HS_USB_ACM_RX_BUFF_CNT       (1)
#define HS_USB_NMEA_RX_BUFF_CNT      (1)
#define HS_USB_SER3_RX_BUFF_CNT      (1)
#define HS_USB_SER5_RX_BUFF_CNT      (1)
#define HS_USB_SER6_RX_BUFF_CNT      (1)
#define HS_USB_DCI_RX_BUFF_CNT       (1)
#define HS_USB_SER4_RX_BUFF_CNT      (1)
#define HS_USB_QDSS_RX_BUFF_CNT      (1)


/*==============================================================================
 * Defines Tx handle complete timer values, only used when TX_TIMEOUT is 0
 * 
 * A timer for calling the stack cdc_handle_tx_completion function.
 * When this API is called, the stack will notify us on completed transfers. 
 * Needed when TX_TIMEOUT_MS is set to 0, otherwise when we reach 
 * the High WM the application stops calling the transmit function and usb_
 * stack will not check for completed transfers and we will never go back to 
 * the Low WM. 
 *============================================================================*/

/* Tx handle complete timer value in millisecond */
#define HS_USB_DIAG_TX_COMP_TIMER_VAL       (20)
#define HS_USB_ACM_TX_COMP_TIMER_VAL        (20)
#define HS_USB_NMEA_TX_COMP_TIMER_VAL       (20)
#define HS_USB_SER3_TX_COMP_TIMER_VAL       (20)
#define HS_USB_SER5_TX_COMP_TIMER_VAL       (20)
#define HS_USB_SER6_TX_COMP_TIMER_VAL       (20)
#define HS_USB_DCI_TX_COMP_TIMER_VAL        (20)
#define HS_USB_SER4_TX_COMP_TIMER_VAL       (20)
#define HS_USB_QDSS_TX_COMP_TIMER_VAL       (20)

/*==============================================================================
 * Defines Tx timeout
 * If this value is not "0", Tx handle complete timeout will not be used.
 *============================================================================*/

/* Zero TX timeout significantly improve CPU utilization with a low risk of 
 * delayed freeing of DSM items */
#define HS_USB_DIAG_TX_TIMEOUT_MS           (0)
#define HS_USB_ACM_TX_TIMEOUT_MS            (0)
#define HS_USB_NMEA_TX_TIMEOUT_MS           (0)
#define HS_USB_SER3_TX_TIMEOUT_MS           (0)
#define HS_USB_SER5_TX_TIMEOUT_MS           (0)
#define HS_USB_SER6_TX_TIMEOUT_MS           (0)
#define HS_USB_DCI_TX_TIMEOUT_MS            (0)
#define HS_USB_SER4_TX_TIMEOUT_MS           (0)
#define HS_USB_QDSS_TX_TIMEOUT_MS           (0)

/*==============================================================================
 * Defines Rx handle complete timer values
 *============================================================================*/
#define HS_USB_DIAG_RX_TIMEOUT_MS           (0)
#define HS_USB_ACM_RX_TIMEOUT_MS            (0)
#define HS_USB_NMEA_RX_TIMEOUT_MS           (0)
#define HS_USB_SER3_RX_TIMEOUT_MS           (0)
#define HS_USB_SER5_RX_TIMEOUT_MS           (0)
#define HS_USB_SER6_RX_TIMEOUT_MS           (0)
#define HS_USB_DCI_RX_TIMEOUT_MS            (0)
#define HS_USB_SER4_RX_TIMEOUT_MS           (0)
#define HS_USB_QDSS_RX_TIMEOUT_MS           (0)

/*==============================================================================

Typedefs

==============================================================================*/

/*==============================================================================

PUBLIC FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/

uint32 al_ser_get_obex_slot_idx_for_sio_port
(
  sio_port_id_type port_id,
  uint32 *core_index
);


#endif /* _HSU_AL_SER_I_H_ */
