/****************************************************************************

Copyright (c) 2010-2013 Qualcomm Technologies International, Ltd. 
All Rights Reserved. 
Qualcomm Technologies International, Ltd. Confidential and Proprietary.

REVISION:      $Revision: #1 $
****************************************************************************/

#include "csr_synergy.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "csr_bt_aac_parser.h"
#include "csr_app_lib.h"
#include "csr_bt_ui_strings.h"
#include "csr_bt_av2.h"
#include "csr_bt_file.h"

#define MUX_CONFIG_PRESENT (TRUE)
#define MAX_STREAM_COUNT   (256)
#define MAX_LAY            (8)
#define MAX_PROG           (16)
#define MAX_CHNK           (16)
#define IN_BUF_SIZE        (1024*128)

/* #define DEBUG */

/* Structure to hold an input area of data from the file */
/* We use this to limit file I/O by reading larger chunks of data at once */
typedef struct {
    int           framebits;
    int           inputIndex;
    int           inputLen;
    unsigned long cword;
    int           nbits;
    CsrUint32      readBits;
    CsrUint8       area[IN_BUF_SIZE];
} InputArea;
static InputArea *inputArea;

/*Structure for the ADIF header if file is in ADIF AAC format */
typedef struct {
    CsrUint16 syncWord;
    char         adifID[5];
    char         layer;
    int             copyIDPresent;
    char         copyID[10];
    int             originalCopy;
    int             home;
    int             bitstreamType;
    long         bitrate;
    int             numPCE;
    long         adifBufferFullness;
    int             progTags[(1<<4)];
} ADIFHeader;

static ADIFHeader *adifHeader;

typedef struct{
    /* fixed */
    CsrBool       ID;
    CsrUint8      layer;
    CsrBool       protectionAbsent;
    CsrUint8      profile;
    CsrUint8      samplingFrequencyIndex;
    CsrBool       privateBit;
    CsrUint8      channelConfiguration;
    CsrBool       original;
    CsrBool       home;
    /* variable */
    CsrBool       copyrightIDBit;
    CsrBool       copyrightIDStart;
    CsrUint16     frameLength;
    CsrUint16     adtsBufferFullness;
    CsrUint8      numRawBlocksInFrame;
    CsrUint16     crcCheck;
} ADTSHeader;
static ADTSHeader *adtsHeader;

typedef struct {
    char        *filename;
    FILE        *file;
    CsrUint32     fileLength;
    CsrBool       adifHeaderPresent;
} FileConfig;
static FileConfig *fileConfig;

typedef struct {
    CsrUint8      audioObjectType;
    CsrUint8      samplingFrequencyIndex;
    CsrUint32     samplingFrequency;
    CsrUint8      channelConfiguration;
    CsrUint8      extensionSamplingFrequencyIndex;
    CsrUint32     extensionSamplingFrequency;
    CsrUint8      extensionAudioObjectType;
    CsrBool       frameLengthFlag;
    CsrBool       dependsOnCoreCoder;
    CsrUint16     coreDecoderDelay;
    CsrBool       extensionFlag;
    CsrBool       extensionFlag3;                     /* only to be used in Version3 and above */
    CsrUint8      layerNr;
    int          numOfSubFrame;
    CsrUint16     layerLength;
    CsrBool       aacSectionDataResilienceFlag;
    CsrBool       aacScalefactorDataResilienceFlag;
    CsrBool       aacSpectralDataResilienceFlag;
    CsrUint8      epConfig;
    CsrBool       directMapping;
} AudioConfig;
static AudioConfig *audioConfig;

/* Structure for an Element List (ised in Program Config Element - PCE)*/
typedef struct {
    int          numElements;
    int          elementIsCPE[(1<<4)];
    int          elementTag[(1<<4)];
} EleList;

/* Mixdown structure to use in Program Config Element */
typedef struct {
    int          present;
    int          elementTag;
    int          pseudoSurroundEnable;
} MIXdown;

/* The Program Config Element structure. This data is read from the file */
typedef struct {
    CsrUint8      elementInstanceTag;
    CsrUint8      objectType;
    int          samplingFrequencyIndex;
    EleList      front;
    EleList      side;
    EleList      back;
    EleList      lfe;
    EleList      data;
    EleList      coupling;
    MIXdown      monoMix;
    MIXdown      stereoMix;
    MIXdown      matrixMix;
    char         comments[(1<<8)+1];

} ProgConfig;
static ProgConfig *progConfig;

typedef struct {

    CsrUint8      audioMuxVersion;
    CsrUint8      audioMuxVersionA;
    CsrBool       otherDataPresent;
    CsrUint16     otherDataLenBits;
    CsrBool      *otherData;
    CsrUint32     taraBufferFullness;
    CsrBool       allStreamsSameTimeFraming;
    int          numSubFrames;
    CsrUint8      numProgram;
    CsrUint8      numLayer;
    CsrUint8      numChunk;
    CsrBool       useSameConfig;
    CsrUint8      frameLengthType[MAX_PROG*MAX_LAY];
    CsrUint16     frameLength[MAX_PROG*MAX_LAY];
    CsrUint8      latmBufferFullness[MAX_PROG*MAX_LAY];
    CsrUint8      audioObjectType[MAX_LAY];
    CsrUint8      CELPframeLengthTableIndex[MAX_PROG*MAX_LAY];
    CsrUint8      HVXCframeLengthTableIndex[MAX_PROG*MAX_LAY];
    CsrUint8      streamID[MAX_PROG][MAX_LAY];
    CsrUint8      muxSlotLengthBytes[MAX_PROG*MAX_LAY];
    CsrUint8      muxSlotLengthCoded[MAX_PROG*MAX_LAY];
    CsrUint8      layChunkIndex[MAX_PROG*MAX_LAY];
    CsrUint8      progChunkIndex[MAX_PROG*MAX_LAY];
    CsrUint8      layStreamIndex[MAX_PROG*MAX_LAY];
    CsrUint8      progStreamIndex[MAX_PROG*MAX_LAY];
    CsrBool       auEndFlag[MAX_PROG*MAX_LAY];
    CsrUint8      streamIndex;
    CsrUint8      coreFrameOffset;
    CsrBool       crcCheckPresent;
    CsrUint8      bytesForValue;

} MuxConfig;
static MuxConfig *muxConfig;

typedef struct{
    CsrUint16     payloadSize;                /* framesize excl. headers */
    CsrUint8     *data;                       /* pointer to latest payload data */
    CsrBool       inFragment;
} PayloadConfig;
static PayloadConfig *payloadConfig;

