/******************************************************************************

Copyright (c) 2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#ifndef _PRODUCT_TEST_COMMAND_H_
#define _PRODUCT_TEST_COMMAND_H_

#include "connx_util.h"
#include "bt_hci_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif


#define HCI_VS_PROD_TEST_COMMAND                ((uint16_t) 0x04)
#define HCI_VS_NVM_COMMAND                      ((uint16_t) 0x0B)
#define HCI_VS_DEBUG_OPCODE                     ((uint16_t) 0x0C)

/* Product test commands defined in "80-WL400-14_D_Bluetooth_HCI_Vendor-Specific_Command_Reference". */
typedef uint8_t  QcaSubOpcodeT;
#define PROD_TEST_MIN_SUB_OPCODE                ((QcaSubOpcodeT) 0x00)

#define PROD_TEST_SLAVE                         ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x00))
#define PROD_TEST_MASTER                        ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x01))
#define PROD_TEST_STATS                         ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x02))
#define PROD_TEST_RESERVED                      ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x03))  /* Reserved test command. */
#define PROD_TEST_TX_BURST                      ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x04))
#define PROD_TEST_TX_CONTINUOUS                 ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x05))
#define PROD_TEST_RX_BURST                      ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x06))
#define PROD_TEST_RX_BURST_CONFIG               ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x07))

#define PROD_TEST_MAX_SUB_OPCODE                ((QcaSubOpcodeT) (PROD_TEST_MIN_SUB_OPCODE + 0x07))
#define PROD_TEST_COUNT                         (PROD_TEST_MAX_SUB_OPCODE - PROD_TEST_MIN_SUB_OPCODE + 1)

#define PROD_TEST_UNKNOWN                       ((QcaSubOpcodeT) 0xFF)

#define VALID_PROD_TEST_CMD(c)                  ((c) != PROD_TEST_UNKNOWN)

/* Five hopping channels used if limited hopping */
#define HOP_CHANNELS_COUNT                      5

/* Channel number */
#define MIN_HOP_CHANNEL                         0       /* 0x00 */
#define MAX_HOP_CHANNEL                         78      /* 0x4E */

/* Transmit output power */
#define TX_OUTPUT_POWER_LOWEST                  0x00
#define TX_OUTPUT_POWER_HIGHEST                 0x09

/* ------------------------------------------------------------------------------- */
/* Common definition for PROD_TEST_SLAVE / PROD_TEST_MASTER / PROD_TEST_TX_BURST / PROD_TEST_RX_BURST command */
/* ------------------------------------------------------------------------------- */

/* Transmitted pattern payload, independent of the master pattern (Byte 9) */
#define TX_PATTERN_PAYLOAD_ALL_ZEROS            0x00
#define TX_PATTERN_PAYLOAD_ALL_ONES             0x01
#define TX_PATTERN_PAYLOAD_ALTERNATING_BIT      0x02
#define TX_PATTERN_PAYLOAD_ALTERNATING_NIBBLE   0x03
#define TX_PATTERN_PAYLOAD_PSEUDO_RANDOM        0x04

/* Packet type (Byte 10) */
#define PACKET_TYPE_NULL                        0x00
#define PACKET_TYPE_POLL                        0x01
#define PACKET_TYPE_FHS                         0x02
#define PACKET_TYPE_DM1                         0x03
#define PACKET_TYPE_DH1                         0x04
#define PACKET_TYPE_DM3                         0x0A
#define PACKET_TYPE_DH3                         0x0B
#define PACKET_TYPE_DM5                         0x0E
#define PACKET_TYPE_DH5                         0x0F
#define PACKET_TYPE_AUX1                        0x09
#define PACKET_TYPE_2_DH1                       0x24
#define PACKET_TYPE_2_DH3                       0x2A
#define PACKET_TYPE_2_DH5                       0x2E
#define PACKET_TYPE_3_DH1                       0x28
#define PACKET_TYPE_3_DH3                       0x2B
#define PACKET_TYPE_3_DH5                       0x2F
#define PACKET_TYPE_HV1                         0x05
#define PACKET_TYPE_HV2                         0x06
#define PACKET_TYPE_HV3                         0x07
#define PACKET_TYPE_DV                          0x08
#define PACKET_TYPE_EV3                         0x17
#define PACKET_TYPE_EV4                         0x1C
#define PACKET_TYPE_EV5                         0x1D
#define PACKET_TYPE_2_EV3                       0x36
#define PACKET_TYPE_2_EV5                       0x3C
#define PACKET_TYPE_3_EV3                       0x37
#define PACKET_TYPE_3_EV5                       0x3D

