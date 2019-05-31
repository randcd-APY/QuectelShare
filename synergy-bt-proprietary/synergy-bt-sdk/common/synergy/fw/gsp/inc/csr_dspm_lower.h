#ifndef CSR_DSPM_LOWER_H__
#define CSR_DSPM_LOWER_H__
/*****************************************************************************

Copyright (c) 2011-2016 Qualcomm Technologies International, Ltd.
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

*****************************************************************************/

#include "csr_synergy.h"
#include "csr_types.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************

    NAME
        CsrDspmConfigAcquireFunction, CsrDspmConfigAcquireFunctionRegister

    DESCRIPTION
        This callback function can optionally be registered to allow DSP
        Manager configuration data blocks to be passed to the chip. These
        blocks are numbered consecutively, starting at 0.

        To register the callback, call CsrDspmConfigAcquireFunctionRegister
        with a pointer to a function of the type CsrDspmConfigAcquireFunction.

        Please note that the registered callback function may be called any
        number of times during normal operation, even with the same arguments
        multiple times.

    PARAMETERS (CsrDspmConfigAcquireFunction only)
        buildId - The build ID reported by the chip. The implementation can use
            this for identifying the correct configuration data.
        configBlockIndex - The index of the requested configuration data block.
        configBlockCount [out] - The total number of configuration data blocks
            available. The DSP Manager will (but may choose not to) repeatedly
            call the function to retrieve all the available blocks. Shall be
            set to 0 if no configuration data is available for the specified
            buildId.
        configBlockLength [out] - The total length (in bytes) of the data
            returned including the 16bit block count field. Shall be set to 0
            if no configuration data is available at the specified configBlock
            index.

    RETURNS (CsrDspmConfigAcquireFunction only)
        Pointer to the configuration data block. Will be passed to CsrPmemFree
        after use. Shall be NULL if no configuration data is available at the
        specified configBlockIndex. The first 16bit of the data shall contain
        the total number of configuration blocks (using little endian
        encoding), and the actual configuration data shall follow immediately
        after.

*****************************************************************************/
typedef CsrUint8 *(*CsrDspmConfigAcquireFunction)(
    CsrUint16 buildId,
    CsrUint16 configBlockIndex,
    CsrUint16 *configBlockCount,
    CsrUint16 *configBlockLength);
void CsrDspmConfigAcquireFunctionRegister(CsrDspmConfigAcquireFunction configAcquireFunction);


/*****************************************************************************

    NAME
        CsrDspmOperatorPatchAcquireFunction,
        CsrDspmOperatorPatchAcquireFunctionRegister

    DESCRIPTION
        This callback function can optionally be registered to allow patching
        of operators as they are created.

        An operator patch set is specific to a chip build ID and capability and
        consists of a number of patch items and a skip count.

        To register the callback, call
        CsrDspmOperatorPatchAcquireFunctionRegister with a pointer to a
        function of the type CsrDspmOperatorPatchAcquireFunction.

        The registered function can be used by DSPM in two distinct ways:

        1) If the itemIndex argument is 0, the itemCount and skipCount
        arguments will be valid pointers and shall be updated with the values
        appropriate to the specified build ID and capability and the function
        shall return NULL. In this case, the itemLength will be a NULL pointer
        and should not be touched. This use is solely for retrieving the item
        count and skip count.

        2) The second use is for retrieving patch items. In this case the
        itemIndex will be between 1 and the previously retrieved itemCount,
        and the function shall return the corresponding patch item and update
        the itemLength output parameter value. In this case the itemCount and
        skipCount will be NULL pointers and should not be touched, and
        itemLength will be a valid pointer. If the function is called with an
        itemIndex greater than the previously retrieved itemCount, the
        function shall return NULL and set itemLength to 0.

        Please note that the registered callback function may be called any
        number of times during normal operation, even with the same arguments
        multiple times.

    PARAMETERS (CsrDspmOperatorPatchAcquireFunction only)
        buildId - The build ID reported by the chip. The implementation can use
            this for identifying the correct patch items.
        capability - The capability index. The implementation can use this for
            identifying the correct patch items.
        itemIndex - The index of the patch item to acquire. If 0, no patch item
            shall be returned (returns NULL), but the itemCount, skipCount and
            itemLength shall be set accordingly (see above). When not 0, valid
            range is between 1 and itemCount.
        itemCount [out] - The number of patch items for the specified build ID
            and capability.
        skipCount [out] - The skip count for the selected set of patch items.
        itemLength [out] - The total length (in byte) of the data
            returned, including the 16bit length field.

    RETURNS (CsrDspmOperatorPatchAcquireFunction only)
        Pointer to the patch item. Will be passed to CsrPmemFree after use.
        Shall be NULL if the specified itemIndex is greater than the actual
        item count. The first 16bit of the data shall contain the length of
        the patch item in byte, not including the length field (using little
        endian encoding), and the actual configuration data shall follow
        immediately after.

*****************************************************************************/
typedef CsrUint8 *(*CsrDspmOperatorPatchAcquireFunction)(
    CsrUint16 buildId,
    CsrUint16 capability,
    CsrUint16 itemIndex,
    CsrUint16 *itemCount,
    CsrUint16 *skipCount,
    CsrUint16 *itemLength);