/*PRIVATE */
long     GetBits(int n);
long     GetShort();
int      DoByteAlign(void);
int      ReadInputFile();
void     ReadInputArea();
void     GetElementList(EleList *p, CsrBool enable_cpe);
void     GetProgramConfigElement();
CsrUint8  GetAudioObjectType();
CsrUint16 GetBitsToDecode();
void     GetDSTSpecificConfig();
void     GetMPEG12SpecificConfig();
void     GetSSCSpecificConfig();
void     GetParametricSpecificConfig();
void     GetErrorResilientHvxcSpecificConfig();
void     GetErrorResilientCelpSpecificConfig();
void     GetErrorProtectionSpecificConfig();
void     GetStructuredAudioSpecificConfig();
void     GetTTSSpecificConfig();
void     GetHvxcSpecificConfig();
void     GetCelpSpecificConfig();
void     GetGASSpecificConfig(CsrUint8 samplingFrequencyIndex,CsrUint8 channelConfiguration,CsrUint8 audioObjectType);
CsrUint16 GetAudioSpecificConfig(CsrUint8 lay);
CsrUint32 GetLatmValue();

CsrUint8* GetPayloadMux();
void     GetPayloadLengthInfo();
CsrBool   GetStreamMuxConfig();
CsrUint8* GetAdifAudioMuxElement(CsrBool muxConfigPresent);
CsrUint8* GetAdtsAudioMuxElement(CsrBool muxConfigPresent);
void     GetAdtsErrorCheck();
void     GetAdtsVariableHeader();
void     GetAdtsFixedHeader();
int      GetAdifHeader();
void     dumpADIFHeader();

CsrBool AACInitParser(char* file)
{
    char adif_id[5];
    int i;
    CsrBool result = TRUE;
    /* init global structures */
    inputArea     = (InputArea*)CsrPmemZalloc(sizeof(InputArea));
    fileConfig    = (FileConfig*)CsrPmemZalloc(sizeof(FileConfig));
    muxConfig     = (MuxConfig*)CsrPmemZalloc(sizeof(MuxConfig));
    CsrMemSet(muxConfig,0,sizeof(MuxConfig));
    adtsHeader    = (ADTSHeader*)CsrPmemZalloc(sizeof(ADTSHeader));
    payloadConfig = (PayloadConfig*)CsrPmemZalloc(sizeof(PayloadConfig));
    progConfig    = (ProgConfig*)CsrPmemZalloc(sizeof(ProgConfig));
    audioConfig    = (AudioConfig*)CsrPmemZalloc(sizeof(AudioConfig));
    inputArea->framebits       = 0;
    inputArea->inputIndex      = 0;
    inputArea->inputLen        = 0;
    inputArea->nbits           = 0;
    payloadConfig->payloadSize = 0;
    payloadConfig->data        = NULL;
    payloadConfig->inFragment  = FALSE;
    muxConfig->numSubFrames    = -1; /*important*/
    if(file != NULL)
    {
        fileConfig->filename = file;
        fileConfig->file = CsrBtFopen(file, "rb");
        if(fileConfig->file != NULL)
        {
            fseek(fileConfig->file, 0, SEEK_END);
            fileConfig->fileLength = ftell(fileConfig->file);
            fseek(fileConfig->file, 0, SEEK_SET);
            /* Check if this is a ADIF formatted file (it has an ADIF header) */
            for (i=0; i<4; i++)
            {
                adif_id[i] = fgetc(fileConfig->file);
            }
            adif_id[4] = '\0';
            if (CsrStrNCmp(adif_id, "ADIF", 4) == 0)
            {
                fileConfig->adifHeaderPresent = 1;
            }
            else
            {
                fileConfig->adifHeaderPresent = 0;
            }
            /* reset filepointer completely */
            inputArea->cword = 0;
            inputArea->nbits = 0;
            inputArea->readBits =0;
            fseek(fileConfig->file, 0, SEEK_SET);

            /* read startblock */
            if (fileConfig->adifHeaderPresent)
            {

                if (GetAdifHeader() < 0)
                {
                    fileConfig->adifHeaderPresent = 0;
                }
                else
                {
                    fileConfig->adifHeaderPresent = 1;
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"We Currently do not support ADIF files, bailing out\n");
                    result = FALSE;


                }
            }
        }
        else
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"Can't open file: '%s'\n", fileConfig->filename);
            result = FALSE;
        }
    }
    return result;
}

CsrBool AACDeInitParser()
{

    if(fileConfig->file != NULL)
    {
        CsrBtFclose(fileConfig->file);
    }
    CsrPmemFree(inputArea);
    CsrPmemFree(fileConfig);
    CsrPmemFree(progConfig);
    CsrPmemFree(muxConfig);
    CsrPmemFree(adtsHeader);
    CsrPmemFree(adifHeader);
    CsrPmemFree(payloadConfig);
    return TRUE;

}


/*
 * get the adif header from the file and store it in instance data
 */
int GetAdifHeader()
{
    int i, n,  select_status;
    ADIFHeader *h;

    /* we only allocate when we actually need this ADIF header*/
    adifHeader = (ADIFHeader*)CsrPmemAlloc(sizeof(ADIFHeader));
    h = adifHeader;


    /* adif header */
    for (i=0; i<4; i++)
    {
        h->adifID[i] = (char)GetBits(8);
    }
    h->adifID[i] = 0;        /* null terminated string */
    /* read copyright string */
    if ((h->copyIDPresent = GetBits(1)) == 1)
    {

        for (i=0; i<9; i++)
        {
            h->copyID[i] = (char)GetBits(8);
        }
        h->copyID[i] = 0;  /* null terminated string */
    }
    h->originalCopy = GetBits(1);
    h->home = GetBits(1);
    h->bitstreamType = GetBits(1);
    h->bitrate = GetBits(23);

    /* read program config elements */
    select_status = -1;
    h->numPCE = GetBits(4);
    n = h->numPCE + 1;


    for (i=0; i<n; i++)
    {
        if(h->bitstreamType == 0)
        {
            h->adifBufferFullness = GetBits(20);
        }
        else
        {
            h->adifBufferFullness = 0;
        }

        GetProgramConfigElement();
        select_status=1;

    }
#ifdef DEBUG
    dumpADIFHeader();
#endif
    return select_status;
}


