/*!
  @file
  qcli.h

  @brief
  QCLI Processing functionality
*/
/*===========================================================================
  Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
 
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
 ======================================================================*/


/*
 * Copyright (c) 2015 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 */

#ifndef __QCLI_H__   // [
#define __QCLI_H__

/*-------------------------------------------------------------------------
 * Include Files
 *-----------------------------------------------------------------------*/

#include "qapi_types.h"
#include "qcli_api.h"
#include "qapi_socket.h"
#include "qurt_mutex.h"
#include "qapi_cli.h"

#include "qurt_mutex.h"
#include "qurt_error.h"
#include "qurt_mutex.h"
#include "qurt_signal.h"
#include "qurt_thread.h"
#include "qurt_types.h"


/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 *-----------------------------------------------------------------------*/
	/* protocol values for the pip->ip_prot field */
#define  UDP_PROT    17  /* UDP Internet protocol number */
#define  TCP_PROT    6   /* TCP Internet protocol number */
#define  ICMP_PROT   1   /* TCP Internet protocol number */
#define  MAXARPSQCLI 8          /* maximum mumber of arp table entries */

#define QAPI_NET_DNS_MAXDNSSERVERS   4
#define QAPI_NET_DNS_MAXDNSADDRS     5
#define QAPI_NET_DNS_MAXDNSNAMEBUF   256
#define QAPI_NET_DNS_MAXDNSNAME      256
#define QAPI_NET_DNS_MAXDNSALIAS     2
#define QAPI_NET_INET6_ADDRSTRLEN    46
#define QAPI_NET_DNS_TYPE_IPADDR     1     /* Type value for IPv4 address */
#define QAPI_NET_DNS_TYPE_AAAA     0x1c  /* IPv6 address ("AAAA" record) */
#define QAPI_NET_DNS_NUM_TXTPTRS   7




#define htons(s) ((((s) >> 8) & 0xff) | \
                 (((s) << 8) & 0xff00))

/* define the "endian-ness" of the target processor */

#ifndef BYTE_ORDER   /* may inherit from ipport.h */
#define BYTE_ORDER   LITTLE_ENDIAN  /* Intel x86 */
#endif

/* Macro for passing IP addresses to printf octet at a time.
usage: printf("IP is %u.%u.%u.%u\n", PUSH_IPADDR(ip));
Since we store IP addresses in net endian, it's endian sensitive. 
*/

#if (BYTE_ORDER==LITTLE_ENDIAN)
#define  PUSH_IPADDR(ip)\
   (unsigned)(ip&0xff),\
   (unsigned)((ip>>8)&0xff),\
   (unsigned)((ip>>16)&0xff),\
   (unsigned)(ip>>24)
#else
#define  PUSH_IPADDR(ip)\
   (unsigned)(ip>>24),\
   (unsigned)((ip>>16)&0xff),\
   (unsigned)((ip>>8)&0xff),\
   (unsigned)(ip&0xff)
#endif




/*-------------------------------------------------------------------------
 * Type Declarations
 *-----------------------------------------------------------------------*/

/**
   This definition determines the size of the input buffer for CLI
   commansd. It effectively controls the maximum length of a command and
   its parameters.
*/
#define MAXIMUM_QCLI_COMMAND_STRING_LENGTH                              (256)

/**
   This definition determines the maximum number of parameters that can be
   provided. Note that this may also include paramters used to navigete
   into groups.
*/
#define MAXIMUM_NUMBER_OF_PARAMETERS                                    (20)

/**
   This definition determines the size of the buffer used for formatted
   messages to the console when using QCLI_Printf.
*/
#define MAXIMUM_PRINTF_LENGTH                                           (512)

/**
   This definition determines the maximum number of command threads that
   the QCLI allows to be running at a time.
*/
#define MAXIMUM_THREAD_COUNT                                            (5)

/**
   This definition determines the size of the stack (in bytes) that is used
   for command threads.
*/
#define THREAD_STACK_SIZE                                               (8096)

/**
   This definition indicates if received characters should be echoed to
   the console.
*/
#define ECHO_CHARACTERS                                                 (true)

