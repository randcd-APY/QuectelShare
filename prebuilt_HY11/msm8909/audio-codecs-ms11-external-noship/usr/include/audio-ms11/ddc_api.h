/* $Header: //depot/lechin/ms11q/20111026_ms11_5_development/ddc/components/dual_decoder_converter/c_ref/code/sub/ddc_api.h#1 $ */

/***************************************************************************
*
*   Unpublished work.  Copyright 2011 Dolby Laboratories, Inc. and
*   Dolby Laboratories Licensing Corporation.  All Rights Reserved.
*
*   Module:     Dual-Decoder-Converter API Module
*
*   File:       ddc_api.h
*
***************************************************************************/

/*! \defgroup ddc DDC - Dual-Decoder-Converter
*
*   The Dolby Digital Plus Dual-Decoder Converter (DDC) decodes two DD/DD+
*   input bitstreams (designated "main" and "associated", up to 5.1 channels each)
*   to PCM audio, and converts the main audio bitstream from Dolby Digital Plus to
*   Dolby Digital.  The DDC optionally mixes the two PCM outputs utilizing mixing
*   metadata extracted from the associated audio bitstream.
*
*   The DDC has four subcomponents:
*
*   - <b>Decoder-Converter</b>: The Dolby Digital Plus Decoder-Converter (DCV)
*     decodes a DD/DD+ input bitstream (up to 5.1 channels) to PCM audio, and
*     converts the input bitstream from Dolby Digital Plus to Dolby Digital.
*
*   - <b>Decoder</b>: The Dolby Digital Plus Decoder (DEC) decodes a DD/DD+ input
*     bitstream (up to 5.1 channels) to PCM audio. The decoder also optionally
*     outputs mixing metadata for use by an external application to mix two DD/DD+
*     bitstreams.
*
*   - <b>Frame Information</b>: The Frame Information (FMI) module parses a DD/DD+
*     frame and unpacks its bitstream information.  A CRC check is performed on
*     the frame as part of the unpacking process.
*
*   - <b>Buffer Descriptors</b>: The Buffer Descriptors (BFD) module contains Dolby
*     Digital, Dolby Digital Plus and PCM buffer descriptions and functions for
*     buffer operations.
*/

/*! \ingroup msd */
/*! \addtogroup ddc */
/*! \ingroup ddc */

/*!
  \file ddc_api.h

  \brief DDC API header file.
*/

#ifndef DDC_API_H
#define DDC_API_H

/**** API Dependencies ****/

#include "gbl.h"
#include "err.h"

/**** API Configurations ****/

/**** API Defines ****/

#define DDPI_DDC_MAXPCMOUTCHANS     (6)          /*!< DDC decodes to 3/2 PCM              */

#define DDPI_DDC_MAXPCMOUTCHANS_SIM2CHD        (2)            /*!< Number of output PCM channels for the simultaneous 2-channel downmix output */

#define DDPI_DDC_BUFSPERPROG        (2)          /*!< Number of input buffers per program */

#define DDPI_DDC_TOTALINBUFS        ((DDPI_DDC_PROG_COUNT)*(DDPI_DDC_BUFSPERPROG))

#define DDPI_DDC_MINSCALEFACTOR     (0)          /*!< Minimum scale factor (zero percent) */
#define DDPI_DDC_MAXSCALEFACTOR     (100)        /*!< Maximum scale factor (100 percent)  */

#define DDPI_DDC_SUBSTREAMID_MIN    (DDPI_DDC_SUBSTREAMID_1)
#define DDPI_DDC_SUBSTREAMID_MAX    (DDPI_DDC_SUBSTREAMID_3)

#define DDPI_DDC_MIN_INPUT_WORDS    (3)          /*! Minimum number of words required to determine input frame size */
#ifdef DEBUG
#define DDC_DBG_OUTFLAG                (0x08000000)
#define DDC_DBG_FNAME                ("ddc_debug.ebg")
#endif /* DEBUG */

