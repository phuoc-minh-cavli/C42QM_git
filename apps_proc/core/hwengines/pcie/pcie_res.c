/**
 * @file pcie_res.c
 * @brief
 * Handles and defines types for pcie power and clock resources.
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "pcie_res.h"
#include "pcie_i.h"
#include "pcie_core.h"
#include "pcie_osal.h"
#include "pcie_tgt.h"
#include "DALDeviceId.h"
#include "DDIClock.h"

#ifndef  PCIE_BOOT
/* For NPA calls */
#include "pmapp_npa.h"
#include "npa.h"
/** For VMIDT restoration */
#include "tz_syscalls.h"
#include "syscalls.h"
#ifdef PCIE_TURN_OFF_LDO_IN_M2
#include "pcie_phy.h"
#endif /* PCIE_TURN_OFF_LDO_IN_M2 */
#endif /* PCIE_BOOT */

#define PCIE_GEN1_CLK_FREQ_HZ      62500000       /** PCIe GEN1 clock frequency */
#define PCIE_GEN2_CLK_FREQ_HZ     125000000       /** PCIe GEN2 clock frequency */

/** PM configuration of LDOs.
 * This enum entries from 0 till the last one should match the ones defined in pmapp_npa.h file.
 * Created a copy for easier naming within PCIe.
 */
typedef enum
{
   PCIE_NO_MODE_REQUESTED = -1,        /** < -- For init purpose only */
   PCIE_OFF = 0,                       /** < -- Completely powered down state */
   PCIE_SUSPEND,                       /** < -- Suspended state, but no VddMin */
   PCIE_ACTIVE_NOMINAL,                /** < -- Active mode with Nominal frequencies */
   PCIE_ACTIVE_SVS,                    /** < -- Active mode with SVS frequencies */
   PCIE_ACTIVE_TURBO,                  /** < -- Active mode with Turbo frequencies */
   PCIE_MAX,
}pcie_res_pmic_modes_type;

/** Power control object */
typedef struct {
   uint32 pwr_id;              /** < --  ID for the Power handle */
} pcie_res_pwr_ctl_type;

/** PCIe Clock Handles */
typedef struct {
   pcie_res_clock_ctl_type pcie_clk_ctl[PCIE_MAX_CLK];        /** < --  Clock handles for various PCIe Clocks */
} pcie_res_clk_handle_type;

#ifndef  PCIE_BOOT
/** Context for LDOs NPA modes */
typedef struct {
   npa_client_handle          pcie_res_pmic_client_handle;     /** < --  PCIe handle */
   pcie_res_pmic_modes_type   pcie_res_pmic_mode_requested;    /** < --  NPA mode requested */
} pcie_res_pmic_ctx_type;

/** Context for cx mx rails NPA Client handles */
typedef struct {
   npa_client_handle     pcie_cx_client;        /** < --  NPA Handle for the client for CX resource */
   npa_client_handle     pcie_mx_client;        /** < --  NPA Handle for the client for MX resource */
} pcie_res_rail_client_handle_type;

/** Context for clock NPA Client handles */
typedef struct {
   npa_client_handle     pcie_bus_snoc_npa_client;         /** < --  NPA Handle for the client for SNOC resource */
   npa_client_handle     pcie_bus_pcnoc_npa_client;        /** < --  NPA Handle for the client for PCNOC resource */
   npa_client_handle     pcie_bus_bimc_npa_client;         /** < --  NPA Handle for the client for BIMC resource */
} pcie_res_clk_client_handle_type;

/** Context for PCIe NPA handles */
typedef struct {
   pcie_res_pmic_ctx_type              pcie_res_pmic_vregmgr;        /** < -- NPA Handle for the client for LDOs */
   pcie_res_clk_client_handle_type     pcie_res_clk_client_handle;   /** < -- NPA Handle for the client for other resources */
   pcie_res_rail_client_handle_type    pcie_res_rails_client_handle; /** < -- NPA Handle for the client for CX MX rails */
} pcie_res_npa_ctx_type;
#endif /* PCIE_BOOT */

/** PCIe Resource Context Structure */
typedef struct _pcie_res_ctx_type
{
   DalDeviceHandle *hpcieOsalDALClockPwr;             /** DAL handle for Clock and Power */
#ifndef  PCIE_BOOT
   pcie_res_npa_ctx_type pcie_res_npa;                /** PCIe NPA context */
#endif /* PCIE_BOOT */
   pcie_res_clk_handle_type pcie_res_clk_handle;      /** PCIe Clock Handles */
   pcie_res_pwr_ctl_type pcie_res_pwr_ctl;            /** PCIe Power Control Handle */
   pcie_tgtcfg_type *tgt_cfg;                         /** PCIe Target Configuration */
}pcie_res_ctx_type;

/** PCIe Resource Context */
static pcie_res_ctx_type *pcie_res_ctx;

/** Prototypes */

/* Initializes the PCIe Clocks */
static void pcie_resi_clk_init(void);

/* Init the PCIe Core power domain with system  driver */
static void pcie_resi_init_pwr_domain(void);

