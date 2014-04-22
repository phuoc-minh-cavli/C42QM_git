#include "ecm_diag.h"
#include "log_codes.h"
#include "log.h"
#include "msg.h"
#include "diagpkt.h"
#include "ecm_frame.h"
#include "spmi_fetcher.h"
#include "ecm.h"
#include "pm_version.h"
#include<stdlib.h>
#include "diag.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "ecm_diag.h"
#include "spmif_cfgdata.h"

#define ECM_DIAG_CMD  (256 + 20)
#define ECM_DIAG_FTM_CMD (256+21)

#define ECM_DIAG_RESP_SUCCESS  0
#define ECM_DIAG_RESP_ERROR 1

#define ECM_LOG_CODE  LOG_ECM_C

static int meta_count = 0;
static int regulator_count = 0;

static uint8 ecm_data_backup[1024];
static uint32 ecm_data_size;

static ecm_diag_pkt_pmic_dev_info_type pmic_dev_info;
static ecm_regulator_info_type*         regulators;


static int ecm_diag_client_start_oneshot(void);
static int ecm_diag_client_start_streaming(void);
static int ecm_diag_client_stop(void);
#pragma pack(1)
struct ecm_diag_control_pkt
{
   diagpkt_subsys_header_type hdr;     /**<-- Do not access directly             */
   uint8  req;                         /**<-- What type of request is being made */
   uint8 data0;                       /**<-- data associated with the command   */

};

#pragma pack(1)
struct ecm_diag_control_resp_pkt
{
   diagpkt_subsys_header_type hdr;  /**< -- do not access directly                     */
   uint8 resp;
}; 

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
    ecm_regulator_map_type regulator_map[ECM_MAX_REGULATORS];
}ecm_ftm_mode_regulator_info_type;


#pragma pack(1)
struct ecm_diag_ftm_cmd_resp_pkt
{
   diagpkt_subsys_header_type hdr;  /**< -- do not access directly                     */
   uint8 resp;
   ecm_diag_pkt_pmic_dev_info_type     pmic_info;
   ecm_ftm_mode_regulator_info_type    regulator_info;
   uint8        nr_frames;              /**< number of ECM data frames */
   uint16       size_per_frame;         /**< size per data frame*/
   uint16       ecm_sample_rate;        /**< ecm sample rate in ms*/
   uint32       ecm_timestamp;          /**< the ECM timestamp (RTC) just prior to the log packet alloc*/
   uint8        buck_cnt;               /**< number of buck regulators in the frame */ 
   uint8        ldo_cnt;                /**< number of ldo regulators in the frame */ 
   uint8        misc_cnt;               /**< number of misc regulators in the frame */ 
   uint8 pfifo_data[1024];
}; 