/**** API Error Codes ****/

#define DDPI_DDC_ERR_NO_ERROR                   ERR_NO_ERROR
#define DDPI_DDC_ERR_UNSUPPORTED_PARAM          ERR_ERRCODE(DDC_ID,1)
#define DDPI_DDC_ERR_MISSING_PROG               ERR_ERRCODE(DDC_ID,2)
#define DDPI_DDC_ERR_PROG_INCONSISTENT_FS       ERR_ERRCODE(DDC_ID,3)
#define DDPI_DDC_ERR_REPEAT_STREAMTYPE_ID       ERR_ERRCODE(DDC_ID,4)
#define DDPI_DDC_ERR_MISORDERED_SUBSTREAM_ID    ERR_ERRCODE(DDC_ID,5)
#define DDPI_DDC_ERR_NO_INDEPENDENT_FRAME       ERR_ERRCODE(DDC_ID,6)
#define DDPI_DDC_ERR_UNKNOWN_STREAMTYPE         ERR_ERRCODE(DDC_ID,7)
#define DDPI_DDC_ERR_INVALID_SUBSTREAMID        ERR_ERRCODE(DDC_ID,8)
#define DDPI_DDC_ERR_INVALID_COMPMODE           ERR_ERRCODE(DDC_ID,9)
#define DDPI_DDC_ERR_INVALID_PCMSCALE           ERR_ERRCODE(DDC_ID,10)
#define DDPI_DDC_ERR_INVALID_DYNSCLHI           ERR_ERRCODE(DDC_ID,11)
#define DDPI_DDC_ERR_INVALID_DYNSCLLO           ERR_ERRCODE(DDC_ID,12)
#define DDPI_DDC_ERR_INVALID_MAINONLYMODE       ERR_ERRCODE(DDC_ID,13)
#define DDPI_DDC_ERR_INVALID_STEREOMODE         ERR_ERRCODE(DDC_ID,14)
#define DDPI_DDC_ERR_INVALID_DUALMODE           ERR_ERRCODE(DDC_ID,15)
#define DDPI_DDC_ERR_INVALID_DECCONCEALMODE     ERR_ERRCODE(DDC_ID,16)
#define DDPI_DDC_ERR_INVALID_DECERRRPTCNT       ERR_ERRCODE(DDC_ID,17)
#define DDPI_DDC_ERR_INVALID_DCVCONCEALMODE     ERR_ERRCODE(DDC_ID,18)
#define DDPI_DDC_ERR_INVALID_KARAOKEMODE        ERR_ERRCODE(DDC_ID,19)
#define DDPI_DDC_ERR_SYNC_ERROR                 ERR_ERRCODE(DDC_ID,20)
#define DDPI_DDC_ERR_ADDFRAME_COMPLETETS        ERR_ERRCODE(DDC_ID,21)
#define DDPI_DDC_ERR_INVALID_INPUTMODE          ERR_ERRCODE(DDC_ID,22)
#define DDPI_DDC_ERR_DUALMONO_INPUT             ERR_ERRCODE(DDC_ID,23)
#define DDPI_DDC_ERR_PROG_INCONSISTENT_NBLKS    ERR_ERRCODE(DDC_ID,24)
#define DDPI_DDC_ERR_UNDEFINED_SUBSTREAMID      ERR_ERRCODE(DDC_ID,25)
#define DDPI_DDC_ERR_UNSUPPORTED_STRMTYP        ERR_ERRCODE(DDC_ID,26)
#define DDPI_DDC_ERR_DISABLED_STEREOMODE        ERR_ERRCODE(DDC_ID,27)
#define DDPI_DDC_ERR_INVALID_OUTLFE             ERR_ERRCODE(DDC_ID,28)
#define DDPI_DDC_ERR_INVALID_OUTMODE            ERR_ERRCODE(DDC_ID,29)
#define DDPI_DDC_ERR_NO_OUTPUT_CHANS            ERR_ERRCODE(DDC_ID,30)
#define DDPI_DDC_ERR_INVALID_EXTCHAN_ROUTE      ERR_ERRCODE(DDC_ID,31)
#define DDPI_DDC_ERR_ILLEGAL_CHAN_ROUTE_ARRAY   ERR_ERRCODE(DDC_ID,32)
#define DDPI_DDC_ERR_INCOMPLETE_INPUT_FRAME     ERR_ERRCODE(DDC_ID,33)
#define DDPI_DDC_ERR_INSUFFICIENT_FRAME_WORDS   ERR_ERRCODE(DDC_ID,34)
#define DDPI_DDC_ERR_GETFRAMESIZE_ERROR         ERR_ERRCODE(DDC_ID,35)
#define DDPI_DDC_ERR_INVALID_DRCSUPPRESS_ARG    ERR_ERRCODE(DDC_ID,36)
#define DDPI_DDC_ERR_UNKNOWN_REENCDATA          ERR_ERRCODE(DDC_ID,38)
#define DDPI_DDC_ERR_INVALID_MDCTBANDLIMIT      ERR_ERRCODE(DDC_ID,39)

