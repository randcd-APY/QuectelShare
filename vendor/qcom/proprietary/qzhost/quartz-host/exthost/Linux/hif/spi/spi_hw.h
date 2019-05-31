/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

#ifndef  __SPI_HW_H_INCLUDED

#define  __SPI_HW_H_INCLUDED

#define HIF_ACTIVE_MBOX_INDEX (0)

#define HIF_MBOX_BLOCK_SIZE                32
#define HIF_MBOX_BASE_ADDR                 0x800
#define HIF_MBOX_WIDTH                     0x800

#define ATH_TRANS_ADDR_MASK 0x7FFF  

#define HIF_MBOX_START_ADDR(mbox)  (HIF_MBOX_BASE_ADDR + mbox * HIF_MBOX_WIDTH)

typedef struct spi_read_data_s {
	uint8_t  space[2];
	union  {
		uint8_t   val8;
		uint16_t  val16;
		uint32_t  val32;
	} real_data;
} spi_read_data_t;

typedef union mb_status_s {
	uint32_t  mb_status_val;
	struct  {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		uint8_t  gmbox_rx_avail;
		uint8_t  host_int_status2;
		uint8_t  rx_lookahead_valid;
		uint8_t  mbox_fram;
#else
		uint8_t  mbox_fram;
		uint8_t  rx_lookahead_valid;
		uint8_t  host_int_status2;
		uint8_t  gmbox_rx_avail;
#endif
	} status_byte;
} mb_status_t;

typedef union mailbox_transfer_s {
	uint16_t  status;
	uint16_t  buf_size;
	uint8_t   *pbuf;
} mailbox_transfer_t;

typedef struct spi_hw_ctxt_s {
	mb_status_t  mb_status;
} spi_hw_ctxt_t;

int read_mailboxs(mailbox_transfer_t *pXfer, int size);		

#endif
