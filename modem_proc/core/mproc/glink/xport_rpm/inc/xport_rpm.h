#ifndef XPORT_RPM_H
#define XPORT_RPM_H

/**
 * @file xport_rpm.h
 *
 * Internal definitions for RPM Glink transport
 */

/*==============================================================================
     Copyright (c) 2014 - 2016,2018 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.10/mproc/glink/xport_rpm/inc/xport_rpm.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/02/15   an      Added common function declarations
01/21/15   bc      Removed priority related macros
12/09/14   db      Added macros to set and get xport priority
10/20/14   an      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/* Version request command */
#define XPORT_RPM_CMD_VERSION_REQ       0x0
/* Version ACK command */
#define XPORT_RPM_CMD_VERSION_ACK       0x1
/* Open channel command */
#define XPORT_RPM_CMD_OPEN_CHANNEL      0x2
/* Close channel command */
#define XPORT_RPM_CMD_CLOSE_CHANNEL     0x3
/* Open channel ACK command */
#define XPORT_RPM_CMD_OPEN_CHANNEL_ACK  0x4
/* Tx data command */
#define XPORT_RPM_CMD_TX_DATA           0x9
/* Close channel ACK command */
#define XPORT_RPM_CMD_CLOSE_CHANNEL_ACK 0xB
/* Tx signals command */
#define XPORT_RPM_CMD_TX_SIGNALS        0xF


#define XPORT_RPM_GET_CMD_ID(c)      ((c) & 0xFFFF)
#define XPORT_RPM_SET_CMD_ID(cmd_id) ((cmd_id) & 0xFFFF)

#define XPORT_RPM_GET_CHANNEL_ID(c)   (((c) >> 16) & 0xFFFF)
#define XPORT_RPM_SET_CHANNEL_ID(cid) (((cid) << 16) & 0xFFFF0000)

#define XPORT_RPM_GET_VERSION(c) (((c) >> 16) & 0xFFFF)
#define XPORT_RPM_SET_VERSION(v) (((v) << 16) & 0xFFFF0000)

#define ROUNDUP64(d) (((d) + 7) & (~7))
#define ROUNDUP32(d) (((d) + 3) & (~3))

#define ALIGNED32(d) ((d&0x3) == 0)
#define ALIGNED64(d) ((d&0x7) == 0)
#define ALIGNED(x,d) (ALIGNED##x(d))

#define MSGRAM_READ32(pfifo, ind) (*(volatile uint32*)&(pfifo)->ptr[ind])
#define CHECK_INDEX_WRAP_AROUND(ind, size) \
          {if ((ind) >= (size)) ind = 0;}

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/
/* RPM FIFO descriptor */
typedef struct _xport_rpm_ind_type
{
  uint32 read_ind;
  uint32 write_ind;
} xport_rpm_ind_type;

/* RPM FIFO */
typedef struct _xport_rpm_fifo_type
{
  /* Pointer to FIFO descriptor */
  volatile xport_rpm_ind_type* desc;
  /* Pointer to FIFO start address */
  char* ptr;
  /* FIFO size */
  uint32 size;

} xport_rpm_fifo_type;

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION      xport_rpm_is_toc_present
===========================================================================*/
/**

  Checks RPM MSG RAM for ToC presence.

  @return     TRUE if ToC is detected, FALSE otherwise.

  @sideeffects  None.
*/
/*=========================================================================*/
boolean xport_rpm_is_toc_present(void);

/*===========================================================================
  FUNCTION      xport_rpm_get_fifo
===========================================================================*/
/**

  Looks through ToC to find matching FIFO and initalizes given FIFO structure.

  @param[in]  fifo_id   FIFO ID to find.
  @param[in]  pfifo     pointer FIFO structure to initialize.

  @return     TRUE if FIFO found, FALSE otherwise.

  @sideeffects  None.
*/
/*=========================================================================*/
boolean xport_rpm_get_fifo(uint32 fifo_id, xport_rpm_fifo_type* pfifo);

/*===========================================================================
  FUNCTION      xport_rpm_write_cmd
===========================================================================*/
/**

  Writes a single command into FIFO.

  @param[in]  pfifo             Pointer FIFO structure.
  @param[in]  cmd               Buffer containing the command
  @param[in]  cmd_size          Size of command buffer.
  @param[in]  data              Buffer containing the data
  @param[in]  data_size         Size of data buffer.

  @return     TRUE if command and data written into FIFO.

  @dependencies None.
*/
/*=========================================================================*/
boolean xport_rpm_write_cmd
(
  xport_rpm_fifo_type *pfifo,
  uint32              *cmd,
  uint32               cmd_size,
  uint32              *data,
  uint32               data_size
);

/*===========================================================================
  FUNCTION      xport_rpm_read_pkt_size
===========================================================================*/
/**

  Reads packet size from FIFO and updates read index to point to
  the beginning of packet data.

  @param[in]      pfifo         Pointer FIFO structure.
  @param[in/out]  pind          Pointer to FIFO read index

  @return     Packet size.

  @dependencies None.
*/
/*=========================================================================*/
uint32 xport_rpm_read_pkt_size
(
  xport_rpm_fifo_type *pfifo,
  uint32              *pind
);

/*===========================================================================
  FUNCTION      xport_rpm_copy_fifo
===========================================================================*/
/**

  Copies data between buffer and FIFO located at MSG RAM

  @param[in]  pfifo     Pointer FIFO structure.
  @param[in]  ind       Index to start copying from.
  @param[in]  buffer    Buffer.
  @param[in]  size      Size of the data in the buffer.
  @param[in]  dir       TRUE if writing into FIFO, FALSE otherwise.


  @return     New FIFO index.

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_rpm_copy_fifo
(
  xport_rpm_fifo_type *pfifo,
  uint32 ind,
  uint32 *buffer,
  uint32 size,
  boolean dir
);

/*===========================================================================
  FUNCTION      xport_rpm_update_fifo_read_ind
===========================================================================*/
/**

  Updates read index in the descriptor according to RPM transport requirements

  @param[in]  pfifo     Pointer FIFO structure.
  @param[in]  ind       New index value.


  @return     Updated index.

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_rpm_update_fifo_read_ind
(
  xport_rpm_fifo_type *pfifo,
  uint32 ind
);



/*===========================================================================
  FUNCTION      xport_rpm_update_fifo_write_ind
===========================================================================*/
/**

  Updates write index in the descriptor according to RPM transport requirements

  @param[in]  pfifo     Pointer FIFO structure.
  @param[in]  ind       New index value.


  @return     Updated index.

  @sideeffects  None.
*/
/*=========================================================================*/
uint32 xport_rpm_update_fifo_write_ind
(
  xport_rpm_fifo_type *pfifo,
  uint32 ind
);


#endif //XPORT_RPM_H

