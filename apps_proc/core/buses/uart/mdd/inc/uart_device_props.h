#ifndef __UART_DEVICE_PROPS_H__
#define __UART_DEVICE_PROPS_H__

/*==================================================================================================

FILE: uart_device_props.h

DESCRIPTION: Defines the common properties needed for the UART driver used by SIO

                     Copyright (c) 2015 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#include "com_dtypes.h"
#include "DALSys.h"
#include "uart_log.h"

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

//  xxx:  There is no DAL invalid device id but 0 is not used. 

#define UART_INVALID_DALDEVICEID    0

#define DALDEVICEID_UARTBAM_DEVICE_1   0x02001000
#define DALDEVICEID_UARTBAM_DEVICE_2   0x02001001
#define DALDEVICEID_UARTBAM_DEVICE_3   0x02001002
#define DALDEVICEID_UARTBAM_DEVICE_4   0x02001003
#define DALDEVICEID_UARTBAM_DEVICE_5   0x02001004
#define DALDEVICEID_UARTBAM_DEVICE_6   0x02001005
#define DALDEVICEID_UARTBAM_DEVICE_7   0x02001006
#define DALDEVICEID_UARTBAM_DEVICE_8   0x02001007
#define DALDEVICEID_UARTBAM_DEVICE_9   0x02001008
#define DALDEVICEID_UARTBAM_DEVICE_10  0x02001009
#define DALDEVICEID_UARTBAM_DEVICE_11  0x0200100A
#define DALDEVICEID_UARTBAM_DEVICE_12  0x0200100B
#define DALDEVICEID_UARTCXM            0x0200100C

//----------------------------------------------------------------------------
///  Choice values for driver type in custsio.h.
//----------------------------------------------------------------------------

#define NO_DRIVER               0   //  Should only be used with UART_INVALID_DALDEVICEID.
#define PIO_DRIVER              1   //  Programmed I/O => don't use DMA.  Run in "legacy" mode.
                                    //  Can be used with legacy UARTs (non DM) or UART DM.
#define BAM_DRIVER              2   //  Use BAM for DMA.     
#define DM_DRIVER               3   //  Use DM for DMA.    

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/
void get_bool_property( DALSYSPropertyHandle handle, const char *name, boolean *prop_handle,
                        boolean default_value );
void get_dword_property( DALSYSPropertyHandle handle, const char *property_name,
                         uint32 *property_value, uint32 default_value );

#endif
