/*!
  @file
  bench.h
  
  @brief
  Header for performing all the networking related operations.
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


#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "qapi_status.h"
#include "qapi_ssl.h"
#include "bench.h"
#include "qapi_socket.h"

#include "qapi_netservices.h"

#include "qcli.h"


#define USE_SERVER_STATS
//#undef INCLUDE_SSLDEMO
extern QCLI_Group_Handle_t qcli_net_handle; /* Handle for Net Command Group. */

extern QCLI_Command_t net_cmd_list[];

uint8_t v6_enable_flag[NO_OF_APPS];
static uint32_t port_in_use[NO_OF_APPS];        //Used to prevent two streams from using same port
static THROUGHPUT_CXT *p_rxtCxt[NO_OF_APPS];
boolean benchrx_started[NO_OF_APPS] = {FALSE};
uint8_t bench_quit[NO_OF_APPS][2];

TX_THREAD* tid_rx[NO_OF_APPS] = {0};
TX_THREAD* tid_tx[NO_OF_APPS] = {0}; 

extern bool dam_thrd_running[NO_OF_APPS];
extern  sock_opt option[NO_OF_APPS];

extern TX_BYTE_POOL * byte_pool_qcli;
#ifndef timeval
struct timeval
{
   long tv_sec;     /* seconds */
   long tv_usec;    /* and microseconds */
};
#endif

#if INCLUDE_SSLDEMO
qapi_Net_SSL_Role_t ssl_role[NO_OF_APPS][MAX_SSL_INST];
SSL_INST ssl_inst[NO_OF_APPS][MAX_SSL_INST];
#endif /* INCLUDE_SSLDEMO */

#define BENCH_RX 0
#define BENCH_TX 1

bool enable_us_delay = 0;

extern bool enable_cat4;

int packet_factor = 34;
int udp_packet_factor = 30;

int udp_normal_delay = 70;


volatile int pkt_demux_factor = 10;

#if INCLUDE_SSLDEMO
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

int strcasecmp(const char * s1, const char * s2)
{

    unsigned char c1, c2;
    int diff;

        do
        {
            c1 = (unsigned char)(*s1++);
            c2 = (unsigned char)(*s2++);

            /* Convert UC to LC */
            if (('A' <= c1) && ('Z' >= c1))
            {
                   c1 = c1 - 'A' + 'a';
            }
            if (('A' <= c2) && ('Z' >= c2))
            {
                  c2 = c2 - 'A' + 'a';
            }
            diff = c1 - c2;
            if (0 != diff)
            {                
                return diff;
            }
            if ('\0' == c1)
            {
                break;
            }
        } while (!*s1);
		return 0;
}

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
    struct sockaddr *to;   
	struct sockaddr_in local_addr;
	struct sockaddr_in6 local_addr6;
	struct sockaddr * from;
    int pktnum=0;
    int tolen;
    char ip_str[48];
	char ip46_str[48];
    uint32_t packet_size = p_tCxt->params.tx_params.packet_size;
    uint32_t cur_packet_number;
    uint32_t buffer_offset;
    uint32_t is_test_done;
    struct linger linger_option;
    struct linger get_linger_option = {0};

#if INCLUDE_SSLDEMO
    int32_t result;
    SSL_INST *ssl = &ssl_inst[app_id][SSL_CLIENT_INST];
