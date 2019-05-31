/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef DATAMANGER_H
#define DATAMANAGER_H


typedef enum _DataManager_Msg_Type{
    DM_INTERFACE_INIT_CMD,
    DM_INTERFACE_INIT_RESP,
    DM_INTERFACE_DESTROY_CMD,
    DM_INTERFACE_DESTROY_RESP,
    DM_INTERFACE_LINK_STATE_CMD,
    DM_INTERFACE_LINK_STATE_RESP,
    DM_INTERFACE_QUERY_PREFIX_CMD,
    DM_INTERFACE_QUERY_PREFIX_RESP,
    DM_INTERFACE_DATA,           /*Data(payload) packet*/ 
    DM_INTERFACE_ADD_IP,         /*Add IPv6 address to interface*/
    DM_INTERFACE_DEL_IP,         /*Delete IPv6 address from interface*/
    DM_INTERFACE_ADD_MCAST,      /*Subscribe to Multicast group*/
    DM_INTERFACE_DEL_MCAST,      /*Remove all mcast subscriptions*/
    DM_INTERFACE_ADD_ROUTE,      /*Add route to interface*/
    DM_INTERFACE_DEL_ROUTE,      /*Remove all routes from interface*/
    DM_INTERFACE_PREFIX_MATCH,
} DATAMGR_MSG_TYPE_T;

typedef enum _DataManager_Interface_Type{
    DM_TAP,   /*TAP interface (ETH header)*/
    DM_TUN,   /*TUN Interface (IP Header)*/
} DATAMGR_INT_TYPE_T;

typedef enum _DataManager_Response_Type{
    DM_RESP_OK,
    DM_RESP_ERR,
} DATAMGR_RESP_TYPE_T;

typedef enum _DM_FLAGS{
    DM_IF_DOWN,
    DM_IF_UP,
} DATAMGR_FLAGS_T;

#define IPV6_ADDR_IS_LINK_LOCAL(_addr)                ((((uint8_t *)(_addr))[0] == 0xfe) && ((((uint8_t *)(_addr))[1] & 0xC0) == 0x80))
#define IPV6_ADDR_IS_SITE_LOCAL(_addr)                ((((uint8_t *)(_addr))[0] == 0xfe) && ((((uint8_t *)(_addr))[1] & 0xC0) == 0xC0))
#define IPV6_ADDR_IS_MULTICAST(_addr)                 (((uint8_t *)(_addr))[0] == 0xff)

#define IPV6_ADDR_IS_LOCALHOST(_addr)                 ((((uint8_t *)(_addr))[0] == 0) && (((uint8_t *)(_addr))[1] == 0) && (((uint8_t *)(_addr))[2] == 0) && (((uint8_t *)(_addr))[3] == 0) && (((uint8_t *)(_addr))[4] == 0) && (((uint8_t *)(_addr))[5] == 0) && (((uint8_t *)(_addr))[6] == 0) && (((uint8_t *)(_addr))[7] == 0) && (((uint8_t *)(_addr))[8] == 0) && (((uint8_t *)(_addr))[9] == 0) && (((uint8_t *)(_addr))[10] == 0) && (((uint8_t *)(_addr))[11] == 0) && (((uint8_t *)(_addr))[12] == 0) && (((uint8_t *)(_addr))[13] == 0) && (((uint8_t *)(_addr))[14] == 0) && (((uint8_t *)(_addr))[15] == 1))

