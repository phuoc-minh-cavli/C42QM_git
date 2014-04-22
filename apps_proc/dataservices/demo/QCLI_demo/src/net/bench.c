/*!
   @file
   bench.c
 
   @brief
   Header for performing all the data transefer related operations.
*/
 
/*===========================================================================
 
   Copyright (c) 2017-2023 Qualcomm Technologies, Inc. All Rights Reserved
 
   Qualcomm Technologies Proprietary
 
   Export of this technology or software is regulated by the U.S. Government.
   Diversion contrary to U.S. law prohibited.
 
   All ideas, data and information contained in or disclosed by
   this document are confidential and proprietary information of
   Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
   By accepting this material the recipient agrees that this material
   and the information contained therein are held in confidence and in
   trust and will not be used, copied, reproduced in whole or in part,
   nor its contents revealed in any manner to others without the express
   written permission of Qualcomm Technologies, Inc.
 
===========================================================================*/


/* Copyright (c) 2011-2015.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qapi_status.h"
#include "qapi_ssl.h"
#include "bench.h"
#include "qapi_netservices.h"
#include "qapi_socket.h"
#include "qapi_ns_gen_v4.h"
#include "qapi_ns_gen_v6.h"
#include "qcli.h"
#include "qurt_thread.h"
/*+RAW IPv4*/
#include "qapi_ns_utils.h"
/*+configure exclude list with file*/
#include "fs_public.h"
/*-configure exclude list with file*/
/*-RAW IPv4*/
//#define UDP_RX_DEBUG
//#define UDP_RX_TS_DEBUG
//#define TCP_RX_DEBUG
//#define ZCOPY_UDP_RX_DEBUG
//#define ZCOPY_TCP_RX_DEBUG
//#define UDP_TX_DEBUG
//#define TCP_TX_DEBUG
//#define SEND_ACK_DEBUG
//#define TCP_RX_RETRY_AFTER_FIN
//#define CHECK_PACKET_SIZE
#define USE_SERVER_STATS
//#undef INCLUDE_SSLDEMO
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

extern QCLI_Command_t net_cmd_list[];

uint8_t v6_enable_flag[NO_OF_APPS];
static uint32_t port_in_use[NO_OF_APPS];        //Used to prevent two streams from using same port
static THROUGHPUT_CXT *p_rxtCxt[NO_OF_APPS];
uint8_t bench_quit[NO_OF_APPS][2];
qurt_thread_t tid_rx[NO_OF_APPS] = {0};
qurt_thread_t tid_tx[NO_OF_APPS] = {0}; 

extern bool thrd_running[NO_OF_APPS];
extern  sock_opt option[NO_OF_APPS];

 bool enable_us_delay = 0;

#ifdef FEATURE_9x07_CAT4
int packet_factor = 34;
int udp_packet_factor = 30;
#endif
int udp_normal_delay = 70;
#ifndef timeval
struct timeval
{
   long tv_sec;     /* seconds */
   long tv_usec;    /* and microseconds */
};
#endif

#ifdef INCLUDE_SSLDEMO
qapi_Net_SSL_Role_t ssl_role[NO_OF_APPS][MAX_SSL_INST];
SSL_INST ssl_inst[NO_OF_APPS][MAX_SSL_INST];
#endif /* INCLUDE_SSLDEMO */

#define BENCH_RX 0
#define BENCH_TX 1



 /*+RAW IPv4*/
 struct ip	 {
	unsigned char	 ip_ver_ihl;	/* 4 bit version, 4 bit hdr len in 32bit words */
	unsigned char	 ip_tos;		/* Type of Service */
	unsigned short  ip_len;		/* Total packet length including header */
	unsigned short  ip_id; 		/* ID for fragmentation */
	unsigned short  ip_flgs_foff;	/* mask in flags as needed */
	unsigned char	 ip_time;		/* Time to live (secs) */
	unsigned char	 ip_prot;		/* protocol */
	unsigned short  ip_chksum; 	/* Header checksum */
	ip_addr  ip_src;		/* Source name */
	ip_addr  ip_dest;		/* Destination name */ 
};
struct udp
{
  unsigned short  ud_srcp;    /* source port */
  unsigned short  ud_dstp;    /* dest port */
  unsigned short  ud_len;	   /* length of UDP packet */
  unsigned short  ud_cksum;   /* UDP checksum */
};
/*
unsigned short csum(unsigned short *buf, int nwords)
{       //
        unsigned long sum;
        for(sum=0; nwords>0; nwords--)
                sum += *buf++;
        sum = (sum >> 16) + (sum &0xffff);
        sum += (sum >> 16);
        return (unsigned short)(~sum);
}
*/
/*+RAW IPv6*/
ip6_addr ip6allzero = { /* :: */
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};
ip6_addr ip6loop = { /* ::1 */
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};
volatile int pkt_demux_factor = 10;
/*
 * THE IPv6 header
 */
struct ipv6
{
   unsigned char   ip_ver_flo[4];    /* 4 bits version (6), 8 bits class, & flow label */
   unsigned short  ip_len;           /* payload length */
   unsigned char   ip_nexthdr;       /* next header type 6 = TCP, etc */
   unsigned char   ip_hopcount;      /* hops left until expires */
   ip6_addr ip_src;           /* source and dest addresses */
   ip6_addr ip_dest;
};

/*-RAW IPv6*/
static void process_raw_ip_rx_data(char * app_data , int data_len);
static void build_IP_packet(THROUGHPUT_CXT *p_tCxt, int hdrincl, int raw_rx);
static int set_IP_exclude_list(THROUGHPUT_CXT *p_tCxt, unsigned long addr, int size, int debug, int incl_addr, int file_source, int foreign); 
static int get_IP_exclude_list(THROUGHPUT_CXT *p_tCxt, int debug, int foreign); 
/*+RAW IPv6*/
static void build_IPv6_packet(THROUGHPUT_CXT *p_tCxt, int hdrincl, int raw_rx);
static int set_IP6_exclude_list(THROUGHPUT_CXT *p_tCxt, ip6_addr * addr, int size, int debug, int incl_addr, int file_source, int foreign);
static int get_IP6_exclude_list(THROUGHPUT_CXT *p_tCxt, int debug, int foreign);
static void print_IPv6_address(uint8 addr[16]);
/*-RAW IPv6*/
extern u_long 
inet_addr(char *str);
/*+configure exclude list with file*/
extern int data_getline(char **lineptr, size_t *n, int stream);
/*-configure exclude list with file*/
/*-RAW IPv4*/
#ifdef INCLUDE_SSLDEMO

/************************************************************************
 ************************************************************************/
	static int ssl_rx_setup(THROUGHPUT_CXT *p_tCxt, SSL_INST *ssl, int app_id)
	{
		int result;
		int dtls = 0;
	
		if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
		{
			// Create SSL connection object
			if((ssl_inst[app_id][SSL_SERVER_INST].config_set == 1) && ((ssl_inst[app_id][SSL_SERVER_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_0) || (ssl_inst[app_id][SSL_SERVER_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_2)))
			{
				dtls = 1;
				ssl->ssl = qapi_Net_SSL_Con_New(ssl->sslCtx, QAPI_NET_SSL_DTLS_E);
			}
			else
				ssl->ssl = qapi_Net_SSL_Con_New(ssl->sslCtx, QAPI_NET_SSL_TLS_E);
	
			if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
			{
				QCLI_Printf(qcli_net_handle, "ERROR: Unable to create SSL context\n");
				return -1;
			}
	
			// configure the SSL connection
			if (ssl->config_set)
			{
				result = qapi_Net_SSL_Configure(ssl->ssl, &ssl->config);
				if (result < QAPI_OK)
				{
					QCLI_Printf(qcli_net_handle, "ERROR: SSL configure failed (%d)\n", result);
					goto ssl_error;
				}
			}
	
		}
	
		// Add socket handle to SSL connection
		if(dtls)
		result = qapi_Net_SSL_Fd_Set(ssl->ssl, p_tCxt->sock_local);
		else
		result = qapi_Net_SSL_Fd_Set(ssl->ssl, p_tCxt->sock_peer);
		if (result < QAPI_OK)
		{
			QCLI_Printf(qcli_net_handle, "ERROR: Unable to add socket handle to SSL\n");
			goto ssl_error;
		}
	
		// SSL handshake with server
		result = qapi_Net_SSL_Accept(ssl->ssl);
		if (result != QAPI_SSL_OK_HS)
		{
			QCLI_Printf(qcli_net_handle, "ERROR: SSL accept failed (%d)\n", result);
			goto ssl_error;
		}
	
		 QCLI_Printf(qcli_net_handle, "SSL accept Success(%d)\n", result);
		
	
		return 0;
	
	ssl_error:
		qapi_Net_SSL_Shutdown(ssl->ssl);
		ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
		return -1;
	}

#endif /* INCLUDE_SSLDEMO */


/************************************************************************
* NAME: check_test_time
*
* DESCRIPTION: If test mode is time, check if current time has exceeded
* test time limit
* Parameters: pointer to throughput context
************************************************************************/
static uint32_t check_test_time(THROUGHPUT_CXT *p_tCxt)
{
    uint32_t sec_interval = (p_tCxt->last_time.seconds - p_tCxt->first_time.seconds);
    uint32_t last_time_ms   = (p_tCxt->last_time.seconds * 1000 + p_tCxt->last_time.milliseconds);
    uint32_t first_time_ms  = (p_tCxt->first_time.seconds * 1000 + p_tCxt->first_time.milliseconds);
    uint32_t total_interval;

    if (sec_interval < p_tCxt->params.tx_params.tx_time || last_time_ms < first_time_ms)
        return 0;

    total_interval = last_time_ms - first_time_ms;

    if (total_interval > p_tCxt->params.tx_params.tx_time*1000)
        return 1;
    else
        return 0;
}


/************************************************************************
 * The pattern:
 * < ---          len                       -->
 * 00 01 02 03 .. FE FF 00 01 02 .. FF 00 01 ..
 * A
 * |
 * p
************************************************************************/
static void add_pattern(char *p, int len)
{
    int n, ml;

    while (len)
    {
        ml = min(256, len);
        for (n = 0; n < ml; ++n, ++p)
        {
            *p = (char)n;
        }
        len -= ml; 
    }
}

/************************************************************************
************************************************************************/
static void print_test_results(THROUGHPUT_CXT *p_tCxt)
{
    /* Print throughput results.*/
    unsigned long long total_bytes;
    uint32_t total_interval;
    uint32_t throughput;
    uint32_t last_time_ms   = (p_tCxt->last_time.seconds * 1000 + p_tCxt->last_time.milliseconds);
    uint32_t first_time_ms  = (p_tCxt->first_time.seconds * 1000 + p_tCxt->first_time.milliseconds);

    if (last_time_ms < first_time_ms)
    {
        QCLI_Printf(qcli_net_handle, "Error in calculating throughput.\n");
        return;
    }

    total_interval = last_time_ms - first_time_ms;
    total_bytes = p_tCxt->bytes;

    if (total_interval > 0)
    {
        /*No test was run, or test is terminated, print results of previous test*/
        if (p_tCxt->bytes == 0)
        {
            total_bytes     = p_tCxt->last_bytes;
            total_interval  = p_tCxt->last_interval;
            throughput      = p_tCxt->last_throughput;
        }
        else
        {
            /* Take care of wrap around cases. If number of bytes exceeds 
               0x3FFFFFFFFFFFFFLL, it exceeds 64 bits and wraps around
               resulting in wrong throughput number */
            if (p_tCxt->bytes <= 0x3FFFFFFFFFFFFFLL)
            {
                /* (N/T) bytes/ms = (1000/128)*(N/T) Kbits/s */
                throughput = (p_tCxt->bytes*125) / (total_interval*16); /* in Kb/sec */
            }
            else
            {
                uint32_t sec_interval;
                unsigned long long bytes;
                unsigned long long kbytes;

                /* Convert bytes to kb and divide by seconds for this case */
                sec_interval = total_interval / 1000; 
                kbytes  = p_tCxt->bytes / 1024;
                bytes   = p_tCxt->bytes % 1024;
                throughput = ((kbytes*8) / sec_interval) +
                    ((bytes*8/1024) / sec_interval); /* Kb/sec */
            }
            p_tCxt->last_interval   = total_interval;
            p_tCxt->last_bytes      = p_tCxt->bytes;
            p_tCxt->last_throughput = throughput;
        }
    }
    else
    {
        throughput = 0;
    }

    switch(p_tCxt->test_type)
    {
        case UDP_TX:
            QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "UDP Transmit");
            break;

        case UDP_RX:
            QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "UDP Receive");
            break;

        case TCP_TX:
            QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "TCP Transmit");
            break;

        case TCP_RX:
            QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "TCP Receive");
            break;

        case SSL_TX:
            QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "SSL Transmit");
            break;

        case SSL_RX:
            QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "SSL Receive");
            break;
/*+RAW IPv4*/			
		case RAW_IP_RX:
			QCLI_Printf(qcli_net_handle, "\nResults for %s test:\n\n", "RAW Receive");
			break;
/*-RAW IPv4*/
    }

    QCLI_Printf(qcli_net_handle, "\t%llu KBytes %llu bytes (%llu bytes) in %u seconds %u ms (%u miliseconds)\n\n",
            total_bytes/1024, total_bytes%1024, total_bytes, total_interval/1000, total_interval%1000, total_interval);
    QCLI_Printf(qcli_net_handle, "\t throughput %u Kbits/sec\n", throughput);
}

/************************************************************************
* NAME: qca_tcp_tx
*
* DESCRIPTION: Start TCP Transmit test.
************************************************************************/
static void qca_tcp_tx(THROUGHPUT_CXT *p_tCxt, int app_id)
{
    struct sockaddr_in foreign_addr;
    struct sockaddr_in6 foreign_addr6;
    struct sockaddr_in local_addr;
    struct sockaddr_in6 local_addr6;
	#ifdef FEATURE_9x07_CAT4
     int pktnum=0;
	#endif

    struct sockaddr *addr;
    uint32_t addrlen;
    
    struct sockaddr *to;
    uint32_t tolen;
    char ip_str[48];
    uint32_t packet_size = p_tCxt->params.tx_params.packet_size;
    uint32_t cur_packet_number;
    uint32_t buffer_offset;
    uint32_t is_test_done;
    struct linger linger_option;
	struct linger get_linger_option ={0};
    int32_t error = 0;
    int opt_val=0;

#if INCLUDE_SSLDEMO
    int32_t result;
    SSL_INST *ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
#endif


    uint32_t i = 0, j = 0;
    uint32_t netbuf_id;
    int send_flag;
    int family;
    uint32_t bytes_to_send;
    int bytes_sent;

    bench_quit[app_id][BENCH_TX] = 0;
    memset(ip_str, 0, sizeof(ip_str));

#ifdef CHECK_PACKET_SIZE
    if (packet_size > CFG_PACKET_SIZE_MAX_TX)
        packet_size = CFG_PACKET_SIZE_MAX_TX;
#endif

    if (v6_enable_flag[app_id])
    {
        family = AF_INET6; 
        inet_ntop(family, &p_tCxt->params.tx_params.v6addr[0], ip_str, sizeof(ip_str));
        

        memset(&foreign_addr6, 0, sizeof(foreign_addr6));
        memcpy(&foreign_addr6.sin_addr, p_tCxt->params.tx_params.v6addr, sizeof(foreign_addr6.sin_addr));
        foreign_addr6.sin_port     = htons(p_tCxt->params.tx_params.port);
        foreign_addr6.sin_family   = family;
        foreign_addr6.sin_scope_id = p_tCxt->params.tx_params.scope_id;

        to = (struct sockaddr *)&foreign_addr6;
        tolen = sizeof(foreign_addr6);


        if(p_tCxt->params.tx_params.bind_enabled)
        {
           addr = (struct sockaddr *)&local_addr6;
           addrlen = sizeof(struct sockaddr_in6);
       
          memset(&local_addr6, 0, sizeof(local_addr6));
          memcpy(&local_addr6.sin_addr, p_tCxt->params.tx_params.sv6addr, sizeof(local_addr6.sin_addr));
          local_addr6.sin_port = htons(p_tCxt->params.tx_params.tx_port);
          local_addr6.sin_family = family;
        }
    }
    else
    {
        family = AF_INET; 
        inet_ntop(family, &p_tCxt->params.tx_params.ip_address, ip_str, sizeof(ip_str));

        memset(&foreign_addr, 0, sizeof(foreign_addr));
        foreign_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.ip_address;
        foreign_addr.sin_port           = htons(p_tCxt->params.tx_params.port);
        foreign_addr.sin_family         = family;

        to = (struct sockaddr *)&foreign_addr;
        tolen = sizeof(foreign_addr);

        if(p_tCxt->params.tx_params.bind_enabled)
        {
          addr = (struct sockaddr *)&local_addr;
          addrlen = sizeof(struct sockaddr_in);
          memset(&local_addr, 0, sizeof(local_addr));
          local_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.sip_address;
          local_addr.sin_port = htons(p_tCxt->params.tx_params.tx_port);
          local_addr.sin_family = family;
        }
    }


    /* ------ Start test.----------- */
    QCLI_Printf(qcli_net_handle, "\n**********************************************************\n");



#if INCLUDE_SSLDEMO
    QCLI_Printf(qcli_net_handle, "IOT %s TX Test\n", p_tCxt->test_type == SSL_TX ? "SSL" : "TCP");
#else
    QCLI_Printf(qcli_net_handle, "IOT TCP TX Test\n" );
#endif


    QCLI_Printf(qcli_net_handle, "**********************************************************\n");
    QCLI_Printf(qcli_net_handle, "Remote IP addr: %s\n", ip_str);
    QCLI_Printf(qcli_net_handle, "Remote port: %d\n", p_tCxt->params.tx_params.port);
    QCLI_Printf(qcli_net_handle, "Message size: %d\n", p_tCxt->params.tx_params.packet_size);
    QCLI_Printf(qcli_net_handle, "Number of messages: %d\n", p_tCxt->params.tx_params.packet_number);
    QCLI_Printf(qcli_net_handle, "Delay in ms: %d\n", p_tCxt->params.tx_params.interval);
    QCLI_Printf(qcli_net_handle, "Press Ctrl+C to cancel\n");
    QCLI_Printf(qcli_net_handle, "**********************************************************\n");

    QCLI_Printf(qcli_net_handle, "Going to set:\n IRT = %d  MR = %d  OT = %d  MSS= %d \n",option[app_id].irt,
                          option[app_id].max_rxt, option[app_id].overall_timer, option[app_id].mss);

    QCLI_Printf(qcli_net_handle, "IDLE = %d  INTVL = %d  CNT = %d \n",option[app_id].keep_idle,
                          option[app_id].keep_intvl, option[app_id].keep_cnt);

    /*Reset all counters*/
    cur_packet_number = 0;
    buffer_offset = 0;

    /* Create socket */
    if ((p_tCxt->sock_peer = qapi_socket(family, SOCK_STREAM, 0)) == A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Unable to create socket\n");
        goto ERROR_1;
    }


    if(p_tCxt->params.tx_params.bind_enabled)
    {
        /* Bind socket */
        if (qapi_bind(p_tCxt->sock_peer, addr, addrlen) == (UINT)A_ERROR)
       {
          QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
          goto ERROR_2;
       }
    }
    
    if(option[app_id].enable_sackopt)
    {
	   if (qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET , SO_TCPSACK , &option[app_id].enable_sackopt, sizeof(option[app_id].enable_sackopt)) != QAPI_OK)
       {
          QCLI_Printf(qcli_net_handle,"set TCPSACK option failed enable_sackopt[%d] = %d\n" ,app_id , option[app_id].enable_sackopt);
          goto ERROR_1;
       }
	   else
	   {
          QCLI_Printf(qcli_net_handle,"set TCPSACK option returned success enable_sackopt[%d] = %d\n" ,app_id , option[app_id].enable_sackopt);
	   }
   }

    if (option[app_id].snd_buf)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_SNDBUF, &option[app_id].snd_buf, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_SNDBUF socket option failed with error[%d]\n", error);
        }
    }

    if (option[app_id].irt)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_IRT, &option[app_id].irt, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_IRT socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].max_rxt)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MAX_RXT, &option[app_id].max_rxt, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_MAX_RXT socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].overall_timer)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_OT, &option[app_id].overall_timer, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_OT socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].mss)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MAXSEG, &option[app_id].mss, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_MAXSEG socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].keepalive)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_KEEPALIVE, &option[app_id].keepalive, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_KEEPALIVE socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].keepalive && option[app_id].keep_idle)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_KEEPIDLE, &option[app_id].keep_idle, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_KEEPIDLE socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].keepalive && option[app_id].keep_intvl)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_KEEPINTVL, &option[app_id].keep_intvl, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_KEEPINTVL socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].keepalive && option[app_id].keep_cnt)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_KEEPCNT, &option[app_id].keep_cnt, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_KEEPCNT socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    if (option[app_id].linger_time_val0 || option[app_id].linger_time)
    {
        linger_option.l_onoff = 1;
        linger_option.l_linger = option[app_id].linger_time;
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_LINGER, &linger_option, sizeof(struct linger));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_LINGER socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].t_msl)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MSL, &option[app_id].t_msl, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_MSL option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].persfails)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_PERSIST_PROBE, &option[app_id].persfails, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_PERSIST_PROBE option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].rto_min)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_RTO_MIN, &option[app_id].rto_min, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_RTO_MIN option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].rto_max)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_RTO_MAX, &option[app_id].rto_max, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_RTO_MAX option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].win_scale)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_WINSCALE, &option[app_id].win_scale, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_WINSCALE option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].max_sackblock)
    {
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MAX_SACK_BLOCKS, &option[app_id].max_sackblock, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_MAX_SACK_BLOCKS option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if(option[app_id].enable_timestamp)
    {
	   if (qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET , SO_TIMESTAMP , &option[app_id].enable_timestamp, sizeof(option[app_id].enable_timestamp)) != QAPI_OK)
       {
          QCLI_Printf(qcli_net_handle,"set SO_TIMESTAMP option failed enable_timestamp[%d] = %d\n" ,app_id , option[app_id].enable_timestamp);
          goto ERROR_1;
       }
	   else
	   {
          QCLI_Printf(qcli_net_handle,"set SO_TIMESTAMP option returned success enable_timestamp[%d] = %d\n" ,app_id , option[app_id].enable_timestamp);
	   }
   }
	qapi_setsockopt(p_tCxt->sock_peer, 0, SO_CALLBACK, NULL, 0);

 
     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_SNDBUF, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_SNDBUF =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_IRT, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_IRT =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MAX_RXT, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_MAX_RXT =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MAXSEG, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_MAXSEG =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_OT, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_OT =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_KEEPALIVE, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_KEEPALIVE =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_KEEPIDLE, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_KEEPIDLE =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_KEEPINTVL, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_KEEPINTVL =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_KEEPCNT, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_KEEPCNT =%d\n",app_id, opt_val);

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_LINGER, &get_linger_option, sizeof(get_linger_option));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_LINGER_TIME =%d\n",app_id, get_linger_option.l_linger);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MSL, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_MSL =%d\n",app_id, opt_val);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_PERSIST_PROBE, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_PERSIST_PROBE =%d\n",app_id, opt_val);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_RTO_MIN, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_RTO_MIN =%d\n",app_id, opt_val);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_RTO_MAX, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_RTO_MAX =%d\n",app_id, opt_val);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_WINSCALE, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_WINSCALE =%d\n",app_id, opt_val);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, TCP_MAX_SACK_BLOCKS, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_MAX_SACK_BLOCKS =%d\n",app_id, opt_val);

	 qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_TIMESTAMP, &opt_val, sizeof(int));
     QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_TIMESTAMP =%d\n",app_id, opt_val);
	 
    /* Connect to the server.*/
    QCLI_Printf(qcli_net_handle, "Connecting\n");
    if (qapi_connect( p_tCxt->sock_peer, to, tolen) == A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "Connection failed.\n");
        goto ERROR_2;
    }

    if(option[app_id].benchtx_sleep)       /*duration in ms to make the benchtx sleep */
    {
        QCLI_Printf(qcli_net_handle, "benchtx is in sleep mode for %lu ms\n",option[app_id].benchtx_sleep);
        app_msec_delay(option[app_id].benchtx_sleep);
    }

