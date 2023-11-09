#include <qapi_fs.h>
#include "qca4004.h"
#include "qca4004_internal.h"
#include "qca4004_security.h"
#include "qca4004_utils.h"
#include "qca4004_fota.h"
#include "qca4004_api.h"

QCA4004_FOTA_Ctx_t qca4004_fota_ctxt;
QCA4004_FOTA_Ctx_t *qca4004_fota_ctxt_p = NULL;

qca4004_Callback_Table_t ota_cb = {NULL, NULL};

int qca4004_fota_img_file_stat()
{
	struct qapi_FS_Stat_Type_s SBuf = {0};
	
	if(qapi_FS_Stat(qca4004_fota_ctxt_p->image_name, &SBuf) == QAPI_OK) {
		if(SBuf.st_size > QCA4004_IMG_HDR_SIZE && SBuf.st_size <= (QCA4004_PARTITION_SIZE + QCA4004_IMG_HDR_SIZE)) {
			QCA4004_DEBUG_LOG("file size %d\n",SBuf.st_size);
			qca4004_fota_ctxt_p->image_size = SBuf.st_size;
			return 0;
		}
	}
	
	return -1;
}

int qca4004_img_header_check(uint8_t *buf, uint32_t len)
{
	uint32_t ota_flag = 0;
	uint32_t ota_len = 0;
	if(len >= QCA4004_IMG_HDR_SIZE) {
		memcpy(&ota_flag, buf, sizeof(uint32_t));
		ota_flag = qca4004_swap32(ota_flag);
		if(ota_flag == QCA4004_IMG_HDR_FLAG) {
			memcpy(&ota_len, buf+4, sizeof(uint32_t));
			ota_len = qca4004_swap32(ota_len);
			if(ota_len + QCA4004_IMG_HDR_SIZE == qca4004_fota_ctxt_p->image_size) {
				memcpy(qca4004_fota_ctxt_p->checksum, buf+8, QCA4004_MD5_CHECKSUM_LEN);
				return 0;
			}
		}
	}
	
	return -1;
}

int qca4004_fota_verify_image(void)
{
	qapi_FS_Offset_t seekStatus = 0;
	int fd = -1, ret = -1;
	uint32_t read_length = 0, offset = 0;
	uint8_t buf[128];
	uint8_t checksum_l[QCA4004_MD5_CHECKSUM_LEN];
	
	if(qca4004_fota_img_file_stat() < 0)
		return ret;
		
	if(QAPI_OK != qapi_FS_Open(qca4004_fota_ctxt_p->image_name, QAPI_FS_O_RDONLY_E, &fd))
		return ret;
		
	qca4004_md5_init();
	
	do {
		ret = qapi_FS_Seek(fd, offset, QAPI_FS_SEEK_SET_E, &seekStatus);
		if(ret != QAPI_OK)
			goto error;
			
		if(offset == 0) {
			qapi_FS_Read(fd, buf, QCA4004_IMG_HDR_SIZE, &read_length);
			ret = qca4004_img_header_check(buf, read_length);
			if(ret < 0)
				goto error;
		}
		else {
			qapi_FS_Read(fd, buf, 128, &read_length);
			if(read_length > 0)
				qca4004_md5_update(buf, read_length);
		}
		if(read_length > 0) {
			offset += read_length;
		}
		else {
			break;
		}
	} while(1);
error:
	qapi_FS_Close(fd);
	
	qca4004_md5_final(checksum_l);
	
	if(ret == 0)
		ret = memcmp(checksum_l, qca4004_fota_ctxt_p->checksum, QCA4004_MD5_CHECKSUM_LEN);

	return ret;
}

int qca4004_fota_select_partition()
{
	int ret = 0;
	char cmd_str[20];
	uint32_t cmd_len = 0;
	char *cmd_ptr = &cmd_str[0];
	uint32 set_signal = 0;

	memset(cmd_ptr, 0 , 20);

	memcpy(cmd_ptr, COMMAND_OTA_STRING, strlen(COMMAND_OTA_STRING));
	cmd_ptr += strlen(COMMAND_OTA_STRING);

	*cmd_ptr++ = ',';
	if(qca4004_fota_ctxt_p->flag & QCA4004_FOTA_AUTO_RESET)
		*cmd_ptr++ = '1'; 
	else
		*cmd_ptr++ = '0';
	*cmd_ptr++ = ',';
	
	cmd_len = cmd_ptr - &cmd_str[0];
		
	ret = (int)qca4004_send_command(cmd_len, &cmd_str[0]); 	
	if(ret == 0) {
		ret = qurt_signal_wait_timed(&(qca4004_fota_ctxt_p->signalc),
							0x01,QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK,
							&set_signal,500*QURT_TIMER_NEXT_EXPIRY);
		if(QURT_EOK == ret){
			if(qca4004_fota_ctxt_p->retValue > 0)
				return 0;
		}
	}
	
	return ret;
}

int qca4004_send_block(uint8_t *block, int fd)
{
	int  read_length = 0;
	uint16_t crc;
	memset(block,0,sizeof(block));

	block[0] = QCA4004_FOTA_SOH;
	block[1] = (qca4004_fota_ctxt_p->seq) >> 8;
	block[2] = (qca4004_fota_ctxt_p->seq) & 0x00ff;
	
	qapi_FS_Read(fd, &(block[3]), QCA4004_FOTA_BLOCK_DATA_SIZE, &read_length);

	if(read_length > 0) 
	{
		crc = qca4004_crc16_calc(&(block[3]),read_length);
		block[3+read_length] = crc >> 8;
		block[4+read_length] = crc & 0x00ff;

		qca4004_send_buf_direct(5+read_length,(char *)block);
		QCA4004_DEBUG_LOG("send block %d,%d,%d\n",(qca4004_fota_ctxt_p->seq),read_length,crc);
	}
	
	return read_length;	
}

