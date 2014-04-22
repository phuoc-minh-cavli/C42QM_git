/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        LOWI Wrapper

GENERAL DESCRIPTION
  This file contains the Wrapper around LOWI Client

  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
=============================================================================*/
#ifndef __LOWI_WRAPPER_H__
#define __LOWI_WRAPPER_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================================
 * Function description:
 *   This function initializes LOWI client.
 *   Please note that this function shall be called only once before destroy is called
 *
 * Parameters:
 *    ptr2ResultFunc Pointer to function to be called for passive scan results
 *    ptr2ResultFunc Pointer to function to be called for rtscts scan results
 *
 * Return value:
 *    error code: 0: success
 *                non-zero: error
 =============================================================================================*/
extern int lowi_wrapper_init ();

/*=============================================================================================
 * Function description:
 *   This function destroys LOWI client.
 *   Please note that this function shall be called once when the procC daemon starts
 *
 * Parameters:
 *    none
 *
 * Return value:
 *    error code: 0: success
 *                non-zero: error
 =============================================================================================*/
extern int lowi_wrapper_destroy ();

/*=============================================================================================
 * lowi_queue_discovery_scan_req
 *
 * Description:
 *   This function requests for discovery scan based on the parameters
 *
 * Parameters:
 * mode - 0 (FORCED_FRESH), 1 (NORMAL), 2 (CACHE), 3 (CACHE_FALLBACK)
 * band - 0 (2.4G), 1 (5G), 2 (ALL)
 * type - 0 (PASSIVE), 1 (ACTIVE)
 * meas_filter_sec - Measurement age filter in sec
 * fb_tol_sec - Fallback tolerance in Sec
 * uint32 num of Requests [Periodic scan]
 * uint32 Delay between Requests in sec [Periodic scan]
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_discovery_scan_req(uint8 mode, uint8 band,
    uint8 type, uint8 meas_filter_sec, uint8 fb_tol_sec, uint32 num_of_requests, uint32 delay);

/*=============================================================================================
 * lowi_queue_passive_scan_req
 *
 * Description:
 *   This function requests for passive scan on all channels.
 *
 * Parameters:
 * uint32 num of Requests [Periodic scan]
 * uint32 Delay between Requests in sec [Periodic scan]
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_passive_scan_req(uint32 num_of_requests, uint32 delay);

/*=============================================================================================
 * lowi_queue_capabilities_req
 *
 * Description:
 *   This function requests for driver capabilities from lowi
 *
 * Parameters: None
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_capabilities_req( void );

/*=============================================================================================
 * lowi_queue_async_discovery_scan_result_req
 *
 * Description:
 *   This function requests for async discovery scan results
 *
 * Parameters: uint32 : Timeout in seconds after which the request can be dropped
 * uint32 : 0 - Cancel subscription / 1 - Subscribe
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_async_discovery_scan_result_req( uint32 timeout, uint32 subscribe );

/*=============================================================================================
 * lowi_queue_events_req
 *
 * Description:
 *   This function requests for events
 *
 * Parameters:
 *  uint32 : 0 - Subscription / 1 - Query
 *  uint32 : 0 - Cancel subscription / 1 - Subscribe (Valid only if request type is set to subscription)
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_events_req(uint32 req_type, uint32 subscribe);

/*=============================================================================================
 * lowi_queue_ranging_req
 *
 * Description:
 *   This function requests ranging on APs discovered in the discovery scan
 *   Discovery Scan is trigerred first in Active mode
 *   After the results are received, Ranging scan is trigerred on all the APs
 *   received in the scan.
 *
 * Parameters:
 *   int: Ranging Type 0: Single Sided, 1: Dual Sided, 2: Best possible
 *   int: Report Type (0 - Basic, 1 - Detailed)
 *   uint32: Max number of nodes to range with
 *   int: Minimum RSSI of the Node for ranging (+ive value)
 *   uint32: Number of requests [For periodicity only]
 *	 uint32: Delay in sec between 2 requests [For periodicity only]
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_ranging_req(int rttType, int report_type,
  uint32 num_nodes, int min_rssi, uint32 num_of_req, uint32 delay);

/*=============================================================================================
 * lowi_queue_single_ranging_req
 *
 * Description:
 *   This function requests ranging on APs discovered in the discovery scan
 *   Discovery Scan is trigerred first in Active mode on all bands
 *   After the results are received, Ranging scan is trigerred on the APs
 *   requested in this request.
 *
 * Parameters:
 *   int: Ranging Type 0: Single Sided, 1: Dual Sided, 2: Best possible
 *   int: Report Type (0 - Basic, 1 - Detailed)
 *   uint8: mac_0 of the AP mac address
 *   uint8: mac_1 of the AP mac address
 *   uint8: mac_2 of the AP mac address
 *   uint8: mac_3 of the AP mac address
 *   uint8: mac_4 of the AP mac address
 *	 uint8: mac_5 of the AP mac address
 *   uint32: Number of requests [For periodicity only]
 *	 uint32: Delay in sec between 2 requests [For periodicity only]
 *
 * Return value:
 *   0 - success
 *   non-zero: error
 =============================================================================================*/
extern int lowi_queue_single_ranging_req(int rttType, int report_type,
  uint8 mac_0, uint8 mac_1, uint8 mac_2, uint8 mac_3, uint8 mac_4, uint8 mac_5,
  uint32 num_of_req, uint32 delay);

#ifdef __cplusplus
}
#endif

#endif /* __LOWI_WRAPPER_H__ */
