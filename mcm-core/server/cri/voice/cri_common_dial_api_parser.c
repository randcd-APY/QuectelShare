

/*************************************************************************************
   Copyright (c) 2014 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
**************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cri_common_dial_api_parser.h"
#include <sys/types.h>
#include <regex.h>

#define ACTION_SYMBOL_ACTIVATION    "*"
#define ACTION_SYMBOL_DEACTIVATION  "#"
#define ACTION_SYMBOL_INTERROGATION "*#"
#define ACTION_SYMBOL_REGISTRATION  "**"
#define ACTION_SYMBOL_ERASURE       "##"

// Codes for defined Supplementary Services
// Ref: 3GPP TS 22.030 [11] Annex B

// Calling Line Presentation
#define SERVICE_CODE_CLIP               "30"
#define SERVICE_CODE_CLIR               "31"

// Call Forwarding
#define SERVICE_CODE_CFU                "21"
#define SERVICE_CODE_CFB                "67"
#define SERVICE_CODE_CFNRy              "61"
#define SERVICE_CODE_CFNR               "62"
#define SERVICE_CODE_CF_ALL             "002"
#define SERVICE_CODE_CF_ALL_CONDITIONAL "004"

// Call Waiting
#define SERVICE_CODE_WAIT               "43"

// Call Barring
#define SERVICE_CODE_BAOC               "33"
#define SERVICE_CODE_BAOIC              "331"
#define SERVICE_CODE_BAOICxH            "332"
#define SERVICE_CODE_BAIC               "35"
#define SERVICE_CODE_BAICr              "351"
#define SERVICE_CODE_BA_ALL             "330"
#define SERVICE_CODE_BA_MO              "333"
#define SERVICE_CODE_BA_MT              "353"

#define MAX_SI_SIZE 12  // Verify! Not mentioned in 3GPP TS 22.030 6.5.2

struct _common_dial_token_type
{
    regex_t* regex_compiled;

    char action_symbol_activation[3];
    char action_symbol_deactivation[3];
    char action_symbol_interrogation[3];
    char action_symbol_registration[3];
    char action_symbol_erasure[3];

    // Calling Line Presentation
    char service_code_clip[3];
    char service_code_clir[3];

    // Call Forwarding
    char service_code_cfu[3];
    char service_code_cfb[3];
    char service_code_cfnry[3];
    char service_code_cfnr[3];
    char service_code_cf_all[4];
    char service_code_cf_all_conditional[4];

    // Call Waiting
    char service_code_wait[3];

    // Call Barring
    char service_code_baoc[3];
    char service_code_baoic[4];
    char service_code_baoicxh[4];
    char service_code_baic[3];
    char service_code_baicr[4];
    char service_code_ba_all[4];
    char service_code_ba_mo[4];
    char service_code_ba_mt[4];
};

char regex_standard[] = "((\\*|#|\\*#|\\*\\*|##)([[:digit:]]{2,3})(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*)(\\*([^*#]*))?)?)?)?#)([^#]*)";
/*                       1  2                    3                  4  5       6   7         8    9     10  11             12

         1 = Full string up to and including #
         2 = action (activation/interrogation/registration/erasure)
         3 = service code
         5 = SIA
         7 = SIB
         9 = SIC
         10 = dialing number
*/

