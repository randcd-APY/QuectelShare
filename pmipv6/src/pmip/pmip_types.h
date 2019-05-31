/*! \file pmip_types.h
* \brief Describe all types for pmip
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair3@eurecom.fr
*/

/** @defgroup PACKETS TYPES
 * @ingroup PMIP6D
 *  PMIP Types
 *  @{
 */

#ifndef __PMIP_TYPES_H__
#    define __PMIP_TYPES_H__
//---------------------------------------------------------------------------------------------------------------------
#    include <linux/types.h>
#    include <netinet/ip6mh.h>
//---------------------------------------------------------------------------------------------------------------------
#    include "mh.h"
//---------------------------------------------------------------------------------------------------------------------
#include "pmipv4_conf.h"


#define PMIP_IPV6_ADDR_LEN 16
#define PMIP_MAC_ADDR_LEN  6
#define QCMAP_MSGR_MAC_ADDR_NUM_CHARS_V01 18
#define PMIP_MAX_IDENTIFIER_STRING_LEN  255



/*
* Mobility Option TLV data structure
*New options defined for Proxy BU & BA
*/
/*! \struct  ip6_mh_opt_home_net_prefix_t
* \brief Mobility Option TLV data structure: Home Network Prefix Option.
*/
struct ip6_mh_opt_home_net_prefix_t {   /*Home netowork prefix option */
__u8				ip6hnp_type;	/*!< \brief Type is  "Home Network Prefix Option" value.*/
__u8				ip6hnp_len;		/*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 18.*/
__u8			ip6hnp_reserved;	/*!< \brief This 8-bit field is unused for now.  The value MUST be initialized to 0 by the sender and MUST be ignored by the receiver.*/
__u8			ip6hnp_prefix_len;	/*!< \brief 8-bit unsigned integer indicating the prefix length of the IPv6 prefix contained in the option.*/
struct in6_addr	ip6hnp_prefix;  /*!< \brief A sixteen-byte field containing the mobile node's IPv6 Home Network Prefix.*/
} __attribute__ ((__packed__));

typedef struct ip6_mh_opt_home_net_prefix_t ip6_mh_opt_home_net_prefix_t;
#if 0
union info_t {
struct info_t1{
   __u16 plen : 6; /*!< \brief 6-bit unsigned integer indicating the length of prefix.*/
   __u16 res : 10; /*!< \brief This 10-bit field is unused for now.  The value MUST be initialized to 0 by the sender and MUST be ignored by the receiver.*/
}m;
__u16 data;
};
#endif

struct flag_res_t{
   unsigned int res : 7; /*!< \brief This 7-bit field is unused for now.  The value MUST be initialized to 0 by the sender and MUST be ignored by the receiver.*/
   unsigned int isv4dmnp : 1; /*!< \brief 1-bit unsigned integer indicating ipv4 or ipv6 DMNP prefix.*/
} __attribute__ ((__packed__));


/*! \struct  ip_mh_opt_v4_home_addr_request_option_t
* \brief Mobility Option TLV data structure: Home Network Prefix request Option.
*/
struct ip_mh_opt_v4_home_addr_request_option_t {   /*Home netowork prefix option */
__u8				ip4hnp_type;	/*!< \brief Type is  "Home Network Prefix Option" value.*/
__u8 ip4_opt_len; //option length
#if 0
#if defined(__LITTLE_ENDIAN_BITFIELD)
         __u16    plen:6,
                 res:10;
#elif defined (__BIG_ENDIAN_BITFIELD)
         __u16    res:10,
                 plen:6;
#else
#endif
#endif
__u16 plen;
 //union info_t var;
struct in_addr	ip4hnp_prefix;  /*!< \brief A four-byte field containing the mobile node's IPv4 Home Network Prefix.*/
} __attribute__ ((__packed__));

typedef struct ip_mh_opt_v4_home_addr_request_option_t ip_mh_opt_v4_home_addr_request_option_t;
#if 0
struct bit_info_t{
   unsigned int res : 2; /*!< \brief This 2-bit field is unused for now.  The value MUST be initialized to 0 by the sender and MUST be ignored by the receiver.*/
   unsigned int plen : 6; /*!< \brief 6-bit unsigned integer indicating the length of prefix.*/
} __attribute__ ((__packed__));
#endif

/*! \struct  ip_mh_opt_v4_home_addr_reply_option_t
* \brief Mobility Option TLV data structure: Home Network Prefix reply Option.
*/
struct ip_mh_opt_v4_home_addr_reply_option_t {   /*Home netowork prefix option */
__u8				ip4hnp_type;	/*!< \brief Type is  "Home Network Prefix Option" value.*/
__u8 ip4_opt_len; //option length
__u8 status;
__u8 plen; //include 6 bit  prfix lengh & 2 bit  reserv field
// struct bit_info_t var;
struct in_addr	ip4hnp_prefix;  /*!< \brief A four-byte field containing the mobile node's IPv4 Home Network Prefix.*/
} __attribute__ ((__packed__));

