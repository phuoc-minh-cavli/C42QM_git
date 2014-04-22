/*===========================================================================

                  Shared Memory Driver SIO Interface Source File


 Copyright (c) 2005 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smd/src/smd_sio.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/17/14   rv      Timer undef is added.
06/26/13   bm      Expose API to get SIO port ID given SMD port name.
01/31/14   rv      Replaced the memcpy and memmove functions with their 
                   respective secure memscpy and memsmove functions.
08/01/13   bt      Trigger DTR callbacks when they are first registered.
07/24/13   bt      Make SMD use secure SMEM partitions.
05/01/13   bm      Remove usage of DSM_DIAG_SIO_RX_ITEM_POOL for DIAG.
04/13/12   pa      Use size param from sio_open_type in smd_sio_control_open
01/11/12   bm      Invert smd_sio_get_port_defaults() logic to set PKTTFR_TYPE
                   as the default transfer mode.
12/14/11   bm      Open SIO_PORT_SMD_DATA11 data channel in streaming mode by 
                   default to allow it to use tty interface on Linux side.
11/13/11   rs      + Fixed SIO Device registration of DIAG2 port 
                   + Corrected parameter validation for DIAG2 SMD SIO port
10/17/11   bm      Removing legacy SMD Bridge dependencies from smd_sio layer.
                   + Adding support for an extra DIAG port - CL 1983230
07/20/11   bt      Added Asserts to check for NULL sio info structures.
04/28/11   bt      Add default to switch in smd_sio_esc_seq_guardtime_isr.
04/01/11   bm      Added smd_xfrflow_type param as part of smd_open_param_type
                   struct and removed it as a separate argument from 
                   smd_sio_get_port_defaults().
03/23/11   bm      Added handling of port_ids SIO_PORT_SMD_RPC_CALL/
                   SIO_PORT_SMD_RPC_REPLY in smd_sio_get_port_defaults
01/06/11   bt      Removed SMD Block and Data Mover protocols.
12/02/10   rs      Fixed use of SIO port id in IOCTL command handler 
11/24/10   hwu     Enable dtr callback registration for control port.  
10/08/10   esh     Changed DSM_DS_LARGE_ITEM_POOL to DSM_DS_SMALL_ITEM_POOL for
                   compatibility with legacy code in smd_sio_get_port_defaults.
09/28/10   tl      Set SMD bridge ports to packet mode
09/13/10   hwu     Let client to specify the dsm pool for control ports.
09/08/10   hwu     Enable control port to queue pkts before port is fully open. 
08/25/10   hwu     Implemented smd_sio_control_ioctl.
07/06/10   hwu     Changed to use smd_sio_ioctl for control port.
07/21/10   tl      Implemented SMD API for SMD priority.
06/07/10   tl      Expanded bridge ports to the 14 supported by bridge
05/03/10   tl      Use critical sections instead of INTLOCK
02/11/10   esh     Added temporary featurization for Q6 integration
01/11/10   hwu     Change to let SIO client pass in smd fifo size.
12/11/09   hwu     Need to enable the flow control when opens a control
                   port.
12/10/09   hwu     fix 0 lengh packet header issue
12/08/09   esh     Added checks to fix Klocwork warnings
12/01/09   hwu     Changed to assign diag dsm pool by using the sio param. 
11/25/09   rs      Removed usage of timers for QDSP6 and qube 
11/25/09   hwu     Merged the control port demux changes 
                   Fixed the data ports transfer type/dsm poll selection bug.
                   Removed smd_channel_ids and smd_control_channel_ids.
11/18/09   hwu     Fixed the issue where allocating the same port twice failed.
11/12/09   hwu     Featurize the bridge API access. 
11/11/09   hwu     Reverted the control demux changes. 
10/25/09   rs      Fixed support for control ports, by splitting port_to_info 
                   structs in two different arrays
10/12/09   bfc/rs  Removed control port muxing.
09/29/09   bfc/rs  Added variable sized FIFO API support.
09/29/09   bfc/rs  Fix an array sizing problem in smd-_channel_ids.
08/04/09   nnv     Updated code to not process data in autodetect mode,
                   just pass it up.
06/26/09   hwu     new smd_sio/bridge check in.
06/25/09   hwu     Fixed the SIO3.0 incompatible issue. Needs to use both 
                   the major and the minor number to determine a connection. 
06/10/09   hwu     Merged in support for data on modem.
06/10/09   bfc     CMI updates.
03/31/09   hwu     Merge in the changes to enable data stacking in smd_sio.
03/12/09   jlk     fix for diag pools
02/13/09   jlk     Added SIO minor ID get macro for forwards compatibility
01/21/09   bfc     Added new ioctl translations.
10/16/08   jlk     added features to be compatible with different sio.h 
                   versions
09/18/08   jlk     removed tranlation arrays to break dependence on sio.h
                   enumerations
09/16/08   jlk     Added special ports for diag
06/10/08   sa      Smd_target.h cleanup. 
04/29/08   ih      Added WM7 support
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
11/30/07   ptm     Fix compiler warnings.
10/31/07   hwu     Added smd_port_control to smd_sio_port_translation[] to 
                   match up with sio.h.
10/08/07   hwu     Changed the order to include smd_os.h and smd.h
                   because smd_channel_type moved to smd.h
07/25/07   bfc     We no longer support RPC ports from this layer.
05/10/07   hn      Fixed featurization around setting of default dsm_pool_id.
05/02/07   hwu     Change to use SMD_NUM_CHANNELS. 
04/26/07   ptm     Use SMD NUM PORTS in place of SMD NUM STREAMS
02/28/07   ptm     Updated smd port ID and ioctl cmd translations.
02/22/07   ptm     Added VOTE TO SLEEP/WAKEUP IOCTLs and fixed compiler warning.
02/12/07   bfc     Added an IOCTL for smd_bridge
12/18/06   bfc     Moved control port logic to the smd_wm_layer
10/14/06   ptm     Changed WinMobile featurization to support DIAG on WinMobile.
11/03/06   ptm     Change SMD_SMD_... symbols to SMD_... symbols.
10/16/06   ptm     Change "change mode" IOCTL to use smd param struct instead
                   of sio param struct.
10/10/06   ptm     Remove smsm.h and fix a few lint warnings.
09/29/06   ptm     Add code to update the rx_func_ptr in smd_sio_change_mode.
09/28/06   taw     Added translation tables for translating SIO ports and 
                   commands to SMD ports and commands.  Translate from SIO to
                   SMD before calling into smd_wm.
09/18/06   ptm     Remove references to REX.
09/16/06   ptm     Featurized DIAG dsm item pools for WinMobile.
09/15/06   ptm     Use new DIAG dsm item pools.
08/27/06   ptm     Changed control port to use uint32 instead of enum.
08/23/06   ptm     Changed name of oncrpc dsm item pool
08/23/06   ptm     Fix call to smd_bridge_ioctl.
08/22/06   ptm     Merge WinMobile changes to main line.
08/15/06   bfc     Pulling dsm_items from a new pool for the DIAG port
07/05/06   bfc     Send BAUD ioctl comments to smd_bridge for bridge ports
06/22/06   bfc     Added the client capability to specify a receive DSM
                   WM queue
06/21/06   bfc     Fixed a race condition that could occur when the
                   client changes mode
06/16/06   ptm     Always set new_rx_queue in smd_sio_change_mode.
05/23/06   ptm     Remove smd_task.h and fix a few comments.
04/27/06   bfc     Fixed some race conditions
04/24/06   ptm     Added port for GPS NMEA and re-added oncrpc dms item pool.
03/27/06   bfc     Modified SMD to use the new sio extended funcion callback
                   parameters
03/03/06   ptm     Added oncrpc dsm item pool.
01/26/06   bfc     Added and defined smd_sio_close_ext and 
                   smd_sio_flush_tx_ext
01/24/06   bfc     Added and defined smd_sio_contol_(open|close|transmit) 
11/23/05   ptm     Moved smd_sio_bridge_*() functions to smd_bridge.c and
                   moved dsm management to smd_wm.c
09/28/05   TMR     Added all smd_sio_bridge_*() functions.
08/28/05   ptm     Convert collection of arrays to array of structures.
05/18/05   ptm     Add INTLOCK/INTFREE to process_tx_buffer to prevent race
                   conditions with dev_transmit. Simplify dev_transmit to fix
                   various bugs.
05/18/05   ptm     General clean up.
02/10/05   adm     Made changes to support flush, DM/Memcpy, STreaming/Packet
                   transfers                   
11/07/04   adm     The SIO interface related code was leveraged from MSP1000
                   shared memory driver code. Added support for multiple 
                   DS pipes, and made changes to interpretation of DSR/DTR 
                   and RTS signals
2004/02/06 cr      Initial version ported to 6500 surf.
===========================================================================*/

/*===========================================================================
                            INCLUDE FILES
===========================================================================*/
#include "smd_os.h"

#include "sio.h"                  /* Serial I/O Services */

#include "dsm.h"                  /* DSM items */

#include "smd.h"
#include "smd_v.h"
#include "smd_internal.h"
#include "smd_os_common.h"
#include "smd_dsm.h"
#include "smd_dsmi.h"
#include "smd_sio_api.h"

#include "smd_sio.h"
#include "smd_sio_internal.h" 
#include "smd_sio_target.h"

/* TODO: remove this once dsm pool can be properly specified through sio on Q6*/
#if defined(FEATURE_QDSP6)
#include "diagdsm.h"
#endif

/*===========================================================================
                     CONSTANT AND VARIABLE DECLARATIONS
===========================================================================*/

/* Control or Data port */
typedef enum
{
  SMD_SIO_DATA_PORT,
  SMD_SIO_CONTROL_PORT,
  SMD_SIO_UNASSIGNED_PORT
} smd_sio_port_type;

/*---------------------------------------------------------------------------
 Constants used in '+++' escape code detection.
---------------------------------------------------------------------------*/

/* Guard time that is needed before and after the Escape Characters in order
** for the escape characters to be detected as escape sequence.  This time
** is also an upper bound on the amount of time that can pass between the
** escape characters.  Typically guard-time is 1s, but expire in .9s so that
** if application's timing is off (and/or phone's timing is off) escape 
** detection still works.  Phone's timing should not be off by more than 5ms,
** unless there are some extreme performance problems.
*/
#define SIOSMEM_ESC_SEQ_GUARD_TIME         900

/* Escape sequence character
*/
#define ESC_SEQ_CHAR          '+'

typedef enum
{
  SIOSMEM_ESC_SEQ_NO_GUARDTIME,   /* no 1st guardtime                        */
  SIOSMEM_ESC_SEQ_1ST_GUARDTIME,  /* 1st Guardtime occurred                  */
  SIOSMEM_ESC_SEQ_1ST_PLUS,       /* Received 1st plus sign before guardtime */
  SIOSMEM_ESC_SEQ_2ND_PLUS,       /* Received 2nd plus sign before guardtime */
  SIOSMEM_ESC_SEQ_3RD_PLUS,       /* Received 3rd plus sign before guardtime */
  SIOSMEM_ESC_SEQ_DETECTED,       /* Escape Sequence detected                */
  SIOSMEM_ESC_SEQ_MAX             /* For Bounds checking only                */
} smd_sio_esc_seq_enum_type;
                   
