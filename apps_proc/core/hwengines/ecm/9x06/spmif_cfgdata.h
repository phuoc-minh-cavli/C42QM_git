#ifndef __SPMIF_CFGDATA_H__
#define __SPMIF_CFGDATA_H__
#include "ecm_diag.h"
#define SPMIF_PMIC_SLAVE_ID 0x0
/* poll-gap timer - time between two status reads.
 * time = 0.4 ms x poll-gap-timer
 */
#define POLLGAP_TIME 1 //10 //0x7ff in VI
/* read-gap-timer - time between two PFIFO reads.
 * time = 6.6us x read-gap-timer
 */
#define READGAP_TIME 1 //1  //0xFF in VI
//#define PMICARB_CHNL_ADDR 0x242D000 //working
// channel 22 #define PMICARB_CHNL_ADDR 0x24B5000
#define  PMICARB_CHNL_ADDR              0x2465000
// #ifdef T_RUMI
//#define ECMFIFO_DATA_ADDR 0x8060
//#define ECMFIFO_STS_ADDR 0x8008
// #else
#define ECMFIFO_DATA_ADDR 0x3d60
#define ECMFIFO_STS_ADDR  0x3d08 
//#endif
#define SPMIF_BAM_PA 0xc4000
#define SPMIF_BAM_IRQ 133

/* descriptor fifo size in words */
#define DESC_SZ_WRDS      (8*10/4)
/* buffer size in words */
#define BUFFER_SZ_WRDS    1024
/* number of data buffers */
#define NR_BUFFERS        4

/* total physically cont. memory required */
#define TOTAL_BUF_SIZE    (DESC_SZ_WRDS*4 + BUFFER_SZ_WRDS*4*NR_BUFFERS)


#define PMIC_ECM_PERIPH_ID       0x3d
#define PMIC_ECM_SLAVE_ID       0x0

#define ECM_PRODUCER_PIPE 1

#define LDO_CONST_ENCODE(x, sign)  (sign<<15|x)

static ecm_regulator_map_type regulator_map[] = {
   /*[0]*/     { 0x15, "S1",0x1, {0x5A, (uint16)-1, (uint16)-1, (uint16)-1}, 5688},
   /*[1]*/     { 0x18, "S2",0x1, {0x5d, (uint16)-1, (uint16)-1, (uint16)-1}, 2088},
   /*[2]*/     { 0x1b, "S3",0x1, {0x5F, (uint16)-1, (uint16)-1, (uint16)-1}, 3528},
   /*[3]*/     { 0x1e, "S4",0x1, {0x50, (uint16)-1, (uint16)-1, (uint16)-1}, 4260},
   /*[4]*/     { 0x21, "S5",0x1, {0x61, (uint16)-1, (uint16)-1, (uint16)-1}, 2808},
                                    /*mtrim  */                  /* y0 */                   /*gldo*/
   /*[5]*/     { 0x40, "LDO1",0x3, {LDO_CONST_ENCODE(1,1),  LDO_CONST_ENCODE(63,1),  LDO_CONST_ENCODE(1524,0), (uint16)-1}, 0},
   /*[6]*/     { 0x41, "LDO2",0x3, {LDO_CONST_ENCODE(2,0),  LDO_CONST_ENCODE(49,1),  LDO_CONST_ENCODE(1524,0), (uint16)-1}, 0},
   /*[7]*/     { 0x42, "LDO3",0x3, {LDO_CONST_ENCODE(12,0), LDO_CONST_ENCODE(286,1), LDO_CONST_ENCODE(1524,0), (uint16)-1}, 0},
   /*[8]*/     { 0x43, "LDO4",0x3, {LDO_CONST_ENCODE(1,1),  LDO_CONST_ENCODE(60,1),  LDO_CONST_ENCODE(381,0), (uint16)-1}, 0},
   /*[9]*/     { 0x44, "LDO5",0x3, {LDO_CONST_ENCODE(1,0),  LDO_CONST_ENCODE(270,1), LDO_CONST_ENCODE(889,0), (uint16)-1}, 0},
   /*[10]*/    { 0x45, "LDO6",0x3, {LDO_CONST_ENCODE(7,0),  LDO_CONST_ENCODE(153,1), LDO_CONST_ENCODE(889,0), (uint16)-1}, 0},
   /*[11]*/    { 0x47, "LDO8",0x3, {LDO_CONST_ENCODE(17,1), LDO_CONST_ENCODE(77,1),  LDO_CONST_ENCODE(1524,0), (uint16)-1}, 0},
   /*[12]*/    { 0x48, "LDO9",0x3, {LDO_CONST_ENCODE(8,1),  LDO_CONST_ENCODE(116,1), LDO_CONST_ENCODE(1524,0), (uint16)-1}, 0},
                                     /*mtrim  */                  /* y0 */         
   /*[13]*/    { 0x9,  "QPOET",0x2, {LDO_CONST_ENCODE(1,1), LDO_CONST_ENCODE(60,1), 0, 0}, 0}
};

static uint32 ecm_vref_map [] = {300,400,500,600};

static uint32 ecm_clk_freq_map[] = {
    /*0*/ 9600000, /*1*/ 9600000, /*2*/ 6400000, /*3*/ 4800000, 
    /*4*/ 3800000, /*5*/ 3200000, /*6*/ 2700000, /*7*/ 2400000, 
    /*8*/ 2100000, /*9*/ 1900000, /*10*/1700000, /*11*/1600000, 
    /*12*/1500000, /*13*/1400000, /*14*/1300000, /*15*/1200000
};

/* ECM peripheral specific constants */
#define I_REF_UA         10
#define LDO_RTRIM        15
#define LDO_MMAX         30
#define QPOET_RTRIM      15
#define QPOET_MMAX       30
#define NR_REGULATORS    14
#define VTHP_MV          700

/* SPMI fetcher readgap and pollgap times*/
#define SPMIF_POLLGAP_GRANULARITY      400
#define SPMIF_READGAP_GRANULARITY      6.6
#endif