/**
   This defintion determines the index that is used for the first command
   in a command list.  Typically this will be eiher 0 or 1.
*/
#define COMMAND_START_INDEX                                             1


/*-------------------------------------------------------------------------
 * Static & global Variable Declarations
 *-----------------------------------------------------------------------*/

extern qbool_t        Start_Thread_Check;

#define MAIN_PRINTF_HANDLE                                              ((QCLI_Group_Handle_t)&(QCLI_Context.Root_Group))


#define TAKE_LOCK(__lock__)                                             ((qurt_mutex_lock_timed_ext(&(__lock__), QURT_TIME_WAIT_FOREVER)) == QURT_EOK)
#define RELEASE_LOCK(__lock__)                                          do { qurt_mutex_unlock(&(__lock__)); } while(0)

/**
   This structure contains the context information for the QCLI module.
*/

/**
   This structure contains the information needed for starting a command
   thread.
*/
typedef struct Thread_Info_s
{
   qurt_signal_t         Thread_Ready_Event; /** Event which indicates the thread no longer needs this information structure. */
   uint32_t              Command_Index;      /** The index of the command that will be executed.                              */
   const QCLI_Command_t *Command;            /** The command that will be executed.                                           */
   uint32_t              Parameter_Count;    /** The number of parameters specified for the command.                          */
   QCLI_Parameter_t     *Parameter_List;     /** The list of paramters for the command.                                       */
} Thread_Info_t;

typedef struct Group_List_Entry_s
{
   const QCLI_Command_Group_t *Command_Group;         /** The command group information.                  */
   struct Group_List_Entry_s  *Parent_Group;          /** the parent group for this subgroup.             */
   struct Group_List_Entry_s  *Subgroup_List;         /** The list of subgroups registerd for this group. */
   struct Group_List_Entry_s  *Next_Group_List_Entry; /** The next entry in the list.                     */
} Group_List_Entry_t;

typedef struct Find_Result_s
{
   qbool_t Is_Group;                           /** A flag indicating if the result is a command or a group. */
   union
   {
      const QCLI_Command_t *Command;           /** The entry that was found if it is a command.             */
      Group_List_Entry_t   *Group_List_Entry;  /** The entry that was found if it is a group.               */
   } Data;
} Find_Result_t;

typedef struct QCLI_Context_s
{
   Group_List_Entry_t  Root_Group;                                            /** The root of the group menu structure.                                    */
   Group_List_Entry_t *Current_Group;                                         /** The current group.                                                       */

   uint32_t            Input_Length;                                          /** The length of the current console input string.                          */
   uint8_t             Input_String[MAXIMUM_QCLI_COMMAND_STRING_LENGTH + 1];  /** Buffer containing the current console input string.                      */
   QCLI_Parameter_t    Parameter_List[MAXIMUM_NUMBER_OF_PARAMETERS + 1];      /** List of parameters for input command.                                    */

   uint32_t            Thread_Count;                                          /** THe number of command threads that are currently running.                */
   Thread_Info_t       Thread_Info;                                           /** Information structure for passing information to command threads.        */
   qurt_mutex_t        CLI_Mutex;                                             /** The Mutex used to protect shared resources of the module.                */

   uint8_t             Printf_Buffer[MAXIMUM_PRINTF_LENGTH];                  /** The buffer used for formatted output strings.                            */
   QCLI_Group_Handle_t Current_Printf_Group;                                  /** The group handle that was last passed to QCLI_Printf().                  */
   qbool_t             Printf_New_Line;                                       /** Indicates that a newline should be displayed if a printf changes groups. */

} QCLI_Context_t;


/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

/**
   @brief This function is used to initialize the QCLI module.

   This function must be called before any other QCLI functions.

   @return
    - true if QCLI was initialized successfully.
    - false if initialization failed.
*/
qbool_t QCLI_Initialize(void);

/**
   @brief This function passes characters input from the command line to 
          the QCLI module for processing.

   @param Length is the number of bytes in the provided buffer.
   @param Buffer is a pointer to the buffer containing the inputted data.

   @return
    - true if QCLI was initialized successfully.
    - false if initialization failed.
*/
void QCLI_Process_Input_Data(uint32_t Length, uint8_t *Buffer);