void GetAdtsFixedHeader()
{
    /* allocate structure if this  is the first run */
    if(adtsHeader == NULL)
    {
        adtsHeader = CsrPmemZalloc(sizeof(ADTSHeader));
    }
    adtsHeader->ID                     = (CsrUint8)GetBits(1);
    adtsHeader->layer                  = (CsrUint8)GetBits(2);
    adtsHeader->protectionAbsent       = (CsrUint8)GetBits(1);
    adtsHeader->profile                = (CsrUint8)GetBits(2);
    adtsHeader->samplingFrequencyIndex = (CsrUint8)GetBits(4);
    adtsHeader->privateBit             = (CsrUint8)GetBits(1);
    adtsHeader->channelConfiguration   = (CsrUint8)GetBits(3);
    adtsHeader->original               = (CsrUint8)GetBits(1);
    adtsHeader->home                   = (CsrUint8)GetBits(1);

}

void GetAdtsVariableHeader()
{

        adtsHeader->copyrightIDBit     = (CsrUint8)GetBits(1);
        adtsHeader->copyrightIDStart   = (CsrUint8)GetBits(1);
        adtsHeader->frameLength        = (CsrUint16)GetBits(13);
        adtsHeader->adtsBufferFullness  = (CsrUint16)GetBits(11);
        adtsHeader->numRawBlocksInFrame= (CsrUint8)GetBits(2);
}

void GetAdtsErrorCheck()
{
    if(adtsHeader->protectionAbsent == 0)
    {
        adtsHeader->crcCheck = (CsrUint16)GetBits(16);
    }
}

void dumpAdtsHeader()
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n==============================================\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"FIXED HEADER\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  ID                     : %d\n",adtsHeader->ID);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Layer                  : %d\n",adtsHeader->layer);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  ProtectionAbsent       : %d\n",adtsHeader->protectionAbsent);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Profile                : %d\n",adtsHeader->profile);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  SamplingFrequencyIndex : %d\n",adtsHeader->samplingFrequencyIndex);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  PrivateBit             : %d\n",adtsHeader->privateBit);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  ChannelConfiguration   : %d\n",adtsHeader->channelConfiguration);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Original               : %d\n",adtsHeader->original);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Home                   : %d\n",adtsHeader->home);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"VARIABLE HEADER\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  CopyrightIDBit         : %d\n",adtsHeader->copyrightIDBit);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  CopyrightIDStart       : %d\n",adtsHeader->copyrightIDStart);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  FrameLength            : %d\n",adtsHeader->frameLength);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  AdtsBufferFullness     : %d\n",adtsHeader->adtsBufferFullness);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  NumRawBlocksInFrame    : %d\n",adtsHeader->numRawBlocksInFrame);
    if(!adtsHeader->protectionAbsent)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"CRC                      : %d\n",adtsHeader->crcCheck);
    }
}

void dumpADIFHeader()
{


    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n==============================================\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"FIXED HEADER\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  ID                     : %s\n",adifHeader->adifID);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  CopyIDPresent          : %d\n",adifHeader->copyIDPresent);
    if(adifHeader->copyIDPresent)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  CopyID                 : %s\n",adifHeader->copyID);
    }
    else
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  CopyID                 : NONE\n");
    }
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  OriginalCopy          : %d\n",adifHeader->originalCopy);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  home                   : %d\n",adifHeader->home);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  BitstreamType          : %d\n",adifHeader->bitstreamType);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  Bitrate                : %ld\n",adifHeader->bitrate);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  numPCE                 : %d\n",adifHeader->numPCE);
}

void dumpADIFProgConfig()
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n==============================================\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"PROGRAM CONFIG ELEMENT\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  elementInstanceTag     : %d\n", progConfig->elementInstanceTag);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  objectType             : %d\n", progConfig->objectType);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  samplingFreqIndex      : %d\n", progConfig->samplingFrequencyIndex);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  front                  : %d\n", progConfig->front.numElements);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  side                   : %d\n", progConfig->side.numElements);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  back                   : %d\n", progConfig->back.numElements);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  lfe                    : %d\n", progConfig->lfe.numElements);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  data                   : %d\n", progConfig->data.numElements);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  coupling               : %d\n", progConfig->coupling.numElements);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  monoMix                : %d\n", progConfig->monoMix.present);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  stereoMix              : %d\n", progConfig->stereoMix.present);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  matrixMix              : %d\n", progConfig->matrixMix.present);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  comments               : %s\n",progConfig->comments);


}

void dumpADIFMuxConfig()
{
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"\n==============================================\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"MUX CONFIG\n");
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  AudioMuxVersion        : %d\n",  muxConfig->audioMuxVersion);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  AudioMuxVersionA       : %d\n", muxConfig->audioMuxVersionA);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  otherDataPresent       : %d\n", muxConfig->otherDataPresent);
    if(muxConfig->otherDataPresent)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  otherDataLenBits       : %d\n", muxConfig->otherDataLenBits);
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  otherData              : %s\n", muxConfig->otherData);
    }
    if(muxConfig->audioMuxVersion && muxConfig->audioMuxVersionA)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  taraBufferFullness     : %d\n", muxConfig->taraBufferFullness);
    }
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  allStreamsSameTimeFrame: %d\n", muxConfig->allStreamsSameTimeFraming);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  numSubFrames           : %d\n", muxConfig->numSubFrames);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  numProgram             : %d\n", muxConfig->numProgram);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  numLayer               : %d\n", muxConfig->numLayer);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  useSameConfig          : %d\n", muxConfig->useSameConfig);
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"  CRC Check Present      : %d\n", muxConfig->crcCheckPresent);

}
CsrUint8* GetAdtsAudioMuxElement(CsrBool muxConfigPresent)
{
    CsrUint8 *data = NULL;
    CsrUint32 payloadSize;
    CsrUint32 i;
    CsrUint8 headerSize= 7; /*sync: 12bit, fixed: 16bit, variable: 28bit; */
    if(GetBits(12)== 0xFFF) /* syncword - always 111111111111*/
    {

        GetAdtsFixedHeader();
        GetAdtsVariableHeader();
        GetAdtsErrorCheck();
        DoByteAlign();
#ifdef DEBUG
        dumpAdtsHeader();
#endif

        if(!adtsHeader->protectionAbsent)
        {
            headerSize += 2; /* 16bit CRC */
        }
        payloadSize = adtsHeader->frameLength - headerSize;
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"ADTS payload is          : %dbytes\n",payloadSize);
#endif
        data = (CsrUint8*)CsrPmemZalloc(payloadSize );
        for( i=0; i<payloadSize; i++)
        {
            data[i] = (CsrUint8)GetBits(8);
        }
    }
    else
    {
#ifdef DEBUG
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"End of files reached\n");
#endif
    }
    return data;
}


