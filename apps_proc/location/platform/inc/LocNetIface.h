/******************************************************************************
 * LOC NET IFACE . H
 * Interface to sockets and network operations.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *******************************************************************************/
#ifndef __LOC_NET_IFACE_H__
#define __LOC_NET_IFACE_H__

#include <comdef.h>
#include <LocDataTypes.h>
#include <string>
#include <qapi_dss.h>
#include <LocUtilTimer.h>

// APIs implemented in C
#ifdef __cplusplus
extern "C" {
#endif

#include <qapi_socket.h>

#ifdef __cplusplus
}
#endif

using namespace std;

/*---------------------------------------------------------------------------
 * Macros for endian-ness conversion
 *-------------------------------------------------------------------------*/
#define htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

#define htons(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
                  (((uint16)(x) & 0xff00) >> 8)))

#define ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
                  (((uint32)(x) & 0x0000ff00U) <<  8) | \
                  (((uint32)(x) & 0x00ff0000U) >>  8) | \
                  (((uint32)(x) & 0xff000000U) >> 24)))

#define ntohs(x) \
        ((uint16)((((uint16)(x) & 0x00ff) << 8) | \
        (((uint16)(x) & 0xff00) >> 8)))


/* Function Pointer: LocNetIface_callback
 *
 * Invoked to client on receiving DSS callback.
 * */
typedef void (*LocNetIface_callback)(bool dataCallSuccess, void* dataPtr);


/* Class : LocNetIface
 *
 * Abstracts interaction with DSS library to setup a data call.
 * */
class LocNetIface {

public:
    // Constructor : accepts client callback and opaque data pointer
    LocNetIface(LocNetIface_callback cb, void* cbDataPtr);

    // Destructor : would terminate the data call
    ~LocNetIface();

    /* Method: startDataCallBlocking()
     *
     * Start the data call and wait for NET_IS_CONN indication from data layer.
     * Times out and returns after the specified timeout if no indication is received.
     *
     * Returns 0 for success, -1 for failure.
     * */
    int startDataCallBlocking(uint32_t timeoutSec);

    /* Method: stopDataCallBlocking()
     *
     * Stop the ongoing data call and waits for NO_NET indication from data layer.
     * Times out and returns after the specified timeout if no indication is received.
     *
     * Returns 0 for success, -1 for failure.
     * */
    int stopDataCallBlocking(uint32_t timeoutSec);

    /* Method: getSocketAddress()
     *
     * Call DNS resolution function getHostByName() for passed in host name.
     * and populates the sockaddr struct and adress size passed in.
     * Returns true for success, false for failure.
     * */
    bool getSocketAddress(const char* namePtr, struct sockaddr &server, size_t &serverSize, uint16 port);

    /* Method: getHostByName()
     *
     * Perform DNS resolution for passed in host name.
     * Uses DNS module API to resolve the char* name and populates the
     * resolved_addr struct pointer passed in.
     *
     * Returns 0 for success, -1 for failure.
     * */
    int getHostByName(const char* namePtr, struct ip46addr* resolvedAddrPtr);

    /* Method: getDssIPFamily()
     *
     * Returns IP family of existing data call.
     *
     * Returns IP family.
     * */
    uint16 getDssIPFamily();

    /* Method: getDssHandle()
     *
     * Returns handle of the  data call.
     * */
    qapi_DSS_Hndl_t getDssHandle();

    qapi_DSS_Addr_Info_t getAddrInfo();
    char* getIfaceName();

    // Read IP version to use for call bringup from dss.conf file
    int getIpVersionFromConfig();
private:
    LocNetIface_callback mClientCb;
    void* mClientCbDataPtr;

    /* Method: startDataCall()
     *
     * Sets up a data call. Fetches APN to use from /location/location.conf
     *
     * Returns 0 for success, -1 for failure.
     * */
    int startDataCall();

    /* Method: stopDataCall()
     *
     * Stop the ongoing data call.
     *
     * Returns 0 for success, -1 for failure.
     * */
    int stopDataCall();

    // Read APN name from dss.conf file
    string getApnNameFromConfig();

    // Read DNS resolution retry count from dss.conf
    uint32 getDnsResolRetryCountFromConfig();

    // Perform setup steps for DNS module, before it can be used for
    // hostname resolution
    int setupDnsModule();

    // Cleanup dns module
    void cleanupDnsModule();

    // Force stop the data call
    int stopDataCall(bool forceStop);

    /* Params related to DSS call */
    bool mDssInitDone;
    qapi_DSS_Hndl_t mDssHandle;
	qapi_DSS_Addr_Info_t  mAddrInfo;
    uint16 mDssIPFamily;
    bool mDssStartCallPending;
    bool mDssStopCallPending;

    #define MAX_IFACE_NAME_LEN 64
    char mIfaceName[MAX_IFACE_NAME_LEN + 4];
    bool mDeviceNameFetched;

    // Timer for blocking start/stop calls
    LocUtilTimer mBlockingStartTimer;
    LocUtilTimer mBlockingStopTimer;

    // Callback registered with DSS
    static void dssCb(
            qapi_DSS_Hndl_t         hndl,          /* Handle for which this event is associated */
            void              *user_data,          /* Application-provided user data */
            qapi_DSS_Net_Evt_t      evt,           /* Event identifier */
            qapi_DSS_Evt_Payload_t *payload_ptr);

    // Callback for blocking stop timer
    static void sBlockingStopTimerCb(void* locnetifaceInstPtr);
    static void sBlockingStartTimerCb(void* locnetifaceInstPtr);

	/* Read DNS timeout from dss.conf file*/
	uint32 getDNSTimeoutSecFromConfig();
};

#endif //__LOC_NET_IFACE_H__
