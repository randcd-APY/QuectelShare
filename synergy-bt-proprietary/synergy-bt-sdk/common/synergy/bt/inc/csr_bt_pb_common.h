#ifndef CSR_BT_PB_COMMON_H_
#define CSR_BT_PB_COMMON_H_

/******************************************************************************

Copyright (c) 2014-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
******************************************************************************/

#include "csr_unicode.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PBAP features */
#define CSR_BT_PB_FEATURE_DOWNLOAD      (1 << 0) /* Download                     */
#define CSR_BT_PB_FEATURE_BROWSING      (1 << 1) /* Browsing                     */
#define CSR_BT_PB_FEATURE_DATABASE_ID   (1 << 2) /* Database Identifier          */
#define CSR_BT_PB_FEATURE_FOLDER_VER    (1 << 3) /* Folder Version Counters      */
#define CSR_BT_PB_FEATURE_VCARD_SEL     (1 << 4) /* vCard Selecting              */
#define CSR_BT_PB_FEATURE_MISSED_CALL   (1 << 5) /* Enhanced Missed Calls        */
#define CSR_BT_PB_FEATURE_UCI           (1 << 6) /* X-BT-UCI vCard Property      */
#define CSR_BT_PB_FEATURE_UID           (1 << 7) /* X-BT-UID vCard Property      */
#define CSR_BT_PB_FEATURE_CONTACT_REF   (1 << 8) /* Contact Referencing          */
#define CSR_BT_PB_FEATURE_CONTACT_IMG   (1 << 9) /* Default Contact Image Format */

/* All the features supported by PBAP server and client */
#define CSR_BT_PB_FEATURE_ALL           (CSR_BT_PB_FEATURE_DOWNLOAD     |\
                                         CSR_BT_PB_FEATURE_BROWSING     |\
                                         CSR_BT_PB_FEATURE_DATABASE_ID  |\
                                         CSR_BT_PB_FEATURE_FOLDER_VER   |\
                                         CSR_BT_PB_FEATURE_VCARD_SEL    |\
                                         CSR_BT_PB_FEATURE_MISSED_CALL  |\
                                         CSR_BT_PB_FEATURE_UCI          |\
                                         CSR_BT_PB_FEATURE_UID          |\
                                         CSR_BT_PB_FEATURE_CONTACT_REF  |\
                                         CSR_BT_PB_FEATURE_CONTACT_IMG)

/* PBAP repositories */
#define CSR_BT_PB_REPO_NONE             (0x00)  /* Supported repositories not known */
#define CSR_BT_PB_REPO_PHONE            (0x01)  /* Local Phonebook folders */
#define CSR_BT_PB_REPO_SIM              (0x02)  /* SIM Card folders */
#define CSR_BT_PB_REPO_SPD              (0x04)  /* Speed dial folder */
#define CSR_BT_PB_REPO_FAV              (0x08)  /* Favourites folder */

/* All the repositories supported by PBAP server and client */
#define CSR_BT_PB_REPOSITORY_ALL        (CSR_BT_PB_REPO_PHONE   |\
                                         CSR_BT_PB_REPO_SIM     |\
                                         CSR_BT_PB_REPO_SPD     |\
                                         CSR_BT_PB_REPO_FAV     )

/* PBAP vCard format types */
#define CSR_BT_PB_FORMAT_VCARD2_1       (0x00)  /* Request for vCard entities in format of vCard 2.1 */
#define CSR_BT_PB_FORMAT_VCARD3_0       (0x01)  /* Request for vCard entities in format of vCard 3.0 */

/* PBAP listing order types */
#define CSR_BT_PB_ORDER_INDEXED         (0x00)  /* Request for vCards in increasing order of vCard handles */
#define CSR_BT_PB_ORDER_ALPHABETICAL    (0x01)  /* Request for vCards in alphabetical order of vCard Name property. */
#define CSR_BT_PB_ORDER_PHONETICAL      (0x02)  /* Request for vCards in order of vCard Sound property. */

/* PBAP search attribute types */
#define CSR_BT_PB_SEARCH_ATT_NAME       (0x00)  /* Search for vCards with matching vCard Name property */
#define CSR_BT_PB_SEARCH_ATT_NUMBER     (0x01)  /* Search for vCards with matching vCard Telephone Number property */
#define CSR_BT_PB_SEARCH_ATT_SOUND      (0x02)  /* Search for vCards with matching vCard Sound property */

/* PBAP vCard selector operators */
#define CSR_BT_PB_VCARD_SELECTOR_OPERATOR_OR        0   /* OR operator. Select a vCard if any of vCard property in vCardSelector is present in it. */
#define CSR_BT_PB_VCARD_SELECTOR_OPERATOR_AND       1   /* AND operator. Select a vCard only if all of vCard property  */

