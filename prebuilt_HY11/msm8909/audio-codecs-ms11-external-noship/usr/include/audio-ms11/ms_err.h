/*
 *        Confidential Information - Limited distribution to authorized persons
 *        only. This material is protected under international copyright laws as
 *        an unpublished work. Do not copy.
 *        Copyright (C) 2010-2011 Dolby Laboratories Inc.
 *        Copyright (C) 2010-2011 Dolby International AB
 *        All rights reserved.
 */


/*! \addtogroup msd */

/*! \file ms_err.h
 *  \brief MultiStreamDecoder error handling header file
 */

#ifndef MS_ERR_H_
#define MS_ERR_H_

#include	<assert.h>

#include "ms_exec.h"

#define		MS_ERR_CHKARG(a)    assert(a)    /*!< Macro used to assert arguments */

/**** Module Error Codes ****/
#define	MS_ERR_INVALID_INPUT_FILE_TYPE            1
#define	MS_ERR_INVALID_INPUT_FILE                 2
#define MS_ERR_INCOMPATIBLE_INPUTS                3
#define	MS_ERR_UNKNOWN_ERRCODE                    4
#define	MS_ERR_INVALID_OUTPUT_FILE_TYPE           5
#define	MS_ERR_INVALID_OUTPUT_FILE                6
#define	MS_ERR_INVALID_DD_OUTPUT_FILE             7
#define	MS_ERR_INVALID_2CH_OUTPUT_FILE            8
#define	MS_ERR_INVALID_DRC_BOOST_FACTOR           9
#define	MS_ERR_INVALID_DRC_CUT_FACTOR            10
#define	MS_ERR_INVALID_DMX_MODE                  11
#define	MS_ERR_INVALID_DRC_MODE                  12
#define	MS_ERR_INVALID_DV_MODE                   13
#define	MS_ERR_INVALID_MS10_MODE                 14
#define	MS_ERR_INVALID_DUALMONO_MODE             15
#define	MS_ERR_INVALID_VERBOSE_MODE              16
#define MS_ERR_INVALID_IDK_TESTMODE_FLAG         17
#define	MS_ERR_INVALID_IDK_DEC_OUT_FILENAME      18
#define MS_ERR_INVALID_IDK_DD_ENC_TEST_FILENAME  19
#define MS_ERR_INVALID_ASSOCIATED_SUBSTREAM      20
#define	MS_ERR_INVALID_DDP_DEBUG_VALUE           21
#define	MS_ERR_INVALID_COMPRESSOR_PROFILE        22
#define	MS_ERR_INVALID_PROGRAM_REF_LEVEL         23
#define MS_ERR_ASSOCIATED_2CH_RESTRICT           24
#define	MS_ERR_INVALID_PULSE_DEBUG_VALUE         25
#define	MS_ERR_INVALID_DIALNORM_VALUE            26
#define	MS_ERR_INVALID_TRANSPORT_FORMAT          27
#define MS_ERR_INVALID_MULTICHANNEL_MODE         28
#define MS_ERR_INVALID_ASSOC_MIXING_VALUE        29
#define MS_ERR_INVALID_USER_BALANCE_VALUE        30
#define MS_ERR_UNSUPPORTED_PARAMETER             31
#define MS_ERR_MISSING_INPUT_MAIN                32
#define MS_ERR_MISSING_OUTPUT_MAIN               33
#define MS_ERR_MISSING_COMMAND_FILE              34
#define MS_ERR_MISSING_ASSOC_2CH_DMX_FILE        35
#define MS_ERR_NOTAPP_ASSOC_2CH_DMX_FILE         36
#define MS_ERR_MISSING_INPUT_ASSOCIATED          37
#define MS_ERR_NOTAPP_DOLBY_VOLUME               38
#define MS_ERR_NOTAPP_MULTICHANNEL_MODE          39
#define MS_ERR_NOTAPP_VIRTUALIZER_OUTPUT         40
#define MS_ERR_OUTOFRANGE_DOLBYVOLUME            41
#define MS_ERR_OUTOFRANGE_DDP_DBG_FRAME          42
#define MS_ERR_OUTOFRANGE_DDP_DBG_DEC            43
#define MS_ERR_OUTOFRANGE_PCM_COMP_PROFILE       44
#define MS_ERR_OUTOFRANGE_PCM_PROG_REF_LEVEL     45
#define MS_ERR_OUTOFRANGE_PULSE_DBG              46
#define MS_ERR_OUTOFRANGE_PULSE_DIALNORM         47
#define MS_ERR_OUTOFRANGE_PULSE_TRANSFMT         48
#define MS_ERR_OUTOFRANGE_DUALMONO               49
#define MS_ERR_OUTOFRANGE_DRC_BOOST              50
#define MS_ERR_OUTOFRANGE_DRC_CUT                51
#define MS_ERR_OUTOFRANGE_ST_DOWNMIX             52
#define MS_ERR_OUTOFRANGE_DRC_MODE               53
#define MS_ERR_OUTOFRANGE_ASSOC_MIX              54
#define MS_ERR_OUTOFRANGE_USER_BALANCE           55
#define MS_ERR_OUTOFRANGE_ASSOCIATED_SUBSTREAM   56
#define MS_ERR_INVALID_DDP_2CH_MODE              57
#define MS_ERR_INVALID_MS10_DMX_TYPE             58

