/* $Header: //depot/lechin/ms11q/20111026_ms11_5_development/ddc/common/global/c_ref/code/gbl.h#1 $ */

/***************************************************************************\
*
*	Unpublished work.  Copyright 2003-2011 Dolby Laboratories, Inc. and
*	Dolby Laboratories Licensing Corporation.  All Rights Reserved.
*
*	Module:		Global Module
*
*	File:		gbl.h
*
\***************************************************************************/

/*! \defgroup gbl Support - Global

	Defines global macros, structures and variables
*/
/*! \ingroup dec */
/*! \ingroup dcv */
/*! \addtogroup gbl */
/*! \ingroup gbl */

/*! \file gbl.h
*
*  \brief GBL module global types header file.
*
*  Part of the Global module.
*/

#ifndef GBL_H
#define GBL_H

#include "gbl_type.h"

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif

/**** Module Equates ****/
#define		GBL_UNDEFINED			(-1)	/*!< Undefined value for initialization */
#define		GBL_FALSE		        0		/*!< Unambiguous value for FALSE		*/
#define		GBL_TRUE				1		/*!< Unambiguous value for TRUE		*/

/* general system equates */
#define		GBL_MAXCHANS			6		/*!< Maximum number of channels	*/
#define		GBL_MAXFCHANS			5		/*!< Maximum number of full bandwidth channels */
#define		GBL_MAXPCMCHANS			6		/*!< Maximum number of output channels	*/
#define		GBL_MAXCHANCFGS			8		/*!< Maximum number of channel configs */
#define		GBL_MAXREMATCHANS		2		/*!< Maximum number ofrematrix cahnnels	*/
#define		GBL_MAXIPRGMS			2		/*!< Maximum number of dual mono channels */
#define		GBL_MAXBLOCKS			6		/*!< Number of blocks per frame */
#define     GBL_MAXFRAMES           6       /*!< Maximum number of frames in a frame set */
#define		GBL_MAXAUXDATAWRDS		1024	/*!< Maximum number of aux data words */
#define		GBL_MAXDDFRMWRDS		1920	/*!< Maximum number of words per DD frame  */
#define		GBL_MAXDDPFRMWRDS		2048	/*!< Maximum number of words per DD+ frame */

#define		GBL_MAXCHBWCOD			60		/*!< Maximum channel bandwidth code */
#define		GBL_MAXCHBWOFFST		73		/*!< Channel bandwidth code offset */
#define		GBL_BYTESPERWRD			2		/*!< Bytes per packed word */
#define		GBL_BITSPERWRD			(GBL_BYTESPERWRD*8)	/*!< Bytes per packed word */
#define		GBL_BITSPERWRD_SHFT		4 		/*!< Shift required to divide by bits per word log2(GBL_BITSPERWRD) */
#define		GBL_MAXMIXDATAWRDS		17		/*!< Maximum number of mix data words */
#define		GBL_MAXPROGS			8		/*!< Maximum number of programs in a timeslice */
#define		GBL_MAXIFRMS			1		/*!< Maximum number of independent frames in a program */
#define		GBL_MAXDFRMS			8		/*!< Maximum number of dependent frames in a program */
#define		GBL_MINRPTCNT			1		/*!< Minimum number of error conceal block repeats */
#define		GBL_MAXRPTCNT			32767	/*!< Maximum number of error conceal block repeats */

/* channel ordering equates */
typedef enum {
	GBL_NONE = -1,    /*!< Channel not in use */
	GBL_LEFT =  0,    /*!< Left channel */
	GBL_CNTR =  1,    /*!< Center channel */
	GBL_RGHT =  2,    /*!< Right channel */
	GBL_LSUR =  3,    /*!< Left surround channel */
	GBL_MSUR =  3,    /*!< Mono surround channel */
	GBL_RSUR =  4,    /*!< Right surround channel */
	GBL_LFE  =  5     /*!< Low frequency effects channel */
} GBL_CH_ID;

/* filter bank equates */
#define		GBL_BLKSIZE				256		/*!< Block length (half transform length) */
#define		GBL_FFTSIZE				128		/*!< FFT transform size	*/
#define		GBL_FFTSIZELG2M3		4		/*!< log2(GBL_FFTSIZE) - 3 */
#define		GBL_MAXMANTS			253		/*!< Maximum number of mantissas per channel */
#define		GBL_LFEMANTS			7		/*!< Number of LFE mantissas */