typedef struct {
  /* the smd channel id of this port */
  smd_port_id_type smd_channel_id;

  /* control or data port */
  smd_sio_port_type port_type;

  /* the smd channel type of this port */
  smd_channel_type channel_type;

  /* Copy of open structure from sio */
  sio_open_type open;

  /* Function pointer which should be called when detecting an escape sequence*/
  sio_vv_func_ptr_type esc_callback_fn;

  /* Function pointer which should be called when detecting a PPP packet */
  sio_vv_func_ptr_type pkt_callback_fn;

  /* state variable for '+++' escape code detection  */
  smd_sio_esc_seq_enum_type esc_seq_state;

  /* Clock call back structure which is used in escape code detection and
     auto-detect. */
  SMD_SIO_TIMER_TYPE esc_seq_timer;

  /* TRUE if dsm item contains valid data */
  boolean item_has_valid_data;

  /* extended flush callback function pointer and data*/
  sio_vpu_func_ptr_type flush_callback_ext_fn;
  void                  *flush_callback_ext_data;

  /* flush callback function pointer */
  sio_vv_func_ptr_type  flush_callback_fn;

  /* Function to call when stream is closed */
  sio_vv_func_ptr_type close_cb;

  /* extended close callback function pointer and data */
  sio_vpu_func_ptr_type  close_callback_ext_fn;
  void                   *close_callback_ext_data;

  /* DTR changed callback function pointer */
  sio_vv_func_ptr_type   dtr_callback_fn;

  /* extended DTR changed callback function pointer and data */
  sio_vpu_func_ptr_type     dtr_callback_ext_fn;
  void                      *dtr_callback_ext_data;
} smd_sio_info_type;

/* There are about 0x33 SIO SMD ports reserved already. We can only host
  14 bridge connections. */ 
#define SMD_SIO_NUM_PORTS \
  (SIO_MINOR_SMD_MAX - SIO_MINOR_SMD_FIRST + 1 )

/* a 2d array for accessing the smd_sio_info_type structs */
static smd_sio_info_type* 
        smd_sio_data_port_to_info[SMD_SIO_NUM_PORTS][SMD_SIO_NUM_PROCS+1];

static smd_sio_info_type* 
        smd_sio_control_port_to_info[SMD_SIO_NUM_PORTS][SMD_SIO_NUM_PROCS+1];

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/*===========================================================================
                     LOCAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_sio_get_port_name

DESCRIPTION   Translates an SIO PORT ID to the equivalent SMD PORT name
              If there is no equivalent port ID, calls ERR_FATAL.

ARGUMENTS     pid - an SIO PORT ID

DEPENDENCIES  None
 
RETURN VALUE  char * the smd port name
 
SIDE EFFECTS  None
===========================================================================*/
char * smd_sio_get_port_name(sio_port_id_type pid)
{
  const char * name = NULL;

  switch(SIO_MINOR_NUM_OF_DEVICE(pid))
  {
  case SIO_MINOR_SMD_DS:
    name = "DS";
    break;
  case SIO_MINOR_SMD_DIAG:
  case SIO_MINOR_SMD_DIAG_APPS:
  case SIO_MINOR_SMD_DIAG_MODEM:
  case SIO_MINOR_SMD_DIAG_QDSP:
    name = "DIAG";
    break;
  case SIO_MINOR_SMD_DIAG2:
    name = "DIAG_2";
    break;
  case SIO_MINOR_SMD_RPC_CALL:
    name = "RPCCALL";
    break;
  case SIO_MINOR_SMD_RPC_REPLY:
    name = "RPCRPY";
    break;
  case SIO_MINOR_SMD_BT:
    name = "BT";
    break;
  case SIO_MINOR_SMD_CONTROL:
    name = "CONTROL";
    break;
  case SIO_MINOR_SMD_MEMCPY_SPARE1:
    name = "MCPY_RVD";
    break;
  case SIO_MINOR_SMD_DATA1:
    name = "DATA1";
    break;
  case SIO_MINOR_SMD_DATA2:
    name = "DATA2";
    break;
  case SIO_MINOR_SMD_DATA3:
    name = "DATA3";
    break;
  case SIO_MINOR_SMD_DATA4:
    name = "DATA4";
    break;
  case SIO_MINOR_SMD_DATA5:
    name = "DATA5";
    break;
  case SIO_MINOR_SMD_DATA6:
    name = "DATA6";
    break;
  case SIO_MINOR_SMD_DATA7:
    name = "DATA7";
    break;
  case SIO_MINOR_SMD_DATA8:
    name = "DATA8";
    break;
  case SIO_MINOR_SMD_DATA9:
    name = "DATA9";
    break;
  case SIO_MINOR_SMD_DATA10:
    name = "DATA10";
    break;
  case SIO_MINOR_SMD_DATA11:
    name = "DATA11";
    break;
  case SIO_MINOR_SMD_DATA12:
    name = "DATA12";
    break;
  case SIO_MINOR_SMD_DATA13:
    name = "DATA13";
    break;
  case SIO_MINOR_SMD_DATA14:
    name = "DATA14";
    break;
  case SIO_MINOR_SMD_DATA15:
    name = "DATA15";
    break;
  case SIO_MINOR_SMD_DATA16:
    name = "DATA16";
    break;
  case SIO_MINOR_SMD_DATA17:
    name = "DATA17";
    break;
  case SIO_MINOR_SMD_DATA18:
    name = "DATA18";
    break;
  case SIO_MINOR_SMD_DATA19:
    name = "DATA19";
    break;
  case SIO_MINOR_SMD_DATA20:
    name = "DATA20";
    break;
  case SIO_MINOR_SMD_DATA21:
    name = "DATA21";
    break;
  case SIO_MINOR_SMD_DATA22:
    name = "DATA22";
    break;
  case SIO_MINOR_SMD_DATA23:
    name = "DATA23";
    break;
  case SIO_MINOR_SMD_DATA24:
    name = "DATA24";
    break;
  case SIO_MINOR_SMD_DATA25:
    name = "DATA25";
    break;
  case SIO_MINOR_SMD_DATA26:
    name = "DATA26";
    break;
  case SIO_MINOR_SMD_DATA27:
    name = "DATA27";
    break;
  case SIO_MINOR_SMD_DATA28:
    name = "DATA28";
    break;
  case SIO_MINOR_SMD_DATA29:
    name = "DATA29";
    break;
  case SIO_MINOR_SMD_DATA30:
    name = "DATA30";
    break;
  case SIO_MINOR_SMD_DATA31:
    name = "DATA31";
    break;
  case SIO_MINOR_SMD_DATA32:
    name = "DATA32";
    break;
  case SIO_MINOR_SMD_DATA33:
    name = "DATA33";
    break;
  case SIO_MINOR_SMD_DATA34:
    name = "DATA34";
    break;
  case SIO_MINOR_SMD_DATA35:
    name = "DATA35";
    break;
  case SIO_MINOR_SMD_DATA36:
    name = "DATA36";
    break;
  case SIO_MINOR_SMD_DATA37:
    name = "DATA37";
    break;
  case SIO_MINOR_SMD_DATA38:
    name = "DATA38";
    break;
  case SIO_MINOR_SMD_DATA39:
    name = "DATA39";
    break;
  case SIO_MINOR_SMD_DATA40:
    name = "DATA40";
    break;
  case SIO_MINOR_SMD_GPS_NMEA:
    name = "GPSNMEA";
    break;
  default:
    ERR_FATAL("smd_sio_get_port_name: unrecognized sio port id: %d",pid,0,0);
  }

  return (char *)name;
}/* smd_sio_get_port_name */

/*===========================================================================
FUNCTION      smd_sio_get_cntl_port_name

DESCRIPTION   Translates a Control port SIO PORT ID to the equivalent 
              SMD PORT name.  If there is no equivalent port ID, calls
              ERR_FATAL.

ARGUMENTS     pid - an SIO PORT ID

DEPENDENCIES  None
 
RETURN VALUE  char * the smd port name
 
SIDE EFFECTS  None
===========================================================================*/
char *smd_sio_get_cntl_port_name(sio_port_id_type pid)
{
  const char * name = NULL;

  switch(SIO_MINOR_NUM_OF_DEVICE(pid))
  {
  case SIO_MINOR_SMD_DS:
    name = "DS_CNTL";
    break;
  case SIO_MINOR_SMD_DIAG:
  case SIO_MINOR_SMD_DIAG_MODEM:
  case SIO_MINOR_SMD_DIAG_APPS:
  case SIO_MINOR_SMD_DIAG_QDSP:
    name = "DIAG_CNTL";
    break;
  case SIO_MINOR_SMD_DIAG2:
    name = "DIAG_CNTL_2";
    break;
  case SIO_MINOR_SMD_RPC_CALL:
    name = "RPCCALL_CNTL";
    break;
  case SIO_MINOR_SMD_RPC_REPLY:
    name = "RPCRPY_CNTL";
    break;
  case SIO_MINOR_SMD_BT:
    name = "BT_CNTL";
    break;
  case SIO_MINOR_SMD_CONTROL:
    name = "CONTROL_CNTL";
    break;
  case SIO_MINOR_SMD_MEMCPY_SPARE1:
    name = "MCPY_RVD_CNTL";
    break;
  case SIO_MINOR_SMD_DATA1:
    name = "DATA1_CNTL";
    break;
  case SIO_MINOR_SMD_DATA2:
    name = "DATA2_CNTL";
    break;
  case SIO_MINOR_SMD_DATA3:
    name = "DATA3_CNTL";
    break;
  case SIO_MINOR_SMD_DATA4:
    name = "DATA4_CNTL";
    break;
  case SIO_MINOR_SMD_DATA5:
    name = "DATA5_CNTL";
    break;
  case SIO_MINOR_SMD_DATA6:
    name = "DATA6_CNTL";
    break;
  case SIO_MINOR_SMD_DATA7:
    name = "DATA7_CNTL";
    break;
  case SIO_MINOR_SMD_DATA8:
    name = "DATA8_CNTL";
    break;
  case SIO_MINOR_SMD_DATA9:
    name = "DATA9_CNTL";
    break;
  case SIO_MINOR_SMD_DATA10:
    name = "DATA10_CNTL";
    break;
  case SIO_MINOR_SMD_DATA11:
    name = "DATA11_CNTL";
    break;
  case SIO_MINOR_SMD_DATA12:
    name = "DATA12_CNTL";
    break;
  case SIO_MINOR_SMD_DATA13:
    name = "DATA13_CNTL";
    break;
  case SIO_MINOR_SMD_DATA14:
    name = "DATA14_CNTL";
    break;
  case SIO_MINOR_SMD_DATA15:
    name = "DATA15_CNTL";
    break;
  case SIO_MINOR_SMD_DATA16:
    name = "DATA16_CNTL";
    break;
  case SIO_MINOR_SMD_DATA17:
    name = "DATA17_CNTL";
    break;
  case SIO_MINOR_SMD_DATA18:
    name = "DATA18_CNTL";
    break;
  case SIO_MINOR_SMD_DATA19:
    name = "DATA19_CNTL";
    break;
  case SIO_MINOR_SMD_DATA20:
    name = "DATA20_CNTL";
    break;
  case SIO_MINOR_SMD_DATA21:
    name = "DATA21_CNTL";
    break;
  case SIO_MINOR_SMD_DATA22:
    name = "DATA22_CNTL";
    break;
  case SIO_MINOR_SMD_DATA23:
    name = "DATA23_CNTL";
    break;
  case SIO_MINOR_SMD_DATA24:
    name = "DATA24_CNTL";
    break;
  case SIO_MINOR_SMD_DATA25:
    name = "DATA25_CNTL";
    break;
  case SIO_MINOR_SMD_DATA26:
    name = "DATA26_CNTL";
    break;
  case SIO_MINOR_SMD_DATA27:
    name = "DATA27_CNTL";
    break;
  case SIO_MINOR_SMD_DATA28:
    name = "DATA28_CNTL";
    break;
  case SIO_MINOR_SMD_DATA29:
    name = "DATA29_CNTL";
    break;
  case SIO_MINOR_SMD_DATA30:
    name = "DATA30_CNTL";
    break;
  case SIO_MINOR_SMD_DATA31:
    name = "DATA31_CNTL";
    break;
  case SIO_MINOR_SMD_DATA32:
    name = "DATA32_CNTL";
    break;
  case SIO_MINOR_SMD_DATA33:
    name = "DATA33_CNTL";
    break;
  case SIO_MINOR_SMD_DATA34:
    name = "DATA34_CNTL";
    break;
  case SIO_MINOR_SMD_DATA35:
    name = "DATA35_CNTL";
    break;
  case SIO_MINOR_SMD_DATA36:
    name = "DATA36_CNTL";
    break;
  case SIO_MINOR_SMD_DATA37:
    name = "DATA37_CNTL";
    break;
  case SIO_MINOR_SMD_DATA38:
    name = "DATA38_CNTL";
    break;
  case SIO_MINOR_SMD_DATA39:
    name = "DATA39_CNTL";
    break;
  case SIO_MINOR_SMD_DATA40:
    name = "DATA40_CNTL";
    break;
  case SIO_MINOR_SMD_GPS_NMEA:
    name = "GPSNMEA_CNTL";
    break;
  default:
    ERR_FATAL("smd_sio_get_cntl_port_name: unrecognized sio port id: %d",
               pid,0,0);
  }

  return (char *)name;
}/* smd_sio_get_cntl_port_name */

