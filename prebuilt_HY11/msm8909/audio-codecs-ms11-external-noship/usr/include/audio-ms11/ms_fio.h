/*
 *        Confidential Information - Limited distribution to authorized persons
 *        only. This material is protected under international copyright laws as
 *        an unpublished work. Do not copy.
 *        Copyright (C) 2010 Dolby Laboratories Inc.
 *        Copyright (C) 2010 Dolby International AB
 *        All rights reserved.
 */


/*! \addtogroup msd */

/*! \file ms_fio.h
 *  \brief Multistream Decoder file input/output handling - header file
 */


#ifndef MS_FIO_H_
#define MS_FIO_H_

#include "ms_exec.h"
#include "ddc_api.h"


#define FIO_MAXPCMCHANS			(6)
#define FIO_MAXPCMCHBUFSIZE		(GBL_BLKSIZE * GBL_MAXBLOCKS)
#define FIO_MAXPCMBUFSIZE		(FIO_MAXPCMCHANS * FIO_MAXPCMCHBUFSIZE)
#define MAX_TMP_BUFFER_SIZE       3 * FIO_MAXPCMBUFSIZE * sizeof(DLB_LFRACT)   /* worst case is the mixer with two 6 channel inputs and one output */
#define DPULSE_INPUT_BUFFER_SIZE  7 * 2 * 6144 / 8  * sizeof(unsigned char)
#define DD_OUTPUT_BUFFER_SIZE     1920
#define MAX_PULSE_FRAMESIZE       2048

/*! Buffer descriptor which can be locked */
typedef struct
{
	BFD_BUFDESC_DDPFRM			ddpbfd;		/*!< DD+ input frame buffer */
	int16_t						      isfree;   /*!< Indicates if the buffer is locked or not */
} LOCKINGDDPFRMBFD;


/*! Struct containing all io buffer needed for the Multistream Decoder */
struct MS_IOBUFFER{
  /* Input buffer */
  DLB_SFRACT    *pp_in_pcmbuffer[MS_MAX_EXTPCM_CH];  /*!< PCM input buffer for external reencoding (DDRE) */
  unsigned char *pp_input_buffer[2];                 /*!< Dolby Pulse bitstream buffer for main and AD instance DDT */

  LOCKINGDDPFRMBFD    p_lockingddpinbfds[DDPI_DDC_TOTALINBUFS]; /*!< DDPlus input buffer descriptor with locking */

  /* Output Buffer */
  BFD_BUFDESC_DDPFRM   ddpoutbfd;    /*!< DD output buffer descriptor */

  BFD_BUFDESC_PCMCH  pp_pcmprocbfds[3][DDPI_DDC_MAXPCMOUTCHANS]; /*!< 6-channel processing buffer descriptors --
                                                                      worst case is mixer, 2 in 1 out */
  BFD_BUFDESC_PCMCH   p_pcmprocbfd_2ch[2]; /*!< 2-channel processing buffer descriptor, need one for output */

  BFD_BUFDESC_PCMCH   *pp_ddre_in_pcmbfd[DDPI_DDC_MAXPCMOUTCHANS]; /*! Array of pointers to the DDRE input channels (packed, left justified) */

  /* DDT/DDRE use case */
  int16_t    *pp_pcmout_buf[2];      /*!< Array with interleaved PCM data of either 2 channel and 6 channel output buffer,
                                       or in MS10 mode of main and associated output buffer */
  uint16_t      p_pcmout_buf_size[2]; /*!< Sizes per channel of the two PCM output buffers */

  unsigned char *p_ddout;           /*!< Buffer holding the reencoded Dolby Digital bitstream */

  int           p_input_buffer_size[2];    /*!< Sizes of the two input buffers (main and assoc.) */
  uint16_t      p_num_pcm_out_samples[2];  /*!< Number of PCM samples in the output buffer of the main and assoc. instance (valid for 2ch and 6ch output) */
  uint16_t      num_pcm_in_samples;        /*!< Number of PCM samples in the input buffer (external PCM reencoding only) */
  uint16_t      dd_output_buffer_size;     /*!< Size of the reencoded DD output buffer */

  uint32_t      *p_tmp_buf;                /*!< Temporary buffer to overlay with different intermediate buffers */
};

/*! \brief This function allocates the input, output and temporary buffer for the multistream decoder */
//MS_RETVAL ms_allocate_iobuffer(MS_USE_CASE *use_case,     /*!< [in] Pointer to the struct which defines the current use case */
//                               MS_IOBUFFER *p_iobuffer    /*!< [in,out] Struct containing all io buffer related information */
//                               );

/*! \brief This function frees up the memory allocated by ms_allocate_iobuffer() */
//MS_RETVAL ms_free_iobuffer(MS_IOBUFFER *p_iobuffer         /*!< [in] Struct containing all io buffer related information */
//                           );

/*!
 * This function looks for a locking input buffer in the array
 * that matches the BFD pointer passed in and locks it.
 * \return ERR_NO_ERROR (0) if no error, nonzero otherwise
 */
//int16_t ms_ddc_lock_inbuf(
//	      LOCKINGDDPFRMBFD          p_ddpinbfds[DDPI_DDC_TOTALINBUFS], /*!< [in,out] Pointer to array of locking input buffers */
//	const BFD_BUFDESC_DDPFRM *const p_ddbufdesc                        /*!< [in] address of BFD to lock */
//);

/*!
 * 	This function looks for a locking input buffer in the array
 * 	that matches the BFD pointer passed in and unlocks it.
 * \return ERR_NO_ERROR (0) if no error, nonzero otherwise
 */
//int16_t ms_ddc_unlock_inbuf(
//	      LOCKINGDDPFRMBFD          p_ddpinbfds[DDPI_DDC_TOTALINBUFS], /*!< [in,out] Pointer to array of locking input buffers*/
//	const BFD_BUFDESC_DDPFRM *const p_ddbufdesc                        /*!< [in] address of BFD to unlock */
//);

#endif /* MS_FIO_H_ */