/* Enable the PCIe Core power domain */
static void pcie_resi_enable_pwr_domain(void);

/* Disable the PCIe Core power domain */
static void pcie_resi_disable_pwr_domain(void);

/* Initialize the PMIC NPA client by registering it with the resources */
static void pcie_resi_pmic_client_init(void);

/* Votes for requested power mode configurations for various LDOs */
static void pcie_resi_npa_pmic_mode_request(pcie_res_pmic_modes_type pcie_res_npa_pmic_mode);

/* Initialize the clk NPA client by registering it with the resources */
static void pcie_resi_npa_clk_client_init(void);

/* Vote for requested clk frequency for CNOC */
static void pcie_resi_npa_clk_mode_request(void);

/* Takes the votes back for CNOC */
static void pcie_resi_npa_clk_mode_release(void);

/* Initialize the cx mx rails NPA client by registering it with the resources */
static void pcie_resi_npa_rails_client_init(void);

/* Vote for requested frequency for CX MX rails */
static void pcie_resi_npa_rails_mode_request(void);

/* Takes the votes back for CX MX rails */
static void pcie_resi_npa_rails_mode_release(void);

#ifndef PCIE_BOOT
/* Initializes a clock control object */
static pcie_return_status pcie_resi_clock_ctl_init(pcie_res_clk_id_type id);

/* Turn on clock */
static pcie_return_status pcie_resi_clock_ctl_enable(pcie_res_clk_id_type id);

/* Selects an external source for a clock */
static pcie_return_status pcie_resi_select_extern_source(pcie_res_clk_id_type id, uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D);

/* Initializes a power domain control object */
static pcie_return_status pcie_resi_pwr_domain_init(pcie_res_pwr_ctl_type *pwr_ctl, char *pwr_name);

/* Enable the power domain */
static pcie_return_status pcie_resi_pwr_domain_enable(pcie_res_pwr_ctl_type *pwr_ctl);
#endif /* PCIE_BOOT */

/* Turn off clock */
static pcie_return_status pcie_resi_clock_ctl_disable(pcie_res_clk_id_type id);

/* Disable power domain */
static pcie_return_status pcie_resi_pwr_domain_disable(pcie_res_pwr_ctl_type *pwr_ctl);

/* ============================================================================
**  Function : pcie_resi_clk_init
** ============================================================================
*/
/**
 * Initializes the PCIe Clocks.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_clk_init(void)
{
#ifndef  PCIE_BOOT
   uint32 id;

   pcie_osal_debug(MSG_HIGH, "PCIe clocks init begin");


   for(id=0;id<PCIE_MAX_CLK;id++)
   {
      pcie_resi_clock_ctl_init((pcie_res_clk_id_type)id);
   }

   /* Turn off clocks that are not needed. Enable them first
    * so clock driver can register they are on */
   pcie_resi_clock_ctl_enable(PCIE_LDO_CLK);
   pcie_resi_clock_ctl_disable(PCIE_LDO_CLK);

   /* Set the frequency of the AUX clock */
   pcie_res_set_clock_freq(PCIE_AUX_CLK, pcie_res_ctx->tgt_cfg->aux_clk_freq_hz, PCIE_CLOCK_FREQUENCY_HZ_EXACT);

   pcie_osal_debug(MSG_HIGH, "PCIe clocks init end");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_res_init_pwr_domain
** ============================================================================
*/
/**
 * Init the PCIe Core power domain with system  driver.
 *
 * @param[in]   void
 *
 * @return      void
 */