/**** DCV API Error Codes ****/

#define		DDPI_DDC_DCV_ERR_UNSUPPORTED_PARAM		ERR_ERRCODE(DCV_ID, 3)	/*!< Unsupported parameter */
#define		DDPI_DDC_DCV_ERR_PROCESSING			    ERR_ERRCODE(DCV_ID, 4)	/*!< Processing error occurred */
#define		DDPI_DDC_DCV_ERR_UNSUPPORTED_FRAME		ERR_ERRCODE(DCV_ID, 5)	/*!< Unsupported frame */
#define		DDPI_DDC_DCV_ERR_UNSUPPORTED_STRMTYP	ERR_ERRCODE(DCV_ID, 10)	/*!< Unsupported stream type */
#define		DDPI_DDC_DCV_ERR_UNCONVERTIBLE		    ERR_ERRCODE(DCV_ID, 11)	/*!< Cannot convert bit stream */
#define		DDPI_DDC_DCV_ERR_UNSUPPORTED_OPERATION 	ERR_ERRCODE(DCV_ID, 13)	/*!< Unsupported operation */
#define     DDPI_DDC_DCV_ERR_INVALID_DRCSUPPRESS_ARG ERR_ERRCODE(DCV_ID, 14)  /*!< Invalid DRC suppression mode */

/**** DEC API Error Codes ****/

#define		DDPI_DDC_DEC_ERR_UNSUPPORTED_PARAM		ERR_ERRCODE(DEC_ID, 3)	/*!< Unsupported parameter */
#define		DDPI_DDC_DEC_ERR_PROCESSING				ERR_ERRCODE(DEC_ID, 4)	/*!< Processing error occurred */
#define		DDPI_DDC_DEC_ERR_UNSUPPORTED_FRAME		ERR_ERRCODE(DEC_ID, 5)	/*!< Unsupported frame */
#define		DDPI_DDC_DEC_ERR_UNSUPPORTED_STRMTYP	ERR_ERRCODE(DEC_ID, 10)	/*!< Unsupported stream type */
#define		DDPI_DDC_DEC_ERR_INVALID_PARAM          ERR_ERRCODE(DEC_ID, 11)	/*!< Invalid parameter */
#define		DDPI_DDC_DEC_ERR_UNSUPPORTED_OPERATION 	ERR_ERRCODE(DEC_ID, 13)	/*!< Unsupported operation */
#define     DDPI_DDC_DEC_ERR_INVALID_DRCSUPPRESS_ARG ERR_ERRCODE(DEC_ID, 14)  /*!< Invalid DRC suppression mode */

/**** API equates and enumerations ****/