/*===========================================================================
FUNCTION      smd_sio_ioctl_cmd_translate

DESCRIPTION   Translates an SIO IOCTL command to the equivalent SMD IOCTL
              command. If there is no equivalent command it returns
              SMD_IOCTL_INVALID.

ARGUMENTS     cmd - an SIO IOCTL command

DEPENDENCIES  None
 
RETURN VALUE  An SMD IOCTL command
 
SIDE EFFECTS  None
===========================================================================*/
static smd_ioctl_cmd_type smd_sio_ioctl_cmd_translate
(
 sio_ioctl_cmd_type cmd
){
  smd_ioctl_cmd_type smd_ioctl;

  switch(cmd)
  {
  case SIO_IOCTL_CD_ASSERT:
    smd_ioctl = SMD_IOCTL_CD_ASSERT;
    break;
  case SIO_IOCTL_CD_DEASSERT:
    smd_ioctl = SMD_IOCTL_CD_DEASSERT;
    break;
  case SIO_IOCTL_GET_CURRENT_CD:
    smd_ioctl = SMD_IOCTL_GET_CURRENT_CD;
    break;
  case SIO_IOCTL_RI_ASSERT:
    smd_ioctl = SMD_IOCTL_RI_ASSERT;
    break;
  case SIO_IOCTL_RI_DEASSERT:
    smd_ioctl = SMD_IOCTL_RI_DEASSERT;
    break;
  case SIO_IOCTL_GET_CURRENT_RI:
    smd_ioctl = SMD_IOCTL_GET_CURRENT_RI;
    break;
  case SIO_IOCTL_ENABLE_DTR_EVENT:
    smd_ioctl = SMD_IOCTL_ENABLE_DTR_EVENT;
    break;
  case SIO_IOCTL_DISABLE_DTR_EVENT:
    smd_ioctl = SMD_IOCTL_DISABLE_DTR_EVENT;
    break;
  case SIO_IOCTL_INBOUND_FLOW_ENABLE:
    smd_ioctl = SMD_IOCTL_INBOUND_FLOW_ENABLE;
    break;
  case SIO_IOCTL_INBOUND_FLOW_DISABLE:
    smd_ioctl = SMD_IOCTL_INBOUND_FLOW_DISABLE;
    break;
  case SIO_IOCTL_DSR_ASSERT:
    smd_ioctl = SMD_IOCTL_DSR_ASSERT;
    break;
  case SIO_IOCTL_DSR_DEASSERT:
    smd_ioctl = SMD_IOCTL_DSR_DEASSERT;
    break;
  case SIO_IOCTL_DTE_READY_ASSERTED:
    smd_ioctl = SMD_IOCTL_DTE_READY_ASSERTED;
    break;
  case SIO_IOCTL_GET_CURRENT_RTS:
    smd_ioctl = SMD_IOCTL_GET_CURRENT_RTS;
    break;
  case SIO_IOCTL_SET_FLOW_CTL:
    smd_ioctl = SMD_IOCTL_SET_FLOW_CTL;
    break;
  case SIO_IOCTL_GET_FLOW_CTL:
    smd_ioctl = SMD_IOCTL_GET_FLOW_CTL;
    break;
  case SIO_IOCTL_ENABLE_AUTODETECT:
    smd_ioctl = SMD_IOCTL_ENABLE_AUTODETECT;
    break;
  case SIO_IOCTL_RECORD_ESC_FUNC_PTR:
    smd_ioctl = SMD_IOCTL_RECORD_ESC_FUNC_PTR;
    break;
  case SIO_IOCTL_RECORD_PKT_FUNC_PTR:
    smd_ioctl = SMD_IOCTL_RECORD_PKT_FUNC_PTR;
    break;
  case SIO_IOCTL_BEGIN_FAX_IMAGE:
    smd_ioctl = SMD_IOCTL_BEGIN_FAX_IMAGE;
    break;
  case SIO_IOCTL_CHANGE_BAUD_RATE:
    smd_ioctl = SMD_IOCTL_CHANGE_BAUD_RATE;
    break;
  case SIO_IOCTL_CHANGE_BAUD_NOW:
    smd_ioctl = SMD_IOCTL_CHANGE_BAUD_NOW;
    break;
  case SIO_IOCTL_FLUSH_TX:
    smd_ioctl = SMD_IOCTL_FLUSH_TX;
    break;
  case SIO_IOCTL_CHANGE_MODE:
    smd_ioctl = SMD_IOCTL_CHANGE_MODE;
    break;
  case SIO_IOCTL_REG_TLP_CHANGE_CB:
    smd_ioctl = SMD_IOCTL_REG_TLP_CHANGE_CB;
    break;
  case SIO_IOCTL_GET_PACKET_SIZE:
    smd_ioctl = SMD_IOCTL_GET_PACKET_SIZE;
    break;
  case SIO_IOCTL_SET_PACKET_SIZE:
    smd_ioctl = SMD_IOCTL_SET_PACKET_SIZE;
    break;
  case SIO_IOCTL_GET_BUFFER_SIZE:
    smd_ioctl = SMD_IOCTL_GET_BUFFER_SIZE;
    break;
  case SIO_IOCTL_SET_BUFFER_SIZE:
    smd_ioctl = SMD_IOCTL_SET_BUFFER_SIZE;
    break;
  case SIO_IOCTL_GET_XFR_MODE:
    smd_ioctl = SMD_IOCTL_GET_XFR_MODE;
    break;
  case SIO_IOCTL_SET_XFR_MODE:
    smd_ioctl = SMD_IOCTL_SET_XFR_MODE;
    break;
  case SIO_IOCTL_GET_XFR_TYPE:
    smd_ioctl = SMD_IOCTL_GET_XFR_TYPE;
    break;
  case SIO_IOCTL_SET_XFR_TYPE:
    smd_ioctl = SMD_IOCTL_SET_XFR_TYPE;
    break;
  case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
    smd_ioctl = SMD_IOCTL_ENABLE_DTR_EVENT_EXT;
    break;
  case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
    smd_ioctl = SMD_IOCTL_DISABLE_DTR_EVENT_EXT;
    break;
  case SIO_IOCTL_ENABLE_CD_EVENT_EXT:
    smd_ioctl = SMD_IOCTL_ENABLE_CD_EVENT_EXT;
    break;
  case SIO_IOCTL_DISABLE_CD_EVENT_EXT:
    smd_ioctl = SMD_IOCTL_DISABLE_CD_EVENT_EXT;
    break;
  case SIO_IOCTL_ENABLE_RI_EVENT_EXT:
    smd_ioctl = SMD_IOCTL_ENABLE_RI_EVENT_EXT;
    break;
  case SIO_IOCTL_DISABLE_RI_EVENT_EXT:
    smd_ioctl = SMD_IOCTL_DISABLE_RI_EVENT_EXT;
    break;
  case SIO_IOCTL_SET_DROP_PEND:
    smd_ioctl = SMD_IOCTL_SET_DROP_PEND;
    break;
  case SIO_IOCTL_GET_DROP_PEND:
    smd_ioctl = SMD_IOCTL_GET_DROP_PEND;
    break;
  case SIO_IOCTL_BT_CONFIGURE:
    smd_ioctl = SMD_IOCTL_INVALID;
    break;
  case SIO_IOCTL_BT_DISCONNECT:
    smd_ioctl = SMD_IOCTL_INVALID;
    break;
  case SIO_IOCTL_BT_GET_STATUS:
    smd_ioctl = SMD_IOCTL_INVALID;
    break;
  case SIO_IOCTL_POWERDOWN:
    smd_ioctl = SMD_IOCTL_INVALID;
    break;
  case SIO_IOCTL_WAKEUP:
    smd_ioctl = SMD_IOCTL_INVALID;
    break;
  case SIO_IOCTL_WM_SET_WAIT:
    smd_ioctl = SMD_IOCTL_WM_SET_WAIT;
    break;
  case SIO_IOCTL_WM_GET_WAIT:
    smd_ioctl = SMD_IOCTL_WM_GET_WAIT;
    break;
  default:
    smd_ioctl = SMD_IOCTL_INVALID;
  }

  return smd_ioctl;
} /* smd_sio_ioctl_cmd_translate */