#if INCLUDE_SSLDEMO
    QCLI_Printf(qcli_net_handle, "SSL Connecting\n");
    if (p_tCxt->test_type == SSL_TX)
    {
        if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
        {
            // Create SSL connection object
            QCLI_Printf(qcli_net_handle, " qapi_Net_SSL_Con_New\n");
            ssl->ssl = qapi_Net_SSL_Con_New(ssl->sslCtx, QAPI_NET_SSL_TLS_E);
            if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
            {
                QCLI_Printf(qcli_net_handle, "ERROR: Unable to create SSL context\n");
                goto ERROR_2;
            }

            // configure the SSL connection
            if (ssl->config_set)
            {
                result = qapi_Net_SSL_Configure(ssl->ssl, &ssl->config);
                if (result < QAPI_OK)
                {
                    QCLI_Printf(qcli_net_handle, "ERROR: SSL configure failed (%d)\n", result);
                    goto ERROR_2;
                }
            }

        }

        QCLI_Printf(qcli_net_handle, " Add socket handle to SSL connection\n");
        // Add socket handle to SSL connection
        result = qapi_Net_SSL_Fd_Set(ssl->ssl, p_tCxt->sock_peer);
        if (result < 0)
        {
            QCLI_Printf(qcli_net_handle, "ERROR: Unable to add socket handle to SSL (%d)\n", result);
            goto ERROR_2;
        }

        QCLI_Printf(qcli_net_handle, " SSL handshake with server\n");
        // SSL handshake with server
        result = qapi_Net_SSL_Connect(ssl->ssl);
        app_msec_delay(10);
        if (result < 0)
        {
            if (result == QAPI_SSL_OK_HS)
            {
                /** The peer's SSL certificate is trusted, CN matches the host name, time is valid */
                QCLI_Printf(qcli_net_handle, "The certificate is trusted\n");
            }
            else if (result == QAPI_ERR_SSL_CERT_CN)
            {
                /** The peer's SSL certificate is trusted, CN matches the host name, time is expired */
                QCLI_Printf(qcli_net_handle, "ERROR: The certificate is expired\n");
                goto ERROR_2;
            }
            else if (result == QAPI_ERR_SSL_CERT_TIME)
            {
                /** The peer's SSL certificate is trusted, CN does NOT match the host name, time is valid */
                QCLI_Printf(qcli_net_handle, "ERROR: The certificate is trusted, but the host name is not valid\n");
                goto ERROR_2;
            }
            else if (result == QAPI_ERR_SSL_CERT_NONE)
            {
                /** The peer's SSL certificate is NOT trusted,  */
                QCLI_Printf(qcli_net_handle, "ERROR: The peer's SSL certificate is NOT trusted\n");
                goto ERROR_2;
            }
            else
            {
                QCLI_Printf(qcli_net_handle, "ERROR: SSL connect failed (%d)\n", result);
                goto ERROR_2;
            }
        }
    }
#endif


    QCLI_Printf(qcli_net_handle, "Sending\n");


        netbuf_id = QAPI_NETBUF_APP;
        send_flag = 0;
    

    app_get_time(&p_tCxt->first_time);
    is_test_done = 0;
    while ( !is_test_done )
    {
        if (bench_quit[app_id][BENCH_TX])
        {
            is_test_done = 1;
            break;
        }

        /* allocate the buffer, if needed */
        if ( !p_tCxt->buffer )
        {
            while ((p_tCxt->buffer = qapi_Net_Buf_Alloc(packet_size, netbuf_id)) == NULL)
            {
                /*Wait till we get a buffer*/
                if (bench_quit[app_id][BENCH_TX])
                {
                    is_test_done = 1;
                    goto ERROR_2;
                }
                /*Allow small delay to allow other thread to run*/
                app_msec_delay(100);
            }

            if (netbuf_id == QAPI_NETBUF_APP) {
                /* Clear the buffer */
                memset(p_tCxt->buffer, 0, packet_size);
            } else {
                
            }
        }

        /* Update net buffer:
         *
         * [START]<4-byte Packet Index><4-byte Packet Size>000102..FF000102..FF0001..[END]
         * Byte counts: 8 + 4 + 4 + (packet_size-22) + 6
         *
         */
        if (packet_size >= 22)
        {
            char *pkt_start = "[START]";
            char *pkt_end = "[END]";
            uint32_t val;

            /* Add [START] */
            qapi_Net_Buf_Update(p_tCxt->buffer, 0, pkt_start, 8, netbuf_id);

            /* Packet index */
            val = htonl(cur_packet_number);
            qapi_Net_Buf_Update(p_tCxt->buffer, 8, &val, 4, netbuf_id);

            /* Packet size */
            val = htonl(packet_size);
            qapi_Net_Buf_Update(p_tCxt->buffer, 12, &val, 4, netbuf_id);

            /* Add pattern
             * The pattern is repeated 00 01 02 03 .. FE FF
             */
            if (netbuf_id == QAPI_NETBUF_APP)
            {
                add_pattern(p_tCxt->buffer + 16, packet_size - 16 - 6);
            }

            /* Add [END] */
            qapi_Net_Buf_Update(p_tCxt->buffer, packet_size-6, pkt_end, 6, netbuf_id);
        }
        else if (packet_size >= 4)
        {
            /* Small buffer, just send the packet index */
            uint32_t val;

            /* Packet index */
            val = htonl(cur_packet_number);
            qapi_Net_Buf_Update(p_tCxt->buffer, 0, &val, 4, netbuf_id);
        }

        bytes_to_send = packet_size - buffer_offset;
        bytes_sent = 0;


#if INCLUDE_SSLDEMO
        if (p_tCxt->test_type == SSL_TX)
        {
            app_msec_delay(5);
            bytes_sent = qapi_Net_SSL_Write(ssl->ssl, &p_tCxt->buffer[buffer_offset], bytes_to_send);
        }
        else
#endif


        {
          // QCLI_Printf(qcli_net_handle, "calling qapi_send\n");
            bytes_sent = qapi_send( p_tCxt->sock_peer, &p_tCxt->buffer[buffer_offset], bytes_to_send, send_flag);
        }

        ++j;
#ifdef TCP_TX_DEBUG
        QCLI_Printf(qcli_net_handle, "%d bytes_sent = %d\n", j, bytes_sent);
#endif

        app_get_time(&p_tCxt->last_time);

        if (++i >= 100)
        {
        #ifndef FEATURE_9x07_CAT4
            QCLI_Printf(qcli_net_handle, "#");
        #endif
		    QCLI_Printf(qcli_net_handle, "#");
            i = 0;
        }

        if ( bytes_sent != bytes_to_send ) {
            int errno = qapi_errno(p_tCxt->sock_peer);
            QCLI_Printf(qcli_net_handle, "Failed on a call to qapi_send, bytes_sent=%d, errno=%d\n", bytes_sent, errno);
            if ( 0 ||
                    (EPIPE == errno) ||
                    (ENOTSOCK == errno) ||
                    (EBADF == errno) )
            {
                // the socket has closed - no point of continuing the test
                QCLI_Printf(qcli_net_handle, "Socket got closed\n");
                if (!v6_enable_flag[app_id] && (bytes_sent >= 0) && (bytes_sent < bytes_to_send) ) {
                    p_tCxt->buffer = 0;
                }
                is_test_done = 1;
                break;
            }
            else {
                // severe push back, let the other processes run for 1 ms
                QCLI_Printf(qcli_net_handle," ENOBUFFS HIT");
                app_msec_delay(1);
            }
        }

        // Useful notes related to TCP send:
        //
        // IPv4:
        //  return_code < 0: the buffer was not sent, qapi_send() did not free the buffer if qapi_errno() returns ENOBUFS.
        //  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
        //  return_code < packet_size: the buffer sent partially, qapi_send() freed the buffer
        // IPv6:
        //  return_code < 0: the buffer was not sent, qapi_send() did not free the buffer
        //  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
        // if the buffer is transmitted partially, the remaining of the buffer is freed (lost).

        /****Bandwidth control- add delay if user has specified it************/
        #ifdef FEATURE_9x07_CAT4
        pktnum ++;
        #endif

        if (p_tCxt->params.tx_params.interval)
        {
          #ifdef FEATURE_9x07_CAT4
            if((pktnum % packet_factor )== 0)
            {
                app_msec_delay(p_tCxt->params.tx_params.interval);
            }
          #else
          if(enable_us_delay == false)
          app_msec_delay(p_tCxt->params.tx_params.interval);
          else
          {
            app_usec_delay(p_tCxt->params.tx_params.interval);
          }
          #endif
        }

        if ( bytes_sent >= 0  )
        {
            p_tCxt->bytes += bytes_sent;

            if ( bytes_sent == bytes_to_send ) {
                cur_packet_number++;
                buffer_offset = 0;
            }
            else {
                buffer_offset += bytes_sent;
            }
        }

        // check the test completion condition based on number of packets sent
        if(p_tCxt->params.tx_params.test_mode == PACKET_TEST)
        {
            if((cur_packet_number >= p_tCxt->params.tx_params.packet_number))
            {
                is_test_done = 1;/* Test completed, print throughput results.*/
                break;
            }
        }

        // check the test comletion condition based on time
        if( p_tCxt->params.tx_params.test_mode == TIME_TEST )
        {
            if(check_test_time(p_tCxt))
            {
                is_test_done = 1;/* Test completed, print test results.*/
                break;
            }
        }

    } /* send loop */
    
    if ( p_tCxt->buffer ) {
        qapi_Net_Buf_Free(p_tCxt->buffer, netbuf_id);
    }

    QCLI_Printf(qcli_net_handle, "\nSent %u/%u messages, %llu bytes to %s %d\n",
            cur_packet_number, j, p_tCxt->bytes, ip_str, p_tCxt->params.tx_params.port);


ERROR_2:
    print_test_results(p_tCxt);

#if INCLUDE_SSLDEMO
    if (ssl_role[app_id][SSL_CLIENT_INST] == QAPI_NET_SSL_CLIENT_E &&  ssl->ssl != QAPI_NET_SSL_INVALID_HANDLE)
    {
        qapi_Net_SSL_Shutdown(ssl->ssl);
        ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
    }
#endif


    qapi_socketclose( p_tCxt->sock_peer);

ERROR_1:
    QCLI_Printf(qcli_net_handle, BENCH_TEST_COMPLETED);
}


/************************************************************************
* NAME: qca_udp_tx
*
* DESCRIPTION: Start TX UDP throughput test.
************************************************************************/
static void qca_udp_tx(THROUGHPUT_CXT *p_tCxt, int app_id)
{
    struct sockaddr_in foreign_addr;
    struct sockaddr_in6 foreign_addr6;
    struct sockaddr *to;
    struct sockaddr_in  local_addr;
    struct sockaddr_in6 local_addr6;
    #ifdef FEATURE_9x07_CAT4
     int pktnum=0;
    #endif

    struct sockaddr *addr;
    int addrlen;
    uint32_t tolen;
    char ip_str [48];
    int32_t send_bytes;
#ifdef INCLUDE_SSLDEMO
    int32_t result;
#endif
    uint32_t packet_size = p_tCxt->params.tx_params.packet_size;
    uint32_t cur_packet_number, i, n_send_ok;
    uint32_t netbuf_id;
    int send_flag;
    int family;
    uint32_t is_test_done;


#if INCLUDE_SSLDEMO
    SSL_INST *ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
#endif


    bench_quit[app_id][BENCH_TX] = 0;

#ifdef CHECK_PACKET_SIZE
    if(packet_size > CFG_PACKET_SIZE_MAX_TX) /* Check max size.*/
        packet_size = CFG_PACKET_SIZE_MAX_TX;
#endif

    if (v6_enable_flag[app_id])
    {
        family = AF_INET6;
        inet_ntop(family, p_tCxt->params.tx_params.v6addr, ip_str, sizeof(ip_str));

        memset(&foreign_addr6, 0, sizeof(foreign_addr6));
        memcpy(&foreign_addr6.sin_addr, p_tCxt->params.tx_params.v6addr, sizeof(foreign_addr6.sin_addr));
        foreign_addr6.sin_port      = htons(p_tCxt->params.tx_params.port);
        foreign_addr6.sin_family    = family;
        foreign_addr6.sin_scope_id  = p_tCxt->params.tx_params.scope_id;
        
        to = (struct sockaddr *)&foreign_addr6;
        tolen = sizeof(foreign_addr6);

        if(p_tCxt->params.tx_params.bind_enabled)
        {

           addr = (struct sockaddr *)&local_addr6;
           addrlen = sizeof(struct sockaddr_in6);

           memset(&local_addr6, 0, sizeof(local_addr6));
           memcpy(&local_addr6.sin_addr, p_tCxt->params.tx_params.sv6addr, sizeof(local_addr6.sin_addr));
           local_addr6.sin_port = htons(p_tCxt->params.tx_params.port);
           local_addr6.sin_family = family;
        }
        
    }
    else
    {
        family = AF_INET;
        inet_ntop(family, &p_tCxt->params.tx_params.ip_address, ip_str, sizeof(ip_str));

        memset(&foreign_addr, 0, sizeof(foreign_addr));
        foreign_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.ip_address;
        foreign_addr.sin_port           = htons(p_tCxt->params.tx_params.port);
        foreign_addr.sin_family         = family;

        to = (struct sockaddr *)&foreign_addr;
        tolen = sizeof(foreign_addr);

        if(p_tCxt->params.tx_params.bind_enabled)
        {
             addr = (struct sockaddr *)&local_addr;
             addrlen = sizeof(struct sockaddr_in);
             memset(&local_addr, 0, sizeof(local_addr));
             local_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.sip_address;
             local_addr.sin_port = htons(p_tCxt->params.tx_params.port);
             local_addr.sin_family = family;
        }
    }

    /* ------ Start test.----------- */
    QCLI_Printf(qcli_net_handle, "****************************************************************\n");
#if INCLUDE_SSLDEMO
    QCLI_Printf(qcli_net_handle, "IOT %s TX Test\n", p_tCxt->test_type == SSL_TX ? "SSL" : "UDP");
#else
    QCLI_Printf(qcli_net_handle, "IOT UDP TX Test\n" );
#endif
    QCLI_Printf(qcli_net_handle, "****************************************************************\n");
    QCLI_Printf(qcli_net_handle, "Remote IP addr: %s\n", ip_str);
    QCLI_Printf(qcli_net_handle, "Remote port: %d\n", p_tCxt->params.tx_params.port);
    QCLI_Printf(qcli_net_handle, "Message size: %d\n", p_tCxt->params.tx_params.packet_size);
    QCLI_Printf(qcli_net_handle, "Number of messages: %d\n", p_tCxt->params.tx_params.packet_number);
    QCLI_Printf(qcli_net_handle, "Delay in ms: %d\n", p_tCxt->params.tx_params.interval);
    QCLI_Printf(qcli_net_handle, "Press Ctrl+C to terminate test\n");
    QCLI_Printf(qcli_net_handle, "****************************************************************\n");

    /* Create UDP socket */
    if ((p_tCxt->sock_peer = qapi_socket(family, SOCK_DGRAM, 0)) == A_ERROR) {
        QCLI_Printf(qcli_net_handle, "Socket creation failed\n");
        goto ERROR_1;
    }

    if(p_tCxt->params.tx_params.bind_enabled)

    {       /* Bind socket */
       if (qapi_bind(p_tCxt->sock_peer, addr, addrlen) == (UINT)A_ERROR)
       {
          QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
          goto ERROR_2;
       }
    }
    
    /* Connect to the server.*/
    QCLI_Printf(qcli_net_handle, "Connecting\n");
    if (qapi_connect( p_tCxt->sock_peer, to, tolen) == A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "Conection failed\n");
        goto ERROR_2;
    }