static void pcie_resi_init_pwr_domain(void)
{
#ifndef  PCIE_BOOT
   char* pcie_power_domain_name;

   pcie_power_domain_name = pcie_res_ctx->tgt_cfg->pcie_power_domain_name;

   pcie_osal_debug(MSG_HIGH, "PCIe initializing power domain");
   pcie_resi_pwr_domain_init(&pcie_res_ctx->pcie_res_pwr_ctl, pcie_power_domain_name);
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_enable_pwr_domain
** ============================================================================
*/
/**
 * Enable the PCIe Core power domain
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_enable_pwr_domain(void)
{
#ifndef PCIE_BOOT
   pcie_osal_debug(MSG_HIGH, "PCIe enabling power domain");
   pcie_resi_pwr_domain_enable(&pcie_res_ctx->pcie_res_pwr_ctl);

   /* Disable reset for PHY */
   DalClock_ResetClock(pcie_res_ctx->hpcieOsalDALClockPwr,pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[PCIE_PHY_CLK].clock_id, CLOCK_RESET_PULSE); 
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_disable_pwr_domain
** ============================================================================
*/
/**
 * Disable the PCIe Core power domain
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_disable_pwr_domain(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe disabling power domain");
   pcie_resi_pwr_domain_disable(&pcie_res_ctx->pcie_res_pwr_ctl);
}

/* ============================================================================
**  Function : pcie_resi_pmic_client_init
** ============================================================================
*/
/**
 * Initialize the PMIC NPA client by registering it with the resources.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_pmic_client_init(void)
{
#ifndef  PCIE_BOOT
   pcie_res_pmic_ctx_type *ctx = &pcie_res_ctx->pcie_res_npa.pcie_res_pmic_vregmgr;

   /* Create a synchronous request client to the PCIe PMIC resource */
   ctx->pcie_res_pmic_client_handle = npa_create_sync_client(PMIC_NPA_GROUP_ID_PCIE, "pcie_pmic_npa_client", NPA_CLIENT_REQUIRED);

   /* Set the pcie_pmic_npa_mode_requested to PCIE_NO_MODE_REQUESTED to know that no mode has been voted for yet  */
   ctx->pcie_res_pmic_mode_requested = PCIE_NO_MODE_REQUESTED;

   if(NULL == ctx->pcie_res_pmic_client_handle)
   {
      pcie_osal_debug(MSG_FATAL, "PMIC NPA client registration failed");
   }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_res_npa_pmic_mode_request
** ============================================================================
*/
/**
 * Votes for requested power mode configurations for various LDOs
 *
 * @param[in]  pcie_res_npa_pmic_mode  The requested power mode configuration
 *
 * @return     void
 */
static void pcie_resi_npa_pmic_mode_request(pcie_res_pmic_modes_type pcie_res_npa_pmic_mode)
{
#ifndef  PCIE_BOOT
   pcie_res_pmic_ctx_type *ctx = &pcie_res_ctx->pcie_res_npa.pcie_res_pmic_vregmgr;

   if(NULL == ctx->pcie_res_pmic_client_handle)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_vregmgr->pcie_handle is null");
   }

   if(PCIE_OFF == pcie_res_npa_pmic_mode)
   {
      npa_set_request_attribute(ctx->pcie_res_pmic_client_handle,
                                NPA_REQUEST_FIRE_AND_FORGET);
   }
   else
   {
      npa_set_request_attribute(ctx->pcie_res_pmic_client_handle,
                                NPA_REQUEST_DEFAULT);
   }

   /* Vote only if the requested vote is different from the existing vote */
   if(ctx->pcie_res_pmic_mode_requested != pcie_res_npa_pmic_mode)
   {
      if(PCIE_OFF == pcie_res_npa_pmic_mode)
      {
         /* vote for PCIE_OFF  */
         npa_issue_required_request(ctx->pcie_res_pmic_client_handle, PMIC_NPA_MODE_ID_PCIE_OFF);
         ctx->pcie_res_pmic_mode_requested = pcie_res_npa_pmic_mode;
         pcie_osal_debug(MSG_HIGH, "PMIC_NPA_MODE_ID_PCIE_OFF mode requested");
      }
      else if(PCIE_SUSPEND == pcie_res_npa_pmic_mode)
      {
         /* vote for PCIE_SUSPEND  */
         npa_issue_required_request(ctx->pcie_res_pmic_client_handle, PMIC_NPA_MODE_ID_PCIE_SUSPEND);
         ctx->pcie_res_pmic_mode_requested = pcie_res_npa_pmic_mode;
         pcie_osal_debug(MSG_HIGH, "PMIC_NPA_MODE_ID_PCIE_SUSPEND mode requested");
      }
      else if(PCIE_ACTIVE_NOMINAL == pcie_res_npa_pmic_mode)
      {
         /* vote for PCIE_ACTIVE_NOMINAL  */
         npa_issue_required_request(ctx->pcie_res_pmic_client_handle, PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL);
         ctx->pcie_res_pmic_mode_requested = pcie_res_npa_pmic_mode;
         pcie_osal_debug(MSG_HIGH, "PMIC_NPA_MODE_ID_PCIE_ACTIVE_NOMINAL mode requested");
      }
      else if(PCIE_ACTIVE_SVS == pcie_res_npa_pmic_mode)
      {
         /* vote for PCIE_ACTIVE_SVS  */
         npa_issue_required_request(ctx->pcie_res_pmic_client_handle, PMIC_NPA_MODE_ID_PCIE_ACTIVE_SVS);
         ctx->pcie_res_pmic_mode_requested = pcie_res_npa_pmic_mode;
         pcie_osal_debug(MSG_HIGH, "PMIC_NPA_MODE_ID_PCIE_ACTIVE_SVS mode requested");
      }
      else if(PCIE_ACTIVE_TURBO == pcie_res_npa_pmic_mode)
      {
         /* vote for PCIE_ACTIVE_TURBO  */
         npa_issue_required_request(ctx->pcie_res_pmic_client_handle, PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO);
         ctx->pcie_res_pmic_mode_requested = pcie_res_npa_pmic_mode;
         pcie_osal_debug(MSG_HIGH, "PMIC_NPA_MODE_ID_PCIE_ACTIVE_TURBO mode requested");
      }
      else if(PCIE_MAX == pcie_res_npa_pmic_mode)
      {
         /* vote for PCIE_MAX  */
         npa_issue_required_request(ctx->pcie_res_pmic_client_handle, PMIC_NPA_MODE_ID_PCIE_MAX);
         ctx->pcie_res_pmic_mode_requested = pcie_res_npa_pmic_mode;
         pcie_osal_debug(MSG_HIGH, "PMIC_NPA_MODE_ID_PCIE_MAX mode requested");
      }
      else
      {
         pcie_osal_debug(MSG_ERR, "Unsupported PMIC NPA mode requested");
      }
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "Already voted for %d", pcie_res_npa_pmic_mode);
   }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_npa_clk_client_init
