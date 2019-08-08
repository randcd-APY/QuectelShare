
/*************************************************************************************
   Copyright (c) 2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include <sys/types.h>
#include <regex.h>

#define MAX_SI_SIZE               12 // Verify! Not mentioned in 3GPP TS 22.030 6.5.2
#define MAX_EMERGENCY_NUMBER_SIZE 4  // Verify!
#define MAX_POUNDSTRING_SIZE      40 // Verify! Not mentioned in 3GPP TS 22.030 6.5.3.2
#define MAX_DIAL_NUMBER_SIZE      81

typedef enum
{
    COMMON_DIAL_VOICE_CALL,
    COMMON_DIAL_USSD,
    COMMON_DIAL_SS
} common_dial_request_type;

typedef enum
{
   // Note: Comments are the supplementary services as specified in
   //       3GPP TS 22.030 Annex B.

   COMMON_DIAL_SS_CLIP,                                               // CLIP
   COMMON_DIAL_SS_CLIR,                                               // CLIR
   COMMON_DIAL_SS_CALL_FORWARDING_UNCONDITIONAL,                      // CFU
   COMMON_DIAL_SS_CALL_FORWARDING_BUSY,                               // CF Busy
   COMMON_DIAL_SS_CALL_FORWARDING_NO_REPLY,                           // CF No Reply
   COMMON_DIAL_SS_CALL_FORWARDING_NOT_REACHABLE,                      // CF Not Reachable
   COMMON_DIAL_SS_CALL_FORWARDING_ALL,                                // CF All
   COMMON_DIAL_SS_CALL_FORWARDING_ALL_CONDITIONAL,                    // CF All Conditional
   COMMON_DIAL_SS_CALL_WAITING,                                       // WAIT
   COMMON_DIAL_SS_CALL_BARRING_OUTGOING,                              // BAOC
   COMMON_DIAL_SS_CALL_BARRING_OUTGOING_INTERNAL,                     // BAOIC
   COMMON_DIAL_SS_CALL_BARRING_OUTGOING_INTERNAL_EXCLUDING_HOME,      // BAOIC Excluding Home
   COMMON_DIAL_SS_CALL_BARRING_INCOMING,                              // BAIC
   COMMON_DIAL_SS_CALL_BARRING_INCOMING_ROAMING,                      // BAIC Roaming
   COMMON_DIAL_SS_CALL_BARRING_ALL_SERVICES,                          // All Barring Services
   COMMON_DIAL_SS_CALL_BARRING_OUTGOING_SERVICES,                     // Outgoing Barring Services
   COMMON_DIAL_SS_CALL_BARRING_INCOMING_SERVICES,                     // Incoming Barring Services
   COMMON_DIAL_SS_UNKNOWN
} common_dial_ss_type;

typedef enum
{
    COMMON_DIAL_ACTION_ACTIVATION,
    COMMON_DIAL_ACTION_DEACTIVATION,
    COMMON_DIAL_ACTION_INTERROGATION,
    COMMON_DIAL_ACTION_REGISTRATION,
    COMMON_DIAL_ACTION_ERASURE,
    COMMON_DIAL_ACTION_UNKNOWN
} common_dial_action_type; // Ref: 3GPP TS 22.030 6.5

typedef enum
{
    COMMON_DIAL_VOICE,
    COMMON_DIAL_IMF
} common_dial_call_type;

typedef enum
{
    COMMON_DIAL_NO_ERROR,
    COMMON_DIAL_ERROR_EMPTY_DIAL_STRING,
    COMMON_DIAL_ERROR_DIAL_STRING_TOO_LONG,
    COMMON_DIAL_ERROR_INVALID_NETWORK_CONFIG,
    COMMON_DIAL_ERROR_INVALID_STANDARD_OVERRIDE
} common_dial_error_type;

typedef struct
{
    common_dial_error_type    error;
    common_dial_request_type  reqtype;
    common_dial_ss_type       service;
    common_dial_action_type   action;
    char                      sia[MAX_SI_SIZE];
    char                      sib[MAX_SI_SIZE];
    char                      sic[MAX_SI_SIZE];
    char                      poundstring[MAX_POUNDSTRING_SIZE];
    char                      dialnumber[MAX_DIAL_NUMBER_SIZE];
} common_dial_service_info_type; // Ref: 3GPP TS 22.030 6.5

typedef struct
{
    // !! Under Construction !!
} common_dial_custom_config_type;


struct _common_dial_token_type;
typedef struct _common_dial_token_type common_dial_token_type;

//=============================================================================
// FUNCTION: common_dial_parser_init
//
// DESCRIPTION:
// A function to initialize the Common Dial API parser. The function returns a
// token which will be required to call other parser methods.
//
// @return
//    common_dial_token_type*
//=============================================================================
common_dial_token_type* common_dial_parser_init();


//=============================================================================
// FUNCTION: common_dial_change_config
//
// DESCRIPTION:
// A function to initialize the Common Dial API parser. The function returns a
// token which will be required to call other parser methods.
//
// @param[in]
//    token   A pointer to the token returned by the init method.
//    config  A pointer to the custom configuration.
//
// @return
//    common_dial_token_type
//=============================================================================
common_dial_token_type* common_dial_change_config( common_dial_token_type*          token,
                                                   common_dial_custom_config_type*  config );

//=============================================================================
// FUNCTION: common_dial_get_service_type
//
// DESCRIPTION:
// A function to identify the service requested by the input string after
// parsing and analyzing it's structure and arguments.
//
// @param[in]
//    token    A pointer to the token returned by the init method.
//    input     The string input to be analyzed.
//    type      Voice or IMF call type.
//
// @return
//    common_dial_service_type
//=============================================================================
common_dial_service_info_type common_dial_get_service_info( common_dial_token_type*  token,
                                                            char                     input[],
                                                            common_dial_call_type    type );

//=============================================================================
// FUNCTION: common_dial_get_service_type
//
// DESCRIPTION:
// A function to deallocate memory for all the internal data structures.
//
// @param[in]
//    token    A pointer to the token returned by the init method.
//
//=============================================================================
void common_dial_cleanup(common_dial_token_type* token);