/*===========================================================================
FUNCTION      smd_sio_validate_port_id

DESCRIPTION   validates that the sio port id given has a valid major and 
              minor numbers.  The indexes into smd_sio_port_to_info can
              also be returned.

ARGUMENTS     sio_port_id_type port_id - SIO port to be used by this stream.
              uint32* channel_type_idx_out - returns the index into 
                smd_sio_channel_types for this port id.  This is also the
                first index into smd_sio_port_to_info.
              uint32* channel_idx_out - returns the corresponding second index 
                into smd_sio_port_to_info.

RETURN VALUE  boolean - if port_id is valid

SIDE EFFECTS  None
===========================================================================*/
static boolean smd_sio_validate_port_id
(
  sio_port_id_type sio_port_id,
  smd_sio_port_type port_type,
  uint32* channel_type_idx_out,
  uint32* channel_idx_out
){
    boolean valid = FALSE;
    uint32 channel_type_idx;
    uint32 channel_idx;
    uint32 minor_id;

    /* search through the smd_sio_channel_types array (defined in 
       smd_sio_target.h) for an entry with the correct major id */
    channel_type_idx = 0;
    while((channel_type_idx <= SMD_SIO_NUM_PROCS ) &&
            (smd_sio_channel_types[channel_type_idx].major_id !=
                                  SIO_MAJOR_NUM_OF_DEVICE(sio_port_id)))
    {
      channel_type_idx++;
    }

    /* find out the smd channel id */
    minor_id = SIO_MINOR_NUM_OF_DEVICE(sio_port_id);
    channel_idx = minor_id - SIO_MINOR_SMD_FIRST;
    
    if((channel_type_idx <= SMD_SIO_NUM_PROCS ) &&
      (channel_idx < SMD_SIO_NUM_PORTS))
    {  
        valid = TRUE;
    }
   
    if(channel_type_idx_out != NULL)
    {
      *channel_type_idx_out = channel_type_idx;
    }

    if(channel_idx_out != NULL)
    {
      *channel_idx_out = channel_idx;
    }

    return valid;
}/* smd_sio_validate_port_id */

/*===========================================================================
FUNCTION      smd_sio_get_info_location

DESCRIPTION   Returns the location in smd_sio_port_to_info for the desired
              port id.

ARGUMENTS     port_id - SIO port to be used by this stream.
              channel_type - can be used to return the smd channel type

RETURN VALUE  smd_sio_info_type** - the location in smd_sio_port_to_info
              corresponding to the given sio port id. 

SIDE EFFECTS  None
===========================================================================*/
static smd_sio_info_type **smd_sio_get_info_location
(
  sio_port_id_type sio_port_id,
  smd_sio_port_type port_type,
  smd_channel_type *channel_type
){
    uint32 channel_type_idx;
    uint32 channel_idx;
    smd_sio_info_type **rtn_value = NULL;

    if(smd_sio_validate_port_id(sio_port_id,
                                port_type,
                                &channel_type_idx,
                                &channel_idx)
      )
    {
      if( SMD_SIO_DATA_PORT == port_type) 
      {
        rtn_value = &smd_sio_data_port_to_info[channel_idx][channel_type_idx];
      }
      else
      {
        rtn_value =
                 &smd_sio_control_port_to_info[channel_idx][channel_type_idx];
      }
    }
    else
    {
      ERR_FATAL("smd_sio_get_info_location: illegal sio port id (%d) given"
                "\nchannel type idx %d, channel idx %d", sio_port_id, 
                channel_type_idx, channel_idx);
    }

    if(channel_type != NULL)
    {
      *channel_type = smd_sio_channel_types[channel_type_idx].channel_type;
    }

    return rtn_value;
}/* smd_sio_get_info_location */

/*===========================================================================
FUNCTION      smd_sio_get_info

DESCRIPTION   Returns the smd_sio_info_type struct from smd_sio_port_to_info 
              for the desired sio port id.

ARGUMENTS     port_id - SIO port to be used by this stream.

RETURN VALUE  smd_sio_info_type* - the smd_sio_type in smd_sio_port_to_info
              corresponding to the given sio port id. 

SIDE EFFECTS  None
===========================================================================*/
static smd_sio_info_type *smd_sio_get_info
(
  sio_stream_id_type stream_id,
  sio_port_id_type sio_port_id,
  smd_sio_port_type port_type
){
  smd_sio_info_type **location;
  smd_sio_info_type *info = NULL;

  location = smd_sio_get_info_location(sio_port_id, port_type, NULL);

  if(location != NULL)
  {
    info = *location;
  }
  
  return info;
}/* smd_sio_get_info */

/*===========================================================================
FUNCTION      smd_sio_alloc_info

DESCRIPTION   Allocates an smd_sio_info_type structure, initializes all of its
              fields to zero and then initializes the link field.

ARGUMENTS     port_id - SIO port to be used by this stream.

RETURN VALUE  Pointer to allocated info structure.

SIDE EFFECTS  None
===========================================================================*/
static smd_sio_info_type * smd_sio_alloc_info
(
  sio_port_id_type   port_id, 
  smd_sio_port_type  port_type
)
{
  smd_sio_info_type *info;
  smd_sio_info_type **info_location;
  smd_channel_type channel_type;

  /* find the location in smd_sio_port_to_info and validate it */
  info_location = smd_sio_get_info_location(port_id, port_type, &channel_type);

  if(info_location == NULL)
  {
    ERR_FATAL("smd_sio_alloc_info: Illegal sio port_id given (0x%x)",
              port_id, 0, 0);
  }
  
  info = *info_location;
  if(info == NULL)
  {
    /* grab a free info struct from the queue */
    info = (smd_sio_info_type *) smd_malloc(sizeof(smd_sio_info_type));

    if( info != NULL )
    {
      memset( info, 0, sizeof(smd_sio_info_type) );
      *info_location = info;
      info->channel_type = channel_type;
      info->port_type = port_type;
    }
  }

  return info;
} /* smd_sio_alloc_info */

/*===========================================================================
FUNCTION      smd_sio_free_info

DESCRIPTION   Deallocate an smd_sio_info_type structure.

ARGUMENTS     pointer to the allocated info structure.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_free_info(smd_sio_info_type *info)
{
  smd_sio_info_type **location;
  int32 ret;

  location = 
         smd_sio_get_info_location(info->open.port_id, info->port_type, NULL);

  if(*location != info)
  {
    ERR_FATAL("smd_sio_free_info: info mismatch (sio port id %d)",
               info->open.port_id, 0, 0);
  }

  *location = NULL;
  ret = smd_free( info );
  ASSERT(ret == 0);
} /* smd_sio_free_info */

/*===========================================================================
FUNCTION      smd_sio_get_port_defaults

DESCRIPTION   Returns the defaults for a given port_id.

ARGUMENTS     port_id - SIO port to use as a search key 
 
              param - SIO param pointer 

              dataxfr_mode - the data transfer mode: memcpy or data
                             mover (output)

              dsm_pool_id - the DSM pool to use for allocating read
                            buffers (output)
 
              smd_param - the SMD parameters, including FIFO size, port
                          priority and xfrflow type [packet or stream] (output)
 
RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_get_port_defaults
(
  sio_port_id_type          port_id,
  sio_open_param_type      *param,
  smd_dataxfr_mode_type    *dataxfr_mode,
  dsm_mempool_id_enum_type *dsm_pool_id,
  smd_open_param_type      *smd_param
)
{
  /* Set default values */
  *dataxfr_mode = SMD_MEMCPY_MODE;
  smd_param->xfrflow = SMD_PKTXFR_TYPE;

  /* Use small pool by default - flexible allocation API 
     will use large pool if needed.
   */
  *dsm_pool_id  = DSM_DS_SMALL_ITEM_POOL;
  /* Since we know that we are going to set the xfrflow_type param in this
   * struct, set the corresponding fields flag
   */
  smd_param->fields = SMD_OPEN_PARAM_XFRFLOW;

  /* get the FIFO size if the client specified one */
  if(param && param->dsm_pool_param.smd_fifo_size)
  {
    smd_param->fields |= SMD_OPEN_PARAM_FIFO_SIZE;
    smd_param->fifo_size = param->dsm_pool_param.smd_fifo_size;
  }

  /* Set values based on port id */
  if (( port_id == SIO_PORT_SMD_DIAG )       || 
      ( port_id == SIO_PORT_SMD_DIAG2 )      ||
      ( port_id == SIO_PORT_SMD_DIAG_MODEM ) ||
      ( port_id == SIO_PORT_SMD_DIAG_APPS ))  
	  //||( port_id == SIO_PORT_SMD_DIAG_QDSP ))
  {
    if( param )
    {
      *dsm_pool_id  = param->dsm_pool_param.dsm_mempool_id;
    }

    smd_param->fields |= SMD_OPEN_PARAM_PRIORITY;
    smd_param->priority = SMD_PRIORITY_LOW;
  }
  else if( port_id == SIO_PORT_SMD_DS ||
           port_id == SIO_PORT_SMD_DATA11 )
  {
    smd_param->xfrflow = SMD_STREAMING_TYPE;
  }  
} /* smd_sio_get_port_defaults */


/*===========================================================================
FUNCTION      smd_sio_esc_seq_guardtime_isr

DESCRIPTION   This function processes a timer used for escape sequence
              detection.  This function is a callback function for the timer.
              This function changes the escape sequence state based upon the
              current state.

ARGUMENT      time_ms - 

              port_id - SIO port associated with timer

DEPENDENCIES  This function works in conjunction with the receive (OUT)
              interrupt. It should only be called while in RAWDATA mode. It
              must be invoked by Clock Services. 

RETURN VALUE  None.

SIDE EFFECTS  The escape sequence state is shared between this timer callback
              function and the receive (OUT) interrupt.
===========================================================================*/
static void smd_sio_esc_seq_guardtime_isr 
(
  timer_cb_data_type  port_id
)
{
  smd_sio_info_type *info;

  info = smd_sio_get_info( 0, (sio_port_id_type) port_id, SMD_SIO_DATA_PORT );
  ASSERT(info != NULL);

  /* Perform escape sequence processing */
  switch (info->esc_seq_state)
  {
    case SIOSMEM_ESC_SEQ_NO_GUARDTIME:
    {
      /* The first guardtime timer occurred after
         receiving any characters. */
      info->esc_seq_state = SIOSMEM_ESC_SEQ_1ST_GUARDTIME;
      break;
    } /* end case - SIOSMEM_ESC_SEQ_NO_GUARDTIME */

    case SIOSMEM_ESC_SEQ_1ST_GUARDTIME:
    {
      /* The first guardtime timer occurred (again) while waiting 
         for the first escape sequence character. */
      info->esc_seq_state = SIOSMEM_ESC_SEQ_1ST_GUARDTIME;
      break;
    } /* end case - SIOSMEM_ESC_SEQ_1ST_GUARDTIME */

    case SIOSMEM_ESC_SEQ_1ST_PLUS:
    {
      /* The first guardtime timer occurred while waiting 
         for the second escape sequence character. */
      info->esc_seq_state = SIOSMEM_ESC_SEQ_1ST_GUARDTIME;
      break;
    } /* end case - SIOSMEM_ESC_SEQ_1ST_PLUS */

    case SIOSMEM_ESC_SEQ_2ND_PLUS:
    {
      /* The first guardtime timer occurred while waiting 
         for the third escape sequence character. */
      info->esc_seq_state = SIOSMEM_ESC_SEQ_1ST_GUARDTIME;
      break;
    } /* end case - SIOSMEM_ESC_SEQ_2ND_PLUS */

    case SIOSMEM_ESC_SEQ_3RD_PLUS:
    {
      /* The final guardtime occurred after receiving the third
         escape sequence character.  This is a valid escape sequence.
         Call the escape sequence callback function. */
      if (info->esc_callback_fn)
      {
        info->esc_callback_fn ();
      }
      /* Wait for the first guardtime of another escape sequence */
      info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;

      /* Set the timer to wait for another guardtime. */
      SMD_SIO_TIMER_SET( &(info->esc_seq_timer),
                         SIOSMEM_ESC_SEQ_GUARD_TIME);
      break;
    } /* end case - SIOSMEM_ESC_SEQ_3RD_PLUS */

    case SIOSMEM_ESC_SEQ_DETECTED:
    {
      /* Reset the guardtime timer. */
      info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
      break;
    } /* end case - SIOSMEM_ESC_SEQ_DETECTED */
    default:
    {
      /* illegal state */
      ERR_FATAL("smd_sio_esc_seq_guardtime_isr: illegal state (%d)",
                    info->esc_seq_state,0,0);
    }
  } /* end switch - info->esc_seq_state */
} /* end smd_sio_esc_seq_guardtime_isr */