PACK(void*) ecm_diag_control_fnc(PACK(void*) req_pkt, uint16 pkt_len)
{
	struct ecm_diag_control_pkt* req;
	struct ecm_diag_control_resp_pkt * resp;
    int ret = 0;
	if ((NULL == req_pkt) || (pkt_len < sizeof(struct ecm_diag_control_pkt))) {
		return ((void*)diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
	}
	req = (struct ecm_diag_control_pkt*)req_pkt;


	if (req->req == ECM_DIAG_START_ONESHOT) {
		ret = ecm_diag_client_start_oneshot();
	} else if (req->req == ECM_DIAG_START_STREAMING ){
		ret = ecm_diag_client_start_streaming();
	} else if (req->req == ECM_DIAG_STOP) {
        ret = ecm_diag_client_stop();
	} else if (req->req == ECM_DIAG_CHANGE_SAMPLE_RATE) {
        ret = ecm_set_sample_rate((uint8)req->data0);
    }

	resp = (struct ecm_diag_control_resp_pkt*)diagpkt_subsys_alloc(DIAG_SUBSYS_COREBSP, ECM_DIAG_CMD, sizeof(struct ecm_diag_control_resp_pkt));

    if (resp == NULL) {
        MSG_ERROR("ECM diag response packet allocation failed",0,0,0);
        return NULL;
    }
	if (ret == 0) {
		resp->resp = ECM_DIAG_RESP_SUCCESS ;
	} else {
		MSG_ERROR("ECM Request FAILED req %d \n",ret,0,0);
		resp->resp = ECM_DIAG_RESP_ERROR;
	}
	return resp;
}

PACK(void*) ecm_diag_ftm_cmd_fnc(PACK(void*) req_pkt, uint16 pkt_len)
{
    struct ecm_diag_control_pkt* req;
	struct ecm_diag_ftm_cmd_resp_pkt * resp;
	uint32 rtc_count;
    uint8 buck_cnt, ldo_cnt, misc_cnt;

	if ((NULL == req_pkt) || (pkt_len < sizeof(struct ecm_diag_control_pkt))) {
		return ((void*)diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
	}
	req = (struct ecm_diag_control_pkt*)req_pkt;


	if (req->req != ECM_DIAG_FTM_GET_DATA) {
		return ((void*)diagpkt_err_rsp(DIAG_BAD_LEN_F, req_pkt, pkt_len));
	} 
    ecm_diag_client_start_oneshot();
    diagpkt_async_block ();
	resp = (struct ecm_diag_ftm_cmd_resp_pkt*)diagpkt_subsys_alloc(DIAG_SUBSYS_COREBSP, ECM_DIAG_FTM_CMD, sizeof(struct ecm_diag_ftm_cmd_resp_pkt));

    if (resp == NULL) {
        MSG_ERROR("ECM diag response packet allocation failed",0,0,0);
        return NULL;
    }

	if (ecm_data_size !=0) {
		resp->resp = ECM_DIAG_RESP_SUCCESS ;
        resp->pmic_info = pmic_dev_info;
        resp->regulator_info.vthp_mv  =  regulators->vthp_mv;
        resp->regulator_info.vthp_mv  =  regulators->vthp_mv;
        resp->regulator_info.vref_mv  =  regulators->vref_mv;
        resp->regulator_info.ecm_clk_freq =  regulators->ecm_clk_freq;
        resp->regulator_info.i_ref  =  regulators->i_ref;
        resp->regulator_info.ldo_rtrim  =  regulators->ldo_rtrim;
        resp->regulator_info.ldo_mmax  =  regulators->ldo_mmax;
        resp->regulator_info.qpoet_rtrim  =  regulators->qpoet_rtrim;
        resp->regulator_info.qpoet_mmax  =  regulators->qpoet_mmax;
        memcpy(resp->regulator_info.regulator_map, &regulator_map, sizeof(regulator_map));
        resp->size_per_frame = ecm_get_ecm_frame_size();         /**< size per data frame*/
        resp->nr_frames = ecm_data_size/resp->size_per_frame;              /**< number of ECM data frames */
        
        resp->ecm_sample_rate = ecm_get_sample_rate();        /**< ecm sample rate in ms*/
        if (ECM_ERROR == ecm_get_pmic_rtc_count(&rtc_count)) {
            rtc_count = 0;
            MSG_ERROR("Error getting pmic rtc count ", 0,0,0);
        }
        ecm_get_enabled_regulator_cnt(&buck_cnt, &ldo_cnt, &misc_cnt);

        resp->ecm_timestamp = rtc_count ;          /**< the ECM timestamp (RTC) just prior to the log packet alloc*/
        resp->buck_cnt = buck_cnt;               /**< number of buck regulators in the frame */ 
        resp->ldo_cnt = ldo_cnt;                /**< number of ldo regulators in the frame */ 
        resp->misc_cnt = misc_cnt;               /**< number of misc regulators in the frame */ 
        if (ecm_data_size > 1024)
        {
            ecm_data_size = 1024;
        }
        memcpy(resp->pfifo_data,ecm_data_backup,ecm_data_size);
        
	} else {
		MSG_ERROR("No ECM Data is avaliable\n",0,0,0);
		resp->resp = ECM_DIAG_RESP_ERROR;
	}
	return resp;

}

static const diagpkt_user_table_entry_type ecm_diag_subsys_common_tbl[] =
{
  {ECM_DIAG_CMD, ECM_DIAG_CMD, ecm_diag_control_fnc}
};

static const diagpkt_user_table_entry_type ecm_diag_ftm_cmd_tbl[] =
{
  {ECM_DIAG_FTM_CMD, ECM_DIAG_FTM_CMD, ecm_diag_ftm_cmd_fnc}
};



int ecm_diag_send_regulator_info(void)
{
	LOG_ECM_type* log_ptr = NULL;
    size_t size = sizeof(ecm_regulator_info_type) + regulators->nr_regulators*sizeof(ecm_regulator_map_type);
    log_ptr = (LOG_ECM_type*)log_alloc_ex((log_code_type)ECM_LOG_CODE, sizeof(LOG_ECM_type) + size);

    if (!log_ptr) {
        MSG_ERROR("Error sending regulator info ", 0,0,0);
        return 1;
    }

    log_ptr->ver = ECM_PKT_VERSION;
    log_ptr->type = ECM_LOG_TYPE_REGULATOR_INFO;
    log_ptr->ecm_timestamp = 0; 
    log_ptr->ecm_sample_rate = 0;
    log_ptr->format = 0;
    log_ptr->size_per_frame = sizeof(ecm_regulator_info_type);
    log_ptr->nr_frames = 1;
    log_ptr->buck_cnt = 0;
    log_ptr->ldo_cnt  = 0;
    log_ptr->misc_cnt  = 0;

    memcpy(log_ptr+1, regulators, size);
    log_commit(log_ptr);

    return 0;
}

int ecm_diag_send_pmic_info(void)
{
    LOG_ECM_type* log_ptr = NULL;
    log_ptr = (LOG_ECM_type*)log_alloc_ex((log_code_type)ECM_LOG_CODE, sizeof(LOG_ECM_type) + sizeof(pmic_dev_info));
    if (!log_ptr) {
        MSG_ERROR("Error sending pmic info ", 0,0,0);
        return 1;
    }
    log_ptr->ver = ECM_PKT_VERSION;
    log_ptr->type = ECM_LOG_TYPE_PMIC_INFO;
    log_ptr->ecm_timestamp = 0; 
    log_ptr->ecm_sample_rate = 0;
    log_ptr->format = 0;
    log_ptr->size_per_frame = sizeof(pmic_dev_info);
    log_ptr->nr_frames = 1;
    log_ptr->buck_cnt = 0;
    log_ptr->ldo_cnt  = 0;
    log_ptr->misc_cnt  = 0;

    memcpy(log_ptr+1, &pmic_dev_info, sizeof(pmic_dev_info));
    log_commit(log_ptr);

    return 0;
}

int ecm_diag_send_data(void* data, uint32 data_size)
{
    uint32 rtc_count;
    uint8 buck_cnt, ldo_cnt, misc_cnt;
    LOG_ECM_type* log_ptr = NULL;

    if (data_size == 0 ) {
        //This indicates a end of transfer packet
        return 0;
    }

    ecm_data_size = data_size;
    if (ecm_data_size > 1024)
    {
        ecm_data_size  = 1024;
    }
    memcpy(ecm_data_backup,data,ecm_data_size) ;
    diagpkt_async_release();
    log_ptr = (LOG_ECM_type*)log_alloc_ex((log_code_type)ECM_LOG_CODE, sizeof(LOG_ECM_type) + data_size);
    if (!log_ptr) {
        MSG_ERROR("Error sending data ", 0,0,0);
        return 1;
    }
    if (ECM_ERROR == ecm_get_pmic_rtc_count(&rtc_count)) {
        rtc_count = 0;
        MSG_ERROR("Error getting pmic rtc count ", 0,0,0);
    }
    ecm_get_enabled_regulator_cnt(&buck_cnt, &ldo_cnt, &misc_cnt);
    log_ptr->ver = ECM_PKT_VERSION;
    log_ptr->type = ECM_LOG_TYPE_DATA;
    log_ptr->ecm_timestamp = rtc_count;
    log_ptr->ecm_sample_rate = ecm_get_sample_rate();
    log_ptr->format = ECM_PKT_FORMAT;
    log_ptr->size_per_frame = ecm_get_ecm_frame_size();
    log_ptr->nr_frames = data_size/log_ptr->size_per_frame;
    log_ptr->buck_cnt = buck_cnt;
    log_ptr->ldo_cnt  = ldo_cnt;
    log_ptr->misc_cnt  = misc_cnt;
    memcpy(log_ptr+1, data, data_size);
    log_commit(log_ptr);
    return 0;
}

int ecm_diag_log(void* data, uint32 data_size)
{
    

    if (!meta_count) {
        if (ecm_diag_send_pmic_info()) {
            MSG_HIGH("Error sending pmic info",0,0,0);
        }
    }
    if (!regulator_count) {
        if (ecm_diag_send_regulator_info()) {
            MSG_HIGH("Error sending ecm regulator info",0,0,0);
        }
    }
    if (ecm_diag_send_data(data,data_size)) {
        MSG_HIGH("Error sending ecm data",0,0,0);
        return 1;
    }
    meta_count++;
    regulator_count++;

    if (meta_count > ECM_META_DATA_FREQ) {
        meta_count = 0;
    }

    if (regulator_count > ECM_REGULATOR_DATA_FREQ) {
        regulator_count = 0;
    }
	return 0;
}

void ecm_diag_cb(void* buf, uint32 size)
{
    //got the buffer. Check how big it is.
    if (!buf || !size) {
        MSG_HIGH("ecm read-notifier buf == NULL\n",0,0,0);
        /* this indicates this is the last data */
    }
    if (ecm_diag_log((void*)buf,size)){
        //error
        MSG_HIGH("ECM error in spmif_diag_cb\n",0,0,0);
    }
};

static int fd;
static int ecm_init_errors;

void ecm_diag_client_init()
{
    regulators = calloc(1,sizeof(ecm_regulator_info_type) + 
                          sizeof(ecm_regulator_map_type)*NR_REGULATORS);

	ecm_dev_init();
    ecm_fill_pmic_info(&pmic_dev_info);
    ecm_fill_calibration_data(regulators);

	if ( ECM_OK != ecm_open(LOGGER_SPMIF,&fd)) {
		ecm_init_errors++;
	}
	ecm_register_read_notifier(fd, ecm_diag_cb);
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_COREBSP, ecm_diag_subsys_common_tbl);
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_COREBSP, ecm_diag_ftm_cmd_tbl);
}


static int ecm_diag_client_start_oneshot(void)
{
    meta_count = 0;
    regulator_count = 0;

	if (ECM_OK == ecm_read_single(fd, NULL, NULL))
		return 0;
	return 1;
}

static int ecm_diag_client_start_streaming(void)
{
    meta_count = 0;
    regulator_count = 0;

	if (ECM_OK == ecm_read(fd, NULL, NULL)){

		return 0;
	}
	return 1;
}

static int ecm_diag_client_stop(void)
{
   
	if (ECM_OK == ecm_stop(fd)){
		return 0;
	}
	return 1;
}
