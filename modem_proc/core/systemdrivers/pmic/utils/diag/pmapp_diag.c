/*! \file
 *  \n
 *  \brief  pmapp_diag.c ---- PMIC-DIAG
 *  \details  This file contains variable and function declarations to support
 *   the PMIC diag interface.
 *
 *
 *  \n &copy; Copyright (c) 2009        by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/systemdrivers/pmic/utils/diag/pmapp_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/12   hs      Moved pm_app_diag_lib_diag_dev_i() from pm_app.cpp.
11/30/11   dy      Fix diag crash when parameter is missing from peek cmd
06/16/09   jtn     Added command to peek/poke PMIC registers in upper pages.
05/24/09   wra     Added pm_app_set_mode_diag for FTM messages
05/01/09   jtn     Branched for SCMM
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES

===========================================================================*/
#include "core_variation.h"
#include "diagpkt.h"            /* Diagnostic packets  */

#include "pmapp_diag.h"
#include "pm_qc_pmic.h"
#include "pm_comm.h"
#include "pm_target_information.h"
#include "device_info.h"

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */


/*===========================================================================

                   DIAG COMMAND FUNCTION DEFINITIONS

===========================================================================*/

typedef PACK(struct)
{
  diagpkt_subsys_header_type  header;
  uint8 peek_poke; // 0 - to peek 1 - poke
  uint8 device;  // Select the PMIC to read from or write to
  uint8 sid;  //Slave ID
  uint16 reg; // PMIC register to peek/poke
  uint8 val; // Value to set/get from register
} pm_app_diag_lib_diag_dev_type;

/**
 *
 * @brief Function to read/write paged PMIC registers.
 *
 * This command adds the "send_data 75 36 0" command, to read and
 * write PMIC registers
 *
 * The QXDM command is
 * "send_data 75 36 0 0 [1-read 0=write] [deviceIndex] [sid] [register] [value if this is a write]
 *
 * So to write 0x80 to 0x4C46 (sid 1), the command is "send_data 75 36 0 0 0 0 1 0x46 0X4C 0x80"
 *
 * @param req_pkt Command request.
 * @param pkt_len Length of command request packet
 *
 * @return pointer to a response packet
 */
static PACKED void * pm_app_diag_peek_poke
(
  PACKED void *                   req_pkt,
  uint16                          pkt_len
)
{
    pm_app_diag_lib_diag_dev_type* req_ptr = (pm_app_diag_lib_diag_dev_type*)req_pkt;
    pm_app_diag_lib_diag_dev_type* rsp_ptr = (pm_app_diag_lib_diag_dev_type*)diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_PM, 0, 10 );
    uint8 val = 0;
    uint8 num_pmic_devices = PM_MAX_NUM_PMICS;

    uint32  slave_id = 0;

    //Paramater validation prior to requesting peek/poke
    if( req_ptr->device < num_pmic_devices && req_ptr->sid <= 1 && rsp_ptr != NULL )
    {
        pm_get_slave_id(req_ptr->device, req_ptr->sid , &slave_id);

            rsp_ptr->val = req_ptr->val;
            rsp_ptr->peek_poke = req_ptr->peek_poke;
            rsp_ptr->reg = req_ptr->reg;

            if( req_ptr->peek_poke != 0 ) // do a peek
            {
                if( pm_comm_read_byte(slave_id, req_ptr->reg, &val, 0) == PM_ERR_FLAG__SUCCESS )
                {
                    rsp_ptr->val = val;
                    rsp_ptr->peek_poke = 0x0;
                }
                else
                {
                    rsp_ptr->peek_poke = 0xFF;
                }
            }
            else // do a poke
            {
                if( pm_comm_write_byte(slave_id, req_ptr->reg, req_ptr->val, 0) == PM_ERR_FLAG__SUCCESS )
                {
                    rsp_ptr->peek_poke = 0x0;
                }
                else
                {
                    rsp_ptr->peek_poke = 0xFF;
                }
            }
    }

    return ( rsp_ptr );
}

/**
 * Diagnostics command dispatch table.
 */
static const diagpkt_user_table_entry_type pm_app_diag_tbl[] =
{
  {PM_APP_DIAG_PEEK_POKE, PM_APP_DIAG_PEEK_POKE, pm_app_diag_peek_poke},
};


/**
 * @brief Register the diag packet function dispatch table.
 */
void pm_app_diag_init (void)
{
DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_MODEM_PROC, DIAG_SUBSYS_PM, pm_app_diag_tbl);
}

/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */