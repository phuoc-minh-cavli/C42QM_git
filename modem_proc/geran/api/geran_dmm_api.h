/*****************************************************************************
***
*** TITLE
***
*** 
***
***
*** DESCRIPTION
***
***  
*** 
*** Copyright (c) 2001-2017 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10/12/17   nsathya     Added dmm open close function declarations for GERAN dynamic module.
***
*****************************************************************************/

#ifndef GERAN_DMM_API_H
#define GERAN_DMM_API_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/
#include "geran_variation.h"
#include "customer.h"

#if defined(FEATURE_DYNAMIC_LOADING_GERAN) || defined(FEATURE_EXTERNAL_PAGING_GERAN)
#include "bit.h"
#include "comdef.h"
#include "dmm.h"

typedef enum
{ 
  GERAN_DMM_INIT_INTF,    /* State management interface */ 
  GERAN_DMM_GL1_INTF,     /* GL1 interface */ 
  GERAN_DMM_GDRIVER_INTF,
  GERAN_DMM_GMDSP_INTF,
  GERAN_DMM_GMSGRIF_INTF,
  GERAN_DMM_GL2_INTF,     /* GL2 interface */ 
  GERAN_DMM_GMAC_INTF,    /* GMAC interface */  
  GERAN_DMM_GRLC_INTF,    /* GRLC interface */
  GERAN_DMM_GLLC_INTF,    /* GLLC interface */
  GERAN_DMM_GSNDCP_INTF,  /* GRLC interface */
  GERAN_DMM_GRR_INTF,     /* GRR interface */ 
  GERAN_DMM_GRF_INTF      /* RFTECH-GSM interface */
} GERAN_DMM_INTFS; 


#define GERAN_DMM_INIT_INTF_ID     DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_INIT_INTF) 
#define GERAN_DMM_GRR_INTF_ID      DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GRR_INTF) 
#define GERAN_DMM_GL1_INTF_ID      DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GL1_INTF) 
#define GERAN_DMM_GDRIVER_INTF_ID  DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GDRIVER_INTF)
#define GERAN_DMM_GMDSP_INTF_ID    DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GMDSP_INTF)
#define GERAN_DMM_GMSRIF_INTF_ID   DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GMSGRIF_INTF)
#define GERAN_DMM_GL2_INTF_ID      DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GL2_INTF) 
#define GERAN_DMM_GMAC_INTF_ID     DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GMAC_INTF) 
#define GERAN_DMM_GRLC_INTF_ID     DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GRLC_INTF)
#define GERAN_DMM_GSN_INTF_ID      DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GSNDCP_INTF) 
#define GERAN_DMM_GLLC_INTF_ID     DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GLLC_INTF) 
#define GERAN_DMM_GRF_INTF_ID      DMIF_ID_COMBINED(DM_ID_GERAN, GERAN_DMM_GRF_INTF) 

extern boolean geran_dmm_load(void);
extern boolean geran_dmm_unload(void);
extern boolean geran_dmm_suspend(void);
extern boolean geran_dmm_resume(void);
extern boolean geran_dmm_set_loaded(void);
extern boolean geran_dmm_set_inactive(void);
#endif 

#endif /*GERAN_DMM_API_H*/

