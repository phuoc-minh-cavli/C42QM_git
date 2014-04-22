#ifndef MHI_I_H
#define MHI_I_H
/**
  @file mhi_i.h
  @brief
  This file contains the internal data structures, definitions shared by the 
  various MHI driver layers 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "com_dtypes.h" 

#ifndef MHI_BOOT
#include "assert.h"
#endif

/* Execution environment */
#ifdef MHI_BOOT
#define MHI_ENV 1
#else
#define MHI_ENV 2
#endif

/* Channel assignment */
#define HWCHANNELNUM     8
#define HWCHANNELBASE    100
#define HWCHANNELBASEEND 107
#define HWEVENTRINGNUM   2

/* Define the start(inclusive) and end (non inclusive) SW channel indexes */
#ifndef MHI_BOOT
#define SWCHSTART 0
#define SWCHEND   HWCHANNELBASE
#else
#define SWCHSTART 2
#define SWCHEND   4 
#endif


/* PRIMARY event ring */
#define PRIMARY_ER 0 

/* Host address indices */
#define ADDR_CTRL 0 
#define ADDR_DATA 1 

/* Internal return codes  */
#define SUCCESS        0
#define RING_EMPTY     1
#define RING_FULL      2
#define RING_THROTTLED 3
#define TRB_INCOMPLETE 4 
#define REQUESTQ_EMPTY 5
#define BADTRB         6
#define SYSERR         7
#define PENDING_EVENT  8
#define INVALIDCH      9
#define NULL_PTR       10
#define LINK_BUSY      11
#define LINK_ERROR     12
#define PENDING        13
#define CACHE_PENDING  14
#define INVALID_OFFSET 15
#define ABORT          16
#define ERROR          0xFFFFFFFF

/* Levels */
#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef ASSERTED
#define ASSERTED 1
#endif

#ifndef DEASSERTED
#define DEASSERTED 0
#endif

#ifndef ENABLED
#define ENABLED 1
#endif

#ifndef DISABLED
#define DISABLED 0
#endif


#ifndef MHI_BOOT
#define MHI_ASSERT(x) ASSERT(x)
#else
#define MHI_ASSERT(x) 
#endif

/* Max sample size for rolling average */
#define ROLLING_AVG_MAX_SAMPLE_SIZE 10

/* Max time in ms that a link request should take to complete */
#define MAX_TIME_FOR_LINKREQ_MS 1000

/* Max time in ms that M2 exit should take*/
#define MAX_TIME_FOR_M2_EXIT_MS 20

/* Supress compiler warnings for variables that are unused mostly as a result of featurising */ 
#define NOT_USED(x) ((void)(x))

#define ALIGN_OFFSET 64

/** MHI Config structure, defines the MHI capabilities, configuration etc.. */
typedef struct _mhi_config
{
   uint32 mhiRegLen;  /**< MHI register length **/
   uint32 version;    /**< MHI version **/  
   uint32 nEr;        /**< Number of event rings **/
   uint32 nCh;        /**< Number of channels **/
   uint32 nHwEr;      /**< Number of hardware event rings **/
   uint32 nHwCh;      /**< Number of hardware channels **/
   uint32 chdboff;    /**< Channel doorbell array offset **/
   uint32 erdboff;    /**< Event ring doorbell array offset **/
}mhi_config_t;

/** Defines the MHI states **/
typedef enum _mhi_state
{
   MHI_STATE_RESET=0x0,   /**< Reset or initial state of MHI device */
   MHI_STATE_READY=0x1,   /**< Ready state. Entered when device initializes MHI */
   MHI_STATE_M0=0x2,      /**< M0 state , entered when host sets MHISTATE = M0 */
   MHI_STATE_M1=0x3,      /**< M1 state , entered when device detects link inactivity */
   MHI_STATE_M2=0x4,      /**< M2 state , entered when host sets MHISTATE = M2 */
   MHI_STATE_M3=0x5,      /**< M3 state , entered when host sets MHISTATE = M3 */
   MHI_STATE_MAX,         /**< Number of valid M states */
   MHI_STATE_SYSERR=0xFF, /**< System Error state. Entered when device detects a system error */
}mhi_state_t; 

#define M0_LATENCY_US 300
#define M1_LATENCY_US 300
#define M2_LATENCY_US 16000

/* Invalid MHI interrupt vector */
#define MHI_ISR_INVALID_VEC 0xFFFFFFFF

/** Retrieves high dword of uint64 */
#define QUADWORD_HIGH(W) (uint32)(W>>32)
/** Retrieves low dword of uint64 */
#define QUADWORD_LOW(W)  (uint32)(W&0x00000000FFFFFFFF)

/** Converts Quad word to two dwords */
#define QW2DW(x) QUADWORD_HIGH(x), QUADWORD_LOW(x)

/** Converts time in us to time in ms */
#define TIME_US_TO_MS(time_us) ((time_us)/1000)

/* Converts virtual address to physical given the virtual and physical bases */
#define VA2PA(VA, VA_BASE, PA_BASE) ((VA)-(VA_BASE)+(PA_BASE))

/* Channel command types */
typedef enum 
{
   CH_CMD_RESET=0,   /**< Reset Channel command */ 
   CH_CMD_STOP=1,    /**< Stop channel command */ 
   CH_CMD_START=2    /**< Start channel command */ 
}ch_cmd_t;


/** Defines Generic Address Structure */ 
typedef struct _mhi_addr
{
   uint32 va;        /**< Virtual address */
   uint32 pa;        /**< Physical address */ 
}mhi_addr_t;

#endif