/*! Control parameter identification for use with the setprocessparam and getprocessparam */
typedef enum
{
	DDPI_DDC_CTL_DEC_COMPMODE_ID = 0,           /*!< Decoder compression mode                                                      */
	DDPI_DDC_CTL_DEC_STEREOMODE_ID,             /*!< Decoder stereo output mode                                                    */
	DDPI_DDC_CTL_DEC_DUALMODE_ID,               /*!< Decoder dual mono mode                                                        */
	DDPI_DDC_CTL_DEC_DRCSCALEHIGH_ID,           /*!< Decoder DRC high scale factor                                                 */
	DDPI_DDC_CTL_DEC_DRCSCALELOW_ID,            /*!< Decoder DRC low scale factor                                                  */
	DDPI_DDC_CTL_DEC_OUTPCMSCALE_ID,            /*!< Decoder output PCM scale factor                                               */
	DDPI_DDC_CTL_DEC_USRKRKPARAMS_ID,           /*!< Interprets input as (DMXD_KRKPARAMS *)                                        */
	DDPI_DDC_CTL_DEC_USRKRKMODE_ID,             /*!< Decoder karaoke mode                                                          */
	DDPI_DDC_CTL_DEC_ERRORCONCEAL_ID,           /*!< Decoder error concealment                                                     */
	DDPI_DDC_CTL_DEC_ERRORMAXRPTS_ID,           /*!< Decoder error max block repeats                                               */
	DDPI_DDC_CTL_CNV_ERRORCONCEAL_ID,           /*!< Converter error concealment                                                   */
	DDPI_DDC_CTL_INPUTMODE_ID,                  /*!< Single- or Dual-input mode (get only)                                         */
	DDPI_DDC_CTL_SUBSTREAMSELECT_ID,            /*!< Associated audio program select                                               */
	DDPI_DDC_CTL_MAINONLYMODE_ID,               /*!< DDC main only mode flag                                                       */
	DDPI_DDC_CTL_DEC_OUTLFE_ID,                 /*!< Output LFE                                                                    */
	DDPI_DDC_CTL_DEC_OUTMODE_ID,                /*!< Output channel configuration                                                  */
	DDPI_DDC_CTL_DEC_DRCSUPPRESS_ID,            /*!< DRC suppression mode                                                          */
	DDPI_DDC_CTL_DEC_COMPMODE_SIM2CHD_ID,       /*!< Decoder compression mode for the simultaneous 2-channel downmix output        */
	DDPI_DDC_CTL_DEC_STEREOMODE_SIM2CHD_ID,     /*!< Decoder stereo output mode for the simultaneous 2-channel downmix output      */
	DDPI_DDC_CTL_DEC_DRCSCALEHIGH_SIM2CHD_ID,   /*!< Decoder DRC high scale factor for the simultaneous 2-channel downmix output   */
	DDPI_DDC_CTL_DEC_DRCSCALELOW_SIM2CHD_ID,    /*!< Decoder DRC low scale factor for the simultaneous 2-channel downmix output    */
	DDPI_DDC_CTL_DEC_OUTPCMSCALE_SIM2CHD_ID,    /*!< Decoder output PCM scale factor for the simultaneous 2-channel downmix output */
	DDPI_DDC_CTL_DEC_DRCSUPPRESS_SIM2CHD_ID,    /*!< DRC suppression mode for the simultaneous 2-channel downmix output            */
	DDPI_DDC_CTL_DEC_MDCTBANDLIMIT_ID,          /*!< MDCT bandlimiting mode                                                        */
} DDPI_DDC_CTL_ID;

/*! PCM outputs */
typedef enum
{
	DDPI_DDC_PCMOUT_MAIN = 0,           /*!< Main PCM output */
	DDPI_DDC_PCMOUT_ASSOC = 1,          /*!< Associated PCM output */
	DDPI_DDC_PCMOUT_SIM2CHD = 1,        /*!< Simultaneous 2-channel downmix PCM output */
	DDPI_DDC_PCMOUT_COUNT = 2,          /*!< Total PCM count */
} DDPI_DDC_PCMOUT;