#if INCLUDE_SSLDEMO
    if (p_tCxt->test_type == SSL_TX)
    {
        if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
        {
            // Create SSL connection object
            ssl->ssl = qapi_Net_SSL_Con_New(ssl->sslCtx, QAPI_NET_SSL_DTLS_E);
            if (ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
            {
                QCLI_Printf(qcli_net_handle, "ERROR: Unable to create SSL context\n");
                goto ERROR_2;
            }

            // configure the SSL connection
            if (ssl->config_set)
            {
                result = qapi_Net_SSL_Configure(ssl->ssl, &ssl->config);
                if (result < QAPI_OK)
                {
                    QCLI_Printf(qcli_net_handle, "ERROR: SSL configure failed (%d)\n", result);
                    goto ERROR_2;
                }
            }
        }
        
        // Add socket handle to SSL connection
        result = qapi_Net_SSL_Fd_Set(ssl->ssl, p_tCxt->sock_peer);
        if (result < 0)
        {
            QCLI_Printf(qcli_net_handle, "ERROR: Unable to add socket handle to SSL (%d)\n", result);
            goto ERROR_2;
        }

        // SSL handshake with server
        result = qapi_Net_SSL_Connect(ssl->ssl);
        QCLI_Printf(qcli_net_handle, "qapi_Net_SSL_Connect(%d)\n", result);
        if (result < 0)
        {
            if (result == QAPI_SSL_OK_HS)
            {
                /** The peer's SSL certificate is trusted, CN matches the host name, time is valid */
                QCLI_Printf(qcli_net_handle, "The certificate is trusted\n");
            }
            else if (result == QAPI_ERR_SSL_CERT_CN)
            {
                /** The peer's SSL certificate is trusted, CN matches the host name, time is expired */
                QCLI_Printf(qcli_net_handle, "ERROR: The certificate is expired\n");
                goto ERROR_2;
            }
            else if (result == QAPI_ERR_SSL_CERT_TIME)
            {
                /** The peer's SSL certificate is trusted, CN does NOT match the host name, time is valid */
                QCLI_Printf(qcli_net_handle, "ERROR: The certificate is trusted, but the host name is not valid\n");
                goto ERROR_2;
            }
            else if (result == QAPI_ERR_SSL_CERT_NONE)
            {
                /** The peer's SSL certificate is NOT trusted,  */
                QCLI_Printf(qcli_net_handle, "ERROR: The peer's SSL certificate is NOT trusted\n");
                goto ERROR_2;
            }
            else
            {
                QCLI_Printf(qcli_net_handle, "ERROR: SSL connect failed (%d)\n", result);
                goto ERROR_2;
            }
        }
    }
#endif



    /* Sending.*/
    QCLI_Printf(qcli_net_handle, "Sending\n");


        netbuf_id = QAPI_NETBUF_APP;
        send_flag = 0;

    /*Reset all counters*/
    cur_packet_number = 0;
    i = 0;
    n_send_ok = 0;

    app_get_time(&p_tCxt->first_time);

    is_test_done = 0;
    while ( !is_test_done )
    {
        if (bench_quit[app_id][BENCH_TX])
        {
            is_test_done = 1;
            break;
        }

        /* allocate the buffer, if needed */
        if ( !p_tCxt->buffer )
        {
            while ((p_tCxt->buffer = qapi_Net_Buf_Alloc(packet_size, netbuf_id)) == NULL)
            {
                /*Wait till we get a buffer*/
                if (bench_quit[app_id][BENCH_TX])
                {
                    is_test_done = 1;
                    goto ERROR_2;
                }
                /*Allow small delay to allow other thread to run*/
                app_msec_delay(100);
            }

            if (netbuf_id == QAPI_NETBUF_APP) {
                /* Clear the buffer */
                memset(p_tCxt->buffer, 0, packet_size);
            } else {
                
            }
        }

        /* Update net buffer:
         *
         * [START]<4-byte Packet Index><4-byte Packet Size>000102..FF000102..FF0001..[END]
         * Byte counts: 8 + 4 + 4 + (packet_size-22) + 6
         *
         */
        if (packet_size >= 22)
        {
            char *pkt_start = "[START]";
            char *pkt_end = "[END]";
            uint32_t val;

            /* Add [START] */
            qapi_Net_Buf_Update(p_tCxt->buffer, 0, pkt_start, 8, netbuf_id);

            /* Packet index */
            val = htonl(cur_packet_number);
            qapi_Net_Buf_Update(p_tCxt->buffer, 8, &val, 4, netbuf_id);

            /* Packet size */
            val = htonl(packet_size);
            qapi_Net_Buf_Update(p_tCxt->buffer, 12, &val, 4, netbuf_id);

            /* Add pattern
             * The pattern is repeated 00 01 02 03 .. FE FF
             */
            if (netbuf_id == QAPI_NETBUF_APP)
            {
                add_pattern(p_tCxt->buffer + 16, packet_size - 16 - 6);
            }

            /* Add [END] */
            qapi_Net_Buf_Update(p_tCxt->buffer, packet_size-6, pkt_end, 6, netbuf_id);
        }
        else if (packet_size >= 4)
        {
            /* Small buffer, just send the packet index */
            uint32_t val;

            /* Packet index */
            val = htonl(cur_packet_number);
            qapi_Net_Buf_Update(p_tCxt->buffer, 0, &val, 4, netbuf_id);
        }

        do
        {
    #if INCLUDE_SSLDEMO
            if (p_tCxt->test_type == SSL_TX)
            {   
                app_msec_delay(10);
                send_bytes = qapi_Net_SSL_Write(ssl->ssl, p_tCxt->buffer, packet_size);
            }
            else
    #endif
            {
                send_bytes = qapi_sendto(p_tCxt->sock_peer, p_tCxt->buffer, packet_size, send_flag, to, tolen) ;
            }

            if ( send_bytes != packet_size ) {
                int errno = qapi_errno(p_tCxt->sock_peer);
                QCLI_Printf(qcli_net_handle, "Failed on a call to qapi_sendto, send_bytes=%d, errno=%d\n", send_bytes, errno); 
                if ( errno == 0 ||
                        (EPIPE == errno) ||
                        (ENOTSOCK == errno) ||
                        (EBADF == errno))
                {
                    // the socket has closed - no point of continuing the test
                    QCLI_Printf(qcli_net_handle, "Socket got closed, errno=%d\n", errno);
                    if ( !v6_enable_flag[app_id] && (send_bytes >= 0) && (send_bytes < packet_size) ) {
                        p_tCxt->buffer = 0;
                    }
                    is_test_done = 1;
                    break;
                }
                else {
                    if ( ENOBUFS != errno ) {
                        QCLI_Printf(qcli_net_handle, "Failed on a call to qapi_sendto, send_bytes=%d, errno=%d\n", send_bytes, errno);
                    }
                    // severe push back, let the other processes run for 1 ms
                    app_msec_delay(30);
                }
            }
            else 
			{
               cur_packet_number ++;
            }

            app_get_time(&p_tCxt->last_time);
            if (++i >= 500)
            {
              //  QCLI_Printf(qcli_net_handle, ".");
                i = 0;
            }

            /* Useful notes related to UDP  send:
             *
             * In UDP we can have the following qapi_send() return codes:
             * IPv4:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             *  return_code < packet_size: the buffer sent partially, qapi_send() freed the buffer
             * IPv6:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             */
#ifndef FEATURE_9x07_CAT4
    #if 1 //def UDP_TX_DEBUG
            QCLI_Printf(qcli_net_handle, "%d send_bytes = %d\n", cur_packet_number, send_bytes);
    #endif
#endif


        #ifdef FEATURE_9x07_CAT4
		pktnum ++;
		#endif
            /****Bandwidth control***********/

#ifdef FEATURE_9x07_CAT4
          if(p_tCxt->params.tx_params.interval && ((pktnum % udp_packet_factor)== 0))
          { 
            app_msec_delay(p_tCxt->params.tx_params.interval);
          }
		  else
		  {
		    /****Bandwidth control if packet send interval is less than 1ms***********/
            if(pktnum%2 == 0)
            {
              app_msec_delay(10);
            }
		  }
#else
            /****Bandwidth control***********/
        if(p_tCxt->params.tx_params.interval)
        {
           if(p_tCxt->test_type == SSL_TX)
           {
             if(enable_us_delay == false)
             {
               app_msec_delay(p_tCxt->params.tx_params.interval);
             }
             else
			 {
               app_usec_delay(p_tCxt->params.tx_params.interval);
             }
           }
           else if(cur_packet_number % p_tCxt->params.tx_params.interval == 0)
           {
             if(enable_us_delay ==false)
             {
               app_msec_delay(udp_normal_delay);
             }
             else
			 { 
               app_usec_delay(udp_normal_delay *1000);
             }
           }
           else if(p_tCxt->test_type == UDP_TX)
           {
             if(enable_us_delay ==false)
             {
               app_msec_delay(p_tCxt->params.tx_params.interval);
             }
             else
             {
               app_usec_delay(p_tCxt->params.tx_params.interval);
             }
           }
        }
		else
		{    
		    /****Bandwidth control if packet send interval is less than 1ms***********/
            if(cur_packet_number%2 == 0)
            {
              app_msec_delay(10);
            }
		}
#endif
            if (send_bytes > 0)
            {
                p_tCxt->bytes += send_bytes;
                ++n_send_ok;
            }

            /*Test mode can be "number of packets" or "fixed time duration"*/
            if (p_tCxt->params.tx_params.test_mode == PACKET_TEST)
            {
                if ((cur_packet_number >= p_tCxt->params.tx_params.packet_number))
                {
                    is_test_done = 1;
                    break;
                }
            }
            else if (p_tCxt->params.tx_params.test_mode == TIME_TEST)
            {
                if (check_test_time(p_tCxt))
                {
                    is_test_done = 1;
                    break;
                }
            }
        }
        while ( !((is_test_done) || (send_bytes == packet_size) || (NULL == p_tCxt->buffer)) );
    } /* send loop */

    if ( p_tCxt->buffer )
    {
        qapi_Net_Buf_Free(p_tCxt->buffer, netbuf_id);
    }

    QCLI_Printf(qcli_net_handle, "\nSent %u packets, %llu bytes to %s %d (%u)\n",
            cur_packet_number, p_tCxt->bytes, ip_str, p_tCxt->params.tx_params.port, cur_packet_number - n_send_ok);

ERROR_2:
    print_test_results(p_tCxt);


#if INCLUDE_SSLDEMO
    if (ssl_role[app_id][SSL_CLIENT_INST] == QAPI_NET_SSL_CLIENT_E &&  ssl->ssl != QAPI_NET_SSL_INVALID_HANDLE)
    {
        qapi_Net_SSL_Shutdown(ssl->ssl);
        ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
    }
#endif

    qapi_socketclose(p_tCxt->sock_peer);

ERROR_1:
    QCLI_Printf(qcli_net_handle, BENCH_TEST_COMPLETED);
}

/************************************************************************
* NAME: qca_tcp_rx
*
* DESCRIPTION: Start throughput TCP server.
************************************************************************/
static void qca_tcp_rx(THROUGHPUT_CXT *p_tCxt, int app_id)
{
    uint16_t port;
    int32_t received;
    int32_t conn_sock, isFirst, i;
    struct sockaddr_in local_addr;
    struct sockaddr_in6 local_addr6;
    struct sockaddr *addr;
    uint32_t addrlen;
    struct sockaddr_in foreign_addr;
    struct sockaddr_in6 foreign_addr6;
    struct sockaddr *from;
    int32_t fromlen;
    void *sin_addr;
    char ip_str[48];
    time_struct_t ts;
	int error = QAPI_ERROR;
	int opt_val = 0;

#if INCLUDE_SSLDEMO
    SSL_INST *ssl = &ssl_inst[app_id][SSL_SERVER_INST];
#endif


    int32_t netbuf_id = QAPI_NETBUF_APP;
#ifdef TCP_RX_RETRY_AFTER_FIN
    uint32_t retry=30;
#endif
    int family;

    /*Allocate buffer*/
    if ((p_tCxt->buffer = qapi_Net_Buf_Alloc(CFG_PACKET_SIZE_MAX_RX, netbuf_id)) == NULL)
    {
        QCLI_Printf(qcli_net_handle, "Out of memory error\n");
        p_tCxt->buffer = NULL;
        return;
    }

    port = p_tCxt->params.rx_params.port;
    bench_quit[app_id][BENCH_RX] = 0;
    p_tCxt->sock_peer = A_ERROR;

    if (v6_enable_flag[app_id])
    {
        family = AF_INET6;

        memset(&local_addr6, 0, sizeof(local_addr6));
        local_addr6.sin_port = htons(port);
        local_addr6.sin_family = family;

        addr = (struct sockaddr *)&local_addr6;
        addrlen = sizeof(struct sockaddr_in6);
        from = (struct sockaddr *)&foreign_addr6;
        fromlen = sizeof(struct sockaddr_in6);
        sin_addr = &foreign_addr6.sin_addr;

        if(p_tCxt->params.rx_params.local_if == 1)
        {
            memcpy(&local_addr6.sin_addr, p_tCxt->params.rx_params.local_v6addr, sizeof(local_addr6.sin_addr));
        }
    }
    else
    {
        family = AF_INET;

        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_port = htons(port);
        local_addr.sin_family = family;

        addr = (struct sockaddr *)&local_addr;
        addrlen = sizeof(struct sockaddr_in);
        from = (struct sockaddr *)&foreign_addr;
        fromlen = sizeof(struct sockaddr_in);
        sin_addr = &foreign_addr.sin_addr;

        if(p_tCxt->params.rx_params.local_if == 1)
        {
            local_addr.sin_addr.s_addr = p_tCxt->params.rx_params.local_address;
        }
    }

    /* Create listen socket */
    if ((p_tCxt->sock_local = qapi_socket( family, SOCK_STREAM, 0)) == A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Socket creation error.\n");
        goto ERROR_1;
    }

#ifdef TCP_KEEPALIVE_OFFLOAD_TEST
    qapi_setsockopt(p_tCxt->sock_local, 0, SO_KEEPALIVE, NULL, 0);    
#endif

    /* Bind socket */
    if (qapi_bind(p_tCxt->sock_local, addr, addrlen) == A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
        goto ERROR_2;
    }

	if(option[app_id].enable_sackopt)
    {
       if (qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET , SO_TCPSACK , &option[app_id].enable_sackopt, sizeof(option[app_id].enable_sackopt)) != QAPI_OK)
	   {
          QCLI_Printf(qcli_net_handle,"set TCPSACK option failed enable_sackopt[%d] = %d\n" ,app_id , option[app_id].enable_sackopt);
          goto ERROR_1;
       }
       else
	   {
		  QCLI_Printf(qcli_net_handle,"set TCPSACK option returned success enable_sackopt[%d] = %d\n" ,app_id , option[app_id].enable_sackopt);
	   }
    }

	if (option[app_id].mss)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET, TCP_MAXSEG, &option[app_id].mss, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_MAXSEG socket option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].reassm_timer)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET, TCP_REASSMVAL, &option[app_id].reassm_timer, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_REASSMVAL option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].rcv_buf)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_RCVBUF, &option[app_id].rcv_buf, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_RCVBUF socket option failed with error[%d]\n", error);
        }
    }

	if (option[app_id].win_scale)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_WINSCALE, &option[app_id].win_scale, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_WINSCALE option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].max_sackblock)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET, TCP_MAX_SACK_BLOCKS, &option[app_id].max_sackblock, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting TCP_MAX_SACK_BLOCKS option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].ip_ire)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IRE_TMO, &option[app_id].ip_ire, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting IP_IRE_TMO option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].ip_maxfrags)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IPR_MAX_FRAGS, &option[app_id].ip_maxfrags, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting IP_IPR_MAX_FRAGS option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

    /* set to non-blocking mode */
    qapi_setsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_NBIO, NULL, 0);

	qapi_getsockopt(p_tCxt->sock_local, SOL_SOCKET, TCP_REASSMVAL, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_REASSMVAL =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_RCVBUF, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_RCVBUF =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_WINSCALE, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_WINSCALE =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, SOL_SOCKET, TCP_MAX_SACK_BLOCKS, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t TCP_MAX_SACK_BLOCKS =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IRE_TMO, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t IP_IRE_TMO =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IPR_MAX_FRAGS, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t IP_IPR_MAX_FRAGS =%d\n",app_id, opt_val);

    /* ------ Start test.----------- */
    QCLI_Printf(qcli_net_handle, "*****************************************\n");
#if INCLUDE_SSLDEMO
    QCLI_Printf(qcli_net_handle, " %s RX Test\n", p_tCxt->test_type == SSL_RX ? "SSL" : "TCP");
#else
    QCLI_Printf(qcli_net_handle, " TCP RX Test\n" );
#endif
    QCLI_Printf(qcli_net_handle, "*****************************************\n");
    QCLI_Printf(qcli_net_handle, "Local port %d\n", port);
    QCLI_Printf(qcli_net_handle, "Press Ctrl+C to termintate test\n");
    QCLI_Printf(qcli_net_handle, "*****************************************\n");

    /* Listen */
    if (qapi_listen( p_tCxt->sock_local, 1) == A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Socket listen error.\n");
        goto ERROR_2;
    }

	qapi_getsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_ACCEPTCONN, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_ACCEPTCONN =%d\n",app_id, opt_val);

    while (1)
    {
        QCLI_Printf(qcli_net_handle, "\nWaiting\n");

        do
        {
            if (bench_quit[app_id][BENCH_RX])
            {
                goto tcp_rx_QUIT;
            }

            /* Accept incoming connection */
            if ((p_tCxt->sock_peer = qapi_accept(p_tCxt->sock_local, from, &fromlen)) != A_ERROR)
            {
                break;
            }

            app_msec_delay(10);
        } while (1);

        if (option[app_id].delayacktime)
        {
            error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_TCP_ACKDELAYTIME, &option[app_id].delayacktime, sizeof(int));
            if(error != QAPI_OK)
            {
                QCLI_Printf(qcli_net_handle,"Setting SO_TCP_ACKDELAYTIME socket option failed with error: %d\n", error);
                goto ERROR_1;
            }
        }
        
        p_tCxt->bytes = 0;
        memset(&p_tCxt->first_time, 0, sizeof(time_struct_t));
        memset(&p_tCxt->last_time, 0, sizeof(time_struct_t));
        memset(ip_str, 0, sizeof(ip_str));



#if INCLUDE_SSLDEMO
        if (p_tCxt->test_type == SSL_RX)
        {
            QCLI_Printf(qcli_net_handle, "\nssl_rx_setup\n");
            if (ssl_rx_setup(p_tCxt, ssl, app_id) < 0) {
                qapi_socketclose(p_tCxt->sock_peer);
                goto tcp_rx_QUIT;
            }
        }
#endif



        QCLI_Printf(qcli_net_handle, "Accepted conn from %s:%d\n",
                inet_ntop(family, sin_addr, ip_str, sizeof(ip_str)),
                ntohs(from->sa_port));

        i = 0;
        isFirst = 1;

        while (1) /* Receiving data */
        {
            fd_set rset;

            if (bench_quit[app_id][BENCH_RX])
            {

#if INCLUDE_SSLDEMO
                if (ssl->ssl != QAPI_NET_SSL_INVALID_HANDLE)
                {
                    qapi_Net_SSL_Shutdown(ssl->ssl);
                    ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
                }
#endif


                qapi_socketclose(p_tCxt->sock_peer);
                goto tcp_rx_QUIT;
            }

            qapi_fd_zero(&rset);
            qapi_fd_set(p_tCxt->sock_peer, &rset);
            conn_sock = qapi_select(&rset, NULL, NULL, 500);

            if (conn_sock == 0)
            {
                /* No activity. Continue to do select */
                continue;
            }
#ifdef TCP_RX_DEBUG
            QCLI_Printf(qcli_net_handle, "conn_sock=%d\n", conn_sock);
#endif
            if (conn_sock > 0)
            {
                /*Packet is available, receive it*/



#if INCLUDE_SSLDEMO
                if (p_tCxt->test_type == SSL_RX)
                {
                    received = qapi_Net_SSL_Read(ssl->ssl, p_tCxt->buffer, CFG_PACKET_SIZE_MAX_RX);
                    QCLI_Printf(qcli_net_handle, "%d received\n", received); 
                }
                else
#endif
                {
                    received = qapi_recv( p_tCxt->sock_peer, (char*)(&p_tCxt->buffer[0]), CFG_PACKET_SIZE_MAX_RX, 0);
                }
                ++i;

                /*Valid packet received*/
                if (received > 0)
                {
                    p_tCxt->bytes += received;

                    if (isFirst)
                    {
                        QCLI_Printf(qcli_net_handle, "Receiving\n");

                        /*This is the first packet, set initial time used to calculate throughput*/
                        app_get_time(&p_tCxt->first_time);
                        isFirst = 0;
                    }

                    app_get_time(&ts);
                    //QCLI_Printf(qcli_net_handle, "%d sec=%u ms=%u rcvd=%u\n", i, ts.seconds, ts.milliseconds, received);

#ifdef TCP_RX_DEBUG
                   // QCLI_Printf(qcli_net_handle, "%d received= %d total=%llu\n", i, received, p_tCxt->bytes); 
#endif
                }
                else /* received <= 0 */
                {
#ifdef TCP_RX_DEBUG
                    QCLI_Printf(qcli_net_handle, "%d received= %d total=%llu\n", i, received, p_tCxt->bytes); 
#endif
#ifdef TCP_RX_RETRY_AFTER_FIN
                    if (retry > 0)
                    {
                        app_msec_delay(10);
                        --retry;
                        continue;
                    }
                    else
#endif
                    {
                        /* Receive is done */
                        app_get_time(&p_tCxt->last_time);

                        QCLI_Printf(qcli_net_handle, "\nReceived %llu bytes from %s:%d\n",
                                p_tCxt->bytes, ip_str, ntohs(from->sa_port));

#if INCLUDE_SSLDEMO
                        if (ssl->ssl != QAPI_NET_SSL_INVALID_HANDLE)
                        {
                            qapi_Net_SSL_Shutdown(ssl->ssl);
                            ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
                        }
#endif


                        qapi_socketclose(p_tCxt->sock_peer);

                        print_test_results(p_tCxt);

                        break;
                    }
                }
            }
            else /* conn_sock < 0 */
            {
                break;
            }
        }/*receive loop*/
    } /* main loop */

tcp_rx_QUIT:
    print_test_results(p_tCxt);

ERROR_2:
    qapi_socketclose(p_tCxt->sock_local);