#endif

    int opt_val=0;
    int32_t error = 0;
    uint32_t i = 0, j = 0;
    uint32_t netbuf_id;
    int send_flag;
    int family;
    uint32_t bytes_to_send;
    int bytes_sent;
	int result1;
	const char * ipstring;

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
        memcpy(&foreign_addr6.sin_addr, p_tCxt->params.tx_params.v6addr, sizeof(foreign_addr6.sin_addr));;
        foreign_addr6.sin_port     = htons(p_tCxt->params.tx_params.port);
        foreign_addr6.sin_family   = family;
        foreign_addr6.sin_scope_id = p_tCxt->params.tx_params.scope_id;
        from = (struct sockaddr *)&local_addr6;
        to = (struct sockaddr *)&foreign_addr6;
        tolen = sizeof(foreign_addr6);
    }
    else
    {
        family = AF_INET; 
        inet_ntop(family, &p_tCxt->params.tx_params.ip_address, ip_str, sizeof(ip_str));

        memset(&foreign_addr, 0, sizeof(foreign_addr));
        foreign_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.ip_address;
        foreign_addr.sin_port           = htons(p_tCxt->params.tx_params.port);
        foreign_addr.sin_family         = family;
        from = (struct sockaddr *)&local_addr;
        to = (struct sockaddr *)&foreign_addr;
        tolen = sizeof(foreign_addr);
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
        QCLI_Printf(qcli_net_handle,"LINGER TIME = %d ",linger_option.l_linger);
        error = qapi_setsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_LINGER, &linger_option, sizeof(struct linger));
        if (error != QAPI_OK)
        {
           QCLI_Printf(qcli_net_handle,"setting SO_LINGER socket option failed with error[%d]\n", error);
           goto ERROR_1;
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
   

    /* Connect to the server.*/
    QCLI_Printf(qcli_net_handle, "Connecting\n");
    if (qapi_connect( p_tCxt->sock_peer, to, tolen) == (UINT)A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "Connection failed.\n");
        goto ERROR_2;
    }

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

     qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_LINGER, &get_linger_option , sizeof(get_linger_option));
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

   result1 = qapi_getsockname(p_tCxt->sock_peer, from , &tolen) ;
	
   if (!result1)
   {
      memset(ip46_str, 0, sizeof(ip46_str)); 
      if(family==AF_INET)
   	  {
       ipstring=inet_ntop(AF_INET,&(local_addr.sin_addr), ip46_str, sizeof(ip46_str));
	   if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Host : %s ",ipstring);
	   else 
	   QCLI_Printf(qcli_net_handle, "Invalid Host Ip Address\n ");	   	
      }
     
      if(family==AF_INET6)
   	  {
       ipstring = inet_ntop(AF_INET6,&(local_addr6.sin_addr), ip46_str, sizeof(ip46_str));
	   if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Host : %s\n",ipstring);
	   else 
	   QCLI_Printf(qcli_net_handle, "Invalid Host Ip Address\n ");	   
      }
  
   }

   result1 = qapi_getpeername(p_tCxt->sock_peer, to, &tolen);
   
   if(!result1)
   {
      memset(ip46_str, 0, sizeof(ip46_str));
	  if(family==AF_INET)
   	  {
        ipstring=inet_ntop(AF_INET,&(foreign_addr.sin_addr), ip46_str, sizeof(ip46_str));
		if(ipstring)
	   	QCLI_Printf(qcli_net_handle, "Connected to Peer : %s\n",ipstring);
		else
        QCLI_Printf(qcli_net_handle , "Invalid Peer Ip Address\n");
      }
   
      if(family==AF_INET6)
   	  {
       ipstring=inet_ntop(AF_INET6,&(foreign_addr6.sin_addr), ip46_str, sizeof(ip46_str));
	   if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Connected to Peer : %s\n",ipstring);
	   else
	   QCLI_Printf(qcli_net_handle , "Invalid Peer Ip Address\n");	   	
      }
  
   }
   

    if(option[app_id].benchtx_sleep)   /*duration in ms to make the benchtx sleep */
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

        QCLI_Printf(qcli_net_handle, " Add socket handle to SSL connection %d %d \n", ssl->ssl, p_tCxt->sock_peer);
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
              while (p_tCxt->buffer == NULL)
            {

                tx_byte_allocate(byte_pool_qcli, (VOID **) &p_tCxt->buffer, packet_size, TX_NO_WAIT);
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
            bytes_sent = qapi_send( p_tCxt->sock_peer, &p_tCxt->buffer[buffer_offset], bytes_to_send, send_flag);
        }

        ++j;
#ifdef TCP_TX_DEBUG
        QCLI_Printf(qcli_net_handle, "%d bytes_sent = %d\n", j, bytes_sent);