/*! Input bitstream source */
typedef enum
{
	DDPI_DDC_PROG_MAIN =    0,          /*!< Main program ID                        */
	DDPI_DDC_PROG_ASSOC =   1,          /*!< Associated program ID                  */
	DDPI_DDC_PROG_COUNT =   2,          /*!< Total program count                    */
} DDPI_DDC_PROG;

/*! Substream selection */
typedef enum
{
	DDPI_DDC_SUBSTREAMID_0 = 0,
	DDPI_DDC_SUBSTREAMID_1 = 1,
	DDPI_DDC_SUBSTREAMID_2 = 2,
	DDPI_DDC_SUBSTREAMID_3 = 3,
} DDPI_DDC_SUBSTREAMID;

#ifdef KCAPABLE
/*! Karaoke mode presets */
typedef enum
{
	DDPI_DDC_KMODE_NOVOCAL =    GBL_NO_VOCALS,
	DDPI_DDC_KMODE_LVOCAL =     GBL_VOCAL1,
	DDPI_DDC_KMODE_RVOCAL =     GBL_VOCAL2,
	DDPI_DDC_KMODE_BOTHVOCALS = GBL_BOTH_VOCALS,
	DDPI_DDC_KMODE_COUNT,
} DDPI_DDC_KMODE;
#endif /* KCAPABLE */

/*! \brief DDC input mode */
typedef enum
{
	DDPI_DDC_INPUTMODE_SINGLEINPUT,
	DDPI_DDC_INPUTMODE_DUALINPUT,
	DDPI_DDC_INPUTMODE_COUNT,
} DDPI_DDC_INPUTMODE;

/**** Dual-Decoder-Converter API structures ****/

/*! \brief DDC query output parameters structure */
typedef struct
{
	uint32_t        ddc_size;           /*!< Size of the DDC memory in bytes        */
	int16_t        version_major;      /*!< Major version number                   */
	int16_t        version_minor;      /*!< Minor version number                   */
	int16_t        version_update;     /*!< Update version number                  */
	const char   *p_copyright;       /*!< Pointer to copyright string            */
	uint32_t        delaysamples;       /*!< PCM sample delay in decoded output     */
} DDPI_DDC_QUERY_OP;

/*! \brief DDC structure with input parameters for the #ddpi_ddc_addframe function */
typedef struct
{
	DDPI_DDC_PROG               programid;  /*!< Frame source identifier            */
	const BFD_BUFDESC_DDPFRM    *p_inbfd;   /*!< Buffer descriptor used to open fmi */
} DDPI_DDC_AF_IP;

/*! \brief DDC output parameters for the #ddpi_ddc_addframe function */
typedef struct
{
	int         frameassigned;      /*!< Boolean: this frame will be decoded    */
	int         completetimeslice;  /*!< Boolean: DDC has a complete timeslice  */
	unsigned char        timeslicecontents;  /*!< Bits 0-3 indicate i0-i3 in timeslice   */
} DDPI_DDC_AF_OP;