typedef struct ip_mh_opt_v4_home_addr_reply_option_t ip_mh_opt_v4_home_addr_reply_option_t;


/*! \struct  ip_mh_opt_v4_dmnp_prefix_option_t
* \brief DMNP prefix option.
*/
struct ip_mh_opt_v4_dmnp_prefix_option_t {   /*Home netowork prefix option */
__u8				ip4hnp_type;	/*!< \brief Type is  "DMNP prefix type" value.*/
__u8 ip4_opt_len; //option length
 struct flag_res_t var;;
 __u8 prefix_len;
struct in_addr	dmnp_prefix;  /*!< \brief DMNP prefix.*/
} __attribute__ ((__packed__));

typedef struct ip_mh_opt_v4_dmnp_prefix_option_t ip_mh_opt_v4_dmnp_prefix_option_t;

/*! \struct  ip_mh_opt_v4_dmnp_prefix_option_t
* \brief DMNP prefix option.
*/
struct ip_mh_opt_v4_default_router_addr_option_t {   /*Default Router addr*/
__u8				ip4hnp_type;	/*!< \brief Type is  "DMNP prefix type" value.*/
__u8 ip4_opt_len; //option length
__u16 res;
struct in_addr	default_router_addr;  /*!< \brief default router address*/
} __attribute__ ((__packed__));

typedef struct ip_mh_opt_v4_default_router_addr_option_t ip_mh_opt_v4_default_router_addr_option_t;



/*! \struct  ip6mnid_t
* \brief Contain a mobile node identifier.
*/
typedef struct {
	__u32				first;
	__u32				second;
} ip6mnid_t;

/*! \struct  ip6mnid_t
* \brief Contain a mobile node identifier.
*/
typedef struct {
	uint8_t 	  mac_addr[PMIP_MAC_ADDR_LEN];
} ip6mnid_mac_t;

/*! \struct  ip6ts_t
* \brief Timestamp structure.
*/
typedef struct {
	__u8				first[6];
	__u16				second;
} ip6ts_t;

typedef struct {
	__u8				protocol_id[2];
	__u8				length;
} protocol_info;


typedef struct {
	__u8				reserve;
	__u8				extention;
	protocol_info		proto_info;
} vendor_data;