/**
   @brief This function displays the current command list.

   It is intended to provide a means for the initial command list to be
   displayed once platform initialization is complete.
*/
void QCLI_Display_Command_List(void);

QCLI_Command_Status_t abort_handler(void);

/* The below Structures are used by QCLI to display 
output of us stat  command like tcpstat,udpstat and ipstat. 
*/
typedef unsigned long u_long;
struct tcpmibQCLI
{
   long     tcpRtoAlgorithm;
   long     tcpRtoMin;
   long     tcpRtoMax;
   long     tcpMaxConn;
   u_long   tcpActiveOpens;
   u_long   tcpPassiveOpens;
   u_long   tcpAttemptFails;
   u_long   tcpEstabResets;
   u_long   tcpCurrEstab;
   u_long   tcpInSegs;
   u_long   tcpOutSegs;
   u_long   tcpRetransSegs;
   void *   tcpConnTable;  /*32 bit ptr */
   u_long   tcpInErrs;
   u_long   tcpOutRsts;
};

struct   tcpstatQCLI
{
   u_long   tcps_connattempt;    /* connections initiated */
   u_long   tcps_accepts;        /* connections accepted */
   u_long   tcps_connects;       /* connections established */
   u_long   tcps_drops;          /* connections dropped */
   u_long   tcps_conndrops;      /* embryonic connections dropped */
   u_long   tcps_closed;         /* conn. closed (includes drops) */
   u_long   tcps_segstimed;      /* segs where we tried to get rtt */
   u_long   tcps_rttupdated;     /* times we succeeded */
   u_long   tcps_delack;         /* delayed acks sent */
   u_long   tcps_timeoutdrop;    /* conn. dropped in rxmt timeout */
   u_long   tcps_rexmttimeo;     /* retransmit timeouts */
   u_long   tcps_persisttimeo;   /* persist timeouts */
   u_long   tcps_keeptimeo;      /* keepalive timeouts */
   u_long   tcps_keepprobe;      /* keepalive probes sent */
   u_long   tcps_keepdrops;      /* connections dropped in keepalive */
   u_long   tcps_persistdrops;   /* connections dropped in persist */
   u_long   tcps_lingerdrops;    /* connections dropped in linger time */
   u_long   tcps_rasmlowbufdrops; /* Reassembly queue packets dropped:low buffers */
   u_long   tcps_rasmtmodrops;   /* Reassembly queue packets dropped: timeout */

   u_long   tcps_sndtotal;       /* total packets sent */
   u_long   tcps_sndpack;        /* data packets sent */
   u_long   tcps_sndbyte;        /* data bytes sent */
   u_long   tcps_sndrexmitpack;  /* data packets retransmitted */
   u_long   tcps_sndrexmitbyte;  /* data bytes retransmitted */
   u_long   tcps_sndacks;        /* ack-only packets sent */
   u_long   tcps_sndprobe;       /* window probes sent */
   u_long   tcps_sndurg;         /* packets sent with URG only */
   u_long   tcps_sndwinup;       /* window update-only packets sent */
   u_long   tcps_sndctrl;        /* control (SYN|FIN|RST) packets sent */
   u_long   tcps_snderrs;        /* errors encountered in egress path */