** ============================================================================
*/
/**
 * Initialize the clk NPA client by registering it with the resources.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_npa_clk_client_init(void)
{
#ifndef  PCIE_BOOT
   /* Create a synchronous request client to the CNOC resource */
   pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_pcnoc_npa_client = npa_create_sync_client(pcie_dev->pcnoc.res_name,pcie_dev->pcnoc.client_name, NPA_CLIENT_REQUIRED);

   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_pcnoc_npa_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_bus_pcnoc_npa_client registration failed");
   }

   /* Create a synchronous request client to the SNOC resource */
   pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_snoc_npa_client = npa_create_sync_client(pcie_dev->snoc.res_name, pcie_dev->snoc.client_name, NPA_CLIENT_REQUIRED);

   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_snoc_npa_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_bus_snoc_npa_client registration failed");
   }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_npa_clk_mode_request
** ============================================================================
*/
/**
 * Vote for requested clk frequency for CNOC
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_npa_clk_mode_request(void)
{
#ifndef  PCIE_BOOT
   /* Vote for CNOC @ SVS */
   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_pcnoc_npa_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_bus_pcnoc_npa_client is null");
   }

   npa_issue_required_request(pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_pcnoc_npa_client, pcie_dev->pcnoc.freq_vote);
   pcie_osal_debug(MSG_HIGH, "PCIE_PCNOC_SVS_MODE_SPEED_KHZ requested");

   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_snoc_npa_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_bus_snoc_npa_client is null");
   }

   npa_issue_required_request(pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_snoc_npa_client, pcie_dev->snoc.freq_vote);
   pcie_osal_debug(MSG_HIGH, "PCIE_SNOC_SVS_MODE_SPEED_KHZ mode requested");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_npa_clk_mode_release
** ============================================================================
*/
/**
 * Takes the votes back for CNOC
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_npa_clk_mode_release(void)
{
#ifndef  PCIE_BOOT
   /* Remove votes for the CNOC */
   npa_complete_request(pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_pcnoc_npa_client);
   pcie_osal_debug(MSG_HIGH, "pcie_bus_pcnoc_npa_client release complete");

   npa_complete_request(pcie_res_ctx->pcie_res_npa.pcie_res_clk_client_handle.pcie_bus_snoc_npa_client);
   pcie_osal_debug(MSG_HIGH, "pcie_bus_snoc_npa_client request complete");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_npa_rails_client_init
** ============================================================================
*/
/**
 * Initialize the CX MX rails NPA client by registering it with the resources.
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_npa_rails_client_init(void)
{
#ifndef  PCIE_BOOT
   /* Create a synchronous request client to the CX rail */
   pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_cx_client = npa_create_sync_client(pcie_dev->cx.res_name,pcie_dev->cx.client_name, NPA_CLIENT_REQUIRED);

   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_cx_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_cx_client registration failed");
   }

   /* Create a synchronous request client to the MX rail */
   pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_mx_client = npa_create_sync_client(pcie_dev->mx.res_name,pcie_dev->mx.client_name, NPA_CLIENT_REQUIRED);

   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_mx_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_mx_client registration failed");
   }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_resi_npa_rails_mode_request
** ============================================================================
*/
/**
 * Vote for requested frequency for CX MX rails
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_npa_rails_mode_request(void)
{
#ifndef  PCIE_BOOT
   /* Vote for CX */
   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_cx_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_cx_client is null");
   }

   npa_issue_required_request(pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_cx_client, pcie_dev->cx.vote);
   pcie_osal_debug(MSG_HIGH, "PCIE_CX_RAIL_VCS_CORNER_LOW requested");

   /* Vote for MX */
   if(NULL == pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_mx_client)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_mx_client is null");
   }

   npa_issue_required_request(pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_mx_client, pcie_dev->mx.vote);
   pcie_osal_debug(MSG_HIGH, "PCIE_MX_RAIL_VCS_CORNER_LOW requested");
#endif /* PCIE_BOOT */
}

/* Takes the votes back for CX MX rails */
static void pcie_resi_npa_rails_mode_release(void);
/* ============================================================================
**  Function : pcie_resi_npa_rails_mode_release
** ============================================================================
*/
/**
 * Takes the votes back for CX MX rails
 *
 * @param[in]  void
 *
 * @return     void
 */