/*! \brief DDC structure with output parameters for the #ddpi_ddc_processtimeslice function */
typedef struct
{
	/* Frame handles to newly processed input frames */
	const BFD_BUFDESC_DDPFRM    *p_ddpinbfds[DDPI_DDC_PROG_COUNT];  /*!< DDPlus input buffer descriptor */

	/* Pointers to output buffers allocated by the caller */
	BFD_BUFDESC_DDPFRM          *p_ddoutbfd;    /*!< DDPlus output buffer descriptor  */
	BFD_BUFDESC_PCMCH           *ppp_pcmoutbfds[DDPI_DDC_PCMOUT_COUNT][DDPI_DDC_MAXPCMOUTCHANS]; /*!< PCM output buffer descriptor   */
	GBL_MIXDATA                 *p_mixdata;     /*!< Mixing metadata */
	GBL_REENCDATA               *p_reencdata;  /*!< Reencoding metadata */
	int16_t                    *p_dialnorm;   /*!< Effective dialnorm of output audio (probably not identical to dialnorm of input bitstream) */

	/* DCV/DEC output parameters */
	int16_t                    nblkspcm; /*!< Number of PCM blocks */
	uint16_t                   pcmsamplerate;   /*!< PCM sample rate */
	int                     pcmupsampleflag; /*!< PCM upsample flag */
	int16_t                    sourcechcfg[DDPI_DDC_PROG_COUNT];   /*!< Source channel configuration */
	int16_t                    decodechcfg[DDPI_DDC_PROG_COUNT];   /*!< Decoded channel configuration */

	/* status flags */
	DSPerr                      errflag[DDPI_DDC_PCMOUT_COUNT];     /*!< Error flag  */
	int                     pcmdatavalid[DDPI_DDC_PCMOUT_COUNT];    /*!< PCM data valid flag */
	int                     dddatavalid;                        /*!< DD data valid flag */
	int                     mixdatavalid;                       /*!< Mixing metadata valid flag */
	int                     reencdatavalid;                     /*!< Reencoding metadata valid flag */
} DDPI_DDC_PT_OP;

/**** DDPlus Interface Functions for Dual-Decoder-Converter (API) ****/

/*!
****************************************************************
*   This function returns the subroutine memory size (in bytes),
*   the subroutine version number and the Dolby copyright string.
*   Call this function at system startup.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_query(
	DDPI_DDC_QUERY_OP           *p_outparams        /*!< \out: Query output parameters */
);

/*!
****************************************************************
*
*   This function initializes the subroutine memory.  This
*   includes initializing internal buffer pointers and control
*   parameters to default values for #ddpi_ddc_getprocessparam.
*   Call this function once after allocating the memory requested
*   by the call to #ddpi_ddc_query.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_open(
	void                        *p_ddchdl,          /*!< \mod: Pointer to subroutine memory */
	const DDPI_DDC_INPUTMODE    inputmode           /*!< \in: Indicates DDC input mode */
);

/*!
****************************************************************
*
*   This function sets the specified dual-decoder-converter control
*   parameter.  Changes are accumulated and take effect after
*   calls to #ddpi_ddc_processtimeslice complete processing of an
*   entire frameset.  This ensures that no parameters change that
*   could corrupt a single frameset that spans multiple timeslices.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no errors occurred
*   - DDPI_DDC_ERR_UNSUPPORTED_PARAM if unsupported parameter
*   - DDPI_DDC_ERR_INVALID_xxx if value out of valid range
*
*****************************************************************/
DSPerr ddpi_ddc_setprocessparam(
	void                        *p_ddchdl,          /*!< \mod: Pointer to subroutine memory */
	const DDPI_DDC_CTL_ID       paramid,            /*!< \in: Control parameter identification */
	const void                  *p_paramval         /*!< \in: Pointer to parameter value */
);

/*!
****************************************************************
*
*   This function gets the current value of the specified
*   dual-decoder-converter control parameter.  This function
*   returns the value of the parameter currently in use and is not
*   aware of any values that may change after processing a
*   complete frameset (see #ddpi_ddc_setprocessparam).
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no errors occurred
*   - DDPI_DDC_ERR_UNSUPPORTED_PARAM if unsupported parameter
*
*****************************************************************/
DSPerr ddpi_ddc_getprocessparam(
	const void                  *p_ddchdl,          /*!< \mod: Pointer to subroutine memory */
	const DDPI_DDC_CTL_ID       paramid,            /*!< \in: Control parameter identification */
	void                        *p_paramval         /*!< \out: Pointer to parameter value */
);

/*!
****************************************************************
*
*   This function takes a partial frame, contained in the
*   buffer descriptor, and determines the number of words in the
*   complete frame so that the caller can read the rest of the
*   frame from the input buffer.
*
*   The input buffer must contain DDPI_DDC_MIN_INPUT_WORDS of
*   the input frame, starting with the sync word (0x0B77), to
*   successfully determine the input frame size.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no errors, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_getddpframesize(
	BFD_BUFDESC_DDPFRM          *p_ddinbfd,         /*!< \in: Buffer descriptor containing partial frame */
	int16_t                    *p_nwords           /*!< \out: Number of 16-bit words in the complete frame */
);