/*! \struct  ip6_mh_opt_mobile_node_identifier_t
* \brief Mobility Option TLV data structure: Mobile Node Identifier Option (RFC4283).
*/
struct ip6_mh_opt_mobile_node_identifier_t {
    __u8                ip6mnid_type;   /*!< \brief MN-ID-OPTION-TYPE has been assigned value 8 by the IANA. It is an  8-bit identifier of the type mobility option.*/
    __u8                ip6mnid_len;    /*!< \brief 8-bit unsigned integer, representing the length in octets of the Subtype and Identifier fields.*/
    __u8                ip6mnid_subtype;/*!< \brief Mobile interface identifier subtype .*/
    char       ip6mnid_identifier[PMIP_MAX_IDENTIFIER_STRING_LEN];     /*!< \brief Mobile interface identifier */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_mobile_node_identifier_t ip6_mh_opt_mobile_node_identifier_t;



/*! \struct  ip6_mh_opt_mobile_node_link_layer_identifier_t
* \brief Mobility Option TLV data structure: Mobile Node Link-Layer Identifier Option (RFC5213).
*/
struct ip6_mh_opt_mobile_node_link_layer_identifier_t {
	__u8                ip6mnllid_type;     /*!< \brief MN-ID-OPTION-TYPE has been assigned value 8 by the IANA. It is an  8-bit identifier of the type mobility option.*/
	__u8                ip6mnllid_len;      /*!< \brief 8-bit unsigned integer, representing the length in octets of the Subtype and Identifier fields.*/
	__u16               ip6mnllid_reserved; /*!< \brief  16 bits reserved.*/
	ip6mnid_t           ip6mnllid_lli;      /*!< \brief Mobile node link-layer identifier.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_mobile_node_link_layer_identifier_t ip6_mh_opt_mobile_node_link_layer_identifier_t;


/*! \struct  ip6_mh_opt_time_stamp_t
* \brief Mobility Option TLV data structure: Timestamp Option (RFC5213).
*/
struct ip6_mh_opt_time_stamp_t {
	__u8				ip6mots_type;	/*!< \brief Option type, value is 27.*/
	__u8				ip6mots_len;	/*!< \brief 8-bit unsigned integer indicating the length in octets of the option, excluding the type and length fields. The value for this field MUST be set to 8.*/
	ip6ts_t				time_stamp;		/*!< \brief A 64-bit unsigned integer field containing a timestamp.  The value indicates the number of seconds since January 1, 1970, 00:00 UTC, by using a fixed point format.  In this format, the integer number of seconds is contained in the first 48 bits of the field, and the remaining 16 bits indicate the number of 1/65536 fractions of a second.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_time_stamp_t ip6_mh_opt_time_stamp_t;


/*! \struct  ip6_mh_link_local_add_t
* \brief Mobility Option TLV data structure: Link-local Address Option (RFC5213).
*/
struct ip6_mh_opt_link_local_address_t {
    __u8                ip6link_type;   /*!< \brief Option type, value is 26.*/
    __u8                ip6link_len;    /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 16.*/
    struct in6_addr     ip6link_addr;   /*!< \brief A sixteen-byte field containing the link-local address.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_link_local_address_t ip6_mh_opt_link_local_address_t;

/*! \struct  ip6_mh_handoff_indicator_t
* \brief Mobility Option TLV data structure: Handoff Indicator Option (RFC5213).
*/
struct ip6_mh_opt_handoff_indicator_t {
    __u8                ip6hi_type;     /*!< \brief Option type, value is 23.*/
    __u8                ip6hi_len;      /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 2.*/
    __u8                ip6hi_reserved; /*!< \brief Field unused for now (RFC5213), must be set to 0.*/
    __u8                ip6hi_hi;       /*!< \brief A 8-bit field containing the handoff indicator.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_handoff_indicator_t ip6_mh_opt_handoff_indicator_t;


struct ip6_mh_opt_gre_key_t {
    __u8                ip6gre_type;     /*!< \brief Option type, value is 33.*/
    __u8                ip6gre_len;      /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 2.*/
    __u16               ip6gre_reserved; /*!< \brief Field unused for now, must be set to 0.*/
    __u32               ip6gre_key;       /*!< \brief A 32-bit field containing the GRE Key value*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_gre_key_t ip6_mh_opt_gre_key_t;


/*! \struct  ip6_mh_handoff_indicator_t
* \brief Mobility Option TLV data structure: Handoff Indicator Option (RFC5213).
*/
struct ip6_mh_opt_access_technology_type_t {
    __u8                ip6att_type;     /*!< \brief Option type, value is 23.*/
    __u8                ip6att_len;      /*!< \brief 8-bit unsigned integer indicating the length of the option in octets, excluding the type and length fields. This field MUST be set to 2.*/
    __u8                ip6att_reserved; /*!< \brief Field unused for now (RFC5213), must be set to 0.*/
    __u8                ip6att_att;      /*!< \brief A 8-bit field containing the access technology type.*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_access_technology_type_t ip6_mh_opt_access_technology_type_t;


// ******** Extended options for cluster based architecture & Route optimiztion ***********
struct ip6_mh_opt_dst_mn_addr_t {
	__u8				ip6dma_type;
	__u8				ip6dma_len;
	struct in6_addr		dst_mn_addr;    /* Destination MN Address */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_dst_mn_addr_t ip6_mh_opt_dst_mn_addr_t;


struct ip6_mh_opt_serv_mag_addr_t {
	__u8				ip6sma_type;
	__u8				ip6sma_len;
	struct in6_addr		serv_mag_addr;  /* Serving MAG Address of the destination */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_serv_mag_addr_t ip6_mh_opt_serv_mag_addr_t;


struct ip6_mh_opt_serv_lma_addr_t {
	__u8				ip6sla_type;
	__u8				ip6sla_len;
	struct in6_addr		serv_lma_addr;  /* Serving LMA Address of the destination */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_serv_lma_addr_t ip6_mh_opt_serv_lma_addr_t;


struct ip6_mh_opt_src_mn_addr {
	__u8				ip6sma_type;
	__u8				ip6sma_len;
	struct in6_addr		src_mn_addr;    /* Source MN Address */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_src_mn_addr_t ip6_mh_opt_src_mn_addr_t;


struct ip6_mh_opt_src_mag_addr_t {
	__u8				ip6sma_type;
	__u8				ip6sma_len;
	struct in6_addr		src_mag_addr;   /* Source MAG Address */
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_src_mag_addr_t ip6_mh_opt_src_mag_addr_t;

/*! \struct  ip6_mh_opt_mobile_node_identifier_t
* \brief Mobility Option TLV data structure: Mobile Node Identifier Option (RFC4283).
*/
struct ip6_mh_opt_service_selection_t {
    __u8                ip6service_sel_type;   /*!< \brief  It is an  8-bit identifier of the type mobility option.*/
    __u8                ip6service_sel_len;    /*!< \brief 8-bit unsigned integer, representing the length of the
                                                                                    Service Selection Mobility Option in octets, excluding the Option
                                                                                    Type and Option Length fields.*/
    char                identifier[PMIP_MAX_IDENTIFIER_STRING_LEN];       /*!< \brief service identifier string used to identify the requested service .*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_service_selection_t ip6_mh_opt_service_selection_t;

/*! \struct  ip6_mh_opt_vendor_specific_option_t
*/
struct ip6_mh_opt_vendor_specific_option_t {
    __u8                ip6vendor_spec_option_type;   /*!< \brief It is an  8-bit identifier of the type mobility option.*/
    __u8                ip6vendor_spec_option_len;    /*!< \brief 8-bit unsigned integer, representing the length in octets excluding the Type and the Length fields.*/
    __u16               ip6vendor_id[2];                 /*!< \brief Vendor Id .*/
    __u8                ip6sub_type;                  /*!< \brief .An 8-bit field indicating the type of vendor-specific information carried in the option*/
    vendor_data         ven_data;                  /*!< vendor data*/
} __attribute__ ((__packed__));
typedef struct ip6_mh_opt_vendor_specific_option_t ip6_mh_opt_vendor_specific_option_t;



/*! \struct  msg_info_t
* \brief Meta structure that can store all usefull information of any PMIP message.
*/
typedef struct msg_info_t {
	struct in6_addr				src;				/*!< \brief Source address of the message.*/
	struct in6_addr				dst;				/*!< \brief Destination address of the message.*/
	struct in_addr				srcv4;				/*!< \brief Source v4 address of the message.*/
	struct in_addr				dstv4;				/*!< \brief Destination v4 address of the message.*/

	struct in6_addr_bundle		addrs;				/*!< \brief Tuple composed of src and dst address.*/
	struct in_addr_bundle		addrsv4;				/*!< \brief Tuple composed of src and dst address v4.*/
	int							iif;				/*!< \brief Interface identifier.*/
	uint32_t					msg_event;			/*!< \brief Type of event received: hasDEREG hasWLCCP hasRS hasNA hasNS hasPBU hasPBA hasPBREQ hasPBRES */
	struct in6_addr				mn_iid;				/*!< \brief Mobile node IID.*/
	struct in6_addr	            mn_hw_address;		/*!< \brief MAC ADDR */
	struct in6_addr				mn_addr;			/*!< \brief Full mobile node address */
	struct in6_addr				mn_prefix;			/*!< \brief Network Address Prefix for MN */
	struct in6_addr				mn_serv_mag_addr;	/*!< \brief Serving MAG Address */
	struct in6_addr				mn_serv_lma_addr;	/*!< \brief Serving LMA Address */
	struct in6_addr				mn_link_local_addr;	/*!< \brief Link Local Address  for mobile node */
	struct timespec				addtime;			/*!< \brief When was the binding added or modified */
	struct timespec				lifetime;			/*!< \brief Lifetime sent in this BU, in seconds */
	uint16_t					seqno;				/*!< \brief Sequence number of the message */
	uint16_t					PBU_flags;			/*!< \brief PBU flags */
	uint8_t						PBA_flags;			/*!< \brief PBA flags */
	ip6ts_t						timestamp;			/*!< \brief Timestamp */
	struct in6_addr				src_mag_addr;		/*!< \brief Route optimization or flow control: Source MAG Address */
	struct in6_addr				src_mn_addr;		/*!< \brief Route optimization or flow control: Source MN Address */
	struct in6_addr				na_target;			/*!< \brief Route optimization or flow control: Neighbour advertisement target*/
	struct in6_addr				ns_target;			/*!< \brief Route optimization or flow control: Neighbour solicitation target*/
	int							is_dad;				/*!< \brief Route optimization or flow control: is NS used for DAD process?*/
	int							hoplimit;			/*!< \brief Route optimization or flow control: Hop limit*/
	uint8_t						gre_key_valid;		/*!< \brief Gre Key info valid or not */
	uint32_t					gre_key;			/*!< \brief GRE Key value info */
	struct in_addr				dmnp_prefix;				/*!< \brief DMNP prefix.*/
	uint8_t						dmnp_prefix_len;				/*!< \brief DMNP prefix len*/
	uint8_t 					dmnp_pba_status;	/*! < brief DMNP PBA status*/
	int 						hasipv4_dmnp_prefix_option;
	int 						pmipv4_hnp_req_valid;
	int 						pmipv4_hnp_reply_valid;
	struct in_addr				dhcp_request_ip;

	//default router info
	struct in_addr				default_router;				/*!< \brief default router address.*/
	int 						hasipv4_default_router_option;

	struct in_addr				v4_home_prefix;				/*!< \brief DMNP prefix.*/
	uint8_t						v4_home_prefixlen;
	uint8_t						v4_home_addr_reply_status;
} msg_info_t;
#endif
/*@}*/