CsrUint8* GetAdifAudioMuxElement(CsrBool muxConfigPresent)
{
    CsrUint8 *data     = NULL;

    if(muxConfig->numSubFrames < 0){

        if(muxConfigPresent)
        {
            muxConfig->useSameConfig = (CsrUint8)GetBits(1);
            if(!muxConfig->useSameConfig)
            {
                GetStreamMuxConfig();
            }
        }
    }


        if(muxConfig->audioMuxVersionA == 0)
        {

            muxConfig->numSubFrames--;

            GetPayloadLengthInfo();
            /* get actual payload data */
            data = GetPayloadMux();
            if(muxConfig->numSubFrames < 0)
            {
                if(muxConfig->otherDataPresent){

                    muxConfig->otherData = (CsrBool*)CsrPmemZalloc(sizeof(CsrBool)*muxConfig->otherDataLenBits);
                    muxConfig->otherData=(CsrBool*)GetBits(muxConfig->otherDataLenBits);
                    muxConfig->otherData[muxConfig->otherDataLenBits-1] = 0;
                }
            }
        }


    DoByteAlign();
#ifdef DEBUG
    dumpADIFMuxConfig();
#endif

    return data;
}



/* Reads the Mux config and adds it to the Mux config struture */
CsrBool GetStreamMuxConfig()
{

    CsrUint8 streamCnt = 0;
    CsrUint8 lay       = 0;
    CsrUint8 prog      = 0;

    muxConfig->audioMuxVersion = (CsrUint8)GetBits(1);

    if(muxConfig->audioMuxVersion)
    {
        muxConfig->audioMuxVersionA = (CsrUint8)GetBits(1);
    }
    else
    {
        muxConfig->audioMuxVersionA = 0;
    }
    if (!muxConfig->audioMuxVersionA)
    {
        if( muxConfig->audioMuxVersion)
        {
            muxConfig->taraBufferFullness = GetLatmValue();
        }
        muxConfig->allStreamsSameTimeFraming = (CsrBool)GetBits(1);
        muxConfig->numSubFrames              = (int)GetBits(6);
        muxConfig->numProgram                = (CsrUint8)GetBits(4);


        for( prog = 0; prog <= muxConfig->numProgram; prog++ )
        {
            muxConfig->numLayer = (CsrUint8)GetBits(3);
            for (lay = 0; lay <= muxConfig->numLayer; lay++)
            {
                muxConfig->progStreamIndex[streamCnt]= prog;
                muxConfig->layStreamIndex[streamCnt] = lay;
                muxConfig->streamID[prog][lay] = streamCnt++;
                if(!prog && !lay)
                {
                    muxConfig->useSameConfig = FALSE;
                }
                else
                {
                    muxConfig->useSameConfig = (CsrBool)GetBits(1);
                }
                if(!muxConfig->useSameConfig)
                {
                    if(muxConfig->audioMuxVersion ==0)
                    {
                        GetAudioSpecificConfig(lay);
                    }
                    else
                    {
                        int ascLen;
                        ascLen = GetLatmValue();
                        ascLen -= GetAudioSpecificConfig(lay);
                        if(ascLen > 0)
                        {
                            GetBits((CsrUint32)ascLen); /* read FillBits */
                        }
                    }
                }

                muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(3);

                if (muxConfig->frameLengthType[muxConfig->streamID[prog][lay]]== 0)
                {

                    muxConfig->latmBufferFullness[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(8);
                    if(!muxConfig->allStreamsSameTimeFraming)
                    {
                        CsrUint8 tmpLay = lay-1;
                        if(lay==0)
                        {
                            tmpLay = 0;
                        }

                        if((muxConfig->audioObjectType[lay]== 6 ||
                            muxConfig->audioObjectType[lay]== 20) &&
                           (muxConfig->audioObjectType[tmpLay]== 8 ||
                            muxConfig->audioObjectType[tmpLay]== 24))
                        {
                            muxConfig->coreFrameOffset = (CsrUint8)GetBits(6);
                        }
                        else
                        {
                            muxConfig->coreFrameOffset = 0;
                        }
                    }
                }
                else
                {
                    if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 1)
                    {

                        muxConfig->frameLength[muxConfig->streamID[prog][lay]]=(CsrUint16)GetBits(9);
                    }
                    else
                    {
                        if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 4 ||
                           muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 5 ||
                           muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 3)
                        {

                            muxConfig->CELPframeLengthTableIndex[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(6);
                        }
                        else
                        {
                            if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 6 ||
                               muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 7)
                            {

                                muxConfig->HVXCframeLengthTableIndex[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(1);
                            }
                        }
                    }
                }
            }


        }


    }
    muxConfig->otherDataPresent = (CsrBool)GetBits(1);
    if(muxConfig->otherDataPresent)
    {
        if(muxConfig->audioMuxVersion==1)
        {
            muxConfig->otherDataLenBits = (CsrUint16)GetLatmValue();
        }
        else
        {
            CsrBool otherDataLenEsc = TRUE;
            CsrUint8 otherDataLenTmp = 0;
            muxConfig->otherDataLenBits = 0;
            do{
                muxConfig->otherDataLenBits *= 256; /* 2^8*/
                otherDataLenEsc = (CsrBool)GetBits(1);
                otherDataLenTmp = (CsrUint8)GetBits(8);
                muxConfig->otherDataLenBits+=otherDataLenTmp;

            }while(otherDataLenEsc);
        }
    }
    muxConfig->crcCheckPresent = (CsrBool)GetBits(1);

#ifdef DEBUG
        dumpADIFMuxConfig();
#endif


    if(muxConfig->crcCheckPresent)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"ERROR CRC check is not implemented\n");
        return FALSE;
    }
    return TRUE;

}


