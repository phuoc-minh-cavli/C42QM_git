#ifndef __DPS_H__
#define __DPS_H__

#include "ULogFront.h"

/* -------------------------------------------------------------------------
** DPS Task Signal Definitions
** ------------------------------------------------------------------------- */
// DIAG service sigs
#define DPS_ALT_DIAG_OPEN_SIG       	0x00000010    /* open up channel with non active interface 
														 with alt diag services */
#define DPS_ALT_DIAG_CLOSE_SIG      	0x00000020    /* close the channel with non active interface 
														 with alt diag services */
#define DPS_ALT_DIAG_SWITCH_SIG     	0x00000040    /* ALT DIAG switch notification */
#define DPS_DIAG_SWITCH_SIG     		0x00000080    /* DIAG switch notification */
#define DPS_ALT_DIAG_SWITCH_FAIL_SIG    0x00000100    /* service switch failure - trigger fallback */
#define DPS_DIAG_SWITCH_FAIL_SIG    	0x00000200    /* service switch failure - trigger fallback */
#define DPS_ALT_DIAG_SIO_RX_SIG     	0x00000400    /* packet received on non active interface */

// DUN service sigs
#define DPS_ALT_DUN_OPEN_SIG       	    0x00001000    /* open up channel with non active interface 
														 with alt dun services */
#define DPS_ALT_DUN_CLOSE_SIG      	    0x00002000    /* close the channel with non active interface 
														 with alt dun services */
#define DPS_ALT_DUN_SWITCH_SIG      	0x00004000    /* ALT DUN switch notification */
#define DPS_DUN_SWITCH_SIG     	    	0x00008000    /* DUN switch notification */
#define DPS_ALT_DUN_SWITCH_FAIL_SIG     0x00010000    /* service switch failure - trigger fallback */
#define DPS_DUN_SWITCH_FAIL_SIG     	0x00020000    /* service switch failure - trigger fallback */
#define DPS_ALT_DUN_SIO_RX_SIG     	    0x00040000    /* packet received on non active interface */
#define DPS_TIMER_SIG     	            0x00080000    /* DPS timer sig received */

typedef struct 
{
  ULogHandle dps_log;
  uint32     log_size;
} dps_ulog_vars_type;

extern dps_ulog_vars_type dps_ulog_vars; 

void dps_ulog_init(void);

#endif //__DPS_H__