/*===========================================================================
FUNCTION      smd_sio_handle_rawdata

DESCRIPTION   Run state machine looking for "+++" with the appropriate timing

ARGUMENTS     info - pointer to stream info structure

              item - pointer to DSM item associated with info structure

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void smd_sio_handle_rawdata
(
  smd_sio_info_type *info,
  dsm_item_type     *item
)
{
  byte *rx_char;
  int index;

  /* Perform escape sequence processing */
  switch (info->esc_seq_state)
  {
    case SIOSMEM_ESC_SEQ_NO_GUARDTIME:
      {
        /* Don't need to look at the characters. */
        break;
      } /* end case - SIOSMEM_ESC_SEQ_NO_GUARDTIME */

    case SIOSMEM_ESC_SEQ_1ST_GUARDTIME:
      {
        /* Check if there are not too many characters for an escape
           sequence */
        if (item->used <= 3)
        {
          /* Check each byte for an escape sequence */
          for (index = 0; index < item->used; index++)
          {
            /* Point to the byte being processed */
            rx_char = (item->data_ptr + index);
            if (*rx_char == ESC_SEQ_CHAR)
            {
              info->esc_seq_state = 
                (smd_sio_esc_seq_enum_type)(info->esc_seq_state + 1);
            }
            else /* Not an escape sequence */
            {
              /* Reset the guardtime timer. */
              info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
            } /* end if - escape sequence char check */
          } /* end for - check each byte for an escape sequence */
        }
        else
        {
          /* Reset the guardtime timer. */
          info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
        }
        break;
      } /* end case - SIOSMEM_ESC_SEQ_1ST_GUARDTIME */

    case SIOSMEM_ESC_SEQ_1ST_PLUS:
      {
        /* Check if there are not too many characters for an escape
           sequence */
        if (item->used <= 2)
        {
          /* Check each byte for an escape sequence */
          for (index = 0; index < item->used; index++)
          {
            /* Point to the byte being processed */
            rx_char = (item->data_ptr + index);
            if (*rx_char == ESC_SEQ_CHAR)
            {
              info->esc_seq_state = 
                (smd_sio_esc_seq_enum_type)(info->esc_seq_state + 1);
            }
            else /* Not an escape sequence */
            {
              /* Reset the guardtime timer. */
              info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
            } /* end if - escape sequence char check */
          } /* end for - check each byte for an escape sequence */
        }
        else
        {
          /* Reset the guardtime timer. */
          info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
        }
        break;
      } /* end case - SIOSMEM_ESC_SEQ_1ST_PLUS */

    case SIOSMEM_ESC_SEQ_2ND_PLUS:
      {
        /* Check if there are not too many characters for an escape
           sequence */
        if (item->used == 1)
        {
          /* Point to the byte being processed */
          if (*item->data_ptr == ESC_SEQ_CHAR)
          {
            info->esc_seq_state = SIOSMEM_ESC_SEQ_3RD_PLUS;
          }
          else /* Not an escape sequence */
          {
            /* Reset the guardtime timer. */
            info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
          } /* end if - escape sequence char check */
        }
        else
        {
          /* Reset the guardtime timer. */
          info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
        }
        break;
      } /* end case - SIOSMEM_ESC_SEQ_2ND_PLUS */

    case SIOSMEM_ESC_SEQ_3RD_PLUS:
      {
        /* Reset the guardtime timer. */
        info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
        break;
      } /* end case - SIOSMEM_ESC_SEQ_3RD_PLUS */

    case SIOSMEM_ESC_SEQ_DETECTED:
      {
        /* Reset the guardtime timer. */
        info->esc_seq_state = SIOSMEM_ESC_SEQ_NO_GUARDTIME;
        break;
      } /* end case - SIOSMEM_ESC_SEQ_DETECTED */
    default:
      /* illegal state */
      ERR_FATAL("smd_sio_handle_rawdata: illegal state (%d)",
                    info->esc_seq_state,0,0);
  } /* end switch - info->esc_seq_state */

  /* All the data is valid */
  info->item_has_valid_data = TRUE;
  item->kind = DSM_PS_RAW_APP_PAYLOAD;

  /* Set the timer to wait for a guardtime. */
  SMD_SIO_TIMER_SET( &(info->esc_seq_timer),
                     SIOSMEM_ESC_SEQ_GUARD_TIME);
} /* smd_sio_handle_rawdata */

/*===========================================================================
FUNCTION      smd_sio_process_rx_buffer

DESCRIPTION   This function processes a filled receive buffer. It either
              calls the receive buffer callback function supplied by the
              client or enqueues the buffer on the client supplied watermark
              queue.

DEPENDENCIES  The client must supply a callback function or a watermark
              queue so that this function may get the receive data in the
              buffer back to the client. If there is no receive callback
              function nor watermark queue, the data is discarded.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
static void smd_sio_process_rx_buffer
(
  smd_port_id_type   port_id,
  dsm_item_type     *item,
  uint32             cb_data
)
{
  smd_sio_info_type *info = (smd_sio_info_type *) cb_data;
  dsm_watermark_type *rx_queue;
  sio_rx_func_ptr_type rx_cb_fn;

  /* Process the characters based on the stream mode. */
  switch (info->open.stream_mode)
  {
    /* RawData Mode (Backwards compatible) */
    case SIO_DS_RAWDATA_MODE:
      smd_sio_handle_rawdata( info, item );
      break;

    /* Packet mode */
    case SIO_DS_PKT_MODE:
    case SIO_DM_MODE:
    case SIO_GENERIC_MODE:
    case SIO_DS_AUTODETECT_MODE:
      /* All the data is valid */
      info->item_has_valid_data = TRUE;
      break;

    default:
      ERR_FATAL( "Unknown stream mode %d", info->open.stream_mode, 0, 0 );
      break;
  } /* end switch - stream mode */

  /* Process the DSM item for the application only if the DSM item
     has valid data. */
  if (info->item_has_valid_data)
  {
    /* Use the callback function if it was defined. */
    rx_cb_fn = info->open.rx_func_ptr;

    if (rx_cb_fn)
    {
      rx_cb_fn( &item );
    }
    else
    {
      /* This operation needs to be in a critical section because
         the client can free info->open.rx_queue in a different task
         context */
      SMD_DSM_LOCK();
      rx_queue = info->open.rx_queue;
      if( rx_queue != NULL ) 
      {
        /* Use the queue if it was defined. */
        dsm_enqueue( rx_queue, &item );
      }
      SMD_DSM_UNLOCK();

      if( rx_queue == NULL )
      {
        /* Nowhere to send the buffer.  Place it back on the DSM pool.*/
        dsm_free_packet( &item );
      }
    }
  }
  else /* the item does not have valid data */
  {
    dsm_free_packet( &item );
  } /* end if - the item has valid data */
} /* smd_sio_process_rx_buffer */

/*===========================================================================
FUNCTION      smd_sio_close_cb

DESCRIPTION   This callback is called when the stream is closed. It is
              registered when the client calls smd_sio_close. It frees the
              info structure associated with the port and then calls the
              client's callback.

ARGUMENTS     port_id - SIO port being closed

              cb_data - a pointer to the info structure associated with the
                        port_id
  
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_close_cb( void *cb_data )
{
  smd_sio_info_type *info = (smd_sio_info_type *) cb_data;
  sio_vv_func_ptr_type close_cb;
  sio_vpu_func_ptr_type close_cb_ext;
  void  *close_cb_ext_data;

  close_cb = info->close_cb;
  close_cb_ext = info->close_callback_ext_fn;
  close_cb_ext_data = info->close_callback_ext_data;

  SMD_SIO_TIMER_CLR( &(info->esc_seq_timer) );
  SMD_SIO_TIMER_UNDEF( &(info->esc_seq_timer) );

  /* free info structure */
  smd_sio_free_info( info );

  if (close_cb)
  {
    /* Call the close callback function. */
    close_cb();
  }
  else if (close_cb_ext)
  {
    /* call the extended close callback function */
    close_cb_ext( close_cb_ext_data );
  }
} /* smd_sio_close_cb */

/*===========================================================================
FUNCTION      smd_sio_flush_cb

DESCRIPTION   This callback is called when the flush operation is complete.
              It is registered when the client calls smd_sio_flush_tx. It
              calls the client's callback.

ARGUMENTS     port_id - SIO port being flushed

              cb_data - the client's callback function
  
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_flush_cb( void *cb_data )
{
  smd_sio_info_type *info = (smd_sio_info_type *) cb_data;
  sio_vv_func_ptr_type flush_cb;

  flush_cb = info->flush_callback_fn;

  flush_cb();
} /* smd_sio_flush_cb */

/*===========================================================================
FUNCTION      smd_sio_flush_cb_ext

DESCRIPTION   This callback is called when the flush operation is complete.
              It is registered when the client calls smd_sio_flush_tx. It
              calls the client's callback.

ARGUMENTS     port_id - SIO port being flushed

              cb_data - the client's callback function
  
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_flush_cb_ext( void *cb_data )
{
  smd_sio_info_type *info = (smd_sio_info_type *) cb_data;
  sio_vpu_func_ptr_type flush_cb;
  void *flush_cb_ext_data;        

  flush_cb = info->flush_callback_ext_fn;
  flush_cb_ext_data = info->flush_callback_ext_data;

  flush_cb( flush_cb_ext_data );
} /* smd_sio_flush_cb */

