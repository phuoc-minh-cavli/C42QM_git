/*=============================================================================

        P E R I P H E R A L   I M A G E   L O A D E R    M O D U L E
              C L O C K S   H A N D L I N G   M O D U L E 

GENERAL DESCRIPTION
  Implements clocks handling apis for PIL Framework
  Most of the code in this file follows HPG and Boot Build ClockPIL code


REGIONAL FUNCTIONS 
  None
  
EXTERNALIZED FUNCTIONS

  Clock_SetupProcessor(processor type)
    Does Initial setup for Given processor to bring out of reset.

  Clock_EnableProcessor(processor type)
    Starts given processor by setting required clock config
    
  Clock_ShutdownProcessor(processor type)
    Shutdowns given processor by setting required clock config
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None.

Copyright (c) 2016, 2020 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tx/6.0/debugtools/subsys/src/pil_clock.c#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------

09/15/16   ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "pil.h"
#include "pil_clock.h"
#include "pil_hwio.h"
#include "pil_os.h"
#include "pil_utils.h"
#include "msg.h"
#include <HALhwio.h>

/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/*=============================================================================

FUNCTION CLOCK_ENABLEPROCESSOR

DESCRIPTION
  This function triggers given processor to come out of reset

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Clock_EnableProcessor(ClockProcessorType eProcessorType)
{
  boolean bRetVal = FALSE;

  switch(eProcessorType)
  {
     case CLOCK_PROCESSOR_LPASS:
      bRetVal = FALSE;
      break;

     case CLOCK_PROCESSOR_MODEM:

     /* 
      * Change duData write ACC from default to relaxed.
      */
      HWIO_OUTF(MSS_QDSP6SS_STRAP_ACC, DATA, 0x80800000);
      HWIO_OUTF(MSS_QDSP6SS_RESET, STOP_CORE , 0x0);
      /*
       * Wait for core to come out of reset
       */
      pil_busywait(5);
  
      
      HWIO_OUT(MSS_QDSP6SS_GFMUX_CTL,
           (HWIO_MSS_QDSP6SS_GFMUX_CTL_SRC_SWITCH_CLK_OVR_BMSK |\
            HWIO_MSS_QDSP6SS_GFMUX_CTL_CLK_ENA_BMSK ) );
      //Todo: Need to check this for Lykan
      /*
       * SW must clear RET_ARES_ENA bit and set ARCH_ARES_ENA bit in 
       * QDSP6SS_RET_CFG register in advance to enable the system crash debug 
       * support via debug NMI. ARCH_ARES_ENA is already set on reset.
       */
      //HWIO_OUTF(MSS_QDSP6SS_RET_CFG, RET_ARES_ENA,0);
   
      bRetVal = TRUE; 
      break;

     //case CLOCK_PROCESSOR_RPM:
      //HWIO_OUTF(GCC_APSS_MISC, RPM_RESET_REMOVAL , 0x0);
      /*
       * Wait for core to come out of reset
       */
      //pil_busywait(5);
      //bRetVal = TRUE;
      //break;

      default:
       break;
  }

  return bRetVal;

} /* END Clock_EnableProcessor */