/* Data whitening (Byte 11) */
#define DATA_WHITENING_OFF                      0
#define DATA_WHITENING_ON                       1

/* Receiver gain (Byte 13) */
#define RECEIVER_GAIN_LOW                       0x00
#define RECEIVER_GAIN_HIGH                      0x01

/* Hopping functionality (Byte 20) */
#define HOPPING_FUNC_ALL_FIVE_HOP_CHANNELS      0x00    /* Device hops over all five hop channels */
#define HOPPING_FUNC_SPECIFIED_HOP_CHANNEL      0x01    /* Device starts at specified hop channel (0), then hops over the full range of channels */

/* ------------------------------------------------------------------------------- */
/* Definition for PROD_TEST_TX_CONTINUOUS                                          */
/* ------------------------------------------------------------------------------- */

/* Transmit type */
#define TX_TYPE_CARRIER_ONLY                    0x04    /* Carrier (CW) only */
#define TX_TYPE_1_PRBS9                         0x05    /* 1-PRBS9 (GFSK) pseudo random binary sequence */
#define TX_TYPE_1_PRBS15                        0x06    /* 1-PRBS15 (GFSK) pseudo random binary sequence */
#define TX_TYPE_1_PATTERN                       0x07    /* 1-pattern (GFSK) as specified in the fields shown in the pattern length (offset 7) below */
#define TX_TYPE_2_PRBS9                         0x08    /* 2-PRBS9 (Pi/4-DQPSK) */
#define TX_TYPE_2_PRBS15                        0x09    /* 2-PRBS15 (Pi/4-DQPSK) */
#define TX_TYPE_2_PATTERN                       0x0A    /* 2-Pattern (Pi/4-DQPSK) */
#define TX_TYPE_3_PRBS9                         0x0B    /* 3-PRBS9 (8DPSK) */
#define TX_TYPE_3_PRBS15                        0x0C    /* 3-PRBS15 (8DPSK) */
#define TX_TYPE_3_PATTERN                       0x0D    /* 3-Pattern (8DPSK) */

/* Packet length */
#define MIN_REPEATED_PATTERN_LENGTH             0x01
#define MAX_REPEATED_PATTERN_LENGTH             0x20

/* ------------------------------------------------------------------------------- */
/* Definition for Debug command                                                    */
/* ------------------------------------------------------------------------------- */

#define READ_RSSI_CMD                           ((QcaSubOpcodeT) 0x0A)

#define UNKNOWN_DEBUG_CMD                       ((QcaSubOpcodeT) 0xFF)

#define VALID_DEBUG_CMD(c)                      ((c) != UNKNOWN_DEBUG_CMD)

/* ------------------------------------------------------------------------------- */
/* Definition for NVM command                                                      */
/* ------------------------------------------------------------------------------- */

#define NVM_CMD_READ                            0x00     /* Sub-opcode to get NVM tag value. */
#define NVM_CMD_WRITE                           0x01     /* Sub-opcode to set NVM tag value. */

#define UNKNOWN_NVM_TAG                         0xFF
#define VALID_NVM_TAG(t)                        ((t) != UNKNOWN_NVM_TAG)

#define MAX_NVM_VAL_LENGTH                      250


#pragma pack(push, 1)

/* ------------------------------------------------------------------------------- */
/* Product test command                                                            */
/* ------------------------------------------------------------------------------- */

typedef struct
{
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode for HCI_VS_PROD_TEST_COMMAND */
    uint8_t         hop_channels[HOP_CHANNELS_COUNT];   /* Hop channels */
    uint8_t         tx_pattern_payload;                 /* Transmitted pattern payload */
    uint8_t         packet_type;                        /* Packet type */
    uint8_t         data_whitening;                     /* Data whitening */
    uint8_t         tx_output_power;                    /* Transmit output power */
    uint8_t         receiver_gain;                      /* Receiver gain */
    uint8_t         target_addr[BT_ADDR_LENGTH];        /* Bluetooth target device address */
    uint8_t         hopping_func;                       /* Hopping functionality */
    uint16_t        payload_length;                     /* Payload length */
    uint8_t         logical_transport_addr;             /* Logical transport address */
} PROD_TEST_COMMON_CMD_T;