void GetPayloadLengthInfo()
{
    CsrUint8 tmp  = 0;
    CsrUint8 prog = 0;
    CsrUint8 lay  = 0;
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"GetPayloadLengthInfo\n");

    if(muxConfig->allStreamsSameTimeFraming)
    {
        muxConfig->numChunk    = 0;
        muxConfig->streamIndex = 0;
        for(prog = 0; prog<=muxConfig->numProgram; prog++)
        {

            for(lay = 0; lay<=muxConfig->numLayer; lay++)
            {
                if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 0)
                {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"GetPayloadLengthInfo2\n");
                   muxConfig-> muxSlotLengthBytes[muxConfig->streamID[prog][lay]]=0;
                    do {
                        tmp = (CsrUint8)GetBits(8);
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"*****************%d    %d************\n",muxConfig->muxSlotLengthBytes[muxConfig->streamID[prog][lay]],tmp);
                        muxConfig->muxSlotLengthBytes[muxConfig->streamID[prog][lay]] += tmp;
                    } while(tmp == 255);

                }
                else
                {
                        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"GetPayloadLengthInfo3\n");
                    if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 5 ||
                       muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 7 ||
                       muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 3)
                    {
                        muxConfig->muxSlotLengthCoded[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(2);
                    }

                }
            }
        }

    }
    else
    {
        CsrUint8 chunkCnt = 0;

        CsrUint8 tmp = 0;
        muxConfig->numChunk = (CsrUint8)GetBits(4);

        for(chunkCnt = 0; chunkCnt<muxConfig->numChunk; chunkCnt++ )
        {
            muxConfig->streamIndex = (CsrUint8)GetBits(4);
            muxConfig->progChunkIndex[chunkCnt] = muxConfig->progStreamIndex[muxConfig->streamIndex];
            prog = muxConfig->progChunkIndex[chunkCnt];
            muxConfig->layChunkIndex[chunkCnt] = muxConfig->layStreamIndex[muxConfig->streamIndex];
            lay = muxConfig->layChunkIndex[chunkCnt];

            if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]]==0)
            {
                muxConfig->muxSlotLengthBytes[muxConfig->streamID[prog][lay]]=0;
                do{
                    tmp = (CsrUint8)GetBits(8);
                    muxConfig->muxSlotLengthBytes[muxConfig->streamID[prog][lay]] += tmp;
                } while(tmp == 255);
                muxConfig->auEndFlag[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(1);
            }
            else
            {
                if(muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 5 ||
                   muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 7 ||
                   muxConfig->frameLengthType[muxConfig->streamID[prog][lay]] == 3)
                {
                    muxConfig->muxSlotLengthCoded[muxConfig->streamID[prog][lay]] = (CsrUint8)GetBits(2);
                }
            }
        }
    }

}

/* remember that the returned data should be freed by the user of the lib */
CsrUint8* GetPayloadMux()
{
    CsrUint8 prog;
    CsrUint8 lay;
    CsrUint16 chunkCnt;
    CsrUint8* payload = NULL;
    CsrUint32 i=0;
    CsrUint16 size=0;

    if(muxConfig->allStreamsSameTimeFraming)
    {
        for(prog=0;prog<=muxConfig->numProgram; prog++)
        {
            for(lay=0; lay <= muxConfig->numLayer; lay++)
            {
                size +=  muxConfig->muxSlotLengthBytes[muxConfig->streamID[prog][lay]];
            }
        }

    }
    else
    {
        for(chunkCnt=0; chunkCnt <= muxConfig->numChunk; chunkCnt++)
        {
            prog  = muxConfig->progChunkIndex[chunkCnt];
            lay   = muxConfig->layChunkIndex[chunkCnt];
            size +=   muxConfig->muxSlotLengthCoded[muxConfig->streamID[prog][lay]];
        }
    }

    payloadConfig->payloadSize = size;
    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"#################### SIZE er %d ################\n",size);
    if(size >0)
    {
        payload = (CsrUint8*)CsrPmemAlloc(size);
        for (i=0; i <size; i++)
        {
            payload[i] = (CsrUint8)GetBits(8);
        }
    }
    return payload;
}



CsrUint32 GetLatmValue()
{
    CsrUint32 value   = 0;
    int     i        = 0;
    CsrUint8 valuetmp = 0;

    muxConfig->bytesForValue = (CsrUint8)GetBits(2);
    for(i = 0; i <= muxConfig->bytesForValue; i++)
    {
        value   *= 256;  /* 2^8 */
        valuetmp = (CsrUint8)GetBits(8);
        value   += valuetmp;
    }
    return value;
}


 /* return number of bits read */
CsrUint16 GetAudioSpecificConfig(CsrUint8 lay)
{
    int bits           = 0;
    CsrUint32 tmpbits   = inputArea->readBits;

    audioConfig->audioObjectType = GetAudioObjectType();
    muxConfig->audioObjectType[lay] = audioConfig->audioObjectType;
    audioConfig->samplingFrequencyIndex = (CsrUint8)GetBits(4);

    if(audioConfig->samplingFrequencyIndex == 0xf)
    {
        audioConfig->samplingFrequency = GetBits(24);
    }
    audioConfig->channelConfiguration = (CsrUint8)GetBits(4);

    if(audioConfig->audioObjectType == 5)
    {
        audioConfig->extensionAudioObjectType = audioConfig->audioObjectType;
        audioConfig->extensionSamplingFrequencyIndex = (CsrUint8)GetBits(4);
        if(audioConfig->extensionSamplingFrequencyIndex == 0xf)
        {
            audioConfig->extensionSamplingFrequency = GetBits(24);
        }
        audioConfig->audioObjectType = GetAudioObjectType();
        muxConfig->audioObjectType[lay] = audioConfig->audioObjectType;
    }
    else
    {
        audioConfig->extensionAudioObjectType = 0;
    }
    switch(audioConfig->audioObjectType)
    {
        case 1:  /*AAC MAIN*/
        case 2:  /*AAC LC */
        case 3:  /*AAC SSR */
        case 4:  /*AAC LTP */
        case 6:  /*AAC Scalable*/
        case 7:  /*AAC TwinVQ */
        case 17: /*ER AAC LC*/
        case 19: /*ER AAC LTP*/
        case 20: /*ER AAC Scalable */
        case 21: /*ER TwinVQ*/
        case 22: /*ER BSAC */
        case 23: /*ER AAC LD*/
            GetGASSpecificConfig(audioConfig->samplingFrequencyIndex,
                                 audioConfig->channelConfiguration,
                                 audioConfig->audioObjectType);
            break;
        case 8:  /*CELP*/
            GetCelpSpecificConfig();
            break;
        case 9:  /*HVXC*/
            GetHvxcSpecificConfig();
            break;
        case 12: /*TTSI*/
            GetTTSSpecificConfig();
            break;
        case 13: /*Main Synth*/
        case 14: /*Wave Synth*/
        case 15: /*General Midi*/
        case 16: /*Algo Synth and Audio FX*/
            GetStructuredAudioSpecificConfig();
            break;
        case 24: /*ER CELP*/
            GetErrorResilientCelpSpecificConfig();
            break;
        case 25: /*ER HVXC*/
            GetErrorResilientHvxcSpecificConfig();
            break;
        case 26: /*ER HILN*/
        case 27: /*ER Parametric*/
            GetParametricSpecificConfig();
            break;
        case 28: /*SSC*/
            GetSSCSpecificConfig();
            break;
        case 32: /*Layer-1*/
        case 33: /*Layer-2*/
        case 34: /*Layer-3*/
            GetMPEG12SpecificConfig();
            break;
        case 35: /*DST*/
            GetDSTSpecificConfig();
            break;
        default:
            /*reserved*/
            break;
    }
    switch(audioConfig->audioObjectType)
    {
        case 17: /*ER AAC LC*/
        case 19: /*ER AAC LTP*/
        case 20: /*ER AAC Scalable*/
        case 21: /*ER TwinVQ*/
        case 22: /*ER BSAC*/
        case 23: /*ER AAC LD*/
        case 24: /*ER CELP*/
        case 25: /*ER HVXC*/
        case 26: /*ER HILN*/
        case 27: /*ER Parametric*/
            audioConfig->epConfig = (CsrUint8)GetBits(2);
            if (audioConfig->epConfig == 2 ||audioConfig->epConfig == 3){
                GetErrorProtectionSpecificConfig();
            }

            if(audioConfig->epConfig == 3)
            {
                audioConfig->directMapping = (CsrBool)GetBits(1);
                if(!audioConfig->directMapping)
                {
                    CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"ERROR: ErrorProtection without direct mapping not implemented\n");
                }
            }
            break;
    }
    if(audioConfig->extensionAudioObjectType == 5 && GetBitsToDecode() >=16)
    {
        CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"ERROR, GetBitsToDecode() is not implemented\n");
    }

    bits = inputArea->readBits - tmpbits;
    return bits;
}