/* Reset new missed calls */
#define CSR_BT_PB_RESET_NEW_MISSED_CALL             1   /* Do not reset new missed calls counter. */
#define CSR_BT_PB_DO_NOT_RESET_NEW_MISSED_CALL      0   /* Reset new missed calls counter */

/* Default maxListCount value */
#define CSR_BT_PB_LIST_COUNT_REQUEST_SIZE           0x0000  /* Use of this value in Pull requests signify that
                                                            application wants to know number of indexes in the
                                                            phonebook of interest.
                                                            PBAP server ignores all other application parameters
                                                            except ResetNewMissedCalls, vCardSelector and
                                                            vCardSelectorOperator; and returns just PhonebookSize
                                                            in Object Transfer response. */
#define CSR_BT_PB_DEFAULT_MAX_LIST_COUNT            0xFFFF  /* This value indicates PBAP server to not restrict
                                                            number of entries. */

/* PBAP strings */
#define CSR_BT_PB_FOLDER_UP_STR         ".."

#define CSR_BT_PB_VCF_EXT               ".vcf"
#define CSR_BT_PB_PATH_SEPARATOR        "/"

#define CSR_BT_PB_FOLDER_ROOT_STR       "root"
#define CSR_BT_PB_FOLDER_TELECOM_STR    "telecom"
#define CSR_BT_PB_FOLDER_SIM1_STR       "SIM1"
#define CSR_BT_PB_FOLDER_PB_STR         "pb"
#define CSR_BT_PB_FOLDER_ICH_STR        "ich"
#define CSR_BT_PB_FOLDER_OCH_STR        "och"
#define CSR_BT_PB_FOLDER_MCH_STR        "mch"
#define CSR_BT_PB_FOLDER_CCH_STR        "cch"
#define CSR_BT_PB_FOLDER_FAV_STR        "fav"
#define CSR_BT_PB_FOLDER_SPD_STR        "spd"

/* "SIM1/telecom" */
#define CSR_BT_PB_FOLDER_SIM1_TELECOM_STR CSR_BT_PB_FOLDER_SIM1_STR \
    CSR_BT_PB_PATH_SEPARATOR                                        \
    CSR_BT_PB_FOLDER_TELECOM_STR

/* Phonebook names */
#define CSR_BT_PB_FOLDER_PB_VCF         CSR_BT_PB_FOLDER_PB_STR CSR_BT_PB_VCF_EXT  /* "pb.vcf" */
#define CSR_BT_PB_FOLDER_ICH_VCF        CSR_BT_PB_FOLDER_ICH_STR CSR_BT_PB_VCF_EXT /* "ich.vcf" */
#define CSR_BT_PB_FOLDER_OCH_VCF        CSR_BT_PB_FOLDER_OCH_STR CSR_BT_PB_VCF_EXT /* "och.vcf" */
#define CSR_BT_PB_FOLDER_MCH_VCF        CSR_BT_PB_FOLDER_MCH_STR CSR_BT_PB_VCF_EXT /* "mch.vcf" */
#define CSR_BT_PB_FOLDER_CCH_VCF        CSR_BT_PB_FOLDER_CCH_STR CSR_BT_PB_VCF_EXT /* "cch.vcf" */
#define CSR_BT_PB_FOLDER_SPD_VCF        CSR_BT_PB_FOLDER_SPD_STR CSR_BT_PB_VCF_EXT /* "fav.vcf" */
#define CSR_BT_PB_FOLDER_FAV_VCF        CSR_BT_PB_FOLDER_FAV_STR CSR_BT_PB_VCF_EXT /* "spd.vcf" */