void CsrDspmOperatorPatchAcquireFunctionRegister(CsrDspmOperatorPatchAcquireFunction operatorPatchAcquireFunction);


/* New DSP Patching format common for Framework and Operator Patches*/


typedef struct
{
    /*The build ID reported by the chip. The implementation can use this for
    identifying the correct configuration data*/
    CsrUint16 buildId;
    /*capability descriptor Index inside a Patch configuration file*/
    CsrUint16 capabilityId;
    /*Index of the datagram to be fetched inside capability data*/
    CsrUint16 datagramIdx;
}CsrDspmPatchDatagramReq;


typedef struct
{
    /* actual datagram length */
    CsrUint16 datagramLen;
    /*pointer to the patch datagram. In addition to the actual datagram
    length,the application should allocate 2 extra bytes(datagramLen+2) for
    the Header Field. These first 2 Header bytes of the datagram pointer is
    for synergy Frw DSPM's use and the actual patch data retrieved should
    follow immediately after. */
    CsrUint8 *datagram;
}CsrDspmPatchDatagram;

typedef struct
{
    /*number of distinct skipable datagrams for this capability*/
    CsrUint16 totalSkipableDatagrams;
    /*number of distinct non-skipable datagrams for this capability*/
    CsrUint16 totalNonSkipableDatagrams;
}CsrDspmCapDesc;


/*****************************************************************************

    NAME
        csrDspmAcquireCapDatagram

    DESCRIPTION
        This Callback will be used to get CsrDspmCapDesc(Capability descriptor
        details) from Synergy Application.The Synergy Application needs to fill
        CsrDspmCapDesc structure for the capabilityId requested

    PARAMETERS
        buildId - The build ID reported by the chip. The implementation can use
        this for identifying the correct Patch data
        capabilityId - capability descriptor Index
        capabilityDesc [out] - Refer to CsrDspmCapDesc structure

    RETURNS
    None

*****************************************************************************/

typedef void (*csrDspmAcquireCapabilityDescs)(
    CsrUint16 buildId,
    CsrUint16 capabilityId,
    CsrDspmCapDesc *capabilityDesc);

/*****************************************************************************

    NAME
        csrDspmAcquireCapDatagram

    DESCRIPTION
        This function is called by Synergy Frw DSPM to retreive patch
        datagram from the registered Synergy application.
        During a callback from Synergy Frw DSPM,the Synergy application
        should do the following

            1) Retrieve the patch datagram corresponding to the buildID,
            capabilityId,and datagramIdx mentioned in the
            'CsrDspmPatchDatagramReq' structure

            2) Allocate memory for (datagramLen + 2 Header bytes)
            and fill the patch data from the 3rd byte(i.e. datagram[2])
            leaving the first 2 bytes empty for Header.

            Fill the patchData(CsrDspmPatchDatagram)with
                a) datagramlen - actual datagram length
                b) datagram - pointer to the patch datagram.
                NOTE: datagram[0] and datagram[1] are Header bytes
                datagram[2] is where the patch data starts as explained
                above in CsrDspmPatchDatagram structure.
    PARAMETERS
        patchDatagramReq - Refer to CsrDspmPatchDatagramReq
        patchData [out] - Refer to CsrDspmPatchDatagram

    RETURNS
    None

*****************************************************************************/

typedef void (*csrDspmAcquireCapDatagram)(
    CsrDspmPatchDatagramReq *patchDatagramReq,
    CsrDspmPatchDatagram *patchData);

/*****************************************************************************

    NAME
        CsrDspmPatchAcquireFunctionRegister

    DESCRIPTION
        This function is used to register DSP Patching callbacks with
        the Synergy Framework DSPM.Synergy Applications should register
        these callbacks before triggering a DSPM patch download.
    PARAMETERS
        PatchAcquireFunction - Refer to csrDspmAcquireCapDatagram
        CapabilityDescAcquire - Refer to csrDspmAcquireCapabilityDescs

    RETURNS
    None

*****************************************************************************/
void CsrDspmPatchAcquireFunctionRegister(
    csrDspmAcquireCapDatagram PatchAcquireFunction,
    csrDspmAcquireCapabilityDescs CapabilityDescAcquire);


#ifdef __cplusplus
}
#endif

#endif
