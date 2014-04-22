#ifndef HALQDSS_ETM_ARM_H
#define HALQDSS_ETM_ARM_H

/*=============================================================================

FILE:         halqdss_etm_arm.h

DESCRIPTION:  

================================================================================
              Copyright (c) 2013 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/


void HAL_qdss_etmarm_HalConfigInit(uint32 dwBaseAddr);
void  HAL_qdss_etmarm_AccessUnLock(void);
void  HAL_qdss_etmarm_AccessLock(void);

uint32 HAL_qdss_etmarm_Get_CCR(void);
uint32 HAL_qdss_etmarm_Get_CR(void);
uint32 HAL_qdss_etmarm_Get_SR(void);
uint32 HAL_qdss_etmarm_Get_IDR(void);
uint32 HAL_qdss_etmarm_Get_PDCR(void);


void HAL_qdss_etmarm_Set_AUXCR(uint32 val);
void HAL_qdss_etmarm_Set_CIDCMR(uint32 val);
void HAL_qdss_etmarm_Set_CR(uint32 val);
void HAL_qdss_etmarm_Set_EXTINSELR(uint32 val);
void HAL_qdss_etmarm_Set_FFLR(uint32 val);
void HAL_qdss_etmarm_Set_PDCR(uint32 val);
void HAL_qdss_etmarm_Set_SQ12EVR(uint32 val);
void HAL_qdss_etmarm_Set_SQ13EVR(uint32 val);
void HAL_qdss_etmarm_Set_SQ21EVR(uint32 val);
void HAL_qdss_etmarm_Set_SQ23EVR(uint32 val);
void HAL_qdss_etmarm_Set_SQ31EVR(uint32 val);
void HAL_qdss_etmarm_Set_SQ32EVR(uint32 val);
void HAL_qdss_etmarm_Set_SQR(uint32 val);
void HAL_qdss_etmarm_Set_SYNCFR(uint32 val);
void HAL_qdss_etmarm_Set_TECR1(uint32 val);
void HAL_qdss_etmarm_Set_TECR2(uint32 val);
void HAL_qdss_etmarm_Set_TEEVR(uint32 val);
void HAL_qdss_etmarm_Set_TRACEIDR(uint32 val);
void HAL_qdss_etmarm_Set_TRIGGER(uint32 val);
void HAL_qdss_etmarm_Set_TSEVR(uint32 val);
void HAL_qdss_etmarm_Set_TSSCR(uint32 val);
void HAL_qdss_etmarm_Set_VDCR1(uint32 val);
void HAL_qdss_etmarm_Set_VDCR3(uint32 val);
void HAL_qdss_etmarm_Set_VDEVR(uint32 val);
void HAL_qdss_etmarm_Set_VMIDCVR(uint32 val);


void HAL_qdss_etmarm_Set_ACTRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_ACVRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_CIDCVRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_CNTENRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_CNTRLDVRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_CNTVRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_DCMRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_DCVRn(uint8 i,uint32 val);
void HAL_qdss_etmarm_Set_EXTOUTEVRn(uint8 i,uint32 val);

void HAL_qdss_etm_set_power_down(void);
void HAL_qdss_etm_clear_power_down(void);
void HAL_qdss_etm_set_power_up(void);
void HAL_qdss_etm_clear_power_up(void);
void HAL_qdss_etm_wait_prog(void);
void HAL_qdss_etm_set_prog(void);
void HAL_qdss_etm_clear_prog(void);

#endif //HALQDSS_ETM_ARM_H