   u_long   tcps_rcvtotal;       /* total packets received */
   u_long   tcps_rcvpack;        /* packets received in sequence */
   u_long   tcps_rcvbyte;        /* bytes received in sequence */
   u_long   tcps_rcvbadsum;      /* packets received with ccksum errs */
   u_long   tcps_rcvbadoff;      /* packets received with bad offset */
   u_long   tcps_rcvshort;       /* packets received too short */
   u_long   tcps_rcvduppack;     /* duplicate-only packets received */
   u_long   tcps_rcvdupbyte;     /* duplicate-only bytes received */
   u_long   tcps_rcvpartduppack; /* packets with some duplicate data */
   u_long   tcps_rcvpartdupbyte; /* dup. bytes in part-dup. packets */
   u_long   tcps_rcvoopack;      /* out-of-order packets received */
   u_long   tcps_rcvoobyte;      /* out-of-order bytes received */
   u_long   tcps_rcvpackafterwin;   /* packets with data after window */
   u_long   tcps_rcvbyteafterwin;   /* bytes rcvd after window */
   u_long   tcps_rcvafterclose;  /* packets rcvd after "close" */
   u_long   tcps_rcvwinprobe;    /* rcvd window probe packets */
   u_long   tcps_rcvdupack;      /* rcvd duplicate acks */
   u_long   tcps_rcvacktoomuch;  /* rcvd acks for unsent data */
   u_long   tcps_rcvackpack;     /* rcvd ack packets */
   u_long   tcps_rcvackbyte;     /* bytes acked by rcvd acks */
   u_long   tcps_rcvwinupd;      /* rcvd window update packets */
   u_long   tcps_pullups;        /* # of successful TCP/IPv4 header pullups */
   u_long   tcps_pullupfail;     /* # of failed TCP/IPv4 header pullups */
   u_long   tcps_pullup_newbuf;  /* # of successful pullups that required */
                                 /* allocation of a new buffer */
   u_long   tcps_pullup_curbuf;  /* # of successful pullups that didn't */
                                 /* require allocation of a new buffer */

   u_long   tcps_mcopies;        /* m_copy() actual copies */
   u_long   tcps_mclones;        /* m_copy() clones */
   u_long   tcps_mcopiedbytes;   /* m_copy() # bytes copied */
   u_long   tcps_mclonedbytes;   /* m_copy() # bytes cloned */

   u_long   tcps_oprepends;      /* ip_output() prepends of header to data */
   u_long   tcps_oappends;       /* ip_output() appends of data to header */
   u_long   tcps_ocopies;        /* ip_output() copies */
   u_long   tcps_predack;        /* VJ predicted-header acks */
   u_long   tcps_preddat;        /* VJ predicted-header data packets */
   u_long   tcps_zioacks;        /* acks recvd during zio sends */
#ifdef TCP_SACK
   u_long   tcps_sackconn;       /* connections which negotiated SACK */
   u_long   tcps_sacksent;       /* SACK option headers sent */
   u_long   tcps_sackresend;     /* segs resent because of recv SACKs */
   u_long   tcps_sackrcvd;       /* SACK option headers received */
   u_long   tcps_sackmaxblk;     /* most SACK blocks in a received option field */
#endif /* TCP_SACK */
};

struct ipmibQCLI {
    long  ipForwarding;
    long  ipDefaultTTL;
    u_long ipInReceives;
    u_long ipInHdrErrors;
    u_long ipInAddrErrors;
    u_long ipForwDatagrams;
    u_long ipInUnknownProtos;
    u_long ipInDiscards;
    u_long ipInDelivers;
    u_long ipOutRequests;
    u_long ipOutDiscards;
    u_long ipOutNoRoutes;
    long   ipReasmTimeout;
    u_long ipReasmReqds;
    u_long ipReasmOKs;
    u_long ipReasmFails;
    u_long ipFragOKs;
    u_long ipFragFails;
    u_long ipFragCreates;
    u_long ipRoutingDiscards;
};

struct udpmibQCLI
{
   u_long   udpInDatagrams;   /* total delivered Datagrams */
   u_long   udpNoPorts;       /* undelivered datagrams: unused port */
   u_long   udpInErrors;      /* undelivered datagrams: other reasons */
   u_long   udpOutDatagrams;  /* successfully sent datagrams */
};

typedef unsigned short unshort;
typedef unsigned long ip_addr;

struct conn_statsQCLI {
   /* General connection statistics */
   u_long   pkts_i2l;   /* Incoming packet count - internet to local */
   u_long   pkts_l2i;   /* Outgoing packet count - local to internet */
   u_long   bytes_i2l;  /* Incoming byte count - internet to local */
   u_long   bytes_l2i;  /* Outgoing  byte count - local to internet */