/* PBAP VCARD properties */
#define CSR_BT_PB_VCARD_PROP_VERSION                (1 << 0) /* vCard Version                        */
#define CSR_BT_PB_VCARD_PROP_FN                     (1 << 1) /* Formatted Name                       */
#define CSR_BT_PB_VCARD_PROP_N                      (1 << 2) /* Structured Presentation of Name      */
#define CSR_BT_PB_VCARD_PROP_PHOTO                  (1 << 3) /* Associated Image or Photo            */
#define CSR_BT_PB_VCARD_PROP_BDAY                   (1 << 4) /* Birthday                             */
#define CSR_BT_PB_VCARD_PROP_ADR                    (1 << 5) /* Delivery Address                     */
#define CSR_BT_PB_VCARD_PROP_LABEL                  (1 << 6) /* Delivery                             */
#define CSR_BT_PB_VCARD_PROP_TEL                    (1 << 7) /* Telephone Number                     */
#define CSR_BT_PB_VCARD_PROP_EMAIL                  (1 << 8) /* Electronic Mail Address              */
#define CSR_BT_PB_VCARD_PROP_MAILER                 (1 << 9) /* Electronic Mail                      */
#define CSR_BT_PB_VCARD_PROP_TZ                     (1 << 10)/* TZ Time Zone                         */
#define CSR_BT_PB_VCARD_PROP_GEO                    (1 << 11)/* Geographic Position                  */
#define CSR_BT_PB_VCARD_PROP_TITLE                  (1 << 12)/* Job                                  */
#define CSR_BT_PB_VCARD_PROP_ROLE                   (1 << 13)/* Role within the Organization         */
#define CSR_BT_PB_VCARD_PROP_LOGO                   (1 << 14)/* Organization Logo                    */
#define CSR_BT_PB_VCARD_PROP_AGENT                  (1 << 15)/* vCard of Person Representing         */
#define CSR_BT_PB_VCARD_PROP_ORG                    (1 << 16)/* Name of Organization                 */
#define CSR_BT_PB_VCARD_PROP_NOTE                   (1 << 17)/* Comments                             */
#define CSR_BT_PB_VCARD_PROP_REV                    (1 << 18)/* Revision                             */
#define CSR_BT_PB_VCARD_PROP_SOUND                  (1 << 19)/* Pronunciation of Name                */
#define CSR_BT_PB_VCARD_PROP_URL                    (1 << 20)/* Uniform Resource Locator             */
#define CSR_BT_PB_VCARD_PROP_UID                    (1 << 21)/* Unique ID                            */
#define CSR_BT_PB_VCARD_PROP_KEY                    (1 << 22)/* Public Encryption Key                */
#define CSR_BT_PB_VCARD_PROP_NICKNAME               (1 << 23)/* Nickname                             */
#define CSR_BT_PB_VCARD_PROP_CATEGORIES             (1 << 24)/* Categories                           */
#define CSR_BT_PB_VCARD_PROP_PROID                  (1 << 25)/* Product ID                           */
#define CSR_BT_PB_VCARD_PROP_CLASS                  (1 << 26)/* Class information                    */
#define CSR_BT_PB_VCARD_PROP_SORT_STRING            (1 << 27)/* String used for sorting operations   */
#define CSR_BT_PB_VCARD_PROP_X_IRMC_CALL_DATETIME   (1 << 28)/* Time stamp                           */
#define CSR_BT_PB_VCARD_PROP_X_BT_SPEEDDIALKEY      (1 << 29)/* Speed-dial shortcut                  */
#define CSR_BT_PB_VCARD_PROP_X_BT_UCI               (1 << 30)/* Uniform Caller Identifier            */
#define CSR_BT_PB_VCARD_PROP_X_BT_UID               (1 << 31)/* Bluetooth Contact Unique Identifier  */

/* Data structure for holding database version info */
typedef struct
{
    CsrUint8    primaryVersionCounter[16];      /* Primary folder version.
                                                Increments on every completion of changes to any of the
                                                properties in the vCards as well as on insertion or
                                                removal of entries */
    CsrUint8    secondaryVersionCounter[16];    /* Secondary folder version.
                                                increment on every completion of changes to the
                                                vCard's N, FN, TEL, EMAIL, MAILER, ADR, X-BT-UCI
                                                properties as well as on insertion or removal of
                                                entries.*/
    CsrUint8    databaseIdentifier[16];         /* Unique database identifier of the PBAP server. */
} CsrBtPbVersionInfo;


/******************************* PBAP Folder IDs ************************************/
/* PBAP Folder IDs provides a simpler mechanism to manage PBAP folders.
 * PBAP folder ID is a 16-bit value combining source and phonebook folder ID.
 *      0-7 bits:   Sources-            root, telecom, sim, sim/telecom
 *      8-15 bits:  Phonebook Folders-  pb, ich, och, mch, cch, spd, fav */

#define CSR_BT_PB_FOLDER_INVALID_ID         0xFFFF /* Invalid PBAP folder ID */

/* Source folder IDs */
#define CSR_BT_PB_FOLDER_ROOT_ID            0x00    /* root */
#define CSR_BT_PB_FOLDER_SIM1_ID            0x01    /* root/SIM1 */
#define CSR_BT_PB_FOLDER_TELECOM_ID         0x02    /* root/telecom */
#define CSR_BT_PB_FOLDER_SIM1_TELECOM_ID    0x03    /* root/SIM1/telecom */