static void pcie_resi_npa_rails_mode_release(void)
{
#ifndef  PCIE_BOOT
   /* Remove votes for the CNOC */
   npa_cancel_request(pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_cx_client);
   pcie_osal_debug(MSG_HIGH, "pcie_cx_client cancel request complete");

   npa_cancel_request(pcie_res_ctx->pcie_res_npa.pcie_res_rails_client_handle.pcie_mx_client);
   pcie_osal_debug(MSG_HIGH, "pcie_mx_client cancel request complete");
#endif /* PCIE_BOOT */
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_resi_clock_ctl_init
** ============================================================================
*/
/**
 * Initializes a clock control object.
 *
 * @param[in]     id     ID for the PCIe clock
 *
 * @return        pcie_return_status
 *    Success of the operation.
 */
static pcie_return_status pcie_resi_clock_ctl_init(pcie_res_clk_id_type id)
{
   DALResult           dal_result;
   pcie_res_clk_names_type *pcie_res_clk_name_ptr = NULL;

   pcie_res_clk_name_ptr = &pcie_res_ctx->tgt_cfg->pcie_clk_string;

   if(!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_GetClockId(pcie_res_ctx->hpcieOsalDALClockPwr,
                                    pcie_res_clk_name_ptr->pcie_clk_name[id],
                                    &pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_GetClockId failed for Clock id %d",id);
   }

   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_res_boot_clk_enable
** ============================================================================
*/
/**
 * Enable PCIe boot Clock.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_boot_clk_enable(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe enabling boot clock");
   HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK, 0X1);
}

/* ============================================================================
**  Function : pcie_res_boot_clk_disable
** ============================================================================
*/
/**
 * Disable PCIe boot Clock.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_boot_clk_disable(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe disabling boot clock");
   HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK, 0X0);
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_resi_clock_ctl_enable
** ============================================================================
*/
/**
 * Turn on clock.
 *
 * @param[in]      id     ID for the PCIe Clock
 *
 * @return         pcie_return_status
 *    Success of the operation.
 */
static pcie_return_status pcie_resi_clock_ctl_enable(pcie_res_clk_id_type id)
{
   DALResult           dal_result;

   if(!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])
   {
      pcie_osal_debug(MSG_FATAL, "pcie_res_clk_handle.pcie_clk_ctl is NULL for Clock ID %d",id);
      return(PCIE_FAILED);
   }

   dal_result = DalClock_EnableClock(pcie_res_ctx->hpcieOsalDALClockPwr, pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_EnableClock failed for Clock ID %d",id);
   }
   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */
/* ============================================================================
**  Function : pcie_resi_clock_ctl_disable
** ============================================================================
*/
/**
 * Turn off clock.
 *
 * @param[in]      id     ID for the PCIe Clock
 *
 * @return         pcie_return_status
 *    Success of the operation.
 */
static pcie_return_status pcie_resi_clock_ctl_disable(pcie_res_clk_id_type id)
{
   DALResult           dal_result;

   if(!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_DisableClock(pcie_res_ctx->hpcieOsalDALClockPwr, pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_DisableClock failed for Clock ID %d",id);
   }

   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_res_set_clock_freq
** ============================================================================
*/
/**
 * Set Clock Frequency.
 *
 * @param[in] id            ID for the PCIe Clock
 * @param[in] nFreq         Clock frequency in Hz, kHz, or MHz, depending on the
 * eMatch parameter.
 * @param[in] eMatch        Type of match request.
 *
 * @return     pcie_return_status
 *    Success of the operation.
 */
pcie_return_status pcie_res_set_clock_freq(pcie_res_clk_id_type id, uint32  nFreq,  pcie_res_clockfreq_type  eMatch)
{
#ifndef PCIE_BOOT
   uint32 result_freq;
   DALResult           dal_result;

   if(!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_SetClockFrequency(pcie_res_ctx->hpcieOsalDALClockPwr, pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id, nFreq, (ClockFrequencyType)eMatch, &result_freq);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_SetClockFrequency failed for Clock id %d",id);
   }
#endif /* PCIE_BOOT */
   return(PCIE_SUCCESS);
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_resi_select_extern_source
** ============================================================================
*/
/**
 * Selects an external source for a clock.
 *
 * @param[in] id         ID for the PCIe Clock
 * @param[in] nFreqHz    Source frequency in Hz. The value '0' indicates that voltage scaling is not expected for this source.
 * @param[in] nSource    Source to use. This value should match the MUX input on which the external source is connected.
 * @param[in] nDiv2x     Integer divider to use (2x the desired divider).
 * @param[in] nM         The M value for any M/N counter (0 to bypass). Not all clock domains have M/N counters; these values will be ignored in such cases.
 * @param[in] nN         The N value for any M/N counter.
 * @param[in] n2D        Twice the D value for any M/N counter. The raw value
 *                       is doubled to allow for half-integer duty cycles.
 *
 * @return    pcie_return_status
 *    Success of the operation.
 *
 * @note If the specified clock does not support the requested external source, this request selects the first source (in most cases XO) available to the clock.
 */
static pcie_return_status pcie_resi_select_extern_source(pcie_res_clk_id_type id, uint32 nFreqHz, uint32 nSource, uint32 nDiv2x, uint32 nM, uint32 nN, uint32 n2D)
{
   DALResult           dal_result;

   if(!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_SelectExternalSource(pcie_res_ctx->hpcieOsalDALClockPwr,
                                              pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id,
                                              nFreqHz, nSource, nDiv2x, nM, nN, n2D);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_SelectExternalSource failed for Clock ID %d",id);
   }
   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_resi_pwr_domain_init
** ============================================================================
*/
/**
 * Initializes a power domain control object.
 *
 * @param[in,out] pwr_ctl     Pointer to the object to be used for clock control
 * @param[in,out] pwr_name      Pointer to the object to identify the clock
 *
 * @return        pcie_return_status
 *    Success of the operation.
 */
static pcie_return_status pcie_resi_pwr_domain_init(pcie_res_pwr_ctl_type *pwr_ctl, char *pwr_name)
{
   DALResult           dal_result;

   if(!pwr_ctl)
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_GetPowerDomainId(pcie_res_ctx->hpcieOsalDALClockPwr, (char*)pwr_name, &pwr_ctl->pwr_id);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_GetPowerDomainId failed");
   }

   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_resi_pwr_domain_enable
** ============================================================================
*/
/**
 * Enable the power domain.
 *
 * @param[in,out]  pwr_ctl    clock control object to be used
 *
 * @return         pcie_return_status
 *    Success of the operation.
 */
static pcie_return_status pcie_resi_pwr_domain_enable(pcie_res_pwr_ctl_type *pwr_ctl)
{
   DALResult           dal_result;

   if(!pwr_ctl)
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_EnablePowerDomain(pcie_res_ctx->hpcieOsalDALClockPwr, pwr_ctl->pwr_id);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_EnablePowerDomain failed");
   }

   return(PCIE_SUCCESS);
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_resi_pwr_domain_disable
** ============================================================================
*/
/**
 * Disable power domain.
 *
 * @param[in,out]  pwr_ctl    clock control object to be used
 *
 * @return         pcie_return_status
 *    Success of the operation.
 */
static pcie_return_status pcie_resi_pwr_domain_disable(pcie_res_pwr_ctl_type *pwr_ctl)
{
   DALResult           dal_result;

   if(!pwr_ctl)
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_DisablePowerDomain(pcie_res_ctx->hpcieOsalDALClockPwr, pwr_ctl->pwr_id);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_DisablePowerDomain failed");
   }
   return(PCIE_SUCCESS);
}

/* ============================================================================
**  Function : pcie_res_init
** ============================================================================
*/
/**
 * Initialize PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_init(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe resources init begin");

   pcie_res_ctx = (pcie_res_ctx_type*)pcie_osal_malloc(sizeof(pcie_res_ctx_type));
   if(NULL ==  pcie_res_ctx)
   {
      pcie_osal_debug(MSG_FATAL, "Couldn't malloc memregion for pcie_res_ctx");
      return;
   }

   pcie_osal_memset(pcie_res_ctx, 0, sizeof(pcie_res_ctx_type));

   pcie_res_ctx->tgt_cfg = pcie_tgt_get_tgt_config();

#ifndef  PCIE_BOOT
   /* Attach the DAL handle for Clock and Power */
   DAL_DeviceAttach(DALDEVICEID_CLOCK, &pcie_res_ctx->hpcieOsalDALClockPwr);
#endif /* PCIE_BOOT */

   /* Initialize the PCIe Power domain */
   pcie_resi_init_pwr_domain();

   /* Initialize the PMIC NPA client */
   pcie_resi_pmic_client_init();

   /* Initialize the PCIe Clocks */
   pcie_resi_clk_init();

   /* Initialize the CLK NPA client */
   pcie_resi_npa_clk_client_init();
   
   /* Initialize the CX MX rails NPA client */
   pcie_resi_npa_rails_client_init();

   pcie_osal_debug(MSG_HIGH, "PCIe resources init end");
}

/* ============================================================================
**  Function : pcie_res_enable
** ============================================================================
*/
/**
 * Enable PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_enable(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe resources enable begin");

   /* Vote for PCIE_ACTIVE_SVS mode */
   pcie_resi_npa_pmic_mode_request(PCIE_ACTIVE_SVS);

   /* Enable the PCIe Power domain */
   pcie_resi_enable_pwr_domain();

   /* Vote for CNOC SVS frequency */
   pcie_resi_npa_clk_mode_request();

   /* Vote for CX MX rails */
   pcie_resi_npa_rails_mode_request();

   /* Initialize the PCIe Clocks */
   pcie_res_clk_enable();

   pcie_osal_debug(MSG_HIGH, "PCIe resources enable end");
}

/* ============================================================================
**  Function : pcie_res_disable
** ============================================================================
*/
/**
 * Disable PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_disable(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe resources disable begin");
   /*Disable PCIe Clocks */
   pcie_res_clk_disable();

   /*Disable PCIe Power domain */
   pcie_resi_disable_pwr_domain();

   /* Vote for PCIE_OFF mode */
   pcie_resi_npa_pmic_mode_request(PCIE_OFF);

   /* Remove vote for CNOC frequency */
   pcie_resi_npa_clk_mode_release();

   /* Remove vote for CX MX rails */
   pcie_resi_npa_rails_mode_release();

   pcie_osal_debug(MSG_HIGH, "PCIe resources disable end");
}

/* ============================================================================
**  Function : pcie_res_suspend
** ============================================================================
*/
/**
 * Suspend votes for PCIe power and clocks.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_suspend(void)
{
#ifndef PCIE_BOOT
   pcie_osal_debug(MSG_HIGH, "PCIe resources suspend begin");

#if defined(PCIE_CLKREQ_INTR_SUPPORTED) && !defined(PCIE_TURN_OFF_LDO_IN_M2)
   /* Vote for PCIE_SUSPEND mode */
   pcie_resi_npa_pmic_mode_request(PCIE_SUSPEND);
#else
   /* Vote for PCIE_OFF mode */
   pcie_resi_npa_pmic_mode_request(PCIE_OFF);
#endif

   /* Remove vote for CNOC frequency */
   pcie_resi_npa_clk_mode_release();

   /* Remove vote for CX MX rails */
   pcie_resi_npa_rails_mode_release();

   pcie_osal_debug(MSG_HIGH, "PCIe resources suspend end");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_res_resume
** ============================================================================
*/
/**
 * Resume vote for PCIe clock and power resources.
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_resume(void)
{
#ifndef PCIE_BOOT
   pcie_osal_debug(MSG_HIGH, "PCIe resources resume begin");
   /* Vote for PCIE_ACTIVE_SVS mode */
   pcie_resi_npa_pmic_mode_request(PCIE_ACTIVE_SVS);

   /* Vote for CNOC SVS frequency */
   pcie_resi_npa_clk_mode_request();

   /* Vote for CX MX rails */
   pcie_resi_npa_rails_mode_request();

   pcie_osal_debug(MSG_HIGH, "PCIe resources resume end");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_res_clk_enable
** ============================================================================
*/
/**
 * Enable PCIe Core Clocks.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_clk_enable(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe clocks enable begin");
   if(!pcie_dev->pcie_clocks_on)
   {
#ifndef PCIE_BOOT
      pcie_resi_clock_ctl_enable(PCIE_CONFIG_CLK);

#ifdef PCIE_TURN_OFF_LDO_IN_M2
      pcie_phy_unclamp_interfaces();
#endif
      pcie_resi_clock_ctl_enable(PCIE_INTERFACE_MSTR_CLK);
      pcie_resi_clock_ctl_enable(PCIE_INTERFACE_SLV_CLK);
      pcie_resi_clock_ctl_enable(PCIE_PIPE_CLK);
      pcie_resi_clock_ctl_enable(PCIE_AUX_CLK);
      pcie_res_boot_clk_disable();
#else
      pcie_res_boot_clk_enable();
#endif /* PCIE_BOOT */
      pcie_dev->pcie_clocks_on = TRUE;
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "PCIe clocks already on");
   }
   pcie_osal_debug(MSG_HIGH, "PCIe clocks enable end");
}