   /* TCP connection statistics  */
   u_long   lretries;   /* Outging detected TCP retrys by local host */
   u_long   iretries;   /* Incoming detected TCP retrys by inet host */
   unshort  lbad_sums;  /* Outgoing number of times checksum failed */
   unshort  ibad_sums;  /* Incoming number of times checksum failed */
};

/* The basic NAT table entry structure.
NOTE: IP addresses and port numbers are in NET endian */
struct natTabEntQCLI
{
   ip_addr  f_ip;       /* foreign hosts (real internet) IP address */
   unshort  f_port;     /* foreign hosts port (hash on this) */

   ip_addr  out_ip;     /* "outside" (inserted) IP address */
   unshort  out_port;   /* "outside" (inserted) port for TCP or UDP */

   ip_addr  in_ip;      /* "inside" (local host's) IP address */
   unshort  in_port;    /* "inside" port for TCP or UDP */

#ifdef URL_SWITCH   
   ip_addr  inner_ip;   /* our IP address on inside iface */
   ip_addr  inner_port; /* our port on inside iface */
#endif   /* URL_SWITCH */


   /* the next series of variables only apply to TCP connections. Most
   of these are needed to support FTP port translation and URL switching.
   Names are from the received packet perspective.  */
   u_long   int_seq;    /* last TCP sequence number of foreign host */
   u_long   int_ack;    /* last TCP ack number of foriegn host */
   u_long   loc_seq;    /* last TCP sequence number of local host */
   u_long   loc_ack;    /* last TCP ack number of local host */
   struct conn_statsQCLI connstats;  /* General connection statistics */
   /* adjustments for seq and ack numbers */
   long     seq_offset;    /* amount to adjust TCP seq by */
   long     ack_offset;    /* amount to adjust TCP ack by */
   int      ftp_delta;     /* seq space change for a pending FTP PORT cmd */

   unshort  last_i2ldata;  /* TCP data size of last outbound packet */
   unshort  last_l2idata;  /* TCP data size of last incoming packet */
   unshort  tcp_state;     /* TCP connection state */
   struct natTabEntQCLI * next_port;
   struct natTabEntQCLI * next_master;

   int      ntype;      /* type of nat entry */
   u_long   alg_flags;  /* flags for application-level gateway's use */
   unsigned long   last_use;   /* last time we sent/rx a packet */
   unsigned char   prot;       /* protocol mapped (TCP, UDP, or ICMP) */
};

struct arptabentQCLI  {
   unsigned long t_pro_addr;    /* protocol address */
   unsigned char  t_phy_addr[6]; /* physical addr */
   void *net;        /* interface for which this entry is valid */
   void * pending;       /* packets waiting for resolution of this arp */
   u_long   age_expr;      /* When to expire arp entry and pending queue */
   u_long   refexpr;       /* End of recent-reference period */
   u_long   nextreq;       /* When to send next ARP request  */
   int      reqsent;       /* Number of ARP requests send for this entry */
   unshort  flags;         /* mask of the ET flags */
};


struct hostentQCLI
{
   char   *h_name;         /* Official name of host. */
   char  **h_aliases;      /* Alias list.  */
   int     h_addrtype;     /* Host address type.  */
   int     h_length;       /* Length of address.  */
   char  **h_addr_list;    /* List of addresses from name server.  */
   ip_addr  ipaddr_list[QAPI_NET_DNS_MAXDNSADDRS];  /* IP addresses (net endian) */
   char  ip6addr_list[QAPI_NET_DNS_MAXDNSADDRS][16];
   char   *h_z_name;       /* IN- zone name for UPDATE packet */
   struct sockaddr h_add_ipaddr;   /* IN- add this ip address for host name in zone */
   uint32_t  h_ttl;          /* IN- time-to-live field for UPDATE packet */
   uint16_t r_type;        /* ResourceRecord type -- "A" or "AAAA" or "ANY"*/
};

typedef struct
{
   int len;
   char *txt;
} TXTENTRYQCLI;

