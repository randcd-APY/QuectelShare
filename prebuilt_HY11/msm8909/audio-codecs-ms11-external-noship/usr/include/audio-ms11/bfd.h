/***************************************************************************\
*
*	Unpublished work.  Copyright 2010 Dolby Laboratories, Inc. and
*	Dolby Laboratories Licensing Corporation.  All Rights Reserved.
*
*	Module:		Buffer descriptors Module
*
*	File:		bfd.h
*
\***************************************************************************/

/*! \defgroup bfd BFD - Buffer Descriptors

    The Buffer Descriptors (BFD) module contains DD, DD+ and PCM buffer
    descriptions and functions for buffer operations.
*/
/*! \ingroup ddc */
/*! \addtogroup bfd */
/*! \ingroup bfd */

/*! \file bfd.h
    \brief BFD module header file.
*/

#ifndef BFD_H
#define BFD_H

/**** Module Error Codes ****/

#define	BFD_ERR_GENERAL			ERR_ERRCODE(BFD_ID, 1)
#define	BFD_ERR_COUNT			1

/**** Module Structures ****/

/*! \brief Buffer Descriptor module DD/DD+ Frame buffer descriptor */
typedef struct
{
	int16_t	*p_buf;			/*!< Pointer to beginning of the frame buffer */
	uint16_t	nbufwords;		/*!< The number of words of data the buffer may contain */
	uint16_t	ndatawords;		/*!< The number of data words held in the buffer */
} BFD_BUFDESC_DDPFRM;

#ifndef CONVERTER_ONLY
/*! \brief Buffer Descriptor module PCM buffer descriptor */
typedef struct
{
	DLB_LFRACT	*p_buf;			/*!< Pointer to the beginning of the PCM channel buffer */
	uint16_t	nbufsamples;	/*!< The maximum size of the buffer in samples */
	uint16_t	nsamples;		/*!< The current number of samples in the buffer */
	uint16_t	sampleoffset;	/*!< The offset between PCM samples (for interleaved data) */
} BFD_BUFDESC_PCMCH;
#endif /* CONVERTER_ONLY */

/*! \brief Buffer Descriptor module text buffer descriptor */
typedef struct
{
	char		*p_buf;			/*!< Pointer to the null-terminated character buffer */
	uint16_t	nbufchars;		/*!< The total size of the buffer in characters */
} BFD_BUFDESC_TEXT;

/**** Module Functions ****/

/*!
****************************************************************
*
*	This function clears a DDP frame buffer, setting the buffer
*	to zeros and resetting the data word count to zero.
*
*	\return
*	- #ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DLB_API_IMPORT
DSPerr bfd_clearddpfrmbuf(
	BFD_BUFDESC_DDPFRM			*p_ddpfrmbfd		/*!< \mod: Pointer to an initialized DDP frame buffer */
	);

#ifndef CONVERTER_ONLY
/*!
****************************************************************
*
*	This function clears a PCM frame buffer, setting all the
*	buffer samples to zero and resetting the sample count to zero.
*
*	\return
*	- #ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DLB_API_IMPORT
DSPerr bfd_clearpcmchbuf(
	BFD_BUFDESC_PCMCH			*p_pcmchbfd		/*!< \mod: Pointer to an initialized PCM channel buffer	*/
	);

/*!
****************************************************************
*
*	This function initializes the buffer descriptors for a buffer
*	of interleaved PCM samples.  The buffer passed in must hold at
*	least (nchans * samplesperchan) samples.
*
*	\return
*	- #ERR_NO_ERROR if no error, non-zero otherwise
*
*****************************************************************/
DLB_API_IMPORT
DSPerr bfd_initpcmbufdesc(
	BFD_BUFDESC_PCMCH			p_pcmchbfds[],		/*!< \mod: Array of PCM channel BFDs to initialize */
	int16_t					nchans,				/*!< \in: Number of channels in the BFD array */
	int16_t					samplesperchan,		/*!< \in: Number of samples per channel buffer */
	DLB_LFRACT					*p_fractbuf 		/*!< \in: Buffer of size (nchans * samplesperchan) */
	);
#endif /* CONVERTER_ONLY */

#endif /* BFD_H */