ERROR_1:
    QCLI_Printf(qcli_net_handle, BENCH_TEST_COMPLETED);

    if (p_tCxt->buffer)
        qapi_Net_Buf_Free(p_tCxt->buffer, netbuf_id);

    return;
}
/*+RAW IPv4*/
/************************************************************************
* NAME: qca_raw_tx
*
* DESCRIPTION: Start TCP Transmit test.
************************************************************************/
static void qca_raw_tx (THROUGHPUT_CXT *p_tCxt, int app_id)
{

    struct sockaddr_in foreign_addr;
    struct sockaddr_in6 foreign_addr6;
    struct sockaddr *to;
	struct sockaddr_in  local_addr;
    struct sockaddr_in6 local_addr6;

	int isHdlcset = p_tCxt->params.tx_params.hdrincl;
	int ret = -1;
	int proto_num = 255; /*socket protocol number*/
	int rawsock_prot = 80;
   
    struct sockaddr *addr;
    int addrlen;
    uint32_t tolen;
    char ip_str [48];
    int32_t send_bytes, result;
    uint32_t packet_size = p_tCxt->params.tx_params.packet_size;
    uint32_t cur_packet_number, i, n_send_ok;
    uint32_t netbuf_id;
    int send_flag;
    int family;
    uint32_t is_test_done;

    bench_quit[app_id][BENCH_TX] = 0;

#ifdef CHECK_PACKET_SIZE
    if(packet_size > CFG_PACKET_SIZE_MAX_TX) /* Check max size.*/
        packet_size = CFG_PACKET_SIZE_MAX_TX;
    if(packet_size < CFG_PACKET_SIZE_MIN_RAW_TX)
        packet_size = CFG_PACKET_SIZE_MIN_RAW_TX;
#endif

    if (v6_enable_flag[app_id])
    {
        family = AF_INET6;
        inet_ntop(family, p_tCxt->params.tx_params.v6addr, ip_str, sizeof(ip_str));

        memset(&foreign_addr6, 0, sizeof(foreign_addr6));
        memcpy(&foreign_addr6.sin_addr, p_tCxt->params.tx_params.v6addr, sizeof(foreign_addr6.sin_addr));
        foreign_addr6.sin_port      = htons(p_tCxt->params.tx_params.port);
        foreign_addr6.sin_family    = family;
        foreign_addr6.sin_scope_id  = p_tCxt->params.tx_params.scope_id;
        
        to = (struct sockaddr *)&foreign_addr6;
        tolen = sizeof(foreign_addr6);

        if(p_tCxt->params.tx_params.bind_enabled)
        {

           addr = (struct sockaddr *)&local_addr6;
           addrlen = sizeof(struct sockaddr_in6);

		   memset(&local_addr6, 0, sizeof(local_addr6));
		   memcpy(&local_addr6.sin_addr, p_tCxt->params.tx_params.sv6addr, sizeof(local_addr6.sin_addr));
           local_addr6.sin_port = htons(p_tCxt->params.tx_params.port);
           local_addr6.sin_family = family;
        }
		
    }
    else
    {
        family = AF_INET;
        inet_ntop(family, &p_tCxt->params.tx_params.ip_address, ip_str, sizeof(ip_str));

        memset(&foreign_addr, 0, sizeof(foreign_addr));
        foreign_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.ip_address;
        foreign_addr.sin_port           = htons(rawsock_prot); /*use hacked port number*/
        foreign_addr.sin_family         = family;

        to = (struct sockaddr *)&foreign_addr;
        tolen = sizeof(foreign_addr);

        if(p_tCxt->params.tx_params.bind_enabled)
        {
             addr = (struct sockaddr *)&local_addr;
             addrlen = sizeof(struct sockaddr_in);
		     memset(&local_addr, 0, sizeof(local_addr));
             local_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.sip_address;
             local_addr.sin_port = htons(rawsock_prot); /*use hacked port number*/
             local_addr.sin_family = family;
        }
    }

    /* ------ Start test.----------- */
    QCLI_Printf(qcli_net_handle, "****************************************************************\n");
    QCLI_Printf(qcli_net_handle, "IOT RAW TX Test\n" );
    QCLI_Printf(qcli_net_handle, "****************************************************************\n");
    QCLI_Printf(qcli_net_handle, "Remote IP addr: %s\n", ip_str);
    QCLI_Printf(qcli_net_handle, "Socket protocol number: %d\n", p_tCxt->params.tx_params.port);
    QCLI_Printf(qcli_net_handle, "Message size: %d\n", p_tCxt->params.tx_params.packet_size);
    QCLI_Printf(qcli_net_handle, "Number of messages: %d\n", p_tCxt->params.tx_params.packet_number);
    QCLI_Printf(qcli_net_handle, "Delay in ms: %d\n", p_tCxt->params.tx_params.interval);
	QCLI_Printf(qcli_net_handle, "hdrincl: %d\n", isHdlcset);
    QCLI_Printf(qcli_net_handle, "Press Ctrl+C to terminate test\n");
    QCLI_Printf(qcli_net_handle, "****************************************************************\n");

    /* Create RAW socket */
	proto_num = p_tCxt->params.tx_params.port;
    if ((p_tCxt->sock_peer = qapi_socket(family, SOCK_RAW, proto_num)) == A_ERROR) {
        QCLI_Printf(qcli_net_handle, "Socket creation failed\n");
        goto ERROR_1;
    }

    if(p_tCxt->params.tx_params.bind_enabled)

    {	    /* Bind socket */
       if (qapi_bind(p_tCxt->sock_peer, addr, addrlen) == (UINT)A_ERROR)
       {
          QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
          goto ERROR_2;
       }
    }

    ret = qapi_setsockopt(p_tCxt->sock_peer, 0, IP_HDRINCL, &isHdlcset, 0);
	if(ret != 0)
	{
        QCLI_Printf(qcli_net_handle, "Setting header include socket option failed \n");
		return ;
    }
    if (v6_enable_flag[app_id])
    {
    	if(set_IP6_exclude_list(p_tCxt, &ip6allzero, 5, 0, 0, 1, 1) == 0)
		{
			QCLI_Printf(qcli_net_handle, "Init exclude list: \n");
			get_IP6_exclude_list(p_tCxt, 0, 1);
		}
		else
		{
			QCLI_Printf(qcli_net_handle, "Fail to set exclude list.\n");
		}
    }
	else
	{
	    if(set_IP_exclude_list(p_tCxt,0, 5, 0, 0, 1, 1) == 0)
		{
			QCLI_Printf(qcli_net_handle, "Init exclude list: \n");
			get_IP_exclude_list(p_tCxt, 0, 1);
		}
		else
		{
			QCLI_Printf(qcli_net_handle, "Fail to set exclude list.\n");
		}
	}
    /* Sending.*/
    QCLI_Printf(qcli_net_handle, "Sending RAW Data \n");

    netbuf_id = QAPI_NETBUF_APP;
    send_flag = 0;

    /*Reset all counters*/
    cur_packet_number = 0;
    i = 0;
    n_send_ok = 0;

    app_get_time(&p_tCxt->first_time);

    is_test_done = 0;
    while ( !is_test_done )
    {
        if (bench_quit[app_id][BENCH_TX])
        {
            is_test_done = 1;
            break;
        }

        /* allocate the buffer, if needed */
        if ( !p_tCxt->buffer )
        {
            while ((p_tCxt->buffer = qapi_Net_Buf_Alloc(packet_size, netbuf_id)) == NULL)
            {
                /*Wait till we get a buffer*/
                if (bench_quit[app_id][BENCH_TX])
                {
                    is_test_done = 1;
                    goto ERROR_2;
                }
                /*Allow small delay to allow other thread to run*/
                app_msec_delay(100);
            }

            if (netbuf_id == QAPI_NETBUF_APP) {
                /* Clear the buffer */
                memset(p_tCxt->buffer, 0, packet_size);
            } else {
                
            }
        }
        if(v6_enable_flag[app_id])
        {
        	build_IPv6_packet(p_tCxt, isHdlcset, 0);
        }
		else
		{
			build_IP_packet(p_tCxt, isHdlcset, 0);
		}
        do
        {
                send_bytes = qapi_sendto(p_tCxt->sock_peer, p_tCxt->buffer, packet_size, send_flag, to, tolen) ;

            if ( send_bytes != packet_size ) {
                int errno = qapi_errno(p_tCxt->sock_peer);
                QCLI_Printf(qcli_net_handle, "Failed on a call to qapi_sendto, send_bytes=%d, errno=%d\n", send_bytes, errno); 
                if ( errno == 0 ||
                        (EPIPE == errno) ||
                        (ENOTSOCK == errno) ||
                        (EBADF == errno) )
                {
                    // the socket has closed - no point of continuing the test
                    QCLI_Printf(qcli_net_handle, "Socket got closed, errno=%d\n", errno);
                    if ( !v6_enable_flag[app_id] && (send_bytes >= 0) && (send_bytes < packet_size) ) {
                        p_tCxt->buffer = 0;
                    }
                    is_test_done = 1;
                    break;
                }
                else {
                    if ( ENOBUFS != errno ) {
                        QCLI_Printf(qcli_net_handle, "Failed on a call to qapi_sendto, send_bytes=%d, errno=%d\n", send_bytes, errno);
                    }
                    // severe push back, let the other processes run for 1 ms
                    app_msec_delay(30);
                }
            }
            else {
                cur_packet_number ++;
            }

            app_get_time(&p_tCxt->last_time);
            if (++i >= 500)
            {
                QCLI_Printf(qcli_net_handle, ".");
                i = 0;
            }

            /* Useful notes related to UDP  send:
             *
             * In UDP we can have the following qapi_send() return codes:
             * IPv4:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             *  return_code < packet_size: the buffer sent partially, qapi_send() freed the buffer
             * IPv6:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             */

    #if 1 //def UDP_TX_DEBUG
            QCLI_Printf(qcli_net_handle, "App %d: %d send_bytes = %d\n", app_id, cur_packet_number, send_bytes);
    #endif

            /****Bandwidth control***********/
         if(p_tCxt->params.tx_params.interval)
        {
           if(p_tCxt->test_type == SSL_TX)
           {
             app_msec_delay(p_tCxt->params.tx_params.interval);
           }
           else if(cur_packet_number % p_tCxt->params.tx_params.interval == 0)
           {
             app_msec_delay(998);
           }
           else if(p_tCxt->test_type == RAW_IP_TX)
           {
             app_msec_delay(p_tCxt->params.tx_params.interval);
           }
        }
            if (send_bytes > 0)
            {
                p_tCxt->bytes += send_bytes;
                ++n_send_ok;
            }

            /*Test mode can be "number of packets" or "fixed time duration"*/
            if (p_tCxt->params.tx_params.test_mode == PACKET_TEST)
            {
                if ((cur_packet_number >= p_tCxt->params.tx_params.packet_number))
                {
                    is_test_done = 1;
                    break;
                }
            }
            else if (p_tCxt->params.tx_params.test_mode == TIME_TEST)
            {
                if (check_test_time(p_tCxt))
                {
                    is_test_done = 1;
                    break;
                }
            }
        }
        while ( !((is_test_done) || (send_bytes == packet_size) || (NULL == p_tCxt->buffer) || (bench_quit[app_id][BENCH_TX] != 0)) );
    } /* send loop */

    if ( p_tCxt->buffer )
    {
        qapi_Net_Buf_Free(p_tCxt->buffer, netbuf_id);
    }

    QCLI_Printf(qcli_net_handle, "\nSent %u packets, %llu bytes to %s %d (%u)\n",
            cur_packet_number, p_tCxt->bytes, ip_str, p_tCxt->params.tx_params.port, cur_packet_number - n_send_ok);

    result = QAPI_OK;

    if (result != QAPI_OK){

#if INCLUDE_SSLDEMO
        QCLI_Printf(qcli_net_handle, "RAW Transmit test failed, did not receive Ack from Peer\n");
        //QCLI_Printf(qcli_net_handle, "%s Transmit test failed, did not receive Ack from Peer\n", p_tCxt->test_type == SSL_TX ? "SSL" : "UDP");
#else
        QCLI_Printf(qcli_net_handle, "RAW Transmit test failed, did not receive Ack from Peer\n");
#endif

    }

ERROR_2:
    print_test_results(p_tCxt);
    qapi_socketclose(p_tCxt->sock_peer);

ERROR_1:
    QCLI_Printf(qcli_net_handle, BENCH_TEST_COMPLETED);

}


