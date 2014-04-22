#ifndef TIMETEST_H
#define TIMETEST_H
/*===========================================================================

			  T I M E T E S T   H E A D E R    F I L E

DESCRIPTION
  This header file contains all the definitions necessary for supporting
  timing tests via the LED output buffer.

Copyright (c) 1993, 1995, 1996 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1997, 1998, 1999 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2001 - 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2005 - 2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

							 EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/api/debugtools/timetest.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*===========================================================================

TIMETEST CONFIGURATION

===========================================================================*/

/* Multiple task / ISR profiling */
#define TIMETEST_PROF // ALWAYS ON

/* Function level profiling */
#define TIMETEST_FUNCS // ALWAYS ON

/* Function level profiling only (no tasks / ISRs) */
#undef TIMETEST_NO_TASKS // NEVER ON

/* Only count total ISR time, not individual ISR routines */
#undef TIMETEST_NO_ISRS // NEVER ON

#define TIMETEST_DSRLP_STATS // ALWAYS ON

#define TIMETEST_EXPLICIT_TRIGGER // ALWAYS ON

#define TIMETEST_ISR_LEVEL // ALWAYS ON

#define FEATURE_MULTIPORT_TIMETEST // ALWAYS ON

#define L4_Set_Profiling_ID(id) // L4 Backward compatibility

/*===========================================================================

INCLUDES

===========================================================================*/

#include "profile.h" // cross dependency, remove

/*===========================================================================

DATA DECLARATIONS

===========================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

#define TIMETEST_DEPRECATED_ATTRIBUTE __attribute__((deprecated))

// historically, this interface was used by software to instrument diagnostic
// writes to a visible 8 bit (16 bit) port with an attached logic analyzer.

TIMETEST_DEPRECATED_ATTRIBUTE extern unsigned long timetest_port; // software provided timetest_port memory location (mobile.c)
#define TIMETEST_PORT (void*)(&timetest_port)

/* 16-bit value used as a unique parse token for the postprocessor */
#define TT_PARSE_TOKEN            0xffff
#define TT_PARSE_TOKEN_ISR_CONT   0x8000
#define TT_PARSE_TOKEN_ISR_LEVEL  0x0f00
#define TT_PARSE_TOKEN_IS856      0x4000
#define TT_PARSE_TOKEN_IS2K       0x2000
#define TT_PARSE_TOKEN_APPS       0x1000

/* Values used to distinguish between 1x and HDR data rates */
#define TIMETEST_DSRLP_IS856   TT_PARSE_TOKEN_IS856
#define TIMETEST_DSRLP_IS2K    TT_PARSE_TOKEN_IS2K

#define TIMETEST_MAX_TASK_ID                 0x7c

#define TIMETEST_INT_TASK_ID                 TIMETEST_MAX_TASK_ID
#define TIMETEST_ISR_NESTED_ID               0xff

/*** New timetest for function and ISR profiling ***/

#define TT_LOCK() INTLOCK()
#define TT_FREE() INTFREE()

extern unsigned char timetest_isr_markers[32];
extern unsigned char timetest_isr_level;
extern unsigned long timetest_parse_token;
// extern unsigned long timetest_markers[TIMETEST_MAX_TASK_ID][TIMETEST_MAX_FUNC_LEVEL+1];

/* Macro to write an ISR's ID to the LED port */

//#define TIMETEST_ISR_ID(id)
//            TT_LOCK();
//            timetest_isr_markers[timetest_isr_level] = id;
//            outpw (TIMETEST_PORT, (uint16)(0x0000));
//            BIO_LED_BUF = (((timetest_isr_level & 0x0f) << 8) | id);
//            outpw(TIMETEST_PORT, (uint16)BIO_LED_BUF);
//            TT_FREE()

#define TIMETEST_ISR_ID(id) { *TIMETEST_PORT = 0; }

/* Value on the buffer output when in the Interrupt Handler */
#define TIMETEST_INT   TIMETEST_MAX_TASK_ID

/* Macro to write RLP rx and tx statistics out */
/* This macro has been modified specific for 6500 */

// #define TIMETEST_DSRLP_BYTES(rlp_type, rxb, txb)
//      TT_LOCK();
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)(rlp_type));
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, ((((uint16)(rxb)) << 8) | txb));
//      TT_FREE();

#define TIMETEST_DSRLP_BYTES(rlp_type, rxb, txb) /* { *TIMETEST_PORT = 0; } */

/* Macros to write the function address upon beginning and   */
/* end of a function body.                                   */

/* This is dependent on the assembly code in rexarm.s  */
/* Don't change it !!                                  */

#define TIMETEST_MAX_FUNC_LEVEL     31

//#define TT_OUT_FADDR(tid, f_addr)
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)(TT_PARSE_TOKEN));
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)(tid));
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)((((uint32)f_addr) & 0xffff0000) >> 16));
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)(((uint32)f_addr) & 0x0000ffff));

//#define TT_OUT_FADDR_ONLY(tid, f_addr)
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)(TT_PARSE_TOKEN));
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)((((uint32)f_addr) & 0xffff0000) >> 16));
//      outpw(TIMETEST_PORT, (uint16)(0x0000));
//      outpw(TIMETEST_PORT, (uint16)(((uint32)f_addr) & 0x0000ffff));

//#define TT_BEGIN_FUNC(f_addr)
//    { TT_LOCK();
//    if (! rex_self()->leds) ERR("LEDs not set for task %x",rex_self(),0,0);
//    else {
//    ASSERT(timetest_markers[rex_self()->leds][0] < 31);
//    timetest_markers[rex_self()->leds][0]++;
//    timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]]
//      = (uint32) f_addr;
//    TT_OUT_FADDR(rex_self()->leds, f_addr);
//    } TT_FREE(); }

#define TT_BEGIN_FUNC(f_addr) /* { *TIMETEST_PORT = 0; } */

//#define TT_END_FUNC(f_addr)
//    { TT_LOCK();
//    if (rex_self()->leds && timetest_markers[rex_self()->leds][0]){
//    timetest_markers[rex_self()->leds][timetest_markers[rex_self()->leds][0]]
//      = 0x00;
//    timetest_markers[rex_self()->leds][0]--;
//    TT_OUT_FADDR(rex_self()->leds,
//                  timetest_markers[rex_self()->leds]
//                                  [timetest_markers[rex_self()->leds][0]]);
//    } TT_FREE(); }

#define TT_END_FUNC(f_addr) /* { *TIMETEST_PORT = 0; } */

extern void timetest_prof_init(void);

#if defined(__cplusplus)
}
#endif

#endif  /* TIMETEST_H */
