/*
 *        Confidential Information - Limited distribution to authorized persons
 *        only. This material is protected under international copyright laws as
 *        an unpublished work. Do not copy.
 *        Copyright (C) 2010-2011 Dolby Laboratories Inc.
 *        Copyright (C) 2010-2011 Dolby International AB
 *        All rights reserved.
 */


/*! \addtogroup msd */

/*! \file ms_intrinsics.h
 *  \brief MultiStreamDecoder type define header file
 */

#ifndef MS_INTRINSICS_H_
#define MS_INTRINSICS_H_

#include <stdio.h>

typedef int16_t DLB_SFRACT;
typedef int32_t DLB_LFRACT;
typedef int32_t DLB_ACCU;
typedef int64_t DLB_XACCU;
typedef int64_t DLB_NACCU;

#define DLB_SFRACT_MANTBITS 15
#define DLB_LFRACT_MANTBITS 31
#define DLB_ACCU_MANTBITS   30
#define DLB_XACCU_MANTBITS  46
#define DLB_NACCU_MANTBITS  62

#define DLB_SFRACT_BITS 16
#define DLB_LFRACT_BITS 32
#define DLB_ACCU_BITS   32
#define DLB_XACCU_BITS  56
#define DLB_NACCU_BITS  64

#define DLB_INT32_MAX 0x7FFFFFFF
#define DLB_INT32_MIN (-DLB_INT32_MAX - 1)

#define DLB_INT16_MAX 0x7FFF
#define DLB_INT16_MIN (-0x8000)


/*
macros for legacy support
******************************************************************************/
#define DLB_L(x) (DLB_SFRACT)(x)         /* Get the low part of a word */
#define DLB_H(x) (DLB_SFRACT)((x) >> 16) /* Get the high part of a word */

/* The DLB_FRACT_1 values are *unsigned* and may not necessarily fit into the
 * the corresponding signed machine representation.  They exist essentially to enable
 * the compile-time constant generation.  Resulting values should be cast to appropriate
 * signed types where appropriate.
 */
#define DLB_FRACT_1L(mantbits) ((uintmax_t)1<<(mantbits))
#define DLB_FRACT_1(mantbits) (1u<<(mantbits))

/*
scaling macros
******************************************************************************/
#define DLB_UscaleU(i)      (i)
#define DLB_FscaleFU(x,i)   ((x)/(double)DLB_FRACT_1L((i)))

/*
literal conversion macros
******************************************************************************/

/* @name Ranges
 * Definitions of data type ranges.
 */
/* Unlike the DLB_FRACT_1 values the DLB_foo_MAX and DLB_foo_MIN values *are* signed
 * values, and will be representable in the appropriate computational types.
 */
#define DLB_FRACT_fMAX(mantbits) ((DLB_FRACT_1((mantbits)) - 0.5)/DLB_FRACT_1((mantbits)))
#define DLB_FRACT_fMIN(mantbits) ((0.5 - DLB_FRACT_1((mantbits)))/DLB_FRACT_1((mantbits)))
#define DLB_FRACT_MAX(mantbits) (int)(DLB_FRACT_1((mantbits)) - 1)
#define DLB_FRACT_MIN(mantbits) (-DLB_FRACT_MAX((mantbits)) - 1)

#define DLB_FRACT_fMAXL(mantbits) ((DLB_FRACT_1L((mantbits)) - 0.5)/DLB_FRACT_1L((mantbits)))
#define DLB_FRACT_fMINL(mantbits) ((0.5 - DLB_FRACT_1L((mantbits)))/DLB_FRACT_1L((mantbits)))
#define DLB_FRACT_MAXL(mantbits) (intmax_t)(DLB_FRACT_1L((mantbits)) - 1)
#define DLB_FRACT_MINL(mantbits) (-DLB_FRACT_MAXL((mantbits)) - 1)

/* Now parameterize those abstract range values according to the basic types: */

/* An expression to convert a floating point (double) value into a fixed-point value of specified type,
 * scaled according to the specified number of mantissa bits, and rounded to nearest (not zero): */
#define _dlb_tcF(type, a, mantbits) \
        ( (DLB_FRACT_fMAX((mantbits)) <= (a)) \
        ? (type)DLB_FRACT_MAX((mantbits)) \
        : ( ((a) < DLB_FRACT_fMIN((mantbits))) \
          ? (type)DLB_FRACT_MIN((mantbits)) \
          : ( (a) < 0.0) \
            ? (type)(DLB_FRACT_1((mantbits)) * (a) - 0.5) \
            : (type)(DLB_FRACT_1((mantbits)) * (a) + 0.5) ))

#define _dlb_tcI(type, a, s, mantbits)  _dlb_tcF(type, 1.0*a/((intmax_t)1 << (s)), mantbits)