static void qca_raw_rx (THROUGHPUT_CXT *p_tCxt, int app_id)
{
	    uint16_t port;
		int32_t  received = 0;
		int32_t  conn_sock;
		struct sockaddr_in	local_addr;
		struct sockaddr_in6 local_addr6;
		struct sockaddr *addr;
		int addrlen;
		struct sockaddr_in	foreign_addr;
		struct sockaddr_in6 foreign_addr6;
		int32_t fromlen;
		struct sockaddr *from;
		void *sin_addr;
		char ip_str[48];
		int32_t netbuf_id = QAPI_NETBUF_APP;
		int family;
		struct timeval timeout;
		int ret = -1;
		int ishdlcset = p_tCxt->params.rx_params.hdrincl;
		int rawsock_prot = 80;
		boolean end_flag = FALSE;
		int32_t i = 0;
		int32_t j = 0;
		fd_set rset;

		uint16_t test_mode = 0;
		int exclude_list_size = 0;
		int32_t  second_sock = 0;
		unsigned long server_addr = 0;
	    ip6_addr server_addr6; /*+RAW IPv6*/
		time_struct_t change_point_time;
		time_struct_t current_time;
		/*+configure exclude list with file*/
		int el_file_source = 0;
		/*-configure exclude list with file*/
		int debug = p_tCxt->params.rx_params.debug_print;

    	memset(&change_point_time, 0, sizeof(time_struct_t));	
    	memset(&current_time, 0, sizeof(time_struct_t));
        local_addr.sin_addr.s_addr = 0L;	

		if((p_tCxt->buffer = qapi_Net_Buf_Alloc(CFG_PACKET_SIZE_MAX_RAW_RX, netbuf_id)) == NULL)
		{
			QCLI_Printf(qcli_net_handle, "Out of memory error\n");
			p_tCxt->buffer = NULL;
			return;
		}
	
		bench_quit[app_id][BENCH_RX] = 0;
		port = p_tCxt->params.rx_params.port;

        test_mode = p_tCxt->params.rx_params.test_mode;
        exclude_list_size = p_tCxt->params.rx_params.exclude_list_size;
/*+configure exclude list with file*/
    	if(exclude_list_size == 0)
			el_file_source = 1;
/*-configure exclude list with file*/
        if (v6_enable_flag[app_id])
        {
            family = AF_INET6;
            from = (struct sockaddr *)&foreign_addr6;
            sin_addr = &foreign_addr6.sin_addr;
            addr = (struct sockaddr *)&local_addr6;
            fromlen = addrlen = sizeof(struct sockaddr_in6);
    
            memset(&local_addr6, 0, sizeof(local_addr6));
            local_addr6.sin_port = htons(rawsock_prot);
            local_addr6.sin_family = family;
    
            if(p_tCxt->params.rx_params.local_if == 1)
            {
                memcpy(&local_addr6.sin_addr, p_tCxt->params.rx_params.local_v6addr, sizeof(ip6_addr));
            }
			/*+RAW IPv6*/
			memcpy(&local_addr6.sin_addr, p_tCxt->params.rx_params.mcIpv6addr, sizeof(ip6_addr)); //Hack <multicast IP> as the source IP
			//memcpy(&server_addr6, &ip6allzero, sizeof(ip6_addr)); ///Set server address to default address ::
			memcpy(&server_addr6, p_tCxt->params.rx_params.server_address6, sizeof(ip6_addr)); ///Set server address
			/*-RAW IPv6*/
        }
        else
        {
            family = AF_INET;
            from = (struct sockaddr *)&foreign_addr;
            sin_addr = &foreign_addr.sin_addr;
            addr = (struct sockaddr *)&local_addr;
            fromlen = addrlen = sizeof(struct sockaddr_in);
    
            memset(&local_addr, 0, sizeof(local_addr));
            local_addr.sin_port = htons(rawsock_prot);
            local_addr.sin_family = family;
    
            if(p_tCxt->params.rx_params.mcIpaddr != NULL) //Hack <multicast IP> as the source IP
            {
                local_addr.sin_addr.s_addr = p_tCxt->params.rx_params.mcIpaddr; 
            }
            else
            {
                if(p_tCxt->params.rx_params.local_if == 1)
                {
                    local_addr.sin_addr.s_addr = p_tCxt->params.rx_params.local_address;
                }
            }
 			if(p_tCxt->params.rx_params.server_address != NULL)
			{
				server_addr = p_tCxt->params.rx_params.server_address; //save server address
			}
			else
			{
			    server_addr = 0;
			}
        }
        /* Open socket */
        if ((p_tCxt->sock_local = qapi_socket(family, SOCK_RAW, port))== A_ERROR)
        {
            QCLI_Printf(qcli_net_handle, "ERROR: Socket creation error.\n");
            goto ERROR_1;
        }
        /* set socket timeout */
         timeout.tv_sec = 1;  
         timeout.tv_usec = 0;
    
        if (qapi_setsockopt (p_tCxt->sock_local, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) 
        {
            QCLI_Printf(qcli_net_handle,"setsockopt failed \n");
            goto ERROR_2;
        }
        ret = qapi_setsockopt(p_tCxt->sock_local, 0, IP_HDRINCL, &ishdlcset, 0);
        
        if(ret != 0)
        {
            QCLI_Printf(qcli_net_handle, "Setting IP Header Include socket option failed \n");
        }
        /* ------ Start test.----------- */
        QCLI_Printf(qcli_net_handle, "****************************************************\n");
        QCLI_Printf(qcli_net_handle, " RAW RX Test\n" );
        QCLI_Printf(qcli_net_handle, "****************************************************\n");

		QCLI_Printf(qcli_net_handle, "Test mode is %d\n", test_mode);
		/*+RAW IPv6*/
		if(!v6_enable_flag[app_id])
		{
			QCLI_Printf(qcli_net_handle, "Main RAW socket: protocol number is %d, will bind to addr %lx, but not bind yet\n", port, local_addr.sin_addr.s_addr);
		}
		else
		{
			QCLI_Printf(qcli_net_handle, "Main RAW socket: protocol number is %d, will bind to addr ", port);
			print_IPv6_address(local_addr6.sin_addr.s_addr);
			QCLI_Printf(qcli_net_handle, "but not bind yet\n");
		}
		/*-RAW IPv6*/
		QCLI_Printf(qcli_net_handle, "hdrincl: %d\n", ishdlcset);
		QCLI_Printf(qcli_net_handle, "Press 21 rx to termintate test\n");
		QCLI_Printf(qcli_net_handle, "****************************************************\n");
		/*+RAW IPv6*/
		if(v6_enable_flag[app_id])
		{
			if(test_mode == 0)
			{
				QCLI_Printf(qcli_net_handle, "Step 1: set and get IPv6 exclude list with normal address:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Exclude List set result %d \n", set_IP6_exclude_list(p_tCxt, &server_addr6,exclude_list_size, debug, 1, el_file_source, 0));
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP6_exclude_list(p_tCxt, debug, 0));
				}
				QCLI_Printf(qcli_net_handle, "Step 2: set invalid IPv6 address:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Exclude List set result %d \n", set_IP6_exclude_list(p_tCxt, &ip6allzero, 5, debug, 1, 0, 0));
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP6_exclude_list(p_tCxt, debug, 0));
				}
				QCLI_Printf(qcli_net_handle, "Step 3: set empty exclude list:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Before set:\n" );
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP6_exclude_list(p_tCxt, debug, 0));
					QCLI_Printf(qcli_net_handle, "    Set empty exclude List result %d \n", set_IP6_exclude_list(p_tCxt, &server_addr6, 0, debug, 1, 0, 0));
					QCLI_Printf(qcli_net_handle, "    After set:\n" );
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP6_exclude_list(p_tCxt, debug, 0));
				}
				QCLI_Printf(qcli_net_handle, "Step 4: before bind, call socket function:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Call qapi_fd_set() result %d \n", qapi_fd_set(p_tCxt->sock_local, &rset));
					build_IPv6_packet(p_tCxt, ishdlcset,1);
					memcpy(&foreign_addr6.sin_addr, &ip6loop, sizeof(ip6_addr));
					QCLI_Printf(qcli_net_handle, "    Call qapi_send_to() result %d \n", qapi_sendto(p_tCxt->sock_local, p_tCxt->buffer, ishdlcset*40+9, 0, from, fromlen));
					QCLI_Printf(qcli_net_handle, "    Call qapi_recv_from() result %d \n", qapi_recvfrom(p_tCxt->sock_local, (char*)(&p_tCxt->buffer[0]), CFG_PACKET_SIZE_MAX_RAW_RX, 0, from, &fromlen));
				}
				QCLI_Printf(qcli_net_handle, "Step 5: Two RAW socket coexist:\n");
				{
					QCLI_Printf(qcli_net_handle, "    Main Socket to bind address: ");
					print_IPv6_address(local_addr6.sin_addr.s_addr);
					QCLI_Printf(qcli_net_handle, "\n");
					QCLI_Printf(qcli_net_handle, "    Call qapi_bind() result %d \n",qapi_bind( p_tCxt->sock_local, addr, addrlen));
					QCLI_Printf(qcli_net_handle, "    Call qapi_getsockname() result %d \n",qapi_getsockname(p_tCxt->sock_local, from, &fromlen));
					QCLI_Printf(qcli_net_handle, "    Geted socket address: ");
					print_IPv6_address(foreign_addr6.sin_addr.s_addr);
					QCLI_Printf(qcli_net_handle, "\n");

					QCLI_Printf(qcli_net_handle, "    1. Coexist with 255_RAW_Socket:\n" );
					{
						if ((second_sock = qapi_socket(AF_INET6, SOCK_RAW, 255)) != A_ERROR)
						{	
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket: protocol number is %d\n", 255);
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call bind to ");
							print_IPv6_address(local_addr6.sin_addr.s_addr);
							QCLI_Printf(qcli_net_handle, " , result %d \n", qapi_bind( second_sock, addr, addrlen));
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call close, result %d\n",qapi_socketclose(second_sock));
						}
						else
						{
							QCLI_Printf(qcli_net_handle, "ERROR: 2nd Socket creation error.\n");
							goto ERROR_2;
						}
					}
					QCLI_Printf(qcli_net_handle, "    2. Coexist with !255_RAW_Socket:\n" );
					{
						if ((second_sock = qapi_socket(AF_INET6, SOCK_RAW, 112)) != A_ERROR)
						{	
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket: protocol number is %d\n", 112);
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call bind to ");
							print_IPv6_address(local_addr6.sin_addr.s_addr);
							QCLI_Printf(qcli_net_handle, " , result %d \n", qapi_bind( second_sock, addr, addrlen));
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call close, result %d\n",qapi_socketclose(second_sock));
						}
						else
						{
							QCLI_Printf(qcli_net_handle, "ERROR: 2nd Socket creation error.\n");
							goto ERROR_2;
						}
					}
				}

				goto ERROR_2;
			}
			if(set_IP6_exclude_list(p_tCxt, &server_addr6, exclude_list_size, debug, 0, el_file_source, 0) == 0)
			{
				QCLI_Printf(qcli_net_handle, "Init exclude list to not include server address ");
				print_IPv6_address(server_addr6.s_addr);
				QCLI_Printf(qcli_net_handle, "\n");
				get_IP6_exclude_list(p_tCxt, debug, 0);
			}
			else
			{
				QCLI_Printf(qcli_net_handle, "Fail to set exclude list.\n");
				if(test_mode == 2)
					goto ERROR_2;
			}

		}
		/*-RAW IPv6*/
		else
		{

			if(test_mode == 0)
			{
				QCLI_Printf(qcli_net_handle, "Step 1: set and get exclude list with normal address:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Exclude List set result %d \n", set_IP_exclude_list(p_tCxt,server_addr,exclude_list_size, debug, 1, el_file_source, 0));
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP_exclude_list(p_tCxt, debug, 0));
				}
				QCLI_Printf(qcli_net_handle, "Step 2: set invalid address:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Exclude List set result %d \n", set_IP_exclude_list(p_tCxt, 0, 5, debug, 1, 0, 0));
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP_exclude_list(p_tCxt, debug, 0));
				}
				QCLI_Printf(qcli_net_handle, "Step 3: set empty exclude list:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Before set:\n" );
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP_exclude_list(p_tCxt, debug, 0));
					QCLI_Printf(qcli_net_handle, "    Set empty exclude List result %d \n", set_IP_exclude_list(p_tCxt, server_addr, 0, debug, 1, 0, 0));
					QCLI_Printf(qcli_net_handle, "    After set:\n" );
					QCLI_Printf(qcli_net_handle, "    Exclude List get result %d \n", get_IP_exclude_list(p_tCxt, debug, 0));
				}
				QCLI_Printf(qcli_net_handle, "Step 4: before bind, call socket function:\n" );
				{
					QCLI_Printf(qcli_net_handle, "    Call qapi_fd_set() result %d \n", qapi_fd_set(p_tCxt->sock_local, &rset));
					build_IP_packet(p_tCxt, ishdlcset, 1);
					foreign_addr.sin_addr.s_addr = 0x0100007F;
					QCLI_Printf(qcli_net_handle, "    Call qapi_send_to() result %d \n", qapi_sendto(p_tCxt->sock_local, p_tCxt->buffer, ishdlcset*20+29, 0, from, fromlen));
					QCLI_Printf(qcli_net_handle, "    Call qapi_recv_from() result %d \n", qapi_recvfrom(p_tCxt->sock_local, (char*)(&p_tCxt->buffer[0]), CFG_PACKET_SIZE_MAX_RAW_RX, 0, from, &fromlen));
				}
				QCLI_Printf(qcli_net_handle, "Step 5: Two RAW socket coexist:\n" );
				{
					if (qapi_bind( p_tCxt->sock_local, addr, addrlen) != QAPI_OK)
					{
						QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
						goto ERROR_2;
					}
					QCLI_Printf(qcli_net_handle, "    Main RAW socket binded to %lx\n", local_addr.sin_addr.s_addr);
					QCLI_Printf(qcli_net_handle, "    1. Coexist with 255_RAW_Socket:\n" );
					{
						if ((second_sock = qapi_socket(AF_INET, SOCK_RAW, 255)) != A_ERROR)
						{	
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket: protocol number is %d\n", 255);
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call bind to %lx, result %d \n", local_addr.sin_addr.s_addr, qapi_bind( second_sock, addr, addrlen));
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call close, result %d\n",qapi_socketclose(second_sock));
						}
						else
						{
							QCLI_Printf(qcli_net_handle, "ERROR: 2nd Socket creation error.\n");
							goto ERROR_2;
						}
					}
					QCLI_Printf(qcli_net_handle, "    2. Coexist with !255_RAW_Socket:\n" );
					{
						if ((second_sock = qapi_socket(AF_INET, SOCK_RAW, 112)) != A_ERROR)
						{	
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket: protocol number is %d\n", 112);
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call bind to %lx, result %d \n", local_addr.sin_addr.s_addr, qapi_bind( second_sock, addr, addrlen));
							QCLI_Printf(qcli_net_handle, "        2nd RAW socket call close, result %d\n",qapi_socketclose(second_sock));
						}
						else
						{
							QCLI_Printf(qcli_net_handle, "ERROR: 2nd Socket creation error.\n");
							goto ERROR_2;
						}
					}
				}
				goto ERROR_2;
			}
		//config IP exclude list
			if(set_IP_exclude_list(p_tCxt, server_addr, exclude_list_size, debug, 0, el_file_source, 0) == 0)
			{
				QCLI_Printf(qcli_net_handle, "Init exclude list to not include server address %lx.\n", server_addr);
				get_IP_exclude_list(p_tCxt, debug, 0);
			}
			else
			{
				QCLI_Printf(qcli_net_handle, "Fail to set exclude list.\n");
				if(test_mode == 2)
					goto ERROR_2;
			}
		}
	
		/* Bind */
		if (qapi_bind( p_tCxt->sock_local, addr, addrlen) != QAPI_OK)
		{
			QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
			goto ERROR_2;
		}


        while (bench_quit[app_id][BENCH_RX]==0 && end_flag != TRUE) /* Main loop */
        {
    
            QCLI_Printf(qcli_net_handle, "Waiting\n");
    
            p_tCxt->bytes = 0;
            p_tCxt->pkts_recvd = 0;
            memset(ip_str,0,sizeof(ip_str));
    
            while(bench_quit[app_id][BENCH_RX]==0  && end_flag != TRUE)  /* Receive loop */
            {
                do
                {

                    if(bench_quit[app_id][BENCH_RX]){
                        goto ERROR_3;
                    }
    
                    /* block for 500msec or until a packet is received */
                    qapi_fd_zero(&rset);
					if (qapi_fd_set(p_tCxt->sock_local, &rset) < 0) 
					{
						QCLI_Printf(qcli_net_handle,"qapi_fd_set() failed \n");
						goto ERROR_2;
					}

                    conn_sock = qapi_select(&rset, NULL, NULL, 500);
                    if(conn_sock == A_ERROR)
                        goto ERROR_3;       // socket no longer valid 

					if(test_mode == 2)
					{
						app_get_time(&current_time);
						if(change_point_time.seconds == 0 && change_point_time.milliseconds == 0)
						{
							app_get_time(&change_point_time);
						}
						if((current_time.seconds - change_point_time.seconds) > 30)
						{
							j++;
							app_get_time(&change_point_time);
							if(j%2 == 1)
							{
								if(!v6_enable_flag[app_id])
								{
									if(set_IP_exclude_list(p_tCxt, server_addr, exclude_list_size, debug, 1, el_file_source, 0) == 0)
									{
										QCLI_Printf(qcli_net_handle, "Update exclude list to include %lx.\n", server_addr);
										get_IP_exclude_list(p_tCxt, debug, 0);
									}
									else
									{
										QCLI_Printf(qcli_net_handle, "ERROR: set exclude list error.\n");
										goto ERROR_3;
									}
								}
								else
								{
									if(set_IP6_exclude_list(p_tCxt, &server_addr6, exclude_list_size, debug, 1, el_file_source, 0) == 0)
									{
										QCLI_Printf(qcli_net_handle, "Update exclude list to include server address: ");
										print_IPv6_address(server_addr6.s_addr);
										QCLI_Printf(qcli_net_handle, "\n");
										get_IP6_exclude_list(p_tCxt, debug, 0);
									}
									else
									{
										QCLI_Printf(qcli_net_handle, "ERROR: set exclude list error.\n");
										goto ERROR_3;
									}
								}
							}
							else
							{
								if(!v6_enable_flag[app_id])
								{
									if(set_IP_exclude_list(p_tCxt, server_addr, exclude_list_size, debug, 0, el_file_source, 0) == 0)
									{
										QCLI_Printf(qcli_net_handle, "Update exclude list to not include %lx.\n", server_addr);
										get_IP_exclude_list(p_tCxt, debug, 0);
									}
									else
									{
										QCLI_Printf(qcli_net_handle, "ERROR: set exclude list error.\n");
										goto ERROR_3;
									}
								}
								else
								{	
									if(set_IP6_exclude_list(p_tCxt, &server_addr6, exclude_list_size, debug, 0, el_file_source, 0) == 0)
									{
										QCLI_Printf(qcli_net_handle, "Update exclude list to not include server address: ");
										print_IPv6_address(server_addr6.s_addr);
										QCLI_Printf(qcli_net_handle, "\n");
										get_IP6_exclude_list(p_tCxt, debug, 0);
									}
									else
									{
										QCLI_Printf(qcli_net_handle, "ERROR: set exclude list error.\n");
										goto ERROR_3;
									}
								}
							}
						}
					}
					
                }while(conn_sock == 0);
    
                /* Receive data */  

                received = qapi_recvfrom( p_tCxt->sock_local,
                           (char*)(&p_tCxt->buffer[0]),
                            CFG_PACKET_SIZE_MAX_RAW_RX, 0,
                            from, &fromlen);
				QCLI_Printf(qcli_net_handle, "RAW socket seletcted, recevied %d\n", received);

                ++i;

				if(received > 0)
				{
					p_tCxt->bytes += received;
					++p_tCxt->pkts_recvd;	
					if(!v6_enable_flag[app_id])
					{
						QCLI_Printf(qcli_net_handle, "App %d: %d, one packet receive %d bytes, from address %lx\n", app_id, i, received, foreign_addr.sin_addr.s_addr);
					}
					else
					{
						QCLI_Printf(qcli_net_handle, "App %d: %d, one packet receive %d bytes, from address: ", app_id, i, received);
						print_IPv6_address(foreign_addr6.sin_addr.s_addr);
						QCLI_Printf(qcli_net_handle, "\n");
					}
					if((p_tCxt->pkts_recvd % 100) == 1)
					{
						process_raw_ip_rx_data(p_tCxt->buffer ,received);
					}
					if(p_tCxt->pkts_recvd == 1)
					{
						app_get_time(&p_tCxt->first_time);
					}
				}
			} /* receive_loop */
	
	ERROR_3:
			app_get_time(&p_tCxt->last_time);
			QCLI_Printf(qcli_net_handle, "Received %d packets, from %s:%d,\n",
				    p_tCxt->pkts_recvd, inet_ntop(family, sin_addr, ip_str, sizeof(ip_str)),
					ntohs(from->sa_port));
	
			print_test_results (p_tCxt);

    
        } /* main loop */

    ERROR_2:

        qapi_socketclose(p_tCxt->sock_local);
    
    ERROR_1:
    
        QCLI_Printf(qcli_net_handle, BENCH_TEST_COMPLETED);
    
        if(p_tCxt->buffer)
            qapi_Net_Buf_Free(p_tCxt->buffer, netbuf_id);
    
}
/*-RAW IPv4*/	
int32_t g_cookie_uc = 1234;
int32_t g_cookie_mc = 4321;

#define UINT32MAX   (0xffffffff)
#ifndef min
#define  min(a,b)    (((a) <= (b)) ? (a) : (b))
#endif
#define RATIO_BASE    (10000)
/*+RAW IPv4*/
static void build_IP_packet(THROUGHPUT_CXT *p_tCxt, int hdrincl, int raw_rx)
{
	struct ip * ip_header;
	struct udp * udp_header;
	int j = 0;
	if(raw_rx== 1)
	{
		if(hdrincl == 1)
		{
			/* Append IP and UDP Header to the payload to be sent
			 */
			ip_header = (struct ip *)p_tCxt->buffer;
			udp_header = (struct udp *)(p_tCxt->buffer + sizeof(struct ip));

			ip_header->ip_ver_ihl = 0x45;
			ip_header->ip_tos = 16; // Low delay
			ip_header->ip_len = htons(49);
			ip_header->ip_id = htons(22222);
			ip_header->ip_time = 255; // hops
			ip_header->ip_prot = p_tCxt->params.rx_params.port; 
			// Source IP address, can use spoofed address here!!!
			ip_header->ip_src = p_tCxt->params.rx_params.mcIpaddr;
			// The destination IP address
			ip_header->ip_dest = 0x0100007F;

			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len  = htons(49 - sizeof(struct ip)); 
			for (j = 0; j < (49 - sizeof(struct ip) - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct ip) + sizeof(struct udp) + j) = 0x45; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0;
			ip_header->ip_chksum = 0;  
		}
		else
		{
			/* Append IP and UDP Header to the payload to be sent
			 */
			udp_header = (struct udp *)p_tCxt->buffer;

			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len  = htons(29); 
			for (j = 0; j < (29 - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct udp) + j) = 0x45; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0; 
		}
	}
	else{
		if(hdrincl == 1)
		{
			/* Append IP and UDP Header to the payload to be sent
			 */
			ip_header = (struct ip *)p_tCxt->buffer;
			udp_header = (struct udp *)(p_tCxt->buffer + sizeof(struct ip));

			ip_header->ip_ver_ihl = 0x45;
			ip_header->ip_tos = 16; // Low delay
			ip_header->ip_len = htons(p_tCxt->params.tx_params.packet_size);
			ip_header->ip_id = htons(22222);
			ip_header->ip_time = 255; // hops
			ip_header->ip_prot = p_tCxt->params.tx_params.port; 
			// Source IP address, can use spoofed address here!!!
			ip_header->ip_src = inet_addr(p_tCxt->params.tx_params.src_raw_ip_address);
			// The destination IP address
			ip_header->ip_dest = inet_addr(p_tCxt->params.tx_params.dst_raw_ip_address);

			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len  = htons(p_tCxt->params.tx_params.packet_size - sizeof(struct ip)); 
			for (j = 0; j < (p_tCxt->params.tx_params.packet_size - sizeof(struct ip) - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct ip) + sizeof(struct udp) + j) = 0x54; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0;
			ip_header->ip_chksum = 0;  
		}
		else
		{
			/* Append IP and UDP Header to the payload to be sent
			 */
			udp_header = (struct udp *)p_tCxt->buffer;

			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len  = htons(p_tCxt->params.tx_params.packet_size); 
			for (j = 0; j < (p_tCxt->params.tx_params.packet_size - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct udp) + j) = 0x54; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0; 
		}
	}
}

static void process_raw_ip_rx_data(char * app_data , int data_len)
{

   int i = 0;

   QCLI_Printf(qcli_net_handle, "process_raw_ip_rx_data in\n");

   if(app_data ==  NULL)
   {
     QCLI_Printf(qcli_net_handle, "Null RAW IP DATA received from the stack\n");
     return;
   }
   else
   {
      QCLI_Printf(qcli_net_handle, "RAW IP DATA received from the stack, it is: ");
      for(i = 0; i < data_len; i++)
      {
         QCLI_Printf(qcli_net_handle, "%02x",*(app_data + i));
      }
      QCLI_Printf(qcli_net_handle, "\n");
      return;
   }

}

static void free_IP_exclude_list(struct sockaddr_ep ** sockaddr_ep_header)
{
    struct sockaddr_ep * tmp_ep;
    struct sockaddr_ep * loop_ep;
    
    if(sockaddr_ep_header == NULL || *sockaddr_ep_header == NULL)
       return;
    
    loop_ep = *sockaddr_ep_header;
    
    while(loop_ep != NULL)
    {
       tmp_ep = loop_ep;
       loop_ep = loop_ep->sockaddr_ep_next; 
       qapi_Net_Buf_Free(tmp_ep->sockaddr_ep_addr, QAPI_NETBUF_APP);
       qapi_Net_Buf_Free(tmp_ep, QAPI_NETBUF_APP);
    }
    *sockaddr_ep_header = NULL;
}

static void print_exclude_list(struct sockaddr_ep * sockaddr_ep_header, int debug)
{
	struct sockaddr_ep * tmp_ep;
	int i = 0;
	char ip_str[48];
	QCLI_Printf(qcli_net_handle, "exclude list:");
	for(tmp_ep = sockaddr_ep_header; tmp_ep; tmp_ep = tmp_ep->sockaddr_ep_next)
	{
		if(debug == 1)
		{
			if(tmp_ep->sockaddr_ep_addr != NULL)
			{
           		QCLI_Printf(qcli_net_handle, " > %s",inet_ntop(((struct sockaddr_in *)tmp_ep->sockaddr_ep_addr)->sin_family, 
                                                          &((struct sockaddr_in *)tmp_ep->sockaddr_ep_addr)->sin_addr, 
                                                          ip_str, sizeof(ip_str))); 
			}
			else
			{
				QCLI_Printf(qcli_net_handle, " > NULL");
			}
			if(i%5 == 2)
			{
				QCLI_Printf(qcli_net_handle, "\n");
			}
		}
		else
		{
			if(tmp_ep == sockaddr_ep_header && tmp_ep->sockaddr_ep_next != NULL)
			{
				if(tmp_ep->sockaddr_ep_addr != NULL)
				{
        	   		QCLI_Printf(qcli_net_handle, " > %s",inet_ntop(((struct sockaddr_in *)tmp_ep->sockaddr_ep_addr)->sin_family, 
        	                                                  &((struct sockaddr_in *)tmp_ep->sockaddr_ep_addr)->sin_addr, 
        	                                                  ip_str, sizeof(ip_str))); 
				}
				else
				{
					QCLI_Printf(qcli_net_handle, " > NULL");
				}
				QCLI_Printf(qcli_net_handle, " > ......");	
			}
			else if(tmp_ep->sockaddr_ep_next == NULL)
			{
				if(tmp_ep->sockaddr_ep_addr != NULL)
				{
       	    		QCLI_Printf(qcli_net_handle, " > %s",inet_ntop(((struct sockaddr_in *)tmp_ep->sockaddr_ep_addr)->sin_family, 
        	                                                  &((struct sockaddr_in *)tmp_ep->sockaddr_ep_addr)->sin_addr, 
        	                                                  ip_str, sizeof(ip_str))); 
				}
				else
				{
					QCLI_Printf(qcli_net_handle, " > NULL");
				}
			}
			else
			{			
				if(tmp_ep->sockaddr_ep_addr == NULL)
				{
					QCLI_Printf(qcli_net_handle, " > NULL");
				}
			}
		}
       ++i;
    }
    QCLI_Printf(qcli_net_handle, " > list end, total length %d.\n", i);
}