/* Phonebook folder IDs */
#define CSR_BT_PB_PB_ID                 0x01
#define CSR_BT_PB_ICH_ID                0x02
#define CSR_BT_PB_OCH_ID                0x03
#define CSR_BT_PB_MCH_ID                0x04
#define CSR_BT_PB_CCH_ID                0x05
#define CSR_BT_PB_FAV_ID                0x06
#define CSR_BT_PB_SPD_ID                0x07

/* Phonebook PBAP folders IDs */
#define CSR_BT_PB_FOLDER_PHONE_PB_ID    ((CSR_BT_PB_PB_ID  << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/pb  */
#define CSR_BT_PB_FOLDER_PHONE_ICH_ID   ((CSR_BT_PB_ICH_ID << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/ich */
#define CSR_BT_PB_FOLDER_PHONE_OCH_ID   ((CSR_BT_PB_OCH_ID << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/och */
#define CSR_BT_PB_FOLDER_PHONE_MCH_ID   ((CSR_BT_PB_MCH_ID << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/mch */
#define CSR_BT_PB_FOLDER_PHONE_CCH_ID   ((CSR_BT_PB_CCH_ID << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/cch */
#define CSR_BT_PB_FOLDER_PHONE_FAV_ID   ((CSR_BT_PB_FAV_ID << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/fav */
#define CSR_BT_PB_FOLDER_PHONE_SPD_ID   ((CSR_BT_PB_SPD_ID << 8) | CSR_BT_PB_FOLDER_TELECOM_ID) /* root/telecom/spd */

/* SIM1 PBAP folders IDs */
#define CSR_BT_PB_FOLDER_SIM_PB_ID      ((CSR_BT_PB_PB_ID  << 8) | CSR_BT_PB_FOLDER_SIM1_TELECOM_ID) /* root/SIM1/telecom/pb  */
#define CSR_BT_PB_FOLDER_SIM_ICH_ID     ((CSR_BT_PB_ICH_ID << 8) | CSR_BT_PB_FOLDER_SIM1_TELECOM_ID) /* root/SIM1/telecom/ich */
#define CSR_BT_PB_FOLDER_SIM_OCH_ID     ((CSR_BT_PB_OCH_ID << 8) | CSR_BT_PB_FOLDER_SIM1_TELECOM_ID) /* root/SIM1/telecom/och */
#define CSR_BT_PB_FOLDER_SIM_MCH_ID     ((CSR_BT_PB_MCH_ID << 8) | CSR_BT_PB_FOLDER_SIM1_TELECOM_ID) /* root/SIM1/telecom/mch */
#define CSR_BT_PB_FOLDER_SIM_CCH_ID     ((CSR_BT_PB_CCH_ID << 8) | CSR_BT_PB_FOLDER_SIM1_TELECOM_ID) /* root/SIM1/telecom/cch */


/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPbGetFolderId
 *
 *  DESCRIPTION
 *      The Application uses this function to manage extract target PBAP folder
 *      Id. It takes current folder Id and relative path string as input and
 *      returns resultant folder Id when path (string) is followed from current folder.
 *      It is easier for the application to understand and manage paths in terms of
 *      the PBAP folder ID than in terms of UCS2 strings.
 *
 *  PARAMETERS
 *      currentFolder:  Current folder ID.
 *      relativePath:   Relative Path (UCS2 string) from current folder to
 *                      destination folder.
 *                      ".vcf", if present, is neglected.
 *
 *  RETURN
 *      If combination of currentFolder and relativePath is valid, this function
 *      returns destination PBAP folder ID. Otherwise CSR_BT_PB_FOLDER_INVALID_ID
 *      is returned.
 *----------------------------------------------------------------------------*/
CsrUint16 CsrBtPbGetFolderId(CsrUint16 currentFolder,
                             CsrUcs2String *relativePath);

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrBtPbGetFolderStr
 *
 *  DESCRIPTION
 *      This library function generates path string from the PBAP folder IDs.
 *      This function makes it easier to generate the PBAP folder paths to use
 *      with PAC (and PAS) interfaces.
 *
 *  PARAMETERS
 *      currentFolder:  Current folder ID.
 *      targetFoler:    Destination folder ID.
 *      vcf:            If TRUE, append ".vcf" at the end of path
 *
 *  RETURN
 *      If both currentFolder and relativePath is valid PBAP folders, this function
 *      returns relative path (UCS2 string) from currentFolder to targetFolder.
 *      Otherwise a NULL string is returned.
 *----------------------------------------------------------------------------*/
CsrUcs2String *CsrBtPbGetFolderStr(CsrUint16 currentFolder,
                                   CsrUint16 targetFolder,
                                   CsrBool vcf);

#ifdef __cplusplus
}
#endif

#endif /* CSR_BT_PB_COMMON_H_ */