common_dial_token_type* common_dial_parser_init()
{
    common_dial_token_type* token = malloc(sizeof(common_dial_token_type));
    token->regex_compiled = malloc(sizeof(regex_t));

    int resp = regcomp(token->regex_compiled, regex_standard, REG_EXTENDED);
    if (resp == 0)
    {
        printf("Regex compilation successful! %zd paranthetical subexpressions found.\n", token->regex_compiled->re_nsub);
    }
    else
    {
        printf("Error number %d occured while parsing regex.\n", resp);
    }

    // Load the token with standard values.

    strlcpy(token->action_symbol_activation, ACTION_SYMBOL_ACTIVATION, 3);
    strlcpy(token->action_symbol_deactivation, ACTION_SYMBOL_DEACTIVATION, 3);
    strlcpy(token->action_symbol_interrogation, ACTION_SYMBOL_INTERROGATION, 3);
    strlcpy(token->action_symbol_registration, ACTION_SYMBOL_REGISTRATION, 3);
    strlcpy(token->action_symbol_erasure, ACTION_SYMBOL_ERASURE, 3);

    strlcpy(token->service_code_clip, SERVICE_CODE_CLIP, 3);
    strlcpy(token->service_code_clir, SERVICE_CODE_CLIR, 3);
    strlcpy(token->service_code_cfu, SERVICE_CODE_CFU, 3);
    strlcpy(token->service_code_cfb, SERVICE_CODE_CFB, 3);
    strlcpy(token->service_code_cfnry, SERVICE_CODE_CFNRy, 3);
    strlcpy(token->service_code_cfnr, SERVICE_CODE_CFNR, 3);
    strlcpy(token->service_code_cf_all, SERVICE_CODE_CF_ALL, 4);
    strlcpy(token->service_code_cf_all_conditional, SERVICE_CODE_CF_ALL_CONDITIONAL, 4);
    strlcpy(token->service_code_wait, SERVICE_CODE_WAIT, 3);
    strlcpy(token->service_code_baoc, SERVICE_CODE_BAOC, 3);
    strlcpy(token->service_code_baoic, SERVICE_CODE_BAOIC, 4);
    strlcpy(token->service_code_baoicxh, SERVICE_CODE_BAOICxH, 4);
    strlcpy(token->service_code_baic, SERVICE_CODE_BAIC, 3);
    strlcpy(token->service_code_baicr, SERVICE_CODE_BAICr, 4);
    strlcpy(token->service_code_ba_all, SERVICE_CODE_BA_ALL, 4);
    strlcpy(token->service_code_ba_mo, SERVICE_CODE_BA_MO, 4);
    strlcpy(token->service_code_ba_mt, SERVICE_CODE_BA_MT, 4);

    return token;
};

// *** TO DO ***: Regex overriding function. It will compile the custom regex and update the token. //