/*===========================================================================
FUNCTION      smd_sio_change_mode

DESCRIPTION   Change the mode of an open data stream.

ARGUMENTS     info - pointer to stream info structure

              param - pointer to sio_ioctl parameter union, the open_ptr field
                      contains a pointer that points to the new mode info.
  
DEPENDENCIES  
  
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_change_mode
(
  smd_sio_info_type    *info,
  sio_ioctl_param_type *param
)
{
  sio_open_type *open_ptr = param->open_ptr;
  dsm_watermark_type *old_rx_queue;
  smd_ioctl_param_type smd_param;

  if( open_ptr->stream_mode == info->open.stream_mode )
  {
    return;   /* do nothing, same mode.... */
  }

  /*-----------------------------------------------------
    Reset All of the variables upon mode change.  This
    is common for all types of modes.  We do the tx first,
    then the Rx vars.
    -----------------------------------------------------*/
  info->open.stream_mode = open_ptr->stream_mode;

  /*  
  **   TX vars
  */

  if (open_ptr->tx_queue != info->open.tx_queue)
  {
    info->open.tx_queue = open_ptr->tx_queue;
    smd_param.tx_buffer = param->open_ptr->tx_queue;
    smd_wm_ioctl( info->smd_channel_id, 
                  SMD_IOCTL_CHANGE_MODE,
                  &smd_param );
  }

  /*  
  **   RX vars
  */
  /* Clear out the callback function pointers. */
  info->esc_callback_fn = NULL;
  info->pkt_callback_fn = NULL;

  /* These paramters must be protected from SMD task access */
  SMD_DSM_LOCK();

  old_rx_queue = info->open.rx_queue;
  info->open.rx_queue =  open_ptr->rx_queue;

  if( (old_rx_queue != NULL) && (old_rx_queue->current_cnt > 0) )
  {
    ERR("We are going to erase a non-empty watermark", old_rx_queue, 0, 0); 
    dsm_empty_queue(old_rx_queue);
  }

  info->open.rx_func_ptr = open_ptr->rx_func_ptr;

  SMD_DSM_UNLOCK();
} /* smd_sio_change_mode */

/*===========================================================================
FUNCTION      smd_sio_dtr_cb

DESCRIPTION   DTR toggle callback that is called when the DTR toggles.
              Registered immediately after open.

              When DTR goes away it means that the other processor has crashed,
              so we reset the the autodetect and escape sequence state machines.

              The Client's callback (if any) is called.

ARGUMENTS     port_id - SIO port being flushed

              cb_data - a pointer to the info structure associated with the
                        port_id

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_sio_dtr_cb( void *cb_data )
{
  smd_sio_info_type *info = (smd_sio_info_type *) cb_data;
  smd_ioctl_param_type param;
  boolean dte_asserted;
  sio_vv_func_ptr_type  dtr_cb;
  sio_vpu_func_ptr_type dtr_cb_ext;
  void *dtr_cb_ext_data;

  param.dte_ready_asserted = &dte_asserted;

  smd_wm_ioctl( info->smd_channel_id, 
                SMD_IOCTL_DTE_READY_ASSERTED,
                &param);

  dtr_cb = info->dtr_callback_fn;

  SMD_DTR_LOCK();
  dtr_cb_ext = info->dtr_callback_ext_fn;
  dtr_cb_ext_data = info->dtr_callback_ext_data;
  SMD_DTR_UNLOCK();

  if( ! dte_asserted )
  {
    info->esc_seq_state = SIOSMEM_ESC_SEQ_1ST_GUARDTIME;
  }

  /* Call the "simple" callback */
  if( dtr_cb != NULL )
  {
    dtr_cb();
  }

  /* Call the "extended" callback */
  if( dtr_cb_ext != NULL )
  {
    dtr_cb_ext( dtr_cb_ext_data );
  }
} /* smd_sio_dtr_cb */

/*===========================================================================
FUNCTION      SMD_SIO_REGISTER_DEVICES

DESCRIPTION   registers all of the available smd ports with sio
 
ARGUMENTS     sio_register_device_driver_cb_type register_driver_fn
               the sio registration funciton (passed from sio)
  
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_register_devices
(
  sio_register_device_driver_cb_type register_driver_fn
){
  sio_driver_interface driver_iface;
  sio_device_interafce device_iface;
  uint32 ret, i, port_id;

  /* build the sio device registration info structure */
  driver_iface.HW_open =             smd_sio_open;
  driver_iface.HW_close =            smd_sio_close;
  driver_iface.HW_ioctl =            smd_sio_ioctl;
  driver_iface.HW_transmit =         smd_sio_dev_transmit;
  driver_iface.HW_flush_tx =         smd_sio_flush_tx;
  driver_iface.HW_disable_device =   smd_sio_disable_device;
  driver_iface.HW_control_open =     smd_sio_control_open;
  driver_iface.HW_control_close =    smd_sio_control_close;
  driver_iface.HW_control_ioctl =    smd_sio_control_ioctl;
  driver_iface.HW_control_transmit = smd_sio_control_transmit;

  device_iface.driver_interface = &driver_iface;
  device_iface.forward_port = FALSE;
  device_iface.driver_available = TRUE;

  /* cycle trough each valid processor */
  for(i = 0; i <= SMD_SIO_NUM_PROCS; i++)
  {
    /* if this is a invalid destination processor for this processor */
    if(smd_sio_channel_types[i].channel_type == SMD_INVALID_CHANNEL_TYPE)
    {
       continue;
    }

    /* register for legacy SMD ports */
    if( smd_sio_channel_types[i].major_id == SIO_MAJOR_SMD )
    {
      for(port_id = SIO_MINOR_SMD_FIRST; port_id <= SIO_MINOR_SMD_MAX; port_id++)
      {
        device_iface.device_id = DEVICE_ID(SIO_MAJOR_SMD, port_id);
        ret = register_driver_fn(&device_iface);
        if(ret == 0)
        {
          ERR_FATAL("SIO_MAJOR_SMD registration failed with minor = 0x%x.", 
                    port_id, 0, 0);
        }
      }
    }
    else /* register for n-way SMD ports */
    {
      if( smd_sio_channel_types[i].major_id == SIO_MAJOR_SMD_TO_APPS )
      {
        device_iface.device_id = DEVICE_ID(SIO_MAJOR_SMD_TO_APPS ,
                                           SIO_MINOR_SMD_DIAG );
      }
      if( smd_sio_channel_types[i].major_id == SIO_MAJOR_SMD_TO_MODEM )
      {
        device_iface.device_id = DEVICE_ID(SIO_MAJOR_SMD_TO_MODEM ,
                                           SIO_MINOR_SMD_DIAG );
      }
      //if( smd_sio_channel_types[i].major_id == SIO_MAJOR_SMD_TO_QDSP6 )
      //{
        //device_iface.device_id = DEVICE_ID(SIO_MAJOR_SMD_TO_QDSP6 ,
          //                                 SIO_MINOR_SMD_DIAG );
      //}

      ret = register_driver_fn(&device_iface);
      if(ret == 0)
      {
        ERR_FATAL("registration failed with major = 0x%x.", 
                  smd_sio_channel_types[i].major_id, 0, 0);
      }
    }
  }
  
}/* smd_sio_register_devices */


/*===========================================================================
                     EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      SMD_SIO_DEV_INIT

DESCRIPTION   Initializes the SMD SIO device.
  
DEPENDENCIES  Should be called just once.
  
RETURN VALUE  None
  
SIDE EFFECTS  None
===========================================================================*/
void smd_sio_dev_init( void )
{
  memset( smd_sio_data_port_to_info, 0, sizeof(smd_sio_data_port_to_info) );
  memset( smd_sio_control_port_to_info, 0,
          sizeof(smd_sio_control_port_to_info) );
} /* smd_sio_dev_init */

/*===========================================================================
FUNCTION      smd_sio_open

DESCRIPTION   This function opens a shared memory stream.

ARGUMENTS     open_ptr - SIO open structure containing open params.

DEPENDENCIES  smd_sio_dev_init must have been called.
 
RETURN VALUE  SIO_DONE_S:     SIO stream as specified in open_ptr has been
                              successfully opened.
              SIO_BADP_S:     Bad Parameter
              SIO_UNAVAIL_S:  All available streams have been allocated.
    
SIDE EFFECTS  None
===========================================================================*/
sio_status_type smd_sio_open( sio_open_type *open_ptr )
{
  smd_sio_info_type *info;
  smd_dataxfr_mode_type dataxfr_mode;
  dsm_mempool_id_enum_type dsm_pool_id;
  smd_ioctl_param_type param;
  smd_port_id_type smd_id;
  smd_open_param_type smd_param;
  
  if(smd_sio_validate_port_id(open_ptr->port_id, 
                              SMD_SIO_DATA_PORT, NULL, NULL) == FALSE)
  {
    return SIO_BADP_S;
  }

  info = smd_sio_alloc_info(open_ptr->port_id, SMD_SIO_DATA_PORT);
  if( info == NULL )
  {
    return SIO_UNAVAIL_S;
  }

  smd_mem_copy( &info->open, sizeof(sio_open_type), open_ptr, 
                sizeof(sio_open_type) );

  smd_sio_get_port_defaults( open_ptr->port_id,
                             open_ptr->open_param,
                             &dataxfr_mode,
                             &dsm_pool_id,
                             &smd_param);

  info->esc_seq_state = SIOSMEM_ESC_SEQ_1ST_GUARDTIME;

  /* Initialize the callback structure for escape sequence timer */
  SMD_SIO_TIMER_DEF ( &(info->esc_seq_timer), 
                      smd_sio_esc_seq_guardtime_isr,
                      open_ptr->port_id);

  smd_id = smd_wm_open_3( smd_sio_get_port_name( open_ptr->port_id ),
                          info->channel_type,
                          info->open.tx_queue,
                          NULL,
                          smd_sio_process_rx_buffer,
                          (uint32) info,
                          dataxfr_mode,
                          dsm_pool_id,
                          &smd_param);

  /* store the channel id */
  info->smd_channel_id = smd_id;
  
  param.enable_dte_ready_event_ext.cb_func = smd_sio_dtr_cb;
  param.enable_dte_ready_event_ext.cb_data = (void *) info;

  smd_wm_ioctl( smd_id, 
                SMD_IOCTL_ENABLE_DTR_EVENT_EXT, 
                &param );

  return SIO_DONE_S;
} /* smd_sio_open */

/*===========================================================================
FUNCTION      smd_sio_close
                                                                             
DESCRIPTION   This function starts closing the port. It saves the client's
              callback function which will be called when the port is closed.

ARGUMENTS     stream_id - the SIO stream id of the port

              port_id - SIO port if of the port to close

              close_cb - callback to call when the stream is closed and all
                        of the queued data has been received by the other
                        processor.
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called when all pending data has been received
              by the other processor
===========================================================================*/
void smd_sio_close
( 
    sio_stream_id_type stream_id,
    sio_port_id_type   port_id,
    sio_vv_func_ptr_type close_cb
)
{
  /* FIX ME - If there is a close pending then we should prevent a second
     close from being initiated.
  */

  smd_sio_info_type *info;

  info = smd_sio_get_info( stream_id, port_id, SMD_SIO_DATA_PORT );
  ASSERT(info != NULL);

  /* Save app close function pointer */
  info->close_cb = close_cb;

  /* Close the SMEM stream */
  smd_wm_close( info->smd_channel_id, 
                smd_sio_close_cb, (void *)info, TRUE );

} /* smd_sio_close */

