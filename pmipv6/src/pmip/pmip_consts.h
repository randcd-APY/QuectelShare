/*! \file pmip_consts.h
 * \brief Describe all constants for pmip
 * \author OpenAir3 Group
 * \date 12th of October 2010
 * \version 1.0
 * \company Eurecom
 * \project OpenAirInterface
 * \email: openair3@eurecom.fr
 */

/** \defgroup CONSTANTS CONSTANTS
 * \ingroup PMIP6D
 *  PMIP CONSTANTS
 *  @{
 */

#ifndef __pmip_consts_h
#    define __pmip_consts_h
#    include <netinet/in.h>

#    define MAX_MOBILES 64

/*
 * Mobility Header Message Option Types
 * new mobility header options types defined
 */
#    define IP6_MHOPT_MOBILE_NODE_IDENTIFIER            0x08   /* Mobile Node Identifier Option */
/*
 * Mobility Header Message Option Types
 */
#    define IP6_MHOPT_DST_MN_ADDR                       0x0B   /* Source Mobile Node address */
#    define IP6_MHOPT_SERV_MAG_ADDR                     0x0C   /* Serving MAG address */
#    define IP6_MHOPT_SERV_LMA_ADDR                     0x0D   /* Source Mobile Node address */
#    define IP6_MHOPT_SRC_MN_ADDR                       0x0E   /* Source Mobile Node address */
#    define IP6_MHOPT_SRC_MAG_ADDR                      0x0F   /* Serving MAG address */
#    define IP6_MHOPT_HOME_NETWORK_PREFIX               0x16   /* Home Network Prefix */
#    define IP6_MHOPT_HANDOFF_INDICATOR                 0x17   /* Handoff Indicator Option */
#    define IP6_MHOPT_ACCESS_TECHNOLOGY_TYPE            0x18   /* Access Technology Type Option */
#    define IP6_MHOPT_MOBILE_NODE_LINK_LAYER_IDENTIFIER 0x19   /* Mobile Node Link Local Identifier Option */
#    define IP6_MHOPT_LINK_LOCAL_ADDRESS                0x1A   /* link local address */
#    define IP6_MHOPT_TIME_STAMP                        0x1B   /* Timestamp */
#    define IP6_MHOPT_GRE_KEY                           0x21   /* GRE KEY */
#    define IP6_VENDOR_SPECIFIC_OPTION                  0x13   /* IP6_VENDOR_SPECIFIC_OPTION */
#    define IP6_MHOPT_SERVICE_SELECTION                 0x14   /* SERVICE SELECTION */

//pmipv4 options
#define IPV4_HOME_ADDR_REQUEST_OPTION 0x24
#define IPV4_HOME_ADDR_REPLY_OPTION 0x25
#define IPV4_HOME_DEFAULT_ROUTER_OPTION 0x26
#define IPV4_DMNP_PREFIX_OPTION 0x37

//vendor option macro
#define IP6_VENDOR_SPECIFIC_OPTION_VENDOR_ID            0xaf28
#define IP6_VENDOR_SPECIFIC_OPTION_SUB_TYPE             0x01
#define IP6_VENDOR_SPECIFIC_OPTION_DATA_PROTOCOL_ID     0x0d


#    define IP6_MHOPT_PMIP_MAX                          IPV4_DMNP_PREFIX_OPTION


#    define IP6_MHOPT_HI_RESERVED                                                  0
#    define IP6_MHOPT_HI_ATTACHMENT_OVER_NEW_INTERFACE                             1
#    define IP6_MHOPT_HI_HANDOFF_BETWEEN_2_DIFF_INTERFACES_OF_SAME_MOBILE_NODE     2
#    define IP6_MHOPT_HI_HANDOFF_BETWEEN_MAGS_FOR_SAME_INTERFACE                   3
#    define IP6_MHOPT_HI_HANDOFF_STATE_UNKNOWN                                     4
#    define IP6_MHOPT_HI_HANDOFF_STATE_NOT_CHANGED                                 5

#    define IP6_MHOPT_ATT_RESERVED                                                 0
#    define IP6_MHOPT_ATT_VIRTUAL                                                  1
#    define IP6_MHOPT_ATT_PPP                                                      2
#    define IP6_MHOPT_ATT_IEEE802_3                                                3
#    define IP6_MHOPT_ATT_IEEE802_11ABG                                            4
#    define IP6_MHOPT_ATT_IEEE802_16E                                              5

#    define IP6_MH_TYPE_PBREQ   8   /* Proxy Binding Request */
#    define IP6_MH_TYPE_PBRES   9   /* Proxy Binding Response */

#    define IP6_MH_OPT_GRE_KEY_LEN       6
#    define IP6_MH_OPT_GRE_KEYLESS_LEN   2
#    define IP6_MHOPT_MOBILE_NODE_IDENTIFIER_LEN 7

//Define STATUS FLAGS for FSM.
#    define hasDEREG        0x00000050  /* Has a DEREG */
/*!< \brief WLCCP message originating from access point, captured on MAG, message informing about wireless association of a mobile node with the access point */
#    define hasWLCCP        0x00000040  /* Has a WLCCP CISCO protocol */
#    define hasRS           0x00000030  /* Has a RS */
#    define hasNA           0x00000020  /* Has a NA */
#    define hasNS           0x00000010  /* Has a NS */
#    define hasPBU          0x00000008  /* Has a PBU */
#    define hasPBA          0x00000004  /* Has a PBA */
#    define hasPBREQ        0x00000002  /* Has a PBRR */
#    define hasPBRES        0x00000001  /* Has a PBRE */
#    define PREFIX_LENGTH   64


#endif
/** @}*/