#define MS_ERR_EOT                               -1

/* map newly defined error codes to numbers which are not used yet */
#define MS_DDRE_OPEN_FAILED        DOLBY_DIGITAL_REENCODER_GENERAL_ERROR + 10
#define MS_DDRE_SET_PARAM_FAILED   DOLBY_DIGITAL_REENCODER_GENERAL_ERROR + 11

#define MS_DDT_OPEN_FAILED        DOLBY_PULSE_DECODER_TRANSCODER_UNSUPPORTED_CHANNEL_MODE << 1
#define MS_DDT_SET_PARAM_FAILED   DOLBY_PULSE_DECODER_TRANSCODER_UNSUPPORTED_CHANNEL_MODE << 1

#define MS_DDC_OPEN_FAILED        10 << 12
#define MS_DDC_SET_PARAM_FAILED   11 << 12

/**** Module Structures ****/
/*! This structure holds an error code and a string with a corresponding error message */
typedef struct
{
	int err;           /*!< Error code */
	const char*	p_msg; /*!< String describing the error associated with the error code above */
} MS_ERR;


const MS_ERR ms_ddre_msgtab[] = {
    {DOLBY_DIGITAL_REENCODER_MDC_FAIL,      "Error in metadata converter"},
    {DOLBY_DIGITAL_REENCODER_DDENC_FAIL,    "Error during DD encoding"},
    {DOLBY_DIGITAL_REENCODER_INVALID_PARAM, "Invalid parameter passed to DolbyDigitalReencoderProcess()"},
    {DOLBY_DIGITAL_REENCODER_COMPR_FAIL,    "Error reported by the compressor"},
    {DOLBY_DIGITAL_REENCODER_GENERAL_ERROR, "Unspecified error in Dolby Digital reencoder"},
    {MS_DDRE_OPEN_FAILED,                   "Failed to open the Dolby Digital Reencoder"},
    {MS_DDRE_SET_PARAM_FAILED,              "Failure in DolbyDigitalSetProcessParam, unable to set"},
    {MS_ERR_EOT,                            ""}
};

const MS_ERR ms_ddt_msgtab[] = {
    {MS_DDT_OPEN_FAILED,      "Failed to open the Dolby Pulse Dual Decoder Transcoder"},
    {MS_DDT_SET_PARAM_FAILED, "Failure in DolbyPulseDecoderTranscoderSetProcessParam(), unable to set"},
    {MS_ERR_EOT,              ""}
};