/* ============================================================================
**  Function : pcie_res_clk_disable
** ============================================================================
*/
/**
 * Disable the PCIe Core clock
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_clk_disable(void)
{
   pcie_osal_debug(MSG_HIGH, "PCIe clock disable begin");
   if(pcie_dev->pcie_clocks_on)
   {
      pcie_resi_clock_ctl_disable(PCIE_AUX_CLK);
      pcie_resi_clock_ctl_disable(PCIE_PIPE_CLK);
      pcie_resi_clock_ctl_disable(PCIE_INTERFACE_SLV_CLK);

      /* Do the clamping of PHY after Sleep clk is disabled to prevent PHY Wake up */
#ifdef PCIE_TURN_OFF_LDO_IN_M2
      pcie_phy_clamp_interfaces();
#endif

      pcie_resi_clock_ctl_disable(PCIE_INTERFACE_MSTR_CLK);
      pcie_resi_clock_ctl_disable(PCIE_CONFIG_CLK);
      pcie_dev->pcie_clocks_on = FALSE;
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "PCIe clocks already Off");
   }
   pcie_osal_debug(MSG_HIGH, "PCIe clocks disable end");
}

#ifdef PCIE_AHB_AXI_CLK_CHECK
/* ============================================================================
**  Function : pcie_res_get_interface_clk_state
** ============================================================================
*/
/**
 * Checks if either AHB or AXI clock is off
 *
 * @param[in]  void
 *
 * @return     boolean
 *    Returns 1 if either of them is off else returns 0.
 */
