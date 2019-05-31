/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*
 * Copyright (c) 2004-2006 Atheros Communications Inc.
 * All rights reserved.
 */
#ifndef  __SPITOOL_H_INCLUDED

#define  __SPITOOL_H_INCLUDED

#include <semaphore.h>

#define SPI_IOC_TRANSFERS 1

#define QZ_WAKE_GPIOn                "31"
#define QZ_CHIP_SELECT_GPIO          "45"

#define QZ_SPI_MODE                 SPI_MODE_3
#define QZ_SPI_BITS_PER_WORD        8
#define QZ_SPI_SPEED_HZ             1000000 // 1 MHz

#define	MAX_LINE_SIZE  256
/*
 *  SPI Command
 */
#define	SPI_CMD_REG         0x4000
#define	SPI_CMD_MAILBOX     0x0000
#define	SPI_CMD_READ        0x8000
#define	SPI_CMD_WRITE       0x0000

#define	SPI_CMD_REG_READ    (SPI_CMD_REG | SPI_CMD_READ)
#define	SPI_CMD_REG_WRITE   (SPI_CMD_REG | SPI_CMD_WRITE)

#define	SPI_CMD_MAILBOX_READ    (SPI_CMD_MAILBOX | SPI_CMD_READ)
#define	SPI_CMD_MAILBOX_WRITE   (SPI_CMD_MAILBOX | SPI_CMD_WRITE)

/*
 * SPI register address
 */
#define	SPI_REG_DMA_SIZE           0x100
#define	SPI_REG_WRBUF_SPC_AVAL     0x200
#define	SPI_REG_RDBUF_BYTE_AVAL    0x300
#define	SPI_REG_SPI_CONFIG         0x400
#define	SPI_REG_SPI_STATUS         0x500
#define	SPI_REG_HOST_CTRL_BYTE_SIZE       0x600
#define	SPI_REG_HOST_CTRL_CONFIG          0x700
#define	SPI_REG_HOST_CTRL_RD_PORT         0x800
#define	SPI_REG_HOST_CTRL_WR_PORT         0xA00

#define	SPI_REG_INTR_CAUSE         0xC00
#define	SPI_REG_INTR_ENABLE        0xD00
#define	SPI_REG_WRBUF_WRPTR        0xE00
#define	SPI_REG_WRBUF_RDPTR        0xF00
#define	SPI_REG_RDBUF_WRPTR        0x1000
#define	SPI_REG_RDBUF_RDPTR        0x1100

#define	SPI_REG_RDBUF_WATERMARK    0x1200
#define	SPI_REG_WRBUF_WATERMARK    0x1300
#define	SPI_REG_RDBUF_LOOKAHEAD1   0x1400
#define	SPI_REG_RDBUF_LOOKAHEAD2   0x1500

#define SPI_STATUS_HOST_ACCESS_DONE_MSK     0x01
#define SPI_STATUS_HOST_ACCESS_DONE_VAL     0x01

#define IsHostAceesDone(val)         (((val) & SPI_STATUS_HOST_ACCESS_DONE_MSK) == SPI_STATUS_HOST_ACCESS_DONE_VAL)

/*
 *    SPI_REG_INTR_ENABLE
 */
#define	INTR_ENABLE_PKT_AVAIL               0x01
#define INTR_ENABLE_ALL_CPU                 0x80
#define	INTR_ENABLE_CREDIT_COUNT            0x20
#define	INTR_ENABLE_LOCAL_CPU               0x10

#define	SPI_INTR_ENABLE                     (INTR_ENABLE_PKT_AVAIL | INTR_ENABLE_CREDIT_COUNT)
#define SPI_INTR_ENABLE_PKT_AVAIL           0x0001

#define	HOST_CTRL_BYTE_SIZE_ADDR_INC        0x0040

#define HOST_CTRL_BYTE_SIZE_INC_BMSK                0x40
#define HOST_CTRL_BYTE_SIZE_INC_SHFT                 0x6
#define HOST_CTRL_BYTE_SIZE_INC_SET(v,fld)       (((v) & ~HOST_CTRL_BYTE_SIZE_INC_BMSK) | \
        (((fld) << HOST_CTRL_BYTE_SIZE_INC_SHFT) & HOST_CTRL_BYTE_SIZE_INC_BMSK))
