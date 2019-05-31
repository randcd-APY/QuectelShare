#ifndef CSR_BT_PAC_STRING_H__
#define CSR_BT_PAC_STRING_H__

#include "csr_synergy.h"
/****************************************************************************

Copyright (c) 2014-2015 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_bt_ui_strings.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Key defines */
#define CSR_BT_PAC_APP_INVALID_KEY                          0x00
#define CSR_BT_PAC_APP_CONNECT_KEY                          0x01
#define CSR_BT_PAC_APP_DISCONNECT_KEY                       0x02
#define CSR_BT_PAC_APP_PHONEBOOK_KEY                        0x03
#define CSR_BT_PAC_APP_SETTINGS_KEY                         0x04

#define CSR_BT_PAC_APP_TELECOM_KEY                          0x05
#define CSR_BT_PAC_APP_SIM1_KEY                             0x06

#define CSR_BT_PAC_APP_ALL_CONTACTS_KEY                     0x07
#define CSR_BT_PAC_APP_MISSED_CALLS_KEY                     0x08
#define CSR_BT_PAC_APP_OUTGOING_CALLS_KEY                   0x09
#define CSR_BT_PAC_APP_INCOMING_CALLS_KEY                   0x10
#define CSR_BT_PAC_APP_COMBINED_CALLS_KEY                   0x11
#define CSR_BT_PAC_APP_SPEED_DIAL_KEY                       0x12
#define CSR_BT_PAC_APP_FAVORITES_KEY                        0x13

#define CSR_BT_PAC_APP_SIM1_ALL_CONTACTS_KEY                0x14
#define CSR_BT_PAC_APP_SIM1_MISSED_CALLS_KEY                0x15
#define CSR_BT_PAC_APP_SIM1_OUTGOING_CALLS_KEY              0x16
#define CSR_BT_PAC_APP_SIM1_INCOMING_CALLS_KEY              0x17
#define CSR_BT_PAC_APP_SIM1_COMBINED_CALLS_KEY              0x18

#define CSR_BT_PAC_APP_VCARD_FORMAT_KEY                     0x19
#define CSR_BT_PAC_APP_ORDER_KEY                            0x20
#define CSR_BT_PAC_APP_SEARCH_KEY                           0x21
#define CSR_BT_PAC_APP_PROPERTY_SELECTOR_KEY                0x22
#define CSR_BT_PAC_APP_MAX_LIST_COUNT_KEY                   0x23
#define CSR_BT_PAC_APP_RESET_NEW_MISSED_CALLS_KEY           0x24
#define CSR_BT_PAC_APP_VCARD_FILTER_KEY                     0x25
#define CSR_BT_PAC_APP_SAVE_CHANGES_KEY                     0x26
#define CSR_BT_PAC_APP_SRMP_KEY                             0x27
#define CSR_BT_PAC_APP_SET_TO_DEFAULT_KEY                   0x28

#define CSR_BT_PAC_APP_PROPERTY_KEY                         0x29
#define CSR_BT_PAC_APP_NAME_KEY                             0x30
#define CSR_BT_PAC_APP_NUMBER_KEY                           0x31
#define CSR_BT_PAC_APP_SOUND_KEY                            0x32
#define CSR_BT_PAC_APP_VALUE_KEY                            0x33
#define CSR_BT_PAC_APP_ENTER_VALUE_KEY                      0x34
#define CSR_BT_PAC_APP_ENTER_COUNT_KEY                      0x35

#define CSR_BT_PAC_APP_N_KEY                                0x36
#define CSR_BT_PAC_APP_VERSION_KEY                          0x37
#define CSR_BT_PAC_APP_IMAGE_KEY                            0x38
#define CSR_BT_PAC_APP_ADDRESS_KEY                          0x39
#define CSR_BT_PAC_APP_TELNO_KEY                            0x40
#define CSR_BT_PAC_APP_EMAIL_KEY                            0x41
#define CSR_BT_PAC_APP_TITLE_KEY                            0x42
#define CSR_BT_PAC_APP_ROLE_KEY                             0x43
#define CSR_BT_PAC_APP_ORG_KEY                              0x44
#define CSR_BT_PAC_APP_NICKNAME_KEY                         0x45
#define CSR_BT_PAC_APP_UID_KEY                              0x46
#define CSR_BT_PAC_APP_UCI_KEY                              0x47

#define CSR_BT_PAC_APP_VCARD_SELECTOR_KEY                   0x48
#define CSR_BT_PAC_APP_VCARD_SELECTOR_OPERATOR_KEY          0x49

#define CSR_BT_PAC_APP_DOWNLOAD_KEY                         0x50
#define CSR_BT_PAC_APP_BROWSE_KEY                           0x51