/* PROD_TEST_SLAVE command */
typedef PROD_TEST_COMMON_CMD_T      PROD_TEST_SLAVE_CMD_T;

/* PROD_TEST_MASTER command */
typedef PROD_TEST_COMMON_CMD_T      PROD_TEST_MASTER_CMD_T;

/* PROD_TEST_STATS command */
typedef struct
{
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode */
} PROD_TEST_STATS_CMD_T;

/* PROD_TEST_TX_BURST command */
typedef PROD_TEST_COMMON_CMD_T      PROD_TEST_TX_BURST_CMD_T;

/* PROD_TEST_TX_CONTINUOUS command */
typedef struct
{
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode */
    uint8_t         channel_number;                     /* Device transmit channel number */
    uint8_t         tx_output_power;                    /* Transmit output power */
    uint8_t         tx_type;                            /* Transmit type */
    uint8_t         packet_length;                      /* Specifies the length of the repeated pattern */
    uint32_t        bit_pattern;                        /* Bit pattern transmitted repeatedly */
} PROD_TEST_TX_CONTINUOUS_CMD_T;

/* PROD_TEST_RX_BURST command */
typedef PROD_TEST_COMMON_CMD_T      PROD_TEST_RX_BURST_CMD_T;

/* PROD_TEST_RX_BURST_CONFIG command */
typedef struct
{
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode */
    uint16_t        number_of_packets;                  /* 0x00 = Rx burst continually on */
} PROD_TEST_RX_BURST_CONFIG_CMD_T;

/* ------------------------------------------------------------------------------- */
/* Product test event                                                              */
/* ------------------------------------------------------------------------------- */

/* Channel statistics */
typedef struct
{
    uint32_t        packets_received;                   /* Number of packets received (no access code error) */
    uint32_t        packets_access_error_code;          /* Number of packet access code errors (not received) */
    uint32_t        hec_errors;                         /* Number of received packets with Header Error Check or Checksum (HEC) errors */
    uint32_t        crc_errors;                         /* Number of received packets with CRC errors */
    uint32_t        total_packet_bit_errors;            /* Packet BIT error count */
    uint32_t        first_half_bit_errors;              /* Bit errors found in first half of payload */
    uint32_t        last_half_bit_errors;               /* Bit errors found in last half of payload */
    uint32_t        rssi_reading;                       /* Average Receive Signal Strength Indication (RSSI) reading in 2s complement format */
} CHANNEL_STATISTICS_T;

/* PROD_TEST_STATS event */
typedef struct
{
    uint8_t         op_code;            /* PROD_TEST_COMMAND */
    uint16_t        received_packets;   /* Received packets */
    CHANNEL_STATISTICS_T    channel_stats[HOP_CHANNELS_COUNT];  /* channel statistics */
} PROD_TEST_STATS_EVT_T;


/* ------------------------------------------------------------------------------- */
/* Debug command
/* ------------------------------------------------------------------------------- */

/* Read RSSI command */
typedef struct
{
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode */
    uint8_t         channel_offset;                     /* Offset to the channel to open from 2.4 GHz, that is, 0x02 is 2.402 GHz, 0x04 is 2.404 GHz */
    uint8_t         number_of_samples;                  /* Used to average the returned value */
} READ_RSSI_CMD_T;

typedef struct
{
    HCI_EV_COMMAND_COMPLETE_T   command_complete;       /* Common command complete header */
    QcaSubOpcodeT               sub_opcode;             /* Sub-opcode */
    uint8_t                     rssi;                   /* RSSI in dBm */
} HCI_EV_READ_RSSI_T;


/* ------------------------------------------------------------------------------- */
/* NVM command
/* ------------------------------------------------------------------------------- */

/* Read NVM command */
typedef struct
{
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode to get NVM tag value */
    uint8_t         nvm_tag;                            /* NVM tag index number */
} READ_NVM_CMD_T;

typedef struct
{
    uint8_t         op_code;                            /* NVM command class */
    QcaSubOpcodeT   sub_opcode;                         /* Sub-opcode to get NVM tag value */
    uint8_t         nvm_tag;                            /* NVM tag index number */
    uint8_t         length;                             /* Length of parameter */
    uint8_t         data[MAX_NVM_VAL_LENGTH];           /* NVM data */
} READ_NVM_EVT_T;

#pragma pack(pop)


#ifdef __cplusplus
}
#endif

#endif  /* _PRODUCT_TEST_COMMAND_H_ */