struct dns_querysQCLI
{
   struct dns_querysQCLI *next;   /* for use in queues */
   struct dns_querysQCLI *prev;
   uint32_t send_time;     /* ctick when last request was sent/received */
   uint32_t expire_time;   /* second (local) when this data expires */
   uint16_t tries;         /* retry count */
   uint16_t lport;         /* local (client) UDP port, for verification */
   uint16_t id;            /* ID of request, 0 == unused entry. */
   int      replies;       /* number of replys to current request */
   int      ipaddrs;       /* count of entries in ipaddr_list[] */
   ip_addr  ipaddr_list[QAPI_NET_DNS_MAXDNSADDRS];  /* IP addresses (net endian) */
   char    *addrptrs[QAPI_NET_DNS_MAXDNSADDRS]; /* pointers, for hostent.h_addr_list */
   int      err;                   /* last ENP_ error if, if any */
   int      rcode;                 /* last response code if replys > 1 */
   char     dns_names[QAPI_NET_DNS_MAXDNSNAMEBUF]; /* buffer of names (usually w/domain) */
   char     ptr_name[QAPI_NET_DNS_MAXDNSNAME];  /* The result of PTR query is stored here (just one for now) */
   struct sockaddr  auths_ip;              /* IPv4 addresses of 1st auth server */
   char    *alist[QAPI_NET_DNS_MAXDNSALIAS + 1]; /* alias list--not used */
   int      protocol;              /* from WKS */
   /* Most DNS queries need a hostent structure to return the data to
    * the calling application; so we embed the hostent structure inside
    * the query structure - one less alloced buffer to keep track of.
    */
   struct hostentQCLI he;               /* for return from gethostbyname() */
   char     type;                   /* type of original query */
   char     opcode;                 /* DNS_QUERY or DNS_UPDATE */
   TXTENTRYQCLI *h_TXT_list;      /* List of TXT RRs from name server.  */
   int    h_TXT_listCount; /* count of h_TXT_list entries */
   int      ip6addrs;                  /* count of entries in ip6_list */
   char     ip6_list[QAPI_NET_DNS_MAXDNSADDRS][16]; /* IPv6 address from answers */

};

/* DNSC states */
struct dnsc_state_tQCLI
{
    /* List for querie/database records. */
    struct dns_querysQCLI *dns_qs[QAPI_NET_DNS_MAXDNSSERVERS];
    int dnsc_updates;

    struct sockaddr *dns_servers_list[QAPI_NET_DNS_MAXDNSSERVERS];

    int numdnssvrs_per_iface[QAPI_NET_DNS_MAXDNSSERVERS];         /* Number of DNS servers currently configured for an interface */

    /* pending requests, can be used as a flag to spin dns_check() task */
    unsigned dnsc_active;

    TXTENTRYQCLI dnc_TXTptrs[QAPI_NET_DNS_NUM_TXTPTRS];  /* pointer array for TXT recovery */

#ifdef  DNS_CLIENT_UPDT
    char soa_mname[QAPI_NET_DNS_MAXDNSNAME];
    struct sockaddr dns_update_server;  /* IP addr of (authoratative) server for update request */
#endif  /* DNS_CLIENT_UPDT */
};


typedef struct cli_handle_info_s
{
   struct cli_handle_info_s *next;           /* Application handle list */  
   uint16_t handle_id;                       /* Validate the CLI registerd application client handle (internal) */
   qapi_CLI_Command_CB_t user_cb_fn;
   qapi_CLI_Command_Group_t *cmd_grp;
   qapi_CLI_App_Handler_t hndl;
   void *user_data;
} cli_handle_info_t;

typedef struct cli_ctx_s
{
   uint16_t handle_id_cntr;                 /* Generates and ID to the handle */   
   qurt_mutex_t cli_app_global_mutex;     /* Global CLI application synchronization mutex */  
   cli_handle_info_t *handles;             /* CLI registered application handles */
} cli_ctx_t;

extern cli_ctx_t app_cli_ctx;

qapi_Status_t cli_cmd_notification 
(
  qapi_CLI_App_Handler_t *handle,
  qapi_CLI_Parameter_Data_t *cmd_param
);


#endif // ] #ifndef __QCLI_H__