#define CSR_BT_PAC_APP_N_FILTER_KEY                         0x52
#define CSR_BT_PAC_APP_VERSION_FILTER_KEY                   0x53
#define CSR_BT_PAC_APP_IMAGE_FILTER_KEY                     0x54
#define CSR_BT_PAC_APP_ADDRESS_FILTER_KEY                   0x55
#define CSR_BT_PAC_APP_TELNO_FILTER_KEY                     0x56
#define CSR_BT_PAC_APP_EMAIL_FILTER_KEY                     0x57
#define CSR_BT_PAC_APP_TITLE_FILTER_KEY                     0x58
#define CSR_BT_PAC_APP_ROLE_FILTER_KEY                      0x59
#define CSR_BT_PAC_APP_ORG_FILTER_KEY                       0x60
#define CSR_BT_PAC_APP_NICKNAME_FILTER_KEY                  0x61
#define CSR_BT_PAC_APP_UID_FILTER_KEY                       0x62
#define CSR_BT_PAC_APP_UCI_FILTER_KEY                       0x63
#define CSR_BT_PAC_APP_SPEEDDIAL_FILTER_KEY                 0x64
#define CSR_BT_PAC_APP_SPEEDDIALKEY                         0x65
#define CSR_BT_PAC_APP_ENTRY_BY_UID                         0x66
#define CSR_BT_PAC_APP_START_OFFSET_VALUE_KEY               0x67

/* Main Menu TEXT defines */
#define TEXT_PAC_APP_PBAP_CLIENT_UCS2                   CONVERT_TEXT_STRING_2_UCS2("PBAP Client")
#define TEXT_PAC_APP_CONNECT_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Connect")
#define TEXT_PAC_APP_DISCONNECT_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Disconnect")
#define TEXT_PAC_APP_PHONEBOOK_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Phonebook")
#define TEXT_PAC_APP_SETTINGS_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Settings")
#define TEXT_PAC_APP_CONNECTING_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Connecting")
#define TEXT_PAC_APP_DOWNLOADING_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Downloading")
#define TEXT_PAC_APP_DOWNLOAD_COMPLETE_UCS2             CONVERT_TEXT_STRING_2_UCS2("Download Complete")
#define TEXT_PAC_APP_DOWNLOAD_ALL_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Download All")
#define TEXT_PAC_APP_NOT_FOUND_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Not Found")

#define TEXT_PAC_APP_TELECOM_UCS2                       CONVERT_TEXT_STRING_2_UCS2("telecom")
#define TEXT_PAC_APP_SIM1_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Sim1")

#define TEXT_PAC_APP_ALL_CONTACTS_UCS2                  CONVERT_TEXT_STRING_2_UCS2("All Contacts")
#define TEXT_PAC_APP_MISSED_CALLS_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Missed Calls")
#define TEXT_PAC_APP_INCOMING_CALLS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Incoming Calls")
#define TEXT_PAC_APP_OUTGOING_CALLS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Outgoing Calls")
#define TEXT_PAC_APP_COMBINED_CALLS_UCS2                CONVERT_TEXT_STRING_2_UCS2("Combined Calls")
#define TEXT_PAC_APP_SPEED_DIAL_UCS2                    CONVERT_TEXT_STRING_2_UCS2("Speed Dial Entries")
#define TEXT_PAC_APP_FAVORITES_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Favorites")

#define TEXT_PAC_APP_VCARD_FORMAT_UCS2                  CONVERT_TEXT_STRING_2_UCS2("vCard Format")
#define TEXT_PAC_FORMAT_VCARD2_1_UCS2                   CONVERT_TEXT_STRING_2_UCS2("vCard2.1")
#define TEXT_PAC_FORMAT_VCARD3_0_UCS2                   CONVERT_TEXT_STRING_2_UCS2("vCard3.0")

#define TEXT_PAC_APP_ORDER_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Order")
#define TEXT_PAC_APP_ORDER_ALPHABETICAL_UCS2            CONVERT_TEXT_STRING_2_UCS2("Alphabetical")
#define TEXT_PAC_APP_ORDER_INDEXED_UCS2                 CONVERT_TEXT_STRING_2_UCS2("Indexed")
#define TEXT_PAC_APP_ORDER_PHONETICAL_UCS2              CONVERT_TEXT_STRING_2_UCS2("Phonetical")

#define TEXT_PAC_APP_SEARCH_UCS2                        CONVERT_TEXT_STRING_2_UCS2("Search")
#define TEXT_PAC_APP_PROPERTY_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Property")
#define TEXT_PAC_APP_VALUE_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Value")