/*=============================================================================

FUNCTION SETUP_MSSPROCESSOR

DESCRIPTION
  This function sets up clocks to bring MSS QDSP6 out of reset.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
static boolean Setup_MSSProcessor(void)
{
  uint32 nQ6ACKTimeout = 20000;
  int32 nBankIndex = 0;
  
  //De-Assert GCC_MSS_RESTART register to enable MSS
  HWIO_OUT(GCC_MSS_RESTART, 0);
  
  /*
   * Enable MSS AHB.
   */
  HWIO_OUTF(GCC_MSS_CFG_AHB_CBCR, CLK_ENABLE, 0x1);
  while((HWIO_INF(GCC_MSS_CFG_AHB_CBCR, CLK_OFF) == 1) &&
          (nQ6ACKTimeout > 0))
  {
    pil_busywait(5);
    nQ6ACKTimeout--;
  }

  /*
   * Failed to turn on MSS AHB clock, stop.
   */
  if(!nQ6ACKTimeout)
  {
    PIL_LOG_ERR(1,"Couldnt start AHB clock, timeout over", 0); 
    return FALSE;
  }

  /*
   * Enable Q6 BIMC AXI clock.
   */
  HWIO_OUTF(GCC_MSS_Q6_BIMC_AXI_CBCR, CLK_ENABLE, 1);
    while((HWIO_INF(GCC_MSS_Q6_BIMC_AXI_CBCR, CLK_OFF) == 1) &&
          (nQ6ACKTimeout > 0))
  {
    pil_busywait(5);
    nQ6ACKTimeout--;
  }

  /*
   *  Turn gcc_mss_gpll0_clk_src ON. This is required for Apps to access MSS registers.
   *  Todo: Need to check this for Lykan
   */
  //HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, MSS_GPLL0_CLK_SRC_ENA, 1);

  /*
   * Restart Modem.
   */

  /*
   * Turn on XO clock. XO clock is required for BHS operation.
   */
   HWIO_OUTF(MSS_QDSP6SS_XO_CBCR, CLKEN, 0x1);
   while((HWIO_INF(MSS_QDSP6SS_XO_CBCR, CLKOFF) == 1) &&
          (nQ6ACKTimeout > 0))
   {
     pil_busywait(5);
     nQ6ACKTimeout--;
   }
   
  /*
   * Turn-on BHS, clamp QDSP6 custom memory word line, clamp QDSP6 core IO.
   */
  HWIO_OUT(MSS_QDSP6SS_PWR_CTL, HWIO_MSS_QDSP6SS_PWR_CTL_BHS_ON_BMSK |\
                                HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_QMC_MEM_BMSK|\
                                HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_WL_BMSK|\
                                HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_IO_BMSK);

  //Todo: Check this for Lykan..
  /*
   * Poll BHS status.
   */
  //nQ6ACKTimeout = 20000;
  //while((HWIO_INF(MSS_QDSP6SS_BHS_STATUS, BHS_EN_REST_ACK) == 0) &&
  //        (nQ6ACKTimeout > 0))
  //{
   // pil_busywait(1);
   // nQ6ACKTimeout--;
  //}
  
  pil_busywait(1);

  /* 
   * Put LDO in bypass mode
   */
  HWIO_OUTF(MSS_QDSP6SS_PWR_CTL, LDO_BYP, 1);

  /*
   * De-assert QDSP6 compiler memory clamp
   */
  HWIO_OUTF(MSS_QDSP6SS_PWR_CTL, CLAMP_QMC_MEM, 0);

  /*
   * De-assert memory peripheral sleep and L2 memory standby
   */
  HWIO_OUTM(MSS_QDSP6SS_PWR_CTL, HWIO_MSS_QDSP6SS_PWR_CTL_SLP_RET_N_BMSK |
            HWIO_MSS_QDSP6SS_PWR_CTL_ARR_STBY_N_BMSK,  HWIO_MSS_QDSP6SS_PWR_CTL_SLP_RET_N_BMSK |
            HWIO_MSS_QDSP6SS_PWR_CTL_ARR_STBY_N_BMSK);

  /*
   * Enable memories.Turn on QDSP6 memory footswitch/headswitch one bank at a time.
   */

  for (nBankIndex = 19; nBankIndex >= 6; nBankIndex --)
  {
    HWIO_OUTM(MSS_QDSP6SS_MEM_PWR_CTL, ( 1<< nBankIndex), ( 1<<nBankIndex));
    /* Read to make sure write is complete */
    HWIO_IN(MSS_QDSP6SS_MEM_PWR_CTL);
    pil_busywait(1);
  }

  for (nBankIndex = 0; nBankIndex < 6; nBankIndex ++)
  {
    HWIO_OUTM(MSS_QDSP6SS_MEM_PWR_CTL, ( 1<< nBankIndex), ( 1<<nBankIndex));
    
    /* Read to make sure write is complete */
    HWIO_IN(MSS_QDSP6SS_MEM_PWR_CTL);
    pil_busywait(1);
  }
  
  /*
   * Remove QDSP6 memory word line clamp.
   */
  HWIO_OUTF(MSS_QDSP6SS_PWR_CTL, CLAMP_WL, 0);
  /*
   * Remove QDSP6 I/O clamp.
   */
  HWIO_OUTF(MSS_QDSP6SS_PWR_CTL, CLAMP_IO, 0);

  /*
   * Release core out of reset.
   */
  HWIO_OUTF(MSS_QDSP6SS_RESET, BUS_ARES_ENA, 1 );
  HWIO_OUTF(MSS_QDSP6SS_RESET, STOP_CORE, 1 );
  HWIO_OUTF(MSS_QDSP6SS_RESET, CORE_ARES, 0 );

  pil_busywait(1);

  return TRUE;

} /* END Setup_MSSProcessor */

/*=============================================================================

FUNCTION CLOCK_SETUPPROCESSOR

DESCRIPTION
  This function Setsup clocks to bring processor out of reset

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Clock_SetupProcessor(ClockProcessorType eProcessorType)
{
  boolean bRetVal = FALSE;

  switch(eProcessorType)
  {
     case CLOCK_PROCESSOR_LPASS:
      break;
     case CLOCK_PROCESSOR_MODEM:
      bRetVal = Setup_MSSProcessor();
      break;
     case CLOCK_PROCESSOR_RPM:
      bRetVal = TRUE;
      break;

    default:
      break;
  }

  return bRetVal;

} /* END Clock_SetupProcessor */

