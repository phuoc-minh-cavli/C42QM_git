/* 
	Linker error observed in MCPM code while compiling the build for GPS-Only build flavor.
	Modem-lite builds will not include any RF/RFM/RFLM modules for compilation.
	Stubbing out the RFM/RFLM APIs.
*/

#include "comdef.h" 

/*! @brief 
    Enum of common LM API error return codes.
*/
typedef enum 
{
  RFLM_ERR_NONE=0,
  RFLM_ERR_IGNORED=1,  
  RFLM_ERR_API_FAILED=2,
  RFLM_ERR_API_TxAGC_ORPHAN=3,     ///< TxAGC update is called without a corresponding TxAGC calc
  RFLM_ERR_API_TXAGC_FIXUP_SKIP=4, ///< TxAGC update fixup is skipped due to insufficeint time
  RFLM_ERR_PAD= 0x7FFFFFFF
} rflm_err_t;

typedef struct
{
  uint32 mem_slp_cntl;    /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL */
  uint32 mem_slp_cntl2;  /*!< Retention fields in MSS_OFFLINE_MEM_SLP_CNTL2 */
  uint32 spare[2];              /*!< Future use */
} rfm_sleep_cfg_t;

typedef uint32 rflm_handle_rx_t;

/*==============================================================================

FUNCTION:  rflm_dtr_configure_plls

==============================================================================*/
rflm_err_t rflm_dtr_configure_plls( void )
{
   return RFLM_ERR_NONE;
}

/*==============================================================================

FUNCTION:  rflm_dtr_rx_vote_pll

==============================================================================*/
rflm_err_t rflm_dtr_rx_vote_pll( rflm_handle_rx_t handle,
                                 boolean vote_on )
{   
   return RFLM_ERR_NONE;
}

/*==============================================================================

FUNCTION:  rfm_get_mcpm_rfi_sleep_control

==============================================================================*/
void rfm_get_mcpm_rfi_sleep_control(rfm_sleep_cfg_t * sleepMask)
{
  return;
}

/*==============================================================================

FUNCTION:  rfm_ccs_wakeup

==============================================================================*/
void rfm_ccs_wakeup(void)
{
  return;
}

/*==============================================================================

FUNCTION:  rfm_ccs_sleep

==============================================================================*/
void rfm_ccs_sleep(void)
{
  return;
}