boolean pcie_res_get_interface_clk_state(void)
{
   if((!pcie_res_is_clock_ctl_on(PCIE_CONFIG_CLK)) || (!pcie_res_is_clock_ctl_on(PCIE_INTERFACE_SLV_CLK)) || (!pcie_res_is_clock_ctl_on(PCIE_INTERFACE_MSTR_CLK)))
   {
      pcie_osal_debug(MSG_HIGH, "Either AHB or AXI clock is Off");
      return(1);
   }
   else
   {
      pcie_osal_debug(MSG_HIGH, "AHB and AXI clocks are On");
      return(0);
   }
}
#endif

/* ============================================================================
**  Function : pcie_res_clk_select_extern_src
** ============================================================================
*/
/**
 * Selects the external clk source based on the tgt speed.
 *
 * @param[in]  void
 *
 * @return     void
 */
void pcie_res_clk_select_extern_src(void)
{
#ifndef  PCIE_BOOT
   pcie_targetspeed_type pcie_tgt_speed;
   pcie_tgt_speed = PCIE_CORE_TARGETSPEED;

   if(PCIE_CORE_TARGETSPEED_GEN2 == pcie_tgt_speed)
   {
      pcie_resi_select_extern_source(PCIE_PIPE_CLK, PCIE_GEN2_CLK_FREQ_HZ, 0, 2, 0, 0, 0);
   }
   else if(PCIE_CORE_TARGETSPEED_GEN1 == pcie_tgt_speed)
   {
      pcie_resi_select_extern_source(PCIE_PIPE_CLK, PCIE_GEN1_CLK_FREQ_HZ, 0, 2, 0, 0, 0);
   }
   else
   {
      pcie_osal_debug(MSG_FATAL, "Target speed 0x%x not supported", pcie_tgt_speed);
   }
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_res_is_clock_ctl_on
** ============================================================================
*/
/**
 * Checks if the clock is on.
 *
 * @param[in]      id         ID for the PCIe Clock
 *
 * @return         boolean
 *    True if the clock is on else returns false.
 */
boolean pcie_res_is_clock_ctl_on(pcie_res_clk_id_type id)
{
   boolean      dal_result;

   if(!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])
   {
      return(PCIE_FAILED);
   }

   dal_result = DalClock_IsClockOn(pcie_res_ctx->hpcieOsalDALClockPwr, pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id);

   return(dal_result);
}

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_res_get_clock_freq
** ============================================================================
*/
/**
 * Get Clock Frequency.
 *
 * @param[in] id            ID for the PCIe Clock
 * @param[in] nFreq         Pointer to Clock frequency in Hz
 *
 * @return    void
 */
