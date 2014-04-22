/* Copyright (c) 2016-2019, 2021 Qualcomm Technologies, Inc. and/or its subsidiaries.
   All rights reserved.
   Confidential - Qualcomm Technologies, Inc. - May Contain Trade Secrets
*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

#ifndef __ATHDEFS_H__
#define __ATHDEFS_H__

/*
 * This file contains definitions that may be used across both
 * Host and Target software.  Nothing here is module-dependent
 * or platform-dependent.
 */


/*
 * Generic error codes that can be used by hw, sta, ap, sim, dk
 * and any other environments. Since these are enums, feel free to
 * add any more codes that you need.
 */

typedef enum {
    A_ERROR = -1,               /**< Generic error return. */
    A_OK = 0,                   /**< Success. */
                                /**< Following values start at 1. */
    A_DEVICE_NOT_FOUND,         /**< Not able to find PCI device. */
    A_NO_MEMORY,                /**< Not able to allocate memory, not available. */
    A_MEMORY_NOT_AVAIL,         /**< Memory region is not free for mapping. */
    A_NO_FREE_DESC,             /**< No free descriptors available. */
    A_BAD_ADDRESS,              /**< Address does not match descriptor. */
    A_WIN_DRIVER_ERROR,         /**< Used in NT_HW version, if problem at init. */
    A_REGS_NOT_MAPPED,          /**< Registers not correctly mapped. */
    A_EPERM,                    /**< Not superuser. */
    A_EACCES,                   /**< Access denied. */
    A_ENOENT,                   /**< No such entry, search failed, etc. */
    A_EEXIST,                   /**< The object already exists (can't create). */
    A_EFAULT,                   /**< Bad address fault. */
    A_EBUSY,                    /**< Object is busy. */
    A_EINVAL,                   /**< Invalid parameter. */
    A_EMSGSIZE,                 /**< Inappropriate message buffer length. */
    A_ECANCELED,                /**< Operation canceled. */
    A_ENOTSUP,                  /**< Operation not supported. */
    A_ECOMM,                    /**< Communication error on send. */
    A_EPROTO,                   /**< Protocol error. */
    A_ENODEV,                   /**< No such device. */
    A_EDEVNOTUP,                /**< Device is not UP. */
    A_NO_RESOURCE,              /**< No resources for requested operation. */
    A_HARDWARE,                 /**< Hardware failure. */
    A_PENDING,                  /**< Asynchronous routine; will send up results later (typically in callback). */
    A_EBADCHANNEL,              /**< The channel cannot be used. */
    A_DECRYPT_ERROR,            /**< Decryption error. */
    A_PHY_ERROR,                /**< RX PHY error. */
    A_CONSUMED,                 /**< Object was consumed. */
    A_CLONE,                    /**< The buffer is cloned. */
    A_HW_CONFIG_ERROR,          /**< Error in GPIO operation. */
    A_SOCKCXT_NOT_FOUND,        /**< Socket context not found. */
    A_UNKNOWN_CMD,              /**< Unknown socket command. */
    A_SOCK_UNAVAILABLE,         /**< Socket limit reached. */
    A_MEMFREE_ERROR             /**< Error while freeing resource. */
} A_STATUS;

#define A_SUCCESS(x)        (x == A_OK)
#define A_FAILED(x)         (!A_SUCCESS(x))

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef ATH_TARGET
typedef enum {
  USER,               /**< Max Perf set by user. */
  WPS,                /**< Max Perf set by WPS.  */
  P2P,                /**< Max Perf set by P2P.  */
  SOFTAP,
  SRCALL,
  DHCP,
  TCPL1,
  TCPL2,
  TKIP,
  RAWMODE,
  PWR_MAX = 32,
 } A_PWR_MODULE;

#define EXPECTED_REF_CLK_AR4100 26000000
#define EXPECTED_REF_CLK_AR400X 40000000
#endif

#endif /* __ATHDEFS_H__ */