#define IPV6_ASSIGN_ADDRESS(_dest, _a, _b, _c, _d, _e, _f, _g, _h, _i, _j, _k, _l, _m, _n, _o, _p)                                 \
    do {                                                                                                                               \
        ((uint8_t *)(_dest))[0]  = (_a); ((uint8_t *)(_dest))[1]  = (_b); ((uint8_t *)(_dest))[2]  = (_c); ((uint8_t *)(_dest))[3]  = (_d); \
        ((uint8_t *)(_dest))[4]  = (_e); ((uint8_t *)(_dest))[5]  = (_f); ((uint8_t *)(_dest))[6]  = (_g); ((uint8_t *)(_dest))[7]  = (_h); \
        ((uint8_t *)(_dest))[8]  = (_i); ((uint8_t *)(_dest))[9]  = (_j); ((uint8_t *)(_dest))[10] = (_k); ((uint8_t *)(_dest))[11] = (_l); \
        ((uint8_t *)(_dest))[12] = (_m); ((uint8_t *)(_dest))[13] = (_n); ((uint8_t *)(_dest))[14] = (_o); ((uint8_t *)(_dest))[15] = (_p); \
    } while(0)

#define IPV6_ADDRESS_LENGTH                           16
#define IPV6_PREFIX_LENGTH                            8

typedef struct
{
    uint8_t  Address[IPV6_ADDRESS_LENGTH];
} IPv6_Address_Info_t;


typedef struct _DataManager_Prefix_Info
{
    uint32_t            PrefixLength;
    IPv6_Address_Info_t Prefix;
    uint32_t            ValidLifetime;
    uint32_t            PreferredLifetime;
    uint32_t            Flags;
} IPV6_PREFIX_INFO_T;

typedef struct
{
    uint8_t status;
    uint8_t numPrefix;
    IPV6_PREFIX_INFO_T *info;
} PREFIX_QUERY_RESP_T;


#define IPV6_PREFIX_INFO_SIZE   (sizeof(IPV6_PREFIX_INFO_T))
#define DM_MIN_PAYLOAD_SIZE    (2)
#define DM_HEADER_SIZE         (6)
#define DM_IFACE_ID_OFFSET     (2)
#define DM_RESP_SIZE          (1024)  /*Response size*/

#define GET_DM_CMD_TYPE(_x)   (*((char*)_x + IOTD_HEADER_LEN))
#define DM_CMD_TYPE_OFFSET    (IOTD_HEADER_LEN)

/******Interface Init Command *********/
#define DM_IFACE_INIT_TYPE_OFFSET      (6)
#define DM_IFACE_INIT_MAC_OFFSET       (7)
#define DM_IFACE_INIT_NAMELEN_OFFSET   (13)
#define DM_IFACE_INIT_NAME_OFFSET      (14)

/******Interface Init response*********/
#define DM_IFACE_RESP_SIZE    (7)
#define DM_IFACE_RESP_HDL_OFFSET  (DM_CMD_TYPE_OFFSET +2)


/***** Interface Destroy Command ******/
#define DM_IFACE_DESTROY_SIZE          (6)
#define DM_IFACE_DESTRY_HDL_OFFSET     (2)

/******Interface Destroy Response *****/
#define DM_IFACE_DESTROY_RESP_SIZE     (DM_HEADER_SIZE + 1)

/******Interface Set Link Flags ************/
#define DM_IFACE_FLAGS_OFFSET       (6)

/******Interface Link state response ******/
#define DM_IFACE_LINK_STATE_RESP_SIZE        (7)

/****** DATA *******/
#define DM_DATA_OFFSET                  (IOTD_HEADER_LEN + DM_HEADER_SIZE)

/***** ADD_IP ******/
#define IP_ADDR_OFFSET    (IOTD_HEADER_LEN + DM_HEADER_SIZE)

/****** Prefix match *******/
/*Response contains: 1 byte- status
                     4 bytes- Match length*/
#define DM_PREFIX_MATCH_RESP_SIZE (DM_HEADER_SIZE + 5)

#define MAX_PAYLOAD_SIZE                     (2048)
int dataManager_init(void* pCxt);
int dataManager_deinit(void* pCxt);
int32_t rtlink_get_addr(uint8_t* buffer, int size);
void* get_lifetime(uint8_t* ip, uint8_t* buf, uint32_t size);
int prefix_match(uint8_t* src, uint8_t* dest, int prefixlen);
int mask_to_prefixLen(uint8_t* IP6Mask);
#endif
