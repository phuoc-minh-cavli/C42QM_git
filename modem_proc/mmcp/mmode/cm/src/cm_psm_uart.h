#ifndef _CM_PSM_UART_H_
#define _CM_PSM_UART_H_
#include "sio.h"

#ifdef FEATURE_BUILD_PAGEONLY_IMAGE

#define CM_AT_CMD  "AT$QCFBC"
#define CM_AT_COP_RSP_OK      "OK"
#define CM_AT_COP_RSP_ERR   "ERROR"

#define AT_CMD_LEN   ((sizeof(CM_AT_CMD)/sizeof(char)) - 1)
#endif

#ifdef FEATURE_BUILD_SENSORONLY_IMAGE

#define CM_MAX_SERIAL_BUFFER 512
/*===========================================================================

FUNCTION cm_send_at_cmd_rsp

DESCRIPTION
    it will actually send the response to UART

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/

extern void cm_send_serial_log(char *data, uint16 len);

/*===========================================================================

FUNCTION cm_serial_log_0

DESCRIPTION
    serial log for 0 arguement

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/


void cm_serial_log_0(char *data,uint16 len);

/*===========================================================================

FUNCTION cm_serial_log_1

DESCRIPTION
    serial log for 1 arguement

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/


void cm_serial_log_1(char *data,uint32 int1);

/*===========================================================================

FUNCTION cm_serial_log_2

DESCRIPTION
   serial log for 2 arguement

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/


void cm_serial_log_2(char *data,uint32 int1,uint32 int2);

/*===========================================================================

FUNCTION cm_serial_log_3

DESCRIPTION
    serial log for 3 arguement

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/

void cm_serial_log_3(char *data,uint32 int1,uint32 int2,uint32 int3);

/*===========================================================================

FUNCTION cm_serial_log_4

DESCRIPTION
    serial log for 4 arguement

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/


void cm_serial_log_4(char *data,uint32 int1,uint32 int2,uint32 int3,uint32 int4);

#define CM_SERIAL_LOG_0(str) cm_serial_log_0(str,sizeof(str))
#define CM_SERIAL_LOG_1(str,int1) cm_serial_log_1(str,int1)
#define CM_SERIAL_LOG_2(str,int1,int2) cm_serial_log_2(str,int1,int2)
#define CM_SERIAL_LOG_3(str,int1,int2,int3) cm_serial_log_3(str,int1,int2,int3)
#define CM_SERIAL_LOG_4(str,int1,int2,int3,int4) cm_serial_log_4(str,int1,int2,int3,int4)

/*===========================================================================

FUNCTION cm_psm_sio_uart_init

DESCRIPTION
    It will init the uart 

DEPENDENCIES
   None

RETURN VALUE
  None
  
SIDE EFFECTS
  none

===========================================================================*/

extern void cm_psm_sio_uart_init(uint16 uart_baud);


#endif

#endif