#define	ADDRESS_INC_YES     0
#define	ADDRESS_INC_NO      1

#define HOST_CTRL_BYTE_SIZE_BYTE_SIZE_BMSK                0x3F
#define HOST_CTRL_BYTE_SIZE_BYTE_SIZE_SHFT                 0x0
#define HOST_CTRL_BYTE_SIZE_BYTE_SIZE_SET(v,fld)       (((v) & ~HOST_CTRL_BYTE_SIZE_BYTE_SIZE_BMSK) | \
        (((fld) << HOST_CTRL_BYTE_SIZE_BYTE_SIZE_SHFT) & HOST_CTRL_BYTE_SIZE_BYTE_SIZE_BMSK))


#define HOST_CTRL_CONFIG_ENABLE_BMSK                0x8000
#define HOST_CTRL_CONFIG_ENABLE_SHFT                0xF
#define HOST_CTRL_CONFIG_ENABLE_SET(v,fld)       (((v) & ~HOST_CTRL_CONFIG_ENABLE_BMSK) | \
        (((fld) << HOST_CTRL_CONFIG_ENABLE_SHFT) & HOST_CTRL_CONFIG_ENABLE_BMSK))

#define HOST_CTRL_CONFIG_DIRECTION_BMSK             0x4000
#define HOST_CTRL_CONFIG_DIRECTION_SHFT             0xE
#define HOST_CTRL_CONFIG_DIRECTION_SET(v,fld)       (((v) & ~HOST_CTRL_CONFIG_DIRECTION_BMSK) | \
        (((fld) << HOST_CTRL_CONFIG_DIRECTION_SHFT) & HOST_CTRL_CONFIG_DIRECTION_BMSK))

#define HOST_CTRL_CONFIG_ADDRESS_BMSK             0x3FFF
#define HOST_CTRL_CONFIG_ADDRESS_SHFT             0x0
#define HOST_CTRL_CONFIG_ADDRESS_SET(v,fld)       (((v) & ~HOST_CTRL_CONFIG_ADDRESS_BMSK) | \
        (((fld) << HOST_CTRL_CONFIG_ADDRESS_SHFT) & HOST_CTRL_CONFIG_ADDRESS_BMSK))


/*
   SPI_REG_INTR_CAUSE
   */

#define  SPI_INTR_CAUSE_PKT_AVAIL                 0x0001
#define  SPI_INTR_CAUSE_COUNTER_INTR              0x0020

/*
 *  Host Registers
 */
#define	HOST_INT_STATUS             0x400
#define	CPU_INT_STATUS              0x401
#define	ERROR_INT_STATUS            0x402
#define	COUNTER_INT_STATUS          0x403

#define HOST_READ_OP_LOOP_TIMES     2

#define	MBOX_FRAME                  0x404
#define	RX_LOOKAHEAD_VALID          0x405
#define	HOST_INT_STATUS2            0x406
#define	GMBOX_RX_AVAIL              0x407

#define	RX_LOOKAHEAD0               0x408
#define	RX_LOOKAHEAD1               0x40C
#define	RX_LOOKAHEAD2               0x410
#define	RX_LOOKAHEAD3               0x414

#define	COUNTER_INT_STATUS_ENABLE   0x41B

#define	CREDIT_COUNT_SEND           0x420
#define	CREDIT_COUNT_RECV           0x424
#define	CREDIT_COUNT_DEC_SEND       0x440
#define	CREDIT_COUNT_DEC_RECV       0x450

#define	TARGET_CPU_INT_WLAN         0x472

#define QZ_SPI_DEVICE              "/dev/spidev0.0"

#define SPIP_MAX_PAYLOAD_FRAME_LENGTH   133
#define SPIP_BUFFER_SIZE                136

#define SPIP_FRAME_TERMINATOR           0xA7

#define INTER_COMMAND_SPACING_MS      1

#define     SPI_HOST_REG_ACCESS_DELAY       500

#define     MAX_DATA_SIZE       256
#define     MAX_BUFSZ           (HIF_MBOX_BLOCK_SIZE * 3)

#define     SPI_BLOCK_SIZE      32
#define     SPI_TEST_OK         3163