static int set_IP_exclude_list(THROUGHPUT_CXT *p_tCxt, unsigned long addr, int size, int debug, int incl_addr, int file_source, int foreign)
{
    struct sockaddr_ep * sockaddr_ep_tmp = NULL;
    struct sockaddr_ep * sockaddr_ep_header = NULL;
    struct sockaddr_in * sock_addr_tmp;
    int i = 0;
    int res = 0;
	unsigned long addr_tmp;
/*+configure exclude list with file*/
	int fd = 0;
    char                       *line = NULL;
    unsigned int               line_len = 0;
	int read_len = 0;
	int addr_included = 0;

	if(file_source == 1)
	{
		fd = efs_open("/datatx/ipv4_exclude_list", O_RDONLY);
		if(fd > -1)
		{
			line = (char *)qapi_Net_Buf_Alloc(18,QAPI_NETBUF_APP);
			line_len = 18;
			
			QCLI_Printf(qcli_net_handle, "Build exclude list from config file, to %sinclude server address:\n", incl_addr?" ":"not ");
			while ( (read_len = data_getline(&line, &line_len, fd)) != -1 )
			{
				if(*(line + read_len - 1) == '\n') {*(line + read_len - 1) = '\0';}
				if ((inet_pton(AF_INET, line, &addr_tmp)) != 0)
				{
					QCLI_Printf(qcli_net_handle, "\nIncorrect IPv4 address %s\n", line);
					res = 3; /*3 invalide IP address*/
					break;
				}
				if(incl_addr == 1)
				{
					if(addr_included == 0 && addr_tmp == addr)
					{
						addr_included = 1;
					}
				}
				else
				{
					if(addr_tmp == addr)
					{
						QCLI_Printf(qcli_net_handle, " > same as server address, skip it");
						continue;
					}
				}
				if(debug == 1) QCLI_Printf(qcli_net_handle, " > %s", line);
				sock_addr_tmp = (struct sockaddr_in *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_in),QAPI_NETBUF_APP);
    
				if (sock_addr_tmp == NULL)
				{
					res = 1; /*1 means memory alloc fail*/
					break;
				}   
				sock_addr_tmp->sin_family = AF_INET;
				sock_addr_tmp->sin_port = 0;
				sock_addr_tmp->sin_addr.s_addr = addr_tmp;

				sockaddr_ep_tmp =(struct sockaddr_ep *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_ep),QAPI_NETBUF_APP);
				if (sockaddr_ep_tmp == NULL)
				{
					qapi_Net_Buf_Free(sock_addr_tmp, QAPI_NETBUF_APP);
					res = 1; /*1 means memory alloc fail*/
					break;
				}
				sockaddr_ep_tmp ->sockaddr_ep_addr = (struct sockaddr *)sock_addr_tmp;
				sockaddr_ep_tmp ->sockaddr_ep_next = sockaddr_ep_header;
				sockaddr_ep_header = sockaddr_ep_tmp;
				
				i++;
				if(i%5 == 4)
				{
					if(debug == 1) QCLI_Printf(qcli_net_handle, "\n");
				}
			}
			qapi_Net_Buf_Free(line, QAPI_NETBUF_APP);
			efs_close(fd);
			if(incl_addr == 1 && addr_included == 0)
			{
				sock_addr_tmp = (struct sockaddr_in *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_in),QAPI_NETBUF_APP);
    
				if (sock_addr_tmp == NULL)
				{
					res = 1; /*1 means memory alloc fail*/
				}
				else
				{
					sock_addr_tmp->sin_family = AF_INET;
					sock_addr_tmp->sin_port = 0;
					sock_addr_tmp->sin_addr.s_addr = addr;

					sockaddr_ep_tmp =(struct sockaddr_ep *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_ep),QAPI_NETBUF_APP);
					if (sockaddr_ep_tmp == NULL)
					{
						qapi_Net_Buf_Free(sock_addr_tmp, QAPI_NETBUF_APP);
						res = 1; /*1 means memory alloc fail*/
					}
					else
					{
						sockaddr_ep_tmp ->sockaddr_ep_addr = (struct sockaddr *)sock_addr_tmp;
						sockaddr_ep_tmp ->sockaddr_ep_next = sockaddr_ep_header;
						sockaddr_ep_header = sockaddr_ep_tmp;
						QCLI_Printf(qcli_net_handle, " > To include server address, add it");
					}
				}
			}
			QCLI_Printf(qcli_net_handle, " > End.\n");
		}
		else
		{
			QCLI_Printf(qcli_net_handle, "Not find exclude list configure file\n");
			return 2; /*2 means exclude list file open fail*/
		}
	}
	else
	{
		QCLI_Printf(qcli_net_handle, "Build exclude list locally, to %sinclude server address:\n", incl_addr?" ":"not ");
/*-configure exclude list with file*/
		if(incl_addr)
		{
	    	addr_tmp = addr;
			//QCLI_Printf(qcli_net_handle, " > To include server address");
		}
		else
		{
			addr_tmp = addr+0x01000000;
			//QCLI_Printf(qcli_net_handle, " > To not include server address");
		}	 
    	for( i = 0; i < size; i++)
    	{
        	sock_addr_tmp = (struct sockaddr_in *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_in),QAPI_NETBUF_APP);
    
			if (sock_addr_tmp == NULL)
			{
				res = 1; /*1 means memory alloc fail*/
				break;
 			}   
			sock_addr_tmp->sin_family = AF_INET;
			sock_addr_tmp->sin_port = 0;
			sock_addr_tmp->sin_addr.s_addr = addr_tmp;
			if((addr_tmp & 0xFF000000) == 0xFF000000)
			{
		 	   addr_tmp += 0x01010000;
			}
			else
			{
		  	  addr_tmp += 0x01000000;
			}

			sockaddr_ep_tmp =(struct sockaddr_ep *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_ep),QAPI_NETBUF_APP);
			if (sockaddr_ep_tmp == NULL)
			{
				qapi_Net_Buf_Free(sock_addr_tmp, QAPI_NETBUF_APP);
				res = 1; /*1 means memory alloc fail*/
				break;
			}
			sockaddr_ep_tmp ->sockaddr_ep_addr = (struct sockaddr *)sock_addr_tmp;
			sockaddr_ep_tmp ->sockaddr_ep_next = sockaddr_ep_header;
			sockaddr_ep_header = sockaddr_ep_tmp;
		}
		QCLI_Printf(qcli_net_handle, " > End.\n");
	}
    if(res  == 0)
    {
        QCLI_Printf(qcli_net_handle, "    To set ");
        print_exclude_list(sockaddr_ep_header, debug);
		if(foreign == 1)
		{
			res = qapi_setsockopt(p_tCxt->sock_peer,0,IP_EXCLUDE_LIST,sockaddr_ep_header,0);
		}
		else
		{
        	res = qapi_setsockopt(p_tCxt->sock_local,0,IP_EXCLUDE_LIST,sockaddr_ep_header,0); /*if qapi fail, -1 will be return*/
		}
    }
    free_IP_exclude_list(&sockaddr_ep_header);
    return res; 
}

static int get_IP_exclude_list(THROUGHPUT_CXT *p_tCxt, int debug, int foreign)
{
    struct sockaddr_ep * sockaddr_ep_tmp = NULL;
    int res = 0;
	if(foreign == 1)
	{
    	res = qapi_getsockopt(p_tCxt->sock_peer,0,IP_EXCLUDE_LIST,&sockaddr_ep_tmp,1);
	}
	else
	{
    	res = qapi_getsockopt(p_tCxt->sock_local,0,IP_EXCLUDE_LIST,&sockaddr_ep_tmp,1);
	}
    if(res == 0)
    {
        QCLI_Printf(qcli_net_handle, "    Geted ");
        print_exclude_list(sockaddr_ep_tmp, debug);
    }
    free_IP_exclude_list(&sockaddr_ep_tmp);
    return res;
}

/*+RAW IPv6*/
static void build_IPv6_packet(THROUGHPUT_CXT *p_tCxt, int hdrincl, int raw_rx)
{
	struct ipv6 * ip_header;
	struct udp * udp_header;
	int j = 0;
	if(raw_rx == 1)
	{
		if(hdrincl == 1)
		{
			/* Append IP and UDP Header to the payload to be sent
			 */
			ip_header = (struct ipv6 *)p_tCxt->buffer;
			udp_header = (struct udp *)(p_tCxt->buffer + sizeof(struct ipv6));

			ip_header->ip_ver_flo[0] = 0x60; /* version 6 */
			ip_header->ip_ver_flo[1] = 0;
			ip_header->ip_ver_flo[2] = 0;
			ip_header->ip_ver_flo[3] = 0;
			ip_header->ip_len = htons(9);
			ip_header->ip_nexthdr = p_tCxt->params.rx_params.port;
			ip_header->ip_hopcount = 7;

			memcpy(&ip_header->ip_src, p_tCxt->params.rx_params.mcIpv6addr, sizeof(struct in6_addr));
			memcpy(&ip_header->ip_dest, &ip6loop, sizeof(struct in6_addr));

			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len  = htons(9); 
			for (j = 0; j < (9 - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct ipv6) + sizeof(struct udp) + j) = 0x45; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0;
		}
		else
		{
			udp_header = (struct udp *)p_tCxt->buffer;
		
			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len	= htons(9); 
			for (j = 0; j < (9 - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct udp) + j) = 0x45; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0;
		}

	}
	else
	{
		if(hdrincl == 1)
		{
			/* Append IP and UDP Header to the payload to be sent
			 */
			ip_header = (struct ipv6 *)p_tCxt->buffer;
			udp_header = (struct udp *)(p_tCxt->buffer + sizeof(struct ipv6));

			ip_header->ip_ver_flo[0] = 0x60; /* version 6 */
			ip_header->ip_ver_flo[1] = 0;
			ip_header->ip_ver_flo[2] = 0;
			ip_header->ip_ver_flo[3] = 0;
			ip_header->ip_len = htons(p_tCxt->params.tx_params.packet_size- sizeof(struct ipv6));
			ip_header->ip_nexthdr = p_tCxt->params.tx_params.port;
			ip_header->ip_hopcount = 7;

			memcpy(&ip_header->ip_src, p_tCxt->params.tx_params.sv6addr, sizeof(struct in6_addr));
			memcpy(&ip_header->ip_dest, p_tCxt->params.tx_params.v6addr, sizeof(struct in6_addr));

			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len  = htons(p_tCxt->params.tx_params.packet_size - sizeof(struct ipv6)); 
			for (j = 0; j < (p_tCxt->params.tx_params.packet_size - sizeof(struct ipv6) - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct ipv6) + sizeof(struct udp) + j) = 0x54; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0;
		}
		else
		{
			udp_header = (struct udp *)p_tCxt->buffer;
		
			udp_header->ud_srcp = htons(5001);
			udp_header->ud_dstp = htons(80);
			udp_header->ud_len	= htons(p_tCxt->params.tx_params.packet_size); 
			for (j = 0; j < (p_tCxt->params.tx_params.packet_size - sizeof(struct udp)); j++)
			{
				*(p_tCxt->buffer + sizeof(struct udp) + j) = 0x54; //Fill content with 'T'
			} 
			udp_header->ud_cksum = 0;
		}
	}

}

static void print_IPv6_address(uint8 addr[16])
{
	int i;
	QCLI_Printf(qcli_net_handle, "{ ");
	for(i = 0; i < 16; i++)
	{
		QCLI_Printf(qcli_net_handle, "%02x ", addr[i]);
	}
	QCLI_Printf(qcli_net_handle, "}");
}
static void print6_exclude_list(struct sockaddr_ep * sockaddr_ep_header, int debug)
{
	struct sockaddr_ep * tmp_ep;
	int i = 0;
	char ip_str[48];
	QCLI_Printf(qcli_net_handle, "exclude list:");

	for(tmp_ep = sockaddr_ep_header; tmp_ep; tmp_ep = tmp_ep->sockaddr_ep_next)
	{
		if(debug == 1)
		{
			if(tmp_ep->sockaddr_ep_addr != NULL)
			{
				QCLI_Printf(qcli_net_handle, " > %s",inet_ntop(((struct sockaddr_in6 *)tmp_ep->sockaddr_ep_addr)->sin_family, 
													   &((struct sockaddr_in6 *)tmp_ep->sockaddr_ep_addr)->sin_addr, 
														 ip_str, sizeof(ip_str))); 
			}
			else
			{
				QCLI_Printf(qcli_net_handle, " > NULL");
			}
			if(i%4 == 2)
			{
				QCLI_Printf(qcli_net_handle, "\n");
			}
		}
		else
		{
			if(tmp_ep == sockaddr_ep_header && tmp_ep->sockaddr_ep_next != NULL)
			{
				if(tmp_ep->sockaddr_ep_addr != NULL)
				{
           			QCLI_Printf(qcli_net_handle, " > %s",inet_ntop(((struct sockaddr_in6 *)tmp_ep->sockaddr_ep_addr)->sin_family, 
                	                                          &((struct sockaddr_in6 *)tmp_ep->sockaddr_ep_addr)->sin_addr, 
               	                                           ip_str, sizeof(ip_str))); 
				}
				else
				{
					QCLI_Printf(qcli_net_handle, " > NULL");
				}
				QCLI_Printf(qcli_net_handle, " > ......");	
			}
			else if(tmp_ep->sockaddr_ep_next == NULL)
			{
				if(tmp_ep->sockaddr_ep_addr != NULL)
				{
           			QCLI_Printf(qcli_net_handle, " > %s",inet_ntop(((struct sockaddr_in6 *)tmp_ep->sockaddr_ep_addr)->sin_family, 
               	                                           &((struct sockaddr_in6 *)tmp_ep->sockaddr_ep_addr)->sin_addr, 
                 	                                         ip_str, sizeof(ip_str))); 
				}
				else
				{
					QCLI_Printf(qcli_net_handle, " > NULL");
				}
			}
			else
			{			
				if(tmp_ep->sockaddr_ep_addr == NULL)
				{
					QCLI_Printf(qcli_net_handle, " > NULL");
				}
			}
		}
		++i;
    }
    QCLI_Printf(qcli_net_handle, " > list end, total length %d.\n", i);

}

static int set_IP6_exclude_list(THROUGHPUT_CXT *p_tCxt, ip6_addr * addr, int size, int debug, int incl_addr, int file_source, int foreign)
{
	struct sockaddr_ep * sockaddr_ep_tmp = NULL;
	struct sockaddr_ep * sockaddr_ep_header = NULL;
	struct sockaddr_in6 * sock_addr_tmp;
	int i = 0;
	int res = 0;
	ip6_addr tmp;
/*+configure exclude list with file*/
	int fd = 0;
	char					   *line = NULL;
	unsigned int			   line_len = 0;
	int read_len = 0;
	int addr_included = 0;
	
	if(file_source == 1)
	{
		fd = efs_open("/datatx/ipv6_exclude_list", O_RDONLY);
		if(fd > -1)
		{
			line = (char *)qapi_Net_Buf_Alloc(41,QAPI_NETBUF_APP);
			line_len = 41;
				
			QCLI_Printf(qcli_net_handle, "Build exclude list from config file, to %sinclude server address:\n", incl_addr?" ":"not ");
			while ( (read_len = data_getline(&line, &line_len, fd)) != -1 )
			{
				if(*(line + read_len - 1) == '\n') {*(line + read_len - 1) = '\0';}
				if ((inet_pton(AF_INET6, line, tmp.s_addr)) != 0)
				{
					QCLI_Printf(qcli_net_handle, "\nIncorrect IPv6 address %s\n", line);
					res = 3; /*3 invalide IP address*/
					break;
				}
				if(incl_addr == 1)
				{
					if(addr_included == 0 && memcmp(&tmp, addr, sizeof(sock_addr_tmp->sin_addr)) == 0)
					{
						addr_included = 1;
					}
				}
				else
				{
					if(memcmp(&tmp, addr, sizeof(sock_addr_tmp->sin_addr)) == 0)
					{
						QCLI_Printf(qcli_net_handle, " > same as server address, skip it");
						continue;
					}
				}
				if(debug == 1) QCLI_Printf(qcli_net_handle, " > %s", line);

				sock_addr_tmp = (struct sockaddr_in6 *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_in6),QAPI_NETBUF_APP);
	
	   			if (sock_addr_tmp == NULL)
	    		{
	        		res = 1;
					break;
	    		}	
	    		sock_addr_tmp->sin_family = AF_INET6;
	    		sock_addr_tmp->sin_port = 0;
				memcpy(&sock_addr_tmp->sin_addr, &tmp, sizeof(sock_addr_tmp->sin_addr));

				sockaddr_ep_tmp =(struct sockaddr_ep *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_ep),QAPI_NETBUF_APP);
				if (sockaddr_ep_tmp == NULL)
				{
					qapi_Net_Buf_Free(sock_addr_tmp, QAPI_NETBUF_APP);
					res = 1;
					break;
				}
				sockaddr_ep_tmp ->sockaddr_ep_addr = (struct sockaddr *)sock_addr_tmp;
				sockaddr_ep_tmp ->sockaddr_ep_next = sockaddr_ep_header;
				sockaddr_ep_header = sockaddr_ep_tmp;
			
				i++;
				if(i%4 == 3)
				{
					if(debug == 1) QCLI_Printf(qcli_net_handle, "\n");
				}
			}
			qapi_Net_Buf_Free(line, QAPI_NETBUF_APP);
			efs_close(fd);
			if(incl_addr == 1 && addr_included == 0)
			{				
				sock_addr_tmp = (struct sockaddr_in6 *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_in6),QAPI_NETBUF_APP);
		
				if (sock_addr_tmp == NULL)
				{
					res = 1; /*1 means memory alloc fail*/
				}
				else
				{
					sock_addr_tmp->sin_family = AF_INET6;
					sock_addr_tmp->sin_port = 0;
					memcpy(&sock_addr_tmp->sin_addr, addr, sizeof(sock_addr_tmp->sin_addr));
	
					sockaddr_ep_tmp =(struct sockaddr_ep *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_ep),QAPI_NETBUF_APP);
					if (sockaddr_ep_tmp == NULL)
					{
						qapi_Net_Buf_Free(sock_addr_tmp, QAPI_NETBUF_APP);
						res = 1; /*1 means memory alloc fail*/
					}
					else
					{
						sockaddr_ep_tmp ->sockaddr_ep_addr = (struct sockaddr *)sock_addr_tmp;
						sockaddr_ep_tmp ->sockaddr_ep_next = sockaddr_ep_header;
						sockaddr_ep_header = sockaddr_ep_tmp;
						QCLI_Printf(qcli_net_handle, " > To include server address, add it");
					}
				}
			}
			QCLI_Printf(qcli_net_handle, " > End.\n");
		}
		else
		{
			QCLI_Printf(qcli_net_handle, "Not find exclude list configure file\n");
			return 2; /*2 means exclude list file open fail*/
		}
	}
	else
	{
		QCLI_Printf(qcli_net_handle, "Build exclude list locally, to %sinclude server address:\n", incl_addr?" ":"not ");
	/*-configure exclude list with file*/

		if(incl_addr)
		{
			memcpy(&tmp, addr, sizeof(sock_addr_tmp->sin_addr));
			//QCLI_Printf(qcli_net_handle, " > To include server address");
		}
		else
		{
			memcpy(&tmp, addr, sizeof(sock_addr_tmp->sin_addr));
			tmp.s_addr[15] += 1;
			//QCLI_Printf(qcli_net_handle, " > To not include server address");
		}
 
		for( i = 0; i < size; i++)
		{
			sock_addr_tmp = (struct sockaddr_in6 *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_in6),QAPI_NETBUF_APP);
	
	   		if (sock_addr_tmp == NULL)
	    	{
	        	res = 1;
				break;
	    	}	
	    	sock_addr_tmp->sin_family = AF_INET6;
	    	sock_addr_tmp->sin_port = 0;
			memcpy(&sock_addr_tmp->sin_addr, &tmp, sizeof(sock_addr_tmp->sin_addr));

			if(tmp.s_addr[15] == 255)
			{
				tmp.s_addr[15] += 1;
				tmp.s_addr[14] += 1;
			}
			else
			{		
				tmp.s_addr[15] += 1;
			}

		
			sockaddr_ep_tmp =(struct sockaddr_ep *)qapi_Net_Buf_Alloc(sizeof(struct sockaddr_ep),QAPI_NETBUF_APP);
			if (sockaddr_ep_tmp == NULL)
			{
				qapi_Net_Buf_Free(sock_addr_tmp, QAPI_NETBUF_APP);
				res = 1;
				break;
			}
			sockaddr_ep_tmp ->sockaddr_ep_addr = (struct sockaddr *)sock_addr_tmp;
			sockaddr_ep_tmp ->sockaddr_ep_next = sockaddr_ep_header;
			sockaddr_ep_header = sockaddr_ep_tmp;
		}
		QCLI_Printf(qcli_net_handle, " > End.\n");
	}
	if(res  == 0)
	{
		QCLI_Printf(qcli_net_handle, "	 To set ");
		print6_exclude_list(sockaddr_ep_header, debug);
		if(foreign == 1)
		{
			res = qapi_setsockopt(p_tCxt->sock_peer,0,IP_EXCLUDE_LIST,sockaddr_ep_header,0);
		}
		else
		{
			res = qapi_setsockopt(p_tCxt->sock_local,0,IP_EXCLUDE_LIST,sockaddr_ep_header,0);
		}
	}
	free_IP_exclude_list(&sockaddr_ep_header);
	return res;
}
static int get_IP6_exclude_list(THROUGHPUT_CXT *p_tCxt, int debug, int foreign)
{
	struct sockaddr_ep * sockaddr_ep_tmp = NULL;
	int res = 0;
	if(foreign == 1)
	{
		res = qapi_getsockopt(p_tCxt->sock_peer,0,IP_EXCLUDE_LIST,&sockaddr_ep_tmp,1);
	}
	else
	{
		res = qapi_getsockopt(p_tCxt->sock_local,0,IP_EXCLUDE_LIST,&sockaddr_ep_tmp,1);
	}
	if(res == 0)
	{
		QCLI_Printf(qcli_net_handle, "    Geted ");
		print6_exclude_list(sockaddr_ep_tmp, debug);
	}
	free_IP_exclude_list(&sockaddr_ep_tmp);
    return res;
}
/*-RAW IPv6*/
/*-RAW IPv4*/
static unsigned short ratio(uint32_t numerator, uint32_t denominator, unsigned short base)
{
    unsigned short ret = 0;
    if (base) {
        if (denominator) {
            if (numerator) {
                unsigned short multiplier = min(UINT32MAX/numerator, base);
                ret = ((numerator*multiplier)/denominator)*(base/multiplier);
            } else {
                ret = 0;
            }
        } else {
            QCLI_Printf(qcli_net_handle, "warning, denominator=%d\n", denominator);
        }
    } else {
        QCLI_Printf(qcli_net_handle, "warning, base=%d not supported\n", base);
    }
    return ret;
}