#define TEXT_PAC_APP_PROPERTY_SELECTOR_UCS2             CONVERT_TEXT_STRING_2_UCS2("Property Selector")
#define TEXT_PAC_APP_MAX_LIST_COUNT_UCS2                CONVERT_TEXT_STRING_2_UCS2("Max List Count")
#define TEXT_PAC_APP_START_OFFSET_VALUE_UCS2            CONVERT_TEXT_STRING_2_UCS2("Start Offset Value")
#define TEXT_PAC_APP_RESET_NEW_MISSED_CALLS_UCS2        CONVERT_TEXT_STRING_2_UCS2("Reset New Missed Calls")
#define TEXT_PAC_APP_VCARD_FILTER_UCS2                  CONVERT_TEXT_STRING_2_UCS2("vCard Filter")
#define TEXT_PAC_APP_SRMP_UCS2                          CONVERT_TEXT_STRING_2_UCS2("SRMP")
#define TEXT_PAC_APP_SET_TO_DEFAULT_UCS2                CONVERT_TEXT_STRING_2_UCS2("Set to Default")

#define TEXT_PAC_APP_PROPERTY_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Property")
#define TEXT_PAC_APP_NAME_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Name")
#define TEXT_PAC_APP_NUMBER_UCS2                        CONVERT_TEXT_STRING_2_UCS2("Number")
#define TEXT_PAC_APP_SOUND_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Sound")
#define TEXT_PAC_APP_VALUE_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Value")
#define TEXT_PAC_APP_ENTER_VALUE_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Enter Value")
#define TEXT_PAC_APP_ENTER_COUNT_UCS2                   CONVERT_TEXT_STRING_2_UCS2("Enter Count")

#define TEXT_PAC_APP_IMAGE_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Image")
#define TEXT_PAC_APP_VERSION_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Version")
#define TEXT_PAC_APP_ADDRESS_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Address")
#define TEXT_PAC_APP_TELNO_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Tel No")
#define TEXT_PAC_APP_EMAIL_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Email")
#define TEXT_PAC_APP_TITLE_UCS2                         CONVERT_TEXT_STRING_2_UCS2("Title")
#define TEXT_PAC_APP_ROLE_UCS2                          CONVERT_TEXT_STRING_2_UCS2("Role")
#define TEXT_PAC_APP_ORG_UCS2                           CONVERT_TEXT_STRING_2_UCS2("Organization")
#define TEXT_PAC_APP_NICKNAME_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Nickname")
#define TEXT_PAC_APP_UID_UCS2                           CONVERT_TEXT_STRING_2_UCS2("UID")
#define TEXT_PAC_APP_UCI_UCS2                           CONVERT_TEXT_STRING_2_UCS2("UCI")
#define TEXT_PAC_APP_SPEEDDIAL_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Speed Dial")

#define TEXT_PAC_APP_VCARD_SELECTOR_UCS2                CONVERT_TEXT_STRING_2_UCS2("vCard Selector")
#define TEXT_PAC_APP_VCARD_SELECTOR_OPERATOR_UCS2       CONVERT_TEXT_STRING_2_UCS2("vCard Selector Operator")

#define TEXT_PAC_APP_AND_UCS2                           CONVERT_TEXT_STRING_2_UCS2("AND")
#define TEXT_PAC_APP_OR_UCS2                            CONVERT_TEXT_STRING_2_UCS2("OR")

#define TEXT_PAC_APP_ON_UCS2                            CONVERT_TEXT_STRING_2_UCS2("On")
#define TEXT_PAC_APP_OFF_UCS2                           CONVERT_TEXT_STRING_2_UCS2("Off")

#define TEXT_PAC_APP_ENABLED_UCS2                       CONVERT_TEXT_STRING_2_UCS2("Enabled")
#define TEXT_PAC_APP_DISABLED_UCS2                      CONVERT_TEXT_STRING_2_UCS2("Disabled")
#define TEXT_PAC_APP_DOWNLOAD_PHONEBOOK_UCS2            CONVERT_TEXT_STRING_2_UCS2("Download Phonebook")
#define TEXT_PAC_APP_BROWSE_PHONEBOOK_UCS2              CONVERT_TEXT_STRING_2_UCS2("Browse Phonebook")
#define TEXT_PAC_APP_SAVE_CHANGES_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Save Changes")

#define TEXT_PAC_APP_ENTER_PASSWORD_UCS2                CONVERT_TEXT_STRING_2_UCS2("Enter Password")
#define TEXT_PAC_APP_ENTER_USERID_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Enter User ID")
#define TEXT_PAC_APP_AUTHENTICATION_FAILED_UCS2         CONVERT_TEXT_STRING_2_UCS2("Authentication Failed")
#define TEXT_PAC_APP_ENTRY_BY_UID_UCS2                  CONVERT_TEXT_STRING_2_UCS2("Entry by UID Download")
#define TEXT_PAC_APP_ENTER_UID_UCS2                     CONVERT_TEXT_STRING_2_UCS2("Enter UID")
#define TEXT_PAC_APP_DOWNLOAD_FAILED_UCS2               CONVERT_TEXT_STRING_2_UCS2("Download Failed")

#endif /* CSR_BT_PAC_STRING_H__ */

