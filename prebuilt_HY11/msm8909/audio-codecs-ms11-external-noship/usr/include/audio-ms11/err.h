/* $Header: //depot/lechin/ms11q/20111026_ms11_5_development/ddc/common/error/c_ref/code/err.h#1 $ */

/***************************************************************************\
*
*	Unpublished work.  Copyright 2003-2011 Dolby Laboratories, Inc. and
*	Dolby Laboratories Licensing Corporation.  All Rights Reserved.
*
*	Module:		Error Module
*
*	File:		err.h
*
\***************************************************************************/

/*! \file err.h
*
*  \brief ERR module header file.
*/

#ifndef ERR_H
#define ERR_H

/**** Module Definitions ****/

typedef int16_t		DSPerr;		/*!< DSP error type	*/

/**** Unique Module Identifiers ****/

#define		GBL_ID		1
#define		DBG_ID		2
#define		DSP_ID		3
#define		BSO_ID		4
#define		DRCD_ID		5
#define		BSI_ID		6
#define		EXPD_ID		7
#define		BTA_ID		8
#define		MNT_ID		9
#define		EXMD_ID		10
#define		CPL_ID		11
#define		MTX_ID		12
#define		XFM_ID		13
#define		DMX_ID		14
#define		CHN_ID		15
#define		ABK_ID		16
#define		CRC_ID		17
#define		FED_ID		18
#define		DCV_ID		19
#define		MCV_ID		20
#define		TPND_ID		21
#define     BCT_ID      22
#define		ECPD_ID		23
#define     SNR_ID      24
#define		SPXD_ID		25
#define     STF_ID      26
#define		TSI_ID		27
#define		TSC_ID		28
#define     HTF_ID      29
#define		ENF_ID		30
#define		FIO_ID		31
#define		BFD_ID		32
#define		DDC_ID		33
#define		XMX_ID		34
#define		TSH_ID		35
#define		DDRE_ID		36
#define		DEC_ID		37

/**** Module Macros ****/

/* check input arguments macro */
#ifdef ARGCHK
#include	<assert.h>
#define		ERR_CHKARG(a)			assert(a)
#else /* ARGCHK */
#define		ERR_CHKARG(a)			/*! Defined to nothing */
#endif /* ARGCHK */

/* assert macro */
#ifdef DEBUG
#include <assert.h>
#define		ERR_ASSERT(a)			assert(a)
#else	/* DEBUG */
#define		ERR_ASSERT(a)			/*! Defined to nothing */
#endif	/* DEBUG */

/*! Check error return macro */
#define		ERR_CHKRTN(a)			if ((a)>0) return (a)

/* error message macro */
#ifdef ERRMSG
#include	<stdio.h>
#define		ERR_PRINTERRMSG(a)		fprintf(stderr,"\n\nFATAL ERROR:  %s\n\nError occurred in:\n%s (line %d)\n\n" \
										,(a),__FILE__,__LINE__)
#define		ERR_PRINTWARNMSG(a,b)	if (a) fprintf(stderr,"\nWARNING:  %s\n",(b))
#define		ERR_MSGRTN(a,b)			{ fprintf(stderr,"\n\nFATAL ERROR:  %s\n\nError occurred in:\n%s (line %d)\n\n" \
										,(a),__FILE__,__LINE__); return (b); }
#else	/* ERRMSG */
#define		ERR_PRINTERRMSG(a)	/*!< Defined to nothing */
#define		ERR_PRINTWARNMSG(a,b)	/*!< Defined to nothing */
#define		ERR_MSGRTN(a,b)			return (b)
#endif	/* ERRMSG */

/*  error/warning code marcos */
#define		ERR_ERRCODE(a,b)		(((a)<<8)+(b))
#define		ERR_WARNCODE(a,b)		-(((a)<<8)+(b))

/**** Module equates ****/

#define		ERR_NO_ERROR	0		/*!< Error code representing no errors	*/

#endif /* ERR_H */