void pcie_res_get_clock_freq(pcie_res_clk_id_type id, uint32* nFreq)
{
   DALResult           dal_result;

   if((!&pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id])|| (!nFreq))
   {
      pcie_osal_debug(MSG_FATAL, "Either clock_ctl or nFreq is NULL");
   }

   dal_result = DalClock_GetClockFrequency(pcie_res_ctx->hpcieOsalDALClockPwr, pcie_res_ctx->pcie_res_clk_handle.pcie_clk_ctl[id].clock_id,nFreq);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalClock_GetClockFrequency failed");
   }
}
#endif /* PCIE_BOOT */

/* ============================================================================
**  Function : pcie_res_restore_tz_sys_call
** ============================================================================
*/
/**
 * Restore system call to TZ
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_restore_tz_sys_call(void)
{
#ifndef PCIE_BOOT
   pcie_osal_debug(MSG_HIGH, "Restoring TZ system calls");
   if(0 != scm_sec_restore_syscall(TZ_DEVICE_PCIE_20))
   {
      pcie_osal_debug(MSG_FATAL, "TZ security restore failed for TZ_DEVICE_PCIE_20");
   }
   pcie_osal_debug(MSG_HIGH, "Restored TZ system calls");
#endif /* PCIE_BOOT */
}

/* ============================================================================
**  Function : pcie_res_tz_lock_regions
** ============================================================================
*/
/**
 * System call to TZ to lock regions specified
 *
 * @param[in]      offset   Start of the region to be locked
 * @param[in]      size     Size of the region
 *
 * @return         void
 */
void pcie_res_tz_lock_regions(uint32 offset, uint32 size)
{
#ifdef PCIE_PERIPH_MEM_LOCK_ENABLED
   if(0 != scm_sec_protect_periph_memory(TZ_PCIE_20_CLNT_PCIE_DRIVER, offset, size, TRUE))
   {
      pcie_osal_debug(MSG_FATAL, "Failed to lock unused region");
   }
#endif /* PCIE_PERIPH_MEM_LOCK_ENABLED */
}

/* ============================================================================
**  Function : pcie_res_tz_unlock_regions
** ============================================================================
*/
/**
 * System call to TZ to unlock previously locked regions
 *
 * @param[in]      void
 *
 * @return         void
 */
void pcie_res_tz_unlock_regions(void)
{
#ifdef PCIE_PERIPH_MEM_LOCK_ENABLED
   if(0 != scm_sec_protect_periph_memory(TZ_PCIE_20_CLNT_PCIE_DRIVER, 0, 0, FALSE))
   {
      pcie_osal_debug(MSG_FATAL, "Unable to unlock previously locked regions");
   }
#endif /* PCIE_PERIPH_MEM_LOCK_ENABLED */
}