void rxreorder_udp_payload_init 
    (stat_udp_pattern_t *stat_udp)
{
    memset(stat_udp, 0, sizeof(stat_udp_pattern_t));
}

int rxreorder_udp_payload_valid
    (stat_udp_pattern_t *stat_udp)
{
    return !!(stat_udp->stat_valid);
}

void rxreorder_udp_payload_statistics 
    (stat_udp_pattern_t *stat_udp, char* buffer, int32_t len)
{
    UDP_PATTERN_PACKET udp_pattern;

    if (len < sizeof(UDP_PATTERN_PACKET)) {
        return;
    }

    memcpy(&udp_pattern, buffer, sizeof(UDP_PATTERN_PACKET));
    if (udp_pattern.code != CODE_UDP) {
        return;
    }

    if (!rxreorder_udp_payload_valid(stat_udp)) {
        stat_udp->stat_valid = 1;
        stat_udp->seq_last = udp_pattern.seq;
    }
    stat_udp->pkts_seq_recvd++;
    if (IEEE80211_SN_LESS(udp_pattern.seq, stat_udp->seq_last)) {
        stat_udp->pkts_seq_less++;
    } else {
        stat_udp->seq_last = udp_pattern.seq;
    }
}

void rxreorder_udp_payload_report 
    (stat_udp_pattern_t *stat_udp)
{
    if (!rxreorder_udp_payload_valid(stat_udp)) {
        return;
    }

    if (stat_udp->pkts_plan) {
        stat_udp->ratio_of_drop = ratio((stat_udp->pkts_plan-stat_udp->pkts_recvd), stat_udp->pkts_plan, RATIO_BASE);
    }
    stat_udp->ratio_of_seq_less = ratio(stat_udp->pkts_seq_less, stat_udp->pkts_seq_recvd, RATIO_BASE);
    QCLI_Printf(qcli_net_handle, "udp pkts: plan=%d recvd=%d drop_ratio=%d/%d\n",
        stat_udp->pkts_plan, stat_udp->pkts_recvd, stat_udp->ratio_of_drop, RATIO_BASE);
    QCLI_Printf(qcli_net_handle, "udp pkts of seq: recvd=%d less=%d less_ratio=%d/%d\n",
        stat_udp->pkts_seq_recvd, stat_udp->pkts_seq_less, stat_udp->ratio_of_seq_less, RATIO_BASE);
}

/************************************************************************
* NAME: qca_udp_rx
*
* DESCRIPTION: Start throughput UDP server.
************************************************************************/

static void qca_udp_rx(THROUGHPUT_CXT *p_tCxt, int app_id)
{
    uint16_t port;
    int32_t  received;
    int32_t  conn_sock;
    struct sockaddr_in  local_addr;
    struct sockaddr_in6 local_addr6;
    struct sockaddr *addr;
    int addrlen;
    struct sockaddr_in  foreign_addr;
    struct sockaddr_in6 foreign_addr6;
    int32_t fromlen;
    struct sockaddr *from;
    void *sin_addr;
    char ip_str[48];
    int32_t netbuf_id = QAPI_NETBUF_APP;
    int family;
    int32_t i = 0;
    int32_t is_first = 1;
    stat_udp_pattern_t stat_udp;
    struct timeval timeout;
	int error;
	int opt_val;

#if INCLUDE_SSLDEMO
    SSL_INST *ssl = &ssl_inst[app_id][SSL_SERVER_INST];
#endif

    uint32_t  from_ip_address;
     ssl->ssl = 0;
    if((p_tCxt->buffer = qapi_Net_Buf_Alloc(CFG_PACKET_SIZE_MAX_RX, netbuf_id)) == NULL)
    {
        QCLI_Printf(qcli_net_handle, "Out of memory error\n");
        p_tCxt->buffer = NULL;
        return;
    }

    bench_quit[app_id][BENCH_RX] = 0;
    port = p_tCxt->params.rx_params.port;

    if (v6_enable_flag[app_id])
    {
        family = AF_INET6;
        from = (struct sockaddr *)&foreign_addr6;
        sin_addr = &foreign_addr6.sin_addr;
        addr = (struct sockaddr *)&local_addr6;
        fromlen = addrlen = sizeof(struct sockaddr_in6);

        memset(&local_addr6, 0, sizeof(local_addr6));
        local_addr6.sin_port = htons(port);
        local_addr6.sin_family = family;

        if(p_tCxt->params.rx_params.local_if == 1)
        {
            memcpy(&local_addr6.sin_addr, p_tCxt->params.rx_params.local_v6addr, sizeof(ip6_addr));
        }
    }
    else
    {
        family = AF_INET;
        from = (struct sockaddr *)&foreign_addr;
        sin_addr = &foreign_addr.sin_addr;
        addr = (struct sockaddr *)&local_addr;
        fromlen = addrlen = sizeof(struct sockaddr_in);

        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_port = htons(port);
        local_addr.sin_family = family;

        if(p_tCxt->params.rx_params.local_if == 1)
        {
            local_addr.sin_addr.s_addr = p_tCxt->params.rx_params.local_address;
        }
    }

    /* Open socket */
    if ((p_tCxt->sock_local = qapi_socket(family, SOCK_DGRAM, 0))== A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Socket creation error.\n");
        goto ERROR_1;
    }
    /* set socket timeout */
     timeout.tv_sec = 1;  
     timeout.tv_usec = 0;

     if (qapi_setsockopt (p_tCxt->sock_local, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) 
     {
      QCLI_Printf(qcli_net_handle,"setsockopt failed \n");
      goto ERROR_1;
     }

    /* Bind */
    if (qapi_bind( p_tCxt->sock_local, addr, addrlen) != QAPI_OK)
    {
        QCLI_Printf(qcli_net_handle, "ERROR: Socket bind error.\n");
        goto ERROR_2;
    }
    if(p_tCxt->params.rx_params.mcEnabled){
        if(v6_enable_flag[app_id]){
            struct ipv6_mreq {
                ip6_addr ipv6mr_multiaddr; /* IPv6 multicast addr */
                ip6_addr ipv6mr_interface; /* IPv6 interface address */
            }group6;
            memcpy(&group6.ipv6mr_multiaddr, p_tCxt->params.rx_params.mcIpv6addr, sizeof(ip6_addr));
            memcpy(&group6.ipv6mr_interface, p_tCxt->params.rx_params.local_v6addr, sizeof(ip6_addr));
            if(qapi_setsockopt(p_tCxt->sock_local,IPPROTO_IP,IPV6_JOIN_GROUP,(void *)&group6, sizeof(group6)) != A_OK){
               QCLI_Printf(qcli_net_handle, "ERROR: Socket set option failure.\n");
               goto ERROR_1;
            }
        }else{
            struct ip_mreq {
                uint32_t imr_multiaddr;   /* IP multicast address of group */
                uint32_t imr_interface;   /* local IP address of interface */
            } group;
            group.imr_multiaddr = p_tCxt->params.rx_params.mcIpaddr;
            group.imr_interface = p_tCxt->params.rx_params.local_address;
            if(qapi_setsockopt(p_tCxt->sock_local, IPPROTO_IP, IP_ADD_MEMBERSHIP,(void *)&group, sizeof(group)) != QAPI_OK){
                QCLI_Printf(qcli_net_handle, "ERROR: Socket set option failure.\n");
                goto ERROR_1;
            }
        }
    }

	if (option[app_id].ip_ire)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IRE_TMO, &option[app_id].ip_ire, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting IP_IRE_TMO option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	if (option[app_id].ip_maxfrags)
    {
        error = qapi_setsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IPR_MAX_FRAGS, &option[app_id].ip_maxfrags, sizeof(int));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting IP_IPR_MAX_FRAGS option failed with error[%d]\n", error);
           goto ERROR_1;
        }
    }

	qapi_getsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IRE_TMO, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t IP_IRE_TMO =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, IP_OPTIONS, IP_IPR_MAX_FRAGS, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t IP_IPR_MAX_FRAGS =%d\n",app_id, opt_val);

    /* ------ Start test.----------- */
    QCLI_Printf(qcli_net_handle, "****************************************************\n");
#if INCLUDE_SSLDEMO
    QCLI_Printf(qcli_net_handle, " %s RX Test\n", p_tCxt->test_type == SSL_RX ? "SSL" : "UDP");
#else
    QCLI_Printf(qcli_net_handle, " UDP RX Test\n" );
#endif
    QCLI_Printf(qcli_net_handle, "****************************************************\n");
    QCLI_Printf(qcli_net_handle, "Local port %d\n", port);
    QCLI_Printf(qcli_net_handle, "Press Ctrl+C to termintate test\n");
    QCLI_Printf(qcli_net_handle, "****************************************************\n");

    while (bench_quit[app_id][BENCH_RX]==0) /* Main loop */
    {

        QCLI_Printf(qcli_net_handle, "Waiting\n");

        p_tCxt->bytes = 0;
        p_tCxt->pkts_recvd = 0;
        memset(ip_str,0,sizeof(ip_str));
        rxreorder_udp_payload_init(&stat_udp);

        while(bench_quit[app_id][BENCH_RX]==0)   /* Receive loop */
        {
            do
            {
                fd_set rset;

                if(bench_quit[app_id][BENCH_RX]){
                    goto ERROR_3;
                }

                /* block for 500msec or until a packet is received */
                qapi_fd_zero(&rset);
                qapi_fd_set(p_tCxt->sock_local, &rset);

                conn_sock = qapi_select(&rset, NULL, NULL, 500);
                if(conn_sock == A_ERROR)
                    goto ERROR_3;       // socket no longer valid


            }while(conn_sock == 0);

            /* Receive data */



#if INCLUDE_SSLDEMO
            if (p_tCxt->test_type == SSL_RX)
            {
                if(ssl->ssl == QAPI_NET_SSL_INVALID_HANDLE)
                {
                    received = qapi_recvfrom( p_tCxt->sock_local,
                                              (char*)(&p_tCxt->buffer[0]),CFG_PACKET_SIZE_MAX_RX ,
                                              MSG_PEEK,
                                              from, &fromlen);
                    if(received < 0)
                        goto ERROR_2;

                    /* Bind this socket to the remote address, so DTLS packets
                     * will be sent there without specifying the address in
                     * every sendto call.
                     */
                    qapi_connect(p_tCxt->sock_local, from, fromlen);

                    if (ssl_rx_setup(p_tCxt, ssl, app_id) < 0)
                        goto ERROR_2;

                    continue;
                }
                else
                {
                    received = qapi_Net_SSL_Read(ssl->ssl, (char*)(&p_tCxt->buffer[0]), CFG_PACKET_SIZE_MAX_RX);
                }
            }
            else
#endif


            {
                received = qapi_recvfrom( p_tCxt->sock_local,
                        (char*)(&p_tCxt->buffer[0]),
                        CFG_PACKET_SIZE_MAX_RX, 0,
                        from, &fromlen);
            }

            ++i;

            if (received >= 0)
            {
                if(received > 8)
                {
                    from_ip_address = *((int32_t*)(&p_tCxt->buffer[0])+1);
                    if(htonl(from_ip_address)&0xff == 0xff)
                    {
                        g_cookie_mc = *((int32_t*)(&p_tCxt->buffer[0]));
                    }
                    else
                    {   
                        g_cookie_uc = *((int32_t*)(&p_tCxt->buffer[0]));
                    }
                }
                if (received != sizeof(EOT_PACKET) ||
                    ((EOT_PACKET *)p_tCxt->buffer)->code != (int)END_OF_TEST_CODE)
                {
                    time_struct_t ts;

                    app_get_time(&ts);
                    app_get_time(&p_tCxt->last_time);

                  //  QCLI_Printf(qcli_net_handle, "%d sec=%u ms=%u rcvd=%u\n", i, ts.seconds, ts.milliseconds, received);
                    p_tCxt->bytes += received;
                    ++p_tCxt->pkts_recvd;
                    rxreorder_udp_payload_statistics(&stat_udp,
                        p_tCxt->buffer, received);
                    if(is_first)
                    {
                        QCLI_Printf(qcli_net_handle, "Receiving\n");

                        app_get_time(&p_tCxt->first_time);

                        QCLI_Printf(qcli_net_handle, "first_time: sec=%u ms=%u\n",
                                p_tCxt->first_time.seconds,
                                p_tCxt->first_time.milliseconds);

                        is_first = 0;
                    }
                }
                else /* End of transfer. */
                {
                    app_get_time(&p_tCxt->last_time);
                    if (rxreorder_udp_payload_valid(&stat_udp)) {
                        stat_udp.pkts_plan = ((EOT_PACKET *)p_tCxt->buffer)->packet_count;
                    }

                    QCLI_Printf(qcli_net_handle, "last_time: sec=%u ms=%u rcvd=%u\n",
                            p_tCxt->last_time.seconds,
                            p_tCxt->last_time.milliseconds, received);

                    break;
                }
            }
            else
            {
                QCLI_Printf(qcli_net_handle, "%d received= %d\n", i, received); 
                break;
            }
        } /* receive_loop */

ERROR_3:
        QCLI_Printf(qcli_net_handle, "Received %llu bytes from %s:%d\n",
                p_tCxt->bytes, inet_ntop(family, sin_addr, ip_str, sizeof(ip_str)),
                ntohs(from->sa_port));
        if (rxreorder_udp_payload_valid(&stat_udp)) {
            stat_udp.pkts_recvd = p_tCxt->pkts_recvd;
            rxreorder_udp_payload_report(&stat_udp);
        }

        print_test_results (p_tCxt);


#if INCLUDE_SSLDEMO
        if (ssl->ssl != QAPI_NET_SSL_INVALID_HANDLE)
        {
            qapi_Net_SSL_Shutdown(ssl->ssl);
            ssl->ssl = QAPI_NET_SSL_INVALID_HANDLE;
        }

        /* Clear any remote host association on the socket. We can reuse addr
         * here, to set a zero remote address, since that sockaddr is only
         * used to bind the socket to a local port.
         */
        memset(addr, 0, addrlen);
        qapi_connect(p_tCxt->sock_local, addr, addrlen);
#endif

    break;

    } /* main loop */

ERROR_2:
    qapi_socketclose(p_tCxt->sock_local);

ERROR_1:

    QCLI_Printf(qcli_net_handle, BENCH_TEST_COMPLETED);

    if(p_tCxt->buffer)
        qapi_Net_Buf_Free(p_tCxt->buffer, netbuf_id);

}


/************************************************************************
 *         [0]           [1]  [2]   [3] [4]  [5] [6] [7] [8]
 * benchtx 192.168.1.100 2390 tcp   500  0   30   0  v4  sender_ip
 *         [0]                             [1]  [2] [3] [4] [5]   [6] [7] [8]
 * benchtx FE80::865D:D7FF:FE40:3498%wlan1 2390 udp 1400 1  10000 10  v6  sender_ip
 ************************************************************************/
QCLI_Command_Status_t benchtx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = BENCHTX_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
  #ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 14;
