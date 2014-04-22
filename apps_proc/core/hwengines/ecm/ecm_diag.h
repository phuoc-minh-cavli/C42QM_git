#ifndef __ECM_DIAG_H__
#define __ECM_DIAG_H__
#include "diagpkt.h"
#include "log.h"

#define VERSION(major,minor)                     ((major)<<4|(minor))
#define VERSION_MAJOR(version)                   (((version)>>4))&0xF)
#define VERSION_MINOR(version)                   ((version)&0xF)

#define ECM_PM_PBS_INFO_NUM_LOT_IDS 12

#define ECM_PKT_VERSION                     VERSION(0,1)
#define ECM_PKT_MAGIC_NUM                   0xec33
#define REGULATOR_MAX_CONSTANTS             4
#define REG_MAX_LEN                         7

#define ECM_DIAG_START_STREAMING            5
#define ECM_DIAG_START_ONESHOT              4
#define ECM_DIAG_STOP                       6
#define ECM_DIAG_CHANGE_SAMPLE_RATE         7
#define ECM_DIAG_CHANGE_BUFFER_SIZE         8

#define ECM_DIAG_FTM_GET_DATA               1
/* ECM packet type */
#define ECM_LOG_TYPE_DATA                   0
#define ECM_LOG_TYPE_PMIC_INFO              1
#define ECM_LOG_TYPE_REGULATOR_INFO         2

/* ECM pkt format 
 *  0  - raw uprocessed data
 *  1  - processed data 
 */
#define ECM_PKT_FORMAT                      0

#define ECM_META_DATA_FREQ                  1000
#define ECM_REGULATOR_DATA_FREQ             100
#define ECM_MAX_REGULATORS                  14

typedef PACK(struct)
{
    uint8    perip_id;                         /* regulator id */
    char     name[REG_MAX_LEN];                /* regulator name */
    uint8    valid_consts;                     /* bit map of valid constants */
    uint16   consts[REGULATOR_MAX_CONSTANTS];  /* max calibration constants per regulator */
    uint16   transistor_width;                 /* transister width ratio for bucks */
}ecm_regulator_map_type;

typedef PACK(struct)
{
    uint32   vthp_mv;                              /**< VTHp in millivolts. */
    uint32   vref_mv;                              /**< reference volate for ADC in mllivolts.*/ 
    uint32   ecm_clk_freq;                         /**< ECM clk frequency in Hz. */
    uint32   i_ref;                                /**< Iref in uA */
    uint16   ldo_rtrim;                            /**< percentage */
    uint16   ldo_mmax;                             /**< Mmax 30 */
    uint16   qpoet_rtrim;                          /**< RTrim for Qpoet, percentage */
    uint16   qpoet_mmax;                           /**< Qpoet MMax code */
    uint8    nr_regulators;                        /**< nr of regulators in the regulator map */
    ecm_regulator_map_type regulators[];
}ecm_regulator_info_type;

typedef PACK(struct)
{
   uint8    pmic_device_index;                    /**< PMIC device index*/
   uint32   pmic_model;                           /**< PMIC model number*/
   uint32   pmic_all_layer_rev;                   /**< PMIC device all layer revision number*/
   uint32   pmic_metal_rev;                       /**< PMIC device metal revision number */
   uint8    lot_id[ECM_PM_PBS_INFO_NUM_LOT_IDS];  /**< PBS lot id */
   uint32   rom_version;                          /**< PBS ROM Version number */
   uint32   ram_version;                          /**< PBS RAM Version number */
   uint32   fab_id;                               /**< PBS Fab Id */
   uint32   wafer_id;                             /**< PBS Wafer Id */
   uint32   x_coord;                              /**< PBS X Coord */
   uint32   y_coord;                              /**< PBS Y Coord */
   uint32   test_pgm_rev;                         /**< PBS Test PGM Rev */
} ecm_diag_pkt_pmic_dev_info_type;

typedef PACK(struct)
{
    log_hdr_type header;                 /**< diag header */
    uint8        ver;                    /**< packet version */
    uint8        type;                   /**< packet type \see ECM_LOG_TYPE_* macros */
    uint8        format;                 /**< the ECM data format */
    uint8        nr_frames;              /**< number of ECM data frames */
    uint16       size_per_frame;         /**< size per data frame*/
    uint16       ecm_sample_rate;        /**< ecm sample rate in ms*/
    uint32       ecm_timestamp;          /**< the ECM timestamp (RTC) just prior to the log packet alloc*/
    uint8        buck_cnt;               /**< number of buck regulators in the frame */ 
    uint8        ldo_cnt;                /**< number of ldo regulators in the frame */ 
    uint8        misc_cnt;               /**< number of misc regulators in the frame */ 
}LOG_ECM_type;

int ecm_diag_log(void* data, uint32 data_size);
#endif