/* exponent equate */
#define		GBL_MAXEXP				32		/*!< Maximum decoder exponent */
#define		GBL_FEDZEROEXP			24		/*!< Used in Front-End Decode to set exponent/mantissa to zero */

/* miscellaneous equates */
#define		GBL_SYNCWRD				((int16_t)0x0b77)	/*!< AC-3 frame sync word */
#define		GBL_MAXFSCOD			3		/*!< Number of defined sample rates	*/
#define		GBL_MAXDDDATARATE		38		/*!< Number of defined data rates */
#define		GBL_MAXREMATBNDS		4		/*!< Maximum number of rematrixing bands */

/* global table sizes */
#define		GBL_UNGRP3_TABSZ		27		/*!< Ungroup 3 table size */
#define		GBL_UNGRP5_TABSZ		125		/*!< Ungroup 5 table size */
#define		GBL_UNGRP11_TABSZ		121		/*!< Ungroup 11 table size */

/* useful constants */
#define		GBL_SQRT3EXP			(-1)
#define		GBL_SQRT3MANT			((BE_S16)0x6EDA)

#define		GBL_FRACT_HALF			(DLB_LcF(0.5))

/* halfrate equates */
#define 	GBL_MAXHALFRATE			2		/*!< Number of defined half rate codes */

#ifdef DLB_METHOD_IS_FLOAT
#define		GBL_PERCENT2FRACT_MAP(a)		((DLB_LFRACT)((a)/100.0))
#define		GBL_PERCENT2FRACT_DRCMAP(a)	    ((DLB_LFRACT)((a)/100.0))
#else
#define		GBL_PERCENT2FRACT_FACTOR		10737418  /* 0x40000000/100 */
#define		GBL_PERCENT2FRACT_MAP(a)		((DLB_LFRACT)(a) * (GBL_PERCENT2FRACT_FACTOR))

#define		GBL_PERCENT2FRACT_DRCFACTOR		83886   /* (32768 * 256)/100 */
#define		GBL_PERCENT2FRACT_DRCMAP(a)		((((DLB_LFRACT)(a) * (GBL_PERCENT2FRACT_DRCFACTOR)) + 128 ) >> 8)
#endif /*DLB_METHOD_IS_FLOAT*/

/**** Module Macros ****/

#define		GBL_5_8THS(n)			(((n) >> 3) + ((n) >> 1))

/**** Module Enumerations ****/

/* exponent strategy */
enum { GBL_REUSE=0, GBL_D15, GBL_D25, GBL_D45 };

/* delta bit allocation strategy */
enum { GBL_DELTREUSE=0, GBL_DELTNEW, GBL_DELTSTOP };

/* Output Channel modes */
enum { GBL_MODE11=0, GBL_MODE_RSVD=0, GBL_MODE10, GBL_MODE20,
		GBL_MODE30, GBL_MODE21, GBL_MODE31, GBL_MODE22, GBL_MODE32 };

/* dual mono downmix mode */
enum { GBL_DUAL_STEREO=0, GBL_DUAL_LEFTMONO, GBL_DUAL_RGHTMONO, GBL_DUAL_MIXMONO };

/* preferred stereo mode */
enum { GBL_STEREOMODE_AUTO=0, GBL_STEREOMODE_SRND, GBL_STEREOMODE_STEREO };
enum { GBL_STEREODMIX_LTRT=0, GBL_STEREODMIX_LORO, GBL_STEREODMIX_PLII };

/* compression mode */
enum { GBL_COMP_CUSTOM_0=0, GBL_COMP_CUSTOM_1, GBL_COMP_LINE, GBL_COMP_RF };

#ifdef KCAPABLE
/* karaoke capable mode */
enum { GBL_NO_VOCALS=0, GBL_VOCAL1, GBL_VOCAL2, GBL_BOTH_VOCALS };
#endif /* KCAPABLE */

/* half rate codes */
enum { GBL_NORM_SAMPRATE=0, GBL_HALF_SAMPRATE, GBL_QUARTER_SAMPRATE };

/* PCM error concealment modes */
enum { GBL_PCMCONCEAL_ALWAYSRPT=-1, GBL_PCMCONCEAL_ALWAYSMUTE};

/**** Module Tables ****/