#endif

        app_get_time(&p_tCxt->last_time);
        if (++i >= 500)
        {
            QCLI_Printf(qcli_net_handle, ".");
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
                if (  !v6_enable_flag[app_id] && (bytes_sent >= 0) && (bytes_sent < bytes_to_send) ) {
                    // the buffer is actually freed inside qapi_send(), so we must not free it again.
                    p_tCxt->buffer = 0;
                }
                is_test_done = 1;
                break;
            }
            else {
                // severe push back, let the other processes run for 1 ms
                app_msec_delay(100);
            }
        }

        // Useful notes related to TCP send:
        //
        // In TCP send we can have the following qapi_send() return codes:
        // IPv4:
        //  return_code < 0: the buffer was not sent, qapi_send() did not free the buffer if qapi_errno() returns ENOBUFS.
        //  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
        //  return_code < packet_size: the buffer sent partially, qapi_send() freed the buffer
        // IPv6:
        //  return_code < 0: the buffer was not sent, qapi_send() did not free the buffer
        //  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
        // Please note that for TCP  send there are no partial RE-transmit.  

        /****Bandwidth control- add delay if user has specified it************/

  if (enable_cat4)
        pktnum ++;

        if (p_tCxt->params.tx_params.interval)
        {
          if(enable_cat4)
          {
              if((pktnum % packet_factor )== 0)
              {
                  app_msec_delay(p_tCxt->params.tx_params.interval);
              }
          }
          else
          {
            if(enable_us_delay == false)
          app_msec_delay(p_tCxt->params.tx_params.interval);
            else
            app_usec_delay(p_tCxt->params.tx_params.interval);
          }
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
       tx_byte_release(p_tCxt->buffer);
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
	struct sockaddr_in local_addr;
    struct sockaddr_in6 local_addr6;
    struct sockaddr *from;
     int pktnum=0;
     int  tolen;
    char ip_str [48];
	 char ip46_str [48];
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
	int result1;
	const char * ipstring;


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
        from = (struct sockaddr *)&local_addr6;
        to = (struct sockaddr *)&foreign_addr6;
        tolen = sizeof(foreign_addr6);
    }
    else
    {
        family = AF_INET;
        inet_ntop(family, &p_tCxt->params.tx_params.ip_address, ip_str, sizeof(ip_str));

        memset(&foreign_addr, 0, sizeof(foreign_addr));
        foreign_addr.sin_addr.s_addr    = p_tCxt->params.tx_params.ip_address;
        foreign_addr.sin_port           = htons(p_tCxt->params.tx_params.port);
        foreign_addr.sin_family         = family;
        from = (struct sockaddr *)&local_addr;
        to = (struct sockaddr *)&foreign_addr;
        tolen = sizeof(foreign_addr);
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

    /* Connect to the server.*/
    QCLI_Printf(qcli_net_handle, "Connecting\n");
    if (qapi_connect( p_tCxt->sock_peer, to, tolen) == (UINT)A_ERROR)
    {
        QCLI_Printf(qcli_net_handle, "Conection failed\n");
        goto ERROR_2;
    }

   
   result1 = qapi_getsockname(p_tCxt->sock_peer, from , &tolen) ;
	
   if (!result1)
   {
     memset(ip46_str, 0, sizeof(ip46_str));
     if(family==AF_INET)
   	 { 
       ipstring=inet_ntop(AF_INET,&(local_addr.sin_addr), ip46_str, sizeof(ip46_str));
	   if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Host : %s ",ipstring);
	   else
	   QCLI_Printf(qcli_net_handle, "Invalid Host Ip Address\n");	   

	 }	 
   
     if(family==AF_INET6)
   	 {
       ipstring=inet_ntop(AF_INET6,&(local_addr6.sin_addr), ip46_str, sizeof(ip46_str));
	   if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Host : %s\n",ipstring);
	   else
	   QCLI_Printf(qcli_net_handle, "Invalid Host Ip Address\n");	
     }
  
   }

   result1 = qapi_getpeername(p_tCxt->sock_peer, to, &tolen);
   
   if(!result1)
   {
      memset(ip46_str, 0, sizeof(ip46_str));
	  if(family==AF_INET)
   	  { 
       ipstring=inet_ntop(AF_INET,&(foreign_addr.sin_addr), ip46_str, sizeof(ip46_str));
	   if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Connected to Peer : %s\n",ipstring);
	   else
	   QCLI_Printf(qcli_net_handle, "Invalid Peer Ip Address\n");	   
      }
   
      if(family==AF_INET6)
   	  {
       ipstring=inet_ntop(AF_INET6,&(foreign_addr6.sin_addr), ip46_str, sizeof(ip46_str));	   
       if(ipstring)
	   QCLI_Printf(qcli_net_handle, "Connected to Peer : %s\n",ipstring);
	   else
	   QCLI_Printf(qcli_net_handle, "Invalid Peer Ip Address\n");	   
	  }
  
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
                /** The peer's SSL certificate is NoT trusted,  */
                QCLI_Printf(qcli_net_handle, "ERROR: The peer's SSL certificate is NOT trusted \n");
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
            while (p_tCxt->buffer  == NULL)
            {
                tx_byte_allocate(byte_pool_qcli, (VOID **) &p_tCxt->buffer, packet_size, TX_NO_WAIT);
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
                        (EBADF == errno) )
                {
                    // the socket has closed - no point of continuing the test
                    QCLI_Printf(qcli_net_handle, "Socket got closed, errno=%d\n", errno);
                    if (  !v6_enable_flag[app_id] && (send_bytes >= 0) && (send_bytes < packet_size) ) {
                        // the buffer is actually freed inside qapi_send(), so we must not free it again.
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
                QCLI_Printf(qcli_net_handle, ".");
                i = 0;
            }

            /* Useful notes related to UDP send:
             *
             * IPv4:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             *  return_code < packet_size: the buffer sent partially, qapi_send() freed the buffer
             * IPv6:
             *  return_code < 0: the buffer was not sent, qapi_send() freed the buffer
             *  return_code == packet_size: the buffer was sent successfully, qapi_send() freed the buffer
             */

    #ifdef UDP_TX_DEBUG
	#if 0
            QCLI_Printf(qcli_net_handle, "%d send_bytes = %d\n", cur_packet_number, send_bytes);
	#endif
    #endif

    if (enable_cat4)
    {
       pktnum ++;
    }
       /****Bandwidth control***********/

    if(enable_cat4)
    {
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
    }
    else
    {
	            /****Bandwidth control***********/
         if(p_tCxt->params.tx_params.interval)
        {
           if(p_tCxt->test_type == SSL_TX)
           {
             if(enable_us_delay == false)
             app_msec_delay(p_tCxt->params.tx_params.interval);
             else
             app_usec_delay(p_tCxt->params.tx_params.interval);
           }
           else if(cur_packet_number % p_tCxt->params.tx_params.interval == 0)
           {
             if(enable_us_delay == false)
             app_msec_delay(udp_normal_delay);
             else
             app_usec_delay(udp_normal_delay*1000);
           }
           else if(p_tCxt->test_type == UDP_TX)
           {
             if(enable_us_delay == false)
             app_msec_delay(p_tCxt->params.tx_params.interval);
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
        while ( !((is_test_done) || (send_bytes == packet_size) || (NULL == p_tCxt->buffer)) );
    } /* send loop */

    if ( p_tCxt->buffer )
    {
        tx_byte_release(p_tCxt->buffer);
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
    char ip_str[48];
    time_struct_t ts;
	int error = QAPI_ERROR;
	int opt_val = 0;

#if INCLUDE_SSLDEMO
    SSL_INST *ssl = &ssl_inst[app_id][SSL_SERVER_INST];
#endif


#ifdef TCP_RX_RETRY_AFTER_FIN
    uint32_t retry=30;
#endif
    int family;

    tx_byte_allocate(byte_pool_qcli, (VOID **) &p_tCxt->buffer, CFG_PACKET_SIZE_MAX_RX, TX_NO_WAIT);
    /*Allocate buffer*/
    if (p_tCxt->buffer == NULL)
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
    if (qapi_bind(p_tCxt->sock_local, addr, addrlen) == (UINT)A_ERROR)
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

	qapi_getsockopt(p_tCxt->sock_peer, SOL_SOCKET, SO_RCVBUF, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_RCVBUF =%d\n",app_id, opt_val);

	qapi_getsockopt(p_tCxt->sock_local, SOL_SOCKET, SO_WINSCALE, &opt_val, sizeof(int));
    QCLI_Printf(qcli_net_handle," getsockopt:app_id : %d\t SO_WINSCALE =%d\n",app_id, opt_val);

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
    if (qapi_listen( p_tCxt->sock_local, 1) == (UINT)A_ERROR)
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

        
        if(family == AF_INET)
        {
            QCLI_Printf(qcli_net_handle, "Accepted conn from %s:%d\n",
                 inet_ntop(family, (void *)&foreign_addr.sin_addr, ip_str, sizeof(ip_str)),
                 ntohs(from->sa_port));
        }
        else if(family == AF_INET6)
        {
            QCLI_Printf(qcli_net_handle, "Accepted conn from %s:%d\n",
                inet_ntop(family, (void *)&foreign_addr6.sin_addr, ip_str, sizeof(ip_str)),
                ntohs(from->sa_port));
        }
          
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
        tx_byte_release(p_tCxt->buffer);

    return;
}

int32_t g_cookie_uc = 1234;
int32_t g_cookie_mc = 4321;

#define UINT32MAX   (0xffffffff)
#ifndef min
#define  min(a,b)    (((a) <= (b)) ? (a) : (b))
#endif
#define RATIO_BASE    (10000)

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
    char ip_str[48];
    int family;
    int32_t i = 0;
    int32_t is_first = 1;
    stat_udp_pattern_t stat_udp;
	struct timeval timeout;

#if INCLUDE_SSLDEMO
    SSL_INST *ssl = &ssl_inst[app_id][SSL_SERVER_INST];
#endif
	uint32_t  from_ip_address;
     ssl->ssl = 0;

    tx_byte_allocate(byte_pool_qcli, (VOID **) &p_tCxt->buffer, CFG_PACKET_SIZE_MAX_RX, TX_NO_WAIT);
    
    if(p_tCxt->buffer == NULL)
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

     if (qapi_setsockopt (p_tCxt->sock_local, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == (UINT)A_ERROR) 
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
	    if(family == AF_INET)
	    {
        	QCLI_Printf(qcli_net_handle, "Received %llu bytes from %s:%d\n",
                p_tCxt->bytes, inet_ntop(family, (void *)&foreign_addr.sin_addr, ip_str, sizeof(ip_str)),
                ntohs(from->sa_port));
	    }
		else if(family == AF_INET6)
		{
			QCLI_Printf(qcli_net_handle, "Received %llu bytes from %s:%d\n",
                p_tCxt->bytes, inet_ntop(family, (void *)&foreign_addr6.sin_addr, ip_str, sizeof(ip_str)),
                ntohs(from->sa_port));
		}
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
     tx_byte_release(p_tCxt->buffer);

}


/************************************************************************
 *         [0]           [1]  [2]   [3] [4]  [5] [6]
 * benchtx 192.168.1.100 2390 tcp   500  0   30   0
 *         [0]                             [1]  [2] [3] [4] [5]   [6] [7]
 * benchtx FE80::865D:D7FF:FE40:3498%wlan1 2390 udp 1400 1  10000 10  v6
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
  cmd_parser_temp->Command_Index = 14;

  result = multi_app_cmd_fill_func(cmd_parser_temp);
  
  return result;
}


QCLI_Command_Status_t proc_benchtx(uint32_t Parameter_Count, QCLI_Parameter_t *Parameter_List, int app_id)
{
    THROUGHPUT_CXT tCxt;
    int32_t retval;
    int32_t max_packet_size;
    char *receiver_ip, *protocol;
    int i;
    QCLI_Command_Status_t result = QCLI_STATUS_SUCCESS_E;

   if(!tid_tx[app_id])
   {
    if (Parameter_Count < 7 || Parameter_Count > 8 || Parameter_List == NULL)
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

    receiver_ip = (char*)Parameter_List[0].String_Value;

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
    
    if (port_in_use[app_id] == tCxt.params.tx_params.port)
    {
        QCLI_Printf(qcli_net_handle, "%d is in use; use another port\n", port_in_use[app_id]);
    }

    port_in_use[app_id] = tCxt.params.tx_params.port;
    bench_quit[app_id][BENCH_TX] = 0;
    tid_tx[app_id] = tx_thread_identify();
	
    protocol = (char*)Parameter_List[2].String_Value;
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

    if (p_rxtCxt[app_id])
     {
         tx_byte_release(p_rxtCxt[app_id]);
         p_rxtCxt[app_id] = NULL;
     }
    result = QCLI_STATUS_SUCCESS_E;
    goto end;

error:
    port_in_use[app_id] = 0;
    if (p_rxtCxt[app_id])
    {
        tx_byte_release(p_rxtCxt[app_id]);
        p_rxtCxt[app_id] = NULL;
    }
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
        tx_byte_release(Parameter_List[i].String_Value);
        Parameter_List[i].String_Value = NULL;
      }
    }
   }
  return result;
}

/************************************************************************
 *          [0]                            [1]     [2]     [3]
 * benchrx [tcp|udp|ssl] <port> [v4|v6] [<multicast IP>]
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
  cmd_parser_temp->Command_Index = 13;

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
    
    tid_rx[app_id] = tx_thread_identify();
    if (Parameter_Count < 2 || Parameter_Count > 4 || Parameter_List == NULL)
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

    /*Check port validity*/
    if (port_in_use[app_id] == Parameter_List[1].Integer_Value)
    {
        QCLI_Printf(qcli_net_handle, "%d is in use; use another port.\n", port_in_use[app_id]);
        result = QCLI_STATUS_ERROR_E;
        goto ret;
    }

    if (benchrx_started[app_id] == FALSE)
    {
      /* Allocate Rx context */
      tx_byte_allocate(byte_pool_qcli, (VOID **) &p_rxtCxt[app_id], sizeof(THROUGHPUT_CXT), TX_NO_WAIT);
      if (p_rxtCxt[app_id] == NULL)
      {
        QCLI_Printf(qcli_net_handle, "Memory alloc failure\n");
        result = QCLI_STATUS_ERROR_E;
        goto ret;
      }
      memset(p_rxtCxt[app_id], 0, sizeof(THROUGHPUT_CXT));
      p_rxtCxt[app_id]->params.rx_params.port = port_in_use[app_id] = Parameter_List[1].Integer_Value;
      bench_quit[app_id][BENCH_RX] = 0;

      benchrx_started[app_id] = TRUE;
    }
    else
    {
      QCLI_Printf(qcli_net_handle, "Please Press Ctrl+C to terminate the current RX test.\n");
        result = QCLI_STATUS_ERROR_E;
        goto ret;
    }

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
    protocol = (char*)Parameter_List[0].String_Value;
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
        tx_byte_release(p_rxtCxt[app_id]);
        p_rxtCxt[app_id] = NULL;
    }

    if (e)
    {
        for (i = 0; i<Parameter_Count; i++)
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
        tx_byte_release(Parameter_List[i].String_Value);
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
  
      if(strcasecmp("rx",mtd)==0 && (tid_rx[app_id] != NULL))
      {
        bench_quit[app_id][BENCH_RX] = 1; 
       // tx_thread_sleep(100);
       // tx_thread_delete(tid_rx[app_id]);
       tid_rx[app_id] = 0;
       benchrx_started[app_id] = FALSE;
      }
  
      if(strcasecmp("tx",mtd)==0 && (tid_tx[app_id] != NULL))
      {
       bench_quit[app_id][BENCH_TX] = 1;
       // tx_thread_sleep(100);
       // tx_thread_delete(tid_tx);
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
   
       if(strcasecmp("rx",mtd)==0 && (tid_rx[app_id] != NULL))
       {
         bench_quit[app_id][BENCH_RX] = 1; 
        // tx_thread_sleep(100);
        // tx_thread_delete(tid_rx[app_id]);
        tid_rx[app_id] = 0;
        benchrx_started[app_id] = FALSE;
       }
   
       if(strcasecmp("tx",mtd)==0 && (tid_tx[app_id] != NULL))
       {
        bench_quit[app_id][BENCH_TX] = 1;
        // tx_thread_sleep(100);
        // tx_thread_delete(tid_tx);
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
          tx_byte_release(cmd_parser_temp->app_data1.Parameter_List[i].String_Value);
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
          tx_byte_release(cmd_parser_temp->app_data2.Parameter_List[i].String_Value);
          cmd_parser_temp->app_data2.Parameter_List[i].String_Value = NULL;
        }
      }
    }
  }

  tx_byte_release(cmd_parser_temp);

  return result;
}

#define IP_TOS 3
#define	TOS_VO	    0xD0
#define	TOS_VI	    0xA0
#define	TOS_BK	    0x20
#define	TOS_BE	    0x00
