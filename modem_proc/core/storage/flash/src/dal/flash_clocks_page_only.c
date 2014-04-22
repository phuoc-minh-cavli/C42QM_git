/**********************************************************************
 * flash_clocks.c
 *
 * SERVICES:    QPIC/PNOC clock and SPI NOR related node voting management
 *
 * DESCRIPTION: Provides flash operation clocks voting for NAND/SPI NOR
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2013-2014, 2016 QUALCOMM  Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/core.mpss/3.10/storage/flash/src/dal/flash_clocks_page_only.c#1 $ $DateTime: 2019/01/25 01:01:08 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 06/27/16     sb      Add syncronization for clock request/complete APIs
 * 09/30/14     sb      NPA handle for PCNOC on Bear family
 * 05/23/13     eo      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "core_variation.h"
#include "flash.h"

#include "flash_clocks.h"
#include "flash_dal_config.h"

#include "npa.h"

#include "corebsp_rpm.h"

#define FLASH_NAND_REQUEST 100000
#define FLASH_NAND_ZERO 0
uint32 flash_clock_request = FLASH_NAND_REQUEST;
uint32 flash_clock_request_complete = FLASH_NAND_ZERO;
mpss_rpm_handle_t* qpic_handle = NULL;
mpss_rpm_handle_t* pnoc_handle = NULL;

/* Counter for flash_clocks_requests without an equivalent */
uint32 flash_clocks_req_counter = 0;




/* Flash NAND Operations Clocks init */
void flash_nand_clocks_init(void)
{
  /* check if flash nand  pnoc handle exists */
  if (!pnoc_handle)
  {
    pnoc_handle = mpss_kvp_init(RPM_CLOCK_1_REQ);
  }
  if (!qpic_handle)
  {
    qpic_handle = mpss_kvp_init(RPM_CLOCK_QPIC_REQ);
  }

  if (!pnoc_handle || !qpic_handle)
  {
    /* Error Fatal on  pnoc/qpic clock node registration failure. */
    DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
                    "Flash: Pageonly  pnoc/qpic clock node registration failed!");
  }
  flash_clocks_req_counter = 0;
}

/* Flash NAND Operations Clocks deinit */
void flash_nand_clocks_deinit(void)
{
  /* check if npa pnoc handle is registered */
  if (pnoc_handle)
  {
    mpss_kvp_deinit(pnoc_handle);
    pnoc_handle = NULL;
  }
  /* check if npa qpic handle is registered */
  if (qpic_handle)
  {
    mpss_kvp_deinit(qpic_handle);
    qpic_handle = NULL;
  }
}

/* Enter vote for QPIC and PNOC clocks for flash before
 * begining of flash operations.
 */
void flash_nand_clocks_request(void)
{
  if (flash_clocks_req_counter == 0)
  {
    if (pnoc_handle && qpic_handle)
    {
      /* Request pnoc clock for flash nand transfer */
      mpss_rpm_resource_request(pnoc_handle,  flash_clock_request,CLK_KEY_KHZ);
      /* Request qpic clock for flash nand transfer */
      mpss_rpm_resource_request(qpic_handle,  flash_clock_request,CLK_KEY_KHZ);
    }
  }
  flash_clocks_req_counter++;
}

/* Release vote for QPIC and PNOC clocks for flash at end of 
 * flash operations.
 */
void flash_nand_clocks_request_complete(void)
{
  flash_clocks_req_counter--;
  
  if (flash_clocks_req_counter == 0)
  {
    if (pnoc_handle && qpic_handle)
    {
        /* Request pnoc clock for flash nand transfer */
      mpss_rpm_resource_request(pnoc_handle,  flash_clock_request_complete,CLK_KEY_KHZ);
      /* Request qpic clock for flash nand transfer */
      mpss_rpm_resource_request(qpic_handle,  flash_clock_request_complete,CLK_KEY_KHZ);
    }
  }
}


//stubs to resolve linker errors
void flash_clocks_init(void)
{
}

void flash_clocks_deinit(void)
{
}

void flash_clocks_request(void)
{
}

void flash_clocks_request_complete(void)
{
}


