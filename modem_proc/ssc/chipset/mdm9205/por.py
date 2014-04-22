#==============================================================================
# POR Config Script
#
# Copyright (c) 2019 - 2020 Qualcomm Technologies Inc.  All Rights Reserved
#==============================================================================

include_sensor_vendor_libs = []
exclude_libs = []

def exists(env):
   return env.Detect('por')

def generate(env):

    if 'SENSORS_DD_DEV_FLAG' in env:
      env.Append(CPPDEFINES = ['SENSORS_DD_DEV_FLAG'])
    if 'SENSORS_ADD_SHARED_OEM1' in env:
      env.Append(CPPDEFINES = ['SENSORS_ADD_SHARED_OEM1'])
    if 'SENSORS_ADD_SHARED_ACCEL_CAL' in env:
      env.Append(CPPDEFINES = ['SENSORS_ADD_SHARED_ACCEL_CAL'])
    if 'SENSORS_ALGO_DEV_FLAG' in env:
      env.Append(CPPDEFINES = ['SENSORS_ALGO_DEV_FLAG'])
    if 'USES_CORE_MEMORY_OPT_CHRE' not in env:
      env.Append(CPPDEFINES = ['SNS_ISLAND_INCLUDE_DIAG'])  

    #POR sensors list
    include_sensor_vendor_libs.extend(['lsm6dsm','sns_lps22hx','sns_tmd2725','sns_shtw2'])
        
    exclude_libs.extend(['sns_aont','sns_basic_gestures','sns_bring_to_ear','sns_ccd','sns_cmc','sns_device_orient','sns_distance_bound','dae_sensor',
       'sns_dpc','sns_facing','sns_fmv','sns_game_rv','sns_geomag_rv','sns_gravity','sns_gyro_cal','sns_gyro_rot_matrix','sns_mag_cal','sns_mcmd','sns_oem1',
       'sns_psmd','sns_rmd','sns_rotv','sns_multishake','sns_threshold','sns_tilt','sns_tilt_to_wake','Low_lat_client_manager','cm_qsocket','sns_device_mode','pram','spi',
       'sns_pah_common', 'sns_pah_8131_core', 'sns_wrist_tilt', 'sns_wrist_pedometer','sns_pah_8131'])
    env.Replace(SSC_INCLUDE_SENS_VEND_LIBS=include_sensor_vendor_libs)
    env.Replace(SSC_EXCLUDE_LIBS=exclude_libs)

    #TODO: Avoid using -Wno-unused-parameter
    env.Append(CFLAGS = '-Wno-unused-parameter')
    env.Append(CFLAGS = '-mllvm -disable-shrink-wrap-common-args')
    env.AddUsesFlags('SNS_NO_ISLAND_PTR_CHECKS')
    env.Append(CPPDEFINES = ['SSC_TARGET_MDM9205'])
    env.Append(CPPDEFINES = ['SNS_USES_MSG_API_V4'])
    env.Append(CPPDEFINES = ['SSC_TARGET_HEXAGON'])
    env.Append(CPPDEFINES = ['SSC_TARGET_HEXAGON_CORE_SLPI_1_0'])
    env.Append(CPPDEFINES = ['SSC_TARGET_HEXAGON_CORE_SPI_NOT_SUPPORTED'])
    env.Append(CPPDEFINES = ['SNS_USE_LOCAL_CLK_SRC'])
    env.Append(CPPDEFINES = ['SSC_TARGET_NO_F3_TRACE'])
    env.Append(CPPDEFINES = ['SSC_TARGET_HEXAGON_NO_ISLAND'])
    env.Append(CPPDEFINES = ['SSC_TARGET_NO_UTLMM_GPIO'])
    env.Append(CPPDEFINES = ['SSC_TARGET_NO_CCD'])
    env.Append(CPPDEFINES = ['SSC_TARGET_NO_DIRECT_GPIO_INT_SUPPORT'])
    env.Append(CPPDEFINES = ['SNS_AVOID_HANDLING_INTERRUPT_DURING_PC'])
    env.Append(CPPDEFINES = ['SNS_SSC_BW_DEF_VOTE = 170000000'])
    env.Append(CPPDEFINES = ['SNS_BANDWIDTH_VOTING_ENABLED'])
    if env.IsTagEnable('MODEM_SENSORONLY'):
      env.Append(CPPDEFINES = ['SNS_MODEM_SENSORONLY'])
    #no hlos client for mdm based sensors and following helps for QMI testpro communication 
    
    env.Append(CPPDEFINES = ['SNS_DISABLE_REGISTRY'])
    env.AddUsesFlags(['SNS_DISABLE_REGISTRY'])
    env.AddUsesFlags(['SNS_DISABLE_TEST_CLIENT'])

