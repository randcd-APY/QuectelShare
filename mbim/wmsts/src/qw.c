/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Q U A D W O R D    S E R V I C E S

GENERAL DESCRIPTION
  This module contains services for manipulating qword (64 bit) types.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright(c) 1990-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/qw.c_v   1.5   08 Mar 2002 12:10:28   rajeevg  $
$Header: //source/qcom/qct/core/pkg/2H09/voyager_modem/rel/3.0/AMSS/products/9X00/core/services/utils/src/qw.c#3 $ $DateTime: 2011/04/19 19:57:49 $ $Author: coresvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/08/10   sri     Porting mis-aligned access methods for Q6
08/13/09   sri     Fixed mis-aligned access bug in qw_set
05/29/09   sri     Fixed mis-aligned access bug in qw_dec, qw_add, qw_sub
01/26/09   etv     Fixed mis-aligned access bug with qw_inc
11/06/07    th     prefast cleanup
06/11/07   enj     Fixing lint errors
03/06/02    gr     Moved pragma that suppresses subexpression elimination to
                   cover entire file.
01/22/02    gr     Added suppression of common subexpression elimination in
                   qw_inc() as a workaround for a compiler bug.
11/29/00   bgc     Fixed qw_div() for the misaligned case.
10/27/00   lad     Optimized with run-time alignment check.
05/24/00   hrk     Simple optimizations to qw_mul().
09/08/99   hrk     explicitly casting to uint32 the intermediate product
                   in qw_mul().
08/09/99    sk     Introduced qc_qword for qword internal usuage.
04/23/99    sk     added PACKED qualifier for proper handling in qw_mul().
04/09/99    ms     Lint cleanup.
03/17/99    ms     Changes for Virtual Boot Block.
                     Removed bb_ prefix
                     Not including bbjmpout.h
02/08/99    sk     Introduced optimized qword routines
02/04/99    ms     Exported qw_shift.
12/16/98   jct     Removed 80186 support
12/04/98    ms     Added qw_div_by_power_of_2.
09/21/98   jct     Incorporated ARM porting changes
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/28/96   dna     Prepended bb_ to function names and put module in the
                   Boot Block.  Turned off some lint warnings for the entire
                   file due to _asm code.
07/07/92   ip      Release to DMSS PVCS.
04/29/92   gb      Large model version.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
04/18/91   rdb     Finished off first set of services.
04/17/91   rdb     Created first cut of module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __ARMCC_VERSION
#if ( __ARMCC_VERSION >= 110000 ) && ( __ARMCC_VERSION < 130000 )
   #pragma Ono_cse
   /* turn off common subexpression elimination as a workaround for a
   ** compiler bug in ADS 1.1 and ADS 1.2 (build 805)
   */
#endif
#endif /* __ARMCC_VERSION */

#include "comdef.h"
#include "qw.h"
#ifdef CUST_H
   #include "customer.h"
#endif
#ifndef FEATURE_WINCE
#include "amssassert.h"
#else
#define ASSERT
#endif

#ifdef FEATURE_WINCE
  #pragma prefast(disable:414, "Alignment checked by QW_NATIVE_ALIGNED")
  /* turn off warning 414: The buffer 'temp_quotient' type has a lower
   *                       alignment than the type it's being casted to.
   *                       This could cause a problem with new compiler
   *                       drops or on different architectures.
   *                       Use __declspec(align(#)) keyword to fix the
   *                       problem.
   *
   * Alignment is being handled in this code using the QW_NATIVE_ALIGNED macro */
#endif



/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* The following macros allow for non-featurized alignment checks in the
   optimized code below. */

#ifdef ARCH_QDSP6
/* QDSP6 is a 64-bit natively aligned architecture */
#define QW_NATIVE_ALIGNED(qw) (!(((int) qw) << 29))
#else /* else assume 32-bit natively aligned architecture/s */
#define QW_NATIVE_ALIGNED(qw) (!(((int) qw) << 30))
#endif /* ARCH_QDSP6 */

/* Strictly 32-bit alignment checking macro */
#define QW_UINT32_ALIGNED(qw) (!(((int) qw) << 30))

/* Handy typedef and macros for accessing upper and lower 32-bit values
   of an unaligned uint64. */
typedef PACKED struct PACKED_POST
{
  uint32 x;
  uint32 y;
} unaligned_uint32_2;

#define PACKED_UINT32_ACCESS_LO(PTR) ((unaligned_uint32_2 *) (PTR))->x
#define PACKED_UINT32_ACCESS_HI(PTR) ((unaligned_uint32_2 *) (PTR))->y

// use non-ARM-optimized code for Q6/gcc or if requested
#if (!defined(FEATURE_QWORD_MISALIGNED_METHODS))


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_SET

DESCRIPTION
  This macro sets the high order 32 bits of a specified qword to a
  specified 32 bit value, and the low order 32 bits to another specified
  value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qw_set
(
  qc_qword qw,
    /* the qword to set               */
  uint32 hi,
    /* the value for the high 32 bits */
  uint32 lo
    /* the value for the low 32 bits  */
)
{
  ASSERT( (uint32*)qw != NULL );
  if (QW_UINT32_ALIGNED(qw)) {

    qw[0] = lo;
    qw[1] = hi;

  } else {

    PACKED_UINT32_ACCESS_LO(qw) = lo;
    PACKED_UINT32_ACCESS_HI(qw) = hi;

  }

} /* qw_set */
#endif /*MSM5000_IRAM_FWD*/


#ifndef MSM5000_IRAM_FWD
/*==========================================================================

FUNCTION QW_HI

DESCRIPTION
  This macro returns the most significant 32 bits of a specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  A uint32 equal to the high order 32 bit bits of the qword.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qw_hi
(
  qc_qword qw
    /* qword to return the high 32 bits from */
)
{
  ASSERT( (uint32*)qw != NULL );
  if (QW_UINT32_ALIGNED(qw)) {
    uint32 temp = *(((uint32 *) qw) + 1);

    return temp;
  } else {
    return PACKED_UINT32_ACCESS_HI(qw);
  }
} /* END qw_hi */
#endif /* MSM5000_IRAM_FWD */


#ifndef MSM5000_IRAM_FWD
/*===========================================================================

FUNCTION QW_LO

DESCRIPTION
  This macro returns the least significant 32 bits of a specified qword.

DEPENDENCIES
  None.

RETURN VALUE
  A uint32 equal to the low order 32 bits of the qword.

SIDE EFFECTS
  None.

===========================================================================*/
uint32 qw_lo
(
  qc_qword qw
    /* qword to return the low 32 bits from */
)
{
  ASSERT( (uint32*)qw != NULL );
  if (QW_UINT32_ALIGNED(qw)) {
    return *(uint32 *) qw;
  } else {
    return PACKED_UINT32_ACCESS_LO(qw);
  }
} /* qw_lo */
#endif /*MSM5000_IRAM_FWD*/

#endif /* FEATURE_QWORD_MISALIGNED_METHODS */

#ifdef FEATURE_WINCE
   #pragma prefast(pop)
#endif

#ifdef __ARMCC_VERSION
#if ( __ARMCC_VERSION >= 110000 ) && ( __ARMCC_VERSION < 130000 )
   #pragma Ocse
   /* turn on common subexpression elimination optimization
   */
#endif
#endif /* __ARMCC_VERSION */