void  GetGASSpecificConfig(CsrUint8 samplingFrequencyIndex,CsrUint8 channelConfiguration,CsrUint8 audioObjectType)
{
    audioConfig->frameLengthFlag = (CsrBool)GetBits(1);
    audioConfig->dependsOnCoreCoder = (CsrBool)GetBits(1);
    if(audioConfig->dependsOnCoreCoder)
    {
        audioConfig->coreDecoderDelay = (CsrUint16)GetBits(14);
    }
    audioConfig->extensionFlag = (CsrBool)GetBits(1);

    if(!channelConfiguration)
    {
       GetProgramConfigElement();

    }
    if((audioObjectType == 6)||(audioObjectType == 20))
    {
        audioConfig->layerNr = (CsrUint8)GetBits(3);
    }
    if(audioConfig->extensionFlag)
    {
        if(audioObjectType == 22)
        {
            audioConfig->numOfSubFrame = (CsrUint8)GetBits(5);
            audioConfig->layerLength = (CsrUint16)GetBits(11);
        }
        if(audioObjectType == 17 ||
           audioObjectType == 19 ||
           audioObjectType == 20 ||
           audioObjectType == 23)
        {
            audioConfig->aacSectionDataResilienceFlag = (CsrBool)GetBits(1);
            audioConfig->aacScalefactorDataResilienceFlag = (CsrBool)GetBits(1);
            audioConfig->aacSpectralDataResilienceFlag = (CsrBool)GetBits(1);
        }
        audioConfig->extensionFlag3 = (CsrBool)GetBits(1);
        if(audioConfig->extensionFlag3)
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"ERROR, Version 3 not supported\n");
        }
    }

}

void GetCelpSpecificConfig()
{
    /* TBA */
}

void  GetHvxcSpecificConfig()
{
    /* TBA */
}

void  GetTTSSpecificConfig()
{
    /* TBA */
}

void GetStructuredAudioSpecificConfig()
{
    /* TBA */
}
void GetErrorResilientCelpSpecificConfig()
{
     /* TBA */
}
void GetErrorResilientHvxcSpecificConfig()
{
    /* TBA */
}
void GetErrorProtectionSpecificConfig()
{
    /* TBA */
}
void GetParametricSpecificConfig()
{
     /* TBA */
}
void GetSSCSpecificConfig()
{
     /* TBA */
}
void GetMPEG12SpecificConfig()
{
    /* TBA */
}
void GetDSTSpecificConfig()
{
       /* TBA */
}

CsrUint16 GetBitsToDecode()
{
    /* TBA */
    return 0;
}


CsrUint8 GetAudioObjectType()
{
    CsrUint8 type;
    CsrUint8 eType;
    type = (CsrUint8)GetBits(5);
    if(type==31)
    {
        eType = (CsrUint8)GetBits(6);
        type = 32+eType;
    }
    return type;
}



void GetProgramConfigElement()
{
    int i, j;

    progConfig->elementInstanceTag     = (CsrUint8)GetBits(4);
    progConfig->objectType             = (CsrUint8)GetBits(2);
    progConfig->samplingFrequencyIndex = (CsrUint8)GetBits(4);
    progConfig->front.numElements      = (CsrUint8)GetBits(4);
    progConfig->side.numElements       = (CsrUint8)GetBits(4);
    progConfig->back.numElements       = (CsrUint8)GetBits(4);
    progConfig->lfe.numElements        = (CsrUint8)GetBits(2);
    progConfig->data.numElements       = (CsrUint8)GetBits(3);
    progConfig->coupling.numElements   = (CsrUint8)GetBits(4);
    if ((progConfig->monoMix.present = (CsrUint8)GetBits(1)) == 1)
    {
        progConfig->monoMix.elementTag = (CsrUint8)GetBits(4);
    }
    if ((progConfig->stereoMix.present = (CsrUint8)GetBits(1)) == 1)
    {
        progConfig->stereoMix.elementTag = (CsrUint8)GetBits(4);
    }
    if ((progConfig->matrixMix.present = (CsrUint8)GetBits(1)) == 1)
    {
        progConfig->matrixMix.elementTag = (CsrUint8)GetBits(2);
        progConfig->matrixMix.pseudoSurroundEnable = (CsrUint8)GetBits(1);
    }
    GetElementList(&progConfig->front, 1);
    GetElementList(&progConfig->side, 1);
    GetElementList(&progConfig->back, 1);
    GetElementList(&progConfig->lfe, 0);
    GetElementList(&progConfig->data, 0);
    GetElementList(&progConfig->coupling, 1);

    DoByteAlign();
    j = GetBits(8);
    for (i=0; i<j; i++)
    {
        progConfig->comments[i] = (char)GetBits(4);
    }
    progConfig->comments[i] = 0;    /* null terminator for string */
    dumpADIFProgConfig();
}