common_dial_service_info_type common_dial_get_service_info( common_dial_token_type*  token,
                                                        char                     input[],
                                                        common_dial_call_type    type )
{

    printf("Dial string: %s\n", input);

    int ss_flag = 0;

    // Initialize the common_dial_service info structure.
    common_dial_service_info_type info;
    info.error          = COMMON_DIAL_NO_ERROR;
    info.sia[0]         = '\0';
    info.sib[0]         = '\0';
    info.sic[0]         = '\0';
    info.poundstring[0] = '\0';
    info.dialnumber[0]  = '\0';
    info.reqtype        = -1;
    info.service        = -1;
    info.action         = -1;

    if (strlen(input) == 0)
    {
        info.error = COMMON_DIAL_ERROR_EMPTY_DIAL_STRING;
        return info;
    }
    else if (strlen(input) > 81)
    {
        info.error = COMMON_DIAL_ERROR_DIAL_STRING_TOO_LONG;
        return info;
    }

    regmatch_t matchptr[12];
    regex_t *regex = token->regex_compiled;
    int res = regexec(regex, input, 12, matchptr, 0);
    regmatch_t match;
    match = matchptr[2];

    // Check if string is in SS format.
    if (res == 0 && match.rm_so == 0)
    {
        printf("Valid SS string.\n");
        ss_flag = 1;
        info.reqtype = COMMON_DIAL_SS;
    }
    // Check if string is a pound (#) string. Ref: TS 22.030 6.5.3.2.
    else if (input[strlen(input) - 1] == '#')
    {
        printf("Pound string detected.\n");
        memcpy(info.poundstring, &input[0], strlen(input) + 1);
        info.reqtype = COMMON_DIAL_USSD;
    }
    // Check if the string is a Short String. Ref: TS 22.030 6.5.3.2.
    else if (common_dial_short_string_check(input) == 0)
    {
        printf("Short string detected.\n");
        memcpy(info.dialnumber, &input[0], strlen(input) + 1);
        info.reqtype = COMMON_DIAL_VOICE_CALL;
    }
    // Else treat dial string as dial number.
    else
    {
        printf("Dial string treated as dial number.\n");
        memcpy(info.dialnumber, &input[0], strlen(input) + 1);
        info.reqtype = COMMON_DIAL_VOICE_CALL;
    }

    // If the dial string conforms to the SS format.
    if (ss_flag == 1)
    {
    int i;
    for (i = 0; i < 12; i++)
    {
        // Check if the subexpression group has any information.
        if (matchptr[i].rm_so != -1)
        {
            // printf("%d: %d --> %d\n", i, matchptr[i].rm_so, matchptr[i].rm_eo);
            match = matchptr[i];

            switch (i)
            {
                case 2: // Action
                {
                    int size = match.rm_eo - match.rm_so;
                    char action[size + 1];
                    memcpy(action, &input[match.rm_so], size);
                    action[size] = '\0';

                    if (strcmp(action, token->action_symbol_activation) == 0)
                    {
                        info.action = COMMON_DIAL_ACTION_ACTIVATION;
                    }
                else if (strcmp(action, token->action_symbol_deactivation) == 0)
                {
                info.action = COMMON_DIAL_ACTION_DEACTIVATION;
            }
                    else if (strcmp(action, token->action_symbol_interrogation) == 0)
                    {
                        info.action = COMMON_DIAL_ACTION_INTERROGATION;
                    }
                    else if (strcmp(action, token->action_symbol_registration) == 0)
                    {
                        info.action = COMMON_DIAL_ACTION_REGISTRATION;
                    }
                    else if (strcmp(action, token->action_symbol_erasure) == 0)
                    {
                        info.action = COMMON_DIAL_ACTION_ERASURE;
                    }
                    else
                    {
                        info.action = COMMON_DIAL_ACTION_UNKNOWN;
                    }
                    break;
                }

                case 3: // Service code
                {
                    int size = match.rm_eo - match.rm_so;
                    char code[size + 1];
                    memcpy(code, &input[match.rm_so], size);
                    code[size] = '\0';

                    if (strcmp(code, token->service_code_clip) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CLIP;
                    }
                    else if (strcmp(code, token->service_code_clir) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CLIR;
                    }
                    else if (strcmp(code, token->service_code_cfu) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_FORWARDING_UNCONDITIONAL;
                    }
                    else if (strcmp(code, token->service_code_cfb) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_FORWARDING_BUSY;
                    }
                    else if (strcmp(code, token->service_code_cfnry) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_FORWARDING_NO_REPLY;
                    }
                    else if (strcmp(code, token->service_code_cfnr) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_FORWARDING_NOT_REACHABLE;
                    }
                    else if (strcmp(code, token->service_code_cf_all) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_FORWARDING_ALL;
                    }
                    else if (strcmp(code, token->service_code_cf_all_conditional) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_FORWARDING_ALL_CONDITIONAL;
                    }
                    else if (strcmp(code, token->service_code_wait) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_WAITING;
                    }
                    else if (strcmp(code, token->service_code_baoc) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_OUTGOING;
                    }
                    else if (strcmp(code, token->service_code_baoic) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_OUTGOING_INTERNATIONAL;
                    }
                    else if (strcmp(code, token->service_code_baoicxh) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_OUTGOING_INTERNATIONAL_EXCLUDING_HOME;
                    }
                    else if (strcmp(code, token->service_code_baic) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_INCOMING;
                    }
                    else if (strcmp(code, token->service_code_baicr) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_INCOMING_ROAMING;
                    }
                    else if (strcmp(code, token->service_code_ba_all) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_ALL_SERVICES;
                    }
                    else if (strcmp(code, token->service_code_ba_mo) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_OUTGOING_SERVICES;
                    }
                    else if (strcmp(code, token->service_code_ba_mt) == 0)
                    {
                       info.service = COMMON_DIAL_SS_CALL_BARRING_INCOMING_SERVICES;
                    }
                    else
                    {
                       info.service = COMMON_DIAL_SS_UNKNOWN;
                    }

                    break;
                }

                case 5: // SIA
                {
                    int size = match.rm_eo - match.rm_so;
                    if (size < MAX_SI_SIZE)
                    {
                        memcpy(info.sia, &input[match.rm_so], size);
                        info.sia[size] = '\0';
                    }
                    break;
                }

                case 7: // SIB
                {
                    int size = match.rm_eo - match.rm_so;
                    if (size < MAX_SI_SIZE)
                    {
                        memcpy(info.sib, &input[match.rm_so], size);
                        info.sib[size] = '\0';
                    }
                    break;
                }

                case 9: // SIC
                {
                    int size = match.rm_eo - match.rm_so;
                    if (size < MAX_SI_SIZE)
                    {
                        memcpy(info.sic, &input[match.rm_so], size);
                        info.sic[size] = '\0';
                    }
                    break;
                }
            }

        }
    }
    }

    return info;

}


void common_dial_cleanup(common_dial_token_type* token)
{
    regfree(token->regex_compiled);
    free(token->regex_compiled);
    free(token);
}


int common_dial_short_string_check(char input[])
{
    if (strlen(input) > 2)
    {
        return -1;
    }
    else
    {
        // Note: May need to include allowed character verification by ASCII codes.
        return 0;
    }

}