/*===========================================================================
FUNCTION      smd_sio_close_ext
                                                                             
DESCRIPTION   This function starts closing the port. It saves the client's
              extended callback function which will be called when the port 
              is closed.

ARGUMENTS     port_id - SIO port to close

              close_cb - callback to call when the stream is closed and all
                        of the queued data has been received by the other
                        processor.

              cb_data - data to pass the the close_cb when ready
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called when all pending data has been received
              by the other processor
===========================================================================*/
void smd_sio_close_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type close_cb,
  void                  *cb_data
)
{
  /* FIX ME - If there is a close pending then we should prevent a second
     close from being initiated.
  */

  smd_sio_info_type *info;

  //info = smd_sio_get_info( stream_id, port_id );
  info = smd_sio_get_info( 0, port_id, SMD_SIO_DATA_PORT );
  ASSERT(info != NULL);

  /* Save app close function pointer */
  info->close_callback_ext_fn = close_cb;
  info->close_callback_ext_data = cb_data;

  /* Close the SMEM stream */
  smd_wm_close( info->smd_channel_id,
                smd_sio_close_cb, (void *)info, TRUE );

} /* smd_sio_close_ext */

/*===========================================================================
FUNCTION      smd_sio_flush_tx
                                                                             
DESCRIPTION   This function flushes the TX.

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to flush

              flush_cb - callback to call when all of the queued data has
                         been received by the other processor.

DEPENDENCIES  User should not queue more data until after the registered
              callback is executed.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_flush_tx
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type   port_id,
  sio_vv_func_ptr_type flush_cb
)
{
  /* FIX ME - If there is a flush pending then we should prevent a second
     flush from being initiated.
  */

  smd_sio_info_type *info;

  info = smd_sio_get_info( stream_id, port_id, SMD_SIO_DATA_PORT );
  ASSERT(info != NULL);

  if( flush_cb == NULL )
  {
    ERR_FATAL("smd_sio_flush_tx: no flush callback function specified",0,0,0);
  }
  info->flush_callback_fn = flush_cb;

  smd_flush_tx( info->smd_channel_id,
                smd_sio_flush_cb,
                ( void * )info );
} /* smd_sio_flush_tx */

/*===========================================================================
FUNCTION      smd_sio_flush_tx_ext
                                                                             
DESCRIPTION   This function flushes the TX and then calls an extended
              callback.

ARGUMENTS     port_id - SIO port to flush

              flush_cb - extended callback to call when all of the queued data 
                         has been received by the other processor.
                  
              cb_data - data to use when calling the extended callback
DEPENDENCIES  User should not queue more data until after the registered
              callback is executed.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_flush_tx_ext
( 
  sio_port_id_type      port_id,
  sio_vpu_func_ptr_type flush_cb,
  void                  *cb_data
)
{
  /* FIX ME - If there is a flush pending then we should prevent a second
     flush from being initiated.
  */

  smd_sio_info_type *info;

  info = smd_sio_get_info(0, port_id, SMD_SIO_DATA_PORT);
  ASSERT(info != NULL);

  if( flush_cb == NULL )
  {
    ERR_FATAL("smd_sio_flush_tx_ext: no flush callback function specified",
               0,0,0);
  }
  info->flush_callback_ext_fn = flush_cb;
  info->flush_callback_ext_data = (void *)cb_data;

  smd_flush_tx( info->smd_channel_id,
                smd_sio_flush_cb_ext,
                ( void * ) info );
} /* smd_sio_flush_tx_ext */

/*===========================================================================
FUNCTION      smd_sio_dev_transmit

DESCRIPTION   This function queues the dsm chain and signals the smd task that
              data is pending.

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to send the data on

              tx_ptr - pointer to DSM item to send

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_dev_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type   port_id,             /* SIO Port ID */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
  smd_sio_info_type *info;

  if(dsm_length_packet(tx_ptr) == 0)
  {
    dsm_free_packet(&tx_ptr);
  }
  else
  {
    info = smd_sio_get_info( stream_id, port_id, SMD_SIO_DATA_PORT );
    ASSERT(info != NULL);
    dsm_enqueue(info->open.tx_queue, &tx_ptr);
    smd_dsm_notify_write( info->smd_channel_id );
  }
} /* smd_sio_dev_transmit */

/*===========================================================================
FUNCTION      smd_sio_ioctl_cmd

DESCRIPTION   This function is used to control various features of the SMD
              interface(both SMD_SIO_DATA_PORT and SMD_SIO_CONTROL_PORT).
              The behaviour of this function depends on the "command" issued.

ARGUMENTS     info - pointer to smd sio info

              cmd - SIO IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_ioctl_cmd
(
  smd_sio_info_type    *info,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
  smd_ioctl_cmd_type smd_cmd;

  switch ( cmd )
  {
    case SIO_IOCTL_ENABLE_DTR_EVENT:/* Register DTR-Toggle callback function */
      info->dtr_callback_fn = param->enable_dte_ready_event;
      if( info->dtr_callback_fn == NULL )
      {
        ERR_FATAL("smd_sio_ioctl: no dtr callback function specified "
                  "(sio port id %x)",info->open.port_id,0,0);
      }
      else
      {
        (param->enable_dte_ready_event)();
      }
      break;

    case SIO_IOCTL_DISABLE_DTR_EVENT:/*Deregister the DTR-Toggle callback fn*/
      info->dtr_callback_fn = NULL;
      break;

    case SIO_IOCTL_ENABLE_AUTODETECT:
      info->open.stream_mode = SIO_DS_AUTODETECT_MODE;
      break;

    case SIO_IOCTL_RECORD_ESC_FUNC_PTR:
      /* This command changes the callback function pointer for the escape
         code detection. */
      if ( info->open.stream_mode != SIO_DS_RAWDATA_MODE)
      {
        ERR( "Escape code func. ptr recorded in mode != RawData",0,0,0);  
      }

      if( param->record_escape_func_ptr == NULL )
      {
        ERR_FATAL("smd_sio_ioctl: no record escape function specified "
                  "(sio port id %x)",info->open.port_id,0,0);
      }

      /* Use this callback function when detecting an escape sequence. */
      info->esc_callback_fn = param->record_escape_func_ptr;
      break;

    case SIO_IOCTL_RECORD_PKT_FUNC_PTR:
      /* This command changes the callback function for the PPP packet
         detection. */
      if ( info->open.stream_mode != SIO_DS_AUTODETECT_MODE)
      {
        ERR( "Packet func. ptr recorded in mode != AutoDetect",0,0,0);  
      }

      if( param->record_pkt_func_ptr == NULL )
      {
        ERR_FATAL("smd_sio_ioctl: no record packet function specified "
                  "(sio port id %x)",info->open.port_id,0,0);
      }

      /* Use this packet function pointer when detecting a PPP packet */
      info->pkt_callback_fn = param->record_pkt_func_ptr;
      break;

    case SIO_IOCTL_BEGIN_FAX_IMAGE:
      /* Nothing needs to be done here.  This is used to disable software
         control when a Fax image is being received.  We do NOT support
         software flow control over SMEM. */
      break;

    case SIO_IOCTL_FLUSH_TX:
      /* Allow the data to go out and call the callback function. */
      smd_sio_flush_tx (info->open.stream_id,
                        info->open.port_id,
                        param->record_flush_func_ptr);
      break; /* SIO_IOCTL_FLUSH_TX */

    case SIO_IOCTL_CHANGE_MODE:
      smd_sio_change_mode( info, param );
      break;

    case SIO_IOCTL_ENABLE_DTR_EVENT_EXT:
      /* these two values are set in client context and read in
         smd_task context */
      SMD_DTR_LOCK();
      info->dtr_callback_ext_fn = param->enable_dte_ready_event_ext.cb_func;
      info->dtr_callback_ext_data = param->enable_dte_ready_event_ext.cb_data;
      SMD_DTR_UNLOCK();

      if( info->dtr_callback_ext_fn == NULL )
      {
        ERR_FATAL("smd_sio_ioctl: no dtr ext callback function specified "
                  "(sio port id %x)",info->open.port_id,0,0);
      }
      else
      {
        (param->enable_dte_ready_event_ext.cb_func)(
            param->enable_dte_ready_event_ext.cb_data);
      }

      break;

    case SIO_IOCTL_DISABLE_DTR_EVENT_EXT:
      /* these two values are set in client context and read in
         smd_task context */
      SMD_DTR_LOCK();
      info->dtr_callback_ext_fn = NULL;
      info->dtr_callback_ext_data = NULL;
      SMD_DTR_UNLOCK();
      break;

    case SIO_IOCTL_POWERDOWN:
    case SIO_IOCTL_WAKEUP:
      /* SMD ignores these IOCTL */
      break;

    /* these are only meaningful for physical ports, which should go
       through smd_bridge_ioctl. filter it out for local smd ports */
    case SIO_IOCTL_CHANGE_BAUD_RATE:
    case SIO_IOCTL_CHANGE_BAUD_NOW:
    case SIO_IOCTL_REG_TLP_CHANGE_CB:
      MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "IOCTL(%d) not supported by Local"
            " SMD port(0x%08x)", cmd, info->smd_channel_id);
      break;

    default:
      smd_cmd = smd_sio_ioctl_cmd_translate( cmd );

      if( smd_cmd != SMD_IOCTL_INVALID )
      {
        smd_wm_ioctl( info->smd_channel_id,
                      smd_cmd,
                      ( smd_ioctl_param_type * )param );
      }
      else
      {
        ERR( "IOCTL %d not handled by SMD", (uint32) cmd, 0, 0 );
      }
      break;
  }
} /* smd_sio_ioctl_cmd */

/*===========================================================================
FUNCTION      smd_sio_ioctl

DESCRIPTION   Handles data port ioctls

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to control

              cmd - SIO IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_port_id_type      port_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
  smd_sio_info_type *info;

  info = smd_sio_get_info( stream_id, port_id, SMD_SIO_DATA_PORT );
  ASSERT(info != NULL);
  smd_sio_ioctl_cmd(info, cmd, param);
}


/*===========================================================================
FUNCTION      smd_sio_disable_device

DESCRIPTION   Does nothing, there's no way to disable this device.

RETURN VALUE  None
===========================================================================*/
void smd_sio_disable_device( sio_port_id_type port_id )
{
} /* sio_disable_device */