void GetElementList(EleList *p, CsrBool enable_cpe)
{
    int i, j;
    for (i=0, j=p->numElements; i<j; i++)
    {
        if (enable_cpe)
        {
            p->elementIsCPE[i] = GetBits(1);

        }
        else
        {
            p->elementIsCPE[i] = 0; /* sdb */
        }
        p->elementTag[i] = GetBits(4);

    }

}

/* read data from the file into the inputArea */
void ReadInputArea()
{

    inputArea->inputIndex = 0;
    inputArea->inputLen = fread(inputArea->area, 1, IN_BUF_SIZE, fileConfig->file);
}


/* read next part of the file. fill up inputArea if needed */
int ReadInputFile()
{
    if (inputArea->inputIndex == inputArea->inputLen)
    {
        ReadInputArea();

        if (inputArea->inputLen == 0)
        {
            return -1;
        }
    }
    return inputArea->area[inputArea->inputIndex++];
}

/* byte align data in cword. return number of bytes aligned */
int DoByteAlign(void)
{
    int i=0;

    if(inputArea->nbits <= 16){
        inputArea->cword = (inputArea->cword<<16) | GetShort();
        inputArea->nbits += 16;
    }

    while (inputArea->nbits & ((1<<3)-1)) {
        inputArea->nbits -= 1;
        i += 1;
    }
    return(i);
}

/* get next short from input file/inputArea and put it in return it*/
long GetShort()
{
    int c1, c2;
    CsrBool err = FALSE;
    if ((c1 = ReadInputFile()) < 0)
    {
        err = TRUE;
    }
    if ((c2 = ReadInputFile()) < 0)
    {
        err = TRUE;
    }
    if(err)
    {
        return 0;
    }
    else
    {
        inputArea->framebits += 16;
        return (c1<<8) | c2;
    }

}
/* get N next bits from cword */
long GetBits(int n)
{
    inputArea->readBits +=n;
    if(inputArea->nbits <= 16)
    {
        inputArea->cword = (inputArea->cword<<16) | GetShort();
        inputArea->nbits += 16;
    }
    inputArea->nbits -= n;
    return (inputArea->cword>>inputArea->nbits) & ((1<<n)-1);
}



CsrBool AACGetNextFrame(CsrUint8 **frame)
{

    CsrPmemFree(*frame);
    /* reset bit counter */
    inputArea->readBits =0;

    /* Get an adif frame */
    if(fileConfig->adifHeaderPresent)
    {
        *frame = GetAdifAudioMuxElement(1);
    }
    else   /*Get an ADTS frame instead */
    {
       *frame = GetAdtsAudioMuxElement(1);

    }
    /* store pointer to payload*/
    payloadConfig->data = *frame;
    /* Did we actually get a frame? */
    if(*frame == NULL)
    {
        return FALSE;
    }

    return TRUE;
}



