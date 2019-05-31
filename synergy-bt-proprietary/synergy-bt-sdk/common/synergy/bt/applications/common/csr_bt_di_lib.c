/****************************************************************************

Copyright (c) 2009 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include "csr_bt_app.h"

void CmnDiPrintServiceRecordV13(CsrBtSdDiServiceRecordV13Struct *v13)
{
    printf ("Specification ID valid: %i\n", v13->specificationIdValid);
    if (TRUE == v13->specificationIdValid)
    {
        printf ("Specification ID value: 0x%02x\n", v13->specificationIdValue);
    }

    printf ("Vendor ID valid: %i\n", v13->vendorIdValid);
    if (TRUE == v13->vendorIdValid)
    {
        printf ("Vendor ID value: %i\n", v13->vendorIdValue);
    }

    printf ("Product ID valid: %i\n", v13->productIdValid);
    if (TRUE == v13->productIdValid)
    {
        printf ("Product ID value: %i\n", v13->productIdValue);
    }

    printf ("Version valid: %i\n", v13->versionValid);
    if (TRUE == v13->versionValid)
    {
        printf ("Version value: %i\n", v13->versionValue);
    }

    printf ("Primary Record valid: %i\n", v13->primaryRecordValid);
    if (TRUE == v13->primaryRecordValid)
    {
        printf ("Primary Record value: %i\n", v13->primaryRecordValue);
    }

    printf ("Vendor ID Source valid: %i\n", v13->vendorIdSourceValid);
    if (TRUE == v13->vendorIdSourceValid)
    {
        printf ("Vendor ID Source value: %i\n", v13->vendorIdSourceValue);
    }

    printf ("Client Executable Url valid: %i\n",
            v13->clientExecutableUrlValid);
    if (TRUE == v13->clientExecutableUrlValid)
    {
        CsrUint8 x;
        printf ("Client Executable Url value:");
        for (x=0; x<v13->clientExecutableUrlValueLen; x++)
        {
            printf("%c", *(v13->clientExecutableUrlValue+x));
        }
        printf("\n");
    }

    printf ("Service Description valid: %i\n", v13->serviceDescriptionValid);
    if (TRUE == v13->serviceDescriptionValid)
    {
        CsrUint8 x;
        printf ("Vendor ID Source value:");
        for (x=0; x<v13->serviceDescriptionValueLen; x++)
        {
            printf("%c", *(v13->serviceDescriptionValue+x));
        }
        printf("\n");
    }

    printf ("Documentation Url valid: %i\n", v13->documentationUrlValid);
    if (TRUE == v13->documentationUrlValid)
    {
        CsrUint8 x;
        printf ("Documentation Url value:");
        for (x=0; x<v13->documentationUrlValueLen; x++)
        {
            printf("%c", *(v13->documentationUrlValue+x));
        }
        printf("\n");
    }

    printf("\n");
}

void CmnPrintDataArrayInHex(CsrUint8  *data_p, CsrUint16  dataLen)
{
    CsrUint16 x;
    printf("Length %i", dataLen);
    for (x=0; x<dataLen; x++)
    {
        printf("0x%02x, ", data_p[x]);
    }
    printf("\n");
}