extern const char  gbl_dolby_copyright[];
extern const int32_t  gbl_sampratetab[GBL_MAXFSCOD];
extern const int16_t gbl_frmsizetab[GBL_MAXFSCOD][GBL_MAXDDDATARATE];
extern const int16_t gbl_chanary[GBL_MAXCHANCFGS];
extern const int16_t gbl_chantab[GBL_MAXCHANCFGS][GBL_MAXCHANS];
#ifdef DLB_METHOD_IS_FLOAT
extern const DLB_LFRACT gbl_lshftab[GBL_MAXEXP + 1];
extern const DLB_LFRACT gbl_rshftab[GBL_MAXEXP + 1];
#endif /*DLB_METHOD_IS_FLOAT*/
extern const uint16_t gbl_ungrp3[GBL_UNGRP3_TABSZ];
extern const uint16_t gbl_ungrp5[GBL_UNGRP5_TABSZ];
extern const uint16_t gbl_ungrp11[GBL_UNGRP11_TABSZ];
#ifdef MIPS_OPT
extern const uint16_t gbl_ungrp3_opt[GBL_UNGRP3_TABSZ];
extern const uint16_t gbl_ungrp5_opt[GBL_UNGRP5_TABSZ];
extern const uint16_t gbl_ungrp11_opt[GBL_UNGRP11_TABSZ];
#endif /* MIPS_OPT */
extern const uint16_t gbl_msktab[GBL_BITSPERWRD + 1];
extern const int16_t gbl_lxrx2cmix_tab[8];
extern const int16_t gbl_lxrx2surmix_tab[8];
extern const DLB_LFRACT gbl_norm_range_table[20];

/**** Module Structures ****/

/*! \brief Global module range structure */
typedef struct
{
	int16_t 	begin;  /*!< Beginning of range */
	int16_t 	end;    /*!< End of range */
} GBL_RANGE;

/*! \brief Global module sample rate structure */
typedef struct
{
	int16_t bse_fscod;			/*!< Sampling frequency code */
} GBL_SAMPRATE;

/*! \brief Global Dolby Digital Plus mixing data fields */
typedef struct
{
	int16_t	dmixmod;					/*!< Preferred downmix mode */
	int16_t	cmixlev;					/*!< Center downmix level */
	int16_t	surmixlev;					/*!< Surround downmix level */
	int16_t	ltrtcmixlev;				/*!< Lt/Rt center downmix level */
	int16_t	ltrtsurmixlev;				/*!< Lt/Rt surround downmix level */
	int16_t	lorocmixlev;				/*!< Lo/Ro center downmix level */
	int16_t	lorosurmixlev;				/*!< Lo/Ro surround downmix level */
	int16_t	pgmscl[GBL_MAXIPRGMS];		/*!< Program scale factor */
	int16_t	extpgmscl;					/*!< External program scale factor  */
	int16_t	paninfo[GBL_MAXIPRGMS];		/*!< Panning information */
	int16_t	panmean[GBL_MAXIPRGMS];		/*!< Pan mean angle data */
	int16_t	lfemixlevcod;				/*!< LFE mix level code */
	int16_t	premixcmpsel;				/*!< Premix compression word select */
	int16_t	drcsrc;						/*!< Dynamic range control word source (external or current) */
	int16_t	premixcmpscl;				/*!< Premix compression word scale factor */
	int16_t	extpgmlscl;					/*!< External program left scale factor */
	int16_t	extpgmcscl;					/*!< External program center scale factor */
	int16_t	extpgmrscl;					/*!< External program right scale factor */
	int16_t	extpgmlsscl;				/*!< External program left surround scale factor */
	int16_t	extpgmrsscl;				/*!< External program right surround scale factor */
	int16_t	extpgmlfescl;				/*!< External program LFE scale factor */
	int16_t	dmixscl;					/*!< Downmix scale factor */
	int16_t	extpgmaux1scl;				/*!< External program 1st auxiliary channel scale factor */
	int16_t	extpgmaux2scl;				/*!< External program 2nd auxiliary channel scale factor */
} GBL_MIXDATA;

/*! \brief Global Dolby Digital Re-encoding metadata fields */
typedef struct
{
	uint16_t    cmixlev;         /* Also in GBL_MIXDATA */
	uint16_t    surmixlev;       /* Also in GBL_MIXDATA */
	uint16_t    dsurmod;         /* Also in DDPI_FMI_FRM_PRODDATA */
	uint16_t    acmod;           /* Also in DDPI_FMI_FRM_GENDATA */
	uint16_t    samplerate;      /* Also in DDPI_FMI_FRM_GENDATA */
	uint16_t    lfeon;           /* Also in DDPI_FMI_FRM_GENDATA */
} GBL_REENCDATA;

#endif /* GBL_H */