CsrUint16 AACGetLatmPacket(void **packetIn, CsrUint16 maxSize, CsrTime *duration){
    CsrUint8    *payloadPtr   = NULL;
    CsrUint8    *headerPtr    = NULL;
    CsrUint16    payloadSize  = 0;
    CsrUint8    *frame        = NULL;
    CsrUint16    headerSize   = 9;
    CsrUint16    usedSize     = 0;
    CsrUint8     numSubFrames = 0;
    CsrTime        dura         = 0;

    /* cleanup dirty input*/
    if(*packetIn != NULL)
    {
        CsrPmemFree(*packetIn);
        *packetIn = NULL;
    }
    /* if this the first time we call this function */
    if(payloadConfig->data == NULL)
    {
        /* if no frame available return 0 size */
        if(!AACGetNextFrame(&frame))
        {
            CsrAppBacklogReqSend(TECH_BT, PROFILE_AV, FALSE,"no more frames left - returning 0 length!\n");
            return 0;
        }
    }

    payloadSize = AACGetFrameLength();


    if(payloadSize + 12 >= maxSize)
    {
        maxSize = payloadSize +12;

    }
   *packetIn  = (CsrUint8*)CsrPmemZalloc(maxSize);
    CsrMemSet(*packetIn, 0,maxSize);
    headerPtr = (CsrUint8*)*packetIn;


    /*set variables */
    if(fileConfig->adifHeaderPresent)
    {


        headerPtr[0] |= (0<<7);      /* useSameStreamMux          1bit */
        headerPtr[0] |= (1<<6);      /* audioMuxVersion           1bit */
        headerPtr[0] |= (0<<5);      /* audioMuxVersionA          1bit */
        headerPtr[0] |= (0<<3);      /* BytesForValue             2bit */
        headerPtr[0] |= (0x07);      /* taraBF first part         3bit */

        headerPtr[1] |= (0xFC);      /* taraBufferFullness        5bit */
        headerPtr[1] |= (1<<2);      /* allStreamsSameTimeFraming 1bit */
        headerPtr[1] |= (0);         /* numSubframes first part   2bit */
        headerPtr[2] |= (0<<4);      /* numSubframes              4bit */
        headerPtr[2] |= (0);         /* numProgram                4bit */
        headerPtr[3] |= (0<<5);      /* numLayer                  3bit */
        headerPtr[3] |= (0<<3);      /* BytesForValue             2bit */
        headerPtr[3] |= (0);         /* LSB from padding size     3bit */
        headerPtr[4] |= (22<<3);      /* padding+ASC size in bits  5bit */
        /* AudioSpecificConfig from here and down */
        headerPtr[5] |=              /* objectType                5bit */
            (adtsHeader->profile+1)<< 6;
        headerPtr[5] |=              /* samplingFrequencyIndex    4bit */
            (adtsHeader->samplingFrequencyIndex) << 2;

        headerPtr[6] |=              /* channelConfiguration      4bit */
            (adtsHeader->channelConfiguration) << 6;
        headerPtr[6] |= (0<<5);      /* frameLengthFlag           1bit */
        headerPtr[6] |= (0<<4);      /* Dependsoncorecoder        1bit */
        headerPtr[6] |= (0<<3);      /* ExtensionFlag             1bit */
        headerPtr[7] |= (0<<5);      /* Padding                   6bit */
        headerPtr[7] |= (0<<2);      /* FrameLengthType           3bit */
        headerPtr[7] |= (0x3);       /* LSB from latmBF size      2bit */

        headerPtr[8] |= ((0x3F) << 2);          /* latmBufferFullness  MSB   6bit */
        headerPtr[8] |= (0 << 1);               /* otherDataPresent          1bit */
        headerPtr[8] |= (0);                    /* crcCheckPresent           1bit */
        payloadPtr = &(headerPtr[9]);

    }
    else
    {
        headerPtr[0] |= (0<<7);      /* useSameStreamMux          1bit */
        headerPtr[0] |= (1<<6);      /* audioMuxVersion           1bit */
        headerPtr[0] |= (0<<5);      /* audioMuxVersionA          1bit */
        headerPtr[0] |= (0<<3);      /* BytesForValue             2bit */
        headerPtr[0] |= (0x07);      /* taraBF first part         3bit */

        headerPtr[1] |= (0xFC);      /* taraBufferFullness        5bit */
        headerPtr[1] |= (1<<2);      /* allStreamsSameTimeFraming 1bit */
        headerPtr[1] |= (0);         /* numSubframes first part   2bit */
        headerPtr[2] |= (0<<4);      /* numSubframes              4bit */
        headerPtr[2] |= (0);         /* numProgram                4bit */
        headerPtr[3] |= (0<<5);      /* numLayer                  3bit */
        headerPtr[3] |= (0<<3);      /* BytesForValue             2bit */
        headerPtr[3] |= (0);         /* LSB from padding size     3bit */

        headerPtr[4] |= (22<<3);      /* padding+ASC size in bits  5bit */
        /* AudioSpecificConfig from here and down */
        headerPtr[5] |=              /* objectType                5bit */
            (adtsHeader->profile+1)<< 6;
        headerPtr[5] |=              /* samplingFrequencyIndex    4bit */
            (adtsHeader->samplingFrequencyIndex) << 2;

        headerPtr[6] |=              /* channelConfiguration      4bit */
            (adtsHeader->channelConfiguration) << 6;
        headerPtr[6] |= (0<<5);      /* frameLengthFlag           1bit */
        headerPtr[6] |= (0<<4);      /* Dependsoncorecoder        1bit */
        headerPtr[6] |= (0<<3);      /* ExtensionFlag             1bit */
        headerPtr[7] |= (0<<5);      /* Padding                   6bit */
        headerPtr[7] |= (0<<2);      /* FrameLengthType           3bit */
        headerPtr[7] |= (0x3);       /* LSB from latmBF size      2bit */

        headerPtr[8] |= ((0x3F) << 2);          /* latmBufferFullness  MSB   6bit */
        headerPtr[8] |= (0 << 1);               /* otherDataPresent          1bit */
        headerPtr[8] |= (0);                    /* crcCheckPresent           1bit */
        payloadPtr = &(headerPtr[9]);
    }
        /* payloadLengthInfo from here and down */
        do {
            numSubFrames++;
            /*should add 0xFF for each payloadSize/255*/
            if(payloadSize>=255)
            {
                *payloadPtr = (0xFF);              /* payload size (255bytes)   8 bit*/
                payloadPtr++;
                *payloadPtr = (CsrUint8)(payloadSize%255); /* last part of payloadLengthInfo 8 bit*/
                headerSize +=2;
                payloadPtr++;
            }
            else
            {
                *payloadPtr = (CsrUint8)payloadSize;
                payloadPtr++;
                headerSize++;
            }
            /* copy frame data to payload*/
            CsrMemCpy(payloadPtr, payloadConfig->data, payloadSize);
            if(payloadConfig->data)
            {
                CsrPmemFree(payloadConfig->data);
                frame =NULL;
            }
            usedSize += payloadSize;
            dura += AACGetDuration();
            if(!AACGetNextFrame(&frame))
            {
                break;
            }
            payloadPtr +=payloadSize;              /* move payloadPtr over payload */
            payloadSize = AACGetFrameLength();



        }while(( usedSize + payloadSize + headerSize + 2)<maxSize);
        /* add payloadLengthInfo header sizes to used size */
        usedSize += headerSize;



    /*update numSubFrames */
    headerPtr[1] |= ((numSubFrames-1)&0x30)>>4;    /* numSubframes first part   2bit */
    headerPtr[2] |= (((numSubFrames-1)&0x0F)<<4);  /* numSubframes              4bit */

    *duration = dura;
    return usedSize;
}


/* Public Function to get Bit Rate */
CsrUint32 AACGetBitRate()
{
    if(fileConfig->adifHeaderPresent)
    {
        return adifHeader->bitrate;
    }
    else
    {
        return 0;
    }

}

CsrUint8 AACGetObjectType()
{
    if(fileConfig->adifHeaderPresent)
    {
/*         if(a) /\* This is MPEG-2 *\/ */
/*         { */
            return 0; /* special case for MPEG-2 */
/*         } */
/*         else */
/*         { */
/*             return audioConfig->audioObjectType; */
/*         } */
    }
    else
    {
        if(adtsHeader->ID) /* This is MPEG-2 */
        {
            return 0; /* special case for MPEG-2 */
        }
        else
        {
            return adtsHeader->profile + 1;
        }
    }
}

CsrUint16 AACGetFrameLength()
{
    if(fileConfig->adifHeaderPresent)
    {

        return payloadConfig->payloadSize;
    }
    else
    {
        CsrUint8 header = 7;
        if(!adtsHeader->protectionAbsent)
        {
            header +=2;
        }
        return adtsHeader->frameLength - header;
    }
}

/* Public Function to get channels */
CsrUint8 AACGetChannels(){
    if(fileConfig->adifHeaderPresent)
    {
        return audioConfig->channelConfiguration;
    }
    else
    {
        return adtsHeader->channelConfiguration;
    }
}
/* public function that returns duration of current frame */
CsrTime AACGetDuration()
{
    int sampleRate = AACGetSampleRate();

    CsrTime duration = ((1024*1000*1000)/sampleRate);

    return duration;
}

/* public function to get sampleRate*/
int AACGetSampleRate(){
    static int sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};

    if(fileConfig->adifHeaderPresent)
    {
        return sample_rates[progConfig->samplingFrequencyIndex];
    }
    else
    {
        return sample_rates[adtsHeader->samplingFrequencyIndex];
    }
}