#else
  cmd_parser_temp->Command_Index = 11;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_benchtx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    THROUGHPUT_CXT tCxt;
    int32_t retval;
    int32_t max_packet_size;
    char *receiver_ip, *protocol;
    char *sender_ip = NULL;
    int i;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

   if(!tid_tx[app_id])
   {
    if (Parameter_Count < 7 || Parameter_Count > 11 || Parameter_List == NULL)
    {
      result = QCLI_STATUS_USAGE_E;
      goto end;
    }

    /* IPv4 or IPv6 */
    if (Parameter_Count == 7 || strcmp((char*)Parameter_List[7].String_Value, "v4") == 0)
    {
        v6_enable_flag[app_id] = 0;
    }
    else if (strcmp((char*)Parameter_List[7].String_Value, "v6") == 0)
    {
        v6_enable_flag[app_id] = 1;
    }
    else
    {
        result = QCLI_STATUS_USAGE_E;
        goto end;
    }

    memset(&tCxt, 0, sizeof(THROUGHPUT_CXT));

    if(Parameter_Count >= 9)
    {
        tCxt.params.tx_params.bind_enabled = 1;
		tCxt.params.tx_params.tx_port = Parameter_List[10].Integer_Value;
		/*+RAW IPv4*/
		sender_ip =  (char*)Parameter_List[8].String_Value; 
		strlcpy(tCxt.params.tx_params.src_raw_ip_address,sender_ip,128);
		/*-RAW IPv4*/
    }

    receiver_ip = (char*)Parameter_List[0].String_Value;
	/*+RAW IPv4*/
    strlcpy(tCxt.params.tx_params.dst_raw_ip_address,receiver_ip,128);
    if(strcasecmp("raw", (char*)Parameter_List[2].String_Value)==0)
    {
        if(Parameter_Count < 9)
        {
            QCLI_Printf(qcli_net_handle, "Tx address is necessary for 255 RAW socket test\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
    }
	if(Parameter_Count == 10)
	{
	    if((strcasecmp("raw", (char*)Parameter_List[2].String_Value)==0) && Parameter_List[9].Integer_Is_Valid 
			&& (Parameter_List[9].Integer_Value == 1 || Parameter_List[9].Integer_Value == 0))
	    {
        	tCxt.params.tx_params.hdrincl = Parameter_List[9].Integer_Value;
	    }
		else
		{
		    QCLI_Printf(qcli_net_handle, "Invalid header include select, header_include is only for raw socket, please input 0 or 1\n");
            result = QCLI_STATUS_ERROR_E;
            goto end;
		}
	}
	else
	{
		if(strcasecmp("raw", (char*)Parameter_List[2].String_Value)==0)
	    {
			tCxt.params.tx_params.hdrincl = 1;
	    }
	}
	/*-RAW IPv4*/
    if (v6_enable_flag[app_id])
    {
        max_packet_size = V6_PACKET_SIZE_MAX_TX;

        /*Get IPv6 address of Peer*/
        if ((retval = inet_pton(AF_INET6, receiver_ip, tCxt.params.tx_params.v6addr)) != 0)
        {
            QCLI_Printf(qcli_net_handle, "Incorrect IPv6 address %s\n", receiver_ip);
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        if(tCxt.params.tx_params.bind_enabled )
        {
           if ((retval = inet_pton(AF_INET6, sender_ip, tCxt.params.tx_params.sv6addr)) != 0)
           {
              QCLI_Printf(qcli_net_handle, "Incorrect IPv6 address %s\n", sender_ip);
              result = QCLI_STATUS_ERROR_E;
              goto end;
           }
        }

        if ( QAPI_IS_IPV6_LINK_LOCAL(tCxt.params.tx_params.v6addr) )
        {
            /* if this is a link local address, then the interface must be specified after % */
            int host_argument_length = strlen(receiver_ip);
            char * interface_name_with_percent_char = strchr(receiver_ip, '%');
            char * interface_name = NULL;
            if ( interface_name_with_percent_char ) {
                interface_name = interface_name_with_percent_char + 1;
            }
            else {
                QCLI_Printf(qcli_net_handle, "Link local IPv6 address is used, must append %%interface_name to the address\n");
                QCLI_Printf(qcli_net_handle, "For example: fe80::a2ce:c8ff:fe03:68f4%%wlan1 if you intend to talk over link local address on interface wlan1\n");
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }

            retval = qapi_Net_IPv6_Get_Scope_ID(interface_name, &tCxt.params.tx_params.scope_id);
            if ( retval != 0 ) {
                QCLI_Printf(qcli_net_handle, "Failed to get scope id for the interface %s\n", interface_name);
                result = QCLI_STATUS_ERROR_E;
                goto end;
            }
        }
    }
    else
    {
        max_packet_size = CFG_PACKET_SIZE_MAX_TX;

        /*Get IPv4 address of Peer (network order)*/
        if ((retval = inet_pton(AF_INET, receiver_ip, &tCxt.params.tx_params.ip_address)) != 0)
        {
            QCLI_Printf(qcli_net_handle, "Incorrect IPv4 address %s\n", receiver_ip);
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }

        if(tCxt.params.tx_params.bind_enabled)
        {
            /*Get IPv4 address of Peer (network order)*/
            if ((retval = inet_pton(AF_INET, sender_ip, &tCxt.params.tx_params.sip_address)) != 0)
            {
               QCLI_Printf(qcli_net_handle, "Incorrect IPv4 address %s\n", sender_ip);
               result = QCLI_STATUS_ERROR_E;
               goto end;
            }
        }
    }

    /* Packet size */
    if (!Parameter_List[3].Integer_Is_Valid ||
        (tCxt.params.tx_params.packet_size = Parameter_List[3].Integer_Value) == 0
#ifdef CHECK_PACKET_SIZE
            || tCxt.params.tx_params.packet_size > max_packet_size
#endif
       )
    {
        QCLI_Printf(qcli_net_handle, "Invalid message size for %s, allowed %d\n",
                v6_enable_flag[app_id] ? "v6" : "v4", max_packet_size);
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    /* Test mode */
    if (!Parameter_List[4].Integer_Is_Valid ||
        ((tCxt.params.tx_params.test_mode = Parameter_List[4].Integer_Value) != TIME_TEST &&
         tCxt.params.tx_params.test_mode != PACKET_TEST))
    {
        QCLI_Printf(qcli_net_handle, "Invalid test mode, please enter 0 for duration test or 1 for packet test\n");
        result = QCLI_STATUS_ERROR_E;
        goto end;
    }

    /* Number of packets OR time period */
    if (tCxt.params.tx_params.test_mode == TIME_TEST)
    {
        tCxt.params.tx_params.tx_time = Parameter_List[5].Integer_Value;
    }
    else if (tCxt.params.tx_params.test_mode == PACKET_TEST)
    {
        tCxt.params.tx_params.packet_number = Parameter_List[5].Integer_Value;
    }

    /* Inter packet interval for Bandwidth control */
    tCxt.params.tx_params.interval = Parameter_List[6].Integer_Value;

    /* Port */
    tCxt.params.tx_params.port = Parameter_List[1].Integer_Value;

	/*+RAW IPv4*/
    /* Socket Type*/
    protocol = (char*)Parameter_List[2].String_Value;

    if(strcasecmp("raw", protocol)==0)
    {
        if(tCxt.params.tx_params.port > 255 || TCP_PROT == tCxt.params.tx_params.port /*|| UDP_PROT == tCxt.params.tx_params.port*/)
        {
            QCLI_Printf(qcli_net_handle, "Invalid socket protocol number, TCP(6) should not be used for rawsock\n");
            /*TODO: 0 allowed or not, UDP allowed or not*/
            result = QCLI_STATUS_ERROR_E;
            goto end;
        }
    }
	/*-RAW IPv4*/
    else{
        if (port_in_use[app_id] == tCxt.params.tx_params.port)
        {
           QCLI_Printf(qcli_net_handle, "%d is in use; use another port\n", port_in_use[app_id]);
        //result = QCLI_STATUS_ERROR_E;
        //goto end;
        }
        port_in_use[app_id] = tCxt.params.tx_params.port;
    }
    bench_quit[app_id][BENCH_TX] = 0;
    tid_tx[app_id] = qurt_thread_get_id();

    /* TCP */
    if (strcasecmp("tcp", protocol) == 0)
    {
        tCxt.test_type = TCP_TX;
        qca_tcp_tx(&tCxt, app_id);
    }
    /* UDP */
    else if(strcasecmp("udp", protocol) == 0)
    {
        tCxt.test_type = UDP_TX;
        qca_udp_tx(&tCxt, app_id);
    }
	/*+RAW IPv4*/
    else if(strcasecmp("raw", protocol)==0)
    {
      
       tCxt.test_type = RAW_IP_TX;
       qca_raw_tx(&tCxt, app_id);
    }
	/*-RAW IPv4*/


#if INCLUDE_SSLDEMO
    else if (strcasecmp("ssl", protocol) == 0)
    {
        if (ssl_role[app_id][SSL_CLIENT_INST] )
        {
            QCLI_Printf(qcli_net_handle, "ERROR: busy.\n");
            goto error;
        }
        if (ssl_inst[app_id][SSL_CLIENT_INST].sslCtx == QAPI_NET_SSL_INVALID_HANDLE || ssl_inst[app_id][SSL_CLIENT_INST].role != QAPI_NET_SSL_CLIENT_E)
        {
            QCLI_Printf(qcli_net_handle, "ERROR: SSL client not started (Use 'ssl start client' first).\n");
            goto error;
        }

        tCxt.test_type = SSL_TX;
        ssl_role[app_id][SSL_CLIENT_INST] = QAPI_NET_SSL_CLIENT_E;
        if((ssl_inst[app_id][SSL_CLIENT_INST].config_set == 1) && ((ssl_inst[app_id][SSL_CLIENT_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_0) || (ssl_inst[app_id][SSL_CLIENT_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_2)))
            qca_udp_tx(&tCxt, app_id);
        else
            qca_tcp_tx(&tCxt, app_id);
        ssl_role[app_id][SSL_CLIENT_INST] = (qapi_Net_SSL_Role_t)0;
    }
#endif



    else
    {
        QCLI_Printf(qcli_net_handle, "Invalid protocol: %s\n", protocol);
        goto error;
    }

    port_in_use[app_id] = 0;
    result = QCLI_STATUS_SUCCESS_E;
    goto end;

error:
    port_in_use[app_id] = 0;
    result = QCLI_STATUS_ERROR_E;
    goto end;
   }
   else
   {
    QCLI_Printf(qcli_net_handle, "Benchtx operation already running. \n");
     result = QCLI_STATUS_ERROR_E;
    goto end;
   }

end:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

/************************************************************************
 *          [0]                            [1]     [2]     [3]
 * benchrx [tcp|udp|ssl|raw] <port> [v4|v6] [<multicast IP>]
 ************************************************************************/
QCLI_Command_Status_t benchrx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
  multi_app_cmd_parser_t *cmd_parser_temp = NULL;
  QCLI_Command_Status_t result =  QCLI_STATUS_SUCCESS_E;

  cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
  if(cmd_parser_temp == NULL)
  {
    QCLI_Printf(qcli_net_handle, "Unable to parse given command");
    return QCLI_STATUS_ERROR_E;
  }

  cmd_parser_temp->cmd_id = BENCHRX_EVT;
  cmd_parser_temp->Command = (QCLI_Command_t *)&net_cmd_list[0];
#ifndef MINI_QCLI
  cmd_parser_temp->Command_Index = 13;
#else
  cmd_parser_temp->Command_Index = 10;
#endif

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t 
proc_benchrx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    char *protocol;
    int e = 0;
    char *receiver_ip;
    int i;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

   if(!tid_rx[app_id])
   {
    if (Parameter_Count < 2 || Parameter_Count > 9 || Parameter_List == NULL)/*+RAW IPv4*/
    {
      result= QCLI_STATUS_USAGE_E;
      goto ret;
    }

    /* IPv4 or IPv6 */
    if (Parameter_Count == 2 || strcmp((char*)Parameter_List[2].String_Value, "v4") == 0)
    {
        v6_enable_flag[app_id] = 0;
    }
    else if (strcmp((char*)Parameter_List[2].String_Value, "v6") == 0)
    {
        v6_enable_flag[app_id] = 1;
    }
    else
    {
        result = QCLI_STATUS_USAGE_E;
        goto ret;
    }
	/*+RAW IPv4*/
    protocol = (char*)Parameter_List[0].String_Value;

    /*Check port validity*/
    if(strcasecmp("raw", protocol) == 0)
    {
        if(Parameter_List[1].Integer_Value > 255 || TCP_PROT == Parameter_List[1].Integer_Value || UDP_PROT == Parameter_List[1].Integer_Value)
        {
            QCLI_Printf(qcli_net_handle, "Invalide socket protocol number.\n");
            e = -1;
            result = QCLI_STATUS_ERROR_E;
            goto ret;
        }
        if(Parameter_Count < 4)
        {
            QCLI_Printf(qcli_net_handle, "RAW socket must bind to local address.\n");
            e = -1;
            result = QCLI_STATUS_ERROR_E;
            goto ret;
        }
    }
	/*-RAW IPv4*/
    else{
        if (port_in_use[app_id] == Parameter_List[1].Integer_Value)
        {
            QCLI_Printf(qcli_net_handle, "%d is in use; use another port.\n", port_in_use[app_id]);
          //  result = QCLI_STATUS_ERROR_E;
          //  goto ret;
        }
    }
    if (p_rxtCxt[app_id])
    {
        QCLI_Printf(qcli_net_handle, "Please Press Ctrl+C to terminate the current RX test.\n");
        result = QCLI_STATUS_ERROR_E;
        goto ret;
    }

    /* Allocate Rx context */
    if ((p_rxtCxt[app_id] = qapi_Net_Buf_Alloc(sizeof(THROUGHPUT_CXT), QAPI_NETBUF_APP)) == NULL)
    {
        QCLI_Printf(qcli_net_handle, "No memory\n");
        result = QCLI_STATUS_ERROR_E;
        goto ret;
    }
    memset(p_rxtCxt[app_id], 0, sizeof(THROUGHPUT_CXT));

    p_rxtCxt[app_id]->params.rx_params.port = port_in_use[app_id] = Parameter_List[1].Integer_Value;
    bench_quit[app_id][BENCH_RX] = 0;

    if(Parameter_Count > 3){
        receiver_ip = (char*)Parameter_List[3].String_Value;
        if(v6_enable_flag[app_id]){
            /* IPv6 address: verify mcast address to be IPv6 */
            if ((inet_pton(AF_INET6, receiver_ip, p_rxtCxt[app_id]->params.rx_params.mcIpv6addr)) != 0)
            {
                QCLI_Printf(qcli_net_handle, "Incorrect IPv6 address %s\n", receiver_ip);
                e = -1;
                goto end;
            }
            if(QAPI_IS_IPV6_MULTICAST(p_rxtCxt[app_id]->params.rx_params.mcIpv6addr)){
                p_rxtCxt[app_id]->params.rx_params.mcEnabled = 1;
            }
        }
        else
        {
            if ((inet_pton(AF_INET, receiver_ip, &p_rxtCxt[app_id]->params.rx_params.mcIpaddr)) != 0)
            {
                QCLI_Printf(qcli_net_handle, "Incorrect IPv4 address %s\n", receiver_ip);
                e = -1;
                goto end;
            }
            if(QAPI_IPV4_IS_MULTICAST(htonl(p_rxtCxt[app_id]->params.rx_params.mcIpaddr))){
                p_rxtCxt[app_id]->params.rx_params.mcEnabled = 1;
            }

        }
    }
	/*+RAW IPv4*/
    if(Parameter_Count > 4)
    {
        if(strcasecmp("raw", protocol) != 0)
        {
            QCLI_Printf(qcli_net_handle, "exclude_list_size, test_mode, server_address and header_include is only for RAW socket.\n");        
            e = -1;
            goto end;
        }
        /* Number address in exlude list */

        if((p_rxtCxt[app_id]->params.rx_params.exclude_list_size = Parameter_List[4].Integer_Value) > 1000)
        {
            QCLI_Printf(qcli_net_handle, "Invalid exclude_list_size, please enter 0 ~ 1000\n");       
            e = -1;
            goto end;
        }
    }
    if(Parameter_Count > 5)
    {
        /* Test mode */

        if((p_rxtCxt[app_id]->params.rx_params.test_mode = Parameter_List[5].Integer_Value) > 2)
        {
            QCLI_Printf(qcli_net_handle, "Invalid test_mode, please enter 0 for error check mode,1 single RAW socket case or 2 for RAW + UDP double socket case\n");        
            e = -1;
            goto end;
        }
        if(p_rxtCxt[app_id]->params.rx_params.test_mode == 2 && p_rxtCxt[app_id]->params.rx_params.port != 255)
        {
            QCLI_Printf(qcli_net_handle, "RAW socket test mode 2 only for socket protocol number is 255\n");        
            e = -1;
            goto end;
        }

    }
    if(Parameter_Count > 6)
    {
        receiver_ip = (char*)Parameter_List[6].String_Value;

        /*+RAW IPv6*/
		if(v6_enable_flag[app_id])
		{
            if ((inet_pton(AF_INET6, receiver_ip, p_rxtCxt[app_id]->params.rx_params.server_address6)) != 0)
            {
                QCLI_Printf(qcli_net_handle, "Incorrect server address %s\n", receiver_ip);
                e = -1;
                goto end;
            }
        }
	    /*-RAW IPv6*/
		else
        {
        	if ((inet_pton(AF_INET, receiver_ip, &p_rxtCxt[app_id]->params.rx_params.server_address)) != 0)
        	{
           	 	QCLI_Printf(qcli_net_handle, "Incorrect server address %s\n", receiver_ip);
            	e = -1;
            	goto end;
        	}
		}
    }
	if(Parameter_Count > 7)
	{	
	    if(Parameter_List[7].Integer_Is_Valid && (Parameter_List[7].Integer_Value == 1 || Parameter_List[7].Integer_Value == 0))
	    {
        	p_rxtCxt[app_id]->params.rx_params.hdrincl = Parameter_List[7].Integer_Value;
	    }
		else
		{
		    QCLI_Printf(qcli_net_handle, "Invalid header include select, please input 0 or 1\n");
            e = -1;
            goto end;
		}
	}
	else
	{
		p_rxtCxt[app_id]->params.rx_params.hdrincl = 1;
	}
	if(Parameter_Count > 8)
	{	
	    if(Parameter_List[8].Integer_Is_Valid && (Parameter_List[8].Integer_Value == 1 || Parameter_List[8].Integer_Value == 0))
	    {
        	p_rxtCxt[app_id]->params.rx_params.debug_print = Parameter_List[8].Integer_Value;
	    }
		else
		{
		    QCLI_Printf(qcli_net_handle, "Invalid header include select, please input 0 or 1\n");
            e = -1;
            goto end;
		}
	}
	else
	{
		p_rxtCxt[app_id]->params.rx_params.debug_print = 0;
	}

	/*-RAW IPv4*/

    tid_rx[app_id] = qurt_thread_get_id();


    if (strcasecmp("tcp", protocol) == 0)
    {
        p_rxtCxt[app_id]->test_type = TCP_RX;
        qca_tcp_rx(p_rxtCxt[app_id], app_id);
    }
    else if (strcasecmp("udp", protocol) == 0)
    {
        p_rxtCxt[app_id]->test_type = UDP_RX;
        qca_udp_rx(p_rxtCxt[app_id], app_id);
    }
	/*+RAW IPv4*/
    else if(strcasecmp("raw", protocol) == 0)
    {
       p_rxtCxt[app_id]->test_type = RAW_IP_RX;
       qca_raw_rx(p_rxtCxt[app_id], app_id);
    }
	/*-RAW IPv4*/

#if INCLUDE_SSLDEMO
    else if ((strcasecmp("ssl", protocol) == 0))
    {
        if (ssl_role[app_id][SSL_SERVER_INST])
        {
            QCLI_Printf(qcli_net_handle, "ERROR: busy.\n");
            e = -1;
            goto end;
        }
        if (ssl_inst[app_id][SSL_SERVER_INST].sslCtx == QAPI_NET_SSL_INVALID_HANDLE || ssl_inst[app_id][SSL_SERVER_INST].role != QAPI_NET_SSL_SERVER_E)
        {
            QCLI_Printf(qcli_net_handle, "ERROR: SSL server not stated (Use 'ssl start server' first).\n");
            e = -1;
            goto end;
        }
        p_rxtCxt[app_id]->test_type = SSL_RX;
        ssl_role[app_id][SSL_SERVER_INST] = QAPI_NET_SSL_SERVER_E;
		
         if((ssl_inst[app_id][SSL_CLIENT_INST].config_set == 1 ||  ssl_inst[app_id][SSL_SERVER_INST].config_set == 1 ) && ((ssl_inst[app_id][SSL_CLIENT_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_0) || (ssl_inst[app_id][SSL_CLIENT_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_2) 
			|| (ssl_inst[app_id][SSL_SERVER_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_2)  ||  (ssl_inst[app_id][SSL_SERVER_INST].config.protocol == QAPI_NET_SSL_PROTOCOL_DTLS_1_0)))        
        {
            if ((strcasecmp("ssl", protocol) == 0)) {
                qca_udp_rx(p_rxtCxt[app_id], app_id);
            }
            else {
               
            }
        }
        else
        {
            if ((strcasecmp("ssl", protocol) == 0)) {
                qca_tcp_rx(p_rxtCxt[app_id], app_id);
            }
        }
        ssl_role[app_id][SSL_SERVER_INST] = (qapi_Net_SSL_Role_t)0;
    }
#endif

    else
    {
        QCLI_Printf(qcli_net_handle, "Invalid protocol: %s\n", protocol);
    }

end:
    port_in_use[app_id] = 0;

    if (p_rxtCxt[app_id])
    {
        qapi_Net_Buf_Free(p_rxtCxt[app_id], QAPI_NETBUF_APP);
        p_rxtCxt[app_id] = NULL;
    }

    if (e)
    {
        result = QCLI_STATUS_ERROR_E;
        goto ret;
    }
    result = QCLI_STATUS_SUCCESS_E;
    goto ret;
   }
   else
   {
    QCLI_Printf(qcli_net_handle, "Benchrx operation already running. \n");
    result = QCLI_STATUS_ERROR_E;
    goto ret;
   }

ret:
  if (Parameter_List != NULL)
  {
    for (i = 0; i<Parameter_Count; i++)
    {
      if (Parameter_List[i].String_Value != NULL)
      {
        free(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
  }
  return result;
}

/************************************************************************
 * benchquit
 ************************************************************************/
QCLI_Command_Status_t benchquit(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List)
{
    char *mtd;
    int app_id;
    multi_app_cmd_parser_t *cmd_parser_temp = NULL;
    int i;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

    cmd_parser_temp = multi_app_cmd_parser_func(Parameter_Count, Parameter_List, false);
    if(cmd_parser_temp == NULL)
    {
      LOG_INFO("Unable to parse given command");
      return QCLI_STATUS_ERROR_E;
    }

    if (cmd_parser_temp->app_data1.app_id != 0)
    {
      app_id = cmd_parser_temp->app_data1.app_id - 1;

      if (cmd_parser_temp->app_data1.Parameter_Count < 1 || cmd_parser_temp->app_data1.Parameter_List == NULL)
      {
        result = QCLI_STATUS_USAGE_E;
        goto end;
      }
  
      mtd = (char*)cmd_parser_temp->app_data1.Parameter_List[0].String_Value;
  
      if(strcasecmp("rx",mtd)==0 && tid_rx[app_id] != 0)
      {
       bench_quit[app_id][BENCH_RX] = 1;
       tid_rx[app_id] = 0;
      }
  
      if(strcasecmp("tx",mtd)==0 && tid_tx[app_id] != 0)
      {
       bench_quit[app_id][BENCH_TX] = 1;
       tid_tx[app_id] = 0;
      }
    }

    if (cmd_parser_temp->app_data2.app_id != 0)
    {
      app_id = cmd_parser_temp->app_data2.app_id - 1;

      if (cmd_parser_temp->app_data2.Parameter_Count < 1 || cmd_parser_temp->app_data2.Parameter_List == NULL)
      {
        result = QCLI_STATUS_USAGE_E;
        goto end;
      }
  
      mtd = (char*)cmd_parser_temp->app_data2.Parameter_List[0].String_Value;
  
      if(strcasecmp("rx",mtd)==0 && tid_rx[app_id] != 0)
      {
       bench_quit[app_id][BENCH_RX] = 1;
       tid_rx[app_id] = 0;
      }
  
      if(strcasecmp("tx",mtd)==0 && tid_tx[app_id] != 0)
      {
       bench_quit[app_id][BENCH_TX] = 1;
       tid_tx[app_id] = 0;
      }
    }

end:
  if (cmd_parser_temp->app_data1.app_id != 0)
  {
    if (cmd_parser_temp->app_data1.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data1.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data1.Parameter_List[i].String_Value != NULL)
        {
          free(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data1.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  if (cmd_parser_temp->app_data2.app_id != 0)
  {
    if (cmd_parser_temp->app_data2.Parameter_List != NULL)
    {
      for (i = 0; i< cmd_parser_temp->app_data2.Parameter_Count; i++)
      {
        if (cmd_parser_temp->app_data2.Parameter_List[i].String_Value != NULL)
        {
          free(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  free(cmd_parser_temp);

  return result;
}

#define IP_TOS 3
#define TOS_VO      0xD0
#define TOS_VI      0xA0
#define TOS_BK      0x20
#define TOS_BE      0x00
