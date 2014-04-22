/******************************************************************************
 * Copyright © 2019 Qualcomm Technologies, Inc.
 * @file    cell_con_app_v2.h
 * @author  Qualcomm Technologies, Inc.
 * @brief   Contains the infomration needed for apn connection profile app stub
 *****************************************************************************/

void cell_con_init_v2(void);

int32_t cell_con_create_instance_v2(void);

int32_t cell_con_delete_instance_v2(void);

qapi_Status_t cell_con_get_v2(qapi_Net_LWM2M_Id_Info_v2_t *id_info);

void cell_con_app_deregister_v2(void);




