/* $Header: //depot/lechin/ms11q/20111026_ms11_5_development/ddc/common/global/c_ref/code/gbl_type.h#1 $ */

/***************************************************************************\
*
*	Unpublished work.  Copyright 2003-2011 Dolby Laboratories, Inc. and
*	Dolby Laboratories Licensing Corporation.  All Rights Reserved.
*
*	Module:		Global Type Definitions
*
*	File:		gbl_type.h
*
\***************************************************************************/

/*! \ingroup dec */
/*! \ingroup dcv */
/*! \addtogroup gbl */
/*! \ingroup gbl */

/*!
*  \file gbl_type.h
*
*  \brief GBL module definition of global types.
*
*  Part of the Global module.
*/

#ifndef GBL_TYPE_H
#define GBL_TYPE_H

/* Bit-Exact (BE) type definitions */
typedef unsigned short			BE_U16;		/*!< Bit-exact unsigned 16-bit integer */
typedef signed short			BE_S16;		/*!< Bit-exact signed 16-bit integer */
typedef unsigned int			BE_U32;		/*!< Bit-exact unsigned 32-bit integer */
typedef signed int				BE_S32;		/*!< Bit-exact signed 32-bit integer */

/*! \brief Global mantissa representation union */
#if (defined(MIPS_OPT) && defined(MNT_IN_BE_HIGH))
/*
	Redefinition of mantissa union to allow for storing the signed 16 bit FED representation
	of the mantissa in the high word of the union, which saves the << 16 bit shift when
	converting to 32 bit mantissa after SPX in chnd.c:chnd_decspx().
*/
#define BE_HIGH be.high
typedef struct
{
	BE_S16		low;		/* dummy */
	BE_S16		high;		/* bit-exact signed 16-bit mantissa representation in high word */
} DSPbemant;
typedef union
{
	DSPbemant   be;			/* struct to allow for accessing 16-bit mantissa in high word */
	DLB_LFRACT	ntv;		/* native mantissa representation */
} DSPmant;

#else /* MIPS_OPT && MNT_IN_BE_HIGH */

#define BE_HIGH be
typedef union
{
	BE_S16		be;			/* bit-exact signed 16-bit mantissa representation */
	DLB_LFRACT	ntv;		/* native mantissa representation */
} DSPmant;

#endif /* MIPS_OPT && MNT_IN_BE_HIGH */

/*! Null pointer equate	*/
#define		P_NULL		((void*)0)

/* API being exported by components */
#ifndef MIPS_OPT
#define DLB_API_IMPORT  /* Default to NULL */
#define DLB_API_EXPORT  /* Default to NULL */
#else
#define DLB_API_IMPORT  /* Default to NULL */
#define DLB_API_EXPORT  /* Default to NULL */
#endif

#endif /* GBL_TYPE_H */

