

#include "ddr_common.h"
#include "HALhwio.h"
#include "DDR_PHY.h"
#include "ddr_target.h"
#include "ddr_hwio.h"
#include "ddr_config.h"
#include "HAL_DDR.h"
#include "DDR_PHY.h"
#include "AHB2PHY.h"
#include "ddr_params.h"
#include "bimc_training_apis.h"

#define FAIL_HISTOGRAM_SIZE 										51  //Number of CDC_Delay values
#define PRECS_RISE                                                   0
#define PRECS_FALL                                                   1
#define CS_RISE                                                      2
#define CS_FALL                                                      3
#define POSTCS_RISE                                                  4
#define POSTCS_FALL                                                  5

#define CA_PATTERN_NUM 												 4

//#define MAX_COARSE_STEP            21
#define MAX_COARSE_STEP            51
#define MAX_FINE_STEP              6
#define COARSE_STEP                10


#define CHNL_CNT              1
#define NO_DWORDS             1
#define NUM_BYTES             4
#define DELAY_CNT             4
#define DQ_CNT                2

#define BUS_SIZE              32
#define SITE_PER_CA_CNT       1
#define ALL_CA_FAIL_MAP0      0x000001ef
#define ALL_CA_FAIL_MAP1      0x00000210
#define ALL_CA_PASS_PAT       0x00000000

#define DQ_TRAINING_PATTERN_SIZE   48
#define ALL_DQ_FAIL_PAT      0xFFFFFFFF
#define ALL_DQ_PASS_PAT      0x00000000

#define DQWR_TRAINING_ENABLED        TRUE //NOT USED YET
#define DQRD_TRAINING_ENABLED        TRUE //NOT USED YET
#define CA_TRAINING_ENABLED          TRUE //NOT USED YET

#define BYTE_SIZE                  8
#define WORD_SIZE                  32
#define PHY_SIZE                   16
#define WORD_PER_BUS               (BUS_SIZE  / WORD_SIZE)
#define PHY_PER_BUS                (BUS_SIZE  / PHY_SIZE)
#define BYTE_PER_BUS               (BUS_SIZE  / BYTE_SIZE)
#define BYTE_PER_WORD              (WORD_SIZE / BYTE_SIZE)
#define BYTE_PER_PHY               (PHY_SIZE  / BYTE_SIZE)

 //static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = {0,50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000};
// static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = {0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150,160,170,180,190,200,210,220,230,240,250,260,270,280,290,300,310,320,330,340,350,360,370,380,390,400,410,420,430,440,450,460,470,480,490,500,510,520,530,540,550,560,570,580,590,600,610,620,630,640,650,660,670,680,690,700,710,720,730,740,750,760,770,780,790,800,810,820,830,840,850,860,870,880,890,900,910,920,930,940,950,960,970,980,990,1000};
//static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = {0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,320,340,360,380,400,420,440,460,480,500,520,540,560,580,600,620,640,660,680,700,720,740,760,780,800,820,840,860,880,900,920,940,960,980,1000};
static const uint16 cdc_lut[FAIL_HISTOGRAM_SIZE] = {0,30,60,90,120,150,180,210,240,270,300,330,360,390,420,450,480,510,540,570,600,630,660,690,720,750,780,810,840,870,900,930,960,990,1020,1050,1080,1110,1140,1170,1200,1230,1260,1290,1320,1350,1380,1410,1440,1470,1500};
 static const uint8 dq_remap_lut[CHNL_CNT][NUM_BYTES]  = 
{{ 0, 1, 2, 3}}; //{{ 2, 0, 3, 1}}
static const uint8 ca_remap_lut[CHNL_CNT][10] =
{{0,1,2,3,4,5,6,7,8,9,}};   //{{,9,8,7,6,5,4,3,0,1,2}};    
static const uint8 bit_remap_lut[CHNL_CNT][NUM_BYTES][8]  =
{
 {
   { 0 ,1, 2, 3, 4, 6, 5, 7 },      //    { 1 ,2 ,5 ,0 ,7 ,3 ,4 ,6 },    
   { 0 ,1, 2, 3, 4, 6, 5, 7 },      //    { 0 ,1 ,2 ,3 ,4 ,6 ,5 ,7 },    
   { 0 ,1, 2, 3, 4, 6, 5, 7 },      //    { 5 ,2 ,6 ,3 ,7 ,0 ,1 ,4 },    
   { 0 ,1, 2, 3, 4, 6, 5, 7 },      //    { 7 ,6 ,5 ,3 ,4 ,1 ,2 ,0 }     
 }
};  

boolean DDRSS_EBI1_PHY_Set_Training_Data(SDRAM_INTERFACE interface, union ddr_trained_params tr_data);
uint16 DDRSS_ddr_phy_dq_wr_training_ddrss( uint32 _inst_,SDRAM_INTERFACE  interface, uint16 rank, uint16 max_loop_count, uint16 coarse_init_val, uint16 dq_write_training[PHY_DQ_MAX_NUM], uint32 clk_freq_in_khz );
uint16 HAL_SDRAM_ddr_phy_ca_training( uint32 _inst_, SDRAM_INTERFACE interface, uint16 rank,uint16 max_loop_count, uint16 coarse_init_val, uint16 *ca_training_full_cdc, uint32 clk_freq_in_khz);
uint16 DDRSS_ddr_phy_dq_rd_training_ddrss( uint32 _inst_, SDRAM_INTERFACE interface, uint16 rank, uint16 max_loop_count, uint16 coarse_init_val, uint16 dq_read_training[PHY_DQ_MAX_NUM],uint32 clk_freq_in_khz );
uint32 HAL_SDRAM_DDR_PHY_read_dq_training_status_for_ca_training(uint32 base, SDRAM_INTERFACE interface);
void HAL_SDRAM_DDR_PHY_CA_training_mode(uint32 base, SDRAM_INTERFACE interface, uint32 mode);
void HAL_SDRAM_DDR_PHY_Update_WR_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config, uint32 which_dq);
void HAL_SDRAM_DDR_PHY_Update_RD_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config, uint32 which_dq);
void HAL_SRAM_DDR_PHY_Update_CA_CDC_Config(uint32 base, SDRAM_INTERFACE interface, uint32 config);
uint16 DDRSS_ca_training_histogram_update(uint8 ca_mapping, uint8 delay_step, uint16 read, uint16 expected);
uint16 DDRSS_get_ca_exp_pattern(uint32 _inst_, uint16 ca_pat_rise, uint16 ca_pat_fall, uint16 ca_train_mapping);
void DDRSS_histogram_min_max_scan(uint32 _inst_, uint8 *min, uint8 *max, uint8 fail_count_histogram[FAIL_HISTOGRAM_SIZE],uint32 coarse_init_val, uint32 histogram_size);
void clear_histogram(void);
uint32 DDRSS_dq_rd_wr_training_histogram_update(SDRAM_INTERFACE  interface, uint8 delay_step, uint32 read[DQ_TRAINING_PATTERN_SIZE], uint32 expected[DQ_TRAINING_PATTERN_SIZE], uint8 fail_count_histogram_perbyte[PHY_DQ_MAX_NUM][FAIL_HISTOGRAM_SIZE],uint8 dq);
//void HAL_SDRAM_Write_MR(uint32 base, SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data);
void BIMC_Disable_All_Periodic(uint32 reg_base, SDRAM_INTERFACE interface, uint32 chip_select);