/*===========================================================================
FUNCTION      smd_sio_process_control_rx_buffer

DESCRIPTION   The function is called when a client registering at the
              SIO layer registers a recv callback, and a packet of data
              is received over the SMEM control port.

ARGUMENTS     control_port_id - the port id of the control port
              item - a pointer to the dsm_item coming over the SM
              cb_data - a pointer to the client CB function

DEPENDENCIES  smd_sio_dev_init must have been called.
 
RETURN VALUE  None
    
SIDE EFFECTS  This function will not service a client recv wm_queue.  
===========================================================================*/
static void smd_sio_process_control_rx_buffer
(
  smd_port_id_type   port_id,
  dsm_item_type     *item,
  uint32             cb_data
)
{
  sio_rx_func_ptr_type rx_cb_fn;

  /* since the open struct is not copied for CONTROL ports we store the
     client callback pointer in the cb_data */
  rx_cb_fn = (sio_rx_func_ptr_type) cb_data;

  if( rx_cb_fn != NULL )
  {
    /* call the client defined callback */
    rx_cb_fn( &item );
  }
  else
  {
    /* no place to put the dsm_items...free the chain */
    dsm_free_packet( &item );
  }
}

/*===========================================================================
FUNCTION      smd_sio_control_open

DESCRIPTION   This function opens a control shared memory stream.

ARGUMENTS     open_ptr - SIO open structure containing open params.

DEPENDENCIES  smd_sio_dev_init must have been called.
 
RETURN VALUE  SIO_DONE_S:     SIO stream as specified in open_ptr has been
                              successfully opened.
              SIO_BADP_S:     Bad Parameter
              SIO_UNAVAIL_S:  All available streams have been allocated.
    
SIDE EFFECTS  None
===========================================================================*/
sio_status_type smd_sio_control_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
)
{
  smd_wm_read_callback_fn   client_cb;
  smd_port_id_type          smd_id;
  smd_sio_info_type        *info;
  smd_ioctl_param_type      param;
  dsm_mempool_id_enum_type  dsm_pool_id;
  uint32                    size;

  if(smd_sio_validate_port_id(open_ptr->port_id,
                              SMD_SIO_CONTROL_PORT, NULL, NULL) == FALSE)
  {
    return SIO_BADP_S;
  }

  info = smd_sio_alloc_info(open_ptr->port_id, SMD_SIO_CONTROL_PORT);
  ASSERT(info != NULL);

  smd_mem_copy( &info->open, sizeof(sio_open_type), open_ptr, 
                sizeof(sio_open_type) );

  /* Only set up a callback for the wm_layer if the user defines one */
  client_cb = ((open_ptr->rx_func_ptr==NULL) ? (NULL) : 
               (smd_sio_process_control_rx_buffer));

  dsm_pool_id = DSM_DS_SMALL_ITEM_POOL;
  if (( open_ptr->port_id == SIO_PORT_SMD_DIAG )       || 
      ( open_ptr->port_id == SIO_PORT_SMD_DIAG2 )      ||
      ( open_ptr->port_id == SIO_PORT_SMD_DIAG_MODEM ) ||
      ( open_ptr->port_id == SIO_PORT_SMD_DIAG_APPS ))
      //||( open_ptr->port_id == SIO_PORT_SMD_DIAG_QDSP ))
  {
    if( open_ptr-> open_param )
    {
      dsm_pool_id  = open_ptr->open_param->dsm_pool_param.dsm_mempool_id;
    }
  }
  
  /* Set size field.  If NULL or not specified, default to 1k */
  /* Note: max size is checked in smd_open_memcpy_3 sub-function */
  size = (open_ptr->open_param &&
          open_ptr->open_param->dsm_pool_param.smd_fifo_size) ?
          open_ptr->open_param->dsm_pool_param.smd_fifo_size  :
          SMD_MIN_FIFO;

  /* control port logic is handled at the wm layer */
  smd_id = smd_wm_open_sz( smd_sio_get_cntl_port_name( open_ptr->port_id ),
                           info->channel_type,
                           open_ptr->tx_queue,
                           open_ptr->rx_queue,
                           client_cb,
                           (uint32)open_ptr->rx_func_ptr,
                           SMD_MEMCPY_MODE,
                           SMD_PKTXFR_TYPE,
                           dsm_pool_id,
                           size);

  /* store the channel id */

  info->smd_channel_id = smd_id;

  /* Set flow control to allow flow */
  smd_ioctl( smd_id, SMD_IOCTL_INBOUND_FLOW_ENABLE, NULL);

  /* Set CD line */
  smd_ioctl( smd_id, SMD_IOCTL_CD_ASSERT, NULL);

  /* Set DSR line */
  smd_ioctl( smd_id, SMD_IOCTL_DSR_ASSERT, NULL);

  /* Clear RI line */
  smd_ioctl( smd_id, SMD_IOCTL_RI_DEASSERT, NULL);

  /* connect the dtr callback */
  param.enable_dte_ready_event_ext.cb_func = smd_sio_dtr_cb;
  param.enable_dte_ready_event_ext.cb_data = (void *) info;

  smd_wm_ioctl( smd_id,
                SMD_IOCTL_ENABLE_DTR_EVENT_EXT,
                &param );

  return SIO_DONE_S;
} /* smd_sio_control_open */

/*===========================================================================
FUNCTION      smd_sio_control_close_cb

DESCRIPTION   This callback is called when the stream is closed. It is
              registered when the client calls smd_sio_control_close with a
              non-NULL close_cb.  This function simply converts the pointer
              to the client CB to the appropriate sio type, and then calls it
              if it is non-NULL.

ARGUMENTS     data - A pointer to the client CB

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_sio_control_close_cb( void *data )
{
  sio_vv_func_ptr_type client_cb;

  client_cb = (sio_vv_func_ptr_type)data;

  if( client_cb != NULL )
  {
    client_cb();
  }
}

/*===========================================================================
FUNCTION      smd_sio_control_close
                                                                             
DESCRIPTION   This function starts closes a control port.  The closure
              of the control section of this 

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to close

              close_cb - callback to call when the stream is closed
                  
DEPENDENCIES  None
      
RETURN VALUE  None
  
SIDE EFFECTS  close_cb will be called regardless of if there is pending data
              for this control port in the stream
===========================================================================*/
void smd_sio_control_close
(
    sio_stream_id_type stream_id,
    sio_port_id_type   port_id,
    void             (*close_cb)(void)
)
{
  smd_sio_info_type *info;

  info = smd_sio_get_info(stream_id, port_id, SMD_SIO_CONTROL_PORT);
  ASSERT(info != NULL);

  if( close_cb != NULL )
  {
    /* Only register a CB if there if the client inputted one */
    smd_wm_close( info->smd_channel_id, 
                  smd_sio_control_close_cb, 
                  (void *)close_cb,
                  FALSE );
  }
  else
  {
    smd_wm_close( info->smd_channel_id,
                  NULL,
                  NULL,
                  FALSE );
  }

  smd_sio_free_info(info);

} /* smd_sio_control_close */

/*===========================================================================
FUNCTION      smd_sio_control_transmit

DESCRIPTION   Call smd_wm_control_transmit for a control port

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to send the data on

              tx_ptr - pointer to DSM item to send

DEPENDENCIES  None
 
RETURN VALUE  None
 
SIDE EFFECTS  None
===========================================================================*/
void smd_sio_control_transmit
( 
  sio_stream_id_type stream_id,
  sio_port_id_type   port_id,
  dsm_item_type     *tx_ptr)
{
  smd_sio_info_type *info;

  info = smd_sio_get_info(stream_id, port_id, SMD_SIO_CONTROL_PORT);
  ASSERT(info != NULL);
  dsm_enqueue(info->open.tx_queue, &tx_ptr);
  smd_dsm_notify_write( info->smd_channel_id );
} /* smd_sio_control_transmit */

/*===========================================================================
FUNCTION      smd_sio_control_ioctl

DESCRIPTION   Handles control port ioctls

ARGUMENTS     stream_id -sio stream id of the port to close (ignored)

              port_id - SIO port to control

              cmd - SIO IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void smd_sio_control_ioctl
(
  sio_stream_id_type    stream_id,
  sio_port_id_type      port_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
  smd_sio_info_type *info;

  info = smd_sio_get_info( stream_id, port_id, SMD_SIO_CONTROL_PORT );
  ASSERT(info != NULL);
  smd_sio_ioctl_cmd(info, cmd, param);

}/* smd_sio_control_ioctl */

/*===========================================================================
 Functions external to SMD
============================================================================*/

/*===========================================================================
FUNCTION      SMD_SIO_DEV_INIT

DESCRIPTION   Initializes the SMD SIO device.
  
DEPENDENCIES  Should be called just once.

ARGUMENTS     sio_register_device_driver_cb_type register_driver_fn 
               The registration function passed by SIO

RETURN VALUE  Always 1
  
SIDE EFFECTS  None
===========================================================================*/
uint32 smd_sio_driver_init
(
  sio_register_device_driver_cb_type register_driver_fn
){
  memset( smd_sio_data_port_to_info, 0, sizeof(smd_sio_data_port_to_info) );
  memset( smd_sio_control_port_to_info, 0,
          sizeof(smd_sio_control_port_to_info) );

  /* register all of the ports with SIO */
  smd_sio_register_devices(register_driver_fn);

  return 1;
} /* smd_sio_dev_init */

/*===========================================================================
FUNCTION      smd_sio_get_sio_id_from_name

DESCRIPTION   Get SIO port ID for a given SMD port name
  
DEPENDENCIES  None

ARGUMENTS     const char * smd_port_name
               SMD port name (string)

RETURN VALUE  SIO port ID for the given SMD port name
              SIO_PORT_NULL : Error
   
SIDE EFFECTS  None
===========================================================================*/
sio_port_id_type smd_sio_get_sio_id_from_name
(
  const char * smd_port_name
)
{
  char* endptr;
  int data_port_num, smd_minor_num;
  
  if(smd_port_name == NULL)
  {
    return SIO_PORT_NULL;
  }

  if(strlen(smd_port_name) > strlen("DATA40_CNTL"))
  {
    return SIO_PORT_NULL;
  }

  /* Check for validity of first 4 bytes */
  if( smd_port_name[0] != 'D' ||
      smd_port_name[1] != 'A' ||
      smd_port_name[2] != 'T' ||
      smd_port_name[3] != 'A' )
  {
    return SIO_PORT_NULL;
  }

  /* starting 5th byte, the characters represent port number */
  data_port_num = strtol(&smd_port_name[4], &endptr, 10);

  if(data_port_num < 1 || data_port_num > 40)
  {
    return SIO_PORT_NULL;
  }

  /* Check the character following the port number */
  if(endptr[0] != '\0')
  {
    /* check to see if port name has _CNTL at the end */
    if(endptr[0] != '_' ||
       endptr[1] != 'C' ||
       endptr[2] != 'N' ||
       endptr[3] != 'T' ||
       endptr[4] != 'L' ||
       endptr[5] != '\0' )
    {
      return SIO_PORT_NULL;
    }
  }

  /* Deduce SIO_PORT_SMD_DATA_# */
  smd_minor_num = SIO_MINOR_SMD_DATA1 + (data_port_num - 1);

  return (sio_port_id_type)( DEVICE_ID(SIO_MAJOR_SMD , smd_minor_num) );
}