int qca4004_fota_data_transmit()
{
	qapi_FS_Offset_t seekStatus = 0;
	int fd = -1, ret = -1,status = 0,block_fail_cnt = 0;
	uint32_t read_length = 0, offset = 0;
	uint8_t buf[QCA4004_FOTA_BLOCK_SIZE];	
	uint32 set_signal = 0;

	qapi_FS_Open(qca4004_fota_ctxt_p->image_name, QAPI_FS_O_RDONLY_E, &fd);
	do {
		qapi_FS_Seek(fd, offset, QAPI_FS_SEEK_SET_E, &seekStatus);
		ret = qca4004_send_block(buf, fd);
		if(ret > 0) {
			status = qurt_signal_wait_timed(&(qca4004_fota_ctxt_p->signal), 0x01, 
						QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK,
						&set_signal,5*QURT_TIMER_NEXT_EXPIRY);
			if(qca4004_fota_ctxt_p->retValue == 1 && status == QURT_EOK){
				offset += ret;
				(qca4004_fota_ctxt_p->seq)++;
				block_fail_cnt = 0;
			}
			else {
				block_fail_cnt++;
			}
			if(block_fail_cnt > 10){
				QCA4004_DEBUG_LOG("fail %d\n",offset);
				break;
			}
		}
	} while(ret > 0);
	qapi_FS_Close(fd);

	qca4004_fota_ctxt_p->fota_state = QCA4004_FOTA_STATE_H2T_COMPLETE;
	status = qurt_signal_wait_timed(&(qca4004_fota_ctxt_p->signalc), 0x01, 
						QURT_SIGNAL_ATTR_WAIT_ANY | QURT_SIGNAL_ATTR_CLEAR_MASK,
						&set_signal,500*QURT_TIMER_NEXT_EXPIRY);
	return ret;
}

uint32_t qca4004_init_fota_ctx(void)
{
	qca4004_fota_ctxt_p = &qca4004_fota_ctxt;
	memset(qca4004_fota_ctxt_p, 0, sizeof(QCA4004_FOTA_Ctx_t));

	qurt_signal_init(&(qca4004_fota_ctxt_p->signal));
	qurt_signal_init(&(qca4004_fota_ctxt_p->signalc));
}

uint32_t qca4004_deinit_fota_ctx(void)
{
	qurt_signal_destroy(&(qca4004_fota_ctxt_p->signal));
	qurt_signal_destroy(&(qca4004_fota_ctxt_p->signalc));
	
	memset(qca4004_fota_ctxt_p, 0, sizeof(QCA4004_FOTA_Ctx_t));
	qca4004_fota_ctxt_p = NULL;
}

extern uint32_t fw_version;
int qca4004_fota_start_internal(char *filePath, int32_t flag, uint32_t *version)
{
	int ret = 0;

	if(flag & QCA4004_FOTA_VERSION_CHECK) {
		//TODO
		if(version == NULL || *version == fw_version)
			return ret;
	}
	
	qca4004_init_fota_ctx();
	
	if(filePath) {
		memcpy(qca4004_fota_ctxt_p->image_name, filePath, strlen(filePath)+1);
	}
	else {
		memcpy(qca4004_fota_ctxt_p->image_name, QCA4004_IMG_PATH_DFT, sizeof(QCA4004_IMG_PATH_DFT));
	}
	qca4004_fota_ctxt_p->flag = flag;
	
	if(flag & QCA4004_FOTA_IMG_CHECK) {
		ret = qca4004_fota_verify_image();
		QCA4004_DEBUG_LOG("image file verify %d\n", ret);
		if(ret != 0) 
			goto error;
	}
	
	qca4004_fota_ctxt_p->fota_state = QCA4004_FOTA_STATE_H2T_NOTIFY;
	ret = qca4004_fota_select_partition();
	QCA4004_DEBUG_LOG("select partition:%d, ret:%d\n",qca4004_fota_ctxt_p->retValue,ret);
	if(ret != 0 || qca4004_fota_ctxt_p->retValue <= 0) 		
		goto error;

	qurt_thread_sleep_ext(QURT_TIMER_NEXT_EXPIRY);
	qca4004_fota_ctxt_p->fota_state = QCA4004_FOTA_STATE_H2T_DATA;
	ret = qca4004_fota_data_transmit();
	QCA4004_DEBUG_LOG("data transmit ret:%d\n",ret);
	
	if(flag & QCA4004_FOTA_AUTO_RESET)
		qca4004_fota_ctxt_p->fota_state = QCA4004_FOTA_STATE_H2T_COMPLETE;
	else
		qca4004_fota_ctxt_p->fota_state = QCA4004_FOTA_STATE_H2T_RESET;
		
	if(flag & QCA4004_FOTA_VERSION_CHECK) {
		//TODO
		if(version == NULL || *version != fw_version)
			return QCA4004_ERR_FOTA_VER;
	}	
	if(version)
		*version = fw_version;
error:
	qca4004_deinit_fota_ctx();
	return ret;
}

void process_ota(uint8_t* rsp, uint16_t rsp_len)
{
	if(rsp_len == sizeof(uint32_t)) {
		memcpy(&(qca4004_fota_ctxt_p->retValue), rsp, sizeof(uint32_t));
		qurt_signal_set(&(qca4004_fota_ctxt_p->signalc), 0x01);
		QCA4004_DEBUG_LOG("ota %d\n",qca4004_fota_ctxt_p->retValue);
	}
}
