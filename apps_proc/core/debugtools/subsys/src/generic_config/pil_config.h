#ifndef __PIL_CONFIG_H__
#define __PIL_CONFIG_H__

/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
                 C O N F I G U R A T I O N   F I L E 
              
GENERAL DESCRIPTION
  Provides definitions for PIL Configuration

Copyright (c) 2016-17,2019 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/generic_config/pil_config.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/15/16   ab      File created.
05/04/17   ab      Added Support for PIL Hibernation
=============================================================================*/

//Todo: Need to move all these to DevCfg xml or something

/*=============================================================================

                            DATA DECLARATIONS
                            
=============================================================================*/

#define MBA_PARTITION_NAME      "0:MBA"
#define MODEM_PARTITION_NAME    "0:QDSP"

#define PIL_RMB_WAIT_TIMEOUT_MS 100
#define PIL_SMP2P_WAIT_TIMEOUT_MS 10000  //10 secs

#define PIL_LOAD_SEGMENT_WISE_CONFIG TRUE

#define IMEM_BASE_ADDRESS 0x08600000
#define SHARED_IMEM_ADDRESS IMEM_BASE_ADDRESS + 0x28000

#define PIL_IMEM_ADDRESS  SHARED_IMEM_ADDRESS+0x94C 
#define PIL_IMEM_SIZE     200
#define PIL_IMEM_COOKIE   0x53444247


/* Define start address for Q6 /Modem bootrom */
//Todo: Try without this PBL address programming and see if that is fine or needed..
#define MODEM_PBL_START_ADDR 0x00118000

#define PIL_MBA_RAM_BASE               MPSS_MBA_RAM_BASE
#define PIL_MBA_RAM_SIZE               MPSS_MBA_RAM_SIZE
//#define PIL_MODEM_META_DATA_SIZE       0x20000
//#define PIL_MODEM_META_DATA_ADDR       PIL_MBA_RAM_BASE + PIL_MBA_RAM_SIZE - PIL_MODEM_META_DATA_SIZE
#define PIL_MODEM_META_DATA_SIZE       MPSS_PIL_META_DATA_SIZE
#define PIL_MODEM_META_DATA_ADDR       MPSS_PIL_META_DATA_BASE

//BIMC frequencies
// gcc_bimc_clk  cbc gcc_bimc_ddr_grp  bimc  sm_cbc  202.10  404.20  808.40  1066.00
// The 19.2 MHz vote is for allowing USB HW access to DDR while APPS is in power collapse
#define PIL_BIMC_MIN_USB_MODE_SPEED_KHZ                (19200)
#define PIL_BIMC_SVS_MODE_SPEED_KHZ                    (120000)
#define PIL_BIMC_NOMINAL_MODE_SPEED_KHZ                (240000)
#define PIL_BIMC_TURBO_MODE_SPEED_KHZ                  (400000)

#endif   /* __PIL_CONFIG_H__ */