const MS_ERR ms_ddc_msgtab[] = {
    /* subroutine errors */
    {DDPI_DDC_ERR_UNSUPPORTED_PARAM,         "Unsupported parameter ID passed to set/getprocessparam"},
    {DDPI_DDC_ERR_MISSING_PROG,              "Selected associated audio program not present in bitstream"},
    {DDPI_DDC_ERR_PROG_INCONSISTENT_FS,      "Main and associated audio programs have inconsistent sample rates"},
    {DDPI_DDC_ERR_PROG_INCONSISTENT_NBLKS,   "Main and associated audio programs have inconsistent blocks per frame"},
    {DDPI_DDC_ERR_REPEAT_STREAMTYPE_ID,      "Repeat streamtype aAnd substream ID before I0 frame - potential main program dropout"},
    {DDPI_DDC_ERR_MISORDERED_SUBSTREAM_ID,   "Misordered streamtype and substream ID - potential main program dropout"},
    {DDPI_DDC_ERR_NO_INDEPENDENT_FRAME,      "Missing I0 frame in input bitstream"},
    {DDPI_DDC_ERR_UNKNOWN_STREAMTYPE,        "Unable to determine streamtype and stream ID for corrupt frame"},
    {DDPI_DDC_ERR_INVALID_SUBSTREAMID,       "Substream ID selection out of valid range"},
    {DDPI_DDC_ERR_UNDEFINED_SUBSTREAMID,     "Substream ID selection not defined"},
    {DDPI_DDC_ERR_INVALID_COMPMODE,          "Compression mode out of valid range"},
    {DDPI_DDC_ERR_INVALID_PCMSCALE,          "PCM scale factor out of valid range"},
    {DDPI_DDC_ERR_INVALID_DYNSCLHI,          "DRC high scale factor out of valid range"},
    {DDPI_DDC_ERR_INVALID_DYNSCLLO,          "DRC low scale factor out of valid range"},
    {DDPI_DDC_ERR_INVALID_MAINONLYMODE,      "Main-only mode selection out of valid range"},
    {DDPI_DDC_ERR_INVALID_STEREOMODE,        "Stereo mode selection out of valid range"},
    {DDPI_DDC_ERR_DISABLED_STEREOMODE,       "Stereo mode selection only enabled in single-decode mode (-mo)"},
    {DDPI_DDC_ERR_INVALID_OUTMODE,           "Invalid output channel configuration"},
    {DDPI_DDC_ERR_INVALID_DUALMODE,          "Dual-mode selection out of valid range"},
    {DDPI_DDC_ERR_INVALID_DECCONCEALMODE,    "Decoder error concealment flag out of valid range"},
    {DDPI_DDC_ERR_INVALID_DECERRRPTCNT,      "Decoder error concealment strategy out of valid range"},
    {DDPI_DDC_ERR_INVALID_DCVCONCEALMODE,    "Decoder-Converter error concealment flag out of valid range"},
    {DDPI_DDC_ERR_INVALID_KARAOKEMODE,       "Karaoke mode out of valid range"},
    {DDPI_DDC_ERR_SYNC_ERROR,                "Lost sync with main audio program"},
    {DDPI_DDC_ERR_ADDFRAME_COMPLETETS,       "Attempt to add frames to a complete timeslice - call {DDPI_DDc_processtimeslice first"},
    {DDPI_DDC_ERR_INVALID_INPUTMODE,         "Invalid input mode sent to {DDPI_DDc_open()"},
    {DDPI_DDC_ERR_DUALMONO_INPUT,            "Attempt to decode dual-mono input outside of main-only mode"},
    {DDPI_DDC_ERR_UNSUPPORTED_STRMTYP,       "Input streamtype not supported by DDC"},
    {DDPI_DDC_ERR_INCOMPLETE_INPUT_FRAME,    "Input frame is incomplete"},
    {DDPI_DDC_ERR_INSUFFICIENT_FRAME_WORDS,  "Not enough data to determine frame size"},
    {DDPI_DDC_ERR_GETFRAMESIZE_ERROR,        "Couldn't determine size of frame"},
    {DDPI_DDC_ERR_INVALID_DRCSUPPRESS_ARG,   "Invalid Suppress DRC mode (must be either ON or OFF)"},
    {DDPI_DDC_ERR_INVALID_MDCTBANDLIMIT,     "MDCT bandlimiting mode not supported"},
    {MS_DDC_OPEN_FAILED,                     "Failed to open the Dolby Digital Dual Decoder Converter"},
    {MS_DDC_SET_PARAM_FAILED,                "Failure in ddpi_ddc_setprocessparam(), unable to set"},
    {MS_ERR_EOT,                             ""}
};

/**** Module Functions ****/

/**
 * \brief This function evaluates the return value from the DDRE functions
 *         and prints out the appropriate error message if necessary.
 */
// MS_RETVAL ms_handle_ddre_retval(int err_code, const char *additional_infostr);

/**
 * \brief This function evaluates the return value from the DDT functions
 *         and prints out the appropriate error message if necessary.
 */
// MS_RETVAL ms_handle_ddt_retval(int err_code,  int instance, const char *additional_infostr);

/**
 * \brief This function evaluates the return value from the DDC functions
 *         and prints out the appropriate error message if necessary.
 */
// MS_RETVAL ms_handle_ddc_retval(int err_code,  const char *additional_infostr);

/**
*	\brief This function looks for a human-readable text string to display
*	       for a given application error code.
*
*	\return ERR_NO_ERROR (0) if string found, nonzero otherwise
*/
//int ms_errorlookup(
//                   const MS_ERR *p_msgtab,   /*!< [in]  Pointer to table which translates error codes into error messages */
//                   int           err,        /*!< [in]  Error code returned during processing */
//                   const char  **p_errtext   /*!< [out] User-supplied additional text */
//                   );

#endif /* MS_ERR_H_ */
