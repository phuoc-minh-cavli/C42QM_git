/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "rpm_config.h"

static const unsigned SUPPORTED_CLASSES   = 24;
static const unsigned SUPPORTED_RESOURCES = 128;

//
// BEGIN config data; should migrate to the system enumeration data method
//
static SystemData temp_config_data =
{
#if (DAL_CONFIG_TARGET_ID == 0x9205)
    .num_ees = 3, // 2 EE's, [apps, modem]
#else
    .num_ees = 5, // 5 EE's, [apps, modem, pronto, tz, qdsp]
#endif

    .ees    = (EEData[] ) {
        [0] = {
            .remote_ss = "apss",
            .glink_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = TASK_PRIORITY_APPS,
            .wakeupInt = (1 << 5) | (1 << 7),

#if ((DAL_CONFIG_TARGET_ID == 0x9645) || (DAL_CONFIG_TARGET_ID == 0x9607) || (DAL_CONFIG_TARGET_ID == 0x9205))
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) {  1 },
                             .bringupAcks  = (unsigned[]) { 16 },
                             .shutdownInts = (unsigned[]) {  0 },
                             .shutdownAcks = (unsigned[]) {  0 },
                         },
#else
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) {  15 },
                             .bringupAcks  = (unsigned[]) {  20 },
                             .shutdownInts = (unsigned[]) {  14 },
                             .shutdownAcks = (unsigned[]) {   4 },
                         },
#endif
        },
        [1] = {
            .remote_ss = "mpss",
            .glink_fifo_sz = 1024,
            .ee_buflen = 1024,
            .priority  = TASK_PRIORITY_MODEM,
            .wakeupInt = (1 << 13) | (1 << 15),
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) { 25 },
                             .bringupAcks  = (unsigned[]) { 22 },
                             .shutdownInts = (unsigned[]) { 24 },
                             .shutdownAcks = (unsigned[]) {  6 },
                         },
        },
#if (DAL_CONFIG_TARGET_ID != 0x9205) 
        [2] = {
            .remote_ss = "lpass",
            .glink_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = TASK_PRIORITY_QDSP,
            .wakeupInt = (1 << 17) | (1 << 19),
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) { 31 },
                             .bringupAcks  = (unsigned[]) { 23 },
                             .shutdownInts = (unsigned[]) { 30 },
                             .shutdownAcks = (unsigned[]) {  7 },
                         },
        },
#else
        [2] = {
            .remote_ss = "tz",
            .glink_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = TASK_PRIORITY_TZ,
            .wakeupInt = 0,
            .spm       = {
                             .numCores = 0,
                             .bringupInts  = (unsigned[]) { 31 },
                             .bringupAcks  = (unsigned[]) { 23 },
                             .shutdownInts = (unsigned[]) { 30 },
                             .shutdownAcks = (unsigned[]) {  7 },
                         },
        },
#endif
#if (DAL_CONFIG_TARGET_ID != 0x9205)
        [3] = {
            .remote_ss = "tz",
            .glink_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = TASK_PRIORITY_TZ,
            .wakeupInt = 0,
            .spm       = {
                             .numCores = 0,
                             .bringupInts  = (unsigned[]) { 31 },
                             .bringupAcks  = (unsigned[]) { 23 },
                             .shutdownInts = (unsigned[]) { 30 },
                             .shutdownAcks = (unsigned[]) {  7 },
                         },
        },
        [4] = {
            .remote_ss = "cdsp",
            .glink_fifo_sz = 1024,
            .ee_buflen = 256,
            .priority  = TASK_PRIORITY_CDSP,
            .wakeupInt = (1 << 1) | (1 << 3),
            .spm       = {
                             .numCores = 1,
                             .bringupInts  = (unsigned[]) { 55 },
                             .bringupAcks  = (unsigned[]) { 21 },
                             .shutdownInts = (unsigned[]) { 54 },
                             .shutdownAcks = (unsigned[]) {  5 },
                         },
        },
#endif
    },

    .supported_classes   = SUPPORTED_CLASSES,
    .supported_resources = SUPPORTED_RESOURCES,
    .classes             = (ResourceClassData[SUPPORTED_CLASSES]) { 0 },
    .resources           = (ResourceData[SUPPORTED_RESOURCES])    { 0 },
    .resource_seeds      = (int16_t[SUPPORTED_RESOURCES])         { 0 },
};
//
// END config data
//

SystemData * const rpm = &temp_config_data;