/*!
****************************************************************
*
*   This function takes a partial frame, contained in the
*   buffer descriptor, and extracts the fscod bitstream
*   element, which is converted to sample rate and returned.
*
*   The input buffer must contain DDPI_DDC_MIN_INPUT_WORDS of
*   the input frame, starting with the sync word (0x0B77), to
*   successfully determine the input frame size.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no errors, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_getddpsamplerate(
	BFD_BUFDESC_DDPFRM          *p_ddinbfd,         /*!< \in: Buffer descriptor containing partial frame */
	int32_t                     *p_samplerate       /*!< \out: Sample rate of the frame */
);

/*!
****************************************************************
*
*   This function takes a complete frame, contained in the FMI
*   handle, and adds it to the current timeslice, indicating in
*   the frame status whether this frame is required for decoding.
*   If the newly added frame starts the next timeslice, the
*   frame is saved for the next timeslice and the <i>completetimeslice</i>
*   flag in the output parameters structure is set.
*
*   Once this function returns with <i>completetimeslice</i> set to
*   GBL_TRUE, #ddpi_ddc_processtimeslice must be called before
*   attempting to call this function again, else an error will
*   result.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no errors, non-zero otherwise
*   - DDPI_DDC_ERR_ADDFRAME_COMPLETETS if timeslice is full
*
*****************************************************************/
DSPerr ddpi_ddc_addframe(
	void                        *p_ddchdl,          /*!< \in: Pointer to subroutine memory */
	const DDPI_DDC_AF_IP        *p_inparams,        /*!< \in: Add frame input parameters */
	DDPI_DDC_AF_OP              *p_outparams        /*!< \out: Add frame output parameters */
);

/*!
****************************************************************
*
*   This function is responsible for handling the processing of a
*   timeslice of DDPlus data, consisting of one frame of data from
*   the main audio program and one frame of data from the selected
*   associated audio program (if it exists in the current timeslice).
*
*   This function will return valid PCM, DD and mix data only when
*   a complete frameset has been processed.  In the case of
*   framesets that span more than one frame, the intermediate PCM
*   will be stored until all frames in the frameset are processed.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no errors, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_processtimeslice(
	void                        *p_ddchdl,          /*!< \in: Pointer to subroutine memory */
	DDPI_DDC_PT_OP              *p_outparams        /*!< \out: Process timeslice output parameters */
);

/*!
****************************************************************
*
*   This function performs all clean up necessary to close the
*   dual-decoder-converter.  Call this function once, at system
*   shutdown.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_close(
	void                        *p_ddchdl   /*!< \mod: Pointer to subroutine memory */
);

#ifdef DEBUG
/*!
****************************************************************
*
*   This function opens the necessary debug files based on the
*   debug flags passed in.  Call this function once, at system
*   startup, after calling #ddpi_ddc_open.
*
*   See dec_api.h or dcv_api.h for debug flag definitions.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_opendebug(
	void                        *p_ddchdl,                          /*!< \in: Point to subroutine memory */
	const int               dbgoutflags[DDPI_DDC_PROG_COUNT],   /*!< \in: Enable debug output flags */
	const uint32_t              frm_debugflags,                     /*!< \in: Frame debug flags */
	const uint32_t              dec_debugflags                      /*!< \in: Decode debug flags */
);

/*!
****************************************************************
*
*   This function closes all debug output files.  Call this
*   function once, at system shutdown, before calling #ddpi_ddc_close.
*
*   \return
*   - DDPI_DDC_ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DSPerr ddpi_ddc_closedebug(
	void                        *p_ddchdl           /*!< \mod: Pointer to subroutine memory */
);
#endif  /* DEBUG */

#endif /* DDC_API_H */