typedef union spi_reg_cmd_s {
    uint8_t     bytes[4];
    struct      cmd_val_s {
        uint16_t    cmd;
        uint16_t    val;
    } cmd_val;
} spi_reg_cmd_t;

typedef enum {
    DIR_INPUT,
    DIR_OUTPUT
} GpioDir;

typedef struct spi_ctxt_s {
    int  qzChipSelectFd;
    int  qzChipSelectActiveLow;
    int  qzIntFd;
    int  qzSpiFd;

    pthread_t thread;
    int  spi_interrupt_init_flag;

    pthread_t rcv_thread;
    int  rcv_thread_init_flag;
} spi_ctxt_t;

extern spi_ctxt_t    spi_ctxt;

typedef struct _HTC_FRAME_HDR {
    /* do not remove or re-arrange these fields, these are minimally required
     * to take advantage of 4-byte lookaheads in some hardware implementations */
    uint8_t   EndpointID;
    uint8_t   Flags;
    uint16_t  PayloadLen;       /* length of data (including trailer) that follows the header */

    /***** end of 4-byte lookahead ****/

    uint8_t   ControlBytes[2];
    /* message payload starts after the header */
} __attribute__((packed)) HTC_FRAME_HDR;

typedef union {
    struct  {
        uint16_t  head1;
        uint16_t  head2;
    } val16;
    struct  {
        uint8_t   epid;
        uint8_t   flags;
        uint16_t  payload_len;
    } attr;
} __attribute__((packed))  pkt_head_t;

typedef struct {
    uint16_t    mode;
    uint16_t    bytes_per_word;
    uint32_t    baudrate;
} spi_config_t;

/*
 *  Endian
 */

#define SWAP16(a) ((uint16_t)((((a)>>8) & 0xFF) | (((a)&0xFF)<<8)))
#define SWAP32(a) (SWAP16((((a)>>16))&0xFFFF) | (SWAP16((a)&0xFFFF) << 16))

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

#define CPU2SPI16(a) SWAP16(a)
#define SPI2CPU16(a) SWAP16(a)

#define SPI2CPU32(a) SWAP32(a)

#define LE2CPU16(a)   (a)
#define BE2CPU16(a)   SWAP16(a)
#define BE2CPU32(a)   SWAP32(a)

#else
#define	CPU2SPI16(a)  (a)
#define LE2CPU16(a)   SWAP16(a)
#define BE2CPU16(a)   (a)
#define BE2CPU32(a)   (a)
#endif

extern sem_t  pkt_avail_signal;
extern sem_t  pkt_avail_done_signal;

uint16_t spiSendWriteRegisterCmd(uint16_t reg_addr, uint16_t reg_val);
uint16_t spiSendReadRegisterCmd(uint16_t reg_addr, uint16_t *reg_val);

int  qzHostIntAsserted(void);
int32_t spiWriteFIFO(uint16_t addr, uint8_t *pdata, uint32_t len);
int write_mailbox_data(uint32_t pkt_count, uint32_t pktsz);

int read_mailbox_data(int mb);
int32_t spiWriteRegisterCmd(uint16_t reg_addr, uint16_t reg_val);
int32_t read_host_register(uint16_t reg_addr, uint16_t size);

void setup_spi_int();
int waitForHostInterruptWithMsTimeout(int milliseconds);
int receive_on_interrupt();
int32_t spi_loop_test(uint32_t pkt_count, uint32_t pktsz);
int32_t spi_loop_test_init();

void setup_receive_thread();
void core_cs_spi_test();

uint16_t ccs_spiReadRegisterCmd(uint16_t reg_addr, uint16_t *reg_val);
uint16_t ccs_spiWriteRegisterCmd(uint16_t reg_addr, uint16_t reg_val);

int32_t ccs_spiRead(uint16_t reg_addr, uint8_t *pdata, uint32_t len);
int32_t ccs_spiWrite(uint16_t reg_addr, uint8_t *pdata, uint32_t len);

int ccs_write_mailbox(uint16_t mbidx, uint32_t pkt_count, uint32_t pktsz);
int32_t ccs_spi_loop_test(uint16_t mb, uint32_t pkt_count, uint32_t pktsz);

#endif