/* Configures Halt Registers as part of Shutdown Sequence */
#define Clock_Set_Halt_Reg(reg) \
{ \
   uint32 val1, val2; \
   HWIO_OUT(reg##_HALTREQ, 1); \
   pil_busywait(100); \
   val1 = HWIO_IN(reg##_HALTACK); \
   val2 = HWIO_IN(reg##_MASTER_IDLE); \
   PIL_LOG_LOW(3,"reg values read: val1: %d, val2: %d", val1, val2, 0); \
   if(val1 == 0 || val2 == 0) \
     PIL_LOG_WARN(3,"halt didnt happen properly val1: %d, val2: %d, expected in some targets", val1, val2, 0); \
   HWIO_OUT(reg##_HALTREQ, 0); \
}

/*=============================================================================

FUNCTION SHUTDOWN_MSSPROCESSOR

DESCRIPTION
  This function shutsdown Modem QDSP6 Processor

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Shutdown_MSSProcessor(void)
{
  uint32 nQ6ACKTimeout = 20000;
  
  Clock_Set_Halt_Reg(TCSR_MSSQ6);
  Clock_Set_Halt_Reg(TCSR_MSS_MODEM);
  Clock_Set_Halt_Reg(TCSR_MSS_NC);

	/*
	 * Software workaround to avoid high MX current during LPASS/MSS
	 * restart.
	 */
  
  /*
   * Enable MSS AHB.
   */
  nQ6ACKTimeout = 20000;
  HWIO_OUTF(GCC_MSS_CFG_AHB_CBCR, CLK_ENABLE, 0x1);
  while((HWIO_INF(GCC_MSS_CFG_AHB_CBCR, CLK_OFF) == 1) &&
          (nQ6ACKTimeout > 0))
  {
    pil_busywait(5);
    nQ6ACKTimeout--;
  }

  /*
   * Failed to turn on MSS AHB clock, stop.
   */
  if(!nQ6ACKTimeout)
  {
    PIL_LOG_ERR(1,"Couldnt start AHB clk", 0);
    return FALSE;
  }
  
  /*
   * Turn-on clamp QDSP6 custom memory word line, clamp QDSP6 core IO.
   */
  HWIO_OUT(MSS_QDSP6SS_PWR_CTL, HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_QMC_MEM_BMSK|\
                                HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_WL_BMSK|\
                                HWIO_MSS_QDSP6SS_PWR_CTL_CLAMP_IO_BMSK);
  
  /* ensure clamps are asserted before setting restart reg */
  pil_os_mem_barrier();
  
  //Assert Restart Register
  HWIO_OUT(GCC_MSS_RESTART, 1);
  
  pil_os_mem_barrier();
  
  pil_busywait(5);
  
  //Turn Off the clocks
   /*
   * Disable Q6 BIMC AXI clock.
   */
  HWIO_OUTF(GCC_MSS_Q6_BIMC_AXI_CBCR, CLK_ENABLE, 0);

  /*
   * Disable MSS AHB.
   */
   //Todo: Check if we should wait for CLK_OFF status
  //nQ6ACKTimeout = 20000;
  HWIO_OUTF(GCC_MSS_CFG_AHB_CBCR, CLK_ENABLE, 0x0);
  //while((HWIO_INF(GCC_MSS_CFG_AHB_CBCR, CLK_OFF) == 0) &&
    //      (nQ6ACKTimeout > 0))
  //{
   // pil_busywait(5);
    //nQ6ACKTimeout--;
  //}

  /*
   * Failed to turn on MSS AHB clock, stop.
   */
  //if(!nQ6ACKTimeout)
  //{
    //Todo: Print error log
    //return FALSE;
  //}

  return TRUE;
}

/*=============================================================================

FUNCTION CLOCK_SHUTDOWNPROCESSOR

DESCRIPTION
  This function Setsup clocks to bring processor down

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If successful
  FALSE - On Failure

SIDE EFFECTS
  None.

=============================================================================*/
boolean Clock_ShutdownProcessor(ClockProcessorType eProcessorType)
{
  boolean bRetVal = FALSE;

  switch(eProcessorType)
  {
     case CLOCK_PROCESSOR_LPASS:
      break;
     case CLOCK_PROCESSOR_MODEM:
      bRetVal = Shutdown_MSSProcessor();
      break;
     case CLOCK_PROCESSOR_RPM:
      bRetVal = TRUE;
      break;

    default:
      break;
  }

  return bRetVal;

} /* END Clock_ShutdownProcessor */