#define _dlb_tcFL(type, a, mantbits) \
        ( (DLB_FRACT_fMAXL((mantbits)) <= (a)) \
        ? (type)DLB_FRACT_MAXL((mantbits)) \
        : ( ((a) < DLB_FRACT_fMINL((mantbits))) \
          ? (type)DLB_FRACT_MINL((mantbits)) \
          : ( (a) < 0.0) \
            ? (type)(DLB_FRACT_1L((mantbits)) * (a) - 0.5) \
            : (type)(DLB_FRACT_1L((mantbits)) * (a) + 0.5) ))

/* internal-use functions */
#define _dlb_scf(a)        _dlb_tcF(DLB_SFRACT, (a), DLB_SFRACT_MANTBITS)
#define _dlb_sciu(a,s)     _dlb_tcI(DLB_SFRACT, (a), (s), DLB_SFRACT_MANTBITS)
#define _dlb_scfi(a,hr)    _dlb_tcF(DLB_SFRACT, (a), (DLB_SFRACT_MANTBITS-(hr)))
#define _dlb_lcf(a)        _dlb_tcF(DLB_LFRACT, (a), DLB_LFRACT_MANTBITS)
#define _dlb_lciu(a,s)     _dlb_tcI(DLB_LFRACT, (a), (s), DLB_LFRACT_MANTBITS)
#define _dlb_acf(a)        _dlb_tcF(DLB_ACCU,   (a), DLB_ACCU_MANTBITS)
#define _dlb_xcf(a)        _dlb_tcFL(DLB_XACCU,  (a), DLB_XACCU_MANTBITS)
#define _dlb_ncf(a)        _dlb_tcFL(DLB_NACCU,  (a), DLB_NACCU_MANTBITS)
#define _dlb_lcfi(a,hr)    _dlb_tcF(DLB_LFRACT, (a), (DLB_LFRACT_MANTBITS-(hr)))
#define _dlb_acfi(a,hr)    _dlb_tcF(DLB_ACCU,   (a), (DLB_ACCU_MANTBITS-(hr)))
#define _dlb_xcfi(a,hr)    _dlb_tcFL(DLB_XACCU,  (a), (DLB_XACCU_MANTBITS-(hr)))
#define _dlb_ncfi(a,hr)    _dlb_tcFL(DLB_NACCU,  (a), (DLB_NACCU_MANTBITS-(hr)))

/* export-use macros */
#define DLB_ScF(a)     _dlb_scf((a))
#define DLB_ScIU(a,s)  _dlb_sciu((a), (s))
#define DLB_LcF(a)     _dlb_lcf((a))
#define DLB_LcIU(a,s)  _dlb_lciu((a), (s))
#define DLB_AcF(a)     _dlb_acf((a))
#define DLB_XcF(a)     _dlb_xcf((a))
#define DLB_NcF(a)     _dlb_ncf((a))
#define DLB_ScFU(a,hr) _dlb_scfi((a),(hr))
#define DLB_LcFU(a,hr) _dlb_lcfi((a),(hr))
#define DLB_AcFU(a,hr) _dlb_acfi((a),(hr))
#define DLB_XcFU(a,hr) _dlb_xcfi((a),(hr))
#define DLB_NcFU(a,hr) _dlb_ncfi((a), 2*(hr))


/* helper functions: factor common operation patterns out, to minimise bugs */
#ifndef DLB_SGN_SHIFT31
#define _dlb_sgn(a) (-(a < 0))  /* default form uses no undefined behaviour */
#else
#define _dlb_sgn(a) ((a) >> 31)
#endif  /* DLB_SGN_VERn */

static inline int
_dlb_sat(int a, unsigned int n) {
{
    int sgn = _dlb_sgn(a);
    return sgn == (a >> n) ? a
                           : ((DLB_INT32_MAX >> (31 - n)) ^ sgn); } }

static inline int
_dlb_sadd(int a, int b) {
    int sum = a + b;
    int sgn = _dlb_sgn(sum);
    return ((a ^ sum) & (b ^ sum)) >= 0 ? sum : DLB_INT32_MIN ^ sgn; }

static inline DLB_SFRACT
DLB_S_16(int16_t x) {
    return (DLB_SFRACT)x; }

static inline DLB_LFRACT
DLB_L_16(int16_t s) {
    return (DLB_LFRACT)s << 16; }

static inline DLB_SFRACT
DLB_StruncL(DLB_LFRACT a) {
    return (DLB_SFRACT)(a >> 16); }

static inline DLB_LFRACT
DLB_L_S(DLB_SFRACT x) {
    return (DLB_LFRACT)x << 16; }

static inline DLB_SFRACT
DLB_SsrndL(DLB_LFRACT a) {
    int rnd = 0x8000;
    return (DLB_SFRACT)_dlb_sat(_dlb_sadd(a, rnd) >> 16, 15); }

static inline DLB_LFRACT
DLB_L_32(int32_t s) {
    return (DLB_LFRACT)s; }

#endif /* MS_INTRINSICS_H_ */
